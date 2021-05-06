/**
 * @file tuya_os_adapt_semaphore.h
 * @brief semaphore相关接口封装
 * 
 * @copyright Copyright(C),2018-2020, 涂鸦科技 www.tuya.com
 * 
 */

#ifndef _TUYA_OS_ADAPT_SEMAPHORE_H
#define _TUYA_OS_ADAPT_SEMAPHORE_H

#include <stdint.h>
#include "tuya_os_adapter.h"

#ifdef __cplusplus
extern "C" {
#endif


/* tuya sdk definition of semaphore */
typedef void* SEM_HANDLE;

//阻塞等待
#define TUYA_OS_ADAPT_SEM_FOREVER 0xffffffff

/**
 * @brief tuya_os_adapt_semaphore_create_init用于创建并初始化semaphore
 * 
 * @param[out] *pHandle semaphore句柄
 * @param[in] semCnt 
 * @param[in] sem_max 
 * @return int 0=成功，非0=失败
 */
int tuya_os_adapt_semaphore_create_init(SEM_HANDLE *pHandle, const unsigned int semCnt, const unsigned int sem_max);

/**
 * @brief tuya_os_adapt_semaphore_wait用于wait semaphore
 * 
 * @param[in] semHandle semaphore句柄
 * @return int 0=成功，非0=失败
 */
int tuya_os_adapt_semaphore_wait(const SEM_HANDLE semHandle);

/**
 * @brief tuya_os_adapt_semaphore_wait used fo wait semaphore with timeout
 *
 * @param[in] semHandle semaphore句柄
 * @param[in] timeout  semaphore超时时间(TUYA_OS_ADAPT_SEM_FOREVER:表示永久阻塞)
 * @return int 0=成功，非0=失败
*/
int tuya_os_adapt_semaphore_waittimeout(const SEM_HANDLE semHandle, unsigned int timeout);

/**
 * @brief tuya_os_adapt_semaphore_post用于post semaphore
 * 
 * @param[in] semHandle semaphore句柄
 * @return int 0=成功，非0=失败
 */
int tuya_os_adapt_semaphore_post(const SEM_HANDLE semHandle);

/**
 * @brief tuya_os_adapt_semaphore_release用于release semaphore
 * 
 * @param[in] semHandle 
 * @return int 0=成功，非0=失败
 */
int tuya_os_adapt_semaphore_release(const SEM_HANDLE semHandle);

/* add begin: by sunkz, interface regist */
OPERATE_RET tuya_os_adapt_reg_semaphore_intf(void);
/* add end */


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

