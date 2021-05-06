/**
 * @file tuya_os_adapt_output.h
 * @brief 日志操作接口
 * 
 * @copyright Copyright(C),2018-2020, 涂鸦科技 www.tuya.com
 * 
 */

#ifndef _TUYA_OS_ADAPT_OUTPUT_H
#define _TUYA_OS_ADAPT_OUTPUT_H

#include <stdbool.h>
#include <stdint.h>
#include "tuya_os_adapter.h"
#include "uart_pub.h"

#ifdef __cplusplus
extern "C" {
#endif


#define LOG_DEBUG(...)  bk_printf("[PLATFORM DEBUG]" __VA_ARGS__)
#define LOG_TRACE(...)  bk_printf("[PLATFORM TRACE]" __VA_ARGS__)
#define LOG_NOTICE(...) bk_printf("[PLATFORM NOTICE]" __VA_ARGS__)
#define LOG_ERR(...)    bk_printf("[PLATFORM ERROR]" __VA_ARGS__)


/**
 * @brief tuya_os_adapt_output_log用于输出log信息
 * 
 * @param[in] *str log buffer指针
 * 函数实现在tuya_os_adapt_output.c
 */
void tuya_os_adapt_output_log(const         char *str);

/**
 * @brief 用于关闭原厂sdk默认的log口
 * 
 */
int tuya_os_adapt_log_close(void);

/**
 * @brief 用于恢复原厂sdk默认的log口
 * 
 */
int tuya_os_adapt_log_open(void);

OPERATE_RET tuya_os_adapt_reg_output_intf(void);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

