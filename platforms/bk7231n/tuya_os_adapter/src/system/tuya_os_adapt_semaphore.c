/**
 * @file tuya_os_adapt_semaphore.c
 * @brief semaphore相关接口封装
 * 
 * @copyright Copyright(C),2018-2020, 涂鸦科技 www.tuya.com
 * 
 */
 
#include "tuya_os_adapt_semaphore.h"
#include "tuya_os_adapter_error_code.h"
#include "tuya_os_adapt_system.h"
#include "tuya_os_adapt_memory.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"


/***********************************************************
*************************micro define***********************
***********************************************************/
typedef struct
{
    xSemaphoreHandle sem;
}SEM_MANAGE, *P_SEM_MANAGE;


/***********************************************************
*************************variable define********************
***********************************************************/
static const TUYA_OS_SEMAPHORE_INTF m_tuya_os_semaphore_intfs = {
    .init    = tuya_os_adapt_semaphore_create_init, 
    .wait    = tuya_os_adapt_semaphore_wait,
    .waittimeout = tuya_os_adapt_semaphore_waittimeout,
    .post    = tuya_os_adapt_semaphore_post,
    .release = tuya_os_adapt_semaphore_release,
};

/***********************************************************
*************************function define********************
***********************************************************/
static SEM_HANDLE CreateSemaphore(void)
{
    P_SEM_MANAGE pSemManage;
    
    pSemManage = (P_SEM_MANAGE)tuya_os_adapt_system_malloc(sizeof(SEM_MANAGE));

    return (SEM_HANDLE)pSemManage;
}

static int InitSemaphore(const SEM_HANDLE semHandle, const unsigned int semCnt,\
                         const unsigned int sem_max)
{
    if(!semHandle)
        return OPRT_OS_ADAPTER_INVALID_PARM;
        
    P_SEM_MANAGE pSemManage;
    pSemManage = (P_SEM_MANAGE)semHandle;

    pSemManage->sem = xSemaphoreCreateCounting(sem_max, semCnt);
    if(NULL == pSemManage->sem) {
        return OPRT_OS_ADAPTER_SEM_CREAT_FAILED;
    }

    return OPRT_OS_ADAPTER_OK;
}

/**
 * @brief tuya_os_adapt_semaphore_create_init用于创建并初始化semaphore
 * 
 * @param[out] *pHandle semaphore句柄
 * @param[in] semCnt 
 * @param[in] sem_max 
 * @return int 0=成功，非0=失败
 */
int tuya_os_adapt_semaphore_create_init(SEM_HANDLE *pHandle, const unsigned int semCnt, const unsigned int sem_max)
{
    if(NULL == pHandle) {
        return OPRT_OS_ADAPTER_INVALID_PARM;
    }

    *pHandle = CreateSemaphore();
    if(*pHandle == NULL) {
        return OPRT_OS_ADAPTER_MALLOC_FAILED;
    }

    int ret = InitSemaphore(*pHandle, semCnt, sem_max);
    if(ret != OPRT_OS_ADAPTER_OK) {
        tuya_os_adapt_system_free(*pHandle);
        *pHandle = NULL;
        return ret;
    }

    return OPRT_OS_ADAPTER_OK;
}

/**
 * @brief tuya_os_adapt_semaphore_wait用于wait semaphore
 * 
 * @param[in] semHandle semaphore句柄
 * @return int 0=成功，非0=失败
 */
int tuya_os_adapt_semaphore_wait(const SEM_HANDLE semHandle)
{
    if(!semHandle) {
        return OPRT_OS_ADAPTER_INVALID_PARM;
    }

    P_SEM_MANAGE pSemManage;
    pSemManage = (P_SEM_MANAGE)semHandle;

    BaseType_t ret;
    ret = xSemaphoreTake(pSemManage->sem, portMAX_DELAY);
    if(pdTRUE != ret) {
        return OPRT_OS_ADAPTER_SEM_WAIT_FAILED;
    }

    return OPRT_OS_ADAPTER_OK;
}

/**
 * @brief tuya_os_adapt_semaphore_waittimeout used fo wait semaphore with timeout
 *
 * @param[in] semHandle semaphore句柄
 * @param[in] timeout  semaphore超时时间
 * @return int 0=成功，非0=失败
*/
int tuya_os_adapt_semaphore_waittimeout(IN const SEM_HANDLE semHandle, unsigned int timeout)
{
    if(!semHandle) {
        return OPRT_OS_ADAPTER_INVALID_PARM;
    }

    P_SEM_MANAGE pSemManage;
    pSemManage = (P_SEM_MANAGE)semHandle;

    BaseType_t ret;
    if (timeout == TUYA_OS_ADAPT_SEM_FOREVER) {
        ret = xSemaphoreTake(pSemManage->sem, portMAX_DELAY);
    } else {
        ret = xSemaphoreTake(pSemManage->sem, timeout / tuya_os_adapt_get_tickratems());
    }
    if (pdTRUE != ret) {
        return OPRT_OS_ADAPTER_SEM_WAIT_FAILED;
    }

    return OPRT_OS_ADAPTER_OK;
}

/**
 * @brief tuya_os_adapt_semaphore_post用于post semaphore
 * 
 * @param[in] semHandle semaphore句柄
 * @return int 0=成功，非0=失败
 */
int tuya_os_adapt_semaphore_post(const SEM_HANDLE semHandle)
{
    if(!semHandle) {
        return OPRT_OS_ADAPTER_INVALID_PARM;
    }

    P_SEM_MANAGE pSemManage;
    pSemManage = (P_SEM_MANAGE)semHandle;

    BaseType_t ret;
    if(FALSE == tuya_os_adapt_system_isrstatus()) {
        ret = xSemaphoreGive(pSemManage->sem);
    }else {
        signed portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
        ret = xSemaphoreGiveFromISR(pSemManage->sem,
                                    &xHigherPriorityTaskWoken);
        portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
    }

    if(pdTRUE != ret) {
        return OPRT_OS_ADAPTER_SEM_POST_FAILED;
    }

    return OPRT_OS_ADAPTER_OK;
}

/**
 * @brief tuya_os_adapt_semaphore_release用于release semaphore
 * 
 * @param[in] semHandle 
 * @return int 0=成功，非0=失败
 */
int tuya_os_adapt_semaphore_release(const SEM_HANDLE semHandle)
{
    if(!semHandle) {
        return OPRT_OS_ADAPTER_INVALID_PARM;
    }

    P_SEM_MANAGE pSemManage;
    pSemManage = (P_SEM_MANAGE)semHandle;

    vSemaphoreDelete(pSemManage->sem);
    tuya_os_adapt_system_free(semHandle); // 释放信号量管理结构

    return OPRT_OS_ADAPTER_OK;
}

OPERATE_RET tuya_os_adapt_reg_semaphore_intf(void)
{
    return tuya_os_adapt_reg_intf(INTF_SEMAPHORE, (void *)&m_tuya_os_semaphore_intfs);
}


