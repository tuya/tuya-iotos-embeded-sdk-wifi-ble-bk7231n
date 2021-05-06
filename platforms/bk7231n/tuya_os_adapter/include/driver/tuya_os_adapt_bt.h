/**
 * @file tuya_os_adapt_bt.h
 * @brief BLE操作接口
 * 
 * @copyright Copyright(C),2018-2020, 涂鸦科技 www.tuya.com
 * 
 */



#ifndef TUYA_OS_ADAPT_BT_H__
#define TUYA_OS_ADAPT_BT_H__


#include <stdbool.h>
#include <stdint.h>
#include "tuya_cloud_types.h"
#include "tuya_ble_type.h"
#include "tuya_os_adapter.h"

#if defined(TY_BT_MOD) && TY_BT_MOD == 1

/**
 * @brief tuya_os_adapt_bt 蓝牙初始化
 * @return OPERATE_RET 
 */
int tuya_os_adapt_bt_port_init(ty_bt_param_t *p);

/**
 * @brief tuya_os_adapt_bt 蓝牙断开关闭
 * @return OPERATE_RET 
 */
int tuya_os_adapt_bt_port_deinit(void);

/**
 * @brief tuya_os_adapt_bt 蓝牙断开
 * @return OPERATE_RET 
 */
int tuya_os_adapt_bt_gap_disconnect(void);

/**
 * @brief tuya_os_adapt_bt 蓝牙发送
 * @return OPERATE_RET 
 */
int tuya_os_adapt_bt_send(unsigned char *data, unsigned char len);

/**
 * @brief tuya_os_adapt_bt 广播包重置
 * @return OPERATE_RET 
 */
int tuya_os_adapt_bt_reset_adv(tuya_ble_data_buf_t *adv, tuya_ble_data_buf_t *scan_resp);

/**
 * @brief tuya_os_adapt_bt 获取rssi信号值
 * @return OPERATE_RET 
 */
int tuya_os_adapt_bt_get_rssi(signed char *rssi);

/**
 * @brief tuya_os_adapt_bt 停止广播
 * @return OPERATE_RET 
 */
int tuya_os_adapt_bt_start_adv(void);

/**
 * @brief tuya_os_adapt_bt 停止广播
 * @return OPERATE_RET 
 */
int tuya_os_adapt_bt_stop_adv(void);

/**
 * @brief tuya_os_adapt_bt 主动扫描
 * @return OPERATE_RET 
 */
int tuya_os_adapt_bt_assign_scan(IN OUT ty_bt_scan_info_t *info);

/**
 * @brief tuya_os_adapt_bt 接口注册
 * @return OPERATE_RET 
 */
OPERATE_RET tuya_os_adapt_reg_bt_intf(void);
/* add end */

#endif
#endif


