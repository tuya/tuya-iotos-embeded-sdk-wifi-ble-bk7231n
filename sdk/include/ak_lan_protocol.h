/***********************************************************
*  File: ak_lan_protocol.h 
*  Author: nzy
*  Date: 20170418
***********************************************************/
#ifndef _AK_LAN_PROTOCOL_H
    #define _AK_LAN_PROTOCOL_H

    #include "tuya_cloud_types.h"

#ifdef __cplusplus
	extern "C" {
#endif

#ifdef  __AK_LAN_PROTOCOL_GLOBALS
    #define __AK_LAN_PROTOCOL_EXT
#else
    #define __AK_LAN_PROTOCOL_EXT extern
#endif


/***********************************************************
*  Function: ak_lan_nwc_fin_inform
*  Input: random
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
__AK_LAN_PROTOCOL_EXT \
OPERATE_RET ak_lan_nwc_fin_inform(IN CONST BYTE_T random);

/***********************************************************
*   Function: ak_lan_disc_init
*   Input: appid dev_id
*   Output: none
*   Return: OPERATE_RET
***********************************************************/
__AK_LAN_PROTOCOL_EXT \
OPERATE_RET ak_lan_disc_init(IN CONST CHAR_T* appid,IN CONST CHAR_T* dev_id);

/***********************************************************
*   Function: ak_lan_disc_pack_send
*   Input: none
*   Output: none 
*   Return: none
***********************************************************/
__AK_LAN_PROTOCOL_EXT \
OPERATE_RET ak_lan_disc_pack_send(VOID);

__AK_LAN_PROTOCOL_EXT \
VOID ak_lan_rev_active_task(PVOID_T pArg);

__AK_LAN_PROTOCOL_EXT \
INT_T get_ak_active_status(VOID);

#ifdef __cplusplus
}
#endif
#endif

