/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      tuya_service.h
* @brief
* @details
* @author    
* @date      
* @version   
* *********************************************************************************************************
*/

#ifndef _TUYA_SERVICE_H
#define _TUYA_SERVICE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "profile_server.h"

#define PARA_VALUE_LEN_MAX  20
/****************************************************************************************************************
* macros that other .c files may use all defined here
****************************************************************************************************************/
/** @brief  GATT UUID definitions. */
#define GATT_UUID_EXS  0xFFD0
#define GATT_UUID_CHAR_PARAM 0xFFD1
#define GATT_UUID_CHAR_KEY  0xFFD2

#define GATT_UUID128_KNS          0x70, 0x75, 0x6F, 0x72, 0x47, 0x74, 0x62, 0x2E, 0x61, 0x79, 0x75, 0x74, 0x00, 0xA3, 0x01, 0x56,
#define GATT_UUID128_CHAR_PARAM   0x70, 0x75, 0x6F, 0x72, 0x47, 0x74, 0x62, 0x2E, 0x61, 0x79, 0x75, 0x74, 0x01, 0xA3, 0x01, 0x56,
#define GATT_UUID128_CHAR_KEY     0x70, 0x75, 0x6F, 0x72, 0x47, 0x74, 0x62, 0x2E, 0x61, 0x79, 0x75, 0x74, 0x02, 0xA3, 0x01, 0x56,

#define GATT_UUID16_CHAR_PARAM    0x11, 0x2b,
#define GATT_UUID16_CHAR_KEY      0x10, 0x2b,

/** @brief  PXP Extended characteristics index definitions. */
#define KNS_PARAM_VALUE_INDEX  2
#define KNS_KEY_VALUE_INDEX  4
#define KNS_KEY_VALUE_CCCD_INDEX 5

#define KNS_READ_PARA      2
#define KNS_NOTIFY_ENABLE  1
#define KNS_NOTIFY_DISABLE 0

typedef enum
{
    KNS_PARAM_VALUE
} T_KNS_PARAM_TYPE;
/**
 * @brief PXP extended data struct for notification data to application.
 *
 * PXP extended service data to inform application.
*/
typedef struct
{
    uint8_t data[PARA_VALUE_LEN_MAX];
    uint8_t data_len; 
}T_KNS_UPSTREAM_WRITE_DATA;
typedef union
{
    uint8_t notification_indification_index;
    uint8_t read_index;
    T_KNS_UPSTREAM_WRITE_DATA write_value;
} T_KNS_UPSTREAM_MSG_DATA;

typedef struct _TOTA_CALLBACK_DATA
{
    T_SERVICE_CALLBACK_TYPE     msg_type;                    /**<  @brief EventId defined upper */
    T_KNS_UPSTREAM_MSG_DATA  msg_data;
} T_KNS_CALLBACK_DATA;

/* Add PXP Extended service to your application. */
uint8_t kns_add_service(void *pFunc);
bool kns_set_parameter(T_KNS_PARAM_TYPE param_type, uint8_t length, uint8_t *p_value);
#ifdef __cplusplus
}
#endif

#endif /* _TUYA_SERVICE_H */
