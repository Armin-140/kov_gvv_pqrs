import os

from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.conditions import IfCondition
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node


def generate_launch_description():
    pkg_share = get_package_share_directory('kov_gvv_pqrs')
    rviz_config = os.path.join(pkg_share, 'rviz', 'lissajous.rviz')

    return LaunchDescription([
        DeclareLaunchArgument(
            'rviz',
            default_value='false',
            description='Start RViz2 to visualize the trajectory'),
        Node(
            package='kov_gvv_pqrs',
            executable='lissajous_generator',
            name='lissajous_generator',
            output='screen',
            parameters=[{
                'amplitude_x': 4.0,
                'amplitude_y': 3.0,
                'freq_a': 3.0,
                'freq_b': 2.0,
                'rate_hz': 20.0,
            }],
        ),
        Node(
            package='kov_gvv_pqrs',
            executable='path_recorder',
            name='path_recorder',
            output='screen',
            parameters=[{'max_poses': 500}],
        ),
        Node(
            package='rviz2',
            executable='rviz2',
            name='rviz2',
            arguments=['-d', rviz_config],
            condition=IfCondition(LaunchConfiguration('rviz')),
        ),
    ])
