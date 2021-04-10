/*
tuya_iot_com_api.h
Copyright(C),2018-2020, 涂鸦科技 www.tuya.comm
*/

#ifndef __TUYA_IOT_COM_API_H
#define __TUYA_IOT_COM_API_H

#include "tuya_cloud_types.h"
#include "tuya_cloud_com_defs.h"

#if defined(ENABLE_ENGINEER_TO_NORMAL) && (ENABLE_ENGINEER_TO_NORMAL==1)
#include "tuya_cloud_com_defs_engr.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif


/***********************************************************
*  Function: tuya_iot_get_sdk_info
*  Desc:     get tuya_iot_sdk build info
*  Return:   sdk info
***********************************************************/
CHAR_T *tuya_iot_get_sdk_info(VOID);
CHAR_T *tuya_iot_get_os_adapt_info(VOID_T);

/***********************************************************
*  Function: tuya_iot_init_params
*  Desc:     init tuya_iot_sdk
*  Input:    fs_storge_path: filesystem read write storge path
*            (if os have no fs,then fs_storge_path is invalid)
*  Input:    p_param: custom init params
*  Return:  OPRT_OK: success  Other: fail
***********************************************************/
#define tuya_iot_init(fs_storge_path) \
    tuya_iot_init_params(fs_storge_path, NULL)
OPERATE_RET tuya_iot_init_params(IN CONST CHAR_T *fs_storge_path, IN CONST TY_INIT_PARAMS_S *p_param);


/***********************************************************
*  Function: tuya_iot_kv_flash_init
*  Desc:     init kv flash
*  Input:    fs_storge_path: filesystem read write storge path
*            (if os have no fs,then fs_storge_path is invalid)
*  Input:    p_param: custom init params
*  Return:  OPRT_OK: success  Other: fail
***********************************************************/
#define tuya_iot_kv_flash_init     tuya_iot_kv_flash_init_param
OPERATE_RET tuya_iot_kv_flash_init_param(IN CONST CHAR_T *fs_storge_path);
/***********************************************************
*  Function: tuya_iot_oem_set
*  Desc:     set oem mode
*  Input:    oem
*  Note: must call first
***********************************************************/
VOID tuya_iot_oem_set(IN CONST BOOL_T oem);

/***********************************************************
*  Function: tuya_iot_set_udf_parameter
*  Desc:     set user defined funtions
*  Input:    udf: user defined funtions(json), max length: 255
*  Return:   OPRT_OK: success  Other: fail
*  Note:     call before init
***********************************************************/
OPERATE_RET tuya_iot_set_udf_parameter(IN CONST CHAR_T *udf);

/***********************************************************
*  Function: tuya_iot_upload_rst_log_set
*  Desc:     set rst log upload mode
*  Input:    upload
*  Note: must call first
***********************************************************/
VOID tuya_iot_upload_rst_log_set(IN CONST BOOL_T upload);

/***********************************************************
*  Function: tuya_iot_upload_rst_log_set
*  Desc:     set rst log upload mode
*  Input:    upload
*  Note: must call first
***********************************************************/
VOID tuya_iot_ignore_upgrade_set(IN CONST BOOL_T ignore);

/***********************************************************
*  Function: tuya_iot_mqtt_disconnect
*  Desc:     force disconnect MQTT 
*  Input:    upload
*  Note: must call first
***********************************************************/
VOID tuya_iot_mqtt_disconnect();

/***********************************************************
*  Function: tuya_iot_mqtt_restart
*  Desc:     restart MQTT 
*  Input:    upload
*  Note: must call first
***********************************************************/
VOID tuya_iot_mqtt_restart();

/***********************************************************
*  Function: tuya_iot_get_dev_online_status
*  Desc:     get device online & offline status
*  Input:    unique id of device
*  Output:   status: TRUE is online, FALSE is offline
*  Return:   OPRT_OK: success  Other: fail
***********************************************************/
OPERATE_RET tuya_iot_get_dev_online_status(IN CONST CHAR_T *dev_id, OUT BOOL_T *status);

/***********************************************************
*  Function: tuya_iot_upgrade_gw
*  Desc:     upgrage gateway firmware
*  Input:    fw: firmware info
*  Input:    get_file_cb: download content storage callback
*  Input:    upgrd_nofity_cb: firmware download finish callback
*  Input:    pri_data: private param of get_file_cb && upgrd_nofity_cb
*  Input:    notify: notify app the upgrade status
*  Input:    download_buf_size: max download buffer size when downloading
*  Return:   OPRT_OK: success  Other: fail
***********************************************************/
#define tuya_iot_upgrade_gw(fw, get_file_cb, upgrd_nofity_cb, pri_data) \
    tuya_iot_upgrade_gw_notify(fw, get_file_cb, upgrd_nofity_cb, pri_data, TRUE, 0)
OPERATE_RET tuya_iot_upgrade_gw_notify(IN CONST FW_UG_S *fw,
                                       IN CONST GET_FILE_DATA_CB get_file_cb,\
                                       IN CONST UPGRADE_NOTIFY_CB upgrd_nofity_cb,\
                                       IN CONST PVOID_T pri_data,\
                                       BOOL_T notify, UINT_T download_buf_size);

OPERATE_RET tuya_iot_refuse_upgrade(IN CONST FW_UG_S *fw, IN CONST CHAR_T *dev_id);


/***********************************************************
*  Function: tuya_iot_reset_upgrade_detect_timer
*  Desc:     reset the upgrade detect time
*  Input:    time_ms: the time you want start the detect 
*  Return:   OPRT_OK: success  Other: fail
***********************************************************/
OPERATE_RET tuya_iot_reset_upgrade_detect_timer(IN CONST INT_T time_ms);


/***********************************************************
*  Function: tuya_iot_gw_module_update
*  Desc:     update gw module version
*  Input:    type: modult type
*  Input:    ver: version
*  Return:   OPRT_OK: success  Other: fail
***********************************************************/
OPERATE_RET tuya_iot_gw_version_update(IN GW_PERMIT_DEV_TP_T type, IN CONST CHAR_T *ver);


/***********************************************************
*  Function: tuya_iot_upgrade_dev
*  Desc:     upgrage sub-device/soc/mcu firmware
*  Input:    devid: if upgrade sub-device, then devid = sub-device_id
*                   if upgrade soc/mcu, then devid = NULL
*  Input:    fw: firmware info
*  Input:    get_file_cb: download content storage callback
*  Input:    upgrd_nofity_cb: firmware download finish callback
*  Input:    pri_data: private param of get_file_cb && upgrd_nofity_cb
*  Return:   OPRT_OK: success  Other: fail
***********************************************************/
#define tuya_iot_upgrade_dev(devid, fw, get_file_cb, upgrd_nofity_cb, pri_data) \
    tuya_iot_upgrade_dev_notify(devid, fw, get_file_cb, upgrd_nofity_cb, pri_data, TRUE, 0)
OPERATE_RET tuya_iot_upgrade_dev_notify(IN CONST CHAR_T *devid,
                                        IN CONST FW_UG_S *fw, \
                                        IN CONST GET_FILE_DATA_CB get_file_cb,\
                                        IN CONST UPGRADE_NOTIFY_CB upgrd_nofity_cb,\
                                        IN CONST PVOID_T pri_data,\
                                        BOOL_T notify, UINT_T download_buf_size);

/***********************************************************
*  Function: tuya_iot_get_dev_if
*  Desc:     get sub-device info
*  Input:    devid: if devid = sub-device_id, then return the sub-dev info
*                   if devid = NULL, then return the gateway info
*  Return:   (DEV_DESC_IF_S *): success  NULL: fail
***********************************************************/
DEV_DESC_IF_S *tuya_iot_get_dev_if(IN CONST CHAR_T *dev_id);

/***********************************************************
*  Function: tuya_iot_dev_traversal
*  Desc:     Traversing all devices under the gateway,
*            except the gateway device.should access a data token first.
*  Input:    token
*  Input:    iterator
*  Return:   (DEV_DESC_IF_S *) (NULL means end)
***********************************************************/
DEV_DESC_IF_S *tuya_iot_dev_traversal(INOUT VOID **iterator);

#define DEV_DESC_TRAVERSAL_BEGIN(dev_if) \
do { \
    DEV_DESC_IF_S * dev_if = NULL; \
    VOID *iterator = NULL; \
    do { \
        dev_if = (DEV_DESC_IF_S *)tuya_iot_dev_traversal(&iterator); \
        if(dev_if) {

#define DEV_DESC_TRAVERSAL_FINISH(dev_if) \
        } \
    }while(dev_if); \
}while(0);

#define DEV_DESC_TRAVERSAL_BREAK(dev_if)     break;
#define DEV_DESC_TRAVERSAL_CONTINUE(dev_if)  continue;

/***********************************************************
*  Function: tuya_iot_get_dp_desc
*  Desc:     get dp info of a sub-device and a dp id
*  Input:    dev_id: sub-device id
*  Input:    dpid: dp id
*  Return:   (DP_DESC_IF_S *): success  NULL: fail
***********************************************************/
DP_DESC_IF_S *tuya_iot_get_dp_desc(IN CONST CHAR_T *dev_id,IN CONST BYTE_T dpid);

/***********************************************************
*  Function: tuya_iot_get_dp_prop_value
*  Desc:     get dp value of a sub-device and a dp id
*  Input:    dev_id: sub-device id
*  Input:    dpid: dp id
*  Return:   (DP_PROP_VALUE_U *): success  NULL: fail
***********************************************************/
DP_PROP_VALUE_U *tuya_iot_get_dp_prop_value(IN CONST CHAR_T *dev_id,IN CONST BYTE_T dpid);

/***********************************************************
*  Function: tuya_iot_gw_subdevice_update
*  Desc:     update sub-device version
*  Input:    id: sub-device id
*  Input:    ver: sub-device version
*  Input:    is_support_ota: sub-device version
*  Return:   OPRT_OK: success  Other: fail
***********************************************************/
#define tuya_iot_gw_subdevice_update(id, ver, is_support_ota) \
    tuya_iot_gw_subdevice_update_versions(id, ver, NULL, 0, 0, is_support_ota)
OPERATE_RET tuya_iot_gw_subdevice_update_versions(IN CONST CHAR_T *id,IN CONST CHAR_T *ver, \
                                                  IN CONST GW_ATTACH_ATTR_T *attr,IN CONST UINT_T attr_num, \
                                                  IN CONST BOOL_T is_force, IN CONST BOOL_T is_support_ota);

/***********************************************************
*  Function: tuya_iot_dev_upgd_progress_rept
*  Input: percent id tp
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET tuya_iot_dev_upgd_progress_rept(IN CONST UINT_T percent, IN CONST CHAR_T *devid, IN CONST DEV_TYPE_T tp);

/***********************************************************
*  Function: tuya_iot_dev_upgd_result_report
*  Input: id result
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET tuya_iot_dev_upgd_result_report(IN CONST CHAR_T *dev_id, IN CONST DEV_TYPE_T type, IN CONST INT_T result);


/***********************************************************
*  Function: dev_report_dp_json_async
*  Desc:     report dp info a-synced.
*  Input:    devid: if sub-device, then devid = sub-device_id
*                   if gateway/soc/mcu, then devid = NULL
*  Input:    dp_data: dp array header
*  Input:    cnt    : dp array count
*  Return:   OPRT_OK: success  Other: fail
***********************************************************/
OPERATE_RET dev_report_dp_json_async(IN CONST CHAR_T *dev_id,IN CONST TY_OBJ_DP_S *dp_data,IN CONST UINT_T cnt);

/***********************************************************
*  Function: dev_query_dp_json_async
*  Desc:     report dp info a-synced.
*  Input:    devid: if sub-device, then devid = sub-device_id
*                   if gateway/soc/mcu, then devid = NULL
*  Input:    dp_data: dp array header
*  Input:    cnt    : dp array count
*  Return:   OPRT_OK: success  Other: fail
***********************************************************/
OPERATE_RET dev_query_dp_json_async(IN CONST CHAR_T *dev_id,IN CONST TY_OBJ_DP_S *dp_data,IN CONST UINT_T cnt);

/***********************************************************
*  Function: dev_report_dp_raw_sync
*  Desc:     report dp raw info synced.
*  Input:    devid: if sub-device, then devid = sub-device_id
*                   if gateway/soc/mcu, then devid = NULL
*  Input:    dpid: raw dp id
*  Input:    data: raw data
*  Input:    len : len of raw data
*  Input:    timeout: function blocks until timeout seconds
*  Return:   OPRT_OK: success  Other: fail
***********************************************************/
#define dev_report_dp_raw_sync(dev_id, dpid, data, len, timeout) \
    dev_report_dp_raw_sync_extend(dev_id, dpid, data, len, timeout, TRUE)
OPERATE_RET dev_report_dp_raw_sync_extend(IN CONST CHAR_T *dev_id,IN CONST BYTE_T dpid,\
                                                      IN CONST BYTE_T *data,IN CONST UINT_T len,\
                                                      IN CONST UINT_T timeout, IN CONST BOOL_T enable_auto_retrans);

/***********************************************************
*  Function: dev_report_dp_raw_sync
*  Desc:	   report dp raw info synced.
*  Input:    devid: if sub-device, then devid = sub-device_id
* 				  if gateway/soc/mcu, then devid = NULL
*  Input:    dpid: raw dp id
*  Input:    data: raw data
*  Input:    len : len of raw data
*  Input:    timeout: function blocks until timeout seconds
*  Return:   OPRT_OK: success  Other: fail
***********************************************************/
#define dev_report_dp_raw_sync_with_time(dev_id, dpid, data, len, timeout, time_str) \
    dev_report_dp_raw_sync_extend_with_time(dev_id, dpid, data, len, timeout, TRUE, time_str)
OPERATE_RET dev_report_dp_raw_sync_extend_with_time(IN CONST CHAR_T *dev_id,IN CONST BYTE_T dpid,\
                                                    IN CONST BYTE_T *data,IN CONST UINT_T len,\
                                                    IN CONST UINT_T timeout, IN CONST BOOL_T enable_auto_retrans,\
                                                    IN CONST CHAR_T *time_str);

/***********************************************************
*  Function: dev_report_dp_stat_sync
*  Desc:     report dp status info synced.
*            if time_stamp==0, time_stamp = time of msg arrival of the server
*  Input:    devid: if sub-device, then devid = sub-device_id
*                   if gateway/soc/mcu, then devid = NULL
*  Input:    dp_data: dp status array header
*  Input:    cnt    : dp status array count
*  Input:    timeout: function blocks until timeout seconds
*  Return:   OPRT_OK: success  Other: fail
***********************************************************/
#define dev_report_dp_stat_sync(dev_id, dp_data, cnt, timeout) \
    dev_report_dp_stat_sync_extend(dev_id, dp_data, cnt, timeout, TRUE)
OPERATE_RET dev_report_dp_stat_sync_extend(IN CONST CHAR_T *dev_id,IN CONST TY_OBJ_DP_S *dp_data,\
                                    IN CONST UINT_T cnt,IN CONST UINT_T timeout, IN CONST BOOL_T enable_auto_retrans);

/***********************************************************
*  Function: tuya_iot_get_wakeup_data
*  Desc:     get mqtt wakeup string pattern
*  Output:   wakeup_data_arr: wakeup string arr
*  Output:   p_len: wakeup string len
*  Return:   OPRT_OK: success  Other: fail
***********************************************************/
OPERATE_RET tuya_iot_get_wakeup_data(INOUT BYTE_T *wakeup_data_arr, INOUT UINT_T *p_len);

/***********************************************************
*  Function: tuya_iot_get_heartbeat_data
*  Desc:     get mqtt heartbeat string pattern
*  Output:   heartbeat_data_arr: heartbeat string arr
*  Output:   p_len: heartbeat string len
*  Return:   OPRT_OK: success  Other: fail
***********************************************************/
OPERATE_RET tuya_iot_get_heartbeat_data(INOUT BYTE_T *heartbeat_data_arr, INOUT UINT_T *p_len);

/***********************************************************
*  Function: tuya_iot_get_heartbeat_interval
*  Desc:     get mqtt heartbeat interval
*  Return:   mqtt heartbeat interval in seconds
***********************************************************/
INT_T tuya_iot_get_heartbeat_interval(VOID);

/***********************************************************
*  Function: tuya_iot_book_wakeup_topic
*  Desc:     unscribe mqtt msg topic and subscribe wakeup topic
*  Return:   OPRT_OK: success  Other: fail
***********************************************************/
OPERATE_RET tuya_iot_book_wakeup_topic(VOID);

/***********************************************************
*  Function: tuya_iot_get_mqtt_socket_fd
*  Desc:     get curr mqtt socket fd
*  Output:   mqtt socket fd
***********************************************************/
INT_T tuya_iot_get_mqtt_socket_fd(VOID);

/***********************************************************
*  Function: tuya_iot_send_custom_mqtt_msg
*  Desc:     send a custom protocol mqtt msg
*  Input:    protocol: mqtt protocol
*  Input:    p_data: mqtt msg
*  Return:   OPRT_OK: success  Other: fail
***********************************************************/
OPERATE_RET tuya_iot_send_custom_mqtt_msg(IN CONST UINT_T protocol, IN CONST BYTE_T *p_data);
///***********************************************************
//*  Function: tuya_iot_media_init
//*  Input: none
//*  Output: none
//*  Return: OPERATE_RET
//***********************************************************/
//OPERATE_RET tuya_iot_media_init(VOID);

/***********************************************************
*  Function: tuya_iot_upload_incre_data
*  Input:
   map_id: Each map has an Id
   offset: Map data offset
   pbuffer:Map data cache pointer
   buf_len:Map data cache length
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET tuya_iot_upload_incre_data(IN CONST USHORT_T map_id,IN CONST UINT_T offset,IN CONST BYTE_T *pbuffer, IN CONST USHORT_T buf_len);

/***********************************************************
*  Function: tuya_iot_media_data_report
*  Input: dt_body->media data
*         timeout->need report time
*  Output: none
*  WANGING: please dont use tuya_iot_media_data_report and tuya_iot_media_data_report_v2 in one application
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET tuya_iot_media_data_report(IN CONST FLOW_BODY_ST *dt_body,IN CONST UINT_T timeout);

/***********************************************************
*  Function: tuya_iot_media_data_report_v2
*  Input: dt_body->media data version 2
*         timeout->need report time
*  Output: none
*  WANGING: please dont use tuya_iot_media_data_report and tuya_iot_media_data_report_v2 in one application
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET tuya_iot_media_data_report_v2(IN CONST FLOW_BODY_V2_ST *dt_body,IN CONST UINT_T timeout);

#define tuya_iot_upload_layout_buffer(map_id, buffer, len) \
    tuya_iot_map_cleaner_upload_buffer(map_id, buffer, len, "layout/lay.bin", UP_CLEANER_MAP)
#define tuya_iot_upload_route_buffer(map_id, buffer, len) \
    tuya_iot_map_cleaner_upload_buffer(map_id, buffer, len, "route/rou.bin", UP_CLEANER_PATH)

#define tuya_iot_upload_layout_file(map_id, local_file_name) \
    tuya_iot_map_cleaner_upload_file(map_id, local_file_name, "layout/lay.bin", UP_CLEANER_MAP)
#define tuya_iot_upload_route_file(map_id, local_file_name) \
    tuya_iot_map_cleaner_upload_file(map_id, local_file_name, "route/rou.bin", UP_CLEANER_PATH)

/***********************************************************
*  Function: tuya_iot_map_cleaner_upload_buffer
*  Desc:  sweeper function. upload cleaner map info
*  Input: map_id
*  Input: buffer
*  Input: len
*  Input: cloud_file_name
*  Input: map_type
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET tuya_iot_map_cleaner_upload_buffer(IN CONST INT_T map_id, IN CONST BYTE_T *buffer, IN CONST UINT_T len, \
                                               IN CONST CHAR_T *cloud_file_name, IN CONST UP_MAP_TYPE_E map_type);

/***********************************************************
*  Function: tuya_iot_map_cleaner_upload_file
*  Desc:  sweeper function. upload cleaner map info
*  Input: map_id
*  Input: local_file_name
*  Input: cloud_file_name
*  Input: map_type
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET tuya_iot_map_cleaner_upload_file(IN CONST INT_T map_id, IN CONST CHAR_T *local_file_name, \
                                             IN CONST CHAR_T *cloud_file_name, IN CONST UP_MAP_TYPE_E map_type);

/***********************************************************
*  Function: tuya_iot_map_record_upload_buffer
*  Desc:  sweeper function. upload record map info
*  Input: map_id
*  Input: buffer
*  Input: len
*  Input: descript
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET tuya_iot_map_record_upload_buffer(IN CONST INT_T map_id, IN CONST BYTE_T *buffer, IN CONST UINT_T len, IN CONST CHAR_T *descript);

/***********************************************************
*  Function: tuya_iot_map_record_upload_buffer_with_filename
*  Desc:  sweeper function. upload record map info
*  Input: map_id
*  Input: buffer
*  Input: len
*  Input: cloud file name
*  Input: descript
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET tuya_iot_map_record_upload_buffer_with_filename(IN CONST INT_T map_id, IN CONST BYTE_T *buffer, IN CONST UINT_T len, IN CONST CHAR_T *cloud_file_name, IN CONST CHAR_T *descript);

#if 0
/***********************************************************
*  Function: tuya_iot_map_record_upload_file
*  Desc:  sweeper function. upload record map info
*  Input: map_id
*  Input: layout_name
*  Input: route_name
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET tuya_iot_map_record_upload_file(IN CONST INT_T map_id, IN CONST CHAR_T *layout_name, IN CONST CHAR_T *route_name);
#endif
/***********************************************************
*  Function: tuya_iot_get_gw_id
*  Input: none
*  Output: none
*  Return: (CHAR_T *)->device id
***********************************************************/
CHAR_T *tuya_iot_get_gw_id(VOID);

/***********************************************************
*  Function: tuya_iot_get_region_info
*  Input: none
*  Output: p_region_info
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET tuya_iot_get_region_info(INOUT TY_IOT_REGION_INFO_S *p_region_info);

/***********************************************************
*  Function: tuya_iot_get_custom_cfg
*  Input: type
*  Output: pp_cfg_str
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET tuya_iot_get_custom_cfg(IN CONST TY_CUSTOM_CFG_E type, OUT CHAR_T **pp_cfg_str);

/***********************************************************
*  Function: tuya_iot_release_custom_cfg
*  Input: p_cfg_str
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET tuya_iot_release_custom_cfg(IN CHAR_T *p_cfg_str);

/***********************************************************
*  Function: tuya_iot_get_location_info
*  Input: p_location
*  Output: p_location
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET tuya_iot_get_location_info(INOUT TY_LOCATION_INFO_S *p_location);

OPERATE_RET tuya_iot_set_log_attr(IN CONST INT_T log_level);
OPERATE_RET tuya_iot_set_log_bool_time(BOOL_T if_ms_level);

/***********************************************************
*  Function: tuya_iot_custom_data_report_sync
*  Input: data len topic qos timeout
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET tuya_iot_custom_data_report_sync(IN CONST BYTE_T *data, IN CONST INT_T len, IN CONST CHAR_T *topic, 
                                                        IN CONST BYTE_T qos, IN CONST UINT_T timeout);

/***********************************************************
*  Function: tuya_iot_custom_data_report_async
*  Input: data len topic qos timeout
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET tuya_iot_custom_data_report_async(IN CONST BYTE_T *data, IN CONST INT_T len, IN CONST CHAR_T *topic, 
                                                            IN CONST BYTE_T qos, IN CONST UINT_T timeout);

/*
* Function : tuya_iot_get_active_stat
* note : get stat of sdk active
* Input : none
* Output : none
* Return : BYTE_T
*/
BYTE_T tuya_iot_get_active_stat();
#if defined(ENABLE_ALARM) && (ENABLE_ALARM==1)

/***********************************************************
*  Function: tuya_iot_home_secruity_info_set
*  Input: mode_str: mode str: "0":disarm, "1","2".
*         delay_json: {"1":1000}.mode 1 is delay time 1000s or NULL
*
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET tuya_iot_home_secruity_info_set(IN CONST CHAR_T *mode_str, IN CONST CHAR_T *node_id, IN CONST CHAR_T *delay_str);

/***********************************************************
*no use, use new interface tuya_iot_home_secruity_set_alarm_status
*  Function: tuya_iot_home_secruity_alarm_status
*            application must sync alarm status by this interface 
*  Input:  BOOL_T alarm_status: true: alarm status, false: normal status
*
*  Output: none
*  Return: OPERATE_RET
************************************************************/
OPERATE_RET tuya_iot_home_secruity_alarm_status(IN CONST BOOL_T alarm_status);//no use, use new interface tuya_iot_home_secruity_set_alarm_status

/***********************************************************
*  Function: tuya_iot_home_secruity_set_alarm_status
*            application must sync alarm status by this interface 
*  Input:  BOOL_T alarm_status: true: alarm status, false: normal status,HOME_SECURITY_INFO_SET_E cmd_from: HOME_SECURITY_INFO_SET_MQ: from mqtt,
*  HOME_SECURITY_INFO_SET_AP: from app, HOME_SECURITY_INFO_SET_LAN: from lan, HOME_SECURITY_INFO_SET_SDK: from sdk.
*  Output: none
*  Return: OPERATE_RET
************************************************************/
OPERATE_RET tuya_iot_home_secruity_set_alarm_status(IN CONST BOOL_T alarm_status, IN CONST HOME_SECURITY_INFO_SET_E cmd_from);

/***********************************************************
*  Function: tuya_iot_home_secruity_get_alarm_info
*  Input: mode_str: mode str: "0":disarm, "1","2".
*
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET tuya_iot_home_secruity_get_alarm_info(OUT ALARM_INFO_S *alarm_info);

/***********************************************************
*  Function: tuya_iot_net_mode_reprot
*  Input: net_mode
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET tuya_iot_net_mode_reprot(IN CONST HOME_SECURITY_NET_MODE net_mode);

/***********************************************************
*  Function: tuya_iot_is_support_home_security
*  Input: none
*  Output: none
*  Return: BOOL_T
***********************************************************/
BOOL_T tuya_iot_is_support_home_security(VOID);

/***********************************************************
*  Function: tuya_iot_support_home_security_set
*  Input: is_support： TRUE/FALSE support home security,must init,
*                   the interface was used to the new and old versions compatible
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET tuya_iot_support_home_security_set(BOOL_T is_support);
#endif

/***********************************************************
*  Function: tuya_iot_app_cbs_init
*  Input: user callbacks
*  Output: none
*  Return:
***********************************************************/
VOID tuya_iot_app_cbs_init(IN CONST TY_IOT_APP_CBS_S *app_cbs);

/***********************************************************
*  Function: tuya_iot_dp_low_power_query
*  Input: dps: data of DP
*         cnt: count of DP
*  Output: obj_dps: get object type DP
*          raw_dps: get raw type DP
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET tuya_iot_dp_low_power_query(IN CONST UCHAR_T *dps, IN CONST UINT_T cnt,
                                    OUT TY_RECV_OBJ_DP_S **obj_dps, OUT TY_RECV_RAW_DP_S **raw_dps);

/***********************************************************
*  Function: tuya_iot_set_engineer_mode
*  Desc:     set tuya_iot_sdk in engineer mode
*  Return:   sdk info
***********************************************************/
VOID tuya_iot_set_engineer_mode(VOID);

/***********************************************************
*  Function: tuya_iot_get_engineer_mode
*  Desc:     get whether tuya_iot_sdk in engineer mode.
*  Return:   TRUE: in engineer mode.
***********************************************************/
BOOL_T tuya_iot_get_engineer_mode(VOID);

#if defined(ENABLE_ENGINEER_TO_NORMAL) && (ENABLE_ENGINEER_TO_NORMAL==1)
/***********************************************************
*  Function: tuya_iot_gw_engineer_check
*  Desc:     gw started from engineer mode to normal mode.
*  Return:   TRUE: OPERATE_RET.
***********************************************************/
OPERATE_RET tuya_iot_gw_engineer_check(IN CONST CHAR_T *engineer_path, ENGR_TO_NORMAL_FINISH_CB cb);
#endif

#ifdef __cplusplus
}
#endif

#endif
