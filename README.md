<img src="./doc/figures/MELFA_t.png" width="400" height="98">

# __MITSUBISHI ELECTRIC MELFA ROS2 Monitor__
    
## __1. Overview__

MELFA ROS2 Monitor exposes MELFA runtime parameters and states to be read from ROS2. This includes error state, error codes, joint states, servo states, internal sensor readings (temperature, current), external sensor readings (force torque sensor). 

</br>

[![ROS2 demo](https://markdown-videos.vercel.app/youtube/RP6lIamz9-8?si=v53JdDJ4vaFBPd5M)](https://youtu.be/RP6lIamz9-8?si=v53JdDJ4vaFBPd5M)
- [Learn more](https://www.mitsubishielectric.com/fa/products/rbt/robot/index.html)
- [Robot catalog](https://www.mitsubishielectric.com/app/fa/download/search.do?kisyu=/robot&mode=catalog)

</br>

## __2. MELFA ROS2 Monitor Feature__

MELFA ROS2 Monitor consists of five components:

### __melfa_mon_bringup__

- provides launch files for start-up.

### __melfa_mon_description__

- contains ros2_control config files.

### __melfa_monitor__

- supports [ros2_control](https://control.ros.org/humble/doc/getting_started/getting_started.html).
- provides __real time communication__<sup>1</sup> hardware interface with our CR800/860-R/Q/D robot controllers via __rtmon api__ <sup>2</sup>. 
- connects to the robot controller via __rtmon api__ to monitor robot parameters via __Real Time Monitor Function__.
- includes quality of life features built into __rtmon api__ such as user configurable disconnection detection and debugging tools.

### __melfa_mon_controllers__

- supports [ros2_control](https://control.ros.org/humble/doc/getting_started/getting_started.html).
- 10 user configurable 32-bit I/O controllers with almost 2 kilobytes of total addressable memory and 1 kilobyte of addressable shared memory.
- provides ROS2 controllers for GPIO control.

### __melfa_mon_msgs__

- provides custom ROS2 msgs.

&#10146; <sup>1</sup>  __real time communication__ frequency is 286Hz for CR800/860-R & CR-800/860-D and 141Hz for CR800/860-Q.

&#10146; <sup>2</sup>  __rtmon api__ stands for Real Time Monitor API.



>Note1: You can download the [CR750/CR751 Series Controller, CR800 Series Controller Ethernet Function Instruction Manual](https://www.mitsubishielectric.com/fa/download/search.page?mode=manual&kisyu=/robot&q=CR750%2FCR751%20Series%20Controller%2C%20CR800%20Series%20Controller%20Ethernet%20Function%20Instruction%20Manual&sort=0&style=0&lang=2&category1=0&filter_discontinued=0&filter_bundled=0) from [Robot Industrial/Collaborative Robot MELFA Manual](https://www.mitsubishielectric.com/fa/download/search.page?mode=manual&kisyu=/robot).</br>


## __3. MELFA ROS2 Driver Usage and Installation__

MELFA ROS2 Driver is designed to interface CR800 robot controllers with the ROS2 so that developers can leverage the contributions from the Open Source Community with an industry proven robot platform. Please select a guide below to get started.
</br>

- [MELFA ROS2 user guide](./doc/melfa_ros2_monitor.md) : Usage and Installation of MELFA ROS2.
- [RT Toolbox3 Setup](./doc/rt_toolbox3_setup.md) : Create your first RT Toolbox3 Project File for ROS2.
- [RT Toolbox3 Simulator Setup](./doc/rt_sim_setup.md) : Connect to RT Toolbox3 simulator as if it is a real robot.
- [RT Toolbox3 Real Robot Setup](./doc/rt_real_setup.md): Connect to a MELFA robot.

  
<div> </div>

## __4. Other MELFA ROS2 Related Repositories__

- [MELFA ROS2 Driver](https://github.com/Mitsubishi-Electric-Asia/melfa_ros2_driver) : Official MELFA ROS2 Driver.


<div> </div>

## __5. Contact us / Technical support__
More Support & Service, please contact us on [github](https://github.com/Mitsubishi-Electric-Asia/). For contributing and reporting, refer to [this](./CONTRIBUTING.md) for development related enquiries.

<div> </div>
