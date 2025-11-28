<img src="./figures/MELFA_t.png" width="400" height="98">

# __MELFA ROS2 Monitor Usage and Installation__

## Index
[1. Installation](#1-installation)  
[2. Launch MELFA ROS2 Monitor](#2-launch-melfa-ros2-monitor)  
[3. Robot Data Monitoring](#3-robot-data-monitoring)  
[4. Robot I/O Monitoring](#4-robot-io-monitoring)  
[5. Conclusion](#5-conclusion)  

## __1. Installation__

> Note: For more information regarding __real-time monitor function__ , please refer to [CR750/CR751 Series Controller, CR800 Series Controller Ethernet Function Instruction Manual](https://www.mitsubishielectric.com/fa/download/search.page?mode=manual&kisyu=/robot&q=CR750%2FCR751%20Series%20Controller%2C%20CR800%20Series%20Controller%20Ethernet%20Function%20Instruction%20Manual&sort=0&style=0&lang=2&category1=0&filter_discontinued=0&filter_bundled=0) __chapter 3.4__.

### __Build and Install__

1. Create your MELFA workspace

    ```bash
    mkdir -p ~/melfa_ws/src
    cd ~/melfa_ws/src
    source /opt/ros/humble/setup.bash
    ```

    Download [MELFA ROS2 Monitor](https://github.com/Mitsubishi-Electric-Asia/melfa_ros2_monitor/releases) from the repository and extract it into your working directory.

    Or you can clone the repository

    ```bash
    git clone -b humble https://github.com/Mitsubishi-Electric-Asia/melfa_ros2_monitor.git
    ```

    > Note: only the release tag versions are tested.

2. Update package dependency sources

    ```bash
    cd ~/melfa_ws
    rosdep update
    ```

3. Install dependencies

    ```bash
    cd ~/melfa_ws
    rosdep install -r --from-paths . --ignore-src --rosdistro humble -y --skip-keys warehouse_ros_mongo
    ```

4. Build the packages

    ```bash
    cd ~/melfa_ws
    colcon build --cmake-args -DCMAKE_BUILD_TYPE=Release
    ```

5. Remember to source the workspace

    ```bash
    source install/setup.bash
    ```

## __2. Launch MELFA ROS2 Monitor__

You require a [real robot](./rt_real_setup.md) or [RT ToolBox3 simulator](./rt_sim_setup.md) to proceed. If you have not created your RT ToolBox3 project file, refer to [this guide](./rt_toolbox3_setup.md)

</div>

> Do not forget to source both the ROS 2 installation and package `setup.bash` when you open a new terminal.

Launch robot arm with a real robot or RT ToolBox3 simulator. [Terminal 1]

```bash
ros2 launch melfa_mon_bringup monitor_control.launch.py controller_type:=<CONTROLLER TYPE>  robot_ip:=<ROBOT IP> robot_model:=<ROBOT MODEL>  prefix:=<PREFIX>  namespace:=<NAMESPACE>  controller_manager_name:=<CONTROLLER MANAGER> send_port:=<SEND PORT> recv_port:=<RECEIVE PORT> scara:=<SCARA>
```

Example:

1. Minimum required.
   
   ```bash
   ros2 launch melfa_mon_bringup monitor_control.launch.py controller_type:="R" robot_ip:=192.168.3.100 packet_lost_log:=0
   ```

2. For monitoring __multi-robots__, use the namespace argument. If the robots are the same model, use the prefix argument and put on different prefixes.
   
   ```bash
   # Terminal 1
   ros2 launch melfa_mon_bringup monitor_control.launch.py controller_type:="R" robot_ip:=192.168.3.250 packet_lost_log:=0 robot_model:="rv7frl_" prefix:="rc1_" namespace:="/rc1" controller_manager_name:="/rc1/controller_manager" send_port:=12000 recv_port:=12001
   ```
   
   ```bash
   # Terminal 2
   ros2 launch melfa_mon_bringup monitor_control.launch.py controller_type:="R" robot_ip:=192.168.3.250 packet_lost_log:=0 robot_model:="rv7frl_" prefix:="rc2_" namespace:="/rc2" controller_manager_name:="/rc2/controller_manager" send_port:=12100 recv_port:=12101
   ```

3. For SCARA robots such as RH-6FRH.
   
   ```bash
   # Terminal 1
   ros2 launch melfa_mon_bringup monitor_control.launch.py controller_type:="R" robot_ip:=192.168.3.250 packet_lost_log:=0 scara:=1
   ```

## __3. Robot Data Monitoring__

There are mainly two types of monitoring in this package: **robot data** and **robot GPIO**.

- **Robot data** refers to real-time readings of sensor and runtime information from the robot controller.  
  (covered in this section)

- **Robot GPIO** refers to the monitoring of the CR800 controller’s I/O ports.  
  (covered in Section 4)

The MELFA ROS2 Monitor supports monitoring **up to 4 types of robot data at the same time**, selected from the total of 7 supported data types.

The monitored data types can be divided into the following categories:

| Data Type                                                       | Topic Name                              | Description                                                         | MELFA Data ID |
| --------------------------------------------------------------- | --------------------------------------- | ------------------------------------------------------------------- | ------------- |
| XYZ position (Feedback) *Angle in radians                       | `/tcp_pose_broadcastor`                 | Position of the robot's EEF,TCP pose (XYZ + orientation)            | 7             |
| Joint position (Feedback) *Angle in radians                     | `/joint_states`                         | Joint Angle for joint1 up to joint8                                 | 8             |
| Speed (Feedback) [rpm]                                          | `/joint_states`velocity field           | Rotational speed of joints                                          | 15            |
| Force sensor current position xyz[N]abc[Nm]                     | `/fts_curr_broadcaster/wrench`          | Force sensor's data, before offset.                                 | 101           |
| Force sensor original data (after offset cancel) xyz[N]abc[Nm]  | `/fts_after_offset_broadcastor/wrench`  | Force sensor data, for most of use case, this will suffice.         | 102           |
| Force sensor original data (before offset cancel) xyz[N]abc[Nm] | `/fts_before_offset_broadcaster/wrench` | Force sensor data used for force sensor debugging                   | 103           |
| Servo Current Feed back                                         | `/joint_states`effort field             | The data is being put into the `Effort` field in the `joint_state`. | 1011          |

### 3.1 How to configure types of robot data to monitor

The MELFA ROS2 Monitor Package supports the simultaneous monitoring of **4 types of robot data** from the above lists, meaning the user must choose **4 out of the 7 supported types**.

Below are the steps for the configuration:

1. Go to `melfa_mon_description/config/custom_monitor_params.yaml`.

2. In the `data_queue`, change the 4 queues **data type ID** to the ones you want to monitor.

3. Build the workspace again and source the package's `setup.bash`. The monitoring queues should be updated now.

> **Note on data type ID:**  
> A *data ID* corresponds to the [CR750/CR751 Series Controller, CR800 Series Controller Ethernet Function Instruction Manual](https://www.mitsubishielectric.com/fa/download/search.page?mode=manual&kisyu=/robot&q=CR750%2FCR751%20Series%20Controller%2C%20CR800%20Series%20Controller%20Ethernet%20Function%20Instruction%20Manual&sort=0&style=0&lang=2&category1=0&filter_discontinued=0&filter_bundled=0) __chapter 3.4.6__ and is used to label each supported robot data type. 
> Changing the numbers in `data_queue` switches which data types are monitored.

Some points to be aware of:

- The MELFA ROS2 Monitor is based on UDP connection, which may be affected by other network traffic. 

- The numbering of the queues does not mean the order of arrival — all four data streams are sent **synchronously**.

## __4. Robot I/O Monitoring__

This section will guide you through a simple demo of melfa_io_controllers using RT ToolBox3 simulator.

### __4.1 RT ToolBox3 I/O Monitoring__

Before we go into ROS 2 interfaces, some familiarity with Mitsubishi Electric's robot engineering software, RT ToolBox3, will be needed. Since the monitor in RT ToolBox3 and MELFA ROS2 Monitor are listening to the same source, the CR800 series robot controller, the RT ToolBox3 Monitor will be a useful reference. 

1. Expand __Simulation__ or __Online__ &rArr; __Monitor__ &rArr; __Signal__. _Double click_ to open __General Purpose Signal__. On the far right of the window, _select_ __Monitor Setting__. Use address "10000" for input and output.

2. Expand __Simulation__ or __Online__ &rArr; __Parameter__ &rArr; __Signal Parameter__ &rArr; __Dedicated Input/Output Signals Assignment__. _Double click_ to open __General1__ and __General2__.

On a side note, the `general1_io` and `general2_io` are set to the same source as __General1__ and __General2__ in RT ToolBox3.

### __4.2 I/O Controller Demo: Enable/Disable I/O interface__

In this demo, we will be showing some basic operation with io_controls, including viewing the enabled I/O interfaces, enable/disable I/O interfaces, and examining the contents of any `io_interface`. 

1. Start by launching the MELFA ROS2 Monitor.
   In this example, we use a real robot controller of type "R".
   ⚠ Note: The GPIO address ranges differ between CR800-R, CR800-Q and CR800-D controllers, so the I/O interface addresses shown in this demo may not match your controller exactly.

    ```bash
    # Terminal 1

    ros2 launch melfa_mon_bringup monitor_control.launch.py use_fake_hardware:=false controller_type:="R" robot_ip:=192.168.3.100
    ```

2. For __viewing the enabled I/O interfaces__.

    ```bash
    ros2 topic echo /gpio_controller/io_control_mode
    ```

3. You can __enable/disable desired I/O interfaces__ using the provided ROS 2 Service like below. By default, all interfaces are enabled.

    ```bash
    # Terminal 2

    ros2 service call /io_mon_controller/configure_io_monitor_mode melfa_mon_msgs/srv/ModeConfigure "{general1_io_interface: true, general2_io_interface: true, user1_io_interface: true, user2_io_interface: true, user3_io_interface: true, user4_io_interface: true, user5_io_interface: true, user6_io_interface: true, user7_io_interface: true, user8_io_interface: true,}"
    ```

    To verify the result, run the command below.

    ```bash
    # Terminal 3

    ros2 topic echo /io_mon_controller/io_monitor_mode
    ```

    You should observe the following console prints.

    ```bash
    # Terminal 3

    ---
    general1_io_interface: true
    general2_io_interface: true
    user1_io_interface: true
    user2_io_interface: true
    user3_io_interface: true
    user4_io_interface: true
    user5_io_interface: true
    user6_io_interface: true
    user7_io_interface: true
    user8_io_interface: true
    ---
    ```

### **4.3 I/O Controller Demo: Customize I/O interface Through ROS 2 Service**

There are 8 user I/O interfaces in MELFA ROS2 Monitor, user can map them to GPIO address they desired to, other than ROS 2 Service that is discussed here, user can also change them in `melfa_mon_description/config/custom_monitor_params.yaml`, and build the workspace again to apply the address change. 

1. To configure user I/O address, you can use the `configure_io_monitor_address` service.

    Run the command below to verify the contents of user1_io_state:    

    ```bash
    # Terminal 3

    ros2 topic echo /io_mon_controller/user1_io_state 
    ```

    Observe the following. 11024 is the default starting address for user1_io_state interface.

    ```bash
    # Terminal 3

    ---
    interface_name: User1 IO State
    input_addrr: 11024
    output_addrr: 11024
    input_data: 0
    output_data: 0
    ---
    ```

2. Run the command below to use the service.

    ```bash
    # Terminal 2

    ros2 service call /io_mon_controller/configure_io_monitor_address melfa_mon_msgs/srv/GpioConfigure "{input_addrr: 11025, output_addrr: 11025}"
    ```
    The ROS 2 service format is as below:

    ```
    # Fields Required
    uint16 input_addrr
    uint16 output_addrr
    ---
    bool success
    ```

    | Parameter    | Description                 |
    | ------------ | --------------------------- |
    | input_addrr  | Address of first input bit  |
    | output_addrr | Address of first output bit |

3. Run the command below to observe the changes.
    ```bash
    # Terminal 3

    ros2 topic echo /io_mon_controller/user1_io_state 
    ```

    ```bash
    # Terminal 3

    ---
    interface_name: User1 IO State
    input_addrr: 11025
    output_addrr: 11025
    input_data: 0
    output_data: 0
    ---
    ```

## __5. Conclusion__

You have now completed the installation and learned how to:

- Launch the MELFA ROS2 Monitor with a real robot or RT ToolBox3 simulator
- Configure and monitor real-time robot data
- Monitor and customize robot GPIO interfaces



## Other guides:

- Install and configure MELFA ROS2 Monitor.
  - [MELFA ROS2 Monitor user guide](./melfa_ros2_monitor.md) : Usage and Installation of MELFA ROS2 Monitor.

- Create an RT ToolBox3 project.
  - [RT ToolBox3 Setup](./rt_toolbox3_setup.md) : Create your first RT ToolBox3 Project File for MELFA ROS2 Monitor.

- Choose your development environment.
  - [RT ToolBox3 Simulator Setup](./rt_sim_setup.md) : Connect to RT ToolBox3 simulator in place of real robots for MELFA ROS2 Monitor.
  - [RT ToolBox3 Real Robot Setup](./rt_real_setup.md): Connect to a MELFA robot for MELFA ROS2 Monitor.