#ifndef TUYA_BLE_TYPE_H__
#define TUYA_BLE_TYPE_H__

#include <stdbool.h>
#include <stdint.h>
#include "tuya_ble_config.h"

#if defined(TY_BT_MOD) && (TY_BT_MOD==1)

typedef struct {
    uint32_t ack_sn;
    uint16_t type;
    uint16_t len;
    uint8_t  *data;
}tuya_ble_user_data_t;

#endif

#endif

