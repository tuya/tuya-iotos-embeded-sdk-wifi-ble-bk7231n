/**
 * @file: tuya_device.h
 * @author: wuls@tuya.com
 * @brief: 应用设备管理模块，用于填写固件key、产品key等
 * @history: 
 *          2020-12-07: style modify
 *          2019-05-22: create
 * @Copyright (c) tuya.inc 2020
 */
#ifndef _TUYA_DEVICE_H
#define _TUYA_DEVICE_H

#include "tuya_cloud_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _TUYA_DEVICE_GLOBAL
    #define _TUYA_DEVICE_EXT 
#else
    #define _TUYA_DEVICE_EXT extern
#endif

/***********************************************************
*************************micro define***********************
***********************************************************/

// oem firmware
#define PRODUCT_KEY     "yzj1ixsujkklblq9"      // 产品key 


/**
 * @brief: 设备初始化前置准备工作
 * @param: none
 * Note: 
 *		to initialize device before device_init
 * Return: none
 */
_TUYA_DEVICE_EXT VOID pre_device_init(VOID);

/**
 * @brief: 应用常规初始化程序, 被user_main调用
 * @param: none
 * @retval: none
 */
_TUYA_DEVICE_EXT VOID app_init(VOID);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
