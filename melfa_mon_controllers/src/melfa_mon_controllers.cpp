//  COPYRIGHT (C) 2024 Mitsubishi Electric Corporation

//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at

//      http://www.apache.org/licenses/LICENSE-2.0

//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

#include "melfa_mon_controllers/melfa_mon_controllers.hpp"

namespace melfa_mon_controllers
{
controller_interface::CallbackReturn MelfaMonController::on_init()
{
  /**
   * @brief on_init method for MelfaMonController class
   *
   * This method initializes the limits of the IO interfaces by reading from the IO limits YAML config file
   *
   * @returns CallbackReturn::SUCCESS if IO limits components are loading in YAML node
   * @returns CallbackReturn::ERROR if exception is raised
   *
   */

  try
  {
    const std::string package_name = "melfa_mon_controllers";
    std::string config_file = "io_limits.yaml";
    std::string yamlFilePath = ament_index_cpp::get_package_share_directory(package_name) + "/config/" + config_file;

    // Load the YAML file
    io_limits_ = YAML::LoadFile(yamlFilePath);
  }
  catch (const std::exception& e)
  {
    fprintf(stderr, "Exception thrown during init stage with message: %s \n", e.what());
    return controller_interface::CallbackReturn::ERROR;
  }

  return controller_interface::CallbackReturn::SUCCESS;
}

controller_interface::InterfaceConfiguration MelfaMonController::command_interface_configuration() const
{
  /**
   * @brief configuration method for MelfaMonController's command interface
   *
   * This method configures command interfaces for Melfa GPIO controller
   *
   * @return An InterfaceConfiguration object representing the configuration of the command interface
   *
   */
  controller_interface::InterfaceConfiguration config;
  config.type = controller_interface::interface_configuration_type::INDIVIDUAL;

  config.names.insert(config.names.end(), general1_io_command_interfaces_.begin(),
                      general1_io_command_interfaces_.end());
  config.names.insert(config.names.end(), general2_io_command_interfaces_.begin(),
                      general2_io_command_interfaces_.end());
  config.names.insert(config.names.end(), user1_io_command_interfaces_.begin(), user1_io_command_interfaces_.end());
  config.names.insert(config.names.end(), user2_io_command_interfaces_.begin(), user2_io_command_interfaces_.end());
  config.names.insert(config.names.end(), user3_io_command_interfaces_.begin(), user3_io_command_interfaces_.end());
  config.names.insert(config.names.end(), user4_io_command_interfaces_.begin(), user4_io_command_interfaces_.end());
  config.names.insert(config.names.end(), user5_io_command_interfaces_.begin(), user5_io_command_interfaces_.end());
  config.names.insert(config.names.end(), user6_io_command_interfaces_.begin(), user6_io_command_interfaces_.end());
  config.names.insert(config.names.end(), user7_io_command_interfaces_.begin(), user7_io_command_interfaces_.end());
  config.names.insert(config.names.end(), user8_io_command_interfaces_.begin(), user8_io_command_interfaces_.end());
  config.names.insert(config.names.end(), misc_io_command_interfaces_.begin(), misc_io_command_interfaces_.end());

  return config;
}

controller_interface::InterfaceConfiguration MelfaMonController::state_interface_configuration() const
{
  /**
   * @brief configuration method for MelfaMonController's state interface
   *
   * This method configures state interfaces for Melfa GPIO controller
   *
   * @return An InterfaceConfiguration object representing the configuration of the state interface
   *
   */
  controller_interface::InterfaceConfiguration config;
  config.type = controller_interface::interface_configuration_type::INDIVIDUAL;

  config.names.insert(config.names.end(), general1_io_state_interfaces_.begin(), general1_io_state_interfaces_.end());
  config.names.insert(config.names.end(), general2_io_state_interfaces_.begin(), general2_io_state_interfaces_.end());
  config.names.insert(config.names.end(), user1_io_state_interfaces_.begin(), user1_io_state_interfaces_.end());
  config.names.insert(config.names.end(), user2_io_state_interfaces_.begin(), user2_io_state_interfaces_.end());
  config.names.insert(config.names.end(), user3_io_state_interfaces_.begin(), user3_io_state_interfaces_.end());
  config.names.insert(config.names.end(), user4_io_state_interfaces_.begin(), user4_io_state_interfaces_.end());
  config.names.insert(config.names.end(), user5_io_state_interfaces_.begin(), user5_io_state_interfaces_.end());
  config.names.insert(config.names.end(), user6_io_state_interfaces_.begin(), user6_io_state_interfaces_.end());
  config.names.insert(config.names.end(), user7_io_state_interfaces_.begin(), user7_io_state_interfaces_.end());
  config.names.insert(config.names.end(), user8_io_state_interfaces_.begin(), user8_io_state_interfaces_.end());
  config.names.insert(config.names.end(), misc_io_state_interfaces_.begin(), misc_io_state_interfaces_.end());

  return config;
}

controller_interface::return_type MelfaMonController::update(const rclcpp::Time& /*time*/,
                                                             const rclcpp::Duration& /*period*/)
{
  /**
   * @brief Update method for MelfaMonController class
   *
   * This method periodically updates IO state information, binary IO control mode and Controller Type
   *
   * @param time The time recorded at the beginning of the current iteration of the update loop
   * @param period The duration measured for the last iteration of the update loop
   * @returns Controller_interface::return_type::OK after update of the states
   * @note updateGPIO function facilitates updating different IO interfaces current state
   *
   */

  auto updateGPIO = [&](melfa_mon_msgs::msg::GpioState& gpio_msg, const char* io_interface_name,
                        GpioIdentifier identifier,
                        rclcpp::Publisher<melfa_mon_msgs::msg::GpioState>::SharedPtr& gpio_msg_publisher_) {
    gpio_msg.interface_name = io_interface_name;
    gpio_msg.input_addrr = static_cast<uint16_t>(state_interfaces_.at(identifier).get_value());
    gpio_msg.output_addrr = static_cast<uint16_t>(state_interfaces_.at(identifier + 1).get_value());
    gpio_msg.input_data = static_cast<uint32_t>(state_interfaces_.at(identifier + 2).get_value());
    gpio_msg.output_data = static_cast<uint32_t>(state_interfaces_.at(identifier + 3).get_value());
    gpio_msg_publisher_->publish(gpio_msg);
  };

  // Publishes updated IO State Interfaces
  updateGPIO(general1_gpio_msg_, "General1 IO State", GpioIdentifier::general1_io_state, general1_io_state_publisher_);
  updateGPIO(general2_gpio_msg_, "General2 IO State", GpioIdentifier::general2_io_state, general2_io_state_publisher_);
  updateGPIO(user1_gpio_msg_, "User1 IO State", GpioIdentifier::user1_io_state, user1_io_state_publisher_);
  updateGPIO(user2_gpio_msg_, "User2 IO State", GpioIdentifier::user2_io_state, user2_io_state_publisher_);
  updateGPIO(user3_gpio_msg_, "User3 IO State", GpioIdentifier::user3_io_state, user3_io_state_publisher_);
  updateGPIO(user4_gpio_msg_, "User4 IO State", GpioIdentifier::user4_io_state, user4_io_state_publisher_);
  updateGPIO(user5_gpio_msg_, "User5 IO State", GpioIdentifier::user5_io_state, user5_io_state_publisher_);
  updateGPIO(user6_gpio_msg_, "User6 IO State", GpioIdentifier::user6_io_state, user6_io_state_publisher_);
  updateGPIO(user7_gpio_msg_, "User7 IO State", GpioIdentifier::user7_io_state, user7_io_state_publisher_);
  updateGPIO(user8_gpio_msg_, "User8 IO State", GpioIdentifier::user8_io_state, user8_io_state_publisher_);

  // Publishes Binary IO Control Mode
  control_mode_config_ =
      std::bitset<10>(state_interfaces_.at(GpioIdentifier::control_mode_io_state).get_value()).to_ulong();

  io_control_mode.general1_io_interface = (control_mode_config_ & 0b0000000001) != 0;
  io_control_mode.general2_io_interface = (control_mode_config_ & 0b0000000010) != 0;
  io_control_mode.user1_io_interface = (control_mode_config_ & 0b0000000100) != 0;
  io_control_mode.user2_io_interface = (control_mode_config_ & 0b0000001000) != 0;
  io_control_mode.user3_io_interface = (control_mode_config_ & 0b0000010000) != 0;
  io_control_mode.user4_io_interface = (control_mode_config_ & 0b0000100000) != 0;
  io_control_mode.user5_io_interface = (control_mode_config_ & 0b0001000000) != 0;
  io_control_mode.user6_io_interface = (control_mode_config_ & 0b0010000000) != 0;
  io_control_mode.user7_io_interface = (control_mode_config_ & 0b0100000000) != 0;
  io_control_mode.user8_io_interface = (control_mode_config_ & 0b1000000000) != 0;

  control_mode_publisher_->publish(io_control_mode);

  // Resets Command interfaces for disabled IO interface
  auto resetIOCommandInterface = [&](GpioIdentifier identifier, bool skipReset) {
    if (!skipReset)
    {
      for (int i = 0; i < 2; ++i)
        command_interfaces_[identifier + i].set_value(0.0);
    }
  };

  resetIOCommandInterface(GpioIdentifier::general1_io_command, control_mode_config_ & 0b0000000001);
  resetIOCommandInterface(GpioIdentifier::general2_io_command, control_mode_config_ & 0b0000000010);
  resetIOCommandInterface(GpioIdentifier::user1_io_command, control_mode_config_ & 0b0000000100);
  resetIOCommandInterface(GpioIdentifier::user2_io_command, control_mode_config_ & 0b0000001000);
  resetIOCommandInterface(GpioIdentifier::user3_io_command, control_mode_config_ & 0b0000010000);
  resetIOCommandInterface(GpioIdentifier::user4_io_command, control_mode_config_ & 0b0000100000);
  resetIOCommandInterface(GpioIdentifier::user5_io_command, control_mode_config_ & 0b0001000000);
  resetIOCommandInterface(GpioIdentifier::user6_io_command, control_mode_config_ & 0b0010000000);
  resetIOCommandInterface(GpioIdentifier::user7_io_command, control_mode_config_ & 0b0100000000);
  resetIOCommandInterface(GpioIdentifier::user8_io_command, control_mode_config_ & 0b1000000000);

  ctrl_type_msg.controller_type =
      (state_interfaces_.at(GpioIdentifier::ctrl_type_state).get_value() == 1.0) ?
          "R" :
          ((state_interfaces_.at(GpioIdentifier::ctrl_type_state).get_value() == 2.0) ? "Q" : "D");

  controller_type_publisher_->publish(ctrl_type_msg);

  return controller_interface::return_type::OK;
}

controller_interface::CallbackReturn MelfaMonController::on_configure(const rclcpp_lifecycle::State& /*previous_state*/)
{
  /**
   * @brief configuration method for MelfaMonController class
   *
   * This method retrieves IO state and command interfaces, creates publishers for IO interfaces state,
   * current binary IO control mode, controller type and services for configuring IO interfaces and IO control mode
   *
   * @param previous_state lifecycle state object representing state before current state
   * @returns CallbackReturn::SUCCESS if interfaces are retrieved, publishers and services are created
   * @returns CallbackReturn::ERROR if exception is raised
   */
  try
  {
    general1_io_state_interfaces_ = get_node()->get_parameter("general1_io_state_interfaces").as_string_array();
    general2_io_state_interfaces_ = get_node()->get_parameter("general2_io_state_interfaces").as_string_array();
    user1_io_state_interfaces_ = get_node()->get_parameter("user1_io_state_interfaces").as_string_array();
    user2_io_state_interfaces_ = get_node()->get_parameter("user2_io_state_interfaces").as_string_array();
    user3_io_state_interfaces_ = get_node()->get_parameter("user3_io_state_interfaces").as_string_array();
    user4_io_state_interfaces_ = get_node()->get_parameter("user4_io_state_interfaces").as_string_array();
    user5_io_state_interfaces_ = get_node()->get_parameter("user5_io_state_interfaces").as_string_array();
    user6_io_state_interfaces_ = get_node()->get_parameter("user6_io_state_interfaces").as_string_array();
    user7_io_state_interfaces_ = get_node()->get_parameter("user7_io_state_interfaces").as_string_array();
    user8_io_state_interfaces_ = get_node()->get_parameter("user8_io_state_interfaces").as_string_array();
    misc_io_state_interfaces_ = get_node()->get_parameter("misc_io_state_interfaces").as_string_array();

    general1_io_command_interfaces_ = get_node()->get_parameter("general1_io_command_interfaces").as_string_array();
    general2_io_command_interfaces_ = get_node()->get_parameter("general2_io_command_interfaces").as_string_array();
    user1_io_command_interfaces_ = get_node()->get_parameter("user1_io_command_interfaces").as_string_array();
    user2_io_command_interfaces_ = get_node()->get_parameter("user2_io_command_interfaces").as_string_array();
    user3_io_command_interfaces_ = get_node()->get_parameter("user3_io_command_interfaces").as_string_array();
    user4_io_command_interfaces_ = get_node()->get_parameter("user4_io_command_interfaces").as_string_array();
    user5_io_command_interfaces_ = get_node()->get_parameter("user5_io_command_interfaces").as_string_array();
    user6_io_command_interfaces_ = get_node()->get_parameter("user6_io_command_interfaces").as_string_array();
    user7_io_command_interfaces_ = get_node()->get_parameter("user7_io_command_interfaces").as_string_array();
    user8_io_command_interfaces_ = get_node()->get_parameter("user8_io_command_interfaces").as_string_array();
    misc_io_command_interfaces_ = get_node()->get_parameter("misc_io_command_interfaces").as_string_array();

    general1_io_state_publisher_ = get_node()->create_publisher<melfa_mon_msgs::msg::GpioState>(
        "~/general1_io_state", rclcpp::SystemDefaultsQoS());

    general2_io_state_publisher_ = get_node()->create_publisher<melfa_mon_msgs::msg::GpioState>(
        "~/general2_io_state", rclcpp::SystemDefaultsQoS());

    user1_io_state_publisher_ =
        get_node()->create_publisher<melfa_mon_msgs::msg::GpioState>("~/user1_io_state", rclcpp::SystemDefaultsQoS());

    user2_io_state_publisher_ =
        get_node()->create_publisher<melfa_mon_msgs::msg::GpioState>("~/user2_io_state", rclcpp::SystemDefaultsQoS());
    user3_io_state_publisher_ =
        get_node()->create_publisher<melfa_mon_msgs::msg::GpioState>("~/user3_io_state", rclcpp::SystemDefaultsQoS());
    user4_io_state_publisher_ =
        get_node()->create_publisher<melfa_mon_msgs::msg::GpioState>("~/user4_io_state", rclcpp::SystemDefaultsQoS());
    user5_io_state_publisher_ =
        get_node()->create_publisher<melfa_mon_msgs::msg::GpioState>("~/user5_io_state", rclcpp::SystemDefaultsQoS());
    user6_io_state_publisher_ =
        get_node()->create_publisher<melfa_mon_msgs::msg::GpioState>("~/user6_io_state", rclcpp::SystemDefaultsQoS());
    user7_io_state_publisher_ =
        get_node()->create_publisher<melfa_mon_msgs::msg::GpioState>("~/user7_io_state", rclcpp::SystemDefaultsQoS());
    user8_io_state_publisher_ =
        get_node()->create_publisher<melfa_mon_msgs::msg::GpioState>("~/user8_io_state", rclcpp::SystemDefaultsQoS());
    controller_type_publisher_ = get_node()->create_publisher<melfa_mon_msgs::msg::ControllerType>(
        "~/controller_type", rclcpp::SystemDefaultsQoS());
    control_mode_publisher_ = get_node()->create_publisher<melfa_mon_msgs::msg::ControlMode>(
        "~/io_monitor_mode", rclcpp::SystemDefaultsQoS());

    io_mon_command_subscriber_ = get_node()->create_subscription<melfa_mon_msgs::msg::GpioCommand>(
        "~/io_monitor_address_command", rclcpp::SystemDefaultsQoS(),
        [this](const melfa_mon_msgs::msg::GpioCommand::SharedPtr gpio_msg) { commandGPIOCallback(gpio_msg); });

    configure_io_mon_srv_ = get_node()->create_service<melfa_mon_msgs::srv::GpioConfigure>(
        "~/configure_io_monitor_address",
        std::bind(&MelfaMonController::configGpio, this, std::placeholders::_1, std::placeholders::_2));

    configure_mon_mode_srv_ = get_node()->create_service<melfa_mon_msgs::srv::ModeConfigure>(
        "~/configure_io_monitor_mode",
        std::bind(&MelfaMonController::configControlMode, this, std::placeholders::_1, std::placeholders::_2));
  }
  catch (...)
  {
    return LifecycleNodeInterface::CallbackReturn::ERROR;
  }
  return LifecycleNodeInterface::CallbackReturn::SUCCESS;
}

bool MelfaMonController::configGpio(melfa_mon_msgs::srv::GpioConfigure::Request::SharedPtr req,
                                    melfa_mon_msgs::srv::GpioConfigure::Response::SharedPtr res)
{
  /**
   * @brief GPIO configuration method for MelfaMonController class
   *
   * This method binds to configure the GPIO service.
   *
   * @param req The request containing IO interface configuration parameters
   * @param res The response with the result of the configuration service
   * @returns res->success as true if configured else false
   */
  auto processIO = [&](auto& command_interfaces_, melfa_mon_msgs::srv::GpioConfigure::Request::SharedPtr req,
                       GpioIdentifier identifier) {
    res->success = true;

    command_interfaces_[identifier].set_value(static_cast<double>(req->input_addrr));
    command_interfaces_[identifier + 1].set_value(static_cast<double>(req->output_addrr));
  };

  if ((req->input_addrr >= ctrl_limits[0] && req->input_addrr <= ctrl_limits[1]) &&
      (req->output_addrr >= ctrl_limits[0] && req->output_addrr <= ctrl_limits[1]))
  {
    processIO(command_interfaces_, req, GpioIdentifier::general1_io_command);
  }
  else if ((req->input_addrr >= ctrl_limits[2] && req->input_addrr <= ctrl_limits[3]) &&
           (req->output_addrr >= ctrl_limits[2] && req->output_addrr <= ctrl_limits[3]))
  {
    processIO(command_interfaces_, req, GpioIdentifier::general2_io_command);
  }
  else if ((req->input_addrr >= ctrl_limits[4] && req->input_addrr <= ctrl_limits[5]) &&
           (req->output_addrr >= ctrl_limits[4] && req->output_addrr <= ctrl_limits[5]))
  {
    processIO(command_interfaces_, req, GpioIdentifier::user1_io_command);
  }
  else if ((req->input_addrr >= ctrl_limits[6] && req->input_addrr <= ctrl_limits[7]) &&
           (req->output_addrr >= ctrl_limits[6] && req->output_addrr <= ctrl_limits[7]))
  {
    processIO(command_interfaces_, req, GpioIdentifier::user2_io_command);
  }
  else if ((req->input_addrr >= ctrl_limits[8] && req->input_addrr <= ctrl_limits[9]) &&
           (req->output_addrr >= ctrl_limits[8] && req->output_addrr <= ctrl_limits[9]))
  {
    processIO(command_interfaces_, req, GpioIdentifier::user3_io_command);
  }
  else if ((req->input_addrr >= ctrl_limits[10] && req->input_addrr <= ctrl_limits[11]) &&
           (req->output_addrr >= ctrl_limits[10] && req->output_addrr <= ctrl_limits[11]))
  {
    processIO(command_interfaces_, req, GpioIdentifier::user4_io_command);
  }
  else if ((req->input_addrr >= ctrl_limits[12] && req->input_addrr <= ctrl_limits[13]) &&
           (req->output_addrr >= ctrl_limits[12] && req->output_addrr <= ctrl_limits[13]))
  {
    processIO(command_interfaces_, req, GpioIdentifier::user5_io_command);
  }
  else if ((req->input_addrr >= ctrl_limits[14] && req->input_addrr <= ctrl_limits[15]) &&
           (req->output_addrr >= ctrl_limits[14] && req->output_addrr <= ctrl_limits[15]))
  {
    processIO(command_interfaces_, req, GpioIdentifier::user6_io_command);
  }
  else if ((req->input_addrr >= ctrl_limits[16] && req->input_addrr <= ctrl_limits[17]) &&
           (req->output_addrr >= ctrl_limits[16] && req->output_addrr <= ctrl_limits[17]))
  {
    processIO(command_interfaces_, req, GpioIdentifier::user7_io_command);
  }
  else if ((req->input_addrr >= ctrl_limits[18] && req->input_addrr <= ctrl_limits[19]) &&
           (req->output_addrr >= ctrl_limits[18] && req->output_addrr <= ctrl_limits[19]))
  {
    processIO(command_interfaces_, req, GpioIdentifier::user8_io_command);
  }
  else
  {
    RCLCPP_WARN(get_node()->get_logger(),
                "configure_gpio service: INVALID ADDRESS (%d) (%d)"
                "Please select memory addresses that are within acceptable range of the SAME interface",
                req->input_addrr, req->output_addrr);
  }

  return res->success;
}

void MelfaMonController::commandGPIOCallback(const melfa_mon_msgs::msg::GpioCommand::SharedPtr io_cmd_)
{
  /**
   * @brief GPIO command callback method for MelfaMonController class
   *
   * This method binds to gpio_command_subscriber_
   *
   * @param io_cmd_ The message containing IO interface configuration parameters
   */

  auto commandGPIO = [&](GpioIdentifier identifier) {
    command_interfaces_[identifier].set_value(static_cast<double>(io_cmd_->input_addrr));
    command_interfaces_[identifier + 1].set_value(static_cast<double>(io_cmd_->output_addrr));
  };

  if ((io_cmd_->input_addrr >= ctrl_limits[0] && io_cmd_->input_addrr <= ctrl_limits[1]) &&
      (io_cmd_->output_addrr >= ctrl_limits[0] && io_cmd_->output_addrr <= ctrl_limits[1]))
  {
    commandGPIO(GpioIdentifier::general1_io_command);
  }
  else if ((io_cmd_->input_addrr >= ctrl_limits[2] && io_cmd_->input_addrr <= ctrl_limits[3]) &&
           (io_cmd_->output_addrr >= ctrl_limits[2] && io_cmd_->output_addrr <= ctrl_limits[3]))
  {
    commandGPIO(GpioIdentifier::general2_io_command);
  }
  else if ((io_cmd_->input_addrr >= ctrl_limits[4] && io_cmd_->input_addrr <= ctrl_limits[5]) &&
           (io_cmd_->output_addrr >= ctrl_limits[4] && io_cmd_->output_addrr <= ctrl_limits[5]))
  {
    commandGPIO(GpioIdentifier::user1_io_command);
  }
  else if ((io_cmd_->input_addrr >= ctrl_limits[6] && io_cmd_->input_addrr <= ctrl_limits[7]) &&
           (io_cmd_->output_addrr >= ctrl_limits[6] && io_cmd_->output_addrr <= ctrl_limits[7]))
  {
    commandGPIO(GpioIdentifier::user2_io_command);
  }
  else if ((io_cmd_->input_addrr >= ctrl_limits[8] && io_cmd_->input_addrr <= ctrl_limits[9]) &&
           (io_cmd_->output_addrr >= ctrl_limits[8] && io_cmd_->output_addrr <= ctrl_limits[9]))
  {
    commandGPIO(GpioIdentifier::user3_io_command);
  }
  else if ((io_cmd_->input_addrr >= ctrl_limits[10] && io_cmd_->input_addrr <= ctrl_limits[11]) &&
           (io_cmd_->output_addrr >= ctrl_limits[10] && io_cmd_->output_addrr <= ctrl_limits[11]))
  {
    commandGPIO(GpioIdentifier::user4_io_command);
  }
  else if ((io_cmd_->input_addrr >= ctrl_limits[12] && io_cmd_->input_addrr <= ctrl_limits[13]) &&
           (io_cmd_->output_addrr >= ctrl_limits[12] && io_cmd_->output_addrr <= ctrl_limits[13]))
  {
    commandGPIO(GpioIdentifier::user5_io_command);
  }
  else if ((io_cmd_->input_addrr >= ctrl_limits[14] && io_cmd_->input_addrr <= ctrl_limits[15]) &&
           (io_cmd_->output_addrr >= ctrl_limits[14] && io_cmd_->output_addrr <= ctrl_limits[15]))
  {
    commandGPIO(GpioIdentifier::user6_io_command);
  }
  else if ((io_cmd_->input_addrr >= ctrl_limits[16] && io_cmd_->input_addrr <= ctrl_limits[17]) &&
           (io_cmd_->output_addrr >= ctrl_limits[16] && io_cmd_->output_addrr <= ctrl_limits[17]))
  {
    commandGPIO(GpioIdentifier::user7_io_command);
  }
  else if ((io_cmd_->input_addrr >= ctrl_limits[18] && io_cmd_->input_addrr <= ctrl_limits[19]) &&
           (io_cmd_->output_addrr >= ctrl_limits[18] && io_cmd_->output_addrr <= ctrl_limits[19]))
  {
    commandGPIO(GpioIdentifier::user8_io_command);
  }
  else
  {
    RCLCPP_WARN(get_node()->get_logger(),
                "command GPIO Callback: INVALID ADDRESS (%d) (%d)"
                "Please select memory addresses that are within acceptable range of the SAME interface",
                io_cmd_->input_addrr, io_cmd_->output_addrr);
  }
}

bool MelfaMonController::configControlMode(melfa_mon_msgs::srv::ModeConfigure::Request::SharedPtr req,
                                           melfa_mon_msgs::srv::ModeConfigure::Response::SharedPtr res)
{
  /**
   * @brief Control mode configuration method for MelfaMonController class
   *
   * This is a binding method for configure_mode_srv_ service
   *
   * @param req The request containing control mode configuration
   * @param res The response with the result of the configuration service.
   * @returns true if configured else false
   */
  std::string control_mode_binary_ = std::to_string(req->user8_io_interface) + std::to_string(req->user7_io_interface) +
                                     std::to_string(req->user6_io_interface) + std::to_string(req->user5_io_interface) +
                                     std::to_string(req->user4_io_interface) + std::to_string(req->user3_io_interface) +
                                     std::to_string(req->user2_io_interface) + std::to_string(req->user1_io_interface) +
                                     std::to_string(req->general2_io_interface) +
                                     std::to_string(req->general1_io_interface);

  int control_mode_int_ = std::bitset<10>(control_mode_binary_).to_ulong();

  res->success = (control_mode_int_ <= 0b1111111111) ?
                     (command_interfaces_[GpioIdentifier::control_mode_io_command].set_value(
                          static_cast<double>(control_mode_int_)),
                      true) :
                     false;

  return res->success;
}

controller_interface::CallbackReturn MelfaMonController::on_activate(const rclcpp_lifecycle::State& /*previous_state*/)
{
  /**
   * @brief activation method for MelfaMonController class
   *
   * This method retrieves specific IO limits from the yaml node based on controller type
   *
   * @param previous_state lifecycle state object representing state before current state
   * @returns CallbackReturn::SUCCESS if limits are retrieved
   */
  if (state_interfaces_.at(GpioIdentifier::ctrl_type_state).get_value() == 1.0)
    ctrl_limits = io_limits_["R"].as<std::vector<long int>>();
  else if (state_interfaces_.at(GpioIdentifier::ctrl_type_state).get_value() == 2.0)
    ctrl_limits = io_limits_["Q"].as<std::vector<long int>>();
  else if (state_interfaces_.at(GpioIdentifier::ctrl_type_state).get_value() == 3.0)
    ctrl_limits = io_limits_["D"].as<std::vector<long int>>();
  else
    RCLCPP_WARN(get_node()->get_logger(), "Controller Type not recognized. %s: %f.",
                state_interfaces_.at(GpioIdentifier::ctrl_type_state).get_name().c_str(),
                state_interfaces_.at(GpioIdentifier::ctrl_type_state).get_value());

  return LifecycleNodeInterface::CallbackReturn::SUCCESS;
}

controller_interface::CallbackReturn
MelfaMonController::on_deactivate(const rclcpp_lifecycle::State& /*previous_state*/)
{
  /**
   * @brief deactivation method for MelfaMonController class
   *
   * This method resets all IO state publishers, configuration services
   *
   * @param previous_state lifecycle state object representing state before current state
   * @returns CallbackReturn::SUCCESS if resets are done
   * @returns CallbackReturn::ERROR if exception is raised
   */
  try
  {
    general1_io_state_publisher_.reset();
    general2_io_state_publisher_.reset();
    user1_io_state_publisher_.reset();
    user2_io_state_publisher_.reset();
    user3_io_state_publisher_.reset();
    user4_io_state_publisher_.reset();
    user5_io_state_publisher_.reset();
    user6_io_state_publisher_.reset();
    user7_io_state_publisher_.reset();
    user8_io_state_publisher_.reset();
    configure_io_mon_srv_.reset();
    configure_mon_mode_srv_.reset();
  }
  catch (...)
  {
    return LifecycleNodeInterface::CallbackReturn::ERROR;
  }
  return LifecycleNodeInterface::CallbackReturn::SUCCESS;
}

}  // namespace melfa_mon_controllers

#include "pluginlib/class_list_macros.hpp"

PLUGINLIB_EXPORT_CLASS(melfa_mon_controllers::MelfaMonController, controller_interface::ControllerInterface)