#ifndef TUYA_BLE_API_H__
#define TUYA_BLE_API_H__
#include "tuya_ble_type.h"
#include "tuya_hal_bt.h"
#include "tuya_cloud_types.h"
#include "tuya_ble_config.h"

#ifdef __cplusplus
extern "C" {
#endif
#if defined(TY_BT_MOD) && (TY_BT_MOD==1)

#define FRM_QRY_DEV_INFO_REQ                0x0000  //APP->BLE
#define FRM_QRY_DEV_INFO_RESP               0x0000  //BLE->APP
#define PAIR_REQ                            0x0001  //APP->BLE
#define PAIR_RESP                           0x0001  //BLE->APP
#define FRM_CMD_SEND                        0x0002  //APP->BLE
#define FRM_CMD_RESP                        0x0002  //BLE->APP
#define FRM_STATE_QUERY                     0x0003  //APP->BLE
#define FRM_STATE_QUERY_RESP                0x0003  //BLE->APP
#define FRM_LOGIN_KEY_REQ                   0x0004  //APP->BLE
#define FRM_LOGIN_KEY_RESP                  0x0004  //BLE->APP
#define FRM_UNBONDING_REQ                   0x0005  //APP->BLE
#define FRM_UNBONDING_RESP                  0x0005  //BLE->APP
#define FRM_DEVICE_RESET                    0x0006  //APP->BLE
#define FRM_DEVICE_RESET_RESP               0x0006  //BLE->APP

#define FRM_FACTORY_TEST                    0x0012 //APP->BLE

#define FRM_ANOMALY_UNBONDING_REQ           0x0014 //APP->BLE
#define FRM_ANOMALY_UNBONDING_RESP          0x0014 //BLE->APP

#define FRM_RPT_DEV_WF_NW_STAT_REQ          0x0001E  //BLE->APP
#define FRM_RPT_DEV_WF_NW_STAT_RESP         0x0001E  //APP->BLE

#define FRM_NET_CONFIG_INFO_REQ             0x0021 //APP->BLE
#define FRM_NET_CONFIG_INFO_RESP            0x0021 //BLE->APP

#define FRM_NET_CONFIG_RESPONSE_REPORT_REQ  0x0022 //BLE->APP
#define FRM_NET_CONFIG_RESPONSE_REPORT_RESP 0x0022 //APP->BLE

#define FRM_DATA_PASSTHROUGH_REQ            0x0023 //APP<->BLE

#define FRM_DATA_WAKEUP_REQ                 0x0024 // APP<->BLE


#define FRM_STAT_REPORT                     0x8001  //BLE->APP
#define FRM_STAT_REPORT_RESP                0x8001  //APP->BLE

#define FRM_STAT_WITH_TIME_REPORT           0x8003  //BLE->APP
#define FRM_STAT_WITH_TIME_RESP             0x8003  //APP->BLE

#define FRM_GET_UNIX_TIME_CHAR_MS_REQ      0x8011  //BLE->APP
#define FRM_GET_UNIX_TIME_CHAR_MS_RESP     0x8011  //APP->BLE

typedef enum{
    TUYA_BT_UNBONDING_UNCONN = 0,
    TUYA_BT_UNBONDING_CONN,
    TUYA_BT_BONDING_UNCONN,
    TUYA_BT_BONDING_CONN,
    TUYA_BT_UNKNOW_STATUS
}TUYA_BT_CONN_STAT;

/***********************************************************
*  callback function: GET_BT_CONN_STAT_CB
*  Desc:    tuya-sdk BT check callback (wifi version)
*  Input:   stat: curr BT status
***********************************************************/
typedef VOID (*GET_BT_CONN_STAT_CB)(IN CONST TUYA_BT_CONN_STAT stat);

/*
** 1. 在回调函数中,不要阻塞
** 2. 如需要使用数据,需要重新分配空间
** 3. 回调函数完成后,无需释放参数user_data相关的内存
*/
typedef VOID (*TUYA_BLE_DATA_CB)(const tuya_ble_user_data_t *user_data); 


typedef enum{
    TUYA_BLE_PRODUCT_ID_TYPE_PID,
    TUYA_BLE_PRODUCT_ID_TYPE_PRODUCT_KEY,
}tuya_ble_product_id_type_t;

typedef struct{
    TUYA_BLE_DATA_CB cb;
    uint8_t device_name[DEVICE_NAME_LEN];

    tuya_ble_product_id_type_t p_type;
    uint8_t product_id_len;
    uint8_t product_id[TUYA_BLE_PRODUCT_ID_MAX_LEN];

    uint8_t uuid[DEVICE_ID_LEN];
    uint8_t auth_key[AUTH_KEY_LEN];
    uint8_t uuid_actual_len;

    uint8_t login_key[LOGIN_KEY_LEN];
    uint8_t bound_flag;
}TUYA_BLE_PARAM_S;

OPERATE_RET tuya_ble_sdk_init(TUYA_BLE_PARAM_S * param_data);

OPERATE_RET tuya_ble_sdk_reinit(VOID);


OPERATE_RET tuya_ble_device_update_param(tuya_ble_product_id_type_t type, UINT_T len, CHAR_T* product_id,
                                         CHAR_T *p_uuid, CHAR_T *p_authkey);

OPERATE_RET tuya_ble_device_update_login_key(uint8_t* p_buf, uint8_t len);

OPERATE_RET tuya_ble_device_update_bound_state(uint8_t state);

uint8_t tuya_ble_device_get_bound_state(VOID);

uint8_t *tuya_ble_device_get_login_key(VOID);
uint8_t *tuya_ble_device_get_auth_key(VOID);
uint8_t *tuya_ble_device_get_uuid(VOID);

void tuya_ble_device_id_20_to_16(uint8_t *in,uint8_t *out);
void tuya_ble_device_id_16_to_20(uint8_t *in,uint8_t *out);

#if defined(BT_REMOTE_CTRL) && (BT_REMOTE_CTRL==1)
//sdk内部接口
VOID_T tuya_ble_init_mesh(VOID_T);
OPERATE_RET tuya_ble_save_mesh(VOID_T);
VOID_T tuya_ble_clear_mesh(VOID_T);
VOID_T tuya_ble_scan_adv_proc(CHAR_T *data, UINT_T len);


//给应用提供的接口：蓝牙广播扫描数据注册
typedef VOID (*TUYA_BLE_APP_SCAN_HANDLE)(UCHAR_T *data, UCHAR_T len);
OPERATE_RET tuya_ble_reg_app_scan_adv_cb(TUYA_BLE_APP_SCAN_HANDLE cb);
#endif

VOID_T tuya_ble_connect_monitor_timer_stop(VOID_T);
VOID_T tuya_ble_raw_print(CHAR_T *title, UINT8_T width, UINT8_T *buf, UINT16_T size);
OPERATE_RET tuya_ble_monitor_init(VOID);

VOID tuya_bt_set_connect_status(TUYA_BT_CONN_STAT status);
TUYA_BT_CONN_STAT tuya_bt_get_connect_status(VOID);
OPERATE_RET tuya_ble_reg_get_bt_stat_cb(IN CONST GET_BT_CONN_STAT_CB bt_conn_stat_cb);

//蓝牙配对后，设置设备联网状态标志，定时器中发送状态给APP
VOID_T tuya_set_dev_nw_stat(VOID_T);

#endif

#ifdef __cplusplus
}
#endif

#endif

