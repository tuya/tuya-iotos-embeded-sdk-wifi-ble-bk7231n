/**
* @file tuya_svc_timer_task.h
* @author liukang@tuya.com
* @brief Common process - Initialization
* @version 0.1
* @date 2020-11-09
*
* @copyright Copyright 2020-2021 Tuya Inc. All Rights Reserved.
*
*/
#ifndef __TUYA_SVC_TIME_TASK_H__
#define __TUYA_SVC_TIME_TASK_H__

#include "tuya_cloud_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Init a timer task.
 *
 * @param[in] increase_unit: increase unit
 *
 * @note This API is used for initializing the timer task. 
 * when the task queue is full, it will expand the size with the unit user input. 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_svc_timer_task_init(UINT_T increase_unit);

/**
 * @brief Reset the timer task.
 *
 * @param VOID
 *
 * @note This API is used to reset the timer task. the resource of timer task will be released. 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_svc_timer_task_reset(VOID_T);

/**
 * @brief Clear up the timer task data in flash.
 *
 * @param VOID
 *
 * @note This API is used to clear up the timer task data in flash.
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_svc_timer_task_clear_db(VOID_T);

/**
 * @brief Get the count of timer task.
 *
 * @param VOID
 *
 * @note This API is used for getting the count of timer task.
 * @return the count of timer task.
 */
UINT_T tuya_svc_timer_get_task_count(VOID_T);

#if defined(BT_TIMER) && (BT_TIMER == 1)
/**
 * @brief Get the num of timer task.
 *
 * @param VOID
 *
 * @note This API is used for getting the num of timer task.
 * @return the capacity of timer task.
 */
UINT_T tuya_svc_timer_get_num(VOID_T);

/**
 * @brief Get the capacity of timer task.
 *
 * @param VOID
 *
 * @note This API is used for getting the capacity of timer task.
 * @return the capacity of timer task.
 */
UINT_T tuya_svc_timer_get_capacity(VOID_T);

/**
 * @brief delete timer task.
 *
 * @param[in] timer_id: timer id
 * @param[in] update_flash: if need update timer to flash
 *
 * @note This API is used for deleting the timer task.
 * @return void
 */
VOID_T tuya_svc_timer_delete(uint32_t timer_id, BOOL_T update_flash);

/**
 * @brief get timer task data.
 *
 * @param[out] timer_data: timer id and crc32
 *
 * @note This API is used for getting timer task data.
 * @return void
 */
VOID_T tuya_svc_timer_get_data(uint8_t *timer_data);

/**
 * @brief add timer task.
 *
 * @param[in] data: timer data
 * @param[in] len: data len
 *
 * @note This API is used for adding timer task.
 * @return void
 */
VOID_T tuya_svc_timer_add(uint8_t* data, uint16_t len);
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  /*__TUYA_SVC_TIME_TASK_H__ */

