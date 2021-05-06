/**
****************************************************************************************
*
* @file lld_int.h
*
* @brief LLD Internal API
*
* Copyright (C) RivieraWaves 2009-2016
*
****************************************************************************************
*/

#ifndef LLD_INT_H_
#define LLD_INT_H_

/**
 ****************************************************************************************
 * @addtogroup LLD
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"    // stack configuration

#include <stdint.h>         // integer
#include <stdbool.h>        // boolean

#include "lld.h"


/*
 * MACROS
 ****************************************************************************************
 */


/*
 * CONSTANT DEFINITIONS
 ****************************************************************************************
 */

/// Allocate an event with an associated data structure
#define LLD_ALLOC_EVT(data_struct)           (struct data_struct *) kernel_malloc(sizeof(struct data_struct), KERNEL_MEM_ENV);

/// Initialize memory of an event and an associated data structure
#define LLD_INIT_EVT(evt, data_struct)       memset(evt , 0, sizeof(struct data_struct));

#if 0 //(RW_BLE_WLAN_COEX) || (RW_BLE_MWS_COEX)
/// Coexistence disabled
#define BLECOEX_DISABLED        0
/// Coexistence enabled
#define BLECOEX_ENABLED         1

/// Coexistence Definitions
#define BLEMPRIO_CONREQ     0
#define BLEMPRIO_LLCP        1
#define BLEMPRIO_DATA       2
#define BLEMPRIO_INITSC     3
#define BLEMPRIO_ACTSC      4
#define BLEMPRIO_CONADV     5
#define BLEMPRIO_NCONADV    6
#define BLEMPRIO_PASSC      7
#endif // (RW_BLE_WLAN_COEX) || (RW_BLE_MWS_COEX)

/// Undefined BT clock value
#define LLD_CLOCK_UNDEF       0xFFFFFFFF

/// Max fine counter value
#define LLD_FINECNT_MAX             (HALF_SLOT_SIZE - 1)

/// Adv packet error masking
#define LLD_ADV_ERR_MASK        (EM_BLE_SYNC_ERR_BIT|EM_BLE_LEN_ERR_BIT|EM_BLE_CRC_ERR_BIT|EM_BLE_PRIV_ERR_BIT|EM_BLE_TYPE_ERR_BIT)

/// ADV_INT (time between two ADV_XXX packets) in us
#define BLE_ADV_INT_US               (729)

/// Minimum scan window size when advertising using legacy PDUs in us
#define BLE_ADV_MIN_SCAN_WIN_SIZE_US (40)

/// Calculate AUX OFFSET in usecs from AuxPtr AUX Offset and Offset Units fields
#define AUX_OFFSET_USECS(aux_offset, offset_units) ((aux_offset) * ((offset_units)?300:30))

/// AUX OFFSET Offset Units in usecs
#define AUX_OFFSET_UNIT_USECS(offset_units) ((offset_units)?300:30)

/// Calculate drift in ppm from local drift and AuxPtr CA field
#define AUX_DRIFT_PPM(local_drift, ca) ((local_drift) + ((ca)?50:500))

/// Margin for event time reservation (in half-us)
#define BLE_RESERVATION_TIME_MARGIN_HUS    (2 * IP_PREFETCHABORT_TIME_US)

/// AUX_PTR Thresholds, below which HW manages AuxPtr automatically in same event (granularity 16us, 0 disabled)
#define LLD_TX_AUX_PTR_THR   (0xFF) // 255 x 16 = 4080us (max value)
#define LLD_RX_AUX_PTR_THR   (150) // 150 x 16 = 2520us (optimized for SW offload)

/// Maximum supported CTE IQ samples - CTE RX descriptors dimensioned REG_EM_BLE_RX_CTE_DESC_SIZE to accommodate maximum
// An IQ sample is taken each usec during the reference period (8us) and one each sample slot thereafter.
// This results in 8 reference IQ samples, 1 to 37 IQ samples with 2 us slots, and 2 to 74 IQ samples with 1 us slots, meaning 9 to 82 samples in total.
#define LLD_MAX_CTE_IQ_SAMPLES   (82)

/// Hoping mode
enum lld_hop_mode
{
    /// Frequency Hopping Scheme #1
    LLD_HOP_MODE_CHAN_SEL_1         = 0,
    /// Frequency Hopping Scheme #2
    LLD_HOP_MODE_CHAN_SEL_2         = 1,
    /// Primary Advertising Channel randomized order mode
    LLD_HOP_MODE_RAND_PRIM_ADV_CH   = 3,
};

/*
 * STRUCTURE DEFINITIONS
 ****************************************************************************************
 */

/// LLD environment structure
struct lld_env_tag
{
    /// Channel assessment data
    struct lld_ch_assess_data ch_assess;

    /// ADV_SIDs bit field for PAL entries
    uint16_t                  adv_sids[BLE_WHITELIST_MAX];

    /// Maximum value of the local clock drift (in ppm)
    uint16_t                  local_drift;

    /// Local sleep clock accuracy (see @enum SCA)
    uint16_t                  local_sca;

    /// Current RX descriptor index [0:N-1]
    uint8_t                  curr_rxdesc_index;

    /// LE Coded PHY 500 Kbps selection (false: 125 Kbps, true: 500 Kbps)
    bool                      le_coded_phy_500;

    /// Connectionless channel map lookup table (more time-efficient than parsing a bitmap every time)
    uint8_t ch_map_tab[DATA_CHANNEL_NB];

    /// Number of used channels in the connectionless channel map
    uint8_t nb_used_ch;
};

/// LLD Scheduling information for next AUX channel
struct lld_calc_aux_rx_out
{
    /// Timestamp (in half-slots) and delay (in half-us) from slot boundary for AUX offload channel receive
    rwip_time_t time;

    /// Sync window size for the AUX offload channel (us)
    uint32_t sync_win_size_us;

    /// Channel index for AUX offload channel
    uint8_t ch_idx;

    /// PHY rate for AUX offload channel (@see enum lld_rate)
    uint8_t rate;
};

/*
 * VARIABLE DECLARATION
 *****************************************************************************************
 */

/// Table indicating the expected sync position depending on the rate used
extern uint16_t lld_exp_sync_pos_tab[];

/// LLD environment variable
extern struct lld_env_tag lld_env;

/*
 * FUNCTIONS DECLARATION
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Free current RX descriptor
 ****************************************************************************************
 */
void lld_rxdesc_free(void);

/**
 ****************************************************************************************
 * @brief Check if the current RX descriptor is consumed by a given activity
 *
 * @param[in]   label   representing the activity (as indicated in the CS)
 *
 * @return True if current RX descriptor is consumed by the activity identified by label
 ****************************************************************************************
 */
bool lld_rxdesc_check(uint8_t label);

/**
 ****************************************************************************************
 * @brief Assess a RF channel
 *
 * @param[in]  channel       Channel index
 * @param[in]  rx_ok         True: reception is correct | False: reception is corrupted
 * @param[in]  timestamp     Timestamp of the assessment
 *
 * @return status            0: success | 1-255: error
 ****************************************************************************************
 */
void lld_channel_assess(uint8_t channel, bool rx_ok, uint32_t timestamp);

/**
 ****************************************************************************************
 * @brief Search if a device is in resolving address list
 *
 * @param[in] bd_addr      BD Address
 * @param[in] bd_addr_type BD Address type (0: public | 1: random)
 *
 * @return position of the device in the list (Max value if not found)
 ****************************************************************************************
 */
uint8_t lld_ral_search(const struct bd_addr *bd_addr, uint8_t bd_addr_type);

#if (BLE_BROADCASTER)
/**
 ****************************************************************************************
 * @brief Initialize advertising driver
 * @param[in] init_type  Type of initialization (@see enum rwip_init_type)
 ****************************************************************************************
 */
void lld_adv_init(uint8_t init_type);

/**
 ****************************************************************************************
 * @brief Initialize periodic advertising driver
 * @param[in] init_type  Type of initialization (@see enum rwip_init_type)
 ****************************************************************************************
 */
void lld_per_adv_init(uint8_t init_type);

/**
 ****************************************************************************************
 * @brief Get sync info from a periodic advertiser
 *
 * @param[in]  act_id        Activity ID
 * @param[out] sync_ind_ts   Timestamp of the AUX_SYNC_IND packet
 * @param[out] pa_evt_cnt    Corresponding paEventCounter value
 * @param[out] map           Corresponding channel map
 *
 * @return status            0: success | 1-255: error
 ****************************************************************************************
 */
uint8_t lld_per_adv_sync_info_get(uint8_t act_id, uint32_t* sync_ind_ts, uint16_t* pa_evt_cnt, struct le_chnl_map *map);

/**
 ****************************************************************************************
 * @brief Get initialization info from a periodic advertiser
 *
 * @param[in]  act_id        Activity ID
 * @param[out] aa            Periodic advertising Access Address
 * @param[out] crcinit       Periodic advertising CRC Initialization value
 * @param[out] chm           Periodic advertising Channel Map
 *
 * @return status            0: success | 1-255: error
 ****************************************************************************************
 */
uint8_t lld_per_adv_init_info_get(uint8_t act_id, struct access_addr* aa, struct crc_init* crcinit, struct le_chnl_map* chm);

/**
 ****************************************************************************************
 * @brief Select an auxiliary channel index
 *
 * @return ch_idx            Selected channel index
 ****************************************************************************************
 */
uint8_t lld_ch_idx_get(void);
#endif // (BLE_BROADCASTER)

/**
 ****************************************************************************************
 * @brief Generating the Access Address for a new Link.
 *
 * The function will generate the following 4 bytes:
 * |Byte 3           | Byte 2           |  Byte 1            | Byte 0            |
 * |CT1 | PRAND_4MSB | CT2 | PRAND_4LSB | CT2 | INCRAND_4MSB | CT2 | INCRAND_4LSB|
 *
 * The AA shall be a random 32-bit value, generated by the device in the Initiating State.
 * It shall have no more than six consecutive zeros or ones.
 * It shall not be the advertising channel packets’ Access Address.
 * It shall not be a sequence that differs from the advertising channel packets’ Access Address by only one bit.
 * It shall not have all four octets equal.
 * It shall have no more than 24 transitions.
 * It shall have a minimum of two transitions in the most significant six bits
 *
 * For each new generated AA,
 *  - PRAND is a pseudo random 1 byte number.
 *  - INCRAND is an initially generated random number, increased by 1 for each AA
 *  - CT2 is toggled between the two values
 *  - CT1 is again toggled between the 3 possible values
 *
 * Considering the formats of CT1 and CT2, the rules for AA generation are respected:
 *  - since CT2 is put between bytes, and has two equal bits on each half,
 *    it ensures no more than 6 consecutive bits inside the address. CT1 also.
 *
 *  - CT2 and CT1 ensure that the newly generated address is different and at least 1 bit
 *    different from the advertising access address
 *
 *  - since CT1 != CT2 , the four bytes of the AA can never be equal
 *
 *  - Since in a 32 bit number there are 31 transitions, and the neighboring equal bits
 *    in CT1 and CT2 represent 7 transitions, there can never be more than 31-7=24
 *    transitions in the AA
 *
 *  - The format of CT1 ensures at least two transitions in the most significant 6 bits.
 *
 * BT5.0 Requirements:
 *
 * 1/ On an implementation that also supports the LE Coded PHY the Access Address shall also
 * meet the following requirements:
 * It shall have at least three ones in the least significant 8 bits.
 * It shall have no more than eleven transitions in the least significant 16 bits.
 * - CT2 format selected to satisfy these requirements.
 *
 * 2/ It shall not be the Access Address for any enabled periodic advertising.
 * - The act_id is now passed as an arguemnt to AA generation to ensure unique values.
 *
 * @param acc_addr Pointer to the 4 byte array space for the AA.
 *
 *
 *****************************************************************************************
 */
void lld_aa_gen(uint8_t *acc_addr, uint8_t act_id);

/**
 ****************************************************************************************
 * @brief Calculate an AUX RX activity
 *
 * @param[in]  ux_rx_out   Scheduling information for next AUX channel
 * @param[out] index_pkt   current RX descriptor index
 * @param[out] aux_data    received Aux data
 *
 * @return offload         true: offload. false: no offload
 ****************************************************************************************
 */
bool lld_calc_aux_rx(struct lld_calc_aux_rx_out* aux_rx_out, uint8_t index_pkt, uint32_t aux_data);


#if (BLE_OBSERVER)
/**
 ****************************************************************************************
 * @brief Initialize scanning driver
 * @param[in] init_type  Type of initialization (@see enum rwip_init_type)
 ****************************************************************************************
 */

void lld_scan_init(uint8_t init_type);

/**
****************************************************************************************
* @brief Initialize periodic advertising Rx driver
* @param[in] init_type  Type of initialization (@see enum rwip_init_type)
****************************************************************************************
*/
void lld_sync_init(uint8_t init_type);

#endif // (BLE_OBSERVER)

#if (BLE_CENTRAL)
/**
 ****************************************************************************************
 * @brief Initialize initiating driver
 * @param[in] init_type  Type of initialization (@see enum rwip_init_type)
 ****************************************************************************************
 */
void lld_init_init(uint8_t init_type);
#endif // (BLE_CENTRAL)

#if (BLE_CENTRAL || BLE_PERIPHERAL)
/**
 ****************************************************************************************
 * @brief Initialize connection driver
 * @param[in] init_type  Type of initialization (@see enum rwip_init_type)
 ****************************************************************************************
 */
void lld_con_init(uint8_t init_type);

#endif // (BLE_CENTRAL || BLE_PERIPHERAL)

/**
 ****************************************************************************************
 * @brief Initialize test driver
 * @param[in] init_type  Type of initialization (@see enum rwip_init_type)
 ****************************************************************************************
 */
void lld_test_init(uint8_t init_type);


/// @} LLD
#endif // LLD_INT_H_
