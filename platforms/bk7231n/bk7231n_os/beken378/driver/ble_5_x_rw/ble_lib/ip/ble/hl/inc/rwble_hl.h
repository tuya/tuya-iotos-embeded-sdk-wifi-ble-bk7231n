/**
 ****************************************************************************************
 *
 * @file rwble_hl.h
 *
 * @brief Entry points of the BLE HL software
 *
 * Copyright (C) RivieraWaves 2009-2016
 *
 *
 ****************************************************************************************
 */

#ifndef RWBLE_HL_H_
#define RWBLE_HL_H_

#include <stdint.h>
#include <stdbool.h>                // standard boolean definitions
/**
 ****************************************************************************************
 * @addtogroup ROOT
 * @brief Entry points of the BLE Host stack
 *
 * This module contains the primitives that allow an application accessing and running the
 * BLE protocol stack
 *
 * @{
 ****************************************************************************************
 */


/*
 * DEFINES
 ****************************************************************************************
 */

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */


/**
 ****************************************************************************************
 * @brief Initialize the BLE Host stack.
 *
 * @param[in] init_type  Type of initialization (@see enum rwip_init_type)
 ****************************************************************************************
 */
void rwble_hl_init(uint8_t init_type);

/// @} RWBLE_HL

#endif // RWBLE_HL_H_
