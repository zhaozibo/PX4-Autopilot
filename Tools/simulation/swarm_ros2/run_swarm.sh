#!/bin/bash
# Run the full swarm test inside Docker: PX4 SIH + XRCE-DDS + ROS 2
# Usage: ./run_swarm.sh [NUM_DRONES] [SPEED_FACTOR]
#
# Must be run inside the px4-swarm-ros2 container with PX4 source mounted at /px4:
#   docker run -it --rm -v /path/to/PX4-Autopilot:/px4 px4-swarm-ros2 /opt/ros2_ws/src/swarm_ros2/run_swarm.sh 4

set -eo pipefail

NUM_DRONES="${1:-4}"
SPEED_FACTOR="${2:-1}"
PX4_SRC="${PX4_SRC:-/px4}"
BUILD_DIR="${PX4_SRC}/build/px4_sitl_sih"

echo "=== PX4 SIH Swarm Test ==="
echo "  Drones: $NUM_DRONES"
echo "  Speed factor: $SPEED_FACTOR"
echo ""

# Check PX4 build exists
if [ ! -f "$BUILD_DIR/bin/px4" ]; then
    echo "PX4 binary not found at $BUILD_DIR/bin/px4"
    echo "Building PX4 (this may take a few minutes)..."
    cd "$PX4_SRC"
    make px4_sitl_sih
fi

# Source ROS 2
source /opt/ros/jazzy/setup.bash
source /opt/ros2_ws/install/setup.bash

# Cleanup handler
PIDS=()
cleanup() {
    echo ""
    echo "Shutting down..."
    for pid in "${PIDS[@]}"; do
        kill "$pid" 2>/dev/null || true
    done
    pkill -x px4 2>/dev/null || true
    pkill -x MicroXRCEAgent 2>/dev/null || true
    wait 2>/dev/null
    echo "Done."
}
trap cleanup EXIT SIGINT SIGTERM

# Kill any existing px4
pkill -x px4 2>/dev/null || true
sleep 1

# Start XRCE-DDS agent
echo "Starting XRCE-DDS agent on port 8888..."
MicroXRCEAgent udp4 -p 8888 > /dev/null 2>&1 &
PIDS+=($!)
sleep 1

# Launch PX4 instances
export PX4_SIM_MODEL=sihsim_quadx_vision
export PX4_SIM_SPEED_FACTOR="$SPEED_FACTOR"

# Clean stale instance data (old parameters.bson can override param set-default)
echo "Cleaning stale instance data..."
for n in $(seq 0 $((NUM_DRONES - 1))); do
    rm -rf "$BUILD_DIR/instance_$n"
done

echo "Launching $NUM_DRONES PX4 SIH instances..."
for n in $(seq 0 $((NUM_DRONES - 1))); do
    working_dir="$BUILD_DIR/instance_$n"
    mkdir -p "$working_dir"

    pushd "$working_dir" &>/dev/null
    # Force consistent DDS namespace for all instances (including 0)
    # so ROS 2 nodes can use px4_0, px4_1, ... uniformly
    PX4_UXRCE_DDS_NS="px4_$n" "$BUILD_DIR/bin/px4" -i "$n" -d "$BUILD_DIR/etc" >out.log 2>err.log &
    PIDS+=($!)
    popd &>/dev/null
    echo "  Instance $n started (PID ${PIDS[-1]})"
done

# Wait for PX4 instances to initialize
echo ""
echo "Waiting 5s for PX4 instances to initialize..."
sleep 5

# Launch ROS 2 swarm (blocks until mission completes)
echo ""
echo "Launching ROS 2 swarm controller..."
echo "=========================================="
ros2 launch swarm_ros2 swarm_launch.py num_drones:="$NUM_DRONES"

echo ""
echo "=== Mission complete ==="

# Collect ULogs
echo "Collecting ULog files..."
python3 "$PX4_SRC/Tools/simulation/collect_swarm_ulogs.py" \
    --build-dir "$BUILD_DIR" \
    --output-dir "$PX4_SRC/swarm_ulogs_$(date +%Y%m%d_%H%M%S)"
