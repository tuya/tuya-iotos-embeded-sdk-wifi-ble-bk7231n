/***********************************************************
*  File: iot_httpc.h
*  Author: nzy
*  Date: 20150527
***********************************************************/
#ifndef _IOT_HTTPC_H
#define _IOT_HTTPC_H

#include "tuya_cloud_types.h"
#include "ty_cJSON.h"
#include "gw_intf.h"
#include "http_inf.h"

#ifdef __cplusplus
    extern "C" {
#endif

OPERATE_RET httpc_gw_opeinfo_common_post_no_remalloc(IN CONST CHAR_T *url, IN CONST CHAR_T *api_name, IN CONST CHAR_T *api_ver, IN CONST CHAR_T *uuid, IN CONST CHAR_T *aes_key, IN CONST CHAR_T *gwid, IN CHAR_T *post_data, IN UINT_T data_total_len, IN CONST CHAR_T *p_head_other, OUT ty_cJSON **pp_result);
OPERATE_RET httpc_common_post_no_remalloc(IN CONST CHAR_T *api_name, IN CONST CHAR_T *api_ver, IN CONST CHAR_T *uuid, IN CONST CHAR_T *devid, IN CHAR_T *post_data, IN UINT_T data_total_len,IN CONST CHAR_T *p_head_other, OUT ty_cJSON **pp_result);
OPERATE_RET httpc_common_post(IN CONST CHAR_T *api_name, IN CONST CHAR_T *api_ver, IN CONST CHAR_T *uuid, IN CONST CHAR_T *devid, IN CHAR_T *post_data, IN CONST CHAR_T *p_head_other, OUT ty_cJSON **pp_result);

/* in workqueue */
OPERATE_RET http_gw_get_timer_cnt(IN CONST INT_T lastFetchTime, OUT ty_cJSON **result);

/* in workqueue */
OPERATE_RET http_gw_get_timer_content(IN CONST INT_T offset, IN CONST INT_T limit, OUT ty_cJSON **result);

/* in workqueue */
OPERATE_RET http_device_bind(IN CONST CHAR_T *dev_id, IN CONST CHAR_T *product_key, IN CONST CHAR_T *firmware_key,\
                                 IN CONST CHAR_T *sw_ver, IN CONST BOOL_T online, IN CONST DEV_TYPE_T type, \
                                 IN CONST GW_ATTACH_ATTR_T *attr, IN CONST UINT_T attr_num, IN CONST BOOL_T is_oem,\
                                 IN CONST UINT_T dev_attr,OUT ty_cJSON **result);
/*direct call*/
OPERATE_RET http_device_unbind_v41(IN CONST CHAR_T *dev_id);
/*direct call*/
OPERATE_RET http_device_reset_v10(IN CONST CHAR_T *dev_id);
/*direct call*/
OPERATE_RET http_device_dp_cache_query_v10(VOID);
/*direct call*/
OPERATE_RET http_device_dp_cache_push_v10(IN CONST CHAR_T *dev_id);
/* in workqueue */
OPERATE_RET http_device_update_versions_v41(IN CONST CHAR_T *dev_id, IN CONST CHAR_T *ver, IN CONST DEV_TYPE_T tp, \
                                            IN CONST GW_ATTACH_ATTR_T *attr, IN CONST UINT_T attr_num, IN CONST UINT_T dev_attr);
/* in workqueue */
OPERATE_RET http_gw_reset(VOID);
//OPERATE_RET http_gw_check_exist(VOID);
/* in workqueue */
OPERATE_RET http_iot_dns_get_cloud_url(VOID);
//OPERATE_RET http_gw_get_cloud_url(VOID);
OPERATE_RET http_iot_dns_get_fast_cloud_url(CHAR_T *env);

VOID http_iot_dns_cache_cloud_url(VOID);
OPERATE_RET http_iot_dns_get_host_ip(IN CONST CHAR_T *domain, OUT ty_cJSON **result);

//by pass
OPERATE_RET http_gw_register_V41(OUT ty_cJSON **result);
/* in workqueue */
OPERATE_RET http_get_dev_sigmesh_info_v10(IN CONST CHAR_T *sub_id, OUT ty_cJSON **result);
#if defined(ENABLE_SIGMESH) && (ENABLE_SIGMESH==1)
/*direct call*/
OPERATE_RET http_get_dev_sigmesh_free_node_list_v10(IN CONST INT_T node_num, OUT ty_cJSON **result);
/* in workqueue */
OPERATE_RET http_dev_sigmesh_join_v11(IN CONST CHAR_T *node_id, IN CONST CHAR_T *product_key, IN CONST CHAR_T *firmware_key, IN CONST CHAR_T *uuid, \
                                      IN CONST CHAR_T *sw_ver, IN CONST CHAR_T *mac, IN CONST CHAR_T *dev_key, IN CONST BOOL_T online, IN CONST DEV_TYPE_T type,\
                                      IN CONST GW_ATTACH_ATTR_T *attr, IN CONST UINT_T attr_num, IN CONST BOOL_T is_oem, OUT ty_cJSON **result);
OPERATE_RET http_dev_ble_mesh_join(IN CONST CHAR_T *node_id, IN CONST CHAR_T *product_key, IN CONST CHAR_T *firmware_key, IN CONST CHAR_T *uuid, \
                                                 IN CONST CHAR_T *sw_ver, IN CONST CHAR_T *mac, IN CONST CHAR_T *dev_key, IN CONST BOOL_T online, IN CONST CHAR_T *type,\
                                                    IN CONST GW_ATTACH_ATTR_T *attr, IN CONST UINT_T attr_num, IN CONST BOOL_T is_oem, USER_DEV_DTL_DEF_T uddd, \
                                                    OUT ty_cJSON **result);
OPERATE_RET http_get_dev_sigmesh_auth_key(IN CONST CHAR_T *uuid,OUT ty_cJSON **result);
OPERATE_RET http_get_dev_sigmesh_bind_status(IN CONST CHAR_T *uuid,IN CONST CHAR_T *encrypted_value,\
                                                            OUT ty_cJSON **result);
OPERATE_RET http_rept_dev_sig_ble_bind_status(IN CONST CHAR_T *node_id,IN BOOL_T bind_status);
OPERATE_RET http_rept_dev_sig_ble_bind_status_batch(IN CONST CHAR_T *data,IN INT_T len);
#endif
/* in workqueue */
OPERATE_RET http_gw_active_v41(OUT ty_cJSON **result);
/* in workqueue */
OPERATE_RET http_gw_update_version_v41(VOID);

/* ffs */
OPERATE_RET http_ffs_pre_active(IN CONST CHAR_T *wss_token, IN CONST CHAR_T*gw_endpoint);

typedef OPERATE_RET (*HTTP_GW_GET_FILE_DATA_CB)(IN PVOID_T priv_data, IN CONST UINT_T total_len,IN CONST UINT_T offset,\
                                            IN CONST BYTE_T *data,IN CONST UINT_T len,OUT UINT_T *remain_len);
/* in thread */
OPERATE_RET http_gw_download_file(IN CONST CHAR_T *url,IN CONST UINT_T mlk_buf_len,\
                                  IN CONST HTTP_GW_GET_FILE_DATA_CB gfd_cb, IN PVOID_T priv_data, \
                                  IN CONST UINT_T total_len, IN CONST BYTE_T *p_key, IN CONST UINT_T key_len, INOUT BYTE_T file_hmac[32]);

OPERATE_RET wechat_gw_set_cloud_url(VOID);
OPERATE_RET http_pskkey_get(OUT ty_cJSON **result);
/* in workqueue */
OPERATE_RET http_put_rst_log(VOID);
/* in workqueue */
OPERATE_RET http_get_self_fm_ug_info(IN CONST CHAR_T *sub_id, OUT ty_cJSON **result);

typedef enum {
    HTTP_DYNAMIC_CFG_ALL,//all
    HTTP_DYNAMIC_CFG_TZ, //time zone
    HTTP_DYNAMIC_CFG_RATERULE,//rate rule for dp
} HTTP_DYNAMIC_CFG_TYPE;

/* in workqueue */
OPERATE_RET http_dynamic_cfg_get(IN CONST HTTP_DYNAMIC_CFG_TYPE type, OUT ty_cJSON **result);
/* in workqueue */
//OPERATE_RET http_dynamic_cfg_ack(IN CONST CHAR_T *ackid);
OPERATE_RET http_dynamic_cfg_ack_v20(IN CONST CHAR_T* timezone_ackId, IN CONST CHAR_T* rateRule_actId);

/* in workqueue */
OPERATE_RET http_get_dev_list_v30(OUT ty_cJSON **result);
#if defined(ENABLE_SIGMESH) && (ENABLE_SIGMESH==1)
/*direct call*/
OPERATE_RET http_get_dev_sigmesh_source_node(OUT ty_cJSON **result);
/*direct call*/
OPERATE_RET http_dev_sigmesh_gateway_create_v10(OUT ty_cJSON **result);
#endif
/* in workqueue */
OPERATE_RET http_get_dev_list_v20(OUT ty_cJSON **result);
/* in workqueue */
OPERATE_RET http_device_update_skill(IN CONST CHAR_T *dev_id, IN CONST CHAR_T *skill);


#if defined(ENABLE_CLOUD_OPERATION) && (ENABLE_CLOUD_OPERATION==1)
/*direct call*/
OPERATE_RET httpc_domain_certs_get(OUT ty_cJSON **result, IN CONST CHAR_T *url_msg);
/*direct call*/
OPERATE_RET httpc_cloud_storage_cfg_get(OUT ty_cJSON **result, IN CONST BOOL_T withvideostorage);
/*direct call*/
OPERATE_RET httpc_cloud_storage_common_cfg_get(OUT ty_cJSON **result);
/*direct call*/
OPERATE_RET httpc_cloud_upload_complete(IN CONST UINT_T time, IN CONST CHAR_T *p_filetype, IN CONST CHAR_T *p_extend, IN CONST CHAR_T *p_bucket, IN CONST CHAR_T *p_file);
#endif

#if defined(ENABLE_LOCAL_LINKAGE) && (ENABLE_LOCAL_LINKAGE==1)
OPERATE_RET http_gw_get_linkage_content(IN CONST INT_T offset, IN CONST INT_T limit,IN CONST CHAR_T *ruleIds, IN CONST INT_T linkage_type, OUT ty_cJSON **result);
OPERATE_RET http_gw_update_linkage_rule_localize(IN CONST CHAR_T *local_ruleId, IN CONST INT_T linkage_type);
OPERATE_RET http_gw_linkage_rule_failback(IN CONST CHAR_T *ruleIds, IN CONST INT_T linkage_type, IN CONST INT_T takeOverType, IN CONST INT_T timestamp);
#endif

#if defined(ENABLE_LAN_LINKAGE_MASTER) && (ENABLE_LAN_LINKAGE_MASTER==1)
OPERATE_RET http_gw_lan_node_report(IN CONST CHAR_T *devIds, IN CONST UINT_T data_len, OUT ty_cJSON **result);
#endif

//OPERATE_RET http_upload_timer_log(IN CHAR_T *timer_info);

OPERATE_RET http_put_cloud_config(IN CONST CHAR_T *data);

OPERATE_RET http_gw_property_save(IN CONST CHAR_T *data);

typedef enum {
    DEV_STATUS_UNKNOWN,
    DEV_STATUS_RESET,
    DEV_STATUS_RESET_FACTORY,
    DEV_STATUS_ENABLE,
} DEV_SYNC_STATUS_E;
OPERATE_RET http_gw_dev_sync_check(DEV_SYNC_STATUS_E *p_status);

/*
0: Normal Status
1: unactived but registered
2: unregistered
*/
//OPERATE_RET http_gw_get_delete_status(INT_T *p_status);

OPERATE_RET http_gw_dev_sync_proc(VOID_T);

OPERATE_RET http_secure_verify_v20(IN CONST CHAR_T *chipId, IN CONST CHAR_T *secureCode1, IN CONST CHAR_T *randomA, \
                 IN CONST CHAR_T *token,  IN CONST CHAR_T *region, OUT ty_cJSON **result);

OPERATE_RET httpc_iot_certificate_get_v10(OUT cJSON **result,IN CONST CHAR_T *url_msg);

OPERATE_RET httpc_iot_cloud_storage_encrypt_key_get(OUT cJSON **result);

OPERATE_RET http_gw_custom_cfg_get(IN CONST INT_T biztype, OUT ty_cJSON **result);

OPERATE_RET http_gw_location_info_get(OUT ty_cJSON **result);


OPERATE_RET http_dns_by_tuya(IN CONST CHAR_T *p_host_arr[], IN CONST UINT_T arr_size, OUT ty_cJSON **result);

typedef PVOID_T RAW_HTTP_S;
OPERATE_RET httpc_raw_read_resp_header(INOUT RAW_HTTP_S p_raw_http, OUT INT_T *p_status_code, OUT BOOL_T *p_chunked, OUT UINT_T *p_content_len);
INT_T httpc_raw_read_content(IN RAW_HTTP_S p_raw_http, IN BYTE_T *buf, IN CONST UINT_T buf_len);
INT_T httpc_raw_write_content(IN RAW_HTTP_S p_raw_http, IN CONST BYTE_T *buf, IN CONST UINT_T buf_len, BOOL_T chunked);
OPERATE_RET httpc_raw_close(IN RAW_HTTP_S p_raw_http);
OPERATE_RET httpc_raw_post(IN CONST CHAR_T *p_url, IN CONST UINT_T data_total_len, INOUT RAW_HTTP_S *pp_raw_http);
OPERATE_RET httpc_raw_get(IN CONST CHAR_T *p_url, INOUT RAW_HTTP_S *pp_raw_http, IN UINT_T offset, IN UINT_T total_len);
OPERATE_RET httpc_raw_put(IN CONST CHAR_T *p_url, IN CONST UINT_T data_total_len, INOUT RAW_HTTP_S *pp_raw_http);
#if defined(ENABLE_ALARM) && (ENABLE_ALARM==1)
OPERATE_RET http_gw_get_enable_delay_def_time(OUT ty_cJSON **result);
#endif
OPERATE_RET http_get_dev_token_create_v20(OUT ty_cJSON **result);

/*
{"type":0,"t":1527656455}
http://a.daily.tuya-inc.cn/d.json?a=tuya.device.upgrade.get&devId=d5dae27ecb5643b8f1pjra&t=1527656455&v=4.1&sign=19e25330915607b5ac2fb3824aedcc32

{"t":1527656456,"success":true}
*/
OPERATE_RET httpc_get_gw_upgrd_if_v44(IN CONST CHAR_T *url, IN CONST CHAR_T *gw_id, IN CONST CHAR_T *active_key, IN CONST DEV_TYPE_T tp, OUT ty_cJSON **result);

/*
{"type":0,"upgradeStatus":0,"t":1527656457}
http://a.daily.tuya-inc.cn/d.json?a=tuya.device.upgrade.status.update&devId=d5dae27ecb5643b8f1pjra&t=1527656457&v=4.1&sign=8ecdbe367de149fd620d6dc22df1c8b3

{"t":1527656457,"success":true}
*/
OPERATE_RET httpc_gw_upgrd_stat_update_v41(IN CONST CHAR_T *url, IN CONST CHAR_T *gw_id, IN CONST DEV_TYPE_T tp, IN CONST CHAR_T *active_key, IN CONST BYTE_T stat);

/*
{"subId":"changcheng_sub_id","type":0,"t":1527656460}
http://a.daily.tuya-inc.cn/d.json?a=tuya.device.upgrade.get&devId=d5dae27ecb5643b8f1pjra&t=1527656460&v=4.1&sign=f0e6ef83a4bddf94344bcf94609a59b7

{"t":1527656460,"success":false,"errorCode":"ILLEGAL","errorMsg":"subIdlanguageUtil failed key:ILLEGALnot exist or bundle file not exist "}
*/
OPERATE_RET httpc_get_dev_upgrd_if_v44(IN CONST CHAR_T *url, IN CONST CHAR_T *gw_id, IN CONST CHAR_T *active_key, IN CONST CHAR_T *sub_id, IN CONST DEV_TYPE_T tp, OUT ty_cJSON **result);

/*
{"subId":"changcheng_sub_id","upgradeStatus":0,"type":0,"t":1527656461}
http://a.daily.tuya-inc.cn/d.json?a=tuya.device.upgrade.status.update&devId=d5dae27ecb5643b8f1pjra&t=1527656461&v=4.1&sign=64c7814649945af77855f2e556b172f6

{"t":1527656461,"success":false,"errorCode":"ILLEGAL","errorMsg":"subIdlanguageUtil failed key:ILLEGALnot exist or bundle file not exist "}
*/
OPERATE_RET httpc_dev_upgrd_stat_update_v41(IN CONST CHAR_T *url, IN CONST CHAR_T *gw_id, IN CONST CHAR_T *active_key, IN CONST CHAR_T *sub_id, IN CONST DEV_TYPE_T tp, IN CONST BYTE_T stat);
#if defined(ENABLE_SIGMESH) && (ENABLE_SIGMESH==1)
OPERATE_RET httpc_dev_ble_mesh_join_v10(IN CONST CHAR_T *url, IN CONST CHAR_T *gw_id, IN CONST CHAR_T *active_key, IN CONST CHAR_T *node_id,
                              IN CONST CHAR_T *product_key, IN CONST CHAR_T *firmware_key, IN CONST CHAR_T *uuid, IN CONST CHAR_T *sw_ver, IN CONST CHAR_T *mac, IN CONST CHAR_T *dev_key, IN CONST BOOL_T online,
                              IN CONST CHAR_T *type, IN CONST GW_ATTACH_ATTR_T *attr, IN CONST UINT_T attr_num, IN CONST BOOL_T is_oem, USER_DEV_DTL_DEF_T uddd, OUT ty_cJSON **result);

OPERATE_RET httpc_get_dev_sigmesh_auth_key_v10(IN CONST CHAR_T *url, IN CONST CHAR_T *gw_id, \
                                               IN CONST CHAR_T *active_key,IN CONST CHAR_T *uuid,OUT ty_cJSON **result);
OPERATE_RET httpc_get_dev_sigmesh_bind_status_v10(IN CONST CHAR_T *url, IN CONST CHAR_T *gw_id, \
                                               IN CONST CHAR_T *active_key,IN CONST CHAR_T *uuid, \
                                               IN CONST CHAR_T *encrypted_value,OUT ty_cJSON **result);
OPERATE_RET httpc_rept_dev_sig_ble_bind_status_v10(IN CONST CHAR_T *url, IN CONST CHAR_T *gw_id,
                                     IN CONST CHAR_T *active_key,IN CONST CHAR_T *node_id,IN BOOL_T bind_status);
OPERATE_RET httpc_rept_dev_sig_ble_bind_status_batch_v10(IN CONST CHAR_T *url, IN CONST CHAR_T *gw_id,
                                     IN CONST CHAR_T *active_key,IN CONST CHAR_T *data,IN INT_T len);
#endif
CHAR_T *http_active_url(VOID);
CHAR_T *http_url(VOID);

#if defined(GW_FAULT_REPLACE) && (GW_FAULT_REPLACE==1)
OPERATE_RET httpc_gw_oss_upload_filepath_get_v10(IN CONST CHAR_T *url, IN CONST CHAR_T *gw_id, 
                                                          IN CONST CHAR_T *active_key,IN CHAR_T *filename, 
                                                          IN CHAR_T *content_type,IN INT_T storage_type,
                                                          OUT ty_cJSON **result);
OPERATE_RET httpc_gw_transfer_filepath_get_v10(IN CONST CHAR_T *url, IN CONST CHAR_T *gw_id, 
                                                            IN CONST CHAR_T *active_key, OUT ty_cJSON **result);
OPERATE_RET httpc_gw_oss_filepath_save_v10(IN CONST CHAR_T *url, IN CONST CHAR_T *gw_id, 
                                                          IN CONST CHAR_T *active_key,IN CONST CHAR_T * allocate_id,
                                                          IN CONST CHAR_T *filename, IN CONST CHAR_T *content_type,
                                                          IN INT_T storage_type);
OPERATE_RET httpc_gw_transfer_gw_initd_v10(IN CONST CHAR_T *url, IN CONST CHAR_T *gw_id, 
                                                    IN CONST CHAR_T *active_key, IN INT_T stat, 
                                                    IN CHAR_T *ncp_mac,IN INT_T result);
OPERATE_RET httpc_gw_transfer_ncp_mac_get_v10(IN CONST CHAR_T *url, IN CONST CHAR_T *gw_id, 
                                                            IN CONST CHAR_T *active_key, OUT ty_cJSON **result);
#endif

#if defined(QRCODE_ACTIVE_MODE) && (QRCODE_ACTIVE_MODE==1)
OPERATE_RET httpc_active_shorturl_get(char *uuid, char *appid, ty_cJSON **result);
#endif

#ifdef __cplusplus
}
#endif
#endif

