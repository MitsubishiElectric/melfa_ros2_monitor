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

#ifndef MELFA_MONITOR__HARDWARE_INTERFACE_HPP_
#define MELFA_MONITOR__HARDWARE_INTERFACE_HPP_

// Include headers for ROS 2 control hardware_interface
#include "hardware_interface/hardware_info.hpp"     // Declaration related to hardware information
#include "hardware_interface/system_interface.hpp"  // Declaration related to system interfaces
#include "hardware_interface/sensor_interface.hpp"
#include "hardware_interface/types/hardware_interface_type_values.hpp"  // Declaration related to type values of hardware interfaces

// Include headers for MELFA
#include "melfa_monitor/melfa_rt_mon.hpp"      // Declaration related to MELFA real-time exceptions
#include "melfa_monitor/rt_mon_def.hpp"        // Definition of MELFA real-time exceptions
#include "melfa_monitor/visibility_control.h"  // Visibility control for MELFA

// Include headers for ROS 2
#include "rclcpp/rclcpp.hpp"                                              // ROS 2 C++ client library
#include "rclcpp/macros.hpp"                                              // Macros for ROS 2 C++
#include "rclcpp_lifecycle/node_interfaces/lifecycle_node_interface.hpp"  // Lifecycle node interface for ROS 2
#include "rclcpp_lifecycle/state.hpp"                                     // State definitions for ROS 2 lifecycle nodes
#include "geometry_msgs/msg/transform_stamped.hpp"  // Message type for stamped transforms in ROS 2
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>

// Include headers for system libraries
#include <bitset>  // Provides the std::bitset class for managing bit fields
#include <sstream>

namespace melfa_monitor
{
enum MELFAGpioConstants
{
  io_fb_pack_size = 4,
  io_cmd_pack_size = 2,
  io_interface_state_size = 1,
  io_interface_command_size = 1,
  input_state_idx_ = 3,
  output_state_idx_ = 4

};

class MELFAMonitorHardwareInterface : public hardware_interface::SystemInterface
{
public:
  RCLCPP_SHARED_PTR_DEFINITIONS(MELFAMonitorHardwareInterface)

  MELFA_HARDWARE_PUBLIC
  hardware_interface::CallbackReturn on_init(const hardware_interface::HardwareInfo& system_info) override;

  MELFA_HARDWARE_PUBLIC
  std::vector<hardware_interface::StateInterface> export_state_interfaces() override;

  MELFA_HARDWARE_PUBLIC
  std::vector<hardware_interface::CommandInterface> export_command_interfaces() override;

  MELFA_HARDWARE_PUBLIC
  hardware_interface::CallbackReturn on_activate(const rclcpp_lifecycle::State& previous_state) override;

  MELFA_HARDWARE_PUBLIC
  hardware_interface::CallbackReturn on_deactivate(const rclcpp_lifecycle::State& previous_state) override;

  MELFA_HARDWARE_PUBLIC
  hardware_interface::return_type read(const rclcpp::Time& time, const rclcpp::Duration& period) final;

  MELFA_HARDWARE_PUBLIC
  hardware_interface::return_type write(const rclcpp::Time& time, const rclcpp::Duration& period) final;

  std::vector<uint16_t> readDataQueue(const std::string& input);

  void setIO(const std::vector<double>& io_commands);

private:
  // communication API
  std::unique_ptr<MelfaEthernet::rtmon> api_wrap_;
  uint8_t packet_lost_log;
  uint8_t scara;
  uint8_t joint_broadcaster_toggle;

  bool execution_init_;
  unsigned int binary_config_;

  // State Interfaces
  std::vector<double> joint_position_states_;
  std::vector<double> joint_velocity_states_;
  std::vector<double> joint_ampere_rms_states_;
  std::vector<double> joint_current_rate_states_;
  std::vector<double> joint_encoder_misscount_states_;
  std::vector<double> joint_encoder_temperature_states_;
  std::vector<double> motor_voltage_states_;
  std::vector<double> fts_curr_states_;
  std::vector<double> fts_before_states_;
  std::vector<double> fts_after_states_;
  std::vector<double> tcp_pose_states_;

  // General 1 Interfaces
  std::string general1_io_name = "general1_io";
  std::vector<double> general1_io_commands_;
  std::vector<double> general1_io_states_;

  // General 2 Interfaces
  std::string general2_io_name = "general2_io";
  std::vector<double> general2_io_commands_;
  std::vector<double> general2_io_states_;

  //  User1 Interfaces
  std::string user1_io_name = "user1_io";
  std::vector<double> user1_io_states_;
  std::vector<double> user1_io_commands_;

  //  User2 Interfaces
  std::string user2_io_name = "user2_io";
  std::vector<double> user2_io_states_;
  std::vector<double> user2_io_commands_;

  //  User3 Interfaces
  std::string user3_io_name = "user3_io";
  std::vector<double> user3_io_states_;
  std::vector<double> user3_io_commands_;

  //  User4 Interfaces
  std::string user4_io_name = "user4_io";
  std::vector<double> user4_io_states_;
  std::vector<double> user4_io_commands_;

  //  User5 Interfaces
  std::string user5_io_name = "user5_io";
  std::vector<double> user5_io_states_;
  std::vector<double> user5_io_commands_;

  //  User6 Interfaces
  std::string user6_io_name = "user6_io";
  std::vector<double> user6_io_states_;
  std::vector<double> user6_io_commands_;

  //  User7 Interfaces
  std::string user7_io_name = "user7_io";
  std::vector<double> user7_io_states_;
  std::vector<double> user7_io_commands_;

  //  User8 Interfaces
  std::string user8_io_name = "user8_io";
  std::vector<double> user8_io_states_;
  std::vector<double> user8_io_commands_;

  // Binary IO Control Mode Interfaces
  std::string io_control_mode_name = "io_control_mode";
  std::vector<double> mode_io_command_;
  std::vector<double> mode_io_state_;

  // Melfa Controller Type Interfaces
  std::string ctrl_name = "ctrl";
  std::vector<double> ctrl_type_io_command_;
  std::vector<double> ctrl_type_io_state_;

  std::string io_control_mode_;
  std::string controller_type_;

  // IO Interface Limits
  std::vector<uint16_t> general1_io_limits_;
  std::vector<uint16_t> general2_io_limits_;
  std::vector<uint16_t> user1_io_limits_;
  std::vector<uint16_t> user2_io_limits_;
  std::vector<uint16_t> user3_io_limits_;
  std::vector<uint16_t> user4_io_limits_;
  std::vector<uint16_t> user5_io_limits_;
  std::vector<uint16_t> user6_io_limits_;
  std::vector<uint16_t> user7_io_limits_;
  std::vector<uint16_t> user8_io_limits_;

  // data queues
  std::vector<uint16_t> dat1_queue_;
  std::vector<uint16_t> dat2_queue_;
  std::vector<uint16_t> dat3_queue_;
  std::vector<uint16_t> dat4_queue_;
  uint16_t dat1_count_;
  uint16_t dat2_count_;
  uint16_t dat3_count_;
  uint16_t dat4_count_;
};

}  // namespace melfa_monitor
#endif  // MELFA_MONITOR__HARDWARE_INTERFACE_HPP_