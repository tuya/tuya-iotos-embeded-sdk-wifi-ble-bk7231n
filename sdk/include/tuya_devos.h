/**
 * @file tuya_sdk.h
 * @author maht@tuya.com
 * @brief SDK通用流程管理，SDK对象管理模块
 * @version 0.1
 * @date 2019-08-28
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef __TUYA_DEV_H__
#define __TUYA_DEV_H__


#include "tuya_cloud_com_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    BOOL_T is_udf_active_url;       // 是否支持用户自定义active接口
    CHAR_T *p_active_api;           // 用户自定义active接口名称
    CHAR_T *p_active_ver;           // 用户自定义active接口版本
}ty_devos_udf_active_t;

typedef struct {
    BOOL_T is_ignore_upgrade;
    ty_devos_udf_active_t udf_active;
    // TBD
}ty_devos_udf_cfg_t;

typedef struct {
    ty_devos_udf_cfg_t udf_cfg;
}ty_devos_t;

int devos_set_udf_ignore_upgrade(const BOOL_T is_ignore);
BOOL_T devos_get_udf_ignore_upgrade();
int devos_set_udf_active(CHAR_T *active_api, CHAR_T *active_ver);
BOOL_T devos_get_udf_active(CHAR_T **active_api, CHAR_T **active_ver);


#ifdef __cplusplus
}
#endif


#endif
