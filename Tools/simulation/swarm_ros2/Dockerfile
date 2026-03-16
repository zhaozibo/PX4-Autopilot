# ROS 2 Jazzy layer on top of PX4 dev environment
# For swarm simulation with px4-ros2-interface-lib
#
# Build (from PX4-Autopilot root):
#   Tools/simulation/swarm_ros2/build_docker.sh
#
# Run:
#   docker run -it --rm -v /path/to/PX4-Autopilot:/px4 px4-swarm-ros2
#
FROM px4io/px4-dev:v1.17.0-rc1
LABEL maintainer="Ramon Roche <mrpollo@gmail.com>"

ENV DEBIAN_FRONTEND=noninteractive
ENV ROS_DISTRO=jazzy

# Add ROS 2 Jazzy repository (Ubuntu 24.04 = noble)
RUN apt-get update && apt-get install -y --no-install-recommends \
        curl \
        gnupg \
        lsb-release \
        software-properties-common \
    && curl -sSL https://raw.githubusercontent.com/ros/rosdistro/master/ros.key \
        -o /usr/share/keyrings/ros-archive-keyring.gpg \
    && echo "deb [arch=$(dpkg --print-architecture) signed-by=/usr/share/keyrings/ros-archive-keyring.gpg] \
        http://packages.ros.org/ros2/ubuntu noble main" \
        > /etc/apt/sources.list.d/ros2.list \
    && apt-get update \
    && apt-get install -y --no-install-recommends \
        ros-jazzy-ros-base \
        ros-jazzy-rmw-cyclonedds-cpp \
        python3-colcon-common-extensions \
        python3-rosdep \
        python3-numpy \
    && rm -rf /var/lib/apt/lists/*

# Initialize rosdep
RUN rosdep init 2>/dev/null || true \
    && su user -c "rosdep update --rosdistro jazzy" 2>/dev/null || true

# Set ROS 2 middleware to CycloneDDS (better multi-vehicle support)
ENV RMW_IMPLEMENTATION=rmw_cyclonedds_cpp

# Build Micro XRCE-DDS Agent from source
RUN git clone --depth 1 https://github.com/eProsima/Micro-XRCE-DDS-Agent.git /tmp/xrce-agent \
    && cd /tmp/xrce-agent \
    && mkdir build && cd build \
    && cmake .. -DCMAKE_BUILD_TYPE=Release \
    && make -j$(nproc) \
    && make install \
    && ldconfig \
    && rm -rf /tmp/xrce-agent

# Build px4_msgs from PX4 source tree msg definitions (ensures type hash match).
# The build_docker.sh script copies msg/ and srv/ into _px4_msgs_defs/ before build.
ENV ROS2_WS=/opt/ros2_ws
COPY _px4_msgs_defs/ /tmp/px4_msg_defs/
RUN mkdir -p ${ROS2_WS}/src \
    && cd ${ROS2_WS}/src \
    && git clone --depth 1 https://github.com/PX4/px4_msgs.git \
    && rm -rf px4_msgs/msg/*.msg px4_msgs/srv/*.srv \
    && cp /tmp/px4_msg_defs/msg/*.msg px4_msgs/msg/ \
    && cp /tmp/px4_msg_defs/srv/*.srv px4_msgs/srv/ \
    && rm -rf /tmp/px4_msg_defs \
    && cd ${ROS2_WS} \
    && . /opt/ros/jazzy/setup.sh \
    && colcon build --packages-select px4_msgs --cmake-args -DCMAKE_BUILD_TYPE=Release \
    && rm -rf log

# Build px4-ros2-interface-lib (depends on px4_msgs built above)
RUN cd ${ROS2_WS}/src \
    && git clone --depth 1 --recursive https://github.com/Auterion/px4-ros2-interface-lib.git \
    && cd ${ROS2_WS} \
    && . /opt/ros/jazzy/setup.sh \
    && . install/setup.sh \
    && colcon build --packages-skip px4_msgs --cmake-args -DCMAKE_BUILD_TYPE=Release \
    && rm -rf log src

# Copy swarm_ros2 package and build it
COPY . ${ROS2_WS}/src/swarm_ros2/
RUN cd ${ROS2_WS} \
    && . /opt/ros/jazzy/setup.sh \
    && . install/setup.sh \
    && colcon build --packages-select swarm_ros2 \
    && rm -rf log

# Source everything on shell entry
RUN echo "source /opt/ros/jazzy/setup.bash" >> /etc/bash.bashrc \
    && echo "source ${ROS2_WS}/install/setup.bash" >> /etc/bash.bashrc

# XRCE-DDS UDP port
EXPOSE 8888/udp

WORKDIR ${ROS2_WS}

CMD ["/bin/bash"]
