 /*============================================================================
 *                                                                            *
 * Copyright (C) by Tuya Inc                                                  *
 * All rights reserved                                                        *
 *                                                                            *
 *                                                                            *
 =============================================================================*/

/*============================ INCLUDES ======================================*/
#include "tuya_uart.h"
#include "tuya_os_adapter_error_code.h"
#include "drv_model_pub.h"
#include "uart_pub.h"
#include "BkDriverUart.h"

/*============================ MACROS ========================================*/
#define UART_DEV_NUM            2

/*============================ TYPES =========================================*/
typedef struct {
    tuya_uart_t         dev;  
    bk_uart_t           port;
    tuya_uart_isr_cb    isr_cb;
} uart_dev_t;

/*============================ PROTOTYPES ====================================*/
static int uart_dev_init        (tuya_uart_t *uart, tuya_uart_cfg_t *cfg);
static int uart_dev_write_byte  (tuya_uart_t *uart, uint8_t byte);
static int uart_dev_read_byte   (tuya_uart_t *uart, uint8_t *byte);
static int uart_dev_control     (tuya_uart_t *uart, uint8_t cmd, void *arg);
static int uart_dev_deinit      (tuya_uart_t *uart);
/*============================ LOCAL VARIABLES ===============================*/
static uart_dev_t s_uart_dev[UART_DEV_NUM];

static const tuya_uart_ops_t  uart_dev_ops = {
    .init       = uart_dev_init,
    .read_byte  = uart_dev_read_byte,
    .write_byte = uart_dev_write_byte,
    .control    = uart_dev_control,
    .deinit     = uart_dev_deinit,
};
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/
int platform_uart_init(void)
{
    s_uart_dev[TUYA_UART0].dev.ops = (tuya_uart_ops_t *)&uart_dev_ops;
    TUYA_UART_8N1_CFG(&s_uart_dev[TUYA_UART0].dev, TUYA_UART_BAUDRATE_9600, 256, 0);
    tuya_driver_register(&s_uart_dev[TUYA_UART0].dev.node, TUYA_DRV_UART, TUYA_UART0);

    s_uart_dev[TUYA_UART1].dev.ops = (tuya_uart_ops_t *)&uart_dev_ops;
    TUYA_UART_8N1_CFG(&s_uart_dev[TUYA_UART1].dev, TUYA_UART_BAUDRATE_9600, 256, 0);
    tuya_driver_register(&s_uart_dev[TUYA_UART1].dev.node, TUYA_DRV_UART, TUYA_UART1);
    
    return OPRT_OS_ADAPTER_OK;
}

void uart_dev_irq_handler(int uport, void *param)
{
    uart_dev_t *uart = (uart_dev_t *)param;

    uart->isr_cb(&uart->dev, TUYA_UART_INT_RX_EVENT);
}

/**
 * @brief 用于初始化串口
 * 
 * @param[in]  uart     串口句柄
 * @param[in]  cfg      串口配置结构体
 */
static int uart_dev_init(tuya_uart_t *uart, tuya_uart_cfg_t *cfg)
{
    uart_dev_t *uart_dev = (uart_dev_t *)uart;
    bk_uart_config_t    bkcfg;

    if (TUYA_UART0 == uart->node.port) {
        uart_dev->port = BK_UART_1;
    } else if (TUYA_UART1 == uart->node.port) {
        uart_dev->port = BK_UART_2;
    } else {
        return OPRT_OS_ADAPTER_INVALID_PARM;
    }
    //! data bits
    if (TUYA_UART_DATA_BIT5 == cfg->databits) {
        bkcfg.data_width = DATA_WIDTH_5BIT;
    } else if (TUYA_UART_DATA_BIT6 == cfg->databits) {
        bkcfg.data_width = DATA_WIDTH_6BIT;
    } else if (TUYA_UART_DATA_BIT7 ==  cfg->databits) {
        bkcfg.data_width = DATA_WIDTH_7BIT;
    } else if (TUYA_UART_DATA_BIT8 == cfg->databits) {
        bkcfg.data_width = DATA_WIDTH_8BIT;
    } else {
        return OPRT_OS_ADAPTER_INVALID_PARM;
    }
    //! stop bits
    if (TUYA_UART_STOP_BIT1 == cfg->stopbits) {
        bkcfg.stop_bits = BK_STOP_BITS_1;
    } else if (TUYA_UART_STOP_BIT2 == cfg->stopbits) {
        bkcfg.stop_bits = BK_STOP_BITS_2;
    } else {
        return OPRT_OS_ADAPTER_INVALID_PARM;
    }
    //!  parity bits
    if (TUYA_UART_PARITY_NONE == cfg->parity) {
        bkcfg.parity = BK_PARITY_NO;
    } else if (TUYA_UART_PARITY_EVEN == cfg->parity) {
        bkcfg.parity = BK_PARITY_EVEN;
    } else if (TUYA_UART_PARITY_ODD == cfg->parity) {
        bkcfg.parity = BK_PARITY_ODD;
    } else {
        return OPRT_OS_ADAPTER_INVALID_PARM;
    }
    //! baudrate
    bkcfg.baud_rate    = cfg->baudrate;
    //! flow control
    bkcfg.flow_control = 0;
    bkcfg.flags        = 0;

    bk_uart_initialize(uart_dev->port, &bkcfg, NULL);

    return OPRT_OS_ADAPTER_OK;
}

extern OSStatus bk_uart_diable_rx(bk_uart_t uart);

static int uart_dev_control(tuya_uart_t *uart, uint8_t cmd, void *arg)
{
    int result = OPRT_OK;

    uart_dev_t *uart_dev = (uart_dev_t *)uart;

    switch (cmd) {
    case TUYA_DRV_SET_INT_CMD:
        if ((uint32_t)arg == TUYA_DRV_INT_RX_FLAG) {
            //! TODO:
        } else if ((uint32_t)arg == TUYA_DRV_INT_TX_FLAG) {
            //! TODO:
        } else {
            result = OPRT_INVALID_PARM; 
        }
        break;
    case TUYA_DRV_CLR_INT_CMD:
        if ((uint32_t)arg == TUYA_DRV_INT_RX_FLAG) {
            bk_uart_diable_rx(uart_dev->port); 
            
        } else if ((uint32_t)arg == TUYA_DRV_INT_TX_FLAG) {
            //! TODO:
        } else {
            result = OPRT_INVALID_PARM;
        }
        break;

    case TUYA_DRV_SET_ISR_CMD:
        uart_dev->isr_cb = (tuya_uart_isr_cb)arg;
        bk_uart_set_rx_callback(uart_dev->port, uart_dev_irq_handler, uart);
        break;

    case TUYA_DRV_ISR_MODE_CMD: {
        uint8_t *isr_mode = (uint8_t *)arg;
        *isr_mode = TUYA_UART_RX_ISR_FIFO_MODE;
    }

    }

    return result;
}
extern void bk_send_byte(UINT8 uport, UINT8 data);
/**
 * @brief 用于发送一个字节
 * 
 * @param[in]  uart     串口句柄
 * @param[in]  byte     要发送的字节
 */
static int uart_dev_write_byte(tuya_uart_t *uart, uint8_t byte)
{
    uart_dev_t *uart_dev = (uart_dev_t *)uart;

    bk_send_byte(uart_dev->port, byte);

    return OPRT_OS_ADAPTER_OK;
}

/**
 * @brief 用于读取一个字节
 * 
 * @param[in]   uart     串口句柄
 * @param[out]  ch       要读取的字节指针
 */
static int uart_dev_read_byte(tuya_uart_t *uart, uint8_t *byte)
{
    int result = OPRT_OS_ADAPTER_COM_ERROR;

    uart_dev_t *uart_dev = (uart_dev_t *)uart;

    result = uart_read_byte(uart_dev->port);
    if (-1 != result) {
        *byte  = (uint8_t) result;
        result = OPRT_OS_ADAPTER_OK;
    }

    return result;
}

/**
 * @brief 用于释放串口
 * 
 * @param[in]  port     串口句柄
 */
static int uart_dev_deinit(tuya_uart_t *uart)
{
    uart_dev_t *uart_dev = (uart_dev_t *)uart;
    
    bk_uart_finalize(uart_dev->port);

    return OPRT_OS_ADAPTER_OK;
}

