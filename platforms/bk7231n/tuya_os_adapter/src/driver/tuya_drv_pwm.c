 /*============================================================================
 *                                                                            *
 * Copyright (C) by Tuya Inc                                                  *
 * All rights reserved                                                        *
 *                                                                            *
 *                                                                            *
 =============================================================================*/

/*============================ INCLUDES ======================================*/
#include "tuya_os_adapter_error_code.h"
#include "tuya_os_adapt_output.h"
#include "drv_model_pub.h"
#include "tuya_pwm.h"
#include "BkDriverPwm.h"

/*============================ MACROS ========================================*/
#define PWM_DEV_NUM             6
#define PWM_POLARITTY_INIT_CNT  2

/*============================ TYPES =========================================*/


/*============================ PROTOTYPES ====================================*/
static int pwm_dev_init(tuya_pwm_t   *pwm, tuya_pwm_cfg_t *cfg);
static int pwm_dev_start(tuya_pwm_t  *pwm);
static int pwm_dev_stop(tuya_pwm_t  *pwm);
static int pwm_dev_control(tuya_pwm_t  *pwm, uint8_t cmd, void *arg);
static int pwm_dev_deinit(tuya_pwm_t   *pwm);

/*============================ LOCAL VARIABLES ===============================*/
static tuya_pwm_t  s_pwm_dev[PWM_DEV_NUM];

static const tuya_pwm_ops_t  s_pwm_dev_ops = {
    .init       = pwm_dev_init,
    .start      = pwm_dev_start,
    .stop       = pwm_dev_stop,
    .control    = pwm_dev_control,
    .deinit     = pwm_dev_deinit
};

static unsigned char polarity_init_cnt = 0;
static char polarity_channel[PWM_POLARITTY_INIT_CNT] = {-1};


/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/
int platform_pwm_init(void)
{
    int  i = 0;

    for (i = 0; i < PWM_DEV_NUM; i++) {
        s_pwm_dev[i].ops = (tuya_pwm_ops_t *)&s_pwm_dev_ops;
        tuya_driver_register(&s_pwm_dev[i].node, TUYA_DRV_PWM, i);
    }

    return OPRT_OS_ADAPTER_OK;
}

static char pwm_find_channel(unsigned char pin)
{
    unsigned char i;
    char pin_channel = 0;
    CONST unsigned char ucGpioAllowList[6][2] = {
                                                    {6,  0},
                                                    {7,  1},
                                                    {8,  2},
                                                    {9,  3},
                                                    {24, 4},
                                                    {26, 5}
                                                };

    for(i = 0; i < 6; i++) {
        if( ucGpioAllowList[i][0] == pin) {
            pin_channel = ucGpioAllowList[i][1];
            break;
        }
    }

    if(i >= 6){
        pin_channel = -1;
    }

    return pin_channel;

}

static int pwm_mutex_check(bk_pwm_t channel_num)
{
    if(PWM_POLARITTY_INIT_CNT != polarity_init_cnt && 0 != polarity_init_cnt) {
        return -1;
    }

    if(channel_num == polarity_channel[0] && PWM_POLARITTY_INIT_CNT == polarity_init_cnt) {
        return 1;

    } else if(channel_num == polarity_channel[1] && PWM_POLARITTY_INIT_CNT == polarity_init_cnt) {
        return 2;

    }

    return 0;
}

static int pwm_dev_init(tuya_pwm_t *pwm, tuya_pwm_cfg_t *cfg)
{
    bk_pwm_t channel_num;
    unsigned int period;
    unsigned int duty;
    static unsigned int cold_duty;
    int ret = 0;
 
    channel_num = pwm_find_channel(cfg->pin);
    if(channel_num < 0 || channel_num >= BK_PWM_MAX) {
        return OPRT_OS_ADAPTER_INVALID_PARM;
    }

    period = (unsigned int) (26 * cfg->period_ns / 1000); //26M(主频) * 周期(ns) = 计数值
    
    duty = (unsigned int) (cfg->percent * period);

    switch(pwm_mutex_check(channel_num)) {
        case 0: 
            ret = bk_pwm_initialize(channel_num, period, duty, 0, 0);
            break;
        case 1: 
            cold_duty = duty;
            break;
        case 2: 
            ret = bk_pwm_cw_initialize(polarity_channel[0], polarity_channel[1], period, cold_duty, duty, 0);
            if(ret == 0) {
                ret = bk_pwm_cw_start(polarity_channel[0], polarity_channel[1]);
            }
            break;
        default:
            cold_duty = duty;    //初始化 互斥 polarity_init_cnt == 1时候，需要同步下 cold_duty 值 （pwm_mutex_check 接口缺陷问题导致）
            return OPRT_OS_ADAPTER_COM_ERROR;
    }
    
    return ret;
}

static int pwm_dev_start(tuya_pwm_t *pwm)
{   
    int ret = 0;
    bk_pwm_t channel_num;

    channel_num = pwm_find_channel(pwm->cfg.pin);
    if(channel_num < 0 || channel_num >= BK_PWM_MAX) {
        return OPRT_OS_ADAPTER_INVALID_PARM;
    }

    switch(pwm_mutex_check(channel_num)) {
        case 0: 
            ret = bk_pwm_start(channel_num);
            break;
        case 1: 
            ;
            break;
        case 2: 
            ret = bk_pwm_cw_start(polarity_channel[0], polarity_channel[1]);
            break;
        default:
            return OPRT_OS_ADAPTER_COM_ERROR;
    }

    return ret;
}

static int pwm_dev_stop(tuya_pwm_t *pwm)
{
    int ret = 0;
    bk_pwm_t channel_num;
    
    channel_num = pwm_find_channel(pwm->cfg.pin);
    if(channel_num < 0 || channel_num >= BK_PWM_MAX) {
        return OPRT_OS_ADAPTER_INVALID_PARM;
    }

    switch(pwm_mutex_check(channel_num)) {
        case 0: 
            ret = bk_pwm_stop(channel_num);
            break;
        case 1: 
            ;
            break;
        case 2: 
            ret = bk_pwm_cw_stop(polarity_channel[0], polarity_channel[1]);
            break;
        default:
            return OPRT_OS_ADAPTER_COM_ERROR;
    }

    return ret;
}

static int pwm_dev_control(tuya_pwm_t *pwm, uint8_t cmd, void *arg)
{
    int ret = 0;
    char channel_num;
    unsigned int period;
    unsigned int duty;
    static unsigned int cold_duty = 0;
    
    switch (cmd) {
    
        case TUYA_PWM_SET_CMD:
        case TUYA_PWM_PERIOD_SET_CMD:
        case TUYA_PWM_PULSE_SET_CMD:
            
            channel_num = pwm_find_channel(pwm->cfg.pin);
            if(channel_num < 0 || channel_num >= BK_PWM_MAX) {
                
                return OPRT_INVALID_PARM;
            }

            period = (unsigned int) (26 * pwm->cfg.period_ns / 1000); //26M(主频) * 周期(ns) = 计数值

            duty = (unsigned int)(pwm->cfg.percent * period);
            //bk_printf("channel_num duty:%d,%d,%d,%d\r\n",pwm->cfg.pin,channel_num,duty,pwm_mutex_check(channel_num));

            switch(pwm_mutex_check(channel_num)) {
                case 0: 
                    //bk_printf("channel_num duty:%d,%d,%d\r\n",pwm->cfg.pin,channel_num,duty);
                    ret = bk_pwm_update_param(channel_num, period, duty, 0, 0);
                    break;
                case 1: 
                    cold_duty = duty;
                    break;
                case 2: 
                    //bk_printf("duty:%d,%d\r\n",cold_duty,duty);
                    ret = bk_pwm_cw_update_param(polarity_channel[0], polarity_channel[1], period, cold_duty, duty, 0);
                    break;
                default:
                    return OPRT_OS_ADAPTER_COM_ERROR;
            }

        break;            
             
        case TUYA_PWM_POLARIITY_SET_CMD:          
            channel_num = pwm_find_channel(pwm->cfg.pin);
            if(channel_num < 0 || channel_num >= BK_PWM_MAX) {
                return OPRT_OS_ADAPTER_INVALID_PARM;
            }
            if(polarity_init_cnt >= PWM_POLARITTY_INIT_CNT) { // 避免多次初始化溢出
                polarity_init_cnt = 0;
            }
            
            polarity_channel[polarity_init_cnt] = channel_num; // 保存极性相反的channel
           
            polarity_init_cnt++;
            LOG_DEBUG("cw set +++++++++\r\n");
            break;

        default : 
            return OPRT_OS_ADAPTER_INVALID_PARM;
        
    }

    return ret;
}


static int pwm_dev_deinit(tuya_pwm_t *pwm)
{
    //! TODO

    return OPRT_OS_ADAPTER_OK;
}


