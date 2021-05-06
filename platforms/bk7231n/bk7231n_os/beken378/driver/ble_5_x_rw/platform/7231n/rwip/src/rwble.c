/**
****************************************************************************************
*
* @file rwble.c
*
* @brief RWBLE core interrupt handler
*
* Copyright (C) RivieraWaves 2009-2016
*
****************************************************************************************
*/

/**
 ****************************************************************************************
 * @addtogroup ROOT
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "rwip_config.h"      // stack configuration

#include <string.h>           // for mem* functions
#include "common_version.h"
#include "common_math.h"
#include "rwble.h"            // BLE API definition
#include "rwip.h"             // stack main module

#include "ble_util_buf.h"     // BLE EM buffer management
#include "lld.h"              // link layer driver definition
#include "llctrl.h"              // link layer controller definition
#include "llm.h"              // link layer manager definition

#if (BLE_ISO_PRESENT)
#include "lli.h"              // Link Layer ISO definition
#endif // (BLE_ISO_PRESENT)

#include "kernel_event.h"         // kernel event definition

#include "sch_arb.h"          // Scheduling Arbiter
#include "sch_prog.h"         // Scheduling Programmer

#include "dbg.h"              // debug definitions

#include "ble_reg_blecore.h"      // BLE Core registers


/*
 * DEFINES
 ****************************************************************************************
 */

/*
 * STRUCTURE DEFINITIONS
 ****************************************************************************************
 */


/*
 * LOCAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */


/*
 * GLOBAL VARIABLES
 ****************************************************************************************
 */


/*
 * LOCAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

/*
 * EXPORTED FUNCTION DEFINITIONS
 ****************************************************************************************
 */

void rwble_init(uint8_t init_type)
{
    if(!init_type)
    {
        uint32_t seed;
        rwip_time_t current_time = rwip_time_get();

        seed =  current_time.hs;
        seed += current_time.hus;

        //Init the random seed
        common_random_init(seed);
    }

    // Initialize buffer management system
    ble_util_buf_init(init_type);

    #if (BLE_ISO_PRESENT)
    // Initialize ISO descriptor management system
    ble_util_isodesc_init(init_type);
    #endif // (BLE_ISO_PRESENT)

    // LLD block does nothing at IP INIT state
    if (init_type)
    {
        // Initialize the Link Layer Driver
        lld_init(init_type);
    }

    #if(BLE_CENTRAL || BLE_PERIPHERAL)
    // Initialize the Link Layer Controller
    llc_init(init_type);
    #endif // (BLE_CENTRAL || BLE_PERIPHERAL)

    // Initialize the Link Layer Manager
    llm_init(init_type);

    #if (BLE_ISO_PRESENT)
    // Initialize the Link Layer ISO
    lli_init(init_type);
    #endif // (BLE_ISO_PRESENT)
}

#if BT_DUAL_MODE
bool rwble_activity_ongoing_check(void)
{
    // check that a BLE activity is ongoing (advertising, scan, initiating, connection)
    return llm_activity_ongoing_check();
}
#endif //BT_DUAL_MODE

__BLEIRQ void rwble_isr(void)
{
    DBG_SWDIAG(ISR, BLE, 1);

    // Loop until no more interrupts have to be handled
    while (1)
    {
        // Check BLE interrupt status and call the appropriate handlers
        uint32_t irq_stat      = ble_intstat0_get();

        if (irq_stat == 0)
            break;

        // Error interrupt
        if (irq_stat & BLE_ERRORINTSTAT_BIT)
        {
            // Clear the interrupt
            ble_intack0_errorintack_clearf(1);

            BLE_ASSERT_INFO(0, ble_errortypestat_get(), (ble_errortypestat_get()>>16));
        }
    }

    DBG_SWDIAG(ISR, BLE, 0);
}

///@} RWBTINT
