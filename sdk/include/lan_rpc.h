 /*============================================================================
 *                                                                            *
 * Copyright (C) by Tuya Inc                                                  *
 * All rights reserved                                                        *
 *                                                                            *
 =============================================================================*/
#ifndef __LAN_RPC_H__
#define __LAN_RPC_H__


/*============================ INCLUDES ======================================*/
#include "rpc_udp.h"

#include "tuya_cloud_com_defs.h"
#include "tuya_cloud_types.h"

/*============================ MACROS ========================================*/
#define LAN_RPC_ID_LEN                  (GW_ID_LEN)
#define LAN_RPC_KEY_LEN                 (LOCAL_KEY_LEN)
#define LAN_RPC_NODE_TIMEOUT            20

#define LAN_RPC_NODE_ADD                 0   
#define LAN_RPC_NODE_DEL                 1 
#define LAN_RPC_NODE_REJOIN              2

/*============================ TYPES =========================================*/
typedef enum {
    LAN_RPC_SLAVER,
    LAN_RPC_MASTER,
} LAN_RPC_TYPE;

typedef struct {
    UINT_T              ip;
    INT_T               seqno;
    USHORT_T 			cap;
    UINT8_T             id[LAN_RPC_ID_LEN + 1];
    UINT8_T             key[LAN_RPC_KEY_LEN + 1]; //key是非字符串的
} LAN_RPC_NODE_S;

typedef struct {
    UINT8_T             owner;
    UINT8_T             stat;
    LAN_RPC_NODE_S      node;
} LAN_RPC_MASTER_S;

typedef OPERATE_RET (*LAN_RPC_MASTER_GET_CB)(LAN_RPC_MASTER_S *master);
typedef OPERATE_RET (*LAN_RPC_NODE_STAT_CB)(UINT8_T stat, LAN_RPC_NODE_S *node);
typedef VOID (*LAN_RPC_NODE_OUTPUT_CB)(LAN_RPC_NODE_S *node, UINT16_T num, VOID *arg);



typedef struct {
    UINT8_T                 type;
    LAN_RPC_NODE_STAT_CB    node_stat_cb;
    LAN_RPC_MASTER_GET_CB   master_get_cb;
} LAN_RPC_CFG_S;
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
INT_T lan_rpc_fd_get(VOID);
VOID lan_rpc_node_timeout_check(VOID);
OPERATE_RET lan_rpc_node_add(LAN_RPC_NODE_S *node);
BOOL_T lan_rpc_node_find_by_ip(UINT_T ip, LAN_RPC_NODE_S *node);
BOOL_T lan_rpc_node_find_ip_by_id(UINT8_T *id, UINT_T *ip);
OPERATE_RET lan_rpc_node_key_set(UINT8_T *id, UINT8_T *key);
OPERATE_RET lan_rpc_node_output(LAN_RPC_NODE_OUTPUT_CB node_output_cb, VOID *arg);
OPERATE_RET lan_rpc_session_key_set(UINT8_T *session_key);
OPERATE_RET lan_rpc_init(LAN_RPC_CFG_S *cfg);
OPERATE_RET lan_rpc_slave_call_sync(JRPC_MSG_S *requset, JRPC_MSG_S *result, UINT_T timeout);
OPERATE_RET lan_rpc_master_call_sync(UINT8_T *id, JRPC_MSG_S *requset, JRPC_MSG_S *result, UINT_T timeout);
OPERATE_RET lan_rpc_call_async(UINT8_T *id, JRPC_MSG_S *requset, RPC_RET_CB cb, VOID *cb_param, UINT_T timeout);
OPERATE_RET lan_rpc_recv(VOID);
OPERATE_RET lan_rpc_method_register(CHAR_T *name, RPC_METHOD_CB cb, VOID *arg);


#endif
