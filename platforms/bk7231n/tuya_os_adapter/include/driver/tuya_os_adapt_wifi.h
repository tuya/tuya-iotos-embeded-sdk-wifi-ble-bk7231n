/**
 * @file tuya_os_adapt_wifi.h
 * @brief WIFI设备操作接口
 * 
 * @copyright Copyright(C),2018-2020, 涂鸦科技 www.tuya.com
 * 
 */

#ifndef __TUYA_OS_ADAPT_WIFI_H__
#define __TUYA_OS_ADAPT_WIFI_H__


#include <stdbool.h>
#include "tuya_os_adapt_network.h"
#include "tuya_os_adapter.h"


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief scan current environment and obtain all the ap
 *        infos in current environment
 * 
 * @param[out]      ap_ary      current ap info array
 * @param[out]      num         the num of ar_ary
 * @return  OPRT_OS_ADAPTER_OK: success  Other: fail
 */
int tuya_os_adapt_wifi_all_ap_scan(AP_IF_S **ap_ary, unsigned int *num);

/**
 * @brief scan current environment and obtain the specific
 *        ap info.
 * 
 * @param[in]       ssid        the specific ssid
 * @param[out]      ap          the ap info
 * @return  OPRT_OS_ADAPTER_OK: success  Other: fail
 */
int tuya_os_adapt_wifi_assign_ap_scan(const signed char *ssid, AP_IF_S **ap);

/**
 * @brief release the memory malloced in <tuya_os_adapt_wifi_all_ap_scan>
 *        and <tuya_os_adapt_wifi_assign_ap_scan> if needed. tuya-sdk
 *        will call this function when the ap info is no use.
 * 
 * @param[in]       ap          the ap info
 * @return  OPRT_OS_ADAPTER_OK: success  Other: fail
 */
int tuya_os_adapt_wifi_release_ap(AP_IF_S *ap);

/**
 * @brief set wifi interface work channel
 * 
 * @param[in]       chan        the channel to set
 * @return  OPRT_OS_ADAPTER_OK: success  Other: fail
 */
int tuya_os_adapt_wifi_set_cur_channel(const unsigned char chan);

/**
 * @brief get wifi interface work channel
 * 
 * @param[out]      chan        the channel wifi works
 * @return  OPRT_OS_ADAPTER_OK: success  Other: fail
 */
int tuya_os_adapt_wifi_get_cur_channel(unsigned char *chan);

/**
 * @brief enable / disable wifi sniffer mode.
 *        if wifi sniffer mode is enabled, wifi recv from
 *        packages from the air, and user shoud send these
 *        packages to tuya-sdk with callback <cb>.
 * 
 * @param[in]       en          enable or disable
 * @param[in]       cb          notify callback
 * @return  OPRT_OK: success  Other: fail
 */
int tuya_os_adapt_wifi_sniffer_set(const BOOL_T en, const SNIFFER_CALLBACK cb);

/**
 * @brief get wifi ip info.when wifi works in
 *        ap+station mode, wifi has two ips.
 * 
 * @param[in]       wf          wifi function type
 * @param[out]      ip          the ip addr info
 * @return  OPRT_OS_ADAPTER_OK: success  Other: fail
 */
int tuya_os_adapt_wifi_get_ip(const WF_IF_E wf, NW_IP_S *ip);

/**
 * @brief set wifi mac info.when wifi works in
 *        ap+station mode, wifi has two macs.
 * 
 * @param[in]       wf          wifi function type
 * @param[in]       mac         the mac info
 * @return  OPRT_OS_ADAPTER_OK: success  Other: fail
 */
int tuya_os_adapt_wifi_set_mac(const WF_IF_E wf, const NW_MAC_S *mac);

/**
 * @brief get wifi mac info.when wifi works in
 *        ap+station mode, wifi has two macs.
 * 
 * @param[in]       wf          wifi function type
 * @param[out]      mac         the mac info
 * @return  OPRT_OS_ADAPTER_OK: success  Other: fail
 */
int tuya_os_adapt_wifi_get_mac(const WF_IF_E wf, NW_MAC_S *mac);

/**
 * @brief set wifi work mode
 * 
 * @param[in]       mode        wifi work mode
 * @return  OPRT_OS_ADAPTER_OK: success  Other: fail
 */
int tuya_os_adapt_wifi_set_work_mode(const WF_WK_MD_E mode);

/**
 * @brief get wifi work mode
 * 
 * @param[out]      mode        wifi work mode
 * @return  OPRT_OS_ADAPTER_OK: success  Other: fail
 */
int tuya_os_adapt_wifi_get_work_mode(WF_WK_MD_E *mode);

/**
 * @brief start a soft ap
 * 
 * @param[in]       cfg         the soft ap config
 * @return  OPRT_OS_ADAPTER_OK: success  Other: fail
 */
int tuya_os_adapt_wifi_ap_start(const WF_AP_CFG_IF_S *cfg);

/**
 * @brief stop a soft ap
 * 
 * @return  OPRT_OS_ADAPTER_OK: success  Other: fail
 */
int tuya_os_adapt_wifi_ap_stop(void);

/**
 * @brief : get ap info for fast connect
 * @param[out]      fast_ap_info
 * @return  OPRT_OS_ADAPTER_OK: success  Other: fail
 */
int tuya_os_adapt_wifi_get_connected_ap_info_v2(FAST_WF_CONNECTED_AP_INFO_V2_S **fast_ap_info);

/**
 * @brief : fast connect
 * @param[in]      fast_ap_info
 * @return  OPRT_OS_ADAPTER_OK: success  Other: fail
 */
int tuya_os_adapt_wifi_fast_station_connect_v2(const FAST_WF_CONNECTED_AP_INFO_V2_S *fast_ap_info);

/**
 * @brief connect wifi with ssid and passwd
 * 
 * @param[in]       ssid
 * @param[in]       passwd
 * @return  OPRT_OS_ADAPTER_OK: success  Other: fail
 */
int tuya_os_adapt_wifi_station_connect(const signed char *ssid, const signed char *passwd);

/**
 * @brief disconnect wifi from connect ap
 * 
 * @return  OPRT_OS_ADAPTER_OK: success  Other: fail
 */
int tuya_os_adapt_wifi_station_disconnect(void);

/**
 * @brief get wifi connect rssi
 * 
 * @param[out]      rssi        the return rssi
 * @return  OPRT_OS_ADAPTER_OK: success  Other: fail
 */
int tuya_os_adapt_wifi_station_get_conn_ap_rssi(signed char *rssi);

/**
 * @brief get wifi bssid
 * 
 * @param[out]      mac         uplink mac
 * @return  OPRT_OS_ADAPTER_OK: success  Other: fail
 */
int tuya_os_adapt_wifi_get_bssid(unsigned char *mac);

/**
 * @brief get wifi station work status
 * 
 * @param[out]      stat        the wifi station work status
 * @return  OPRT_OS_ADAPTER_OK: success  Other: fail
 */
int tuya_os_adapt_wifi_station_get_status(WF_STATION_STAT_E *stat);

/**
 * @brief get wifi err status 
 * @param[in] none
 * @return last err status, 0 meaning no error
 */
int tuya_os_adapt_wifi_err_status_get(WF_STATION_STAT_E *stat);

/**
 * @brief set wifi country code
 * 
 * @param[in]       ccode  country code
 * @return  OPRT_OS_ADAPTER_OK: success  Other: fail
 */
int tuya_os_adapt_wifi_set_country_code(const COUNTRY_CODE_E ccode);

/**
 * @brief send wifi management
 * 
 * @param[in]       buf         pointer to buffer
 * @param[in]       len         length of buffer
 * @return  OPRT_OS_ADAPTER_OK: success  Other: fail
 */
int tuya_os_adapt_wifi_send_mgnt(const unsigned char *buf, const unsigned int len);

/**
 * @brief register receive wifi management callback
 * 
 * @param[in]       enable
 * @param[in]       recv_cb     receive callback
 * @return  OPRT_OS_ADAPTER_OK: success  Other: fail
 */
int tuya_os_adapt_wifi_register_recv_mgnt_callback(const BOOL_T enable, const WIFI_REV_MGNT_CB recv_cb);

/**
 * @brief get wifi rf param exist or not
 * 
 * @param[in]       none
 * @return  true: rf param exist  Other: fail
 */
bool_t tuya_os_adapt_wifi_rf_calibrated(void);

/**
 * @brief set wifi lowerpower mode
 * 
 * @param[in]       en
 * @param[in]       dtim
 * @return  OPRT_OS_ADAPTER_OK: success  Other: fail
 */
int tuya_os_adapt_set_wifi_lp_mode(const BOOL_T en, const unsigned char dtim);

OPERATE_RET tuya_os_adapt_reg_wifi_intf(void);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif



