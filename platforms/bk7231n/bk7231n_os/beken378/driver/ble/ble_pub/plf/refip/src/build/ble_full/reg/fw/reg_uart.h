#ifndef _REG_UART_H_
#define _REG_UART_H_

#include <stdint.h>
#include "_reg_uart.h"
#include "ble_compiler.h"
#include "architect.h"
#include "ble_reg_access.h"
#include "ble.h"

#define REG_UART_COUNT 9

#define REG_UART_DECODING_MASK 0x0000003F

/**
 * @brief UART conf register definition
 * <pre>
 *   Bits           Field Name   Reset Value  
 *  -----   ------------------   -----------
 *	20:8					UART_CLK_DIVID	0        UART_CLK_DIVID=(UART_CLK/BAND - 1)
 *  		7         UART_STOP_LEN   0				0: 1bit 1:2bits
 *  		6         UART_PAR_MODE   0				0x0:Å¼ 0x1£ºÆæ
 *  		5         UART_PAR_EN   	0
 *  04:03         UART_LEN    		0  0:5bit 1:6bit 2:7bit 3:8bit
 *     02         UART_IRDA  			 0
 *     01        	UART_RX_ENABLE   0
 *     00         UART_TX_ENABLE   0					
 * </pre>
 */
#if (BLE_DUT_UART_PORT == PORT_UART2)
#define UART_CONF_ADDR   0x00802200
#else
#define UART_CONF_ADDR   0x00802100
#endif // (BLE_DUT_UART_PORT == PORT_UART2)

#define UART_CONF_OFFSET 0x00000000
#define UART_CONF_INDEX  0x00000000
#define UART_CONF_RESET  0x00000000

__INLINE uint32_t uart_conf_get(void)
{
    return REG_PL_RD(UART_CONF_ADDR);
}

__INLINE void uart_conf_set(uint32_t value)
{
    REG_PL_WR(UART_CONF_ADDR, value);
}



__INLINE void uart_conf_pack(uint8_t txen, uint8_t rxen, uint8_t irda,
				uint8_t bitlen, uint8_t paren, uint8_t parmode,uint8_t stoplen,uint16_t clkdiv)
{
   
    ASSERT_ERR((((uint32_t)clkdiv << 8) & ~((uint32_t)0x001FFF00)) == 0);
	ASSERT_ERR((((uint32_t)stoplen << 7) & ~((uint32_t)0x00000080)) == 0);
    ASSERT_ERR((((uint32_t)parmode << 6) & ~((uint32_t)0x00000040)) == 0);
    ASSERT_ERR((((uint32_t)paren << 5) & ~((uint32_t)0x00000020)) == 0);
	
    ASSERT_ERR((((uint32_t)bitlen << 3) & ~((uint32_t)0x00000018)) == 0);
    ASSERT_ERR((((uint32_t)irda << 2) & ~((uint32_t)0x00000004)) == 0);
    ASSERT_ERR((((uint32_t)rxen << 1) & ~((uint32_t)0x00000002)) == 0);
    ASSERT_ERR((((uint32_t)txen << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(UART_CONF_ADDR,  	 ((uint32_t)clkdiv << 8) | ((uint32_t)stoplen << 7) | ((uint32_t)parmode << 6) |
																((uint32_t)paren << 5)   | ((uint32_t)bitlen << 3) | ((uint32_t)irda << 2) | 
																((uint32_t)rxen << 1) |  ((uint32_t)txen << 0));
}

#if (BLE_DUT_UART_PORT == PORT_UART2)
#define UART_FIFO_CONF_ADDR				0x00802204
#else
#define UART_FIFO_CONF_ADDR				0x00802104
#endif // (BLE_DUT_UART_PORT == PORT_UART2)


__INLINE void uart_fifo_conf_pack(uint8_t tx_threshold, uint8_t rx_threshold, uint8_t rx_detect_time)
{
		ASSERT_ERR((((uint32_t)rx_detect_time << 9) & ~((uint32_t)0x00010000)) == 0);
		ASSERT_ERR((((uint32_t)rx_threshold << 8) & ~((uint32_t)0x0000FF00)) == 0);
		ASSERT_ERR((((uint32_t)tx_threshold << 0) & ~((uint32_t)0x00000000)) == 0);
	
	  REG_PL_WR(UART_FIFO_CONF_ADDR, ((uint32_t)rx_detect_time << 9) | ((uint32_t)rx_threshold << 8) | ((uint32_t)tx_threshold << 0));
		
}

__INLINE void uart_tx_fifo_threshold_setf(uint8_t threshold)
{
    ASSERT_ERR((((uint32_t)threshold << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(UART_FIFO_CONF_ADDR, (REG_PL_RD(UART_FIFO_CONF_ADDR) & ~((uint32_t)0x000000FF)) | ((uint32_t)threshold << 0));
}

__INLINE void uart_rx_fifo_threshold_setf(uint8_t threshold)
{
    ASSERT_ERR((((uint32_t)threshold << 8) & ~((uint32_t)0x0000FF00)) == 0);
    REG_PL_WR(UART_FIFO_CONF_ADDR, (REG_PL_RD(UART_FIFO_CONF_ADDR) & ~((uint32_t)0x0000FF00)) | ((uint32_t)threshold << 8));
}

__INLINE void uart_rx_stop_edtect_time_setf(uint8_t time)
{
    ASSERT_ERR((((uint32_t)time << 9) & ~((uint32_t)0x00010000)) == 0);
    REG_PL_WR(UART_FIFO_CONF_ADDR, (REG_PL_RD(UART_FIFO_CONF_ADDR) & ~((uint32_t)0x00010000)) | ((uint32_t)time << 8));
}






#if (BLE_DUT_UART_PORT == PORT_UART2)
#define UART_FLOW_CONF_ADDR				0x00802218
#else
#define UART_FLOW_CONF_ADDR				0x00802118
#endif // (BLE_DUT_UART_PORT == PORT_UART2)

__INLINE void uart_flow_en_setf(uint8_t forcerts)
{
    ASSERT_ERR((((uint32_t)forcerts << 16) & ~((uint32_t)0x00010000)) == 0);
    REG_PL_WR(UART_FLOW_CONF_ADDR, (REG_PL_RD(UART_FLOW_CONF_ADDR) & ~((uint32_t)0x00010000)) | ((uint32_t)forcerts << 16));
}

__INLINE void uart_flow_cnt_low_setf(uint8_t cnt)
{
    ASSERT_ERR((((uint32_t)cnt << 0) & ~((uint32_t)0x000000FF)) == 0);
    REG_PL_WR(UART_FLOW_CONF_ADDR, (REG_PL_RD(UART_FLOW_CONF_ADDR) & ~((uint32_t)0x000000FF)) | ((uint32_t)cnt << 0));
}

__INLINE void uart_flow_cnt_high_setf(uint8_t cnt)
{
    ASSERT_ERR((((uint32_t)cnt << 8) & ~((uint32_t)0x0000FF00)) == 0);
    REG_PL_WR(UART_FLOW_CONF_ADDR, (REG_PL_RD(UART_FLOW_CONF_ADDR) & ~((uint32_t)0x0000FF00)) | ((uint32_t)cnt << 8));
}


__INLINE void uart_flow_rts_polarity_setf(uint8_t rts)
{
    ASSERT_ERR((((uint32_t)rts << 17) & ~((uint32_t)0x00010000)) == 0);
    REG_PL_WR(UART_FLOW_CONF_ADDR, (REG_PL_RD(UART_FLOW_CONF_ADDR) & ~((uint32_t)0x00010000)) | ((uint32_t)rts << 17));
}

__INLINE void uart_flow_cts_polarity_setf(uint8_t cts)
{
    ASSERT_ERR((((uint32_t)cts << 18) & ~((uint32_t)0x00020000)) == 0);
    REG_PL_WR(UART_FLOW_CONF_ADDR, (REG_PL_RD(UART_FLOW_CONF_ADDR) & ~((uint32_t)0x00020000)) | ((uint32_t)cts << 18));
}



/**
 * @brief FIFO_STATUS register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     21       FIFO_RD_READY   0
 *     20       FIFO_WR_READY   0
 *     19       RX_FIFO_EMPTY   0
 *     18       RX_FIFO_FULL   0
 *     17       TX_FIFO_EMPTY   0
 *     16       TX_FIFO_FULL  0
 *     8-15     RX_FIFO_COUNT   0
 *     0-7      TX_FIFO_COUNT   0
 * </pre>
 */
#if (BLE_DUT_UART_PORT == PORT_UART2)
#define UART_FIFO_STATUS_ADDR   0x00802208
#else
#define UART_FIFO_STATUS_ADDR   0x00802108
#endif // (BLE_DUT_UART_PORT == PORT_UART2)

__INLINE uint8_t uart_tx_fifo_count_getf(void)
{
		uint32_t localVal = REG_PL_RD(UART_FIFO_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x000000FF)) >> 0);
}

__INLINE uint8_t uart_rx_fifo_count_getf(void)
{
		uint32_t localVal = REG_PL_RD(UART_FIFO_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x0000FF00)) >> 8);
}


__INLINE uint8_t uart_tx_fifo_full_getf(void)
{
    uint32_t localVal = REG_PL_RD(UART_FIFO_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}
__INLINE uint8_t uart_tx_fifo_empty_getf(void)
{
    uint32_t localVal = REG_PL_RD(UART_FIFO_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

__INLINE uint8_t uart_rx_fifo_full_getf(void)
{
    uint32_t localVal = REG_PL_RD(UART_FIFO_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x00040000)) >> 18);
}

__INLINE uint8_t uart_rx_fifo_empty_getf(void)
{
    uint32_t localVal = REG_PL_RD(UART_FIFO_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x00080000)) >> 19);
}

__INLINE uint8_t uart_tx_fifo_wr_ready_getf(void)
{
    uint32_t localVal = REG_PL_RD(UART_FIFO_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

__INLINE uint8_t uart_rx_fifo_rd_ready_getf(void)
{
    uint32_t localVal = REG_PL_RD(UART_FIFO_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}


__INLINE uint8_t uart_ext_wakeup_getf()
{
	return 0 ;
}


/**
 * @brief CLK register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  31:00                 FREQ   0x0
 * </pre>
 */


__INLINE uint32_t uart_clk_get(void)
{
    return 16000000;
}


/**
 * @brief INT_STATUS register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     07    UART_RXD_WAKEUP   0
 *     06    UART_RX_STOP_END   0
 *     05    UART_TX_STOP_END  0
 *     04    UART_RX_STOP_ERROR   0
 *     03    UART_RX_PARITY_ERROR   0
 *     02    RX_FIFO_OVER_FLOW          0
 *     01    RX_FIFO_NEED_READ   0
 *     00    TX_FIFO_NEED_WRITE   0
 * </pre>
 */
#if (BLE_DUT_UART_PORT == PORT_UART2)
#define UART_ISR_STAT_ADDR   0x00802214
#else
#define UART_ISR_STAT_ADDR   0x00802114
#endif // (BLE_DUT_UART_PORT == PORT_UART2)


__INLINE uint32_t uart_isr_stat_get(void)
{
    return REG_PL_RD(UART_ISR_STAT_ADDR);
}

__INLINE void uart_isr_stat_set( uint8_t stat)
{ 
	 ASSERT_ERR((((uint32_t)stat << 0) & ~((uint32_t)0x000000FF)) == 0);
   REG_PL_WR(UART_ISR_STAT_ADDR, (REG_PL_RD(UART_ISR_STAT_ADDR) & ~((uint32_t)0x000000FF)) | ((uint32_t)stat << 0));
	//(*(volatile uint32_t *)(UART_ISR_STAT_ADDR)) = stat;
}

__INLINE uint8_t uart_tx_fifo_need_wr_isr_getf(void)
{
    uint32_t localVal = REG_PL_RD(UART_ISR_STAT_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

__INLINE uint8_t uart_rx_fifo_need_rd_isr_getf(void)
{
    uint32_t localVal = REG_PL_RD(UART_ISR_STAT_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}


__INLINE uint8_t uart_rx_fifo_over_isr_getf(void)
{
    uint32_t localVal = REG_PL_RD(UART_ISR_STAT_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

__INLINE void uart_rx_fifo_over_isr_clr_setf(uint8_t rx_fifo_over)
{
   ASSERT_ERR((((uint32_t)rx_fifo_over << 2) & ~((uint32_t)0x00000004)) == 0);
   REG_PL_WR(UART_ISR_STAT_ADDR, (REG_PL_RD(UART_ISR_STAT_ADDR) & ~((uint32_t)0x00000004)) | ((uint32_t)rx_fifo_over << 2));

}


__INLINE uint8_t uart_rx_parity_err_isr_getf(void)
{
    uint32_t localVal = REG_PL_RD(UART_ISR_STAT_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

__INLINE void uart_rx_parity_err_isr_clr_setf(uint8_t rx_parity_err)
{
   ASSERT_ERR((((uint32_t)rx_parity_err << 3) & ~((uint32_t)0x00000008)) == 0);
   REG_PL_WR(UART_ISR_STAT_ADDR, (REG_PL_RD(UART_ISR_STAT_ADDR) & ~((uint32_t)0x00000008)) | ((uint32_t)rx_parity_err << 3));
}

__INLINE uint8_t uart_rx_stopbit_err_isr_getf(void)
{
    uint32_t localVal = REG_PL_RD(UART_ISR_STAT_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

__INLINE void uart_rx_stopbit_err_isr_clr_setf(uint8_t rx_stopbit_err)
{
   ASSERT_ERR((((uint32_t)rx_stopbit_err << 4) & ~((uint32_t)0x00000010)) == 0);
   REG_PL_WR(UART_ISR_STAT_ADDR, (REG_PL_RD(UART_ISR_STAT_ADDR) & ~((uint32_t)0x00000010)) | ((uint32_t)rx_stopbit_err << 4));
}


__INLINE uint8_t uart_tx_end_isr_getf(void)
{
    uint32_t localVal = REG_PL_RD(UART_ISR_STAT_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

__INLINE void uart_tx_end_isr_clr_setf(uint8_t tx_end)
{
   ASSERT_ERR((((uint32_t)tx_end << 5) & ~((uint32_t)0x00000020)) == 0);
   REG_PL_WR(UART_ISR_STAT_ADDR, (REG_PL_RD(UART_ISR_STAT_ADDR) & ~((uint32_t)0x00000020)) | ((uint32_t)tx_end << 5));
}

__INLINE uint8_t uart_rx_end_isr_getf(void)
{
    uint32_t localVal = REG_PL_RD(UART_ISR_STAT_ADDR);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}
__INLINE void uart_rx_end_isr_clr_setf(uint8_t rx_end)
{
   ASSERT_ERR((((uint32_t)rx_end << 6) & ~((uint32_t)0x00000040)) == 0);
   REG_PL_WR(UART_ISR_STAT_ADDR, (REG_PL_RD(UART_ISR_STAT_ADDR) & ~((uint32_t)0x00000040)) | ((uint32_t)rx_end << 6));
}

__INLINE uint8_t uart_rxd_wakeup_isr_getf(void)
{
    uint32_t localVal = REG_PL_RD(UART_ISR_STAT_ADDR);
    return ((localVal & ((uint32_t)0x00000080)) >> 7);
}

__INLINE void uart_rxd_wakeup_isr_clr_setf(uint8_t rxd_wakeup)
{
   ASSERT_ERR((((uint32_t)rxd_wakeup << 7) & ~((uint32_t)0x00000080)) == 0);
   REG_PL_WR(UART_ISR_STAT_ADDR, (REG_PL_RD(UART_ISR_STAT_ADDR) & ~((uint32_t)0x00000080)) | ((uint32_t)rxd_wakeup << 7));
}


/**
 * @brief ISR_EN register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     
 * </pre>
 */
#if (BLE_DUT_UART_PORT == PORT_UART2)
#define UART_ISR_EN_ADDR   0x00802210
#else
#define UART_ISR_EN_ADDR   0x00802110
#endif // (BLE_DUT_UART_PORT == PORT_UART2)


__INLINE void uart_isr_en_set(uint32_t value)
{
    REG_PL_WR(UART_ISR_EN_ADDR, value);
}



__INLINE void uart_isr_en_pack(uint8_t rx_need_rd, uint8_t tx_need_wr, uint8_t rx_over,
							uint8_t rx_parity_err, uint8_t rx_stopbit_err, uint8_t tx_end, uint8_t rx_end,
							uint8_t rxd_wakeup)
{
	ASSERT_ERR((((uint32_t)rxd_wakeup << 7) & ~((uint32_t)0x00000080)) == 0);
    
    ASSERT_ERR((((uint32_t)rx_end << 6) & ~((uint32_t)0x00000040)) == 0);
    ASSERT_ERR((((uint32_t)tx_end << 5) & ~((uint32_t)0x00000020)) == 0);
    ASSERT_ERR((((uint32_t)rx_stopbit_err << 4) & ~((uint32_t)0x00000010)) == 0);
	ASSERT_ERR((((uint32_t)rx_parity_err << 3) & ~((uint32_t)0x00000008)) == 0);
	ASSERT_ERR((((uint32_t)rx_over << 2) & ~((uint32_t)0x00000004)) == 0);
    ASSERT_ERR((((uint32_t)tx_need_wr << 1) & ~((uint32_t)0x00000002)) == 0);
    ASSERT_ERR((((uint32_t)rx_need_rd << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(UART_ISR_EN_ADDR,  ((uint32_t)rxd_wakeup << 7) | ((uint32_t)rx_end << 6) |
							((uint32_t)tx_end << 5) | ((uint32_t)rx_stopbit_err << 4) | ((uint32_t)rx_parity_err << 3) |
							((uint32_t)rx_over << 2) |((uint32_t)tx_need_wr << 1) |((uint32_t)rx_need_rd << 0));
}


__INLINE uint8_t uart_rx_wakeup_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(UART_ISR_EN_ADDR);
    return ((localVal & ((uint32_t)0x00000080)) >> 7);
}

__INLINE void uart_rx_wakeup_en_setf(uint8_t wakernel_up)
{
    ASSERT_ERR((((uint32_t)wakernel_up << 7) & ~((uint32_t)0x00000080)) == 0);
    REG_PL_WR(UART_ISR_EN_ADDR, (REG_PL_RD(UART_ISR_EN_ADDR) & ~((uint32_t)0x00000080)) | ((uint32_t)wakernel_up << 7));
}


__INLINE uint8_t uart_rx_end_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(UART_ISR_EN_ADDR);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

__INLINE void uart_rx_end_en_setf(uint8_t rx_end)
{
    ASSERT_ERR((((uint32_t)rx_end << 6) & ~((uint32_t)0x00000040)) == 0);
    REG_PL_WR(UART_ISR_EN_ADDR, (REG_PL_RD(UART_ISR_EN_ADDR) & ~((uint32_t)0x00000040)) | ((uint32_t)rx_end << 6));
}

__INLINE uint8_t uart_tx_end_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(UART_ISR_EN_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

__INLINE void uart_tx_end_en_setf(uint8_t tx_end)
{
    ASSERT_ERR((((uint32_t)tx_end << 5) & ~((uint32_t)0x00000020)) == 0);
    REG_PL_WR(UART_ISR_EN_ADDR, (REG_PL_RD(UART_ISR_EN_ADDR) & ~((uint32_t)0x00000020)) | ((uint32_t)tx_end << 5));
}


__INLINE uint8_t uart_rx_stopbit_err_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(UART_ISR_EN_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

__INLINE void uart_rx_stopbit_err_en_setf(uint8_t rx_stopbit_err)
{
    ASSERT_ERR((((uint32_t)rx_stopbit_err << 4) & ~((uint32_t)0x00000010)) == 0);
    REG_PL_WR(UART_ISR_EN_ADDR, (REG_PL_RD(UART_ISR_EN_ADDR) & ~((uint32_t)0x00000010)) | ((uint32_t)rx_stopbit_err << 4));
}


__INLINE uint8_t uart_rx_parity_err_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(UART_ISR_EN_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

__INLINE void uart_rx_parity_err_en_setf(uint8_t rx_parity_err)
{
    ASSERT_ERR((((uint32_t)rx_parity_err << 3) & ~((uint32_t)0x00000008)) == 0);
    REG_PL_WR(UART_ISR_EN_ADDR, (REG_PL_RD(UART_ISR_EN_ADDR) & ~((uint32_t)0x00000008)) | ((uint32_t)rx_parity_err << 3));
}


__INLINE uint8_t uart_rx_over_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(UART_ISR_EN_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

__INLINE void uart_rx_over_en_setf(uint8_t rx_over)
{
    ASSERT_ERR((((uint32_t)rx_over << 2) & ~((uint32_t)0x00000004)) == 0);
    REG_PL_WR(UART_ISR_EN_ADDR, (REG_PL_RD(UART_ISR_EN_ADDR) & ~((uint32_t)0x00000004)) | ((uint32_t)rx_over << 2));
}


__INLINE uint8_t uart_tx_need_wr_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(UART_ISR_EN_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

__INLINE void uart_tx_need_wr_en_setf(uint8_t tx_need_wr)
{
    ASSERT_ERR((((uint32_t)tx_need_wr << 1) & ~((uint32_t)0x00000002)) == 0);
    REG_PL_WR(UART_ISR_EN_ADDR, (REG_PL_RD(UART_ISR_EN_ADDR) & ~((uint32_t)0x00000002)) | ((uint32_t)tx_need_wr << 1));
}


__INLINE uint8_t uart_rx_need_rd_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(UART_ISR_EN_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

__INLINE void uart_rx_need_rd_en_setf(uint8_t rx_need_rd)
{
    ASSERT_ERR((((uint32_t)rx_need_rd << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(UART_ISR_EN_ADDR, (REG_PL_RD(UART_ISR_EN_ADDR) & ~((uint32_t)0x00000001)) | ((uint32_t)rx_need_rd << 0));
}



#endif // _REG_UART_H_

