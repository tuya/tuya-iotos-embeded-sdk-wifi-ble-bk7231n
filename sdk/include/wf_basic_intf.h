/***********************************************************
*  File: wf_basic_intf.h 
*  Author: nzy
*  Date: 20170220
***********************************************************/
#ifndef _WF_BASIC_INTF_H
#define _WF_BASIC_INTF_H
#ifdef __cplusplus
	extern "C" {
#endif

#include "tuya_hal_wifi.h"
#include "tuya_cloud_types.h"
#include "tuya_hal_network.h"

#ifdef  __WF_BASIC_INTF_GLOBALS
    #define __WF_BASIC_INTF_EXT
#else
    #define __WF_BASIC_INTF_EXT extern
#endif

/***********************************************************
*************************macro define***********************
***********************************************************/

/***********************************************************
*************************variable define********************
***********************************************************/

/***********************************************************
*************************function define********************
***********************************************************/

/***********************************************************
*  Function: wf_all_ap_scan
*  Input: none
*  Output: ap_ary->scan ap info array
*          num->scan ap nums
*  Return: OPERATE_RET
***********************************************************/
__WF_BASIC_INTF_EXT \
OPERATE_RET wf_all_ap_scan(OUT AP_IF_S **ap_ary,OUT UINT_T *num);

/***********************************************************
*  Function: wf_assign_ap_scan
*  Input: ssid->assign ap ssid
*  Output: ap->ap info
*  Return: OPERATE_RET
***********************************************************/
__WF_BASIC_INTF_EXT \
OPERATE_RET wf_assign_ap_scan(IN CONST CHAR_T *ssid,OUT AP_IF_S **ap);

/***********************************************************
*  Function: wf_release_ap
*  Input: ap
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
__WF_BASIC_INTF_EXT \
OPERATE_RET wf_release_ap(IN AP_IF_S *ap);

/***********************************************************
*  Function: wf_set_cur_channel
*  Input: chan
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
__WF_BASIC_INTF_EXT \
OPERATE_RET wf_set_cur_channel(IN CONST BYTE_T chan);

/***********************************************************
*  Function: wf_get_cur_channel
*  Input: none
*  Output: chan
*  Return: OPERATE_RET
***********************************************************/
__WF_BASIC_INTF_EXT \
OPERATE_RET wf_get_cur_channel(OUT BYTE_T *chan);

/***********************************************************
*  Function: wf_sniffer_set
*  Input: en->TRUE/FALSE
*         cb->sniffer callback
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
__WF_BASIC_INTF_EXT \
OPERATE_RET wf_sniffer_set(IN CONST BOOL_T en,IN CONST SNIFFER_CALLBACK cb);

/***********************************************************
*  Function: wf_get_ip
*  Input: wf->WF_IF_E
*  Output: ip
*  Return: OPERATE_RET
***********************************************************/
__WF_BASIC_INTF_EXT \
OPERATE_RET wf_get_ip(IN CONST WF_IF_E wf,OUT NW_IP_S *ip);

/***********************************************************
*  Function: wf_get_mac
*  Input: wf->WF_IF_E
*  Output: mac
*  Return: OPERATE_RET
***********************************************************/
__WF_BASIC_INTF_EXT \
OPERATE_RET wf_get_mac(IN CONST WF_IF_E wf,OUT NW_MAC_S *mac);

/***********************************************************
*  Function: wf_set_mac
*  Input: wf->WF_IF_E 
*         mac
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
__WF_BASIC_INTF_EXT \
OPERATE_RET wf_set_mac(IN CONST WF_IF_E wf,IN CONST NW_MAC_S *mac);

/***********************************************************
*  Function: wf_wk_mode_set
*  Input: mode->WF_WK_MD_E
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
__WF_BASIC_INTF_EXT \
OPERATE_RET wf_wk_mode_set(IN CONST WF_WK_MD_E mode);

/***********************************************************
*  Function: wf_close_concurrent_ap
*  Input: none
*  Output: mode
*  Return: OPERATE_RET
***********************************************************/
__WF_BASIC_INTF_EXT \
OPERATE_RET wf_close_concurrent_ap(VOID);

/***********************************************************
*  Function: wf_wk_mode_get
*  Input: none
*  Output: mode
*  Return: OPERATE_RET
***********************************************************/
__WF_BASIC_INTF_EXT \
OPERATE_RET wf_wk_mode_get(OUT WF_WK_MD_E *mode);


#if defined(ENABLE_AP_FAST_CONNECT) && (ENABLE_AP_FAST_CONNECT==1)
/***********************************************************
*  Function: wf_get_connected_ap_info
*  Input: none
*  Output: fast_ap_infoone
*  Return: OPERATE_RET
***********************************************************/
__WF_BASIC_INTF_EXT \
OPERATE_RET wf_get_connected_ap_info(OUT FAST_WF_CONNECTED_AP_INFO_V2_S **fast_ap_info);

/***********************************************************
*  Function: wf_fast_station_connect
*  Input: fast_ap_info
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
__WF_BASIC_INTF_EXT \
OPERATE_RET wf_fast_station_connect(IN FAST_WF_CONNECTED_AP_INFO_V2_S *fast_ap_info);
#endif

/***********************************************************
*  Function: wf_station_connect
*  Input: ssid
*         passwd
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
__WF_BASIC_INTF_EXT \
OPERATE_RET wf_station_connect(IN CONST CHAR_T *ssid,IN CONST CHAR_T *passwd);

/***********************************************************
*  Function: wf_station_disconnect
*  Input: none
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
__WF_BASIC_INTF_EXT \
OPERATE_RET wf_station_disconnect(VOID);

/***********************************************************
*  Function: wf_station_get_conn_ap_rssi
*  Input: none
*  Output: rssi
*  Return: OPERATE_RET
***********************************************************/
__WF_BASIC_INTF_EXT \
OPERATE_RET wf_station_get_conn_ap_rssi(OUT SCHAR_T *rssi);

/***********************************************************
*  Function: wf_station_stat_get
*  Input: none
*  Output: stat
*  Return: OPERATE_RET
***********************************************************/
__WF_BASIC_INTF_EXT \
OPERATE_RET wf_station_stat_get(OUT WF_STATION_STAT_E *stat);

/***********************************************************
*  Function: wf_ap_start
*  Input: cfg
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
__WF_BASIC_INTF_EXT \
OPERATE_RET wf_ap_start(IN CONST WF_AP_CFG_IF_S *cfg);

/***********************************************************
*  Function: wf_ap_stop
*  Input: none
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
__WF_BASIC_INTF_EXT \
OPERATE_RET wf_ap_stop(VOID);

OPERATE_RET wf_get_ap_mac(INOUT NW_MAC_S *mac);

__WF_BASIC_INTF_EXT \
OPERATE_RET wf_set_country_code(IN CONST CHAR_T *p_country_code);
__WF_BASIC_INTF_EXT \
COUNTRY_CODE_E wf_get_country_code(IN CONST CHAR_T *p_country_code);
__WF_BASIC_INTF_EXT \
VOID_T tuya_wifi_set_lps_dtim(const unsigned int dtim);
__WF_BASIC_INTF_EXT \
OPERATE_RET tuya_wifi_lp_disable(VOID);
__WF_BASIC_INTF_EXT \
OPERATE_RET tuya_wifi_lp_enable(VOID);
__WF_BASIC_INTF_EXT \
OPERATE_RET tuya_cpu_lp_disable(VOID);
__WF_BASIC_INTF_EXT \
OPERATE_RET tuya_cpu_lp_enable(VOID);
__WF_BASIC_INTF_EXT \
BOOL_T wf_rf_calibrated(VOID_T);


#ifdef __cplusplus
}
#endif
#endif

