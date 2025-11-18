#    COPYRIGHT (C) 2025 Mitsubishi Electric Corporation

#    Licensed under the Apache License, Version 2.0 (the "License");
#    you may not use this file except in compliance with the License.
#    You may obtain a copy of the License at

#        http://www.apache.org/licenses/LICENSE-2.0

#    Unless required by applicable law or agreed to in writing, software
#    distributed under the License is distributed on an "AS IS" BASIS,
#    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#    See the License for the specific language governing permissions and
#    limitations under the License.

from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.conditions import IfCondition
from launch.substitutions import Command, FindExecutable, LaunchConfiguration, PathJoinSubstitution

from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare
from launch_ros.parameter_descriptions import ParameterFile

def generate_launch_description():
    # Declare arguments
    declared_arguments = []
    declared_arguments.append(
        DeclareLaunchArgument(
            "namespace",
            default_value="",
            description="Namespace of controller manager and controllers. This is useful for \
        multi-robot scenarios.",
        )
    )
    declared_arguments.append(
        DeclareLaunchArgument(
            "prefix",
            default_value="",
            description="Namespace of controller manager and controllers. This is useful for \
        multi-robot scenarios.",
        )
    )
    declared_arguments.append(
        DeclareLaunchArgument(
            "robot_model",
            default_value="",
            description="Namespace of controller manager and controllers. This is useful for \
        multi-robot scenarios.",
        )
    )
    declared_arguments.append(
        DeclareLaunchArgument(
            "robot_model",
            default_value="",
            description="Namespace of controller manager and controllers. This is useful for \
        multi-robot scenarios.",
        )
    )
    declared_arguments.append(
        DeclareLaunchArgument(
            'runtime_config_package',
            default_value='melfa_mon_description',
            description='Package with the controller\'s configuration in "config" folder. \
        Usually the argument is not set, it enables use of a custom setup.',
        )
    )
    declared_arguments.append(
        DeclareLaunchArgument(
            'controllers_file',
            default_value='monitor_controllers.yaml',
            description='YAML file with the controllers configuration.',
        )
    )
    declared_arguments.append(
        DeclareLaunchArgument(
            'description_package',
            default_value='melfa_mon_description',
            description='Description package with robot URDF/xacro files. Usually the argument \
            is not set, it enables use of a custom description.',
        )
    )
    declared_arguments.append(
        DeclareLaunchArgument(
            'description_file',
            default_value='monitor.urdf.xacro',
            description='URDF/XACRO description file with the robot.',
        )
    )
    declared_arguments.append(
        DeclareLaunchArgument(
            "controller_manager_name",
            default_value="/controller_manager",
            description="Full name of the controller manager. This values should be set if \
        controller manager is used under a namespace.",
        )
    )
    declared_arguments.append(
        DeclareLaunchArgument(
            'robot_ip',
            default_value='192.168.0.20',
            description='Robot IP.',
        )
    )
    declared_arguments.append(
        DeclareLaunchArgument(
            'send_port',
            default_value='12000',
            description='Robot port.',
        )
    )
    declared_arguments.append(
        DeclareLaunchArgument(
            'recv_port',
            default_value='12001',
            description='Robot port.',
        )
    )
    declared_arguments.append(
        DeclareLaunchArgument(
            'controller_config',
            default_value='custom_monitor_params.yaml',
            description='Configuration file of controller limits.',
        )
    )
    declared_arguments.append(
        DeclareLaunchArgument(
            'controller_type',
            default_value="R",
            description='Select MELFA Controller Type : [R or Q or D]',
        )
    )
    declared_arguments.append(
        DeclareLaunchArgument(
            'packet_lost_log',
            default_value='1',
            description='Enable/disable packet lost warning. DO NOTE DISABLE WHEN USING A REAL ROBOT.',
        )
    )
    declared_arguments.append(
        DeclareLaunchArgument(
            'joint_broadcaster',
            default_value='true',
            description='Toggle joint_state_broadcaster',
        )
    )
    declared_arguments.append(
        DeclareLaunchArgument(
            'scara',
            default_value='0',
            description='Toggle joint_state_broadcaster',
        )
    )
    # Initialize Arguments
    namespace = LaunchConfiguration("namespace")
    prefix = LaunchConfiguration("prefix")
    robot_model = LaunchConfiguration("robot_model")
    runtime_config_package = LaunchConfiguration('runtime_config_package')
    controllers_file = LaunchConfiguration('controllers_file')
    description_package = LaunchConfiguration('description_package')
    description_file = LaunchConfiguration('description_file')
    controller_manager_name = LaunchConfiguration("controller_manager_name")
    joint_broadcaster = LaunchConfiguration('joint_broadcaster')
    robot_ip = LaunchConfiguration('robot_ip')
    send_port = LaunchConfiguration('send_port')
    recv_port = LaunchConfiguration('recv_port')
    controller_type = LaunchConfiguration('controller_type')
    packet_lost_log = LaunchConfiguration('packet_lost_log')
    scara = LaunchConfiguration('scara')

    controller_config = LaunchConfiguration('controller_config')
    
    controller_config = PathJoinSubstitution(
        [FindPackageShare(description_package), "config", controller_config]
    )
    robot_controllers = PathJoinSubstitution(
        [
            FindPackageShare(runtime_config_package),
            'config',
            controllers_file,
        ]
    )
    # Get URDF via xacro
    robot_description_content = Command(
        [
            PathJoinSubstitution([FindExecutable(name='xacro')]),
            ' ',
            PathJoinSubstitution(
                [FindPackageShare(description_package), 'urdf', description_file]
            ),
            ' ',
            'prefix:=',
            prefix,
            ' ',
            'robot_model:=',
            robot_model,
            ' ',
            'robot_ip:=',
            robot_ip,
            ' ',
            'send_port:=',
            send_port,
            ' ',
            'recv_port:=',
            recv_port,
            ' ',
            'controller_type:=',
            controller_type,
            ' ',
            'controller_config:=',
            controller_config,
            ' ',
            "packet_lost_log:=",
            packet_lost_log,
            ' ',
            "joint_broadcaster:=",
            joint_broadcaster,
            ' ',
            "scara:=",
            scara,
        ]
    )
    robot_description = {'robot_description': robot_description_content}

    control_node = Node(
        package='controller_manager',
        executable='ros2_control_node',
        namespace=namespace,
        parameters=[robot_description, ParameterFile(robot_controllers, allow_substs=True)],
        output={
            'stdout': 'screen',
            'stderr': 'screen',
        },

    )

    joint_state_broadcaster_spawner = Node(
        package='controller_manager',
        executable='spawner',
        namespace=namespace,
        arguments=['joint_state_broadcaster', '--controller-manager', controller_manager_name],
        condition=IfCondition(joint_broadcaster)
    )

    motor_state_broadcaster_spawner = Node(
        package='controller_manager',
        executable='spawner',
        namespace=namespace,
        arguments=['motor_state_broadcaster', '--controller-manager', controller_manager_name],
        condition=IfCondition(joint_broadcaster)
    )

    fts_curr_broadcaster_spawner = Node(
        package='controller_manager',
        executable='spawner',
        namespace=namespace,
        arguments=['fts_curr_broadcaster', '--controller-manager', controller_manager_name],
    )

    fts_before_offset_broadcaster_spawner = Node(
        package='controller_manager',
        executable='spawner',
        namespace=namespace,
        arguments=['fts_before_offset_broadcaster', '--controller-manager', controller_manager_name],
    )

    fts_after_offset_broadcaster_spawner = Node(
        package='controller_manager',
        executable='spawner',
        namespace=namespace,
        arguments=['fts_after_offset_broadcaster', '--controller-manager', controller_manager_name],
    )

    tcp_state_broadcaster_spawner = Node(
        package='controller_manager',
        executable='spawner',
        namespace=namespace,
        arguments=['tcp_pose_broadcaster', '--controller-manager', controller_manager_name],
    )

    gpio_controller_spawner = Node(
        package="controller_manager",
        executable="spawner",
        namespace=namespace,
        arguments=["io_mon_controller", "-c", controller_manager_name],
    )

    nodes = [
        control_node,
        joint_state_broadcaster_spawner,
        motor_state_broadcaster_spawner,
        gpio_controller_spawner,
        fts_curr_broadcaster_spawner,
        fts_before_offset_broadcaster_spawner,
        fts_after_offset_broadcaster_spawner,
        tcp_state_broadcaster_spawner
    ]

    return LaunchDescription(declared_arguments + nodes)
