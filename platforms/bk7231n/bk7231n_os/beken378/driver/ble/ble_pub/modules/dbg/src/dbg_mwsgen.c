/**
 ****************************************************************************************
 *
 * @file dbg_mwsgen.c
 *
 * @brief MWS/WLAN Generator API.
 *
 * Copyright (C) RivieraWaves 2015
 *
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup DBGMWSGEN
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"

#if (RW_WLAN_COEX_TEST) || (RW_MWS_COEX_TEST)

#include "dbg_mwsgen.h"
#include "reg_mwsgen.h"       // MWS Event Generator register functions

#include "reg_btcore.h" // BT MWS/WLAN COEX configurations

#include "rwip.h"              // SW interface


/*
 * DEFINES & ENUMERATIONS
 ****************************************************************************************
 */

///WLAN/MWS source
enum {
    EXTERNAL    = 0,
    INTERNAL    = 1
};

///WLAN/MWS enable
enum {
    DISABLE        = 0,
    ENABLE         = 1
};

///WLAN/MWS priority level (BT with higher PTI will prioritize over WLAN/MWS)
#define MWSGEN_PTA_ARBITRATION_THRESHOLD 15


/*
 * GLOBAL VARIABLES
 ****************************************************************************************
 */

#if (RW_WLAN_COEX_TEST)
uint32_t dbg_wlcoex_scenario;
#endif // (RW_WLAN_COEX_TEST)

#if (RW_MWS_COEX_TEST)
uint32_t dbg_mwscoex_scenario;
#endif // (RW_MWS_COEX_TEST)

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */
#if (RW_MWS_COEX_TEST)
uint8_t dbg_mwscoex_scen_set(uint32_t scenario)
{
    uint8_t status = COMMON_ERROR_NO_ERROR;

    dbg_mwscoex_scenario = scenario;

    // lower word configures MWS COEX mode for BT
    switch (scenario & 0xFFFF)
    {
    case 0x0000:
        /* default mode: mws_rx blocks bt_tx / leave as configured */
        //dbg_mws_config( 0/*txmode*/, 0/*rxmode*/, 0/*txmsk*/, 1/*rxmsk*/, 0/*txfmsk*/, 0/*rxfmsk*/, 0/*scanfmsk*/, 0/*knudge*/);
        break;
    case 0x0001:
        /* customization: mws_rx frequencies block bt_tx */
        dbg_mws_config( 0/*txmode*/, 0/*rxmode*/, 0/*txmsk*/, 0/*rxmsk*/, 0/*txfmsk*/, 1/*rxfmsk*/, 0/*scanfmsk*/, 0/*knudge*/);
        break;
    case 0x0002:
        /* customization: extended mode, mws_rx & mws_tx frequencies block bt_tx & bt_rx */
        dbg_mws_config( 1/*txmode*/, 1/*rxmode*/, 0/*txmsk*/, 0/*rxmsk*/, 3/*txfmsk*/, 3/*rxfmsk*/, 0/*scanfmsk*/, 0/*knudge*/);
        break;
    case 0x0003:
        /* customization: mws_rx, mws_tx & scan frequencies block bt_tx */
        dbg_mws_config( 0/*txmode*/, 0/*rxmode*/, 0/*txmsk*/, 0/*rxmsk*/, 1/*txfmsk*/, 1/*rxfmsk*/, 1/*scanfmsk*/, 0/*knudge*/);
        break;
    case 0x0004:
        /* priority mode: with mws_rx blocks bt_tx */
        dbg_mws_config( 2/*txmode*/, 2/*rxmode*/, 0/*txmsk*/, 1/*rxmsk*/, 0/*txfmsk*/, 0/*rxfmsk*/, 0/*scanfmsk*/, 0/*knudge*/);
        break;
    case 0x0005:
        /* priority mode: with mws_rx, mws_tx & scan frequencies block bt_tx */
        dbg_mws_config( 2/*txmode*/, 2/*rxmode*/, 0/*txmsk*/, 0/*rxmsk*/, 1/*txfmsk*/, 1/*rxfmsk*/, 1/*scanfmsk*/, 0/*knudge*/);
        break;
    default:
        status = COMMON_ERROR_INVALID_HCI_PARAM;
        break;
    }

    if (COMMON_ERROR_NO_ERROR == status)
    {
        // upper word configures WLAN GEN characteristics
        switch (scenario & 0xFF000000)
        {
        case 0x00000000:
            dbg_mwsgen_init();
            break;
        case 0x01000000:
            dbg_mwsgen_config(10000 /*mws_period*/, 6250 /*mws_duty_cycle*/, 4000 /*tx_act*/, 3000 /*rx_act*/);
            mwsgen_mws_scan_frequency_setf((scenario >> 16) & 0xF);
            break;
        case 0x02000000:
            dbg_mwsgen_config(20000 /*mws_period*/, 8000 /*mws_duty_cycle*/, 2000 /*tx_act*/, 1000 /*rx_act*/);
            mwsgen_mws_scan_frequency_setf((scenario >> 16) & 0xF);
            break;
        case 0x03000000:
            mwsgen_mws_inactivity_duration_setf((scenario >> 16) & 0x1F);
            break;
        default:
            status = COMMON_ERROR_INVALID_HCI_PARAM;
        }
    }

    return status;
}

void dbg_mwsgen_init(void)
{
    dbg_mwsgen_stop();

    mwsgen_mwsgen_period_set(0);
    mwsgen_mwsgen_duty_cycle_set(0);

    mwsgen_mws_rx_delay_setf(0);
    mwsgen_mws_tx_delay_setf(0);

    mwsgen_mwsgen_tx_activity_set(0);
    mwsgen_mwsgen_rx_activity_set(0);

    mwsgen_mws_en_setf(0);
    mwsgen_wci_en_setf(0);
    mwsgen_mws_txrnd_en_setf(0);
    mwsgen_mws_rxrnd_en_setf(0);
    mwsgen_mws_tx_en_setf(0);
    mwsgen_mws_rx_en_setf(0);
    mwsgen_wlanpti_setf(0);
}

void dbg_mwsgen_config(uint32_t period, uint32_t duty_cycle, uint32_t tx_act, uint32_t rx_act)
{
    // Set MWS frame_sync period & duty cycle
    mwsgen_mwsgen_period_set(period); /* duration between frame_sync */
    mwsgen_mwsgen_duty_cycle_set(duty_cycle); /* falling edge */

    // Set uplink/downlink offsets
    mwsgen_mws_rx_delay_setf(10); /* frame_sync rising edge to downlink mws_rx */
    mwsgen_mws_tx_delay_setf(10); /* frame_sync falling edge to uplink mws_tx */

    // Set duration of mws_tx and mws_rx activities
    mwsgen_mwsgen_tx_activity_set(tx_act);
    mwsgen_mwsgen_rx_activity_set(rx_act);

    // Configure MWS_SCAN_FREQUENCY index output
    mwsgen_mws_scan_frequency_setf(0);

    // Set general control flags
   // mwsgen_mws_en_setf(ENABLE);
    mwsgen_wci_en_setf(0);
    mwsgen_mws_txrnd_en_setf(0);
    mwsgen_mws_rxrnd_en_setf(0);
    mwsgen_mws_tx_en_setf(1);
    mwsgen_mws_rx_en_setf(1);

    // Configure the arbitration threshold
    mwsgen_wlanpti_setf(MWSGEN_PTA_ARBITRATION_THRESHOLD);

    dbg_mwsgen_start();
}

void dbg_mwsgen_config_scan_frequency(uint8_t scan_freq)
{
    mwsgen_mws_scan_frequency_setf(scan_freq);
}

void dbg_mws_config(uint8_t txmode, uint8_t rxmode, uint8_t txmsk, uint8_t rxmsk, uint8_t txfmsk, uint8_t rxfmsk, uint8_t scanfmsk, uint8_t knudge)
{
    bt_coexifcntl0_knudgeinc_setf(knudge); /* knudge value for train nudging */
    bt_coexifcntl0_mwsscanfreqmsk_setf(scanfmsk); /* mws scan frequency impact on bt_tx/bt_rx (default mode = no impact) */
    bt_coexifcntl0_mwstxfrqmsk_setf(txfmsk); /* mws tx frequency impact on bt_tx/bt_rx (default mode = no impact) */
    bt_coexifcntl0_mwsrxfrqmsk_setf(rxfmsk); /* mws rx frequency impact on bt_tx/bt_rx (default mode = no impact) */
    bt_coexifcntl0_mwstxmsk_setf(txmsk); /* mws_tx impact on bt_tx/bt_rx (default mode = no impact) */
    bt_coexifcntl0_mwsrxmsk_setf(rxmsk); /* mws_rx impact on bt_tx/bt_rx (default mode = no impact) */

    // the following is shared with the wlan coexistance interface -
    bt_coexifcntl0_wlcrxpriomode_setf(rxmode); /* set default/extended/priority mode for bt_rx */
    bt_coexifcntl0_wlctxpriomode_setf(txmode); /* set default/extended/priority mode for bt_tx */
    bt_coexifcntl1_pack(8 /*wlcpRXthr*/, 8 /*wlcpTXthr*/, 20 /*wlcpduration*/, 0 /*wlcpdelay*/); /* priority mode configuration */
}

void dbg_mwsgen_start(void)
{
#if DEEP_SLEEP
    rwip_prevent_sleep_set(RW_MWS_WLAN_EVENT_GENERATOR_ACTIVE);
#endif//DEEP_SLEEP
    mwsgen_mws_en_setf(ENABLE);
}

void dbg_mwsgen_stop(void)
{
#if DEEP_SLEEP
    rwip_prevent_sleep_clear(RW_MWS_WLAN_EVENT_GENERATOR_ACTIVE);
#endif//DEEP_SLEEP
    mwsgen_mws_en_setf(DISABLE);
}

#endif //(RW_MWS_COEX_TEST)

#if (RW_WLAN_COEX_TEST)
uint8_t dbg_wlcoex_scen_set(uint32_t scenario)
{
    uint8_t status = COMMON_ERROR_NO_ERROR;

    dbg_wlcoex_scenario = scenario;

    // lower word configures WLAN COEX mode for BT
    switch (scenario & 0xFFFF)
    {
    case 0x0000:
        /* default mode: wlan_rx blocks bt_tx / leave as configured */
        //dbg_wlan_config( 0/*txmode*/, 0/*rxmode*/, 0/*txmsk*/, 1/*rxmsk*/, 0/*txthr*/, 0/*rxthr*/, 0/*pduration*/, 0/*pdelay*/);
        break;
    case 0x0001:
        /* customization: wlan_tx and wlan_rx block bt_tx */
        dbg_wlan_config( 0/*txmode*/, 0/*rxmode*/, 1/*txmsk*/, 1/*rxmsk*/, 0/*txthr*/, 0/*rxthr*/, 0/*pduration*/, 0/*pdelay*/);
        break;
    case 0x0002:
        /* customization: extended mode,and wlan_rx blocks bt_tx and bt_rx */
        dbg_wlan_config( 1/*txmode*/, 1/*rxmode*/, 0/*txmsk*/, 3/*rxmsk*/, 0/*txthr*/, 0/*rxthr*/, 0/*pduration*/, 0/*pdelay*/);
        break;
    case 0x0003:
        /* priority mode: wlan_rx blocks bt_tx, threshold at 8, maintained until bt_tx/rx EN are deasserted */
        dbg_wlan_config( 2/*txmode*/, 2/*rxmode*/, 0/*txmsk*/, 1/*rxmsk*/, 8/*txthr*/, 8/*rxthr*/, 0/*pduration*/, 0/*pdelay*/);
        break;
    case 0x0004:
        /* priority mode: wlan_rx blocks bt_tx, threshold at 4, pulse duration at 20us */
        dbg_wlan_config( 2/*txmode*/, 2/*rxmode*/, 0/*txmsk*/, 1/*rxmsk*/, 4/*txthr*/, 4/*rxthr*/, 20/*pduration*/, 0/*pdelay*/);
        break;
    case 0x0005:
        /* priority mode: wlan_rx blocks bt_tx, threshold at 10, pulse duration at 10us */
        dbg_wlan_config( 2/*txmode*/, 2/*rxmode*/, 0/*txmsk*/, 1/*rxmsk*/, 10/*txthr*/, 10/*rxthr*/, 10/*pduration*/, 0/*pdelay*/);
        break;
    default:
        status = COMMON_ERROR_INVALID_HCI_PARAM;
        break;
    }

    if (COMMON_ERROR_NO_ERROR == status)
    {
        // upper word configures WLAN GEN characteristics
        switch (scenario & 0xFFFF0000)
        {
        case 0x00000000:
            dbg_wlangen_init();
            break;
        case 0x01000000:
            dbg_wlangen_config(5000 /*wlan_period*/, 3125 /*wlan_duty_cycle*/, 1000 /*tx_act*/, 3000 /*rx_act*/);
            break;
        case 0x02000000:
            dbg_wlangen_config(8000 /*wlan_period*/, 6000 /*wlan_duty_cycle*/, 1000 /*tx_act*/, 3000 /*rx_act*/);
            break;
        default:
            status = COMMON_ERROR_INVALID_HCI_PARAM;
        }
    }

    return status;
}

void dbg_wlangen_init(void)
{
    dbg_wlangen_stop();

    mwsgen_wlan_duty_cycle_setf(0);
    mwsgen_wlan_period_setf(0);

    mwsgen_wlan_rx_delay_setf(0);
    mwsgen_wlan_tx_delay_setf(0);

    mwsgen_wlangen_tx_activity_set(0);
    mwsgen_wlangen_rx_activity_set(0);

    mwsgen_wlan_en_setf(0);
    mwsgen_wlan_source_setf(0);
    mwsgen_wlan_txrnd_en_setf(0);
    mwsgen_wlan_rxrnd_en_setf(0);
    mwsgen_wlan_tx_en_setf(0);
    mwsgen_wlan_rx_en_setf(0);
    mwsgen_wlanpti_setf(0);
}

void dbg_wlangen_config(uint32_t period, uint32_t duty_cycle, uint32_t tx_act, uint32_t rx_act)
{
    // Set WLAN period & duty cycle
    mwsgen_wlan_duty_cycle_setf(duty_cycle);
    mwsgen_wlan_period_setf(period);

    // Set wlan_tx and wlan_rx offsets
    mwsgen_wlan_rx_delay_setf(10); /* pseudo WLAN frame Rx part to wlan_rx rising edge */
    mwsgen_wlan_tx_delay_setf(10); /* pseudo WLAN frame Tx part to wlan_tx rising edge */

    // Set duration of wlan_tx and wlan_rx activities
    mwsgen_wlangen_tx_activity_set(tx_act);
    mwsgen_wlangen_rx_activity_set(rx_act);

    // Set general control flags
    //mwsgen_wlan_en_setf(ENABLE);
    mwsgen_wlan_source_setf(INTERNAL);
    mwsgen_wlan_txrnd_en_setf(0);
    mwsgen_wlan_rxrnd_en_setf(0);
    mwsgen_wlan_tx_en_setf(1);
    mwsgen_wlan_rx_en_setf(1);

    // Configure the arbitration threshold
    mwsgen_wlanpti_setf(MWSGEN_PTA_ARBITRATION_THRESHOLD);

    dbg_wlangen_start();
}

void dbg_wlan_config(uint8_t txmode, uint8_t rxmode, uint8_t txmsk, uint8_t rxmsk, uint8_t txthr, uint8_t rxthr, uint8_t pduration, uint8_t pdelay)
{
    bt_coexifcntl0_wlcrxpriomode_setf(rxmode); /* set default/extended/priority mode for bt_rx */
    bt_coexifcntl0_wlctxpriomode_setf(txmode); /* set default/extended/priority mode for bt_tx */
    bt_coexifcntl0_wlantxmsk_setf(txmsk); /* wlan_tx impact on bt_tx/bt_rx (default mode = no impact) */
    bt_coexifcntl0_wlanrxmsk_setf(rxmsk); /* wlan_rx impact on bt_tx/bt_rx (default mode = can stop bt_tx) */
    bt_coexifcntl1_pack(rxthr /*wlcpRXthr*/, txthr /*wlcpTXthr*/, pduration /*wlcpduration*/, pdelay /*wlcpdelay*/); /* priority mode configuration */
}

void dbg_wlangen_start(void)
{
#if DEEP_SLEEP
    rwip_prevent_sleep_set(RW_MWS_WLAN_EVENT_GENERATOR_ACTIVE);
#endif//DEEP_SLEEP
    mwsgen_wlan_en_setf(ENABLE);
}

void dbg_wlangen_stop(void)
{
#if DEEP_SLEEP
    rwip_prevent_sleep_clear(RW_MWS_WLAN_EVENT_GENERATOR_ACTIVE);
#endif//DEEP_SLEEP
    mwsgen_wlan_en_setf(DISABLE);
}
#endif // RW_WLAN_COEX_TEST

#endif // (RW_WLAN_COEX_TEST) || (RW_WLAN_COEX_TEST)

/// @} DBGMWSGEN
