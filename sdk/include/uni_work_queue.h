/***********************************************************
*  File: uni_work_queue.h
*  Author: nzy
*  Date: 20161122
***********************************************************/
#ifndef _UNI_WORK_QUEUE_H
    #define _UNI_WORK_QUEUE_H

    #include "tuya_cloud_types.h"
    #include "tuya_base_utilities.h"

#ifdef __cplusplus
    extern "C" {
#endif

#ifdef  __UNI_WORK_QUEUE_GLOBALS
    #define __UNI_WORK_QUEUE_EXT
#else
    #define __UNI_WORK_QUEUE_EXT extern
#endif

/***********************************************************
*************************micro define***********************
***********************************************************/
typedef PVOID_T WORK_QUEUE_HANDLE;
typedef VOID (*WK_CALLBACK)(VOID *data);

/***********************************************************
*************************variable define********************
***********************************************************/


/***********************************************************
*************************function define********************
***********************************************************/
/***********************************************************
*  Function: work_queue_create
*  Input: hand stack_size pri work_num
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
__UNI_WORK_QUEUE_EXT \
OPERATE_RET work_queue_create(OUT WORK_QUEUE_HANDLE *phand,\
                              IN CONST STACK_SIZE stack_size,\
                              IN CONST TRD_PRI pri,\
                              IN CONST UINT_T work_num);

/***********************************************************
*  Function: work_queue_add
*  Input: hand cb data
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
__UNI_WORK_QUEUE_EXT \
OPERATE_RET work_queue_add(IN CONST WORK_QUEUE_HANDLE hand,IN CONST WK_CALLBACK cb,\
                                  IN CONST VOID *data);
__UNI_WORK_QUEUE_EXT \
OPERATE_RET work_queue_check_and_del(IN CONST WORK_QUEUE_HANDLE hand, IN CONST VOID *data);

__UNI_WORK_QUEUE_EXT \
OPERATE_RET work_queue_release(IN CONST WORK_QUEUE_HANDLE hand);

__UNI_WORK_QUEUE_EXT \
UINT_T work_queue_get_depth(IN CONST WORK_QUEUE_HANDLE hand);


#ifdef __cplusplus
}
#endif
#endif

