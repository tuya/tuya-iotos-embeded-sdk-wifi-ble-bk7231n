/**
 * \file tuya_ble_cryption_api.h
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

#ifndef TUYA_BLE_ENCRYPTION_API_H_
#define TUYA_BLE_ENCRYPTION_API_H_

#include <stdint.h>
#include <stdbool.h>
#include "tuya_ble_config.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(TY_BT_MOD) && (TY_BT_MOD==1)

enum
{
    ENCRYPTION_MODE_NONE,
    ENCRYPTION_MODE_KEY_1,
    ENCRYPTION_MODE_KEY_2,//MD5（secret_key_1,srand）的结果16字节转32个字符，取第8-24个字符，只用于配对请求指令。
    ENCRYPTION_MODE_KEY_3,//MD5(secret_key_1,srand,user_rand) 的结果16字节转32个字符，取第8-24个字符,适用于高级加密设备
    ENCRYPTION_MODE_KEY_4,//MD5（login_key）的结果16字节转32个字符，取第8-24个字符
    ENCRYPTION_MODE_SESSION_KEY,//MD5（login_key,srand）的结果16字节转32个字符，取第8-24个字符
    ENCRYPTION_MODE_MAX,
};

typedef struct {
    uint8_t *auth_key;
    uint8_t *user_rand;
    uint8_t *login_key;
    uint8_t *pair_rand;
}tuya_ble_cryption_param_t;

uint8_t tuya_ble_encryption(tuya_ble_cryption_param_t *p, uint8_t encryption_mode,uint8_t *iv,uint8_t *in_buf,uint32_t in_len,uint32_t *out_len,uint8_t *out_buf);

uint8_t  tuya_ble_encrypt_old_with_key(uint8_t *key,uint8_t *in_buf,uint8_t in_len,uint8_t *out_buf);

bool tuya_ble_device_id_encrypt(uint8_t *key_in,uint16_t key_len,uint8_t *input,uint16_t input_len,uint8_t *output);

uint8_t tuya_ble_decryption(tuya_ble_cryption_param_t *p, uint8_t const *in_buf,uint32_t in_len,uint32_t *out_len,uint8_t *out_buf);

bool tuya_ble_register_key_generate(uint8_t *output, uint8_t *auth_key);

#endif

#ifdef __cplusplus
}
#endif

#endif // TUYA_BLE_ENCRYPTION_API_H_


