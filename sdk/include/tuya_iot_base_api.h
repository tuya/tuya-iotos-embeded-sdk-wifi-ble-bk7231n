/*
tuya_iot_base_api.h
Copyright(C),2018-2020, 涂鸦科技 www.tuya.comm
*/

#ifndef __TUYA_IOT_BASE_API_H
#define __TUYA_IOT_BASE_API_H

#include "tuya_cloud_types.h"
#include "tuya_cloud_com_defs.h"
#include "tuya_cloud_base_defs.h"
#include "tuya_iot_com_api.h"
#include "tuya_os_adapter.h"
#ifdef __cplusplus
extern "C" {
#endif

#ifdef  __TUYA_IOT_BASE_API_GLOBALS
    #define __TUYA_IOT_BASE_API_EXT
#else
    #define __TUYA_IOT_BASE_API_EXT extern
#endif


/***********************************************************
*  Function: tuya_iot_set_gw_prod_info
*  Desc:     set tuya-sdk product info (wired version)
*  Input:    prod_info: tuya-sdk product info
*  Return:   OPRT_OK: success  Other: fail
***********************************************************/
__TUYA_IOT_BASE_API_EXT \
OPERATE_RET tuya_iot_set_gw_prod_info(IN CONST GW_PROD_INFO_S *prod_info);

/***********************************************************
*  Function: tuya_iot_mcu_dev_init
*  Desc:     init tuya-sdk (wired version, [mcu] mode)
*  Input:    cbs: tuya-sdk user callbacks
*  Input:    product_key: product key
*  Input:    sw_ver: netlink module version.format:xx.xx.xx (0<=x<=9)
*  Input:    mcu_sw_ver: mcu version format:xx.xx.xx (0<=x<=9)
*  Return:   OPRT_OK: success  Other: fail
***********************************************************/
__TUYA_IOT_BASE_API_EXT \
OPERATE_RET tuya_iot_mcu_dev_init(IN CONST TY_IOT_CBS_S *cbs,IN CONST CHAR_T *product_key, IN CONST CHAR_T *p_firmware_key,\
                                            IN CONST CHAR_T *sw_ver,IN CONST CHAR_T *mcu_sw_ver);

/***********************************************************
*  Function: tuya_iot_soc_init
*  Desc:     init tuya-sdk (wired version, [soc] mode)
*  Input:    cbs: tuya-sdk user callbacks
*  Input:    product_key: product key
*  Input:    sw_ver: soc version.format:xx.xx.xx (0<=x<=9)
*  Return:   OPRT_OK: success  Other: fail
***********************************************************/
#define tuya_iot_soc_init(cbs, product_key, sw_ver) \
        tuya_iot_soc_init_param(cbs, product_key, NULL, sw_ver)
__TUYA_IOT_BASE_API_EXT \
OPERATE_RET tuya_iot_soc_init_param(IN CONST TY_IOT_CBS_S *cbs,IN CONST CHAR_T *product_key,IN CONST CHAR_T *firmware_key,\
                                      IN CONST CHAR_T *sw_ver);

/***********************************************************
*  Function: tuya_iot_gw_init
*  Desc:     init tuya-sdk (wired version, [pure gateway] mode)
*  Input:    cbs: tuya-sdk user callbacks
*  Input:    gw_cbs: tuya-sdk sub-device management callbacks
*  Input:    product_key: product key
*  Input:    sw_ver: gateway version.format:xx.xx.xx (0<=x<=9)
*  Input:    attr && attr_num: gateway attributes attr array
*  Return:   OPRT_OK: success  Other: fail
***********************************************************/
__TUYA_IOT_BASE_API_EXT \
OPERATE_RET tuya_iot_gw_init(IN CONST TY_IOT_CBS_S *cbs,IN CONST TY_IOT_GW_CBS_S *gw_cbs,\
                                     IN CONST CHAR_T *product_key,IN CONST CHAR_T *sw_ver,\
                                     IN CONST GW_ATTACH_ATTR_T *attr,IN CONST UINT_T attr_num);

/***********************************************************
*  Function: tuya_iot_gw_dev_init
*  Desc:     init tuya-sdk (wired version, [gateway+inner_dev] mode)
*  Input:    cbs: tuya-sdk user callbacks
*  Input:    gw_cbs: tuya-sdk sub-device management callbacks
*  Input:    product_key: product key
*  Input:    sw_ver: gateway version.format:xx.xx.xx (0<=x<=9)
*  Input:    attr && attr_num: gateway attributes attr array
*  Return:   OPRT_OK: success  Other: fail
***********************************************************/
__TUYA_IOT_BASE_API_EXT \
OPERATE_RET tuya_iot_gw_dev_init(IN CONST TY_IOT_CBS_S *cbs,IN CONST TY_IOT_GW_CBS_S *gw_cbs,\
                                           IN CONST CHAR_T *product_key,IN CONST CHAR_T *sw_ver,\
                                           IN CONST GW_ATTACH_ATTR_T *attr,IN CONST UINT_T attr_num);

/***********************************************************
*  Function: tuya_iot_gw_unactive
*  Desc:     unactive this hardware from tuya-cloud
*  Return:   OPRT_OK: success  Other: fail
***********************************************************/
__TUYA_IOT_BASE_API_EXT \
OPERATE_RET tuya_iot_gw_unactive(VOID);

/***********************************************************
*  Function: tuya_iot_gw_reset
*  Desc:     reset this hardware from tuya-cloud
*  Return:   OPRT_OK: success  Other: fail
***********************************************************/
__TUYA_IOT_BASE_API_EXT \
OPERATE_RET tuya_iot_gw_reset(VOID);

#if (defined(WIFI_GW) && (WIFI_GW==0)) || (defined(GW_SUPPORT_WIRED_WIFI) && (GW_SUPPORT_WIRED_WIFI==1))
/***********************************************************
*  Function: tuya_iot_reg_get_nw_stat_cb
*  Desc:     enable tuya-sdk network check. sdk will check the
*            network every 1 second. if network change, sdk
*            will notify by calling nw_stat_cb
*  Input:    nw_stat_cb: network change callback
*  Return:   OPRT_OK: success  Other: fail
***********************************************************/
#define tuya_iot_reg_get_nw_stat_cb(nw_stat_cb) \
    tuya_iot_reg_get_nw_stat_cb_params(nw_stat_cb, 1);
__TUYA_IOT_BASE_API_EXT \
OPERATE_RET tuya_iot_reg_get_nw_stat_cb_params(IN CONST GET_NW_STAT_CB nw_stat_cb, IN CONST INT_T min_interval_s);
#endif
#ifdef __cplusplus
}
#endif
#endif

