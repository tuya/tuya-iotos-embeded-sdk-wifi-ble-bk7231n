/**
 ****************************************************************************************
 *
 * @file gattc.h
 *
 * @brief Header file - GATT Controller.
 *
 * Copyright (C) RivieraWaves 2009-2016
 *
 *
 ****************************************************************************************
 */

#ifndef GATTC_H_
#define GATTC_H_

/**
 ****************************************************************************************
 * @addtogroup GATTC Generic Attribute Profile Controller
 * @ingroup GATT
 * @brief Generic Attribute Profile Controller.
 *
 * This GATT module is responsible for providing an API for all attribute related operations
 * related to a BLE connection.
 * It is responsible for all the service framework activities using the Attribute protocol
 * for discovering services and for reading and writing characteristic values on a peer device.
 * To achieve this, the GATT interfaces with @ref ATTC "ATTC" and the @ref ATTS "ATTS".
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
/* kernel task */
#include "rwip_config.h"
#if (BLE_GATTC)

#include "common_list.h"


/*
 * DEFINES
 ****************************************************************************************
 */

/// Operation type
enum gattc_op_type
{
    #if (BLE_ATTS)
    /// Operation used to Server Request operations
    GATTC_OP_SERVER,
    #endif // (BLE_ATTS)

    #if (BLE_ATTC)
    /// Operation used to Client Request operations
    GATTC_OP_CLIENT,
    /// Service Discovery Procedure operation
    GATTC_OP_SDP,
    /// Operation used to enable Client features
    GATTC_OP_FEAT,
    #endif // (BLE_ATTC)

    /// Max number of operations
    GATTC_OP_MAX
};

/*
 * TYPES DEFINITIONS
 ****************************************************************************************
 */

#if (BLE_ATTC)
/// Attribute Client environment variable requirements
struct attc_env
{
    /// List of ATT message used to aggregate long value in a single buffer.
    struct common_list rsp_list;
    /// List that contains peer device event registration
    struct common_list reg_evt;
    /// List that contains data for service discovery
    struct common_list sdp_data;

    /// GATT Service Start handle
    uint16_t       gatt_start_handle;
    /// Number of attributes present in GATT service
    uint8_t        gatt_nb_att;
    /// Service Change value handle offset
    uint8_t        svc_chg_offset;
};
#endif // (BLE_ATTC)


#if (BLE_ATTS)
/// Attribute server environment variables
struct atts_env
{
    /// This is used to merge save all the prepare write request received ,
    /// before receiving the execute or cancel or disconnection.
    struct common_list             prep_wr_req_list;
    /// This list is used to put any data in order to send a response to peer device
    struct common_list             rsp;
    /// List of PDU to process
    struct common_list             pdu_queue;
    /// This structure is used to store in cache latest attribute read value
    struct gattc_read_cfm*     read_cache;
    /// Number of attributes found
    uint8_t                    nb_att_fd;

    /// Client bond data information (@see enum gattc_cli_info)
    uint8_t                    cli_info;
};
#endif // (BLE_ATTS)

/// GATT controller environment variable structure.
struct gattc_env_tag
{
    /// Request operation Kernel message
    void* operation[GATTC_OP_MAX];

    #if (BLE_ATTC)
    /// Client specific environment
    struct attc_env client;
    #endif // (BLE_ATTC)

    #if (BLE_ATTS)
    /// Server specific environment
    struct atts_env server;
    #endif // (BLE_ATTS)

    /// Current MTU Size
    uint16_t mtu_size;

    /// A transaction timeout occurs, reject next attribute commands
    bool     trans_timeout;
    /// Used to know if MTU exchange already done.
    bool     mtu_exch;
};

/*
 * MACRO DEFINITIONS
 ****************************************************************************************
 */

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Created link connection parameters (from bond data) has been set, connection ready to be used.
 *
 * @note cli_info and cli_feat are considered only if bond == true
 *
 * @param[in] conidx            Connection Index
 * @param[in] bond              Used to know if link correspond to a bonded device
 * @param[in] cli_info          Client bond information (@see enum gapc_cli_info)
 * @param[in] cli_feat          Get Peer client supported features enabled in previous link (@see enum gapc_cli_feat)
 * @param[in] gatt_start_handle Peer GATT Service Start handle
 * @param[in] gatt_end_handle   Peer GATT Service End Handle
 * @param[in] svc_chg_handle    Peer Service Change value handle
 ****************************************************************************************
 */
void gattc_con_enable(uint8_t conidx, bool bond, uint8_t cli_info, uint8_t cli_feat,
                      uint16_t gatt_start_handle, uint16_t gatt_end_handle, uint16_t svc_chg_handle);

/**
 ****************************************************************************************
 * @brief Gets the negotiated MTU. This function gets the negotiated MTU.
 *
 * @param[in] conidx connection index
 *
 * @return MTU negotiated
 *
 ****************************************************************************************
 */
uint16_t gattc_get_mtu(uint8_t conidx);

/**
 ****************************************************************************************
 * @brief Sets the negotiated MTU This function stores the negotiated MTU.
 *
 * @param[in] conidx connection index
 * @param[in] mtu    negotiated MTU
 *
 * @return status   indicates if the MTU setting operation is successful
 *
 ****************************************************************************************
 */
void gattc_set_mtu(uint8_t conidx, uint16_t mtu);

/**
 ****************************************************************************************
 * @brief Informs peer device database has been updated
 *
 * @param[in] conidx connection index
 ****************************************************************************************
 */
void gattc_db_updated(uint8_t conidx);

#endif /* (BLE_GATTC) */

/// @} GATTC
#endif // GATTC_H_
