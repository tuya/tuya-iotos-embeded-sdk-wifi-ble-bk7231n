/**
 * @file tuya_os_init.c
 * @author sunkz@tuya.com
 * @brief 
 * @version 0.1
 * @date 2020-05-15
 * 
 * @copyright Copyright (c) tuya.inc 2019
 * 
 */

#include "tuya_os_adapter.h"
#include "tuya_os_init.h"
#include "tuya_os_adapt_memory.h"
#include "tuya_os_adapt_thread.h"
#include "tuya_os_adapt_mutex.h"
#include "tuya_os_adapt_semaphore.h"
#include "tuya_os_adapt_storge.h"
#include "tuya_os_adapt_network.h"
#include "tuya_os_adapt_output.h"
#include "tuya_os_adapt_system.h"
#include "tuya_os_adapt_wifi.h"
#include "tuya_os_adapt_bt.h"
#include "tuya_os_adapt_storge.h"
#include "tuya_os_adapt_ota.h"

extern int platform_uart_init(void);
extern int platform_pin_init(void);
extern int platform_i2c_init(void);
extern int platform_rtc_init(void);
extern int platform_pwm_init(void);
extern int platform_timer_init(void);
extern int platform_adc_init(void);

void tuya_os_init(void)
{
    tuya_os_adapt_reg_memory_intf();
    tuya_os_adapt_reg_mutex_intf();
    tuya_os_adapt_reg_semaphore_intf();
    tuya_os_adapt_reg_thread_intf();
    tuya_os_adapt_reg_network_intf();
    tuya_os_adapt_reg_output_intf();
    tuya_os_adapt_reg_system_intf();

    tuya_os_adapt_reg_wifi_intf();
#if defined(TY_BT_MOD) && TY_BT_MOD == 1
    tuya_os_adapt_reg_bt_intf();
#endif
    tuya_os_adapt_reg_storage_intf();
    tuya_os_adapt_reg_ota_intf();

    platform_uart_init();
    platform_pin_init();
    platform_pwm_init();
    platform_timer_init();
    platform_adc_init();
    
#if defined(TY_RTC) && TY_RTC == 1
    platform_rtc_init();
#endif
}

char* tuya_os_adapt_get_ver(void)
{
    return TUYA_OS_ADAPT_PLATFORM_VERSION;
}


