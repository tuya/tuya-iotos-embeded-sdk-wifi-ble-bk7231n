 /*============================================================================
 *                                                                            *
 * Copyright (C) by Tuya Inc                                                  *
 * All rights reserved                                                        *
 *                                                                            *
 * @author  :   wuls                                                         *
 * @date    :   2019-10-10                                                    *
 * @brief   :                                                                 *
 *                                                                            *
 =============================================================================*/


/*============================ INCLUDES ======================================*/
#include "tuya_timer.h"
#include "tuya_os_adapt_output.h"
#include "BkDriverTimer.h"

/*============================ MACROS ========================================*/
#define TIMER_DEV_NUM           4


/*============================ TYPES =========================================*/


/*============================ PROTOTYPES ====================================*/
static int timer_dev_init     (tuya_timer_t  *timer, tuya_timer_cfg_t *cfg);
static int timer_dev_start    (tuya_timer_t  *timer, uint32_t us);
static int timer_dev_stop     (tuya_timer_t  *timer);
static int timer_dev_control  (tuya_timer_t  *timer, uint8_t cmd, void *arg);
static int timer_dev_deinit   (tuya_timer_t  *timer);

/*============================ LOCAL VARIABLES ===============================*/
static tuya_timer_t s_timer_dev[TIMER_DEV_NUM];


static const tuya_timer_ops_t  timer_dev_ops = {
    .init       = timer_dev_init,
    .start      = timer_dev_start,
    .stop       = timer_dev_stop,
    .control    = timer_dev_control,
    .deinit     = timer_dev_deinit,
};


/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/
int platform_timer_init(void)
{
    int  i = 0;

    for (i = 0; i < TIMER_DEV_NUM; i++) {
        s_timer_dev[i].ops = (tuya_timer_ops_t *)&timer_dev_ops;
        tuya_driver_register(&s_timer_dev[i].node, TUYA_DRV_TIMER, i);
    }
    
    return OPRT_OK;
}

static int timer_dev_init(tuya_timer_t  *timer, tuya_timer_cfg_t *cfg)
{
    
    return OPRT_OK ;
}

extern OSStatus bk_timer_initialize_us(uint8_t timer_id, uint32_t time_us, void *callback);
static int timer_dev_start(tuya_timer_t  *timer, uint32_t us)
{
    int iRet;

    if((TUYA_TIMER0 == timer->node.port) || (TUYA_TIMER1 == timer->node.port)) { //timer0&timer1
        iRet = bk_timer_initialize_us(timer->node.port, us, timer->cfg.cb);
    } else {
        if(us < 1000) {
            LOG_ERR("tuya timer2~timer3 can't not set cycle less than 1ms\r\n");
            return OPRT_INVALID_PARM; 
        }
        iRet = bk_timer_initialize((timer->node.port + 2), us /1000, timer->cfg.cb); //bk timer2、3 被系统占用了实际 bk timerID 为 timer4~timer5
    }

    return iRet;
}

static int timer_dev_stop(tuya_timer_t  *timer)
{
    int iRet;

    if((TUYA_TIMER0 == timer->node.port) || (TUYA_TIMER1 == timer->node.port)) { //timer0&timer1
        iRet = bk_timer_stop(timer->node.port);
    } else {
        iRet = bk_timer_stop(timer->node.port + 2); //bk timer2、3 被系统占用了 bk timer4~timer5
    }

    return iRet;
}

extern OSStatus bk_timer_read_cnt(uint8_t timer_id, uint32_t *timer_cnt);
static int timer_dev_control(tuya_timer_t  *timer, uint8_t cmd, VOID *arg)
{
    int iRet = 0;
    unsigned int read_cnt = 0;
    
    switch (cmd) {
    
        case TUYA_TIMER_GET_COUNT_CMD:
            if((TUYA_TIMER0 == timer->node.port) || (TUYA_TIMER1 == timer->node.port)) {
                iRet = bk_timer_read_cnt(timer->node.port, (uint32_t *)&read_cnt);
            } else {
                LOG_ERR("tuya timer2~timer3 can't read count!\r\n");
            }
            
            if(iRet != kNoErr) {
                return OPRT_COM_ERROR;
            } 

            *(uint32_t *)arg = read_cnt / 26;  // 获取时间

            break;

        default: 
            return OPRT_INVALID_PARM;
    }
    
    return iRet;
}

static int timer_dev_deinit(tuya_timer_t  *timer)
{
    return OPRT_OK;
}

