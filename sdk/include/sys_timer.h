/***********************************************************
*  File: sys_timer.h
*  Author: nzy
*  Date: 20150901
***********************************************************/
#ifndef _SYS_TIMER_H
#define _SYS_TIMER_H

#include "tuya_cloud_types.h"
#include "uni_time_queue.h"

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************
*************************micro define***********************
***********************************************************/

/***********************************************************
*************************variable define********************
***********************************************************/

/***********************************************************
*************************function define********************
***********************************************************/
/***********************************************************
*  Function: system_timer_init 系统定时器初始化
*  Input: none
*  Output: none 
*  Return: OPERATE_RET
***********************************************************/
extern \
OPERATE_RET system_timer_init(void);

/***********************************************************
*  Function: sys_add_timer 添加一个系统定时器
*  Input: timerID->定时器ID
*         pTimerFunc->定时器处理函数
*         pTimerArg->定时器函数处理参数
*  Output: none 
*  Return: OPERATE_RET
***********************************************************/
extern \
OPERATE_RET sys_add_timer(IN CONST P_TIMER_FUNC pTimerFunc,\
                          IN CONST PVOID_T pTimerArg,\
                          OUT TIMER_ID *p_timerID);

/***********************************************************
*  Function: sys_delete_timer 删除一个定时器
*  Input: timerQueHandle->定时器队列管理结构句柄
          timerID->定时器ID
*  Output: none 
*  Return: OPERATE_RET
*  Date: 120427
***********************************************************/
extern \
OPERATE_RET sys_delete_timer(IN CONST TIMER_ID timerID);

/***********************************************************
*  Function: sys_stop_timer 停止一个定时器
*  Input: timerID->定时器ID
*  Output: none 
*  Return: OPERATE_RET
***********************************************************/
extern \
OPERATE_RET sys_stop_timer(IN CONST TIMER_ID timerID);

/***********************************************************
*  Function: 该系统定时器是否运行
*  Input: timerID->定时器ID
*  Output: none 
*  Return: OPERATE_RET
***********************************************************/
extern \
BOOL_T IsThisSysTimerRun(IN CONST TIMER_ID timer_id);

/***********************************************************
*  Function: sys_start_timer 启动一个定时器
*  Input: timerID->定时器ID
*  Output: none 
*  Return: OPERATE_RET
***********************************************************/
extern \
OPERATE_RET sys_start_timer(IN CONST TIMER_ID timerID,\
                            IN CONST TIME_MS timeCycle,\
                            IN CONST TIMER_TYPE timer_type);


/***********************************************************
*  Function: sys_trigger_timer 立即触发一个定时器
*  Input: timerID->定时器ID
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
extern \
OPERATE_RET sys_trigger_timer(IN CONST TIMER_ID timerID);



/***********************************************************
*  Function: system_timer_release 系统定时器资源释放
*  Input: none
*  Output: none 
*  Return: OPERATE_RET
***********************************************************/
extern \
OPERATE_RET system_timer_release(void);

/***********************************************************
*  Function: system_timer_set_sleep_interval 
*  Input: interval
*  Output: none 
*  Return: VOID
***********************************************************/
extern \
VOID system_timer_set_sleep_interval(ULONG_T interval);

extern \
ULONG_T system_timer_get_sleep_interval(VOID);

/***********************************************************
*  Function: sys_get_timer_num 获取timer node数量
*  Input: timerID->定时器ID
*  Output: none
*  Return: INT_T
***********************************************************/
extern \
INT_T sys_get_timer_num(void);



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif








