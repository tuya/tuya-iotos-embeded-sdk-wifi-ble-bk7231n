/**
 * @file tuya_os_adapt_system.c
 * @brief ²Ù×÷ÏµÍ³Ïà¹Ø½Ó¿Ú
 * 
 * @copyright Copyright(C),2018-2020, Í¿Ñ»¿Æ¼¼ www.tuya.com
 * 
 */
#define _UNI_SYSTEM_GLOBAL

#include "tuya_os_adapt_system.h"
#include "tuya_os_adapt_wifi.h"
#include "tuya_os_adapter_error_code.h"
#include "tuya_os_adapt_output.h"

#include "wlan_ui_pub.h"
#include "mem_pub.h"
#include "wdt_pub.h"
#include "drv_model_pub.h"

#include "start_type_pub.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/***********************************************************
*************************micro define***********************
***********************************************************/

/***********************************************************
*************************variable define********************
***********************************************************/
static const TUYA_OS_SYSTEM_INTF m_tuya_os_system_intfs = {
    .get_systemtickcount   = tuya_os_adapt_get_systemtickcount,
    .get_tickratems        = tuya_os_adapt_get_tickratems,
    .system_sleep          = tuya_os_adapt_system_sleep,
    .system_reset          = tuya_os_adapt_system_reset,
    .watchdog_init_start   = tuya_os_adapt_watchdog_init_start,
    .watchdog_refresh      = tuya_os_adapt_watchdog_refresh,
    .watchdog_stop         = tuya_os_adapt_watchdog_stop,
    .system_getheapsize    = tuya_os_adapt_system_getheapsize,
    .system_get_rst_info   = tuya_os_adapt_system_get_rst_info,
    .system_get_rst_ext_info = NULL,
    .get_random_data       = tuya_os_adapt_get_random_data,
    .set_cpu_lp_mode       = tuya_os_adapt_set_cpu_lp_mode,
};

/***********************************************************
*************************function define********************
***********************************************************/
/**
 * @brief tuya_os_adapt_get_systemtickcountÓÃÓÚ»ñÈ¡ÏµÍ³ÔËÐÐticket 
 * @return SYS_TICK_T 
 */
SYS_TICK_T tuya_os_adapt_get_systemtickcount(void)
{
    return (SYS_TICK_T)xTaskGetTickCount();
}


/**
 * @brief tuya_os_adapt_get_tickratems
 * 
 * @return the time is ms of a system ticket
 */
unsigned int tuya_os_adapt_get_tickratems(void)
{
    return (unsigned int)portTICK_RATE_MS;
}


/**
 * @brief tuya_os_adapt_system_sleep
 * 
 * @param[in] msTime sleep time is ms
 */
void tuya_os_adapt_system_sleep(const unsigned long msTime)
{
    vTaskDelay((msTime)/(portTICK_RATE_MS));
}


/**
 * @brief tuya_os_adapt_system_isrstatus
 * 
 * @return true 
 * @return false 
 */
bool tuya_os_adapt_system_isrstatus(void)
{
    if(0 !=  bk_wlan_get_INT_status()) {
        return TRUE;
    }

    return FALSE;
}


/**
 * @brief tuya_os_adapt_system_reset
 * 
 */
void tuya_os_adapt_system_reset(void)
{
    LOG_NOTICE("tuya_os_adapt_system_reset\r\n");
    bk_reboot();
}


/***********************************************************
*  Function: tuya_os_adapt_watchdog_init_start 
*  Input: timeval
*  Output: none 
*  Return: void *
***********************************************************/
unsigned int tuya_os_adapt_watchdog_init_start(const unsigned int timeval)
{
    unsigned int ret;
    //init 
    int cyc_cnt = timeval * 1000;
    
    if(cyc_cnt > 0xFFFF) { /* 60s */
        cyc_cnt = 0xFFFF;
    }
    
    //init wdt
    ret = sddev_control(WDT_DEV_NAME, WCMD_SET_PERIOD, &cyc_cnt);
    
    // start wdt timer
    ret |= sddev_control(WDT_DEV_NAME, WCMD_POWER_UP, NULL);

    if(ret != 0) {
        LOG_ERR("watch dog set error!\r\n");
    }
    
    return timeval;
}

/***********************************************************
*  Function: tuya_os_adapt_watchdog_refresh 
*  Input: none
*  Output: none 
*  Return: void *
***********************************************************/
void tuya_os_adapt_watchdog_refresh(void)
{
    if(sddev_control(WDT_DEV_NAME, WCMD_RELOAD_PERIOD, NULL) != 0) {
        LOG_ERR("refresh watchdog err!\r\n");
    }
}

/***********************************************************
*  Function: tuya_os_adapt_watchdog_stop 
*  Input: none
*  Output: none 
*  Return: void *
***********************************************************/
void tuya_os_adapt_watchdog_stop(void)
{
    sddev_control(WDT_DEV_NAME, WCMD_POWER_DOWN, NULL);
}
/***********************************************************
*  Function: tuya_os_adapt_system_getheapsize 
*  Input: none
*  Output: none 
*  Return: int-> <0 means don't support to get heapsize
***********************************************************/
int tuya_os_adapt_system_getheapsize(void)
{
    return (int)xPortGetFreeHeapSize();
}

/**
 * @brief tuya_os_adapt_system_getMiniheapsize/×îÐ¡Ê£ÓàÄÚ´æ´óÐ¡
 * 
 * @return int <0: don't support  >=0: mini heap size/free memory
 */
int tuya_os_adapt_system_getMiniheapsize(void)
{
	return (int)xPortGetMinimumEverFreeHeapSize();
}

/***********************************************************
*  Function: system_get_rst_info 
*  Input: none
*  Output: none 
*  Return: char *->reset reason
***********************************************************/
TY_RST_REASON_E tuya_os_adapt_system_get_rst_info(void)
{
    unsigned char value = bk_misc_get_start_type() & 0xFF;
    TY_RST_REASON_E bk_value;
    
    switch(value) {
        case RESET_SOURCE_POWERON:
            bk_value = TY_RST_POWER_OFF;
            break;

        case RESET_SOURCE_REBOOT:
            bk_value = TY_RST_SOFTWARE;
            break;

        case RESET_SOURCE_WATCHDOG:
            bk_value = TY_RST_HARDWARE_WATCHDOG;
            break;

        case RESET_SOURCE_CRASH_XAT0:
        case RESET_SOURCE_CRASH_UNDEFINED:
        case RESET_SOURCE_CRASH_PREFETCH_ABORT:
        case RESET_SOURCE_CRASH_DATA_ABORT:
        case RESET_SOURCE_CRASH_UNUSED:
            bk_value = TY_RST_FATAL_EXCEPTION;
            break;

        default:
            bk_value = TY_RST_POWER_OFF;
            break;

    }

    LOG_NOTICE("bk_rst:%d tuya_rst:%d\r\n",value, bk_value);
    
    return bk_value;
}

/**
 * @brief tuya_os_adapt_get_random_data 
 * 
 * @param[in] range 
 * @return rand data
 */
int tuya_os_adapt_get_random_data(const unsigned int range)
{
    unsigned int trange = range;
    
    if(range == 0) {
        trange = 0xFF;
    }

    static char exec_flag = FALSE;

    if(!exec_flag) {
        exec_flag = TRUE;
    }

    return (rand() % trange);// + random_seed;
}


/**
 * @brief tuya_os_adapt_set_cpu_lp_mode -- set CPU lowpower mode
 * 
 * @param[in] en 
 * @param[in] mode
 * @return int 0=trueï¼Œ!0=false
 */
unsigned char cpu_lp_flag = 0;

int tuya_os_adapt_cpu_get_lp_mode(void)
{
    return cpu_lp_flag;
}
int tuya_os_adapt_set_cpu_lp_mode(const bool_t en,const TY_CPU_SLEEP_MODE_E mode)
{
    LOG_DEBUG("*******************************tuya_os_adapt_set_cpu_lp_mode,en = %d, mode = %d\r\n",en,mode);

    if(mode == TY_CPU_SLEEP) {
        if(en) {
            if(cpu_lp_flag == 0) {
                cpu_lp_flag = 1;                
                LOG_DEBUG("pmu_release_wakelock(PMU_OS)\r\n");
            }

            bk_wlan_mcu_ps_mode_enable();
            LOG_DEBUG("bk_wlan_mcu_ps_mode_enable()\r\n");
        }else {
            bk_wlan_mcu_ps_mode_disable();
            LOG_DEBUG("bk_wlan_mcu_ps_mode_disable()\r\n");
        }
    }else {
        return OPRT_OS_ADAPTER_CPU_LPMODE_SET_FAILED;
    }

    return OPRT_OS_ADAPTER_OK;
}

OPERATE_RET tuya_os_adapt_reg_system_intf(void)
{
    return tuya_os_adapt_reg_intf(INTF_SYSTEM, (void *)&m_tuya_os_system_intfs);
}

