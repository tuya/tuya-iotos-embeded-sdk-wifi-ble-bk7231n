/*
 * @Author: wls
 * @email: wuls@tuya.com
 * @LastEditors: wls
 * @file name: light_set_color.h
 * @Description: light set color(choose drive mode)include file
 * @Copyright: HANGZHOU TUYA INFORMATION TECHNOLOGY CO.,LTD
 * @Company: http://www.tuya.com
 * @Date: 2019-04-26 13:55:40
 * @LastEditTime: 2020-03-06 11:36:54
 */

#ifndef __LIHGT_SET_COLOR_H__
#define __LIHGT_SET_COLOR_H__

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#include "light_types.h"

#include "user_pwm.h"

/* enum need to define in json analysize & get file */
//灯驱动模式枚举值
typedef BYTE_T DRIVER_MODE_E;
#define DRIVER_MODE_PWM     0

//驱动配置结构体
typedef struct {
    DRIVER_MODE_E mode;
    union{
        user_pwm_init_t pwm_init;
    } config;
}driver_config_t;


/**
 * @brief: 灯驱动初始化
 * @param[in]: light_config 驱动参数
 * @return: OPERATE_LIGHT 0表示成功，非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_light_drive_init(IN driver_config_t *light_config);


/**
 * @brief: 设置灯的RGBCW值
 * @param[in]: red 	 R值(范围: 0~1000)
 * @param[in]: green G值(范围: 0~1000)
 * @param[in]: blue	 B值(范围: 0~1000)
 * @param[in]: cold  C值(范围: 0~1000)
 * @param[in]: warm  W值(范围: 0~1000)
 * @retval: OPERATE_LIGHT 0表示成功，非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_light_set_rgbcw(IN USHORT_T red, IN USHORT_T green, IN USHORT_T blue, IN USHORT_T cold, IN USHORT_T warm);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif  /* __LIHGT_SET_COLOR_H__ */
