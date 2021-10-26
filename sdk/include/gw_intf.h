/***********************************************************
*  File: gw_intf.h
*  Author: nzy
*  Date: 20150822
***********************************************************/
#ifndef _GW_INTF_H
#define _GW_INTF_H

#include "tuya_cloud_types.h"
#include "tuya_os_adapter.h"
#include "tuya_base_utilities.h"

#include "uni_msg_queue.h"
#include "tuya_cloud_com_defs.h"
#include "tuya_cloud_wifi_defs.h"
#include "tuya_hal_wifi.h"
#include "tuya_cloud_base_defs.h"
#include "tuya_iot_internal_api.h"

#include "hashmap.h"

#ifdef __cplusplus
    extern "C" {
#endif

#ifdef  __GW_INTF_GLOBALS
    #define __GW_INTF_EXT
#else
    #define __GW_INTF_EXT extern
#endif

/***********************************************************
*************************micro define***********************
***********************************************************/
#define SERIAL_NO_STR_LEN 32
#define GET_ACCESS_TOKEN_INTERVAL  (300 * 1000) //5min

#if TY_SECURITY_CHIP
typedef struct
{
    CHAR_T chipId1[32 + 1];				//芯片ID
    CHAR_T securityCode1[16 + 1];		//安全认证码1
    CHAR_T randomA[8 + 1];				//随机数A
    CHAR_T getTempKey[32];				//临时通信密钥
    CHAR_T cbc_iv[16];					//CBC加密初始密钥
}SECURITY_CHIP_S;

// gw BASE information
typedef struct {
    CHAR_T uuid[GW_UUID_LEN+1];
    CHAR_T auth_key[AUTH_KEY_LEN+1];
    BOOL_T is_security_chip_ok; 		//是否从加密芯片获得授权数据 即uuid和auth_key
}GW_AUTH_INFO_S;

#endif


// gateway network attribute decribe
typedef struct {
    GW_ABI abi;

    // only (tp == GNT_WIFI_NEED_CFG) valid
    #if defined(WIFI_GW) && (WIFI_GW==1)
    GW_WF_CFG_MTHD_SEL mthd;
    BOOL_T enable_station_ap;
    GW_WF_START_MODE wifi_start_mode;
    #endif
#if defined(GW_SUPPORT_WIRED_WIFI) && (GW_SUPPORT_WIRED_WIFI==1)
    IOT_GW_NET_TYPE_T net_mode;
#endif
}GW_CFG_S;

// gw BASE information
typedef struct {
	CHAR_T uuid[GW_UUID_LEN+1];	//配合产测二次授权，需要明文uuid
	CHAR_T psk_key[PSK_KEY_LEN+1];
#if !TY_SECURITY_CHIP    
    CHAR_T auth_key[AUTH_KEY_LEN+1];	
#else
    BOOL_T has_auth;
#endif
    #if defined(WIFI_GW) && (WIFI_GW==1)
    CHAR_T ap_ssid[WIFI_SSID_LEN+1]; // ap ssid info
    CHAR_T ap_passwd[WIFI_PASSWD_LEN+1]; // ap passwd info
    
    #endif
    #if (defined(WIFI_GW) && (WIFI_GW==1)) || (defined(GW_SUPPORT_COUNTRY_CODE) && (GW_SUPPORT_COUNTRY_CODE==1))
	CHAR_T country_code[COUNTRY_CODE_LEN]; //  country code
	#endif
#if defined(GPRS_GW) && (GPRS_GW==1)
    CHAR_T imei[TUYA_IMEI_LEN+1];
    CHAR_T sn[TUYA_SN_LEN+1];
#endif
    BOOL_T prod_test; // production test
	#if defined(TY_BT_MOD) && TY_BT_MOD == 1
    CHAR_T bt_mac[BT_MAC_LEN+1];
    CHAR_T bt_hid[BT_HID_LEN+1];
#endif
#if defined(ENABLE_ALARM) && (ENABLE_ALARM==1)
    UCHAR_T alarm_flag; //产测时写入，表示支持安防功能
#endif
    CHAR_T fac_pin[20 + 1];    // fac_pin == pid
}GW_BASE_IF_S;

#if defined(WIFI_GW) && (WIFI_GW==1)
typedef BYTE_T GW_WF_MD_T;
#define GWM_NORMAL 				0
#define GWM_SPECIAL_SMT_CFG 	1
#define GWM_SPECIAL_AP_CFG 		2
#define GWM_SPECIAL_SMT_AP_CFG 	3
#endif

// gw register status
typedef BYTE_T GW_WORK_STAT_T;
#define UNREGISTERED 0 // unregistered
#define REGISTERED 1 // registered & activate start
#define ACTIVATED 2 // already active

typedef struct {
    // wifi connect info
    #if defined(WIFI_GW) && (WIFI_GW==1)
    GW_WF_NWC_STAT_T nc_tp; // valid when gateway is wireless
    GW_WF_MD_T md;
    CHAR_T ssid[WIFI_SSID_LEN+1];
    CHAR_T passwd[WIFI_PASSWD_LEN+1];
    BYTE_T random; // used to save airkiss random
    #endif
    TY_DNS_PRIO_T dns_prio;

    GW_WORK_STAT_T stat;
    CHAR_T token[TOKEN_LEN+1]; // recvive from app
    CHAR_T region[REGION_LEN + 1]; // recvive from app
    CHAR_T regist_key[REGIST_KEY_LEN + 1]; // recvive from app
}GW_WORK_STAT_MAG_S;


typedef struct {
    GW_ABI abi; // ability
    CHAR_T id[GW_ID_LEN+1]; // virtual id
    CHAR_T sw_ver[SW_VER_LEN+1]; // xx.xx
    CHAR_T bs_ver[SW_VER_LEN+1]; // base version xx.xx
    CHAR_T prtl_ver[SW_VER_LEN+1]; // protocol version
    CHAR_T lan_prtl_ver[SW_VER_LEN+1]; // lan protocol version
    CHAR_T cad_ver[SW_VER_LEN+1]; // cad version
    CHAR_T cd_ver[SW_VER_LEN+1]; // cd version
    CHAR_T product_key[PRODUCT_KEY_LEN+1];
    CHAR_T firmware_key[PRODUCT_KEY_LEN+1]; // firmware-key

    // networklink module single device struct
    CHAR_T dev_sw_ver[SW_VER_LEN+1];
    CHAR_T schema_id[SCHEMA_ID_LEN+1];
    DEV_TYPE_T tp; // only (DEV_NM_ATH_SNGL or DEV_NM_NOT_ATH_SNGL)

    // GW attach attribute
    BYTE_T attr_num;
    GW_ATTACH_ATTR_T attr[GW_ATTACH_ATTR_LMT];

    BOOL_T sync;
    #if defined(TUYA_GW_OPERATOR) && (TUYA_GW_OPERATOR==1)
    CH_CODE_ST ch_dminfo;
    #endif
    #if defined(ENABLE_SIGMESH) && (ENABLE_SIGMESH==1)
    BOOL_T gw_sigmesh_sync;
    #endif
}GW_DESC_IF_S;

typedef struct {
    CHAR_T product_key[PRODUCT_KEY_LEN+1];
    CHAR_T firmware_key[PRODUCT_KEY_LEN+1]; // firmware-key
}GW_STRIDE_UPDATE_S;

typedef struct {
    CHAR_T key[SEC_KEY_LEN+1];
    CHAR_T local_key[LOCAL_KEY_LEN+1];
    CHAR_T psk21_key[TUYA_PSK21_LEN+1];
    CHAR_T http_url[HTTP_URL_LMT+1];
    CHAR_T http_ip[HTTP_URL_LMT+1];
    CHAR_T https_url[HTTP_URL_LMT+1];
    CHAR_T https_ip[HTTP_URL_LMT+1];
    CHAR_T httpsPSK_url[HTTP_URL_LMT+1];
    CHAR_T httpsPSK_ip[HTTP_URL_LMT+1];

    CHAR_T mq_psk_url[MQ_URL_LMT+1];
    CHAR_T mq_psk_ip[MQ_URL_LMT+1];
    CHAR_T mq_tls_url[MQ_URL_LMT+1];
    CHAR_T mq_tls_ip[MQ_URL_LMT+1];
    CHAR_T mq_proto_url[MQ_URL_LMT+1];
    CHAR_T mq_proto_ip[MQ_URL_LMT+1];
    CHAR_T ai_speech_url[HTTP_URL_LMT+1];
    CHAR_T ai_speech_ip[HTTP_URL_LMT+1];

    CHAR_T time_zone[TIME_ZONE_LEN+1];
    CHAR_T summer_time_zone[SUMMER_TM_ZONE_LEN+1];

    // wechat compatible
    CHAR_T wx_app_id[WXAPPID_LEN+1];
    // wechat compatible
    CHAR_T wx_uuid[WXUUID_LEN+1];
    INT_T cloud_capability;

    BYTE_T dynamic_tls_mode;/*0:invalid 1:disable 2:psk_only 3:ecc_psk 4:ecc_only 6:client auth */
}GW_ACTV_IF_S;

/*
TUYA云端PID具有的能力，bit位
*/
#define CLOUD_CAPABILITY_WIFI         0
#define CLOUD_CAPABILITY_CABLE        1
#define CLOUD_CAPABILITY_GPRS         2
#define CLOUD_CAPABILITY_NBIOT        3
#define CLOUD_CAPABILITY_BLUETOOTH    10
#define CLOUD_CAPABILITY_BLEMESH      11
#define CLOUD_CAPABILITY_ZIGBEE       12
#define CLOUD_CAPABILITY_INFRARED     13
#define CLOUD_CAPABILITY_SUBPIECES    14
#define CLOUD_CAPABILITY_SIGMESH      15
#define CLOUD_CAPABILITY_MCU          16

typedef struct {
    CHAR_T net_key[SIGMESH_NET_KEY_LEN+1];
    CHAR_T app_key[SIGMESH_APP_KEY_LEN+1];
}GW_SIGMESH_IF_S;

typedef BYTE_T DP_PV_STAT_E;
#define INVALID 0 // 数据无效
#define VALID_LC 1 // 本地有效数据
#define VALID_ULING 2 // 数据云端上传中 add by nzy 20150808
#define VALID_CLOUD 3 // 本地有效数据与服务端一致

#if 0
typedef struct {
    USHORT_T cir_time; // circle time unit:second
    USHORT_T cir_rept_item_lmt; // admitting report to cloud items in circle(time == time_lmt)
    USHORT_T cir_rept_items; // report items in one circle
    TIME_S fir_rept_timestamp; // the first report success timestamp.
}DP_REPT_CNTL_S;
#endif

typedef struct {
    UINT_T   period; // long filter period
    UINT_T   limit;  // dpcount limit in long filter period
    UINT_T   rept_cnt; // report counter
    TIME_S   fir_rept_timestamp;//the first report success timestamp
}DP_REPT_FLOW_CTRL_UNIT;

typedef struct { 
    DP_REPT_FLOW_CTRL_UNIT filter_short;//flow control rules for short period
    DP_REPT_FLOW_CTRL_UNIT filter_long;//flow control rules for long period
}DP_REPT_FLOW_CTRL;

#if defined(RELIABLE_TRANSFER) && (RELIABLE_TRANSFER==1)
typedef struct {
    TIME_S cur_set_time;
    UINT_T cur_set_rept_num;
}DP_CUR_SET_REPT_CNTL_S;
#endif

typedef struct {
    DP_DESC_IF_S dp_desc;
    DP_PROP_VALUE_U prop;   /* 存储dp的值 */
    DP_PV_STAT_E pv_stat;  /* 当前缓存状态 */
    //DP_REPT_CNTL_S rept_cntl;
    DP_REPT_FLOW_CTRL rept_flow_ctrl; /* flow control for dp report */
#if defined(RELIABLE_TRANSFER) && (RELIABLE_TRANSFER==1)
    TIME_S rept_new_time;
    DP_PROP_VALUE_U cur_set;//当前下发的值
    DP_CUR_SET_REPT_CNTL_S cur_set_rept;
    BYTE_T is_set_flag;//是否要检查当前下发的值与实际状态是否一样
#endif
}DP_CNTL_S;

typedef BYTE_T DEV_OL_STAT_T;
#define OFFLINE FALSE
#define ONLINE TRUE

typedef BYTE_T DEV_OL_STAT_MAG_T;
#define DOSM_VLD_LC 0
#define DOSM_VLD_ULING 1
#define DOSM_VLD_CLD 2

#define GRP_LMT 64 // group limit
#define SCENE_LMT 64 // scene limit

#define LOG_PHAT_MAX_LEN 64

typedef struct {
    CHAR_T gid[GRP_ID_LEN+1];
}GRP_REC_S;

typedef struct {
    CHAR_T s_id[SCENE_ID_LEN+1];
    CHAR_T g_id[GRP_ID_LEN+1];
}SCENE_REC_S;

typedef struct {
    GRP_REC_S g_rec[GRP_LMT];
    BYTE_T cnt;
}GRP_MAG_S;

typedef struct {
    SCENE_REC_S s_rec[SCENE_LMT];
    BYTE_T cnt;
}SCENE_MAG_S;

// device active attribute
typedef struct {
    BOOL_T attach_dp_if; // is attach dp info
    BOOL_T preprocess; // 指示该设备是否预处理
    BOOL_T wakeup_flag; // need wakeup this subdevice
    DEV_OL_STAT_T stat; // device online status
    DEV_OL_STAT_MAG_T dosm; // device online status manage
    TIME_S last_ol_time; // last online time,using for Heartbeat system trusteeship mode
    TIME_S hb_timeout_time; // if(hb_timeout_time == 0xffffffff) then device online forever
    TIME_S hb_send_time; // heartbeat send time
    UINT_T retry_send_times; //resend times
    UINT_T max_resend_times; // max send times

    TIME_S hb_offline_send_time; // offline send time
    UINT_T retry_onffline_send_times; //offline resend times
#if defined(TUYA_LARGE_SCALE_HEARTBEET) && (TUYA_LARGE_SCALE_HEARTBEET==1)
    TIME_S hb_judge_timeout_time; //last query time
    TIME_S hb_cur_query_time; // cur query
    BOOL_T is_lowpower;
#endif
    
    // grpou info
    //GRP_MAG_S *grp;

    // scene info
    //SCENE_MAG_S *scene;
}DEV_ACTV_ATTR_S;

#define RETRY_BIND_LMT 3
#define RETRY_SYNC_LMT 3
#define RETRY_SIGMESH_LMT 3

typedef struct dev_cntl_n_s {
    struct dev_cntl_n_s *next;
    DEV_DESC_IF_S dev_if;
    DEV_ACTV_ATTR_S attr;
    BYTE_T retry_bind_times;
    BYTE_T retry_sync_times;
    BYTE_T dev_delete;
#if defined(RELIABLE_TRANSFER) && (RELIABLE_TRANSFER==1)
    BYTE_T reliable_transfer_flag;//1表示要可靠性检测
#endif
#if defined(ENABLE_SIGMESH) && (ENABLE_SIGMESH==1)
    BYTE_T retry_sigmesh_snyc_times;
#endif
    BYTE_T dp_num;
    DP_CNTL_S dp[0];
}DEV_CNTL_N_S;

typedef struct {
    // gateway manage
    MUTEX_HANDLE gw_mutex;
    CHAR_T *serial_no; // gw serial num

    GW_WF_CFG_MTHD_SEL mthd;
    BOOL_T enable_station_ap;
    GW_WF_START_MODE wifi_start_mode;

    GW_EXT_STAT_T ext_stat;

    BOOL_T is_oem; // is oem?
    BOOL_T is_init; // has inited?
    BOOL_T is_stride;  //stride pid or key
    /** Is need recovery or not */
    BOOL_T is_need_recovery;

#if TY_SECURITY_CHIP
    GW_AUTH_INFO_S secu_auth_info;
#endif

    GW_BASE_IF_S gw_base;
    GW_WORK_STAT_MAG_S gw_wsm;
    GW_DESC_IF_S gw_if;
    GW_ACTV_IF_S gw_actv;
    #if defined(ENABLE_SIGMESH) && (ENABLE_SIGMESH==1)
    GW_SIGMESH_IF_S gw_sigmesh;
    #endif
    // device manage
    MUTEX_HANDLE dev_mutex;
    UINT_T dev_num;
    DEV_CNTL_N_S *dev;
    map_t dev_maps;
    BOOL_T is_dev_list_synced;

    /****** gw attribute ******/
    // nofity callback
    TY_IOT_CBS_S cbs;
    TY_IOT_GW_CBS_S gw_cbs;
    TY_IOT_APP_CBS_S app_cbs;

    // timer message
    TM_MSG_S *tmm_get_url_cfg;
    #if defined(ENABLE_AP_FAST_CONNECT) && (ENABLE_AP_FAST_CONNECT==1)
    TM_MSG_S *tmm_update_connect_ap_info;
    #endif
    TM_MSG_S *tmm_gw_register;
    TM_MSG_S *tmm_gw_actv;
    TM_MSG_S *tmm_gw_update_info;
	TM_MSG_S *tmm_gw_pskkey_get;
    TM_MSG_S *tmm_gw_if_sync;
    TM_MSG_S *tmm_dev_bind;
    TM_MSG_S *tmm_dev_if_sync;
    #if defined(ENABLE_SIGMESH) && (ENABLE_SIGMESH==1)
    TM_MSG_S *tmm_dev_sigmesh_sync;
    TM_MSG_S *tmm_gw_sigmesh;
    #endif
#if OPERATING_SYSTEM != SYSTEM_LINUX	
	TM_MSG_S *tmm_mf_close;
#endif
    TM_MSG_S *tmm_force_upd_log;
    TM_MSG_S *tmm_rst_log;//firmware rst log
    TM_MSG_S *tmm_gw_ugcfg;
    TM_MSG_S *tmm_get_dev_list;
    TM_MSG_S *tmm_dev_delete;

#if defined(GW_FAULT_REPLACE) && (GW_FAULT_REPLACE==1)
    TM_MSG_S *tmm_get_restore_dev_list;
    TM_MSG_S *tmm_restore_dev_sync;
#endif

#if defined(QRCODE_ACTIVE_MODE) && (QRCODE_ACTIVE_MODE==1)
    TM_MSG_S *tmm_qrcode_active;
#endif
    #if defined(WIFI_GW) && (WIFI_GW==1)
    // use for GWCM_LOW_POWER or GWCM_SPCL_MODE
    TM_MSG_S *tmm_short; // 10s
    TM_MSG_S *tmm_long; // 3*60s
    TM_MSG_S *tmm_gw_monitor; // to monitor whether gw is connect to router lql
    TM_MSG_S *tmm_gw_security_verify; // to monitor whether gw is connect to router lql
    TM_MSG_S *tmm_gw_mod_common_ap; // to modify ap ssid+passwd wym

    // message
    MSG_ID mid_gw_nw_lowpower; // network set low power
    MSG_ID mid_gw_nw_smt_cfg; // network set smartconfig
    MSG_ID mid_gw_nw_cfg_stop; // network stop smartconfig, ap config or bt config
    MSG_ID mid_gw_nw_ap_cfg; // network set ap config
    MSG_ID mid_gw_nw_ap_cfg_stop; // network stop ap config
    MSG_ID mid_wf_reset;
	MSG_ID mid_gw_nw_cfg; //network cfg async routine for bt netcfg, ffs netcfg, etc.
    #endif
    MSG_ID mid_gw_net_conn_set; // network connect set
    MSG_ID mid_gw_actived; // gw actived

    MSG_ID mid_gw_upload_log;
    MSG_ID mid_gw_unregister;
    MSG_ID mid_gw_unactive;
    #if defined(ENABLE_SIGMESH) && (ENABLE_SIGMESH==1)
    MSG_ID mid_dev_ble_mesh_topo_update;
    #endif

#if defined(ENABLE_SUBDEVICE) && (ENABLE_SUBDEVICE==1)
    MSG_ID mid_dev_unregister;
#endif
    // MSG_ID mid_dev_unbind;

    TM_MSG_S *nw_stat_moni_tm;
    #if defined(WIFI_GW) && (WIFI_GW==1)
    GET_WF_NW_STAT_CB get_wf_nw_stat_cb;
	WF_NW_CFG_ERR_CODE_CB wf_nw_err_code_cb;
	NW_CFG_ERR_CODE_E wf_nw_cfg_err_code;
#if defined(GW_SUPPORT_WIRED_WIFI) && (GW_SUPPORT_WIRED_WIFI==1)
    IOT_GW_NET_TYPE_T net_mode;//网络模式
    CUR_NET_STATUS_T current_net;        //0有线，1无线
    GET_NW_STAT_CB get_nw_stat_cb;
#endif
    #else
    GET_NW_STAT_CB get_nw_stat_cb;
    #endif
	/*用于标识共存配网时，用户实际是通过哪种配网类型，执行配网操作的*/
	int concurrent_nw_cfg_type;
    TIME_S data_update_time;//global data update time
}GW_CNTL_S;

typedef BYTE_T GW_RESET_TP_T;
#define GRT_LOCAL 0
#define GRT_REMOTE 1

typedef struct {
    GW_RESET_TP_T gw_rst_tp;
}GW_RESET_S;

typedef struct {
    GW_RESET_TP_T tp;
    CHAR_T dev_id[DEV_ID_LEN+1];
}DEV_RESET_S;

#if defined(ENABLE_ENGINEER_TO_NORMAL) && (ENABLE_ENGINEER_TO_NORMAL==1)
#define ENGINEER_STAT   "engineer_stat"
typedef enum {
    ENGINEER_STAT_MIN = 0,
    ENGINEER_NEED_SYNC,
    ENGINEER_ALREADY_SYNC,
    ENGINEER_ALREADY_UPLOAD,    //upload cloud config
    ENGINEER_STAT_MAX,

} ENGINEER_STAT_E;              //engineer state
#endif

#define IS_STRIDE_PID_KEY  "is_stride"

/***********************************************************
*************************variable define********************
***********************************************************/


/***********************************************************
*************************function define********************
***********************************************************/
/***********************************************************
*  Function: __gw_cntl_init
*  Input: cfg->gw cfg
*         product_key->gw product key
*         sw_ver->gw version
*.........dev_sw_ver->device version
*.........tp->device type (DEV_NM_ATH_SNGL,DEV_NM_NOT_ATH_SNGL)
*         attr->gw attach attribute array when (cfg->abi >= GW_ENTITY_ATTH_DEV) valid
*         attr_num->gw attach attribute number when (cfg->abi >= GW_ENTITY_ATTH_DEV) valid
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
__GW_INTF_EXT \
OPERATE_RET gw_cntl_init(IN CONST GW_CFG_S *cfg,IN CONST CHAR_T *product_key,IN CONST CHAR_T *firmware_key,\
                               IN CONST CHAR_T *sw_ver,IN CONST CHAR_T *dev_sw_ver,\
                               IN CONST DEV_TYPE_T tp,IN CONST GW_ATTACH_ATTR_T *attr,\
                               IN CONST UINT_T attr_num);

#if defined(WIFI_GW) && (WIFI_GW==1)

/***********************************************************
*  Function: gw_wifi_modify_and_conn
*  Input: ssid passwd
*  Output:
*  Return:
*  Note: use at the gateway is active,modify the wifi info
***********************************************************/
__GW_INTF_EXT \
OPERATE_RET gw_wifi_modify_and_conn(IN CONST CHAR_T *ssid,IN CONST CHAR_T *passwd);
#endif

/***********************************************************
*  Function: gw_cloud_connect
*  Input: 
*  Output:
*  Return:
***********************************************************/
VOID gw_cloud_connect(VOID);

/***********************************************************
*  Function: gw_unregister
*  Input: rst_t
*  Output:
*  Return:
***********************************************************/
__GW_INTF_EXT \
OPERATE_RET gw_unregister(IN CONST GW_RESET_S *rst_t);

/***********************************************************
*  Function: gw_unactive
*  Input: rst_t
*  Output:
*  Return:
***********************************************************/
__GW_INTF_EXT \
OPERATE_RET gw_unactive(IN CONST GW_RESET_S *rst_t);

/***********************************************************
*  Function: gw_register_cbs
*  Input: cbs
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
__GW_INTF_EXT \
VOID gw_register_cbs(IN CONST TY_IOT_CBS_S *cbs);

/***********************************************************
*  Function: gw_register_gw_cbs
*  Input: cbs
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
__GW_INTF_EXT \
VOID gw_register_gw_cbs(IN CONST TY_IOT_GW_CBS_S *gw_cbs);

/***********************************************************
*  Function: get_gw_dev_cntl
*  Input:
*  Output:
*  Return: GW_CNTL_S
***********************************************************/
__GW_INTF_EXT \
DEV_CNTL_N_S *get_gw_dev_cntl(VOID);

/***********************************************************
*  Function: get_gw_nw_status
*  Input:
*  Output:
*  Return: GW_NW_STAT_T
***********************************************************/
__GW_INTF_EXT \
GW_NW_STAT_T get_gw_nw_status(VOID);

#if defined(WIFI_GW) && (WIFI_GW==1)
/***********************************************************
*  Function: set_user_def_ap_if
*  Input: ssid passwd
*  Output:
*  Return: OPERATE_RET
*  Note: Must call on the front of the program
***********************************************************/
__GW_INTF_EXT \
OPERATE_RET set_user_def_ap_if(IN CONST CHAR_T *ssid,IN CONST CHAR_T *passwd);
__GW_INTF_EXT \
OPERATE_RET get_dev_ap_if(OUT CHAR_T* ssid, OUT CHAR_T* passwd);

#endif

__GW_INTF_EXT \
VOID set_max_sta_conn(IN CONST UCHAR_T max_conn);
__GW_INTF_EXT \
VOID set_nw_ip(IN CONST CHAR_T* ip, IN CONST CHAR_T* mask, IN CONST CHAR_T* gw);

__GW_INTF_EXT \
VOID gw_register_app_cbs(IN CONST TY_IOT_APP_CBS_S *app_cbs);

/***********************************************************
*  Function: set_gw_ext_stat
*  Input: ext_stat
*  Output:
*  Return:
***********************************************************/
__GW_INTF_EXT \
VOID set_gw_ext_stat(IN CONST GW_EXT_STAT_T ext_stat);

/***********************************************************
*  Function: get_gw_ext_stat
*  Input:
*  Output:
*  Return: GW_EXT_STAT_T
***********************************************************/
__GW_INTF_EXT \
GW_EXT_STAT_T get_gw_ext_stat(VOID);

/***********************************************************
*  Function: gw_lc_bind_device_pk
*  Input:
*  Output:
*  Return:
*  Note: use the prodect key to register
***********************************************************/
__GW_INTF_EXT \
OPERATE_RET gw_lc_bind_device_pk(IN CONST DEV_DESC_IF_S *dev_if,IN CONST BOOL_T attach_dp_if);

/***********************************************************
*  Function: gw_lc_unbind_device
*  Input:
*  Output:
*  Return:
*  Note:
***********************************************************/
__GW_INTF_EXT \
OPERATE_RET gw_lc_unbind_device(IN CONST CHAR_T *id);

/***********************************************************
*  Function: get_dev_cntl_unlock
*  Input:
*  Output:
*  Return:
*  Note:
***********************************************************/
__GW_INTF_EXT \
DEV_CNTL_N_S *get_dev_cntl_unlock(IN CONST CHAR_T *id);

/***********************************************************
*  Function: get_dev_cntl
*  Input:
*  Output:
*  Return:
*  Note:
***********************************************************/
__GW_INTF_EXT \
DEV_CNTL_N_S *get_dev_cntl(IN CONST CHAR_T *id);

/***********************************************************
*  Function: get_dev_dp_cntl
*  Input:dev_cntl
*        dpid
*  Output:
*  Return:
*  Note:
***********************************************************/
__GW_INTF_EXT \
DP_CNTL_S *get_dev_dp_cntl(IN CONST CHAR_T *id,IN CONST BYTE_T dpid);

/***********************************************************
*  Function: get_all_dp_flow_ctrl_rule
*  Input:
*  Output:
*  Return:
*  Note:
***********************************************************/
__GW_INTF_EXT \
DP_REPT_FLOW_CTRL_UNIT* get_all_dp_flow_ctrl_rule(VOID_T);

/***********************************************************
*  Function: get_gw_cntl
*  Input:
*  Output:
*  Return: GW_CNTL_S
***********************************************************/
__GW_INTF_EXT \
GW_CNTL_S *get_gw_cntl(VOID);

__GW_INTF_EXT \
OPERATE_RET gw_update_version(IN GW_PERMIT_DEV_TP_T type, IN CONST CHAR_T *ver);

/***********************************************************
*  Function: gw_user_token_bind
*  Input: token->user token
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
__GW_INTF_EXT \
OPERATE_RET gw_user_token_bind(IN CONST CHAR_T *token);

/***********************************************************
*  Function: gw_get_ip
*  Input: none
*  Output: p_ip
*  Return: OPERATE_RET
***********************************************************/
__GW_INTF_EXT \
OPERATE_RET gw_get_ip(NW_IP_S *p_ip);

/***********************************************************
*  Function: gw_get_mac
*  Input: none
*  Output: p_ip
*  Return: OPERATE_RET
***********************************************************/
__GW_INTF_EXT \
OPERATE_RET gw_get_mac(NW_MAC_S *p_mac);

/***********************************************************
*  Function: gw_get_rssi
*  Input: none
*  Output: p_rssi if(*p_rssi == 99) then do't support get rssi
*  Return: OPERATE_RET
***********************************************************/
__GW_INTF_EXT \
OPERATE_RET gw_get_rssi(SCHAR_T *p_rssi);

#if defined(WIFI_GW) && (WIFI_GW==1)
/***********************************************************
*  Function: get_wf_gw_nw_status
*  Input:
*  Output:nw_stat
*  Return:
***********************************************************/
__GW_INTF_EXT \
OPERATE_RET get_wf_gw_nw_status(OUT GW_WIFI_NW_STAT_E *nw_stat);

/***********************************************************
*  Function: get_wf_gw_nw_cfg_err_code
*  Input:
*  Output:wifi nw err code
*  Return:
***********************************************************/
OPERATE_RET get_wf_gw_nw_cfg_err_code(OUT NW_CFG_ERR_CODE_E *nw_cfg_err_code);
#if defined(GW_SUPPORT_WIRED_WIFI) && (GW_SUPPORT_WIRED_WIFI==1)
__GW_INTF_EXT \
OPERATE_RET get_base_gw_nw_status(OUT GW_BASE_NW_STAT_T *nw_stat);

__GW_INTF_EXT \
VOID get_wired_wifi_gw_net_mode(OUT IOT_GW_NET_TYPE_T *net_mode);

__GW_INTF_EXT \
VOID get_wired_wifi_current_net(OUT CUR_NET_STATUS_T *current_net);

__GW_INTF_EXT \
VOID set_wired_wifi_current_net(IN CUR_NET_STATUS_T current_net);
#endif
#else
/***********************************************************
*  Function: get_base_gw_nw_status
*  Input:
*  Output:
*  Return:
***********************************************************/
__GW_INTF_EXT \
OPERATE_RET get_base_gw_nw_status(OUT GW_BASE_NW_STAT_T *nw_stat);
#endif


#if defined(WIFI_GW) && (WIFI_GW==1)
/***********************************************************
*  Function: gw_wifi_user_cfg
*  Input: ssid passwd token
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
__GW_INTF_EXT \
OPERATE_RET gw_wifi_user_cfg(IN CONST CHAR_T *ssid,IN CONST CHAR_T *passwd,IN CONST CHAR_T *token);
#endif

/***********************************************************
*  Function: gw_bind_device
*  Input: tp uddd id pk ver
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
__GW_INTF_EXT \
OPERATE_RET gw_bind_device(IN CONST GW_PERMIT_DEV_TP_T tp,IN CONST USER_DEV_DTL_DEF_T uddd,IN CONST USER_DEV_DTL_DEF_T uddd2,\
                           IN CONST CHAR_T *id,IN CONST CHAR_T *pk,IN CONST CHAR_T *firmware_key, IN CONST CHAR_T *ver, \
                           IN CONST GW_ATTACH_ATTR_T *attr,IN CONST UINT_T attr_num, IN CONST BOOL_T oem, \
                           IN CONST CHAR_T *mac, IN CONST CHAR_T *uuid, IN CONST CHAR_T *dev_key,IN CONST DEV_SUB_TYPE_T sub_tp,\
                           IN CONST BOOL_T is_support_ota);
/***********************************************************
*  Function: gw_subdevice_update
*  Input: id ver
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
__GW_INTF_EXT \
OPERATE_RET gw_subdevice_update(IN CONST CHAR_T *id,IN CONST CHAR_T *ver, IN CONST GW_ATTACH_ATTR_T *attr, \
                                    IN CONST UINT_T attr_num, IN CONST BOOL_T is_force, IN CONST BOOL_T is_support_ota);

/***********************************************************
*  Function: gw_bind_device_inner
*  Input: dev_cntl schema
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
__GW_INTF_EXT \
OPERATE_RET gw_bind_device_inner(IN CONST DEV_CNTL_N_S *dev_cntl,IN CONST CHAR_T *schema);

/***********************************************************
*  Function: gw_unbind_device_inner
*  Input: dev_cntl
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
__GW_INTF_EXT \
OPERATE_RET gw_unbind_device_inner(IN CONST DEV_CNTL_N_S *dev_cntl);

/***********************************************************
*  Function: gw_unbind_device
*  Input: dev_cntl
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
__GW_INTF_EXT \
OPERATE_RET gw_unbind_device(IN CONST DEV_CNTL_N_S *dev_cntl);

__GW_INTF_EXT \
VOID gw_dev_lock(VOID);

__GW_INTF_EXT \
VOID gw_dev_unlock(VOID);

__GW_INTF_EXT \
OPERATE_RET dev_unregister(IN CONST DEV_RESET_S *rst_t);

/***********************************************************
*  Function: gw_dev_traversal
*  Input: iterator->if(*iterator == NULL) then get the first device
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
__GW_INTF_EXT \
DEV_DESC_IF_S *gw_dev_traversal_unlock(BOOL_T include_gw, INOUT VOID **iterator);

__GW_INTF_EXT \
VOID gw_oem_set(IN CONST BOOL_T oem);

__GW_INTF_EXT \
OPERATE_RET gw_set_udf(IN CONST CHAR_T *udf);

__GW_INTF_EXT \
CHAR_T * gw_get_udf(VOID);

__GW_INTF_EXT \
VOID gw_upload_rst_log_set(IN CONST BOOL_T upload);

__GW_INTF_EXT \
BOOL_T is_gw_oem(VOID);

/**
 * @brief Set extension params for activation
 * 
 * @param[in] param Extension params
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET gw_active_set_ext_param(IN CONST CHAR_T *param);

/**
 * @brief Get extension params for activation
 * 
 * @return NULL on error
 */
CHAR_T * gw_active_get_ext_param(VOID);

__GW_INTF_EXT \
OPERATE_RET dev_update_grp_info(IN CONST GRP_ACTION_E action,IN CONST CHAR_T *dev_id, \
                                      IN CONST CHAR_T *grp_id);

__GW_INTF_EXT \
OPERATE_RET dev_update_scene_info(IN CONST SCE_ACTION_E action,IN CONST CHAR_T *dev_id, \
                                  IN CONST CHAR_T *grp_id, IN CONST CHAR_T *sce_id);

__GW_INTF_EXT \
OPERATE_RET gw_upload_log(CHAR_T *cData, UINT_T iLen);

__GW_INTF_EXT \
OPERATE_RET gw_reset_device(IN CONST DEV_CNTL_N_S *dev_cntl);

__GW_INTF_EXT \
OPERATE_RET gw_update_versions(VOID);

#if defined(WIFI_GW) && (WIFI_GW==1)
__GW_INTF_EXT \
OPERATE_RET gw_fast_unactive(IN CONST GW_WF_CFG_MTHD_SEL mthd,IN CONST GW_WF_START_MODE wifi_start_mode);

__GW_INTF_EXT \
OPERATE_RET gw_fast_get_nc_type(GW_WF_NWC_FAST_STAT_T *nc_type);
typedef enum {
    TY_COUNTRY_CODE_CN,
    TY_COUNTRY_CODE_US,
    TY_COUNTRY_CODE_JP,
    TY_COUNTRY_CODE_EU
} TUYA_COUNTRY_CODE_E;
TUYA_COUNTRY_CODE_E tuya_wifi_get_country_code(VOID_T);
#endif

#if defined(ENABLE_SIGMESH) && (ENABLE_SIGMESH==1)
__GW_INTF_EXT \
OPERATE_RET get_gw_sigmesh_net_info(TY_SIGMESH_NET_INFO_S *sigmesh_net_info);

__GW_INTF_EXT \
OPERATE_RET get_dev_sigmesh_free_node_list(IN CONST INT_T node_num, INOUT TY_SIGMESH_NODE_LIST_S *node_alloc_list);

__GW_INTF_EXT \
OPERATE_RET get_dev_sigmesh_source_node(USHORT_T *source_node);

__GW_INTF_EXT \
BOOL_T gw_is_support_sigmesh(VOID);

__GW_INTF_EXT \
OPERATE_RET get_gw_sigmesh_auth_key(IN CONST CHAR_T *uuid,INOUT TY_SIGMESH_AUTH_KEY *sigmesh_auth_key);

__GW_INTF_EXT \
OPERATE_RET get_gw_sigmesh_bind_status(IN CONST CHAR_T *uuid,IN CONST CHAR_T *encrypted_value,\
                                              INOUT BOOL_T *bind_status);
__GW_INTF_EXT \
OPERATE_RET gw_rept_sig_ble_bind_status(IN CONST CHAR_T *node_id,IN BOOL_T bind_status);

__GW_INTF_EXT \
OPERATE_RET gw_ble_mesh_topo_update(IN CHAR_T *cData,IN UINT_T iLen);
#endif
__GW_INTF_EXT \
VOID tmm_get_dev_token_cb(struct s_tm_msg *tm_msg);

__GW_INTF_EXT \
VOID gw_dev_schema_delete_all(VOID);

__GW_INTF_EXT \
GW_STRIDE_UPDATE_S* gw_get_new_pid_key(VOID_T);

//设置低功耗模式
__GW_INTF_EXT \
VOID tuya_cpu_set_lp_mode(BOOL_T lp_enable);


//获取低功耗模式
__GW_INTF_EXT \
BOOL_T tuya_cpu_get_lp_mode(VOID);


//获取mac地址
__GW_INTF_EXT \
CHAR_T *tuya_get_serialno(VOID);

__GW_INTF_EXT \
BOOL_T gw_is_support_home_security(VOID);

__GW_INTF_EXT \
OPERATE_RET gw_set_support_home_security(BOOL_T is_support);

//获取schema信息，注意：返回的字符串指针需要外部释放
__GW_INTF_EXT \
CHAR_T *tuya_get_schema(VOID);

//获取schema信息，注意：出参dp_info使用后需要外部释放
__GW_INTF_EXT \
OPERATE_RET tuya_get_dp_info(IN CONST CHAR_T *dev_id, INT_T* dp_num, DP_CNTL_S** dp_info);

__GW_INTF_EXT \
OPERATE_RET gw_get_gpd_type(IN CONST UINT_T type, OUT GW_PERMIT_DEV_TP_T *gpd);
#if defined(ENABLE_SUBDEVICE) && (ENABLE_SUBDEVICE==1)
/**
 * @brief Set the maximum count of sub-devices
 * @param cnt : Maximum count of user allowed
 * @return TRUE is success
 */
__GW_INTF_EXT \
BOOL_T gw_set_subdev_max_cnt(UINT_T cnt);

__GW_INTF_EXT \
UINT_T gw_get_subdev_max_cnt(VOID);

/**
 * @brief Get the count of sub-devices
 * @return UINT_T, The count of sub-devices.
 *         If upgrad from an unrestricted version, the current count may exceed the limit.
 */
__GW_INTF_EXT \
UINT_T gw_get_subdev_cnt(VOID);

/**
 * @brief Check current count of sub-devices
 * @return TRUE is over max
 */
__GW_INTF_EXT \
BOOL_T gw_is_subdev_cnt_over_max(VOID);
#endif

#if defined(GW_FAULT_REPLACE) && (GW_FAULT_REPLACE==1)
__GW_INTF_EXT \
OPERATE_RET gw_restore_dev_list(VOID);
#endif

#ifdef __cplusplus
}
#endif
#endif

