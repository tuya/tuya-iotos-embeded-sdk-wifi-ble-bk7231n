/**
 * @file tuya_cnt_rst.h
 * @brief 
 * @date 2019-04-25
 * 
 * copyright Copyright (c) {2018-2020} 涂鸦科技 www.tuya.com
 * 
 */
#ifndef _TUYA_CNT_RST_H_
#define _TUYA_CNT_RST_H_

#include "tuya_cloud_types.h"

typedef VOID(* TUYA_CNT_RST_INFORM_CB)(VOID);

/**
 * @brief 设置计数重置模式，需在pre_device_init中调用
 * 
 * @param rst_num 重置通知回调触发次数
 * @param cb 重置通知回调，如果设备连续电源重启次数达到rst_num，会在pre_app_init之后，app_init之前触发回调。
 * @return OPRT_OK 设置成功
 * @return other 设置失败
 */
OPERATE_RET tuya_set_cnt_rst_mode(IN CONST UINT8_T rst_num, IN CONST TUYA_CNT_RST_INFORM_CB cb);

#endif