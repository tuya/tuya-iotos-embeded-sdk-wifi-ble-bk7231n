/**
****************************************************************************************
*
* @file dbg_swdiag.c
*
* @brief SW profiling module
*
* Copyright (C) RivieraWaves 2009-2015
*
****************************************************************************************
*/

/**
 ****************************************************************************************
 * @addtogroup DBGSWDIAG
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <stdint.h>                  // standard integer definitions
#include "dbg_swdiag.h"              // SW profiling definition
#include "common_utils.h"                // common utility declaration

#if (RW_SWDIAG)

#if (NVDS_SUPPORT)
#include "nvds.h"         // NVDS definitions
#endif // NVDS_SUPPORT

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/// Allocate SW profiles to HW banks
static uint8_t hw_to_sw[DBG_SWDIAG_NB_HW_BANKS];

/// Allocate HW banks to SW profiles
uint8_t sw_to_hw[DBG_SWDIAG_NB_PROFILES];


/*
 * LOCAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

/**
 ******************************************************************************************
 * @brief Update SW profile.
 *
 * This function reserves the allocation of SW profiles to HW bank. It allocates to each
 * SW profile a HW bank, depending on the HwToSW allocation table.
 ******************************************************************************************
 */
static void dbg_swdiag_update(void)
{
    uint8_t i;

    GLOBAL_INT_DIS();

    // Clear sw_to_hw table
    for(i = 0 ; i < DBG_SWDIAG_NB_PROFILES ; i++)
    {
        sw_to_hw[i] = DBG_SWDIAG_UNDEFINED;
    }

    // Parse all HW banks
    for(i = 0 ; i < DBG_SWDIAG_NB_HW_BANKS ; i++)
    {
        // Check if a valid profile is allocated to this HW bank
        if(hw_to_sw[i] != DBG_SWDIAG_UNDEFINED)
        {
            // Allocate the SW profile
            sw_to_hw[hw_to_sw[i]] = i;
        }
    }

    // Clear signals
    SW_DIAG_SET(0);

    GLOBAL_INT_RES();
}


/*
 * EXPORTED FUNCTION DEFINITIONS
 ****************************************************************************************
 */

void dbg_swdiag_init(void)
{
    #if (NVDS_SUPPORT)
    uint8_t length = NVDS_LEN_DIAG_SW;

    // Check SW diags configuration in NVDS
    if(nvds_get(NVDS_TAG_DIAG_SW, &length, hw_to_sw) != NVDS_OK)
    #endif // NVDS_SUPPORT
    {
        // Allocation is NULL
        hw_to_sw[0] = DBG_SWDIAG_UNDEFINED;
        hw_to_sw[1] = DBG_SWDIAG_UNDEFINED;
        hw_to_sw[2] = DBG_SWDIAG_UNDEFINED;
        hw_to_sw[3] = DBG_SWDIAG_UNDEFINED;
    }

    // Update profiles allocation
    dbg_swdiag_update();
}

uint32_t dbg_swdiag_read(void)
{
    return common_read32p(hw_to_sw); //*((uint32_t*) &hw_to_sw[0]);
}

void dbg_swdiag_write(uint32_t profile)
{
    // Write new configuration into global variable
    common_write32p(hw_to_sw, profile);
//    *((uint32_t*) &hw_to_sw[0]) = profile;

    // Update profiles allocation
    dbg_swdiag_update();
}

#endif //RW_SWDIAG

/// @} DBGSWDIAG
