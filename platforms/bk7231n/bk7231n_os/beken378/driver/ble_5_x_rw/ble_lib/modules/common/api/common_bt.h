/**
 ****************************************************************************************
 *
 * @file common_bt.h
 *
 * @brief This file contains the common Bluetooth defines, enumerations and structures
 *        definitions for use by all modules in RW stack.
 *
 * Copyright (C) RivieraWaves 2009-2015
 *
 *
 ****************************************************************************************
 */

#ifndef COMMON_BT_H_
#define COMMON_BT_H_

/**
 ****************************************************************************************
 * @addtogroup COMMON Common SW Block
 * @ingroup ROOT
 * @brief The Common RW SW Block.
 *
 * The COMMON is the block with Bluetooth definitions and structures shared
 * to all the protocol stack blocks. This also contain software wide error code
 * definitions, mathematical functions, help functions, list and buffer definitions.
 *
 * @{
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup COMMON_BT Common Bluetooth defines
 * @ingroup COMMON
 * @brief Common Bluetooth definitions and structures.
 *
 * @{
 ****************************************************************************************
 */


/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include <stdbool.h>       // standard boolean definitions
#include <stddef.h>        // standard definitions
#include <stdint.h>        // standard integer definitions

/*
 * DEFINES
 ****************************************************************************************
 */

#include "common_bt_defines.h" // Bluetooth defines
#include "common_lmp.h"        // Bluetooth LMP definitions
#include "common_hci.h"        // Bluetooth HCI definitions
#include "common_error.h"      // Bluetooth error codes definitions

/// @} COMMON_BT
#endif // COMMON_BT_H_
