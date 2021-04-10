 /*============================================================================
 *                                                                            *
 * Copyright (C) by Tuya Inc                                                  *
 * All rights reserved                                                        *
 *                                                                            *
 =============================================================================*/

#ifndef __TUYA_PIN_H__
#define __TUYA_PIN_H__

/*============================ INCLUDES ======================================*/
#include "tuya_driver.h"

#ifdef __cplusplus
	extern "C" {
#endif

/*============================ MACROS ========================================*/
#define TUYA_PINS_NAME(PORT)                            \
    PORT##0,  PORT##1,  PORT##2,  PORT##3,  PORT##4,    \
    PORT##5,  PORT##6,  PORT##7,  PORT##8,  PORT##9,    \
    PORT##10, PORT##11, PORT##12, PORT##13, PORT##14,   \
    PORT##15, PORT##16, PORT##17, PORT##18, PORT##19,   \
    PORT##20, PORT##21, PORT##22, PORT##23, PORT##24,   \
    PORT##25, PORT##26, PORT##27, PORT##28, PORT##29,   \
    PORT##30, PORT##31 

//! 2bit
#define TUYA_PIN_INIT_LOW               ((uint16_t)(0x01 << 0))
#define TUYA_PIN_INIT_HIGH              ((uint16_t)(0x02 << 0))
#define TUYA_PIN_INIT_MASK              ((uint16_t)(0x03 << 0))

//! 3bit
#define TUYA_PIN_IN                     ((uint16_t)(0x01 << 2))
#define TUYA_PIN_IN_FL                  ((uint16_t)(0x02 << 2))
#define TUYA_PIN_IN_IRQ                 ((uint16_t)(0x03 << 2))
#define TUYA_PIN_OUT_PP                 ((uint16_t)(0x04 << 2))
#define TUYA_PIN_OUT_OD                 ((uint16_t)(0x05 << 2))
#define TUYA_PIN_INOUT_MASK             ((uint16_t)(0x07 << 2))

//! 3bit
#define TUYA_PIN_IRQ_RISE               ((uint16_t)(0x01 << 5))
#define TUYA_PIN_IRQ_FALL               ((uint16_t)(0x02 << 5))
#define TUYA_PIN_IRQ_RISE_FALL          ((uint16_t)(0x03 << 5))
#define TUYA_PIN_IRQ_LOW                ((uint16_t)(0x04 << 5))
#define TUYA_PIN_IRQ_HIGH               ((uint16_t)(0x05 << 5))
#define TUYA_PIN_IRQ_MASK               ((uint16_t)(0x07 << 5))

//! 4bit 
#define TUYA_PIN_PULL_UP                ((uint16_t)(0x01 << 9))
#define TUYA_PIN_PULL_DOWN              ((uint16_t)(0x02 << 9))
#define TUYA_PIN_PULL_NONE              ((uint16_t)(0x03 << 9))
#define TUYA_PIN_MODE_MASK              ((uint16_t)(0x0F << 9))


/*============================ MACROFIED FUNCTIONS ===========================*/
#define TUYA_PIN_NUM(__PINNAME)         (__PINNAME & 0x1F)
#define TUYA_PIN_PORT(__PINNAME)        (__PINNAME >> 5)

/*============================ TYPES =========================================*/
//! 3bit(bit7-bit5) port   5bit(bit4-bit0) pinmun
typedef enum {
    TUYA_PA  = 0,
    TUYA_PB  = 1,
    TUYA_PC  = 2,
    TUYA_PD  = 3,
    TUYA_PE  = 4,
} tuya_pin_port_t;

//! TUYA_PA ->  TUYA_PA0 - TUYA_PA31
//! TUYA_PB ->  TUYA_PB0 - TUYA_PB31
//! TUYA_PC ->  TUYA_PC0 - TUYA_PC31
//! TUYA_PD ->  TUYA_PD0 - TUYA_PD31
//! TUYA_PE ->  TUYA_PE0 - TUYA_PE31
typedef enum {
    TUYA_PINS_NAME(TUYA_PA),
    TUYA_PINS_NAME(TUYA_PB),
    TUYA_PINS_NAME(TUYA_PC),
    TUYA_PINS_NAME(TUYA_PD),
    TUYA_PINS_NAME(TUYA_PE),
} tuya_pin_name_t;

typedef enum {
    TUYA_PIN_LOW  = 0,
    TUYA_PIN_HIGH
} tuya_pin_level_t;

typedef enum {
    //! PU  ->  pull up
    //! PD  ->  pull dowm
    //! FL  ->  floating
    //! PP  ->  push pull
    //! OD  ->  open drain
    //! hiz ->  high-impedance level

    TUYA_PIN_MODE_IN_PU               = TUYA_PIN_IN     | TUYA_PIN_PULL_UP,
    TUYA_PIN_MODE_IN_PD               = TUYA_PIN_IN     | TUYA_PIN_PULL_DOWN,
    TUYA_PIN_MODE_IN_FL               = TUYA_PIN_IN_FL,

    TUYA_PIN_MODE_IN_IRQ_RISE         = TUYA_PIN_IN_IRQ | TUYA_PIN_IRQ_RISE | TUYA_PIN_PULL_UP, 
    TUYA_PIN_MODE_IN_IRQ_FALL         = TUYA_PIN_IN_IRQ | TUYA_PIN_IRQ_FALL | TUYA_PIN_PULL_UP,
    TUYA_PIN_MODE_IN_IRQ_RISE_FALL    = TUYA_PIN_IN_IRQ | TUYA_PIN_IRQ_RISE_FALL | TUYA_PIN_PULL_UP,
    TUYA_PIN_MODE_IN_IRQ_LOW          = TUYA_PIN_IN_IRQ | TUYA_PIN_IRQ_LOW   | TUYA_PIN_PULL_UP,
    TUYA_PIN_MODE_IN_IRQ_HIGH         = TUYA_PIN_IN_IRQ | TUYA_PIN_IRQ_HIGH  | TUYA_PIN_PULL_UP,
    
    TUYA_PIN_MODE_OUT_PP_LOW          = TUYA_PIN_OUT_PP | TUYA_PIN_INIT_LOW,
    TUYA_PIN_MODE_OUT_PP_HIGH         = TUYA_PIN_OUT_PP | TUYA_PIN_INIT_HIGH,

    TUYA_PIN_MODE_OUT_PP_PU_LOW       = TUYA_PIN_OUT_PP | TUYA_PIN_PULL_UP | TUYA_PIN_INIT_LOW,
    TUYA_PIN_MODE_OUT_PP_PU_HIGH      = TUYA_PIN_OUT_PP | TUYA_PIN_PULL_UP | TUYA_PIN_INIT_HIGH,

    TUYA_PIN_MODE_OUT_PP_PD_LOW       = TUYA_PIN_OUT_PP | TUYA_PIN_PULL_DOWN | TUYA_PIN_INIT_LOW,
    TUYA_PIN_MODE_OUT_PP_PD_HIGH      = TUYA_PIN_OUT_PP | TUYA_PIN_PULL_DOWN | TUYA_PIN_INIT_HIGH,

    TUYA_PIN_MODE_OUT_OD_LOW          = TUYA_PIN_OUT_OD | TUYA_PIN_INIT_LOW,
    TUYA_PIN_MODE_OUT_OD_HIZ          = TUYA_PIN_OUT_OD | TUYA_PIN_INIT_HIGH,

    TUYA_PIN_MODE_OUT_OD_PU_LOW       = TUYA_PIN_OUT_OD | TUYA_PIN_PULL_UP | TUYA_PIN_INIT_LOW,
    TUYA_PIN_MODE_OUT_OD_PU_HIGH      = TUYA_PIN_OUT_OD | TUYA_PIN_PULL_UP | TUYA_PIN_INIT_HIGH,
} tuya_pin_mode_t;

typedef void (*tuya_pin_irq_cb)(void *args);

typedef struct {
    tuya_pin_mode_t     mode;
    tuya_pin_irq_cb     cb;
    void                *arg;
} tuya_pin_irq_t;

typedef struct {
    int   (*init)       (tuya_pin_name_t pin, tuya_pin_mode_t mode);
    int   (*write)      (tuya_pin_name_t pin, tuya_pin_level_t  level);
    int   (*read)       (tuya_pin_name_t pin);
    int   (*toggle)     (tuya_pin_name_t pin);
    int   (*control)    (tuya_pin_name_t pin, uint8_t cmd, void *arg);
} tuya_pin_ops_t;

/*============================ PROTOTYPES ====================================*/
int tuya_pin_init         (tuya_pin_name_t pin, tuya_pin_mode_t  mode);
int tuya_pin_write        (tuya_pin_name_t pin, tuya_pin_level_t level);
int tuya_pin_read         (tuya_pin_name_t pin);
int tuya_pin_control      (tuya_pin_name_t pin, uint8_t cmd, void *arg);
int tuya_pin_irq_init     (tuya_pin_name_t pin, tuya_pin_mode_t irq_mode, tuya_pin_irq_cb cb, void *arg);
int tuya_pin_irq_enable   (tuya_pin_name_t pin);
int tuya_pin_irq_disable  (tuya_pin_name_t pin);
int tuya_pin_register     (tuya_pin_ops_t *ops);


#ifdef __cplusplus
} // extern "C"
#endif

#endif
