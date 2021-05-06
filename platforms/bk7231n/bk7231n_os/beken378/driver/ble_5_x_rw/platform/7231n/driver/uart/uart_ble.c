#include "includes.h"
#include "arm_arch.h"
#include <stddef.h>     // standard definition
#include <stdarg.h>
#include <stdint.h>        // standard integer definition
#include <string.h>        // string manipulation
#include <stdio.h>
#include "rwip.h"
#include "uart.h"
#include "uart_ble.h"
#include "cmd_evm.h"
#include "ble_pub.h"

volatile static struct uart_env_tag uart_env;
volatile static uint8_t  uart_rx_done = 0;
volatile static uint32_t uart_rx_index = 0;
uint8_t uart_rx_buf[UART_FIFO_MAX_COUNT];
extern uint8_t tx_pwr_idx;
uint8_t ble_test_mode = IDLE_MODE;
uint8_t tx_mode;

void ble_uart_init(void)
{
	uart_rx_done = 0;
    uart_rx_index = 0;

    //// Initialize RX and TX transfer callbacks
    uart_env.rx.callback = NULL;
    uart_env.tx.callback = NULL;
    uart_env.uart_tx_buf = NULL;
    uart_env.uart_rx_buf = NULL;
    uart_env.uart_tx_length = 0;
    uart_env.uart_rx_length = 0;
    uart_env.uart_tx_enable = 0;
    uart_env.uart_rx_enable = 0;
    
    if (ble_get_sys_mode() == DUT_FCC_MODE) {
        hci_data_init((HCI_DATA_TYPE_CMD | HCI_DATA_TYPE_EVENT));
        host_get_event_cbReg(ble_uart_send);
    }
}

void ble_uart_read(uint8_t *bufptr, uint32_t size, void (*callback) (void*, uint8_t), void* dummy)
{
    BLE_ASSERT_ERR(bufptr != NULL);
    BLE_ASSERT_ERR(size != 0);
    BLE_ASSERT_ERR(callback != NULL);
    uart_env.rx.callback = callback;
    uart_env.rx.dummy    = dummy;

    uart_env.uart_rx_buf = bufptr;	////	uart_rx_ptr_setf((uint32_t) bufptr);
    uart_env.uart_rx_length = size; ////	uart_rx_size_setf(size);
    uart_env.uart_rx_enable = 1;	////	uart_rx_start_setf(1);
}

void ble_uart_write(uint8_t *bufptr, uint32_t size, void (*callback) (void*, uint8_t), void* dummy)
{
	// Sanity check
    BLE_ASSERT_ERR(bufptr != NULL);
    BLE_ASSERT_ERR(size != 0);
    BLE_ASSERT_ERR(callback != NULL);
    uart_env.tx.callback = callback;
    uart_env.tx.dummy    = dummy;
        
    uart_env.uart_tx_buf = bufptr;
    uart_env.uart_tx_length = size;
    uart_env.uart_tx_enable = 1;

	ble_send_msg(BLE_MSG_DUT);
}

void ble_uart_flow_on(void)
{

}
bool ble_uart_flow_off(void)
{
    return true;
}

static void ble_uart_send_byte(uint8_t data)
{
	data = (data & UART_TX_FIFO_DIN_MASK) << UART_TX_FIFO_DIN_POSI;
	REG_WRITE(REG_UART1_FIFO_PORT, data);
}

void ble_uart_send(void *buff, uint16_t len)
{
    uint8_t *tmpbuf = (uint8_t *)buff;
    while (len--)
        ble_uart_send_byte(*tmpbuf++);
}

void uart_rx_cmd_handler(uint8_t *buff, uint8_t len)
{
    uint8_t length;
    length = len;
    switch (buff[0]) {
    	case EXIT_DUT_CMD:
			if ((length == 2) && (buff[1] == EXIT_DUT_ACT)) {
				ble_send_msg(BLE_DUT_EXIT);
			} else {
				bk_printf("unknow dut cmd\r\n");
			}
			break;
		case TX_PWR_SET_CMD:
			if (length == 2) {
				if (buff[1] > 79) {
					tx_pwr_idx = 79;
				} else {
					tx_pwr_idx = buff[1];
				}
				extern void ble_cal_set_txpwr(uint8_t idx);
				ble_cal_set_txpwr(tx_pwr_idx);
				bk_printf("idx:%d\r\n", tx_pwr_idx);

				if (ble_test_mode == USER_TX_MODE)
				{
					ble_ctrl(CMD_BLE_STOP_TX, NULL);
					ble_ctrl(CMD_BLE_START_TX, &tx_mode);
				}
			} else {
				bk_printf("unknow dut cmd\r\n");
			}
			break;
		case TX_PWR_SAVE_CMD:
			if (length == 3) {
				uint8_t channel = buff[1];
				uint8_t pwr_idx = buff[2];

				if (channel > 39)
					channel = 39;
				if (pwr_idx > 79)
					pwr_idx = 79;
				extern void manual_cal_save_txpwr(UINT32 rate, UINT32 channel, UINT32 pwr_gain);
				manual_cal_save_txpwr(EVM_DEFUALT_BLE_RATE, channel, pwr_idx);

			} else {
				bk_printf("unknow dut cmd\r\n");
			}
			break;
		case USER_SEND_CMD:
			if (length == 3) {
				uint8_t channel = buff[1];
				tx_mode = buff[2];
				if (channel > 39)
					channel = 39;

				uint32_t freq = (uint32_t)((channel + 1) * 2);

				ble_ctrl(CMD_BLE_SET_CHANNEL, &freq);
				ble_ctrl(CMD_BLE_START_TX, &tx_mode);

				ble_test_mode = USER_TX_MODE;
			} else {
				bk_printf("unknow dut cmd\r\n");
			}
			break;
		case USER_STOP_CMD:
			if (length == 2) {
				if (buff[1] == USER_SEND_CMD) {
					ble_ctrl(CMD_BLE_STOP_TX, NULL);
					ble_test_mode = IDLE_MODE;
				} else {
					bk_printf("unknow dut cmd");
				}
			} else {
				bk_printf("unknow dut cmd");
			}
			break;
		default:
			break;
    }
}

extern int bkreg_run_command(const char *content, int cnt);
void  ble_uart_isr(void)
{
	UINT32 status;
	UINT32 intr_en;
	UINT32 intr_status;

	intr_en = REG_READ(REG_UART1_INTR_ENABLE);
	intr_status = REG_READ(REG_UART1_INTR_STATUS);
	REG_WRITE(REG_UART1_INTR_STATUS, intr_status);
	status = intr_status & intr_en;

	if (status & (RX_FIFO_NEED_READ_STA | UART_RX_STOP_END_STA)) {
		while(REG_READ(REG_UART1_FIFO_STATUS) & FIFO_RD_READY)
		{
            uart_rx_buf[uart_rx_index] = ((REG_READ(REG_UART1_FIFO_PORT) >> UART_RX_FIFO_DOUT_POSI) & UART_RX_FIFO_DOUT_MASK);
            uart_rx_index++;
            if (uart_rx_index == UART_FIFO_MAX_COUNT) {
                uart_rx_index = 0;
            }
        }

		if ((uart_rx_buf[0] == 0x01) && (uart_rx_buf[1] == 0xe0) && (uart_rx_buf[2] == 0xfc)) {
			if (uart_rx_buf[3] == (uart_rx_index - 4)) {
				uart_rx_cmd_handler((uint8_t *)&uart_rx_buf[4], uart_rx_buf[3]);
				bkreg_run_command((char*)&uart_rx_buf[0], uart_rx_index);
			}
		}

		if ((uart_rx_buf[0] == 0x01) && ((uart_rx_buf[3] + 4) == uart_rx_index)
			&& (uart_rx_buf[1] != 0xe0) && (uart_rx_buf[2] != 0xfc)) {
			if (ble_test_mode == USER_TX_MODE) {
				bk_printf("user test is running\r\n");
			} else {
				host_send_cmd(uart_rx_buf, uart_rx_index);
			}
		}

		// work around:ble dut have own print show command success, wo should not add print
		if ((uart_rx_buf[0] == 0x01) && (uart_rx_buf[1] == 0x1e) && (uart_rx_buf[2] == 0x20)) {
#if (CFG_SUPPORT_MANUAL_CALI)
			extern uint8_t manual_cal_get_ble_pwr_idx(uint8_t channel);
			tx_pwr_idx = manual_cal_get_ble_pwr_idx(uart_rx_buf[4]);
#endif
			extern void ble_cal_set_txpwr(uint8_t idx);
			ble_cal_set_txpwr(tx_pwr_idx);
			bk_printf("c:%d\r\n", uart_rx_buf[4]);
		}

		uart_rx_index=0;
        uart_rx_done = 1;
	}
}

volatile struct hci_cmd_event_data host_cmd_data;
volatile struct hci_cmd_event_data host_event_data;

void hci_data_init(uint8_t type)
{	////type: 0x01-Clear host cmd data; 0x02-Clear host event data
    
    if(type & HCI_DATA_TYPE_CMD)
    {
        host_cmd_data.callback = NULL;
        memset((void *)&host_cmd_data.data_buf[0], 0, HCI_DATA_BUF_SIZE);
        host_cmd_data.data_len = 0;
    }
    if(type & HCI_DATA_TYPE_EVENT)
    {
    ////	host_event_data.callback = NULL;		////Will clear callback func
        memset((void *)&host_event_data.data_buf[0], 0, HCI_DATA_BUF_SIZE);
        host_event_data.data_len = 0;
    }
}

void host_send_cmd(uint8_t *bufptr, uint16_t length)
{
	//uint16_t tmpCnt = 0;
	BLE_ASSERT_ERR(length <= HCI_DATA_BUF_SIZE);
	host_cmd_data.callback = NULL;		////Test Only
	memcpy((void *)&host_cmd_data.data_buf[0], bufptr, length);
	host_cmd_data.data_len = length;
	ble_send_msg(BLE_MSG_DUT);
}

////void host_get_event(uint8 *bufptr, uint8 length)
void host_get_event(void)
{

	if(host_event_data.callback != NULL)
	{
		host_event_data.callback((void *)host_event_data.data_buf, host_event_data.data_len);
	}
	hci_data_init(HCI_DATA_TYPE_EVENT);
}

void host_get_event_cbReg(void (*callback) (void*, uint16_t))
{
	host_event_data.callback = callback;
}

void uart_h4tl_data_switch(void)
{   
    void (*callback) (void*, uint8_t) = NULL;
    void* data =NULL;
    uint16_t data_len = 0;

    while(uart_env.uart_tx_enable == 1)
    {
        //uart_printf("uart_h4tl_data_switch tx_enable\r\n");
        // Retrieve callback pointer
        callback = uart_env.tx.callback;
        data     = uart_env.tx.dummy;

        uart_env.uart_tx_enable = 0;
        memcpy((void *)&host_event_data.data_buf[data_len], uart_env.uart_tx_buf, uart_env.uart_tx_length);
        data_len += uart_env.uart_tx_length;
        host_event_data.data_len += uart_env.uart_tx_length;
        if(callback != NULL)
        {
            // Clear callback pointer
            uart_env.tx.callback = NULL;
            uart_env.tx.dummy    = NULL;

            // Call handler
            callback(data, RWIP_EIF_STATUS_OK);
        }
        else
        {
            BLE_ASSERT_ERR(0);
        }
    }
    
    if(host_event_data.data_len != 0)
    {	////New Event
        host_get_event();           
    }

    data_len = 0;

    if(host_cmd_data.data_len > 0)
    {
        while(uart_env.uart_rx_enable == 1)
        {
            // Retrieve callback pointer
            callback = uart_env.rx.callback;
            data     = uart_env.rx.dummy;
            uart_env.uart_rx_enable = 0;

            memcpy((void *)uart_env.uart_rx_buf, (void *)&host_cmd_data.data_buf[data_len], uart_env.uart_rx_length);

            data_len += uart_env.uart_rx_length;
            if(callback != NULL)
            {
                // Clear callback pointer
                uart_env.rx.callback = NULL;
                uart_env.rx.dummy    = NULL;

                // Call handler
                callback(data, RWIP_EIF_STATUS_OK);
            }
            else
            {
                BLE_ASSERT_ERR(0);
            }
            if(data_len >= host_cmd_data.data_len)
                break;
        }////while(uart_env.uart_rx_enable == 1)
     
      	////Clear HCI Cmd data
       hci_data_init(HCI_DATA_TYPE_CMD);        
    }
}

