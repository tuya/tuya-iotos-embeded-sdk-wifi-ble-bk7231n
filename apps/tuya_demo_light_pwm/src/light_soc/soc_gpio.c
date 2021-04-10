/*
 * @Author: wls
 * @email: wuls@tuya.com
 * @LastEditors: wls
 * @file name:  soc_gpio.c
 * @Description: BK7231通用GPIO口设置程序
 * @Copyright: 杭州涂鸦信息技术有限公司
 * @Company: http://www.tuya.com
 * @Date: 2019-04-16 16:46:43
 * @LastEditTime: 2019-04-25 17:21:28
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


