//  COPYRIGHT (C) 2025 Mitsubishi Electric Corporation

//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at

//      http://www.apache.org/licenses/LICENSE-2.0

//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

#include "melfa_monitor/hardware_interface.hpp"

namespace melfa_monitor
{

std::vector<uint16_t> MELFAMonitorHardwareInterface::readDataQueue(const std::string& limit_string)
{
  /**
   * @brief Reads configured IO Limit parameters
   *
   * This function reads the IO limit strings from hardware parameter and generates it as a vector
   *
   * @param limit_string A string with IO limits separated by delimiter (,)
   * @return The delimited values as a vector
   *
   * @note The function can delimit and vectorize any number of values
   */

  std::vector<uint16_t> result;
  std::istringstream ss(limit_string);
  std::string delimited_value;

  while (std::getline(ss, delimited_value, ','))
  {
    result.push_back(static_cast<uint16_t>(std::stoi(delimited_value)));
  }
  return result;
}

void MELFAMonitorHardwareInterface::setIO(const std::vector<double>& io_commands)
{
  /**
   * @brief Sets IO parameters for the MELFA API
   *
   * This function sets required IO parameters for the command packet sent to the API
   *
   * @param io_commands A vector with IO command parameters
   * @param read_only A boolean parameter to differentiate read and write
   * @return void
   */

  api_wrap_->cmd_pack.INBitTop = static_cast<uint16_t>(io_commands[0]);
  api_wrap_->cmd_pack.OUTBitTop = static_cast<uint16_t>(io_commands[1]);
}

hardware_interface::CallbackReturn
MELFAMonitorHardwareInterface::on_init(const hardware_interface::HardwareInfo& system_info)
{
  /**
   * @brief Initialization method for MELFAMonitorHardwareInterface class
   *
   * This function initializes IO data entities and validates hardware components
   *
   * @param system_info hardware_info structure with data from robot description file.
   * @returns CallbackReturn::SUCCESS if components and interfaces meets set expectations
   * @returns CallbackReturn::ERROR if component or interface doesn't meet set expectations
   *
   */

  if (hardware_interface::SystemInterface::on_init(system_info) != hardware_interface::CallbackReturn::SUCCESS)
  {
    return hardware_interface::CallbackReturn::ERROR;
  }
  info_ = system_info;
  execution_init_ = true;

  // Reading IO limits from hardware info structure for specific IO interfaces
  general1_io_limits_ = readDataQueue(info_.hardware_parameters["general1_io_limits"]);
  general2_io_limits_ = readDataQueue(info_.hardware_parameters["general2_io_limits"]);
  user1_io_limits_ = readDataQueue(info_.hardware_parameters["user1_io_limits"]);
  user2_io_limits_ = readDataQueue(info_.hardware_parameters["user2_io_limits"]);
  user3_io_limits_ = readDataQueue(info_.hardware_parameters["user3_io_limits"]);
  user4_io_limits_ = readDataQueue(info_.hardware_parameters["user4_io_limits"]);
  user5_io_limits_ = readDataQueue(info_.hardware_parameters["user5_io_limits"]);
  user6_io_limits_ = readDataQueue(info_.hardware_parameters["user6_io_limits"]);
  user7_io_limits_ = readDataQueue(info_.hardware_parameters["user7_io_limits"]);
  user8_io_limits_ = readDataQueue(info_.hardware_parameters["user8_io_limits"]);

  // Read data queue
  dat1_queue_ = readDataQueue(info_.hardware_parameters["data_1_queue"]);
  dat2_queue_ = readDataQueue(info_.hardware_parameters["data_2_queue"]);
  dat3_queue_ = readDataQueue(info_.hardware_parameters["data_3_queue"]);
  dat4_queue_ = readDataQueue(info_.hardware_parameters["data_4_queue"]);

  dat1_count_ = 0;
  dat2_count_ = 0;
  dat3_count_ = 0;
  dat4_count_ = 0;

  // joint_broadcaster_toggle = stoi(info_.hardware_parameters["joint_broadcaster"]);
  if (info_.hardware_parameters["joint_broadcaster"] == "False")
  {
    joint_broadcaster_toggle = 0;
    RCLCPP_WARN(rclcpp::get_logger("MELFAMonitorHardwareInterface"), "Joint Broadcaster is OFF");
  }
  else
  {
    joint_broadcaster_toggle = 1;
    RCLCPP_WARN(rclcpp::get_logger("MELFAMonitorHardwareInterface"), "Joint Broadcaster is ON");
  }

  // Reading user defined IO binary control mode and MELFA Controller type
  io_control_mode_ = info_.hardware_parameters["io_control_mode"];
  controller_type_ = info_.hardware_parameters["controller_type"];
  RCLCPP_INFO(rclcpp::get_logger("MELFAMonitorHardwareInterface"), "CONTROLLER TYPE: %s.", controller_type_.c_str());

  // Joint position commands and states initialization
  if (joint_broadcaster_toggle != 0)
  {
    joint_position_states_.resize(info_.joints.size(), std::numeric_limits<double>::quiet_NaN());
    joint_velocity_states_.resize(info_.joints.size(), std::numeric_limits<double>::quiet_NaN());
    joint_ampere_rms_states_.resize(info_.joints.size(), std::numeric_limits<double>::quiet_NaN());
    joint_encoder_misscount_states_.resize(info_.joints.size(), std::numeric_limits<double>::quiet_NaN());
    joint_encoder_temperature_states_.resize(info_.joints.size(), std::numeric_limits<double>::quiet_NaN());
    motor_voltage_states_.resize(info_.joints.size(), std::numeric_limits<double>::quiet_NaN());
  }
  fts_curr_states_.resize(info_.sensors[0].state_interfaces.size(), std::numeric_limits<double>::quiet_NaN());

  fts_before_states_.resize(info_.sensors[1].state_interfaces.size(), std::numeric_limits<double>::quiet_NaN());

  fts_after_states_.resize(info_.sensors[2].state_interfaces.size(), std::numeric_limits<double>::quiet_NaN());

  tcp_pose_states_.resize(info_.sensors[3].state_interfaces.size(), std::numeric_limits<double>::quiet_NaN());

  // Joint state and command interface validation
  for (const hardware_interface::ComponentInfo& joint : info_.joints)
  {
    if (joint.state_interfaces.size() != 6)
    {
      RCLCPP_FATAL(rclcpp::get_logger("MELFAMonitorHardwareInterface"),
                   "Joint '%s' has %zu state interface. 6 expected.", joint.name.c_str(),
                   joint.state_interfaces.size());
      return hardware_interface::CallbackReturn::ERROR;
    }

    if (joint.state_interfaces[0].name != hardware_interface::HW_IF_POSITION)
    {
      RCLCPP_FATAL(rclcpp::get_logger("MELFAMonitorHardwareInterface"),
                   "Joint '%s' have %s position state interface as first state interface. '%s' expected.",
                   joint.name.c_str(), joint.state_interfaces[0].name.c_str(), hardware_interface::HW_IF_POSITION);
      return hardware_interface::CallbackReturn::ERROR;
    }

    if (joint.state_interfaces[1].name != hardware_interface::HW_IF_VELOCITY)
    {
      RCLCPP_FATAL(rclcpp::get_logger("MELFAMonitorHardwareInterface"),
                   "Joint '%s' have %s velocity state interface as second state interface. '%s' expected.",
                   joint.name.c_str(), joint.state_interfaces[1].name.c_str(), hardware_interface::HW_IF_VELOCITY);
      return hardware_interface::CallbackReturn::ERROR;
    }

    if (joint.state_interfaces[2].name != "ampere_rms")
    {
      RCLCPP_FATAL(rclcpp::get_logger("MELFAMonitorHardwareInterface"),
                   "Joint '%s' have %s ampere_rms interface as third state interface. '%s' expected.",
                   joint.name.c_str(), joint.state_interfaces[2].name.c_str(), "ampere_rms");
      return hardware_interface::CallbackReturn::ERROR;
    }

    if (joint.state_interfaces[3].name != "encoder_misscount")
    {
      RCLCPP_FATAL(rclcpp::get_logger("MELFAMonitorHardwareInterface"),
                   "Joint '%s' have %s encoder_misscount interface as forth state interface. '%s' expected.",
                   joint.name.c_str(), joint.state_interfaces[3].name.c_str(), "encoder_misscount");
      return hardware_interface::CallbackReturn::ERROR;
    }

    if (joint.state_interfaces[4].name != "encoder_temperature")
    {
      RCLCPP_FATAL(rclcpp::get_logger("MELFAMonitorHardwareInterface"),
                   "Joint '%s' have %s encoder_temperature interface as fifth state interface. '%s' expected.",
                   joint.name.c_str(), joint.state_interfaces[4].name.c_str(), "encoder_temperature");
      return hardware_interface::CallbackReturn::ERROR;
    }

    if (joint.state_interfaces[5].name != "motor_voltage")
    {
      RCLCPP_FATAL(rclcpp::get_logger("MELFAMonitorHardwareInterface"),
                   "Joint '%s' have %s motor_voltage interface as sixth state interface. '%s' expected.",
                   joint.name.c_str(), joint.state_interfaces[5].name.c_str(), "motor_voltage");
      return hardware_interface::CallbackReturn::ERROR;
    }
  }

  // GPIO components, state and command interfaces validation
  if (info_.gpios.size() != 12)
  {
    RCLCPP_FATAL(rclcpp::get_logger("MELFAMonitorHardwareInterface"),
                 "MELFAMonitorHardwareInterface has '%ld' GPIO components, '%d' expected.", info_.gpios.size(), 12);
    return hardware_interface::CallbackReturn::ERROR;
  }
  for (const hardware_interface::ComponentInfo& gpios : info_.gpios)
  {
    if (!gpios.name.compare(io_control_mode_name) || !gpios.name.compare(ctrl_name))
    {
      if (gpios.command_interfaces.size() != 1)
      {
        RCLCPP_FATAL(rclcpp::get_logger("MELFAMonitorHardwareInterface"),
                     "GPIO component %s has '%ld' command interfaces, '%d' expected, as it has no write permission.",
                     gpios.name.c_str(), gpios.command_interfaces.size(), 1);
        return hardware_interface::CallbackReturn::ERROR;
      }
      if (gpios.state_interfaces.size() != 1)
      {
        RCLCPP_FATAL(rclcpp::get_logger("MELFAMonitorHardwareInterface"),
                     "GPIO component %s has '%ld' state interfaces, '%d' expected.", gpios.name.c_str(),
                     gpios.state_interfaces.size(), 1);
        return hardware_interface::CallbackReturn::ERROR;
      }
    }
    else
    {
      if (gpios.command_interfaces.size() != 2)
      {
        RCLCPP_FATAL(rclcpp::get_logger("MELFAMonitorHardwareInterface"),
                     "GPIO component %s has '%ld' command interfaces, '%d' expected, as it has no write permission.",
                     gpios.name.c_str(), gpios.command_interfaces.size(), 2);
        return hardware_interface::CallbackReturn::ERROR;
      }
      if (gpios.state_interfaces.size() != 4)
      {
        RCLCPP_FATAL(rclcpp::get_logger("MELFAMonitorHardwareInterface"),
                     "GPIO component %s has '%ld' state interfaces, '%d' expected.", gpios.name.c_str(),
                     gpios.state_interfaces.size(), 4);
        return hardware_interface::CallbackReturn::ERROR;
      }
    }
  }
  return hardware_interface::CallbackReturn::SUCCESS;
}

hardware_interface::CallbackReturn
MELFAMonitorHardwareInterface::on_activate(const rclcpp_lifecycle::State& previous_state)
{
  /**
   * @brief Activation method for MELFAMonitorHardwareInterface class
   *
   * This function initializes control cycle, communication, joint position states and controller type
   *
   * @param previous_state lifecycle state object representing state before current state
   * @returns CallbackReturn::SUCCESS if components and interfaces meets set expectations
   *
   */
  RCLCPP_INFO(rclcpp::get_logger("MELFAMonitorHardwareInterface"), "Starting ...please wait...");

  float control_cycle_period;
  if (info_.hardware_parameters["controller_type"] == "R")
    control_cycle_period = 3.5F;
  if (info_.hardware_parameters["controller_type"] == "Q")
    control_cycle_period = 7.11F;
  if (info_.hardware_parameters["controller_type"] == "D")
    control_cycle_period = 3.5F;

  // Configure communication settings
  api_wrap_ = std::make_unique<MelfaEthernet::rtmon>(control_cycle_period);

  // API debug mode
  api_wrap_->RT_API_DEBUG_MODE = 0;

  api_wrap_->robot_ip.dst_ip_address = info_.hardware_parameters["robot_ip"];
  api_wrap_->robot_ip.sendport = stoi(info_.hardware_parameters["send_port"]);
  api_wrap_->robot_ip.recvport = stoi(info_.hardware_parameters["recv_port"]);
  packet_lost_log = stoi(info_.hardware_parameters["packet_lost_log"]);
  scara = stoi(info_.hardware_parameters["scara"]);
  api_wrap_->create_port();

  api_wrap_->cmd_pack.cmd_type = MON_CMD_START;                   // set start command
  *(api_wrap_->cmd_pack.mon_dat) = dat1_queue_[dat1_count_];      // set first feedback
  *(api_wrap_->cmd_pack.mon_dat + 1) = dat2_queue_[dat2_count_];  // set second feedback
  *(api_wrap_->cmd_pack.mon_dat + 2) = dat3_queue_[dat3_count_];  // set thrid feedback
  *(api_wrap_->cmd_pack.mon_dat + 3) = dat4_queue_[dat4_count_];  // set forth feedback

  // Initialize external control in robot controller.
  uint8_t to_cc = 0;
  while (api_wrap_->ReadFromRobot_FB_() != 0 && to_cc < 100)
  {
    if (api_wrap_->WriteToRobot_MON_() != 0)
    {
      RCLCPP_FATAL(rclcpp::get_logger("MELFAMonitorHardwareInterface"), "ERROR: Unable to send command to robot.");
      return hardware_interface::CallbackReturn::ERROR;
    }
    to_cc++;
  }

  if (to_cc >= 100)
  {
    RCLCPP_FATAL(rclcpp::get_logger("MELFAMonitorHardwareInterface"),
                 "ERROR: Unable to connect to robot. First receive FAIL");
    return hardware_interface::CallbackReturn::ERROR;
  }

  api_wrap_->robot_status = true;
  RCLCPP_INFO(rclcpp::get_logger("MELFAMonitorHardwareInterface"), "System successfully started!");

  // Reads joint state from rtmon API feedback packet
  if (joint_broadcaster_toggle != 0)
  {
    joint_position_states_[0] = api_wrap_->fb_pack.jnt_EFB.j1;
    joint_position_states_[1] = api_wrap_->fb_pack.jnt_EFB.j2;
    joint_position_states_[2] = api_wrap_->fb_pack.jnt_EFB.j3;
    if(scara==1){
        joint_position_states_[2]/=1000.0; // SCARA robot type correction. mm to m.
    }
    joint_position_states_[3] = api_wrap_->fb_pack.jnt_EFB.j4;
    joint_position_states_[4] = api_wrap_->fb_pack.jnt_EFB.j5;
    joint_position_states_[5] = api_wrap_->fb_pack.jnt_EFB.j6;
    joint_position_states_[6] = api_wrap_->fb_pack.jnt_EFB.j7;
    joint_position_states_[7] = api_wrap_->fb_pack.jnt_EFB.j8;

    joint_velocity_states_[0] = (double)(api_wrap_->fb_pack.speed_fb.m1) / 100.0;
    joint_velocity_states_[1] = (double)(api_wrap_->fb_pack.speed_fb.m2) / 100.0;
    joint_velocity_states_[2] = (double)(api_wrap_->fb_pack.speed_fb.m3) / 100.0;
    joint_velocity_states_[3] = (double)(api_wrap_->fb_pack.speed_fb.m4) / 100.0;
    joint_velocity_states_[4] = (double)(api_wrap_->fb_pack.speed_fb.m5) / 100.0;
    joint_velocity_states_[5] = (double)(api_wrap_->fb_pack.speed_fb.m6) / 100.0;
    joint_velocity_states_[6] = (double)(api_wrap_->fb_pack.speed_fb.m7) / 100.0;
    joint_velocity_states_[7] = (double)(api_wrap_->fb_pack.speed_fb.m8) / 100.0;

    joint_ampere_rms_states_[0] = api_wrap_->fb_pack.curr_fb_arms.f1;
    joint_ampere_rms_states_[1] = api_wrap_->fb_pack.curr_fb_arms.f2;
    joint_ampere_rms_states_[2] = api_wrap_->fb_pack.curr_fb_arms.f3;
    joint_ampere_rms_states_[3] = api_wrap_->fb_pack.curr_fb_arms.f4;
    joint_ampere_rms_states_[4] = api_wrap_->fb_pack.curr_fb_arms.f5;
    joint_ampere_rms_states_[5] = api_wrap_->fb_pack.curr_fb_arms.f6;
    joint_ampere_rms_states_[6] = api_wrap_->fb_pack.curr_fb_arms.f7;
    joint_ampere_rms_states_[7] = api_wrap_->fb_pack.curr_fb_arms.f8;

    joint_encoder_misscount_states_[0] = api_wrap_->fb_pack.enc_mis.m1;
    joint_encoder_misscount_states_[1] = api_wrap_->fb_pack.enc_mis.m2;
    joint_encoder_misscount_states_[2] = api_wrap_->fb_pack.enc_mis.m3;
    joint_encoder_misscount_states_[3] = api_wrap_->fb_pack.enc_mis.m4;
    joint_encoder_misscount_states_[4] = api_wrap_->fb_pack.enc_mis.m5;
    joint_encoder_misscount_states_[5] = api_wrap_->fb_pack.enc_mis.m6;
    joint_encoder_misscount_states_[6] = api_wrap_->fb_pack.enc_mis.m7;
    joint_encoder_misscount_states_[7] = api_wrap_->fb_pack.enc_mis.m8;

    joint_encoder_temperature_states_[0] = api_wrap_->fb_pack.enc_temp.m1;
    joint_encoder_temperature_states_[1] = api_wrap_->fb_pack.enc_temp.m2;
    joint_encoder_temperature_states_[2] = api_wrap_->fb_pack.enc_temp.m3;
    joint_encoder_temperature_states_[3] = api_wrap_->fb_pack.enc_temp.m4;
    joint_encoder_temperature_states_[4] = api_wrap_->fb_pack.enc_temp.m5;
    joint_encoder_temperature_states_[5] = api_wrap_->fb_pack.enc_temp.m6;
    joint_encoder_temperature_states_[6] = api_wrap_->fb_pack.enc_temp.m7;
    joint_encoder_temperature_states_[7] = api_wrap_->fb_pack.enc_temp.m8;

    motor_voltage_states_[0] = api_wrap_->fb_pack.motor_v.m1;
    motor_voltage_states_[1] = api_wrap_->fb_pack.motor_v.m2;
    motor_voltage_states_[2] = api_wrap_->fb_pack.motor_v.m3;
    motor_voltage_states_[3] = api_wrap_->fb_pack.motor_v.m4;
    motor_voltage_states_[4] = api_wrap_->fb_pack.motor_v.m5;
    motor_voltage_states_[5] = api_wrap_->fb_pack.motor_v.m6;
    motor_voltage_states_[6] = api_wrap_->fb_pack.motor_v.m7;
    motor_voltage_states_[7] = api_wrap_->fb_pack.motor_v.m8;

    
    RCLCPP_INFO(rclcpp::get_logger("MELFAMonitorHardwareInterface"), "Joint States Published!");
  }

  // Reads joint position state from rtmon API feedback packet
  RCLCPP_INFO(rclcpp::get_logger("MELFAMonitorHardwareInterface"), "Loading FT sensor 1...");
  fts_curr_states_[0] = (double)api_wrap_->fb_pack.fs_curr_pose.fx;
  fts_curr_states_[1] = (double)api_wrap_->fb_pack.fs_curr_pose.fy;
  fts_curr_states_[2] = (double)api_wrap_->fb_pack.fs_curr_pose.fz;
  fts_curr_states_[3] = (double)api_wrap_->fb_pack.fs_curr_pose.mx;
  fts_curr_states_[4] = (double)api_wrap_->fb_pack.fs_curr_pose.my;
  fts_curr_states_[5] = (double)api_wrap_->fb_pack.fs_curr_pose.mz;

  RCLCPP_INFO(rclcpp::get_logger("MELFAMonitorHardwareInterface"), "Loading FT sensor 2...");
  fts_before_states_[0] = api_wrap_->fb_pack.fs_before.fx;
  fts_before_states_[1] = api_wrap_->fb_pack.fs_before.fy;
  fts_before_states_[2] = api_wrap_->fb_pack.fs_before.fz;
  fts_before_states_[3] = api_wrap_->fb_pack.fs_before.mx;
  fts_before_states_[4] = api_wrap_->fb_pack.fs_before.my;
  fts_before_states_[5] = api_wrap_->fb_pack.fs_before.mz;

  RCLCPP_INFO(rclcpp::get_logger("MELFAMonitorHardwareInterface"), "Loading FT sensor 3...");
  fts_after_states_[0] = api_wrap_->fb_pack.fs_after.fx;
  fts_after_states_[1] = api_wrap_->fb_pack.fs_after.fy;
  fts_after_states_[2] = api_wrap_->fb_pack.fs_after.fz;
  fts_after_states_[3] = api_wrap_->fb_pack.fs_after.mx;
  fts_after_states_[4] = api_wrap_->fb_pack.fs_after.my;
  fts_after_states_[5] = api_wrap_->fb_pack.fs_after.mz;

  RCLCPP_INFO(rclcpp::get_logger("MELFAMonitorHardwareInterface"), "FT Sensor States Published!");

  tf2::Quaternion orientation;
  orientation.setRPY(api_wrap_->fb_pack.pos_EFB.a, api_wrap_->fb_pack.pos_EFB.b, api_wrap_->fb_pack.pos_EFB.c);
  tcp_pose_states_[0] = api_wrap_->fb_pack.pos_EFB.x / 1000.0;
  tcp_pose_states_[1] = api_wrap_->fb_pack.pos_EFB.y / 1000.0;
  tcp_pose_states_[2] = api_wrap_->fb_pack.pos_EFB.z / 1000.0;
  tcp_pose_states_[3] = orientation.x();
  tcp_pose_states_[4] = orientation.y();
  tcp_pose_states_[5] = orientation.z();
  tcp_pose_states_[6] = orientation.w();

  // Initialization of IO commands
  general1_io_commands_[0] = general1_io_limits_[0];
  general1_io_commands_[1] = general1_io_limits_[0];

  general2_io_commands_[0] = general2_io_limits_[0];
  general2_io_commands_[1] = general2_io_limits_[0];

  user1_io_commands_[0] = user1_io_limits_[0];
  user1_io_commands_[1] = user1_io_limits_[0];

  user2_io_commands_[0] = user2_io_limits_[0];
  user2_io_commands_[1] = user2_io_limits_[0];

  user3_io_commands_[0] = user3_io_limits_[0];
  user3_io_commands_[1] = user3_io_limits_[0];

  user4_io_commands_[0] = user4_io_limits_[0];
  user4_io_commands_[1] = user4_io_limits_[0];

  user5_io_commands_[0] = user5_io_limits_[0];
  user5_io_commands_[1] = user5_io_limits_[0];

  user6_io_commands_[0] = user6_io_limits_[0];
  user6_io_commands_[1] = user6_io_limits_[0];

  user7_io_commands_[0] = user7_io_limits_[0];
  user7_io_commands_[1] = user7_io_limits_[0];

  user8_io_commands_[0] = user8_io_limits_[0];
  user8_io_commands_[1] = user8_io_limits_[0];

  // Initialization of Controller type command
  if (controller_type_ == "R")
    ctrl_type_io_state_[0] = 1.0;
  if (controller_type_ == "Q")
    ctrl_type_io_state_[0] = 2.0;
  if (controller_type_ == "D")
    ctrl_type_io_state_[0] = 3.0;

  // Initialization of Controller type command
  mode_io_command_[0] = std::bitset<10>(io_control_mode_).to_ulong();

  return hardware_interface::CallbackReturn::SUCCESS;
}

hardware_interface::CallbackReturn
MELFAMonitorHardwareInterface::on_deactivate(const rclcpp_lifecycle::State& previous_state)
{
  /**
   * @brief Deactivation method for MELFAMonitorHardwareInterface class
   *
   * This function sends stopping signal to the MELFA Controller API
   *
   * @param previous_state lifecycle state object representing state before current state
   * @returns CallbackReturn::SUCCESS if deactivation condition is met
   * @returns CallbackReturn::ERROR if deactivation condition is not met
   *
   */

  // Sends End command to the MELFA controller API for terminating communication
  RCLCPP_INFO(rclcpp::get_logger("MELFAMonitorHardwareInterface"), "Stopping ...please wait...");
  api_wrap_->cmd_pack.cmd_type = MON_CMD_END;
  int is_deactivated_sent = api_wrap_->WriteToRobot_MON_();
  int is_deactivated_recv = api_wrap_->ReadFromRobot_FB_();

  if ((is_deactivated_sent == 0) && (is_deactivated_recv != 0))
  {
    RCLCPP_INFO(rclcpp::get_logger("MELFAMonitorHardwareInterface"), "System successfully stopped!");
    return hardware_interface::CallbackReturn::SUCCESS;
  }
  else
  {
    RCLCPP_INFO(rclcpp::get_logger("MELFAMonitorHardwareInterface"), "Not able to stop system!!!");
    return hardware_interface::CallbackReturn::ERROR;
  }
}

std::vector<hardware_interface::StateInterface> MELFAMonitorHardwareInterface::export_state_interfaces()
{
  /**
   * @brief State Interfaces export method for MELFAMonitorHardwareInterface class
   *
   * This function exports available state interfaces to the relevant ROS2 controllers
   *
   * @returns state interfaces as vector
   * @note addStateInterfaces function facilitates addition of different IO state interfaces
   *
   */

  std::vector<hardware_interface::StateInterface> state_interfaces_;

  // Add joint position states as reference to State interfaces
  if (joint_broadcaster_toggle != 0)
  {
    for (uint i = 0; i < info_.joints.size(); i++)
    {
      state_interfaces_.emplace_back(hardware_interface::StateInterface(
          info_.joints[i].name, hardware_interface::HW_IF_POSITION, &joint_position_states_[i]));
    }

    for (uint i = 0; i < info_.joints.size(); i++)
    {
      state_interfaces_.emplace_back(hardware_interface::StateInterface(
          info_.joints[i].name, hardware_interface::HW_IF_VELOCITY, &joint_velocity_states_[i]));
    }

    for (uint i = 0; i < info_.joints.size(); i++)
    {
      state_interfaces_.emplace_back(
          hardware_interface::StateInterface(info_.joints[i].name, "ampere_rms", &joint_ampere_rms_states_[i]));
    }
    for (uint i = 0; i < info_.joints.size(); i++)
    {
      state_interfaces_.emplace_back(hardware_interface::StateInterface(info_.joints[i].name, "encoder_misscount",
                                                                        &joint_encoder_misscount_states_[i]));
    }
    for (uint i = 0; i < info_.joints.size(); i++)
    {
      state_interfaces_.emplace_back(hardware_interface::StateInterface(info_.joints[i].name, "encoder_temperature",
                                                                        &joint_encoder_temperature_states_[i]));
    }
    for (uint i = 0; i < info_.joints.size(); i++)
    {
      state_interfaces_.emplace_back(hardware_interface::StateInterface(info_.joints[i].name, "motor_voltage",
                                                                        &motor_voltage_states_[i]));
    }
  }

  // export sensor state interface

  for (uint i = 0; i < info_.sensors[0].state_interfaces.size(); i++)
  {
    state_interfaces_.emplace_back(hardware_interface::StateInterface(
        info_.sensors[0].name, info_.sensors[0].state_interfaces[i].name, &fts_curr_states_[i]));
  }

  for (uint i = 0; i < info_.sensors[1].state_interfaces.size(); i++)
  {
    state_interfaces_.emplace_back(hardware_interface::StateInterface(
        info_.sensors[1].name, info_.sensors[1].state_interfaces[i].name, &fts_before_states_[i]));
  }

  for (uint i = 0; i < info_.sensors[2].state_interfaces.size(); i++)
  {
    state_interfaces_.emplace_back(hardware_interface::StateInterface(
        info_.sensors[2].name, info_.sensors[2].state_interfaces[i].name, &fts_after_states_[i]));
  }

  for (uint i = 0; i < info_.sensors[3].state_interfaces.size(); i++)
  {
    state_interfaces_.emplace_back(hardware_interface::StateInterface(
        info_.sensors[3].name, info_.sensors[3].state_interfaces[i].name, &tcp_pose_states_[i]));
  }

  general1_io_states_.resize(MELFAGpioConstants::io_fb_pack_size);
  general2_io_states_.resize(MELFAGpioConstants::io_fb_pack_size);
  user1_io_states_.resize(MELFAGpioConstants::io_fb_pack_size);
  user2_io_states_.resize(MELFAGpioConstants::io_fb_pack_size);
  user3_io_states_.resize(MELFAGpioConstants::io_fb_pack_size);
  user4_io_states_.resize(MELFAGpioConstants::io_fb_pack_size);
  user5_io_states_.resize(MELFAGpioConstants::io_fb_pack_size);
  user6_io_states_.resize(MELFAGpioConstants::io_fb_pack_size);
  user7_io_states_.resize(MELFAGpioConstants::io_fb_pack_size);
  user8_io_states_.resize(MELFAGpioConstants::io_fb_pack_size);
  mode_io_state_.resize(MELFAGpioConstants::io_interface_state_size);
  ctrl_type_io_state_.resize(MELFAGpioConstants::io_interface_state_size);

  size_t counter_ = 0;

  // Add IO states as reference to State interfaces
  auto addStateInterfaces = [&](const std::string& interface_name, auto& interface_states) {
    counter_ = 0;
    for (size_t i = 0; i < info_.gpios.size(); ++i)
    {
      if (info_.gpios[i].name == interface_name)
      {
        for (auto state_if : info_.gpios.at(i).state_interfaces)
        {
          state_interfaces_.emplace_back(
              hardware_interface::StateInterface(info_.gpios.at(i).name, state_if.name, &interface_states[counter_++]));
          RCLCPP_INFO(rclcpp::get_logger("MELFAMonitorHardwareInterface"), "Added State Interface:  %s/%s",
                      info_.gpios.at(i).name.c_str(), state_if.name.c_str());
        }
        break;
      }
    }
  };

  // Add IO interfaces, control mode and controller type as reference to State interface
  addStateInterfaces(general1_io_name, general1_io_states_);
  addStateInterfaces(general2_io_name, general2_io_states_);
  addStateInterfaces(user1_io_name, user1_io_states_);
  addStateInterfaces(user2_io_name, user2_io_states_);
  addStateInterfaces(user3_io_name, user3_io_states_);
  addStateInterfaces(user4_io_name, user4_io_states_);
  addStateInterfaces(user5_io_name, user5_io_states_);
  addStateInterfaces(user6_io_name, user6_io_states_);
  addStateInterfaces(user7_io_name, user7_io_states_);
  addStateInterfaces(user8_io_name, user8_io_states_);
  addStateInterfaces(io_control_mode_name, mode_io_state_);
  addStateInterfaces(ctrl_name, ctrl_type_io_state_);

  return state_interfaces_;
}

std::vector<hardware_interface::CommandInterface> MELFAMonitorHardwareInterface::export_command_interfaces()
{
  /**
   * @brief Command Interfaces export method for MELFAMonitorHardwareInterface class
   *
   * This function exports available command interfaces to the relevant ROS2 controllers
   *
   * @returns command interfaces as vector
   * @note addCommandInterfaces function facilitates addition of different IO command interfaces
   *
   */

  std::vector<hardware_interface::CommandInterface> command_interfaces_;

  general1_io_commands_.resize(MELFAGpioConstants::io_cmd_pack_size);
  general2_io_commands_.resize(MELFAGpioConstants::io_cmd_pack_size);
  user1_io_commands_.resize(MELFAGpioConstants::io_cmd_pack_size);
  user2_io_commands_.resize(MELFAGpioConstants::io_cmd_pack_size);
  user3_io_commands_.resize(MELFAGpioConstants::io_cmd_pack_size);
  user4_io_commands_.resize(MELFAGpioConstants::io_cmd_pack_size);
  user5_io_commands_.resize(MELFAGpioConstants::io_cmd_pack_size);
  user6_io_commands_.resize(MELFAGpioConstants::io_cmd_pack_size);
  user7_io_commands_.resize(MELFAGpioConstants::io_cmd_pack_size);
  user8_io_commands_.resize(MELFAGpioConstants::io_cmd_pack_size);
  mode_io_command_.resize(MELFAGpioConstants::io_interface_command_size);
  ctrl_type_io_command_.resize(MELFAGpioConstants::io_interface_command_size);

  size_t counter_ = 0;

  // Add IO commands as reference to Command interfaces
  auto addCommandInterfaces = [&](const std::string& interface_name, auto& interface_commands) {
    counter_ = 0;
    for (size_t i = 0; i < info_.gpios.size(); ++i)
    {
      if (info_.gpios[i].name == interface_name)
      {
        for (auto command_if : info_.gpios.at(i).command_interfaces)
        {
          command_interfaces_.emplace_back(hardware_interface::CommandInterface(info_.gpios.at(i).name, command_if.name,
                                                                                &interface_commands[counter_++]));
          RCLCPP_INFO(rclcpp::get_logger("MELFAMonitorHardwareInterface"), "Added Command Interface:  %s/%s",
                      info_.gpios.at(i).name.c_str(), command_if.name.c_str());
        }
        break;
      }
    }
  };

  // Add IO interfaces, control mode and controller type as reference to Command interfaces
  addCommandInterfaces(general1_io_name, general1_io_commands_);
  addCommandInterfaces(general2_io_name, general2_io_commands_);
  addCommandInterfaces(user1_io_name, user1_io_commands_);
  addCommandInterfaces(user2_io_name, user2_io_commands_);
  addCommandInterfaces(user3_io_name, user3_io_commands_);
  addCommandInterfaces(user4_io_name, user4_io_commands_);
  addCommandInterfaces(user5_io_name, user5_io_commands_);
  addCommandInterfaces(user6_io_name, user6_io_commands_);
  addCommandInterfaces(user7_io_name, user7_io_commands_);
  addCommandInterfaces(user8_io_name, user8_io_commands_);
  addCommandInterfaces(io_control_mode_name, mode_io_command_);
  addCommandInterfaces(ctrl_name, ctrl_type_io_command_);

  return command_interfaces_;
}

hardware_interface::return_type MELFAMonitorHardwareInterface::read(const rclcpp::Time& time,
                                                                    const rclcpp::Duration& period)
{
  /**
   * @brief Read method for MELFAMonitorHardwareInterface class
   *
   * This function reads joint states and IO states from MELFA Controller API feedback
   *
   * @param time The time recorded at the beginning of the current iteration of the control loop
   * @param period The duration measured for the last iteration of the control loop.
   * @returns hardware_interface::return_type::OK after interfaces are read
   * @note readIOFeedback function facilitates reading different IO state interfaces
   *
   */

  // Read Feedback packet from MELFA Controller API
  if (api_wrap_->ReadFromRobot_FB_() != 0)
  {
    if (!api_wrap_->robot_status)
    {
      RCLCPP_FATAL(rclcpp::get_logger("MELFAMonitorHardwareInterface"), "ERROR: Connection lost.");
      return hardware_interface::return_type::ERROR;
    }
    if (packet_lost_log != 0)
    {
      RCLCPP_WARN(rclcpp::get_logger("MELFAMonitorHardwareInterface"), "WARN: Packet lost. %d",
                  api_wrap_->packet_recv_lost);
    }

    return hardware_interface::return_type::OK;
  }
  if (dat1_count_ < dat1_queue_.size() - 1)
  {
    dat1_count_++;
  }
  else
  {
    dat1_count_ = 0;
  }
  if (dat2_count_ < dat2_queue_.size() - 1)
  {
    dat2_count_++;
  }
  else
  {
    dat2_count_ = 0;
  }
  if (dat3_count_ < dat3_queue_.size() - 1)
  {
    dat3_count_++;
  }
  else
  {
    dat3_count_ = 0;
  }
  if (dat4_count_ < dat4_queue_.size() - 1)
  {
    dat4_count_++;
  }
  else
  {
    dat4_count_ = 0;
  }

  if (joint_broadcaster_toggle != 0)
  {
    joint_position_states_[0] = api_wrap_->fb_pack.jnt_EFB.j1;
    joint_position_states_[1] = api_wrap_->fb_pack.jnt_EFB.j2;
    joint_position_states_[2] = api_wrap_->fb_pack.jnt_EFB.j3;
    if(scara==1){
        joint_position_states_[2]/=1000.0; // SCARA robot type correction. mm to m.
    }
    joint_position_states_[3] = api_wrap_->fb_pack.jnt_EFB.j4;
    joint_position_states_[4] = api_wrap_->fb_pack.jnt_EFB.j5;
    joint_position_states_[5] = api_wrap_->fb_pack.jnt_EFB.j6;
    joint_position_states_[6] = api_wrap_->fb_pack.jnt_EFB.j7;
    joint_position_states_[7] = api_wrap_->fb_pack.jnt_EFB.j8;

    joint_velocity_states_[0] = api_wrap_->fb_pack.speed_fb.m1 / 100.0;
    joint_velocity_states_[1] = api_wrap_->fb_pack.speed_fb.m2 / 100.0;
    joint_velocity_states_[2] = api_wrap_->fb_pack.speed_fb.m3 / 100.0;
    joint_velocity_states_[3] = api_wrap_->fb_pack.speed_fb.m4 / 100.0;
    joint_velocity_states_[4] = api_wrap_->fb_pack.speed_fb.m5 / 100.0;
    joint_velocity_states_[5] = api_wrap_->fb_pack.speed_fb.m6 / 100.0;
    joint_velocity_states_[6] = api_wrap_->fb_pack.speed_fb.m7 / 100.0;
    joint_velocity_states_[7] = api_wrap_->fb_pack.speed_fb.m8 / 100.0;

    joint_ampere_rms_states_[0] = api_wrap_->fb_pack.curr_fb_arms.f1;
    joint_ampere_rms_states_[1] = api_wrap_->fb_pack.curr_fb_arms.f2;
    joint_ampere_rms_states_[2] = api_wrap_->fb_pack.curr_fb_arms.f3;
    joint_ampere_rms_states_[3] = api_wrap_->fb_pack.curr_fb_arms.f4;
    joint_ampere_rms_states_[4] = api_wrap_->fb_pack.curr_fb_arms.f5;
    joint_ampere_rms_states_[5] = api_wrap_->fb_pack.curr_fb_arms.f6;
    joint_ampere_rms_states_[6] = api_wrap_->fb_pack.curr_fb_arms.f7;
    joint_ampere_rms_states_[7] = api_wrap_->fb_pack.curr_fb_arms.f8;

    joint_encoder_misscount_states_[0] = api_wrap_->fb_pack.enc_mis.m1;
    joint_encoder_misscount_states_[1] = api_wrap_->fb_pack.enc_mis.m2;
    joint_encoder_misscount_states_[2] = api_wrap_->fb_pack.enc_mis.m3;
    joint_encoder_misscount_states_[3] = api_wrap_->fb_pack.enc_mis.m4;
    joint_encoder_misscount_states_[4] = api_wrap_->fb_pack.enc_mis.m5;
    joint_encoder_misscount_states_[5] = api_wrap_->fb_pack.enc_mis.m6;
    joint_encoder_misscount_states_[6] = api_wrap_->fb_pack.enc_mis.m7;
    joint_encoder_misscount_states_[7] = api_wrap_->fb_pack.enc_mis.m8;

    joint_encoder_temperature_states_[0] = api_wrap_->fb_pack.enc_temp.m1;
    joint_encoder_temperature_states_[1] = api_wrap_->fb_pack.enc_temp.m2;
    joint_encoder_temperature_states_[2] = api_wrap_->fb_pack.enc_temp.m3;
    joint_encoder_temperature_states_[3] = api_wrap_->fb_pack.enc_temp.m4;
    joint_encoder_temperature_states_[4] = api_wrap_->fb_pack.enc_temp.m5;
    joint_encoder_temperature_states_[5] = api_wrap_->fb_pack.enc_temp.m6;
    joint_encoder_temperature_states_[6] = api_wrap_->fb_pack.enc_temp.m7;
    joint_encoder_temperature_states_[7] = api_wrap_->fb_pack.enc_temp.m8;

    motor_voltage_states_[0] = api_wrap_->fb_pack.motor_v.m1;
    motor_voltage_states_[1] = api_wrap_->fb_pack.motor_v.m2;
    motor_voltage_states_[2] = api_wrap_->fb_pack.motor_v.m3;
    motor_voltage_states_[3] = api_wrap_->fb_pack.motor_v.m4;
    motor_voltage_states_[4] = api_wrap_->fb_pack.motor_v.m5;
    motor_voltage_states_[5] = api_wrap_->fb_pack.motor_v.m6;
    motor_voltage_states_[6] = api_wrap_->fb_pack.motor_v.m7;
    motor_voltage_states_[7] = api_wrap_->fb_pack.motor_v.m8;
  }

  // Reads joint position state from rtmon API feedback packet

  fts_curr_states_[0] = api_wrap_->fb_pack.fs_curr_pose.fx;
  fts_curr_states_[1] = api_wrap_->fb_pack.fs_curr_pose.fy;
  fts_curr_states_[2] = api_wrap_->fb_pack.fs_curr_pose.fz;
  fts_curr_states_[3] = api_wrap_->fb_pack.fs_curr_pose.mx;
  fts_curr_states_[4] = api_wrap_->fb_pack.fs_curr_pose.my;
  fts_curr_states_[5] = api_wrap_->fb_pack.fs_curr_pose.mz;

  fts_before_states_[0] = api_wrap_->fb_pack.fs_before.fx;
  fts_before_states_[1] = api_wrap_->fb_pack.fs_before.fy;
  fts_before_states_[2] = api_wrap_->fb_pack.fs_before.fz;
  fts_before_states_[3] = api_wrap_->fb_pack.fs_before.mx;
  fts_before_states_[4] = api_wrap_->fb_pack.fs_before.my;
  fts_before_states_[5] = api_wrap_->fb_pack.fs_before.mz;

  fts_after_states_[0] = api_wrap_->fb_pack.fs_after.fx;
  fts_after_states_[1] = api_wrap_->fb_pack.fs_after.fy;
  fts_after_states_[2] = api_wrap_->fb_pack.fs_after.fz;
  fts_after_states_[3] = api_wrap_->fb_pack.fs_after.mx;
  fts_after_states_[4] = api_wrap_->fb_pack.fs_after.my;
  fts_after_states_[5] = api_wrap_->fb_pack.fs_after.mz;

  tf2::Quaternion orientation;
  orientation.setRPY(api_wrap_->fb_pack.pos_EFB.a, api_wrap_->fb_pack.pos_EFB.b, api_wrap_->fb_pack.pos_EFB.c);
  tcp_pose_states_[0] = api_wrap_->fb_pack.pos_EFB.x  / 1000.0;
  tcp_pose_states_[1] = api_wrap_->fb_pack.pos_EFB.y  / 1000.0;
  tcp_pose_states_[2] = api_wrap_->fb_pack.pos_EFB.z  / 1000.0;
  tcp_pose_states_[3] = orientation.x();
  tcp_pose_states_[4] = orientation.y();
  tcp_pose_states_[5] = orientation.z();
  tcp_pose_states_[6] = orientation.w();

  // Function to read IO feedbacks for different IO interfaces
  auto readINFeedback = [&](std::vector<double>& io_states, std::unique_ptr<MelfaEthernet::rtmon>& api_wrap_) {
    io_states[0] = static_cast<double>(api_wrap_->fb_pack.INBitTop);
    io_states[2] = static_cast<double>(api_wrap_->fb_pack.INBitData);
  };

  auto readOUTFeedback = [&](std::vector<double>& io_states, std::unique_ptr<MelfaEthernet::rtmon>& api_wrap_) {
    io_states[1] = static_cast<double>(api_wrap_->fb_pack.OUTBitTop);
    io_states[3] = static_cast<double>(api_wrap_->fb_pack.OUTBitData);
  };

  // Binary IO control mode and Controller Type state update
  mode_io_state_[0] = mode_io_command_[0];

  // IO interface allocation based on IO bit top in the feedback
  if (((api_wrap_->fb_pack.INBitTop >= general1_io_limits_[0]) &&
       (api_wrap_->fb_pack.INBitTop <= general1_io_limits_[1])) &&
      ((api_wrap_->fb_pack.OUTBitTop >= general1_io_limits_[0]) &&
       (api_wrap_->fb_pack.OUTBitTop <= general1_io_limits_[1])))
  {
    readINFeedback(general1_io_states_, api_wrap_);
    readOUTFeedback(general1_io_states_, api_wrap_);
  }

  if (((api_wrap_->fb_pack.INBitTop >= general2_io_limits_[0]) &&
       (api_wrap_->fb_pack.INBitTop <= general2_io_limits_[1])) &&
      ((api_wrap_->fb_pack.OUTBitTop >= general2_io_limits_[0]) &&
       (api_wrap_->fb_pack.OUTBitTop <= general2_io_limits_[1])))
  {
    readINFeedback(general2_io_states_, api_wrap_);
    readOUTFeedback(general2_io_states_, api_wrap_);
  }

  if (((api_wrap_->fb_pack.INBitTop >= user1_io_limits_[0]) && (api_wrap_->fb_pack.INBitTop <= user1_io_limits_[1])) &&
      ((api_wrap_->fb_pack.OUTBitTop >= user1_io_limits_[0]) && (api_wrap_->fb_pack.OUTBitTop <= user1_io_limits_[1])))
  {
    readINFeedback(user1_io_states_, api_wrap_);
    readOUTFeedback(user1_io_states_, api_wrap_);
  }

  if (((api_wrap_->fb_pack.INBitTop >= user2_io_limits_[0]) && (api_wrap_->fb_pack.INBitTop <= user2_io_limits_[1])) &&
      ((api_wrap_->fb_pack.OUTBitTop >= user2_io_limits_[0]) && (api_wrap_->fb_pack.OUTBitTop <= user2_io_limits_[1])))
  {
    readINFeedback(user2_io_states_, api_wrap_);
    readOUTFeedback(user2_io_states_, api_wrap_);
  }

  if (((api_wrap_->fb_pack.INBitTop >= user3_io_limits_[0]) && (api_wrap_->fb_pack.INBitTop <= user3_io_limits_[1])) &&
      ((api_wrap_->fb_pack.OUTBitTop >= user3_io_limits_[0]) && (api_wrap_->fb_pack.OUTBitTop <= user3_io_limits_[1])))
  {
    readINFeedback(user3_io_states_, api_wrap_);
    readOUTFeedback(user3_io_states_, api_wrap_);
  }

  if (((api_wrap_->fb_pack.INBitTop >= user4_io_limits_[0]) && (api_wrap_->fb_pack.INBitTop <= user4_io_limits_[1])) &&
      ((api_wrap_->fb_pack.OUTBitTop >= user4_io_limits_[0]) && (api_wrap_->fb_pack.OUTBitTop <= user4_io_limits_[1])))
  {
    readINFeedback(user4_io_states_, api_wrap_);
    readOUTFeedback(user4_io_states_, api_wrap_);
  }

  if (((api_wrap_->fb_pack.INBitTop >= user5_io_limits_[0]) && (api_wrap_->fb_pack.INBitTop <= user5_io_limits_[1])) &&
      ((api_wrap_->fb_pack.OUTBitTop >= user5_io_limits_[0]) && (api_wrap_->fb_pack.OUTBitTop <= user5_io_limits_[1])))
  {
    readINFeedback(user5_io_states_, api_wrap_);
    readOUTFeedback(user5_io_states_, api_wrap_);
  }

  if (((api_wrap_->fb_pack.INBitTop >= user6_io_limits_[0]) && (api_wrap_->fb_pack.INBitTop <= user6_io_limits_[1])) &&
      ((api_wrap_->fb_pack.OUTBitTop >= user6_io_limits_[0]) && (api_wrap_->fb_pack.OUTBitTop <= user6_io_limits_[1])))
  {
    readINFeedback(user6_io_states_, api_wrap_);
    readOUTFeedback(user6_io_states_, api_wrap_);
  }

  if (((api_wrap_->fb_pack.INBitTop >= user7_io_limits_[0]) && (api_wrap_->fb_pack.INBitTop <= user7_io_limits_[1])) &&
      ((api_wrap_->fb_pack.OUTBitTop >= user7_io_limits_[0]) && (api_wrap_->fb_pack.OUTBitTop <= user7_io_limits_[1])))
  {
    readINFeedback(user7_io_states_, api_wrap_);
    readOUTFeedback(user7_io_states_, api_wrap_);
  }

  if (((api_wrap_->fb_pack.INBitTop >= user8_io_limits_[0]) && (api_wrap_->fb_pack.INBitTop <= user8_io_limits_[1])) &&
      ((api_wrap_->fb_pack.OUTBitTop >= user8_io_limits_[0]) && (api_wrap_->fb_pack.OUTBitTop <= user8_io_limits_[1])))
  {
    readINFeedback(user8_io_states_, api_wrap_);
    readOUTFeedback(user8_io_states_, api_wrap_);
  }

  // Resets State interfaces for disabled IO interface
  unsigned int control_mode_config_ = static_cast<unsigned int>(mode_io_state_[0]);

  auto resetIOFeedback = [&](std::vector<double>& io_states) { std::fill(io_states.begin(), io_states.end(), 0.0); };

  if ((control_mode_config_ & 0b0000000001) == 0)
    resetIOFeedback(general1_io_states_);
  if ((control_mode_config_ & 0b0000000010) == 0)
    resetIOFeedback(general1_io_states_);
  if ((control_mode_config_ & 0b0000000100) == 0)
    resetIOFeedback(user1_io_states_);
  if ((control_mode_config_ & 0b0000001000) == 0)
    resetIOFeedback(user2_io_states_);
  if ((control_mode_config_ & 0b0000010000) == 0)
    resetIOFeedback(user3_io_states_);
  if ((control_mode_config_ & 0b0000100000) == 0)
    resetIOFeedback(user4_io_states_);
  if ((control_mode_config_ & 0b0001000000) == 0)
    resetIOFeedback(user5_io_states_);
  if ((control_mode_config_ & 0b0010000000) == 0)
    resetIOFeedback(user6_io_states_);
  if ((control_mode_config_ & 0b0100000000) == 0)
    resetIOFeedback(user7_io_states_);
  if ((control_mode_config_ & 0b1000000000) == 0)
    resetIOFeedback(user8_io_states_);

  // For reading raw feedback from API.
  // api_wrap_->print_monitored_feedback();

  return hardware_interface::return_type::OK;
}

hardware_interface::return_type MELFAMonitorHardwareInterface::write(const rclcpp::Time& time,
                                                                     const rclcpp::Duration& period)
{
  /**
   * @brief Write method for MELFAMonitorHardwareInterface class
   *
   * This function writes joint commands and IO commands to MELFA Controller API
   *
   * @param time The time recorded at the beginning of the current iteration of the control loop
   * @param period The duration measured for the last iteration of the control loop.
   * @returns hardware_interface::return_type::OK after writing interfaces to API.
   *
   */

  *(api_wrap_->cmd_pack.mon_dat) = dat1_queue_[dat1_count_];      // set first feedback
  *(api_wrap_->cmd_pack.mon_dat + 1) = dat2_queue_[dat2_count_];  // set second feedback
  *(api_wrap_->cmd_pack.mon_dat + 2) = dat3_queue_[dat3_count_];  // set thrid feedback
  *(api_wrap_->cmd_pack.mon_dat + 3) = dat4_queue_[dat4_count_];  // set forth feedback

  // Interface selection based on Binary IO control mode
  if (execution_init_)
  {
    binary_config_ = static_cast<unsigned int>(mode_io_command_[0]);
    execution_init_ = false;
  }

  if ((binary_config_ & 0b0000000001) != 0)
  {
    setIO(general1_io_commands_);
    binary_config_ ^= 0b0000000001;
  }
  else if ((binary_config_ & 0b0000000010) != 0)
  {
    setIO(general2_io_commands_);
    binary_config_ ^= 0b0000000010;
  }
  else if ((binary_config_ & 0b0000000100) != 0)
  {
    setIO(user1_io_commands_);
    binary_config_ ^= 0b0000000100;
  }
  else if ((binary_config_ & 0b0000001000) != 0)
  {
    setIO(user2_io_commands_);
    binary_config_ ^= 0b0000001000;
  }
  else if ((binary_config_ & 0b0000010000) != 0)
  {
    setIO(user3_io_commands_);
    binary_config_ ^= 0b0000010000;
  }
  else if ((binary_config_ & 0b0000100000) != 0)
  {
    setIO(user4_io_commands_);
    binary_config_ ^= 0b0000100000;
  }
  else if ((binary_config_ & 0b0001000000) != 0)
  {
    setIO(user5_io_commands_);
    binary_config_ ^= 0b0001000000;
  }
  else if ((binary_config_ & 0b0010000000) != 0)
  {
    setIO(user6_io_commands_);
    binary_config_ ^= 0b0010000000;
  }
  else if ((binary_config_ & 0b0100000000) != 0)
  {
    setIO(user7_io_commands_);
    binary_config_ ^= 0b0100000000;
  }
  else if ((binary_config_ & 0b1000000000) != 0)
  {
    setIO(user8_io_commands_);
    binary_config_ ^= 0b1000000000;
  }

  if (binary_config_ == 0)
  {
    execution_init_ = true;
  }

  if (api_wrap_->robot_status)
  {
    if (api_wrap_->WriteToRobot_MON_() != 0)
    {
      if (!api_wrap_->robot_status)
      {
        RCLCPP_FATAL(rclcpp::get_logger("MELFAMonitorHardwareInterface"), "ERROR: Connection lost.");
        return hardware_interface::return_type::ERROR;
      }
      RCLCPP_WARN(rclcpp::get_logger("MELFAMonitorHardwareInterface"), "ERROR: Command Fail.");
      return hardware_interface::return_type::OK;
    }
    return hardware_interface::return_type::OK;
  }
  else
  {
    RCLCPP_FATAL(rclcpp::get_logger("MELFAMonitorHardwareInterface"), "ERROR: Connection lost.");
    return hardware_interface::return_type::ERROR;
  }
}
}  // namespace melfa_monitor

#include "pluginlib/class_list_macros.hpp"

PLUGINLIB_EXPORT_CLASS(melfa_monitor::MELFAMonitorHardwareInterface, hardware_interface::SystemInterface)