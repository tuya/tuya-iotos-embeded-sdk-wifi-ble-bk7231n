 /*============================================================================
 *                                                                            *
 * Copyright (C) by Tuya Inc                                                  *
 * All rights reserved                                                        *
 *                                                                            *
 =============================================================================*/
#ifndef __JSON_RPC_H__
#define __JSON_RPC_H__

/*============================ INCLUDES ======================================*/
#include "tuya_cloud_types.h"
#include "cJSON.h"

/*============================ MACROS ========================================*/
#define JRPC_PASER_ERROR            -32700
#define JPRC_INVALID_REQUEST        -32600	
#define JPRC_METHOD_NOT_FOUND       -32601

/*============================ TYPES =========================================*/
typedef enum {
    JRPC_REQUEST        = 0,
    JRPC_RESPONSE,
    JRPC_NOTIFICATION,
    JRPC_ERROR,
    JRPC_WRONG_OBJECT
} JRPC_TYPE_E;

typedef enum {
    JRPC_PARAMS         = 0,
    JRPC_RESULT
} JRPC_NODE_TYPE_E;

typedef struct {
    cJSON           *json;
    UINT8_T         *binary;
    JRPC_TYPE_E     type;
} JRPC_MSG_S;

/*============================ PROTOTYPES ====================================*/
OPERATE_RET jrpc_create_request(JRPC_MSG_S *msg, UINT8_T *method, INT_T id);
OPERATE_RET jrpc_create_response(JRPC_MSG_S *msg, INT_T id);
OPERATE_RET jrpc_create_error(JRPC_MSG_S *msg, INT_T id, INT_T code, UINT8_T *message);
OPERATE_RET jrpc_create_notification(JRPC_MSG_S *msg, UINT8_T *method);

OPERATE_RET jrpc_write_int(JRPC_MSG_S *msg, UINT8_T type, UINT8_T *key, INT_T value);
OPERATE_RET jrpc_write_string(JRPC_MSG_S *msg, UINT8_T type, UINT8_T *key, UINT8_T *value);
OPERATE_RET jrpc_read_int(JRPC_MSG_S *msg, UINT8_T type, UINT8_T *key, INT_T *value);
OPERATE_RET jrpc_read_string(JRPC_MSG_S *msg, UINT8_T type, UINT8_T *key, UINT8_T **value);
OPERATE_RET jrpc_read_method(JRPC_MSG_S *msg, UINT8_T **method);
OPERATE_RET jrpc_read_error(JRPC_MSG_S *msg, INT_T *code, UINT8_T **message);

OPERATE_RET jrpc_write_binary(JRPC_MSG_S *msg, UINT8_T type, UINT8_T *key, UINT8_T *binary, UINT16_T size);
OPERATE_RET jrpc_read_binary(JRPC_MSG_S *msg, UINT8_T type, UINT8_T *key, UINT8_T **binary, UINT16_T *size);

VOID jrpc_delete(JRPC_MSG_S *msg);


#endif