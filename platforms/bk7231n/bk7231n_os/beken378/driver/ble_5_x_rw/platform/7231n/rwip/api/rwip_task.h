/**
 ****************************************************************************************
 *
 * @file rwip_task.h
 *
 * @brief Task Identifier description for the RW IP
 *
 * Copyright (C) RivieraWaves 2009-2016
 *
 ****************************************************************************************
 */

#ifndef RWIP_TASK_H_
#define RWIP_TASK_H_

/**
 ****************************************************************************************
 * @addtogroup ROOT
 * @{
 *
 *  Information about RW SW TASK
 *
 * @name RW TASK Configuration
 * @{
 ****************************************************************************************
 */

#include <stdint.h>

/*
 * DEFINES
 ****************************************************************************************
 */


/// Build the first message ID of a task. (in fact a kernel_msg_id_t)
#define TASK_BLE_FIRST_MSG(task) ((uint16_t)((task) << 8))

/// Builds the task identifier from the type and the index of that task.
#define TASK_BLE_BUILD(type, index) ((uint16_t)(((index) << 8)|(type)) )

/// Retrieves task type from task id.
#define TASK_BLE_TYPE_GET(kernel_task_id) (((uint16_t)kernel_task_id) & 0xFF)

/// Retrieves task index number from task id.
#define TASK_BLE_IDX_GET(kernel_task_id) ((((uint16_t)kernel_task_id) >> 8) & 0xFF)

/// Message identifier index
#define MSG_ID(task, idx) (TASK_BLE_FIRST_MSG((TASK_BLE_ID_ ## task)) + idx)

/// Tasks types definition, this value shall be in [0-254] range
/*@TRACE*/
enum TASK_BLE_API_ID
{
    // -----------------------------------------------------------------------------------
    // ---------------------- Controller Task identifer ----------------------------------
    // -----------------------------------------------------------------------------------
    // Link Layer Tasks
    TASK_BLE_ID_LLM          = 0,//!< TASK_BLE_ID_LLM
    TASK_BLE_ID_LLC          = 1,//!< TASK_BLE_ID_LLC
    TASK_BLE_ID_LLD          = 2,//!< TASK_BLE_ID_LLD
    TASK_BLE_ID_LLI          = 3,//!< TASK_BLE_ID_LLI

    TASK_BLE_ID_DBG          = 4,//!< TASK_BLE_ID_DBG

    // BT Controller Tasks
    TASK_BLE_ID_LM           = 5,//!< TASK_BLE_ID_LM
    TASK_BLE_ID_LC           = 6,//!< TASK_BLE_ID_LC
    TASK_BLE_ID_LB           = 7,//!< TASK_BLE_ID_LB
    TASK_BLE_ID_LD           = 8,//!< TASK_BLE_ID_LD

    // -----------------------------------------------------------------------------------
    // --------------------- BLE HL TASK API Identifiers ---------------------------------
    // ---------------------     SHALL NOT BE CHANGED    ---------------------------------
    // -----------------------------------------------------------------------------------

    TASK_BLE_ID_L2CC         = 10,   // L2CAP Controller Task
    TASK_BLE_ID_GATTM        = 11,   // Generic Attribute Profile Manager Task
    TASK_BLE_ID_GATTC        = 12,   // Generic Attribute Profile Controller Task
    TASK_BLE_ID_GAPM         = 13,   // Generic Access Profile Manager
    TASK_BLE_ID_GAPC         = 14,   // Generic Access Profile Controller
    TASK_BLE_ID_APP          = 15,   // Application API

    // -----------------------------------------------------------------------------------
    // --------------------- TRANSPORT AND PLATFORM TASKS --------------------------------
    // -----------------------------------------------------------------------------------
    TASK_BLE_ID_AHI          = 16,   // Application Host Interface
    TASK_BLE_ID_HCI          = 17,   //!< TASK_BLE_ID_HCI
    TASK_BLE_ID_DISPLAY      = 19,   //!< TASK_BLE_ID_DISPLAY

    // -----------------------------------------------------------------------------------
    // --------------------- BLE Profile TASK API Identifiers ----------------------------
    // ---------------------     SHALL NOT BE CHANGED    ---------------------------------
    // -----------------------------------------------------------------------------------

#if (BEKEN_TASK_ENABLE)
	TASK_BLE_ID_BEKEN		 = 100,	 //// BEKEN Test
#endif

#if (BLE_COMM_SERVER)
    TASK_BLE_ID_COMMON       = 120,
#endif

#if (BLE_MESH)
    TASK_BLE_ID_MESH         = 200,  // Mesh Task
#endif

    /* 240 -> 241 reserved for Audio Mode 0 */
    TASK_BLE_ID_AM0          = 240,  // BLE Audio Mode 0 Task
    TASK_BLE_ID_AM0_HAS      = 241,  // BLE Audio Mode 0 Hearing Aid Service Task


    TASK_BLE_ID_THPP         = 242,  // Throughput profile tester used for debugging

    TASK_BLE_ID_INVALID      = 0xFF, // Invalid Task Identifier
};

/// @} BT Stack Configuration
/// @} ROOT

#endif //RWIP_CONFIG_H_
