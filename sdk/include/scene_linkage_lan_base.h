#ifndef __SCENE_LINKAGE_LAN_BASE_H
#define __SCENE_LINKAGE_LAN_BASE_H

#include "tuya_cloud_com_defs.h"
#include "lan_cluster_api.h"


#ifdef __cplusplus
	extern "C" {
#endif


/***********************************************************
*************************micro define***********************
***********************************************************/
typedef struct {
	CHAR_T cid[DEV_ID_LEN+1];
	BYTE_T dp_id;
	BOOL_T check;
} TOGGLE_PARAM_S;	


/***********************************************************
*************************variable define********************
***********************************************************/

/***********************************************************
*************************function define********************
***********************************************************/

OPERATE_RET lan_scene_linkage_base_init(IN CONST CHAR_T * dev_id);


OPERATE_RET lan_dp_condition_base_detect(IN CONST CHAR_T *id, IN CONST CHAR_T *dp_cmd);


typedef OPERATE_RET (*LAN_DP_DETECT_LOCAL)(IN CONST CHAR_T *id, IN CONST CHAR_T *dp_cmd, IN CONST CHAR_T *gw_id);

OPERATE_RET lan_scene_linkage_base_reg_detect_local(IN LAN_DP_DETECT_LOCAL func);

//master下发给普通节点的数据 发请求消息
//dp_cmd {"cid":"000d6ffffe6d8009","dps":{"2":true}}
//{"dp_cmd": {"cid":"000d6ffffe6d8009","dps":2}, "act_type": "toggle", "check": false}
OPERATE_RET lan_msg_master_to_normal_req(IN CONST CHAR_T *gw_id, IN BOOL_T raw, IN CONST CHAR_T *dp_cmd, IN BOOL_T check);

BOOL_T lan_scene_linkage_base_cmp_gwid(IN CONST CHAR_T *gw_id);

VOID lan_scene_linkage_on_mqtt_onlie(IN BOOL_T online);

BOOL_T lan_scene_linkage_master_is_alive();

OPERATE_RET lan_scene_linkage_base_toggle_2_json(IN CONST TOGGLE_PARAM_S *toggle, OUT CHAR_T *json_buf, IN INT_T json_buf_len);

OPERATE_RET lan_scene_linkage_base_scene_2_json(IN CONST CHAR_T *scene_id, OUT CHAR_T *json_buf, IN INT_T json_buf_len);

OPERATE_RET lan_scene_linkage_base_local_scene_2_json(IN CONST CHAR_T *gid, IN CONST CHAR_T *sid, OUT CHAR_T *json_buf, IN INT_T json_buf_len);

#ifdef __cplusplus
}
#endif
#endif

