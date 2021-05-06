#ifndef _APP_BLE_TASK_H_
#define _APP_BLE_TASK_H_

#include "rwip_config.h"     // SW configuration
#if (BLE_APP_PRESENT)

#include "app_ble.h"
#include "ble_ui.h"
#include "app_comm.h"                //  Application Module Definitions
#include "ble_api.h"

/**
 ****************************************************************************************
 * @brief Start/stop advertising
 *
 * @param[in] start     True if advertising has to be started, else false
 ****************************************************************************************
 */
void appm_update_adv_state(char start);

/**
 ****************************************************************************************
 * @brief Send a disconnection request
 ****************************************************************************************
 */
void appm_disconnect(unsigned char reason);

extern void appm_build_adv_data(uint16_t max_length, uint16_t *p_length, uint8_t *p_buf);
extern int appm_update_adv_data( uint8_t* adv_buff, uint8_t adv_len, uint8_t* scan_buff, uint8_t scan_len);
extern ble_err_t appm_stop_advertising(void);
extern ble_err_t appm_start_advertising(void);

extern void appm_scan_fsm_next(void);

#endif
#endif
