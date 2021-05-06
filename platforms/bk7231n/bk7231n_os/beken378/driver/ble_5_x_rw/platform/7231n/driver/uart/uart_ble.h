
#ifndef __UART_BLE_H__
#define __UART_BLE_H__

#define HCI_DATA_BUF_SIZE		512
#define UART_FIFO_MAX_COUNT		128
#define HCI_DATA_TYPE_CMD		0x01
#define HCI_DATA_TYPE_EVENT		0x02

enum
{
    UART_CMD_STATE_HEAD,
    UART_CMD_STATE_OPCODE_ONE,
    UART_CMD_STATE_OPCODE_TWO,
    UART_CMD_STATE_LENGTH,
    UART_CMD_STATE_CMD,
};

enum
{
    EXIT_DUT_CMD			= 0x0E,
	EXIT_DUT_ACT			= 0xA0,
	TX_PWR_SET_CMD			= 0x10,
	TX_PWR_SAVE_CMD			= 0x11,
	XTAL_SET_CMD			= 0x12,
	USER_SEND_CMD			= 0x21,
	USER_STOP_CMD			= 0x2F,
};

enum
{
	IDLE_MODE			= 0,
	USER_TX_MODE		= 1,
	DUT_MODE			= 2,
};

/// UART TX RX Channel
struct uart_txrxchannel
{
    /// call back function pointer
    void (*callback) (void*, uint8_t);
    /// Dummy data pointer returned to callback when operation is over.
    void* dummy;
};

/// UART environment structure
struct uart_env_tag
{
    /// tx channel
    struct uart_txrxchannel tx;
    /// rx channel
    struct uart_txrxchannel rx;
    /// error detect
    uint8_t errordetect;
    /// external wakeup
    bool ext_wakeup;
	//// Modified
	uint8_t *uart_tx_buf;
	uint8_t *uart_rx_buf;
	uint32_t uart_tx_length;
	uint32_t uart_rx_length;
	uint8_t uart_tx_enable;		////Maybe no need
	uint8_t uart_rx_enable;		////Maybe no need
	uint8_t uart_download_state;
};

struct hci_cmd_event_data
{
    /// call back function pointer
    void (*callback) (void*, uint16_t);
    /// Dummy data pointer returned to callback when operation is over.
    uint8_t data_buf[HCI_DATA_BUF_SIZE];
	uint32_t data_len;
};

void ble_uart_init(void);
void ble_uart_read(uint8_t *bufptr, uint32_t size, void (*callback) (void*, uint8_t), void* dummy);
void ble_uart_write(uint8_t *bufptr, uint32_t size, void (*callback) (void*, uint8_t), void* dummy);
void host_get_event_cbReg(void (*callback) (void*, uint16_t));
void uart_h4tl_data_switch(void);
void host_get_event(void);
void host_send_cmd(uint8_t *bufptr, uint16_t length);
void hci_data_init(uint8_t type);
void ble_uart_send(void *buff, uint16_t len);
void ble_uart_flow_on(void);
bool ble_uart_flow_off(void);
void  ble_uart_isr(void);

#endif