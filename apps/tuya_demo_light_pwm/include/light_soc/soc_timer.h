/*
 * @Author: wuls
 * @email: wulsu@tuya.com
 * @LastEditors:
 * @file name: soc_timer.h
 * @Description: soc timer proc include file
 * @Copyright: HANGZHOU TUYA INFORMATION TECHNOLOGY CO.,LTD
 * @Company: http://www.tuya.com
 * @Date: 2019-05-06 10:00:26
 * @LastEditTime: 2019-08-28 16:15:05
 */

#ifndef __SOC_TIMER_H__
#define __SOC_TIMER_H__

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#include "light_types.h"

/**
 * @brief: SOC 开启硬件定时器
 * @param[in]: cycle_us 循环定时时间, 单位us
 * @param[in]: callback 定时回调函数指针
 * @retval: OPERATE_LIGHT 0表示成功，非0请参照tuya error code描述文档 
 */
OPERATE_LIGHT tuya_soc_hardware_timer_start(IN UINT_T cycle_us, IN VOID* callback);

/**
 * @brief: SOC 停止硬件定时器
 * @param: none 
 * @retval: OPERATE_LIGHT 0表示成功，非0请参照tuya error code描述文档 
 */
OPERATE_LIGHT tuya_soc_hardware_timer_stop(VOID);

/**
 * @berief: SOC 开启软件定时器
 * @param[in]: timer_id 软件timer ID号
 * @param[in]: time_ms 定时的时间, 单位ms 
 * @param[in]: callback 定时回调函数指针
 * @note: 此函数为软件定时器的适配实现，可以依照现有资源进行实现，
 *        如使用RTOS的软件timer，或者涂鸦SDK中提供的软件timer实现
 * @return: OPERATE_LIGHT 0表示成功，非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_soc_software_timer_start(IN UCHAR_T timer_id, IN UINT_T cycle_ms, IN VOID* callback);

/**
 * @brief: SOC 停止软件定时器
 * @param[in]: timer_id  软件timer ID号
 * @retval: OPERATE_LIGHT 0表示成功，非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_soc_software_timer_stop(IN UCHAR_T timer_id);


/**
 * @brief: 软件定时器是否有效（正在计时或者正在生效）判断适配模块
 * @param[in]: timer_id 软件timer ID号
 * @return: TURE-> 软件定时器有效, FALSE-> 当前无效
 */
BOOL_T tuya_soc_software_timer_active_check(IN UCHAR_T timer_id);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif  /* __SOC_TIMER_H__ */
