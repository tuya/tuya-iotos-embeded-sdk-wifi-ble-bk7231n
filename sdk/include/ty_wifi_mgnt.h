/***********************************************************
*  File: wifi_hwl.h
*  Author: nzy
*  Date: 20170914
***********************************************************/
#ifndef _WIFI_MGNT_H
#define _WIFI_MGNT_H

#include "tuya_cloud_types.h"
#include "tuya_hal_network.h"
#include "tuya_hal_wifi.h"


#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIFI_MGNT_GLOBAL
    #define  _WIFI_MGNT_EXT
#else
    #define  _WIFI_MGNT_EXT extern
#endif

/***********************************************************
*************************micro define***********************
***********************************************************/
#define PROBE_SSID "tuya_smart"


/***********************************************************
*************************variable define********************
***********************************************************/



/***********************************************************
*************************function define********************
***********************************************************/
/***********************************************************
*  Function: ty_wf_send_probe_request_mgnt
*  Input: in_buf in_len srcmac dstmac
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
_WIFI_MGNT_EXT \
OPERATE_RET ty_wf_send_probe_request_mgnt(IN CONST UCHAR_T *in_buf, IN CONST UINT_T in_len,IN CONST UCHAR_T* ssid,NW_MAC_S *srcmac,NW_MAC_S *dstmac);

OPERATE_RET ty_wf_send_probe_response_mgnt(IN CONST UCHAR_T *in_buf, IN CONST UINT_T in_len,IN CONST UCHAR_T* ssid,NW_MAC_S *srcmac,NW_MAC_S *dstmac);

/***********************************************************
*  Function: ty_wf_register_recv_mgnt_callback
*  Input: enbale recv_cb
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
_WIFI_MGNT_EXT \
OPERATE_RET ty_wf_register_recv_mgnt_callback(BOOL_T enbale,WIFI_REV_MGNT_CB recv_cb);

/***********************************************************
*  Function: OPERATE_RET ty_wf_send_beacon
*  Input:  ssid chan src_mac
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
_WIFI_MGNT_EXT \
OPERATE_RET ty_wf_send_beacon(UCHAR_T* ssid, UCHAR_T chan, BYTE_T* src_mac, BYTE_T* vsie_data, UINT_T vsie_data_len);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif



