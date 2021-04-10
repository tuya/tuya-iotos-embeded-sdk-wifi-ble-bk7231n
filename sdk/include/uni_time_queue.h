/***********************************************************
*  File: uni_time_queue.h
*  Author: nzy
*  Date: 120427
***********************************************************/
#ifndef _UNI_TIME_QUEUE_H
#define _UNI_TIME_QUEUE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "tuya_cloud_types.h"

#ifdef _UNI_TIME_QUEUE_GLOBAL
    #define _UNI_TIME_QUEUE_EXT 
#else
    #define _UNI_TIME_QUEUE_EXT extern
#endif

/***********************************************************
*************************micro define***********************
***********************************************************/
typedef PVOID_T TIMER_QUE_HANDLE; // 定时器队列操作句柄
typedef WORD_T TIMER_ID;  // 定时器ID



// 设置的定时器处理函数
typedef VOID (* P_TIMER_FUNC)(UINT_T timerID,PVOID_T pTimerArg);

typedef enum {
    TIMER_ONCE = 0,
    TIMER_CYCLE,
}TIMER_TYPE;

/***********************************************************
*************************variable define********************
***********************************************************/

/***********************************************************
*************************function define********************
***********************************************************/
_UNI_TIME_QUEUE_EXT \
OPERATE_RET CreateTimerQueAndInit(OUT TIMER_QUE_HANDLE *pTimerQueHandle);

_UNI_TIME_QUEUE_EXT \
OPERATE_RET AddTimerToQueue(IN CONST TIMER_QUE_HANDLE timerQueHandle,\
                            IN CONST TIME_MS timeCycle,\
                            IN CONST P_TIMER_FUNC pTimerFunc,\
                            OUT TIMER_ID *timerID,\
                            IN CONST PVOID_T pTimerArg);

_UNI_TIME_QUEUE_EXT \
OPERATE_RET DeleteTimerFromQueue(IN CONST TIMER_QUE_HANDLE timerQueHandle,\
                                 IN CONST TIMER_ID timerID);

_UNI_TIME_QUEUE_EXT \
OPERATE_RET StartTimerFromQueue(IN CONST TIMER_QUE_HANDLE timerQueHandle,\
                                IN CONST TIMER_ID timerID,\
                                IN CONST TIME_MS timeCycle,\
                                IN CONST TIMER_TYPE timer_type);

_UNI_TIME_QUEUE_EXT \
OPERATE_RET TriggerTimerFromQueue(IN CONST TIMER_QUE_HANDLE timerQueHandle,\
                                IN CONST TIMER_ID timerID);

_UNI_TIME_QUEUE_EXT \
OPERATE_RET StopTimerFromQueue(IN CONST TIMER_QUE_HANDLE timerQueHandle,\
                               IN CONST TIMER_ID timerID);

_UNI_TIME_QUEUE_EXT \
OPERATE_RET DispatchTimerFromQuene(IN CONST TIMER_QUE_HANDLE timerQueHandle);

_UNI_TIME_QUEUE_EXT
OPERATE_RET GetTimerNodeNum(IN CONST TIMER_QUE_HANDLE timerQueHandle,\
                            OUT PINT_T pTimerNodeNum);

_UNI_TIME_QUEUE_EXT \
OPERATE_RET ReleaseTimerQue(IN CONST TIMER_QUE_HANDLE timerQueHandle);

/***********************************************************
*  Function: GetIdleTimerIdRapid 快速获取空闲的timer_id
*  Input: pMsgQueHandle->消息管理结构句柄
*  Output: p_timer_id
*  Return: OPERATE_RET
*  Date: 20140625
***********************************************************/
_UNI_TIME_QUEUE_EXT \
TIMER_ID GetIdleTimerIdRapid(IN CONST TIMER_QUE_HANDLE timerQueHandle);

#if 0
/***********************************************************
*  Function: GetIdleTimerId 获取空闲的timer_id
*  Input: pMsgQueHandle->消息管理结构句柄
*  Output: p_timer_id
*  Return: OPERATE_RET
*  Date: 20140625
***********************************************************/
_UNI_TIME_QUEUE_EXT \
OPERATE_RET GetIdleTimerId(IN CONST TIMER_QUE_HANDLE timerQueHandle,\
                           OUT TIMER_ID *p_timer_id);
#endif
/***********************************************************
*  Function: IsThisTimerRun 确定该定时器是否运行
*  Input: pMsgQueHandle->消息管理结构句柄
*         p_timer_id
*  Output: none
*  Return: OPERATE_RET
*  Date: 20140625
***********************************************************/
_UNI_TIME_QUEUE_EXT \
BOOL_T IsThisTimerRun(IN CONST TIMER_QUE_HANDLE timerQueHandle,\
                    IN CONST TIMER_ID timer_id);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
