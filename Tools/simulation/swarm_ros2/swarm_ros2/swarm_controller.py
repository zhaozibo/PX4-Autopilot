"""
Swarm controller node for PX4 multi-vehicle coordination.

Manages all drones through a phased state machine:
  1. Wait for EV lock
  2. Arm + takeoff to formation
  3. Hold grid formation
  4. Transition to circle via cosine interpolation
  5. Hold circle and rotate
  6. Kamikaze dive to target

Uses PX4 offboard control via px4_msgs topics.
"""

import math
import os
from enum import IntEnum, auto

import numpy as np
import rclpy
from rclpy.node import Node
from rclpy.qos import QoSProfile, ReliabilityPolicy, HistoryPolicy, DurabilityPolicy

from px4_msgs.msg import (
    OffboardControlMode,
    TrajectorySetpoint,
    VehicleCommand,
    VehicleLocalPosition,
    VehicleStatus,
)


PX4_QOS = QoSProfile(
    reliability=ReliabilityPolicy.BEST_EFFORT,
    durability=DurabilityPolicy.TRANSIENT_LOCAL,
    history=HistoryPolicy.KEEP_LAST,
    depth=1,
)

# Volatile QoS for subscriptions to avoid stale data from before
# namespace discovery is complete.
PX4_SUB_QOS = QoSProfile(
    reliability=ReliabilityPolicy.BEST_EFFORT,
    durability=DurabilityPolicy.VOLATILE,
    history=HistoryPolicy.KEEP_LAST,
    depth=1,
)


class Phase(IntEnum):
    """State machine phases."""
    WAIT_EV_LOCK = auto()
    ARM_TAKEOFF = auto()
    HOLD_GRID = auto()
    TRANSITION_CIRCLE = auto()
    HOLD_CIRCLE = auto()
    KAMIKAZE = auto()
    DONE = auto()


class DroneState:
    """Per-drone bookkeeping."""

    def __init__(self, drone_id: int, namespace: str):
        self.drone_id = drone_id
        self.namespace = namespace

        # Telemetry
        self.xy_valid = False
        self.z_valid = False
        self.position = np.zeros(3)  # NED [x, y, z]
        self.has_initial_pos = False
        self._init_samples: list = []  # Buffer for initial position consensus
        self.armed = False
        self.nav_state = 0

        # Formation target (grid)
        self.grid_pos = np.zeros(3)

        # Circle target
        self.circle_angle = 0.0

        # Mission status
        self.impacted = False


class SwarmController(Node):
    """Coordinates N drones through the swarm mission phases."""

    # Phase durations in seconds
    EV_LOCK_TIMEOUT = 15.0
    TAKEOFF_DURATION = 20.0
    HOLD_GRID_DURATION = 10.0
    TRANSITION_DURATION = 15.0
    HOLD_CIRCLE_DURATION = 10.0
    KAMIKAZE_TIMEOUT = 120.0

    # Circle parameters
    CIRCLE_RADIUS = 4.0       # metres
    CIRCLE_SPEED = 3.0        # m/s tangential

    # Kamikaze parameters
    KAMIKAZE_SPEED = 12.0     # m/s (MPC_XY_VEL_MAX)
    IMPACT_DIST = 2.0         # metres
    IMPACT_Z_THRESH = -1.0    # NED (z > -1 means within 1m of ground)

    # PX4 command constants
    VEHICLE_CMD_COMPONENT_ARM_DISARM = 400
    VEHICLE_CMD_DO_SET_MODE = 176

    CONTROL_HZ = 20.0

    def __init__(self):
        super().__init__('swarm_controller')

        # Parameters
        self.declare_parameter('num_drones', 16)
        self.declare_parameter('speed_factor', 1.0)
        self.declare_parameter('target_x', 200.0)
        self.declare_parameter('target_y', 0.0)
        self.declare_parameter('target_z', 0.0)
        self.declare_parameter('takeoff_altitude', -5.0)  # NED, negative = up

        self._num_drones = self.get_parameter('num_drones').value
        self._speed_factor = self.get_parameter('speed_factor').value

        # Kamikaze target (NED)
        self._target = np.array([
            float(os.environ.get('PX4_KAMIKAZE_X',
                                 self.get_parameter('target_x').value)),
            float(os.environ.get('PX4_KAMIKAZE_Y',
                                 self.get_parameter('target_y').value)),
            float(os.environ.get('PX4_KAMIKAZE_Z',
                                 self.get_parameter('target_z').value)),
        ])

        takeoff_alt = self.get_parameter('takeoff_altitude').value

        self.get_logger().info(
            f'Swarm controller: {self._num_drones} drones, '
            f'speed_factor={self._speed_factor}, '
            f'target={self._target.tolist()}'
        )

        # Build grid formation (4x4 or NxM, 2m spacing, centered at origin).
        cols = int(math.ceil(math.sqrt(self._num_drones)))
        rows = int(math.ceil(self._num_drones / cols))
        grid_center_x = (cols - 1) / 2.0
        grid_center_y = (rows - 1) / 2.0

        # ------------------------------------------------------------------ #
        # Per-drone state and ROS interfaces
        # ------------------------------------------------------------------ #
        self._drones: list[DroneState] = []
        self._pub_offboard: list[rclpy.publisher.Publisher] = []
        self._pub_traj: list[rclpy.publisher.Publisher] = []
        self._pub_cmd: list[rclpy.publisher.Publisher] = []

        for i in range(self._num_drones):
            ns = f'px4_{i}'
            drone = DroneState(i, ns)

            # Grid position: 2m spacing, centered, at takeoff altitude.
            col = i % cols
            row = i // cols
            drone.grid_pos = np.array([
                (col - grid_center_x) * 2.0,
                (row - grid_center_y) * 2.0,
                float(takeoff_alt),
            ])

            # Circle angle for this drone.
            drone.circle_angle = 2.0 * math.pi * i / self._num_drones

            self._drones.append(drone)

            # Publishers
            self._pub_offboard.append(
                self.create_publisher(
                    OffboardControlMode,
                    f'/{ns}/fmu/in/offboard_control_mode',
                    PX4_QOS,
                )
            )
            self._pub_traj.append(
                self.create_publisher(
                    TrajectorySetpoint,
                    f'/{ns}/fmu/in/trajectory_setpoint',
                    PX4_QOS,
                )
            )
            self._pub_cmd.append(
                self.create_publisher(
                    VehicleCommand,
                    f'/{ns}/fmu/in/vehicle_command',
                    PX4_QOS,
                )
            )

            # Subscribers
            # PX4 XRCE-DDS uses versioned topic names: _v<MESSAGE_VERSION>
            # VehicleLocalPosition: MESSAGE_VERSION=1 -> _v1
            # VehicleStatus: MESSAGE_VERSION=2 -> _v2
            self.create_subscription(
                VehicleLocalPosition,
                f'/{ns}/fmu/out/vehicle_local_position_v1',
                lambda msg, idx=i: self._local_pos_cb(idx, msg),
                PX4_SUB_QOS,
            )
            self.create_subscription(
                VehicleStatus,
                f'/{ns}/fmu/out/vehicle_status_v2',
                lambda msg, idx=i: self._vehicle_status_cb(idx, msg),
                PX4_SUB_QOS,
            )

        # ------------------------------------------------------------------ #
        # State machine
        # ------------------------------------------------------------------ #
        self._phase = Phase.WAIT_EV_LOCK
        self._phase_start_time = self.get_clock().now()
        self._mission_start_time = self.get_clock().now()
        self._circle_center = np.array([0.0, 0.0, float(takeoff_alt)])
        self._last_ev_print = 0

        # Main control timer
        period_s = 1.0 / self.CONTROL_HZ
        self._timer = self.create_timer(period_s, self._control_loop)

    # ===================================================================== #
    # Subscriber callbacks
    # ===================================================================== #
    def _local_pos_cb(self, idx: int, msg: VehicleLocalPosition):
        d = self._drones[idx]
        d.xy_valid = msg.xy_valid
        d.z_valid = msg.z_valid

        new_pos = np.array([msg.x, msg.y, msg.z])

        if not d.has_initial_pos:
            if not (d.xy_valid and d.z_valid):
                return
            # All drones start near origin. Reject samples > 20m from origin
            # during initialization (DDS cross-talk from other instances).
            if np.linalg.norm(new_pos) > 20.0:
                return
            # Require 5 consistent samples within 5m to establish initial pos.
            d._init_samples.append(new_pos)
            if len(d._init_samples) >= 5:
                ref = d._init_samples[-1]
                consistent = all(
                    np.linalg.norm(s - ref) < 5.0
                    for s in d._init_samples[-5:]
                )
                if consistent:
                    d.position = new_pos
                    d.has_initial_pos = True
                else:
                    d._init_samples = d._init_samples[-1:]
            return

        # Guard against DDS cross-talk: reject position jumps > 50m/tick.
        # At 50Hz and 12m/s max, real movement is < 0.3m per tick.
        jump = np.linalg.norm(new_pos - d.position)
        if jump > 50.0:
            return  # Discard cross-talk sample

        d.position = new_pos

    def _vehicle_status_cb(self, idx: int, msg: VehicleStatus):
        d = self._drones[idx]
        d.armed = (msg.arming_state == VehicleStatus.ARMING_STATE_ARMED)
        d.nav_state = msg.nav_state

    # ===================================================================== #
    # Publishing helpers
    # ===================================================================== #
    def _publish_offboard_mode(self, idx: int, *, position: bool = False,
                               velocity: bool = False):
        """Send OffboardControlMode for drone idx."""
        msg = OffboardControlMode()
        msg.timestamp = int(self.get_clock().now().nanoseconds / 1000)
        msg.position = position
        msg.velocity = velocity
        msg.acceleration = False
        msg.attitude = False
        msg.body_rate = False
        self._pub_offboard[idx].publish(msg)

    def _publish_trajectory(self, idx: int, *,
                            position: list[float] | None = None,
                            velocity: list[float] | None = None,
                            yaw: float = float('nan'),
                            yawspeed: float = float('nan')):
        """Send TrajectorySetpoint for drone idx."""
        msg = TrajectorySetpoint()
        msg.timestamp = int(self.get_clock().now().nanoseconds / 1000)

        nan3 = [float('nan')] * 3
        msg.position = list(position) if position is not None else nan3
        msg.velocity = list(velocity) if velocity is not None else nan3
        msg.yaw = yaw
        msg.yawspeed = yawspeed
        self._pub_traj[idx].publish(msg)

    def _publish_vehicle_command(self, idx: int, command: int,
                                 param1: float = 0.0, param2: float = 0.0,
                                 param3: float = 0.0):
        """Send VehicleCommand for drone idx."""
        msg = VehicleCommand()
        msg.timestamp = int(self.get_clock().now().nanoseconds / 1000)
        msg.command = command
        msg.param1 = param1
        msg.param2 = param2
        msg.param3 = param3
        msg.target_system = idx + 1       # MAV_SYS_ID
        msg.target_component = 1
        msg.source_system = 255           # GCS
        msg.source_component = 0
        msg.from_external = True
        self._pub_cmd[idx].publish(msg)

    def _arm(self, idx: int):
        self._publish_vehicle_command(
            idx, self.VEHICLE_CMD_COMPONENT_ARM_DISARM, param1=1.0)

    def _disarm_force(self, idx: int):
        self._publish_vehicle_command(
            idx, self.VEHICLE_CMD_COMPONENT_ARM_DISARM,
            param1=0.0, param2=21196.0)

    def _set_offboard_mode(self, idx: int):
        self._publish_vehicle_command(
            idx, self.VEHICLE_CMD_DO_SET_MODE,
            param1=1.0, param2=6.0, param3=0.0)

    # ===================================================================== #
    # Phase elapsed time helper
    # ===================================================================== #
    def _phase_elapsed(self) -> float:
        """Seconds since current phase started."""
        return (self.get_clock().now() - self._phase_start_time).nanoseconds / 1e9

    def _mission_elapsed(self) -> float:
        """Seconds since mission started."""
        return (self.get_clock().now() - self._mission_start_time).nanoseconds / 1e9

    def _advance_phase(self, new_phase: Phase):
        self.get_logger().info(f'Phase transition: {self._phase.name} -> {new_phase.name}')
        self._phase = new_phase
        self._phase_start_time = self.get_clock().now()

    # ===================================================================== #
    # Main control loop (20 Hz)
    # ===================================================================== #
    def _control_loop(self):
        if self._phase == Phase.DONE:
            return

        if self._phase == Phase.WAIT_EV_LOCK:
            self._phase_wait_ev_lock()
        elif self._phase == Phase.ARM_TAKEOFF:
            self._phase_arm_takeoff()
        elif self._phase == Phase.HOLD_GRID:
            self._phase_hold_grid()
        elif self._phase == Phase.TRANSITION_CIRCLE:
            self._phase_transition_circle()
        elif self._phase == Phase.HOLD_CIRCLE:
            self._phase_hold_circle()
        elif self._phase == Phase.KAMIKAZE:
            self._phase_kamikaze()

    # ------------------------------------------------------------------ #
    # Phase 1: Wait for EV lock
    # ------------------------------------------------------------------ #
    def _phase_wait_ev_lock(self):
        locked = sum(1 for d in self._drones if d.xy_valid and d.z_valid)
        elapsed = self._phase_elapsed()

        # Print status every second.
        elapsed_int = int(elapsed)
        if elapsed_int > self._last_ev_print:
            self._last_ev_print = elapsed_int
            self.get_logger().info(
                f'{locked}/{self._num_drones} drones have EV lock '
                f'({elapsed:.0f}s elapsed)')

        if locked == self._num_drones or elapsed > self.EV_LOCK_TIMEOUT:
            if locked < self._num_drones:
                self.get_logger().warn(
                    f'EV lock timeout: only {locked}/{self._num_drones} locked')
            self._advance_phase(Phase.ARM_TAKEOFF)

    # ------------------------------------------------------------------ #
    # Phase 2: Arm + takeoff
    # ------------------------------------------------------------------ #
    def _phase_arm_takeoff(self):
        elapsed = self._phase_elapsed()

        for i in range(self._num_drones):
            d = self._drones[i]

            # Keep sending offboard + setpoint before and after arming.
            self._publish_offboard_mode(i, position=True)
            self._publish_trajectory(
                i,
                position=d.grid_pos.tolist(),
                yaw=0.0,
            )

            # Arm and set offboard mode repeatedly for the first few seconds.
            if elapsed < 5.0:
                self._set_offboard_mode(i)
                self._arm(i)

        # Log status every 5 seconds.
        elapsed_int = int(elapsed)
        if elapsed_int > 0 and elapsed_int % 5 == 0 and elapsed - elapsed_int < 0.1:
            armed = sum(1 for d in self._drones if d.armed)
            self.get_logger().info(
                f'ARM_TAKEOFF {elapsed:.0f}s: {armed}/{self._num_drones} armed, '
                f'positions: ' + ', '.join(
                    f'D{i}=[{d.position[2]:.1f}m]'
                    for i, d in enumerate(self._drones)
                )
            )

        if elapsed > self.TAKEOFF_DURATION:
            self._advance_phase(Phase.HOLD_GRID)

    # ------------------------------------------------------------------ #
    # Phase 3: Hold grid
    # ------------------------------------------------------------------ #
    def _phase_hold_grid(self):
        for i in range(self._num_drones):
            d = self._drones[i]
            self._publish_offboard_mode(i, position=True)
            self._publish_trajectory(
                i,
                position=d.grid_pos.tolist(),
                yaw=0.0,
            )

        if self._phase_elapsed() > self.HOLD_GRID_DURATION:
            # Compute circle center as the mean of grid positions.
            mean_pos = np.mean([d.grid_pos for d in self._drones], axis=0)
            self._circle_center = mean_pos.copy()
            self._advance_phase(Phase.TRANSITION_CIRCLE)

    # ------------------------------------------------------------------ #
    # Phase 4: Transition to circle (cosine interpolation)
    # ------------------------------------------------------------------ #
    def _phase_transition_circle(self):
        elapsed = self._phase_elapsed()
        alpha = 0.5 - 0.5 * math.cos(math.pi * min(elapsed / self.TRANSITION_DURATION, 1.0))

        for i in range(self._num_drones):
            d = self._drones[i]

            # Target circle position.
            cx = self._circle_center[0] + self.CIRCLE_RADIUS * math.cos(d.circle_angle)
            cy = self._circle_center[1] + self.CIRCLE_RADIUS * math.sin(d.circle_angle)
            cz = self._circle_center[2]
            circle_pos = np.array([cx, cy, cz])

            # Interpolate between grid and circle.
            target = (1.0 - alpha) * d.grid_pos + alpha * circle_pos

            # Yaw facing center.
            yaw = math.atan2(
                self._circle_center[1] - target[1],
                self._circle_center[0] - target[0],
            )

            self._publish_offboard_mode(i, position=True)
            self._publish_trajectory(i, position=target.tolist(), yaw=yaw)

        if elapsed > self.TRANSITION_DURATION:
            self._advance_phase(Phase.HOLD_CIRCLE)

    # ------------------------------------------------------------------ #
    # Phase 5: Hold circle (rotate CW)
    # ------------------------------------------------------------------ #
    def _phase_hold_circle(self):
        elapsed = self._phase_elapsed()
        omega = self.CIRCLE_SPEED / self.CIRCLE_RADIUS  # rad/s

        for i in range(self._num_drones):
            d = self._drones[i]
            # CW rotation: subtract omega*t from angle.
            angle = d.circle_angle - omega * elapsed

            px = self._circle_center[0] + self.CIRCLE_RADIUS * math.cos(angle)
            py = self._circle_center[1] + self.CIRCLE_RADIUS * math.sin(angle)
            pz = self._circle_center[2]

            # Yaw facing center.
            yaw = math.atan2(
                self._circle_center[1] - py,
                self._circle_center[0] - px,
            )

            self._publish_offboard_mode(i, position=True)
            self._publish_trajectory(
                i,
                position=[px, py, pz],
                yaw=yaw,
            )

        if elapsed > self.HOLD_CIRCLE_DURATION:
            self._advance_phase(Phase.KAMIKAZE)

    # ------------------------------------------------------------------ #
    # Phase 6: Kamikaze dive
    # ------------------------------------------------------------------ #
    def _phase_kamikaze(self):
        all_done = True
        elapsed = self._phase_elapsed()

        for i in range(self._num_drones):
            d = self._drones[i]

            if d.impacted:
                continue
            all_done = False

            # Direction to target.
            to_target = self._target - d.position
            dist = np.linalg.norm(to_target)

            # Impact detection.
            if dist < self.IMPACT_DIST and d.position[2] > self.IMPACT_Z_THRESH:
                self.get_logger().info(
                    f'[Drone {i}] Impact! pos=[{d.position[0]:.1f}, '
                    f'{d.position[1]:.1f}, {d.position[2]:.1f}]')
                d.impacted = True
                self._disarm_force(i)
                continue

            # Velocity toward target.
            if dist > 0.01:
                direction = to_target / dist
            else:
                direction = np.array([1.0, 0.0, 0.0])
            vel = (direction * self.KAMIKAZE_SPEED).tolist()

            # Yaw toward target.
            yaw = math.atan2(to_target[1], to_target[0])

            self._publish_offboard_mode(i, velocity=True)
            self._publish_trajectory(i, velocity=vel, yaw=yaw)

        # Log kamikaze progress every 10 seconds.
        elapsed_int = int(elapsed)
        if elapsed_int > 0 and elapsed_int % 10 == 0 and elapsed - elapsed_int < 0.1:
            for i, d in enumerate(self._drones):
                if not d.impacted:
                    dist = np.linalg.norm(self._target - d.position)
                    self.get_logger().info(
                        f'KAMIKAZE {elapsed:.0f}s: D{i} pos=[{d.position[0]:.1f}, '
                        f'{d.position[1]:.1f}, {d.position[2]:.1f}] '
                        f'dist={dist:.1f}m armed={d.armed}')

        if all_done or elapsed > self.KAMIKAZE_TIMEOUT:
            self._print_summary()
            self._advance_phase(Phase.DONE)
            # Shutdown after a short delay to let final messages flush.
            self.create_timer(1.0, lambda: self._shutdown())

    def _print_summary(self):
        impacted = sum(1 for d in self._drones if d.impacted)
        total_time = self._mission_elapsed()
        self.get_logger().info('=' * 50)
        self.get_logger().info('SWARM MISSION SUMMARY')
        self.get_logger().info(f'  Drones: {self._num_drones}')
        self.get_logger().info(f'  Impacts: {impacted}/{self._num_drones}')
        self.get_logger().info(f'  Total time: {total_time:.1f}s')
        self.get_logger().info('=' * 50)

    def _shutdown(self):
        self.get_logger().info('Swarm controller shutting down.')
        self._timer.cancel()
        raise SystemExit(0)


def main(args=None):
    rclpy.init(args=args)
    node = SwarmController()
    try:
        rclpy.spin(node)
    except (KeyboardInterrupt, SystemExit):
        node.get_logger().info('Swarm controller exiting.')
    finally:
        node.destroy_node()
        rclpy.try_shutdown()


if __name__ == '__main__':
    main()
