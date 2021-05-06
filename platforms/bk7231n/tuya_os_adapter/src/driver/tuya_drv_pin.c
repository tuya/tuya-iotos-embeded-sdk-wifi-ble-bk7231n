 /*============================================================================
 *                                                                            *
 * Copyright (C) by Tuya Inc                                                  *
 * All rights reserved                                                        *
 *                                                                            *
 =============================================================================*/


/*============================ INCLUDES ======================================*/
#include "tuya_pin.h"
#include "tuya_os_adapter.h"
#include "tuya_os_adapter_error_code.h"
#include "BkDriverGpio.h"
#include "gpio_pub.h"

/*============================ MACROS ========================================*/
#define PIN_DEV_CHECK_ERROR_RETURN(__PIN, __ERROR)                          \
    if (__PIN >= sizeof(pinmap)/sizeof(pinmap[0])) {                        \
        return __ERROR;                                                     \
    }


/*============================ TYPES =========================================*/
typedef struct {
    bk_gpio_t           gpio;
	tuya_pin_irq_cb	 	cb;
    void                *args;
} pin_dev_map_t;

/*============================ PROTOTYPES ====================================*/
static int pin_dev_init(tuya_pin_name_t pin, tuya_pin_mode_t mode);
static int pin_dev_write(tuya_pin_name_t pin, tuya_pin_level_t level);
static int pin_dev_read(tuya_pin_name_t pin);
static int pin_dev_toggle(tuya_pin_name_t pin);
static int pin_dev_control(tuya_pin_name_t pin, uint8_t cmd, void *arg);
/*============================ LOCAL VARIABLES ===============================*/
static pin_dev_map_t pinmap[] = {
    //! PA
    {GPIO0,  NULL, NULL}, {GPIO1,  NULL, NULL}, {GPIO2,  NULL, NULL}, {GPIO3,  NULL, NULL}, 
    {GPIO4,  NULL, NULL}, {GPIO5,  NULL, NULL}, {GPIO6,  NULL, NULL}, {GPIO7,  NULL, NULL}, 
    {GPIO8,  NULL, NULL}, {GPIO9,  NULL, NULL}, {GPIO10, NULL, NULL}, {GPIO11, NULL, NULL}, 
    {GPIO12, NULL, NULL}, {GPIO13, NULL, NULL}, {GPIO14, NULL, NULL}, {GPIO15, NULL, NULL}, 
    {GPIO16, NULL, NULL}, {GPIO17, NULL, NULL}, {GPIO18, NULL, NULL}, {GPIO19, NULL, NULL}, 
    {GPIO20, NULL, NULL}, {GPIO21, NULL, NULL}, {GPIO22, NULL, NULL}, {GPIO23, NULL, NULL}, 
    {GPIO24, NULL, NULL}, {GPIO25, NULL, NULL}, {GPIO26, NULL, NULL}, {GPIO27, NULL, NULL}, 
    {GPIO28, NULL, NULL}, {GPIO29, NULL, NULL}, {GPIO30, NULL, NULL}, {GPIO31, NULL, NULL},
    //! PB
    {GPIO32,  NULL, NULL}, {GPIO33,  NULL, NULL}, {GPIO34,  NULL, NULL}, {GPIO35,  NULL, NULL}, 
    {GPIO36,  NULL, NULL}, {GPIO37,  NULL, NULL}, {GPIO38,  NULL, NULL}, {GPIO39,  NULL, NULL}
};

//! ro data
static const  tuya_pin_ops_t s_pin_dev_ops = {
    .init     = pin_dev_init,
    .write    = pin_dev_write,  
    .read     = pin_dev_read,
    .toggle   = pin_dev_toggle,  
    .control  = pin_dev_control,
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/
int platform_pin_init(void)
{
    return tuya_pin_register((tuya_pin_ops_t *)&s_pin_dev_ops);
}

static int pin_dev_init(tuya_pin_name_t pin, tuya_pin_mode_t mode)
{
    bk_gpio_config_t   bk_gpio_cfg; 

    //! set pin direction
    switch (mode & TUYA_PIN_INOUT_MASK)  {
    case TUYA_PIN_IN:
        bk_gpio_cfg = INPUT_NORMAL;
        break;
    case TUYA_PIN_OUT_PP:
        bk_gpio_cfg = OUTPUT_NORMAL;
        break;
    case TUYA_PIN_OUT_OD:
        bk_gpio_cfg = OUTPUT_NORMAL;
        break;
    default:
        return OPRT_OS_ADAPTER_NOT_SUPPORTED;
    }

    //! set pin mdoe
    switch (mode & TUYA_PIN_MODE_MASK) {
    case TUYA_PIN_PULL_UP:
        bk_gpio_cfg = bk_gpio_cfg == INPUT_NORMAL ? INPUT_PULL_UP : bk_gpio_cfg;
        break;
    case TUYA_PIN_PULL_DOWN:
        bk_gpio_cfg = bk_gpio_cfg == INPUT_NORMAL ? INPUT_PULL_DOWN : bk_gpio_cfg;
        break;
    case TUYA_PIN_PULL_NONE:
        break;
    }

    BkGpioInitialize(pinmap[pin].gpio, bk_gpio_cfg);

    //! set pin init level
    switch (mode & TUYA_PIN_INIT_MASK) {
    case TUYA_PIN_INIT_LOW:
        BkGpioOutputLow(pinmap[pin].gpio);
        break;

    case TUYA_PIN_INIT_HIGH:
        BkGpioOutputHigh(pinmap[pin].gpio);
        break;
    }

    return OPRT_OS_ADAPTER_OK;
}


static int pin_dev_write(tuya_pin_name_t pin, tuya_pin_level_t level)
{
    PIN_DEV_CHECK_ERROR_RETURN(pin, OPRT_OS_ADAPTER_INVALID_PARM);
    if (level == TUYA_PIN_HIGH) {
        BkGpioOutputHigh(pinmap[pin].gpio);
    } else{
        BkGpioOutputLow(pinmap[pin].gpio);
    }

    return OPRT_OS_ADAPTER_OK;
}

static int pin_dev_read(tuya_pin_name_t pin)
{
    PIN_DEV_CHECK_ERROR_RETURN(pin, OPRT_OS_ADAPTER_INVALID_PARM);

    return BkGpioInputGet(pinmap[pin].gpio);
}

static int pin_dev_toggle(tuya_pin_name_t pin)
{
    PIN_DEV_CHECK_ERROR_RETURN(pin, OPRT_OS_ADAPTER_INVALID_PARM);

    BkGpioOutputTrigger(pinmap[pin].gpio);

    return OPRT_OS_ADAPTER_OK;
}


static int pin_dev_irq_config(tuya_pin_name_t pin, tuya_pin_irq_t *irq)
{
    bk_gpio_irq_trigger_t trigger;

    if (TUYA_PIN_IN_IRQ != (irq->mode & TUYA_PIN_INOUT_MASK)) {
        return OPRT_OS_ADAPTER_INVALID_PARM;
    }
    
    switch (irq->mode & TUYA_PIN_IRQ_MASK) {
    case TUYA_PIN_IRQ_RISE:
        trigger = IRQ_TRIGGER_RISING_EDGE; 
        break;
    case TUYA_PIN_IRQ_FALL:
        trigger = IRQ_TRIGGER_FALLING_EDGE; 
        break;
    case TUYA_PIN_IRQ_LOW:
        trigger = IRQ_TRIGGER_LOW_LEVEL; 
        break;
    case TUYA_PIN_IRQ_HIGH:
        trigger = IRQ_TRIGGER_HGIH_LEVEL; 
        break;
    default: return OPRT_OS_ADAPTER_NOT_SUPPORTED;
    }

    pinmap[pin].cb = irq->cb;
    pinmap[pin].args = irq->arg;
    BkGpioEnableIRQ(pinmap[pin].gpio, trigger, (bk_gpio_irq_handler_t)pinmap[pin].cb, pinmap[pin].args);

    return OPRT_OS_ADAPTER_OK;
}


static int pin_dev_control(tuya_pin_name_t pin, uint8_t cmd, void *arg)
{
    int result = OPRT_OK;

    switch (cmd) {

    case TUYA_DRV_SET_ISR_CMD: 
        result = pin_dev_irq_config(pin, (tuya_pin_irq_t *)arg);
        break;

    case TUYA_DRV_SET_INT_CMD:
        PIN_DEV_CHECK_ERROR_RETURN(pin, OPRT_OS_ADAPTER_INVALID_PARM);
        break;

    case TUYA_DRV_CLR_INT_CMD:
        PIN_DEV_CHECK_ERROR_RETURN(pin, OPRT_OS_ADAPTER_INVALID_PARM);
        BkGpioDisableIRQ(pinmap[pin].gpio);
        break;    
    }

    return result;
}
