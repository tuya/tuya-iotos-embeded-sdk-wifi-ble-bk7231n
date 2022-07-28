/**
* @file light_driver_adapter.c
* @author www.tuya.com
* @brief light driver adapter proc
* @version 0.2
* @date 2022-03-25
*
* @copyright Copyright (c) tuya.inc 2022
*
*/

#include "light_set_color.h"
#include "light_printf.h"
#include "light_types.h"

#define LIGHT_SEND_VALUE_MAX	1000

STATIC DRIVER_MODE_E sg_driver_mode = DRIVER_MODE_PWM;    //驱动模式
STATIC UCHAR_T sg_drive_init_flag = FALSE;    //驱动初始化标志

/**
 * @brief: 灯驱动初始化
 * @param[in]: light_config 驱动参数
 * @return: OPERATE_LIGHT 0表示成功，非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_light_drive_init(IN driver_config_t *light_config)
{
    OPERATE_LIGHT ret = -1;

    if (sg_drive_init_flag != FALSE) {
        PR_NOTICE("Drive init already init ok");
        return LIGHT_OK;
    }

    sg_driver_mode = light_config->mode;

    switch (light_config->mode) {
        case DRIVER_MODE_PWM : {
            user_pwm_init_t *pwm_config = (user_pwm_init_t *)(&light_config->config);

            //pwm初始化
            ret = tuya_user_pwm_init(pwm_config);
            if (ret != LIGHT_OK) {
                PR_ERR("Light drive pwm init error!");
                return LIGHT_INVALID_PARM;
            }
            break;
        }
        default :
            PR_ERR("Light Drive Init mode error!");
            break;
    }

    //已初始化
    sg_drive_init_flag = TRUE;
    
    return LIGHT_OK;
}

/**
 * @brief: 设置灯的RGBCW值
 * @param[in]: red 	 R值(范围: 0~1000)
 * @param[in]: green G值(范围: 0~1000)
 * @param[in]: blue	 B值(范围: 0~1000)
 * @param[in]: cold  C值(范围: 0~1000)
 * @param[in]: warm  W值(范围: 0~1000)
 * @return: OPERATE_LIGHT 0表示成功，非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_light_set_rgbcw(IN USHORT_T red, IN USHORT_T green, IN USHORT_T blue, IN USHORT_T cold, IN USHORT_T warm)
{
    OPERATE_LIGHT ret = -1;
  
    switch(sg_driver_mode) {
        case DRIVER_MODE_PWM: {
    
            user_pwm_color_t pwm_color; 
            memset(&pwm_color, 0, SIZEOF(user_pwm_color_t));   

            //pwm color range 0 ~ 1000
            pwm_color.red = red;       
            pwm_color.green = green;
            pwm_color.blue = blue;
            pwm_color.cold = cold;
            pwm_color.warm = warm;
            //用户pwm设置RGBCW值
            ret = tuya_user_pwm_set_rgbcw(&pwm_color);
            if (ret != LIGHT_OK) {
                PR_ERR("PWM send data error!");
            }
            break;
        }
        default:
            PR_ERR("DRIVER mode error!");
            break;
    }   

    if (LIGHT_OK != ret) {
        PR_ERR("Light set RGBCW ERROR!");
    }
    return ret;
}


