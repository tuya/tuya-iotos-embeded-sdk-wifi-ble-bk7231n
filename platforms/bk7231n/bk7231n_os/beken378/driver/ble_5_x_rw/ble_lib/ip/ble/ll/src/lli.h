/**
 ****************************************************************************************
 *
 * @file lli.h
 *
 * @brief Main API file for the Link Layer ISO
 *
 * Copyright (C) RivieraWaves 2009-2015
 *
 ****************************************************************************************
 */

#ifndef LLI_H_
#define LLI_H_

/**
 ****************************************************************************************
 * @defgroup LLI Link Layer ISO
 * @ingroup ROOT
 * @brief BLE Lower Layers
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"

#if (BLE_ISO_PRESENT)

#include "rwip_task.h"      // Task definitions
#include "common_bt.h"          // BLE standard definitions
#include "common_llcp.h"        // Definition of LL_CIS_REQ PDU
#include "lld.h"            // LLD Definitions

/*
 * DEFINES
 ****************************************************************************************
 */

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialization of the BLE LLI task
 *
 * This function initializes the the LLI task.
 *
 * @param[in]  reset    True: reset | False: init
 ****************************************************************************************
 */
void lli_init(bool reset);

/**
 ****************************************************************************************
 * @brief Inform that a link has terminated to immediately stop corresponding ISO channels
 *
 * @param[in] link_id Link identifier
 * @param[in] reason  Link termination reason
 ****************************************************************************************
 */
void lli_link_stop_ind(uint8_t link_id, uint8_t reason);

#if (BLE_ISO_MODE_0)
/**
 ****************************************************************************************
 * @brief Check if link is used for audio mode 0
 *
 * @param[in] link_id Link identifier
 *
 * @return   True if link is used for audio mode 0
 ****************************************************************************************
 */
bool lli_am0_check(uint8_t link_id);
#endif // (BLE_ISO_MODE_0)

/**
 ****************************************************************************************
 * @brief Retrieve number of available channels
 *
 * @return Number of channel that can be allocated
 ****************************************************************************************
 */
uint8_t  lli_nb_chan_ava_get(void);

#endif //(BLE_ISO_PRESENT)

/// @} LLI

#endif // LLI_H_
