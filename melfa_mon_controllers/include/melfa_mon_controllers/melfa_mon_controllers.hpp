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

#ifndef MELFA_MON__CONTROLLER_HPP_
#define MELFA_MON__CONTROLLER_HPP_

// Include headers for MELFA and ros2_control interfaces
#include "controller_interface/controller_interface.hpp"
#include "melfa_mon_controllers/visibility_control.h"

// Melfa message service interfaces
#include "melfa_mon_msgs/srv/gpio_configure.hpp"
#include "melfa_mon_msgs/srv/mode_configure.hpp"

#include "melfa_mon_msgs/msg/gpio_state.hpp"
#include "melfa_mon_msgs/msg/gpio_command.hpp"
#include "melfa_mon_msgs/msg/robot_info.hpp"
#include "melfa_mon_msgs/msg/control_mode.hpp"
#include "melfa_mon_msgs/msg/controller_type.hpp"
#include "std_msgs/msg/string.hpp"

// Include headers for system libraries
#include "yaml-cpp/yaml.h"
#include "ament_index_cpp/get_package_share_directory.hpp"
#include <bitset>

namespace melfa_mon_controllers
{

enum GpioIdentifier
{
  general1_io_state = 0,
  general2_io_state = 4,
  user1_io_state = 8,
  user2_io_state = 12,
  user3_io_state = 16,
  user4_io_state = 20,
  user5_io_state = 24,
  user6_io_state = 28,
  user7_io_state = 32,
  user8_io_state = 36,
  control_mode_io_state = 40,
  ctrl_type_state = 41,
  general1_io_command = 0,
  general2_io_command = 2,
  user1_io_command = 4,
  user2_io_command = 6,
  user3_io_command = 8,
  user4_io_command = 10,
  user5_io_command = 12,
  user6_io_command = 14,
  user7_io_command = 16,
  user8_io_command = 18,
  control_mode_io_command = 20,
  ctrl_type_command = 21
};

class MelfaMonController : public controller_interface::ControllerInterface
{
public:
  RCLCPP_SHARED_PTR_DEFINITIONS(MelfaMonController)

  MELFA_MON_CONTROLLERS_PUBLIC
  controller_interface::InterfaceConfiguration command_interface_configuration() const override;

  MELFA_MON_CONTROLLERS_PUBLIC
  controller_interface::InterfaceConfiguration state_interface_configuration() const override;

  MELFA_MON_CONTROLLERS_PUBLIC
  controller_interface::return_type update(const rclcpp::Time& time, const rclcpp::Duration& period) override;

  MELFA_MON_CONTROLLERS_PUBLIC
  CallbackReturn on_configure(const rclcpp_lifecycle::State& previous_state) override;

  MELFA_MON_CONTROLLERS_PUBLIC
  CallbackReturn on_activate(const rclcpp_lifecycle::State& previous_state) override;

  MELFA_MON_CONTROLLERS_PUBLIC
  CallbackReturn on_deactivate(const rclcpp_lifecycle::State& previous_state) override;

  MELFA_MON_CONTROLLERS_PUBLIC
  CallbackReturn on_init() override;

private:
  std::vector<std::string> general1_io_state_interfaces_;
  std::vector<std::string> general2_io_state_interfaces_;
  std::vector<std::string> user1_io_state_interfaces_;
  std::vector<std::string> user2_io_state_interfaces_;
  std::vector<std::string> user3_io_state_interfaces_;
  std::vector<std::string> user4_io_state_interfaces_;
  std::vector<std::string> user5_io_state_interfaces_;
  std::vector<std::string> user6_io_state_interfaces_;
  std::vector<std::string> user7_io_state_interfaces_;
  std::vector<std::string> user8_io_state_interfaces_;
  std::vector<std::string> misc_io_state_interfaces_;

  std::vector<std::string> general1_io_command_interfaces_;
  std::vector<std::string> general2_io_command_interfaces_;
  std::vector<std::string> user2_io_command_interfaces_;
  std::vector<std::string> user1_io_command_interfaces_;
  std::vector<std::string> user3_io_command_interfaces_;
  std::vector<std::string> user4_io_command_interfaces_;
  std::vector<std::string> user5_io_command_interfaces_;
  std::vector<std::string> user6_io_command_interfaces_;
  std::vector<std::string> user7_io_command_interfaces_;
  std::vector<std::string> user8_io_command_interfaces_;
  std::vector<std::string> misc_io_command_interfaces_;

  YAML::Node io_limits_;

  bool configGpio(melfa_mon_msgs::srv::GpioConfigure::Request::SharedPtr req,
                  melfa_mon_msgs::srv::GpioConfigure::Response::SharedPtr res);

  bool configControlMode(melfa_mon_msgs::srv::ModeConfigure::Request::SharedPtr req,
                         melfa_mon_msgs::srv::ModeConfigure::Response::SharedPtr res);

  void commandGPIOCallback(const melfa_mon_msgs::msg::GpioCommand::SharedPtr gpio_msg);

protected:
  // GPIO command message
  std::shared_ptr<melfa_mon_msgs::msg::GpioCommand> io_cmd_;

  // IO interface state publishers
  std::shared_ptr<rclcpp::Publisher<melfa_mon_msgs::msg::GpioState>> general1_io_state_publisher_;
  std::shared_ptr<rclcpp::Publisher<melfa_mon_msgs::msg::GpioState>> general2_io_state_publisher_;
  std::shared_ptr<rclcpp::Publisher<melfa_mon_msgs::msg::GpioState>> user1_io_state_publisher_;
  std::shared_ptr<rclcpp::Publisher<melfa_mon_msgs::msg::GpioState>> user2_io_state_publisher_;
  std::shared_ptr<rclcpp::Publisher<melfa_mon_msgs::msg::GpioState>> user3_io_state_publisher_;
  std::shared_ptr<rclcpp::Publisher<melfa_mon_msgs::msg::GpioState>> user4_io_state_publisher_;
  std::shared_ptr<rclcpp::Publisher<melfa_mon_msgs::msg::GpioState>> user5_io_state_publisher_;
  std::shared_ptr<rclcpp::Publisher<melfa_mon_msgs::msg::GpioState>> user6_io_state_publisher_;
  std::shared_ptr<rclcpp::Publisher<melfa_mon_msgs::msg::GpioState>> user7_io_state_publisher_;
  std::shared_ptr<rclcpp::Publisher<melfa_mon_msgs::msg::GpioState>> user8_io_state_publisher_;

  // Melfa Controller type publisher
  std::shared_ptr<rclcpp::Publisher<melfa_mon_msgs::msg::ControllerType>> controller_type_publisher_;

  // Binary IO control mode publisher
  std::shared_ptr<rclcpp::Publisher<melfa_mon_msgs::msg::ControlMode>> control_mode_publisher_;

  // IO interface state messages
  melfa_mon_msgs::msg::GpioState general1_gpio_msg_;
  melfa_mon_msgs::msg::GpioState general2_gpio_msg_;
  melfa_mon_msgs::msg::GpioState user1_gpio_msg_;
  melfa_mon_msgs::msg::GpioState user2_gpio_msg_;
  melfa_mon_msgs::msg::GpioState user3_gpio_msg_;
  melfa_mon_msgs::msg::GpioState user4_gpio_msg_;
  melfa_mon_msgs::msg::GpioState user5_gpio_msg_;
  melfa_mon_msgs::msg::GpioState user6_gpio_msg_;
  melfa_mon_msgs::msg::GpioState user7_gpio_msg_;
  melfa_mon_msgs::msg::GpioState user8_gpio_msg_;

  // Melfa Controller type message
  melfa_mon_msgs::msg::ControllerType ctrl_type_msg;

  // Melfa Binary IO control mode message
  melfa_mon_msgs::msg::ControlMode io_control_mode;

  // GPIO command subscriber
  rclcpp::Subscription<melfa_mon_msgs::msg::GpioCommand>::SharedPtr io_mon_command_subscriber_;

  // GPIO configuration service
  rclcpp::Service<melfa_mon_msgs::srv::GpioConfigure>::SharedPtr configure_io_mon_srv_;

  // GPIO binary control mode service
  rclcpp::Service<melfa_mon_msgs::srv::ModeConfigure>::SharedPtr configure_mon_mode_srv_;

  std::vector<long int> ctrl_limits;
  unsigned int control_mode_config_;
};
}  // namespace melfa_mon_controllers

#endif  // MELFA_MON__CONTROLLER_HPP_