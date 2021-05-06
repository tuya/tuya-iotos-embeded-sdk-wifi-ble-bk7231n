/**
 ****************************************************************************************
 *
 * @file app_task.h
 *
 * @brief Header file - APPTASK.
 *
 * Copyright (C) RivieraWaves 2009-2015
 *
 *
 ****************************************************************************************
 */

#ifndef APP_TASK_H_
#define APP_TASK_H_

/**
 ****************************************************************************************
 * @addtogroup APPTASK Task
 * @ingroup APP
 * @brief Routes ALL messages to/from APP block.
 *
 * The APPTASK is the block responsible for bridging the final application with the
 * RWBLE software host stack. It communicates with the different modules of the BLE host,
 * i.e. @ref SMP, @ref GAP and @ref GATT.
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"             // SW configuration

#if (BLE_APP_PRESENT)

#include <stdint.h>         	// Standard Integer
#include "rwip.h"
#include "rwip_task.h"      	// Task definitions
#include "kernel_task.h"        	// Kernel Task
#include "common_bt.h"


/*
 * DEFINES
 ****************************************************************************************
 */

/// Number of APP Task Instances
#define APP_IDX_MAX                 (1)
#define MAX_SCAN_NUM                (15)

/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/// States of APP task
enum appm_state
{
    /// Initialization state
    APPM_INIT,
    /// Database create state
    APPM_CREATE_DB,
    /// Ready State
    APPM_READY,
    /// Advertising state
    APPM_ADVERTISING,
    /// Scan state
    APPM_SCANNING,

    APPM_WAIT_SCANNEND,
    /// Connecting state
    APPM_CONNECTING,
    /// Link Connected state
    APPM_LINK_CONNECTED,

    APPM_SDP_DISCOVERING,

    APPM_CONNECTED,//Prf Connected

    APPM_DISCONNECT,

    /// Number of defined states.
    APPM_STATE_MAX
};


/// APP Task messages
enum appm_msg
{
     APPM_DUMMY_MSG = TASK_FIRST_MSG(TASK_ID_APP),
	
   	 /// Timer used to automatically stop advertising
     APP_ADV_TIMEOUT_TIMER,
	
    #if (BLE_APP_HT)
    /// Timer used to refresh the temperature measurement value
    APP_HT_MEAS_INTV_TIMER,
    #endif //(BLE_APP_HT)
	
    #if (BLE_APP_HID)
    /// Timer used to disconnect the moue if no activity is detecter
    APP_HID_MOUSE_TIMEOUT_TIMER,
    #endif //(BLE_APP_HID)
	
	APP_SEND_SECURITY_REQ,

	APP_GATTC_EXC_MTU_CMD,
	
    APP_PARAM_UPDATE_REQ_IND,

	APP_PERIOD_TIMER,
		
	APP_ENCODE_IND,
	
	APP_ADV_ENABLE_TIMER,

	APP_DISCONNECT_TIMER,

	APP_PERIOD_TEST_TIMER,  //data test
};


struct sbc_encode
{
	uint32_t len;
	int16_t buf[__ARRAY_EMPTY];
};


typedef struct
{
	uint8_t number;
	uint8_t addr_type;
	uint8_t rssi;
	uint8_t adv_type;
	struct bd_addr adv_addr;
}device_info_t;

typedef struct
{
	device_info_t info[MAX_SCAN_NUM];
}ble_scan_list_t;

/*
 * GLOBAL VARIABLE DECLARATIONS
 ****************************************************************************************
 */

extern const struct kernel_state_handler appm_default_handler;
extern kernel_state_t appm_state[APP_IDX_MAX];

extern uint8_t send_buf[20];
extern uint8_t adv_count;
extern ble_scan_list_t ble_scan_list;

/// @} APPTASK

#endif //(BLE_APP_PRESENT)

#endif // APP_TASK_H_
