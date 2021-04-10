 /*============================================================================
 *                                                                            *
 * Copyright (C) by Tuya Inc                                                  *
 * All rights reserved                                                        *
 *                                                                            *
 =============================================================================*/
#ifndef __RPC_UDP_H__
#define __RPC_UDP_H__

/*============================ INCLUDES ======================================*/
#include "tuya_cloud_types.h"
#include "base_rpc_com.h"
#include "hashmap.h"
#include "json_rpc.h"
#include "uni_thread.h"
#include "tuya_os_adapter.h"

/*============================ MACROS ========================================*/
#define OPRT_RPC_CALL_TIMEOUT       -10
#define OPRT_RPC_CALL_ERROR         -11

#define RPC_DEFAULT_BUFSIZE         256
/*============================ TYPES =========================================*/
typedef enum {
    RPC_CLIENT,
    RPC_SERVER,
    RPC_LOOPIF,
    RPC_CUSTOM,
} RPC_TYPE_E;

typedef enum {
    RPC_BUFSIZE_SET_CMD,
    RPC_CUSTOM_HOST_CMD,
    RPC_KEY_ENABLE_CMD,
    RPC_KEY_SET_CMD,
    RPC_METHOD_UNREGISTER_CMD,
    RPC_EXIT_CMD,
    RPC_CALL_RETRY_CMD,
} RPC_CMD_E;

typedef struct {
    INT_T           udp_fd;
    UINT_T          flag;

    THRD_HANDLE     thread;

    RPC_TYPE_E      type;

    BRC_HAND_T      brchd;
    MUTEX_HANDLE    mutex;

    UINT_T          ipaddr;
    UINT_T          port;

    UINT8_T         key[16];

    map_t           method_list;

    UINT16_T        buffer_size;
    UINT8_T         *buffer;
    UINT8_T         default_buffer[RPC_DEFAULT_BUFSIZE];
} TUYA_RPC_S;

typedef VOID (*RPC_METHOD_CB)(IN JRPC_MSG_S *request, IN VOID *arg, IN UNW_IP_ADDR_T peer_ip, OUT JRPC_MSG_S *reply);

typedef VOID (*RPC_RET_CB)(IN OPERATE_RET op_ret, IN JRPC_MSG_S *result, IN VOID *prv_data);

/*============================ PROTOTYPES ====================================*/
OPERATE_RET tuya_rpc_init(TUYA_RPC_S *rpc, RPC_TYPE_E type, UINT8_T *host);
OPERATE_RET tuya_rpc_run(TUYA_RPC_S *rpc, UINT_T stack_size, UINT_T prio);
OPERATE_RET tuya_rpc_method_register(TUYA_RPC_S *rpc, CHAR_T *name, RPC_METHOD_CB cb, VOID *arg);
OPERATE_RET tuya_rpc_call_sync(TUYA_RPC_S *rpc, JRPC_MSG_S *requset, JRPC_MSG_S *result, UINT_T timeout);
OPERATE_RET tuya_rpc_control(TUYA_RPC_S *rpc, UINT8_T cmd, VOID *arg);

#endif
