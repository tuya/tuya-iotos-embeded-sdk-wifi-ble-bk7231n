/***********************************************************
*  File: uni_module.h
*  Author: nzy
*  Date: 120427
***********************************************************/
#ifndef _UNI_MODULE_H
#define _UNI_MODULE_H

#include "tuya_cloud_types.h"
#include "tuya_base_utilities.h"
#include "uni_msg_queue.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _UNI_MODULE_GLOBAL
    #define _UNI_MODULE_EXT
#else
    #define _UNI_MODULE_EXT extern
#endif

typedef PVOID_T MODULE_HANDLE;

_UNI_MODULE_EXT \
OPERATE_RET ReleaseModule(IN CONST MODULE_HANDLE moduleHandle);


_UNI_MODULE_EXT \
OPERATE_RET CreateModuleAndStart(OUT MODULE_HANDLE *pModuleHandle,\
                                 IN CONST P_CONSTRUCT_FUNC enter,\
                                 IN CONST P_EXTRACT_FUNC exit,\
                                 IN CONST THRD_PARAM_S *thrd_param);

_UNI_MODULE_EXT \
OPERATE_RET ModulePostMessage(IN CONST MODULE_HANDLE moduleHandle,\
                              IN CONST MSG_ID msgID,\
                              IN CONST P_MSG_DATA pMsgData,\
                              IN CONST MSG_DATA_LEN msgDataLen);

_UNI_MODULE_EXT \
OPERATE_RET ModPostMsgMalloc(IN CONST MODULE_HANDLE moduleHandle,\
                             IN CONST MSG_ID msgID,\
                             IN CONST P_MSG_DATA pMsgData,\
                             IN CONST MSG_DATA_LEN msgDataLen);

_UNI_MODULE_EXT \
OPERATE_RET ModulePostInstancyMsg(IN CONST MODULE_HANDLE moduleHandle,\
                                  IN CONST MSG_ID msgID,\
                                  IN CONST P_MSG_DATA pMsgData,\
                                  IN CONST MSG_DATA_LEN msgDataLen);

_UNI_MODULE_EXT \
OPERATE_RET ModPostInstancyMsgMalloc(IN CONST MODULE_HANDLE moduleHandle,\
                                     IN CONST MSG_ID msgID,\
                                     IN CONST P_MSG_DATA pMsgData,\
                                     IN CONST MSG_DATA_LEN msgDataLen);

_UNI_MODULE_EXT \
OPERATE_RET ModuleRegisterMsgCb(IN CONST MODULE_HANDLE moduleHandle,\
                                IN CONST MSG_CALLBACK msg_cb,\
                                OUT MSG_ID *msg_id);


_UNI_MODULE_EXT \
OPERATE_RET ModuleUnregisterMsgCb(IN CONST MODULE_HANDLE moduleHandle,IN CONST MSG_ID msgID);

_UNI_MODULE_EXT \
OPERATE_RET module_cr_tm_msg_hand(IN CONST MODULE_HANDLE moduleHandle,IN CONST VOID *data,\
                                  IN CONST TM_MSG_CB cb,OUT TM_MSG_S **tm_msg);


_UNI_MODULE_EXT \
OPERATE_RET module_start_tm_msg(IN CONST TM_MSG_S *tm_msg,IN CONST TIME_MS timeCycle,\
                                IN CONST TIMER_TYPE timer_type);

_UNI_MODULE_EXT \
OPERATE_RET module_stop_tm_msg(IN CONST TM_MSG_S *tm_msg);


_UNI_MODULE_EXT \
OPERATE_RET module_release_tm_msg(IN CONST TM_MSG_S *tm_msg);

_UNI_MODULE_EXT \
OPERATE_RET ModuleGetMsgNodeNum(IN CONST MODULE_HANDLE moduleHandle,OUT PINT_T pMsgNodeNum);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

