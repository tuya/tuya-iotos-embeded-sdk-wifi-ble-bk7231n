 /*============================================================================
 *                                                                            *
 * Copyright (C) by Tuya Inc                                                  *
 * All rights reserved                                                        *
 *                                                                            *
 =============================================================================*/

#ifndef __TUYA_RTC_H__
#define __TUYA_RTC_H__
/*============================ INCLUDES ======================================*/
#include "tuya_driver.h"
#include "time.h"

/*============================ MACROS =========================================*/
/*============================ TUYAPES ========================================*/
typedef struct {
    int (*init)       (void);
    int (*time_get)   (time_t *timestamp);
    int (*time_set)   (time_t  timestamp);
    int (*control)    (uint8_t cmd, void *arg);
    int (*deinit)     (void);
} tuya_rtc_ops_t;

/*============================ PROTOTUYAPES ===================================*/
int  tuya_rtc_init        (void);
int  tuya_rtc_time_set    (time_t   timestamp);
int  tuya_rtc_time_get    (time_t  *timestamp);
int  tuya_rtc_control     (uint8_t cmd, void *arg);
int  tuya_rtc_deinit      (void);

int tuya_rtc_register(tuya_rtc_ops_t *ops);

#endif