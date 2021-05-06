#include "include.h"
#include "arm_arch.h"

#include "spi.h"
#include "spi_pub.h"

#include "drv_model_pub.h"
#include "intc_pub.h"
#include "mcu_ps_pub.h"
#include "icu_pub.h"
#include "gpio_pub.h"
#include "uart_pub.h"

static spi_config_t spi_config;
//volatile spi_trans_t spi_trans;
static spi_message_t spi_rx_msg,spi_tx_msg;

static struct spi_callback_des spi_receive_end_callback = {NULL, NULL};
static struct spi_callback_des spi_txfifo_needwr_callback = {NULL, NULL};
static struct spi_callback_des spi_tx_end_callback = {NULL, NULL};




static SDD_OPERATIONS spi_op =
{
    spi_ctrl
};

static void spi_active(BOOLEAN val)
{
    UINT32 value;

    value = REG_READ(SPI_CTRL);
    if(val == 0)
    {
        value &= ~SPIEN;
    }
    else if(val == 1)
    {
        value |= SPIEN;
    }
    REG_WRITE(SPI_CTRL, value);
}

static void spi_set_msten(UINT8 val)
{
    UINT32 value;

    value = REG_READ(SPI_CTRL);
    if(val == 0)
    {
        value &= ~MSTEN;
    }
    else if(val == 1)
    {
        value |= MSTEN;
    }
    REG_WRITE(SPI_CTRL, value);
    spi_config.master = val;
}

static void spi_set_ckpha(UINT8 val)
{
    UINT32 value;

    value = REG_READ(SPI_CTRL);
    if(val == 0)
    {
        value &= ~CKPHA;
    }
    else
    {
        value |= CKPHA;
    }
    REG_WRITE(SPI_CTRL, value);
}

static void spi_set_skpol(UINT8 val)
{
    UINT32 value;

    value = REG_READ(SPI_CTRL);
    if(val == 0)
    {
        value &= ~CKPOL;
    }
    else
    {
        value |= CKPOL;
    }
    REG_WRITE(SPI_CTRL, value);
}



static void spi_set_bit_wdth(UINT8 val)
{
    UINT32 value;

    value = REG_READ(SPI_CTRL);
    if(val == 0)
    {
        value &= ~BIT_WDTH;
    }
    else if(val == 1)
    {
        value |= BIT_WDTH;
    }
    REG_WRITE(SPI_CTRL, value);
     spi_config.bit_wdth = val;
}

static void spi_set_nssmd(UINT8 val)
{
    UINT32 value;

    value = REG_READ(SPI_CTRL);
    
    #if (CFG_SOC_NAME == SOC_BK7231U)
    value &= ~(NSSMD_MASK << NSSMD_POSI);
    if(val)
        val=1;
    else
        val=0;
    value |= (val << NSSMD_POSI);
    #elif (CFG_SOC_NAME == SOC_BK7231N)
    value &= ~(CTRL_NSSMD_3);
        if(val)
           value |= CTRL_NSSMD_4;
        else
           value |= CTRL_NSSMD_3;
    #else
        value |= (val << NSSMD_POSI);
    #endif
    REG_WRITE(SPI_CTRL, value);
    spi_config.nss_mode = val;
}
//static void spi_set_ckr(UINT8 val)
//{
//    UINT32 value;
//
//    value = REG_READ(SPI_CTRL);
//    value &= ~(SPI_CKR_MASK << SPI_CKR_POSI);
//    value |= (val << SPI_CKR_POSI);
//    REG_WRITE(SPI_CTRL, value);
//}

/*
    spi_clk : 90M/3=30M    - DC0180 - DIV2
    spi_clk : 90M/4=22.5M  - DC0180 - DIV3
    spi_clk : 90M/5=18M    - DC0180 - DIV4
    spi_clk : 90M/6=15M    - DC0180 - DIV5
    spi_clk : 90M/7=12.85M - DC0180 - DIV6
    spi_clk : 90M/8=11.25M - DC0180 - DIV7
    spi_clk : 90M/9=10M    - DC0180 - DIV8
    spi_clk : 90M/10=9M    - DC0180 - DIV9
    spi_clk : 90M/11=8.18M - DC0180 - DIV10
*/
#define SPI_PERI_CLK_26M		(26 * 1000 * 1000)
#define SPI_PERI_CLK_DCO		(80 * 1000 * 1000)

static void spi_set_clock(UINT32 max_hz)
{
    int source_clk = 0;
    int spi_clk = 0;
    int div = 0;
    UINT32 param;
	//os_printf("\rmax_hz :%d\r\n", max_hz);

	if ((max_hz == 26000000) || (max_hz == 13000000) || (max_hz == 6500000) )
    {
        BK_SPI_PRT("config spi clk source 26MHz\n");

        spi_clk = max_hz;
        
#if CFG_XTAL_FREQUENCE

        source_clk = CFG_XTAL_FREQUENCE;
#else
        source_clk = SPI_PERI_CLK_26M;
#endif
        param = PCLK_POSI_SPI;
	    sddev_control(ICU_DEV_NAME, CMD_CONF_PCLK_26M, &param);
    }
    else if (max_hz > 4333000)
    {
        BK_SPI_PRT("config spi clk source DCO\n");

        if(max_hz > 30000000) // 120M/2 / (2 + 1) = 30M
        {
            spi_clk = 30000000; 
            BK_SPI_PRT("input clk > 30MHz, set input clk = 30MHz\n");
        } else {
            spi_clk = max_hz;
        }
        sddev_control(ICU_DEV_NAME, CMD_CLK_PWR_DOWN, &param);        
        source_clk = SPI_PERI_CLK_DCO;
        param = PCLK_POSI_SPI;
    	sddev_control(ICU_DEV_NAME, CMD_CONF_PCLK_DCO, &param);
        param = PWD_SPI_CLK_BIT;
        sddev_control(ICU_DEV_NAME, CMD_CLK_PWR_UP, &param); 
    }
    else 
    {
        BK_SPI_PRT("config spi clk source 26MHz\n");

        spi_clk = max_hz;
#if CFG_XTAL_FREQUENCE
        source_clk = CFG_XTAL_FREQUENCE;
#else
        source_clk = SPI_PERI_CLK_26M;
#endif
            
        param = PCLK_POSI_SPI;
	    sddev_control(ICU_DEV_NAME, CMD_CONF_PCLK_26M, &param);
    }
	if ((max_hz == 26000000) || (max_hz == 13000000) || (max_hz == 6500000))
    {
    	div = source_clk/spi_clk -1;
	}
	else
	{
	    // spi_clk = in_clk / (2 * (div + 1))
	    div = ((source_clk >> 1) / spi_clk); 

	    if (div < 2)
	    {
	        div = 2; 
	    }
	    else if (div >= 255)
	    {
	        div = 255;
	    }		
	}

    param = REG_READ(SPI_CTRL);
    param &= ~(SPI_CKR_MASK << SPI_CKR_POSI);
    param |= (div << SPI_CKR_POSI);
    REG_WRITE(SPI_CTRL, param);
    
    BK_SPI_PRT("\rdiv = %d \r\n", div);
    BK_SPI_PRT("\rspi_clk = %d \r\n", spi_clk);
    BK_SPI_PRT("\rsource_clk = %d \r\n", source_clk);
    os_printf("\rtarget frequency = %d, actual frequency = %d \r\n", max_hz, source_clk / 2 / div);
    spi_config.rate = spi_clk ;
}

static void spi_set_tx_buff(struct spi_message *msg)
{
    if(msg->send_buf)
    {
        spi_tx_msg.ptr_buff = msg->send_buf;
         spi_tx_msg.buff_len = msg->send_len;
          spi_tx_msg.count = msg->send_len;
    }
    if(msg->recv_buf)
    {
        spi_rx_msg.ptr_buff = msg->recv_buf;
         spi_rx_msg.buff_len = msg->recv_len;
          spi_rx_msg.count = msg->recv_len;
    }

}
static void spi_rxint_enable(UINT8 val)
{
    UINT32 value;

    value = REG_READ(SPI_CTRL);
    if(val == 0)
    {
        value &= ~RXINT_EN;
    }
    else if(val == 1)
    {
        value |= RXINT_EN;
    }
    REG_WRITE(SPI_CTRL, value);
    spi_config.rx_int = val;
    
}

static void spi_txint_enable(UINT8 val)
{
    UINT32 value;

    value = REG_READ(SPI_CTRL);
    if(val == 0)
    {
        value &= ~TXINT_EN;
    }
    else if(val == 1)
    {
        value |= TXINT_EN;
    }
    REG_WRITE(SPI_CTRL, value);
    spi_config.tx_int = val;
}

static void spi_rxovr_enable(UINT8 val)
{
    UINT32 value;

    value = REG_READ(SPI_CTRL);
    if(val == 0)
    {
        value &= ~RXOVR_EN;
    }
    else if(val == 1)
    {
        value |= RXOVR_EN;
    }
    REG_WRITE(SPI_CTRL, value);
    spi_config.rxovr_int = val;
}

static void spi_txovr_enable(UINT8 val)
{
    UINT32 value;

    value = REG_READ(SPI_CTRL);
    if(val == 0)
    {
        value &= ~TXOVR_EN;
    }
    else if(val == 1)
    {
        value |= TXOVR_EN;
    }
    REG_WRITE(SPI_CTRL, value);
    spi_config.txovr_int = val;
}

#if  ((CFG_SOC_NAME != SOC_BK7231U ) && (CFG_SOC_NAME != SOC_BK7231N ))
static void spi_rxfifo_clr(void)
{
    UINT32 value;

    value = REG_READ(SPI_CTRL);
    value |= RXFIFO_CLR;
    REG_WRITE(SPI_CTRL, value);
    
}

static void spi_txfifo_clr(void)
{
    UINT32 value;

    value = REG_READ(SPI_CTRL);
    value |= RXFIFO_CLR;
    REG_WRITE(SPI_CTRL, value);
}
#endif

static void spi_txint_mode(UINT8 val)
{
    UINT32 value;
#if  ((CFG_SOC_NAME == SOC_BK7231U)|| (CFG_SOC_NAME == SOC_BK7231N))
    value = REG_READ(SPI_CTRL);
    value &= ~(TXINT_MODE_MASK<<TXINT_MODE_POSI);
    value |= (val<<TXINT_MODE_POSI);
    REG_WRITE(SPI_CTRL, value);
    spi_config.tx_int_mode=val;
 #else  
     value = REG_READ(SPI_CTRL);
    if(val == 0)
    {
        value &= ~TXINT_MODE;
    }
    else if(val == 1)
    {
        value |= TXINT_MODE;
    }
    REG_WRITE(SPI_CTRL, value);
   #endif
}


static void spi_rxint_mode(UINT8 val)
{
    UINT32 value;
#if  ((CFG_SOC_NAME == SOC_BK7231U)|| (CFG_SOC_NAME == SOC_BK7231N))
    value = REG_READ(SPI_CTRL);
    value &= ~(RXINT_MODE_MASK<<RXINT_MODE_POSI);
    value |= (val<<RXINT_MODE_POSI);
    REG_WRITE(SPI_CTRL, value);
    spi_config.rx_int_mode=val;
 #else  
     value = REG_READ(SPI_CTRL);
    if(val == 0)
    {
        value &= ~RXINT_MODE;
    }
    else if(val == 1)
    {
        value |= RXINT_MODE;
    }
    REG_WRITE(SPI_CTRL, value);
   #endif
}


//仅对4线从模式有效
static void spi_slave_set_cs_finish_interrupt(UINT8 enable)
{
    UINT32 value;
	
#if (CFG_SOC_NAME == SOC_BK7231U)
    value = REG_READ(SPI_SLAVE_CTRL);
    if(enable)
    {
        value |= SPI_S_CS_UP_INT_EN;
    }
    else
    {
        value &= ~(SPI_S_CS_UP_INT_EN);
    }
    
    // don't clean cs finish status
    value &= ~(SPI_S_CS_UP_INT_STATUS);
    
    REG_WRITE(SPI_SLAVE_CTRL, value);
    spi_config.slave_cs_int = enable?1:0;
#elif (CFG_SOC_NAME == SOC_BK7231N)
	value = REG_READ(SPI_CTRL);
	if(enable)
	{
		value |= SPI_S_CS_UP_INT_EN;
	}
	else
	{
		value &= ~(SPI_S_CS_UP_INT_EN);
	}
	REG_WRITE(SPI_CTRL, value);
    spi_config.slave_cs_int = enable?1:0;
#endif
}

static void spi_gpio_configuration() //有被重置 的风险
{
    UINT32 param;
    param = GFUNC_MODE_SPI;

    
    sddev_control(GPIO_DEV_NAME, CMD_GPIO_ENABLE_SECOND, &param);

}

static void spi_icu_configuration(UINT32 enable)
{
    UINT32 param;

    if(enable) 
    {
        param = PWD_SPI_CLK_BIT;
	    sddev_control(ICU_DEV_NAME, CMD_CLK_PWR_UP, &param);

        param = (IRQ_SPI_BIT);
        sddev_control(ICU_DEV_NAME, CMD_ICU_INT_ENABLE, &param);
    }
    else
    {
        param = (IRQ_SPI_BIT);
        sddev_control(ICU_DEV_NAME, CMD_ICU_INT_DISABLE, &param);
        
        param = PWD_SPI_CLK_BIT;
	    sddev_control(ICU_DEV_NAME, CMD_CLK_PWR_DOWN, &param);
    }
}
#if (CFG_SOC_NAME == SOC_BK7231N)

static void spi_lsb_enbale(UINT8 val)
{
    UINT32 value;

    value = REG_READ(SPI_CTRL);
    if(val == 0)
    {
        value &= ~LSB_FIRST;
    }
    else if(val == 1)
    {
        value |= LSB_FIRST;
    }
    REG_WRITE(SPI_CTRL, value);
}


static void spi_tx_enbale(UINT8 val)
{
    UINT32 value;

    value = REG_READ(SPI_CONFIG);
    if(val == 0)
    {
        value &= ~SPI_TX_EN;
    }
    else if(val == 1)
    {
        value |= SPI_TX_EN;
    }
    REG_WRITE(SPI_CONFIG, value);
}

static void spi_rx_enbale(UINT8 val)
{
    UINT32 value;

    value = REG_READ(SPI_CONFIG);
    if(val == 0)
    {
        value &= ~SPI_RX_EN;
    }
    else if(val == 1)
    {
        value |= SPI_RX_EN;
    }
    REG_WRITE(SPI_CONFIG, value);
}

static void spi_txfinish_enbale(UINT8 val)
{
    UINT32 value;

    value = REG_READ(SPI_CONFIG);
    if(val == 0)
    {
        value &= ~SPI_TX_FINISH_EN;
    }
    else if(val == 1)
    {
        value |= SPI_TX_FINISH_EN;
    }
    REG_WRITE(SPI_CONFIG, value);

}

static void spi_rxfinish_enbale(UINT8 val)
{
    UINT32 value;

    value = REG_READ(SPI_CONFIG);
    if(val == 0)
    {
        value &= ~SPI_RX_FINISH_EN;
    }
    else if(val == 1)
    {
        value |= SPI_RX_FINISH_EN;
    }
    REG_WRITE(SPI_CONFIG, value);

}

static void set_txtrans_len(UINT32 val)
{
	UINT32 value;

    value = REG_READ(SPI_CONFIG);

    value &= ~(0xFFF << SPI_TX_TRAHS_LEN_POSI);
    value |= ((val & 0xFFF) << SPI_TX_TRAHS_LEN_POSI);
    
    REG_WRITE(SPI_CONFIG, value);
}

static void set_rxtrans_len(UINT32 val)
{
	UINT32 value;

    value = REG_READ(SPI_CONFIG);

    value &= ~(0xFFF << SPI_RX_TRAHS_LEN_POSI);
    value |= ((val & 0xFFF) << SPI_RX_TRAHS_LEN_POSI);
    
    REG_WRITE(SPI_CONFIG, value);
}
#endif

//4线模式的时候调用
static void spi_trans_start(UINT8 onoff)
{
    UINT32 value;
    #if (CFG_SOC_NAME == SOC_BK7231U)
    value = REG_READ(SPI_CTRL);
    value |= (0x03UL << NSSMD_POSI);
     REG_WRITE(SPI_CTRL, value);
     value &= ~(NSSMD_MASK << NSSMD_POSI);
    if (spi_config.nss_mode & 0X10)
    {
        value = REG_READ(SPI_CTRL);
        if(onoff){
            value |= (0x02UL << NSSMD_POSI);
        }
        else{
             value |= (0x03UL << NSSMD_POSI);
        }
        REG_WRITE(SPI_CTRL, value);
    }
     #endif
}

static void spi_init_msten(UINT8 msten)
{
    UINT32 value = 0;
    UINT32 param;

    spi_icu_configuration(1);
    spi_gpio_configuration();
    #if (CFG_SOC_NAME == SOC_BK7231U)
    param = GPIO_CFG_PARAM(17, GMODE_SECOND_FUNC_PULL_DOWN);
    sddev_control(GPIO_DEV_NAME, CMD_GPIO_CFG, &param);

    #endif

    value=0;
    REG_WRITE(SPI_CTRL, value);
    if(msten == 0)
    {
        value = SPIEN
                | (0x07UL << SPI_CKR_POSI);
    }
    else if(msten == 1)
    {
        value = SPIEN
                | MSTEN
                | (0x07UL << SPI_CKR_POSI);

    }
    REG_WRITE(SPI_CTRL, value);


}
static void spi_deinit_msten(void)
{
    UINT32 value = 0;

    spi_icu_configuration(0);
    //spi_gpio_configuration();
    value=0;
    REG_WRITE(SPI_CTRL, value);
    


}

#if (CFG_SOC_NAME != SOC_BK7231N)

static UINT8 spi_get_busy(void)
{
    UINT32 value;

    value = REG_READ(SPI_STAT);
    if(value & SPIBUSY)
    {
        return 1;
    }

    return 0;
}
#endif


void spi_init(void)
{
    intc_service_register(IRQ_SPI, PRI_IRQ_SPI, spi_isr);

    sddev_register_dev(SPI_DEV_NAME, &spi_op);


}

void spi_exit(void)
{
    sddev_unregister_dev(SPI_DEV_NAME);

}

UINT32 spi_ctrl(UINT32 cmd, void *param)
{
    UINT32 ret = SPI_SUCCESS;
    peri_busy_count_add();
    switch(cmd)
    {
    case CMD_SPI_UNIT_ENABLE:
        spi_active(*(UINT8 *)param);
        break;
    case CMD_SPI_SET_MSTEN:
        spi_set_msten(*(UINT8 *)param);
        break;
    case CMD_SPI_SET_CKPHA:
        spi_set_ckpha(*(UINT8 *)param);
        break;
    case CMD_SPI_SET_CKPOL:
        spi_set_skpol(*(UINT8 *)param);
        break;

    case CMD_SPI_SET_BITWIDTH:
        spi_set_bit_wdth(*(UINT8 *)param);
        break;
    case CMD_SPI_SET_NSSMD:
        spi_set_nssmd(*(UINT8 *)param);
        break;
    case CMD_SPI_SET_CKR:
        spi_set_clock(*(UINT32 *)param);
        break;
    case CMD_SPI_RXINT_EN:
        spi_rxint_enable(*(UINT8 *)param);
        break;
    case CMD_SPI_TXINT_EN:
        spi_txint_enable(*(UINT8 *)param);
        break;
    case CMD_SPI_SET_RX_CALLBACK:
        {
        struct spi_callback_des *callback = (struct spi_callback_des *)param;
        spi_receive_end_callback.callback = callback->callback;
        spi_receive_end_callback.param = callback->param;
        }
    break;    
    case CMD_SPI_SET_TX_FINISH_CALLBACK:
        {
            struct spi_callback_des *callback = (struct spi_callback_des *)param;
            spi_tx_end_callback.callback=callback->callback;
            spi_tx_end_callback.param=callback->param;
        }
        break;
    case CMD_SPI_RXOVR_EN:
        spi_rxovr_enable(*(UINT8 *)param);
        break;
    case CMD_SPI_TXOVR_EN:
        spi_txovr_enable(*(UINT8 *)param);
        break;
   #if  ((CFG_SOC_NAME != SOC_BK7231U ) && (CFG_SOC_NAME != SOC_BK7231N ))
    case CMD_SPI_RXFIFO_CLR:
        spi_rxfifo_clr();
        break;
    case CMD_SPI_TXFIFO_CLR:
        spi_txfifo_clr();
        break;
    #endif
    case CMD_SPI_RXINT_MODE:
        spi_rxint_mode(*(UINT8 *)param);
        break;
    case CMD_SPI_TXINT_MODE:
        spi_txint_mode(*(UINT8 *)param);
        break;
//    case CMD_SPI_START_TRANS:
//        spi_trans_start(*(UINT8 *)param);
//        break;
    case CMD_SPI_INIT_MSTEN:
        spi_init_msten(*(UINT8 *)param);
        break;
    case CMD_SPI_DEINIT_MSTEN:
        spi_deinit_msten();
    break;
    case CMD_SPI_GET_BUSY:
#if (CFG_SOC_NAME != SOC_BK7231N)
                (*((UINT8 *)param)) = spi_get_busy();
#endif
        break;
    case CMD_SPI_SET_BUFF:

        spi_set_tx_buff((struct spi_message *)param);
    break;
#if (CFG_SOC_NAME == SOC_BK7231N)	
        case CMD_SPI_LSB_EN:
            spi_lsb_enbale(*(UINT8 *)param);
            break;
        case CMD_SPI_TX_EN:
            spi_tx_enbale(*(UINT8 *)param);
            break;
        case CMD_SPI_RX_EN:
            spi_rx_enbale(*(UINT8 *)param);
            break;
        case CMD_SPI_TXFINISH_EN:
            spi_txfinish_enbale(*(UINT8 *)param);
            break;
        case CMD_SPI_RXFINISH_EN:
            spi_rxfinish_enbale(*(UINT8 *)param);
            break;
        case CMD_SPI_TXTRANS_EN:
            set_txtrans_len(*(UINT32 *)param);
            break;
        case CMD_SPI_RXTRANS_EN:
            set_rxtrans_len(*(UINT32 *)param);
            break;
        case CMD_SPI_CS_EN:
            spi_slave_set_cs_finish_interrupt(*(UINT32 *)param);
            break;
#endif	

    default:
        ret = SPI_FAILURE;
        break;
    }

    peri_busy_count_dec();

    return ret;
}




void spi_isr(void)
{
    UINT32 status, slv_status;
    volatile UINT8 fifo_empty_num, data_num, rxfifo_empty;
     UINT32 rx_date;
    //REG_WRITE((0x00802800+(19*4)), 0x02);
    //REG_WRITE((0x00802800+(0x1a*4)), 0x02);
	//REG_WRITE((0x00802800+(0x1a*4)), 0x00);
    
	data_num = 0; /*fix warning by clang analyzer*/
	fifo_empty_num = 0; /*fix warning by clang analyzer*/
	
#if (CFG_SOC_NAME != SOC_BK7231N)		
    status = REG_READ(SPI_STAT);
    slv_status = REG_READ(SPI_SLAVE_CTRL);
    REG_WRITE(SPI_SLAVE_CTRL, slv_status);
#else
	status = REG_READ(SPI_STAT);
//	REG_WRITE(SPI_STAT, status);
#endif
    REG_WRITE(SPI_STAT, status);

    BK_SPI_PRT("SPI_STAT:0x%08x\r\n", status);
    //REG_WRITE((0x00802800+(19*4)), 0x00);
    
#if (CFG_SOC_NAME != SOC_BK7231N)	    
    if((status & RXINT) || (slv_status & SPI_S_CS_UP_INT_STATUS)) 
#else
	if( (status & RXINT) || (status & SPI_S_CS_UP_INT_STATUS) )
#endif
    {
        
        do{
            rx_date= REG_READ(SPI_DAT);
            if(spi_rx_msg.count)
            {
                if(spi_rx_msg.ptr_buff)
                {
                     *(spi_rx_msg.ptr_buff) = rx_date;
                     spi_rx_msg.ptr_buff++;
                     spi_rx_msg.count--;
                 }

            }
            else
            {
                spi_rxint_enable(0);
                break;

            }

        }
    #if (CFG_SOC_NAME != SOC_BK7231N)     
        while((REG_READ(SPI_STAT) & RXFIFO_EMPTY) == 0);
    #else
        while(REG_READ(SPI_STAT) & RXFIFO_RD_READ);
    #endif
        if(spi_rx_msg.count==0)
        {
            spi_rxint_enable(0);
            if(spi_receive_end_callback.callback)
                spi_receive_end_callback.callback(0);
         }
           
    }

    if(status & TXINT)
    {
        //REG_WRITE((0x00802800+(0x1c*4)), 0x02);
		//REG_WRITE((0x00802800+(0x1c*4)), 0x00);
		//os_printf("spi txint\r\n");

            do{
                BK_SPI_PRT("Count:%d \r\n",spi_tx_msg.count);
                if(spi_tx_msg.count)
                {
                
                    if(spi_tx_msg.ptr_buff)
                    {
                        REG_READ(SPI_DAT) =  *(spi_tx_msg.ptr_buff);
                        spi_tx_msg.ptr_buff++;
                        
                    }
                    spi_tx_msg.count--;
                }
                else
                {
                    spi_txint_enable(0);
                    break;

                }

            }
            #if (CFG_SOC_NAME != SOC_BK7231N)     
                while((REG_READ(SPI_STAT) & TXFIFO_EMPTY));
            #else
                while(REG_READ(SPI_STAT) & TXFIFO_WR_READ);
            #endif
                if (spi_tx_end_callback.callback != 0)
                {
                    void *param = spi_tx_end_callback.param;
                    spi_tx_end_callback.callback( param);
                }
                else
                {
                    /*fill txfifo with 0xff*/
                    //spi_txfifo_fill();
                }

    }

    if(status & TXOVR)
    {
        os_printf("txovr\r\n");
    }

    if(status & RXOVR)
    {
        os_printf("rxovr\r\n");
    }
	
#if (CFG_SOC_NAME == SOC_BK7231N)	
	if(status & RX_FINISH_INT)
    {
        os_printf("rx finish int \r\n");
    } 
#endif

#if (CFG_SOC_NAME != SOC_BK7231N)	
    if(status & MODF)
    {
        os_printf("spi mode error\r\n");
    }
#endif

//#if (CFG_SOC_NAME != SOC_BK7231N)	
//    if(status & TXFIFO_EMPTY)
//#else
//	if(status & TXFIFO_WR_READ)
//#endif
//    {
//        if (spi_tx_end_callback.callback != 0)
//        {
//            void *param = spi_tx_end_callback.param;
//            uint8_t disenable=0;
//            spi_ctrl(CMD_SPI_TX_EN, &disenable);
//            spi_tx_end_callback.callback(0, param);
//        }
//        else
//        {
//            /*fill txfifo with 0xff*/
//            //spi_txfifo_fill();
//        }
//    }
}
// eof 


// eof 

