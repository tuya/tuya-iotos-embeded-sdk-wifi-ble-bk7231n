/**
****************************************************************************************
*
* @file rwip.c
*
* @brief RW IP SW main module
*
* Copyright (C) RivieraWaves 2009-2015
*
*
****************************************************************************************
*/

/**
 ****************************************************************************************
 * @addtogroup RW IP SW main module
 * @ingroup ROOT
 * @brief The RW IP SW main module.
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"     // RW SW configuration

#include <string.h>          // for mem* functions
#include "rwip.h"            // RW definitions
#include "architect.h"            // Platform architecture definition
#include "BK3435_reg.h"
#include "ble_rf.h"

#if (NVDS_SUPPORT)
#include "nvds.h"         // NVDS definitions
#endif // NVDS_SUPPORT

#if (BT_EMB_PRESENT)
#include "rwbt.h"            // rwbt definitions
#endif //BT_EMB_PRESENT

#if (BLE_EMB_PRESENT)
#include "rwble.h"           // rwble definitions
#endif //BLE_EMB_PRESENT

#if (BLE_HOST_PRESENT)
#include "rwble_hl.h"        // BLE HL definitions
#include "gapc.h"
#include "gattc.h"
#include "l2cc.h"
#endif //BLE_HOST_PRESENT

#if (BLE_APP_PRESENT)
#include "application.h"             // Application definitions
#endif //BLE_APP_PRESENT

#if (DEEP_SLEEP)
#if (BLE_EMB_PRESENT)
#include "lld_sleep.h"       // definitions for sleep mode
#endif //BLE_EMB_PRESENT
#endif //DEEP_SLEEP

#if (BLE_EMB_PRESENT)
#include "llcontrl.h"
#endif //BLE_EMB_PRESENT

#if (BLE_EMB_PRESENT)
#include "lld_wlcoex.h"
#endif //BLE_EMB_PRESENT
#if (BT_EMB_PRESENT)
#include "ld.h"
#endif //BT_EMB_PRESENT

#if (DISPLAY_SUPPORT)
#include "display.h"         // display definitions
#endif //DISPLAY_SUPPORT

#if (EA_PRESENT)
#include "ea.h"              // Event Arbiter definitions
#endif //EA_PRESENT

#if (BT_EMB_PRESENT || BLE_EMB_PRESENT)
#include "plf.h"             // platform definition

#endif //BT_EMB_PRESENT || BLE_EMB_PRESENT

#if (H4TL_SUPPORT)
#include "h4tl.h"
#endif //H4TL_SUPPORT

#if (AHI_TL_SUPPORT)
#include "ahi.h"
#endif //AHI_TL_SUPPORT

#if (HCI_PRESENT)
#include "hci.h"             // HCI definition
#endif //HCI_PRESENT

#if (KERNEL_SUPPORT)
#include "kernel.h"              // kernel definition
#include "kernel_event.h"        // kernel event
#include "kernel_timer.h"        // definitions for timer
#include "kernel_mem.h"          // kernel memory manager
#endif //KERNEL_SUPPORT

#if (SECURE_CONNECTIONS && (BT_EMB_PRESENT || BLE_EMB_PRESENT))
#include "ecc_p256.h"        // ECC P256 library
#endif // (SECURE_CONNECTIONS && (BT_EMB_PRESENT || BLE_EMB_PRESENT))

#include "dbg.h"             // debug definition

#include "driver_sys_ctrl.h"
#include "uart_pub.h"
#include "ble.h"
#include "ble_pub.h"
#include "ble_api.h"

/*
 * DEFINES
 ****************************************************************************************
 */

/// Time information
typedef struct
{
    /// Time in 625 us step.
    uint32_t time;
    /// number of us before getting next tick
    uint32_t next_tick;
} rwip_time_t;
 

#if (DEEP_SLEEP)
/// Sleep Duration Value in periodic wake-up mode
#define MAX_SLEEP_DURATION_PERIODIC_WAKEUP     0x0320*20  //10s // 0.5s   
/// Sleep Duration Value in external wake-up mode
#define MAX_SLEEP_DURATION_EXTERNAL_WAKEUP     0x3E80*3  //30s //10s

#define MAX_SLEEP_DURATION_SHORT_WAKEUP     160

#endif //DEEP_SLEEP



// Heap header size is 12 bytes
#define RWIP_HEAP_HEADER             (12 / sizeof(uint32_t))
// ceil(len/sizeof(uint32_t)) + RWIP_HEAP_HEADER
#define RWIP_CALC_HEAP_LEN(len)      ((((len) + (sizeof(uint32_t) - 1)) / sizeof(uint32_t)) + RWIP_HEAP_HEADER)

/*
 * STRUCT DEFINITIONS
 ****************************************************************************************
 */
#if (BLE_EMB_PRESENT || BT_EMB_PRESENT)
/*
/// Local supported commands
const struct rwip_prio rwip_priority[RWIP_PRIO_IDX_MAX]={
   #if (BT_EMB_PRESENT)
   {RWIP_PRIO_ACL_DFT,       RWIP_INCR_ACL_DFT},
   {RWIP_PRIO_ACL_ACT,       RWIP_INCR_ACL_ACT},
   {RWIP_PRIO_ACL_RSW,       RWIP_INCR_ACL_RSW},
   {RWIP_PRIO_ACL_SNIFF_DFT, RWIP_INCR_ACL_SNIFF_DFT},
   {RWIP_PRIO_ACL_SNIFF_TRANS,RWIP_INCR_ACL_SNIFF_TRANS},
   #if MAX_NB_SYNC
   {RWIP_PRIO_SCO_DFT,       RWIP_INCR_SCO_DFT},
   #endif //MAX_NB_SYNC
   {RWIP_PRIO_BCST_DFT,      RWIP_INCR_BCST_DFT},
   {RWIP_PRIO_BCST_ACT,      RWIP_INCR_BCST_ACT},
   {RWIP_PRIO_CSB_RX_DFT,    RWIP_INCR_CSB_RX_DFT},
   {RWIP_PRIO_CSB_TX_DFT,    RWIP_INCR_CSB_TX_DFT},
   {RWIP_PRIO_INQ_DFT,       RWIP_INCR_INQ_DFT},
   {RWIP_PRIO_ISCAN_DFT,     RWIP_INCR_ISCAN_DFT},
   {RWIP_PRIO_PAGE_DFT,      RWIP_INCR_PAGE_DFT},
{RWIP_PRIO_PAGE_1ST_PKT,  RWIP_INCR_PAGE_1ST_PKT},
   {RWIP_PRIO_PCA_DFT,       RWIP_INCR_PCA_DFT},
   {RWIP_PRIO_PSCAN_DFT,     RWIP_INCR_PSCAN_DFT},
   {RWIP_PRIO_PSCAN_1ST_PKT, RWIP_INCR_PSCAN_1ST_PKT},
   {RWIP_PRIO_SSCAN_DFT,     RWIP_INCR_SSCAN_DFT},
   {RWIP_PRIO_STRAIN_DFT,    RWIP_INCR_STRAIN_DFT},
   #endif // #if (BT_EMB_PRESENT)
   #if (BLE_EMB_PRESENT)
   {RWIP_PRIO_SCAN_DFT,         RWIP_INCR_SCAN_DFT},
   {RWIP_PRIO_INIT_DFT,         RWIP_INCR_INIT_DFT},
   {RWIP_PRIO_LE_ESTAB_DFT,     RWIP_INCR_LE_ESTAB_DFT},
   {RWIP_PRIO_LE_CON_IDLE_DFT,  RWIP_INCR_LE_CON_IDLE_DFT},
   {RWIP_PRIO_LE_CON_ACT_DFT,   RWIP_INCR_LE_CON_ACT_DFT},
   {RWIP_PRIO_ADV_DFT,          RWIP_INCR_ADV_DFT},
   {RWIP_PRIO_ADV_HDC_DFT,      RWIP_INCR_ADV_HDC_PRIO_DFT},
   #endif // #if (BLE_EMB_PRESENT)
};
*/
/*
const uint8_t rwip_coex_cfg[RWIP_COEX_CFG_MAX]={
    #if (BT_EMB_PRESENT)
    [RWIP_COEX_MSSWITCH_IDX]  = (RWIP_PTI_TXEN << RWIP_TXBSY_POS)  | (RWIP_PTI_RXEN << RWIP_RXBSY_POS)  | (RWIP_PTI_DNABORTDIS << RWIP_DNABORT_POS) \
            | (RWIP_MWS_TXEN << RWIP_MWSTXDSB_POS) | (RWIP_MWS_RXEN << RWIP_MWSRXDSB_POS),
    [RWIP_COEX_SNIFFATT_IDX]  = (RWIP_PTI_TXEN << RWIP_TXBSY_POS)  | (RWIP_PTI_RXEN << RWIP_RXBSY_POS)  | (RWIP_PTI_DNABORTDIS << RWIP_DNABORT_POS) \
            | (RWIP_MWS_TXEN << RWIP_MWSTXDSB_POS) | (RWIP_MWS_RXEN << RWIP_MWSRXDSB_POS),
    [RWIP_COEX_PAGE_IDX    ]  = (RWIP_PTI_TXEN << RWIP_TXBSY_POS)  | (RWIP_PTI_RXEN << RWIP_RXBSY_POS)  | (RWIP_PTI_DNABORTEN << RWIP_DNABORT_POS) \
            | (RWIP_MWS_TXEN << RWIP_MWSTXDSB_POS) | (RWIP_MWS_RXEN << RWIP_MWSRXDSB_POS),
    [RWIP_COEX_PSCAN_IDX   ]  = (RWIP_PTI_TXEN << RWIP_TXBSY_POS)  | (RWIP_PTI_RXEN << RWIP_RXBSY_POS)  | (RWIP_PTI_DNABORTDIS << RWIP_DNABORT_POS) \
            | (RWIP_MWS_TXEN << RWIP_MWSTXDSB_POS) | (RWIP_MWS_RXEN << RWIP_MWSRXDSB_POS),
    [RWIP_COEX_INQ_IDX     ]  = (RWIP_PTI_TXEN << RWIP_TXBSY_POS)  | (RWIP_PTI_RXEN << RWIP_RXBSY_POS)  | (RWIP_PTI_DNABORTDIS << RWIP_DNABORT_POS) \
            | (RWIP_MWS_TXEN << RWIP_MWSTXDSB_POS) | (RWIP_MWS_RXEN << RWIP_MWSRXDSB_POS),
    [RWIP_COEX_INQRES_IDX  ]  = (RWIP_PTI_TXEN << RWIP_TXBSY_POS)  | (RWIP_PTI_RXEN << RWIP_RXBSY_POS)  | (RWIP_PTI_DNABORTDIS << RWIP_DNABORT_POS) \
            | (RWIP_MWS_TXEN << RWIP_MWSTXDSB_POS) | (RWIP_MWS_RXEN << RWIP_MWSRXDSB_POS),
    [RWIP_COEX_SCORSVD_IDX ]  = (RWIP_PTI_TXEN << RWIP_TXBSY_POS)  | (RWIP_PTI_RXEN << RWIP_RXBSY_POS)  | (RWIP_PTI_DNABORTDIS << RWIP_DNABORT_POS) \
            | (RWIP_MWS_TXEN << RWIP_MWSTXDSB_POS) | (RWIP_MWS_RXEN << RWIP_MWSRXDSB_POS),
    [RWIP_COEX_BCAST_IDX   ]  = (RWIP_PTI_TXEN << RWIP_TXBSY_POS)  | (RWIP_PTI_RXEN << RWIP_RXBSY_POS)  | (RWIP_PTI_DNABORTDIS << RWIP_DNABORT_POS) \
            | (RWIP_MWS_TXEN << RWIP_MWSTXDSB_POS) | (RWIP_MWS_RXEN << RWIP_MWSRXDSB_POS),
    [RWIP_COEX_CONNECT_IDX ]  = (RWIP_PTI_TXEN << RWIP_TXBSY_POS)  | (RWIP_PTI_RXEN << RWIP_RXBSY_POS)  | (RWIP_PTI_DNABORTDIS << RWIP_DNABORT_POS) \
            | (RWIP_MWS_TXEN << RWIP_MWSTXDSB_POS) | (RWIP_MWS_RXEN << RWIP_MWSRXDSB_POS),
    #endif // #if (BT_EMB_PRESENT)
    #if (BLE_EMB_PRESENT)
    [RWIP_COEX_CON_IDX]     = (uint8_t)((RWIP_PTI_TXDIS << RWIP_TXBSY_POS)  | (RWIP_PTI_RXDIS << RWIP_RXBSY_POS)  | (RWIP_PTI_DNABORTDIS << RWIP_DNABORT_POS)),
    [RWIP_COEX_CON_DATA_IDX]= (uint8_t)((RWIP_PTI_TXEN << RWIP_TXBSY_POS)  | (RWIP_PTI_RXEN << RWIP_RXBSY_POS)  | (RWIP_PTI_DNABORTDIS << RWIP_DNABORT_POS)),
    [RWIP_COEX_ADV_IDX]     = (uint8_t)((RWIP_PTI_TXEN << RWIP_TXBSY_POS)  | (RWIP_PTI_RXDIS << RWIP_RXBSY_POS) | (RWIP_PTI_DNABORTDIS << RWIP_DNABORT_POS)),
    [RWIP_COEX_SCAN_IDX]    = (uint8_t)((RWIP_PTI_TXDIS << RWIP_TXBSY_POS) | (RWIP_PTI_RXEN << RWIP_RXBSY_POS)  | (RWIP_PTI_DNABORTDIS << RWIP_DNABORT_POS)),
    [RWIP_COEX_INIT_IDX]    = (uint8_t)((RWIP_PTI_TXEN << RWIP_TXBSY_POS)  | (RWIP_PTI_RXEN << RWIP_RXBSY_POS)  | (RWIP_PTI_DNABORTDIS << RWIP_DNABORT_POS)),
    #endif // #if (BLE_EMB_PRESENT)
};

*/
#endif//#if (BLE_EMB_PRESENT || BT_EMB_PRESENT)

/// RWBT Environment
#if (DEEP_SLEEP)
struct rwip_env_tag
{
    /// Power_up delay
    uint32_t lp_cycle_wakeup_delay;
    /// Contains sleep duration accumulated timing error
    uint32_t sleep_acc_error;
    /// Duration of sleep and wake-up algorithm (depends on CPU speed) expressed in us.
    uint16_t sleep_algo_dur;
    /// Prevent sleep bit field
    uint16_t prevent_sleep;
    uint8_t wakeup_delay;
    bool sleep_enable;
    bool ext_wakeup_enable;
};
#endif //DEEP_SLEEP
/*
 * GLOBAL VARIABLES
 ****************************************************************************************
 */
#if 0
#define KERNEL_HEAP                    __attribute__((section("kernel_heap"),zero_init))
#else
#define KERNEL_HEAP 
#endif


#if (DEEP_SLEEP)
/// RW SW environment
static struct rwip_env_tag rwip_env;
#endif //DEEP_SLEEP
/// RF API
struct rwip_rf_api rwip_rf;

/// Heap definitions - use uint32 to ensure that memory blocks are 32bits aligned.
#if (KERNEL_MEM_RW)

/// Memory allocated for environment variables
uint32_t KERNEL_HEAP rwip_heap_env[RWIP_CALC_HEAP_LEN(RWIP_HEAP_ENV_SIZE)];
//uint32_t rwip_heap_env[RWIP_CALC_HEAP_LEN(RWIP_HEAP_ENV_SIZE)];
#if (BLE_HOST_PRESENT)
/// Memory allocated for Attribute database
uint32_t KERNEL_HEAP rwip_heap_db[RWIP_CALC_HEAP_LEN(RWIP_HEAP_DB_SIZE)];
//uint32_t rwip_heap_db[RWIP_CALC_HEAP_LEN(RWIP_HEAP_DB_SIZE)];
#endif // (BLE_HOST_PRESENT)

/// Memory allocated for kernel messages
//uint32_t rwip_heap_msg[RWIP_CALC_HEAP_LEN(RWIP_HEAP_MSG_SIZE)];
/// Non Retention memory block
//uint32_t rwip_heap_non_ret[RWIP_CALC_HEAP_LEN(RWIP_HEAP_NON_RET_SIZE)];

/// Memory allocated for kernel messages
uint32_t KERNEL_HEAP rwip_heap_msg[RWIP_CALC_HEAP_LEN(RWIP_HEAP_MSG_SIZE)];
/// Non Retention memory block
uint32_t KERNEL_HEAP rwip_heap_non_ret[RWIP_CALC_HEAP_LEN(RWIP_HEAP_NON_RET_SIZE)];

#endif // (KERNEL_MEM_RW)

/*
 * LOCAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */


#if (DEEP_SLEEP && BLE_EMB_PRESENT)
/**
 ****************************************************************************************
 * @brief  check If the fine counter is close the 3/4 of the slot boundary (624 >> 2) * 3 = 468
 *
 * @return when boundary wak-up is ok
 ****************************************************************************************
 */
bool rwip_check_wakeup_boundary(void)
{
    //Sample the base time count
    ble_samp_setf(1);
    while (ble_samp_getf());
    return  ((ble_finetimecnt_get() < 468) ? false : true);
}
#endif //DEEP_SLEEP && BLE_EMB_PRESENT


#if (DEEP_SLEEP)
/**
 ****************************************************************************************
 * @brief Converts a duration in slots into a number of low power clock cycles.
 * The function converts a duration in slots into a number of low power clock cycles.
 * Sleep clock runs at either 32768Hz or 32000Hz, so this function divides the value in
 * slots by 20.48 or 20 depending on the case.
 * To do this the following formulae are applied:
 *
 *   N = x * 20.48 = (2048 * x)/100 for a 32.768kHz clock or
 *   N = x * 20                     for a 32kHz clock
 *
 * @param[in] slot_cnt    The value in slot count
 *
 * @return The number of low power clock cycles corresponding to the slot count
 *
 ****************************************************************************************
 */
static uint32_t rwip_slot_2_lpcycles(uint32_t slot_cnt)
{
    uint32_t lpcycles;

    // Sanity check: The number of slots should not be too high to avoid overflow
    ASSERT_ERR(slot_cnt < 1000000);

    #if HZ32000
    // Compute the low power clock cycles - case of a 32kHz clock
    lpcycles = slot_cnt * 20;
    #else //HZ32000
    // Compute the low power clock cycles - case of a 32.768kHz clock
    lpcycles = (slot_cnt << 11)/100;
    #endif //HZ32000

    // Corner case, Sleep duration is exactly on slot boundary so slot interrupt will not
    // be generated on expected slot (but next one).

    // So reduce little bit sleep duration in order to allow fine time compensation
    // Note compensation will be in range of [1 , 2[ lp cycles

    lpcycles--;

    return(lpcycles);
}
#endif //(DEEP_SLEEP)

void rwip_reg_init(void)
{
    ble_set_power_up(1);
    
	//ICU_TL410_BLE_CLK = 0x0;  //enable ble clock  icu 0x04
	ble_clk_power_up();

    //REG_ICU_INT_ENABLE |= (1<<30);  //enable ble INT    icu 0x10
    ble_intc_set(1);
    
	REG_BLE_XVR_SLOT_TIME = 0x0D123B6D;  // BLE_XVR 0x2a
	REG_BLE_XVR_TX_CONFIG = REG_BLE_XVR_TX_CONFIG | 0x80; // BLE_XVR 0x30
	REG_BLE_XVR_AGC_CONFIG = 0x03371C02; // BLE_XVR 0x3c

    //ble_switch_rf_to_ble();
}

void rwip_reg_deinit(void)
{
    ble_switch_rf_to_wifi();
    
    ble_intc_set(0);
    
	ble_clk_power_down();

    ble_set_power_up(0);
}

void rwip_reg_reinit(void)
{
	ble_clk_power_up();

    ble_intc_set(1);
	//ble_switch_rf_to_ble();
}

/*
 * EXPORTED FUNCTION DEFINITIONS
 ****************************************************************************************
 */
extern uint8_t cur_read_buf_idx;
	
void rwip_init(uint32_t error)
{
    #if (NVDS_SUPPORT && DEEP_SLEEP)
    uint8_t length = 1;
    uint8_t sleep_enable;
    uint8_t ext_wakeup_enable;
    #endif //NVDS_SUPPORT && DEEP_SLEEP
    #if (DEEP_SLEEP)
    // Reset RW environment
    memset(&rwip_env, 0, sizeof(rwip_env));
    #endif //DEEPSLEEP
    #if (KERNEL_SUPPORT)
    // Initialize kernel
    kernel_init();
    // Initialize memory heap used by kernel.
    #if (KERNEL_MEM_RW)
    // Memory allocated for environment variables
    kernel_mem_init(KERNEL_MEM_ENV,           (uint8_t*)rwip_heap_env,     RWIP_HEAP_ENV_SIZE);
    #if (BLE_HOST_PRESENT)
    // Memory allocated for Attribute database
    kernel_mem_init(KERNEL_MEM_ATT_DB,        (uint8_t*)rwip_heap_db,      RWIP_HEAP_DB_SIZE);
    #endif // (BLE_HOST_PRESENT)
    // Memory allocated for kernel messages
    kernel_mem_init(KERNEL_MEM_KERNEL_MSG,        (uint8_t*)rwip_heap_msg,     RWIP_HEAP_MSG_SIZE);
    // Non Retention memory block
    kernel_mem_init(KERNEL_MEM_NON_RETENTION, (uint8_t*)rwip_heap_non_ret, RWIP_HEAP_NON_RET_SIZE);
    #endif // (KERNEL_MEM_RW)
    #endif //KERNEL_SUPPORT
    // Initialize RF
    #if (BT_EMB_PRESENT || BLE_EMB_PRESENT)
     rf_init(&rwip_rf);
    #endif //BT_EMB_PRESENT || BLE_EMB_PRESENT

    #if (SECURE_CONNECTIONS && (BT_EMB_PRESENT || BLE_EMB_PRESENT))
    // Initialize Diffie Hellman Elliptic Curve Algorithm
    ecc_init(false);
    #endif // (SECURE_CONNECTIONS && (BT_EMB_PRESENT || BLE_EMB_PRESENT))
    // Initialize H4 TL
    #if (H4TL_SUPPORT)
    #if (BLE_HOST_PRESENT)
    #if (BLE_EMB_PRESENT) // FULL HOST
    h4tl_init(H4TL_TYPE_AHI, rwip_eif_get(RWIP_EIF_AHI),&cur_read_buf_idx);
    #else // !(BLE_EMB_PRESENT)  // SPLIT HOST
    #if(AHI_TL_SUPPORT)
    h4tl_init(H4TL_TYPE_AHI, rwip_eif_get(RWIP_EIF_AHI),&cur_read_buf_idx);
    #endif // AHI_TL_SUPPORT
    h4tl_init(H4TL_TYPE_HCI, rwip_eif_get(RWIP_EIF_HCIH),&cur_read_buf_idx);
    #endif // (BLE_EMB_PRESENT)
    #else // !(BLE_HOST_PRESENT) // SPLIT EMB
    h4tl_init(H4TL_TYPE_HCI, rwip_eif_get(RWIP_EIF_HCIC),&cur_read_buf_idx);
    #endif // (BLE_HOST_PRESENT)
    #endif //(H4TL_SUPPORT)
    #if (HCI_PRESENT)
    // Initialize the HCI
    hci_init();
    #endif //HCI_PRESENT

    #if (AHI_TL_SUPPORT)
    // Initialize the Application Host Interface
    //  ahi_init();
    #endif //AHI_TL_SUPPORT

    #if (BLE_HOST_PRESENT)
    // Initialize BLE Host stack
    rwble_hl_init();
    #endif //BLE_HOST_PRESENT
    #if (BT_EMB_PRESENT)
    // Initialize BT
    rwbt_init();
    #endif //BT_EMB_PRESENT

    #if (BLE_EMB_PRESENT)
    // Initialize BLE
    rwble_init();
    #endif //BLE_EMB_PRESENT
    #if (BT_EMB_PRESENT || BLE_EMB_PRESENT)
    #if (RW_DEBUG)
    // Initialize the debug process
    dbg_init();
    #endif //(RW_DEBUG)
    #endif //(BT_EMB_PRESENT || BLE_EMB_PRESENT)

    #if (EA_PRESENT)
    //Initialize Event arbiter
    ea_init(false);
    #endif //(EA_PRESENT)

    #if 0// (NVDS_SUPPORT && DEEP_SLEEP)
    // Activate deep sleep feature if enabled in NVDS
    if(nvds_get(NVDS_TAG_SLEEP_ENABLE, &length, &sleep_enable) == NVDS_OK)
    {
        if(sleep_enable != 0)
        {
            rwip_env.sleep_enable = true;

            // Set max sleep duration depending on wake-up mode
            if(nvds_get(NVDS_TAG_EXT_WAKEUP_ENABLE, &length, &ext_wakeup_enable) == NVDS_OK)
            {
                if(ext_wakeup_enable != 0)
                {
                    rwip_env.ext_wakeup_enable = true;
                }
            }
        }
    }
	
    #endif //NVDS_SUPPORT && DEEP_SLEEP   
	
		
	#if ( DEEP_SLEEP)
		rwip_env.sleep_enable = true;
		rwip_env.ext_wakeup_enable = true;
        rwip_env.wakeup_delay = 3;///16;
	#endif		
	
	//rwip_env.ext_wakeup_enable = true;
    #if (BT_EMB_PRESENT || (BLE_EMB_PRESENT && !BLE_HOST_PRESENT))
    // If FW initializes due to FW reset, send the message to Host
    if(error != RESET_NO_ERROR)
    {
        if(error == RESET_TO_ROM || error == RESET_AND_LOAD_FW)
        {
            // Send platform reset command complete if requested by user
            ////
        }
        else
        {
            // Allocate a message structure for hardware error event
            struct hci_hw_err_evt *evt = KERNEL_MSG_ALLOC(HCI_EVENT, 0, HCI_HW_ERR_EVT_CODE, hci_hw_err_evt);

            // Fill the HW error code
            switch(error)
            {
                case RESET_MEM_ALLOC_FAIL: evt->hw_code = COMMON_ERROR_HW_MEM_ALLOC_FAIL; break;
                default: ASSERT_INFO(0, error, 0); break;
            }

            // Send the message
            hci_send_2_host(evt);
        }
    }
    #endif //(BT_EMB_PRESENT || (BLE_EMB_PRESENT && !BLE_HOST_PRESENT))
   /*
     ************************************************************************************
     * Application initialization
     ************************************************************************************
     */
    #if (BLE_APP_PRESENT)
    // Initialize APP
    appm_init();
    #endif //BLE_APP_PRESENT
}

void rwip_reset(void)
{
#if (NVDS_SUPPORT && DEEP_SLEEP)
    //uint8_t length = 1;
    //uint8_t sleep_enable;
    //uint8_t ext_wakeup_enable;
 #endif //NVDS_SUPPORT && DEEP_SLEEP

	
    // Disable interrupts until reset procedure is completed
    GLOBAL_INT_DIS();

    #if (KERNEL_SUPPORT)
    //Clear all message and timer pending
    kernel_flush();
    #endif //KERNEL_SUPPORT

    #if (SECURE_CONNECTIONS && (BT_EMB_PRESENT || BLE_EMB_PRESENT))
    // Reset Diffie Hellman Elliptic Curve Algorithm
    ecc_init(true);
    #endif // (SECURE_CONNECTIONS && (BT_EMB_PRESENT || BLE_EMB_PRESENT))

    #if (HCI_PRESENT)
    // Reset the HCI
    hci_reset();
    #endif //HCI_PRESENT

    #if (BT_EMB_PRESENT)
    // Reset BT
    rwbt_reset();
    #endif //BT_EMB_PRESENT

    #if (BLE_EMB_PRESENT)
    // Reset BLE
    rwble_reset();
    #endif //BLE_EMB_PRESENT

#if 0
	#if (NVDS_SUPPORT && DEEP_SLEEP)
    // Activate deep sleep feature if enabled in NVDS
    if(nvds_get(NVDS_TAG_SLEEP_ENABLE, &length, &sleep_enable) == NVDS_OK)
    {
        if(sleep_enable != 0)
        {
            rwip_env.sleep_enable    = true;
            rwip_env.sleep_acc_error = 0;

            // Set max sleep duration depending on wake-up mode
            if(nvds_get(NVDS_TAG_EXT_WAKEUP_ENABLE, &length, &ext_wakeup_enable) == NVDS_OK)
            {
                if(ext_wakeup_enable != 0)
                {
                    rwip_env.ext_wakeup_enable = true;
                }
            }

            // Set max sleep duration depending on wake-up mode
            length = NVDS_LEN_SLEEP_ALGO_DUR;
            if(nvds_get(NVDS_TAG_SLEEP_ALGO_DUR, &length,  (uint8_t*) &rwip_env.sleep_algo_dur) != NVDS_OK)
            {
                // set a default duration: 200 us
                rwip_env.sleep_algo_dur = 200;
            }
        }
    }
    #endif //NVDS_SUPPORT && DEEP_SLEEP
#endif

    #if (EA_PRESENT)
    ea_init(true);
    #endif //(EA_PRESENT)
  
    // Reset the RF
    rwip_rf.reset();

    #if (DISPLAY_SUPPORT)
    // Restart display module
    display_resume();
    #endif //DISPLAY_SUPPORT

    // Restore interrupts once reset procedure is completed
    GLOBAL_INT_RES();
}

void rwip_version(uint8_t* fw_version, uint8_t* hw_version)
{
    #if (BT_EMB_PRESENT)
    rwbt_version(fw_version , hw_version);
    #elif (BLE_EMB_PRESENT)
    rwble_version(fw_version , hw_version);
    #endif //BT_EMB_PRESENT / BLE_EMB_PRESENT
}

uint32_t rwip_get_current_time(void)
{
    return (ea_time_get_halfslot_rounded());
}

uint32_t rwip_get_next_target_time(void)
{
    return (ea_timer_next_target_get());
}

void rwip_schedule(void)
{
    #if (KERNEL_SUPPORT)
    #if (DEEP_SLEEP)
    // If system is waking up, delay the handling
    if ((rwip_env.prevent_sleep & RW_WAKERNEL_UP_ONGOING) == 0)
    #endif // DEEP_SLEEP

    {
        // schedule all pending events
        kernel_event_schedule();
			  
    }
    #endif //KERNEL_SUPPORT
}

uint8_t rwip_sleep(void)
{
    uint8_t proc_sleep = RW_NO_SLEEP;
    #if (DEEP_SLEEP)
    uint32_t sleep_duration;
	if(ble_ps_enabled())
	{
	sleep_duration = MAX_SLEEP_DURATION_EXTERNAL_WAKEUP;///MAX_SLEEP_DURATION_SHORT_WAKEUP;
	}
	else
	{
	sleep_duration = MAX_SLEEP_DURATION_EXTERNAL_WAKEUP;  //10s
	}
    #endif //DEEP_SLEEP

    DBG_SWDIAG(SLEEP, ALGO, 0);

    do
    {
        /************************************************************************
         **************            CHECK KERNEL EVENTS             **************
         ************************************************************************/
        // Check if some kernel processing is ongoing
        if (!kernel_sleep_check())
            {
            ble_ps_forbid_trace(BLE_PS_FORBID_KRL);
            break;
            }
				
        //Processor sleep can be enabled
        proc_sleep |= RW_MCU_IDLE_SLEEP;

        DBG_SWDIAG(SLEEP, ALGO, 1);

        #if (DEEP_SLEEP)
        /************************************************************************
         **************             CHECK ENABLE FLAG              **************
         ************************************************************************/
        // Check sleep enable flag
        if(!rwip_env.sleep_enable)
            {
            ble_ps_forbid_trace(BLE_PS_FORBID_ENABLED);
            break;
            }
        /************************************************************************
         **************              CHECK RW FLAGS                **************
         ************************************************************************/
        // First check if no pending procedure prevent from going to sleep
        if (rwip_env.prevent_sleep != 0)
		{
			//proc_sleep |= RW_MCU_IDLE_SLEEP; 
			ble_ps_forbid_trace(BLE_PS_FORBID_PREVENT);
			break;
		}
            
        DBG_SWDIAG(SLEEP, ALGO, 2);
        /************************************************************************
         **************           CHECK EXT WAKEUP FLAG            **************
         ************************************************************************/
        /* If external wakeup enable, sleep duration can be set to maximum, otherwise
         *  system should be woken-up periodically to poll incoming packets from HCI */
        if(!rwip_env.ext_wakeup_enable)
            sleep_duration = MAX_SLEEP_DURATION_PERIODIC_WAKEUP;

        #if (BLE_EMB_PRESENT)
        /************************************************************************
         * Wait until there's enough time for SLP to restore clocks when the chip
         * wakes up.
         ************************************************************************/
        while(!rwip_check_wakeup_boundary());
        #endif //(BLE_EMB_PRESENT)

        /************************************************************************
         **************            CHECK KERNEL TIMERS             **************
         ************************************************************************/
        // Compute the duration up to the next software timer expires
        if (!kernel_timer_sleep_check(&sleep_duration, rwip_env.wakeup_delay))
            {
            ble_ps_forbid_trace(BLE_PS_FORBID_TM);
            break;
            }

        DBG_SWDIAG(SLEEP, ALGO, 3);

        #if (BT_EMB_PRESENT)
        /************************************************************************
         **************                 CHECK BT                   **************
         ************************************************************************/
        if (!rwbt_sleep_check())
            {
            ble_ps_forbid_trace(BLE_PS_FORBID_RWBT);
            break;
            }
        #endif //BT_EMB_PRESENT

        #if (BLE_EMB_PRESENT)
        /************************************************************************
         **************                 CHECK BLE                   **************
         ************************************************************************/
        if (!rwble_sleep_check())
		{
            proc_sleep = RW_NO_SLEEP;
            ble_ps_forbid_trace(BLE_PS_FORBID_RWBLE);
   			break;
		}    
        #endif //(BLE_EMB_PRESENT)
        DBG_SWDIAG(SLEEP, ALGO, 4);

        /************************************************************************
         **************                 CHECK EA                   **************
         ************************************************************************/
        if (!ea_sleep_check(&sleep_duration, rwip_env.wakeup_delay))
		{
			//proc_sleep = false;
			ble_ps_forbid_trace(BLE_PS_FORBID_EA);
    		break;
		
		}
		
        DBG_SWDIAG(SLEEP, ALGO, 5);

        #if (H4TL_SUPPORT)
        /************************************************************************
         **************                 CHECK TL                   **************
         ************************************************************************/
        // Try to switch off TL
        if (!h4tl_stop())
        {
            proc_sleep = RW_NO_SLEEP;
            ble_ps_forbid_trace(BLE_PS_FORBID_H4TL);
            break;
        }
        #endif //H4TL_SUPPORT
				
		#if 0
        /************************************************************************
         **************                 CHECK UART                   **************
         ************************************************************************/
        // Try to switch off TL
        if (!check_uart_stop())
        {
            proc_sleep = false;
            break; 
        }
        #endif //PLF_UART

        DBG_SWDIAG(SLEEP, ALGO, 6);

        /************************************************************************
         **************          PROGRAM CORE DEEP SLEEP           **************
         ************************************************************************/

        // Put BT core into deep sleep
        lld_sleep_enter(rwip_slot_2_lpcycles(sleep_duration), rwip_env.ext_wakeup_enable);
        rwip_prevent_sleep_set(RW_SLEEP_EVENT_ONGOING);

        DBG_SWDIAG(SLEEP, SLEEP, 1);

        /************************************************************************
         **************               SWITCH OFF RF                **************
         ************************************************************************/
        rwip_rf.sleep();
		proc_sleep |= RW_MCU_DEEP_SLEEP;
        #endif // DEEP_SLEEP
			
    } while(0);

    return proc_sleep;
}

#if (DEEP_SLEEP)
void rwip_wakeup(void)
{
    DBG_SWDIAG(SLEEP, SLEEP, 0);

    // Prevent going to deep sleep until a slot interrupt is received
    rwip_prevent_sleep_set(RW_WAKERNEL_UP_ONGOING);

    #if (BT_EMB_PRESENT)
    // Wake-up BT core
    rwbt_sleep_wakeup();
    #elif (BLE_EMB_PRESENT)
    // Wake-up BLE core
    lld_sleep_wakeup();
    #endif //BT_EMB_PRESENT / BLE_EMB_PRESENT
}

void rwip_wakeup_end(void)
{
    if(rwip_env.prevent_sleep & RW_WAKERNEL_UP_ONGOING)
    {
        #if (BT_EMB_PRESENT)
        rwbt_sleep_wakeup_end();
        #elif (BLE_EMB_PRESENT)
        // Wake-up BLE core
        lld_sleep_wakeup_end();
        #endif //BT_EMB_PRESENT / BLE_EMB_PRESENT

        // Schedule the kernel timers
        kernel_event_set(KERNEL_EVENT_KERNEL_TIMER);

        // Wake up is complete now, so we allow the deep sleep again
        rwip_prevent_sleep_clear(RW_WAKERNEL_UP_ONGOING);

    }
}

void rwip_wakeup_delay_set(uint16_t wakeup_delay)
{
    rwip_env.lp_cycle_wakeup_delay = rwip_us_2_lpcycles(wakeup_delay);
}

void rwip_prevent_sleep_set(uint16_t prv_slp_bit)
{
    GLOBAL_INT_DIS();
    rwip_env.prevent_sleep |= prv_slp_bit;
    DBG_SWDIAG(SLEEP, PREVENT, rwip_env.prevent_sleep);
    GLOBAL_INT_RES();
}

void rwip_prevent_sleep_clear(uint16_t prv_slp_bit)
{
    GLOBAL_INT_DIS();
    rwip_env.prevent_sleep &= ~prv_slp_bit;
    DBG_SWDIAG(SLEEP, PREVENT, rwip_env.prevent_sleep);
    GLOBAL_INT_RES();
}

bool rwip_sleep_enable(void)
{
    return rwip_env.sleep_enable;
}

bool rwip_ext_wakeup_enable(void)
{
    return rwip_env.ext_wakeup_enable;
}

void ble_rwip_ps_dump(void)
{
    os_printf("bleprevent:%x sleep_enable:%x ext_wakeup_enable:%x\r\n",
        rwip_env.prevent_sleep,rwip_env.sleep_enable,rwip_env.ext_wakeup_enable);
}

/**
 ****************************************************************************************
 * @brief Converts a duration in lp cycles into a duration is us.
 *
 * The function converts a duration in lp cycles into a duration is us, according to the
 * low power clock frequency (32768Hz or 32000Hz).
 *
 * To do this the following formulae are applied:
 *
 *   Tus = x*30.517578125 = 30*x + x/2 + x/64 + x/512 for a 32.768kHz clock or
 *   Tus = x*31.25        = 31*x + x/4                for a 32kHz clock
 *
 *
 * @param[in] lpcycles    duration in lp cycles
 *
 * @return duration in us
 ****************************************************************************************
 */
uint32_t rwip_sleep_lpcycles_2_us(uint32_t lpcycles)
{
    uint32_t res;

    // Sanity check: The number of lp cycles should not be too high to avoid overflow
    ASSERT_ERR(lpcycles < 1000000);

    #if (HZ32000)
    // Compute the sleep duration in us - case of a 32kHz clock and insert previous computed error
    rwip_env.sleep_acc_error = lpcycles + rwip_env.sleep_acc_error;
    // get the truncated value
    res = rwip_env.sleep_acc_error >> 2;
    // retrieve new inserted error
    rwip_env.sleep_acc_error = rwip_env.sleep_acc_error - (res << 2);
    // finish computation
    res = (31 * lpcycles) + res;
    #else //HZ32000
    // Compute the sleep duration in half us - case of a 32.768kHz clock and insert previous computed error
    rwip_env.sleep_acc_error = ((lpcycles << 8) + (lpcycles << 3) + lpcycles) + rwip_env.sleep_acc_error;
    // get the truncated value
    res = rwip_env.sleep_acc_error >> 9;
    // retrieve new inserted error
    rwip_env.sleep_acc_error = rwip_env.sleep_acc_error - (res << 9);
    // finish computation
    res = (30 * lpcycles) + res;
    #endif //HZ32000

    return res;
}


/**
 ****************************************************************************************
 * @brief Converts a duration in us into a duration in lp cycles.
 *
 * The function converts a duration in us into a duration is lp cycles, according to the
 * low power clock frequency (32768Hz or 32000Hz).
 *
 * @param[in] us    duration in us
 *
 * @return duration in lpcycles
 ****************************************************************************************
 */
uint32_t rwip_us_2_lpcycles(uint32_t us)
{
    uint32_t lpcycles;

    #if (HZ32000)
    // Compute the low power clock cycles - case of a 32kHz clock
    lpcycles = (us * 32) / 1000;
    #else //HZ32000
    // Compute the low power clock cycles - case of a 32.768kHz clock
    lpcycles = (us * 32768) / 1000000;
    #endif //HZ32000

    return(lpcycles);
}

#else // DEEP_SLEEP

void rwip_wakeup_delay_set(uint16_t wakeup_delay)
{
    
}

void rwip_prevent_sleep_set(uint16_t prv_slp_bit)
{
  
}

void rwip_prevent_sleep_clear(uint16_t prv_slp_bit)
{
   
}

uint32_t rwip_sleep_lpcycles_2_us(uint32_t lpcycles)
{
	return 1;
}

uint32_t rwip_us_2_lpcycles(uint32_t us)
{
	return 1;
}

#endif// DEEP_SLEEP

#if (BT_EMB_PRESENT)

#if PCA_SUPPORT
bool rwip_pca_clock_dragging_only(void)
{
#if (BLE_EMB_PRESENT)
    return rwble_activity_ongoing_check();
#else
    return false;
#endif // BLE_EMB_PRESENT
}
#endif // PCA_SUPPORT
#endif // BT_EMB_PRESENT

#if (BT_EMB_PRESENT || BLE_EMB_PRESENT)
#if (RW_MWS_COEX)
void rwip_mwscoex_set(bool state)
{
#if (BLE_EMB_PRESENT)
    lld_mwscoex_set(state);
#endif // BLE_EMB_PRESENT
#if (BT_EMB_PRESENT)
    ld_mwscoex_set(state);
#endif // BT_EMB_PRESENT
}
#endif // RW_MWS_COEX
#if (RW_WLAN_COEX)
void rwip_wlcoex_set(bool state)
{
#if (BLE_EMB_PRESENT)
    lld_wlcoex_set(state);
#endif // BLE_EMB_PRESENT
#if (BT_EMB_PRESENT)
    ld_wlcoex_set(state);
#endif // BT_EMB_PRESENT
}
#endif // RW_WLAN_COEX
#endif // (BT_EMB_PRESENT || BLE_EMB_PRESENT)

#if RW_DEBUG
void rwip_assert_err(const char * file, int line, int param0, int param1)
{
    #if (BT_EMB_PRESENT || (BLE_EMB_PRESENT && !BLE_HOST_PRESENT))
    struct hci_dbg_assert_err_evt *evt = KERNEL_MSG_ALLOC_DYN(HCI_DBG_EVT, 0, 0, hci_dbg_assert_err_evt, sizeof(struct hci_dbg_assert_err_evt) + strlen(file));
    evt->subcode = HCI_DBG_ASSERT_ERR_EVT_SUBCODE;
    evt->line = line;
    evt->param0 = param0;
    evt->param1 = param1;
    strcpy((char *) evt->file, file);
    hci_send_2_host(evt);
    #endif //(BT_EMB_PRESENT || (BLE_EMB_PRESENT && !BLE_HOST_PRESENT))
}
#endif //RW_DEBUG

///@} RW
