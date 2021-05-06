/**
 * @file tuya_os_adapt_mutex.c
 * @brief 互斥锁操作接口
 * 
 * @copyright Copyright(C),2018-2020, 涂鸦科技 www.tuya.com
 * 
 */
#include "tuya_os_adapt_mutex.h"
#include "tuya_os_adapter_error_code.h"
#include "tuya_os_adapt_system.h"
#include "tuya_os_adapt_memory.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"


/***********************************************************
*************************micro define***********************
***********************************************************/
typedef xSemaphoreHandle THRD_MUTEX;

typedef struct
{
    THRD_MUTEX mutex;
}MUTEX_MANAGE,*P_MUTEX_MANAGE;

static const TUYA_OS_MUTEX_INTF m_tuya_os_mutex_intfs = {
    .init    = tuya_os_adapt_mutex_create_init, 
    .lock    = tuya_os_adapt_mutex_lock,
    .unlock  = tuya_os_adapt_mutex_unlock,
    .release = tuya_os_adapt_mutex_release,
};

/***********************************************************
*************************variable define********************
***********************************************************/

/***********************************************************
*************************function define********************
***********************************************************/
/**
 * @brief tuya_os_adapt_mutex_create_init用于创建并初始化tuya mutex
 * 
 * @param[out] pMutexHandle 返回mutex句柄
 * @return int 0=成功，非0=失败
 */
int tuya_os_adapt_mutex_create_init(MUTEX_HANDLE *pMutexHandle)
{
    if(!pMutexHandle)
        return OPRT_OS_ADAPTER_INVALID_PARM;
    
    P_MUTEX_MANAGE pMutexManage;
    pMutexManage = (P_MUTEX_MANAGE)tuya_os_adapt_system_malloc(sizeof(MUTEX_MANAGE));
    if(!(pMutexManage))
        return OPRT_OS_ADAPTER_MALLOC_FAILED;
    
#if defined(CREATE_RECURSION_MUTEX)
    pMutexManage->mutex = xSemaphoreCreateRecursiveMutex();
#else
    pMutexManage->mutex = xSemaphoreCreateMutex();
#endif

    if(NULL == pMutexManage->mutex) {
        return OPRT_OS_ADAPTER_MUTEX_CREAT_FAILED;
    }

    *pMutexHandle = (MUTEX_HANDLE)pMutexManage;

    return OPRT_OS_ADAPTER_OK;
}

/**
 * @brief tuya_os_adapt_mutex_lock用于lock tuya mutex
 * 
 * @param[in] mutexHandle tuya mutex句柄
 * @return int 0=成功，非0=失败
 */
int tuya_os_adapt_mutex_lock(const MUTEX_HANDLE mutexHandle)
{
    if(!mutexHandle)
        return OPRT_OS_ADAPTER_INVALID_PARM;

    P_MUTEX_MANAGE pMutexManage;
    pMutexManage = (P_MUTEX_MANAGE)mutexHandle;
    
    BaseType_t ret;
    ret = xSemaphoreTake(pMutexManage->mutex, portMAX_DELAY);
    if(pdTRUE != ret) {
        return OPRT_OS_ADAPTER_MUTEX_LOCK_FAILED;
    }

    return OPRT_OS_ADAPTER_OK;
}

/**
 * @brief tuya_os_adapt_mutex_unlock用于unlock tuya mutex
 * 
 * @param[in] mutexHandle tuya mutex句柄
 * @return int 0=成功，非0=失败
 */
int tuya_os_adapt_mutex_unlock(const MUTEX_HANDLE mutexHandle)
{
    if(!mutexHandle)
        return OPRT_OS_ADAPTER_INVALID_PARM;
    
    P_MUTEX_MANAGE pMutexManage;
    pMutexManage = (P_MUTEX_MANAGE)mutexHandle;
    
    BaseType_t ret;
    if(FALSE == tuya_os_adapt_system_isrstatus()) {
        ret = xSemaphoreGive(pMutexManage->mutex);
    }else {
        signed portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
        ret = xSemaphoreGiveFromISR(pMutexManage->mutex,\
                                    &xHigherPriorityTaskWoken);
        portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
    }
    if(pdTRUE != ret) {
        return OPRT_OS_ADAPTER_MUTEX_UNLOCK_FAILED;
    }


    return OPRT_OS_ADAPTER_OK;
}

/**
 * @brief tuya_os_adapt_mutex_release用于释放tuya mutex
 * 
 * @param[in] mutexHandle MUTEX_HANDLE tuya mutex句柄
 * @return int 0=成功，非0=失败
 */
int tuya_os_adapt_mutex_release(const MUTEX_HANDLE mutexHandle)
{
    if(!mutexHandle)
        return OPRT_OS_ADAPTER_INVALID_PARM;

    P_MUTEX_MANAGE pMutexManage;
    pMutexManage = (P_MUTEX_MANAGE)mutexHandle;
    
    vSemaphoreDelete(pMutexManage->mutex);

    tuya_os_adapt_system_free(mutexHandle);

    return OPRT_OS_ADAPTER_OK;
}

OPERATE_RET tuya_os_adapt_reg_mutex_intf(void)
{
    return tuya_os_adapt_reg_intf(INTF_MUTEX, (void *)&m_tuya_os_mutex_intfs);
}

