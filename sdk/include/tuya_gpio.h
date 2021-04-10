 /*============================================================================
 *                                                                            *
 * Copyright (C) by Tuya Inc                                                  *
 * All rights reserved                                                        *
 *                                                                            *
 =============================================================================*/

#ifndef __TUYA_GPIO_H__
#define __TUYA_GPIO_H__

#ifdef __cplusplus
	extern "C" {
#endif

#ifdef  __TUYA_GPIO_GLOBALS
    #define __TUYA_GPIO_EXT
#else
    #define __TUYA_GPIO_EXT extern
#endif
/*============================ INCLUDES ======================================*/
#include "tuya_cloud_types.h"

/*============================ MACROS =========================================*/
typedef enum {
    TY_GPIOA_0 = 0,
    TY_GPIOA_1,
    TY_GPIOA_2,
    TY_GPIOA_3,
    TY_GPIOA_4,
    TY_GPIOA_5,
    TY_GPIOA_6,
    TY_GPIOA_7,
    TY_GPIOA_8,
    TY_GPIOA_9,
    TY_GPIOA_10,
    TY_GPIOA_11,
    TY_GPIOA_12,
    TY_GPIOA_13,
    TY_GPIOA_14,
    TY_GPIOA_15,
    TY_GPIOA_16,
    TY_GPIOA_17,
    TY_GPIOA_18,
    TY_GPIOA_19,
    TY_GPIOA_20,
    TY_GPIOA_21,
    TY_GPIOA_22,
    TY_GPIOA_23,
    TY_GPIOA_24,
    TY_GPIOA_25,
    TY_GPIOA_26,
    TY_GPIOA_27,
    TY_GPIOA_28,
    TY_GPIOA_29,
    TY_GPIOA_30,
    TY_GPIOA_31,
    TY_GPIOB_0 = 32,
    TY_GPIOB_1,
    TY_GPIOB_2,
    TY_GPIOB_3,
    TY_GPIOB_4,
    TY_GPIOB_5,
    TY_GPIOB_6,
    TY_GPIOB_7,
    TY_GPIOB_8,
    TY_GPIOB_9,
    TY_GPIOB_10,
    TY_GPIOB_11,
    TY_GPIOB_12,
    TY_GPIOB_13,
    TY_GPIOB_14,
    TY_GPIOB_15,
    TY_GPIOB_16,
    TY_GPIOB_17,
    TY_GPIOB_18,
    TY_GPIOB_19,
    TY_GPIOB_20,
    TY_GPIOB_21,
    TY_GPIOB_22,
    TY_GPIOB_23,
    TY_GPIOB_24,
    TY_GPIOB_25,
    TY_GPIOB_26,
    TY_GPIOB_27,
    TY_GPIOB_28,
    TY_GPIOB_29,
    TY_GPIOB_30,
    TY_GPIOB_31
}TY_GPIO_PORT_E;

typedef enum {
    TY_GPIO_PULLUP,                    /*!< Pad pull up            */
    TY_GPIO_PULLDOWN,                  /*!< Pad pull down          */
    TY_GPIO_PULLUP_PULLDOWN,           /*!< Pad pull up + pull down*/
    TY_GPIO_OPENDRAIN,                 /*!< Pad open drain         */
    TY_GPIO_FLOATING,                  /*!< Pad floating           */
} TY_GPIO_MODE_E;

typedef enum {
    TY_IRQ_NONE,
    /** IRQ occurs on rising edge */
    TY_IRQ_RISE,
    /** IRQ occurs on falling edge */
    TY_IRQ_FALL,
    /** IRQ occurs on either edge */
    TY_IRQ_BOTH,
    /** IRQ occurs when line is low */
    TY_IRQ_HIGH,
    /** IRQ occurs when line is high */
    TY_IRQ_LOW
} TY_GPIO_IRQ_TRIG_E;
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ PROTOTUYAPES ===================================*/
/***********************************************************
*  Function: tuya_gpio_inout_set
*  Input: port
*         in->TRUE:in
*             FALSE:out
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
__TUYA_GPIO_EXT \
OPERATE_RET tuya_gpio_inout_set(IN CONST TY_GPIO_PORT_E port, IN CONST BOOL_T in);

/***********************************************************
*  Function: tuya_gpio_inout_set_select
*  Input: port
*          in->TRUE:in
*              FALSE:out
          high->TRUE
               FALSE
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET tuya_gpio_inout_set_select(IN CONST TY_GPIO_PORT_E port,IN CONST BOOL_T in,IN CONST BOOL_T high);

/***********************************************************
*  Function: tuya_gpio_mode_set
*  Input: port
*         in->  TY_GPIO_PULLUP,
*               TY_GPIO_PULLDOWN,
*               TY_GPIO_PULLUP_PULLDOWN,
*               TY_GPIO_OPENDRAIN,
*               TY_GPIO_FLOATING
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
__TUYA_GPIO_EXT \
OPERATE_RET tuya_gpio_mode_set(IN CONST TY_GPIO_PORT_E port, IN CONST TY_GPIO_MODE_E mode);

/***********************************************************
*  Function: tuya_gpio_read
*  Input: port
*         in->TRUE:in
*             FALSE:out
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
__TUYA_GPIO_EXT \
INT_T tuya_gpio_read(IN CONST TY_GPIO_PORT_E port);

/***********************************************************
*  Function: tuya_gpio_write
*  Input: port
*         high->TRUE:high
*               FALSE:low
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
__TUYA_GPIO_EXT \
OPERATE_RET tuya_gpio_write(IN CONST TY_GPIO_PORT_E port, IN CONST BOOL_T high);


/***********************************************************
*  Function: tuya_gpio_write
*  Input: port
*         gpio_irq_cb   
*         trig_type
*         arg
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
typedef VOID (*TY_GPIO_IRQ_CB)(VOID *args);

__TUYA_GPIO_EXT \
OPERATE_RET tuya_gpio_irq_init(IN CONST TY_GPIO_PORT_E port,IN CONST TY_GPIO_IRQ_CB gpio_irq_cb, IN CONST TY_GPIO_IRQ_TRIG_E trig_type, UINT_T id);

#ifdef __cplusplus
}
#endif
#endif
