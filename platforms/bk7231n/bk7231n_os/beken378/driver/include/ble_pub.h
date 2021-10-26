#ifndef _BLE_PUB_H_
#define _BLE_PUB_H_

#include "rtos_pub.h"
#include "ble_rf_port.h"

#define BLE_DEV_NAME		"ble"

#define BLE_CMD_MAGIC              (0xe2a0000)

#define MAX_ADV_DATA_LEN           (0x1F)

enum
{
    CMD_BLE_REG_INIT = BLE_CMD_MAGIC + 1,
	CMD_BLE_REG_DEINIT,
	CMD_BLE_SET_CHANNEL,
	CMD_BLE_AUTO_CHANNEL_ENABLE,
	CMD_BLE_AUTO_CHANNEL_DISABLE,
	CMD_BLE_AUTO_SYNCWD_ENABLE,
	CMD_BLE_AUTO_SYNCWD_DISABLE,
	CMD_BLE_SET_PN9_TRX,
	CMD_BLE_SET_GFSK_SYNCWD,
	CMD_BLE_HOLD_PN9_ESTIMATE,
	CMD_BLE_STOP_COUNTING,
	CMD_BLE_START_COUNTING,
	CMD_BLE_START_TX,
	CMD_BLE_STOP_TX,
};

enum
{
    PN9_RX = 0,
    PN9_TX
};

struct ble_session_env
{
	unsigned char ble_active;
	unsigned  ble_dut_flag;
	uint32_t ble_sleep_enable;
	uint32_t system_sleep_flag;
	uint8_t ble_first_sleep;
	uint8_t ble_deep_sleep;
	uint8_t ble_init_over;
};

extern void set_ble_active_status(char active);
extern int get_ble_active_status(void);
extern void set_ble_dut_flag_status(char ble_dut_flag);
extern uint8 is_rf_switch_to_ble(void);
extern uint32_t rwip_get_current_time(void);
extern uint32_t rwip_get_next_target_time(void);
extern void ble_init(void);
extern void ble_exit(void);
extern void ble_dut_start(void);
extern UINT8 ble_is_start(void);
extern UINT8* ble_get_mac_addr(void);
extern UINT8* ble_get_name(void);
extern uint8_t if_ble_sleep(void);
extern void rf_wifi_used_clr(void);
extern void rf_wifi_used_set(void);
extern UINT32 if_rf_wifi_used(void );
extern void rf_not_share_for_ble(void);
extern void rf_can_share_for_ble(void);

#if CFG_USE_BLE_PS
extern void bk_ble_sleep_check(void);
#endif

extern struct ble_session_env ble_session_ctx;
#endif /* _BLE_PUB_H_ */
// eof

