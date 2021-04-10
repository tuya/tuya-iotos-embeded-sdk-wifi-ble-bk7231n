/*
 * @Author: yj 
 * @email: shiliu.yang@tuya.com
 * @LastEditors: yj 
 * @file name: tuya_device.c
 * @Description: template demo for SDK WiFi & BLE for BK7231N
 * @Copyright: HANGZHOU TUYA INFORMATION TECHNOLOGY CO.,LTD
 * @Company: http://www.tuya.com
 * @Date: 2021-02-22 15:10:00
 * @LastEditTime: 2021-02-22 15:10:00
 */

#ifndef _TUYA_DEVICE_H
#define _TUYA_DEVICE_H

/* Includes ------------------------------------------------------------------*/
#include "tuya_cloud_com_defs.h"
    
#ifdef __cplusplus
    extern "C" {
#endif /* __cplusplus */
    
#ifdef _TUYA_DEVICE_GLOBAL
    #define _TUYA_DEVICE_EXT 
#else
    #define _TUYA_DEVICE_EXT extern
#endif /* _TUYA_DEVICE_GLOBAL */ 
    
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/              
/* Exported macro ------------------------------------------------------------*/
// device information define
#define DEV_SW_VERSION USER_SW_VER
#define PRODECT_ID "8cisxkqalq9jevn8"

/* Exported functions ------------------------------------------------------- */

/**
 * @Function: device_init
 * @Description: 设备初始化处理
 * @Input: none
 * @Output: none
 * @Return: OPRT_OK: success  Other: fail
 * @Others: none
 */
_TUYA_DEVICE_EXT \
OPERATE_RET device_init(VOID);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* tuya_device.h */
