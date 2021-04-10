/*
 * @Author: wls
 * @email: wuls@tuya.com
 * @LastEditors:   
 * @file name:  soc_gpio.h
 * @Description: soc gpio include file
 * @Copyright: HANGZHOU TUYA INFORMATION TECHNOLOGY CO.,LTD
 * @Company: http://www.tuya.com
 * @Date: 2019-04-16 16:46:43
 * @LastEditTime: 2019-10-21 14:27:20
 */
#ifndef __SOC_GPIO_H__
#define __SOC_GPIO_H__

#include "light_types.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */


/**
 * @brief: SOC 设置GPIO口为输出模式并且初始电平为低电平
 * @param: none
 * @retval: none
 */
VOID tuya_soc_pin_reset(UCHAR_T pin);


/**
 * @brief: SOC 设置GPIO口为输出模式并且输出电平为高电平
 * @param: none
 * @retval: none
 */
VOID tuya_soc_pin_set(UCHAR_T pin);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif  /* __SOC_GPIO_H__ */
