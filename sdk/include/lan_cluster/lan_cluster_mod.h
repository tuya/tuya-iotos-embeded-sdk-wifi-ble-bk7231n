#ifndef _LAN_CLUSTER_MOD_H
    #define _LAN_CLUSTER_MOD_H

    #include "tuya_cloud_types.h"
    #include "uni_msg_queue.h"
    #include "uni_time_queue.h"

#ifdef __cplusplus
    extern "C" {
#endif

#ifdef  __LAN_CLUSTER_MOD_GLOBALS
    #define __LAN_CLUSTER_MOD_EXT
#else
    #define __LAN_CLUSTER_MOD_EXT extern
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
__LAN_CLUSTER_MOD_EXT \
OPERATE_RET lan_cluster_mod_release(VOID);

__LAN_CLUSTER_MOD_EXT \
OPERATE_RET lan_cluster_mod_init(VOID);

__LAN_CLUSTER_MOD_EXT \
OPERATE_RET  lan_cluster_mod_get_free_msgid(OUT MSG_ID *msg_id);

__LAN_CLUSTER_MOD_EXT \
OPERATE_RET  lan_cluster_mod_reg_msgcb(IN CONST MSG_ID msgID,IN CONST MSG_CALLBACK msg_cb);

__LAN_CLUSTER_MOD_EXT \
OPERATE_RET  lan_cluster_mod_unreg_msgcb(IN CONST MSG_ID msgID);

__LAN_CLUSTER_MOD_EXT \
OPERATE_RET  lan_cluster_mod_post_msg(IN CONST MSG_ID msgID,IN CONST P_MSG_DATA pMsgData,\
                           IN CONST MSG_DATA_LEN msgDataLen);

__LAN_CLUSTER_MOD_EXT \
OPERATE_RET  lan_cluster_mod_post_msg_malloc(IN CONST MSG_ID msgID,IN CONST P_MSG_DATA pMsgData,\
                                  IN CONST MSG_DATA_LEN msgDataLen);

__LAN_CLUSTER_MOD_EXT \
OPERATE_RET  lan_cluster_mod_post_instancy_msg(IN CONST MSG_ID msgID,IN CONST P_MSG_DATA pMsgData,\
                                    IN CONST MSG_DATA_LEN msgDataLen);

__LAN_CLUSTER_MOD_EXT \
OPERATE_RET  lan_cluster_mod_post_instancy_msg_malloc(IN CONST MSG_ID msgID,IN CONST P_MSG_DATA pMsgData,\
                                           IN CONST MSG_DATA_LEN msgDataLen);

__LAN_CLUSTER_MOD_EXT \
OPERATE_RET  lan_cluster_mod_cr_tm_msg_hand(IN CONST TM_MSG_CB cb,IN CONST VOID *data,OUT TM_MSG_S **tm_msg);

__LAN_CLUSTER_MOD_EXT \
OPERATE_RET  lan_cluster_mod_start_tm_msg(IN CONST TM_MSG_S *tm_msg,IN CONST TIME_MS timeCycle,\
                               IN CONST TIMER_TYPE timer_type);

__LAN_CLUSTER_MOD_EXT \
OPERATE_RET  lan_cluster_mod_stop_tm_msg(IN CONST TM_MSG_S *tm_msg);

__LAN_CLUSTER_MOD_EXT \
OPERATE_RET  lan_cluster_mod_release_tm_msg(IN CONST TM_MSG_S *tm_msg);

#ifdef __cplusplus
}
#endif
#endif

