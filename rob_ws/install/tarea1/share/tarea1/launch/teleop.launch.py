from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    return LaunchDescription([
        # Nodo que lee el mando y publica en /joy
        Node(
            package='joy',
            executable='joy_node',
            name='joy_node'
        ),

        # Nodo que traduce /joy a /cmd_vel usando tu YAML
        Node(
            package='teleop_twist_joy',
            executable='teleop_node',
            name='teleop_twist_joy',
            parameters=['config/teleop_twist_joy.yaml']
        )
    ])
