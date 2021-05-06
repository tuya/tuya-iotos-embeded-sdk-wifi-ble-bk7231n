/**
 * @file tuya_os_adapt_thread.c
 * @brief 线程操作接口
 * 
 * @copyright Copyright(C),2018-2020, 涂鸦科技 www.tuya.com
 * 
 */

#include "tuya_os_adapt_thread.h"
#include "tuya_os_adapter_error_code.h"
#include <FreeRTOS.h>
#include <task.h>


/***********************************************************
*************************micro define***********************
***********************************************************/

/***********************************************************
*************************variable define********************
***********************************************************/
static const TUYA_OS_THREAD_INTF m_tuya_os_thread_intfs = {
    .create        = tuya_os_adapt_thread_create, 
    .release       = tuya_os_adapt_thread_release,
    .is_self       = tuya_os_adapt_thread_is_self,
    .set_self_name = tuya_os_adapt_thread_set_self_name,
};

/***********************************************************
*************************function define********************
***********************************************************/
/**
 * @brief create and start a tuya sdk thread
 * 
 * @param[out] thread       the thread handle
 * @param[in] name          the thread name
 * @param[in] stack_size    stack size of thread
 * @param[in] priority      priority of thread
 * @param[in] func          the main thread process function
 * @param[in] arg           the args of the func, can be null
 * @retval OPRT_OS_ADAPTER_OK          success
 * @retval Other            fail
 */
int tuya_os_adapt_thread_create(THREAD_HANDLE* thread, const char* name, unsigned int stack_size, unsigned int priority, THREAD_FUNC_T func, void* const arg)
{
    BaseType_t ret = 0;
    ret = xTaskCreate(func, name, stack_size/sizeof(portSTACK_TYPE), (void * const)arg, priority, thread);
    if (ret != pdPASS) {
        return OPRT_OS_ADAPTER_THRD_CREAT_FAILED;
    }

    return OPRT_OS_ADAPTER_OK;
}

/**
* @brief terminal thread and release thread resources
* 
* @param[in] thread    the input thread handle
* @retval OPRT_OS_ADAPTER_OK      success
* @retval Other        fail
*/
int tuya_os_adapt_thread_release(THREAD_HANDLE thread)
{
    
    // delete thread process 
    vTaskDelete(thread);

    return OPRT_OS_ADAPTER_OK;
}

/**
 * @brief check thread is self thread
 * 
 * @param[in] thread    the thread handle
 * @param[out] is_self  output is self thread
 * @retval OPRT_OS_ADAPTER_OK      success
 * @retval Other        fail
 */
int tuya_os_adapt_thread_is_self(THREAD_HANDLE thread, BOOL_T* is_self)
{
    if (NULL == thread || NULL == is_self) {
        return OPRT_OS_ADAPTER_INVALID_PARM;
    }

    THREAD_HANDLE self_handle = xTaskGetCurrentTaskHandle();
    if (NULL == self_handle) {
        return OPRT_OS_ADAPTER_THRD_JUDGE_SELF_FAILED;
    }

    *is_self = (thread == self_handle);

    return OPRT_OS_ADAPTER_OK;
}

/**
 * @brief set name of self thread
 * 
 * @param[in] name      thread name
 * @retval OPRT_OK      success
 * @retval Other        fail
 */
int tuya_os_adapt_thread_set_self_name(const char* name)
{
    return OPRT_OS_ADAPTER_OK;
}

/**
 * @brief current thread enter critical
 *
 * @param  void
 * @retval void
 */
void tuya_os_adapt_thread_enter_critical(void)
{
    vPortEnterCritical();
}

/**
 * @brief current thread exit critical
 *
 * @param  void
 * @retval void
 */
void tuya_os_adapt_thread_exit_critical(void)
{
    vPortExitCritical();
}

/**
* @brief set thread priority
*
* @param[in] thread       the input thread handle
* @param[in] prio_thread  the priority thread send to
* @retval void
*/
void tuya_os_adapt_thread_priority_set(THREAD_HANDLE thread, int prio_thread)
{
    vTaskPrioritySet(thread, prio_thread);
}

/**
* @brief get thread priority
*
* @param[in]  thread     the input thread handle
* @param[out] priority   the priority of thread
* @retval void
*/
int tuya_os_adapt_thread_priority_get(THREAD_HANDLE thread)
{
    return uxTaskPriorityGet(thread);
}

OPERATE_RET tuya_os_adapt_reg_thread_intf(void)
{
    return tuya_os_adapt_reg_intf(INTF_THREAD, (void *)&m_tuya_os_thread_intfs);
}

