 /*============================================================================
 *                                                                            *
 * Copyright (C) by Tuya Inc                                                  *
 * All rights reserved                                                        *
 *                                                                            *
 * @author  :   Linch                                                         *
 * @date    :   2020-03-16                                                    *
 * @brief   :                                                                 *
 * @log     :                                                                 *
 =============================================================================*/

#ifndef __SVC_ASTRO_TIMER__
#define __SVC_ASTRO_TIMER__

#ifdef __cplusplus
extern "C" {
#endif

/*============================ INCLUDES ======================================*/
#include "ty_cJSON.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
int tuya_astro_timer_init(void);
int tuya_astro_timer_mqc_notify(char *atop, char *ver, ty_cJSON *params);
int tuya_astro_timer_reset(void);


#ifdef __cplusplus
} // extern "C"
#endif

#endif