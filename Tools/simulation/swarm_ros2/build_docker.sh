#!/bin/bash
# Build the px4-swarm-ros2 Docker image with px4_msgs matching this PX4 commit.
# Run from anywhere; the script locates PX4 root automatically.
#
# Usage: ./build_docker.sh

set -eo pipefail

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PX4_ROOT="$( cd "${SCRIPT_DIR}/../../.." && pwd )"

echo "PX4 root: ${PX4_ROOT}"
echo "Staging px4_msgs definitions from this PX4 commit..."

# Stage msg/srv files into the Docker build context
STAGING_DIR="${SCRIPT_DIR}/_px4_msgs_defs"
rm -rf "${STAGING_DIR}"
mkdir -p "${STAGING_DIR}/msg" "${STAGING_DIR}/srv"

cp "${PX4_ROOT}"/msg/*.msg "${STAGING_DIR}/msg/"
cp "${PX4_ROOT}"/msg/versioned/*.msg "${STAGING_DIR}/msg/"
cp "${PX4_ROOT}"/srv/*.srv "${STAGING_DIR}/srv/"

echo "Staged $(ls "${STAGING_DIR}/msg/" | wc -l | tr -d ' ') .msg files and $(ls "${STAGING_DIR}/srv/" | wc -l | tr -d ' ') .srv files"
echo ""
echo "Building Docker image..."

docker build -t px4-swarm-ros2 "${SCRIPT_DIR}"

# Clean up staged files
rm -rf "${STAGING_DIR}"

echo ""
echo "Done. Run with:"
echo "  docker run -it --rm -v ${PX4_ROOT}:/px4 px4-swarm-ros2 bash /px4/Tools/simulation/swarm_ros2/run_swarm.sh 4"
