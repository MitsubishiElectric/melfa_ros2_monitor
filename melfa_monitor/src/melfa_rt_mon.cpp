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
 * @file melfa_rt_mon.cpp
 * @author Liu Muyao
 * @brief Header file for Real Time Monitoring API
 *
 * @copyright COPYRIGHT (C) 2025 Mitsubishi Electric Corporation
 *
 */
#include "melfa_monitor/melfa_rt_mon.hpp"

#if _WIN32
#pragma comment(lib, "Ws2_32.lib")
#endif

namespace MelfaEthernet
{

rtmon::rtmon(float custom_cycle_time)
{
#ifdef _WIN32
  if (WSAStartup(MAKEWORD(1, 1), &Data) == SOCKET_ERROR)
  {
    std::cout << "Error initialising WSA.\n";
    system("pause");
  }

#endif
  if (RT_API_DEBUG_MODE)
    std::cout << "Debug mode Active.\n";
  period = custom_cycle_time;
  counter_ = 0;
  packet_recv_lost = 0;
  memset(&cmd_pack, 0, sizeof(CMD_packet));
  memset(&fb_pack, 0, sizeof(FB_packet));
  robot_ip.recvport = 0;
}
int rtmon::create_port()
{
  // IP Setting
  robot_ip.destSocket = socket(AF_INET, SOCK_DGRAM, 0);  // UDP
  memset(&(robot_ip.send_destSockAddr), 0, sizeof(robot_ip.send_destSockAddr));
  robot_ip.destAddr = inet_addr(robot_ip.dst_ip_address.c_str());  // get IPv4 network address in standard dot format.
  memcpy(&(robot_ip.send_destSockAddr.sin_addr), &(robot_ip.destAddr), sizeof(robot_ip.destAddr));
  robot_ip.send_destSockAddr.sin_port = htons(robot_ip.sendport);  // set port number
  robot_ip.send_destSockAddr.sin_family = AF_INET;                 // set standard struct

  if(robot_ip.recvport!=0)
  {
  robot_ip.recv_destSockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  robot_ip.recv_destSockAddr.sin_port = htons(robot_ip.recvport);  // set port number
  robot_ip.recv_destSockAddr.sin_family = AF_INET;                 // set standard struct
  }

  bind(robot_ip.destSocket, (struct sockaddr*)&robot_ip.recv_destSockAddr, sizeof(robot_ip.recv_destSockAddr));

  // Creating Port
  if (RT_API_DEBUG_MODE)
  {
#if __linux__
    sprintf(
#endif
#if _WIN32
    sprintf_s(
#endif

              console_msg, "Creating Send Socket......\nDestination IP: %s\nSource Port: %d\nDestination Port: %d\n",
            robot_ip.dst_ip_address.c_str(), robot_ip.recvport, robot_ip.sendport);
    std::cout << console_msg;
  }

#if __linux__
  int n = 512;
  if (setsockopt(robot_ip.destSocket, SOL_SOCKET, SO_RCVBUF, &n, sizeof(n)) == -1)
    if (RT_API_DEBUG_MODE)
      std::cerr << "WARNING: setsockopt FAILED\n";
#endif
  if (robot_ip.destSocket < 0)
  {
    if (RT_API_DEBUG_MODE)
      std::cerr << "ERROR: Port Creation FAILED.\n";
    return -1;
  }
  else
  {
    if (RT_API_DEBUG_MODE)
      std::cerr << "Send Port Creation Successful.\n";
  }
  return 0;
}
int rtmon::WriteToRobot_MON_()
{
  memset(&RTMONsend, 0, sizeof(RTMONsend));
  *type_mon = *cmd_pack.mon_dat;
  *(type_mon + 1) = *(cmd_pack.mon_dat + 1);
  *(type_mon + 2) = *(cmd_pack.mon_dat + 2);
  *(type_mon + 3) = *(cmd_pack.mon_dat + 3);
  RTMONsend.Command = cmd_pack.cmd_type;
  RTMONsend.RecvType1 = (uint16_t)type_mon[0];
  RTMONsend.RecvType2 = (uint16_t)type_mon[1];
  RTMONsend.RecvType3 = (uint16_t)type_mon[2];
  RTMONsend.RecvType4 = (uint16_t)type_mon[3];
  RTMONsend.INBitTop = cmd_pack.INBitTop;
  RTMONsend.OUTBitTop = cmd_pack.OUTBitTop;
  if (RT_API_DEBUG_MODE)
  {
    std::cout << "Monitoring Setting:\n";
    for (int i = 0; i < 4; i++)
    {
      switch (type_mon[i])
      {
        case MON_TYP_NULL:
          std::cout << "NULL Monitoring Setting\n";
          break;
        case MON_TYP_JOINT_CMD:
          std::cout << "Joint Command\n";
          break;
        case MON_TYP_FJOINT_CMD:
          std::cout << "Joint Filtered Command\n";
          break;
        case MON_TYP_JOINT_FB:
          std::cout << "Joint Feedback\n";
          break;
        case MON_TYP_POSE_CMD:
          std::cout << "POSE Command\n";
          break;
        case MON_TYP_FPOSE_CMD:
          std::cout << "POSE Filtered Command\n";
          break;
        case MON_TYP_POSE_FB:
          std::cout << "POSE Feedback\n";
          break;
        case MON_TYP_PULSE_CMD:
          std::cout << "PULSE Command\n";
          break;
        case MON_TYP_FPULSE_CMD:
          std::cout << "PULSE Filtered Command\n";
          break;
        case MON_TYP_PULSE_FB:
          std::cout << "PULSE Feedback\n";
          break;
        case MON_TYP_CURR_CMD:
          std::cout << "Current Command\n";
          break;
        case MON_TYP_CURR_FB:
          std::cout << "Current Feedback\n";
          break;
        case MON_TYP_ROBMON:
          std::cout << "Robot Information\n";
          break;
        case MON_TYP_POS_DROOP:
          std::cout << "Position droop\n";
          break;
        case MON_TYP_SPEED_CMD:
          std::cout << "Speed command\n";
          break;
        case MON_TYP_SPEED_FB:
          std::cout << "Speed feedback\n";
          break;
        case MON_TYP_AXIS_LOAD:
          std::cout << "Axis load level\n";
          break;
        case MON_TYP_ENC_TEMP:
          std::cout << "Encoder temperture\n";
          break;
        case MON_TYP_ENC_MIS:
          std::cout << "Encoder miscount\n";
          break;
        case MON_TYP_MOTOR_V:
          std::cout << "Motor voltage\n";
          break;
        case MON_TYP_REGEN_LVL:
          std::cout << "Rgeneration Level\n";
          break;
        case MON_TYP_TOL_CMD_UPPER:
          std::cout << "Tolerable command +\n";
          break;
        case MON_TPY_TOL_CMD_LOWER:
          std::cout << "Tolerable command\n";
          break;
        case MON_TYP_RMS_CURR:
          std::cout << "RMS current\n";
          break;
        case MON_TYP_FORCE_SENSOR_AT_POSE:
          std::cout << "Force sensor current position\n";
          break;
        case MON_TYP_FORCE_SENSOR_AFTER_OFFSET:
          std::cout << "Force sensor original data after offset cancel\n";
          break;
        case MON_TYP_FORCE_SENSOR_BEFORE_OFFSET:
          std::cout << "Force sensor original data before offset cancel\n";
          break;
        case MON_TYP_POSE_CMD_FOR_FORCE_SENSOR_CORRECTION:
          std::cout << "Position command of the force sensor correction\n";
          break;
        case MON_TYP_COL_PRESUMED_TORQUE:
          std::cout << "COL presumed torque\n";
          break;
        case MON_TYP_COL_THRESHOLD_UPPER:
          std::cout << "COL threshold +\n";
          break;
        case MON_TYP_COL_THRESHOLD_LOWER:
          std::cout << "COL threshold -\n";
          break;
        case MON_TYP_POSE_CMD_DEGREES:
          std::cout << "POSE command with angles in degrees\n";
          break;
        case MON_TYP_JOINT_CMD_DEGREES:
          std::cout << "JOINT command with angles in degrees\n";
          break;
        case MON_TYP_POSE_FB_DEGREES:
          std::cout << "POSE feedback with angles in degrees\n";
          break;
        case MON_TYP_JOINT_FB_DEGREES:
          std::cout << "JOINT feedback with angles in degrees\n";
          break;
        case MON_TYP_CURR_CMD_ARMS:
          std::cout << "Curent command in Ampere RMS\n";
          break;
        case MON_TYP_CURR_FB_ARMS:
          std::cout << "Curent feedback in Ampere RMS\n";
          break;
        case MON_TYP_TOL_CMD_UPPER_ARMS:
          std::cout << "Tolerable command +\n";
          break;
        case MON_TYP_TOL_CMD_LOWER_ARMS:
          std::cout << "Tolerable command -\n";
          break;
        case MON_TYP_RMS_CURR_ARMS:
          std::cout << "RMS current";
          break;
        default:
          std::cout << "Invalid Monitoring Setting";
          break;
      }
    }
  }
  if (send_packet_() != 0)
    return -1;
  return 0;
}
int rtmon::send_packet_()
{
  int size;
  memset(sendText, 0, MAXBUFLEN);
  memcpy(sendText, &RTMONsend, sizeof(RTMONsend));

  if (RT_API_DEBUG_MODE)
  {
    memset(console_msg, 0, sizeof(console_msg));
#if __linux__
    sprintf(
#endif
#if _WIN32
    sprintf_s(
#endif
      console_msg, "Sending to: destAddr=%d, destSocket=%d, dst_ip_address = %s, port= %d.\n", robot_ip.destAddr,
            (int)robot_ip.destSocket, robot_ip.dst_ip_address.c_str(), robot_ip.sendport);
    std::cout << console_msg;
  }

  size = sendto(robot_ip.destSocket, sendText, sizeof(RTMONPACK), NO_FLAGS_SET,
                (struct sockaddr*)&(robot_ip.send_destSockAddr), sizeof((robot_ip.send_destSockAddr)));
  if (size != sizeof(RTMONPACK))
  {
    if (RT_API_DEBUG_MODE)
    {
      memset(console_msg, 0, sizeof(console_msg));
#if __linux__
      sprintf(console_msg, "errno= %s,size = %d, size of RTMONPACK = %ld.\n", strerror(errno), size, sizeof(RTMONPACK));
#endif
#if _WIN32
      char error_buffer[20];
      strerror_s(error_buffer, 20, errno);
      sprintf_s(console_msg, "errno= %s,size = %d, size of RTMONPACK = %lld.\n", error_buffer, size, sizeof(RTMONPACK));
#endif
      std::cerr << "Error: Send packet FAILED.\n" << console_msg;
    }
    return -1;
  }
  else
  {
    if (RT_API_DEBUG_MODE)
      std::cout << "Send Packet Size: " << size << "\n";
  }
  if (size < 0)
  {
    if (RT_API_DEBUG_MODE)
    {
      memset(console_msg, 0, sizeof(console_msg));
#ifdef __linux__
      sprintf(console_msg, "errno= %s,size = %d, size of RTMONPACK = %ld.\n", strerror(errno), size, sizeof(RTMONPACK));
#endif
#ifdef _WIN32
      char error_buffer[20];
      strerror_s(error_buffer, 20, errno);
      sprintf_s(console_msg, "WSAerro= %s,size = %d, size of RTMONPACK = %lld.\n", error_buffer, size,
                sizeof(RTMONPACK));
#endif
      std::cerr << "Error: Send packet FAILED.\n" << console_msg;
    }
    return -1;
  }
  if (RT_API_DEBUG_MODE)
    std::cout << "Packet sent.\n";
  counter_++;
  return 0;
}

int rtmon::ReadFromRobot_FB_()
{
  if (recv_packet_() == 0)
  {
    if (RT_API_DEBUG_MODE)
      std::cout << "Packet Received. Processing....\n";
    packet_lost_consec_cnt = 0;
  }
  else
  {
    if (RT_API_DEBUG_MODE)
      std::cout << "Packet receive failed\n";
    packet_lost_consec_cnt += 1;
    if (packet_lost_consec_cnt >= rtmon_timeout_)
      robot_status = false;

    return -1;
  }
  memset(&RTMONrecv, 0, sizeof(RTMONrecv));
  memcpy(&RTMONrecv, &recvText, sizeof(RTMONrecv));
  fb_pack.INBitTop = RTMONrecv.INBitTop;
  fb_pack.OUTBitTop = RTMONrecv.OUTBitTop;
  fb_pack.INBitData = RTMONrecv.INBitData;
  fb_pack.OUTBitData = RTMONrecv.OUTBitData;
  fb_pack.CCount = RTMONrecv.CCount;

  switch (RTMONrecv.RecvType1)
  {
    case MON_TYP_JOINT_CMD:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading JOINT command values\n";
      fb_pack.jnt_FB = RTMONrecv.dat1.jnt;
      break;
    case MON_TYP_FJOINT_CMD:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading filtered JOINT command values\n";
      fb_pack.jnt_FFB = RTMONrecv.dat1.jnt;
      break;
    case MON_TYP_JOINT_FB:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading JOINT encoder values\n";
      fb_pack.jnt_EFB = RTMONrecv.dat1.jnt;
      break;
    case MON_TYP_POSE_CMD:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading POSE command values\n";
      fb_pack.pos_FB = RTMONrecv.dat1.pos;
      break;
    case MON_TYP_FPOSE_CMD:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading filtered POSE command values\n";
      fb_pack.pos_EFB = RTMONrecv.dat1.pos;
      break;
    case MON_TYP_POSE_FB:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading POSE encoder values\n";
      fb_pack.pos_EFB = RTMONrecv.dat1.pos;
      break;
    case MON_TYP_PULSE_CMD:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading PULSE command values\n";
      fb_pack.pls_FB = RTMONrecv.dat1.pls;
      break;
    case MON_TYP_FPULSE_CMD:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading filtered PULSE command values\n";
      fb_pack.pls_FFB = RTMONrecv.dat1.pls;
      break;
    case MON_TYP_PULSE_FB:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading PULSE encoder values\n";
      fb_pack.pls_EFB = RTMONrecv.dat1.pls;
      break;
    case MON_TYP_CURR_CMD:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading CURRENT command values\n";
      fb_pack.curr_cmd = RTMONrecv.dat1.pls;
      break;
    case MON_TYP_CURR_FB:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading CURRENT feedback values\n";
      fb_pack.curr_FB = RTMONrecv.dat1.pls;
      break;
    case MON_TYP_ROBMON:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading Robot Information\n";
      fb_pack.robot_info = RTMONrecv.dat1.robmon;
      break;
    case MON_TYP_POS_DROOP:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading Position Droop\n";
      fb_pack.pos_droop = RTMONrecv.dat1.pls;
      break;
    case MON_TYP_SPEED_CMD:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading Speed Command\n";
      fb_pack.speed_cmd = RTMONrecv.dat1.pls;
      break;
    case MON_TYP_SPEED_FB:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading Speed Command\n";
      fb_pack.speed_fb = RTMONrecv.dat1.pls;
      break;
    case MON_TYP_AXIS_LOAD:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading Speed Feedback\n";
      fb_pack.axis_load = RTMONrecv.dat1.float8;
      break;
    case MON_TYP_ENC_TEMP:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading Encoder Temperature\n";
      fb_pack.enc_temp = RTMONrecv.dat1.pls;
      break;
    case MON_TYP_ENC_MIS:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading Encoder Miscount\n";
      fb_pack.enc_mis = RTMONrecv.dat1.pls;
      break;
    case MON_TYP_MOTOR_V:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading Motor Voltage\n";
      fb_pack.motor_v = RTMONrecv.dat1.pls;
      break;
    case MON_TYP_REGEN_LVL:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading Regeneration Level\n";
      fb_pack.regen_lvl = RTMONrecv.dat1.pls;
      break;
    case MON_TYP_TOL_CMD_UPPER:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading Tolerable command +\n";
      fb_pack.tol_cmd_upper = RTMONrecv.dat1.pls;
      break;
    case MON_TPY_TOL_CMD_LOWER:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading Tolerable command -\n";
      fb_pack.tol_cmd_lower = RTMONrecv.dat1.pls;
      break;
    case MON_TYP_RMS_CURR:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading RMS current\n";
      fb_pack.rms_curr = RTMONrecv.dat1.pls;
      break;
    case MON_TYP_FORCE_SENSOR_AT_POSE:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading Force sensor current position\n";
      fb_pack.fs_curr_pose = RTMONrecv.dat1.force;
      break;
    case MON_TYP_FORCE_SENSOR_AFTER_OFFSET:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading Force sensor original data after offset cancel\n";
      fb_pack.fs_after = RTMONrecv.dat1.force;
      break;
    case MON_TYP_FORCE_SENSOR_BEFORE_OFFSET:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading Force sensor original data before offset cancel\n";
      fb_pack.fs_after = RTMONrecv.dat1.force;
      break;
    case MON_TYP_POSE_CMD_FOR_FORCE_SENSOR_CORRECTION:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading Position command of the force sensor correction\n";
      fb_pack.pos_cmd_fs = RTMONrecv.dat1.pos;
      break;
    case MON_TYP_COL_PRESUMED_TORQUE:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading COL presumed torque\n";
      fb_pack.col_torque = RTMONrecv.dat1.pls;
      break;
    case MON_TYP_COL_THRESHOLD_UPPER:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading COL threshold +\n";
      fb_pack.col_thres_upper = RTMONrecv.dat1.pls;
      break;
    case MON_TYP_COL_THRESHOLD_LOWER:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading COL threshold -\n";
      fb_pack.col_thres_lower = RTMONrecv.dat1.pls;
      break;
    case MON_TYP_POSE_CMD_DEGREES:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading POSE command with angles in degrees\n";
      fb_pack.pos_cmd_deg = RTMONrecv.dat1.pos;
      break;
    case MON_TYP_JOINT_CMD_DEGREES:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading JOINT command with angles in degrees\n";
      fb_pack.jnt_cmd_deg = RTMONrecv.dat1.jnt;
      break;
    case MON_TYP_POSE_FB_DEGREES:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading POSE feedback with angles in degrees\n";
      fb_pack.pos_fb_deg = RTMONrecv.dat1.pos;
      break;
    case MON_TYP_JOINT_FB_DEGREES:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading JOINT feedback with angles in degrees\n";
      fb_pack.jnt_fb_deg = RTMONrecv.dat1.jnt;
      break;
    case MON_TYP_CURR_CMD_ARMS:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading Curent command in Ampere RMS\n";
      fb_pack.curr_cmd_arms = RTMONrecv.dat1.float8;
      break;
    case MON_TYP_CURR_FB_ARMS:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading Curent feedback in Ampere RMS\n";
      fb_pack.curr_fb_arms = RTMONrecv.dat1.float8;
      break;
    case MON_TYP_TOL_CMD_UPPER_ARMS:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading Tolerable command +\n";
      fb_pack.tol_cmd_upper_arms = RTMONrecv.dat1.float8;
      break;
    case MON_TYP_TOL_CMD_LOWER_ARMS:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading Tolerable command -\n";
      fb_pack.tol_cmd_lower_arms = RTMONrecv.dat1.float8;
      break;
    case MON_TYP_RMS_CURR_ARMS:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading RMS current";
      fb_pack.rms_curr_arms = RTMONrecv.dat1.float8;
      break;
    case MON_TYP_NULL:
      if (RT_API_DEBUG_MODE)
      {
        std::cout << "Reading NULL\n";
      }
      break;
    default:
      if (RT_API_DEBUG_MODE)
        std::cout << "Bad data type.¥n" << std::endl;
      break;
  }

  switch (RTMONrecv.RecvType2)
  {
    case MON_TYP_JOINT_CMD:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading JOINT command values\n";
      fb_pack.jnt_FB = RTMONrecv.dat2.jnt;
      break;
    case MON_TYP_FJOINT_CMD:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading filtered JOINT command values\n";
      fb_pack.jnt_FFB = RTMONrecv.dat2.jnt;
      break;
    case MON_TYP_JOINT_FB:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading JOINT encoder values\n";
      fb_pack.jnt_EFB = RTMONrecv.dat2.jnt;
      break;
    case MON_TYP_POSE_CMD:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading POSE command values\n";
      fb_pack.pos_FB = RTMONrecv.dat2.pos;
      break;
    case MON_TYP_FPOSE_CMD:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading filtered POSE command values\n";
      fb_pack.pos_EFB = RTMONrecv.dat2.pos;
      break;
    case MON_TYP_POSE_FB:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading POSE encoder values\n";
      fb_pack.pos_EFB = RTMONrecv.dat2.pos;
      break;
    case MON_TYP_PULSE_CMD:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading PULSE command values\n";
      fb_pack.pls_FB = RTMONrecv.dat2.pls;
      break;
    case MON_TYP_FPULSE_CMD:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading filtered PULSE command values\n";
      fb_pack.pls_FFB = RTMONrecv.dat2.pls;
      break;
    case MON_TYP_PULSE_FB:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading PULSE encoder values\n";
      fb_pack.pls_EFB = RTMONrecv.dat2.pls;
      break;
    case MON_TYP_CURR_CMD:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading CURRENT command values\n";
      fb_pack.curr_cmd = RTMONrecv.dat2.pls;
      break;
    case MON_TYP_CURR_FB:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading CURRENT feedback values\n";
      fb_pack.curr_FB = RTMONrecv.dat2.pls;
      break;
    case MON_TYP_ROBMON:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading Robot Information\n";
      fb_pack.robot_info = RTMONrecv.dat2.robmon;
      break;
    case MON_TYP_POS_DROOP:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading Position Droop\n";
      fb_pack.pos_droop = RTMONrecv.dat2.pls;
      break;
    case MON_TYP_SPEED_CMD:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading Speed Command\n";
      fb_pack.speed_cmd = RTMONrecv.dat2.pls;
      break;
    case MON_TYP_SPEED_FB:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading Speed Command\n";
      fb_pack.speed_fb = RTMONrecv.dat2.pls;
      break;
    case MON_TYP_AXIS_LOAD:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading Speed Feedback\n";
      fb_pack.axis_load = RTMONrecv.dat2.float8;
      break;
    case MON_TYP_ENC_TEMP:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading Encoder Temperature\n";
      fb_pack.enc_temp = RTMONrecv.dat2.pls;
      break;
    case MON_TYP_ENC_MIS:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading Encoder Miscount\n";
      fb_pack.enc_mis = RTMONrecv.dat2.pls;
      break;
    case MON_TYP_MOTOR_V:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading Motor Voltage\n";
      fb_pack.motor_v = RTMONrecv.dat2.pls;
      break;
    case MON_TYP_REGEN_LVL:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading Regeneration Level\n";
      fb_pack.regen_lvl = RTMONrecv.dat2.pls;
      break;
    case MON_TYP_TOL_CMD_UPPER:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading Tolerable command +\n";
      fb_pack.tol_cmd_upper = RTMONrecv.dat2.pls;
      break;
    case MON_TPY_TOL_CMD_LOWER:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading Tolerable command -\n";
      fb_pack.tol_cmd_lower = RTMONrecv.dat2.pls;
      break;
    case MON_TYP_RMS_CURR:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading RMS current\n";
      fb_pack.rms_curr = RTMONrecv.dat2.pls;
      break;
    case MON_TYP_FORCE_SENSOR_AT_POSE:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading Force sensor current position\n";
      fb_pack.fs_curr_pose = RTMONrecv.dat2.force;
      break;
    case MON_TYP_FORCE_SENSOR_AFTER_OFFSET:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading Force sensor original data after offset cancel\n";
      fb_pack.fs_after = RTMONrecv.dat2.force;
      break;
    case MON_TYP_FORCE_SENSOR_BEFORE_OFFSET:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading Force sensor original data before offset cancel\n";
      fb_pack.fs_after = RTMONrecv.dat2.force;
      break;
    case MON_TYP_POSE_CMD_FOR_FORCE_SENSOR_CORRECTION:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading Position command of the force sensor correction\n";
      fb_pack.pos_cmd_fs = RTMONrecv.dat2.pos;
      break;
    case MON_TYP_COL_PRESUMED_TORQUE:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading COL presumed torque\n";
      fb_pack.col_torque = RTMONrecv.dat2.pls;
      break;
    case MON_TYP_COL_THRESHOLD_UPPER:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading COL threshold +\n";
      fb_pack.col_thres_upper = RTMONrecv.dat2.pls;
      break;
    case MON_TYP_COL_THRESHOLD_LOWER:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading COL threshold -\n";
      fb_pack.col_thres_lower = RTMONrecv.dat2.pls;
      break;
    case MON_TYP_POSE_CMD_DEGREES:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading POSE command with angles in degrees\n";
      fb_pack.pos_cmd_deg = RTMONrecv.dat2.pos;
      break;
    case MON_TYP_JOINT_CMD_DEGREES:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading JOINT command with angles in degrees\n";
      fb_pack.jnt_cmd_deg = RTMONrecv.dat2.jnt;
      break;
    case MON_TYP_POSE_FB_DEGREES:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading POSE feedback with angles in degrees\n";
      fb_pack.pos_fb_deg = RTMONrecv.dat2.pos;
      break;
    case MON_TYP_JOINT_FB_DEGREES:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading JOINT feedback with angles in degrees\n";
      fb_pack.jnt_fb_deg = RTMONrecv.dat2.jnt;
      break;
    case MON_TYP_CURR_CMD_ARMS:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading Curent command in Ampere RMS\n";
      fb_pack.curr_cmd_arms = RTMONrecv.dat2.float8;
      break;
    case MON_TYP_CURR_FB_ARMS:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading Curent feedback in Ampere RMS\n";
      fb_pack.curr_fb_arms = RTMONrecv.dat2.float8;
      break;
    case MON_TYP_TOL_CMD_UPPER_ARMS:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading Tolerable command +\n";
      fb_pack.tol_cmd_upper_arms = RTMONrecv.dat2.float8;
      break;
    case MON_TYP_TOL_CMD_LOWER_ARMS:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading Tolerable command -\n";
      fb_pack.tol_cmd_lower_arms = RTMONrecv.dat2.float8;
      break;
    case MON_TYP_RMS_CURR_ARMS:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading RMS current";
      fb_pack.rms_curr_arms = RTMONrecv.dat2.float8;
      break;
    case MON_TYP_NULL:
      if (RT_API_DEBUG_MODE)
      {
        std::cout << "Reading NULL\n";
      }
      break;
    default:
      if (RT_API_DEBUG_MODE)
        std::cout << "Bad data type.¥n" << std::endl;
      break;
  }

  switch (RTMONrecv.RecvType3)
  {
    case MON_TYP_JOINT_CMD:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading JOINT command values\n";
      fb_pack.jnt_FB = RTMONrecv.dat3.jnt;
      break;
    case MON_TYP_FJOINT_CMD:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading filtered JOINT command values\n";
      fb_pack.jnt_FFB = RTMONrecv.dat3.jnt;
      break;
    case MON_TYP_JOINT_FB:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading JOINT encoder values\n";
      fb_pack.jnt_EFB = RTMONrecv.dat3.jnt;
      break;
    case MON_TYP_POSE_CMD:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading POSE command values\n";
      fb_pack.pos_FB = RTMONrecv.dat3.pos;
      break;
    case MON_TYP_FPOSE_CMD:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading filtered POSE command values\n";
      fb_pack.pos_EFB = RTMONrecv.dat3.pos;
      break;
    case MON_TYP_POSE_FB:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading POSE encoder values\n";
      fb_pack.pos_EFB = RTMONrecv.dat3.pos;
      break;
    case MON_TYP_PULSE_CMD:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading PULSE command values\n";
      fb_pack.pls_FB = RTMONrecv.dat3.pls;
      break;
    case MON_TYP_FPULSE_CMD:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading filtered PULSE command values\n";
      fb_pack.pls_FFB = RTMONrecv.dat3.pls;
      break;
    case MON_TYP_PULSE_FB:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading PULSE encoder values\n";
      fb_pack.pls_EFB = RTMONrecv.dat3.pls;
      break;
    case MON_TYP_CURR_CMD:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading CURRENT command values\n";
      fb_pack.curr_cmd = RTMONrecv.dat3.pls;
      break;
    case MON_TYP_CURR_FB:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading CURRENT feedback values\n";
      fb_pack.curr_FB = RTMONrecv.dat3.pls;
      break;
    case MON_TYP_ROBMON:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading Robot Information\n";
      fb_pack.robot_info = RTMONrecv.dat3.robmon;
      break;
    case MON_TYP_POS_DROOP:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading Position Droop\n";
      fb_pack.pos_droop = RTMONrecv.dat3.pls;
      break;
    case MON_TYP_SPEED_CMD:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading Speed Command\n";
      fb_pack.speed_cmd = RTMONrecv.dat3.pls;
      break;
    case MON_TYP_SPEED_FB:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading Speed Command\n";
      fb_pack.speed_fb = RTMONrecv.dat3.pls;
      break;
    case MON_TYP_AXIS_LOAD:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading Speed Feedback\n";
      fb_pack.axis_load = RTMONrecv.dat3.float8;
      break;
    case MON_TYP_ENC_TEMP:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading Encoder Temperature\n";
      fb_pack.enc_temp = RTMONrecv.dat3.pls;
      break;
    case MON_TYP_ENC_MIS:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading Encoder Miscount\n";
      fb_pack.enc_mis = RTMONrecv.dat3.pls;
      break;
    case MON_TYP_MOTOR_V:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading Motor Voltage\n";
      fb_pack.motor_v = RTMONrecv.dat3.pls;
      break;
    case MON_TYP_REGEN_LVL:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading Regeneration Level\n";
      fb_pack.regen_lvl = RTMONrecv.dat3.pls;
      break;
    case MON_TYP_TOL_CMD_UPPER:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading Tolerable command +\n";
      fb_pack.tol_cmd_upper = RTMONrecv.dat3.pls;
      break;
    case MON_TPY_TOL_CMD_LOWER:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading Tolerable command -\n";
      fb_pack.tol_cmd_lower = RTMONrecv.dat3.pls;
      break;
    case MON_TYP_RMS_CURR:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading RMS current\n";
      fb_pack.rms_curr = RTMONrecv.dat3.pls;
      break;
    case MON_TYP_FORCE_SENSOR_AT_POSE:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading Force sensor current position\n";
      fb_pack.fs_curr_pose = RTMONrecv.dat3.force;
      break;
    case MON_TYP_FORCE_SENSOR_AFTER_OFFSET:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading Force sensor original data after offset cancel\n";
      fb_pack.fs_after = RTMONrecv.dat3.force;
      break;
    case MON_TYP_FORCE_SENSOR_BEFORE_OFFSET:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading Force sensor original data before offset cancel\n";
      fb_pack.fs_after = RTMONrecv.dat3.force;
      break;
    case MON_TYP_POSE_CMD_FOR_FORCE_SENSOR_CORRECTION:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading Position command of the force sensor correction\n";
      fb_pack.pos_cmd_fs = RTMONrecv.dat3.pos;
      break;
    case MON_TYP_COL_PRESUMED_TORQUE:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading COL presumed torque\n";
      fb_pack.col_torque = RTMONrecv.dat3.pls;
      break;
    case MON_TYP_COL_THRESHOLD_UPPER:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading COL threshold +\n";
      fb_pack.col_thres_upper = RTMONrecv.dat3.pls;
      break;
    case MON_TYP_COL_THRESHOLD_LOWER:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading COL threshold -\n";
      fb_pack.col_thres_lower = RTMONrecv.dat3.pls;
      break;
    case MON_TYP_POSE_CMD_DEGREES:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading POSE command with angles in degrees\n";
      fb_pack.pos_cmd_deg = RTMONrecv.dat3.pos;
      break;
    case MON_TYP_JOINT_CMD_DEGREES:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading JOINT command with angles in degrees\n";
      fb_pack.jnt_cmd_deg = RTMONrecv.dat3.jnt;
      break;
    case MON_TYP_POSE_FB_DEGREES:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading POSE feedback with angles in degrees\n";
      fb_pack.pos_fb_deg = RTMONrecv.dat3.pos;
      break;
    case MON_TYP_JOINT_FB_DEGREES:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading JOINT feedback with angles in degrees\n";
      fb_pack.jnt_fb_deg = RTMONrecv.dat3.jnt;
      break;
    case MON_TYP_CURR_CMD_ARMS:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading Curent command in Ampere RMS\n";
      fb_pack.curr_cmd_arms = RTMONrecv.dat3.float8;
      break;
    case MON_TYP_CURR_FB_ARMS:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading Curent feedback in Ampere RMS\n";
      fb_pack.curr_fb_arms = RTMONrecv.dat3.float8;
      break;
    case MON_TYP_TOL_CMD_UPPER_ARMS:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading Tolerable command +\n";
      fb_pack.tol_cmd_upper_arms = RTMONrecv.dat3.float8;
      break;
    case MON_TYP_TOL_CMD_LOWER_ARMS:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading Tolerable command -\n";
      fb_pack.tol_cmd_lower_arms = RTMONrecv.dat3.float8;
      break;
    case MON_TYP_RMS_CURR_ARMS:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading RMS current";
      fb_pack.rms_curr_arms = RTMONrecv.dat3.float8;
      break;
    case MON_TYP_NULL:
      if (RT_API_DEBUG_MODE)
      {
        std::cout << "Reading NULL\n";
      }
      break;
    default:
      if (RT_API_DEBUG_MODE)
        std::cout << "Bad data type.¥n" << std::endl;
      break;
  }

  switch (RTMONrecv.RecvType4)
  {
    case MON_TYP_JOINT_CMD:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading JOINT command values\n";
      fb_pack.jnt_FB = RTMONrecv.dat4.jnt;
      break;
    case MON_TYP_FJOINT_CMD:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading filtered JOINT command values\n";
      fb_pack.jnt_FFB = RTMONrecv.dat4.jnt;
      break;
    case MON_TYP_JOINT_FB:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading JOINT encoder values\n";
      fb_pack.jnt_EFB = RTMONrecv.dat4.jnt;
      break;
    case MON_TYP_POSE_CMD:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading POSE command values\n";
      fb_pack.pos_FB = RTMONrecv.dat4.pos;
      break;
    case MON_TYP_FPOSE_CMD:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading filtered POSE command values\n";
      fb_pack.pos_EFB = RTMONrecv.dat4.pos;
      break;
    case MON_TYP_POSE_FB:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading POSE encoder values\n";
      fb_pack.pos_EFB = RTMONrecv.dat4.pos;
      break;
    case MON_TYP_PULSE_CMD:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading PULSE command values\n";
      fb_pack.pls_FB = RTMONrecv.dat4.pls;
      break;
    case MON_TYP_FPULSE_CMD:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading filtered PULSE command values\n";
      fb_pack.pls_FFB = RTMONrecv.dat4.pls;
      break;
    case MON_TYP_PULSE_FB:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading PULSE encoder values\n";
      fb_pack.pls_EFB = RTMONrecv.dat4.pls;
      break;
    case MON_TYP_CURR_CMD:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading CURRENT command values\n";
      fb_pack.curr_cmd = RTMONrecv.dat4.pls;
      break;
    case MON_TYP_CURR_FB:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading CURRENT feedback values\n";
      fb_pack.curr_FB = RTMONrecv.dat4.pls;
      break;
    case MON_TYP_ROBMON:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading Robot Information\n";
      fb_pack.robot_info = RTMONrecv.dat4.robmon;
      break;
    case MON_TYP_POS_DROOP:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading Position Droop\n";
      fb_pack.pos_droop = RTMONrecv.dat4.pls;
      break;
    case MON_TYP_SPEED_CMD:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading Speed Command\n";
      fb_pack.speed_cmd = RTMONrecv.dat4.pls;
      break;
    case MON_TYP_SPEED_FB:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading Speed Command\n";
      fb_pack.speed_fb = RTMONrecv.dat4.pls;
      break;
    case MON_TYP_AXIS_LOAD:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading Speed Feedback\n";
      fb_pack.axis_load = RTMONrecv.dat4.float8;
      break;
    case MON_TYP_ENC_TEMP:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading Encoder Temperature\n";
      fb_pack.enc_temp = RTMONrecv.dat4.pls;
      break;
    case MON_TYP_ENC_MIS:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading Encoder Miscount\n";
      fb_pack.enc_mis = RTMONrecv.dat4.pls;
      break;
    case MON_TYP_MOTOR_V:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading Motor Voltage\n";
      fb_pack.motor_v = RTMONrecv.dat4.pls;
      break;
    case MON_TYP_REGEN_LVL:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading Regeneration Level\n";
      fb_pack.regen_lvl = RTMONrecv.dat4.pls;
      break;
    case MON_TYP_TOL_CMD_UPPER:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading Tolerable command +\n";
      fb_pack.tol_cmd_upper = RTMONrecv.dat4.pls;
      break;
    case MON_TPY_TOL_CMD_LOWER:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading Tolerable command -\n";
      fb_pack.tol_cmd_lower = RTMONrecv.dat4.pls;
      break;
    case MON_TYP_RMS_CURR:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading RMS current\n";
      fb_pack.rms_curr = RTMONrecv.dat4.pls;
      break;
    case MON_TYP_FORCE_SENSOR_AT_POSE:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading Force sensor current position\n";
      fb_pack.fs_curr_pose = RTMONrecv.dat4.force;
      break;
    case MON_TYP_FORCE_SENSOR_AFTER_OFFSET:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading Force sensor original data after offset cancel\n";
      fb_pack.fs_after = RTMONrecv.dat4.force;
      break;
    case MON_TYP_FORCE_SENSOR_BEFORE_OFFSET:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading Force sensor original data before offset cancel\n";
      fb_pack.fs_after = RTMONrecv.dat4.force;
      break;
    case MON_TYP_POSE_CMD_FOR_FORCE_SENSOR_CORRECTION:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading Position command of the force sensor correction\n";
      fb_pack.pos_cmd_fs = RTMONrecv.dat4.pos;
      break;
    case MON_TYP_COL_PRESUMED_TORQUE:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading COL presumed torque\n";
      fb_pack.col_torque = RTMONrecv.dat4.pls;
      break;
    case MON_TYP_COL_THRESHOLD_UPPER:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading COL threshold +\n";
      fb_pack.col_thres_upper = RTMONrecv.dat4.pls;
      break;
    case MON_TYP_COL_THRESHOLD_LOWER:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading COL threshold -\n";
      fb_pack.col_thres_lower = RTMONrecv.dat4.pls;
      break;
    case MON_TYP_POSE_CMD_DEGREES:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading POSE command with angles in degrees\n";
      fb_pack.pos_cmd_deg = RTMONrecv.dat4.pos;
      break;
    case MON_TYP_JOINT_CMD_DEGREES:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading JOINT command with angles in degrees\n";
      fb_pack.jnt_cmd_deg = RTMONrecv.dat4.jnt;
      break;
    case MON_TYP_POSE_FB_DEGREES:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading POSE feedback with angles in degrees\n";
      fb_pack.pos_fb_deg = RTMONrecv.dat4.pos;
      break;
    case MON_TYP_JOINT_FB_DEGREES:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading JOINT feedback with angles in degrees\n";
      fb_pack.jnt_fb_deg = RTMONrecv.dat4.jnt;
      break;
    case MON_TYP_CURR_CMD_ARMS:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading Curent command in Ampere RMS\n";
      fb_pack.curr_cmd_arms = RTMONrecv.dat4.float8;
      break;
    case MON_TYP_CURR_FB_ARMS:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading Curent feedback in Ampere RMS\n";
      fb_pack.curr_fb_arms = RTMONrecv.dat4.float8;
      break;
    case MON_TYP_TOL_CMD_UPPER_ARMS:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading Tolerable command +\n";
      fb_pack.tol_cmd_upper_arms = RTMONrecv.dat4.float8;
      break;
    case MON_TYP_TOL_CMD_LOWER_ARMS:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading Tolerable command -\n";
      fb_pack.tol_cmd_lower_arms = RTMONrecv.dat4.float8;
      break;
    case MON_TYP_RMS_CURR_ARMS:
      if (RT_API_DEBUG_MODE)
        std::cout << "Reading RMS current";
      fb_pack.rms_curr_arms = RTMONrecv.dat4.float8;
      break;
    case MON_TYP_NULL:
      if (RT_API_DEBUG_MODE)
      {
        std::cout << "Reading NULL\n";
      }
      break;
    default:
      if (RT_API_DEBUG_MODE)
        std::cout << "Bad data type.¥n" << std::endl;
      break;
  }
  return 0;
}
int rtmon::recv_packet_()
{
  memset(recvText, 0, MAXBUFLEN);
  int numrcv;
  fd_set SockSet;
  FD_ZERO(&SockSet);                      // SockSet initialization
  FD_SET(robot_ip.destSocket, &SockSet);  // SockSet registration
  int status;
  memset(console_msg, 0, sizeof(console_msg));
  if (RT_API_DEBUG_MODE)
  {
    memset(console_msg, 0, sizeof(console_msg));
#if __linux__
    sprintf(
#endif
#if _WIN32
    sprintf_s(
#endif
    console_msg, "Receiving from: destAddr=%d, destSocket=%d, dst_ip_address = %s, port= %d.\n",
            robot_ip.destAddr, (int)robot_ip.destSocket, robot_ip.dst_ip_address.c_str(), robot_ip.recvport);
    std::cout << console_msg;
  }

  sTimeOut.tv_sec = 0;
  sTimeOut.tv_usec = (long)(2 * period * 1000);

#ifdef _WIN32
  status = select(0, &SockSet, (fd_set*)NULL, (fd_set*)NULL, &sTimeOut);
#endif
#ifdef __linux__
  status = select(robot_ip.destSocket + 4, &SockSet, (fd_set*)NULL, (fd_set*)NULL, &sTimeOut);
#endif
  if (status < 0)
  {
    if (RT_API_DEBUG_MODE)
    {
      std::cerr << "ERROR: Select Receive Socket FAILED.\t" << std::endl;
#ifdef _WIN32
      std::cerr << "WSAGetLastError: " << WSAGetLastError() << std::endl;
#endif
#ifdef __linux__
      std::cerr << "ERRNO: " << strerror(errno) << std::endl;
#endif
    }
    packet_recv_lost++;
    return -1;
  }
  if ((status > 0) && (FD_ISSET(robot_ip.destSocket, &SockSet) != 0))
  {
    numrcv = recvfrom(robot_ip.destSocket, recvText, MAXBUFLEN, 0, NULL, NULL);
    if (numrcv < 0)
    {
      if (RT_API_DEBUG_MODE)
        std::cerr << "ERROR: Receive FAILED.\n";
      packet_recv_lost++;
      return -1;
    }
    if (RT_API_DEBUG_MODE)
      std::cout << "Receive Packet Size: " << numrcv << "\n";
    return 0;
  }
  else
  {
    if (RT_API_DEBUG_MODE)
      std::cerr << "ERROR: Condition [status>0)] and [FD_ISSET()!=0)] FAILED.\n";
    if (status == 0)
    {
      if (RT_API_DEBUG_MODE)
        std::cerr << "Time limit expired.\tstatus:" << status << std::endl;
    }
    packet_recv_lost++;
    return -1;
  }
}

int rtmon::print_monitored_feedback()
{
  memset(console_msg, 0, sizeof(console_msg));
#if __linux__
          sprintf(
#endif
#if _WIN32
          sprintf_s(
#endif
          console_msg,"Input Address: %d\nInput Data: 0x%x\nOutput Address: %d\nOutput Data: 0x%x\n",fb_pack.INBitTop,fb_pack.INBitData,fb_pack.OUTBitTop,fb_pack.OUTBitData);
          std::cout<<console_msg;
          for (int i = 0; i < 4; i++)
          {
    memset(console_msg, 0, sizeof(console_msg));
    switch (type_mon[i])
    {
      case MON_TYP_JOINT_CMD:
#if __linux__
                  sprintf(
#endif
#if _WIN32
                  sprintf_s(
#endif
                      console_msg, "JOINT Command Feedback: j1= %f, j2= %f, j3= %f, j4= %f, j5= %f, j6= %f, j7= %f, j8= %f\n",
                            fb_pack.jnt_FB.j1,
                            fb_pack.jnt_FB.j2,
                            fb_pack.jnt_FB.j3,
                            fb_pack.jnt_FB.j4,
                            fb_pack.jnt_FB.j5,
                            fb_pack.jnt_FB.j6,
                            fb_pack.jnt_FB.j7,
                            fb_pack.jnt_FB.j8);
  
                  std::cout << console_msg;
                  break;
              case MON_TYP_FJOINT_CMD:
#if __linux__
                  sprintf(
#endif
#if _WIN32
                  sprintf_s(
#endif
                      console_msg, "JOINT filtered Command Feedback: j1= %f, j2= %f, j3= %f, j4= %f, j5= %f, j6= %f, j7= %f, j8= %f\n",
                            fb_pack.jnt_FFB.j1,
                            fb_pack.jnt_FFB.j2,
                            fb_pack.jnt_FFB.j3,
                            fb_pack.jnt_FFB.j4,
                            fb_pack.jnt_FFB.j5,
                            fb_pack.jnt_FFB.j6,
                            fb_pack.jnt_FFB.j7,
                            fb_pack.jnt_FFB.j8);
  
                  std::cout << console_msg;
                  break;
              case MON_TYP_JOINT_FB:
#if __linux__
                  sprintf(
#endif
#if _WIN32
                  sprintf_s(
#endif
                      console_msg, "JOINT Encoder Feedback: j1= %f, j2= %f, j3= %f, j4= %f, j5= %f, j6= %f, j7= %f, j8= %f\n",
                            fb_pack.jnt_EFB.j1,
                            fb_pack.jnt_EFB.j2,
                            fb_pack.jnt_EFB.j3,
                            fb_pack.jnt_EFB.j4,
                            fb_pack.jnt_EFB.j5,
                            fb_pack.jnt_EFB.j6,
                            fb_pack.jnt_EFB.j7,
                            fb_pack.jnt_EFB.j8);
                  std::cout << console_msg;
                  break;
              case MON_TYP_POSE_CMD:
#if __linux__
                  sprintf(
#endif
#if _WIN32
                  sprintf_s(
#endif
                      console_msg, "POSE Commmand Feedback: x= %f, y= %f, z= %f, a= %f, b= %f, c= %f, l1= %f, l2= %f, sflg1= %d, sflg2= %d\n",
                            fb_pack.pos_FB.x,
                            fb_pack.pos_FB.y,
                            fb_pack.pos_FB.z,
                            fb_pack.pos_FB.a,
                            fb_pack.pos_FB.b,
                            fb_pack.pos_FB.c,
                            fb_pack.pos_FB.l1,
                            fb_pack.pos_FB.l2,
                            fb_pack.pos_FB.sflg1,
                            fb_pack.pos_FB.sflg2);
  
                  std::cout << console_msg;
                  break;
              case MON_TYP_FPOSE_CMD:
#if __linux__
                  sprintf(
#endif
#if _WIN32
                  sprintf_s(
#endif
  
                      console_msg, "POSE Filtered Command Feedback: x= %f, y= %f, z= %f, a= %f, b= %f, c= %f, l1= %f, l2= %f, sflg1= %d, sflg2= %d\n",
                            fb_pack.pos_FFB.x,
                            fb_pack.pos_FFB.y,
                            fb_pack.pos_FFB.z,
                            fb_pack.pos_FFB.a,
                            fb_pack.pos_FFB.b,
                            fb_pack.pos_FFB.c,
                            fb_pack.pos_FFB.l1,
                            fb_pack.pos_FFB.l2,
                            fb_pack.pos_FFB.sflg1,
                            fb_pack.pos_FFB.sflg2);
                  std::cout << console_msg;
                  break;
              case MON_TYP_POSE_FB:
#if __linux__
                  sprintf(
#endif
#if _WIN32
                  sprintf_s(
#endif
  
                      console_msg, "POSE Encoder Feedback: x= %f, y= %f, z= %f, a= %f, b= %f, c= %f, l1= %f, l2= %f, sflg1= %d, sflg2= %d\n",
                            fb_pack.pos_EFB.x,
                            fb_pack.pos_EFB.y,
                            fb_pack.pos_EFB.z,
                            fb_pack.pos_EFB.a,
                            fb_pack.pos_EFB.b,
                            fb_pack.pos_EFB.c,
                            fb_pack.pos_EFB.l1,
                            fb_pack.pos_EFB.l2,
                            fb_pack.pos_EFB.sflg1,
                            fb_pack.pos_EFB.sflg2);
                  std::cout << console_msg;
                  break;
              case MON_TYP_PULSE_CMD:
#if __linux__
                  sprintf(
#endif
#if _WIN32
                  sprintf_s(
#endif
                      console_msg, "PULSE Command Feedback: m1= %d, m2= %d, m3= %d, m4= %d, m5= %d, m6= %d, m7= %d, m8= %d\n",
                            fb_pack.pls_FB.m1,
                            fb_pack.pls_FB.m2,
                            fb_pack.pls_FB.m3,
                            fb_pack.pls_FB.m4,
                            fb_pack.pls_FB.m5,
                            fb_pack.pls_FB.m6,
                            fb_pack.pls_FB.m7,
                            fb_pack.pls_FB.m8);
                  std::cout << console_msg;
                  break;
              case MON_TYP_FPULSE_CMD:
#if __linux__
                  sprintf(
#endif
#if _WIN32
                  sprintf_s(
#endif
                      console_msg, "PULSE Filtered Command Feedback: m1= %d, m2= %d, m3= %d, m4= %d, m5= %d, m6= %d, m7= %d, m8= %d\n",
                            fb_pack.pls_FFB.m1,
                            fb_pack.pls_FFB.m2,
                            fb_pack.pls_FFB.m3,
                            fb_pack.pls_FFB.m4,
                            fb_pack.pls_FFB.m5,
                            fb_pack.pls_FFB.m6,
                            fb_pack.pls_FFB.m7,
                            fb_pack.pls_FFB.m8);
                  std::cout << console_msg;
                  break;
              case MON_TYP_PULSE_FB:
#if __linux__
                  sprintf(
#endif
#if _WIN32
                  sprintf_s(
#endif
                      console_msg, "PULSE Encoder Feedback: m1= %d, m2= %d, m3= %d, m4= %d, m5= %d, m6= %d, m7= %d, m8= %d\n",
                            fb_pack.pls_EFB.m1,
                            fb_pack.pls_EFB.m2,
                            fb_pack.pls_EFB.m3,
                            fb_pack.pls_EFB.m4,
                            fb_pack.pls_EFB.m5,
                            fb_pack.pls_EFB.m6,
                            fb_pack.pls_EFB.m7,
                            fb_pack.pls_EFB.m8);
                  std::cout << console_msg;
                  break;
              case MON_TYP_CURR_CMD:
#if __linux__
                  sprintf(
#endif
#if _WIN32
                  sprintf_s(
#endif
                      console_msg, "CURRENT Command Feedback: m1= %d, m2= %d, m3= %d, m4= %d, m5= %d, m6= %d, m7= %d, m8= %d\n",
                            fb_pack.curr_cmd.m1,
                            fb_pack.curr_cmd.m2,
                            fb_pack.curr_cmd.m3,
                            fb_pack.curr_cmd.m4,
                            fb_pack.curr_cmd.m5,
                            fb_pack.curr_cmd.m6,
                            fb_pack.curr_cmd.m7,
                            fb_pack.curr_cmd.m8);
                  std::cout << console_msg;
                  break;
              case MON_TYP_CURR_FB:
#if __linux__
                  sprintf(
#endif
#if _WIN32
                  sprintf_s(
#endif
                      console_msg, "CURRENT Actual Feedback: m1= %d, m2= %d, m3= %d, m4= %d, m5= %d, m6= %d, m7= %d, m8= %d\n",
                            fb_pack.curr_FB.m1,
                            fb_pack.curr_FB.m2,
                            fb_pack.curr_FB.m3,
                            fb_pack.curr_FB.m4,
                            fb_pack.curr_FB.m5,
                            fb_pack.curr_FB.m6,
                            fb_pack.curr_FB.m7,
                            fb_pack.curr_FB.m8);
                  std::cout << console_msg;
                  break;
              case MON_TYP_ROBMON:
#if __linux__
                  sprintf(
#endif
#if _WIN32
                  sprintf_s(
#endif
                      console_msg, "Robot Information: Tool Centre Point Speed Feedback= %f, Tool Centre Point Speed Feedback Command= %f, Remaining Distance Feedback= %f, Remaining Distance Command= %f, Gap to Command= %f, Trans Factor= %d, Acceleration State= %d, Program Step Number= %d, Program Name= %s, Controller Temperature= %fC, Operation Cycle Count= %d\n",
                            fb_pack.robot_info.tcp_fb,
                            fb_pack.robot_info.tcp_cmd,
                            fb_pack.robot_info.rem_dist_fb,
                            fb_pack.robot_info.rem_dist_cmd,
                            fb_pack.robot_info.gap_cmd_fb,
                            fb_pack.robot_info.trans_factor,
                            fb_pack.robot_info.acc_state,
                            fb_pack.robot_info.step_num,
                            fb_pack.robot_info.program_name,
                            (float)fb_pack.robot_info.cont_temp/10.0,
                            fb_pack.robot_info.mon_count
                          );
                  std::cout << console_msg;
                  break;
              case MON_TYP_POS_DROOP:
#if __linux__
                  sprintf(
#endif
#if _WIN32
                  sprintf_s(
#endif
                      console_msg, "Position Drop: m1= %d, m2= %d, m3= %d, m4= %d, m5= %d, m6= %d, m7= %d, m8= %d\n",
                            fb_pack.pos_droop.m1,
                            fb_pack.pos_droop.m2,
                            fb_pack.pos_droop.m3,
                            fb_pack.pos_droop.m4,
                            fb_pack.pos_droop.m5,
                            fb_pack.pos_droop.m6,
                            fb_pack.pos_droop.m7,
                            fb_pack.pos_droop.m8);
                      std::cout << console_msg;
                    break;
              case MON_TYP_SPEED_CMD:
#if __linux__
                    sprintf(
#endif
#if _WIN32
                  sprintf_s(
#endif
                      console_msg, "Speed Command (rpm): m1= %d, m2= %d, m3= %d, m4= %d, m5= %d, m6= %d, m7= %d, m8= %d\n",
                            fb_pack.speed_cmd.m1,
                            fb_pack.speed_cmd.m2,
                            fb_pack.speed_cmd.m3,
                            fb_pack.speed_cmd.m4,
                            fb_pack.speed_cmd.m5,
                            fb_pack.speed_cmd.m6,
                            fb_pack.speed_cmd.m7,
                            fb_pack.speed_cmd.m8);
                      std::cout << console_msg;
                    break;
                    case MON_TYP_SPEED_FB:
#if __linux__
                  sprintf(
#endif
#if _WIN32
                sprintf_s(
#endif
                      console_msg, "Speed Command (rpm): m1= %d, m2= %d, m3= %d, m4= %d, m5= %d, m6= %d, m7= %d, m8= %d\n",
                            fb_pack.speed_fb.m1,
                            fb_pack.speed_fb.m2,
                            fb_pack.speed_fb.m3,
                            fb_pack.speed_fb.m4,
                            fb_pack.speed_fb.m5,
                            fb_pack.speed_fb.m6,
                            fb_pack.speed_fb.m7,
                            fb_pack.speed_fb.m8);
                      std::cout << console_msg;
                    break;
                    case MON_TYP_AXIS_LOAD:
#if __linux__
                sprintf(
#endif
#if _WIN32
                sprintf_s(
#endif
                      console_msg, "Axis Load: f1= %f, f2= %f, f3= %f, f4= %f, f5= %f, f6= %f, f7= %f, f8= %f\n",
                            fb_pack.axis_load.f1,
                            fb_pack.axis_load.f2,
                            fb_pack.axis_load.f3,
                            fb_pack.axis_load.f4,
                            fb_pack.axis_load.f5,
                            fb_pack.axis_load.f6,
                            fb_pack.axis_load.f7,
                            fb_pack.axis_load.f8);
                      std::cout << console_msg;
                    break;
                    case MON_TYP_ENC_TEMP:
#if __linux__
                sprintf(
#endif
#if _WIN32
                sprintf_s(
#endif
                      console_msg, "Motor Encoder Temperature: m1= %dC, m2= %dC, m3= %dC, m4= %dC, m5= %dC, m6= %dC, m7= %dC, m8= %dC\n",
                            fb_pack.enc_temp.m1,
                            fb_pack.enc_temp.m2,
                            fb_pack.enc_temp.m3,
                            fb_pack.enc_temp.m4,
                            fb_pack.enc_temp.m5,
                            fb_pack.enc_temp.m6,
                            fb_pack.enc_temp.m7,
                            fb_pack.enc_temp.m8);
                      std::cout << console_msg;
                    break;
                    case MON_TYP_ENC_MIS:
#if __linux__
                sprintf(
#endif
#if _WIN32
                sprintf_s(
#endif
                      console_msg, "Motor Miscount: m1= %d, m2= %d, m3= %d, m4= %d, m5= %d, m6= %d, m7= %d, m8= %d\n",
                            fb_pack.enc_mis.m1,
                            fb_pack.enc_mis.m2,
                            fb_pack.enc_mis.m3,
                            fb_pack.enc_mis.m4,
                            fb_pack.enc_mis.m5,
                            fb_pack.enc_mis.m6,
                            fb_pack.enc_mis.m7,
                            fb_pack.enc_mis.m8);
                      std::cout << console_msg;
                    break;
                    case MON_TYP_MOTOR_V:
#if __linux__
                sprintf(
#endif
#if _WIN32
                sprintf_s(
#endif
                      console_msg, "Motor Voltage: m1= %d, m2= %d, m3= %d, m4= %d, m5= %d, m6= %d, m7= %d, m8= %d\n",
                            fb_pack.motor_v.m1,
                            fb_pack.motor_v.m2,
                            fb_pack.motor_v.m3,
                            fb_pack.motor_v.m4,
                            fb_pack.motor_v.m5,
                            fb_pack.motor_v.m6,
                            fb_pack.motor_v.m7,
                            fb_pack.motor_v.m8);
                      std::cout << console_msg;
                    break;
                    case MON_TYP_REGEN_LVL:
#if __linux__
                sprintf(
#endif
#if _WIN32
                sprintf_s(
#endif
                      console_msg, "Regenertion Level: m1= %d, m2= %d, m3= %d, m4= %d, m5= %d, m6= %d, m7= %d, m8= %d\n",
                            fb_pack.regen_lvl.m1,
                            fb_pack.regen_lvl.m2,
                            fb_pack.regen_lvl.m3,
                            fb_pack.regen_lvl.m4,
                            fb_pack.regen_lvl.m5,
                            fb_pack.regen_lvl.m6,
                            fb_pack.regen_lvl.m7,
                            fb_pack.regen_lvl.m8);
                      std::cout << console_msg;
                    break;
                    case MON_TYP_TOL_CMD_UPPER:
#if __linux__
                sprintf(
#endif
#if _WIN32
                sprintf_s(
#endif
                      console_msg, "Tolerable Command +: m1= %d, m2= %d, m3= %d, m4= %d, m5= %d, m6= %d, m7= %d, m8= %d\n",
                            fb_pack.tol_cmd_upper.m1,
                            fb_pack.tol_cmd_upper.m2,
                            fb_pack.tol_cmd_upper.m3,
                            fb_pack.tol_cmd_upper.m4,
                            fb_pack.tol_cmd_upper.m5,
                            fb_pack.tol_cmd_upper.m6,
                            fb_pack.tol_cmd_upper.m7,
                            fb_pack.tol_cmd_upper.m8);
                      std::cout << console_msg;
                    break;
                    case MON_TPY_TOL_CMD_LOWER:
#if __linux__
                sprintf(
#endif
#if _WIN32
                sprintf_s(
#endif
                      console_msg, "Tolerable Command -: m1= %d, m2= %d, m3= %d, m4= %d, m5= %d, m6= %d, m7= %d, m8= %d\n",
                            fb_pack.tol_cmd_lower.m1,
                            fb_pack.tol_cmd_lower.m2,
                            fb_pack.tol_cmd_lower.m3,
                            fb_pack.tol_cmd_lower.m4,
                            fb_pack.tol_cmd_lower.m5,
                            fb_pack.tol_cmd_lower.m6,
                            fb_pack.tol_cmd_lower.m7,
                            fb_pack.tol_cmd_lower.m8);
                      std::cout << console_msg;
                    break;
                    case MON_TYP_RMS_CURR:
#if __linux__
                sprintf(
#endif
#if _WIN32
                sprintf_s(
#endif
                      console_msg, "RMS Current: m1= %d, m2= %d, m3= %d, m4= %d, m5= %d, m6= %d, m7= %d, m8= %d\n",
                            fb_pack.rms_curr.m1,
                            fb_pack.rms_curr.m2,
                            fb_pack.rms_curr.m3,
                            fb_pack.rms_curr.m4,
                            fb_pack.rms_curr.m5,
                            fb_pack.rms_curr.m6,
                            fb_pack.rms_curr.m7,
                            fb_pack.rms_curr.m8);
                      std::cout << console_msg;
                    break;
                    case MON_TYP_FORCE_SENSOR_AT_POSE:
#if __linux__
                sprintf(
#endif
#if _WIN32
                sprintf_s(
#endif
                      console_msg, "Force Sensor Data at current position: fx= %f, fy= %f, fz= %f, mx= %f, my= %f, mz= %f\n",
                            fb_pack.fs_curr_pose.fx,
                            fb_pack.fs_curr_pose.fy,
                            fb_pack.fs_curr_pose.fz,
                            fb_pack.fs_curr_pose.mx,
                            fb_pack.fs_curr_pose.my,
                            fb_pack.fs_curr_pose.mz);
                      std::cout << console_msg;
                    break;
                    case MON_TYP_FORCE_SENSOR_AFTER_OFFSET:
#if __linux__
                sprintf(
#endif
#if _WIN32
                sprintf_s(
#endif
                      console_msg, "Force Sensor Data after offset: fx= %f, fy= %f, fz= %f, mx= %f, my= %f, mz= %f\n",
                            fb_pack.fs_after.fx,
                            fb_pack.fs_after.fy,
                            fb_pack.fs_after.fz,
                            fb_pack.fs_after.mx,
                            fb_pack.fs_after.my,
                            fb_pack.fs_after.mz);
                      std::cout << console_msg;
                    break;
                    case MON_TYP_FORCE_SENSOR_BEFORE_OFFSET:
#if __linux__
                sprintf(
#endif
#if _WIN32
                sprintf_s(
#endif
                      console_msg, "Force Sensor Data before offset: fx= %f, fy= %f, fz= %f, mx= %f, my= %f, mz= %f\n",
                            fb_pack.fs_before.fx,
                            fb_pack.fs_before.fy,
                            fb_pack.fs_before.fz,
                            fb_pack.fs_before.mx,
                            fb_pack.fs_before.my,
                            fb_pack.fs_before.mz);
                      std::cout << console_msg;
                    break;
                    case MON_TYP_POSE_CMD_FOR_FORCE_SENSOR_CORRECTION:
#if __linux__
                sprintf(
#endif
#if _WIN32
                sprintf_s(
#endif
                      console_msg, "Position Command for Force Sensor Correction: x= %f, y= %f, z= %f, a= %f, b= %f, c= %f, l1= %f, l2= %f, sflg1= %d, sflg2= %d\n",
                            fb_pack.pos_cmd_fs.x,
                            fb_pack.pos_cmd_fs.y,
                            fb_pack.pos_cmd_fs.z,
                            fb_pack.pos_cmd_fs.a,
                            fb_pack.pos_cmd_fs.b,
                            fb_pack.pos_cmd_fs.c,
                            fb_pack.pos_cmd_fs.l1,
                            fb_pack.pos_cmd_fs.l2,
                            fb_pack.pos_cmd_fs.sflg1,
                            fb_pack.pos_cmd_fs.sflg2);
                      std::cout << console_msg;
                    break;
                    case MON_TYP_COL_PRESUMED_TORQUE:
#if __linux__
                sprintf(
#endif
#if _WIN32
                sprintf_s(
#endif
                      console_msg, "COL Presumed Torque: m1= %d, m2= %d, m3= %d, m4= %d, m5= %d, m6= %d, m7= %d, m8= %d\n",
                            fb_pack.col_torque.m1,
                            fb_pack.col_torque.m2,
                            fb_pack.col_torque.m3,
                            fb_pack.col_torque.m4,
                            fb_pack.col_torque.m5,
                            fb_pack.col_torque.m6,
                            fb_pack.col_torque.m7,
                            fb_pack.col_torque.m8);
                      std::cout << console_msg;
                    break;
                    case MON_TYP_COL_THRESHOLD_UPPER:
#if __linux__
                sprintf(
#endif
#if _WIN32
                sprintf_s(
#endif
                      console_msg, "COL Threshold +: m1= %d, m2= %d, m3= %d, m4= %d, m5= %d, m6= %d, m7= %d, m8= %d\n",
                            fb_pack.col_thres_upper.m1,
                            fb_pack.col_thres_upper.m2,
                            fb_pack.col_thres_upper.m3,
                            fb_pack.col_thres_upper.m4,
                            fb_pack.col_thres_upper.m5,
                            fb_pack.col_thres_upper.m6,
                            fb_pack.col_thres_upper.m7,
                            fb_pack.col_thres_upper.m8);
                      std::cout << console_msg;
                    break;
                    case MON_TYP_COL_THRESHOLD_LOWER:
#if __linux__
                sprintf(
#endif
#if _WIN32
                sprintf_s(
#endif
                      console_msg, "COL Threshold -: m1= %d, m2= %d, m3= %d, m4= %d, m5= %d, m6= %d, m7= %d, m8= %d\n",
                            fb_pack.col_thres_lower.m1,
                            fb_pack.col_thres_lower.m2,
                            fb_pack.col_thres_lower.m3,
                            fb_pack.col_thres_lower.m4,
                            fb_pack.col_thres_lower.m5,
                            fb_pack.col_thres_lower.m6,
                            fb_pack.col_thres_lower.m7,
                            fb_pack.col_thres_lower.m8);
                      std::cout << console_msg;
                    break;
                    case MON_TYP_POSE_CMD_DEGREES:
#if __linux__
                sprintf(
#endif
#if _WIN32
                sprintf_s(
#endif
                      console_msg, "POSE Command in degrees: x= %f, y= %f, z= %f, a= %f, b= %f, c= %f, l1= %f, l2= %f, sflg1= %d, sflg2= %d\n",
                            fb_pack.pos_cmd_deg.x,
                            fb_pack.pos_cmd_deg.y,
                            fb_pack.pos_cmd_deg.z,
                            fb_pack.pos_cmd_deg.a,
                            fb_pack.pos_cmd_deg.b,
                            fb_pack.pos_cmd_deg.c,
                            fb_pack.pos_cmd_deg.l1,
                            fb_pack.pos_cmd_deg.l2,
                            fb_pack.pos_cmd_deg.sflg1,
                            fb_pack.pos_cmd_deg.sflg2);
                      std::cout << console_msg;
                    break;
                    case MON_TYP_JOINT_CMD_DEGREES:
#if __linux__
                sprintf(
#endif
#if _WIN32
                sprintf_s(
#endif
                      console_msg, "JOINT Command in degrees: j1= %f, j2= %f, j3= %f, j4= %f, j5= %f, j6= %f, j7= %f, j8= %f\n",
                            fb_pack.jnt_cmd_deg.j1,
                            fb_pack.jnt_cmd_deg.j2,
                            fb_pack.jnt_cmd_deg.j3,
                            fb_pack.jnt_cmd_deg.j4,
                            fb_pack.jnt_cmd_deg.j5,
                            fb_pack.jnt_cmd_deg.j6,
                            fb_pack.jnt_cmd_deg.j7,
                            fb_pack.jnt_cmd_deg.j8);
                      std::cout << console_msg;
                    break;
                    case MON_TYP_POSE_FB_DEGREES:
#if __linux__
                sprintf(
#endif
#if _WIN32
                sprintf_s(
#endif
                      console_msg, "POSE Feedback in degrees: x= %f, y= %f, z= %f, a= %f, b= %f, c= %f, l1= %f, l2= %f, sflg1= %d, sflg2= %d\n",
                            fb_pack.pos_fb_deg.x,
                            fb_pack.pos_fb_deg.y,
                            fb_pack.pos_fb_deg.z,
                            fb_pack.pos_fb_deg.a,
                            fb_pack.pos_fb_deg.b,
                            fb_pack.pos_fb_deg.c,
                            fb_pack.pos_fb_deg.l1,
                            fb_pack.pos_fb_deg.l2,
                            fb_pack.pos_fb_deg.sflg1,
                            fb_pack.pos_fb_deg.sflg2);
                      std::cout << console_msg;
                    break;
                    case MON_TYP_JOINT_FB_DEGREES:
#if __linux__
                sprintf(
#endif
#if _WIN32
                sprintf_s(
#endif
                      console_msg, "JOINT Feedback in degrees: j1= %f, j2= %f, j3= %f, j4= %f, j5= %f, j6= %f, j7= %f, j8= %f\n",
                            fb_pack.jnt_fb_deg.j1,
                            fb_pack.jnt_fb_deg.j2,
                            fb_pack.jnt_fb_deg.j3,
                            fb_pack.jnt_fb_deg.j4,
                            fb_pack.jnt_fb_deg.j5,
                            fb_pack.jnt_fb_deg.j6,
                            fb_pack.jnt_fb_deg.j7,
                            fb_pack.jnt_fb_deg.j8);
                      std::cout << console_msg;
                    break;
                    case MON_TYP_CURR_CMD_ARMS:
#if __linux__
                sprintf(
#endif
#if _WIN32
                sprintf_s(
#endif
                      console_msg, "Current Command in Ampere RMS: j1= %f, j2= %f, j3= %f, j4= %f, j5= %f, j6= %f, j7= %f, j8= %f\n",
                            fb_pack.curr_cmd_arms.f1,
                            fb_pack.curr_cmd_arms.f2,
                            fb_pack.curr_cmd_arms.f3,
                            fb_pack.curr_cmd_arms.f4,
                            fb_pack.curr_cmd_arms.f5,
                            fb_pack.curr_cmd_arms.f6,
                            fb_pack.curr_cmd_arms.f7,
                            fb_pack.curr_cmd_arms.f8);
                      std::cout << console_msg;
                    break;
                    case MON_TYP_CURR_FB_ARMS:
#if __linux__
                sprintf(
#endif
#if _WIN32
                sprintf_s(
#endif
                      console_msg, "Current Feedback in Ampere RMS: j1= %f, j2= %f, j3= %f, j4= %f, j5= %f, j6= %f, j7= %f, j8= %f\n",
                            fb_pack.curr_fb_arms.f1,
                            fb_pack.curr_fb_arms.f2,
                            fb_pack.curr_fb_arms.f3,
                            fb_pack.curr_fb_arms.f4,
                            fb_pack.curr_fb_arms.f5,
                            fb_pack.curr_fb_arms.f6,
                            fb_pack.curr_fb_arms.f7,
                            fb_pack.curr_fb_arms.f8);
                      std::cout << console_msg;
                    break;
                    case MON_TYP_TOL_CMD_UPPER_ARMS:
#if __linux__
                sprintf(
#endif
#if _WIN32
                sprintf_s(
#endif
                      console_msg, "Tolerable Command + in Ampere RMS: j1= %f, j2= %f, j3= %f, j4= %f, j5= %f, j6= %f, j7= %f, j8= %f\n",
                            fb_pack.tol_cmd_upper_arms.f1,
                            fb_pack.tol_cmd_upper_arms.f2,
                            fb_pack.tol_cmd_upper_arms.f3,
                            fb_pack.tol_cmd_upper_arms.f4,
                            fb_pack.tol_cmd_upper_arms.f5,
                            fb_pack.tol_cmd_upper_arms.f6,
                            fb_pack.tol_cmd_upper_arms.f7,
                            fb_pack.tol_cmd_upper_arms.f8);
                      std::cout << console_msg;
                    break;
                    case MON_TYP_TOL_CMD_LOWER_ARMS:
#if __linux__
                sprintf(
#endif
#if _WIN32
                sprintf_s(
#endif
                      console_msg, "Tolerable Command - in Ampere RMS: j1= %f, j2= %f, j3= %f, j4= %f, j5= %f, j6= %f, j7= %f, j8= %f\n",
                            fb_pack.tol_cmd_lower_arms.f1,
                            fb_pack.tol_cmd_lower_arms.f2,
                            fb_pack.tol_cmd_lower_arms.f3,
                            fb_pack.tol_cmd_lower_arms.f4,
                            fb_pack.tol_cmd_lower_arms.f5,
                            fb_pack.tol_cmd_lower_arms.f6,
                            fb_pack.tol_cmd_lower_arms.f7,
                            fb_pack.tol_cmd_lower_arms.f8);
                      std::cout << console_msg;
                    break;
                    case MON_TYP_RMS_CURR_ARMS:
#if __linux__
                sprintf(
#endif
#if _WIN32
                sprintf_s(
#endif
                      console_msg, "Current Feedback in Ampere RMS: j1= %f, j2= %f, j3= %f, j4= %f, j5= %f, j6= %f, j7= %f, j8= %f\n",
                            fb_pack.curr_fb_arms.f1,
                            fb_pack.curr_fb_arms.f2,
                            fb_pack.curr_fb_arms.f3,
                            fb_pack.curr_fb_arms.f4,
                            fb_pack.curr_fb_arms.f5,
                            fb_pack.curr_fb_arms.f6,
                            fb_pack.curr_fb_arms.f7,
                            fb_pack.curr_fb_arms.f8);
                      std::cout << console_msg;
                    break;
                    case MON_TYP_NULL:
                  memset(console_msg, 0, sizeof(console_msg));
#if __linux__
                  sprintf(
#endif
#if _WIN32
                  sprintf_s(
#endif
                      console_msg, "Receive (%u): Type(NULL)=%d ", fb_pack.CCount, 0);
                  std::cout << console_msg << std::endl;
                  break;
              default:
                  if (RT_API_DEBUG_MODE)
                      std::cout << "Bad data type.$n" << std::endl;
                  break;
    }
          }
          memset(console_msg, 0, sizeof(console_msg));
#if __linux__
                            sprintf(
#endif
#if _WIN32
                            sprintf_s(
#endif
                                console_msg, "CCount: %d",fb_pack.CCount);
          std::cout << "Packet Sent: " << counter_ << "\n";
          std::cout << "Packet lost: " << packet_recv_lost << "\n";

  return 0;
}

int rtmon::print_all_feedback()
{
  memset(console_msg, 0, sizeof(console_msg));
#if __linux__
            sprintf(
#endif
#if _WIN32
            sprintf_s(
#endif
            console_msg,"Input Address: %d\nInput Data: 0x%x\nOutput Address: %d\nOutput Data: 0x%x\n",fb_pack.INBitTop,fb_pack.INBitData,fb_pack.OUTBitTop,fb_pack.OUTBitData);
            std::cout<<console_msg;
      memset(console_msg, 0, sizeof(console_msg));
#if __linux__
                    sprintf(
#endif
#if _WIN32
                    sprintf_s(
#endif
                        console_msg, "JOINT Command Feedback: j1= %f, j2= %f, j3= %f, j4= %f, j5= %f, j6= %f, j7= %f, j8= %f\n",
                              fb_pack.jnt_FB.j1,
                              fb_pack.jnt_FB.j2,
                              fb_pack.jnt_FB.j3,
                              fb_pack.jnt_FB.j4,
                              fb_pack.jnt_FB.j5,
                              fb_pack.jnt_FB.j6,
                              fb_pack.jnt_FB.j7,
                              fb_pack.jnt_FB.j8);
    
                    std::cout << console_msg;
                    memset(console_msg, 0, sizeof(console_msg));
#if __linux__
                    sprintf(
#endif
#if _WIN32
                    sprintf_s(
#endif
                        console_msg, "JOINT filtered Command Feedback: j1= %f, j2= %f, j3= %f, j4= %f, j5= %f, j6= %f, j7= %f, j8= %f\n",
                              fb_pack.jnt_FFB.j1,
                              fb_pack.jnt_FFB.j2,
                              fb_pack.jnt_FFB.j3,
                              fb_pack.jnt_FFB.j4,
                              fb_pack.jnt_FFB.j5,
                              fb_pack.jnt_FFB.j6,
                              fb_pack.jnt_FFB.j7,
                              fb_pack.jnt_FFB.j8);
    
                    std::cout << console_msg;
                    memset(console_msg, 0, sizeof(console_msg));
#if __linux__
                    sprintf(
#endif
#if _WIN32
                    sprintf_s(
#endif
                        console_msg, "JOINT Encoder Feedback: j1= %f, j2= %f, j3= %f, j4= %f, j5= %f, j6= %f, j7= %f, j8= %f\n",
                              fb_pack.jnt_EFB.j1,
                              fb_pack.jnt_EFB.j2,
                              fb_pack.jnt_EFB.j3,
                              fb_pack.jnt_EFB.j4,
                              fb_pack.jnt_EFB.j5,
                              fb_pack.jnt_EFB.j6,
                              fb_pack.jnt_EFB.j7,
                              fb_pack.jnt_EFB.j8);
                    std::cout << console_msg;
                    memset(console_msg, 0, sizeof(console_msg));
#if __linux__
                    sprintf(
#endif
#if _WIN32
                    sprintf_s(
#endif
                        console_msg, "POSE Commmand Feedback: x= %f, y= %f, z= %f, a= %f, b= %f, c= %f, l1= %f, l2= %f, sflg1= %d, sflg2= %d\n",
                              fb_pack.pos_FB.x,
                              fb_pack.pos_FB.y,
                              fb_pack.pos_FB.z,
                              fb_pack.pos_FB.a,
                              fb_pack.pos_FB.b,
                              fb_pack.pos_FB.c,
                              fb_pack.pos_FB.l1,
                              fb_pack.pos_FB.l2,
                              fb_pack.pos_FB.sflg1,
                              fb_pack.pos_FB.sflg2);
    
                    std::cout << console_msg;
                    memset(console_msg, 0, sizeof(console_msg));
#if __linux__
                    sprintf(
#endif
#if _WIN32
                    sprintf_s(
#endif
    
                        console_msg, "POSE Filtered Command Feedback: x= %f, y= %f, z= %f, a= %f, b= %f, c= %f, l1= %f, l2= %f, sflg1= %d, sflg2= %d\n",
                              fb_pack.pos_FFB.x,
                              fb_pack.pos_FFB.y,
                              fb_pack.pos_FFB.z,
                              fb_pack.pos_FFB.a,
                              fb_pack.pos_FFB.b,
                              fb_pack.pos_FFB.c,
                              fb_pack.pos_FFB.l1,
                              fb_pack.pos_FFB.l2,
                              fb_pack.pos_FFB.sflg1,
                              fb_pack.pos_FFB.sflg2);
                    std::cout << console_msg;
                    memset(console_msg, 0, sizeof(console_msg));
#if __linux__
                    sprintf(
#endif
#if _WIN32
                    sprintf_s(
#endif
    
                        console_msg, "POSE Encoder Feedback: x= %f, y= %f, z= %f, a= %f, b= %f, c= %f, l1= %f, l2= %f, sflg1= %d, sflg2= %d\n",
                              fb_pack.pos_EFB.x,
                              fb_pack.pos_EFB.y,
                              fb_pack.pos_EFB.z,
                              fb_pack.pos_EFB.a,
                              fb_pack.pos_EFB.b,
                              fb_pack.pos_EFB.c,
                              fb_pack.pos_EFB.l1,
                              fb_pack.pos_EFB.l2,
                              fb_pack.pos_EFB.sflg1,
                              fb_pack.pos_EFB.sflg2);
                    std::cout << console_msg;
                    memset(console_msg, 0, sizeof(console_msg));
#if __linux__
                    sprintf(
#endif
#if _WIN32
                    sprintf_s(
#endif
                        console_msg, "PULSE Command Feedback: m1= %d, m2= %d, m3= %d, m4= %d, m5= %d, m6= %d, m7= %d, m8= %d\n",
                              fb_pack.pls_FB.m1,
                              fb_pack.pls_FB.m2,
                              fb_pack.pls_FB.m3,
                              fb_pack.pls_FB.m4,
                              fb_pack.pls_FB.m5,
                              fb_pack.pls_FB.m6,
                              fb_pack.pls_FB.m7,
                              fb_pack.pls_FB.m8);
                    std::cout << console_msg;
                    memset(console_msg, 0, sizeof(console_msg));
#if __linux__
                    sprintf(
#endif
#if _WIN32
                    sprintf_s(
#endif
                        console_msg, "PULSE Filtered Command Feedback: m1= %d, m2= %d, m3= %d, m4= %d, m5= %d, m6= %d, m7= %d, m8= %d\n",
                              fb_pack.pls_FFB.m1,
                              fb_pack.pls_FFB.m2,
                              fb_pack.pls_FFB.m3,
                              fb_pack.pls_FFB.m4,
                              fb_pack.pls_FFB.m5,
                              fb_pack.pls_FFB.m6,
                              fb_pack.pls_FFB.m7,
                              fb_pack.pls_FFB.m8);
                    std::cout << console_msg;
                    memset(console_msg, 0, sizeof(console_msg));
#if __linux__
                    sprintf(
#endif
#if _WIN32
                    sprintf_s(
#endif
                        console_msg, "PULSE Encoder Feedback: m1= %d, m2= %d, m3= %d, m4= %d, m5= %d, m6= %d, m7= %d, m8= %d\n",
                              fb_pack.pls_EFB.m1,
                              fb_pack.pls_EFB.m2,
                              fb_pack.pls_EFB.m3,
                              fb_pack.pls_EFB.m4,
                              fb_pack.pls_EFB.m5,
                              fb_pack.pls_EFB.m6,
                              fb_pack.pls_EFB.m7,
                              fb_pack.pls_EFB.m8);
                    std::cout << console_msg;
                    memset(console_msg, 0, sizeof(console_msg));
#if __linux__
                    sprintf(
#endif
#if _WIN32
                    sprintf_s(
#endif
                        console_msg, "CURRENT Command Feedback: m1= %d, m2= %d, m3= %d, m4= %d, m5= %d, m6= %d, m7= %d, m8= %d\n",
                              fb_pack.curr_cmd.m1,
                              fb_pack.curr_cmd.m2,
                              fb_pack.curr_cmd.m3,
                              fb_pack.curr_cmd.m4,
                              fb_pack.curr_cmd.m5,
                              fb_pack.curr_cmd.m6,
                              fb_pack.curr_cmd.m7,
                              fb_pack.curr_cmd.m8);
                    std::cout << console_msg;
                    memset(console_msg, 0, sizeof(console_msg));
#if __linux__
                    sprintf(
#endif
#if _WIN32
                    sprintf_s(
#endif
                        console_msg, "CURRENT Actual Feedback: m1= %d, m2= %d, m3= %d, m4= %d, m5= %d, m6= %d, m7= %d, m8= %d\n",
                              fb_pack.curr_FB.m1,
                              fb_pack.curr_FB.m2,
                              fb_pack.curr_FB.m3,
                              fb_pack.curr_FB.m4,
                              fb_pack.curr_FB.m5,
                              fb_pack.curr_FB.m6,
                              fb_pack.curr_FB.m7,
                              fb_pack.curr_FB.m8);
                    std::cout << console_msg;
                    memset(console_msg, 0, sizeof(console_msg));
#if __linux__
                    sprintf(
#endif
#if _WIN32
                    sprintf_s(
#endif
                        console_msg, "Robot Information: Tool Centre Point Speed Feedback= %f, Tool Centre Point Speed Feedback Command= %f, Remaining Distance Feedback= %f, Remaining Distance Command= %f, Gap to Command= %f, Trans Factor= %d, Acceleration State= %d, Program Step Number= %d, Program Name= %s, Controller Temperature= %fC, Operation Cycle Count= %d\n",
                              fb_pack.robot_info.tcp_fb,
                              fb_pack.robot_info.tcp_cmd,
                              fb_pack.robot_info.rem_dist_fb,
                              fb_pack.robot_info.rem_dist_cmd,
                              fb_pack.robot_info.gap_cmd_fb,
                              fb_pack.robot_info.trans_factor,
                              fb_pack.robot_info.acc_state,
                              fb_pack.robot_info.step_num,
                              fb_pack.robot_info.program_name,
                              (float)fb_pack.robot_info.cont_temp/10.0,
                              fb_pack.robot_info.mon_count
                            );
                    std::cout << console_msg;
                    memset(console_msg, 0, sizeof(console_msg));
#if __linux__
                    sprintf(
#endif
#if _WIN32
                    sprintf_s(
#endif
                        console_msg, "Position Drop: m1= %d, m2= %d, m3= %d, m4= %d, m5= %d, m6= %d, m7= %d, m8= %d\n",
                              fb_pack.pos_droop.m1,
                              fb_pack.pos_droop.m2,
                              fb_pack.pos_droop.m3,
                              fb_pack.pos_droop.m4,
                              fb_pack.pos_droop.m5,
                              fb_pack.pos_droop.m6,
                              fb_pack.pos_droop.m7,
                              fb_pack.pos_droop.m8);
                        std::cout << console_msg;
                        memset(console_msg, 0, sizeof(console_msg));
#if __linux__
                      sprintf(
#endif
#if _WIN32
                    sprintf_s(
#endif
                        console_msg, "Speed Command (rpm): m1= %d, m2= %d, m3= %d, m4= %d, m5= %d, m6= %d, m7= %d, m8= %d\n",
                              fb_pack.speed_cmd.m1,
                              fb_pack.speed_cmd.m2,
                              fb_pack.speed_cmd.m3,
                              fb_pack.speed_cmd.m4,
                              fb_pack.speed_cmd.m5,
                              fb_pack.speed_cmd.m6,
                              fb_pack.speed_cmd.m7,
                              fb_pack.speed_cmd.m8);
                        std::cout << console_msg;
                        memset(console_msg, 0, sizeof(console_msg));
#if __linux__
                    sprintf(
#endif
#if _WIN32
                  sprintf_s(
#endif
                        console_msg, "Speed Command (rpm): m1= %d, m2= %d, m3= %d, m4= %d, m5= %d, m6= %d, m7= %d, m8= %d\n",
                              fb_pack.speed_fb.m1,
                              fb_pack.speed_fb.m2,
                              fb_pack.speed_fb.m3,
                              fb_pack.speed_fb.m4,
                              fb_pack.speed_fb.m5,
                              fb_pack.speed_fb.m6,
                              fb_pack.speed_fb.m7,
                              fb_pack.speed_fb.m8);
                        std::cout << console_msg;
                        memset(console_msg, 0, sizeof(console_msg));
#if __linux__
                  sprintf(
#endif
#if _WIN32
                  sprintf_s(
#endif
                        console_msg, "Axis Load: f1= %f, f2= %f, f3= %f, f4= %f, f5= %f, f6= %f, f7= %f, f8= %f\n",
                              fb_pack.axis_load.f1,
                              fb_pack.axis_load.f2,
                              fb_pack.axis_load.f3,
                              fb_pack.axis_load.f4,
                              fb_pack.axis_load.f5,
                              fb_pack.axis_load.f6,
                              fb_pack.axis_load.f7,
                              fb_pack.axis_load.f8);
                        std::cout << console_msg;
                        memset(console_msg, 0, sizeof(console_msg));
#if __linux__
                  sprintf(
#endif
#if _WIN32
                  sprintf_s(
#endif
                        console_msg, "Motor Encoder Temperature: m1= %dC, m2= %dC, m3= %dC, m4= %dC, m5= %dC, m6= %dC, m7= %dC, m8= %dC\n",
                              fb_pack.enc_temp.m1,
                              fb_pack.enc_temp.m2,
                              fb_pack.enc_temp.m3,
                              fb_pack.enc_temp.m4,
                              fb_pack.enc_temp.m5,
                              fb_pack.enc_temp.m6,
                              fb_pack.enc_temp.m7,
                              fb_pack.enc_temp.m8);
                        std::cout << console_msg;
                        memset(console_msg, 0, sizeof(console_msg));
#if __linux__
                  sprintf(
#endif
#if _WIN32
                  sprintf_s(
#endif
                        console_msg, "Motor Miscount: m1= %d, m2= %d, m3= %d, m4= %d, m5= %d, m6= %d, m7= %d, m8= %d\n",
                              fb_pack.enc_mis.m1,
                              fb_pack.enc_mis.m2,
                              fb_pack.enc_mis.m3,
                              fb_pack.enc_mis.m4,
                              fb_pack.enc_mis.m5,
                              fb_pack.enc_mis.m6,
                              fb_pack.enc_mis.m7,
                              fb_pack.enc_mis.m8);
                        std::cout << console_msg;
                        memset(console_msg, 0, sizeof(console_msg));
#if __linux__
                  sprintf(
#endif
#if _WIN32
                  sprintf_s(
#endif
                        console_msg, "Motor Voltage: m1= %d, m2= %d, m3= %d, m4= %d, m5= %d, m6= %d, m7= %d, m8= %d\n",
                              fb_pack.motor_v.m1,
                              fb_pack.motor_v.m2,
                              fb_pack.motor_v.m3,
                              fb_pack.motor_v.m4,
                              fb_pack.motor_v.m5,
                              fb_pack.motor_v.m6,
                              fb_pack.motor_v.m7,
                              fb_pack.motor_v.m8);
                        std::cout << console_msg;
                        memset(console_msg, 0, sizeof(console_msg));
#if __linux__
                  sprintf(
#endif
#if _WIN32
                  sprintf_s(
#endif
                        console_msg, "Regenertion Level: m1= %d, m2= %d, m3= %d, m4= %d, m5= %d, m6= %d, m7= %d, m8= %d\n",
                              fb_pack.regen_lvl.m1,
                              fb_pack.regen_lvl.m2,
                              fb_pack.regen_lvl.m3,
                              fb_pack.regen_lvl.m4,
                              fb_pack.regen_lvl.m5,
                              fb_pack.regen_lvl.m6,
                              fb_pack.regen_lvl.m7,
                              fb_pack.regen_lvl.m8);
                        std::cout << console_msg;
                        memset(console_msg, 0, sizeof(console_msg));
#if __linux__
                  sprintf(
#endif
#if _WIN32
                  sprintf_s(
#endif
                        console_msg, "Tolerable Command +: m1= %d, m2= %d, m3= %d, m4= %d, m5= %d, m6= %d, m7= %d, m8= %d\n",
                              fb_pack.tol_cmd_upper.m1,
                              fb_pack.tol_cmd_upper.m2,
                              fb_pack.tol_cmd_upper.m3,
                              fb_pack.tol_cmd_upper.m4,
                              fb_pack.tol_cmd_upper.m5,
                              fb_pack.tol_cmd_upper.m6,
                              fb_pack.tol_cmd_upper.m7,
                              fb_pack.tol_cmd_upper.m8);
                        std::cout << console_msg;
                        memset(console_msg, 0, sizeof(console_msg));
#if __linux__
                  sprintf(
#endif
#if _WIN32
                  sprintf_s(
#endif
                        console_msg, "Tolerable Command -: m1= %d, m2= %d, m3= %d, m4= %d, m5= %d, m6= %d, m7= %d, m8= %d\n",
                              fb_pack.tol_cmd_lower.m1,
                              fb_pack.tol_cmd_lower.m2,
                              fb_pack.tol_cmd_lower.m3,
                              fb_pack.tol_cmd_lower.m4,
                              fb_pack.tol_cmd_lower.m5,
                              fb_pack.tol_cmd_lower.m6,
                              fb_pack.tol_cmd_lower.m7,
                              fb_pack.tol_cmd_lower.m8);
                        std::cout << console_msg;
                        memset(console_msg, 0, sizeof(console_msg));
#if __linux__
                  sprintf(
#endif
#if _WIN32
                  sprintf_s(
#endif
                        console_msg, "RMS Current: m1= %d, m2= %d, m3= %d, m4= %d, m5= %d, m6= %d, m7= %d, m8= %d\n",
                              fb_pack.rms_curr.m1,
                              fb_pack.rms_curr.m2,
                              fb_pack.rms_curr.m3,
                              fb_pack.rms_curr.m4,
                              fb_pack.rms_curr.m5,
                              fb_pack.rms_curr.m6,
                              fb_pack.rms_curr.m7,
                              fb_pack.rms_curr.m8);
                        std::cout << console_msg;
                        memset(console_msg, 0, sizeof(console_msg));
#if __linux__
                  sprintf(
#endif
#if _WIN32
                  sprintf_s(
#endif
                        console_msg, "Force Sensor Data at current position: fx= %f, fy= %f, fz= %f, mx= %f, my= %f, mz= %f\n",
                              fb_pack.fs_curr_pose.fx,
                              fb_pack.fs_curr_pose.fy,
                              fb_pack.fs_curr_pose.fz,
                              fb_pack.fs_curr_pose.mx,
                              fb_pack.fs_curr_pose.my,
                              fb_pack.fs_curr_pose.mz);
                        std::cout << console_msg;
                        memset(console_msg, 0, sizeof(console_msg));
#if __linux__
                  sprintf(
#endif
#if _WIN32
                  sprintf_s(
#endif
                        console_msg, "Force Sensor Data after offset: fx= %f, fy= %f, fz= %f, mx= %f, my= %f, mz= %f\n",
                              fb_pack.fs_after.fx,
                              fb_pack.fs_after.fy,
                              fb_pack.fs_after.fz,
                              fb_pack.fs_after.mx,
                              fb_pack.fs_after.my,
                              fb_pack.fs_after.mz);
                        std::cout << console_msg;
                        memset(console_msg, 0, sizeof(console_msg));
#if __linux__
                  sprintf(
#endif
#if _WIN32
                  sprintf_s(
#endif
                        console_msg, "Force Sensor Data before offset: fx= %f, fy= %f, fz= %f, mx= %f, my= %f, mz= %f\n",
                              fb_pack.fs_before.fx,
                              fb_pack.fs_before.fy,
                              fb_pack.fs_before.fz,
                              fb_pack.fs_before.mx,
                              fb_pack.fs_before.my,
                              fb_pack.fs_before.mz);
                        std::cout << console_msg;
                        memset(console_msg, 0, sizeof(console_msg));
#if __linux__
                  sprintf(
#endif
#if _WIN32
                  sprintf_s(
#endif
                        console_msg, "Position Command for Force Sensor Correction: x= %f, y= %f, z= %f, a= %f, b= %f, c= %f, l1= %f, l2= %f, sflg1= %d, sflg2= %d\n",
                              fb_pack.pos_cmd_fs.x,
                              fb_pack.pos_cmd_fs.y,
                              fb_pack.pos_cmd_fs.z,
                              fb_pack.pos_cmd_fs.a,
                              fb_pack.pos_cmd_fs.b,
                              fb_pack.pos_cmd_fs.c,
                              fb_pack.pos_cmd_fs.l1,
                              fb_pack.pos_cmd_fs.l2,
                              fb_pack.pos_cmd_fs.sflg1,
                              fb_pack.pos_cmd_fs.sflg2);
                        std::cout << console_msg;
                        memset(console_msg, 0, sizeof(console_msg));
#if __linux__
                  sprintf(
#endif
#if _WIN32
                  sprintf_s(
#endif
                        console_msg, "COL Presumed Torque: m1= %d, m2= %d, m3= %d, m4= %d, m5= %d, m6= %d, m7= %d, m8= %d\n",
                              fb_pack.col_torque.m1,
                              fb_pack.col_torque.m2,
                              fb_pack.col_torque.m3,
                              fb_pack.col_torque.m4,
                              fb_pack.col_torque.m5,
                              fb_pack.col_torque.m6,
                              fb_pack.col_torque.m7,
                              fb_pack.col_torque.m8);
                        std::cout << console_msg;
                        memset(console_msg, 0, sizeof(console_msg));
#if __linux__
                  sprintf(
#endif
#if _WIN32
                  sprintf_s(
#endif
                        console_msg, "COL Threshold +: m1= %d, m2= %d, m3= %d, m4= %d, m5= %d, m6= %d, m7= %d, m8= %d\n",
                              fb_pack.col_thres_upper.m1,
                              fb_pack.col_thres_upper.m2,
                              fb_pack.col_thres_upper.m3,
                              fb_pack.col_thres_upper.m4,
                              fb_pack.col_thres_upper.m5,
                              fb_pack.col_thres_upper.m6,
                              fb_pack.col_thres_upper.m7,
                              fb_pack.col_thres_upper.m8);
                        std::cout << console_msg;
                        memset(console_msg, 0, sizeof(console_msg));
#if __linux__
                  sprintf(
#endif
#if _WIN32
                  sprintf_s(
#endif
                        console_msg, "COL Threshold -: m1= %d, m2= %d, m3= %d, m4= %d, m5= %d, m6= %d, m7= %d, m8= %d\n",
                              fb_pack.col_thres_lower.m1,
                              fb_pack.col_thres_lower.m2,
                              fb_pack.col_thres_lower.m3,
                              fb_pack.col_thres_lower.m4,
                              fb_pack.col_thres_lower.m5,
                              fb_pack.col_thres_lower.m6,
                              fb_pack.col_thres_lower.m7,
                              fb_pack.col_thres_lower.m8);
                        std::cout << console_msg;
                        memset(console_msg, 0, sizeof(console_msg));
#if __linux__
                  sprintf(
#endif
#if _WIN32
                  sprintf_s(
#endif
                        console_msg, "POSE Command in degrees: x= %f, y= %f, z= %f, a= %f, b= %f, c= %f, l1= %f, l2= %f, sflg1= %d, sflg2= %d\n",
                              fb_pack.pos_cmd_deg.x,
                              fb_pack.pos_cmd_deg.y,
                              fb_pack.pos_cmd_deg.z,
                              fb_pack.pos_cmd_deg.a,
                              fb_pack.pos_cmd_deg.b,
                              fb_pack.pos_cmd_deg.c,
                              fb_pack.pos_cmd_deg.l1,
                              fb_pack.pos_cmd_deg.l2,
                              fb_pack.pos_cmd_deg.sflg1,
                              fb_pack.pos_cmd_deg.sflg2);
                        std::cout << console_msg;
                        memset(console_msg, 0, sizeof(console_msg));
#if __linux__
                  sprintf(
#endif
#if _WIN32
                  sprintf_s(
#endif
                        console_msg, "JOINT Command in degrees: j1= %f, j2= %f, j3= %f, j4= %f, j5= %f, j6= %f, j7= %f, j8= %f\n",
                              fb_pack.jnt_cmd_deg.j1,
                              fb_pack.jnt_cmd_deg.j2,
                              fb_pack.jnt_cmd_deg.j3,
                              fb_pack.jnt_cmd_deg.j4,
                              fb_pack.jnt_cmd_deg.j5,
                              fb_pack.jnt_cmd_deg.j6,
                              fb_pack.jnt_cmd_deg.j7,
                              fb_pack.jnt_cmd_deg.j8);
                        std::cout << console_msg;
                        memset(console_msg, 0, sizeof(console_msg));
#if __linux__
                  sprintf(
#endif
#if _WIN32
                  sprintf_s(
#endif
                        console_msg, "POSE Feedback in degrees: x= %f, y= %f, z= %f, a= %f, b= %f, c= %f, l1= %f, l2= %f, sflg1= %d, sflg2= %d\n",
                              fb_pack.pos_fb_deg.x,
                              fb_pack.pos_fb_deg.y,
                              fb_pack.pos_fb_deg.z,
                              fb_pack.pos_fb_deg.a,
                              fb_pack.pos_fb_deg.b,
                              fb_pack.pos_fb_deg.c,
                              fb_pack.pos_fb_deg.l1,
                              fb_pack.pos_fb_deg.l2,
                              fb_pack.pos_fb_deg.sflg1,
                              fb_pack.pos_fb_deg.sflg2);
                        std::cout << console_msg;
                        memset(console_msg, 0, sizeof(console_msg));
#if __linux__
                  sprintf(
#endif
#if _WIN32
                  sprintf_s(
#endif
                        console_msg, "JOINT Feedback in degrees: j1= %f, j2= %f, j3= %f, j4= %f, j5= %f, j6= %f, j7= %f, j8= %f\n",
                              fb_pack.jnt_fb_deg.j1,
                              fb_pack.jnt_fb_deg.j2,
                              fb_pack.jnt_fb_deg.j3,
                              fb_pack.jnt_fb_deg.j4,
                              fb_pack.jnt_fb_deg.j5,
                              fb_pack.jnt_fb_deg.j6,
                              fb_pack.jnt_fb_deg.j7,
                              fb_pack.jnt_fb_deg.j8);
                        std::cout << console_msg;
                        memset(console_msg, 0, sizeof(console_msg));
#if __linux__
                  sprintf(
#endif
#if _WIN32
                  sprintf_s(
#endif
                        console_msg, "Current Command in Ampere RMS: j1= %f, j2= %f, j3= %f, j4= %f, j5= %f, j6= %f, j7= %f, j8= %f\n",
                              fb_pack.curr_cmd_arms.f1,
                              fb_pack.curr_cmd_arms.f2,
                              fb_pack.curr_cmd_arms.f3,
                              fb_pack.curr_cmd_arms.f4,
                              fb_pack.curr_cmd_arms.f5,
                              fb_pack.curr_cmd_arms.f6,
                              fb_pack.curr_cmd_arms.f7,
                              fb_pack.curr_cmd_arms.f8);
                        std::cout << console_msg;
                        memset(console_msg, 0, sizeof(console_msg));
#if __linux__
                  sprintf(
#endif
#if _WIN32
                  sprintf_s(
#endif
                        console_msg, "Current Feedback in Ampere RMS: j1= %f, j2= %f, j3= %f, j4= %f, j5= %f, j6= %f, j7= %f, j8= %f\n",
                              fb_pack.curr_fb_arms.f1,
                              fb_pack.curr_fb_arms.f2,
                              fb_pack.curr_fb_arms.f3,
                              fb_pack.curr_fb_arms.f4,
                              fb_pack.curr_fb_arms.f5,
                              fb_pack.curr_fb_arms.f6,
                              fb_pack.curr_fb_arms.f7,
                              fb_pack.curr_fb_arms.f8);
                        std::cout << console_msg;
                        memset(console_msg, 0, sizeof(console_msg));
#if __linux__
                  sprintf(
#endif
#if _WIN32
                  sprintf_s(
#endif
                        console_msg, "Tolerable Command + in Ampere RMS: j1= %f, j2= %f, j3= %f, j4= %f, j5= %f, j6= %f, j7= %f, j8= %f\n",
                              fb_pack.tol_cmd_upper_arms.f1,
                              fb_pack.tol_cmd_upper_arms.f2,
                              fb_pack.tol_cmd_upper_arms.f3,
                              fb_pack.tol_cmd_upper_arms.f4,
                              fb_pack.tol_cmd_upper_arms.f5,
                              fb_pack.tol_cmd_upper_arms.f6,
                              fb_pack.tol_cmd_upper_arms.f7,
                              fb_pack.tol_cmd_upper_arms.f8);
                        std::cout << console_msg;
                        memset(console_msg, 0, sizeof(console_msg));
#if __linux__
                  sprintf(
#endif
#if _WIN32
                  sprintf_s(
#endif
                        console_msg, "Tolerable Command - in Ampere RMS: j1= %f, j2= %f, j3= %f, j4= %f, j5= %f, j6= %f, j7= %f, j8= %f\n",
                              fb_pack.tol_cmd_lower_arms.f1,
                              fb_pack.tol_cmd_lower_arms.f2,
                              fb_pack.tol_cmd_lower_arms.f3,
                              fb_pack.tol_cmd_lower_arms.f4,
                              fb_pack.tol_cmd_lower_arms.f5,
                              fb_pack.tol_cmd_lower_arms.f6,
                              fb_pack.tol_cmd_lower_arms.f7,
                              fb_pack.tol_cmd_lower_arms.f8);
                        std::cout << console_msg;
                        memset(console_msg, 0, sizeof(console_msg));
#if __linux__
                  sprintf(
#endif
#if _WIN32
                  sprintf_s(
#endif
                        console_msg, "Current Feedback in Ampere RMS: j1= %f, j2= %f, j3= %f, j4= %f, j5= %f, j6= %f, j7= %f, j8= %f\n",
                              fb_pack.curr_fb_arms.f1,
                              fb_pack.curr_fb_arms.f2,
                              fb_pack.curr_fb_arms.f3,
                              fb_pack.curr_fb_arms.f4,
                              fb_pack.curr_fb_arms.f5,
                              fb_pack.curr_fb_arms.f6,
                              fb_pack.curr_fb_arms.f7,
                              fb_pack.curr_fb_arms.f8);
                        std::cout << console_msg;
                        memset(console_msg, 0, sizeof(console_msg));
#if __linux__
                    sprintf(
#endif
#if _WIN32
                    sprintf_s(
#endif
                        console_msg, "Receive (%u): Type(NULL)=%d ", fb_pack.CCount, 0);
                    std::cout << console_msg << std::endl;
            memset(console_msg, 0, sizeof(console_msg));
#if __linux__
                              sprintf(
#endif
#if _WIN32
                              sprintf_s(
#endif
                                  console_msg, "CCount: %d",fb_pack.CCount);
            std::cout << "Packet Sent: " << counter_ << "\n";
            std::cout << "Packet lost: " << packet_recv_lost << "\n";
  
  return 0;
}

}  // namespace MelfaEthernet