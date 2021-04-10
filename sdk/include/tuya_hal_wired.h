/**
 * @file tuya_hal_wired.h
 * @brief 网络设备操作接口
 * 
 * @copyright Copyright(C),2018-2020, 涂鸦科技 www.tuya.com
 * 
 */

#ifndef __TUYA_HAL_WIRED_H__
#define __TUYA_HAL_WIRED_H__


#include <stdbool.h>
#include "tuya_hal_network.h"


#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief get wired ethernet ip info
 * 
 * @param[out]      ip          the ip addr info
 * @return  OPRT_OK: success  Other: fail
 */
OPERATE_RET tuya_hal_wired_get_ip(NW_IP_S *ip);

/**
 * @brief return whether the hardware is connect to internet
 * 
 * @return      true            connected
 * @return      false           disconnected
 */
BOOL_T tuya_hal_wired_station_conn(VOID);

/**
 * @brief get wired ethernet mac info
 * 
 * @param[out]      mac         the mac info
 * @return  OPRT_OK: success  Other: fail
 */
OPERATE_RET tuya_hal_wired_get_mac(NW_MAC_S *mac);

/**
 * @brief set wired ethernet mac info
 * 
 * @param[in]       mac         the mac info
 * @return  OPRT_OK: success  Other: fail
 */
OPERATE_RET tuya_hal_wired_set_mac(const NW_MAC_S *mac);

/**
 * @brief if the hardware has a wifi interface, and user
 *        want to connect wifi. tuya-app will notify current
 *        wifi ssid and passwd,and tuya-sdk will nofity
 *        user with this function. user should implement
 *        wifi connect process in this function.
 * 
 * @param[in]       ssid
 * @param[in]       passwd
 * @return  OPRT_OK: success  Other: fail
 */
OPERATE_RET tuya_hal_wired_wifi_set_station_connect(const char *ssid, const char *passwd);

/**
 * @brief if the hardware has a wifi interface, and user
 *        want to connect wifi, then return true.
 *        if the hardware does not have a wifi interface,
 *        or user do not want to connect wifi,return false.
 * 
 * @return      true        need to connect wifi
 * @return      false       ignore wifi
 */
BOOL_T tuya_hal_wired_wifi_need_cfg(VOID);

/**
 * @brief if the hardware has a wifi interface and wifi is
 *        connected,return wifi rssi. or return 99
 * 
 * @param[out]      rssi        the return rssi.
 * @return  OPRT_OK: success  Other: fail
 */
OPERATE_RET tuya_hal_wired_wifi_station_get_conn_ap_rssi(int8_t *rssi);

/**
 * @brief get network status
 * 
 * @param[in]       stat        the status of network
 * @return  OPRT_OK: success  Other: fail
 */
OPERATE_RET tuya_hal_wired_get_nw_stat(GW_BASE_NW_STAT_T *stat);

/**
 * @brief wired connect status
 * 
 * @param[in]       stat        the status of network
 * @return  OPRT_OK: success  Other: fail
 */
OPERATE_RET tuya_hal_wired_if_connect_internet(BOOL_T *stat);

VOID_T tuya_os_wired_intf_init(VOID_T);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif



