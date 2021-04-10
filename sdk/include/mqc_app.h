/***********************************************************
*  File: mqc_app.h
*  Author: nzy
*  Date: 20170411
***********************************************************/
#ifndef _MQC_APP_H
#define _MQC_APP_H


#include "tuya_cloud_types.h"
#include "mqtt_client.h"
#include "gw_intf.h"
#include "uni_rfc.h"


#ifdef __cplusplus
    extern "C" {
#endif


// mqtt protocol
#define PRO_DATA_PUSH               4   /* dev -> cloud push dp data */
#define PRO_CMD                     5   /* cloud -> dev send dp data */
#define PRO_DEV_UNBIND              8   /* cloud -> dev */
#define PRO_GW_RESET                11  /* cloud -> dev reset dev */
#define PRO_TIMER_UG_INF            13  /* cloud -> dev update timer */
#define PRO_UPGD_REQ                15  /* cloud -> dev update dev/gw */
#define PRO_UPGE_PUSH               16  /* dev -> cloud update upgrade percent */
#define PRO_IOT_DA_REQ              22  /* cloud -> dev send data req */
#define PRO_IOT_DA_RESP             23  /* dev -> cloud send data resp */
#define PRO_DEV_LINE_STAT_UPDATE    25  /* dev -> sub device online status update */
#define PRO_CMD_ACK                 26  /* dev -> cloud  dev send ackId to cloud */
#define PRO_MQ_EXT_CFG_INF          27  /* cloud -> dev runtime cfg update */
#define PRO_MQ_BLE_RAW_CMD          28  /* cloud -> ble dev RAW data */
#define PRO_MQ_QUERY_DP             31  /* cloud -> dev query dp stat */
#define PRO_GW_SIGMESH_TOPO_UPDATE  33  /* cloud -> dev sigmesh topo update */
#define PRO_UG_SUMMER_TABLE         41  // ug sumer timer table
#define PRO_GW_UPLOAD_LOG           45  /* dev -> cloud, upload log*/
#define PRO_GW_LINKAGE_UPDATE       49  /* cloud -> dev 场景更新推送 */
#define PRO_MQ_THINGCONFIG          51  /* 设备免密配网 */
#define PRO_MQ_PEGASUS_CONFIG       63  /* 闪电配网 */
#define PRO_MQ_APP_PROTOCOL_RX      64  /* 应用用户自定义协议 cloud->dev */
#define PRO_MQ_APP_PROTOCOL_TX      65  /* 应用用户自定义协议 dev->cloud */
#define PRO_MQ_LOG_CONFIG           55  /* log config */
#define PRO_DEV_WAKEUP              102 /* cloud -> dev, 更新子设备待唤醒标志 */
#define PRO_MQ_COMMON               100 /* cloud -> dev common protocol*/
#define PRO_MQ_EN_GW_ADD_DEV_REQ    200 // gw enable add sub device request
#define PRO_MQ_EN_GW_ADD_DEV_RESP   201 // gw enable add sub device respond
#define PRO_DEV_LC_GROUP_OPER       202 /* cloud -> dev */
#define PRO_DEV_LC_GROUP_OPER_RESP  203 /* dev -> cloud */
#define PRO_DEV_LC_SENCE_OPER       204 /* cloud -> dev */
#define PRO_DEV_LC_SENCE_OPER_RESP  205 /* dev -> cloud */
#define PRO_DEV_LC_SENCE_EXEC       206 /* cloud -> dev */
#define PRO_CLOUD_STORAGE_ORDER_REQ 300 /* 云存储订单 */
#define PRO_3RD_PARTY_STREAMING_REQ 301 /* echo show/chromecast request */
#define PRO_RTC_REQ                 302 /* cloud -> dev */
#define PRO_AI_DETECT_DATA_SYNC_REQ 304 /* 本地AI数据更新，当前用于人脸检测样本数据更新(新增/删除/变更) */
#define PRO_FACE_DETECT_DATA_SYNC   306 /* 人脸识别数据同步通知,门禁设备使用 */
#define PRO_CLOUD_STORAGE_EVENT_REQ 307 /* 联动触发云存储 */
#define PRO_DOORBELL_STATUS_REQ     308 /* 门铃请求被用户处理，接听或者拒绝 */
#define PRO_MQ_CLOUD_STREAM_GATEWAY 312
#define PRO_GW_COM_SENCE_EXE        403 /* cloud -> dev 原云端场景放到本地执行 */
#define PRO_DEV_ALARM_DOWN    		701 /* cloud -> dev */
#define PRO_DEV_ALARM_UP      		702 /* dev -> cloud */


//VOID mqc_app_enable_media(VOID);

OPERATE_RET mqc_app_init(VOID);

OPERATE_RET mqc_app_start(BOOL_T enable_time_check);

VOID mqc_app_disconnect(VOID);

OPERATE_RET mqc_app_quit(VOID_T);

VOID mqc_app_close(VOID);

BOOL_T get_mqc_conn_stat(VOID);

OPERATE_RET mqc_very_pcmcd_mcd(IN CONST UINT_T cmd_from,IN CONST UINT_T seq);

OPERATE_RET mqc_dp_report_async(RFC_DATA_S *rfc_da, IN CONST UINT_T to_lmt, IN CONST MQ_PUB_ASYNC_IFM_CB cb,IN VOID *prv_data);

OPERATE_RET mqc_obj_dp_query(RFC_DATA_S *rfc_da);

OPERATE_RET mqc_upgd_progress_rept_v41(IN CONST UINT_T percent,IN CONST CHAR_T *devid, IN CONST DEV_TYPE_T tp);

OPERATE_RET mqc_prot_data_rept_seq(IN CONST UINT_T pro,IN CONST CHAR_T *data,IN CONST BYTE_T qos,\
                                   IN CONST UINT_T to_lmt,IN CONST MQ_PUB_ASYNC_IFM_CB cb,IN VOID *prv_data);

OPERATE_RET mqc_get_wakeup_data(INOUT BYTE_T *wakeup_data_arr, INOUT UINT_T *p_len);

OPERATE_RET mqc_get_heartbeat_data(INOUT BYTE_T *heartbeat_data_arr, INOUT UINT_T *p_len);

OPERATE_RET mqc_book_wakeup_topic(VOID);

OPERATE_RET mqc_book_additional_topic(IN CHAR_T *topic);

INT_T mqc_get_alive_time_s(VOID);

INT_T mqc_get_mqtt_socket_fd(VOID);

VOID mqc_restart(VOID);

#if defined(GW_SUPPORT_WIRED_WIFI) && (GW_SUPPORT_WIRED_WIFI==1)
UINT_T mqc_get_mqtt_socket_ip(VOID);
#endif

OPERATE_RET mqc_send_custom_mqtt_msg(IN CONST UINT_T protocol, IN CONST BYTE_T *p_data);
OPERATE_RET mqc_send_mqtt_msg_with_topic(IN CONST CHAR_T* topic, IN CONST UINT_T protocol, IN CONST BYTE_T *p_data);

typedef OPERATE_RET (*mqc_protocol_handler_cb)(IN ty_cJSON *root_json);

OPERATE_RET mqc_app_register_cb(UINT_T mq_pro, mqc_protocol_handler_cb handler);

OPERATE_RET mqc_log_notify(IN CONST CHAR_T *uuid, IN CONST CHAR_T *bucket,
                           IN CONST CHAR_T *log_path, IN CONST CHAR_T *logName, CHAR_T* logSuf);

OPERATE_RET mqc_media_pub_async(IN CONST FLOW_BODY_ST *dt_body,
                                IN CONST UINT_T to_lmt, IN CONST MQ_PUB_ASYNC_IFM_CB cb,IN VOID *prv_data);

OPERATE_RET mqc_media_pub_sync(IN CONST FLOW_BODY_ST *dt_body,IN CONST UINT_T timeout);
OPERATE_RET mqc_media_pub_sync_v2(IN CONST FLOW_BODY_V2_ST *dt_body,IN CONST UINT_T timeout);

OPERATE_RET mqc_media_pub_data_sync(IN CONST USHORT_T map_id,IN CONST UINT_T offset,IN CONST BYTE_T *pbuffer, IN CONST USHORT_T buf_len);

OPERATE_RET mqc_custom_pub_async(IN CONST BYTE_T *data, IN CONST INT_T len, IN CONST CHAR_T *topic,
                                 IN CONST BYTE_T qos, IN CONST UINT_T timeout, IN CONST MQ_PUB_ASYNC_IFM_CB cb,
                                 IN VOID *prv_data);

OPERATE_RET mqc_custom_pub_sync(IN CONST BYTE_T *data, IN CONST INT_T len, IN CONST CHAR_T *topic,
                                IN CONST BYTE_T qos, IN CONST UINT_T timeout);
/** 
 * @brief: 注册应用MQTT扩展协议回调
 *
 * @param[in] sub_mq_pro: 扩展协议名称
 * @param[in] handler: 扩展协议处理函数
 *
 * @return int: 0成功，非0，请参照tuya error code描述文档 
 */
OPERATE_RET mqc_app_reg_ext_proto(CONST CHAR_T *sub_mq_pro, mqc_protocol_handler_cb handler);

/** 
 * @brief: 移除应用MQTT扩展协议回调
 *
 * @param[in] sub_mq_pro: 扩展协议名称
 *
 * @return int: 0成功，非0，请参照tuya error code描述文档 
 */
OPERATE_RET mqc_app_unreg_ext_proto(CONST CHAR_T *sub_mq_pro);

/** 
 * @brief: 应用MQTT扩展协议异步publish
 *
 * @param[in] data: 扩展协议名称
 * @param[in] qos:消息等级，仅支持0/1
 *
 * @return int: 0成功，非0，请参照tuya error code描述文档 
 */
OPERATE_RET mqc_app_ext_proto_data_rept(CONST CHAR_T *data, CONST BYTE_T qos);

/** 
 * @brief: 注册SDK MQTT扩展协议回调
 *
 * @param[in] sub_mq_pro: 扩展协议名称
 * @param[in] handler: 扩展协议处理函数
 *
 * @return int: 0成功，非0，请参照tuya error code描述文档 
 */
OPERATE_RET mqc_sdk_reg_ext_proto(CONST CHAR_T *sub_mq_pro, mqc_protocol_handler_cb handler);

/** 
 * @brief: 移除SDK MQTT扩展协议回调
 *
 * @param[in] sub_mq_pro: 扩展协议名称
 *
 * @return int: 0成功，非0，请参照tuya error code描述文档 
 */
OPERATE_RET mqc_sdk_unreg_ext_proto(CONST CHAR_T *sub_mq_pro);

/** 
 * @brief: SDK MQTT扩展协议异步publish
 *
 * @param[in] data: 扩展协议名称
 * @param[in] qos:消息等级，仅支持0/1
 *
 * @return int: 0成功，非0，请参照tuya error code描述文档 
 */
OPERATE_RET mqc_sdk_ext_proto_data_rept(CONST CHAR_T *data, CONST BYTE_T qos);

#ifdef __cplusplus
}
#endif

#endif

