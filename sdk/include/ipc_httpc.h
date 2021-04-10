#ifndef _IPC_HTTPC_H
#define _IPC_HTTPC_H

#include "tuya_cloud_types.h"
#include "cJSON.h"
#include "gw_intf.h"
#include <stdarg.h>

#ifdef __cplusplus
	extern "C" {
#endif



OPERATE_RET ipc_printf_log_up(IN CONST PCHAR_T pFile,IN CONST INT_T line,
                          IN CONST PCHAR_T pFunc,IN CONST unsigned char log_type,IN CONST PCHAR_T pFmt,...);


#define tuya_ipc_log 1
#define tuya_p2p_log 2

#ifdef __LITEOS__
#define IPC_PRINT_LOG_UP(logo_type,fmt, ...)
#else

#define IPC_PRINT_LOG_UP(logo_type,fmt, ...)  \
do \
{ \
    ipc_printf_log_up(__FILE__,__LINE__,(PCHAR_T)__func__,(unsigned char)logo_type, \
            fmt,##__VA_ARGS__); \
}while(0)
#endif

#define IPC_PR_UPLOAD(fmt, ...) IPC_PRINT_LOG_UP(tuya_ipc_log,fmt, ##__VA_ARGS__)



/*
NULL

{\"withVideoDataStorage\":\"true\"}
*/
OPERATE_RET httpc_ipc_oss_storage_cfg_get_v10(IN CONST CHAR_T *gw_id, IN CONST CHAR_T *param, OUT cJSON **result);

/*
tutk:1
ppcs:2
*/
OPERATE_RET httpc_ipc_p2p_cfg_get_v20(IN CONST CHAR_T *gw_id, IN CONST INT_T p2p_type, OUT cJSON **result);

OPERATE_RET httpc_ipc_p2p_passwd_update_v10(IN CONST CHAR_T *gw_id, IN CONST CHAR_T *p2p_passwd, OUT cJSON **result);

OPERATE_RET httpc_ipc_certificate_get_v10(OUT cJSON **result,IN CONST CHAR_T *url_msg);

/*
0:start
1:stop
2:event
*/
OPERATE_RET httpc_ipc_cloud_storage_event_notify_v10(IN CONST CHAR_T *gw_id, IN CONST INT_T cloud_type, IN CONST CHAR_T *cloud_msg, OUT cJSON **result);



/*
NULL

{\"withVideoDataStorage\":\"true\"}
*/
OPERATE_RET httpc_ipc_oss_storage_cfg_get(IN CONST CHAR_T *param, OUT cJSON **result);
/* tutk:1   ppcs:2 */
OPERATE_RET httpc_ipc_p2p_cfg_get(IN CONST INT_T p2p_type, OUT cJSON **result);
OPERATE_RET httpc_ipc_p2p_passwd_update(IN CONST CHAR_T *p2p_passwd, OUT cJSON **result);

/* 0:start 1:stop 2:event */
OPERATE_RET httpc_ipc_cloud_storage_event_notify(IN CONST INT_T cloud_type, IN CONST CHAR_T *cloud_msg, OUT cJSON **result);

/**** 自定义的HTTP(S)交互请求
api_name: 与服务端约定的接口名
api_version: 接口使用的版本，如 1.0 2.0
message: 接口中需要设备上报给服务端的数据内容，一版为JSON格式字符串
result: 服务端返回的消息，会在该函数内部动态申请空间，需要在外部处理完result之后调用Free接口释放内存
****/
OPERATE_RET http_gw_ipc_custom_msg(IN CONST CHAR_T *api_name, IN CONST CHAR_T *api_version, IN CONST CHAR_T *message, OUT cJSON **result);

VOID tuya_ipc_log_upload_init(void);

OPERATE_RET httpc_cloud_ai_face_storage_config_get(OUT cJSON **result);
OPERATE_RET httpc_cloud_ai_face_api(IN INT_T cmd_type, IN CHAR_T *msg, OUT cJSON **result);

OPERATE_RET httpc_cloud_ai_config_get(OUT cJSON **result);
OPERATE_RET httpc_cloud_ai_detect_msg_report(IN CHAR_T *msg, OUT cJSON **result);

OPERATE_RET http_gw_ipc_preset_del(IN CONST CHAR_T *message, OUT cJSON **result);
OPERATE_RET http_gw_ipc_preset_get(IN CONST CHAR_T *message, OUT cJSON **result);


#ifdef __cplusplus
}
#endif
#endif

