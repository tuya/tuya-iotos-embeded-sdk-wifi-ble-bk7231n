 /*============================================================================
 *                                                                            *
 * Copyright (C) by Tuya Inc                                                  *
 * All rights reserved                                                        *
 *                                                                            *
 =============================================================================*/

#ifndef __TUYA_ADC_H__
#define __TUYA_ADC_H__

#ifdef __cplusplus
	extern "C" {
#endif

/*============================ INCLUDES ======================================*/
#include "tuya_driver.h"

/*============================ MACROS ========================================*/
#define TUYA_ADC_CFG(__ADC, __PIN, __FLAG)                                    \
    (__ADC)->cfg.pin  = __PIN;                                                \
    (__ADC)->cfg.flag = __FLAG

/*============================ TYPES =========================================*/
typedef enum {
    TUYA_ADC0  = 0,                  
    TUYA_ADC1,       
    TUYA_ADC2,          
} tuya_adc_port_t;

typedef struct {
    uint8_t     pin;
    uint8_t     flag;
} tuya_adc_cfg_t;

typedef struct tuya_adc tuya_adc_t;

typedef struct {
    int  (*init)         (tuya_adc_t *adc, tuya_adc_cfg_t *cfg);
    int  (*convert)      (tuya_adc_t *adc, uint16_t *result);
    int  (*control)      (tuya_adc_t *adc, uint8_t cmd, VOID *arg);
    int  (*deinit)       (tuya_adc_t *adc);
} tuya_adc_ops_t;

struct tuya_adc {
    tuya_drv_node_t     node;
    tuya_adc_cfg_t      cfg;    
    tuya_adc_ops_t     *ops;
};

typedef enum {
    TUYA_ADC_VOLTAGE_CMD = TUYA_DRV_CUSTOM_CMD,
} tuya_adc_cmd_t;

typedef struct {
    uint16_t   *data;
    uint16_t   *voltage;
} tuya_adc_voltage_t;

/*============================ PROTOTYPES ====================================*/
int tuya_adc_init       (tuya_adc_t *adc);
int tuya_adc_control    (tuya_adc_t *adc, uint8_t cmd, void *arg);
int tuya_adc_deinit     (tuya_adc_t *adc);
int tuya_adc_convert    (tuya_adc_t *adc, uint16_t *data, uint16_t  num);
int tuya_adc_voltage    (tuya_adc_t *adc, uint16_t *data, uint16_t *voltage, uint16_t count);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
