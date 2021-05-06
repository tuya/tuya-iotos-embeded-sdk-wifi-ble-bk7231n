/**
 ****************************************************************************************
 *
 * @file llm.h
 *
 * @brief Main API file for the Link Layer manager
 *
 * Copyright (C) RivieraWaves 2009-2015
 *
 ****************************************************************************************
 */

#ifndef LLM_H_
#define LLM_H_

/**
 ****************************************************************************************
 * @defgroup LLM Link Layer Manager
 * @ingroup ROOT
 * @brief BLE Lower Layers
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_task.h"      // Task definitions
#include "common_bt.h"          // BLE standard definitions

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// Parameters for syncing to a periodic advertiser
struct llm_per_adv_sync_params
{
    /// Activity identifier
    uint8_t act_id;

    /// SyncInfo
    struct sync_info syncinfo;

    /// The clock value of the reference time (in half-slot)
    uint32_t base_cnt;

    /// The fine timer value the reference time (in half-us)
    uint16_t fine_cnt;

    /// ID (provided by the Host)
    uint16_t       id;

    /// The bit rate of periodic advertising (@see enum lld_rate)
    uint8_t rate;

    /// Advertising SID
    uint8_t adv_sid;

    /// Advertising address type (0: Public / 1: Random)
    uint8_t adv_addr_type;

    /// Advertiser address
    struct bd_addr adv_addr;

    /// Advertiser RPA (unresolved RPA if advertiser address is a resolved identity address, 0 otherwise)
    struct bd_addr adv_rpa;

    /// max Skip after receive
    uint16_t skip;

    /// Sync timeout (Time=N*10ms)
    uint16_t sync_to;

    /// Additional drift to consider on given time reference (in half-us)
    uint16_t add_drift;

    /// Advertising report initial state (enable/disable)
    bool adv_rep_en;

    /// Specifies whether to only synchronize to periodic advertising with certain types of CTE (@see enum sync_cte_type)
    uint8_t sync_cte_type;
};


/*
 * MESSAGES
 ****************************************************************************************
 */

/// Message API of the LLM task
/*@TRACE
 * llm_pub_key_gen_ind  = ecc_result_ind
 * llm_dh_key_gen_ind   = ecc_result_ind
 * llm_encrypt_ind      = ble_aes_func_res_ind*/
enum llm_msg_id
{
    LLM_MSG_ID_FIRST = TASK_BLE_FIRST_MSG(TASK_BLE_ID_LLM),//!< LLM_MSG_ID_FIRST

    #if (BLE_OBSERVER)
    /// Send ACAD Data information
    LLM_ACAD_DATA_IND,                             //!< LLM_ACAD_DATA_IND
    #endif // (BLE_OBSERVER)

    /*
     * ************** Msg LLM->LLC****************
     */
    #if (BLE_CENTRAL || BLE_PERIPHERAL)
    /// Indicate that the channel map has to be updated on link connection
    LLM_CH_MAP_UPDATE_IND,                         //!< LLM_CH_MAP_UPDATE_IND
    #endif //(BLE_CENTRAL || BLE_PERIPHERAL)

    /*
     * ************** Msg LLM->LLM****************
     */
    LLM_SCAN_PERIOD_TO,                            //!< LLM_SCAN_PERIOD_TO
    LLM_CH_MAP_TO,                                 //!< LLM_CH_MAP_TO
    #if (BLE_CENTRAL || BLE_BROADCASTER)
    LLM_NEW_HOST_CLASS_TO,
    #endif //(BLE_CENTRAL || BLE_BROADCASTER)
    LLM_RPA_RENEW_TO,                              //!< LLM_RPA_RENEW_TO
    /// Inform that encryption has been performed
    LLM_ENCRYPT_IND,                               //!< LLM_ENCRYPT_IND

    LLM_PUB_KEY_GEN_IND,                           //!< LLM_PUB_KEY_GEN_IND
    LLM_DH_KEY_GEN_IND,                            //!< LLM_DH_KEY_GEN_IND
};

/// LLM Encryption Request parameters structure
/*@TRACE*/
struct llm_encrypt_req
{
    ///Long term key structure
    struct ltk     key;
    ///Pointer to buffer with plain data to encrypt - 16 bytes
    uint8_t        plain_data[ENC_DATA_LEN];
};

/// LLM Encryption indication structure
/*@TRACE*/
struct llm_encrypt_rsp
{
    /// Status of the encryption
    uint8_t status;
    ///Encrypted data
    uint8_t encrypted_data[ENC_DATA_LEN];
};


/// AES function execution result
/*@TRACE*/
struct llm_encrypt_ind
{
    /// Status of AES execution
    uint8_t status;
    /// Result of the
    uint8_t result[KEY_LEN];
};


#if (BLE_OBSERVER)
/// LLM ACAD data indication structure
/*@TRACE*/
struct llm_acad_data_ind
{
    /// Sync handle
    uint16_t sync_handle;
    /// Status the indication - use to detect loss of periodic sync
    uint8_t  status;
    /// ADV Data Type
    uint8_t  ad_type;
    /// Reference event counter of the periodic advertiser when report received
    uint16_t ref_evt_cnt;

    /// length of the data field
    uint8_t  data_len;
    /// ACAD Data
    uint8_t  data[__ARRAY_EMPTY];
};
#endif // (BLE_OBSERVER)

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialization of the BLE LLM task
 *
 * This function initializes the the LLM task, as well as the environment of the LLM
 *
 * @param[in] init_type  Type of initialization (@see enum rwip_init_type)
 ****************************************************************************************
 */
void llm_init(uint8_t init_type);

#if (BLE_CENTRAL || BLE_PERIPHERAL)
/**
 ****************************************************************************************
 * @brief This function handles LE link disconnection.
 *
 * @param[in] link_id            Link Identifier
 ****************************************************************************************
 */
void llm_link_disc(uint8_t link_id);

/**
 ****************************************************************************************
 * @brief Indicate activity requires accurate clock
 *
 * @param[in] act_id      Activity ID
 * @param[in] clk_acc     True if activity requires accurate clock
 ****************************************************************************************
 */
void llm_clk_acc_set(uint8_t act_id, bool clk_acc);
#endif // (BLE_CENTRAL || BLE_PERIPHERAL)

/**
 ****************************************************************************************
 * @brief This function is used to get the master channel map
 *
 * @return Pointer to master channel map
 ****************************************************************************************
 */
struct le_chnl_map* llm_master_ch_map_get(void);

/**
 ****************************************************************************************
 * @brief This function is used to check if an event is unmasked/masked
 *
 * @param[in]  event_id  Bit position of the event in the mask (see standard specification part II.E.7.8.1)
 *
 * @return True: event allowed | False: event masked
 ****************************************************************************************
 */
bool llm_le_evt_mask_check(uint8_t event_id);

/**
 ****************************************************************************************
 * @brief Retrieve LE Local Supported features
 *
 * @return Local feature mask
 ****************************************************************************************
 */
const struct le_features* llm_le_features_get(void);

/**
 ****************************************************************************************
 * @brief This function is used to get the RF Tx path compensation value
 *
 * @return RF Tx path compensation value (Range: -128.0 dB <= N <= 128.0 dB, Units: 0.1 dB)
 ****************************************************************************************
 */
int16_t llm_tx_path_comp_get(void);

/**
 ****************************************************************************************
 * @brief This function is used to get the RF Rx path compensation value
 *
 * @return RF Rx path compensation value (Range: -128.0 dB <= N <= 128.0 dB, Units: 0.1 dB)
 ****************************************************************************************
 */
int16_t llm_rx_path_comp_get(void);

/**
 ****************************************************************************************
 * @brief This function returns a pointer to the interval element of the link_id provided.
 *
 * @param[in] link_id            Link Identifier
 *
 * @return Pointer to the associated interval element
 ****************************************************************************************
 */
struct sch_plan_elt_tag *llm_plan_elt_get(uint8_t link_id);

/**
 ****************************************************************************************
 * @brief Get available activity index
 *
 * @param[out] act_id            Activity Identifier allocated
 *
 * @return  - COMMON_ERROR_NO_ERROR:         if succeed
 *          - COMMON_ERROR_CON_LIMIT_EXCEED: if no activity available
 ****************************************************************************************
 */
uint8_t llm_activity_free_get(uint8_t* act_id);
/**
 ****************************************************************************************
 * @brief Free activity handle related to the activity
 *
 * @param[in] act_id            Activity Identifier
 ****************************************************************************************
 */
void llm_activity_free_set(uint8_t act_id);

/**
 ****************************************************************************************
 * @brief Get advertising activity identifiers
 *
 * @param[in]  adv_hdl           Advertising handle
 * @param[out] ext_adv_id        Extended advertising ID
 * @param[out] per_adv_id        Periodic advertising ID
 *
 * @return  - COMMON_ERROR_NO_ERROR:               if succeed
 *          - COMMON_ERROR_UNKNOWN_ADVERTISING_ID: if no advertising set exists
 *          - COMMON_ERROR_COMMAND_DISALLOWED:     if periodic advertising not in progress
 ****************************************************************************************
 */
uint8_t llm_adv_act_id_get(uint8_t adv_hdl, uint8_t* ext_adv_id, uint8_t* per_adv_id);

/**
 ****************************************************************************************
 * @brief Get periodic advertising train identifiers
 *
 * @param[in]  act_id        Activity ID of extended advertising
 * @param[out] sid           Advertising SID
 * @param[out] atype         Advertising address type (0: public, 1: random)
 * @param[out] adva          Advertising address (RPA if address is resolvable)
 *
 * @return  - COMMON_ERROR_NO_ERROR:               if succeed
 *          - COMMON_ERROR_COMMAND_DISALLOWED:     if periodic advertising not in progress
 ****************************************************************************************
 */
uint8_t llm_adv_set_id_get(uint8_t act_id, uint8_t* sid, uint8_t* atype, struct bd_addr* adva);

#if (BLE_ISO_MODE_0)
/**
 ****************************************************************************************
 * @brief Get if Activity is an active link
 *
 * @param[out] act_id            Activity Identifier
 *
 * @return  true if in connected state, false else
 ****************************************************************************************
 */
bool llm_iso_link_active(uint8_t act_id);
#endif // (BLE_ISO_MODE_0)


/**
 ****************************************************************************************
 * @brief Get if Activity is ongoing
 *
 * @return  true if active
 ****************************************************************************************
 */
bool llm_activity_ongoing_check(void);

/**
 ****************************************************************************************
 * @brief Attach a task with a periodic scanner to receive specific AD Type data
 *
 * @param[in] sync_act_id     Sync activity ID
 * @param[in] ad_type         AD Type to retrieve in ACAD data
 * @param[in] task            Task that expect the LLM_ACAD_DATA_IND message
 *
 * @return COMMON_ERROR_NO_ERROR if the periodic sync is found
 ****************************************************************************************
 */
uint8_t llm_scan_sync_acad_attach(uint8_t sync_act_id, uint8_t ad_type, uint16_t task);

/**
 ****************************************************************************************
 * @brief Get periodic sync information
 *
 * @param[in]  sync_act_id       Sync activity ID
 * @param[out] sid               Extended advertising set identifier
 * @param[out] atype             Extended advertiser address type (0: public | 1: random)
 * @param[out] adva              Extended advertiser address (for resolvable addresses, this is the unresolved version)
 *
 * @return  - COMMON_ERROR_NO_ERROR:               if succeed
 *          - COMMON_ERROR_UNKNOWN_ADVERTISING_ID: if the sync activity does not exist
 ****************************************************************************************
 */
uint8_t llm_scan_sync_info_get(uint8_t sync_act_id, uint8_t* sid, uint8_t* atype, struct bd_addr* adva);

/**
 ****************************************************************************************
 * @brief Synchronize to a periodic advertising
 *
 * param[in] params    Input parameters (@see llm_per_adv_sync_params)
 *
 * @return 0 if synchronization attempt is started, >0 if issue
 ****************************************************************************************
 */
uint8_t llm_per_adv_sync(struct llm_per_adv_sync_params * params);

/**
 ****************************************************************************************
 * @brief Get pointer of the RAL table in RAM
 *
 * @return    pointer of the RAL table in RAM
 ****************************************************************************************
 */
struct ral_entry* llm_ral_get(void);

/// @} LLM

#endif // LLM_H_
