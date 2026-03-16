"""
Vision provider node for PX4 EKF2 external vision fusion.

Subscribes to ground truth local position and publishes noisy visual odometry
estimates, simulating an external vision system (e.g., motion capture, VIO).

One instance runs per drone. The node decimates to 30 Hz and adds configurable
Gaussian noise to position and velocity.
"""

import numpy as np
import rclpy
from rclpy.node import Node
from rclpy.qos import QoSProfile, ReliabilityPolicy, HistoryPolicy, DurabilityPolicy

from px4_msgs.msg import VehicleLocalPosition as VehicleLocalPositionGroundtruth
from px4_msgs.msg import VehicleOdometry


# QoS profile matching PX4 uORB-to-DDS bridge defaults.
PX4_QOS = QoSProfile(
    reliability=ReliabilityPolicy.BEST_EFFORT,
    durability=DurabilityPolicy.TRANSIENT_LOCAL,
    history=HistoryPolicy.KEEP_LAST,
    depth=1,
)


class VisionProvider(Node):
    """Publishes noisy VehicleOdometry derived from ground truth."""

    # Noise standard deviations
    POS_NOISE_STD = 0.5       # metres
    ANG_VEL_NOISE_STD = 0.1   # rad/s (unused but kept for completeness)

    # Output rate target
    OUTPUT_HZ = 30.0
    OUTPUT_PERIOD_US = int(1e6 / OUTPUT_HZ)

    def __init__(self):
        # Declare parameters before calling super().__init__ so they are
        # available immediately.
        super().__init__('vision_provider')

        self.declare_parameter('namespace', 'px4_0')
        self.declare_parameter('instance_id', 0)

        self._namespace = self.get_parameter('namespace').value
        self._instance_id = self.get_parameter('instance_id').value

        self.get_logger().info(
            f'Starting vision_provider_{self._instance_id} '
            f'for namespace /{self._namespace}'
        )
        self.get_logger().warn(
            'Note: vehicle_local_position_groundtruth is NOT published '
            'over DDS by PX4. This node is a no-op when using '
            'sensor_agp_sim (SENS_EN_AGPSIM=1) for position. '
            'It will only produce output if EV fusion is enabled and '
            'ground truth is published via a custom DDS bridge.'
        )

        self._last_pub_time_us: int = 0
        self._msg_count: int = 0
        self._rng = np.random.default_rng()

        # -- Subscriber: ground truth local position --
        # Note: PX4 SIH does not publish this topic over XRCE-DDS.
        # With sensor_agp_sim, EKF2 gets position internally.
        gt_topic = f'/{self._namespace}/fmu/out/vehicle_local_position_groundtruth'
        self._sub_gt = self.create_subscription(
            VehicleLocalPositionGroundtruth,
            gt_topic,
            self._gt_callback,
            PX4_QOS,
        )

        # -- Publisher: visual odometry for EKF2 --
        vo_topic = f'/{self._namespace}/fmu/in/vehicle_visual_odometry'
        self._pub_vo = self.create_publisher(VehicleOdometry, vo_topic, PX4_QOS)

    # --------------------------------------------------------------------- #
    # Callback
    # --------------------------------------------------------------------- #
    def _gt_callback(self, msg: VehicleLocalPositionGroundtruth):
        """Decimate ground truth and publish noisy visual odometry."""
        now_us = int(self.get_clock().now().nanoseconds / 1000)

        # Rate-limit to OUTPUT_HZ.
        if (now_us - self._last_pub_time_us) < self.OUTPUT_PERIOD_US:
            return
        self._last_pub_time_us = now_us

        # Build VehicleOdometry message.
        vo = VehicleOdometry()
        vo.timestamp = now_us
        vo.timestamp_sample = msg.timestamp

        # Frames: NED
        vo.pose_frame = VehicleOdometry.POSE_FRAME_NED
        vo.velocity_frame = VehicleOdometry.VELOCITY_FRAME_NED

        # Position with Gaussian noise.
        noise_pos = self._rng.normal(0.0, self.POS_NOISE_STD, size=3).astype(np.float32)
        vo.position = [
            msg.x + float(noise_pos[0]),
            msg.y + float(noise_pos[1]),
            msg.z + float(noise_pos[2]),
        ]

        # Quaternion (pass through, no noise on orientation).
        vo.q = [msg.heading, 0.0, 0.0, 0.0]
        # If ground truth provides a full quaternion, prefer that. The
        # VehicleLocalPosition message only carries heading; construct a
        # yaw-only quaternion.
        half_yaw = msg.heading / 2.0
        vo.q = [
            float(np.cos(half_yaw)),
            0.0,
            0.0,
            float(np.sin(half_yaw)),
        ]

        # Velocity with small noise.
        noise_vel = self._rng.normal(0.0, 0.1, size=3).astype(np.float32)
        vo.velocity = [
            msg.vx + float(noise_vel[0]),
            msg.vy + float(noise_vel[1]),
            msg.vz + float(noise_vel[2]),
        ]

        # Angular velocity (not used, set to zero).
        vo.angular_velocity = [0.0, 0.0, 0.0]

        # Variances (squared standard deviations).
        vo.position_variance = [0.25, 0.25, 0.25]       # 0.5^2
        vo.orientation_variance = [0.01, 0.01, 0.01]
        vo.velocity_variance = [0.1, 0.1, 0.1]

        self._pub_vo.publish(vo)


def main(args=None):
    rclpy.init(args=args)
    node = VisionProvider()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        node.get_logger().info('Shutting down vision provider.')
    finally:
        node.destroy_node()
        rclpy.try_shutdown()


if __name__ == '__main__':
    main()
