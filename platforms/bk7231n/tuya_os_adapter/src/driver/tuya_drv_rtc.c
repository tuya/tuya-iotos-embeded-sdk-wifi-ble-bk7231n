 /*============================================================================
 *                                                                            *
 * Copyright (C) by Tuya Inc                                                  *
 * All rights reserved                                                        *
 *                                                                            *
 *                                                                            *
 =============================================================================*/

/*============================ INCLUDES ======================================*/
#include "tuya_rtc.h"
#include "tuya_os_adapter_error_code.h"

/*============================ MACROS ========================================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
static int rtc_dev_init      (void);
static int rtc_dev_time_get  (time_t *timestamp);
static int rtc_dev_time_set  (time_t  timestamp);
static int rtc_dev_control   (uint8_t cmd, void *arg);
static int rtc_dev_deinit    (void);
/*============================ LOCAL VARIABLES ===============================*/
static const tuya_rtc_ops_t  rtc_dev_ops = {
    .init       = rtc_dev_init,
    .time_get   = rtc_dev_time_get,
    .time_set   = rtc_dev_time_set,
    .control    = rtc_dev_control,
    .deinit     = rtc_dev_deinit,
};
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/
int platform_rtc_init(void)
{
    tuya_rtc_register((tuya_rtc_ops_t *) &rtc_dev_ops);

    return OPRT_OS_ADAPTER_OK;
}

static int rtc_dev_init(void)
{
    return OPRT_OS_ADAPTER_OK;
}


static int rtc_dev_time_get(time_t *timestamp)
{    
    return OPRT_OS_ADAPTER_OK;
}

static int rtc_dev_time_set(time_t  timestamp)
{
    return OPRT_OS_ADAPTER_OK;
}

static int rtc_dev_control(uint8_t cmd, void *arg)
{
    return OPRT_OS_ADAPTER_OK;

}

static int rtc_dev_deinit(void)
{
    return OPRT_OS_ADAPTER_OK;
}