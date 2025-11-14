<img src="./figures/MELFA_t.png" width="400" height="98">


# __MELFA ROS2 Monitor Usage and Installation__





## __1. Installation__

This manual is for <u>__ROS2 Humble__</u> using __rtmon api version 1.0__. For more information regarding __rtmon api__ , please refer to [CR750/CR751 Series Controller, CR800 Series Controller Ethernet Function Instruction Manual](https://www.mitsubishielectric.com/fa/download/search.page?mode=manual&kisyu=/robot&q=CR750%2FCR751%20Series%20Controller%2C%20CR800%20Series%20Controller%20Ethernet%20Function%20Instruction%20Manual&sort=0&style=0&lang=2&category1=0&filter_discontinued=0&filter_bundled=0) from [Robot Industrial/Collaborative Robot MELFA Manual](https://www.mitsubishielectric.com/fa/download/search.page?mode=manual&kisyu=/robot).

### __Build and Install__

1. Create your MELFA workspace
```
mkdir -p ~/melfa_ws/src
cd ~/melfa_ws/src
source /opt/ros/humble/setup.bash
```
Download [MELFA ROS2 Monitor](https://github.com/Mitsubishi-Electric-Asia/melfa_ros2_monitor/releases) from the repository and extract it into your working directory.

Or you can clone the repository
```
git clone -b humble https://github.com/Mitsubishi-Electric-Asia/melfa_ros2_monitor.git
```
However, only the release tag versions are tested and covered by after-sales support.


2. Update package dependency sources

```
cd ~/melfa_ws
rosdep update
```

3. Install dependencies

```
cd ~/melfa_ws
rosdep install -r --from-paths . --ignore-src --rosdistro humble -y --skip-keys warehouse_ros_mongo
```

4. Build the packages

```
cd ~/melfa_ws
colcon build --cmake-args -DCMAKE_BUILD_TYPE=Release
```

5. Remember to source the workspace

```
source install/setup.bash
```

## __2. Launch MELFA ROS2 Monitor__

You require a [real robot](./rt_real_setup.md) or [RT Toolbox3 simulator](./rt_sim_setup.md) to proceed. If you have not created your RT Toolbox3 project file, refer to [this guide](./rt_toolbox3_setup.md)
</div>

Launch robot arm with real robot or RT Toolbox3 simulator (the controller_type ["R" (or) "Q" (or) "D"] and robot_ip should be aligned with the robot) [Terminal 1]

```
ros2 launch melfa_mon_bringup monitor_control.launch.py controller_type:=<CONTROLLER TYPE>  robot_ip:=<ROBOT IP> robot_model:=<ROBOT MODEL>  prefix:=<PREFIX>  namespace:=<NAMESPACE>  controller_manager_name:=<CONTROLLER MANAGER> send_port:=<SEND PORT> recv_port:=<RECEIVE PORT> 

```
Example:

1. Minimum required.

```
ros2 launch melfa_mon_bringup monitor_control.launch.py controller_type:="R" robot_ip:=192.168.3.100 packet_lost_log:=0
```
2. For multi-instance, use the namespace argument. If the robots are NOT the same model, use the prefix argument.
```
# Terminal 1
ros2 launch melfa_mon_bringup monitor_control.launch.py controller_type:="R" robot_ip:=192.168.3.250 packet_lost_log:=0 robot_model:="rv7frl_" prefix:="rc1_" namespace:="/rc1" controller_manager_name:="/rc1/controller_manager" send_port:=12000 recv_port:=12001
```
```
# Terminal 2
ros2 launch melfa_mon_bringup monitor_control.launch.py controller_type:="R" robot_ip:=192.168.3.250 packet_lost_log:=0 robot_model:="rv7frl_" prefix:="rc2_" namespace:="/rc2" controller_manager_name:="/rc2/controller_manager" send_port:=12100 recv_port:=12101
```


## __3. Examples of I/O operations using command line interface__

This section will guide you through a simple demo of melfa_io_controllers using RT Toolbox3 simulator

### __RT Toolbox3 I/O Monitoring__

1. Expand __Simulation__ or __Online__ &rArr; __Monitor__ &rArr; __Signal__. _Double click_ to open __General Purpose Signal__. On the far right of the window, _select_ __Monitor Setting__. Use address "10000" for input and output.

2. Expand __Simulation__ or __Online__ &rArr; __Parameter__ &rArr; __Signal Parameter__ &rArr; __Dedicated Input/Output Signals Assignment__. _Double click_ to open __General1__ and __General2__.

You are now ready to try the __I/O Controller Demo__.

### __I/O Controller Demo__

1. Launch real robot arm with controller_type "R"

```
# Terminal 1

ros2 launch melfa_mon_bringup monitor_control.launch.py use_fake_hardware:=false controller_type:="R" robot_ip:=192.168.3.100
```

2. To view the enabled IO interfaces

```
ros2 topic echo /gpio_controller/io_control_mode
```


3. To enable/disable desired IO interfaces use the command below. By default, all interfaces are enabled.

```
# Terminal 2

ros2 service call /io_mon_controller/configure_io_monitor_mode melfa_mon_msgs/srv/ModeConfigure "{general1_io_interface: true, general2_io_interface: true, user1_io_interface: true, user2_io_interface: true, user3_io_interface: true, user4_io_interface: true, user5_io_interface: true, user6_io_interface: true, user7_io_interface: true, user8_io_interface: true,}"
```
To verify, run the command below.
```
# Terminal 3

ros2 topic echo /io_mon_controller/io_monitor_mode
```
Observe the following console prints.
```
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


4. To configure IO, you can use the "configure_io_monitor_address" service or the 

Run the command below to observe user1_io_state
```
# Terminal 3

ros2 topic echo /io_mon_controller/user1_io_state 
```

Observe the following. Address 11024 is default address for user1_io_state interface.
```
# Terminal 3

---
interface_name: User1 IO State
input_addrr: 11024
output_addrr: 11024
input_data: 0
output_data: 0
---
```
Observe the service format below:
```
# Fields Required
uint16 input_addrr
uint16 output_addrr
---
bool success
```
| Parameter| Description|
| ---      | ---      |
| input_addrr    | Address of first input bit|
| output_addrr     | Address of first output bit |

Run the command below to use the service.
```
# Terminal 2

ros2 service call /io_mon_controller/configure_io_monitor_address melfa_mon_msgs/srv/GpioConfigure "{input_addrr: 11025, output_addrr: 11025}"

```
Run the command below to observe the changes
```
# Terminal 3

---
interface_name: User1 IO State
input_addrr: 11025
output_addrr: 11025
input_data: 0
output_data: 0
---
```

Example 4.3: Reading general1 input and output value. 
To observe changes in General1 IO interface

```
ros2 topic echo /io_mon_controller/general1_io_state
```
Observe the console output below.
```
---
interface_name: General1 IO State
input_addrr: 10000
output_addrr: 10000
input_data: 0
output_data: 274
---
```
The output value: 274 relates to the various robot operation states in Dedicated Signals General1.

Example 4.4: To configure using Topic

| Parameter| Description|
| ---      | ---      |
| input_addrr    | Address of first input bit|
| output_addrr   | Address of first input bit|

```
# Terminal 2

ros2 topic pub /io_mon_controller/io_monitor_address_command melfa_mon_msgs/msg/GpioCommand "{input_addrr: 11026, output_addrr: 11026}"

```
Ctrl+C and run the command below

```
# Terminal 2

ros2 topic echo /io_mon_controller/user1_io_state
```

```
---
interface_name: User1 IO State
input_addrr: 11026
output_addrr: 11026
input_data: 0
output_data: 0
---
```


### Other guides:
- [Home page](./../README.md)
- [MELFA ROS2 user guide](./melfa_ros2_monitor.md) : Usage and Installation of MELFA ROS2.
- [RT Toolbox3 Setup](./rt_toolbox3_setup.md) : Create your first RT Toolbox3 Project File for ROS2.
- [RT Toolbox3 Simulator Setup](./rt_sim_setup.md) : Connect to RT Toolbox3 simulator as if it is a real robot.
- [RT Toolbox3 Real Robot Setup](./rt_real_setup.md): Connect to a MELFA robot.