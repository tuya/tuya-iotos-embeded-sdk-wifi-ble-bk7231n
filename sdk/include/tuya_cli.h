 /*============================================================================
 *                                                                            *
 * Copyright (C) by Tuya Inc                                                  *
 * All rights reserved                                                        *
 *                                                                            *
 * @author  :   Linch                                                         *
 * @date    :   2020-05-13                                                    *
 * @brief   :                                                                 *
 * @log     :                                                                 *
 =============================================================================*/

#ifndef __SVC_ASTRO_TIMER__
#define __SVC_ASTRO_TIMER__

#ifdef __cplusplus
extern "C" {
#endif

/*============================ INCLUDES ======================================*/
#include "stdint.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
typedef void (*cli_cmd_func_cb_t)(int argc, char *argv[]);

typedef struct {
    char                *name;
    char                *help;
    cli_cmd_func_cb_t    func;
} cli_cmd_t;
/*============================ PROTOTYPES ====================================*/

//! default uart0
int tuya_cli_init(void);
int tuya_cli_cmd_register(const cli_cmd_t *cmd, uint8_t num);
int tuya_cli_init_with_uart(uint8_t uart_num);


#ifdef __cplusplus
} // extern "C"
#endif

#endif