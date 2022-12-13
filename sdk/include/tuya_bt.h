/**
* @file tuya_bt.h
* @brief Common process - Initialization
* @version 0.1
* @date 2020-11-09
*
* @copyright Copyright 2020-2021 Tuya Inc. All Rights Reserved.
*
*/

#ifndef TUYA_BT_H__
#define TUYA_BT_H__
#include "tuya_cloud_types.h"
#include "tuya_cloud_com_defs.h"

#ifdef __cplusplus
extern "C" {
#endif
#if defined(TY_BT_MOD) && (TY_BT_MOD==1)

/**
 * @brief connection status for bluetooth
 */
typedef enum{
    TUYA_BT_UNBONDING_UNCONN = 0,
    TUYA_BT_UNBONDING_CONN,
    TUYA_BT_BONDING_UNCONN,
    TUYA_BT_BONDING_CONN,
    TUYA_BT_UNKNOW_STATUS
}TUYA_BT_CONN_STAT;

/**
 * @brief bind type for bluetooth remote controller
 */
typedef enum{
    TUYA_BLE_OP_UNBIND = 0,
    TUYA_BLE_OP_BIND,
}TUYA_BLE_BIND_TYPE;


/**
 * @brief parameters for bluetooth
 */
typedef struct {
    CHAR_T *uuid;    //uuid
    CHAR_T *auth_key;//authentication key
}TY_BT_SDK_PARAM_S;

//callback function for user dp query
typedef VOID_T (*BT_DP_QUERY_CB)(VOID_T);
typedef UINT_T (*BT_DP_GET_TM_CB)(CHAR_T*       time_str);

typedef struct {
    UINT8_T priority_flag;      //0:wifi first  1:bt first
    UINT8_T share_flag;         //0:no shared device  1:tuya shared device
    UINT8_T dp_rept_with_time;  //0:without time  1:with time
    UINT8_T rsvd;
    BT_DP_QUERY_CB query_dp_cb; //callback function for dp query
    BT_DP_GET_TM_CB get_timestamp_cb;
}TUYA_BLE_BT_PARA;

/**
 * @brief Init the running environment for bluetooth.
 *
 * @param[in] p: initial parameters for bluetooth
 *
 * @note This API is used for initializing the running environment for bluetooth.
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET ty_bt_init_env(IN CONST TY_BT_SDK_PARAM_S *p);

/**
 * @brief Update parameters for bluetooth.
 *
 * @param[in] product_id: product id
 * @param[in] is_oem: if be oem or not
 * @param[in] p_param: parameters
 *
 * @note This API is used for updating parameters for bluetooth.
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET ty_bt_update_param(CHAR_T *product_id, BOOL_T is_oem, TY_BT_SDK_PARAM_S *p_param);

/**
 * @brief Update local key
 *
 * @param[in] p_buf: local key
 * @param[in] len: lenth of local key
 *
 * @note This API is used for update local key.
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET ty_bt_update_local_key(UINT8_T* p_buf, UINT8_T len);

/**
 * @brief Update broadcast param for bluetooth
 *
 * @param[in] p_buf: local key
 * @param[in] bind_state: bind state
 *
 * @note This API is used for update broadcast param for bluetooth.
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_ble_device_update_adv_para(uint8_t* p_buf, uint8_t bind_state);

/**
 * @brief Init network configuration by bluetooth
 *
 * @param VOID
 *
 * @note This API is used for initializing network configuration by bluetooth
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET bt_netcfg_init(VOID_T);

/**
 * @brief Report dp by bluetooth
 *
 * @param[in] dp_rept_type: dp report type
 * @param[in] data: dp data
 *
 * @note This API is used for reporting dp data
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET ty_bt_dp_report(IN CONST DP_REPT_TYPE_E dp_rept_type, IN CONST VOID_T* data);

/**
 * @brief Monitor for bluetooth
 *
 * @param VOID
 *
 * @note This API is used for monitor for bluetooth.
 * if the device connected cloud, start advertisement, otherwise stoo it.
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_ble_monitor_init(VOID_T);

/**
 * @brief Check bluetooth is connected.
 *
 * @param VOID
 *
 * @note This API is used to check whether the bluetooth is connected.
 *
 * @return be connected or not
 */
BOOL_T is_bt_connected(VOID_T);

//callback function for getting the connection status of bluetooth
typedef VOID (*GET_BT_CONN_STAT_CB)(IN CONST TUYA_BT_CONN_STAT stat);

/**
 * @brief Register callback function for getting the connection status of bluetooth
 *
 * @param[in] bt_conn_stat_cb: callback function
 *
 * @note This API is used for registering callback function for getting the connection status of bluetooth
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_ble_reg_get_bt_stat_cb(IN CONST GET_BT_CONN_STAT_CB bt_conn_stat_cb);

/**
 * @brief Get connection status of bluetooth
 *
 * @param VOID
 *
 * @note This API is used for getting the connection status of bluetooth
 *
 * @return connection status
 */
TUYA_BT_CONN_STAT tuya_bt_get_connect_status(VOID);

/**
 * @brief Set device name for bluetooth.
 *
 * @param[in] dev_name: device name for bluetooth
 *
 * @note This API is used for setting device name for bluetooth. this API should be called before bluetooth initialized.
 *
 * @return VOID
 */
OPERATE_RET tuya_set_bt_device_name(CHAR_T* dev_name);

/**
 * @brief Set parameters for bluetooth.
 *
 * @param[in] para: parameters for bluetooth
 *
 * @note This API is used for setting parameters for bluetooth. this API should be called before bluetooth initialized.
 *
 * @return VOID
 */
VOID_T tuya_ble_set_bt_para(TUYA_BLE_BT_PARA* para);

#if defined(BT_REMOTE_CTRL) && (BT_REMOTE_CTRL==1)

/**
 * @brief Init beacon key for bluetooth remote control.
 *
 * @param VOID
 *
 * @note This API is used for initialize beacon key for bluetooth remote control.
 * This API is userd for new remote control.
 *
 * @return VOID
 */
VOID_T tuya_ble_init_beacon_key(VOID_T);

/**
 * @brief Save beacon key for bluetooth remote control.
 *
 * @param VOID
 *
 * @note This API is used for saving beacon key for bluetooth remote control.
 * This API is userd for new remote control.
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_ble_save_beacon_key(VOID_T);

/**
 * @brief Clear up beacon key for bluetooth remote control.
 *
 * @param VOID
 *
 * @note This API is used for clearing up beacon key for bluetooth remote control.
 * This API is userd for new remote control.
 *
 * @return VOID
 */
VOID_T tuya_ble_clear_beacon_key(VOID_T);

/**
 * @brief Clear up all flash information of bluetooth remote control.
 *
 * @param VOID
 *
 * @note This API is used for all flash information of bluetooth remote control.
 * This API is userd for new remote control.
 *
 * @return VOID
 */
VOID_T tuya_ble_clear_rmt_ctrl_info(UINT8_T stat);


//callback function for advertisement scanning data processing
typedef VOID (*TUYA_BLE_APP_SCAN_HANDLE)(UCHAR_T *data, UCHAR_T len,  UCHAR_T type, UCHAR_T* mac);

/**
 * @brief Register callback function for advertisement scanning data processing
 *
 * @param[in] cb: callback function
 *
 * @note This API is used for registering callback function for advertisement scanning data processing
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_ble_reg_app_scan_adv_cb(TUYA_BLE_APP_SCAN_HANDLE cb);

//callback function for validity check for bind or unbind operation
typedef OPERATE_RET (*BLE_SCAN_ADV_BIND_CHECK_CB)(TUYA_BLE_BIND_TYPE type, UCHAR_T *data, UCHAR_T len);
//callback function for result notify for bind or unbind operation
typedef VOID_T (*BLE_SCAN_ADV_BIND_RSLT_NOTIFY_CB)(TUYA_BLE_BIND_TYPE type, int rslt);

typedef struct {
    BLE_SCAN_ADV_BIND_CHECK_CB       bind_check;  
    BLE_SCAN_ADV_BIND_RSLT_NOTIFY_CB bind_notify;
}TUYA_BLE_SCAN_ADV_HANDLE_CBS;

/**
 * @brief Register callback function for advertisement scanning data processing
 *
 * @param[in] cbs: callback function
 *
 * @note This API is used for registering callback function for advertisement scanning data processing
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_ble_reg_app_scan_adv_handle_cbs(TUYA_BLE_SCAN_ADV_HANDLE_CBS* cbs);

/**
 * @brief Set timeout window for binding remote controller
 *
 * @param[in] time_out: time out in second, 0: no window
 *
 * @note This API is used for setting timeout window for binding remote controller
 *
 * @return VOID
 */
VOID_T tuya_ble_set_bind_window(UINT_T time_out);

/**
 * @brief Open window for binding remote controller
 *
 * @param[in] VOID
 *
 * @note This API is used for opening window for binding remote controller
 *
 * @return VOID
 */
VOID_T tuya_ble_open_bind_window(VOID_T);
#endif

/**
 * @brief Set enable switch for bluetooth service
 *
 * @param[in] switch: enable switch: TRUE-open, FALSE-close
 *
 * @note This API is used for setting enable switch for bluetooth service, it should be called before sdk initialization.
 *
 * @return VOID
 */
VOID_T tuya_ble_set_serv_switch(BOOL_T swith);

/**
 * @brief Check bluetooth service is available
 *
 * @param VOID
 *
 * @note This API is used to check the bluetooth service is available.
 *
 * @return TRUE enable, FALSE disable
 */
BOOL_T tuya_ble_is_serv_opened(VOID_T);

#endif

#ifdef __cplusplus
}
#endif

#endif

