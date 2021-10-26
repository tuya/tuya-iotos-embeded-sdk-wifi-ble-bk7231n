#ifndef __TUYA_FFC_APP_H__
#define __TUYA_FFC_APP_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define FFC_MASTER                    0x01
#define FFC_SLAVER                    0x02

typedef enum {
    FFC_INIT_STATE = 0,
    FFC_CONTROL_STATE,
    FFC_BINDING_BEGIN_STATE,
    FFC_BINDING_SUCCESS_STATE,
    FFC_BINDING_FINSH_STATE,
} ffc_cb_state_t;

typedef int (*ffc_status_cb)(ffc_cb_state_t state);
typedef int (*ffc_recv_cb)(uint8_t *data_cmd, uint16_t data_len);


typedef enum {
    FFC_CHANNEL_SCAN_FLAG = 0x01,
} ffc_flag_t;


typedef struct {
    uint8_t    *uuid;
    uint8_t     auzkey[32 + 1];
    uint8_t     mac[6];
    uint8_t     channel[13];         
    uint8_t     channel_count;     
    uint32_t    flag;
} ffc_cfg_t;

typedef struct {
    int     (*channel_set)(uint8_t new_channel);
    uint8_t (*channel_get)(void);
    int     (*send_frame)(uint8_t *frame, uint16_t frame_len, uint8_t src_mac[6], uint8_t dst_mac[6]);
} ffc_ops_t;

typedef struct {
    ffc_status_cb status_cb;
    ffc_recv_cb   recv_cb;
} ffc_cb_t;


int tuya_iot_wifi_ffc_init(uint8_t role, ffc_status_cb *status_cb, ffc_recv_cb *recv_cb);
int tuya_iot_wifi_ffc_bind(uint16_t timeout_s);
int tuya_iot_wifi_ffc_unbind(uint16_t timeout_s);
int tuya_iot_wifi_ffc_send(uint8_t *data, uint16_t len);
int tuya_iot_wifi_ffc_control(int flag, void *data);


#define ffc_init        tuya_iot_wifi_ffc_init
#define ffc_bind        tuya_iot_wifi_ffc_bind
#define ffc_unbind      tuya_iot_wifi_ffc_unbind
#define ffc_send        tuya_iot_wifi_ffc_send
#define ffc_control     tuya_iot_wifi_ffc_control


#ifdef __cplusplus
}
#endif

#endif  /*__TUYA_FFC_APP_H__ */

