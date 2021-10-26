#include "include.h"
#include "arm_arch.h"
#include "typedef.h"
#include "arm_arch.h"
#include "sys_config.h"

#if(CFG_SOC_NAME == SOC_BK7231N)
#include "icu_pub.h"
#include "spi_pub.h"
#include "sys_ctrl_pub.h"
#include "drv_model_pub.h"
#include "mem_pub.h"
#include "error.h"
#include "rtos_pub.h"
#include "general_dma_pub.h"
#include "general_dma.h"
#include "spi_bk7231n.h"

#if CFG_USE_SPI_MASTER
struct bk_spi_dev {
	UINT8 init_spi:1;
    UINT8 init_dma_tx:1;
    UINT8 init_dma_rx:1;
    UINT8 undef:5;

	UINT8 *tx_ptr;
    UINT8 *rx_ptr;
	UINT32 tx_len;
    UINT32 rx_len; 
	beken_semaphore_t   tx_sem;
	beken_semaphore_t   rx_sem;
    beken_semaphore_t   dma_rx_sem;
    beken_semaphore_t   dma_tx_sem;
    beken_mutex_t       mutex;
	UINT32      rx_offset;
	UINT32      rx_drop;
	UINT32      total_len;
volatile	UINT32      flag;
    

	
};

static struct bk_spi_dev *spi_dev;

static void bk_spi_rx_callback(int is_rx_end, void *param)
{
	UINT8 ch, *rxbuf;
	UINT32 offset, drop;

	GLOBAL_INT_DECLARATION();

	rxbuf = spi_dev->rx_ptr;
	drop = spi_dev->rx_drop;
	offset = spi_dev->rx_offset;

	while (1) {
		if (spi_read_rxfifo(&ch) == 0)
			break;

		if (rxbuf) {
			if (drop != 0)
				drop--;
			else {
				if (offset < spi_dev->rx_len) {
					rxbuf[offset] = ch;
					offset++;
				} else
					BK_SPI_WPRT("rx over flow:%02x, %d\r\n", ch, spi_dev->rx_len);
			}
		}
	}

	GLOBAL_INT_DISABLE();
	spi_dev->rx_drop = drop;
	spi_dev->rx_offset = offset;
	GLOBAL_INT_RESTORE();
}

static void bk_spi_tx_needwrite_callback(int port, void *param)
{
	UINT8 *tx_ptr = spi_dev->tx_ptr, data;
	UINT32 tx_len = spi_dev->tx_len, total_len = spi_dev->total_len, tx_ok = 0;

	UINT8 *rxbuf;
	UINT32 offset, drop;

	rxbuf = spi_dev->rx_ptr;
	drop = spi_dev->rx_drop;
	offset = spi_dev->rx_offset;

	GLOBAL_INT_DECLARATION();

	while (total_len) {
		tx_ok = 0;

		if (tx_len) {
			data = *tx_ptr;
			if (spi_write_txfifo(data) == 1) {
				tx_ok = 1;

				tx_len --;
				tx_ptr ++;
			}
		} else {
			data = 0xff;
			if (spi_write_txfifo(data) == 1)
				tx_ok = 1;
		}

		/* check rx data to prevent rx over flow */
		if (spi_read_rxfifo(&data) == 1) {
			if (rxbuf) {
				if (drop != 0)
					drop--;
				else {
					if (offset < spi_dev->rx_len) {
						rxbuf[offset] = data;
						offset++;
					} else
						BK_SPI_WPRT("0 rx over flow:%02x, %d\r\n", data, spi_dev->rx_len);
				}
			}
		}

		if (tx_ok == 1) {
			total_len --;
			if (total_len == 0) {
				UINT32 enable = 0;
				sddev_control(SPI_DEV_NAME, CMD_SPI_TXINT_EN, (void *)&enable);

				//BK_SPI_PRT("tx fin\r\n");
				break;
			}
		} else
			break;
	}

	GLOBAL_INT_DISABLE();
	spi_dev->tx_ptr = tx_ptr;
	spi_dev->tx_len = tx_len;
	spi_dev->total_len = total_len;

	spi_dev->rx_drop = drop;
	spi_dev->rx_offset = offset;
	GLOBAL_INT_RESTORE();

}

static void bk_spi_tx_finish_callback(int port, void *param)
{
	if ((spi_dev->total_len == 0) && ((spi_dev->flag & TX_FINISH_FLAG) == 0)) {
		spi_dev->flag |= TX_FINISH_FLAG;
		rtos_set_semaphore(&spi_dev->tx_sem);
		//BK_SPI_PRT("tx end\r\n");
	}
}

static void bk_spi_configure(UINT32 rate, UINT32 mode)
{
	UINT32 param;
	struct spi_callback_des spi_dev_cb;

    
	param = 0;
	sddev_control(SPI_DEV_NAME, CMD_SPI_INIT_MSTEN, (void *)&param);

	/* data bit width */
	param = 0;
	sddev_control(SPI_DEV_NAME, CMD_SPI_SET_BITWIDTH, (void *)&param);

	/* baudrate */
	BK_SPI_PRT("max_hz = %d \n", rate);
	sddev_control(SPI_DEV_NAME, CMD_SPI_SET_CKR, (void *)&rate);

	/* mode */
	if (mode & BK_SPI_CPOL)
		param = 1;
	else
		param = 0;
	sddev_control(SPI_DEV_NAME, CMD_SPI_SET_CKPOL, (void *)&param);

	/* CPHA */
	if (mode & BK_SPI_CPHA)
		param = 1;
	else
		param = 0;
	sddev_control(SPI_DEV_NAME, CMD_SPI_SET_CKPHA, (void *)&param);

	/* Master */
	param = 1;
	sddev_control(SPI_DEV_NAME, CMD_SPI_SET_MSTEN, (void *)&param);
	param = 1;
	sddev_control(SPI_DEV_NAME, CMD_SPI_SET_NSSMD, (void *)&param);


	/* set call back func */
//	spi_dev_cb.callback = bk_spi_rx_callback;
//	spi_dev_cb.param = NULL;
//	sddev_control(SPI_DEV_NAME, CMD_SPI_SET_RX_CALLBACK, (void *)&spi_dev_cb);
//
//	spi_dev_cb.callback = bk_spi_tx_needwrite_callback;
//	spi_dev_cb.param = NULL;
//	sddev_control(SPI_DEV_NAME, CMD_SPI_SET_TX_NEED_WRITE_CALLBACK, (void *)&spi_dev_cb);
//
//	spi_dev_cb.callback = bk_spi_tx_finish_callback;
//	spi_dev_cb.param = NULL;
//	sddev_control(SPI_DEV_NAME, CMD_SPI_SET_TX_FINISH_CALLBACK, (void *)&spi_dev_cb);

	/* enable spi */
	param = 1;
	sddev_control(SPI_DEV_NAME, CMD_SPI_UNIT_ENABLE, (void *)&param);

	BK_SPI_PRT("spi_master:[CTRL]:0x%08x \n", REG_READ(SPI_CTRL));
}

static void bk_spi_unconfigure(void)
{
	sddev_control(SPI_DEV_NAME, CMD_SPI_DEINIT_MSTEN, NULL);
}

int bk_spi_master_xfer(struct spi_message *msg)
{
	UINT32 param, total_size;

	ASSERT(spi_dev != NULL);
	ASSERT(msg != NULL);

	rtos_lock_mutex(&spi_dev->mutex);

	total_size = msg->recv_len + msg->send_len;
	if (total_size) {
		GLOBAL_INT_DECLARATION();

		/* initial spi_dev */
		GLOBAL_INT_DISABLE();
		spi_dev->tx_ptr = msg->send_buf;
		spi_dev->tx_len = msg->send_len;

		spi_dev->rx_ptr = msg->recv_buf;
		spi_dev->rx_len = msg->recv_len;
		spi_dev->rx_offset = 0;
		spi_dev->rx_drop = msg->send_len;

		spi_dev->total_len = total_size;
		spi_dev->flag &= ~(TX_FINISH_FLAG);
		GLOBAL_INT_RESTORE();

		/* take CS */
		param = 0x2;
		sddev_control(SPI_DEV_NAME, CMD_SPI_SET_NSSMD, (void *)&param);

		/* enabel tx & rx interrupt */
		param = 1;
		sddev_control(SPI_DEV_NAME, CMD_SPI_RXINT_EN, (void *)&param);
		sddev_control(SPI_DEV_NAME, CMD_SPI_TXINT_EN, (void *)&param);

		//os_printf("0 %d\r\n", total_size);

		/* wait tx finish */
		rtos_get_semaphore(&spi_dev->tx_sem, BEKEN_NEVER_TIMEOUT);

		//os_printf("1 %d\r\n", total_size);

		/* disable tx & rx interrupt again */
		param = 0;
		sddev_control(SPI_DEV_NAME, CMD_SPI_RXINT_EN, (void *)&param);
		sddev_control(SPI_DEV_NAME, CMD_SPI_TXINT_EN, (void *)&param);

		/* release CS */
		param = 0x3;
		sddev_control(SPI_DEV_NAME, CMD_SPI_SET_NSSMD, (void *)&param);

		/* initial spi_dev with zero*/
		GLOBAL_INT_DISABLE();
		spi_dev->tx_ptr = NULL;
		spi_dev->tx_len = 0;

		spi_dev->rx_ptr = NULL;
		spi_dev->rx_len = 0;

		spi_dev->total_len = 0;
		spi_dev->flag |= TX_FINISH_FLAG;
		GLOBAL_INT_RESTORE();
	}

	rtos_unlock_mutex(&spi_dev->mutex);

	return msg->recv_len;
}

int bk_spi_master_init(UINT32 rate, UINT32 mode)
{
	OSStatus result = 0;

	if (spi_dev)
		bk_spi_master_deinit();

	spi_dev = os_malloc(sizeof(struct bk_spi_dev));
	if (!spi_dev) {
		BK_SPI_PRT("[spi]:malloc memory for spi_dev failed\n");
		result = -1;
		goto _exit;
	}
	os_memset(spi_dev, 0, sizeof(struct bk_spi_dev));


	result = rtos_init_semaphore(&spi_dev->tx_sem, 1);
	if (result != kNoErr) {
		BK_SPI_PRT("[spi]: spi tx semp init failed\n");
		goto _exit;
	}

	result = rtos_init_mutex(&spi_dev->mutex);
	if (result != kNoErr) {
		BK_SPI_PRT("[spi]: spi mutex init failed\n");
		goto _exit;
	}

	spi_dev->tx_ptr = NULL;
	spi_dev->tx_len = 0;
	spi_dev->flag |= TX_FINISH_FLAG;

	bk_spi_configure(rate, mode);

	return 0;

_exit:
	if (spi_dev->mutex)
		rtos_deinit_mutex(&spi_dev->mutex);

	if (spi_dev->tx_sem)
		rtos_deinit_semaphore(&spi_dev->tx_sem);

	if (spi_dev) {
		os_free(spi_dev);
		spi_dev = NULL;
	}

	return 1;
}

int bk_spi_master_deinit(void)
{
	if (spi_dev == NULL)
		return 0;

	if (spi_dev->mutex)
		rtos_lock_mutex(&spi_dev->mutex);

	if (spi_dev->tx_sem)
		rtos_deinit_semaphore(&spi_dev->tx_sem);
	if (spi_dev->rx_sem)
		rtos_deinit_semaphore(&spi_dev->rx_sem);
	if (spi_dev->dma_tx_sem)
		rtos_deinit_semaphore(&spi_dev->dma_tx_sem);
	if (spi_dev->dma_rx_sem)
		rtos_deinit_semaphore(&spi_dev->dma_rx_sem);

	if (spi_dev->mutex) {
		rtos_unlock_mutex(&spi_dev->mutex);
		rtos_deinit_mutex(&spi_dev->mutex);
	}

	if (spi_dev) {
		os_free(spi_dev);
		spi_dev = NULL;
	}

	bk_spi_unconfigure();

	return 0;
}

#if CFG_USE_SPI_DMA

#define SPI_TEST_POART1  0
#define SPI_TEST_POART2  1
#define SPI_TX_BUFFER_SIZE  30*24
#define SPI_RX_BUFFER_SIZE  30*24
#define SPI_RX_DMA_CHANNEL     GDMA_CHANNEL_1
#define SPI_TX_DMA_CHANNEL     GDMA_CHANNEL_3

void spi_debug_prt(void);
void spi_dma_tx_enable(UINT8 enable);
void spi_dma_rx_enable(UINT8 enable);
void spi_dma_tx_half_handler(UINT32 param);
void spi_dma_rx_half_handler(UINT32 param);

static void (*user_dma_tx_finish_callback)(void)=NULL;
static void (*user_dma_rx_finish_callback)(void)=NULL;



void bk_spi_dma_rx_finish_callback(UINT32 param)
{
	spi_dev->flag &= ~(RX_FINISH_FLAG);
    rtos_set_semaphore(&spi_dev->dma_rx_sem);
    if(user_dma_rx_finish_callback)
    {
        
        user_dma_rx_finish_callback();
    }
}





void bk_spi_dma_tx_finish_callback(UINT32 param)
{

	spi_dev->flag &= ~(TX_FINISH_FLAG);
    rtos_set_semaphore(&spi_dev->dma_tx_sem);
    if(user_dma_tx_finish_callback)
    {
       
        user_dma_tx_finish_callback();
    }
}

int spi_dma_master_tx_init(struct spi_message *spi_msg)
{
	GDMACFG_TPYES_ST init_cfg;
	GDMA_CFG_ST en_cfg;

	os_printf("spi dma tx init\r\n");
	os_memset(&init_cfg, 0, sizeof(GDMACFG_TPYES_ST));
	os_memset(&en_cfg, 0, sizeof(GDMA_CFG_ST));

	init_cfg.dstdat_width = 8;
	init_cfg.srcdat_width = 32;
	init_cfg.dstptr_incr =  0;
	init_cfg.srcptr_incr =  1;

	init_cfg.src_start_addr = spi_msg->send_buf;
	init_cfg.dst_start_addr = (void *)SPI_DAT;

	init_cfg.channel = SPI_TX_DMA_CHANNEL ;
	init_cfg.prio = 0;
	init_cfg.u.type4.src_loop_start_addr = spi_msg->send_buf;
	init_cfg.u.type4.src_loop_end_addr = spi_msg->send_buf + spi_msg->send_len;

	init_cfg.half_fin_handler = NULL;
	init_cfg.fin_handler = NULL;
    spi_ctrl(CMD_SPI_SET_TX_FINISH_INT_CALLBACK, (void *) bk_spi_dma_tx_finish_callback);


	init_cfg.src_module = GDMA_X_SRC_DTCM_RD_REQ;
	init_cfg.dst_module = GDMA_X_DST_GSPI_TX_REQ;

	sddev_control(GDMA_DEV_NAME, CMD_GDMA_CFG_TYPE4, (void *)&init_cfg);

	en_cfg.channel = SPI_TX_DMA_CHANNEL;
	en_cfg.param = spi_msg->send_len;	// dma translen
	sddev_control(GDMA_DEV_NAME, CMD_GDMA_SET_TRANS_LENGTH, (void *)&en_cfg);

	en_cfg.channel = SPI_TX_DMA_CHANNEL;
	en_cfg.param = 0;					// 0:not repeat 1:repeat
	sddev_control(GDMA_DEV_NAME, CMD_GDMA_CFG_WORK_MODE, (void *)&en_cfg);

	en_cfg.channel = SPI_TX_DMA_CHANNEL;
	en_cfg.param = 0; // src no loop
	sddev_control(GDMA_DEV_NAME, CMD_GDMA_CFG_SRCADDR_LOOP, &en_cfg);



	return 0;
}

int spi_dma_master_rx_init(struct spi_message *spi_msg)
{
	GDMACFG_TPYES_ST init_cfg;
	GDMA_CFG_ST en_cfg;

	os_printf("spi dma rx init\r\n");
	os_memset(&init_cfg, 0, sizeof(GDMACFG_TPYES_ST));
	os_memset(&en_cfg, 0, sizeof(GDMA_CFG_ST));

	init_cfg.dstdat_width = 32;
	init_cfg.srcdat_width = 8;
	init_cfg.dstptr_incr =  1;
	init_cfg.srcptr_incr =  0;

	init_cfg.src_start_addr = (void *)SPI_DAT;
	init_cfg.dst_start_addr = spi_msg->recv_buf;

	init_cfg.channel = SPI_RX_DMA_CHANNEL;
	init_cfg.prio = 0;
	init_cfg.u.type5.dst_loop_start_addr = spi_msg->recv_buf;
	init_cfg.u.type5.dst_loop_end_addr = spi_msg->recv_buf + spi_msg->recv_len;

	init_cfg.half_fin_handler = NULL;
	init_cfg.fin_handler = NULL;
    spi_ctrl(CMD_SPI_SET_RX_FINISH_INT_CALLBACK, (void *) bk_spi_dma_rx_finish_callback);

	init_cfg.src_module = GDMA_X_SRC_GSPI_RX_REQ;
	init_cfg.dst_module = GDMA_X_DST_DTCM_WR_REQ;

	sddev_control(GDMA_DEV_NAME, CMD_GDMA_CFG_TYPE5, (void *)&init_cfg);

	en_cfg.channel = SPI_RX_DMA_CHANNEL;
	en_cfg.param   = spi_msg->recv_len;		// dma translen
	sddev_control(GDMA_DEV_NAME, CMD_GDMA_SET_TRANS_LENGTH, (void *)&en_cfg);


	en_cfg.channel = SPI_RX_DMA_CHANNEL;
	en_cfg.param = 0;						// 0:not repeat 1:repeat
	sddev_control(GDMA_DEV_NAME, CMD_GDMA_CFG_WORK_MODE, (void *)&en_cfg);

	en_cfg.channel = SPI_RX_DMA_CHANNEL;
	en_cfg.param = 0; // src no loop
	sddev_control(GDMA_DEV_NAME, CMD_GDMA_CFG_DSTADDR_LOOP, &en_cfg);

	return 0;
}


void bk_spi_master_dma_config(UINT32 mode, UINT32 rate)
{
	UINT32 param;
	os_printf("spi master dma init: mode:%d, rate:%d\r\n", mode, rate);
	bk_spi_configure(rate, mode);

	//disable tx/rx int disable
	param = 0;
	sddev_control(SPI_DEV_NAME, CMD_SPI_TXINT_EN, (void *)&param);

	param = 0;
	sddev_control(SPI_DEV_NAME, CMD_SPI_RXINT_EN, (void *)&param);

	//disable rx/tx finish enable bit
	param = 1;
	sddev_control(SPI_DEV_NAME, CMD_SPI_TXFINISH_EN, (void *)&param);

	param = 1;
	sddev_control(SPI_DEV_NAME, CMD_SPI_RXFINISH_EN, (void *)&param);

	//disable rx/tx over
	param = 0;
	sddev_control(SPI_DEV_NAME, CMD_SPI_RXOVR_EN, (void *)&param);

	param = 0;
	sddev_control(SPI_DEV_NAME, CMD_SPI_TXOVR_EN, (void *)&param);

	param = 1;
	sddev_control(SPI_DEV_NAME, CMD_SPI_SET_NSSMD, (void *)&param);
    sddev_control(SPI_DEV_NAME, CMD_SPI_SET_3_LINE, NULL);

	param = 0;
	sddev_control(SPI_DEV_NAME, CMD_SPI_SET_BITWIDTH, (void *)&param);

	//disable CSN intterrupt
	param = 0;
	sddev_control(SPI_DEV_NAME, CMD_SPI_CS_EN, (void *)&param);

	//clk test
	//param = 5000000;
	//sddev_control(SPI_DEV_NAME, CMD_SPI_SET_CKR, (void *)&param);

	os_printf("spi_master [CTRL]:0x%08x \n", REG_READ(SPI_CTRL));
	os_printf("spi_master [CONFIG]:0x%08x \n", REG_READ(SPI_CONFIG));

}

int bk_spi_master_dma_tx_rx_init(UINT32 mode, UINT32 rate, struct spi_message *spi_msg)
{
	OSStatus result = 0;
    
	UINT32 param = 0;

    if(spi_dev)
        bk_spi_master_deinit();
	spi_dev = os_malloc(sizeof(struct bk_spi_dev));
	if (!spi_dev) {
		BK_SPI_PRT("[spi]:malloc memory for spi_dev failed\n");
		result = 1;
	    goto _exit;
	}
	os_memset(spi_dev, 0, sizeof(struct bk_spi_dev));

    result = rtos_init_semaphore(&spi_dev->dma_tx_sem, 1);
	if (result != kNoErr) {
		BK_SPI_PRT("[spi]: spi tx semp init failed\n");
        result = 2;
        goto _exit;
	}	

    result = rtos_init_semaphore(&spi_dev->dma_rx_sem, 1);
	if (result != kNoErr) {
		BK_SPI_PRT("[spi]: spi tx semp init failed\n");
       result = 3;
        goto _exit;
	}
    
	bk_spi_master_dma_config(mode, rate);

	spi_dma_master_tx_init(spi_msg);
    spi_dma_tx_enable(0);
    spi_dev->init_dma_tx = 1;
    spi_dev->tx_ptr = spi_msg->send_buf;
    spi_dev->tx_len = spi_msg->send_len;
    sddev_control(SPI_DEV_NAME, CMD_SPI_TXTRANS_EN, (void *)&spi_msg->send_len);
	param = spi_msg->recv_len;
	sddev_control(SPI_DEV_NAME, CMD_SPI_RXTRANS_EN, (void *)&spi_msg->recv_len);
	spi_dma_master_rx_init(spi_msg);
    spi_dma_rx_enable(0);
    spi_dev->init_dma_rx = 1;
    spi_dev->rx_ptr = spi_msg->recv_buf;
    spi_dev->rx_len = spi_msg->recv_len;

	return result;
 _exit:

	bk_spi_master_deinit();
   return result;
}

int bk_spi_master_dma_tx_init(UINT32 mode, UINT32 rate, struct spi_message *spi_msg)
{
	OSStatus result = 0;

	if (spi_dev)
		bk_spi_master_deinit();

	spi_dev = os_malloc(sizeof(struct bk_spi_dev));
	if (!spi_dev) {
		BK_SPI_PRT("[spi]:malloc memory for spi_dev failed\n");
		result = -1;
		goto _exit;
	}
	os_memset(spi_dev, 0, sizeof(struct bk_spi_dev));


	result = rtos_init_semaphore(&spi_dev->dma_tx_sem, 1);
	if (result != kNoErr) {
		BK_SPI_PRT("[spi]: spi tx semp init failed\n");
		goto _exit;
	}

	bk_spi_master_dma_config(mode, rate);

	spi_dma_master_tx_init(spi_msg);
    spi_dma_tx_enable(0);
     sddev_control(SPI_DEV_NAME, CMD_SPI_TXTRANS_EN, (void *)&spi_msg->send_len);

    spi_dev->init_dma_tx = 1;
    spi_dev->tx_ptr = spi_msg->send_buf;
    spi_dev->tx_len = spi_msg->send_len;


	return 0;

_exit:

	bk_spi_master_deinit();

	return 1;
}

int bk_spi_master_dma_rx_init(UINT32 mode, UINT32 rate, struct spi_message *spi_msg)
{
	OSStatus result = 0;

	UINT32 param = 0;
	
	if (spi_dev)
		bk_spi_master_deinit();

	spi_dev = os_malloc(sizeof(struct bk_spi_dev));
	if (!spi_dev) {
		BK_SPI_PRT("[spi]:malloc memory for spi_dev failed\n");
		result = -1;
		goto _exit;
	}
	os_memset(spi_dev, 0, sizeof(struct bk_spi_dev));


	result = rtos_init_semaphore(&spi_dev->dma_rx_sem, 1);
	if (result != kNoErr) {
		BK_SPI_PRT("[spi]: spi tx semp init failed\n");
		goto _exit;
	}

	bk_spi_master_dma_config(mode, rate);
    spi_dma_rx_enable(0);

	param = spi_msg->recv_len;
	sddev_control(SPI_DEV_NAME, CMD_SPI_RXTRANS_EN, (void *)&param);
	
	spi_dma_master_rx_init(spi_msg);
    spi_dma_rx_enable(0);
    spi_dev->init_dma_rx = 1;
    spi_dev->rx_ptr = spi_msg->recv_buf;
    spi_dev->rx_len = spi_msg->recv_len;    
	return 0;

    _exit:
    
        bk_spi_master_deinit();


	return 1;
}

int bk_spi_master_dma_send(struct spi_message *spi_msg)
{
    int ret=0;

	GLOBAL_INT_DECLARATION();
	ASSERT(spi_msg != NULL);
    if(spi_dev->init_dma_tx == 0 || spi_dev==NULL)
    {
        bk_printf("spi_dma_send_ no init!\n");
        return -1;
    }
    if(spi_dev->flag&TX_FINISH_FLAG)
    {
        return -2;
    }

    if(spi_dev->tx_ptr != spi_msg->send_buf)
    {
        GDMA_CFG_ST en_cfg;
    	en_cfg.channel = SPI_TX_DMA_CHANNEL;
	    en_cfg.param   = spi_msg->send_buf;		// dma dst addr
        gdma_ctrl(CMD_GDMA_SET_SRC_START_ADDR, (void *)&en_cfg); 
        spi_dev->tx_ptr = spi_msg->send_buf;
        
    }
    if(spi_dev->tx_len != spi_msg->send_len)
    {
        GDMA_CFG_ST en_cfg;
        spi_ctrl(CMD_SPI_TXTRANS_EN, (void *)&spi_msg->send_len);
        en_cfg.channel = SPI_TX_DMA_CHANNEL;
        en_cfg.param = spi_msg->send_len;   // dma translen
        gdma_ctrl(CMD_GDMA_SET_TRANS_LENGTH, (void *)&en_cfg);
	    spi_dev->tx_len = spi_msg->send_len;
    }   

	GLOBAL_INT_DISABLE();
    spi_dev->flag |= TX_FINISH_FLAG;
	GLOBAL_INT_RESTORE();

	spi_dma_tx_enable(1);

	/* wait tx finish */
     if(user_dma_tx_finish_callback == NULL)
     {      
         //因为写DMA是主动操作，所以DMA传输完成后，SPI不一定发送完成了。

        rtos_get_semaphore(&spi_dev->dma_tx_sem, BEKEN_NEVER_TIMEOUT);
        ret = spi_dev->flag;
     }
     else
        ret = 0;

	if (spi_msg->send_buf != NULL)
		return ret;
	else {
		os_printf("spi_dma tx error send_buff\r\n", spi_msg->send_buf);
		return -3;
	}
}


int bk_spi_master_dma_recv(struct spi_message *spi_msg)
{
    int ret=0;

	GLOBAL_INT_DECLARATION();
	ASSERT(spi_msg != NULL);

    if(spi_dev->init_dma_rx == 0 || spi_dev==NULL)
    {
        bk_printf("spi_dma_recv_ no init!\n");
        return -1;
    }
    if(spi_dev->flag&RX_FINISH_FLAG)
    {
        return -2;
    }
    if(spi_dev->rx_ptr != spi_msg->recv_buf)
    {
        GDMA_CFG_ST en_cfg;
    	en_cfg.channel = SPI_RX_DMA_CHANNEL;
	    en_cfg.param   = spi_msg->recv_buf;		// dma dst addr
        gdma_ctrl( CMD_GDMA_SET_DST_START_ADDR, (void *)&en_cfg); 
        spi_dev->rx_ptr = spi_msg->recv_buf;
        
    }
    if(spi_dev->rx_len != spi_msg->recv_len)
    {
        GDMA_CFG_ST en_cfg;
        //设置spi读长度
	    spi_ctrl(CMD_SPI_RXTRANS_EN, (void *)&spi_msg->recv_len);   //最多12bit
	    //设置DMA长度
        en_cfg.channel = SPI_RX_DMA_CHANNEL;
        en_cfg.param   = spi_msg->recv_len;     // dma translen
        gdma_ctrl(CMD_GDMA_SET_TRANS_LENGTH, (void *)&en_cfg);
	    spi_dev->rx_len = spi_msg->recv_len;
    }        

	GLOBAL_INT_DISABLE();
    spi_dev->flag |= RX_FINISH_FLAG;
	GLOBAL_INT_RESTORE();
    spi_dma_rx_enable(1);

    if(user_dma_rx_finish_callback == NULL)
    {
       rtos_get_semaphore(&spi_dev->dma_rx_sem, BEKEN_NEVER_TIMEOUT);
       ret = spi_dev->flag;
    }
    else
       ret = 0;
       
    if (spi_msg->recv_buf != NULL)
           return ret;
	else {
		os_printf("spi_dma rx error recv_buff\r\n", spi_msg->recv_buf);
		return -3;
	}
}



void bk_master_dma_tx_finshed_callback(void (*func)(void))
{
    user_dma_tx_finish_callback = func;

}
void bk_master_dma_rx_finshed_callback(void (*func)(void))
{
    user_dma_rx_finish_callback = func;

}

void bk_master_dma_tx_disable(void)
{
	GDMA_CFG_ST en_cfg;

	spi_dma_tx_enable(0);
    spi_dev->flag &= (~TX_FINISH_FLAG);


}

void bk_master_dma_rx_disable(void)
{
	GDMA_CFG_ST en_cfg;
	spi_dma_rx_enable(0);
	spi_dev->flag &= (~RX_FINISH_FLAG);

}
#endif  //CFG_USE_SPI_DMA
#endif  // CFG_USE_SPI_MASTER
#endif  //(CFG_SOC_NAME == SOC_BK7231N)




