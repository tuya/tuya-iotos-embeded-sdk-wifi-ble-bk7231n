 /*============================================================================
 *                                                                            *
 * Copyright (C) by Tuya Inc                                                  *
 * All rights reserved                                                        *
 *                                                                            *
 =============================================================================*/

#ifndef __WF_PROTECT__
#define __WF_PROTECT__

#ifdef __cplusplus
extern "C" {
#endif

/*============================ INCLUDES ======================================*/
#include "tuya_os_adapter.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
enum tuya_wfpt_state {
    TUYA_WFPT_SWICTH_START,
    TUYA_WFPT_SWICTH_SUCCESS,
    TUYA_WFPT_SWICTH_FALIED,
}; typedef uint8_t tuya_wfpt_state_t;

typedef void tuya_wfpt_state_cb_t (tuya_wfpt_state_t state, void *priv_data);

/*============================ PROTOTYPES ====================================*/
int tuya_wifi_protect_init(tuya_wfpt_state_cb_t *state_cb, void *priv_data);
int tuya_wifi_protect_backup_ap_check(AP_IF_S *ap_list, uint8_t ap_num);


#ifdef __cplusplus
} // extern "C"
#endif

#endif
