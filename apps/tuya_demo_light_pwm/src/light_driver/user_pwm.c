/*
 * @Author: wls
 * @email: wuls@tuya.com
 * @LastEditors: wls
 * @file name: user_pwm.c
 * @Description: PWM driver program
 * @Copyright: HANGZHOU TUYA INFORMATION TECHNOLOGY CO.,LTD
 * @Company: http://www.tuya.com
 * @Date: 2019-04-16 11:44:21
 * @LastEditTime: 2019-08-13 11:23:01
 */

#include "user_pwm.h"
#include "light_printf.h"
#include "light_tools.h"
#include "soc_pwm.h"
#include "light_set_color.h"
#include "soc_gpio.h"

#define PWM_MAX_FREQ    20000                   // PWM MAX Frequency 20K
#define PWM_MIN_FREQ    100                     // PWM MIN Frequency 100
#define PWM_MAX_DUTY    1000                    // PWM MAX Duty 1000 --> Precision 0.1%

STATIC user_pwm_init_t sg_pwm_init_config;      // PWM配置
STATIC UCHAR_T sg_pwm_init_flag = FALSE;

/**
 * @brief: pwm初始化，用户调用接口
 * @param[in]: pwm_init pwm初始化参数
 *          freq -> PWM Frequency unit Hz, range:100~20000Hz
 *          duty -> PWM Init duty unit 0.1%, range: 0~1000 
 *          list[5] -> RGBCW GPIO list
 *          channel_num -> channel num, rang 1~5 way
 *          polarity -> PWM output polarity
 *              TRUE -> positive
 *              FAlse -> negative          
 *          ctrl_pin -> ctrl pin
 *          ctrl_level -> ctrl pin level}
 * @attention list[5] parm set
 *              List order is always RGBCW !
 * @retval: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_user_pwm_init(IN user_pwm_init_t *pwm_init)
{
    OPERATE_LIGHT ret = -1;
    UCHAR_T list_temp[5] = {0};

    //判断参数是否合法
    if (NULL == pwm_init) {
        PR_ERR("USER PWM init is invalid!");
        return LIGHT_INVALID_PARM;
    }
  
    if ((pwm_init->freq < PWM_MIN_FREQ) || (pwm_init->freq > PWM_MAX_FREQ)) {
        PR_ERR("USER PWM init is invalid!");
        return LIGHT_INVALID_PARM;
    }

    if (pwm_init->duty > PWM_MAX_DUTY) {
        PR_ERR("USER PWM init is invalid!");
        return LIGHT_INVALID_PARM;
    }

    if ((pwm_init->channel_num <= 0) || (pwm_init->channel_num > 5)) {
        PR_ERR("USER PWM init is invalid!");
        return LIGHT_INVALID_PARM;
    }
    
    memcpy(&sg_pwm_init_config, pwm_init, SIZEOF(user_pwm_init_t));

    switch (sg_pwm_init_config.channel_num) {
        case 1: // 1 way -- C 
            list_temp[0] = sg_pwm_init_config.list[3];
            memcpy(sg_pwm_init_config.list, list_temp, SIZEOF(sg_pwm_init_config.list));
            break;

        case 2: //2 way -- CW
            list_temp[0] = sg_pwm_init_config.list[3];
            list_temp[1] = sg_pwm_init_config.list[4];
            memcpy(sg_pwm_init_config.list, list_temp, SIZEOF(sg_pwm_init_config.list));
            break;

        case 3: //3 way -- RGB
        case 4: //4 way -- RGBC
        case 5: //5 way -- RGBCW
            // don't need to process
            break;
            
        default:
            break;
    }

    //pwm初始化
    ret = tuya_soc_pwm_init(sg_pwm_init_config.freq, sg_pwm_init_config.duty, sg_pwm_init_config.channel_num, sg_pwm_init_config.list, sg_pwm_init_config.cct_flag);
    if (ret != LIGHT_OK) {
        PR_ERR("PWM soc init error!");
        return LIGHT_INVALID_PARM;
    }

    sg_pwm_init_flag = TRUE;

    //是否用到控制引脚
    if (PIN_NOEXIST == sg_pwm_init_config.ctrl_pin) {
        PR_DEBUG("no ctrl pin!");
        return LIGHT_OK;
    }

    //初始控制引脚电平为无效电平
    if(sg_pwm_init_config.ctrl_level) {      
        tuya_soc_pin_reset(sg_pwm_init_config.ctrl_pin);
    } else {
        tuya_soc_pin_set(sg_pwm_init_config.ctrl_pin);
    }
    
    return LIGHT_OK;
}

/**
 * @brief: 设置rgbcw值, 用户调用接口
 * @param[in]: color_data rgbcw数据
 *          red   -> R send duty,rang from 0 to 1000,unit 0.1%
 *          green -> G send duty,rang from 0 to 1000,unit 0.1%
 *          blue  -> B send duty,rang from 0 to 1000,unit 0.1%
 *          cold  -> C send duty,rang from 0 to 1000,unit 0.1%
 *          warm  -> W send duty,rang from 0 to 1000,unit 0.1%}
 * @retval: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_user_pwm_set_rgbcw(IN user_pwm_color_t *color_data)
{
    //pwm是否已经初始化
    if(sg_pwm_init_flag != TRUE){
        PR_ERR("user pwm not init!");
        return LIGHT_INVALID_PARM;
    }

    //判断color参数是否有效
    if ((color_data->red > PWM_MAX_DUTY) || (color_data->green > PWM_MAX_DUTY) || \
        (color_data->blue > PWM_MAX_DUTY) || (color_data->cold > PWM_MAX_DUTY) || \
        (color_data->warm > PWM_MAX_DUTY)) {
        PR_ERR("USER PWM data is invalid!");
        return LIGHT_INVALID_PARM;
    }

    //存在控制引脚
    if (sg_pwm_init_config.ctrl_pin != PIN_NOEXIST) {       
        if ((0 != color_data->red) || (0 != color_data->green) || (0 != color_data->blue) ||
            (0 != color_data->cold) || (0 != color_data->warm)) {
            //color数据不为0, 控制引脚输出有效电平
            if (sg_pwm_init_config.ctrl_level) {
                tuya_soc_pin_set(sg_pwm_init_config.ctrl_pin);           
            } else {
                tuya_soc_pin_reset(sg_pwm_init_config.ctrl_pin);
            }
        } else {
            //color数据为0, 控制引脚输出无效电平
            if (sg_pwm_init_config.ctrl_level) {
                tuya_soc_pin_reset(sg_pwm_init_config.ctrl_pin);  
            } else {
                tuya_soc_pin_set(sg_pwm_init_config.ctrl_pin);
            }
        }
    }

    //rgbcw值适配
    if (sg_pwm_init_config.polarity == FALSE) {
        color_data->red = PWM_MAX_DUTY - color_data->red;
        color_data->green = PWM_MAX_DUTY - color_data->green;
        color_data->blue = PWM_MAX_DUTY - color_data->blue;
        color_data->cold = PWM_MAX_DUTY - color_data->cold;
        color_data->warm = PWM_MAX_DUTY - color_data->warm;
    }
    //PR_NOTICE("");
    //设置rgbcw值
    switch (sg_pwm_init_config.channel_num) {
        case 1: //1 way -- C          
            tuya_soc_pwm_set_duty(0, color_data->cold);     //send C value
            break;
        case 2: //2 way -- CW 
            tuya_soc_pwm_set_duty(0, color_data->cold);     //send C value
            tuya_soc_pwm_set_duty(1, color_data->warm);     //send W value
            break;
        case 3: //3 way -- RGB
            tuya_soc_pwm_set_duty(0, color_data->red);      //send R value 
            tuya_soc_pwm_set_duty(1, color_data->green);    //send G value
            tuya_soc_pwm_set_duty(2, color_data->blue);     //send B value
            break;
        case 4: //4 way -- RGBC
            tuya_soc_pwm_set_duty(0, color_data->red);      //send R value 
            tuya_soc_pwm_set_duty(1, color_data->green);    //send G value
            tuya_soc_pwm_set_duty(2, color_data->blue);     //send B value
            tuya_soc_pwm_set_duty(3, color_data->cold);     //send C value
            break;
        case 5: //5 way -- RGBCW
            tuya_soc_pwm_set_duty(0, color_data->red);      //send R value
            tuya_soc_pwm_set_duty(1, color_data->green);    //send G value
            tuya_soc_pwm_set_duty(2, color_data->blue);     //send B value
            tuya_soc_pwm_set_duty(3, color_data->cold);     //send C value
            tuya_soc_pwm_set_duty(4, color_data->warm);     //send w value
            break;
        default:
            PR_ERR("pwm drive channel set error!!");
            break;
    }

    return LIGHT_OK;
}

