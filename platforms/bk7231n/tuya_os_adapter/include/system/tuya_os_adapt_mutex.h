/**
 * @file tuya_os_adapt_mutex.h
 * @brief mutex相关接口封装
 * 
 * @copyright copyright Copyright(C),2018-2020, 涂鸦科技 www.tuya.com
 * 
 */


#ifndef _TUYA_OS_ADAPT_MUTEX_H
#define _TUYA_OS_ADAPT_MUTEX_H

#include "tuya_os_adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

/* tuya sdk definition of mutex context */
typedef void* MUTEX_HANDLE;

/**
 * @brief tuya_os_adapt_mutex_create_init用于创建并初始化tuya mutex
 * 
 * @param[out] pMutexHandle 返回mutex句柄
 * @return int 0=成功，非0=失败
 */
int tuya_os_adapt_mutex_create_init(MUTEX_HANDLE *pMutexHandle);

/**
 * @brief tuya_os_adapt_mutex_lock用于lock tuya mutex
 * 
 * @param[in] mutexHandle tuya mutex句柄
 * @return int 0=成功，非0=失败
 */
int tuya_os_adapt_mutex_lock(const MUTEX_HANDLE mutexHandle);

/**
 * @brief tuya_os_adapt_mutex_unlock用于unlock tuya mutex
 * 
 * @param[in] mutexHandle tuya mutex句柄
 * @return int 0=成功，非0=失败
 */
int tuya_os_adapt_mutex_unlock(const MUTEX_HANDLE mutexHandle);

/**
 * @brief tuya_os_adapt_mutex_release用于释放tuya mutex
 * 
 * @param[in] mutexHandle MUTEX_HANDLE tuya mutex句柄
 * @return int 0=成功，非0=失败
 */
int tuya_os_adapt_mutex_release(const MUTEX_HANDLE mutexHandle);

OPERATE_RET tuya_os_adapt_reg_mutex_intf(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
