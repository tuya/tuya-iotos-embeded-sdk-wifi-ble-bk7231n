/**
 ****************************************************************************************
 *
 * @file llctrl.h
 *
 * @brief Main API file for the Link Layer Controller
 *
 * Copyright (C) RivieraWaves 2009-2016
 *
 ****************************************************************************************
 */

#ifndef LLC_H_
#define LLC_H_

/**
 ****************************************************************************************
 * @defgroup LLC Link Layer Controller
 * @ingroup ROOT
 * @brief BLE Lower Layers
 *
 * The CONTROLLER contains the modules allowing the link establishment, maintenance and management.
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"
#if(BLE_CENTRAL || BLE_PERIPHERAL)
#include "kernel_task.h"      // kernel task definitions
#include "common_bt.h"
#include "common_llcp.h"

/*
 * DEFINES
 ****************************************************************************************
 */


/*
 * ENUMERATION DEFINITIONS
 ****************************************************************************************
 */

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// API structure to be set by LLM and used by the LLC for each new link
struct llc_init_parameters
{
    /// Access address
    struct access_addr  aa;

    /// CRC init
    struct crc_init     crcinit;

    /// Window size (in units of 1,25 ms, i.e. 2 slots)
    uint8_t             winsize;

    /// Window offset (in units of 1,25 ms, i.e. 2 slots)
    uint16_t            winoffset;

    /// Interval (in units of 1,25 ms, i.e. 2 slots)
    uint16_t            interval;

    /// Latency
    uint16_t            latency;

    /// Timeout (in units of 10 ms, i.e. 16 slots)
    uint16_t            timeout;

    /// Channel mapping
    struct le_chnl_map  chm;

    /// Hopping increment (random value in the range of 5 to 16)
    uint8_t             hop_inc;

    /**
     * Master Sleep clock accuracy (only for slave)
     * 0 251 ppm to 500 ppm
     * 1 151 ppm to 250 ppm
     * 2 101 ppm to 150 ppm
     * 3 76 ppm to 100 ppm
     * 4 51 ppm to 75 ppm
     * 5 31 ppm to 50 ppm
     * 6 21 ppm to 30 ppm
     * 7 0 ppm to 20 ppm
     */
    uint8_t master_sca;

    /// CONNECT_REQ packet fine timer count (in us)
    uint16_t fine_cnt_rxsync;

    /// CONNECT_REQ packet base timer count (timestamp, in slots)
    uint32_t base_cnt_rxsync;

    /// First anchor point timestamp (in slots, master only)
    uint32_t first_anchor_ts;

    /// Connection transmit/receive rate (@see enum lld_rate)
    uint8_t rate;

    /// Role (0: Master | 1: Slave)
    uint8_t role;

    /// Indicates whether channel selection algorithm #2 will be used or not
    bool ch_sel_2;

    /// Indicates whether the AUX_CONNECT_REQ PDU was used or not (true for AUX_CONNECT_REQ, false for CONNECT_IND)
    bool aux_connect_req;

    // Default Data Length parameters
    /// Suggested value for the Controller's maximum transmitted number of payload octets
    uint16_t   suggested_max_tx_octets;
    /// Suggested value for the Controller's maximum packet transmission time (in us)
    uint16_t   suggested_max_tx_time;

    // Default PHY parameters
    /// Default TX preferred PHY to use (@see enum le_phy_mask)
    uint8_t    tx_phys;
    /// Default RX preferred PHY to use (@see enum le_phy_mask)
    uint8_t    rx_phys;

    // Default PAST parameters
    /// Mode (@see enum per_adv_sync_info_rec_mode)
    uint8_t   past_mode;
    /// The number of periodic advertising packets that can be skipped after a successful receive
    uint16_t  past_skip;
    /// Sync timeout (Time=N*10ms)
    uint16_t  past_sync_to;
    /// CTE type (@see enum sync_cte_type)
    uint8_t   past_cte_type;
};


/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */
/**
 ****************************************************************************************
 * @brief Initialization/Reset of the BLE LLC task
 *
 * This function initializes the LLC task, as well as the environment of the LLC
 *
 * @param[in] init_type  Type of initialization (@see enum rwip_init_type)
 ****************************************************************************************
 */
void llc_init(uint8_t init_type);


/**
 ****************************************************************************************
 * @brief Start an initialize a new LLC environment
 *
 * @param[in] link_id             link identifier.
 * @param[in] con_params          All parameters needed to start the LLC
 * @param[in] pref                Link preferences (set by host)
 *
 * @return If the environment allocation and initialization has been done or not.
 ****************************************************************************************
 */
uint8_t llc_start(uint8_t link_id, struct llc_init_parameters *con_params);

/**
 ****************************************************************************************
 * @brief Handles a planner request to move the activity
 *
 * @param[in] id             sch_plan identifier.
 ****************************************************************************************
 */
void llc_con_move_cbk(uint16_t id);

/**
 ****************************************************************************************
 *  @brief Modify sleep clock accuracy
 *
 * @param[in] link_id        Link identifier
 * @param[in] action         Switch to more or less accurate clock (@see enum clk_acc_action)
 ****************************************************************************************
 */
void llc_clk_acc_modify(uint8_t link_id, uint8_t action);

#endif// (BLE_CENTRAL || BLE_PERIPHERAL)

/// @} LLC

#endif // LLC_H_
