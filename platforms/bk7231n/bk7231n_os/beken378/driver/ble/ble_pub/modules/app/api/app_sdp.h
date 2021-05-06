/**
 ****************************************************************************************
 *
 * @file app_sdp.h
 *
 * @brief Application entry point
 *
 * Copyright (C) Beken 2009-2016
 *
 *
 ****************************************************************************************
 */
#ifndef APP_SDP_H_
#define APP_SDP_H_


#include "rwip_config.h"     // SW configuration
#if (BLE_APP_SDP)
#include <stdint.h>          // Standard Integer Definition
#include <common_bt.h>           // Common BT Definitions
#include "arch.h"            // Platform Definitions
#include "gap.h"
#include "gapc.h"            // GAPC Definitions
#include "gattc_task.h"
#include <stdbool.h>
#include "kernel_task.h"
#include "common_error.h"
#include "attm.h"
#include "gattc_task.h"
#include "prf_utils.h"
#include "kernel_mem.h"
#include "sdp_service.h"
#include "ble.h"
#include "ble_pub.h"
#include "ble_api.h"



void appm_set_max_scan_nums(uint8_t max);
uint8_t appm_get_max_scan_nums(void);
uint8_t appm_start_scanning(void);
uint8_t appm_stop_scanning(void);
uint8_t appm_start_connenct_by_addr(uint8_t* bdaddr);
uint8_t appm_start_connenct_by_id(uint8_t id);
uint8_t appm_stop_connencting(void);
uint8_t appm_disconnect_link(void);
uint8_t appm_adv_data_decode(uint8_t len,const uint8_t *data,uint8_t *find_str,uint8_t str_len);
uint8_t appm_write_data_by_uuid(uint16_t uuid,uint8_t len,uint8_t *data);
uint8_t appm_write_ntf_cfg(uint16_t uuid,uint16_t number,uint16_t cfg);
uint8_t appm_read_data_by_uuid(uint16_t uuid);
uint8_t appm_read_cfg_by_uuid(uint16_t uuid);
uint8_t appm_read_user_desc_by_uuid(uint16_t uuid);
uint8_t  sdp_enable_all_server_ntf_ind(uint8_t  reset);
void sdp_prf_register_all_atthdl2gatt(void);

#endif
#endif
