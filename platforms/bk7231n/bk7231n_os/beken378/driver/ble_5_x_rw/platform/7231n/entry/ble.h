
#ifndef __BLE_H_
#define __BLE_H_

#define BLE_CONN_IDX_MAX             (0x01)
#define BLE_CHAR_DATA_LEN            (128)

#define BLE_MSG_QUEUE_COUNT          (20)
#define BLE_STACK_SIZE               (4096)

// ble dut uart port
#define PORT_UART1                   (1)
#define PORT_UART2                   (2)
#define BLE_DUT_UART_PORT            PORT_UART1  // PORT_UART2

enum
{
	BLE_MSG_POLL = 0,
	BLE_MSG_DUT,
	BLE_DUT_START,
	BLE_DUT_EXIT,
	BLE_MSG_SLEEP,
	BLE_MSG_NULL,
};

enum system_run_mode {
	NORMAL_MODE = 0,
	DUT_FCC_MODE = (0x01 << 0),
};

typedef struct ble_message {
    uint32_t data;
} BLE_MSG_T;

uint8_t ble_get_sys_mode(void);
void ble_send_msg(UINT32 data);
UINT32 ble_ctrl( UINT32 cmd, void *param );

#endif

