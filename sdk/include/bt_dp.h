#ifndef __BT_DP__
#define __BT_DP__

#include "tuya_bt_sdk.h"
#include "tuya_cloud_types.h"
#include "tuya_cloud_com_defs.h"
#include "gw_intf.h"
#include "tuya_ble_config.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(TY_BT_MOD) && (TY_BT_MOD==1)

/*
	dp上报接口
*/
OPERATE_RET ty_bt_dp_data_report(IN CONST TY_OBJ_DP_S *dp_data,IN CONST UINT_T cnt);

OPERATE_RET ty_bt_raw_dp_report(IN CONST BYTE_T dpid, IN CONST BYTE_T *data,IN CONST UINT_T len);

OPERATE_RET ty_bt_klv_report(klv_node_s *p_node);

OPERATE_RET ty_bt_passthrough_data_report(uint8_t *p_data,uint32_t len);

OPERATE_RET ty_bt_network_status_report(int16_t result_code);
/*
	ble接收数据处理接口
*/

int ty_bt_start_receive_data_from_app();

int ty_bt_stop_receive_data_from_app();

/* 注册gw接口到bt 数据处理组件*/
typedef OPERATE_RET (*fn_gw_action_cb)(IN CONST GW_RESET_S *rst_t);

void ty_bt_register_gw_cb(fn_gw_action_cb unactive, fn_gw_action_cb unregister);

#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
