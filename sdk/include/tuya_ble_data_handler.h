/**
 * \file tuya_ble_data_handler.h
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

#ifndef TUYA_BLE_DATA_HANDLER_H_
#define TUYA_BLE_DATA_HANDLER_H_

#include <stdint.h>
#include "tuya_cloud_types.h"
#include "tuya_hal_bt.h"
#include "tuya_ble_config.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(TY_BT_MOD) && (TY_BT_MOD==1)
void tuya_ble_pair_rand_clear(void);

uint8_t tuya_ble_pair_rand_valid_get(void);


//void tuya_ble_set_device_version(uint32_t firmware_version,uint32_t hardware_version);

//uint8_t tuya_ble_commData_send(uint16_t cmd,uint32_t ack_sn,uint8_t *data,uint16_t len,uint8_t encryption_mode);

void tuya_ble_reset_ble_sn(void);


typedef struct {
    UINT_T sn;
    UINT16_T cmd;
    BYTE_T *data;
    UINT16_T len;
    BYTE_T encrypt_mode;
}tuya_ble_app_data_t;


OPERATE_RET tuya_ble_data_proc(OUT tuya_ble_user_data_t *payload, IN tuya_ble_data_buf_t *fromapp,
    OUT tuya_ble_app_data_t *toapp);

OPERATE_RET tuya_ble_data_packaging(IN tuya_ble_app_data_t *app_data, OUT tuya_ble_data_buf_t *encrypt);

#endif

#ifdef __cplusplus
}
#endif

#endif // TUYA_BLE_COMMDATA_HANDLER_H_




