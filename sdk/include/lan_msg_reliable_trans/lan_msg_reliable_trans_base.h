#ifndef __LAN_MSG_RELIABLE_TRANS_BASE_H
#define __LAN_MSG_RELIABLE_TRANS_BASE_H

#include "tuya_cloud_com_defs.h"
#include "lan_msg_reliable_trans.h"

#ifdef __cplusplus
	extern "C" {
#endif


OPERATE_RET lan_msg_reli_trans_type_init();

OPERATE_RET lan_msg_reli_trans_type_add(LAN_MSG_RELI_TYPE_PARAM *type_param, LAN_MSG_RELI_TYPE *type);

OPERATE_RET lan_msg_reli_trans_type_check(LAN_MSG_RELI_TYPE type);

OPERATE_RET lan_msg_reli_trans_type_get(LAN_MSG_RELI_TYPE type, LAN_MSG_RELI_TYPE_PARAM **param);


#ifdef __cplusplus
}
#endif
#endif

