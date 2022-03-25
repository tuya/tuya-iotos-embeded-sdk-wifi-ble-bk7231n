/**
* @file user_timer.h
* @author www.tuya.com
* @brief Software and hardware timing modules
* @version 0.2
* @date 2022-03-25
*
* @copyright Copyright (c) tuya.inc 2022
*
*/

#ifndef __USER_TIMER_H__
#define __USER_TIMER_H__


#include "light_types.h"
#include "soc_timer.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/**
 * @brief: 硬件定时器启动, 用户调用接口
 * @param[in] cycle_us 硬件定时器的周期, 单位: us
 * @param[in] call_back 定时回调函数指针
 * @retval: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_user_hardware_timer_start(IN UINT_T cycle_us, IN VOID* call_back);

/**
 * @brief: 硬件定时器停止, 用户调用接口
 * @param: none
 * @retval: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_user_hardware_timer_stop(VOID);

/**
 * @brief: 软件定时器启动, 用户调用接口
 * @param[in]: timer_id  软件定时器的ID
 * @param[in]: cyclye_ms 定时时间, 单位: ms
 * @param[in]: call_back 定时回调函数指针
 * @note: 
 *         此软件定时可以在任何情况情况下进行重载reload
 *         此软件定时仅仅实现了单次定时，如果需要循环执行，请再回调中再次start定时器id
 * @retval: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_user_software_timer_start(IN UCHAR_T timer_id, IN UINT_T cyclye_ms, IN VOID* call_back);

/**
 * @brief: 软件定时器停止, 用户调用接口
 * @param[in] timer_id 软件定时器ID
 * @return: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_user_software_timer_stop(IN UCHAR_T timer_id);

/**
 * @brief: 检查软件定时器是否有效(正在计时或者正在生效), 用户调用接口
 * @param[in]: timer_id 软件timer ID号
 * @return: TURE-> 软件定时器有效, FALSE-> 当前无效
 */
BOOL_T tuya_user_software_timer_active_check(IN UCHAR_T timer_id);



#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif  /* __USER_TIMER_H__ */
