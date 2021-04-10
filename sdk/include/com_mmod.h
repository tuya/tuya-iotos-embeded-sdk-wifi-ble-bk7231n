/***********************************************************
*  File: com_mmod.h
*  Author: nzy
*  Date: 20170209
***********************************************************/
#ifndef _COM_MMOD_H
    #define _COM_MMOD_H

    #include "tuya_cloud_types.h"
    #include "uni_msg_queue.h"
    #include "uni_time_queue.h"

#ifdef __cplusplus
    extern "C" {
#endif

#ifdef  __COM_MMOD_GLOBALS
    #define __COM_MMOD_EXT
#else
    #define __COM_MMOD_EXT extern
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
__COM_MMOD_EXT \
OPERATE_RET cmmod_release(VOID);

__COM_MMOD_EXT \
OPERATE_RET cmmod_init(VOID);

__COM_MMOD_EXT \
OPERATE_RET cmmod_reg_msgcb(IN CONST MSG_CALLBACK msg_cb, OUT MSG_ID *msg_id);

__COM_MMOD_EXT \
OPERATE_RET cmmod_unreg_msgcb(IN CONST MSG_ID msgID);

__COM_MMOD_EXT \
OPERATE_RET cmmod_post_msg(IN CONST MSG_ID msgID,IN CONST P_MSG_DATA pMsgData,\
                           IN CONST MSG_DATA_LEN msgDataLen);

__COM_MMOD_EXT \
OPERATE_RET cmmod_post_msg_malloc(IN CONST MSG_ID msgID,IN CONST P_MSG_DATA pMsgData,\
                                  IN CONST MSG_DATA_LEN msgDataLen);

__COM_MMOD_EXT \
OPERATE_RET cmmod_post_instancy_msg(IN CONST MSG_ID msgID,IN CONST P_MSG_DATA pMsgData,\
                                    IN CONST MSG_DATA_LEN msgDataLen);

__COM_MMOD_EXT \
OPERATE_RET cmmod_post_instancy_msg_malloc(IN CONST MSG_ID msgID,IN CONST P_MSG_DATA pMsgData,\
                                           IN CONST MSG_DATA_LEN msgDataLen);

__COM_MMOD_EXT \
OPERATE_RET cmmod_cr_tm_msg_hand(IN CONST TM_MSG_CB cb,IN CONST VOID *data,OUT TM_MSG_S **tm_msg);

__COM_MMOD_EXT \
OPERATE_RET cmmod_start_tm_msg(IN CONST TM_MSG_S *tm_msg,IN CONST TIME_MS timeCycle,\
                               IN CONST TIMER_TYPE timer_type);

__COM_MMOD_EXT \
OPERATE_RET cmmod_stop_tm_msg(IN CONST TM_MSG_S *tm_msg);

__COM_MMOD_EXT \
OPERATE_RET cmmod_release_tm_msg(IN CONST TM_MSG_S *tm_msg);

__COM_MMOD_EXT \
INT_T cmmod_get_msg_node_num(VOID);

#ifdef __cplusplus
}
#endif
#endif

