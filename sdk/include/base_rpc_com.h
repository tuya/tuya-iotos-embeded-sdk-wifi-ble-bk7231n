/***********************************************************
*  File: base_rpc_com.h
*  Author: nzy
*  Date: 20180901
***********************************************************/
#ifndef _BASE_RPC_COM_H
#define _BASE_RPC_COM_H

#include "tuya_cloud_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _BASE_RPC_COM_GLOBAL
    #define _BASE_RPC_COM_EXT 
#else
    #define _BASE_RPC_COM_EXT extern
#endif


/***********************************************************
*************************micro define***********************
***********************************************************/
#define COM_KEY_LEN_LMT             16

#define BRC_SET_PRIV_DATA_CMD       0
#define BRC_GET_PRIV_DATA_CMD       1

typedef VOID * BRC_HAND_T;
//server发送回调
typedef INT_T (*BRC_SEND_FUNC)(IN CONST BRC_HAND_T hand,IN CONST CHAR_T *com_key,IN BYTE_T *data,IN CONST UINT_T len);

//Server命令回调函数
typedef VOID (*RPC_RECV_CB)(IN BRC_HAND_T hand, IN CONST CHAR_T *com_key, IN CONST VOID *input, IN CONST UINT_T len, IN CONST USHORT_T snd_seqno, IN BYTE_T *enc_key, IN CONST BOOL_T rsp);

//client调用结果返回
typedef VOID (*RPC_TRANSFER_CB)(IN OPERATE_RET op_ret,OUT VOID *output,OUT UINT_T out_len,IN VOID *prv_data, IN BYTE_T *enc_key);


/***********************************************************
*************************variable define********************
***********************************************************/

/***********************************************************
*************************function define********************
***********************************************************/
/***********************************************************
*  Function: cr_and_init_brc_hand
*  Input: send
*  Input: recv
*  Output: hand 
*  Return: OPERATE_RET
***********************************************************/
_BASE_RPC_COM_EXT \
OPERATE_RET cr_and_init_brc_hand(IN BRC_SEND_FUNC send, IN RPC_RECV_CB recv, OUT BRC_HAND_T *hand);
/***********************************************************
*  Function: brc_data_recv_process
*  Input: hand data len
*  Output: none 
*  Return: OPERATE_RET
***********************************************************/
_BASE_RPC_COM_EXT \
OPERATE_RET brc_data_recv_process(IN BRC_HAND_T         hand, 
                                  IN CHAR_T             *com_key,
                                  IN BYTE_T             *data,
                                  IN UINT_T             len,
                                  IN UINT8_T*		enc_key);

_BASE_RPC_COM_EXT \
OPERATE_RET rpc_client_call_async(IN BRC_HAND_T         hand,
                                  IN CHAR_T             *com_key, 
                                  IN VOID               *input,
                                  IN UINT_T             in_len,
                                  IN UINT_T             timeout,
                                  IN RPC_TRANSFER_CB    cb,
                                  IN VOID               *pri_data,
                                  IN UINT8_T*			enc_key);

_BASE_RPC_COM_EXT \
OPERATE_RET rpc_serv_respsond_call(IN BRC_HAND_T        hand, 
                                   IN CHAR_T            *com_key,    
                                   IN VOID              *input,
                                   IN UINT_T            in_len,  
                                   IN USHORT_T          snd_seqno,
                                   IN UINT8_T*			enc_key); 
/***********************************************************
*  Function: brc_control
*  Input: hand cmd args
*  Output: args 
*  Return: OPERATE_RET
***********************************************************/
_BASE_RPC_COM_EXT \
OPERATE_RET brc_control(IN BRC_HAND_T hand, IN UCHAR_T cmd, INOUT VOID *args);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

