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
/**
 * @file melfa_rt_mon.hpp
 * @author Liu Muyao
 * @brief Header file for Real Time Monitoring API
 *
 * @copyright COPYRIGHT (C) 2025 Mitsubishi Electric Corporation
 *
 */

#ifndef __RTMON__
#define __RTMON__

#include "rt_mon_def.hpp"
#include <iostream>
#include <iomanip>
#include <string.h>
#ifdef _WIN32
#define _USE_MATH_DEFINES
#include <math.h>
#include <winsock2.h>
#include <windows.h>
#include <conio.h>
#include <ws2tcpip.h>
#include <ws2spi.h>
#endif
#ifdef __linux__
#define _USE_MATH_DEFINES
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sched.h>
#include <sys/mman.h>
#endif
#define MAXBUFLEN 512   // Max Buffer Length
#define NO_FLAGS_SET 0  // Literally no flags set

/**
 * @brief melfa ethernet api namespace. Refer to Mitsubishi Electric Industrial Robot Ethernet Function Instruction
 * Manual for detailed descriptions
 *
 */
namespace MelfaEthernet
{
/**
 * @brief melfa ethernet api, Real Time Monitor Class Object. Refer to Mitsubishi Electric Industrial Robot
 * Ethernet Function Instruction Manual for detailed descriptions
 *
 */
class rtmon
{
protected:
#if _WIN32
  WSAData Data;
#endif

  /**
   * @brief Private variable to store data types for monitoring
   *
   */
  int type_mon[4];  // Monitor data type.

  /**
   * @brief Private variable. Receive transmission timeout setting. \
   * @brief Operation Control Time: CR800 3.5ms. CR750/1 7.11ms.
   *
   */
  timeval sTimeOut;

  /**
   * @brief Packet variable for sending
   *
   */
  RTMONPACK RTMONsend;

  /**
   * @brief Packet variable for receiving
   *
   */
  RTMONPACK RTMONrecv;

  /**
   * @brief Private variable for console prints for debug mode
   *
   */
  char console_msg[MAXBUFLEN];

  /**
   * @brief Send array
   *
   */
  char sendText[MAXBUFLEN];

  /**
   * @brief Receive array
   *
   */
  char recvText[MAXBUFLEN];

  /**
   * @brief counts the number of packets sent
   * 
   */
  uint64_t counter_;

  /**
   * @brief Command Structure used by Real Time Monitoring API
   *
   */
  typedef struct
  {
    /**
     * @brief Command. MON_CMD_START to START Monitoring.\
     * @brief MON_CMD_END to end  END Monitoring
     */
    uint16_t cmd_type;  // Command type 0/1/255

    /**
     * @brief Designate data types for monitoring
     *
     */
    uint16_t mon_dat[4];

    /**
     * @brief Send input/output signal data designation. \
     * @brief MXT_IO_OUT for output signal. MXT_IO_NULL for input signal
     *
     */
    uint16_t INBitTop;

    /**
     * @brief Return input/output signal data designation. \
     * @brief MXT_IO_OUT for output signal. MXT_IO_NULL for input signal
     *
     */
    uint16_t OUTBitTop;

  } CMD_packet;

  /**
   * @brief Feedback structure used by Real Time Monitor API
   *
   */
  typedef struct
  {
    /**
     * @brief POSE command data feedback. Data from MON_TYP_POSE_CMD\
     * @brief [mm/rad]
     *
     */
    POSE pos_FB;

    /**
     * @brief JOINT command data feedback. Data from MON_TYP_JOINT_CMD\
     *@brief [rad]
     *
     */
    JOINT jnt_FB;

    /**
     * @brief PULSE command data feedback. Data from MON_TYP_PULSE_CMD\
     * @brief [pulse]
     *
     */
    PULSE pls_FB;

    /**
     * @brief POSE filtered command data feedback. Data from MON_TYP_FPOSE_CMD\
     * @brief [mm/rad]
     *
     */
    POSE pos_FFB;

    /**
     * @brief JOINT filtered command data feedback. Data from MON_TYP_FJOINT_CMD\
     * @brief [rad]
     *
     */
    JOINT jnt_FFB;

    /**
     * @brief PULSE filtered command data feedback. Data from MON_TYP_FPULSE_CMD\
     * @brief [pulse]
     *
     */
    PULSE pls_FFB;

    /**
     * @brief POSE Encoder feedback. Data from MON_TYP_POSE_FB\
     * @brief [mm/rad]
     *
     */
    POSE pos_EFB;

    /**
     *
     * @brief JOINT Encoder feedback. Data from MON_TYP_JOINT_FB\
     *  @brief [rad]
     *
     */
    JOINT jnt_EFB;

    /**
     * @brief PULSE Encoder feedback. Data from MON_TYP_PULSE_FB\
     * @brief [pulse]
     *
     */
    PULSE pls_EFB;

    /**
     * @brief Current Command from robot controller to motors. Data from MON_TYP_CURR_CMD\
     * @brief [0.1% rate]
     *
     */
    PULSE curr_cmd;

    /**
     * @brief Current Feedback from motors. Data from MON_TYP_CURR_FB\
     * @brief [0.1% rate]
     *
     */
    PULSE curr_FB;

    /**
     * @brief Robot Information. Data from MON_TYP_ROBMON.\
     * @brief See struct ROBMON for breakdown
     *
     */
    ROBMON robot_info;

    /**
     * @brief Position droop. \
     * @brief Data from MON_TYP_POSE_DROOP\
     * @brief [pulse]
     *
     */
    PULSE pos_droop;

    /**
     * @brief Speed command. \
     * @brief Data from MON_TYP_SPEED_CMD\
     * @brief [rpm]
     *
     */
    PULSE speed_cmd;

    /**
     * @brief Speed feedback. \
     * @brief Data from MON_TYP_SPEED_FB\
     * @brief [rpm]
     *
     */
    PULSE speed_fb;

    /**
     * @brief Axis load level. \
     * @brief Data from MON_TYP_AXIS_LOAD\
     * @brief [%]
     *
     */
    FLOAT8 axis_load;

    /**
     * @brief Encoder temperature. \
     * @brief Data from MON_TYP_ENC_TEMP\
     * @brief [degree C]
     *
     */
    PULSE enc_temp;

    /**
     * @brief Encoder miscount. \
     * @brief Data from MON_TYP_ENC_MIS\
     * @brief [pulse]
     *
     */
    PULSE enc_mis;

    /**
     * @brief Motor voltage. \
     * @brief Data from MON_TYP_MOTOR_V\
     * @brief [V]
     *
     */
    PULSE motor_v;

    /**
     * @brief Regeneration Level. \
     * @brief Data from MON_TYP_REGEN_LVL\
     * @brief [%]
     *
     */
    PULSE regen_lvl;

    /**
     * @brief Tolerable command +. \
     * @brief Data from MON_TYP_TOL_CMD_UPPER\
     * @brief [0.1% rate]
     *
     */
    PULSE tol_cmd_upper;

    /**
     * @brief Tolerable command -. \
     * @brief Data from MON_TPY_TOL_CMD_LOWER\
     * @brief [0.1% rate]
     *
     */
    PULSE tol_cmd_lower;

    /**
     * @brief RMS current. \
     * @brief Data from MON_TYP_RMS_CURR\
     * @brief [0.1% rate]
     *
     */
    PULSE rms_curr;

    /**
     * @brief Force sensor current position. \
     * @brief Data from MON_TYP_FORCE_SENSOR_AT_POSE\
     * @brief [N/Nm]
     *
     */
    FORCE fs_curr_pose;

    /**
     * @brief Force sensor original data after offset cancel. \
     * @brief Data from MON_TYP_FORCE_SENSOR_AFTER_OFFSET\
     * @brief [N/Nm]
     *
     */
    FORCE fs_after;

    /**
     * @brief Force sensor original data after before cancel. \
     * @brief Data from MON_TYP_FORCE_SENSOR_BEFORE_OFFSET\
     * @brief [N/Nm]
     *
     */
    FORCE fs_before;

    /**
     *
     * @brief Position command of the force sensor correction. \
     * @brief Data from MON_TYP_POSE_CMD_FOR_FORCE_SENSOR_CORRECTION\
     * @brief [mm/rad]
     *
     */
    POSE pos_cmd_fs;

    /**
     * @brief COL presumed torque. \
     * @brief Data form MON_TYP_COL_PRESUMED_TORQUE\
     * @brief [0.1% rate]
     *
     */
    PULSE col_torque;

    /**
     * @brief COL threshold +. \
     * @brief Data form MON_TYP_COL_THRESHOLD_UPPER\
     * @brief [0.1% rate]
     *
     */
    PULSE col_thres_upper;

    /**
     * @brief COL threshold -. \
     * @brief Data form MON_TYP_COL_THRESHOLD_LOWER\
     * @brief [0.1% rate]
     *
     */
    PULSE col_thres_lower;

    /**
     * @brief POSE command with angles in degrees. \
     * @brief Data from MON_TYP_POSE_CMD_DEGREES.\
     * @brief [mm/deg]
     *
     */
    POSE pos_cmd_deg;

    /**
     * @brief JOINT command with angles in degrees. \
     * @brief Data from MON_TYP_JOINT_CMD_DEGREES.\
     * @brief [deg]
     *
     */
    JOINT jnt_cmd_deg;

    /**
     * @brief POSE feedback with angles in degrees. \
     * @brief Data from MON_TYP_POSE_FB_DEGREES.\
     * @brief [mm/deg]
     *
     */
    POSE pos_fb_deg;

    /**
     * @brief JOINT feedback with angles in degrees. \
     * @brief Data from MON_TYP_JOINT_FB_DEGREES.\
     * @brief [deg]
     *
     */
    JOINT jnt_fb_deg;

    /**
     * @brief Current command in Ampere RMS. \
     * @brief Data from MON_TYP_CURR_CMD_ARMS.\
     * @brief [Arms]
     *
     */
    FLOAT8 curr_cmd_arms;

    /**
     * @brief Current feedback in Ampere RMS. \
     * @brief Data from MON_TYP_CURR_FB_ARMS\
     * @brief [Arms]
     *
     */
    FLOAT8 curr_fb_arms;

    /**
     * @brief Tolerbable command +. \
     * @brief Data from MON_TYP_TOL_CMD_UPPER_ARMS\
     * @brief[Arms]
     *
     */
    FLOAT8 tol_cmd_upper_arms;

    /**
     * @brief Tolerbable command -. \
     * @brief Data from MON_TYP_TOL_CMD_LOWER_ARMS\
     * @brief[Arms]
     *
     */
    FLOAT8 tol_cmd_lower_arms;

    /**
     * @brief RMS current. \
     * @brief Data from MON_TYP_RMS_CURR_ARMS.\
     * @brief [Arms]
     *
     */
    FLOAT8 rms_curr_arms;

    /**
     * @brief Input top bit
     *
     */
    uint16_t INBitTop;

    /**
     * @brief Output top bit
     *
     */
    uint16_t OUTBitTop;

    /**
     * @brief Input bit data
     *
     */
    uint32_t INBitData;

    /**
     * @brief Output bit data
     *
     */
    uint32_t OUTBitData;

    /**
     * @brief Communication Count
     *
     */
    uint32_t CCount;

  } FB_packet;

  /**
   * @brief IP settings used in API.
   *
   */
  typedef struct
  {
/**
 * @brief Generated. Socket descriptor
 *
 */
#if __linux__
    int destSocket;
#endif
#if _WIN32
    SOCKET destSocket;
#endif

    /**
     * @brief Generated. Structure for AF_INET
     *
     */
    struct sockaddr_in send_destSockAddr;

    /**
     * @brief Generated. Structure for AF_INET
     *
     */
    struct sockaddr_in recv_destSockAddr;

    /**
     * @brief User input. Destination IP address.
     *
     */
    std::string dst_ip_address;

    /**
     * @brief User input. Port number
     *
     */
    unsigned short sendport;

    /**
     * @brief User input. Port number
     *
     */
    unsigned short recvport;

    /**
     * @brief Generated. net_addr return value. IPv4 network address in internet standard dot notation.
     *
     */
    int destAddr;

  } ip_settings;

public:

  /**
   * @brief timeout value
   */
  int rtmon_timeout_ = 100;

  /**
   * @brief Switch to debug mode. debug mode = 1. Normal = 0.
   *
   */
  int RT_API_DEBUG_MODE = 0;

  /**
   * @brief Track packet loss
   *
   */
  int packet_recv_lost;

  /**
   * @brief set robot controller clock cycle period. To calculate resolution for receving packets.
   *
   */
  float period = 3.5;

  /**
   * @brief true if robot connection is available. false if robot is disconnected.
   *
   */
  bool robot_status = true;

  /**
   * @brief counts consecutive packet losses. Used to trigger robot_status.
   *
   */
  int packet_lost_consec_cnt;

  /**
   * @brief Local command packet
   *
   */
  CMD_packet cmd_pack;

  /**
   * @brief robot ethernet settings
   *
   */
  ip_settings robot_ip;

  /**
   * @brief local feedback packet
   *
   */
  FB_packet fb_pack;

  /**
   * @brief Construct a rtmon::rtmon object. Initialize class variables.
   * @brief If _WIN32, initialize WSAStartup. If debug mode is active, prints "Debug mode active."
   *
   */
  rtmon(float custom_cycle_time = 3.5);

  /**
   * @brief Create _WIN32 or UNIX UDP port using details in ip_settings this_port.
   * @return int
   */
  int create_port();

  /**
   * @brief Convert and arrange data from cmd_pack to RTMONsend and sends packet.
   * @return int
   */
  int WriteToRobot_MON_();

  /**
   * @brief Receive packet from robot controller. Convert and arrange data from RTMONrecv to fb_pack.
   * @return int
   */
  int ReadFromRobot_FB_();

  /**
   * @brief Send RTMONsend to robot controller. Only 1 RTMONsend per class object
   * @return int
   */
  int send_packet_();

  /**
   * @brief Receive RTMONrecv from robot controller. Only 1 RTMONrecv per class object
   * @return int
   */
  int recv_packet_();

  /**
   * @brief Prints monitored data from feedback struct, FB_packet
   * @return int
   */
  int print_monitored_feedback();

  /**
   * @brief Prints all data from feedback struct, FB_packet
   * @return int
   */
  int print_all_feedback();
};
}  // namespace MelfaEthernet
#endif
/*EOF*/