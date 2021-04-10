/*
 * @Author: wls
 * @email: wuls@tuya.com
 * @LastEditors: wls
 * @file name: soc_pwm.h
 * @Description: soc PWM头文件
 * @Copyright: 杭州涂鸦信息科技有限公司
 * @Company: http://www.tuya.com
 * @Date: 2019-04-24 20:42:29
 * @LastEditTime: 2019-08-28 21:50:23
 */

#ifndef __SOC_PWM_H__
#define __SOC_PWM_H__


#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#include "light_types.h"

/**
 * @brief: SOC pwm初始化
 * @param: IN USHORT_T frequency -->  定时器的频率(1s内计数f次), 单位: HZ(范围: 500 ~ 20KHz) 
 * @param: IN USHORT_T duty_cycle --> 占空值(范围: 0 ~ 1000), 占空比 = duty_cycle / 1000
 * @param: IN UCHAR_T channel_num --> 输出PWM的通道数
 * @param: IN UCHART_T *gpio_list --> 输出PWM的GPIO口列表
 * @retval: OPERATE_LIGHT 0表示成功，非0请参照tuya error code描述文档 
 */
OPERATE_LIGHT tuya_soc_pwm_init(IN USHORT_T frequency, IN USHORT_T duty_cycle, IN UCHAR_T channel_num, IN UCHAR_T *gpio_list, IN BOOL_T cct_flag);


/**
 * @brief: 设置pwm的占空比
 * @param: IN UCHAR_T channel -> pwm通道
 * @param: IN USHORT_T duty   -> 占空值(范围: 0 ~ 1000)
 * @attention channel --> according to the initlize order
 * @retval: none
 */
VOID tuya_soc_pwm_set_duty(IN UCHAR_T channel, IN USHORT_T duty);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif  /* __SOC_PWM_H__ */
