 /*============================================================================
 *                                                                            *
 * Copyright (C) by Tuya Inc                                                  *
 * All rights reserved                                                        *
 *                                                                            *
 * @author  :   Linch                                                         *
 * @date    :   2019-08-20                                                    *
 * @brief   :                                                                 *
 *                                                                            *
 =============================================================================*/

#ifndef __TUYA_TIMER_H__
#define __TUYA_TIMER_H__

/*============================ INCLUDES ======================================*/
#include "tuya_driver.h"

#ifdef __cplusplus
	extern "C" {
#endif

/*============================ MACROS ========================================*/
#define TUYA_TIMER_CFG(__TIMER, __MODE, __CB, __ARG)                          \
    (__TIMER)->cfg.mode = __MODE;                                             \
    (__TIMER)->cfg.cb   = __CB;                                               \
    (__TIMER)->cfg.arg  = __ARG

/*============================ TYPES =========================================*/
typedef enum {
    TUYA_TIMER0 = 0,
    TUYA_TIMER1,
    TUYA_TIMER2,
    TUYA_TIMER3,
    TUYA_TIMER4,
    TUYA_TIMER5,
} tuya_timer_port_t;

typedef enum {
    TUYA_TIMER_MODE_ONCE = 0,
    TUYA_TIMER_MODE_PERIOD
} tuya_timer_mode_t;


typedef void (*tuya_timer_isr_cb)(void *args);


typedef enum {
    TUYA_TIMER_GET_COUNT_CMD    = TUYA_DRV_CUSTOM_CMD,
} tuya_timer_cmd_t;

typedef struct {
    tuya_timer_mode_t    mode;
    tuya_timer_isr_cb    cb;
    void                *arg;
} tuya_timer_cfg_t;

typedef struct {
    uint8_t             support_freq;
    uint32_t            maxcount;
} tuya_timer_info_t;

typedef struct tuya_timer tuya_timer_t;
//! FOR PORT
typedef struct {
    int (*init)     (tuya_timer_t   *timer, tuya_timer_cfg_t *cfg);
    int (*start)    (tuya_timer_t   *timer, uint32_t us);
    int (*stop)     (tuya_timer_t   *timer);
    int (*control)  (tuya_timer_t   *timer, uint8_t cmd, void *arg);
    int (*deinit)   (tuya_timer_t   *timer);
} tuya_timer_ops_t;

struct tuya_timer {
    tuya_drv_node_t         node;
    tuya_timer_cfg_t        cfg;
    tuya_timer_ops_t       *ops;
};

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
int tuya_timer_init       (tuya_timer_t   *timer);
int tuya_timer_start      (tuya_timer_t   *timer, uint32_t us);
int tuya_timer_stop       (tuya_timer_t   *timer);
int tuya_timer_control    (tuya_timer_t   *timer, uint8_t cmd, void *arg);
int tuya_timer_deinit     (tuya_timer_t   *timer);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
