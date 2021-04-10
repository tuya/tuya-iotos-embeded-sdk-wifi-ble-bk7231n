#ifndef __LAN_KEY_EXCH_NODEA_H
#define __LAN_KEY_EXCH_NODEA_H

#include "tuya_cloud_com_defs.h"
#include "uni_network.h"

#include "lan_key_exch_base.h"

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

OPERATE_RET lan_key_exch_nodeA_init();

//OPERATE_RET lan_key_exch_nodeA_add_peer_node(IN UNW_IP_ADDR_T ip, IN CONST BYTE_T *init_key, KEY_EXCH_CB cb);
//OPERATE_RET lan_key_exch_nodeA_del_peer_node(IN UNW_IP_ADDR_T ip);

OPERATE_RET lan_key_exch_nodeA_add_peer_node(IN CONST CHAR_T            *id, IN CONST BYTE_T *init_key, KEY_EXCH_CB cb);

OPERATE_RET lan_key_exch_nodeA_del_peer_node(IN CONST CHAR_T            *id);

OPERATE_RET lan_key_exch_nodeA_del_all_peer_node();

OPERATE_RET lan_key_exch_nodeA_is_peer_node_valid(IN CONST CHAR_T            *id);

OPERATE_RET lan_key_exch_nodeA_get_peer_node_init_key(IN CONST CHAR_T            *id, OUT BYTE_T *init_key);

OPERATE_RET lan_key_exch_nodeA_peer_node_retry(IN CONST CHAR_T            *id);

#ifdef __cplusplus
}
#endif
#endif

