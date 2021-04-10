#ifndef __LAN_PUB_SUB_MQ_BASE_H
#define __LAN_PUB_SUB_MQ_BASE_H

#include "tuya_cloud_com_defs.h"

#ifdef __cplusplus
	extern "C" {
#endif


/***********************************************************
*************************micro define***********************
***********************************************************/
#define LAN_SUBSCRIBE_RPC_NAME 	"LAN_SUBSCRIBE"		//
#define LAN_PUBLISH_RPC_NAME   	"LAN_PUBLISH"	//
#define LAN_PUSH_RPC_NAME   	"LAN_PUSH"	//

typedef struct	{
	CHAR_T *topic;
} LAN_SUB_MSG_S;

#define LAN_SUB_MSG_MAX_LEN 128

typedef struct	{
	CHAR_T *topic;
	CHAR_T *content;
} LAN_PUB_MSG_S;

#define LAN_PUB_MSG_MAX_LEN 512


typedef struct	{
	CHAR_T *topic;
	CHAR_T *content;
	CHAR_T *src_id;	
} LAN_PUSH_MSG_S;

#define LAN_PUSH_MSG_MAX_LEN 512

VOID lan_pub_sub_mq_sub_msgToJson(IN CONST LAN_SUB_MSG_S *msg_node, CHAR_T *msg_json, UINT_T msg_json_buf_len);

//msg_node的生命周期在msg_json之内
OPERATE_RET lan_pub_sub_mq_sub_msgFromJson(IN CONST ty_cJSON *msg_json, LAN_SUB_MSG_S *msg_node);

VOID lan_pub_sub_mq_pub_msgToJson(IN CONST LAN_PUB_MSG_S *msg_node, CHAR_T *msg_json, UINT_T msg_json_buf_len);

//msg_node的生命周期在msg_json之内
OPERATE_RET lan_pub_sub_mq_pub_msgFromJson(IN CONST ty_cJSON *msg_json, LAN_PUB_MSG_S *msg_node);


VOID lan_pub_sub_mq_push_msgToJson(IN CONST LAN_PUSH_MSG_S *msg_node, CHAR_T *msg_json, UINT_T msg_json_buf_len);

//msg_node的生命周期在msg_json之内
OPERATE_RET lan_pub_sub_mq_push_msgFromJson(IN CONST ty_cJSON *msg_json, LAN_PUSH_MSG_S *msg_node);


#ifdef __cplusplus
}
#endif
#endif

