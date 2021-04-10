/***********************************************************
*  File: base_nw_intf.h
*  Author: nzy
*  Date: 20170220
***********************************************************/
#ifndef _BASE_NW_INTF_H
#define _BASE_NW_INTF_H
#ifdef __cplusplus
	extern "C" {
#endif

#include "tuya_hal_network.h"
#include "tuya_hal_wired.h"

#ifdef  __BASE_NW_INTF_GLOBALS
    #define __BASE_NW_INTF_EXT
#else
    #define __BASE_NW_INTF_EXT extern
#endif

#ifndef IN
    #define IN
#endif

#ifndef OUT
    #define OUT
#endif

#ifndef INOUT
    #define INOUT
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
*  Function: bnw_get_ip 
*  Input: none
*  Output: ip
*  Return: OPERATE_RET
***********************************************************/
__BASE_NW_INTF_EXT \
int bnw_get_ip(OUT NW_IP_S *ip);

/***********************************************************
*  Function: bnw_station_conn 
*  Input: none
*  Output: none
*  Return: bool
***********************************************************/
__BASE_NW_INTF_EXT \
bool bnw_station_conn(void);

/***********************************************************
*  Function: bnw_set_station_connect
*  Input: ssid passwd
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
__BASE_NW_INTF_EXT \
int bnw_set_station_connect(IN const char *ssid,IN const char *passwd);

/***********************************************************
*  Function: bnw_need_wifi_cfg
*  Input: none
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
__BASE_NW_INTF_EXT \
bool bnw_need_wifi_cfg(void);

/***********************************************************
*  Function: bnw_station_get_conn_ap_rssi
*  Input: none
*  Output: rssi
*  Return: OPERATE_RET
*  Note: 仅有线返回99,无线+有线/无线，返回实际无线信号
***********************************************************/
__BASE_NW_INTF_EXT \
int bnw_station_get_conn_ap_rssi(OUT int8_t *rssi);

/***********************************************************
*  Function: bnw_get_mac
*  Input: none
*  Output: mac
*  Return: OPERATE_RET
***********************************************************/
__BASE_NW_INTF_EXT \
int bnw_get_mac(OUT NW_MAC_S *mac);

/***********************************************************
*  Function: bnw_set_mac
*  Input: mac
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
__BASE_NW_INTF_EXT \
int bnw_set_mac(IN const NW_MAC_S *mac);

/***********************************************************
*  Function: bnw_if_wired_connect_net
*  Input: status
*  Output: status
*  Return: OPERATE_RET
***********************************************************/
int bnw_if_wired_connect_net(bool *status);

#ifdef __cplusplus
}
#endif
#endif

