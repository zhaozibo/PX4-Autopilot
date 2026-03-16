"""
Launch file for the PX4 swarm coordination stack.

Starts one vision_provider node per drone and a single swarm_controller node.

Usage:
    ros2 launch swarm_ros2 swarm_launch.py num_drones:=16 speed_factor:=1.0
"""

from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node


def generate_launch_description():
    # Declare launch arguments.
    num_drones_arg = DeclareLaunchArgument(
        'num_drones',
        default_value='16',
        description='Number of drones in the swarm',
    )
    speed_factor_arg = DeclareLaunchArgument(
        'speed_factor',
        default_value='1.0',
        description='Simulation speed factor',
    )
    formation_config_arg = DeclareLaunchArgument(
        'formation_config',
        default_value='',
        description='Path to YAML formation configuration file',
    )

    num_drones = LaunchConfiguration('num_drones')
    speed_factor = LaunchConfiguration('speed_factor')

    # We need a concrete integer to iterate, so we support a fixed max and
    # conditionally launch.  ROS 2 launch does not natively support dynamic
    # loops over LaunchConfiguration integers, so we generate nodes for a
    # reasonable upper bound and rely on the controller's num_drones param
    # to limit actual usage.  For simplicity, generate exactly 16 nodes
    # (the default).  For other counts, users can call this launch file
    # programmatically or adjust the constant below.
    MAX_DRONES = 16

    vision_nodes = []
    for i in range(MAX_DRONES):
        ns = f'px4_{i}'
        vision_nodes.append(
            Node(
                package='swarm_ros2',
                executable='vision_provider',
                name=f'vision_provider_{i}',
                parameters=[{
                    'namespace': ns,
                    'instance_id': i,
                }],
                output='screen',
            )
        )

    # Single swarm controller.
    controller_node = Node(
        package='swarm_ros2',
        executable='swarm_controller',
        name='swarm_controller',
        parameters=[{
            'num_drones': num_drones,
            'speed_factor': speed_factor,
        }],
        output='screen',
    )

    return LaunchDescription([
        num_drones_arg,
        speed_factor_arg,
        formation_config_arg,
        *vision_nodes,
        controller_node,
    ])
