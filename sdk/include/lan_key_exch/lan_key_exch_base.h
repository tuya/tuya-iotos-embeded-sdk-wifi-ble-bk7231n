#ifndef __LAN_KEY_EXCH_BASE_H
#define __LAN_KEY_EXCH_BASE_H

#include "tuya_cloud_com_defs.h"

#ifdef __cplusplus
	extern "C" {
#endif


/***********************************************************
*************************micro define***********************
***********************************************************/
#define INIT_KEY_LEN 16
#define SESSIONKEY_LEN 16

typedef BYTE_T KEY_EXCH_NODE_TYPE_T;
#define KEY_EXCH_NODE_A      		0  
#define KEY_EXCH_NODE_B      		1 

typedef BYTE_T KEY_EXCH_NODE_ST_T;

#define KEY_EXCH_NODE_ST_INIT      		0  
#define KEY_EXCH_NODE_ST_DOING      	1 
#define KEY_EXCH_NODE_ST_DONE     		2 

#define LAN_KEY_EXCH_PORT_TCP 6682	//局域网消息，密钥协商链路端口

typedef VOID_T * LAN_KEY_EXCH_SESSION_HANDLE;

typedef VOID(*KEY_EXCH_CB)(IN CONST CHAR_T *id, KEY_EXCH_NODE_ST_T st, OPERATE_RET ret, UCHAR_T *session_key, UNW_IP_ADDR_T s_peer_ip);

/***********************************************************
*************************variable define********************
***********************************************************/

/***********************************************************
*************************function define********************
***********************************************************/

OPERATE_RET lan_key_exch_session_alloc(IN KEY_EXCH_NODE_TYPE_T node_type, IN INT_T fd, IN BYTE_T * init_key, OUT LAN_KEY_EXCH_SESSION_HANDLE *hanlde);

OPERATE_RET lan_key_exch_session_free(IN LAN_KEY_EXCH_SESSION_HANDLE hdl);

OPERATE_RET lan_key_exch_on_data_recv(IN LAN_KEY_EXCH_SESSION_HANDLE hdl);

OPERATE_RET lan_key_exch_send_randA_msg(IN LAN_KEY_EXCH_SESSION_HANDLE hdl);

OPERATE_RET lan_key_exch_on_fd_error(IN LAN_KEY_EXCH_SESSION_HANDLE hdl);

OPERATE_RET lan_key_exch_get_session_key(IN LAN_KEY_EXCH_SESSION_HANDLE hdl, OUT BYTE_T *session_key);

VOID lan_key_exch_debug_hex_dump(CHAR_T *title, UINT8_T width, UINT8_T *buf, UINT16_T size);


#ifdef __cplusplus
}
#endif
#endif

