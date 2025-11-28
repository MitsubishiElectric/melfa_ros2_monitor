<img src="./figures/MELFA_t.png" width="400" height="98">

# __Connecting to RT ToolBox3 Simulator__

This section will guide you to establish a UDP/IP connection between your ROS 2 computer and _RT ToolBox3 simulator_. It is highly recommended to test your ROS 2 applications on __RT ToolBox3 simulator__ before testing it on a real robot. This is helpful as __RT ToolBox3 simulator__ provides "near real" experience with 1-to-1 error conditions, velocity profiles and close to actual cycle times.

Sections:

[__1. Verify Local IP Address on Windows__](#1-verify-local-ip-address-on-windows)  
[__2. Verifying Local IP address on Ubuntu 22.04 LTS__](#2-verifying-local-ip-address-on-ubuntu-2204-lts)  
[__3. ROS 2 Connection to Simulator__](#3-ros-2-connection-to-simulator)  

## __1. Verify Local IP Address on Windows__

This section will guide you to find your local IP address on your Windows device.

1. Go to __control panel__ &rArr; __Network and Internet__ &rArr; __Network and Sharing Centre__ and select __Change adapter settings__. Your Network adapters will be displayed in a popup window.

    <img src="./figures/local_ip.png" width="960" height="540">

2. Select your network adapter that you wish to connect to your ROS 2 computer. Select __Properties__ &rArr; __Internet Protocol Version 4 (TCP/IPv4)__. In the popup window, you can change your IP to your preferred IP address in your local network.

    <img src="./figures/local_ip_final.png" width="960" height="540">

3. Ensure that RT ToolBox3 is __offline__. Select __offline__ -> __Parameter__ -> __Communication Parameter__. Double click on __Ethernet__. In the new window, select __Realtime Monitor__, ensure that MONMODE is checked. Input the port numbers  as shown in the image.
    <img src="./figures/rtmon_ip_offline.png" width="960" height="540">

>Note: If you are monitoring more than one robot on the same Ubuntu machine, each additional robot must use ports offset by +100 from the previous one.
For example, the second robot should use 12100 for the Receive Port and 12101 for the Send Port.

4. Select __Simulator__ to launch __RT ToolBox3 simulator__.

    <img src="./figures/click_sim.png" width="960" height="540">

## __2. Verifying Local IP address on Ubuntu 22.04 LTS__

This section will guide you to find your local IP address on your Ubuntu 22.04 LTS device. The process is similar on other Ubuntu versions.

1. Go to __Settings__ &rArr; __Network__ &rArr; __Wired (+)__ to create a new Ethernet profile. Under __IPv4 Method__, select __Manual__ and input your preferred IP address for your Linux computer.

    <img src="./figures/linux_ip.png" width="1000" height="800"> 

2. From your Windows computer, ping your Linux computer. You may not be able to ping your Windows computer from your Linux computer due to Windows settings. If ping is unsuccessful, verify your Windows and Linux IP addresses and try again.

    ```bash
    #ping <Linux local IP address>
    # Example:
    ping 192.168.3.150
    ```

## __3. ROS 2 Connection to Simulator__

Assuming that ping is successful, you are now ready to connect your RT ToolBox3 simulator as if it is a _real_ robot. For the purpose of this tutorial, run the following command in the terminal. 

The command argument `packet_lost_log:=0` turns off the warning message for packet losses. It is highly recommended to NOT turn it off when connecting to a real robot. However, connecting to a simulation is fine.

```bash
ros2 launch melfa_mon_bringup monitor_control.launch.py controller_type:="R" robot_ip:=192.168.3.100 packet_lost_log:=0
```

## Other guides:

- Install and configure MELFA ROS2 Monitor.
  - [MELFA ROS2 Monitor user guide](./melfa_ros2_monitor.md) : Usage and Installation of MELFA ROS2 Monitor.

- Create an RT ToolBox3 project.
  - [RT ToolBox3 Setup](./rt_toolbox3_setup.md) : Create your first RT ToolBox3 Project File for MELFA ROS2 Monitor.

- Choose your development environment.
  - [RT ToolBox3 Simulator Setup](./rt_sim_setup.md) : Connect to RT ToolBox3 simulator in place of real robots for MELFA ROS2 Monitor.
  - [RT ToolBox3 Real Robot Setup](./rt_real_setup.md): Connect to a MELFA robot for MELFA ROS2 Monitor.