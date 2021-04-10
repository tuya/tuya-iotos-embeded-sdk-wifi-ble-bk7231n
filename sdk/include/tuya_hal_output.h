/**
 * @file tuya_hal_output.h
 * @brief 日志操作接口（注意：放在头文件"uni_log.h"后面，防止覆盖PR_NOTICE…）
 * 
 * @copyright Copyright(C),2018-2020, 涂鸦科技 www.tuya.com
 * 
 */

#ifndef _TUYA_HAL_OUTPUT_H
#define _TUYA_HAL_OUTPUT_H

#include "tuya_os_adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief tuya_hal_output_log用于输出log信息
 * 
 * @param[in] *str log buffer指针
 * 函数实现在tuya_hal_output.c
 */
VOID_T tuya_hal_output_log(CONST CHAR_T *str);

/**
 * @brief 用于关闭原厂sdk默认的log口
 * 
 */
OPERATE_RET tuya_hal_log_close(VOID_T);

/**
 * @brief 用于恢复原厂sdk默认的log口
 * 
 */
OPERATE_RET tuya_hal_log_open(VOID_T);

/* add begin: by sunkz, interface regist */
VOID_T tuya_os_output_intf_init(VOID_T);
/* add end */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

