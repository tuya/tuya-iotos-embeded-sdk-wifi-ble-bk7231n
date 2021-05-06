/**
 ****************************************************************************************
 *
 * @file dbg_task.c
 *
 * @brief DBG task source file
 *
 * Copyright (C) RivieraWaves 2009-2015
 *
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup DBGTASK
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"         // stack configuration
#if (RW_DEBUG && (BT_EMB_PRESENT || BLE_EMB_PRESENT))

#include <string.h>               // for mem* functions
#include "common_endian.h"
#include "common_error.h"
#include "architect.h"                 // arch definition
#include "dbg_task.h"             // debug task definition
#include "dbg.h"                  // debug block definition
#include "rwip.h"                 // RF API definitions
#include "kernel.h"                   // kernel definitions
#include "kernel_mem.h"               // kernel memory management

#if (RW_DEBUG_FLASH)
#include "flash.h"                // flash functions and macros
#endif //RW_DEBUG_FLASH

#if (RW_DEBUG_NVDS)
#if (NVDS_SUPPORT)
#include "nvds.h"         // NVDS definitions
#endif // NVDS_SUPPORT
#endif //RW_DEBUG_NVDS

#if (BLE_EMB_PRESENT)
#include "ble_reg_access.h"           // access registers
#endif // BLE_EMB_PRESENT

#if (BLE_EMB_PRESENT)
#include "reg_blecore.h"
#include "reg_ble_em_cs.h"
#include "lld.h"
#include "lld_evt.h"
#include "lld_util.h"
#include "llm_util.h"
#include "llc_util.h"
#include "llcontrl.h"
#if (BLE_AUDIO)
#include "audio.h"
#endif
#endif //BLE_EMB_PRESENT

#if (RW_WLAN_COEX_TEST || RW_MWS_COEX_TEST)
#include "dbg_mwsgen.h"
#endif //(RW_WLAN_COEX_TEST || RW_MWS_COEX_TEST)


#if (HCI_PRESENT)
#include "hci.h"
#endif //(HCI_PRESENT)


/*
 * DEFINES
 ****************************************************************************************
 */

/// 8 bit access types
#define _8_Bit                              8
/// 16 bit access types
#define _16_Bit                             16
/// 32 bit access types
#define _32_Bit                             32

/// PLATFORM RESET REASON: Reset and load FW from flash
#define PLATFORM_RESET_TO_FW    (0)
/// PLATFORM RESET REASON: Reset and stay in ROM code
#define PLATFORM_RESET_TO_ROM   (1)


/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */


/*
 * LOCAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */
#if (BLE_EMB_PRESENT)
/**
 ****************************************************************************************
 * @brief Update the TX power field of the event passed as parameter with the requested
 * value of TX power
 *
 * @param[in] conhdl    The connection handle for which the TX power is updated
 * @param[in] tx_pwr The TX power to be applied to this event
 *
 * @return The status
 *
 ****************************************************************************************
 */
static uint8_t dbg_task_txpwr_update(uint16_t conhdl, uint8_t tx_pwr)
{
    // Event allocated for the link
    struct lld_evt_tag *evt = NULL;
    uint8_t status = COMMON_ERROR_NO_ERROR;
    if (conhdl == BLE_CONNECTION_MAX)
    {
        // Address of the element is stored in the LLM environment
        evt = LLD_EVT_ENV_ADDR_GET(llm_le_env.elt);
    }
    #if (BLE_CENTRAL || BLE_PERIPHERAL)
    else
    {
        // Check if the connection exists
        if(llc_util_get_element_ptr(conhdl) != NULL)
        {
            evt = LLD_EVT_ENV_ADDR_GET(llc_util_get_element_ptr(conhdl));
        }
        else
        {
            status = COMMON_ERROR_UNKNOWN_CONNECTION_ID;
        }
    }
    #endif //(BLE_CENTRAL || BLE_PERIPHERAL)
    // Update Power in the event structure
    if(evt)
    {
        evt->tx_pwr = tx_pwr;
    }
    return(status);
}
#endif// (BLE_EMB_PRESENT)

/*
 * MESSAGE HANDLERS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Handle the reception of the vendor specific command read from memory.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int
hci_dbg_rd_mem_cmd_handler(kernel_msg_id_t const msgid, struct hci_dbg_rd_mem_cmd const *param,
                       kernel_task_id_t const dest_id, kernel_task_id_t const src_id)
{
    #if (RW_DEBUG_MEM)
    uint8_t length = 0;
    uint32_t init_addr = 0;
    uint32_t value = 0;
    uint32_t i = 0;
    #endif //RW_DEBUG_MEM

    // structure type for the complete command event
    struct hci_dbg_basic_rd_data_cmd_cmp_evt *event = KERNEL_MSG_ALLOC(HCI_CMD_CMP_EVENT , 0, src_id, hci_dbg_basic_rd_data_cmd_cmp_evt);

    #if (RW_DEBUG_MEM)
    init_addr = param->start_addr;
    length = param->length;


    /* Check that data length is not null or too big before reading */
    if ((length == 0)||
        (length > (sizeof(struct buffer_tag)- sizeof(uint8_t))))
    {
        event->status = COMMON_ERROR_INVALID_HCI_PARAM;
    }
    else
    {
        /* Check type of data to be read */
        if (param->type == _8_Bit)
        {
            /* Read bytes */
            for (i = 0; i < length; i++)
            {
                /* Read value at @ set in Param1+i */
                event->buf.data[i] = *(volatile uint8_t *)(init_addr+i);
            }
        }
        else if (param->type == _16_Bit)
        {
            for (i = 0; i < length; i += 2)
            {
                /* Read value at @ set in Param1+i */
                value = (*(volatile uint16_t *)(init_addr+i));

                /* store in the buffer */
                event->buf.data[i]   = (uint8_t) value;
                value >>= 8;
                event->buf.data[i+1] = (uint8_t) value;
             }
        }
        else if (param->type == _32_Bit)
        {
            /* Read 32 bit word */
            for (i = 0; i < length; i += 4)
            {
                value = (*(volatile uint32_t *)(init_addr+i));

                /* store in the buffer */
                event->buf.data[i]   = (uint8_t) value;
                value >>= 8;
                event->buf.data[i+1] = (uint8_t) value;
                value >>= 8;
                event->buf.data[i+2] = (uint8_t) value;
                value >>= 8;
                event->buf.data[i+3] = (uint8_t) value;
            }
        }
        event->buf.length = length;
        event->status = COMMON_ERROR_NO_ERROR;
    }
    #else //RW_DEBUG_MEM
    event->status = COMMON_ERROR_UNSUPPORTED;
    #endif //RW_DEBUG_MEM

    // sends the message
    hci_send_2_host(event);

    return (KERNEL_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handle the reception of the vendor specific command write to memory.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int
hci_dbg_wr_mem_cmd_handler(kernel_msg_id_t const msgid, struct hci_dbg_wr_mem_cmd const *param,
                       kernel_task_id_t const dest_id, kernel_task_id_t const src_id)
{
    #if (RW_DEBUG_MEM)
    uint32_t value = 0;
    uint32_t length = 0;
    uint32_t init_addr = 0;
    uint8_t *data_buf;
    uint32_t i = 0;
    #endif //RW_DEBUG_MEM

    // structure type for the complete command event
    struct hci_basic_cmd_cmp_evt *event = KERNEL_MSG_ALLOC(HCI_CMD_CMP_EVENT , 0, HCI_DBG_WR_MEM_CMD_OPCODE, hci_basic_cmd_cmp_evt);

    #if (RW_DEBUG_MEM)
    length    = param->buf.length;
    data_buf  = (uint8_t*)&param->buf.data[0];
    init_addr = param->start_addr;

    /* Check that data length is not null or too big before reading */
    if ((length == 0)|| (length > (sizeof(struct buffer_tag)- sizeof(uint8_t))))
    {
        event->status = COMMON_ERROR_INVALID_HCI_PARAM;
    }
    else
    {
        /* Check type of data to be written */
        if (param->type == _8_Bit)
        {
            /* Write bytes */
            for (i = 0; i < length; i++)
            {
                /* Set value type at @ Param1 */
                *(volatile uint8_t *)(init_addr+i) = data_buf[i];
            }
        }
        else if (param->type == _16_Bit)
        {
            /* Write 16 bits word */
            for (i = 0; i < length; i += 2)
            {
                /* Set value type at @ Param1 */
                value = ((uint32_t)data_buf[i+1]);
                value <<= 8;
                value |= ((uint32_t)data_buf[i+0]);
                *(volatile uint16_t *)(init_addr+i) = value;
            }
        }
        else if(param->type == _32_Bit)
        {
            /* Write 32 bit word */
            for (i = 0; i < length; i += 4)
            {
                /* Set value at @ Param1 */
                value  = ((uint32_t)data_buf[i+3]);
                value <<= 8;
                value |= ((uint32_t)data_buf[i+2]);
                value <<= 8;
                value |= ((uint32_t)data_buf[i+1]);
                value <<= 8;
                value |= ((uint32_t)data_buf[i+0]);
                *(volatile uint32_t *)(init_addr+i) = value;
            }
        }
        event->status = COMMON_ERROR_NO_ERROR;
    }
    #else //RW_DEBUG_MEM
    event->status = COMMON_ERROR_UNSUPPORTED;
    #endif //RW_DEBUG_MEM

    // sends the message
    hci_send_2_host(event);

    return (KERNEL_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handle the reception of the vendor specific command delete param.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int
hci_dbg_del_par_cmd_handler(kernel_msg_id_t const msgid, struct hci_dbg_del_param_cmd const *param,
                         kernel_task_id_t const dest_id, kernel_task_id_t const src_id)
{
    // structure type for the complete command event
    struct hci_basic_cmd_cmp_evt *event = KERNEL_MSG_ALLOC(HCI_CMD_CMP_EVENT , 0, HCI_DBG_DEL_PAR_CMD_OPCODE, hci_basic_cmd_cmp_evt);

    #if (NVDS_SUPPORT && RW_DEBUG_NVDS && NVDS_READ_WRITE)
    nvds_del(param->param_tag);
    event->status = COMMON_ERROR_NO_ERROR;
    #else
    event->status = COMMON_ERROR_UNSUPPORTED;
    #endif // RW_DEBUG_NVDS && NVDS_READ_WRITE

    // sends the message
    hci_send_2_host(event);

    return (KERNEL_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handle the reception of the vendor specific command id flash.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int
hci_dbg_id_flash_cmd_handler(kernel_msg_id_t const msgid, void const *param, kernel_task_id_t const dest_id, kernel_task_id_t const src_id)
{
    // structure type for the complete command event
    struct hci_dbg_id_flash_cmd_cmp_evt *event = KERNEL_MSG_ALLOC(HCI_CMD_CMP_EVENT , 0, src_id, hci_dbg_id_flash_cmd_cmp_evt);

    #if (RW_DEBUG_FLASH)
    flash_identify(&event->flash_id, NULL);
    event->status = (event->flash_id != FLASH_TYPE_UNKNOWN)?COMMON_ERROR_NO_ERROR : COMMON_ERROR_HARDWARE_FAILURE;
    #else //RW_DEBUG_FLASH
    event->status = COMMON_ERROR_UNSUPPORTED;
    #endif //RW_DEBUG_FLASH

    // sends the message
    hci_send_2_host(event);

    return (KERNEL_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handle the reception of the vendor specific command flash erase.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int
hci_dbg_er_flash_cmd_handler(kernel_msg_id_t const msgid, struct hci_dbg_er_flash_cmd const *param,
                            kernel_task_id_t const dest_id, kernel_task_id_t const src_id)
{
    // structure type for the complete command event
    struct hci_basic_cmd_cmp_evt *event = KERNEL_MSG_ALLOC(HCI_CMD_CMP_EVENT , 0, src_id, hci_basic_cmd_cmp_evt);

    #if (RW_DEBUG_FLASH)
    event->status = flash_erase(param->flashtype, param->startoffset,param->size, NULL);
    #else //RW_DEBUG_FLASH
    event->status = COMMON_ERROR_UNSUPPORTED;
    #endif //RW_DEBUG_FLASH

    // sends the message
    hci_send_2_host(event);

    return (KERNEL_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handle the reception of the vendor specific command write flash.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int
hci_dbg_wr_flash_cmd_handler(kernel_msg_id_t const msgid, struct hci_dbg_wr_flash_cmd const *param,
                            kernel_task_id_t const dest_id, kernel_task_id_t const src_id)
{
    // structure type for the complete command event
    struct hci_basic_cmd_cmp_evt *event = KERNEL_MSG_ALLOC(HCI_CMD_CMP_EVENT , 0, src_id, hci_basic_cmd_cmp_evt);

    #if (RW_DEBUG_FLASH)
    event->status = flash_write(param->flashtype, param->startoffset, param->buf.length, (uint8_t*)&param->buf.data[0], NULL);
    #else //RW_DEBUG_FLASH
    event->status = COMMON_ERROR_UNSUPPORTED;
    #endif //RW_DEBUG_FLASH

    // sends the message
    hci_send_2_host(event);

    return (KERNEL_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handle the reception of the vendor specific command read flash.
 *
 * Read the requested number of byte from flash.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int
hci_dbg_rd_flash_cmd_handler(kernel_msg_id_t const msgid, struct hci_dbg_rd_flash_cmd const *param,
                           kernel_task_id_t const dest_id, kernel_task_id_t const src_id)
{
    // structure type for the complete command event
    struct hci_dbg_basic_rd_data_cmd_cmp_evt *event = KERNEL_MSG_ALLOC(HCI_CMD_CMP_EVENT , 0, src_id, hci_dbg_basic_rd_data_cmd_cmp_evt);

    #if (RW_DEBUG_FLASH)
    event->buf.length = param->size;
    event->status     = flash_read(param->flashtype, param->startoffset, param->size, (uint8_t*)&event->buf.data[0], NULL);
    #else //RW_DEBUG_FLASH
    event->status = COMMON_ERROR_UNSUPPORTED;
    #endif //RW_DEBUG_FLASH

    // sends the message
    hci_send_2_host(event);

    return (KERNEL_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handle the reception of the vendor specific command read
 *        parameter request.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int
hci_dbg_rd_par_cmd_handler(kernel_msg_id_t const msgid, struct hci_dbg_rd_par_cmd const *param,
                         kernel_task_id_t const dest_id, kernel_task_id_t const src_id)
{
    // structure type for the complete command event
    struct hci_dbg_basic_rd_data_cmd_cmp_evt *event = KERNEL_MSG_ALLOC(HCI_CMD_CMP_EVENT , 0, src_id, hci_dbg_basic_rd_data_cmd_cmp_evt);

    #if (NVDS_SUPPORT && RW_DEBUG_NVDS)
    event->buf.length = sizeof(event->buf.data);
    nvds_get(param->param_tag, (uint8_t*)&event->buf.length, (uint8_t*)&event->buf.data[0]);
    event->status = COMMON_ERROR_NO_ERROR;
    #else //RW_DEBUG_NVDS
    event->buf.length = 0;
    event->status = COMMON_ERROR_UNSUPPORTED;
    #endif //RW_DEBUG_NVDS

    // sends the message
    hci_send_2_host(event);

    return (KERNEL_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handle the reception of the vendor specific command write
 *        parameter request.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int
hci_dbg_wr_par_cmd_handler(kernel_msg_id_t const msgid, struct hci_dbg_wr_par_cmd const *param,
                         kernel_task_id_t const dest_id, kernel_task_id_t const src_id)
{
    // structure type for the complete command event
    struct hci_basic_cmd_cmp_evt *event = KERNEL_MSG_ALLOC(HCI_CMD_CMP_EVENT , 0, src_id, hci_basic_cmd_cmp_evt);

    #if (NVDS_SUPPORT && RW_DEBUG_NVDS && NVDS_READ_WRITE)
    nvds_put(param->param_tag, param->buf.length, (uint8_t*)&param->buf.data[0]);
    event->status = COMMON_ERROR_NO_ERROR;
    #else
    event->status = COMMON_ERROR_UNSUPPORTED;
    #endif // RW_DEBUG_NVDS && NVDS_READ_WRITE

    // sends the message
    hci_send_2_host(event);

    return (KERNEL_MSG_CONSUMED);
}


/**
 ****************************************************************************************
 * @brief Handle the reception of the vendor specific command Read Kernel Stats
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int
hci_dbg_rd_kernel_stats_cmd_handler(kernel_msg_id_t const msgid, void const *param,
                              kernel_task_id_t const dest_id, kernel_task_id_t const src_id)
{
    // structure type for the complete command event
    struct hci_dbg_rd_kernel_stats_cmd_cmp_evt *event = KERNEL_MSG_ALLOC(HCI_CMD_CMP_EVENT, 0, HCI_DBG_RD_KERNEL_STATS_CMD_OPCODE, hci_dbg_rd_kernel_stats_cmd_cmp_evt);

    #if (KERNEL_PROFILING)
    if(kernel_stats_get(&event->max_msg_sent, &event->max_msg_saved, &event->max_timer_used, &event->max_heap_used) == KERNEL_SUCCESS)
    {
        event->status = COMMON_ERROR_NO_ERROR;
    }
    else
    {
        event->status = COMMON_ERROR_UNSUPPORTED;
        event->max_msg_sent   = 0;
        event->max_msg_saved  = 0;
        event->max_timer_used = 0;
        event->max_heap_used  = 0;
    }
    #if (RW_DEBUG_STACK_PROF)
    event->max_stack_used = get_stack_usage();
    #else //RW_DEBUG_STACK_PROF
    event->max_stack_used = 0;
    #endif //RW_DEBUG_STACK_PROF
    #else //KERNEL_PROFILING
    event->status         = COMMON_ERROR_UNSUPPORTED;
    event->max_msg_sent   = 0;
    event->max_msg_saved  = 0;
    event->max_timer_used = 0;
    event->max_heap_used  = 0;
    event->max_stack_used = 0;
    #endif //KERNEL_PROFILING

    // sends the message
    hci_send_2_host(event);

    return (KERNEL_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handle the reception of the vendor specific command Read Memory info
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int hci_dbg_rd_mem_info_cmd_handler(kernel_msg_id_t const msgid, void const *param,
                              kernel_task_id_t const dest_id, kernel_task_id_t const src_id)
{
    struct hci_dbg_rd_mem_info_cmd_cmp_evt * meminfo_msg;
    #if (KERNEL_PROFILING)
    uint8_t cursor;
    struct hci_dbg_rd_mem_info_cmd_cmp_evt meminfo;

    // First remove command message in order to be sure it's not taken in account.
    kernel_msg_free(kernel_param2msg(param));

    // Then retrieve memory information from kernel
    meminfo.max_mem_used = kernel_get_max_mem_usage();

    for(cursor = 0; cursor < KERNEL_MEM_BLOCK_MAX ; cursor++)
    {
        meminfo.mem_used[cursor] = kernel_get_mem_usage(cursor);
    }
    #endif // (KERNEL_PROFILING)
    // Finally send indication to application that request memory information
    meminfo_msg = KERNEL_MSG_ALLOC(HCI_CMD_CMP_EVENT , 0, HCI_DBG_RD_MEM_INFO_CMD_OPCODE, hci_dbg_rd_mem_info_cmd_cmp_evt);

    #if (KERNEL_PROFILING)
    memcpy(meminfo_msg, &meminfo, sizeof(struct hci_dbg_rd_mem_info_cmd_cmp_evt));
    meminfo_msg->status = COMMON_ERROR_NO_ERROR;
    #else
    meminfo_msg->status = COMMON_ERROR_UNSUPPORTED;
    #endif // (KERNEL_PROFILING)

    // sends the message
    hci_send_2_host(meminfo_msg);

    // source message already free
    return (KERNEL_MSG_NO_FREE);
}

/**
 ****************************************************************************************
 * @brief Handle the reception of the vendor specific command Read RF Register value.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int
hci_dbg_rf_reg_rd_cmd_handler(kernel_msg_id_t const msgid, struct hci_dbg_rf_reg_rd_cmd const *param,
                          kernel_task_id_t const dest_id, kernel_task_id_t const src_id)
{
    // structure type for the complete command event
    struct hci_dbg_rf_reg_rd_cmd_cmp_evt *event = KERNEL_MSG_ALLOC(HCI_CMD_CMP_EVENT , 0, HCI_DBG_RF_REG_RD_CMD_OPCODE, hci_dbg_rf_reg_rd_cmd_cmp_evt);

    #if (BLE_STD_MODE && BLE_EMB_PRESENT)
    // Unlock BLE<->RF SPI access
    #endif //BLE_STD_MODE && BLE_EMB_PRESENT
    // Read RF register
    event->value   = common_htobl(rwip_rf.reg_rd(common_btohs(param->addr)));

    #if (BLE_STD_MODE && BLE_EMB_PRESENT)
    // Restore BLE<->RF SPI access lock
    #endif //BLE_STD_MODE && BLE_EMB_PRESENT

    event->addr    = param->addr;
    event->status  = COMMON_ERROR_NO_ERROR;

    // sends the message
    hci_send_2_host(event);

    return (KERNEL_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handle the reception of the vendor specific command Write RF Register value.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int
hci_dbg_rf_reg_wr_cmd_handler(kernel_msg_id_t const msgid, struct hci_dbg_rf_reg_wr_cmd const *param,
                          kernel_task_id_t const dest_id, kernel_task_id_t const src_id)
{
    // structure type for the complete command event
    struct hci_dbg_rf_reg_wr_cmd_cmp_evt *event = KERNEL_MSG_ALLOC(HCI_CMD_CMP_EVENT , 0, HCI_DBG_RF_REG_WR_CMD_OPCODE, hci_dbg_rf_reg_wr_cmd_cmp_evt);

    #if (BLE_STD_MODE && BLE_EMB_PRESENT)
    // Unlock BLE<->RF SPI access
    #endif //BLE_STD_MODE && BLE_EMB_PRESENT

    // Write RF register
    rwip_rf.reg_wr(common_btohs(param->addr), common_btohl(param->value));

    #if (BLE_STD_MODE && BLE_EMB_PRESENT)
    // Restore BLE<->RF SPI access lock
    #endif //BLE_STD_MODE && BLE_EMB_PRESENT

    event->addr    = param->addr;
    event->status  = COMMON_ERROR_NO_ERROR;

    // sends the message
    hci_send_2_host(event);

    return (KERNEL_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handle the reception of the vendor specific command platform reset.
 * This command reset FW and return to ROM code.
 *
 * @note command complete(success) is sent by ROM code.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int
hci_dbg_plf_reset_cmd_handler(kernel_msg_id_t const msgid, struct hci_dbg_plf_reset_cmd const *param,
        kernel_task_id_t const dest_id, kernel_task_id_t const src_id)
{
    uint32_t error_code = RESET_NO_ERROR;

    // structure type for the complete command event
    struct hci_basic_cmd_cmp_evt *event = KERNEL_MSG_ALLOC(HCI_CMD_CMP_EVENT , src_id, HCI_DBG_PLF_RESET_CMD_OPCODE, hci_basic_cmd_cmp_evt);

    switch(param->reason)
    {
        case PLATFORM_RESET_TO_FW: error_code = RESET_AND_LOAD_FW; break;
        case PLATFORM_RESET_TO_ROM: error_code = RESET_TO_ROM; break;
        default: break;
    }

    if(error_code != RESET_NO_ERROR)
    {
        // Perform platform Reset (return to ROM code)
        platform_reset(error_code);
    }

    // If reason is not valid, returns a CC event with bad status
    event->status = COMMON_ERROR_INVALID_HCI_PARAM;

    // sends the message
    hci_send_2_host(event);

    return (KERNEL_MSG_CONSUMED);
}

#if (BLE_EMB_PRESENT)
/**
 ****************************************************************************************
 * @brief Handle the reception of the vendor specific command read hw register.
 * Return the value of the register requested in the command.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int
hci_dbg_hw_reg_rd_cmd_handler(kernel_msg_id_t const msgid,
                         struct hci_dbg_hw_reg_rd_cmd const *param,
                         kernel_task_id_t const dest_id,
                         kernel_task_id_t const src_id)
{
    // structure type for the complete command event
    struct hci_dbg_hw_reg_rd_cmd_cmp_evt *event;

    // allocate the status event message
    event =  KERNEL_MSG_ALLOC(HCI_CMD_CMP_EVENT , src_id, HCI_DBG_HW_REG_RD_CMD_OPCODE, hci_dbg_hw_reg_rd_cmd_cmp_evt);

    event->reg_value = common_htobl(REG_BLE_RD(REG_BLECORE_BASE_ADDR + common_btohs(param->reg_addr)));
    event->reg_addr = param->reg_addr;
    event->status = COMMON_ERROR_NO_ERROR;
    // sends the message
    hci_send_2_host(event);

    return (KERNEL_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handle the reception of the vendor specific command write hw register.
 * Write the requested value in the register passed in the command parameter.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int
hci_dbg_hw_reg_wr_cmd_handler(kernel_msg_id_t const msgid,
                         struct hci_dbg_hw_reg_wr_cmd const *param,
                         kernel_task_id_t const dest_id,
                         kernel_task_id_t const src_id)
{
    // structure type for the complete command event
    struct hci_dbg_hw_reg_wr_cmd_cmp_evt *event;

    // allocate the status event message
    event =  KERNEL_MSG_ALLOC(HCI_CMD_CMP_EVENT , src_id, HCI_DBG_HW_REG_WR_CMD_OPCODE, hci_dbg_hw_reg_wr_cmd_cmp_evt);

    #if (BLE_EMB_PRESENT)
    REG_BLE_WR(REG_BLECORE_BASE_ADDR + common_btohs(param->reg_addr), common_btohl(param->reg_value));
    #endif //BLE_EMB_PRESENT


    event->reg_addr = param->reg_addr;
    event->status = COMMON_ERROR_NO_ERROR;
    // sends the message
    hci_send_2_host(event);

    return (KERNEL_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handle the reception of the vendor specific command set bd address.
 * Write the requested bd address in the dedicated registers.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int
hci_dbg_set_bd_addr_cmd_handler(kernel_msg_id_t const msgid,
                         struct hci_dbg_set_bd_addr_cmd const *param,
                         kernel_task_id_t const dest_id,
                         kernel_task_id_t const src_id)
{
    // structure type for the complete command event
    struct hci_basic_cmd_cmp_evt *event;

    // Set public address
    llm_util_set_public_addr((struct bd_addr *) &param->addr);

    // Sends the CC event
    event = KERNEL_MSG_ALLOC(HCI_CMD_CMP_EVENT , src_id, HCI_DBG_SET_BD_ADDR_CMD_OPCODE, hci_basic_cmd_cmp_evt);
    event->status = COMMON_ERROR_NO_ERROR;
    hci_send_2_host(event);

    return (KERNEL_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handle the reception of the vendor specific command set crc.
 * Write the requested crc in the dedicated registers.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int
hci_dbg_set_crc_cmd_handler(kernel_msg_id_t const msgid,
                         struct hci_dbg_set_crc_cmd const *param,
                         kernel_task_id_t const dest_id,
                         kernel_task_id_t const src_id)
{
    // structure type for the complete command event
    struct hci_basic_cmd_cmp_evt *event;
    uint16_t conhdl;

    // Check if the command is for the advertising channel or a connection link
    if (param->conhdl >= BLE_CONNECTION_MAX)
        conhdl = LLD_ADV_HDL;
    else
        conhdl = param->conhdl;

    // Set the random address in the environment variable
    // Initialize the crcinit in the CS
    ble_crcinit0_set(conhdl, common_htobs(common_read16p(&param->crc.crc[0])));
    ble_crcinit1_setf(conhdl,param->crc.crc[2]);
    // allocate the status event message
    event =  KERNEL_MSG_ALLOC(HCI_CMD_CMP_EVENT , src_id, HCI_DBG_SET_CRC_CMD_OPCODE, hci_basic_cmd_cmp_evt);
    event->status = COMMON_ERROR_NO_ERROR;
    // sends the message
    hci_send_2_host(event);

    kernel_state_set(dest_id, DBG_IDLE);
    return (KERNEL_MSG_CONSUMED);
}
#if (BLE_PERIPHERAL || BLE_CENTRAL)
/**
 ****************************************************************************************
 * @brief Handle the reception of the vendor specific command llcp discard.
 * Write the requested crc in the dedicated registers.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int
hci_dbg_llcp_discard_cmd_handler(kernel_msg_id_t const msgid,
                                struct hci_dbg_llcp_discard_cmd const *param,
                                kernel_task_id_t const dest_id,
                                kernel_task_id_t const src_id)
{
    // structure type for the complete command event
    struct hci_basic_cmd_cmp_evt *event;

    // Set the LLCP discard flag in the LLC environment
    llc_util_set_llcp_discard_enable(param->conhdl, param->enable);

    // allocate the status event message
    event =  KERNEL_MSG_ALLOC(HCI_CMD_CMP_EVENT , src_id, HCI_DBG_LLCP_DISCARD_CMD_OPCODE, hci_basic_cmd_cmp_evt);
    event->status = COMMON_ERROR_NO_ERROR;
    // sends the message
    hci_send_2_host(event);

    return (KERNEL_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handle the reception of the vendor specific command write DLE info.
 * This command update the default value of the DLE
 *
 * @note command complete(success) is sent by ROM code.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int
hci_dbg_wr_dle_dft_value_cmd_handler(kernel_msg_id_t const msgid, struct hci_dbg_wr_dle_dft_value_cmd const *param,
        kernel_task_id_t const dest_id, kernel_task_id_t const src_id)
{

    // structure type for the complete command event
    struct hci_basic_cmd_cmp_evt *event = KERNEL_MSG_ALLOC(HCI_CMD_CMP_EVENT , src_id, HCI_DBG_WR_DLE_DFT_VALUE_CMD_OPCODE, hci_basic_cmd_cmp_evt);

    if( (param->suppted_max_rx_octets < BLE_MIN_OCTETS ) || (param->suppted_max_rx_octets > BLE_MAX_OCTETS)
      || (param->suppted_max_tx_octets < BLE_MIN_OCTETS ) || (param->suppted_max_tx_octets > BLE_MAX_OCTETS)
      || (param->suppted_max_rx_time < BLE_MIN_TIME) || (param->suppted_max_rx_time > BLE_MAX_TIME)
      || (param->suppted_max_tx_time < BLE_MIN_TIME) || (param->suppted_max_tx_time > BLE_MAX_TIME))
    {
       event->status = COMMON_ERROR_INVALID_HCI_PARAM;
    }
    else
    {

        if (param->suppted_max_tx_octets < llm_le_env.data_len_val.conn_initial_max_tx_octets)
            llm_le_env.data_len_val.conn_initial_max_tx_octets = param->suppted_max_tx_octets;

        if (param->suppted_max_tx_time < llm_le_env.data_len_val.conn_initial_max_tx_time)
            llm_le_env.data_len_val.conn_initial_max_tx_time = param->suppted_max_tx_time;

        llm_le_env.data_len_val.suppted_max_rx_octets = param->suppted_max_rx_octets;
        llm_le_env.data_len_val.suppted_max_rx_time = param->suppted_max_rx_time;
        llm_le_env.data_len_val.suppted_max_tx_octets = param->suppted_max_tx_octets;
        llm_le_env.data_len_val.suppted_max_tx_time = param->suppted_max_tx_time;
        // If reason is not valid, returns a CC event with bad status
        event->status = COMMON_ERROR_NO_ERROR;
    }
    hci_send_2_host(event);

    return (KERNEL_MSG_CONSUMED);
}
#endif // #if (BLE_PERIPHERAL || BLE_CENTRAL)
/**
 ****************************************************************************************
 * @brief Handle the reception of the vendor specific command reset rx counter in the CS.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int
hci_dbg_reset_rx_cnt_cmd_handler(kernel_msg_id_t const msgid,
                         struct hci_dbg_reset_rx_cnt_cmd const *param,
                         kernel_task_id_t const dest_id,
                         kernel_task_id_t const src_id)
{
    // structure type for the complete command event
    struct hci_basic_cmd_cmp_evt *event;

    // allocate the status event message
    event =  KERNEL_MSG_ALLOC(HCI_CMD_CMP_EVENT , src_id, HCI_DBG_RESET_RX_CNT_CMD_OPCODE, hci_basic_cmd_cmp_evt);
    ble_rxccmpktcnt0_set((uint8_t)param->conhdl,0);
    ble_rxccmpktcnt1_set((uint8_t)param->conhdl,0);
    ble_rxccmpktcnt2_set((uint8_t)param->conhdl,0);
    event->status = COMMON_ERROR_NO_ERROR;
    // sends the message
    hci_send_2_host(event);

    return (KERNEL_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handle the reception of the vendor specific command reset tx counter in the CS.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int
hci_dbg_reset_tx_cnt_cmd_handler(kernel_msg_id_t const msgid,
                         struct hci_dbg_reset_tx_cnt_cmd const *param,
                         kernel_task_id_t const dest_id,
                         kernel_task_id_t const src_id)
{
    // structure type for the complete command event
    struct hci_basic_cmd_cmp_evt *event;

    // allocate the status event message
    event =  KERNEL_MSG_ALLOC(HCI_CMD_CMP_EVENT , src_id, HCI_DBG_RESET_TX_CNT_CMD_OPCODE, hci_basic_cmd_cmp_evt);
    ble_txccmpktcnt0_set((uint8_t)param->conhdl,0);
    ble_txccmpktcnt1_set((uint8_t)param->conhdl,0);
    ble_txccmpktcnt2_set((uint8_t)param->conhdl,0);
    event->status = COMMON_ERROR_NO_ERROR;
    // sends the message
    hci_send_2_host(event);

    return (KERNEL_MSG_CONSUMED);
}
#if (BLE_OBSERVER || BLE_CENTRAL || BLE_PERIPHERAL)
/**
 ****************************************************************************************
 * @brief Handle the reception of the vendor specific command Set Tx power Level in CS.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int
hci_dbg_set_tx_pw_cmd_handler (kernel_msg_id_t const msgid,
                            struct hci_dbg_set_tx_pw_cmd const *param,
                            kernel_task_id_t const dest_id,
                            kernel_task_id_t const src_id)
{
    // structure type for the complete command event
    struct hci_basic_cmd_cmp_evt *event= KERNEL_MSG_ALLOC(HCI_CMD_CMP_EVENT , src_id, HCI_DBG_SET_TX_PW_CMD_OPCODE, hci_basic_cmd_cmp_evt);

    // Check if the command is for the advertising channel or a connection link
    if (param->conhdl > LLD_ADV_HDL)
    {
        event->status = dbg_task_txpwr_update(param->conhdl, param->pw_lvl);
    }
    else
    {
        event->status = COMMON_ERROR_INVALID_HCI_PARAM;
    }

    // send the message
    hci_send_2_host(event);

    // Go to the IDLE state
    kernel_state_set(dest_id, DBG_IDLE);

    return (KERNEL_MSG_CONSUMED);
}
#endif //(BLE_OBSERVER || BLE_CENTRAL || BLE_PERIPHERAL)
#endif //BLE_EMB_PRESENT

#if (RW_WLAN_COEX)
/**
 ****************************************************************************************
 * @brief Handle the reception of the vendor specific command wlan coexistence.
 * This command set the coexistence state.
 *
 * @note command complete(success) is sent by ROM code.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int
hci_dbg_wlan_coex_cmd_handler(kernel_msg_id_t const msgid, struct hci_dbg_wlan_coex_cmd const *param,
        kernel_task_id_t const dest_id, kernel_task_id_t const src_id)
{
    // structure type for the complete command event
    struct hci_basic_cmd_cmp_evt *event = KERNEL_MSG_ALLOC(HCI_CMD_CMP_EVENT , src_id, HCI_DBG_WLAN_COEX_CMD_OPCODE, hci_basic_cmd_cmp_evt);

    rwip_wlcoex_set(param->state);

    // If reason is not valid, returns a CC event with bad status
    event->status = COMMON_ERROR_NO_ERROR;
    hci_send_2_host(event);

    return (KERNEL_MSG_CONSUMED);
}

#if (RW_WLAN_COEX_TEST)
/**
 ****************************************************************************************
 * @brief Handle the reception of the vendor specific command wlan coexistence.
 * This command set the coexistence state.
 *
 * @note command complete(success) is sent by ROM code.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int
hci_dbg_wlan_coextst_scen_cmd_handler(kernel_msg_id_t const msgid, struct hci_dbg_wlan_coextst_scen_cmd const *param,
        kernel_task_id_t const dest_id, kernel_task_id_t const src_id)
{

    // structure type for the complete command event
    struct hci_basic_cmd_cmp_evt *event = KERNEL_MSG_ALLOC(HCI_CMD_CMP_EVENT , src_id, HCI_DBG_WLAN_COEXTST_SCEN_CMD_OPCODE, hci_basic_cmd_cmp_evt);

    event->status = dbg_wlcoex_scen_set(param->scenario);

    // sends the message
    hci_send_2_host(event);

    return (KERNEL_MSG_CONSUMED);
}
#endif //RW_WLAN_COEX_TEST
#endif //RW_WLAN_COEX

#if (RW_MWS_COEX)
/**
 ****************************************************************************************
 * @brief Handle the reception of the vendor specific command mws coexistence.
 * This command set the coexistence state.
 *
 * @note command complete(success) is sent by ROM code.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int
hci_dbg_mws_coex_cmd_handler(kernel_msg_id_t const msgid, struct hci_dbg_mws_coex_cmd const *param,
        kernel_task_id_t const dest_id, kernel_task_id_t const src_id)
{

    // structure type for the complete command event
    struct hci_basic_cmd_cmp_evt *event = KERNEL_MSG_ALLOC(HCI_CMD_CMP_EVENT , src_id, HCI_DBG_MWS_COEX_CMD_OPCODE, hci_basic_cmd_cmp_evt);

    rwip_mwscoex_set(param->state);

    // If reason is not valid, returns a CC event with bad status
    event->status = COMMON_ERROR_NO_ERROR;
    hci_send_2_host(event);

    return (KERNEL_MSG_CONSUMED);
}

#if (RW_MWS_COEX_TEST)
/**
 ****************************************************************************************
 * @brief Handle the reception of the vendor specific command mws coexistence.
 * This command set the coexistence state.
 *
 * @note command complete(success) is sent by ROM code.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int
hci_dbg_mws_coextst_scen_cmd_handler(kernel_msg_id_t const msgid, struct hci_dbg_mws_coextst_scen_cmd const *param,
        kernel_task_id_t const dest_id, kernel_task_id_t const src_id)
{

    // structure type for the complete command event
    struct hci_basic_cmd_cmp_evt *event = KERNEL_MSG_ALLOC(HCI_CMD_CMP_EVENT , src_id, HCI_DBG_MWS_COEXTST_SCEN_CMD_OPCODE, hci_basic_cmd_cmp_evt);

    event->status = dbg_mwscoex_scen_set(param->scenario);

    // sends the message
    hci_send_2_host(event);

    return (KERNEL_MSG_CONSUMED);
}
#endif //RW_MWS_COEX_TEST
#endif //RW_MWS_COEX

#if (BLE_EMB_PRESENT)
#if (BLE_AUDIO)
/**
 ****************************************************************************************
 * @brief Handle the reception of the vendor specific command configure
 *        audio link request.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int
hci_vs_audio_configure_cmd_handler(kernel_msg_id_t const msgid, struct hci_vs_audio_configure_cmd const *param,
                         kernel_task_id_t const dest_id, kernel_task_id_t const src_id)
{
    // structure type for the complete command event
    struct hci_basic_conhdl_cmd_cmp_evt *event =
            KERNEL_MSG_ALLOC(HCI_CMD_CMP_EVENT , param->conhdl, msgid, hci_basic_conhdl_cmd_cmp_evt);

    event->status = audio_configure(param->voice_channel,(struct audio_configure_tag*)&param->tx_size);
    event->conhdl = param->conhdl;

    // sends the message
    hci_send_2_host(event);

    return (KERNEL_MSG_CONSUMED);
}
/**
 ****************************************************************************************
 * @brief Handle the reception of the vendor specific command configure
 *        audio link request.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int
hci_vs_audio_set_pointer_cmd_handler(kernel_msg_id_t const msgid, struct hci_vs_audio_set_pointer_cmd const *param,
                         kernel_task_id_t const dest_id, kernel_task_id_t const src_id)
{
    // structure type for the complete command event
    struct hci_basic_conhdl_cmd_cmp_evt *event =
            KERNEL_MSG_ALLOC(HCI_CMD_CMP_EVENT , param->conhdl, msgid, hci_basic_conhdl_cmd_cmp_evt);

    event->status = audio_set_pointer(param->voice_channel,param->rx_tx_select,param->tog,param->em_ptr);
    event->conhdl = param->conhdl;

    // sends the message
    hci_send_2_host(event);

    return (KERNEL_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handle the reception of the vendor specific command configure
 *        audio link request.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int
hci_vs_audio_get_buffer_range_cmd_handler(kernel_msg_id_t const msgid, struct hci_vs_audio_get_buffer_range_cmd const *param,
                         kernel_task_id_t const dest_id, kernel_task_id_t const src_id)
{
    // structure type for the complete command event
    struct hci_vs_audio_get_buffer_range_cmd_cmp_evt *event = 
            KERNEL_MSG_ALLOC(HCI_CMD_CMP_EVENT, 0, msgid, hci_vs_audio_get_buffer_range_cmd_cmp_evt);

    event->status = audio_get_buffer_range(param->rx_tx_select,&event->base_address,&event->end_address);

    // sends the message
    hci_send_2_host(event);

    return (KERNEL_MSG_CONSUMED);
}
/**
 ****************************************************************************************
 * @brief Handle the reception of the vendor specific command set
 *        audio link mode request.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int
hci_vs_audio_set_mode_cmd_handler(kernel_msg_id_t const msgid, struct hci_vs_audio_set_mode_cmd const *param,
                         kernel_task_id_t const dest_id, kernel_task_id_t const src_id)
{
    // structure type for the complete command event
    struct hci_basic_conhdl_cmd_cmp_evt *event =
            KERNEL_MSG_ALLOC(HCI_CMD_CMP_EVENT , param->conhdl, msgid, hci_basic_conhdl_cmd_cmp_evt);

    event->status = audio_set_mode(param->voice_channel,param->mode);
    event->conhdl = param->conhdl;

    // sends the message
    hci_send_2_host(event);

    return (KERNEL_MSG_CONSUMED);
}


/**
 ****************************************************************************************
 * @brief Handle the reception of the vendor specific command set
 *        audio link mode request.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int
hci_vs_audio_reset_cmd_handler(kernel_msg_id_t const msgid, struct hci_vs_audio_reset_cmd const *param,
                         kernel_task_id_t const dest_id, kernel_task_id_t const src_id)
{
    // structure type for the complete command event
    struct hci_basic_conhdl_cmd_cmp_evt *event =
            KERNEL_MSG_ALLOC(HCI_CMD_CMP_EVENT , param->conhdl, msgid, hci_basic_conhdl_cmd_cmp_evt);

    event->status = audio_reset(param->voice_channel);
    event->conhdl = param->conhdl;

    // sends the message
    hci_send_2_host(event);

    return (KERNEL_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handle the reception of the vendor specific command set
 *        audio link mode request.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int
hci_vs_audio_allocate_cmd_handler(kernel_msg_id_t const msgid, struct hci_vs_audio_allocate_cmd const *param,
                         kernel_task_id_t const dest_id, kernel_task_id_t const src_id)
{
    // structure type for the complete command event
    struct hci_vs_audio_allocate_cmd_cmp_evt *event =
            KERNEL_MSG_ALLOC(HCI_CMD_CMP_EVENT, param->conhdl, msgid, hci_vs_audio_allocate_cmd_cmp_evt);
    uint8_t voice_channel;
    event->status        = audio_allocate(param->conhdl, AUDIO_MODE_0, &voice_channel);
    event->voice_channel = voice_channel;
    event->conhdl        = param->conhdl;

    // sends the message
    hci_send_2_host(event);

    return (KERNEL_MSG_CONSUMED);
}
#endif // (BLE_AUDIO)
#endif //BLE_EMB_PRESENT

/// The message handlers for HCI command complete events
static const struct kernel_msg_handler dbg_hci_command_handler_tab[] =
{
        {HCI_DBG_RD_MEM_CMD_OPCODE,         (kernel_msg_func_t)hci_dbg_rd_mem_cmd_handler},
        {HCI_DBG_WR_MEM_CMD_OPCODE,         (kernel_msg_func_t)hci_dbg_wr_mem_cmd_handler},
        {HCI_DBG_DEL_PAR_CMD_OPCODE,        (kernel_msg_func_t)hci_dbg_del_par_cmd_handler},
        {HCI_DBG_ID_FLASH_CMD_OPCODE,       (kernel_msg_func_t)hci_dbg_id_flash_cmd_handler},
        {HCI_DBG_ER_FLASH_CMD_OPCODE,       (kernel_msg_func_t)hci_dbg_er_flash_cmd_handler},
        {HCI_DBG_WR_FLASH_CMD_OPCODE,       (kernel_msg_func_t)hci_dbg_wr_flash_cmd_handler},
        {HCI_DBG_RD_FLASH_CMD_OPCODE,       (kernel_msg_func_t)hci_dbg_rd_flash_cmd_handler},
        {HCI_DBG_RD_PAR_CMD_OPCODE,         (kernel_msg_func_t)hci_dbg_rd_par_cmd_handler},
        {HCI_DBG_WR_PAR_CMD_OPCODE,         (kernel_msg_func_t)hci_dbg_wr_par_cmd_handler},
        {HCI_DBG_RD_KERNEL_STATS_CMD_OPCODE,    (kernel_msg_func_t)hci_dbg_rd_kernel_stats_cmd_handler},
        {HCI_DBG_RF_REG_RD_CMD_OPCODE,      (kernel_msg_func_t)hci_dbg_rf_reg_rd_cmd_handler},
        {HCI_DBG_RF_REG_WR_CMD_OPCODE,      (kernel_msg_func_t)hci_dbg_rf_reg_wr_cmd_handler},
        {HCI_DBG_PLF_RESET_CMD_OPCODE,      (kernel_msg_func_t)hci_dbg_plf_reset_cmd_handler},
        #if (BLE_EMB_PRESENT)
        {HCI_DBG_HW_REG_RD_CMD_OPCODE,      (kernel_msg_func_t)hci_dbg_hw_reg_rd_cmd_handler},
        {HCI_DBG_HW_REG_WR_CMD_OPCODE,      (kernel_msg_func_t)hci_dbg_hw_reg_wr_cmd_handler},
        {HCI_DBG_SET_BD_ADDR_CMD_OPCODE,    (kernel_msg_func_t)hci_dbg_set_bd_addr_cmd_handler},
        {HCI_DBG_SET_CRC_CMD_OPCODE,        (kernel_msg_func_t)hci_dbg_set_crc_cmd_handler},
        #if (BLE_PERIPHERAL || BLE_CENTRAL)
        {HCI_DBG_LLCP_DISCARD_CMD_OPCODE,   (kernel_msg_func_t)hci_dbg_llcp_discard_cmd_handler},
        #endif // #if (BLE_PERIPHERAL || BLE_CENTRAL)
        {HCI_DBG_RESET_RX_CNT_CMD_OPCODE,   (kernel_msg_func_t)hci_dbg_reset_rx_cnt_cmd_handler},
        {HCI_DBG_RESET_TX_CNT_CMD_OPCODE,   (kernel_msg_func_t)hci_dbg_reset_tx_cnt_cmd_handler},
        #if (BLE_OBSERVER || BLE_CENTRAL || BLE_PERIPHERAL)
        {HCI_DBG_SET_TX_PW_CMD_OPCODE,      (kernel_msg_func_t)hci_dbg_set_tx_pw_cmd_handler},
        #endif //(BLE_OBSERVER || BLE_CENTRAL || BLE_PERIPHERAL)
        #endif //BLE_EMB_PRESENT
        #if (RW_WLAN_COEX)
        {HCI_DBG_WLAN_COEX_CMD_OPCODE,      (kernel_msg_func_t)hci_dbg_wlan_coex_cmd_handler},
        #if (RW_WLAN_COEX_TEST)
        {HCI_DBG_WLAN_COEXTST_SCEN_CMD_OPCODE, (kernel_msg_func_t)hci_dbg_wlan_coextst_scen_cmd_handler},
        #endif //RW_WLAN_COEX_TEST
        #endif //RW_WLAN_COEX
        #if (RW_MWS_COEX)
        {HCI_DBG_MWS_COEX_CMD_OPCODE,      (kernel_msg_func_t)hci_dbg_mws_coex_cmd_handler},
        #if (RW_MWS_COEX_TEST)
        {HCI_DBG_MWS_COEXTST_SCEN_CMD_OPCODE, (kernel_msg_func_t)hci_dbg_mws_coextst_scen_cmd_handler},
        #endif //RW_MWS_COEX_TEST
        #endif //RW_MWS_COEX
        {HCI_DBG_RD_MEM_INFO_CMD_OPCODE,    (kernel_msg_func_t)hci_dbg_rd_mem_info_cmd_handler},
        #if (BLE_EMB_PRESENT)
        #if (BLE_PERIPHERAL || BLE_CENTRAL)
        {HCI_DBG_WR_DLE_DFT_VALUE_CMD_OPCODE, (kernel_msg_func_t)hci_dbg_wr_dle_dft_value_cmd_handler},
        #if(BLE_AUDIO)
        {HCI_VS_AUDIO_ALLOCATE_CMD_OPCODE, (kernel_msg_func_t)hci_vs_audio_allocate_cmd_handler},
        {HCI_VS_AUDIO_CONFIGURE_CMD_OPCODE, (kernel_msg_func_t)hci_vs_audio_configure_cmd_handler},
        {HCI_VS_AUDIO_SET_MODE_CMD_OPCODE, (kernel_msg_func_t)hci_vs_audio_set_mode_cmd_handler},
        {HCI_VS_AUDIO_RESET_CMD_OPCODE, (kernel_msg_func_t)hci_vs_audio_reset_cmd_handler},
        {HCI_VS_AUDIO_SET_POINTER_CMD_OPCODE, (kernel_msg_func_t)hci_vs_audio_set_pointer_cmd_handler},
        {HCI_VS_AUDIO_GET_BUFFER_RANGE_CMD_OPCODE, (kernel_msg_func_t)hci_vs_audio_get_buffer_range_cmd_handler},
        #endif // #if (BLE_AUDIO)
        #endif // #if (BLE_PERIPHERAL || BLE_CENTRAL)
        #endif
};

/**
 ****************************************************************************************
 * @brief Handles any HCI command
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int
hci_command_handler(kernel_msg_id_t const msgid,
        void const *param,
        kernel_task_id_t const dest_id,
        kernel_task_id_t const src_id)
{
    int return_status = KERNEL_MSG_CONSUMED;

    // Check if there is a handler corresponding to the original command opcode
    for(int i = 0; i < (sizeof(dbg_hci_command_handler_tab)/sizeof(dbg_hci_command_handler_tab[0])); i++)
    {
        // Check if opcode matches
        if(dbg_hci_command_handler_tab[i].id == src_id)
        {
            // Check if there is a handler function
            if(dbg_hci_command_handler_tab[i].func != NULL)
            {
                // Call handler
                return_status = dbg_hci_command_handler_tab[i].func(src_id, param, dest_id, src_id);
            }
            break;
        }
    }

    return return_status;
}

/*
 * TASK DESCRIPTOR DEFINITIONS
 ****************************************************************************************
 */

/// Specifies the message handlers that are common to all states.
static const struct kernel_msg_handler dbg_default_state[] =
{
    {HCI_COMMAND,         (kernel_msg_func_t)hci_command_handler},
};

/// Specifies the message handlers that are common to all states.
const struct kernel_state_handler dbg_default_handler = KERNEL_STATE_HANDLER(dbg_default_state);

/// Defines the placeholder for the states of all the task instances.
kernel_state_t dbg_state[DBG_IDX_MAX];

#endif // (RW_DEBUG && (BT_EMB_PRESENT || BLE_EMB_PRESENT))
/// @} DBGTASK
