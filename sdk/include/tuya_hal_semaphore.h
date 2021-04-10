/**
 * @file tuya_hal_semaphore.h
 * @author sunkz@tuya.com
 * @brief semaphore相关接口封装
 * @version 0.1
 * @date 2020-05-15
 * 
 * @copyright Copyright (c) tuya.inc 2019
 * 
 */

#ifndef _TUYA_HAL_SEMAPHORE_H
#define _TUYA_HAL_SEMAPHORE_H

#include "tuya_os_adapter.h"


#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief tuya_hal_semaphore_create_init用于创建并初始化semaphore
 * 
 * @param[out] *pHandle semaphore句柄
 * @param[in] semCnt 
 * @param[in] sem_max 
 * @return int 0=成功，非0=失败
 */
INT_T tuya_hal_semaphore_create_init(SEM_HANDLE *pHandle, CONST UINT_T semCnt, CONST UINT_T sem_max);

/**
 * @brief tuya_hal_semaphore_wait用于wait semaphore
 * 
 * @param semHandle semaphore句柄
 * @return int 0=成功，非0=失败
 */
INT_T tuya_hal_semaphore_wait(CONST SEM_HANDLE semHandle);

/**
 * @brief tuya_hal_semaphore_waittimeout用于wait semaphore
 * 
 * @param semHandle semaphore句柄
 * @param timeout   semaphore超时时间(TUYA_OS_ADAPT_SEM_FOREVER:表示永久阻塞)
 * @return int 0=成功，非0=失败
 */
INT_T tuya_hal_semaphore_waittimeout(CONST SEM_HANDLE semHandle, unsigned int timeout);

/**
 * @brief tuya_hal_semaphore_post用于post semaphore
 * 
 * @param semHandle semaphore句柄
 * @return int 0=成功，非0=失败
 */
INT_T tuya_hal_semaphore_post(CONST SEM_HANDLE semHandle);

/**
 * @brief tuya_hal_semaphore_release用于release semaphore
 * 
 * @param semHandle 
 * @return int 0=成功，非0=失败
 */
INT_T tuya_hal_semaphore_release(CONST SEM_HANDLE semHandle);

/* add begin: by sunkz, interface regist */
VOID_T tuya_os_semaphore_intf_init(VOID_T);
/* add end */


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

