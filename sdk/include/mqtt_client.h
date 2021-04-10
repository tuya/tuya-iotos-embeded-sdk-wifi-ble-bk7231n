/***********************************************************
*  File: mqtt_client.h
*  Author: nzy
*  Date: 20150526
***********************************************************/
#ifndef _MQTT_CLIENT_H
#define _MQTT_CLIENT_H
#ifdef __cplusplus
    extern "C" {
#endif

#include "tuya_cloud_types.h"

#ifdef  __MQTT_CLIENT_GLOBALS
    #define __MQTT_CLIENT_EXT
#else
    #define __MQTT_CLIENT_EXT extern
#endif

/***********************************************************
*************************micro define***********************
***********************************************************/
typedef PVOID_T MQ_HANDLE;
typedef VOID (*MQ_DATA_RECV_CB)(IN BYTE_T *data,IN UINT_T len);
typedef VOID (*MQ_CONNED_CB)(VOID);
typedef VOID (*MQ_DISCONN_CB)(VOID);
typedef VOID (*MQ_CONN_DENY_CB)(IN BYTE_T deny_times);
typedef VOID (*MQ_UPDATE_AUTH_CB)(VOID);

/*
1: permit to connect mqtt
0: not permit to connect mqtt, and reconnect after a random delay
-1: not permit to connect mqtt, and reconnect after a short delay
*/
typedef INT_T (*MQ_PERMIT_CONN_CB)(VOID);

typedef struct {
    CHAR_T *subcribe_topic;
    CHAR_T *client_id;
    CHAR_T *user_name;
    CHAR_T *passwd;
}MQ_CLIENT_IF_S;

// if(op_ret == OPRT_OK) then mqtt_publish_async success else fail.
typedef VOID (*MQ_PUB_ASYNC_IFM_CB)(IN CONST OPERATE_RET op_ret,IN CONST VOID *prv_data);

/***********************************************************
*************************variable define********************
***********************************************************/

/***********************************************************
*************************function define********************
***********************************************************/
/***********************************************************
*  Function: create_mqtt_hand
*  Input: domain_tbl
*         domain_num
*         serv_port
*         mqc_if->mqtt client info
*         s_alive->if(0 == s_alive) then set DEF_MQTT_ALIVE_TIME_S
*  Output: hand
*  Return: OPERATE_RET
***********************************************************/
__MQTT_CLIENT_EXT \
OPERATE_RET create_mqtt_hand(IN CONST CHAR_T **domain_tbl,\
                             IN CONST BYTE_T domain_num,\
                             IN CONST USHORT_T serv_port,\
                             IN CONST BOOL_T  enable_tls,\
                             IN CONST MQ_CLIENT_IF_S *mqc_if,\
                             IN CONST USHORT_T s_alive,\
                             IN CONST MQ_DATA_RECV_CB recv_cb,\
                             IN CONST CHAR_T **wakeup_domain_tbl,\
                             IN CONST BYTE_T wakeup_domain_num,\
                             IN CONST USHORT_T wakeup_serv_port,\
                             OUT MQ_HANDLE *hand);


/***********************************************************
*  Function: mqtt_update_auth
*  Input: hand
*         p_username
*         p_passwd
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
__MQTT_CLIENT_EXT \
OPERATE_RET mqtt_update_auth(IN CONST MQ_HANDLE hand, IN CONST CHAR_T *p_username, IN CONST CHAR_T *p_passwd);

/***********************************************************
*  Function: mqtt_register_cb
*  Input: hand
*         conn_cb
*         dis_conn_cb
*         conn_deny_cb
*         permit_conn_cb
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
__MQTT_CLIENT_EXT \
OPERATE_RET mqtt_register_cb(IN CONST MQ_HANDLE hand,\
                             IN CONST MQ_CONNED_CB conn_cb,\
                             IN CONST MQ_DISCONN_CB dis_conn_cb,\
                             IN CONST MQ_CONN_DENY_CB conn_deny_cb,\
                             IN CONST MQ_PERMIT_CONN_CB permit_conn_cb, \
                             IN CONST MQ_UPDATE_AUTH_CB update_auth_cb);

/***********************************************************
*  Function: mqtt_client_start
*  Input: hand
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
__MQTT_CLIENT_EXT \
OPERATE_RET mqtt_client_start(IN CONST MQ_HANDLE hand,IN CONST CHAR_T *name);

/***********************************************************
*  Function: mqtt_restart
*  Input: hand
*  Output: none
*  Return: none
***********************************************************/
__MQTT_CLIENT_EXT \
VOID mqtt_restart(IN CONST MQ_HANDLE hand);

/***********************************************************
*  Function: mqtt_client_quit
*  Input: hand
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
__MQTT_CLIENT_EXT \
OPERATE_RET mqtt_client_quit(IN CONST MQ_HANDLE hand);

#if defined(ENABLE_IPC) && (LOW_POWER_ENABLE==1)
/***********************************************************
*  Function: mqtt_client_start_lp
*  Input: hand
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
__MQTT_CLIENT_EXT \
OPERATE_RET mqtt_client_start_lp(IN CONST MQ_HANDLE hand,IN CONST CHAR_T *name);

/***********************************************************
*  Function: mqtt_client_exit
*  Input: hand
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
__MQTT_CLIENT_EXT \
OPERATE_RET mqtt_client_exit(IN CONST MQ_HANDLE hand, BOOL_T flag);
#endif

/***********************************************************
*  Function: get_mqtt_conn_stat
*  Input: hand
*  Output: none
*  Return: BOOL_T
***********************************************************/
__MQTT_CLIENT_EXT \
BOOL_T get_mqtt_conn_stat(IN CONST MQ_HANDLE hand);

/***********************************************************
*  Function: mq_disconnect
*  Input: hand
*  Output: none
*  Return: none
***********************************************************/
__MQTT_CLIENT_EXT \
VOID mq_disconnect(IN CONST MQ_HANDLE hand);

/***********************************************************
*  Function: release_mqtt_hand
*  Input: hand
*  Output: none
*  Return: none
***********************************************************/
__MQTT_CLIENT_EXT \
VOID release_mqtt_hand(IN CONST MQ_HANDLE hand);


/***********************************************************
*  Function: mq_close
*  Input: hand
*  Output: none
*  Return: none
***********************************************************/
__MQTT_CLIENT_EXT \
VOID mq_close(IN CONST MQ_HANDLE hand);

/***********************************************************
*  Function: mqtt_publish_async
*  Input: hand
*         topic
*         qos if(0 == qos) then to_lmt cb prv_data useless,
*                 becase no respond wait.
*             else if(1 == qos) then need wait respond.
*             else then do't support.
*         data
*         len
*         to_lmt timeout limit if(0 == to_lmt) then use system default limit
*         cb
*         prv_data
*  Output: hand
*  Return: OPERATE_RET
***********************************************************/
__MQTT_CLIENT_EXT \
OPERATE_RET mqtt_publish_async(IN CONST MQ_HANDLE hand,IN CONST CHAR_T *topic,IN CONST BYTE_T qos,\
                               IN CONST BYTE_T *data,IN CONST INT_T len,IN CONST UINT_T to_lmt,\
                               IN CONST MQ_PUB_ASYNC_IFM_CB cb,IN VOID *prv_data);


__MQTT_CLIENT_EXT \
INT_T mqtt_get_socket_fd(IN CONST MQ_HANDLE hand);

__MQTT_CLIENT_EXT \
INT_T mqtt_get_alive_time_s(IN CONST MQ_HANDLE hand);

__MQTT_CLIENT_EXT \
OPERATE_RET mqtt_book_wakeup_topic(IN CONST MQ_HANDLE hand,IN CONST CHAR_T *wakeup_topic);

__MQTT_CLIENT_EXT \
OPERATE_RET mqtt_book_additional_topic(IN CONST MQ_HANDLE hand, IN CONST CHAR_T *topic);

OPERATE_RET mqtt_keep_alive(IN CONST MQ_HANDLE hand);


#ifdef __cplusplus
}
#endif
#endif

