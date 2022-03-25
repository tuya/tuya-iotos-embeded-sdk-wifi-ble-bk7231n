/**
* @file soc_pwm.c
* @author www.tuya.com
* @brief soc PWM proc
* @version 0.2
* @date 2022-03-25
*
* @copyright Copyright (c) tuya.inc 2022
*
*/

#include "soc_pwm.h"
#include "tuya_gpio.h"
#include "light_printf.h"
#include "tuya_pwm.h"

typedef tuya_pwm_t* P_PWM_LIST;   

STATIC P_PWM_LIST *sg_pwm_list = NULL;

#define ALLOW_CHANNEL_NUM     5

#define MODE_C_NUM            1
#define MODE_CW_NUM           2
#define MODE_RGB_NUM          3
#define MODE_RGBC_NUM         4
#define MODE_RGBCW_NUM        5

#define MIN_FREQUENCY        100
#define MAX_FREQUENCY        20000

#define PORT_PWM3    3      
#define PORT_PWM4    4

/**
 * @brief: SOC pwm初始化
 * @param[in]: frequency -->  PWM的频率, 单位: HZ(范围: 500 ~ 20KHz) 
 * @param[in]: duty --> 占空值(范围: 0 ~ 1000), 占空比 = duty / 1000
 * @param[in]: channel_num --> 输出PWM的通道数
 * @param[in]: gpio_list --> 输出PWM的GPIO口列表
 * @param[in]: cct_flag --> cct模式标志
 * @return: OPERATE_LIGHT 0表示成功，非0请参照tuya error code描述文档 
 */
OPERATE_LIGHT tuya_soc_pwm_init(IN USHORT_T frequency, IN USHORT_T duty, IN UCHAR_T channel_num, IN UCHAR_T *gpio_list, IN BOOL_T cct_flag)
{

    UCHAR_T i = 0;

    FLOAT_T duty_ratio = 0.0;   //占空比 duty / 1000

    sg_pwm_list = (P_PWM_LIST *)Malloc(channel_num * SIZEOF(P_PWM_LIST));
    if (sg_pwm_list == NULL) {
        PR_ERR("SOC PWM Init Error!");
        goto INIT_ERROR;
    }

    memset(sg_pwm_list, 0, channel_num); 

    //检查通道数
    if (channel_num > ALLOW_CHANNEL_NUM) {
        PR_ERR("set channel num error!");   
        goto INIT_ERROR;
    }
    //检查频率范围
    if ((frequency < MIN_FREQUENCY) || (frequency > MAX_FREQUENCY)) {    
        PR_ERR("set frequency error!");   
        goto INIT_ERROR;
    }

    duty_ratio = duty / 1000.0;

    PR_DEBUG("duty_ratio = %f, cct_flag = %d, channel_num = %d", duty_ratio, cct_flag, channel_num);


    for (i = 0; i < channel_num; i++) {

        UCHAR_T port = channel_num < MODE_RGB_NUM ? MODE_RGB_NUM + i : i;    //固定 pwm0~4 对应 RGBCW 输出端口

        //找到对应的pwm对象
        sg_pwm_list[i] = tuya_driver_find(TUYA_DRV_PWM, port);
        if (sg_pwm_list[i] == NULL) {
            PR_ERR("tuya driver find %d error!", port);
            goto INIT_ERROR;
        }

        //设置pwm的参数                    //应用代码对gpio_list做处理
        TUYA_PWM_CFG(sg_pwm_list[i], gpio_list[i], frequency, duty_ratio); 

        if ((channel_num == MODE_RGBCW_NUM || channel_num == MODE_CW_NUM) && (port == PORT_PWM3 || port == PORT_PWM4)) {

            if (!cct_flag) {
                //CW驱动的差异化处理
                tuya_pwm_polarity_set(sg_pwm_list[i], TUYA_PWM_NEGATIVE);
            }

        }

        tuya_pwm_init(sg_pwm_list[i]);
        tuya_pwm_start(sg_pwm_list[i]);

    } 

    return OPRT_OK;

INIT_ERROR:

    if (sg_pwm_list != NULL) {
        Free(sg_pwm_list);
        sg_pwm_list = NULL;
    }

    return OPRT_INVALID_PARM;
}

/**
 * @brief: 设置pwm的占空比
 * @param[in]: channel -> pwm通道
 * @param[in]: duty   -> 占空值(范围: 0 ~ 1000)
 * @attention channel --> according to the initlize order
 * @return: none
 */
VOID tuya_soc_pwm_set_duty(IN UCHAR_T channel, IN USHORT_T duty) 
{
    
    FLOAT_T duty_ratio = 0.0;

    if (sg_pwm_list == NULL) {
        PR_ERR("SOC PWM NOT INIT");
        return;
    }

    if (sg_pwm_list[channel] == NULL) {
        PR_ERR("SOC PWM SET DUTY ERROR");
        return;
    }

    duty_ratio = (FLOAT_T)(duty / 1000.0);

    tuya_pwm_duty_set(sg_pwm_list[channel], duty_ratio);

    return;
}
