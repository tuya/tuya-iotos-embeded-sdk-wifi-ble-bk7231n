/**
* @file soc_gpio.c
* @author www.tuya.com
* @brief soc gpio proc
* @version 0.2
* @date 2022-03-25
*
* @copyright Copyright (c) tuya.inc 2022
*
*/

#include "soc_gpio.h"
#include "tuya_gpio.h"
#include "light_printf.h"

/**
 * @brief: SOC 设置GPIO口为输出模式并且输出电平为低电平
 * @param: none
 * @retval: none
 */
VOID tuya_soc_pin_reset(UCHAR_T pin)
{
    OPERATE_RET ret = -1;

    //设置pin口为输出模式
    ret = tuya_gpio_inout_set(pin, FALSE);
    if (ret != OPRT_OK) {
        PR_ERR("gpio %d set output error!", pin);
        return;
    }

    //pin口输出低电平
    ret = tuya_gpio_write(pin, FALSE);
    if (ret != OPRT_OK) {
        PR_ERR("gpio %d output low error!", pin);
    }

    return;
}

/**
 * @brief: SOC 设置GPIO口为输出模式并且输出电平为高电平
 * @param: none
 * @retval: none
 */
VOID tuya_soc_pin_set(UCHAR_T pin)
{
    OPERATE_RET ret = -1;

    //设置pin口为输出模式
    ret = tuya_gpio_inout_set(pin, FALSE);
    if (ret != OPRT_OK) {
        PR_ERR("gpio %d set output error!", pin);
        return;
    }

    //pin口输出高电平
    ret = tuya_gpio_write(pin, TRUE);
    if (ret != OPRT_OK) { 
        PR_ERR("gpio %d output high error!", pin);
    }

    return;
}


