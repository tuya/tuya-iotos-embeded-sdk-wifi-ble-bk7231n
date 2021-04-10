/*
 * @Author: wls
 * @email: wuls@tuya.com
 * @LastEditors: wls
 * @file name: user_pwm.h
 * @Description: pwm send out include file
 * @Copyright: HANGZHOU TUYA INFORMATION TECHNOLOGY CO.,LTD
 * @Company: http://www.tuya.com
 * @Date: 2019-04-22 20:01:00
 * @LastEditTime: 2019-08-13 11:23:10
 */


#ifndef __USER_PWM_H__
#define __USER_PWM_H__


#include "light_types.h"


#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/**
 * @brief pwm init data structure
 * pwm send data structure
 */
typedef struct {
    USHORT_T freq;          // PWM Frequency
    USHORT_T duty;          // PWM Init duty
    UCHAR_T list[5];        // GPIO List 
    UCHAR_T channel_num;    // GPIO List length
    BOOL_T polarity;        // PWM output polarity
    UCHAR_T ctrl_pin;       // CTRL pin parm
    BOOL_T  ctrl_level;     // Enable level
    BOOL_T  cct_flag;       // CCT drive mode flag
}user_pwm_init_t;

/**
 * @brief pwm send data structure
 * pwm send data structure
 */
typedef struct {
    USHORT_T red;         // R value,rang from 0 to 1000
    USHORT_T green;       // G value,rang from 0 to 1000
    USHORT_T blue;        // B value,rang from 0 to 1000
    USHORT_T cold;        // C value,rang from 0 to 1000
    USHORT_T warm;        // W value,rang from 0 to 1000
}user_pwm_color_t;

/**
 * @brief: pwm初始化，用户调用接口
 * @param[in]: pwm_init pwm初始化参数
 *          usFreq -> PWM Frequency unit Hz, range:100~20000Hz
 *          usDuty -> PWM Init duty unit 0.1%, range: 0~1000 
 *          ucList[5] -> RGBCW GPIO list
 *          ucChannelNum -> channel num, rang 1~5 way
 *          bPolarity -> PWM output polarity
 *              TRUE -> positive
 *              FAlse -> negative          
 *          ucCtrlPin -> ctrl pin
 *          bCtrlLevel -> ctrl pin level}
 * @attention ucList[5] parm set
 *              List order is always RGBCW !
 * @retval: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_user_pwm_init(IN user_pwm_init_t *pwm_init);

/**
 * @brief: 设置rgbcw值, 用户调用接口
 * @param[in]: color_data rgbcw数据
 *          usRed   -> R send duty,rang from 0 to 1000,unit 0.1%
 *          usGreen -> G send duty,rang from 0 to 1000,unit 0.1%
 *          usBlue  -> B send duty,rang from 0 to 1000,unit 0.1%
 *          usCold  -> C send duty,rang from 0 to 1000,unit 0.1%
 *          usWarm  -> W send duty,rang from 0 to 1000,unit 0.1%}
 * @retval: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_user_pwm_set_rgbcw(IN user_pwm_color_t *color_data);



#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif  /* __USER_PWM_H__ */
