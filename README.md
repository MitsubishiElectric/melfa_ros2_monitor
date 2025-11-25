<img src="./doc/figures/MELFA_t.png" width="400" height="98">

# __MITSUBISHI ELECTRIC MELFA ROS2 Monitor__
    
## __1. Overview__

CR800/CR860 robot controllers provide a real-time monitoring function that streams internal controller data over Ethernet cable.
MELFA ROS2 Monitor is the ROS bridge for that capability, implemented on top of `ros2_control`.

It forwards high-frequency runtime data — joint motion, servo status, I/O signals, temperatures, currents, TCP pose and force-torque sensing — directly into ROS 2 state interfaces.
This allows ROS applications to observe MELFA robots in real time and build advanced capabilities such as diagnostics, anomaly detection, telemetry logging and force-feedback behaviors.
</br>

### Requirements

- **ROS 2 Humble** on **Ubuntu 22.04 LTS**
- **MELFA CR800 / CR860 robot controller**  
  (all robot arm models running on these controllers are supported)

Optional (recommended for development and testing):
- **RT Toolbox3 / RT Toolbox3 Simulator**

### Specifications

- Communication method: **UDP (Real-Time Monitor / rtmon API)**
- Communication frequency: **286 Hz (R / D controllers)** / **141 Hz (Q controllers)**
- Supports **multiple robot controllers monitored in parallel**
- Supports **4 monitoring data queues per cycle**
- Supports **2 General IO ranges** + **8 User-defined IO ranges**
- Provides runtime data including:

  
| Data ID | Description                                                  | Unit / Notes                                   |
|--------:|--------------------------------------------------------------|-----------------------------------------------|
| 7       | XYZ position (feedback)                                      | *Angles in radians*                            |
| 8       | Joint position (feedback)                                    | *Angles in radians*                            |
| 15      | Speed (feedback)                                             | rpm                                            |
| 101     | Force sensor **current position**                            | xyz [N], abc [Nm]                              |
| 102     | Force sensor **original data (after offset cancel)**         | xyz [N], abc [Nm]                              |
| 103     | Force sensor **original data (before offset cancel)**        | xyz [N], abc [Nm]                              |
| 1011    | Current feedback                                              | Ampere                                           |

>Note: As stated in overview, this package is a bridge between ROS 2 and MELFA's real-time monitoring function, please read the related chapter if you wish to learn more about the function. You can download the [CR750/CR751 Series Controller, CR800 Series Controller Ethernet Function Instruction Manual](https://www.mitsubishielectric.com/fa/download/search.page?mode=manual&kisyu=/robot&q=CR750%2FCR751%20Series%20Controller%2C%20CR800%20Series%20Controller%20Ethernet%20Function%20Instruction%20Manual&sort=0&style=0&lang=2&category1=0&filter_discontinued=0&filter_bundled=0) from [Robot Industrial/Collaborative Robot MELFA Manual](https://www.mitsubishielectric.com/fa/download/search.page?mode=manual&kisyu=/robot). Refer to chapter __3.4 Real-time monitor function__.</br>

- [Learn more about Mitsubishi Electric FA](https://www.mitsubishielectric.com/fa/products/rbt/robot/index.html)
- [MELFA Robot catalog](https://www.mitsubishielectric.com/app/fa/download/search.do?kisyu=/robot&mode=catalog)

</br>

## __2. MELFA ROS2 Monitor Project Structure__

MELFA ROS2 Monitor consists of five components:

### __melfa_mon_bringup__

- provides launch files for start-up.

### __melfa_mon_description__

- contains ros2_control config files.

### __melfa_monitor__

- supports [ros2_control](https://control.ros.org/humble/doc/getting_started/getting_started.html).
- provides __real time communication__<sup>1</sup> hardware interface with our CR800/860-R/Q/D robot controllers via __rtmon api__ <sup>2</sup>. 
- connects to the robot controller via __rtmon api__ to monitor robot parameters via __real-time monitor function__.
- includes quality of life features built into __rtmon api__ such as user configurable disconnection detection and debugging tools.

### __melfa_mon_controllers__

- supports [ros2_control](https://control.ros.org/humble/doc/getting_started/getting_started.html).
- 10 user configurable 32-bit I/O controllers with almost 2 kilobytes of total addressable memory and 1 kilobyte of addressable shared memory.
- provides ROS 2 controllers for GPIO control.

### __melfa_mon_msgs__

- provides custom ROS2 msgs.

&#10146; <sup>1</sup>  __real time communication__ frequency is 286Hz for CR800/860-R & CR-800/860-D and 141Hz for CR800/860-Q.

&#10146; <sup>2</sup>  __rtmon api__ stands for Real Time Monitor API.



## __3. MELFA ROS2 Driver Usage and Installation__
To get started, first follow the MELFA ROS2 Monitor user guide to install the package and configure the required parameters.
After installation, proceed with the RT Toolbox3 setup guides depending on your development environment.
</br>
1. Install and configure MELFA ROS2 Monitor
    - [MELFA ROS2 Monitor user guide](./doc/melfa_ros2_monitor.md) : Usage and Installation of MELFA ROS2 Monitor.
2. Create an RT Toolbox3 project
    - [RT ToolBox3 Setup](./doc/rt_toolbox3_setup.md) : Create your first RT ToolBox3 Project File for MELFA ROS2 Monitor.
3. Choose your development environment
    - [RT ToolBox3 Simulator Setup](./doc/rt_sim_setup.md) : Connect to RT ToolBox3 simulator in place of real robots for MELFA ROS2 Monitor.
    - [RT ToolBox3 Real Robot Setup](./doc/rt_real_setup.md): Connect to a MELFA robot for MELFA ROS2 Monitor.

  
<div> </div>

## __4. Other MELFA ROS2 Related Repositories__

- [MELFA ROS2 Driver](https://github.com/Mitsubishi-Electric-Asia/melfa_ros2_driver) : Official MELFA ROS2 Driver.
- [MELFA ROS 8XS](https://github.com/Mitsubishi-Electric-Asia/melfa_ros2_8xs) : Sample ROS 2 Package for additional axis. 
- [MELFA ROS2 PLC-HMI Integration](https://github.com/Mitsubishi-Electric-Asia/melfa_ros2_iq_simple) : Sample ROS 2 package for memory sharing with Mitsubishi Electric's PLC and HMI. 


<div> </div>

## __5. Contact us / Technical support__
More Support & Service, please contact us on [github](https://github.com/Mitsubishi-Electric-Asia/). For contributing and reporting, refer to [this](./CONTRIBUTING.md) for development related enquiries.

<div> </div>
