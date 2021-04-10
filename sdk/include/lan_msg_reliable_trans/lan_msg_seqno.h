#ifndef __LAN_MSG_SEQNO_H
#define __LAN_MSG_SEQNO_H

#include "tuya_cloud_com_defs.h"


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


OPERATE_RET lan_msg_init_seqno(IN CONST CHAR_T *id);

OPERATE_RET lan_msg_get_seqno(IN CONST CHAR_T *id, OUT INT_T **send_seq, OUT INT_T **recv_seq);


#ifdef __cplusplus
}
#endif
#endif

