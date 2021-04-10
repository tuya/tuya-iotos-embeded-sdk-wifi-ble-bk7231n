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

#ifndef __TUYA_PWM_H__
#define __TUYA_PWM_H__
/*============================ INCLUDES ======================================*/
#include "tuya_driver.h"

#ifdef __cplusplus
	extern "C" {
#endif

/*============================ MACROS ========================================*/
#define TUYA_PWM_FREQUENCY(__PWM)   ((uint32_t)(1000000000 / (__PWM)->cfg.period_ns))
#define TUYA_PWM_PERCENT(__PWM)     ((__PWM)->cfg.percent)
/*============================ MACROFIED FUNCTIONS ===========================*/
#define TUYA_PWM_CFG(__PWM, __PIN, __FREQUENCY, __PERCENT)                      \
    (__PWM)->cfg.pin       = __PIN;                                             \
    (__PWM)->cfg.period_ns = (uint32_t)1000000000 / (__FREQUENCY);              \
    (__PWM)->cfg.percent   = __PERCENT;                                         \
    (__PWM)->cfg.pulse_ns  = (uint32_t)((__PWM)->cfg.period_ns * (__PERCENT));  \
    (__PWM)->cfg.polarity  = TUYA_PWM_POSITIVE

/*============================ TYPES =========================================*/
/*============================ LOCAL VARIABLES ===============================*/
typedef enum {
    TUYA_PWM0 = 0x00,
    TUYA_PWM1,
    TUYA_PWM2,
    TUYA_PWM3,
    TUYA_PWM4,
    TUYA_PWM5,
    TUYA_PWM6,
    TUYA_PWM7,
    TUYA_PWM8,
} tuya_pwm_port_t;

typedef enum {
    TUYA_PWM_POSITIVE = 0,
    TUYA_PWM_NEGATIVE,
} tuya_pwm_polarity_t;

typedef enum {
    TUYA_PWM_SET_CMD        = TUYA_DRV_CUSTOM_CMD,
    TUYA_PWM_PERIOD_SET_CMD,
    TUYA_PWM_PULSE_SET_CMD,
    TUYA_PWM_POLARIITY_SET_CMD,
} tuya_pwm_cmd_t;

typedef struct {
    uint8_t         pin;
    uint8_t         polarity;
    uint32_t        period_ns;
    uint32_t        pulse_ns;
    float           percent;
} tuya_pwm_cfg_t;

typedef struct tuya_pwm tuya_pwm_t;

typedef struct {
    int (*init)     (tuya_pwm_t *pwm, tuya_pwm_cfg_t *cfg);
    int (*start)    (tuya_pwm_t *pwm);
    int (*stop)     (tuya_pwm_t *pwm);
    int (*control)  (tuya_pwm_t *pwm, uint8_t cmd, void *arg);
    int (*deinit)   (tuya_pwm_t *pwm);
} tuya_pwm_ops_t;

struct tuya_pwm {
    tuya_drv_node_t     node;
    tuya_pwm_cfg_t      cfg;
    tuya_pwm_ops_t     *ops;
};

/*============================ PROTOTYPES ====================================*/
int tuya_pwm_init           (tuya_pwm_t *pwm);
int tuya_pwm_start          (tuya_pwm_t *pwm);
int tuya_pwm_stop           (tuya_pwm_t *pwm);
int tuya_pwm_control        (tuya_pwm_t *pwm, uint8_t cmd, void *arg);
int tuya_pwm_deinit         (tuya_pwm_t *pwm);

int tuya_pwm_set            (tuya_pwm_t *pwm, float frequency, float percent);
int tuya_pwm_frequency_set  (tuya_pwm_t *pwm, float frequency);
int tuya_pwm_duty_set       (tuya_pwm_t *pwm, float percent);
int tuya_pwm_polarity_set   (tuya_pwm_t *pwm, tuya_pwm_polarity_t polarity);


#ifdef __cplusplus
} // extern "C"
#endif

#endif

