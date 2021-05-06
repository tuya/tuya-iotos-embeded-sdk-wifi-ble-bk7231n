/**
 * @file tuya_os_adapt_thread.h
 * @brief 线程操作接口
 * 
 * @copyright Copyright(C),2018-2020, 涂鸦科技 www.tuya.com
 * 
 */

#ifndef __TUYA_OS_ADAPT_THREAD_H__
#define __TUYA_OS_ADAPT_THREAD_H__


#include <stdbool.h>
#include <stdint.h>
#include "tuya_os_adapter.h"


#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief handle of thread
 * 
 */
typedef void* THREAD_HANDLE;

/**
 * @brief thread function type
 * 
 */
typedef void (*THREAD_FUNC_T)(void*);

/**
 * @brief create and start a tuya sdk thread
 * 
 * @param[out] thread       the thread handle
 * @param[in] name          the thread name
 * @param[in] stack_size    stack size of thread
 * @param[in] priority      priority of thread
 * @param[in] func          the main thread process function
 * @param[in] arg           the args of the func, can be null
 * @retval 0          success
 * @retval Other            fail
 */
int tuya_os_adapt_thread_create(THREAD_HANDLE* thread, const char* name, unsigned int stack_size, unsigned int priority, THREAD_FUNC_T func, void* const arg);


/**
 * @brief terminal thread and release thread resources
 * 
 * @param[in] thread    the input thread handle
 * @retval 0      success
 * @retval Other        fail
 */
int tuya_os_adapt_thread_release(THREAD_HANDLE thread);

/**
 * @brief check thread is self thread
 * 
 * @param[in] thread    the thread handle
 * @param[out] is_self  output is self thread
 * @retval 0      success
 * @retval Other        fail
 */
int tuya_os_adapt_thread_is_self(THREAD_HANDLE thread, BOOL_T* is_self);

/**
 * @brief set name of self thread
 * 
 * @param[in] name      thread name
 * @retval OPRT_OK      success
 * @retval Other        fail
 */
int tuya_os_adapt_thread_set_self_name(const char* name);

/**
 * @brief current thread enter critical
 *
 * @param  void
 * @retval void
 */
void tuya_os_adapt_thread_enter_critical(void);

/**
 * @brief current thread exit critical
 *
 * @param  void
 * @retval void
 */
void tuya_os_adapt_thread_exit_critical(void);

/**
* @brief set thread priority
*
* @param[in] thread       the input thread handle
* @param[in] prio_thread  the priority thread send to
* @retval void
*/
void tuya_os_adapt_thread_priority_set(THREAD_HANDLE thread, int prio_thread);

/**
* @brief get thread priority
*
* @param[in]  thread     the input thread handle
* @param[out] priority   the priority of thread
* @retval void
*/
int tuya_os_adapt_thread_priority_get(THREAD_HANDLE thread);


OPERATE_RET tuya_os_adapt_reg_thread_intf(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif // __TUYA_OS_ADAPT_THREAD_H__

