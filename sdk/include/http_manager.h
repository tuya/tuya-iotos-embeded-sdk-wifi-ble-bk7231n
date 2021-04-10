#ifndef __HTTP_MANAGER_H_
#define __HTTP_MANAGER_H_

#include "tuya_cloud_types.h"
#include "httpc.h"
#include "tuya_hal_mutex.h"


#define MAX_HTTP_SESSION_NUM    16
#define INVALID_HTTP_SESSION_ID 0xFFFFFFFF
#define MAX_HTTP_URL_LEN        256

typedef http_session_t SESSION_ID;

typedef enum
{
    HTTP_FREE = 0,
    HTTP_DISCONNECT,
    HTTP_CONNECTING,
    HTTP_CONNECTED,
    HTTP_UPLOADING,
}E_HTTP_SESSION_STATE;

typedef struct
{
    SESSION_ID id;
    BOOL_T is_persistent;
    CHAR_T url[MAX_HTTP_URL_LEN];
    E_HTTP_SESSION_STATE state;
}S_HTTP_SESSION;

typedef SESSION_ID (*FUNC_HTTP_SESSION_CREATE)(IN CONST CHAR_T *url,BOOL_T is_persistent);
typedef OPERATE_RET (*FUNC_HTTP_SESSION_SEND)(IN CONST SESSION_ID session,IN CONST http_req_t *req,http_hdr_field_sel_t field_flags);
typedef OPERATE_RET (*FUNC_HTTP_SESSION_RECEIVE)(SESSION_ID session, http_resp_t **resp);
typedef OPERATE_RET (*FUNC_HTTP_SESSION_DESTORY)(SESSION_ID session);
typedef OPERATE_RET (*FUNC_HTTP_SESSION_RECEIVE_DATA)(SESSION_ID session, http_resp_t *pResp, BYTE_T** pDataOut);

typedef struct
{
    S_HTTP_SESSION session[MAX_HTTP_SESSION_NUM];
    BOOL_T inited;
    MUTEX_HANDLE mutex;
    FUNC_HTTP_SESSION_CREATE create_http_session;
    FUNC_HTTP_SESSION_SEND send_http_request;
    FUNC_HTTP_SESSION_RECEIVE receive_http_response;
    FUNC_HTTP_SESSION_DESTORY destory_http_session;
    FUNC_HTTP_SESSION_RECEIVE_DATA receive_http_data;
}S_HTTP_MANAGER;


/* 获取http manager管理实例 */
S_HTTP_MANAGER *get_http_manager_instance(VOID_T);


#endif

