/**
* @file tuya_device.h
* @author www.tuya.com
* @brief Device initialization process processing module
* @version 0.2
* @date 2022-03-25
* @history: 
*
* @copyright Copyright (c) tuya.inc 2022
*
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
