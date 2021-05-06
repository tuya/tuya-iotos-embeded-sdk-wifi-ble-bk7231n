/**
****************************************************************************************
*
* @file dbg.c
*
* @brief Debug function
*
* Copyright (C) RivieraWaves 2009-2015
*
*
****************************************************************************************
*/

/**
****************************************************************************************
* @addtogroup DBG
* @{
****************************************************************************************
*/

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"    // stack configuration
#if ((BLE_EMB_PRESENT) || (BT_EMB_PRESENT))

#include "common_error.h"       // common error definition
#include "kernel_task.h"        // kernel task definition
#include "dbg_task.h"       // debug task definition
#include "dbg.h"            // debug definition

#if (HCI_PRESENT)
#include "hci.h"
#endif //(HCI_PRESENT)

/*
 * DEFINES
 ****************************************************************************************
 */


/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */
#if (RW_DEBUG)
/// DEBUG task descriptor
static const struct kernel_task_desc TASK_DESC_DBG = {NULL, &dbg_default_handler, dbg_state, DBG_STATE_MAX, DBG_IDX_MAX};
#endif //RW_DEBUG

/*
 * LOCAL FUNCTION DEFINITION
 ****************************************************************************************
 */
 


/*
 * EXPORTED FUNCTION DEFINITION
 ****************************************************************************************
 */
#if (RW_DEBUG)
void dbg_init(void)
{
    // Create DEBUG Task
    kernel_task_create(TASK_DBG, &TASK_DESC_DBG);

    // Initialize DBG task to idle state
    kernel_state_set(TASK_DBG,DBG_IDLE);

    #if (RW_SWDIAG)
    // Initialize SW profiling module
    dbg_swdiag_init();
    #endif //RW_SWDIAG

}
#endif //RW_DEBUG

void dbg_platform_reset_complete(uint32_t error)
{
    // structure type for the complete command event
    struct hci_basic_cmd_cmp_evt *evt = KERNEL_MSG_ALLOC(HCI_CMD_CMP_EVENT, 0, HCI_DBG_PLF_RESET_CMD_OPCODE, hci_basic_cmd_cmp_evt);

    if(error == RESET_TO_ROM)
    {
        evt->status = COMMON_ERROR_HARDWARE_FAILURE;
    }
    else if(error == RESET_AND_LOAD_FW)
    {
        evt->status = COMMON_ERROR_NO_ERROR;
    }

    hci_send_2_host(evt);
}

#endif // ((BLE_EMB_PRESENT) || (BT_EMB_PRESENT))
///@} DBG
