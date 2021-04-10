#ifndef _TUYA_BT_SDK_H
#define _TUYA_BT_SDK_H

#include "tuya_cloud_types.h"
#include "tuya_cloud_com_defs.h"
//#include "tuya_hal_bt.h"
#include "tuya_ble_type.h"
#include "tuya_ble_api.h"
#include "tuya_ble_mutli_tsf_protocol.h"


#ifdef __cplusplus
extern "C" {
#endif

#if defined(TY_BT_MOD) && (TY_BT_MOD==1)

typedef struct {
    CHAR_T *uuid;
    CHAR_T *auth_key;
}TY_BT_SDK_PARAM_S;

OPERATE_RET ty_bt_init_env(IN CONST TY_BT_SDK_PARAM_S *p);
OPERATE_RET ty_bt_update_param(CHAR_T *product_id, BOOL_T is_oem, TY_BT_SDK_PARAM_S *p_param);
OPERATE_RET ty_bt_update_local_key(uint8_t* p_buf, uint8_t len);

OPERATE_RET tuya_ble_sdk_send(const tuya_ble_user_data_t *user_data);

//OPERATE_RET ty_bt_ubound_response(uint8_t result_code);
//OPERATE_RET ty_bt_anomaly_ubound_response(uint8_t result_code);
//OPERATE_RET ty_bt_device_reset_response(uint8_t result_code);

/*
在ble上引入session层，用于把业务和数据传输拆分开：
	- 数据传输：
		ble sdk用于数据传输功能，不关心数据的业务属性，只完成数据打包/解包，数据传输，状态控制，以及和ble相关的业务。
	- 业务开发：
		比如配网，dp收发都基于session来完成，每个业务方通过创建的session来调用ble传输功能
*/



typedef void (*FN_HANDLE_RECEIVE_DATA)(tuya_ble_user_data_t *user_data);

typedef struct{

	FN_HANDLE_RECEIVE_DATA fn_data_cb;
	
	uint8_t (*get_bound_state)(void);
	int (*set_bound_state)(uint8_t bound_state);
	
}BT_TRANSPORT_SESSION_HANDLE, *P_BT_TRANSPORT_SESSION_HANDLE;


int bt_transport_session_create(OUT P_BT_TRANSPORT_SESSION_HANDLE* p_session_out, IN FN_HANDLE_RECEIVE_DATA fn_data_cb);

int bt_transport_session_destroy(IN BT_TRANSPORT_SESSION_HANDLE* p_session_in);

#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  /*_TUYA_BT_SDK_H*/

