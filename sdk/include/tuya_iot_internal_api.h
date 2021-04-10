/*
tuya_iot_internal_api.h
Copyright(C),2018-2020, 涂鸦科技 www.tuya.comm
*/

#ifndef _TUYA_IOT_INTERNAL_API_H
#define _TUYA_IOT_INTERNAL_API_H

#include "tuya_cloud_types.h"
#include "ty_cJSON.h"
#include "tuya_cloud_com_defs.h"

#if defined(WIFI_GW) && (WIFI_GW==1)
#include "tuya_cloud_wifi_defs.h"
#include "thing_config.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* cloud-storage api  */
//TODO

/* http api */
OPERATE_RET iot_httpc_common_post_no_remalloc(IN CONST CHAR_T *api_name, IN CONST CHAR_T *api_ver,
                                              IN CONST CHAR_T *uuid, IN CONST CHAR_T *devid,
                                              IN CHAR_T *post_data, IN UINT_T data_total_len,
                                              IN CONST CHAR_T *p_head_other,
                                              OUT ty_cJSON **pp_result);

OPERATE_RET iot_httpc_common_post(IN CONST CHAR_T *api_name, IN CONST CHAR_T *api_ver,
                                  IN CONST CHAR_T *uuid, IN CONST CHAR_T *devid,
                                  IN CHAR_T *post_data,
                                  IN CONST CHAR_T *p_head_other,
                                  OUT ty_cJSON **pp_result);


OPERATE_RET iot_httpc_common_post_simple(IN CONST CHAR_T *api_name, IN CONST CHAR_T *api_ver,
                                         IN CHAR_T *post_data, IN CONST CHAR_T *p_head_other, OUT ty_cJSON **pp_result);



typedef OPERATE_RET (*IOT_HTTP_GET_FILE_DATA_CB)(IN PVOID_T priv_data, IN CONST UINT_T total_len,IN CONST UINT_T offset,\
                                            IN CONST BYTE_T *data,IN CONST UINT_T len,OUT UINT_T *remain_len);

OPERATE_RET iot_httpc_download_file(IN CONST CHAR_T *url,IN CONST UINT_T mlk_buf_len,\
                                  IN CONST IOT_HTTP_GET_FILE_DATA_CB gfd_cb, IN PVOID_T priv_data, \
                                  IN CONST UINT_T total_len, INOUT BYTE_T file_hmac[32]);





/* mqtt api */
typedef OPERATE_RET (*iot_mqc_protocol_handler_cb)(IN ty_cJSON *root_json);

/* 注册mqtt事件回调函数 */
OPERATE_RET iot_mqc_app_register_cb(UINT_T mq_pro, iot_mqc_protocol_handler_cb handler);

typedef VOID (*iot_mqc_publish_result_cb)(IN CONST OPERATE_RET op_ret,IN CONST VOID *prv_data);
OPERATE_RET iot_mqc_send_custom_msg(IN CONST UINT_T protocol,IN CONST CHAR_T *data,IN CONST BYTE_T qos,\
                                    IN CONST UINT_T to_lmt,IN CONST iot_mqc_publish_result_cb cb,IN VOID *prv_data);


/* storage api */
OPERATE_RET iot_wd_common_write(IN CONST CHAR_T *key, IN CONST BYTE_T *value, IN CONST UINT_T len);
OPERATE_RET iot_wd_common_read(IN CONST CHAR_T *key, OUT BYTE_T **value, OUT UINT_T *p_len);
OPERATE_RET iot_wd_common_free_data(IN BYTE_T *data);

OPERATE_RET iot_wd_common_delete(IN CONST CHAR_T *key);
OPERATE_RET iot_wd_gw_desc_if_read(OUT VOID *di);
#if defined(GW_SUPPORT_COUNTRY_CODE) && (GW_SUPPORT_COUNTRY_CODE==1)
OPERATE_RET iot_wd_gw_write_country_code(CHAR_T *country_code);
#endif

typedef BYTE_T IOT_VAR_TP_T;
#define VT_CHAR 0
#define VT_BYTE 1
#define VT_SHORT 2
#define VT_USHORT 3
#define VT_INT 4
#define VT_BOOL 5
#define VT_STRING 6
#define VT_RAW 7

typedef struct {
    CHAR_T *key;
    IOT_VAR_TP_T vt;
    VOID *val;
    USHORT_T len;
}IOT_TY_DB_RW_S;

OPERATE_RET iot_wd_utils_serialize(IN CONST IOT_TY_DB_RW_S *rw,IN CONST UINT_T rw_cnt,\
                               OUT CHAR_T **out,OUT UINT_T *out_len);
OPERATE_RET iot_wd_utils_free_outbuf(IN CHAR_T *out_buf);
OPERATE_RET iot_wd_utils_deserialize(IN CONST CHAR_T *in,INOUT IOT_TY_DB_RW_S *rw,IN CONST UINT_T rw_cnt);

/* 绑定相关 */
OPERATE_RET iot_gw_user_token_bind(IN CONST CHAR_T *token);

/* 工程版切换为正式版上报配置 */
OPERATE_RET iot_put_cloud_config(IN CONST CHAR_T *data);


#if defined(WIFI_GW) && (WIFI_GW==1)
OPERATE_RET iot_wf_gw_unactive_custom_mode(GW_WF_START_MODE wifi_mode);
#endif


#if defined(ENABLE_IPC) && (ENABLE_IPC==1)
VOID iot_gw_reset_cb(VOID *rst_tp); //GW_RESET_S *
VOID iot_http_active_parse_skill_cb(ty_cJSON *p_skill);
OPERATE_RET iot_tls_register_x509_crt_der_cb(VOID *p_ctx, IN CHAR_T *hostname,IN INT_T port_num);
OPERATE_RET iot_permit_mqtt_connect_cb(VOID);
#endif

#if defined(ENABLE_EXTRA_MQTT) && (ENABLE_EXTRA_MQTT==1)
VOID iot_register_extra_mqt_cb(VOID);
#endif

/***********************************************************
网关升级时增加的前置过滤处理函数
如果*handled返回为TRUE,那么就不在通过正常的用户回调通知用户升级
如果*handled返回为TRUE,那么仍然通过正常的用户回调通知用户升级
***********************************************************/
typedef VOID (*PRE_GW_UG_INFORM_CB)(INOUT BOOL_T *handled, IN CONST FW_UG_S *fw);

VOID iot_register_pre_gw_ug_cb(IN PRE_GW_UG_INFORM_CB pre_ug_cb);

/*
#define MBEDTLS_SSL_VERIFY_NONE                 0
#define MBEDTLS_SSL_VERIFY_OPTIONAL             1
#define MBEDTLS_SSL_VERIFY_REQUIRED             2
*/
VOID iot_tls_set_ssl_verify(IN CONST INT_T verify_mode);

INT_T iot_tls_register_x509_crt_der(VOID *p_ctx, UCHAR_T *p_der, UINT_T der_len);

OPERATE_RET iot_gw_update_versions(VOID);


/* RAW HTTP API */
typedef PVOID_T IOT_RAW_HTTP_S;
OPERATE_RET iot_httpc_raw_post(IN CONST CHAR_T *p_url, IN CONST UINT_T data_total_len, INOUT IOT_RAW_HTTP_S *pp_raw_http);
OPERATE_RET iot_httpc_raw_get(IN CONST CHAR_T *p_url, INOUT IOT_RAW_HTTP_S *pp_raw_http, IN UINT_T offset, IN UINT_T total_len);
OPERATE_RET iot_httpc_raw_put(IN CONST CHAR_T *p_url, IN CONST UINT_T data_total_len, INOUT IOT_RAW_HTTP_S *pp_raw_http);

INT_T iot_httpc_raw_write_content(IN IOT_RAW_HTTP_S p_raw_http, IN CONST BYTE_T *buf, IN CONST UINT_T buf_len, BOOL_T chunked);
OPERATE_RET iot_httpc_raw_read_resp_header(INOUT IOT_RAW_HTTP_S p_raw_http, OUT INT_T *p_status_code, OUT BOOL_T *p_chunked, OUT UINT_T *p_content_len);
INT_T iot_httpc_raw_read_content(IN IOT_RAW_HTTP_S p_raw_http, IN BYTE_T *buf, IN CONST UINT_T buf_len);
OPERATE_RET iot_httpc_raw_close(IN IOT_RAW_HTTP_S p_raw_http);


OPERATE_RET tuya_iot_put_cloud_config(IN CONST CHAR_T *data);


#if defined(WIFI_GW) && (WIFI_GW==1)
OPERATE_RET tuya_iot_thing_config_init(IN THING_CONFIG_SOCKET_CB socket_cb);
OPERATE_RET tuya_iot_thing_config_start(IN CONST CHAR_T *ssid, IN CONST CHAR_T *passwd, IN CONST CHAR_T *token, IN CONST UINT_T timeout);
VOID tuya_iot_thing_config_stop(VOID);
#endif

#if defined(TUYA_OPERATOR_TYPE) && ( ((TUYA_OPERATOR_TYPE&TUYA_OPERATOR_CMCC)==TUYA_OPERATOR_CMCC)||((TUYA_OPERATOR_TYPE&TUYA_OPERATOR_CMCC)==TUYA_OPERATOR_CMCC_ANDLINK) )
OPERATE_RET iot_cmcc_reg_params(IN CONST CHAR_T *p_appkey, IN CONST CHAR_T *p_aeskey,
                                IN CHAR_T *p_deviceType,IN CHAR_T *p_productToken, UINT_T localport);

CHAR_T*iot_cmcc_andlink_get_devId(VOID);
#endif

BOOL_T tuya_iot_compare_two_string(IN CONST CHAR_T *cString1, IN CONST CHAR_T *cString2);
typedef OPERATE_RET (*iot_special_dp_cb)(IN CONST UINT16_T dpid, IN CONST ty_cJSON *dp_obj);
OPERATE_RET iot_special_dp_register_cb(IN iot_special_dp_cb handler);

//com api
BOOL_T tuya_iot_compare_two_string(IN CONST CHAR_T *cString1, IN CONST CHAR_T *cString2);

// 针对json中的K/V外围的双引号(")转换为(\")，调用者需要释放返回的内存
CHAR_T* tuya_iot_get_json_safety_string(IN CONST CHAR_T *json);

#ifdef __cplusplus
}
#endif
#endif /*_TUYA_IOT_INTERNAL_API_H*/

