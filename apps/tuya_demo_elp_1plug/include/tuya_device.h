/*
  Copyright(C),2018-2020, tuya inc, www.tuya.comm
 * @File name: tuya_demo.h
 * @Author: caojq
 * @Version: V1.0.0
 * @Date: 2020-10-26 17:17:08
 * @Description: 
 * @Others: 
 * @LastEditTime: 2020-10-26 21:56:14
 * @LastEditors:  
 */

#ifndef __TUYA_DEVICE_H
#define __TUYA_DEVICE_H

#include "tuya_cloud_error_code.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __TUYA_DEVICE_GLOBAL
    #define __TUYA_DEVICE_EXT
#else 
    #define __TUYA_DEVICE_EXT extern
#endif

#define PRODUCT_KEY "2gway3dzgdunmcao"  /*device product key*/ 
#define DEV_SW_VERSION USER_SW_VER      /*device version*/

/**
 * @Function: device_init
 * @Description: 
 * @Input: none
 * @Output: none
 * @Return: init result 
 * @Others: 
 */
__TUYA_DEVICE_EXT \
OPERATE_RET device_init(VOID);

#ifdef __cplusplus
}
#endif
#endif