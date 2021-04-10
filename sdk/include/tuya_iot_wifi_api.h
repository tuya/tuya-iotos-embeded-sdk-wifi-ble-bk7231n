/*!
\file tuya_iot_wifi_api.h
Copyright(C),2017, 涂鸦科技 www.tuya.comm
*/

#ifndef _TUYA_IOT_WIFI_API_H
#define _TUYA_IOT_WIFI_API_H

#ifdef __cplusplus
extern "C" {
#endif

#include "tuya_cloud_types.h"
#include "tuya_cloud_com_defs.h"
#include "tuya_cloud_wifi_defs.h"
#include "tuya_iot_com_api.h"

#ifdef  __TUYA_IOT_WIFI_API_GLOBALS
    #define __TUYA_IOT_WIFI_API_EXT
#else
    #define __TUYA_IOT_WIFI_API_EXT extern
#endif

/***********************************************************
*************************micro define***********************
***********************************************************/

/***********************************************************
*************************variable define********************
***********************************************************/

/***********************************************************
*************************function define********************
***********************************************************/
/***********************************************************
*  Function: tuya_iot_set_wf_gw_prod_info
*  Input: wf_prod_info
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
__TUYA_IOT_WIFI_API_EXT \
OPERATE_RET tuya_iot_set_wf_gw_prod_info(IN CONST WF_GW_PROD_INFO_S *wf_prod_info);

/***********************************************************
*  Function: tuya_iot_set_wf_gw_debug_info  安全版本调试用的产测信息写入
*  Input: wf_prod_info
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
__TUYA_IOT_WIFI_API_EXT \
OPERATE_RET tuya_iot_set_wf_gw_debug_info(IN CONST DEBUG_GW_PROD_INFO_S *debug_info);

/***********************************************************
*  Function: tuya_iot_wf_timeout_set
*  Desc:     set wifi timeout
*  Input:    time in sec
*  Note: must call first
***********************************************************/
VOID tuya_iot_wf_timeout_set(IN CONST UINT_T timeout);

/***********************************************************
*  Function: tuya_iot_wf_dev_init->tuya iot virtual initialization
*  Input: cfg
*         cbs->tuya wifi sdk user callbacks
*         product_key->product key/proudct id,get from tuya open platform
*         wf_sw_ver->wifi module software version format:xx.xx.xx (0<=x<=9)
*         attr
*         attr_num
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
__TUYA_IOT_WIFI_API_EXT \
OPERATE_RET tuya_iot_wf_dev_init(IN CONST GW_WF_CFG_MTHD_SEL cfg, IN CONST GW_WF_START_MODE start_mode,
                                IN CONST TY_IOT_CBS_S *cbs, IN CONST CHAR_T *firmware_key,
                                IN CONST CHAR_T *product_key, IN CONST CHAR_T *wf_sw_ver,
                                IN CONST GW_ATTACH_ATTR_T *attr, IN CONST UINT_T attr_num);

/***********************************************************
*  Function: tuya_iot_wf_mcu_dev_init->The device consists of MCU and WiFi
*  Input: cfg
*         cbs->tuya wifi sdk user callbacks
*         product_key->product key/proudct id,get from tuya open platform
*         wf_sw_ver->wifi module software version format:xx.xx.xx (0<=x<=9)
*         mcu_sw_ver->mcu software version format:xx.xx.xx (0<=x<=9)
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
__TUYA_IOT_WIFI_API_EXT \
OPERATE_RET tuya_iot_wf_mcu_dev_init(IN CONST GW_WF_CFG_MTHD_SEL cfg, IN CONST GW_WF_START_MODE start_mode,
                                     IN CONST TY_IOT_CBS_S *cbs, IN CONST CHAR_T *p_firmware_key,
                                     IN CONST CHAR_T *product_key,IN CONST CHAR_T *wf_sw_ver,IN CONST CHAR_T *mcu_sw_ver);

/***********************************************************
*  Function: tuya_iot_wf_soc_init->The devcie consists of wifi soc
*  Input: cfg
*         cbs->tuya wifi sdk user callbacks,note cbs->dev_ug_cb is useless
*         product_key->product key/proudct id,get from tuya open platform
*         wf_sw_ver->wifi module software version format:xx.xx.xx (0<=x<=9)
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
#define tuya_iot_wf_soc_dev_init(cfg, start_mode,cbs,product_key,wf_sw_ver) \
        tuya_iot_wf_soc_dev_init_param(cfg, start_mode,cbs,NULL,product_key,wf_sw_ver)

__TUYA_IOT_WIFI_API_EXT \
OPERATE_RET tuya_iot_wf_soc_dev_init_param(IN CONST GW_WF_CFG_MTHD_SEL cfg, IN CONST GW_WF_START_MODE start_mode,
                                     IN CONST TY_IOT_CBS_S *cbs,IN CONST CHAR_T *firmware_key,
                                     IN CONST CHAR_T *product_key,IN CONST CHAR_T *wf_sw_ver);

/***********************************************************
*  Function: tuya_iot_wf_gw_init->tuya iot entity gateway initialization
*  Input: cfg
*         cbs->tuya wifi sdk user callbacks
*         gw_cbs->tuya gw user callbacks
*         product_key->product key/proudct id,get from tuya open platform
*         wf_sw_ver->wifi module software version format:xx.xx.xx (0<=x<=9)
*         attr->gw attr
*         attr_num
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
#define tuya_iot_wf_gw_init(cfg, start_mode, cbs, gw_cbs, product_key,wf_sw_ver, attr, attr_num) \
        tuya_iot_wf_gw_init_param(cfg, start_mode, cbs, gw_cbs, NULL, product_key, wf_sw_ver, attr, attr_num)

__TUYA_IOT_WIFI_API_EXT \
OPERATE_RET tuya_iot_wf_gw_init_param(IN CONST GW_WF_CFG_MTHD_SEL cfg, IN CONST GW_WF_START_MODE start_mode,
                                IN CONST TY_IOT_CBS_S *cbs, IN CONST TY_IOT_GW_CBS_S *gw_cbs,
                                IN CONST CHAR_T *firmware_key, IN CONST CHAR_T *product_key, IN CONST CHAR_T *wf_sw_ver,
                                IN CONST GW_ATTACH_ATTR_T *attr, IN CONST UINT_T attr_num);

/***********************************************************
*  Function: tuya_iot_wf_gw_dev_init->tuya iot entity gateway initialization
*                                     the gateway make as a device meanwhile
*  Input: cfg
*         cbs->tuya wifi sdk user callbacks
*         gw_cbs->tuya gw user callbacks
*         product_key->product key/proudct id,get from tuya open platform
*         wf_sw_ver->wifi module software version format:xx.xx.xx (0<=x<=9)
*         attr->gw attr
*         attr_num
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
#define tuya_iot_wf_gw_dev_init(cfg, start_mode, cbs, gw_cbs, product_key,wf_sw_ver, attr, attr_num) \
        tuya_iot_wf_gw_dev_init_param(cfg, start_mode, cbs, gw_cbs, NULL, product_key, wf_sw_ver, attr, attr_num)

__TUYA_IOT_WIFI_API_EXT \
OPERATE_RET tuya_iot_wf_gw_dev_init_param(IN CONST GW_WF_CFG_MTHD_SEL cfg,  IN CONST GW_WF_START_MODE start_mode,
                                    IN CONST TY_IOT_CBS_S *cbs, IN CONST TY_IOT_GW_CBS_S *gw_cbs,
                                    IN CONST CHAR_T *firmware_key, IN CONST CHAR_T *product_key, IN CONST CHAR_T *wf_sw_ver,
                                    IN CONST GW_ATTACH_ATTR_T *attr, IN CONST UINT_T attr_num);

/***********************************************************
*  Function: tuya_iot_reg_get_wf_nw_stat_cb
*  Input: wf_nw_stat_cb
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
#define tuya_iot_reg_get_wf_nw_stat_cb(wf_nw_stat_cb) \
    tuya_iot_reg_get_wf_nw_stat_cb_params(wf_nw_stat_cb, 1)
__TUYA_IOT_WIFI_API_EXT \
OPERATE_RET tuya_iot_reg_get_wf_nw_stat_cb_params(IN CONST GET_WF_NW_STAT_CB wf_nw_stat_cb, IN CONST INT_T min_interval_s);


/***********************************************************
*  Function: tuya_iot_set_wf_cfg_err_code_cb
*  Input: wf_nw_stat_cb
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET tuya_iot_set_wf_cfg_err_code_cb(IN CONST WF_NW_CFG_ERR_CODE_CB wf_nw_cfg_err_code_cb);

/***********************************************************
*  Function: tuya_iot_wf_gw_unactive
*  Input: none
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
__TUYA_IOT_WIFI_API_EXT \
OPERATE_RET tuya_iot_wf_gw_unactive(VOID);

/***********************************************************
*  Function: tuya_iot_wf_gw_reset
*  Input: none
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
__TUYA_IOT_WIFI_API_EXT \
OPERATE_RET tuya_iot_wf_gw_reset(VOID);
/***********************************************************
*  Function: tuya_iot_wf_gw_fast_unactive
*  Input: none
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
__TUYA_IOT_WIFI_API_EXT \
OPERATE_RET tuya_iot_wf_gw_fast_unactive(IN CONST GW_WF_CFG_MTHD_SEL mthd,IN CONST GW_WF_START_MODE wifi_start_mode);


/***********************************************************
*  Function: tuya_iot_wf_gw_fast_unactive
*  Input: none
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET tuya_iot_wf_fast_get_nc_type(GW_WF_NWC_FAST_STAT_T *nc_type);



/***********************************************************
*  Function: tuya_iot_set_user_def_ap_if
*  Input: ssid passwd
*  Output: none
*  Return: OPERATE_RET
*  Note:need call before tuya_iot_wf_xxx_init
***********************************************************/
__TUYA_IOT_WIFI_API_EXT \
OPERATE_RET tuya_iot_set_user_def_ap_if(IN CONST CHAR_T *ssid,IN CONST CHAR_T *passwd);

/***********************************************************
*  Function: tuya_iot_get_dev_ap_if
*  Iutput: none
*  Onput: ssid passwd
*  Return: OPERATE_RET
*  Note:need call before tuya_iot_wf_xxx_init
***********************************************************/
__TUYA_IOT_WIFI_API_EXT \
OPERATE_RET tuya_iot_get_dev_ap_if(OUT CHAR_T *ssid,OUT CHAR_T *passwd);

/***********************************************************
*  Function: tuya_iot_gw_wf_user_cfg
*  Input: ssid passwd token
*  Output: none
*  Return: OPERATE_RET
*..Note:use to TY_APP_CFG_WF mode
***********************************************************/
__TUYA_IOT_WIFI_API_EXT \
OPERATE_RET tuya_iot_gw_wf_user_cfg(IN CONST CHAR_T *ssid,IN CONST CHAR_T *passwd,IN CONST CHAR_T *token);

/***********************************************************
*  Function: tuya_iot_set_max_sta_conn
*  Input: max_conn
*  Output: none
*  Return: VOID
*  Note:need call before tuya_iot_wf_xxx_init
***********************************************************/
__TUYA_IOT_WIFI_API_EXT \
VOID tuya_iot_set_max_sta_conn(IN CONST UCHAR_T max_conn);

/***********************************************************
*  Function: tuya_iot_set_nw_ip
*  Input: ip mask gw
*  Output: none
*  Return: VOID
*  Note:need call before tuya_iot_wf_xxx_init
***********************************************************/
__TUYA_IOT_WIFI_API_EXT \
VOID tuya_iot_set_nw_ip(IN CONST CHAR_T* ip, IN CONST CHAR_T* mask, IN CONST CHAR_T* gw);

/***********************************************************
*  Function: mf_kv_init
*  Input: none
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET mf_kv_init(VOID);

#ifdef __cplusplus
}
#endif

#endif  /*_TUYA_IOT_API_H*/

