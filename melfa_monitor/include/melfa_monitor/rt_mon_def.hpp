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
 * @file rt_mon_def.hpp
 * @author Liu Muyao
 * @brief Header file for Real Time Monitoring API
 *
 * @copyright COPYRIGHT (C) 2025 Mitsubishi Electric Corporation
 *
 */

#ifndef __RTMON_DEF__
#define __RTMON_DEF__

#include <stdint.h>

/**
 * @brief Joint coordinate system (Set unused additional axis to 0).
 *
 */
typedef struct
{
  /**
   * @brief J1 axis angle
   * @brief [rad]
   *
   */
  float j1;
  /**
   * @brief J2 axis angle
   * @brief [rad]
   *
   */
  float j2;
  /**
   * @brief J3 axis angle
   * @brief [rad]
   *
   */
  float j3;
  /**
   * @brief J4 axis angle
   * @brief [rad]
   *
   */
  float j4;
  /**
   * @brief J5 axis angle
   * @brief [rad]
   *
   */
  float j5;
  /**
   * @brief J6 axis angle
   * @brief [rad]
   *
   */
  float j6;
  /**
   * @brief Additional axis 1 (J7 axis angle)
   * @brief [rad]
   *
   */
  float j7;
  /**
   * @brief Additional axis 2 (J8 axis angle)
   * @brief [rad]
   *
   */
  float j8;

} JOINT;
/**
 * @brief POSE. XYZ coordinate system (Unused additional axis = 0)
 *
 */
typedef struct
{
  /**
   * @brief X axis coordinate value
   * @brief [mm]
   *
   */
  float x;
  /**
   * @brief Y axis coordinate value
   * @brief [mm]
   *
   */
  float y;
  /**
   * @brief Z axis coordinate value
   * @brief [mm]
   *
   */
  float z;
  /**
   * @brief A axis coordinate value
   * @brief [rad]
   *
   */
  float a;
  /**
   * @brief B axis coordinate value
   * @brief [rad]
   *
   */
  float b;
  /**
   * @brief C axis coordinate value
   * @brief [rad]
   *
   */
  float c;
  /**
   * @brief Additional axis 1. Linear or Rotational\
   * @brief [mm or rad]
   *
   */
  float l1;
  /**
   * @brief Additional axis 2. Linear or Rotational\
   * @brief [mm or rad]
   *
   */
  float l2;
  /**
   * @brief Structural flag 1
   *
   */
  uint32_t sflg1;
  /**
   * @brief Structural flag 2
   *
   */
  uint32_t sflg2;

} POSE;

/**
 * @brief Generic int32 struct. Used for different parameters.
 *
 */
typedef struct
{
  /**
   * @brief Genneric int32
   *
   */
  int32_t m1;
  /**
   * @brief Genneric int32
   *
   */
  int32_t m2;
  /**
   * @brief Genneric int32
   *
   */
  int32_t m3;
  /**
   * @brief Genneric int32
   *
   */
  int32_t m4;
  /**
   * @brief Genneric int32
   *
   */
  int32_t m5;
  /**
   * @brief Genneric int32
   *
   */
  int32_t m6;
  /**
   * @brief Genneric int32
   *
   */
  int32_t m7;
  /**
   * @brief Genneric int32
   *
   */
  int32_t m8;

} PULSE;
/**
 * @brief Force Sensor data strcuture
 *
 */
typedef struct
{
  /**
   * @brief Force Feedback along X-axis.
   * @brief [N]
   *
   */
  float fx;
  /**
   * @brief FForce Feedback along Y-axis.
   * @brief [N]
   *
   */
  float fy;
  /**
   * @brief FForce Feedback along Z-axis.
   * @brief [N]
   *
   */
  float fz;
  /**
   * @brief Force Feedback about X-axis.
   * @brief [Nm]
   *
   */
  float mx;
  /**
   * @brief Force Feedback about Y-axis.
   * @brief [Nm]
   *
   */
  float my;
  /**
   * @brief Force Feedback about Z-axis.
   * @brief [Nm]
   *
   */
  float mz;

} FORCE;
/**
 * @brief Robot Information data structure
 *
 */
typedef struct
{
  /**
   * @brief Speed of a tool center point (feedback)
   * @brief [mm/s]
   *
   */
  float tcp_fb;
  /**
   * @brief The remaining distance to the target position while the robot is moving (feedback).
   * @brief [mm]
   *
   */
  float rem_dist_fb;
  /**
   * @brief Speed of a tool center point (command). Same as status variable values “M_RSpd”
   * @brief [mm/s]
   *
   */
  float tcp_cmd;
  /**
   * @brief The remaining distance to the target position while the robot is moving (command). Same as status variable
   * values “M_RDst”.
   * @brief [mm]
   *
   */
  float rem_dist_cmd;
  /**
   * @brief The gap of a command position and a feedback position. Same as status variable values “M_Fbd”. [mm]
   * @brief [mm]
   *
   */
  float gap_cmd_fb;
  /**
   * @brief ???
   *
   */
  uint16_t trans_factor;
  /**
   * @brief The current acceleration/deceleration status. (command) Same as status variable values “M_AclSts”.
   * @brief [0=Stopped,1=Accelerating, 2= Constant speed, 3= Decelerating]
   *
   */
  uint16_t acc_state;
  /**
   * @brief Step number (Only slot 1), (1-32767)
   *
   */
  uint16_t step_num;
  /**
   * @brief Program name (Only slot 1) Max Program name is 6 characters
   *
   */
  char program_name[6];
  /**
   * @brief Controller temperature
   * @brief [0.1°C]
   *
   */
  int16_t cont_temp;
  /**
   * @brief unused 2 bytes
   *
   */
  uint16_t unused;
  /**
   * @brief After power-on, +1 is counted from 0 in an operation control cycle unit (64/9 (*1)). The counting repeats in
   * the range of 0 to 4294967295.
   *
   */
  uint32_t mon_count;

} ROBMON;
/**
 * @brief Generic Float8 data structure
 *
 */
typedef struct
{
  /**
   * @brief Generic 4 byte float
   *
   */
  float f1;
  /**
   * @brief Generic 4 byte float
   *
   */
  float f2;
  /**
   * @brief Generic 4 byte float
   *
   */
  float f3;
  /**
   * @brief Generic 4 byte float
   *
   */
  float f4;
  /**
   * @brief Generic 4 byte float
   *
   */
  float f5;
  /**
   * @brief Generic 4 byte float
   *
   */
  float f6;
  /**
   * @brief Generic 4 byte float
   *
   */
  float f7;
  /**
   * @brief Generic 4 byte float
   *
   */
  float f8;
  /**
   * @brief Unused 8 bytes
   *
   */
  // double unused;
} FLOAT8;

/************************************************************/
/* Data type ID
 */
/***************Monitoring*********************************************/

/**
 * @brief Real Time Monitoring Start command
 *
 */
#define MON_CMD_START 1

/**
 * @brief Real Time Monitoring End command
 *
 */
#define MON_CMD_END 255

// Command or monitor data type //

/**
 * @brief NULL data type\
 * @brief R3n/S3n or later
 *
 */
#define MON_TYP_NULL 0

/**
 * @brief Pose data type command\
 * @brief [mm/rad]\
 * @brief R3n/S3n or later
 *
 */
#define MON_TYP_POSE_CMD 1

/**
 * @brief Joint data type command\
 * @brief [rad]\
 * @brief R3n/S3n or later
 *
 */
#define MON_TYP_JOINT_CMD 2

/**
 * @brief Pulse data type command\
 * @brief [pulse]\
 * @brief R3n/S3n or later
 *
 */
#define MON_TYP_PULSE_CMD 3

/**
 * @brief Pose command type after filter\
 * @brief [mm/rad]\
 * @brief R3n/S3n or later
 *
 */
#define MON_TYP_FPOSE_CMD 4

/**
 * @brief Joint command type after filter\
 * @brief [rad]\
 * @brief R3n/S3n or later
 *
 */
#define MON_TYP_FJOINT_CMD 5

/**
 * @brief Pulse command type after filter\
 * @brief [pulse]\
 * @brief R3n/S3n or later
 *
 */
#define MON_TYP_FPULSE_CMD 6

/**
 * @brief Pose Encoder Feedback\
 * @brief [mm/rad]\
 * @brief R3n/S3n or later
 *
 */
#define MON_TYP_POSE_FB 7

/**
 * @brief Joint Encoder Feedback\
 * @brief [rad]\
 * @brief R3n/S3n or later
 *
 */
#define MON_TYP_JOINT_FB 8

/**
 * @brief Pulse Encoder Feedback\
 * @brief [pulse]\
 * @brief R3n/S3n or later
 *
 */
#define MON_TYP_PULSE_FB 9

// For current related monitors ////////////////////

/**
 * @brief Electric current command\
 * @brief [0.1% rate]\
 * @brief R3n/S3n or later
 *
 */
#define MON_TYP_CURR_CMD 10

/**
 * @brief Electric current feedback\
 * @brief [0.1% rate]\
 * @brief R3n/S3n or later
 *
 */
#define MON_TYP_CURR_FB 11

/**
 * @brief Robot Information\
 * @brief [ROBMON]\
 * @brief R3n/S3n or later
 *
 */
#define MON_TYP_ROBMON 12

/**
 * @brief Position droop\
 * @brief [pulse]\
 * @brief R4b/S4b or later
 *
 */
#define MON_TYP_POS_DROOP 13

/**
 * @brief Speed command\
 * @brief [rpm]\
 * @brief R4b/S4b or later
 *
 */
#define MON_TYP_SPEED_CMD 14

/**
 * @brief Speed feedback\
 * @brief [rpm]\
 * @brief R4b/S4b or later
 *
 */
#define MON_TYP_SPEED_FB 15

/**
 * @brief Axis load level\
 * @brief [%]\
 * @brief R4b/S4b or later
 *
 */
#define MON_TYP_AXIS_LOAD 16

/**
 * @brief Encoder temperature\
 * @brief [degree C]\
 * @brief R4b/S4b or later
 *
 */
#define MON_TYP_ENC_TEMP 17

/**
 * @brief Encoder miscount\
 * @brief [pulse]\
 * @brief R4b/S4b or later
 *
 */
#define MON_TYP_ENC_MIS 18

/**
 * @brief Motor voltage\
 * @brief [V]\
 * @brief R4b/S4b or later
 *
 */
#define MON_TYP_MOTOR_V 19

/**
 * @brief Regeneration level\
 * @brief [%]\
 * @brief R4b/S4b or later
 *
 */
#define MON_TYP_REGEN_LVL 20

/**
 * @brief Tolerable command +\
 * @brief [0.1% rate]\
 * @brief R4b/S4b or later
 *
 */
#define MON_TYP_TOL_CMD_UPPER 21

/**
 * @brief Tolerable command -\
 * @brief [0.1% rate]\
 * @brief R4b/S4b or later
 *
 */
#define MON_TPY_TOL_CMD_LOWER 22

/**
 * @brief RMS current\
 * @brief [0.1% rate]\
 * @brief R4b/S4b or later
 *
 */
#define MON_TYP_RMS_CURR 23

/**
 * @brief Force sensor current position\
 * @brief [N/Nm]\
 * @brief R3n/S3n or later
 *
 */
#define MON_TYP_FORCE_SENSOR_AT_POSE 101

/**
 * @brief Force sensor original data after offset cancel\
 * @brief [N/Nm]\
 * @brief R3n/S3n or later
 *
 */
#define MON_TYP_FORCE_SENSOR_AFTER_OFFSET 102

/**
 * @brief Force sensor original data before offset cancel\
 * @brief [N/Nm]\
 * @brief R3n/S3n or later
 *
 */
#define MON_TYP_FORCE_SENSOR_BEFORE_OFFSET 103

/**
 * @brief Position command of the force sensor correction\
 * @brief [mm/rad]\
 * @brief R3n/S3n or later
 *
 */
#define MON_TYP_POSE_CMD_FOR_FORCE_SENSOR_CORRECTION 104

/**
 * @brief COL presumed torque\
 * @brief [0.1% rate]\
 * @brief R3n/S3n or later
 *
 */
#define MON_TYP_COL_PRESUMED_TORQUE 111

/**
 * @brief COL threshold +\
 * @brief [0.1% rate]\
 * @brief R3n/S3n or later
 *
 */
#define MON_TYP_COL_THRESHOLD_UPPER 112

/**
 * @brief COL threshold -\
 * @brief [0.1% rate]\
 * @brief R3n/S3n or later
 *
 */
#define MON_TYP_COL_THRESHOLD_LOWER 113

/**
 * @brief POSE command with angles in degrees\
 * @brief [mm/deg]\
 * @brief R4b/S4b or later
 *
 */
#define MON_TYP_POSE_CMD_DEGREES 1001

/**
 * @brief JOINT command with angles in degrees\
 * @brief [deg]\
 * @brief R4b/S4b or later
 */
#define MON_TYP_JOINT_CMD_DEGREES 1002

/**
 * @brief POSE feedback with angles in degrees\
 * @brief [mm/deg]\
 * @brief R4b/S4b or later
 *
 */
#define MON_TYP_POSE_FB_DEGREES 1007

/**
 * @brief Joint feedback with angle in degrees\
 * @brief [deg]\
 * @brief R4b/S4b or later
 *
 */
#define MON_TYP_JOINT_FB_DEGREES 1008

/**
 * @brief Curent command in Ampere RMS\
 * @brief [Arms]\
 * @brief R4b/S4b or later
 *
 */
#define MON_TYP_CURR_CMD_ARMS 1010

/**
 * @brief Curent feedback in Ampere RMS\
 * @brief [Arms]\
 * @brief R4b/S4b or later
 *
 */
#define MON_TYP_CURR_FB_ARMS 1011

/**
 * @brief Tolerable command +\
 * @brief [Arms]\
 * @brief R4b/S4b or later
 *
 */
#define MON_TYP_TOL_CMD_UPPER_ARMS 1012

/**
 * @brief Tolerable command -\
 * @brief [Arms]\
 * @brief R4b/S4b or later
 *
 */
#define MON_TYP_TOL_CMD_LOWER_ARMS 1013

/**
 * @brief RMS current\
 * @brief [Arms]\
 * @brief R4b/S4b or later
 *
 */
#define MON_TYP_RMS_CURR_ARMS 1014

/**
 * @brief Real-time function communication data packet
 *
 */
typedef struct enet_rtmon_str
{
  /**
   * @brief Specifies the start or end of the real-time monitoring function.
   */
  uint16_t Command;

  /**
   * @brief Not used
   *
   */
  uint16_t reserve1;

  /**
   * @brief (1) Communication device → Robot controller. Specifies the <Data type ID> of the data that you want to
   * monitor.\
   * @brief (2) Robot controller → Communication device. Echo back of send (1)\
   * @brief Data type ID see Manual BFP-A3379-E, Ethernet Function Instruction Manual, Chapter 3.4.6 Data type ID
   *
   */
  uint16_t RecvType1;

  /**
   * @brief Not used
   *
   */
  uint16_t reserve2;

  /**
   * @brief (1) E Communication device → Robot controller. Not used. Set to zero.\
   * @brief (2) Robot controller → Communication device. The output data sent back from the controller. Data type is
   * seeing in the return data type.\
   * @brief Data structure: POSE, JOINT, PULSE, FORCE, ROBMON, FLOAT8.
   */
  union rtdata1
  {
    POSE pos;       // XYZ type [mm/rad]
    JOINT jnt;      // Joint type [rad]
    PULSE pls;      // PULSE type [pulse] or Integer type [% / non-unit]
    ROBMON robmon;  // Robot Information
    FORCE force;    // Force sensor
    FLOAT8 float8;  // Generic float
  } dat1;

  /**
   * @brief (1) Communication device → Robot controller. Input signal number of the top (0 to 32767)\
   * @brief (2) Robot controller → Communication device. Echo back of send (1)
   *
   */
  uint16_t INBitTop;

  /**
   * @brief (1) Communication device → Robot controller. Output signal number of the top (0 to 32767)\
   * @brief (2) Robot controller → Communication device. Echo back of send (1)
   *
   */
  uint16_t OUTBitTop;

  /**
   * @brief (1) Communication device → Robot controller. Not used. Set to zero.\
   * @brief (2) Robot controller → Communication device. Input signal data(0x00000000-0xffffffff)
   *
   */
  uint32_t INBitData;

  /**
   * @brief (1) Communication device → Robot controller. Not used. Set to zero.\
   * @brief (2) Robot controller → Communication device. Output signal data(0x00000000-0xffffffff)
   *
   */
  uint32_t OUTBitData;

  /**
   * @brief (1) Communication device → Robot controller. Not used. Set to zero.\
   * @brief (2) Robot controller → Communication device. The number of communications. To return to the minimum value 0
   * and the maximum value by integrating.
   *
   */
  uint32_t CCount;

  /**
   * @brief Monitor data type designation for second of four monitor data types
   *
   */
  uint16_t RecvType2;

  /**
   * @brief Not used
   *
   */
  uint16_t reserve3;

  /**
   * @brief (1) E Communication device → Robot controller. Not used. Set to zero.\
   * @brief (2) Robot controller → Communication device. The output data sent back from the controller. Data type is
   * seeing in the return data type.\
   * @brief Data structure: POSE, JOINT, PULSE, FORCE, ROBMON, FLOAT8.
   */
  union rtdata2
  {
    POSE pos;       // XYZ type [mm/rad]
    JOINT jnt;      // Joint type [rad]
    PULSE pls;      // PULSE type [pulse] or Integer type [% / non-unit]
    ROBMON robmon;  // Robot Information
    FORCE force;    // Force sensor
    FLOAT8 float8;  // Generic float
  } dat2;
  /**
   * @brief Monitor data type designation for third of four monitor data types
   *
   */
  uint16_t RecvType3;

  /**
   * @brief Not used
   *
   */
  uint16_t reserve4;

  /**
   * @brief (1) E Communication device → Robot controller. Not used. Set to zero.\
   * @brief (2) Robot controller → Communication device. The output data sent back from the controller. Data type is
   * seeing in the return data type.\
   * @brief Data structure: POSE, JOINT, PULSE, FORCE, ROBMON, FLOAT8.
   */
  union rtdata3
  {
    POSE pos;       // XYZ type [mm/rad]
    JOINT jnt;      // Joint type [rad]
    PULSE pls;      // PULSE type [pulse] or Integer type [% / non-unit]
    ROBMON robmon;  // Robot Information
    FORCE force;    // Force sensor
    FLOAT8 float8;  // Generic float
  } dat3;

  /**
   * @brief Monitor data type designation for forth of four monitor data types
   *
   */
  uint16_t RecvType4;

  /**
   * @brief Not used
   *
   */
  uint16_t reserve5;

  /**
   * @brief (1) E Communication device → Robot controller. Not used. Set to zero.\
   * @brief (2) Robot controller → Communication device. The output data sent back from the controller. Data type is
   * seeing in the return data type.\
   * @brief Data structure: POSE, JOINT, PULSE, FORCE, ROBMON, FLOAT8.
   */
  union rtdata4
  {
    POSE pos;       // XYZ type [mm/rad]
    JOINT jnt;      // Joint type [rad]
    PULSE pls;      // PULSE type [pulse] or Integer type [% / non-unit]
    ROBMON robmon;  // Robot Information
    FORCE force;    // Force sensor
    FLOAT8 float8;  // Generic float
  } dat4;

} RTMONPACK;

#endif
/*EOF*/
