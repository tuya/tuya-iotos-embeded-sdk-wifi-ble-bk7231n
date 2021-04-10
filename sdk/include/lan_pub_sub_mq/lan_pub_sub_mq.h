#ifndef __LAN_PUB_SUB_MQ_H
#define __LAN_PUB_SUB_MQ_H

#include "tuya_cloud_com_defs.h"

#ifdef __cplusplus
	extern "C" {
#endif


/***********************************************************
*************************micro define***********************
***********************************************************/


#define LAN_TOPIC_NAME_LEN_MAX 32

typedef VOID* LAN_PUB_SUB_MQ_HANDLE;

typedef OPERATE_RET (*LAN_TOPIC_RECV_CB)(IN CONST CHAR_T *content, VOID *cb_param);

typedef OPERATE_RET (*LAN_TOPIC_SUB_LOCAL_CB)(IN CONST CHAR_T *topic);
typedef OPERATE_RET (*LAN_TOPIC_PUB_LOCAL_CB)(IN CONST CHAR_T *topic, IN CONST CHAR_T *content);

typedef struct {
	CHAR_T topic_name[LAN_TOPIC_NAME_LEN_MAX + 1];
	LAN_TOPIC_RECV_CB topic_func;	//收到推送后的回调
	VOID *cb_param;					//回调的参数
	BOOL_T omit_self; 				//是否回调自己发布的内容，TRUE为忽略不回掉
} LAN_PUB_SUB_MQ_SUB_PARAM_S;


typedef struct {
	CHAR_T topic_name[LAN_TOPIC_NAME_LEN_MAX + 1];
} LAN_PUB_SUB_MQ_PUB_PARAM_S;


/***********************************************************
*************************variable define********************
***********************************************************/

/***********************************************************
*************************function define********************
***********************************************************/

OPERATE_RET lan_pub_sub_mq_init();

OPERATE_RET lan_pub_sub_mq_add_subscribe_service(IN CONST LAN_PUB_SUB_MQ_SUB_PARAM_S *param);

OPERATE_RET lan_pub_sub_mq_add_publish_service(IN CONST LAN_PUB_SUB_MQ_PUB_PARAM_S *param, OUT LAN_PUB_SUB_MQ_HANDLE *handle);

OPERATE_RET lan_pub_sub_mq_publish(IN LAN_PUB_SUB_MQ_HANDLE handle, IN CONST CHAR_T *content);

//下面是模块内部使用
VOID lan_pub_sub_mq_reg_sub_local_cb(IN LAN_TOPIC_SUB_LOCAL_CB cb);

VOID lan_pub_sub_mq_reg_pub_local_cb(IN LAN_TOPIC_PUB_LOCAL_CB cb);

OPERATE_RET lan_pub_sub_mq_push_local(IN CONST CHAR_T *content_json);


#ifdef __cplusplus
}
#endif
#endif

