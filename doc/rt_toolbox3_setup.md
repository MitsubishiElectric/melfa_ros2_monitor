<img src="./figures/MELFA_t.png" width="400" height="98">

# __RT ToolBox3 Setup__

The complete RT ToolBox3 manual can be downloaded at [RT ToolBox3/RT ToolBox3 mini User's Manual](https://www.mitsubishielectric.com/fa/download/search.page?mode=manual&kisyu=/robot&q=RT%20ToolBox3%2FRT%20ToolBox3%20mini%20User%27s%20Manual&sort=0&style=0&lang=2&category1=0&filter_discontinued=0&filter_bundled=0) from [Robot Industrial/Collaborative Robot MELFA Manual](https://www.mitsubishielectric.com/fa/download/search.page?mode=manual&kisyu=/robot). This chapter will cover the basics of project file creation, setting up a robot in simulation and connecting to a physical robot in the context of operating MELFA ROS2.

If you have completed this guide, please take a look at [real robot setup](./rt_real_setup.md) or [RT ToolBox3 simulator setup](./rt_sim_setup.md). 


## __1. Project File Setup__

This section will guide you on creating an RT ToolBox3 project file in the context of ROS 2.

1. Create a _project file_ by selecting __New__, and give your workspace a name and a title, as shown below.

    <img src="./figures/empty_new.png" width="960" height="540">

2. Select __Robot Model__ from the popup window and select your robot. For this tutorial, RV-7FRL-R will be used. 

    <img src="./figures/new_robot.png" width="960" height="540">

3. Select __Communication__ and select your preferred robot connection method.
   - For CRnQ_R USB connection, it is recommended to use a USB cable with a ferrite core such as  [GT09-C30USB-5P](https://mitsubishi-electric-eshop.mee.com/mee/FA_IA/en/EUR/Catalogue/HMI/Cable/GT09-C30USB-5P/p/000000000000166373)
  
        <img src="./figures/new_robot_usb.png" width="960" height="540">

   - For TCP/IP connection, it is recommended to use a CAT5e or above Ethernet cable. For detailed instructions on communicating with the robot via TCP/IP, refer to [Ethernet Function Instruction Manual](https://www.mitsubishielectric.co.jp/fa/download/search.do?mode=manual&kisyu=/robot&q=Ethernet%20Function%20Instruction%20Manual&sort=0&style=0&lang=2&category1=0&filter_readme=0&filter_discontinued=0&filter_bundled=0) from [Robot Industrial/Collaborative Robot MELFA Manual](https://www.mitsubishielectric.co.jp/fa/download/search.do?mode=manual&kisyu=/robot).
  
        <img src="./figures/new_robot_ip.png" width="960" height="540">
4. Select __Travel Axis__ to configure robot travel axis. For this tutorial, the travel axis will not be used. To learn more about setting up the travel axis, refer to [RT ToolBox3/RT ToolBox3 mini User's Manual](https://www.mitsubishielectric.co.jp/fa/download/search.do?mode=manual&kisyu=/robot&q=RT%20Toolbox3&sort=0&style=0&lang=2&category1=0&filter_readme=0&filter_discontinued=0&filter_bundled=0) from [Robot Industrial/Collaborative Robot MELFA Manual](https://www.mitsubishielectric.co.jp/fa/download/search.do?mode=manual&kisyu=/robot). 
   - Select __Finish__.

        <img src="./figures/new_robot_ta.png" width="960" height="540">

5. Select __Cancel__

    <img src="./figures/new_robot_cancel.png" width="960" height="540">

### Your project file is ready!

You are ready to move on to a [real robot setup](./rt_real_setup.md) or a [RT ToolBox3 simulator setup](./rt_sim_setup.md)

## Other guides:

- Install and configure MELFA ROS2 Monitor.
  - [MELFA ROS2 Monitor user guide](./melfa_ros2_monitor.md) : Usage and Installation of MELFA ROS2 Monitor.

- Create an RT ToolBox3 project.
  - [RT ToolBox3 Setup](./rt_toolbox3_setup.md) : Create your first RT ToolBox3 Project File for MELFA ROS2 Monitor.

- Choose your development environment.
  - [RT ToolBox3 Simulator Setup](./rt_sim_setup.md) : Connect to RT ToolBox3 simulator in place of real robots for MELFA ROS2 Monitor.
  - [RT ToolBox3 Real Robot Setup](./rt_real_setup.md): Connect to a MELFA robot for MELFA ROS2 Monitor.