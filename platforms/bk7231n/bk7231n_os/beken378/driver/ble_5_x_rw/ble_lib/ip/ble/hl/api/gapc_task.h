/**
 ****************************************************************************************
 *
 * @file gapc_task.h
 *
 * @brief Generic Access Profile Controller Task Header.
 *
 * Copyright (C) RivieraWaves 2009-2016
 *
 ****************************************************************************************
 */
#ifndef _GAPC_TASK_H_
#define _GAPC_TASK_H_

/**
 ****************************************************************************************
 * @addtogroup GAPC_TASK Generic Access Profile Controller Task
 * @ingroup GAPC
 * @brief Handles ALL messages to/from GAP Controller block.
 *
 * It handles messages from lower and higher layers related to an ongoing connection.
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "rwip_task.h" // Task definitions
#include "gap.h"
#include <stdbool.h>
#include "kernel_msg.h"

/*
 * MACROS
 ****************************************************************************************
 */

/*
 * DEFINES
 ****************************************************************************************
 */


/// GAP Controller Task messages
/*@TRACE*/
enum gapc_msg_id
{
    /* Default event */
    /// Command Complete event
    GAPC_CMP_EVT                                        = MSG_ID(GAPC, 0x00),

    /* Connection state information */
    /// Indicate that a connection has been established
    GAPC_CONNECTION_REQ_IND                             = MSG_ID(GAPC, 0x01),
    /// Set specific link data configuration.
    GAPC_CONNECTION_CFM                                 = MSG_ID(GAPC, 0x02),

    /// Indicate that a link has been disconnected
    GAPC_DISCONNECT_IND                                 = MSG_ID(GAPC, 0x03),

    /* Link management command */
    /// Request disconnection of current link command.
    GAPC_DISCONNECT_CMD                                 = MSG_ID(GAPC, 0x04),

    /* Peer device info */
    /// Retrieve information command
    GAPC_GET_INFO_CMD                                   = MSG_ID(GAPC, 0x05),
    /// Peer device attribute DB info such as Device Name, Appearance or Slave Preferred Parameters
    GAPC_PEER_ATT_INFO_IND                              = MSG_ID(GAPC, 0x06),
    /// Indication of peer version info
    GAPC_PEER_VERSION_IND                               = MSG_ID(GAPC, 0x07),
    /// Indication of peer features info
    GAPC_PEER_FEATURES_IND                              = MSG_ID(GAPC, 0x08),
    /// Indication of ongoing connection RSSI
    GAPC_CON_RSSI_IND                                   = MSG_ID(GAPC, 0x09),

    /* Device Name Management */
    /// Peer device request local device info such as name, appearance or slave preferred parameters
    GAPC_GET_DEV_INFO_REQ_IND                           = MSG_ID(GAPC, 0x0A),
    /// Send requested info to peer device
    GAPC_GET_DEV_INFO_CFM                               = MSG_ID(GAPC, 0x0B),
    /// Peer device request to modify local device info such as name or appearance
    GAPC_SET_DEV_INFO_REQ_IND                           = MSG_ID(GAPC, 0x0C),
    /// Local device accept or reject device info modification
    GAPC_SET_DEV_INFO_CFM                               = MSG_ID(GAPC, 0x0D),

    /* Connection parameters update */
    /// Perform update of connection parameters command
    GAPC_PARAM_UPDATE_CMD                               = MSG_ID(GAPC, 0x0E),
    /// Request of updating connection parameters indication
    GAPC_PARAM_UPDATE_REQ_IND                           = MSG_ID(GAPC, 0x0F),
    /// Master confirm or not that parameters proposed by slave are accepted or not
    GAPC_PARAM_UPDATE_CFM                               = MSG_ID(GAPC, 0x10),
    /// Connection parameters updated indication
    GAPC_PARAM_UPDATED_IND                              = MSG_ID(GAPC, 0x11),

    /* Bonding procedure */
    /// Start Bonding command procedure
    GAPC_BOND_CMD                                       = MSG_ID(GAPC, 0x12),
    /// Bonding requested by peer device indication message.
    GAPC_BOND_REQ_IND                                   = MSG_ID(GAPC, 0x13),
    /// Confirm requested bond information.
    GAPC_BOND_CFM                                       = MSG_ID(GAPC, 0x14),
    /// Bonding information indication message
    GAPC_BOND_IND                                       = MSG_ID(GAPC, 0x15),

    /* Encryption procedure */
    /// Start Encryption command procedure
    GAPC_ENCRYPT_CMD                                    = MSG_ID(GAPC, 0x16),
    /// Encryption requested by peer device indication message.
    GAPC_ENCRYPT_REQ_IND                                = MSG_ID(GAPC, 0x17),
    /// Confirm requested Encryption information.
    GAPC_ENCRYPT_CFM                                    = MSG_ID(GAPC, 0x18),
    /// Encryption information indication message
    GAPC_ENCRYPT_IND                                    = MSG_ID(GAPC, 0x19),

    /* Security request procedure */
    /// Start Security Request command procedure
    GAPC_SECURITY_CMD                                   = MSG_ID(GAPC, 0x1A),
    /// Security requested by peer device indication message
    GAPC_SECURITY_IND                                   = MSG_ID(GAPC, 0x1B),

    /* Signature procedure */
    /// Indicate the current sign counters to the application
    GAPC_SIGN_COUNTER_IND                               = MSG_ID(GAPC, 0x1C),

    /* Device information */
    /// Indication of ongoing connection Channel Map
    GAPC_CON_CHANNEL_MAP_IND                            = MSG_ID(GAPC, 0x1D),

    /* LE Ping */
    /// Update LE Ping timeout value
    GAPC_SET_LE_PING_TO_CMD                             = MSG_ID(GAPC, 0x28),
    /// LE Ping timeout indication
    GAPC_LE_PING_TO_VAL_IND                             = MSG_ID(GAPC, 0x29),
    /// LE Ping timeout expires indication
    GAPC_LE_PING_TO_IND                                 = MSG_ID(GAPC, 0x2A),

    /* LE Data Length extension*/
    /// LE Set Data Length Command
    GAPC_SET_LE_PKT_SIZE_CMD                            = MSG_ID(GAPC, 0x2B),
    /// LE Set Data Length Indication
    GAPC_LE_PKT_SIZE_IND                                = MSG_ID(GAPC, 0x2C),

    /* Secure Connections */
    /// Request to inform the remote device when keys have been entered or erased
    GAPC_KEY_PRESS_NOTIFICATION_CMD                     = MSG_ID(GAPC, 0x2D),
    /// Indication that a KeyPress has been performed on the peer device.
    GAPC_KEY_PRESS_NOTIFICATION_IND                     = MSG_ID(GAPC, 0x2E),

    /* PHY Management */
    /// Set the PHY configuration for current active link
    GAPC_SET_PHY_CMD                                    = MSG_ID(GAPC, 0x2F),
    /// Active link PHY configuration. Triggered when configuration is read or during an update.
    GAPC_LE_PHY_IND                                     = MSG_ID(GAPC, 0x30),

    /* Channel Selection Algorithm */
    /// Indication of currently used channel selection algorithm
    /// @see struct gapc_chan_sel_algo_ind
    GAPC_CHAN_SEL_ALGO_IND                              = MSG_ID(GAPC, 0x31),

    /* Preferred Slave Latency */
    /// Set the preferred slave latency (for slave only, with RW controller)
    GAPC_SET_PREF_SLAVE_LATENCY_CMD                     = MSG_ID(GAPC, 0x32),
    /// Set the preferred slave event duration (for slave only, with RW controller)
    GAPC_SET_PREF_SLAVE_EVT_DUR_CMD                     = MSG_ID(GAPC, 0x33),

    /// Indication to the task that sends the unknown message
    GAPC_UNKNOWN_MSG_IND                                = MSG_ID(GAPC, 0x34),

    /* Periodic Sync Transfer */
    /// Transfer periodic advertising sync information to peer device
    GAPC_PER_ADV_SYNC_TRANS_CMD                         = MSG_ID(GAPC, 0x35),

    /* Constant Tone Extension */
    /// Constant Tone Extension Transmission configuration command
    GAPC_CTE_TX_CFG_CMD                                 = MSG_ID(GAPC, 0x38),
    /// Constant Tone Extension Reception configuration command
    GAPC_CTE_RX_CFG_CMD                                 = MSG_ID(GAPC, 0x39),
    /// Constant Tone Extension request control command (enable / disable)
    GAPC_CTE_REQ_CTRL_CMD                               = MSG_ID(GAPC, 0x3A),
    /// Constant Tone Extension Response control command (enable / disable)
    GAPC_CTE_RSP_CTRL_CMD                               = MSG_ID(GAPC, 0x3B),
    /// Indicate reception of a IQ Report event over a ble connection
    GAPC_CTE_IQ_REPORT_IND                              = MSG_ID(GAPC, 0x3C),

    // ---------------------- INTERNAL API ------------------------
    /* Internal messages for timer events, not part of API*/
    /// Signature procedure
    GAPC_SIGN_CMD                                       = MSG_ID(GAPC, 0xF0),
    /// Signature result
    GAPC_SIGN_IND                                       = MSG_ID(GAPC, 0xF1),

    /// Parameter update procedure timeout indication
    GAPC_PARAM_UPDATE_TO_IND                            = MSG_ID(GAPC, 0xF2),
    /// Pairing procedure timeout indication
    GAPC_SMP_TIMEOUT_TIMER_IND                          = MSG_ID(GAPC, 0xF3),
    /// Pairing repeated attempts procedure timeout indication
    GAPC_SMP_REP_ATTEMPTS_TIMER_IND                     = MSG_ID(GAPC, 0xF4),
};




/// request operation type - application interface
/*@TRACE*/
enum gapc_operation
{
    /*                 Operation Flags                  */
    /* No Operation (if nothing has been requested)     */
    /* ************************************************ */
    /// No operation
    GAPC_NO_OP                                    = 0x00,

    /* Connection management */
    /// Disconnect link
    GAPC_DISCONNECT                               = 0x01,

    /* Connection information */
    /// Retrieve name of peer device.
    GAPC_GET_PEER_NAME                            = 0x02,
    /// Retrieve peer device version info.
    GAPC_GET_PEER_VERSION                         = 0x03,
    /// Retrieve peer device features.
    GAPC_GET_PEER_FEATURES                        = 0x04,
    /// Get Peer device appearance
    GAPC_GET_PEER_APPEARANCE                      = 0x05,
    /// Get Peer device Slaved Preferred Parameters
    GAPC_GET_PEER_SLV_PREF_PARAMS                 = 0x06,
    /// Retrieve connection RSSI.
    GAPC_GET_CON_RSSI                             = 0x07,
    /// Retrieve Connection Channel MAP.
    GAPC_GET_CON_CHANNEL_MAP                      = 0x08,

    /* Connection parameters update */
    /// Perform update of connection parameters.
    GAPC_UPDATE_PARAMS                            = 0x09,

    /* Security procedures */
    /// Start bonding procedure.
    GAPC_BOND                                     = 0x0A,
    /// Start encryption procedure.
    GAPC_ENCRYPT                                  = 0x0B,
    /// Start security request procedure
    GAPC_SECURITY_REQ                             = 0x0C,

    /* LE Ping*/
    /// get timer timeout value
    GAPC_GET_LE_PING_TO                           = 0x12,
    /// set timer timeout value
    GAPC_SET_LE_PING_TO                           = 0x13,

    /* LE Data Length extension*/
    /// LE Set Data Length
    GAPC_SET_LE_PKT_SIZE                          = 0x14,

    /* Central Address resolution supported*/
    GAPC_GET_ADDR_RESOL_SUPP                      = 0x15,

    /* Secure Connections */
    /// Request to inform the remote device when keys have been entered or erased
    GAPC_KEY_PRESS_NOTIFICATION                   = 0x16,

    /* PHY Management */
    /// Set the PHY configuration for current active link
    GAPC_SET_PHY                                  = 0x17,
    /// Retrieve PHY configuration of active link
    GAPC_GET_PHY                                  = 0x18,

    /* Channel Selection Algorithm */
    /// Retrieve Channel Selection Algorithm
    GAPC_GET_CHAN_SEL_ALGO                        = 0x19,

    /* Preferred slave latency */
    /// Set the preferred slave latency (for slave only, with RW controller)
    GAPC_SET_PREF_SLAVE_LATENCY                   = 0x1A,
    /// Set the preferred slave event duration (for slave only, with RW controller)
    GAPC_SET_PREF_SLAVE_EVT_DUR                   = 0x1B,

    /* Periodic Sync Transfer */
    /// Transfer periodic advertising sync information to peer device
    GAPC_PER_ADV_SYNC_TRANS                       = 0x1C,

    /* Constant Tone Extension */
    /// Constant Tone Extension Transmission configuration
    GAPC_CTE_TX_CFG                                = 0x20,
    /// Constant Tone Extension Reception configuration
    GAPC_CTE_RX_CFG                                = 0x21,
    /// Constant Tone Extension request control (enable / disable)
    GAPC_CTE_REQ_CTRL                              = 0x22,
    /// Constant Tone Extension Response control (enable / disable)
    GAPC_CTE_RSP_CTRL                              = 0x23,

    // ---------------------- INTERNAL API ------------------------
    /* Packet signature */
    /// sign an attribute packet
    GAPC_SIGN_PACKET                              = 0xF0,
    /// Verify signature or an attribute packet
    GAPC_SIGN_CHECK                               = 0xF1,
};

/// Bond event type.
/*@TRACE*/
enum gapc_bond
{
    /// Bond Pairing request
    GAPC_PAIRING_REQ,
    /// Respond to Pairing request
    GAPC_PAIRING_RSP,

    /// Pairing Finished information
    GAPC_PAIRING_SUCCEED,
    /// Pairing Failed information
    GAPC_PAIRING_FAILED,

    /// Used to retrieve pairing Temporary Key
    GAPC_TK_EXCH,
    /// Used for Identity Resolving Key exchange
    GAPC_IRK_EXCH,
    /// Used for Connection Signature Resolving Key exchange
    GAPC_CSRK_EXCH,
    /// Used for Long Term Key exchange
    GAPC_LTK_EXCH,

    /// Bond Pairing request issue, Repeated attempt
    GAPC_REPEATED_ATTEMPT,

    /// Out of Band - exchange of confirm and rand.
    GAPC_OOB_EXCH,

    /// Numeric Comparison - Exchange of Numeric Value -
    GAPC_NC_EXCH
};

/// List of device info that should be provided by application
/*@TRACE*/
enum gapc_dev_info
{
    /// Device Name
    GAPC_DEV_NAME,
    /// Device Appearance Icon
    GAPC_DEV_APPEARANCE,
    /// Device Slave preferred parameters
    GAPC_DEV_SLV_PREF_PARAMS,
    /// Device Central address resolution
    GAPC_DEV_CTL_ADDR_RESOL,
    /// maximum device info parameter
    GAPC_DEV_INFO_MAX,
};

/// List of features available on a device
enum gapc_features_list
{
    /// LE encryption
    GAPC_ENCRYPT_FEAT_MASK              = (1 << 0),
    /// Connection Parameters Request Procedure
    GAPC_CONN_PARAM_REQ_FEAT_MASK       = (1 << 1),
    /// Extended Reject Indication
    GAPC_EXT_REJECT_IND_FEAT_MASK       = (1 << 2),
    /// Slave-initiated Features Exchange
    GAPC_SLAVE_FEAT_EXCH_FEAT_MASK      = (1 << 3),
    /// LE ping
    GAPC_LE_PING_FEAT_MASK              = (1 << 4)
};

/// Option for PHY configuration
enum gapc_phy_option
{
    /// No preference for rate used when transmitting on the LE Coded PHY
    GAPC_PHY_OPT_LE_CODED_ALL_RATES     = (1 << 0),
    /// 500kbps rate preferred when transmitting on the LE Coded PHY
    GAPC_PHY_OPT_LE_CODED_500K_RATE     = (1 << 1),
    /// 125kbps  when transmitting on the LE Coded PHY
    GAPC_PHY_OPT_LE_CODED_125K_RATE     = (1 << 2),
};

/// Keypress Notification types
enum gapc_key_ntf_type
{
    /// Passkey entry started
    GAPC_KEY_NTF_ENTRY_STARTED = 0x00,
    /// Passkey digit entered
    GAPC_KEY_NTF_DIGIT_ENTERED,
    /// Passkey digit erased
    GAPC_KEY_NTF_DIGIT_ERASED,
    /// Passkey cleared
    GAPC_KEY_NTF_CLEARED,
    /// Passkey entry completed
    GAPC_KEY_NTF_ENTRY_COMPLETED
};

/// Client bond information
enum gapc_cli_info
{
    /// Service changed indication enabled
    GAPC_SVC_CHANGED_IND_EN_BIT = (1 << 0),
    GAPC_SVC_CHANGED_IND_EN_POS = 0,
    /// Database updated since last connection
    GAPC_DB_UPDATED_BIT         = (1 << 1),
    GAPC_DB_UPDATED_POS         = 1,
};

/// Client supported features
enum gapc_cli_feat
{
    /// Robust Cache feature enabled
    GAPC_ROBUST_CACHE_EN_BIT    = (1 << 0),
    GAPC_ROBUST_CACHE_EN_POS    = 0,
};

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// Operation command structure in order to keep requested operation.
struct gapc_operation_cmd
{
    /// GAP request type
    uint8_t operation;
};


/// Command complete event data structure
/*@TRACE*/
struct gapc_cmp_evt
{
    /// GAP request type
    uint8_t operation;
    /// Status of the request
    uint8_t status;
};

/// Indicate that an unknown message has been received
/*@TRACE*/
struct gapc_unknown_msg_ind
{
    /// Unknown message id
    kernel_msg_id_t unknown_msg_id;
};

/// Indicate that a connection has been established
/*@TRACE*/
struct gapc_connection_req_ind
{
    /// Connection handle
    uint16_t conhdl;
    /// Connection interval
    uint16_t con_interval;
    /// Connection latency
    uint16_t con_latency;
    /// Link supervision timeout
    uint16_t sup_to;
    /// Clock accuracy
    uint8_t clk_accuracy;
    /// Peer address type
    uint8_t peer_addr_type;
    /// Peer BT address
    bd_addr_t peer_addr;
    /// Role of device in connection (0 = Master / 1 = Slave)
    uint8_t role;
};


/// Set specific link data configuration.
/*@TRACE*/
struct gapc_connection_cfm
{
    /// Local CSRK value
    struct gap_sec_key lcsrk;
    /// Local signature counter value
    uint32_t           lsign_counter;
    /// Remote CSRK value
    struct gap_sec_key rcsrk;
    /// Remote signature counter value
    uint32_t           rsign_counter;
    /// Authentication (@see enum gap_auth)
    uint8_t            auth;
    /// Client bond data information (@see enum gapc_cli_info)
    uint8_t            cli_info;
    /// LTK exchanged during pairing.
    bool               ltk_present;
    /// Client supported features    (@see enum gapc_cli_feat)
    uint8_t            cli_feat;
    /// Peer GATT Service Start handle
    uint16_t           gatt_start_handle;
    /// Peer GATT Service End Handle
    uint16_t           gatt_end_handle;
    /// Peer Service Change value handle
    uint16_t           svc_chg_handle;
};

/// Request disconnection of current link command.
/*@TRACE*/
struct gapc_disconnect_cmd
{
    /// GAP request type:
    /// - GAPC_DISCONNECT: Disconnect link.
    uint8_t operation;

    /// Reason of disconnection
    uint8_t reason;
};


/// Indicate that a link has been disconnected
/*@TRACE*/
struct gapc_disconnect_ind
{
    /// Connection handle
    uint16_t conhdl;
    /// Reason of disconnection
    uint8_t reason;
};


/// Retrieve information command
/*@TRACE*/
struct gapc_get_info_cmd
{
    /// GAP request type:
    /// - GAPC_GET_PEER_NAME: Retrieve name of peer device.
    /// - GAPC_GET_PEER_VERSION: Retrieve peer device version info.
    /// - GAPC_GET_PEER_FEATURES: Retrieve peer device features.
    /// - GAPC_GET_CON_RSSI: Retrieve connection RSSI.
    /// - GAPC_GET_CON_CHANNEL_MAP: Retrieve Connection Channel MAP.
    /// - GAPC_GET_PEER_APPEARANCE: Get Peer device appearance
    /// - GAPC_GET_PEER_SLV_PREF_PARAMS: Get Peer device Slaved Preferred Parameters
    /// - GAPC_GET_ADDR_RESOL_SUPP: Address Resolution Supported
    /// - GAPC_GET_LE_PING_TIMEOUT: Retrieve LE Ping Timeout Value
    uint8_t operation;
};

/// device information data
/*@TRACE
 @trc_ref gapc_dev_info*/
union gapc_dev_info_val
{
    /// Device name
    //@trc_union parent.req == GAPC_DEV_NAME
    struct gap_dev_name name;
    /// Appearance Icon
    //@trc_union parent.req == GAPC_DEV_APPEARANCE
    uint16_t appearance;
    /// Slave preferred parameters
    //@trc_union parent.req == GAPC_DEV_SLV_PREF_PARAMS
    struct gap_slv_pref slv_pref_params;
    /// Central address resolution
    //@trc_union parent.req == GAPC_DEV_CTL_ADDR_RESOL
    uint8_t ctl_addr_resol;
};

/// Peer device attribute DB info such as Device Name, Appearance or Slave Preferred Parameters
/*@TRACE*/
struct gapc_peer_att_info_ind
{
    /// Requested information
    /// - GAPC_DEV_NAME: Device Name
    /// - GAPC_DEV_APPEARANCE: Device Appearance Icon
    /// - GAPC_DEV_SLV_PREF_PARAMS: Device Slave preferred parameters
    /// - GAPC_GET_ADDR_RESOL_SUPP: Address resolution supported
    uint8_t  req;
    /// Attribute handle
    uint16_t handle;

    /// device information data
    union gapc_dev_info_val info;
};

/// Indication of peer version info
/*@TRACE*/
struct gapc_peer_version_ind
{
    /// Manufacturer name
    uint16_t compid;
    /// LMP subversion
    uint16_t lmp_subvers;
    /// LMP version
    uint8_t  lmp_vers;
};

/// Indication of peer features info
/*@TRACE*/
struct gapc_peer_features_ind
{
    /// 8-byte array for LE features
    uint8_t features[GAP_LE_FEATS_LEN];
};

/// Indication of ongoing connection RSSI
/*@TRACE*/
struct gapc_con_rssi_ind
{
    /// RSSI value
    int8_t rssi;
};

/// Indication of ongoing connection Channel Map
/*@TRACE*/
struct gapc_con_channel_map_ind
{
    /// channel map value
    le_chnl_map_t ch_map;
};

/// Sign counter value changed due to packet signing or signature verification.
struct gapc_sign_counter_updated_ind
{
    /// New Local signature counter value
    uint32_t lsign_counter;
    /// New Remote signature counter value
    uint32_t rsign_counter;
};

/// Indication of LE Ping
/*@TRACE*/
struct gapc_le_ping_to_val_ind
{
    ///Authenticated payload timeout
    uint16_t timeout;
};


/// Peer device request local device info such as name, appearance or slave preferred parameters
/*@TRACE*/
struct gapc_get_dev_info_req_ind
{
    /// Requested information
    /// - GAPC_DEV_NAME: Device Name
    /// - GAPC_DEV_APPEARANCE: Device Appearance Icon
    /// - GAPC_DEV_SLV_PREF_PARAMS: Device Slave preferred parameters
    uint8_t req;
};



/// Send requested info to peer device
/*@TRACE*/
struct gapc_get_dev_info_cfm
{
    /// Requested information
    /// - GAPC_DEV_NAME: Device Name
    /// - GAPC_DEV_APPEARANCE: Device Appearance Icon
    /// - GAPC_DEV_SLV_PREF_PARAMS: Device Slave preferred parameters
    uint8_t req;

    /// Peer device information data
    union gapc_dev_info_val info;
};

/// device information data
/*@TRACE
 @trc_ref gapc_dev_info*/
union gapc_set_dev_info
{
    /// Device name
    //@trc_union parent.req == GAPC_DEV_NAME
    struct gap_dev_name name;
    /// Appearance Icon
    //@trc_union parent.req == GAPC_DEV_APPEARANCE
    uint16_t appearance;
};

/// Peer device request to modify local device info such as name or appearance
/*@TRACE*/
struct gapc_set_dev_info_req_ind
{
    /// Requested information
    /// - GAPC_DEV_NAME: Device Name
    /// - GAPC_DEV_APPEARANCE: Device Appearance Icon
    uint8_t req;

    /// device information data
    union gapc_set_dev_info info;
};

/// Local device accept or reject device info modification
/*@TRACE*/
struct gapc_set_dev_info_cfm
{
    /// Requested information
    /// - GAPC_DEV_NAME: Device Name
    /// - GAPC_DEV_APPEARANCE: Device Appearance Icon
    uint8_t req;

    /// Status code used to know if requested has been accepted or not
    uint8_t status;
};

/// Connection Parameter used to update connection parameters
struct gapc_conn_param
{
    /// Connection interval minimum
    uint16_t intv_min;
    /// Connection interval maximum
    uint16_t intv_max;
    /// Latency
    uint16_t latency;
    /// Supervision timeout
    uint16_t time_out;
};

/// Perform update of connection parameters command
/*@TRACE*/
struct gapc_param_update_cmd
{
    /// GAP request type:
    /// - GAPC_UPDATE_PARAMS: Perform update of connection parameters.
    uint8_t operation;
    /// Internal parameter used to manage internally l2cap packet identifier for signaling
    uint8_t pkt_id;
    /// Connection interval minimum
    uint16_t intv_min;
    /// Connection interval maximum
    uint16_t intv_max;
    /// Latency
    uint16_t latency;
    /// Supervision timeout
    uint16_t time_out;
    /// Minimum Connection Event Duration
    uint16_t ce_len_min;
    /// Maximum Connection Event Duration
    uint16_t ce_len_max;
};

/// Request of updating connection parameters indication
/*@TRACE*/
struct gapc_param_update_req_ind
{
    /// Connection interval minimum
    uint16_t intv_min;
    /// Connection interval maximum
    uint16_t intv_max;
    /// Latency
    uint16_t latency;
    /// Supervision timeout
    uint16_t time_out;
};

/// Connection parameters updated indication
/*@TRACE*/
struct gapc_param_updated_ind
{
    ///Connection interval value
    uint16_t            con_interval;
    ///Connection latency value
    uint16_t            con_latency;
    ///Supervision timeout
    uint16_t            sup_to;
};

/// Master confirm or not that parameters proposed by slave are accepted or not
/*@TRACE*/
struct gapc_param_update_cfm
{
    /// True to accept slave connection parameters, False else.
    bool accept;
    /// Minimum Connection Event Duration
    uint16_t ce_len_min;
    /// Maximum Connection Event Duration
    uint16_t ce_len_max;
};

/// Parameters of the @ref GAPC_SET_PREF_SLAVE_LATENCY_CMD message
/*@TRACE*/
struct gapc_set_pref_slave_latency_cmd
{
    /// GAP request type:
    /// - GAPC_SET_PREF_SLAVE_LATENCY_CMD : Set preferred slave latency
    uint8_t operation;
    /// Preferred latency that the controller should use on a connection (in number of connection events)
    uint16_t latency;
};

/// Parameters of the @ref GAPC_SET_PREF_SLAVE_EVT_DUR_CMD message
/*@TRACE*/
struct gapc_set_pref_slave_evt_dur_cmd
{
    /// GAP request type:
    /// - GAPC_SET_PREF_SLAVE_EVT_DUR_CMD : Set preferred slave event duration
    uint8_t operation;
    /// Preferred event duration that the controller should use on a connection (N * 0.625 ms)
    uint16_t duration;
    /// Slave transmits a single packet per connection event (False/True)
    uint8_t single_tx;
};


/// Parameters of the @ref GAPC_PER_ADV_SYNC_TRANS_CMD message
/*@TRACE*/
struct gapc_per_adv_sync_trans_cmd
{
    /// GAP request type:
    /// - GAPC_PER_ADV_SYNC_TRANS : Periodic Advertising Sync Transfer
    uint8_t  operation;
    /// Periodic Advertising or Periodic Sync activity index
    uint8_t  actv_idx;
    /// A value provided by application
    uint16_t service_data;
};

/// Parameters of the @ref GAPC_CTE_TX_CFG_CMD message
/*@TRACE*/
struct gapc_cte_tx_cfg_cmd
{
    /// GAP request type:
    /// - GAPC_CTE_TX_CFG: Constant Tone Extension Transmission configuration
    uint8_t  operation;
    /// CTE types (bit0: AOA | bit1: AOD-1us | bit2: AOD-2us) (@see enum gap_cte_type)
    uint8_t  cte_types;
    /// Length of switching pattern (number of antenna IDs in the pattern)
    uint8_t  switching_pattern_len;
    /// Antenna IDs
    uint8_t  antenna_id[__ARRAY_EMPTY];
};

/// Parameters of the @ref GAPC_CTE_RX_CFG_CMD message
/*@TRACE*/
struct gapc_cte_rx_cfg_cmd
{
    /// GAP request type:
    /// - GAPC_CTE_RX_CFG: Constant Tone Extension Reception configuration
    uint8_t  operation;
    /// Sampling enable
    bool     sample_en;
    /// Slot durations (1: 1us | 2: 2us)
    uint8_t  slot_dur;
    /// Length of switching pattern (number of antenna IDs in the pattern)
    uint8_t  switching_pattern_len;
    /// Antenna IDs
    uint8_t  antenna_id[__ARRAY_EMPTY];
};

/// Parameters of the @ref GAPC_CTE_REQ_CTRL_CMD message
/*@TRACE*/
struct gapc_cte_req_ctrl_cmd
{
    /// GAP request type:
    /// - GAPC_CTE_REQ_CTRL: Constant Tone Extension request control (enable / disable)
    uint8_t  operation;
    /// True to enable TX or RX Constant Tone Extension, False to disable
    bool     enable;
    /// CTE request interval (in number of connection events)
    uint16_t interval;
    /// Requested CTE length (in 8us unit)
    uint8_t  cte_len;
    /// Requested CTE type (0: AOA | 1: AOD-1us | 2: AOD-2us)  (@see enum gap_cte_type)
    uint8_t  cte_type;
};

/// Parameters of the @ref GAPC_CTE_RSP_CTRL_CMD message
/*@TRACE*/
struct gapc_cte_rsp_ctrl_cmd
{
    /// GAP request type:
    /// - GAPC_CTE_RSP_CTRL: Constant Tone Extension response control (enable / disable)
    uint8_t  operation;
    /// True to enable TX or RX Constant Tone Extension, False to disable
    bool     enable;
};

/// Indicate reception of a IQ Report event over a BLE connection
/*@TRACE*/
struct gapc_cte_iq_report_ind
{
    /// Rx PHY  (@see enum gap_phy_val)
    uint8_t  rx_phy;
    /// Data channel index
    uint8_t  data_channel_idx;
    /// RSSI (in 0.1 dBm)
    int16_t  rssi;
    /// RSSI antenna ID
    uint8_t  rssi_antenna_id;
    /// CTE type (0: AOA | 1: AOD-1us | 2: AOD-2us) (@see enum gap_cte_type)
    uint8_t  cte_type;
    /// Slot durations (1: 1us | 2: 2us)
    uint8_t  slot_dur;
    /// Packet status
    uint8_t  pkt_status;
    /// Connection event counter
    uint16_t con_evt_cnt;
    /// Number of samples
    uint8_t  nb_samples;
    /// I/Q sample
    struct gap_iq_sample sample[__ARRAY_EMPTY];
};

/// Pairing parameters
/*@TRACE*/
struct gapc_pairing
{
    /// IO capabilities (@see gap_io_cap)
    uint8_t iocap;
    /// OOB information (@see gap_oob)
    uint8_t oob;
    /// Authentication (@see gap_auth)
    /// Note in BT 4.1 the Auth Field is extended to include 'Key Notification' and
    /// and 'Secure Connections'.
    uint8_t auth;
    /// Encryption key size (7 to 16)
    uint8_t key_size;
    ///Initiator key distribution (@see gap_kdist)
    uint8_t ikey_dist;
    ///Responder key distribution (@see gap_kdist)
    uint8_t rkey_dist;

    /// Device security requirements (minimum security level). (@see gap_sec_req)
    uint8_t sec_req;
};

/// Long Term Key information
/*@TRACE*/
struct gapc_ltk
{
    /// Long Term Key
    struct gap_sec_key ltk;
    /// Encryption Diversifier
    uint16_t ediv;
    /// Random Number
    rand_nb_t randnb;
    /// Encryption key size (7 to 16)
    uint8_t key_size;
};
/// Out of Band Information
/*@TRACE*/
struct gapc_oob
{
    /// Confirm Value
    uint8_t conf[GAP_KEY_LEN];
    /// Random Number
    uint8_t rand[GAP_KEY_LEN];
};

/*@TRACE*/
struct gapc_nc
{
    uint8_t value[4];
};

/// Identity Resolving Key information
/*@TRACE*/
struct gapc_irk
{
    /// Identity Resolving Key
    struct gap_sec_key irk;
    /// Device BD Identity Address
    struct gap_bdaddr addr;
};


/// Start Bonding command procedure
/*@TRACE*/
struct gapc_bond_cmd
{
    /// GAP request type:
    /// - GAPC_BOND:  Start bonding procedure.
    uint8_t operation;
    /// Pairing information
    struct gapc_pairing pairing;
};

/// Bond procedure requested information data
/*@TRACE
 @trc_ref gapc_bond*/
union gapc_bond_req_data
{
    /// Authentication level (@see gap_auth) (if request = GAPC_PAIRING_REQ)
    //@trc_union parent.request == GAPC_PAIRING_REQ
    uint8_t auth_req;
    /// LTK Key Size (if request = GAPC_LTK_EXCH)
    //@trc_union parent.request == GAPC_LTK_EXCH
    uint8_t key_size;
    /// Device IO used to get TK: (if request = GAPC_TK_EXCH)
    ///  - GAP_TK_OOB:       TK get from out of band method
    ///  - GAP_TK_DISPLAY:   TK generated and shall be displayed by local device
    ///  - GAP_TK_KEY_ENTRY: TK shall be entered by user using device keyboard
    //@trc_union parent.request == GAPC_TK_EXCH
    uint8_t tk_type;

    /// Addition OOB Data for the OOB Conf and Rand values
    //@trc_union parent.request == GAPC_OOB_EXCH
    struct gapc_oob  oob_data;
    /// Numeric Comparison Data
    //@trc_union parent.request == GAPC_NC_EXCH
    struct gapc_nc   nc_data;
};

/// Bonding requested by peer device indication message.
/*@TRACE*/
struct gapc_bond_req_ind
{
    /// Bond request type (@see gapc_bond)
    uint8_t request;

    /// Bond procedure requested information data
    union gapc_bond_req_data data;
};

/*@TRACE
 @trc_ref gapc_bond
*/
union gapc_bond_cfm_data
{
    /// Pairing Features (request = GAPC_PAIRING_RSP)
    //@trc_union parent.request == GAPC_PAIRING_RSP
    struct gapc_pairing pairing_feat;
    /// LTK (request = GAPC_LTK_EXCH)
    //@trc_union parent.request == GAPC_LTK_EXCH
    struct gapc_ltk ltk;
    /// CSRK (request = GAPC_CSRK_EXCH)
    //@trc_union parent.request == GAPC_CSRK_EXCH
    struct gap_sec_key csrk;
    /// TK (request = GAPC_TK_EXCH)
    //@trc_union parent.request == GAPC_TK_EXCH
    struct gap_sec_key tk;
    /// IRK (request = GAPC_IRK_EXCH)
    //@trc_union parent.request == GAPC_IRK_EXCH
    struct gapc_irk irk;
    /// OOB Confirm and Random from the peer (request = GAPC_OOB_EXCH)
    //@trc_union parent.request == GAPC_OOB_EXCH
    struct gapc_oob oob;
};

/// Confirm requested bond information.
/*@TRACE*/
struct gapc_bond_cfm
{
    /// Bond request type (@see gapc_bond)
    uint8_t request;
    /// Request accepted
    uint8_t accept;

    /// Bond procedure information data
    union gapc_bond_cfm_data data;
};

/**
 *  Authentication information
 */
/*@TRACE*/
struct gapc_bond_auth
{
    /// Authentication information (@see gap_auth)
    uint8_t info;

    /// LTK exchanged during pairing.
    bool ltk_present;
};

/// Bond procedure information data
/*@TRACE
 @trc_ref gapc_bond*/
union gapc_bond_data
{
    /// Authentication information (@see gap_auth)
    /// (if info = GAPC_PAIRING_SUCCEED)
    //@trc_union parent.info == GAPC_PAIRING_SUCCEED
    struct gapc_bond_auth auth;
    /// Pairing failed reason  (if info = GAPC_PAIRING_FAILED)
    //@trc_union parent.info == GAPC_PAIRING_FAILED
    uint8_t reason;
    /// Long Term Key information (if info = GAPC_LTK_EXCH)
    //@trc_union parent.info == GAPC_LTK_EXCH
    struct gapc_ltk ltk;
    /// Identity Resolving Key information (if info = GAPC_IRK_EXCH)
    //@trc_union parent.info == GAPC_IRK_EXCH
    struct gapc_irk irk;
    /// Connection Signature Resolving Key information (if info = GAPC_CSRK_EXCH)
    //@trc_union parent.info == GAPC_CSRK_EXCH
    struct gap_sec_key csrk;
};

/// Bonding information indication message
/*@TRACE*/
struct gapc_bond_ind
{
    /// Bond information type (@see gapc_bond)
    uint8_t info;

    /// Bond procedure information data
    union gapc_bond_data data;
};

/// Start Encryption command procedure
/*@TRACE*/
struct gapc_encrypt_cmd
{
    /// GAP request type:
    /// - GAPC_ENCRYPT:  Start encryption procedure.
    uint8_t operation;
    /// Long Term Key information
    struct gapc_ltk ltk;
};

/// Encryption requested by peer device indication message.
/*@TRACE*/
struct gapc_encrypt_req_ind
{
    /// Encryption Diversifier
    uint16_t ediv;
    /// Random Number
    rand_nb_t rand_nb;
};

/// Confirm requested Encryption information.
/*@TRACE*/
struct gapc_encrypt_cfm
{
    /// Indicate if a LTK has been found for the peer device
    uint8_t found;
    /// Long Term Key
    struct gap_sec_key ltk;
    /// LTK Key Size
    uint8_t key_size;
};

/// Encryption information indication message
/*@TRACE*/
struct gapc_encrypt_ind
{
    /// Authentication  level (@see gap_auth)
    uint8_t auth;
};

/// Start Security Request command procedure
/*@TRACE*/
struct gapc_security_cmd
{
    /// GAP request type:
    /// - GAPC_SECURITY_REQ: Start security request procedure
    uint8_t operation;
    /// Authentication level (@see gap_auth)
    uint8_t auth;
};
/// Security requested by peer device indication message
/*@TRACE*/
struct gapc_security_ind
{
    /// Authentication level (@see gap_auth)
    uint8_t auth;
};

/// Parameters of the @ref GAPC_SIGN_COUNTER_IND message
/*@TRACE*/
struct gapc_sign_counter_ind
{
    /// Local SignCounter value
    uint32_t local_sign_counter;
    /// Peer SignCounter value
    uint32_t peer_sign_counter;
};


/// Parameters of the @ref GAPC_SET_LE_PING_TO_CMD message
/*@TRACE*/
struct gapc_set_le_ping_to_cmd
{
    /// GAP request type:
    /// - GAPC_SET_LE_PING_TO : Set the LE Ping timeout value
    uint8_t operation;
    /// Authenticated payload timeout
    uint16_t timeout;
};

/// Parameters of the @ref GAPC_SET_LE_PKT_SIZE_CMD message
/*@TRACE*/
struct gapc_set_le_pkt_size_cmd
{
    /// GAP request type:
    /// - GAPC_SET_LE_PKT_SIZE : Set the LE Data length value
    uint8_t operation;
    ///Preferred maximum number of payload octets that the local Controller should include
    ///in a single Link Layer Data Channel PDU.
    uint16_t tx_octets;
    ///Preferred maximum number of microseconds that the local Controller should use to transmit
    ///a single Link Layer Data Channel PDU
    uint16_t tx_time;
};

/// Parameters of the @ref GAPC_LE_PKT_SIZE_IND message
/*@TRACE*/
struct gapc_le_pkt_size_ind
{
    ///The maximum number of payload octets in TX
    uint16_t max_tx_octets;
    ///The maximum time that the local Controller will take to TX
    uint16_t max_tx_time;
    ///The maximum number of payload octets in RX
    uint16_t max_rx_octets;
    ///The maximum time that the local Controller will take to RX
    uint16_t max_rx_time;
};

/// Parameters of the @ref GAPC_KEY_PRESS_NOTIFICATION_CMD message
/*@TRACE*/
struct gapc_key_press_notif_cmd
{
    /// GAP request type:
    /// - GAPC_KEY_PRESS_NOTIFICATION_CMD : Inform the remote device when keys have been entered or erased
    uint8_t operation;
    /// notification type
    uint8_t notification_type;
};

/// Parameters of the @ref GAPC_KEY_PRESS_NOTIFICATION_IND message
/*@TRACE*/
struct gapc_key_press_notif_ind
{
    /// notification type
    uint8_t notification_type;
};

/// Set the PHY configuration for current active link
/*@TRACE*/
struct gapc_set_phy_cmd
{
    /// GAP request type:
    /// - GAPC_SET_PHY : Set the PHY configuration for current active link
    uint8_t operation;
    /// Supported LE PHY for data transmission (@see enum gap_phy)
    uint8_t tx_phy;
    /// Supported LE PHY for data reception (@see enum gap_phy)
    uint8_t rx_phy;
    /// PHY options (@see enum gapc_phy_option)
    uint8_t phy_opt;
};

/// Active link PHY configuration. Triggered when configuration is read or during an update.
/*@TRACE*/
struct gapc_le_phy_ind
{
    /// LE PHY for data transmission (@see enum gap_phy)
    uint8_t tx_phy;
    /// LE PHY for data reception (@see enum gap_phy)
    uint8_t rx_phy;
};

/// Parameters of the @ref GAPC_SIGN_CMD message
/*@TRACE*/
struct gapc_sign_cmd
{
    /// GAP request type:
    /// - GAPC_SIGN_PACKET: Sign an attribute packet
    /// - GAPC_SIGN_CHECK:  Verify signature or an attribute packet
    uint8_t operation;
    /// Data PDU length (Bytes)
    uint16_t byte_len;
    /// Data PDU + SignCounter if generation, Data PDU + SignCounter + MAC if verification
    uint8_t msg[__ARRAY_EMPTY];
};

/// Parameters of the @ref GAPC_SIGN_IND message
/*@TRACE*/
struct gapc_sign_ind
{
    /// GAP request type:
    /// - GAPC_SIGN_PACKET: Sign an attribute packet
    /// - GAPC_SIGN_CHECK:  Verify signature or an attribute packet
    uint8_t operation;
    /// Data PDU length (Bytes)
    uint16_t byte_len;
    /// Data PDU + SignCounter + MAC
    uint8_t signed_msg[__ARRAY_EMPTY];
};

/// Parameters of the @ref GAPC_CHAN_SEL_ALGO_IND
/*@TRACE*/
struct gapc_chan_sel_algo_ind
{
    /// Used channel selection algorithm
    uint8_t chan_sel_algo;
};

/// @} GAPC_TASK

#endif /* _GAPC_TASK_H_ */
