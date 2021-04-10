/*
uni_thread.h
Copyright(C),2018-2020, 涂鸦科技 www.tuya.comm
*/

#ifndef _UNI_THREAD_H
#define _UNI_THREAD_H

#include "tuya_cloud_types.h"

#ifdef __cplusplus
extern "C" {
#endif


/* tuya sdk definition of thread running status */
typedef INT_T THRD_STA;
#define STATE_EMPTY 0       // thread is inited
#define STATE_RUNNING 1     // thread is running
#define STATE_STOP 2        // thread is stop
#define STATE_DELETE 3      // thread delete status 

/* tuya sdk definition of thread process function */
typedef VOID (*P_THRD_FUNC)(PVOID_T pArg);

/* tuya sdk definition of thread module context */
typedef PVOID_T THRD_HANDLE;
/* tuya sdk definition of thread enter and exit function */
typedef VOID(*P_CONSTRUCT_FUNC)(VOID); // thread construct
typedef VOID(*P_EXTRACT_FUNC)(VOID); // thread extract

/* tuya sdk definition of thread priority */
typedef unsigned int TRD_PRI;
#define TRD_PRIO_0     5    /* High */
#define TRD_PRIO_1     4
#define TRD_PRIO_2     3
#define TRD_PRIO_3     2
#define TRD_PRIO_4     1
#define TRD_PRIO_5     0
#define TRD_PRIO_6     0    /* low */

/* tuya sdk definition of thread stack size */
typedef unsigned int STACK_SIZE;
/* tuya sdk definition of thread param */
#define MAX_THREAD_NAME_LEN     16
typedef struct {
    STACK_SIZE stackDepth; /* stack size */
    TRD_PRI priority;      /* thread priority */
    CHAR_T *thrdname;      /* thread name */
}THRD_PARAM_S;

/***********************************************************
*  Function: CreateAndStart
*  Desc:     create and start a tuya sdk thread
*  Input:    enter: the function called before the thread process called.can be null
*  Input:    exit: the function called after the thread process called.can be null
*  Input:    pThrdFunc: the main thread process function
*  Input:    pThrdFuncArg: the args of the pThrdFunc.can be null
*  Input:    thrd_param: the param of creating a thread
*  Output:   pThrdHandle: the tuya sdk thread context
*  Return:   OPRT_OK: success  Other: fail
***********************************************************/
OPERATE_RET CreateAndStart(OUT THRD_HANDLE *pThrdHandle,\
                           IN CONST P_CONSTRUCT_FUNC enter,\
                           IN CONST P_EXTRACT_FUNC exit,\
                           IN CONST P_THRD_FUNC pThrdFunc,\
                           IN CONST PVOID_T pThrdFuncArg,\
                           IN CONST THRD_PARAM_S *thrd_param);

/***********************************************************
*  Function: ThreadRunSelfSpace
*  Desc:     check the function caller is in the input thread context
*  Input:    thrdHandle: the input thread context
*  Output:   bl
*  Return:   OPRT_OK: success  Other: fail
***********************************************************/
OPERATE_RET ThreadRunSelfSpace(IN CONST THRD_HANDLE thrdHandle,OUT BOOL_T *bl);

/***********************************************************
*  Function: GetThrdSta
*  Desc:     get the thread context running status
*  Input:    thrdHandle: the input thread context
*  Return:   status
***********************************************************/
THRD_STA GetThrdSta(IN CONST THRD_HANDLE thrdHandle);

/***********************************************************
*  Function: DeleteThrdHandle
*  Desc:     stop and free a tuya sdk thread
*  Input:    thrdHandle: the input thread context
*  Return:   OPRT_OK: success  Other: fail
***********************************************************/
OPERATE_RET DeleteThrdHandle(IN CONST THRD_HANDLE thrdHandle);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

