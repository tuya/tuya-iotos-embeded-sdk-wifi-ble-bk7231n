/**
* @file soc_gpio.h
* @author www.tuya.com
* @brief soc gpio proc
* @version 0.2
* @date 2022-03-25
*
* @copyright Copyright (c) tuya.inc 2022
*
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
