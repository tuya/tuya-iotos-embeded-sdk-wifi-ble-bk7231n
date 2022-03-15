/**
* @file tuya_device.h
* @author www.tuya.com
* @brief template demo for SDK WiFi & BLE for BK7231N, hardware: BK7231N NANO SoC board
* @version 0.2
* @date 2022-03-14
*
* @copyright Copyright (c) tuya.inc 2022
*
*/

#ifndef __TUYA_DEVICE_H__
#define __TUYA_DEVICE_H__

#include "tuya_cloud_com_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _TUYA_DEVICE_GLOBAL
    #define _TUYA_DEVICE_EXT 
#else
    #define _TUYA_DEVICE_EXT extern
#endif /* _TUYA_DEVICE_GLOBAL */ 

/***********************************************************
*************************micro define***********************
***********************************************************/
/* device information define */
#define DEV_SW_VERSION USER_SW_VER
#define PRODUCT_ID "8cisxkqalq9jevn8"

/***********************************************************
***********************typedef define***********************
***********************************************************/


/***********************************************************
***********************variable define**********************
***********************************************************/


/***********************************************************
***********************function define**********************
***********************************************************/


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /*__TUYA_DEVICE_H__*/
