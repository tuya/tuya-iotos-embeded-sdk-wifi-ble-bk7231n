#ifndef _BLE_UI_H_
#define _BLE_UI_H_

/// Maximal length of the Device Name value
#define APP_DEVICE_NAME_MAX_LEN      (18)

#include "rwip_config.h"             // SW configuration
#include <stdint.h>          // Standard Integer Definition
#include "gapc_task.h"       // GAPC Definitions
#include "common_bt.h"
#include <string.h>
#include "kernel_task.h"        // Kernel Task
#include "kernel_msg.h"
#include "gap.h"                     // GAP Definition
#include "gapm_task.h"               // GAP Manager Task API
#include "gapc_task.h"               // GAP Controller Task API
#include "common_bt.h"                   // Common BT Definition
#include "common_math.h"                 // Common Maths Definition
#include "rwip.h"            // RW definitions
#include "architect.h"            // Platform Definitions
#include "app_task.h"                // Application task Definition
#include "gattc_int.h"
#include "ble_api.h"
#include "gapm_task.h"
#include "co_math.h"

extern void ble_set_event_cb(ble_event_cb_t func);
extern void ble_set_write_cb(bk_ble_write_cb_t func);
extern void ble_set_read_cb(bk_ble_read_cb_t func);
extern void ble_set_recv_adv_cb(ble_recv_adv_cb_t func);


extern void ble_event_cb_handler(ble_event_t event, void *param);
extern void ble_write_cb_handler(write_req_t *write_req);
extern int ble_read_cb_handler(read_req_t *read_req);
extern void ble_recv_adv_cb_handler(recv_adv_t *recv_adv);

extern void ble_appm_create_advertising(unsigned char chnl_map,uint32_t intv_min,uint32_t intv_max);
extern void ble_appm_start_advertising(unsigned char adv_actv_idx,uint16 duration);
extern void ble_appm_stop_advertising(unsigned char adv_actv_idx);
extern void ble_appm_delete_advertising(unsigned char adv_actv_idx);


extern int ble_appm_update_adv_data(unsigned char adv_actv_idx,unsigned char* adv_buff, unsigned char adv_len);
extern int ble_appm_set_scan_rsp_data(unsigned char adv_actv_idx,unsigned char* scan_buff, unsigned char scan_len);


extern int ble_app_gapm_adv_check_data_sanity(uint16_t length, uint8_t *p_data);

extern void ble_appm_update_param(unsigned char conidx,struct gapc_conn_param *conn_param);

///extern int ble_set_scan_param( uint16_t scan_intv, uint16_t scan_wd);


extern void ble_appm_send_gapm_reset_cmd(void);

//////////////scan
extern void ble_appm_create_scaning(void);


//////////////
extern void ble_appc_gatt_mtu_change(unsigned char conidx);



///////////////////////
extern unsigned char ble_appm_get_dev_name(unsigned char* name);
extern unsigned char ble_appm_set_dev_name(unsigned char len,unsigned char* name);



#endif
