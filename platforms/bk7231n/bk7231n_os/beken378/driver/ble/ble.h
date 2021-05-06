#ifndef _BLE_H_
#define _BLE_H_

#include "include.h"
#include "rtos_pub.h"
#include "rwble_config.h"

#define BLE_PS_DEBUG
#ifdef BLE_PS_DEBUG
#define BLE_PS_PRT                 os_printf
#else
#define BLE_PS_PRT                 os_null_printf
#endif

#define BLE_CONN_IDX_MAX           (0x01)
#define BLE_CHAR_DATA_LEN          (128)

#define REG_BLE_XVR_BASE_ADDR                        (0x0080B400)

#define BLE_XVR_SLOT_TIME                            (REG_BLE_XVR_BASE_ADDR + 0x2a * 4)
#define BLE_XVR_RF_TX_DELAY_POSI                     (24)
#define BLE_XVR_RF_TX_DELAY_MASK                     (0xFF)
#define BLE_XVR_RF_RX_DELAY_POSI                     (16)
#define BLE_XVR_RF_RX_DELAY_MASK                     (0xFF)
#define BLE_XVR_TX_SLOT_TIME_POSI                    (8)
#define BLE_XVR_TX_SLOT_TIME_MASK                    (0xFF)
#define BLE_XVR_RX_SLOT_TIME_POSI                    (0)
#define BLE_XVR_RX_SLOT_TIME_MASK                    (0xFF)

#define REG_BLE_XVR_CHANNEL_CONFIG_ADDR              (REG_BLE_XVR_BASE_ADDR + 0x24 * 4)
#define REG_BLE_XVR_CHANNEL_VALUE_POST               (0)
#define REG_BLE_XVR_CHANNEL_VALUE_MASK               (0x7F)
#define REG_BLE_XVR_AUTO_CHANNEL_POST                (17)
#define REG_BLE_XVR_AUTO_SYNCWD_POST                 (19)

#define REG_BLE_XVR_TRX_CONFIG_ADDR                  (REG_BLE_XVR_BASE_ADDR + 0x25 * 4)
#define REG_BLE_XVR_TEST_RADIO_POST                  (13)
#define REG_BLE_XVR_HOLD_ESTIMATE_POST               (9)
#define REG_BLE_XVR_PN9_RECV_POST                    (10)

#define REG_BLE_XVR_GFSK_SYNCWD_ADDR                 (REG_BLE_XVR_BASE_ADDR + 0x20 * 4)


#define REG_BLE_XVR_RADIOON_CONFIG_ADDR              (REG_BLE_XVR_BASE_ADDR + 0x29 * 4)

#define BLE_USER_CMD_OPCODE             (0xFCE0)
#define BLE_USER_CMD_EXIT_DUT_LEN       (2)
#define BLE_USER_CMD_EXIT_DUT_CMD       (0x0E)
#define BLE_USER_CMD_EXIT_DUT_ACT       (0xA0)
#define BLE_USER_CMD_TX_PWR_SET_CMD     (0x10)
#define BLE_USER_CMD_TX_PWR_SAVE_CMD    (0x11)
#define BLE_USER_CMD_XTAL_SET_CMD       (0x12)

// ble dut uart port
#define PORT_UART1                      (1)
#define PORT_UART2                      (2)
#define BLE_DUT_UART_PORT               PORT_UART1  // PORT_UART2


typedef enum {
	BLE_PS_FORBID_KRL = 1,
	BLE_PS_FORBID_ENABLED = 2,
	BLE_PS_FORBID_PREVENT = 3,
	BLE_PS_FORBID_TM = 4,
	BLE_PS_FORBID_RWBT = 5,
	BLE_PS_FORBID_RWBLE = 6,
	BLE_PS_FORBID_EA = 7,
	BLE_PS_FORBID_H4TL = 8,
} BLE_PS_FORBID_STATUS;

typedef struct ble_message 
{
	uint32_t data;
}BLE_MSG_T;

typedef struct rf_time_s
{
    UINT32 ble_rf_time;
    UINT32 wifi_rf_time;
} rf_time_t;

typedef enum
{
    BLE_ROLE_NONE = 0,
    BLE_ROLE_MASTER,
    BLE_ROLE_SLAVE,
    BLE_ROLE_HYBRID, /* reserved */
} ble_role_t;

#define APP_DEVICE_NAME_LENGTH_MAX      (18)
/// Default Device Name if no value can be found in NVDS
#define APP_DFLT_DEVICE_NAME            ("BK7231BT-01")
#define APP_DFLT_DEVICE_NAME_LEN        (sizeof(APP_DFLT_DEVICE_NAME))

extern char app_dflt_dev_name[APP_DEVICE_NAME_LENGTH_MAX];
extern uint32_t  ble_dut_flag;
extern ble_role_t ble_role_mode;
extern beken_queue_t ble_msg_que;
extern uint8_t ble_init_over;
extern uint8_t ble_deep_sleep;

extern UINT32 ble_ctrl( UINT32 cmd, void *param );
extern void ble_isr(void);
extern void ble_set_role_mode(ble_role_t role);
extern ble_role_t ble_get_role_mode();
extern void ble_send_msg(UINT32 msg);
extern void ble_intc_set(uint32_t enable);
extern void ble_clk_power_up(void);
extern void ble_clk_power_down(void);
extern void ble_switch_rf_to_wifi(void);
extern void ble_switch_rf_to_ble(void);
extern void ble_set_power_up(uint32 up);
extern UINT32 ble_ps_enabled(void );
extern UINT16 ble_ps_forbid_trace(BLE_PS_FORBID_STATUS forbid);

#endif /* _BLE_H_ */
