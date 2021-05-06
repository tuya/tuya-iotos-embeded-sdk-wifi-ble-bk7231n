/**
 ****************************************************************************************
 *
 * @file app.h
 *
 * @brief Application entry point
 *
 * Copyright (C) RivieraWaves 2009-2015
 *
 *
 ****************************************************************************************
 */

#ifndef _APP_BLE_H_
#define _APP_BLE_H_

/**
 ****************************************************************************************
 * @addtogroup APP
 * @ingroup RICOW
 *
 * @brief Application entry point.
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"     // SW configuration

#if (BLE_APP_PRESENT)

#include <stdint.h>          // Standard Integer Definition
#include <common_bt.h>           // Common BT Definitions
#include "architect.h"            // Platform Definitions
#include "gapc_task.h"       // GAPC Definitions
#include "ble_ui.h"

#if (NVDS_SUPPORT)
#include "nvds.h"
#endif // (NVDS_SUPPORT)

/*
 * DEFINES
 ****************************************************************************************
 */



/*
 * MACROS
 ****************************************************************************************
 */

#define APP_HANDLERS(subtask)    {&subtask##_msg_handler_list[0], ARRAY_LEN(subtask##_msg_handler_list)}

/*
 * ENUMERATIONS
 ****************************************************************************************
 */

#if (NVDS_SUPPORT)
/// List of Application NVDS TAG identifiers
enum app_nvds_tag
{
    /// BD Address
    NVDS_TAG_BD_ADDRESS                 = 0x01,
    NVDS_LEN_BD_ADDRESS                 = 6,

    /// Device Name
    NVDS_TAG_DEVICE_NAME                = 0x02,
    NVDS_LEN_DEVICE_NAME                = 62,

    /// BLE Application Advertising data
    NVDS_TAG_APP_BLE_ADV_DATA           = 0x0B,
    NVDS_LEN_APP_BLE_ADV_DATA           = 32,

    /// BLE Application Scan response data
    NVDS_TAG_APP_BLE_SCAN_RESP_DATA     = 0x0C,
    NVDS_LEN_APP_BLE_SCAN_RESP_DATA     = 32,

    /// Mouse Sample Rate
    NVDS_TAG_MOUSE_SAMPLE_RATE          = 0x38,
    NVDS_LEN_MOUSE_SAMPLE_RATE          = 1,

    /// Peripheral Bonded
    NVDS_TAG_PERIPH_BONDED              = 0x39,
    NVDS_LEN_PERIPH_BONDED              = 1,

    /// Mouse NTF Cfg
    NVDS_TAG_MOUSE_NTF_CFG              = 0x3A,
    NVDS_LEN_MOUSE_NTF_CFG              = 2,

    /// Mouse Timeout value
    NVDS_TAG_MOUSE_TIMEOUT              = 0x3B,
    NVDS_LEN_MOUSE_TIMEOUT              = 2,

    /// Peer Device BD Address
    NVDS_TAG_PEER_BD_ADDRESS            = 0x3C,
    NVDS_LEN_PEER_BD_ADDRESS            = 7,

    /// Mouse Energy Safe
    NVDS_TAG_MOUSE_ENERGY_SAFE          = 0x3D,
    NVDS_LEN_MOUSE_SAFE_ENERGY          = 2,

    /// EDIV (2bytes), RAND NB (8bytes),  LTK (16 bytes), Key Size (1 byte)
    NVDS_TAG_LTK                        = 0x3E,
    NVDS_LEN_LTK                        = 28,

    /// PAIRING
    NVDS_TAG_PAIRING                    = 0x3F,
    NVDS_LEN_PAIRING                    = 54,

    /// Audio mode 0 task
    NVDS_TAG_AM0_FIRST                  = 0x90,
    NVDS_TAG_AM0_LAST                   = 0x9F,

    /// Local device Identity resolving key
    NVDS_TAG_LOC_IRK                    = 0xA0,
    NVDS_LEN_LOC_IRK                    = KEY_LEN,

    /// Peer device Resolving identity key (+identity address)
    NVDS_TAG_PEER_IRK                   = 0xA1,
    NVDS_LEN_PEER_IRK                   = sizeof(struct gapc_irk),
};
#endif // (NVDS_SUPPORT)

/// Advertising state machine
enum app_adv_state
{
    /// Advertising activity does not exists
    APP_ADV_STATE_IDLE = 0,
    /// Creating advertising activity
    APP_ADV_STATE_CREATING,
    /// Setting advertising data
    APP_ADV_STATE_SETTING_ADV_DATA,
    /// Setting scan response data
    APP_ADV_STATE_SETTING_SCAN_RSP_DATA,
     /// Updata adv data
    APP_ADV_STATE_UPDATA_ADV_DATA,
    /// Advertising activity created
    APP_ADV_STATE_CREATED,
    /// Starting advertising activity
    APP_ADV_STATE_STARTING,
    /// Advertising activity started
    APP_ADV_STATE_STARTED,
    /// Stopping advertising activity
    APP_ADV_STATE_STOPPING,    
    /// WAIT Deleteing advertising activity
    APP_ADV_STATE_WAITING_DELETE,
    
    /// Deleteing advertising activity
    APP_ADV_STATE_DELETEING,

	APP_ADV_STATE_UPDATA_SCAN_RSP_DATA,
	
	APP_ADV_STATE_UPDATA2_ADV_DATA,
	APP_ADV_STATE_UPDATA2_SCAN_RSP_DATA,
};

/// Scaning state machine
enum app_scan_state
{
    /// Scaning activity does not exists
    APP_SCAN_STATE_IDLE = 0,
    /// Creating Scaning activity
    APP_SCAN_STATE_CREATING,
    /// Scaning activity created
    APP_SCAN_STATE_CREATED,
    /// Starting Scaning activity
    APP_SCAN_STATE_STARTING,
    /// Scaning activity started
    APP_SCAN_STATE_STARTED,
    /// Stopping Scaning activity
    APP_SCAN_STATE_STOPPING,

};

/// Initing state machine
enum app_init_state
{
    /// Iint activity does not exists
    APP_INIT_STATE_IDLE = 0,
    /// Creating Iint activity
    APP_INIT_STATE_CREATING,
   /// Iint activity created
    APP_INIT_STATE_CREATED,
    
    /// WAIT Start Iint activity
    APP_INIT_STATE_WAIT_CONECTTING,
    /// Starting Iint activity
    APP_INIT_STATE_CONECTTING,
    /// Iint activity conected
    APP_INIT_STATE_CONECTED,
    /// Stopping Iint activity
    APP_INIT_STATE_STOPPING,

};

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// Structure containing information about the handlers for an application subtask
struct app_subtask_handlers
{
    /// Pointer to the message handler table
    const struct kernel_msg_handler *p_msg_handler_tab;
    /// Number of messages handled
    uint16_t msg_cnt;
};

/// Application environment structure
struct app_env_tag
{
    /// Connection handle
    uint16_t conhdl;
    /// Connection Index
    uint8_t  conidx;

    /// Advertising activity index
    uint8_t adv_actv_idx;
    /// Current advertising state (@see enum app_adv_state)
    uint8_t adv_state;
    /// Next expected operation completed event
    uint8_t adv_op;

    /// Last initialized profile
    uint8_t next_svc;

    /// Bonding status
    bool bonded;

    /// Device Name length
    uint8_t dev_name_len;
    /// Device Name
    uint8_t dev_name[APP_DEVICE_NAME_MAX_LEN];

    /// Local device IRK
    uint8_t loc_irk[KEY_LEN];

    /// Secure Connections on current link
    bool sec_con_enabled;

    /// Counter used to generate IRK
    uint8_t rand_cnt;
#if 1		
	/// Scaning activity index
	uint8_t scan_actv_idx;
	/// Current scaning state (@see enum app_scan_state)
	uint8_t scan_state;
	/// Next expected operation completed event
	uint8_t scan_op;
		/// Scan interval
	uint16_t scan_intv;
	/// Scan window
	uint16_t scan_wd;
	
	/// Init activity index
	uint8_t init_actv_idx;
	/// Current init state (@see enum app_init_state)
	uint8_t init_state;
	/// Next expected operation completed event
	uint8_t init_op;
	/// conn_intv value. Allowed range is 7.5ms to 4s.
	uint16_t conn_intv;
		/// Slave latency. Number of events that can be missed by a connected slave device
	uint16_t conn_latency;
	/// Link supervision timeout (in unit of 10ms). Allowed range is 100ms to 32s
	uint16_t conn_super_to;
	
	uint16_t conn_dev_to;

	uint8_t role[BLE_CONNECTION_MAX];
	/// Address information about a device address
	struct gap_bdaddr con_dev_addr[BLE_CONNECTION_MAX];
#endif
};

/*
 * GLOBAL VARIABLE DECLARATION
 ****************************************************************************************
 */

/// Application environment
extern struct app_env_tag app_ble_ctx;

extern void set_app_ble_adv_actv_idx(unsigned char idx);
extern int get_app_ble_adv_actv_idx(void);
extern void set_app_ble_conidx(unsigned char idx);
extern int get_app_ble_conidx(void);
extern void set_app_ble_adv_state(int adv_state);
extern int get_app_ble_adv_state(void);
extern void set_app_ble_adv_op(int adv_op);
extern int get_app_ble_adv_op(void);;
extern void ble_app_set_event_cb(ble_event_cb_t func);


extern void set_app_ble_scan_state(int scan_state);
extern int get_app_ble_scan_state(void);
extern void set_app_ble_scan_actv_idx(unsigned char idx);
extern int get_app_ble_scan_actv_idx(void);

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialize the BLE demo application.
 ****************************************************************************************
 */
void appm_init(void);


/**
 ****************************************************************************************
 * @brief
 ****************************************************************************************
 */
void appm_adv_fsm_next(char flag);

/**
 ****************************************************************************************
 * @brief Send to request to update the connection parameters
 ****************************************************************************************
 */
void appm_update_param(struct gapc_conn_param *conn_param);




/**
 ****************************************************************************************
 * @brief delete advertising
 *
 * @param[in] none
 ****************************************************************************************
 */

void appm_delete_advertising(void);
/**
 ****************************************************************************************
 * @brief Return if the device is currently bonded
 ****************************************************************************************
 */
bool app_sec_get_bond_status(void);
extern int get_app_ble_init_actv_idx(void);
extern void set_app_ble_init_state(int init_state);
extern void ble_appm_start_scaning(void);
extern void ble_appm_stop_scaning(void);
extern int ble_appm_updata_scan_rsp_data(unsigned char adv_actv_idx,unsigned char* scan_buff, unsigned char scan_len);
extern void ble_appm_disconnect(uint8_t conidx,uint8_t reason);
extern int ble_appm_set_adv_data(unsigned char adv_actv_idx,unsigned char* adv_buff, unsigned char adv_len);
extern void set_app_ble_init_actv_idx(int init_actv_idx);
/// @} APP

#endif //(BLE_APP_PRESENT)

#endif // APP_H_
