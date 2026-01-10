from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    return LaunchDescription([
        # Nodo que lee el mando y publica en /joy
        Node(
            package='joy',
            executable='joy_node',
            name='joy_node',
            parameters=[{
                'device_id': 0,
                'deadzone': 0.05,
                'autorepeat_rate': 20.0
            }]
        ),

        # Tu nodo personalizado "mando"
        Node(
            package='tarea1',      # cambia por el nombre real de tu paquete
            executable='mando',          # el ejecutable que definiste en setup.py o CMakeLists
            name='mando'
        )
    ])
