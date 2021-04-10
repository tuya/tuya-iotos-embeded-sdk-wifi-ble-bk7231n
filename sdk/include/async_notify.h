/***********************************************************
*  File: async_notify.h
*  Author: nzy
*  Date: 120427
***********************************************************/
#ifndef _ASYNC_NOTIFY_H
#define _ASYNC_NOTIFY_H
    
#ifdef __cplusplus
extern "C" {
#endif

#include "tuya_cloud_types.h"
#include "sys_timer.h"

typedef PVOID_T NOTIFY_HANDLE;
typedef PVOID_T NOTIFY_PARAM;
typedef DWORD_T NOTIFY_TIME_MS;

typedef enum
{
    NOTIFY_FINISH = 0,
    NOTIFY_ERROR,
    NOTIFY_OVERTIME,
}NOTIFY_TYPE_E;
typedef VOID (*P_NOTIFY_FUNC)(IN CONST NOTIFY_TYPE_E type, IN NOTIFY_PARAM param);

typedef struct
{
    TIMER_ID timer;
    P_NOTIFY_FUNC func;
    NOTIFY_PARAM param;
    NOTIFY_TIME_MS time_ms;
}NOTIFY_HANDLE_S;

OPERATE_RET create_notify_handle(OUT NOTIFY_HANDLE *p_handle);

OPERATE_RET delete_notify_handle(IN NOTIFY_HANDLE *p_handle);

OPERATE_RET register_notify_handle(IN NOTIFY_HANDLE handle, \
                                   IN CONST P_NOTIFY_FUNC func, \
                                   IN CONST NOTIFY_PARAM param, \
                                   IN CONST NOTIFY_TIME_MS time_ms);

OPERATE_RET notify_handle_finish(IN NOTIFY_HANDLE handle);

OPERATE_RET notify_handle_error(IN NOTIFY_HANDLE handle);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  /*_ASYNC_NOTIFY_H*/
