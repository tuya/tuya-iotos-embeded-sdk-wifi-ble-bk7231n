/**
 * \file tuya_ble_config.h
 *
 * \brief 
 */
/*
 *  Copyright (C) 2014-2019, Tuya Inc., All Rights Reserved
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  This file is part of tuya ble sdk 
 */


#ifndef TUYA_BLE_CONFIG_H__
#define TUYA_BLE_CONFIG_H__
#include "tuya_iot_config.h"

#if defined(TY_BT_MOD) && (TY_BT_MOD==1)

/*
 * If using an OS, be sure to call the tuya api functions and SDK-related queues within the same task
 */
#define TUYA_BLE_USE_OS 1

/*
 * If using an OS, tuya ble sdk  will create a task autonomously to process ble event.
 */

#if TUYA_BLE_USE_OS

#define TUYA_BLE_TASK_PRIORITY  1

#endif


/** @defgroup TUY_BLE_DEVICE_COMMUNICATION_ABILITY tuya ble device communication ability
 * @{
 */
#define TUYA_BLE_DEVICE_COMMUNICATION_ABILITY_BLE                0x0000
#define TUYA_BLE_DEVICE_COMMUNICATION_ABILITY_REGISTER_FROM_BLE  0x0001
#define TUYA_BLE_DEVICE_COMMUNICATION_ABILITY_MESH               0x0002
#define TUYA_BLE_DEVICE_COMMUNICATION_ABILITY_WIFI_24G           0x0004
#define TUYA_BLE_DEVICE_COMMUNICATION_ABILITY_WIFI_5G            0x0008
#define TUYA_BLE_DEVICE_COMMUNICATION_ABILITY_ZIGBEE             0x0010
#define TUYA_BLE_DEVICE_COMMUNICATION_ABILITY_NB                 0x0020
/** End of TUY_BLE_DEVICE_COMMUNICATION_ABILITY
  * @}
  */

#define TUYA_BLE_DEVICE_COMMUNICATION_ABILITY  (TUYA_BLE_DEVICE_COMMUNICATION_ABILITY_BLE|TUYA_BLE_DEVICE_COMMUNICATION_ABILITY_WIFI_24G | \
                                                TUYA_BLE_DEVICE_COMMUNICATION_ABILITY_WIFI_5G)


/** @defgroup TUYA_BLE_SECURE_CONNECTION_TYPE tuya ble secure connection type
 * @{
 */
#define TUYA_BLE_SECURE_CONNECTION_WITH_AUTH_KEY     0x00
#define TUYA_BLE_SECURE_CONNECTION_WITH_ECC          0x01
#define TUYA_BLE_SECURE_CONNECTION_WTIH_PASSTHROUGH  0x02
#define TUYA_BLE_SECURE_CONNECTION_WITH_AUTH_KEY_DEVICE_ID_20      0x03


/** End of TUYA_BLE_SECURE_CONNECTION_TYPE
  * @}
  */
#define  TUYA_BLE_SECURE_CONNECTION_TYPE  TUYA_BLE_SECURE_CONNECTION_WITH_AUTH_KEY

/*
 * TUYA_BLE_DEVICE_NORMAL - normal ble devcie ;
   TUYA_BLE_DEVICE_MULTI_PROTOCOL - ble with wifi or zigbee devcie;
 */
#define TUYA_BLE_DEVICE_TYPE  TUYA_BLE_DEVICE_MULTI_PROTOCOL_WIFI 
 
/*
 * if defined ,include UART module
 */
//#define TUYA_BLE_UART 


/*
 * if defined ,include product test module
 */
//#define TUYA_BLE_PRODUCTION_TEST 


/*
 * gatt mtu max sizes
 */
#define TUYA_BLE_DATA_MTU_MAX  20


/*
 * if defined ,enable log output
 */
//#define TUYA_BLE_LOG_ENABLED


/*
 * MACRO for memory management
 */
#define TUYA_BLE_TOTAL_HEAP_SIZE   ( 2*1024 )

#define MAX_NUMBER_OF_TUYA_MESSAGE        0x20      //!<  tuya ble message queue size


/*
 * MACRO for advanced encryption,if 1 will use user rand check.
 */
#define TUYA_BLE_ADVANCED_ENCRYPTION_DEVICE 0


#define TUYA_BLE_AIR_FRAME_MAX  305//256//255字节的dp下发，255+3+14+1+16+16=305

#define TUYA_BLE_BLE_MEM_SIZE   TUYA_BLE_AIR_FRAME_MAX
#define TUYA_BLE_UART_MEM_SIZE  256


#define TUYA_UART_RECEIVE_MAX_DP_DATA_LEN         200
#define TUYA_UART_RECEIVE_MAX_DP_BUFFER_DATA_LEN  200

#define TUYA_BLE_RECEIVE_MAX_DP_DATA_LEN           TUYA_UART_RECEIVE_MAX_DP_DATA_LEN

#define TUYA_BLE_TRANSMISSION_MAX_DATA_LEN        (TUYA_BLE_AIR_FRAME_MAX-29)

//BLE 通讯协议版本 v3.3 
#define TUYA_BLE_PROTOCOL_VERSION_HIGN   0x03
#define TUYA_BLE_PROTOCOL_VERSION_LOW    0x03


#ifndef TUYA_BLE_MAX_CALLBACKS
#define TUYA_BLE_MAX_CALLBACKS 1
#endif


/*
 * 1 - device register from ble  0 - from others
 * @note: 
 */
#define TUYA_BLE_DEVICE_REGISTER_FROM_BLE  (TUYA_BLE_DEVICE_COMMUNICATION_ABILITY&TUYA_BLE_DEVICE_COMMUNICATION_ABILITY_REGISTER_FROM_BLE)


#define TUYA_BLE_CB_EVT_BASE    0x40

#define DEVICE_NAME_LEN         16
#define H_ID_LEN                19
//#define PRODUCT_ID_LEN          TUYA_BLE_PRODUCT_ID_OR_KEY_LEN
//#define PRODUCT_KEY_LEN         TUYA_BLE_PRODUCT_ID_OR_KEY_LEN

#define DEVICE_ID_LEN           16
#define AUTH_KEY_LEN            32
#define LOGIN_KEY_LEN           6
#define ECC_SECRET_KEY_LEN      32
#define DEVICE_VIRTUAL_ID_LEN   22
#define SECRET_KEY_LEN          16
#define PAIR_RANDOM_LEN         6
#define MAC_LEN                 6

#define TUYA_BLE_PRODUCT_ID_MAX_LEN  16

#define TUYA_BLE_WIFI_DEVICE_REGISTER_MODE 1

#endif
#endif



