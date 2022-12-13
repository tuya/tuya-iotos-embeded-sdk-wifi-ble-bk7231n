/*
tuya_cloud_error_code.h
Copyright(C),2018-2020, 涂鸦科技 www.tuya.comm
*/

#ifndef TUYA_CLOUD_ERROR_CODE_H
#define TUYA_CLOUD_ERROR_CODE_H

#include "tuya_error_code.h"

#ifdef __cplusplus
extern "C" {
#endif

/* tuya sdk definition of error code */
//typedef int OPERATE_RET;

//#define OPRT_OK                             (0)
//#define OPRT_COM_ERROR                      (-1)
//#define OPRT_INVALID_PARM                   (-2)
//#define OPRT_MALLOC_FAILED                  (-3)
//#define OPRT_NOT_SUPPORTED                  (-4)
//#define OPRT_NETWORK_ERROR                  (-5)
//#define OPRT_TIMEOUT                        (-6)
//#define OPRT_FILE_NOT_FIND                  (-7)
//#define OPRT_INIT_MORE_THAN_ONCE            (-8)
//#define OPRT_INDEX_OUT_OF_BOUND             (-9)
//#define OPRT_RESOURCE_NOT_READY             (-10)

#define OPRT_INIT_MUTEX_ATTR_FAILED         (-101)
#define OPRT_SET_MUTEX_ATTR_FAILED          (-102)
#define OPRT_DESTROY_MUTEX_ATTR_FAILED      (-103)
#define OPRT_INIT_MUTEX_FAILED              (-104)
#define OPRT_MUTEX_LOCK_FAILED              (-105)
#define OPRT_MUTEX_TRYLOCK_FAILED           (-106)
#define OPRT_MUTEX_LOCK_BUSY                (-107)
#define OPRT_MUTEX_UNLOCK_FAILED            (-108)
#define OPRT_MUTEX_RELEASE_FAILED           (-109)
//#define OPRT_CR_MUTEX_ERR                   (-110)
#define OPRT_MEM_PARTITION_EMPTY            (-111)
#define OPRT_MEM_PARTITION_FULL             (-112)
#define OPRT_MEM_PARTITION_NOT_FOUND        (-113)
#define OPRT_DONOT_FOUND_MODULE             (-114)

#define OPRT_INIT_SEM_FAILED                (-201)
#define OPRT_WAIT_SEM_FAILED                (-202)
#define OPRT_POST_SEM_FAILED                (-203)

#define OPRT_THRD_STA_UNVALID               (-301)
#define OPRT_THRD_CR_FAILED                 (-302)
#define OPRT_THRD_JOIN_FAILED               (-303)
#define OPRT_THRD_SELF_CAN_NOT_JOIN         (-304)

#define OPRT_TIMERID_EXIST                  (-401)
#define OPRT_TIMERID_NOT_FOUND              (-402)
#define OPRT_TIMERID_UNVALID                (-403)
#define OPRT_GET_IDLE_TIMERID_ERROR         (-404)

#define OPRT_MSG_NOT_FOUND                  (-501)
#define OPRT_MSG_LIST_EMPTY                 (-502)

#define OPRT_WIFI_SCAN_FAIL                 (-601)
#define OPRT_WF_MAC_SET_FAIL                (-602)
#define OPRT_WF_CONN_FAIL                   (-603)
#define OPRT_WF_NW_CFG_FAIL                 (-604)
#define OPRT_WF_AP_SACN_FAIL                (-605)
#define OPRT_WF_NOT_FIND_ASS_AP             (-606)
#define OPRT_DISCONNECTED_WITH_ROUTER       (-607)


//#define OPRT_SOCK_ERR                       (-701)
//#define OPRT_SET_SOCK_ERR                   (-702)
//#define OPRT_SOCK_CONN_ERR                  (-703)
#define OPRT_BUF_NOT_ENOUGH                 (-704)
#define OPRT_URL_PARAM_OUT_LIMIT            (-705)
#define OPRT_HTTP_OS_ERROR                  (-706)
#define OPRT_HTTP_PR_REQ_ERROR              (-707)
#define OPRT_HTTP_SD_REQ_ERROR              (-708)
#define OPRT_HTTP_RD_ERROR                  (-709)
#define OPRT_HTTP_AD_HD_ERROR               (-710)
#define OPRT_HTTP_GET_RESP_ERROR            (-711)
#define OPRT_HTTP_AES_INIT_ERR              (-712)
#define OPRT_HTTP_AES_OPEN_ERR              (-713)
#define OPRT_HTTP_AES_SET_KEY_ERR           (-714)
#define OPRT_HTTP_AES_ENCRYPT_ERR           (-715)
#define OPRT_CR_HTTP_URL_H_ERR              (-716)
#define OPRT_HTTPS_HANDLE_FAIL              (-717)
#define OPRT_HTTPS_RESP_UNVALID             (-718)
#define OPRT_HTTPS_NO_SUPPORT_RANGE         (-719)
#define OPRT_HTTPS_NOT_ENCRYPT_RET          (-720)
#define OPRT_HTTP_FILL_URL_H_ERR            (-721)
#define OPRT_HTTP_FILL_URL_FULL_ERR         (-722)
#define OPRT_HTTP_FILL_DATA_ERR             (-723)
#define OPRT_HTTP_URL_CFG_AI_SPEAKER_ERR    (-724)
#define OPRT_HTTP_URL_CFG_URL_ERR           (-725)
#define OPRT_HTTP_URL_CFG_URL2IP_ERR        (-726)
#define OPRT_HTTP_URL_CFG_URL2IP_SELF_ERR   (-727)
#define OPRT_HTTP_URL_CFG_URL2IP_PSK_ERR    (-728)



//#define OPRT_CR_CJSON_ERR                   (-801)
//#define OPRT_CJSON_PARSE_ERR                (-802)
//#define OPRT_CJSON_GET_ERR                  (-803)

//#define OPRT_NOT_FOUND                      (-901)
#define OPRT_DP_ATTR_ILLEGAL                (-902)
#define OPRT_DP_TYPE_PROP_ILLEGAL           (-903)
#define OPRT_DP_REPORT_CLOUD_ERR            (-904)
#define OPRT_NO_NEED_SET_PRODINFO           (-905)
#define OPRT_NW_INVALID                     (-906)
#define OPRT_SELECT_ERR                     (-907)
#define OPRT_SELECT_TM                      (-908)
//#define OPRT_SEND_ERR                       (-909)
#define OPRT_DEV_NOT_BIND                   (-910)
#define OPRT_FW_UG_FAILED                   (-911)
#define OPRT_VER_FMT_ERR                    (-912)
#define OPRT_FW_NOT_EXIST                   (-913)
#define OPRT_SEM_CR_ERR                     (-914)
#define OPRT_SELECT_TIMEOUT                 (-915)
#define OPRT_GW_MQ_OFFLILNE                 (-916)
#define OPRT_NOT_SPT_CLX_DP                 (-917)
//#define OPRT_RECV_ERR                       (-918)
#define OPRT_UG_PKG_NOT_ENOUGH              (-919)
#define OPRT_SCMA_INVALID                   (-920)
#define OPRT_PRODECT_KEY_NULL               (-921)
#define OPRT_DEVICE_VER_NULL                (-922)
#define OPRT_MSG_OUT_OF_LMT                 (-923)
#define OPRT_NOT_FOUND_AUTH_SSID            (-924)
#define OPRT_SOCKET_FAULT                   (-925)
#define OPRT_MQ_PUBLISH_TIMEOUT             (-926)
#define OPRT_GW_NOT_EXIST                   (-927)
#define OPRT_GW_SCHEMA_SIZE_LMT_OUT         (-928)
#define OPRT_DEV_DP_CNT_INVALID             (-929)
#define OPRT_TOKEN_OVERTIME                 (-930)
#define OPRT_WF_NW_CFG_RECV_CONTINUE        (-931)
//#define OPRT_RECV_DA_NOT_ENOUGH             (-932)
#define OPRT_SERV_VRFY_FAIL                 (-933)
//#define OPRT_KVS_WR_FAIL                    (-934)
//#define OPRT_KVS_RD_FAIL                    (-935)
#define OPRT_NO_AUTHENTICATION              (-936)
#define OPRT_CMD_OUT_OF_TIME_NOT_EXEC       (-937)
#define OPRT_CMD_NOT_EXEC                   (-938)
//#define OPRT_CRC32_FAILED                   (-939)
#define OPRT_LAN_NO_CLIENT                  (-940)
#define OPRT_WF_INTF_LACK_REG_CB            (-941)
#define OPRT_WF_INTF_NO_REG_CB              (-942)
#define OPRT_STORAGE_UPLOAD                 (-943)
#define OPRT_DP_ID_NOT_FOUND                (-944)
#define OPRT_DP_TP_NOT_MATCH                (-945)
#define OPRT_DEV_NEED_REGISTER              (-946)
#define OPRT_DEV_RESET_FACTORY              (-947)
#define OPRT_API_VERSION_WRONG              (-948)
#define OPRT_DEV_ALREADY_BIND               (-949)
#define OPRT_API_DECODE_FAILED              (-950)
#define OPRT_API_TOKEN_EXPIRE               (-951)
#define OPRT_NOT_EXISTS                     (-952)
#define OPRT_DP_ALREADY_PROCESS             (-953)
#define OPRT_API_VERIFY_FAILED              (-954)
#define OPRT_FILE_IS_FULL                   (-999)
#define OPRT_WRITE_FILE_FAILED              (-1000)
#define OPRT_OPEN_FILE_FAILED               (-1001)
#define OPRT_PRINT_LOG_LEVEL_HIGHER         (-1002)
#define OPRT_FORMAT_STRING_FAILED           (-1003)
#define OPRT_STRING_TOO_LONG                (-1004)
#define OPRT_WR_FLASH_ERROR                 (-1005)

#define OPRT_ROUTER_NOT_FIND                (-1006)
//#define OPRT_EXCEED_UPPER_LIMIT             (-1007)
#define OPRT_SECURITY_VERIFY_NOT_OK         (-1008)
#define OPRT_FLASH_NOT_ENOUGH_PAGE          (-1009)
#define OPRT_ERASE_FLASH_ERROR              (-1088) // check the return value by custom

#define OPRT_OSS_OPERATION_INIT_FAILED      (-1100)
#define OPRT_OSS_OPERATION_UPDATE_FAILED    (-1101)
#define OPRT_OSS_OPERATION_UPLOAD_FAILED    (-1102)
#define OPRT_MOTION_DETECTION_ALARM_PACK_REPORT_FAILED      (-1103)
#define OPRT_TRANSFER_INIT_FAILED           (-1104)

#define OPRT_LOG_SEQ_LOCKED                 (-1200)
#define OPRT_LOG_SEQ_DONT_LOCKED            (-1201)

#define OPRT_LOCAL_LINK_IGNORE              (-1300)
#define OPRT_LOCAL_LINK_CONTINUE            (-1301)

/*img_proc*/

#define	OPRT_IMG_PROC_IMG_TYPE              (-1201)    
#define	OPRT_IMG_PROC_IMG_SIZE              (-1202)  
#define	OPRT_IMG_PROC_PTR_NULL              (-1203)
#define	OPRT_IMG_PROC_ARRAY_SIZE            (-1204)  
#define	OPRT_IMG_PROC_SCALE_SIZE            (-1205) 
#define	OPRT_IMG_PROC_PARA_ERROR            (-1206)

/* ipc related */
#define OPRT_EVENT_STATUS_INVALID           (-2000)
#define OPRT_ENCRYPT_KEY_UPDATED            (-2001)
#define OPRT_ORDER_EXPIRE                   (-2002)
#define OPRT_NO_MORE_DATA                   (-2003)
#define OPRT_CLOUD_STORAGE_OFF 			    (-2004)
#define OPRT_CLOUD_STORAGE_STATUS_INVALID   (-2005)
#define OPRT_REMOTE_API_RUN_UNKNOW_FAILED   (-2006)
#define OPRT_DEVICE_REMOVED                 (-2007)
#define OPRT_SIGNATURE_ERROR                (-2008)
#define OPRT_DEVICE_NOT_WRONG               (-2009)
#define OPRT_WORK_NUM_EXIST                 (-2010)
#define OPRT_SERVER_NOT_CONNECTED           (-2011)
#define OPRT_AUTH_FAIL                      (-2012)
#define OPRT_LOCAL_AI_FACE_NOT_INIT         (-2013)
#define OPRT_LOCAL_AI_FACE_DB_ERROR         (-2014)
#define OPRT_UPLOAD_INTERRUPT_ERROR         (-2015)

#define OPRT_DEVICE_IS_SUB_NODE_OR_IN_ELECTION         (-2016)


#define OPRT_WARN_LIVING_CHROMECAST         (148)
#define OPRT_WARN_LIVING_ECHOSHOW           (149)
#define OPRT_INFO_QUIT_CHROMECAST           (150)
#define OPRT_INFO_QUIT_ECHOSHOW             (151)

//mqtt
#define MQTT_DEF_ERR                        (-3000)
#define MQTT_INVALID_PARM                   (-3001)
#define MQTT_MALLOC_FAILED                  (-3002)
#define MQTT_DNS_PARSED_FAILED              (-3010)
#define MQTT_SOCK_CREAT_FAILED              (-3011)
#define MQTT_SOCK_CET_FAILED                (-3012)
#define MQTT_TCP_CONNECD_FAILED             (-3013)
#define MQTT_TCP_TLS_CONNECD_FAILED         (-3014)
#define MQTT_PACK_SEND_FAILED               (-3015)
#define MQTT_RECV_DATA_FORMAT_WRONG         (-3016)
#define MQTT_RECV_DATA_MSGID_NOT_MATCH      (-3017)
#define MQTT_START_TM_MSG_ERR               (-3018)
#define MQTT_OVER_MAX_MESSAGE_LEN           (-3019)
#define MQTT_PING_SEND_ERR                  (-3020)









#ifdef __cplusplus
}
#endif
#endif

