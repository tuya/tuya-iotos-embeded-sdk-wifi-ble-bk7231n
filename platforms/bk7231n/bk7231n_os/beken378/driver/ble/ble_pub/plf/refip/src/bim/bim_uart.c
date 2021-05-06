/**
 ****************************************************************************************
 *
 * @file bim_uart.c
 *
 * @brief UART driver
 *
 * Copyright (C) RivieraWaves 2009-2015
 *
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup UART
 * @{
 ****************************************************************************************
 */ 
/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include <stddef.h>     // standard definition
#include "timer.h"      // timer definition
#include "bim_uart.h"       // uart definition
#include "BK3435_reg.h"
//#include "reg_uart.h"   // uart register

 
/*
 * DEFINES
 *****************************************************************************************
 */

/// Max baudrate supported by this UART (in bps)
#define UART_BAUD_MAX      3500000
/// Min baudrate supported by this UART (in bps)
#define UART_BAUD_MIN      9600

/// Duration of 1 byte transfer over UART (10 bits) in us (for 921600 default baudrate)
#define UART_CHAR_DURATION        11

/*
 * ENUMERATION DEFINITIONS
 *****************************************************************************************
 */
 
 /*
 * common.c
 *
 *  Created on: 2013-5-7
 *      Author: pujie
 */

#include  <stdarg.h>
#include  <stdio.h>
#include "BK3435_reg.h"
#include "bim_uart.h"






#define Uart_Write_Byte(v)               (REG_APB3_UART_PORT=v)

#define UART_TX_FIFO_COUNT               (REG_APB3_UART_FIFO_STAT&0xff)
#define UART_RX_FIFO_COUNT               ((REG_APB3_UART_FIFO_STAT>>8)&0xff)
#define UART_TX_FIFO_FULL                (REG_APB3_UART_FIFO_STAT&0x00010000)
#define UART_TX_FIFO_EMPTY               (REG_APB3_UART_FIFO_STAT&0x00020000)
#define UART_RX_FIFO_FULL                (REG_APB3_UART_FIFO_STAT&0x00040000)
#define UART_RX_FIFO_EMPTY               (REG_APB3_UART_FIFO_STAT&0x00080000) 
#define UART_TX_WRITE_READY              (REG_APB3_UART_FIFO_STAT&0x00100000) 
#define UART_RX_READ_READY               (REG_APB3_UART_FIFO_STAT&0x00200000) 
#define bit_UART_TXFIFO_NEED_WRITE        0x01
#define bit_UART_RXFIFO_NEED_READ         0x02
#define bit_UART_RXFIFO_OVER_FLOW         0x04
#define bit_UART_RX_PARITY_ERROR          0x08
#define bit_UART_RX_STOP_ERROR            0x10
#define bit_UART_TX_PACKET_END            0x20
#define bit_UART_RX_PACKET_END            0x40
#define bit_UART_RXD_WAKEUP_DETECT        0x80




#define uart_tx_en    0x1      // 0: Disable TX, 1: Enable TX 
#define uart_rx_en    0x1      // 0: Disable RX, 1: Enable RX
#define irda_mode     0x0      // 0: UART  MODE, 1: IRDA MODE
#define data_len      0x3      // 0: 5 bits, 1: 6 bits, 2: 7 bits, 3: 8 bits
#define parity_en     0x1      // 0: NO Parity, 1: Enable Parity
#define parity_mode   0x1      // 0: Odd Check, 1: Even Check 
#define stop_bits     0x0      // 0: 1 stop-bit, 1: 2 stop-bit 
#define uart_clks     16000000 // UART's Main-Freq, 26M 
#define baud_rate     115200 // UART's Baud-Rate,  1M 




unsigned char uart_rx_buf[UART0_RX_FIFO_MAX_COUNT];
unsigned char uart_tx_buf[UART0_TX_FIFO_MAX_COUNT];
volatile bool uart_rx_done = 0;
volatile unsigned long uart_rx_index = 0;
uint8_t cur_read_buf_idx = 0;
uint8_t Read_Uart_Buf(void)
{
	return uart_rx_buf[cur_read_buf_idx++];
}	

uint8_t Uart_Read_Byte(void)
{
	return (REG_APB3_UART_PORT&0xff);
}	

#define UART_READ_BYTE()                 (REG_APB3_UART_PORT&0xff)


char uart_buff[128];
int uart_putchar(char * st)
{
	uint8_t num = 0;

    while (*st) 
    {
		if(UART_TX_WRITE_READY)
		{
			REG_APB3_UART_PORT = *st;
	    st++;
			num++;
	    }
	} 
	
    return num;
}

int uart_printf(const char *fmt,...)
{
	int n;	
	va_list ap;
    va_start(ap, fmt);
    n=vsprintf(uart_buff, fmt, ap);
    va_end(ap);
    uart_putchar(uart_buff);
	if(n > sizeof(uart_buff))
	{
		uart_putchar("buff full \r\n");
	}
		
    return n;
}
int uart_printf_null(const char *fmt,...)
{
	return 0;
}


char *hex2Str( uint8_t data)
{

  char        hex[] = "0123456789ABCDEF";
  static char str[3];
  char        *pStr = str;
 
  *pStr++ = hex[data >> 4];
  *pStr++ = hex[data & 0x0F];
  *pStr = 0;

  return str;
}

void uart_print_int(unsigned int num)
{
	
	uint8_t i;
	uint8_t m;

	uart_putchar((char *)"0x");
	for (i = 4;i > 0;i--)
	{
		m = ((num >> (8 * (i - 1)))& 0xff);
		uart_putchar(hex2Str(m));
	}
	uart_putchar("\r\n");
}



void cpu_delay( volatile unsigned int times)
{
    while(times--)
	{
		for(uint32_t i = 0;i < 1000;i++)
		;
	}		
}



///UART Character format
enum UART_CHARFORMAT
{
    UART_CHARFORMAT_8 = 0,
    UART_CHARFORMAT_7 = 1
};

///UART Stop bit
enum UART_STOPBITS
{
    UART_STOPBITS_1 = 0,
    UART_STOPBITS_2 = 1 /* Note: The number of stop bits is 1.5 if a character format
                                 with 5 bit is chosen*/
};

///UART Parity enable
enum UART_PARITY
{
    UART_PARITY_DISABLED = 0,
    UART_PARITY_ENABLED  = 1
};

///UART Parity type
enum UART_PARITYBIT
{
    UART_PARITYBIT_EVEN  = 0,
    UART_PARITYBIT_ODD   = 1,
    UART_PARITYBIT_SPACE = 2, // The parity bit is always 0.
    UART_PARITYBIT_MARK  = 3  // The parity bit is always 1.
};

///UART HW flow control
enum UART_HW_FLOW_CNTL
{
    UART_HW_FLOW_CNTL_DISABLED = 0,
    UART_HW_FLOW_CNTL_ENABLED = 1
};

///UART Input clock select
enum UART_INPUT_CLK_SEL
{
    UART_INPUT_CLK_SEL_0 = 0,
    UART_INPUT_CLK_SEL_1 = 1,
    UART_INPUT_CLK_SEL_2 = 2,
    UART_INPUT_CLK_SEL_3 = 3
};

///UART Interrupt enable/disable
enum UART_INT
{
    UART_INT_DISABLE = 0,
    UART_INT_ENABLE = 1
};

///UART Error detection
enum UART_ERROR_DETECT
{
    UART_ERROR_DETECT_DISABLED = 0,
    UART_ERROR_DETECT_ENABLED  = 1
};

/*
 * STRUCT DEFINITIONS
 *****************************************************************************************
 */
/* TX and RX channel class holding data used for asynchronous read and write data
 * transactions
 */
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
};

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */
/// uart environment structure
struct uart_env_tag uart_env;

/*
 * LOCAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */


/*
 * EXPORTED FUNCTION DEFINITIONS
 ****************************************************************************************
 */


void uart_init(uint32_t baudrate)
{
#if	UART_PRINTF_EN
    unsigned int baud_divisor ;
    REG_AHB0_ICU_UARTCLKCON   &= ~(0x1 << 0) ;  // Enable Uart's Clocks 
    baud_divisor = 0x8a;//uart_clks/baud_rate ;
    baud_divisor = baud_divisor-1 ;
    REG_APB3_UART_CFG  = (baud_divisor<<8) + 
                          (stop_bits   <<7) + 
                          //(parity_mode <<6) + 
                          //(parity_en   <<5) + 
                          (data_len    <<3) + 
                          (irda_mode   <<2) + 
                          (uart_rx_en  <<1) + 
                           uart_tx_en       ;
    REG_APB3_UART_FIFO_CFG  = 0x00002001 ;  // Set Fifo threshold 8
    REG_APB3_UART_INT_ENABLE = ((0x01 << 1) | (0x01 << 6) | (0x01 << 7));  //need read / stop end /rxd wake up
    REG_APB3_UART_FLOW_CFG  = 0x00000000 ;  // No Flow Control   
    REG_APB3_UART_WAKERNEL_CFG  =  ((0x01 << 0 )| (0x01 << 20) |  (0x01 << 21)| (0x01 << 22));  // No Wake Control
	
	REG_APB5_GPIOA_CFG  &= ~((0x3<<BIT_GPIO_PULL_UP)  + (0x3<<BIT_GPIO_PERI_EN)); 
	REG_APB5_GPIOA_CFG  |= ((0x3<<BIT_GPIO_PULL_UP)); 
    REG_APB5_GPIOA_CFG  |=   (0x3<<BIT_GPIO_OUT_EN_N);
		
    REG_APB5_GPIOA_DATA &= ~ (0x3<<BIT_GPIO_INPUT_EN);
	
	uart_env.rx.callback = NULL;
    uart_env.rx.dummy    = NULL;
#endif
}	



/// @} UART
