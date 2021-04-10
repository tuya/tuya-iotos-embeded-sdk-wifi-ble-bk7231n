/**
 * @file tuya_hal_system.c
 * @author maht@tuya.com
 * @brief 系统接口封装
 * @version 0.1
 * @date 2019-08-15
 * 
 * @copyright Copyright (c) tuya.inc 2019
 * 
 */

#ifndef _TUYA_HAL_SYSTEM_H
#define _TUYA_HAL_SYSTEM_H

#include "tuya_os_adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief tuya_hal_get_systemtickcount用于获取系统运行ticket 
 * 		count
 * @return SYS_TICK_T 
 */
SYS_TICK_T tuya_hal_get_systemtickcount(VOID_T);

/**
 * @brief tuya_hal_get_tickratems用于获取系统ticket是多少个ms
 * 
 * @return uint32_t the time is ms of a system ticket
 */
UINT_T tuya_hal_get_tickratems(VOID_T);

/**
 * @brief tuya_hal_system_sleep用于系统sleep
 * 
 * @param[in] msTime sleep time is ms
 */
VOID_T tuya_hal_system_sleep(const unsigned long msTime);

/**
 * @brief tuya_hal_system_reset用于重启系统
 * 
 */
VOID_T tuya_hal_system_reset(VOID_T);

/**
 * @brief tuya_hal_watchdog_init_start用于初始化并运行watchdog
 * 
 * @param[in] timeval watch dog检测时间间隔
 */
unsigned int tuya_hal_watchdog_init_start(const unsigned int timeval);

/**
 * @brief tuya_hal_watchdog_refresh用于刷新watch dog
 * 
 */
VOID_T tuya_hal_watchdog_refresh(VOID_T);

/**
 * @brief tuya_hal_watchdog_stop用于停止watch dog
 * 
 */
VOID_T tuya_hal_watchdog_stop(VOID_T);

/**
 * @brief tuya_hal_system_getheapsize用于获取堆大小/剩余内存大小
 * 
 * @return int <0: don't support  >=0: current heap size/free memory
 */
INT_T tuya_hal_system_getheapsize(VOID_T);

/**
 * @brief tuya_hal_system_get_rst_info用于获取硬件重启原因
 * 
 * @return char* 硬件重启原因
 */
TY_RST_REASON_E tuya_hal_system_get_rst_info(VOID_T);

/**
 * @brief tuya_hal_system_get_rst_ext_info用于获取复位扩展信息
 * 
 * @return OPERATE_RET 0=成功，非0=失败
 */
OPERATE_RET tuya_hal_system_get_rst_ext_info(CHAR_T** ext_info);


/**
 * @brief tuya_hal_get_random_data用于获取指定条件下的随机数
 * 
 * @param[in] range 
 * @return int
 */
int tuya_hal_get_random_data(const unsigned int range);

/**
 * @brief tuya_hal_set_cpu_lp_mode用于设置cpu的低功耗模式
 * 
 * @param[in] en 
 * @param[in] mode
 * @return int 0=成功，非0=失败
 */
int tuya_hal_set_cpu_lp_mode(const BOOL_T en, const TY_CPU_SLEEP_MODE_E mode);

/* add begin: by sunkz, interface regist */
VOID_T tuya_os_system_intf_init(VOID_T);
/* add end */


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

