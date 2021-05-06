#include "include.h"
#include "arm_arch.h"

#include "sys_ctrl_pub.h"
#include "sys_ctrl.h"
#include "target_util_pub.h"

#include "drv_model_pub.h"

#include "uart_pub.h"
#include "flash_pub.h"
#include "power_save_pub.h"
#include "intc_pub.h"
#include "icu_pub.h"
#include "gpio_pub.h"
#include "reg_rc.h"
#include "co_math.h"
#include "rw_pub.h"
#include "manual_ps_pub.h"
#include "mcu_ps_pub.h"
#include "ps_debug_pub.h"
#include "ble_pub.h"
#include "start_type_pub.h"
#include "bk7011_cal_pub.h"

#define DPLL_DIV                0x0
#define DCO_CALIB_26M           0x1
#define DCO_CALIB_60M           0x2
#define DCO_CALIB_80M           0x3
#define DCO_CALIB_120M          0x4
#define DCO_CALIB_180M          0x5

#if (CFG_SOC_NAME == SOC_BK7221U)  
#define DCO_CLK_SELECT          DCO_CALIB_180M
#define USE_DCO_CLK_POWON       1
#else
#define DCO_CLK_SELECT          DCO_CALIB_120M
#define USE_DCO_CLK_POWON       0 //BK7231N could not using DCO as main clock when boot since DCO_AMSEL_BIT should be disable during calibration
#endif

static UINT32 limit_freq_status = 0;
static SCTRL_PS_SAVE_VALUES ps_saves[2];
#if CFG_USE_LOW_IDLE_PS
static UINT32 ps_saves_gpio_cfgs[32];
#endif
static SCTRL_MCU_PS_INFO sctrl_mcu_ps_info =
{
    .hw_sleep = 0,
    .mcu_use_dco = 0,
    .first_sleep = 1,
};


static SDD_OPERATIONS sctrl_op =
{
    sctrl_ctrl
};

UINT32 rf_sleeped = 0;
UINT32 sta_rf_sleeped = 0;

extern void WFI( void );
/**********************************************************************/
void sctrl_dpll_delay10us(void)
{
    volatile UINT32 i = 0;
	
    for(i = 0; i < DPLL_DELAY_TIME_10US; i ++)
    {
        ;
    }
}

void sctrl_dpll_delay200us(void)
{
    volatile UINT32 i = 0;
	
    for(i = 0; i < DPLL_DELAY_TIME_200US; i ++)
    {
        ;
    }
}

void sctrl_ps_dpll_delay(UINT32 time)
{
    volatile UINT32 i = 0;

    for(i = 0; i < time; i ++)
    {
        ;
    }
}

void sctrl_cali_dpll(UINT8 flag)
{
    UINT32 param;
    extern void bk7011_update_tx_power_when_cal_dpll(int start_or_stop);
   
    bk7011_update_tx_power_when_cal_dpll(1);
    param = sctrl_analog_get(SCTRL_ANALOG_CTRL0);
    param &= ~(SPI_TRIG_BIT);
    sctrl_analog_set(SCTRL_ANALOG_CTRL0, param);

    if(!flag)
        sctrl_dpll_delay10us();
    else
        sctrl_ps_dpll_delay(60);

    param |= (SPI_TRIG_BIT);
    sctrl_analog_set(SCTRL_ANALOG_CTRL0, param);   
    
    param = sctrl_analog_get(SCTRL_ANALOG_CTRL0);
    param &= ~(SPI_DET_EN);
    sctrl_analog_set(SCTRL_ANALOG_CTRL0, param);

    if(!flag)
        sctrl_dpll_delay200us();
    else
        sctrl_ps_dpll_delay(340);

    param = sctrl_analog_get(SCTRL_ANALOG_CTRL0);
    param |= (SPI_DET_EN);
    sctrl_analog_set(SCTRL_ANALOG_CTRL0, param);

    bk7011_update_tx_power_when_cal_dpll(0);
}

void sctrl_dpll_isr(void)
{
    os_printf("BIAS Cali\r\n");
    bk7011_cal_bias();

    sddev_control(GPIO_DEV_NAME, CMD_GPIO_CLR_DPLL_UNLOOK_INT, NULL);    
    sctrl_cali_dpll(0);

    os_printf("DPLL Unlock\r\n");
}

void sctrl_dpll_int_open(void)
{
    UINT32 param;
    param = (FIQ_DPLL_UNLOCK_BIT);
    sddev_control(ICU_DEV_NAME, CMD_ICU_INT_ENABLE, &param);

    #if (CFG_SOC_NAME != SOC_BK7231)
    param = 1;
    sddev_control(GPIO_DEV_NAME, CMD_GPIO_EN_DPLL_UNLOOK_INT, &param);
    #endif
}

void sctrl_dpll_int_close(void)
{
    UINT32 param;
    
    #if (CFG_SOC_NAME != SOC_BK7231)
    param = 0;
    sddev_control(GPIO_DEV_NAME, CMD_GPIO_EN_DPLL_UNLOOK_INT, &param);
    #endif
    
    param = (FIQ_DPLL_UNLOCK_BIT);
    sddev_control(ICU_DEV_NAME, CMD_ICU_INT_DISABLE, &param);
}

void sctrl_dco_cali(UINT32 speed)
{
    UINT32 reg_val;
    
    switch(speed) 
    {
        case DCO_CALIB_180M: 
        reg_val = sctrl_analog_get(SCTRL_ANALOG_CTRL1);
        reg_val &= ~((DCO_CNTI_MASK << DCO_CNTI_POSI) | (DCO_DIV_MASK << DCO_DIV_POSI));
        reg_val |= ((0xDD & DCO_CNTI_MASK) << DCO_CNTI_POSI);
#if (CFG_SOC_NAME != SOC_BK7231N)
        reg_val |= DIV_BYPASS_BIT;
#endif
        sctrl_analog_set(SCTRL_ANALOG_CTRL1, reg_val);   
        break;
        
        case DCO_CALIB_120M:             
        reg_val = sctrl_analog_get(SCTRL_ANALOG_CTRL1);
        reg_val &= ~((DCO_CNTI_MASK << DCO_CNTI_POSI) | (DCO_DIV_MASK << DCO_DIV_POSI));
        reg_val |= ((0x127 & DCO_CNTI_MASK) << DCO_CNTI_POSI);
        sctrl_analog_set(SCTRL_ANALOG_CTRL1, reg_val);   
        break;

        case DCO_CALIB_80M:             
        reg_val = sctrl_analog_get(SCTRL_ANALOG_CTRL1);
        reg_val &= ~((DCO_CNTI_MASK << DCO_CNTI_POSI) | (DCO_DIV_MASK << DCO_DIV_POSI));
        reg_val |= ((0x0C5 & DCO_CNTI_MASK) << DCO_CNTI_POSI);
        sctrl_analog_set(SCTRL_ANALOG_CTRL1, reg_val);   
        break;

        case DCO_CALIB_60M:             
        reg_val = sctrl_analog_get(SCTRL_ANALOG_CTRL1);
        reg_val &= ~((DCO_CNTI_MASK << DCO_CNTI_POSI) | (DCO_DIV_MASK << DCO_DIV_POSI));
        reg_val |= ((0x127 & DCO_CNTI_MASK) << DCO_CNTI_POSI);
        reg_val |= ((0x02 & DCO_DIV_MASK) << DCO_DIV_POSI);
        sctrl_analog_set(SCTRL_ANALOG_CTRL1, reg_val);   
        break;

        default:
        reg_val = sctrl_analog_get(SCTRL_ANALOG_CTRL1);
        reg_val &= ~((DCO_CNTI_MASK << DCO_CNTI_POSI) | (DCO_DIV_MASK << DCO_DIV_POSI));
        reg_val |= ((0xC0 & DCO_CNTI_MASK) << DCO_CNTI_POSI);
        reg_val |= ((0x03 & DCO_DIV_MASK) << DCO_DIV_POSI);
        sctrl_analog_set(SCTRL_ANALOG_CTRL1, reg_val);  
        break;
    }

    reg_val = sctrl_analog_get(SCTRL_ANALOG_CTRL1);
    reg_val &= ~(SPI_RST_BIT);
    sctrl_analog_set(SCTRL_ANALOG_CTRL1, reg_val); 

    reg_val = sctrl_analog_get(SCTRL_ANALOG_CTRL1);
    reg_val |= SPI_RST_BIT;
    sctrl_analog_set(SCTRL_ANALOG_CTRL1, reg_val); 

    reg_val = sctrl_analog_get(SCTRL_ANALOG_CTRL1);
    reg_val |= DCO_TRIG_BIT;
    sctrl_analog_set(SCTRL_ANALOG_CTRL1, reg_val); 
    
    reg_val = sctrl_analog_get(SCTRL_ANALOG_CTRL1);
#if (CFG_SOC_NAME == SOC_BK7231N)
    reg_val &= ~(DCO_AMSEL_BIT);
#endif
    reg_val &= ~(DCO_TRIG_BIT);
    sctrl_analog_set(SCTRL_ANALOG_CTRL1, reg_val); 
}

void sctrl_set_cpu_clk_dco(void)
{
    UINT32 reg_val;

    reg_val = REG_READ(SCTRL_CONTROL);
    reg_val &= ~(MCLK_DIV_MASK << MCLK_DIV_POSI);
    reg_val &= ~(MCLK_MUX_MASK << MCLK_MUX_POSI);
    
    reg_val |= ((MCLK_FIELD_DCO&MCLK_MUX_MASK) << MCLK_MUX_POSI);
    reg_val |= HCLK_DIV2_EN_BIT;    
    delay(10);
    REG_WRITE(SCTRL_CONTROL, reg_val); 
}

#if CFG_USE_STA_PS
void sctrl_flash_select_dco(void)
{
    UINT32 reg;
    UINT32 status;
    DD_HANDLE flash_hdl;

    /* Flash 26MHz clock select dco clock*/
    flash_hdl = ddev_open(FLASH_DEV_NAME, &status, 0);
    ASSERT(DD_HANDLE_UNVALID != flash_hdl);
    ddev_control(flash_hdl, CMD_FLASH_SET_DCO, 0);
    /* flash get id  shouldn't remove*/
    ddev_control(flash_hdl, CMD_FLASH_GET_ID, &reg);
}

void sctrl_sta_ps_init(void)
{
    UINT32 reg;
    extern void power_save_wakeup_isr(void);

    reg = REG_READ(SCTRL_LOW_PWR_CLK);
    reg &=~(LPO_CLK_MUX_MASK);
#if (CFG_SOC_NAME == SOC_BK7231)
    reg |=(LPO_SRC_32K_DIV << LPO_CLK_MUX_POSI);
#else
    reg |=(LPO_SRC_ROSC << LPO_CLK_MUX_POSI);
#endif
    REG_WRITE(SCTRL_LOW_PWR_CLK, reg);

#if ((CFG_SOC_NAME == SOC_BK7231U) || (CFG_SOC_NAME == SOC_BK7221U))
    reg = REG_READ(SCTRL_BLOCK_EN_MUX);
    reg &=~(0x1FF);
    reg |=(0x40);
    REG_WRITE(SCTRL_BLOCK_EN_MUX, reg);
#endif

    #if PS_WAKEUP_MOTHOD_RW
    intc_service_register(FIQ_MAC_WAKEUP, PRI_FIQ_MAC_WAKEUP, power_save_wakeup_isr);
    nxmac_enable_lp_clk_switch_setf(0x01);
    os_printf("sctrl_sta_ps_init\r\n");
    #endif

    sctrl_flash_select_dco();

    power_save_rf_ps_wkup_semlist_init();
    
}
#endif

#if CFG_USE_BLE_PS
void sctrl_ble_ps_init(void)
{
}
#endif

void sctrl_init(void)
{
    UINT32 param;

    sddev_register_dev(SCTRL_DEV_NAME, &sctrl_op);

    /*enable blk clk
      Attention: ENABLE 26m xtal block(BLK_BIT_26M_XTAL), for protect 32k circuit
     */
    param = BLK_BIT_26M_XTAL | BLK_BIT_DPLL_480M | BLK_BIT_XTAL2RF | BLK_BIT_DCO;
    sctrl_ctrl(CMD_SCTRL_BLK_ENABLE, &param);

    /*config main clk*/
    #if !USE_DCO_CLK_POWON
    param = REG_READ(SCTRL_CONTROL);
    param &= ~(MCLK_DIV_MASK << MCLK_DIV_POSI);
    param &= ~(MCLK_MUX_MASK << MCLK_MUX_POSI);
    #if (CFG_SOC_NAME == SOC_BK7221U)  
    /* BK7221U ahb bus max rate is 90MHZ, so ahb bus need div 2 from MCU clock */
    /* AHB bus is very import to AUDIO and DMA */
    param |= HCLK_DIV2_EN_BIT;
    #endif // (CFG_SOC_NAME == SOC_BK7221U)
    #if CFG_SYS_REDUCE_NORMAL_POWER
    param |= ((MCLK_DIV_7 & MCLK_DIV_MASK) << MCLK_DIV_POSI);
    #elif (CFG_SOC_NAME == SOC_BK7231N)
    param |= ((MCLK_DIV_5 & MCLK_DIV_MASK) << MCLK_DIV_POSI);
    #else // CFG_SYS_REDUCE_NORMAL_POWER 
    param |= ((MCLK_DIV_3 & MCLK_DIV_MASK) << MCLK_DIV_POSI);
    #endif // CFG_SYS_REDUCE_NORMAL_POWER
    param |= ((MCLK_FIELD_DPLL & MCLK_MUX_MASK) << MCLK_MUX_POSI);
    REG_WRITE(SCTRL_CONTROL, param);
    #endif // (!USE_DCO_CLK_POWON)

    /*sys_ctrl <0x4c> */
    param = 0x00171710;//0x00151510;    LDO BIAS CALIBRATION
    REG_WRITE(SCTRL_BIAS, param);

    /*mac & modem power up */
    sctrl_ctrl(CMD_SCTRL_MAC_POWERUP, NULL);
    sctrl_ctrl(CMD_SCTRL_MODEM_POWERUP, NULL);

    /*sys_ctrl <0x16>, trig spi */
    //170209,from 0x819A54B to 0x819A55B for auto detect dpll unlock
    //170614 from 0x819A55B to 0x819A59B for more easy to trigger
    //BK7231U 2018.11.06 from 0xF819A59B to 0x0819A59B for xtal current
    #if (CFG_SOC_NAME == SOC_BK7231)  
    param = 0x819A59B;
#elif (CFG_SOC_NAME == SOC_BK7231N)
#if (CFG_XTAL_FREQUENCE == CFG_XTAL_FREQUENCE_40M)
    param = 0x71125B57;
#else
    param = 0x71104953;//wangjian20200918 Reg0x16<3:1>=1 Reg0x16<9>=0 Reg0x16<13:10>=2
#endif
    #else
    param = 0x0819A59B;//0x819A59B;
    #endif // (CFG_SOC_NAME == SOC_BK7231) 
    sctrl_analog_set(SCTRL_ANALOG_CTRL0, param);
    
    sctrl_cali_dpll(0);
    
#if (CFG_SOC_NAME == SOC_BK7231N)
    param = 0x3CC019C2;//wangjian20200918 Reg0x17<1>=1
#else
    param = 0x6AC03102;
#endif
    sctrl_analog_set(SCTRL_ANALOG_CTRL1, param);
    /*do dco Calibration*/
    sctrl_dco_cali(DCO_CLK_SELECT);
    #if USE_DCO_CLK_POWON
    sctrl_set_cpu_clk_dco();
    #endif

    #if (CFG_SOC_NAME == SOC_BK7231)
    param = 0x24006000;
#elif (CFG_SOC_NAME == SOC_BK7231N)
    param = 0x500020E2;//0x400020E0; //wangjian20200822 0x40032030->0x48032030->0x48022032//wangjian20200903<17:16>=0//qunshan20201127<28:23>=20
#else
    param = 0x24006080;   // xtalh_ctune   // 24006080
    param &= ~(XTALH_CTUNE_MASK<< XTALH_CTUNE_POSI);
    param |= ((0x10&XTALH_CTUNE_MASK) << XTALH_CTUNE_POSI);
    #endif // (CFG_SOC_NAME == SOC_BK7231)
    sctrl_analog_set(SCTRL_ANALOG_CTRL2, param);

    #if (CFG_SOC_NAME == SOC_BK7221U)
    param = CHARGE_ANALOG_CTRL3_CHARGE_DEFAULT_VALUE;
#elif (CFG_SOC_NAME == SOC_BK7231N)
    param = 0x70000000; //wangjiang20200822 0x00000000->0x70000000
    #else
    param = 0x4FE06C50;
    #endif
    sctrl_analog_set(SCTRL_ANALOG_CTRL3, param);

    /*sys_ctrl <0x1a> */
    #if (CFG_SOC_NAME == SOC_BK7231)
    param = 0x59E04520;
    #elif (CFG_SOC_NAME == SOC_BK7221U)
    param = CHARGE_ANALOG_CTRL4_CHARGE_DEFAULT_VALUE;
#elif (CFG_SOC_NAME == SOC_BK7231N)
    param = 0x19C04520;
    #else
    param = 0x59C04520;  // 0x59E04520 
    #endif // (CFG_SOC_NAME == SOC_BK7231)
    sctrl_analog_set(SCTRL_ANALOG_CTRL4, param);

    /*regist intteruppt handler for Dpll unlock*/
    intc_service_register(FIQ_DPLL_UNLOCK, PRI_FIQ_DPLL_UNLOCK, sctrl_dpll_isr);

    sctrl_sub_reset();

#if (CFG_SOC_NAME == SOC_BK7231N)
	sctrl_fix_dpll_div();
#endif
	
	/*sys ctrl clk gating, for rx dma dead*/
	REG_WRITE(SCTRL_CLK_GATING, 0x3f);

	/* increase VDD voltage*/
    #if 1//CFG_SYS_REDUCE_NORMAL_POWER
	param = 4;
    #else
	param = 5;
    #endif
    /* zhangheng modify to 4 without enable CFG_SYS_REDUCE_NORMAL_POWER, to reduce power by set vdd value, advised by xubin */
	param = 4;
    sctrl_ctrl(CMD_SCTRL_SET_VDD_VALUE, &param);

	/*32K Rosc calib*/
    REG_WRITE(SCTRL_ROSC_CAL, 0x7);

    #if (CFG_SOC_NAME == SOC_BK7221U)
    #if (CFG_USE_AUDIO)
    sctrl_analog_set(SCTRL_ANALOG_CTRL8, 0x0033187C);
    sctrl_analog_set(SCTRL_ANALOG_CTRL9, 0x82204007);
    sctrl_analog_set(SCTRL_ANALOG_CTRL10, 0x80801433);    
    #endif // CFG_USE_AUDIO
    #endif // (CFG_SOC_NAME == SOC_BK7221U)

	#if (RHINO_CONFIG_CPU_PWR_MGMT & CFG_USE_STA_PS)
	sctrl_mcu_init();
	#endif
}

void sctrl_exit(void)
{
    sddev_unregister_dev(SCTRL_DEV_NAME);
}

void sctrl_modem_core_reset(void)
{
    sctrl_ctrl(CMD_SCTRL_MODEM_CORE_RESET, 0);
}

void sctrl_sub_reset(void)
{
    sctrl_ctrl(CMD_SCTRL_MPIF_CLK_INVERT, 0);
    sctrl_ctrl(CMD_SCTRL_MODEM_CORE_RESET, 0);
    sctrl_ctrl(CMD_SCTRL_MODEM_SUBCHIP_RESET, 0);
    sctrl_ctrl(CMD_SCTRL_MAC_SUBSYS_RESET, 0);
    sctrl_ctrl(CMD_SCTRL_USB_SUBSYS_RESET, 0);
}

void ps_delay(volatile UINT16 times)
{
	UINT32	delay = times;
    while(delay--) ;
}

void sctrl_ps_dump()
{
    UINT32 i;
	
    os_printf("reg dump\r\n");
    os_printf("sys\r\n0x%8x:0x%8x\r\n", SCTRL_CONTROL, REG_READ(SCTRL_CONTROL));
    os_printf("0x%8x:0x%8x\r\n", SCTRL_MODEM_CORE_RESET_PHY_HCLK, REG_READ(SCTRL_MODEM_CORE_RESET_PHY_HCLK));
    os_printf("0x%8x:0x%8x\r\n", SCTRL_BLOCK_EN_CFG, REG_READ(SCTRL_BLOCK_EN_CFG));
    os_printf("0x%8x:0x%8x\r\n", SCTRL_ROSC_CAL, REG_READ(SCTRL_ROSC_CAL));
    os_printf("0x%8x:0x%8x\r\n", SCTRL_ANALOG_CTRL2, sctrl_analog_get(SCTRL_ANALOG_CTRL2));
    os_printf("0x%8x:0x%8x\r\n", ICU_INTERRUPT_ENABLE, REG_READ(ICU_INTERRUPT_ENABLE));
    os_printf("0x%8x:0x%8x\r\n", ICU_PERI_CLK_PWD, REG_READ(ICU_PERI_CLK_PWD));
    os_printf("0x%8x:0x%8x\r\n", SCTRL_SLEEP, REG_READ(SCTRL_SLEEP));
    os_printf("0x%8x:0x%8x\r\n", ICU_ARM_WAKEUP_EN, REG_READ(ICU_ARM_WAKEUP_EN));
    os_printf("mac\r\n0x%8x:0x%8x\r\n", NXMAC_TIMERS_INT_UN_MASK_ADDR, nxmac_timers_int_un_mask_get());
    os_printf("0x%8x:0x%8x\r\n", NXMAC_DOZE_CNTRL_1_ADDR, nxmac_doze_cntrl_1_get());
    os_printf("0x%8x:0x%8x\r\n", NXMAC_DOZE_CNTRL_2_ADDR, nxmac_doze_cntrl_2_get());
    os_printf("0x%8x:0x%8x\r\n", NXMAC_BCN_CNTRL_1_ADDR, nxmac_bcn_cntrl_1_get());
    os_printf("saves dump\r\n");
    for(i = 0; i < (3 * (sizeof(SCTRL_PS_SAVE_VALUES) / 4)); i++)
        os_printf(" %d 0x%x\r\n", i, *((UINT32 *)(&ps_saves) + i));   
}

void sctrl_hw_sleep(UINT32 peri_clk)
{    
    UINT32 reg;
    PS_DEBUG_DOWN_TRIGER;

    if(4 == flash_get_line_mode())
    {
        flash_set_line_mode(2);
    }


    if(power_save_if_rf_sleep())
    {
        reg = REG_READ(ICU_ARM_WAKEUP_EN); 
        reg |= (MAC_ARM_WAKEUP_EN_BIT);
    	REG_WRITE(ICU_ARM_WAKEUP_EN, reg);      
    }
    
    #if CFG_USE_BLE_PS
    if(if_ble_sleep())
    {
        reg = REG_READ(ICU_ARM_WAKEUP_EN);
        reg |= (BLE_ARM_WAKEUP_EN_BIT);
        #if (CFG_SOC_NAME == SOC_BK7231N)
        reg |= (BTDM_ARM_WAKEUP_EN_BIT);
        #endif

        REG_WRITE(ICU_ARM_WAKEUP_EN, reg);
    }
    #endif
    
    PS_DEBUG_DOWN_TRIGER;

#if ((CFG_SOC_NAME == SOC_BK7231) || (CFG_SOC_NAME == SOC_BK7231U) || (CFG_SOC_NAME == SOC_BK7221U))
    REG_WRITE(SCTRL_ROSC_CAL, 0x35);
    REG_WRITE(SCTRL_ROSC_CAL, 0x37);
#endif

    if(sctrl_mcu_ps_info.mcu_use_dco == 0)
    {
        /* MCLK(main clock) select:dco*/
        reg = REG_READ(SCTRL_CONTROL);
        reg &= ~(MCLK_DIV_MASK << MCLK_DIV_POSI);
        REG_WRITE(SCTRL_CONTROL, reg);       
        reg = REG_READ(SCTRL_CONTROL);
        reg &= ~(MCLK_MUX_MASK << MCLK_MUX_POSI);
        REG_WRITE(SCTRL_CONTROL, reg);
        PS_DEBUG_DOWN_TRIGER;
    }

    /*close 32K Rosc calib*/
#if (CFG_SOC_NAME == SOC_BK7231)
    REG_WRITE(SCTRL_ROSC_CAL, 0x36);
#endif
    PS_DEBUG_DOWN_TRIGER;
    /* dpll division reset*/
    reg = REG_READ(SCTRL_CONTROL);
    reg |= DPLL_CLKDIV_RESET_BIT;
    REG_WRITE(SCTRL_CONTROL, reg);
    PS_DEBUG_DOWN_TRIGER;
    /* dpll (480m) & xtal2rf  disable*/
    reg = REG_READ(SCTRL_BLOCK_EN_CFG);
    reg &= ~(BLOCK_EN_WORD_MASK << BLOCK_EN_WORD_POSI);
    reg = reg | (BLOCK_EN_WORD_PWD << BLOCK_EN_WORD_POSI);
    reg &= ~(BLK_EN_DPLL_480M | BLK_EN_XTAL2RF );
    REG_WRITE(SCTRL_BLOCK_EN_CFG, reg);
    PS_DEBUG_DOWN_TRIGER;
    /* center bias power down*/
    reg = sctrl_analog_get(SCTRL_ANALOG_CTRL2);
    reg &= (~(CENTRAL_BAIS_ENABLE_BIT));
    sctrl_analog_set(SCTRL_ANALOG_CTRL2, reg);
    sctrl_mcu_ps_info.hw_sleep = 1;

    while(sctrl_analog_get(SCTRL_ANALOG_CTRL2) & (CENTRAL_BAIS_ENABLE_BIT));

    PS_DEBUG_DOWN_TRIGER;
#if PS_CLOSE_PERI_CLK
    /* close all peri clock*/
    ps_saves[0].peri_clk_cfg= REG_READ(ICU_PERI_CLK_PWD);    
    REG_WRITE(ICU_PERI_CLK_PWD, peri_clk);
#endif

    ps_delay(1);
    /* arm clock disable */
    reg = REG_READ(SCTRL_SLEEP);
    reg &= ~(SLEEP_MODE_MASK << SLEEP_MODE_POSI);
    reg = reg | SLEEP_MODE_CFG_NORMAL_VOL_WORD;
    REG_WRITE(SCTRL_SLEEP, reg); 
    ps_delay(1);//5
}

void sctrl_hw_wakeup()
{
    UINT32 reg;
    
	PS_DEBUG_BCN_TRIGER;   
    /* center bias power on*/ 
    reg = sctrl_analog_get(SCTRL_ANALOG_CTRL2);
    reg |= CENTRAL_BAIS_ENABLE_BIT;
    sctrl_analog_set(SCTRL_ANALOG_CTRL2, reg); 
    
    while((sctrl_analog_get(SCTRL_ANALOG_CTRL2) & CENTRAL_BAIS_ENABLE_BIT)  == 0);
    
    
    /*dpll(480m)  & xtal2rf enable*/
    reg = REG_READ(SCTRL_BLOCK_EN_CFG);
    reg &= ~(BLOCK_EN_WORD_MASK << BLOCK_EN_WORD_POSI);
    reg |= (BLOCK_EN_WORD_PWD << BLOCK_EN_WORD_POSI);
    reg |= ( BLK_EN_DPLL_480M | BLK_EN_XTAL2RF );
    REG_WRITE(SCTRL_BLOCK_EN_CFG, reg);
    ps_delay(10);
    PS_DEBUG_BCN_TRIGER;

    if(sctrl_mcu_ps_info.mcu_use_dco == 0)
    {
        /* MCLK(main clock) select:26M*/
        reg = REG_READ(SCTRL_CONTROL);
        reg &= ~(MCLK_MUX_MASK << MCLK_MUX_POSI);    
        reg |= ((MCLK_FIELD_26M_XTAL & MCLK_MUX_MASK) << MCLK_MUX_POSI);
        REG_WRITE(SCTRL_CONTROL, reg);

        ps_delay(500);
	    PS_DEBUG_BCN_TRIGER;   
        /* dpll division reset release*/
        reg = REG_READ(SCTRL_CONTROL); 
        reg &= ~(DPLL_CLKDIV_RESET_BIT);     
        REG_WRITE(SCTRL_CONTROL, reg); 
      
        /* MCLK(main clock) select:dpll*//* MCLK division*/
        reg = REG_READ(SCTRL_CONTROL);
        reg &= ~(MCLK_DIV_MASK << MCLK_DIV_POSI);
        reg |= ((MCLK_DIV_7 & MCLK_DIV_MASK) << MCLK_DIV_POSI);
        REG_WRITE(SCTRL_CONTROL, reg);

        reg = REG_READ(SCTRL_CONTROL);    
        reg &= ~(MCLK_MUX_MASK << MCLK_MUX_POSI);    
        reg |= ((MCLK_FIELD_DPLL & MCLK_MUX_MASK) << MCLK_MUX_POSI);
        REG_WRITE(SCTRL_CONTROL, reg);
	    PS_DEBUG_BCN_TRIGER;   

    }
    else
    {
        ps_delay(500);
        
        /* dpll division reset release*/
        reg = REG_READ(SCTRL_CONTROL); 
        reg &= ~(DPLL_CLKDIV_RESET_BIT);     
        REG_WRITE(SCTRL_CONTROL, reg); 
	    PS_DEBUG_BCN_TRIGER;   
        
    }

    sctrl_mcu_ps_info.hw_sleep = 0;
    sctrl_cali_dpll(1);
    PS_DEBUG_BCN_TRIGER;
    gpio_ctrl(CMD_GPIO_CLR_DPLL_UNLOOK_INT, NULL);

	#if ((CFG_SOC_NAME == SOC_BK7231) || (CFG_SOC_NAME == SOC_BK7231U) || (CFG_SOC_NAME == SOC_BK7221U))
    /*open 32K Rosc calib*/
    REG_WRITE(SCTRL_ROSC_CAL, 0x35);
    REG_WRITE(SCTRL_ROSC_CAL, 0x37);
	#endif

    if(4 == flash_get_line_mode())
    {
        flash_set_line_mode(4);
    }
	PS_DEBUG_BCN_TRIGER;       
}

UINT8 sctrl_if_rf_sleep(void)
{
    UINT32 value;
    GLOBAL_INT_DECLARATION();
    GLOBAL_INT_DISABLE();
    value =  rf_sleeped;
    GLOBAL_INT_RESTORE();
    return value;
}

void sctrl_rf_sleep(void)
{
    UINT32 reg;
    GLOBAL_INT_DECLARATION();
    GLOBAL_INT_DISABLE();
    if(0 == rf_sleeped)
    {
        /*Disable BK7011*/
        rc_cntl_stat_set(0x0);
        rf_sleeped = 1;
    	PS_DEBUG_CK_TRIGER;
        
        /* Modem AHB clock disable*/
        reg = REG_READ(SCTRL_MODEM_CORE_RESET_PHY_HCLK);
        reg &= ~PHY_HCLK_EN_BIT;
        REG_WRITE(SCTRL_MODEM_CORE_RESET_PHY_HCLK, reg);
        /* Modem Subsystem clock 480m disable*/
        reg = REG_READ(SCTRL_CONTROL);
        REG_WRITE(SCTRL_CONTROL, reg | MODEM_CLK480M_PWD_BIT);
        PS_DEBUG_CK_TRIGER;
    }
    
    GLOBAL_INT_RESTORE();
}

void sctrl_rf_wakeup(void)
{
    UINT32 reg;
    GLOBAL_INT_DECLARATION();
    GLOBAL_INT_DISABLE();
    if(rf_sleeped == 1)
    {
        /* Modem AHB clock enable*/
        reg = REG_READ(SCTRL_MODEM_CORE_RESET_PHY_HCLK);
        REG_WRITE(SCTRL_MODEM_CORE_RESET_PHY_HCLK, reg | PHY_HCLK_EN_BIT);
        
        /* Modem Subsystem clock 480m enable*/
        reg = REG_READ(SCTRL_CONTROL);
        reg &= ~MODEM_CLK480M_PWD_BIT;
        REG_WRITE(SCTRL_CONTROL, reg);

    	/*Enable BK7011:rc_en,ch0_en*/
        rc_cntl_stat_set(0x09);
        PS_DEBUG_UP_TRIGER;

#if (CFG_SOC_NAME == SOC_BK7231N)
		sctrl_fix_dpll_div();
#endif

		phy_wakeup_rf_reinit();

        rf_sleeped = 0;
    }
    GLOBAL_INT_RESTORE();
}


void sctrl_set_rf_sleep(void)
{
    extern UINT32 if_other_mode_rf_sleep(void);

    if(power_save_if_rf_sleep() 
        && if_other_mode_rf_sleep()
#if CFG_USE_BLE_PS
    	&& if_ble_sleep()
#else
#if (CFG_SOC_NAME != SOC_BK7231)
        &&  (!(REG_READ(SCTRL_CONTROL) & BLE_RF_EN_BIT))
#endif
#endif
        )
        {
            sctrl_rf_sleep();
        }
}

#if CFG_USE_STA_PS

void sctrl_sta_rf_sleep(void)
{
    UINT32 reg;
    GLOBAL_INT_DECLARATION();
    GLOBAL_INT_DISABLE();

    if(power_save_if_rf_sleep()
	&& (sta_rf_sleeped == 0)
		)
    {
        PS_DEBUG_CK_TRIGER;

        sctrl_set_rf_sleep();
        /* MAC AHB slave clock disable */
        reg = REG_READ(SCTRL_MODEM_CORE_RESET_PHY_HCLK);
        reg &= ~MAC_HCLK_EN_BIT;
        REG_WRITE(SCTRL_MODEM_CORE_RESET_PHY_HCLK, reg);
        /* Mac Subsystem clock 480m disable*/
        reg = REG_READ(SCTRL_CONTROL);
        REG_WRITE(SCTRL_CONTROL, reg | MAC_CLK480M_PWD_BIT);
        PS_DEBUG_CK_TRIGER;

        sta_rf_sleeped = 1;
    }
    
    GLOBAL_INT_RESTORE();
}
void sctrl_sta_rf_wakeup(void)
{
    UINT32 reg;
    GLOBAL_INT_DECLARATION();
    GLOBAL_INT_DISABLE();
    if( sta_rf_sleeped == 1 )
    {
        PS_DEBUG_UP_TRIGER;
        if(sctrl_mcu_ps_info.hw_sleep == 1)
        {
            //if rf add mcu up meanwhile
            os_printf("err, hw not up\r\n");
        }

        /* MAC AHB slave clock enable*/
        reg = REG_READ(SCTRL_MODEM_CORE_RESET_PHY_HCLK);
        REG_WRITE(SCTRL_MODEM_CORE_RESET_PHY_HCLK, reg | MAC_HCLK_EN_BIT);

        /* Mac Subsystem clock 480m enable*/
        reg = REG_READ(SCTRL_CONTROL);
        reg &= ~MAC_CLK480M_PWD_BIT;
        REG_WRITE(SCTRL_CONTROL, reg);
    	PS_DEBUG_UP_TRIGER;

        sctrl_rf_wakeup();

        phy_wakeup_wifi_reinit();

        sta_rf_sleeped = 0;
    }
    GLOBAL_INT_RESTORE();
}
#endif


#if CFG_USE_MCU_PS
UINT8 sctrl_if_mcu_can_sleep(void)
{
    return ((power_save_if_rf_sleep()) 
#if CFG_USE_BLE_PS
    	&& if_ble_sleep()
#else
#if (CFG_SOC_NAME != SOC_BK7231)
        &&  (!(REG_READ(SCTRL_CONTROL) & BLE_RF_EN_BIT))
#endif
#endif
    	&& sctrl_if_rf_sleep()
        && (sctrl_mcu_ps_info.hw_sleep == 0));
}

void sctrl_mcu_sleep(UINT32 peri_clk)
{
    UINT32 reg;

    if(sctrl_if_mcu_can_sleep())
    {
        reg = REG_READ(ICU_ARM_WAKEUP_EN);

        if(sctrl_mcu_ps_info.first_sleep == 1)
        {
            reg = 0x0;
            sctrl_mcu_ps_info.first_sleep = 0;
        }
#if (CHIP_U_MCU_WKUP_USE_TIMER && (CFG_SOC_NAME != SOC_BK7231))
        reg |= (TIMER_ARM_WAKEUP_EN_BIT | UART2_ARM_WAKEUP_EN_BIT
            | UART1_ARM_WAKEUP_EN_BIT| GPIO_ARM_WAKEUP_EN_BIT
            | PWM_ARM_WAKEUP_EN_BIT);
#else
        reg |= (PWM_ARM_WAKEUP_EN_BIT | UART2_ARM_WAKEUP_EN_BIT
            | UART1_ARM_WAKEUP_EN_BIT| GPIO_ARM_WAKEUP_EN_BIT
            );
#endif

    	REG_WRITE(ICU_ARM_WAKEUP_EN, reg);  

        sctrl_hw_sleep(peri_clk);     
    }
    else
    {
        PS_DEBUG_DOWN_TRIGER;
        delay(1);
#if PS_CLOSE_PERI_CLK
        /* close all peri clock*/
        ps_saves[0].peri_clk_cfg= REG_READ(ICU_PERI_CLK_PWD);        
        REG_WRITE(ICU_PERI_CLK_PWD, peri_clk);
#endif
        
        PS_DEBUG_DOWN_TRIGER;
#if (CFG_SOC_NAME == SOC_BK7231)
        REG_WRITE(SCTRL_ROSC_CAL, 0x35);
        REG_WRITE(SCTRL_ROSC_CAL, 0x37);
#endif
        WFI();              
    }
    delay(5);
}
UINT32 sctrl_mcu_wakeup(void)
{
    UINT32 wkup_type;

    if(sctrl_mcu_ps_info.hw_sleep == 1)
    {
        sctrl_hw_wakeup();

        #if PS_CLOSE_PERI_CLK
        REG_WRITE(ICU_PERI_CLK_PWD, ps_saves[0].peri_clk_cfg);
        #endif
        wkup_type = 1;
    }
    else
    {
    	PS_DEBUG_BCN_TRIGER;   
        delay(2);
    	PS_DEBUG_BCN_TRIGER;   
    
        #if PS_CLOSE_PERI_CLK
        /* recovery periphral unit clock config*/        
        REG_WRITE(ICU_PERI_CLK_PWD, ps_saves[0].peri_clk_cfg);  
        #endif
        wkup_type = 0;
     }
    return wkup_type;
}


void sctrl_mcu_init(void)
{
    UINT32 reg;

    reg = LIMIT_FREQ_MCU_PS_BIT;
    sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_MCLK_LIMIT_FREQ_BIT_SET, &reg);
}

void sctrl_mcu_exit(void)
{
    UINT32 reg;

    reg = LIMIT_FREQ_MCU_PS_BIT;
    sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_MCLK_LIMIT_FREQ_BIT_CLR, &reg);
}
#endif

void sctrl_subsys_power(UINT32 cmd)
{
    UINT32 reg = 0;
    UINT32 reg_val;
    UINT32 reg_word = 0;

    switch(cmd)
    {
    case CMD_SCTRL_DSP_POWERDOWN:
        reg = SCTRL_DSP_PWR;
        reg_word = DSP_PWD;
        break;

    case CMD_SCTRL_DSP_POWERUP:
        reg = SCTRL_DSP_PWR;
        reg_word = DSP_PWU;
        break;

#if (CFG_SOC_NAME != SOC_BK7231N)
    case CMD_SCTRL_USB_POWERDOWN:
        reg = SCTRL_USB_PWR;
        reg_word = USB_PWD;
        break;

    case CMD_SCTRL_USB_POWERUP:
        reg = SCTRL_USB_PWR;
        reg_word = USB_PWU;
        break;
#endif

    case CMD_SCTRL_MAC_POWERDOWN:
        reg = SCTRL_PWR_MAC_MODEM;
        reg_val = REG_READ(SCTRL_PWR_MAC_MODEM);
        reg_val &= ~(MAC_PWD_MASK << MAC_PWD_POSI);
        reg_val |= MAC_PWD << MAC_PWD_POSI;
        reg_word = reg_val;
        break;

    case CMD_SCTRL_MAC_POWERUP:
        reg = SCTRL_PWR_MAC_MODEM;
        reg_val = REG_READ(SCTRL_PWR_MAC_MODEM);
        reg_val &= ~(MAC_PWD_MASK << MAC_PWD_POSI);
        reg_val |= MAC_PWU << MAC_PWD_POSI;
        reg_word = reg_val;
        break;

    case CMD_SCTRL_MODEM_POWERDOWN:
        reg = SCTRL_PWR_MAC_MODEM;
        reg_val = REG_READ(SCTRL_PWR_MAC_MODEM);
        reg_val &= ~(MODEM_PWD_MASK << MODEM_PWD_POSI);
        reg_val |= MODEM_PWD << MODEM_PWD_POSI;
        reg_word = reg_val;
        break;

    case CMD_SCTRL_BLE_POWERDOWN:
        reg = SCTRL_USB_PWR;
        reg_val = REG_READ(SCTRL_USB_PWR);
        reg_val &= ~(BLE_PWD_MASK << BLE_PWD_POSI);
        reg_val |= BLE_PWD << BLE_PWD_POSI;
        reg_word = reg_val;
        break;        

    case CMD_SCTRL_MODEM_POWERUP:
        reg = SCTRL_PWR_MAC_MODEM;
        reg_val = REG_READ(SCTRL_PWR_MAC_MODEM);
        reg_val &= ~(MODEM_PWD_MASK << MODEM_PWD_POSI);
        reg_val |= MODEM_PWU << MODEM_PWD_POSI;
        reg_word = reg_val;
        break;

     case CMD_SCTRL_BLE_POWERUP:
        reg = SCTRL_USB_PWR;
        reg_val = REG_READ(SCTRL_USB_PWR);
        reg_val &= ~(BLE_PWD_MASK << BLE_PWD_POSI);
        reg_val |= BLE_PWU << BLE_PWD_POSI;
        reg_word = reg_val;
        break;         

    default:
        break;
    }

    if(reg)
    {
        REG_WRITE(reg, reg_word);
    }
}

void sctrl_subsys_reset(UINT32 cmd)
{
    UINT32 reg = 0;
    UINT32 reset_word = 0;

    switch(cmd)
    {
    case CMD_SCTRL_MODEM_SUBCHIP_RESET:
        reg = SCTRL_MODEM_SUBCHIP_RESET_REQ;
        reset_word = MODEM_SUBCHIP_RESET_WORD;
        break;

    case CMD_SCTRL_MAC_SUBSYS_RESET:
        reg = SCTRL_MAC_SUBSYS_RESET_REQ;
        reset_word = MAC_SUBSYS_RESET_WORD;
        break;

    case CMD_SCTRL_USB_SUBSYS_RESET:
        reg = SCTRL_USB_SUBSYS_RESET_REQ;
        reset_word = USB_SUBSYS_RESET_WORD;
        break;

    case CMD_SCTRL_DSP_SUBSYS_RESET:
        reg = SCTRL_DSP_SUBSYS_RESET_REQ;
        reset_word = DSP_SUBSYS_RESET_WORD;
        break;

    default:
        break;
    }

    if(reg)
    {
        REG_WRITE(reg, reset_word);
        delay(10);
        REG_WRITE(reg, 0);
    }

    return;
}

#if (CFG_SOC_NAME == SOC_BK7231N)
void sctrl_fix_dpll_div()
{
	volatile INT32   i;
	uint32 reg;
	uint32 cpu_clock;

	GLOBAL_INT_DECLARATION();
	GLOBAL_INT_DISABLE();

	reg = REG_READ(SCTRL_CONTROL);
	cpu_clock = reg & 0xFF;
	reg = (reg & 0xFFFFFF00) | 0x52;
	REG_WRITE(SCTRL_CONTROL, reg);

	for(i = 0; i < 100; i ++);

	REG_WRITE(SCTRL_MODEM_SUBCHIP_RESET_REQ, MODEM_SUBCHIP_RESET_WORD);
	REG_WRITE(SCTRL_CONTROL, REG_READ(SCTRL_CONTROL) | (1 << 14));

	for(i = 0; i < 100; i ++);

	REG_WRITE(SCTRL_MODEM_SUBCHIP_RESET_REQ, 0);
	REG_WRITE(SCTRL_CONTROL, REG_READ(SCTRL_CONTROL) & ~(1 << 14));

	for(i = 0; i < 100; i ++);

	reg = REG_READ(SCTRL_CONTROL);
	reg = (reg & 0xFFFFFF00) | cpu_clock;
	REG_WRITE(SCTRL_CONTROL, reg);

	for(i = 0; i < 100; i ++);

	GLOBAL_INT_RESTORE();
}

void sctrl_mdm_reset(void)
{
    volatile INT32 i;
    GLOBAL_INT_DECLARATION();

    os_printf("sctrl_mdm_reset\r\n");

    // Disable the interrupts
    GLOBAL_INT_DISABLE();

    if (1)
    {
        /* MAC reset */
        REG_WRITE(SCTRL_MODEM_SUBCHIP_RESET_REQ, MODEM_SUBCHIP_RESET_WORD);
        REG_WRITE(SCTRL_CONTROL, REG_READ(SCTRL_CONTROL) | DPLL_CLKDIV_RESET_BIT);

        for(i = 0; i < 100; i++);

        REG_WRITE(SCTRL_MODEM_SUBCHIP_RESET_REQ, 0);
        REG_WRITE(SCTRL_CONTROL, REG_READ(SCTRL_CONTROL) & ~DPLL_CLKDIV_RESET_BIT);

        for(i = 0; i < 100; i++);
    }
    else
    {
        /* Modem pwd */
        sctrl_ctrl(CMD_SCTRL_MODEM_POWERDOWN, NULL);
        REG_WRITE(SCTRL_CONTROL, REG_READ(SCTRL_CONTROL) | DPLL_CLKDIV_RESET_BIT);

        for(i = 0; i < 100; i++);

        /* Modem pwu */
        sctrl_ctrl(CMD_SCTRL_MODEM_POWERUP, NULL);
        REG_WRITE(SCTRL_CONTROL, REG_READ(SCTRL_CONTROL) & ~DPLL_CLKDIV_RESET_BIT);

        for(i = 0; i < 100; i++);
    }

    phy_wakeup_rf_reinit();
    phy_wakeup_wifi_reinit();

    // Restore the interrupts
    GLOBAL_INT_RESTORE();
}
#endif

#if CFG_USE_LOW_IDLE_PS
void sctrl_enter_rtos_idle_sleep(UINT32 peri_clk)
{
    DD_HANDLE flash_hdl;
    UINT32 status;
    UINT32 reg;
    int     i;

	uart_wait_tx_over();
	
    /* close all peri int*/
    ps_saves[1].int_enable_cfg = REG_READ(ICU_INTERRUPT_ENABLE);
    REG_WRITE(ICU_INTERRUPT_ENABLE, 0);
    /* MAC AHB slave clock disable */
    reg = REG_READ(SCTRL_MODEM_CORE_RESET_PHY_HCLK);
    reg &= ~MAC_HCLK_EN_BIT;
    REG_WRITE(SCTRL_MODEM_CORE_RESET_PHY_HCLK, reg);

    /* Mac Subsystem clock 480m disable*/
    reg = REG_READ(SCTRL_CONTROL);
    REG_WRITE(SCTRL_CONTROL, reg | MAC_CLK480M_PWD_BIT);

    /* Modem pwd*/
    REG_WRITE(SCTRL_PWR_MAC_MODEM, MODEM_PWD); 
	
    /* Modem AHB clock disable*/
    reg = REG_READ(SCTRL_MODEM_CORE_RESET_PHY_HCLK);
    reg &= ~PHY_HCLK_EN_BIT;
    REG_WRITE(SCTRL_MODEM_CORE_RESET_PHY_HCLK, reg);
    
    /* Modem Subsystem clock 480m disable*/
    reg = REG_READ(SCTRL_CONTROL);
    REG_WRITE(SCTRL_CONTROL, reg | MODEM_CLK480M_PWD_BIT);
	
    /* Flash 26MHz clock select dco clock*/
    flash_hdl = ddev_open(FLASH_DEV_NAME, &status, 0);
    ASSERT(DD_HANDLE_UNVALID != flash_hdl);
    ddev_control(flash_hdl, CMD_FLASH_SET_DCO, 0);
    
    /* MCLK(main clock) select:dco*/ /* MCLK division*/
    reg = REG_READ(SCTRL_CONTROL);
    reg &= ~(MCLK_MUX_MASK << MCLK_MUX_POSI);
    reg &= ~(MCLK_DIV_MASK << MCLK_DIV_POSI);
    REG_WRITE(SCTRL_CONTROL, reg);    

    /*close 32K Rosc calib*/
    REG_WRITE(SCTRL_ROSC_CAL, 0x36);
	
    /* dpll division reset*/
    reg = REG_READ(SCTRL_CONTROL); 
    reg |= (0x1<<14);     
    REG_WRITE(SCTRL_CONTROL, reg); 

    /* dpll (480m) & 26m xtal & xtal2rf & Vsys disable*/
    reg = REG_READ(SCTRL_BLOCK_EN_CFG);
    reg &= ~(BLOCK_EN_WORD_MASK << BLOCK_EN_WORD_POSI);
    reg = reg | (BLOCK_EN_WORD_PWD << BLOCK_EN_WORD_POSI);
    reg &= ~(BLK_EN_DPLL_480M | BLK_EN_26M_XTAL | BLK_EN_XTAL2RF | BLK_EN_ANALOG_SYS_LDO);
    REG_WRITE(SCTRL_BLOCK_EN_CFG, reg); 
    reg = REG_READ(SCTRL_BLOCK_EN_MUX);
    reg = reg | (0x1 << 4);
    REG_WRITE(SCTRL_BLOCK_EN_MUX, reg);
	
    /* close all peri clock*/
    ps_saves[1].peri_clk_cfg= REG_READ(ICU_PERI_CLK_PWD);
    REG_WRITE(ICU_PERI_CLK_PWD, peri_clk);

    /* vdddig decrease to 0.9v*/
    REG_WRITE(SCTRL_DIGTAL_VDD, 0x20);
	
    /* set all gpio to high-z state*/
    for(i =2; i < 32; i ++)
    {
        ps_saves_gpio_cfgs[i] = REG_READ(0x00802800+i*4);
        REG_WRITE(0x00802800+i*4, 0x2c);
    }
    
    /* arm clock disable */
    reg = REG_READ(SCTRL_SLEEP);
    reg &= ~(SLEEP_MODE_MASK << SLEEP_MODE_POSI);
    reg = reg | SLEEP_MODE_CFG_NORMAL_VOL_WORD;
    REG_WRITE(SCTRL_SLEEP, reg);  
    delay(5);
}

void sctrl_exit_rtos_idle_sleep(void)
{
    UINT32 reg;
    int    i;

    /* vdddig increase to 1.2v*/
    REG_WRITE(SCTRL_DIGTAL_VDD, 0x40);
    PS_DEBUG_UP_OUT;

    /*dpll(480m) & 26m xtal & xtal2rf enable*/
    reg = REG_READ(SCTRL_BLOCK_EN_MUX);
    reg &= ~(0x1 << 4);
    REG_WRITE(SCTRL_BLOCK_EN_MUX, reg);
    reg = REG_READ(SCTRL_BLOCK_EN_CFG);
    reg &= ~(BLOCK_EN_WORD_MASK << BLOCK_EN_WORD_POSI);
    reg |= (BLOCK_EN_WORD_PWD << BLOCK_EN_WORD_POSI);
    reg |= (BLK_EN_26M_XTAL | BLK_EN_DPLL_480M | BLK_EN_XTAL2RF | BLK_EN_ANALOG_SYS_LDO);
    REG_WRITE(SCTRL_BLOCK_EN_CFG, reg); 	
	
    delay(1);  //delay 1.1ms for 26MHz DCO clock. need change for other dco clock frequency
    sctrl_cali_dpll(0);
    /* dpll division reset release*/
    reg = REG_READ(SCTRL_CONTROL); 
    reg &= ~(0x1<<14);     
    REG_WRITE(SCTRL_CONTROL, reg); 
    
    /* MCLK(main clock) select:dpll*//* MCLK division*/
    reg = REG_READ(SCTRL_CONTROL);
    reg &= ~(MCLK_DIV_MASK << MCLK_DIV_POSI);
    reg &= ~(MCLK_MUX_MASK << MCLK_MUX_POSI);    
    reg |= ((MCLK_DIV_7 & MCLK_DIV_MASK) << MCLK_DIV_POSI);
    reg |= ((MCLK_FIELD_DPLL & MCLK_MUX_MASK) << MCLK_MUX_POSI);
    REG_WRITE(SCTRL_CONTROL, reg);

    /* Flash clock select dpll clock*/ 
    {
        DD_HANDLE flash_hdl;
        UINT32 status;
        flash_hdl = ddev_open(FLASH_DEV_NAME, &status, 0);
        ASSERT(DD_HANDLE_UNVALID != flash_hdl);
        ddev_control(flash_hdl, CMD_FLASH_SET_DPLL, 0);
    }

    /*open 32K Rosc calib*/
    REG_WRITE(SCTRL_ROSC_CAL, 0x37);
	
    /* MAC AHB slave clock enable*/
    reg = REG_READ(SCTRL_MODEM_CORE_RESET_PHY_HCLK);
    REG_WRITE(SCTRL_MODEM_CORE_RESET_PHY_HCLK, reg | MAC_HCLK_EN_BIT);
    
    /* Mac Subsystem clock 480m enable*/
    reg = REG_READ(SCTRL_CONTROL);
    reg &= ~MAC_CLK480M_PWD_BIT;
    REG_WRITE(SCTRL_CONTROL, reg);

    /* Modem AHB clock enable*/
    reg = REG_READ(SCTRL_MODEM_CORE_RESET_PHY_HCLK);
    REG_WRITE(SCTRL_MODEM_CORE_RESET_PHY_HCLK, reg | PHY_HCLK_EN_BIT);
    
    /* Modem Subsystem clock 480m enable*/
    reg = REG_READ(SCTRL_CONTROL);
    reg &= ~MODEM_CLK480M_PWD_BIT;
    REG_WRITE(SCTRL_CONTROL, reg);

    /* Modem pwd*/
    reg = REG_READ(SCTRL_PWR_MAC_MODEM);
    reg &= ~(MODEM_PWD_MASK << MODEM_PWD_POSI);
    reg = reg | (0 << MODEM_PWD_POSI);
    REG_WRITE(SCTRL_PWR_MAC_MODEM, reg);  
	
    /* recovery periphral unit clock config*/
    REG_WRITE(ICU_PERI_CLK_PWD, ps_saves[1].peri_clk_cfg);

    /* recovery periphral unit int config*/
    REG_WRITE(ICU_INTERRUPT_ENABLE, ps_saves[1].int_enable_cfg);  
    /* recovery gpio configuration*/
    for(i = 2; i < 32; i ++)
    {
        if(i!=10&&i!=11&&i!=14&&i!=15&&i!=17)
        REG_WRITE(0x00802800+i*4, ps_saves_gpio_cfgs[i]);
        else if(i!=16 && i!=10)
        REG_WRITE(0x00802800+i*4, 0x0);
    }
    
    os_printf("idle wake up!\r\n");    
}

#endif

#if CFG_USE_DEEP_PS
void sctrl_enter_rtos_deep_sleep(PS_DEEP_CTRL_PARAM *deep_param)
{
    DD_HANDLE flash_hdl;
    UINT32 status;
    UINT32 param;
    UINT32 reg;
    UINT32 i;
	
	uart_wait_tx_over();
	
	/* close all peri clock*/
	REG_WRITE(ICU_PERI_CLK_PWD, 0xfffff);  //  icu: 0x2;
      
#if CFG_USE_UART2
	uart2_exit();
#endif
#if CFG_USE_UART1
	uart1_exit();
#endif

#if (CFG_SOC_NAME == SOC_BK7231U) || (SOC_BK7231N == CFG_SOC_NAME)
    reg = REG_READ(SCTRL_LOW_PWR_CLK);
    reg &=~(LPO_CLK_MUX_MASK);
    reg |=(LPO_SRC_ROSC << LPO_CLK_MUX_POSI);
    REG_WRITE(SCTRL_LOW_PWR_CLK, reg);

    REG_WRITE(SCTRL_ROSC_CAL, 0x75);
    REG_WRITE(SCTRL_ROSC_CAL, 0x77);
#else
	/*ana_reg set*/
    REG_WRITE(SCTRL_ANALOG_CTRL0, 0x7819a59b);     
    REG_WRITE(SCTRL_ANALOG_CTRL1, 0x7819a59b);     
    REG_WRITE(SCTRL_ANALOG_CTRL2, 0x84036080);     
    REG_WRITE(SCTRL_ANALOG_CTRL3, 0x180004a0);     
    REG_WRITE(SCTRL_ANALOG_CTRL4, 0x84200e52);     
    REG_WRITE(SCTRL_ANALOG_CTRL5, 0x3b13b13b);     
#if (CFG_SOC_NAME != SOC_BK7231)
    REG_WRITE(SCTRL_ANALOG_CTRL6, 0xb09350);     
#endif
#if (CFG_SOC_NAME == SOC_BK7221U)
    REG_WRITE(SCTRL_ANALOG_CTRL7, 0x441a7f0);     
    REG_WRITE(SCTRL_ANALOG_CTRL8, 0x3b187c);     
    REG_WRITE(SCTRL_ANALOG_CTRL9, 0x82204007);     
    REG_WRITE(SCTRL_ANALOG_CTRL10, 0x80801432);
#endif
#endif
    ps_delay(10);	
	/*clear int*/
	REG_WRITE(ICU_INTERRUPT_ENABLE, 0);

    extern void gpio_ops_disable_filter(void);
    gpio_ops_disable_filter();
    
	/*  disable gpio0~31*/
    REG_WRITE(SCTRL_GPIO_WAKEUP_EN,0x0);  //sys_ctrl : 0x48;
    REG_WRITE(SCTRL_GPIO_WAKEUP_INT_STATUS,0xFFFFFFFF);  //sys_ctrl : 0x4a; 
    
#if (CFG_SOC_NAME != SOC_BK7231N)
	/*	disable gpio32~39*/
	REG_WRITE(SCTRL_GPIO_WAKEUP_EN1,0x0);  //sys_ctrl : 0x51;
	REG_WRITE(SCTRL_GPIO_WAKEUP_INT_STATUS1,0xFF);  //sys_ctrl : 0x53; 
#endif

    REG_WRITE(SCTRL_BLOCK_EN_MUX, 0x0);   //sys_ctrl : 0x4F;
    
    /* ROSC_TIMER_int_clear*/
    reg = REG_READ(SCTRL_ROSC_TIMER);
   	reg = reg| ROSC_TIMER_INT_STATUS_BIT ;
    REG_WRITE(SCTRL_ROSC_TIMER,reg);      //sys_ctrl : 0x47;
    
	/*ROSC_TIMER close */
	reg = REG_READ(SCTRL_ROSC_TIMER);
	reg = reg & (~ROSC_TIMER_ENABLE_BIT);																   //'C'
	REG_WRITE(SCTRL_ROSC_TIMER,reg); 
	
    reg = REG_READ(SCTRL_LOW_PWR_CLK);
    reg &=~(LPO_CLK_MUX_MASK);
    if(deep_param->lpo_32k_src == LPO_SELECT_32K_XTAL)
    {
        reg |=(LPO_SRC_32K_XTAL << LPO_CLK_MUX_POSI);
    }
    else
    {
        reg |=(LPO_SRC_ROSC << LPO_CLK_MUX_POSI);
    }
    
    REG_WRITE(SCTRL_LOW_PWR_CLK, reg);    //sys_ctrl : 0x40;

    /* close all peri int*/
	//    REG_WRITE(ICU_INTERRUPT_ENABLE, 0);

    /* MAC pwd*/
    REG_WRITE(SCTRL_PWR_MAC_MODEM, MAC_PWD << MAC_PWD_POSI);   //sys_ctrl : 0x43;
    
    /* MAC AHB slave clock disable */
    reg = REG_READ(SCTRL_MODEM_CORE_RESET_PHY_HCLK);
    reg &= ~MAC_HCLK_EN_BIT;
    REG_WRITE(SCTRL_MODEM_CORE_RESET_PHY_HCLK, reg);

    /* Mac Subsystem clock 480m disable*/
    reg = REG_READ(SCTRL_CONTROL);
    REG_WRITE(SCTRL_CONTROL, reg | MAC_CLK480M_PWD_BIT);

    /* Modem pwd*/
    REG_WRITE(SCTRL_PWR_MAC_MODEM, MODEM_PWD << MODEM_PWD_POSI); 
	
    /* Modem AHB clock disable*/
    reg = REG_READ(SCTRL_MODEM_CORE_RESET_PHY_HCLK);
    reg &= ~PHY_HCLK_EN_BIT;
    REG_WRITE(SCTRL_MODEM_CORE_RESET_PHY_HCLK, reg);
    
    /* Modem Subsystem clock 480m disable*/
    reg = REG_READ(SCTRL_CONTROL);
    REG_WRITE(SCTRL_CONTROL, reg | MODEM_CLK480M_PWD_BIT);
	
    /* Flash 26MHz clock select dco clock*/
    flash_hdl = ddev_open(FLASH_DEV_NAME, &status, 0);
    ASSERT(DD_HANDLE_UNVALID != flash_hdl);
    ddev_control(flash_hdl, CMD_FLASH_SET_DCO, 0);
    
    /* MCLK(main clock) select:dco*/ /* MCLK division*/
    reg = REG_READ(SCTRL_CONTROL);
    reg &= ~(MCLK_MUX_MASK << MCLK_MUX_POSI);
    reg &= ~(MCLK_DIV_MASK << MCLK_DIV_POSI);
    REG_WRITE(SCTRL_CONTROL, reg);                                     //0x02

    if(deep_param->lpo_32k_src == LPO_SELECT_32K_XTAL)
    {
        reg = REG_READ(SCTRL_CONTROL);
        reg =((reg & (~0xF0)) | (0<<4));
        reg =((reg & (~0x03)) | (0<<MCLK_MUX_POSI));
        reg =((reg & (~0x100)) | FLASH_26M_MUX_BIT);
        REG_WRITE(SCTRL_CONTROL,reg); //sys_ctrl : 0x02;
    }

    ps_delay(10);

    reg = 0x0;
   	reg = (reg &(~(BLOCK_EN_WORD_MASK << 20))&(~(0x7FFFUL<<5)) &(~(0x01UL<<1)));
   	reg = (reg |(BLOCK_EN_WORD_PWD<< 20 )|BLK_EN_FLASH|BLK_EN_ROSC32K|BLK_EN_DIGITAL_CORE|BLK_EN_ANALOG_SYS_LDO);
    if(deep_param->lpo_32k_src == LPO_SELECT_32K_XTAL)
    {
        reg = (reg |BLK_EN_32K_XTAL|BLK_EN_26M_XTAL);
    }
    REG_WRITE(SCTRL_BLOCK_EN_CFG, reg);                  //sys_ctrl : 0x4B;                   //'E'

#if (CFG_SOC_NAME != SOC_BK7231U) && (SOC_BK7231N != CFG_SOC_NAME)
    reg = REG_READ(SCTRL_ROSC_CAL);                           //ROSC Calibration disable
    reg =(reg  & (~0x01));
    REG_WRITE(SCTRL_ROSC_CAL, reg);   
#endif

    for(i=0; i<GPIONUM; i++)
    {
#if (CFG_SOC_NAME == SOC_BK7231N)
        if(((i > GPIO1) && (i < GPIO6)) 
            || ((i > GPIO11) && (i < GPIO14)) 
            || ((i > GPIO17) && (i < GPIO20)) 
            || ((i > GPIO24) && (i < GPIO26)) 
            || ((i > GPIO26) && (i < GPIO28)))
        {
            continue;
        }
#endif
        if(((i < BITS_INT)&&(deep_param->gpio_stay_lo_map & (0x01UL << i)))
        ||((i >= BITS_INT)&&(deep_param->gpio_stay_hi_map & (0x01UL << (i - BITS_INT)))) )
        {
            continue;
        }
        param = GPIO_CFG_PARAM(i, GMODE_DEEP_PS);	/*set gpio 0~39 as high impendance*/
        sddev_control(GPIO_DEV_NAME, CMD_GPIO_CFG, &param); 
    }

    if (((deep_param->wake_up_way & PS_DEEP_WAKEUP_RTC))
        && (deep_param->sleep_time!= 0xffffffff))
    {
	/*ROSC_TIMER  init*/
#if (CFG_SOC_NAME != SOC_BK7231)
        reg = (deep_param->sleep_time >> 16)& 0xffff;                                          //'A'
        REG_WRITE(SCTRL_ROSC_TIMER_H,reg);
#endif

        reg = REG_READ(SCTRL_ROSC_TIMER);
        reg |= ROSC_TIMER_INT_STATUS_BIT;                                                                   //'C'
        REG_WRITE(SCTRL_ROSC_TIMER,reg);  //sys_ctrl : 0x47;
		
        reg = REG_READ(SCTRL_ROSC_TIMER);
        reg &= ~(ROSC_TIMER_PERIOD_MASK << ROSC_TIMER_PERIOD_POSI);
        reg |= ((deep_param->sleep_time & ROSC_TIMER_PERIOD_MASK) << ROSC_TIMER_PERIOD_POSI);
        REG_WRITE(SCTRL_ROSC_TIMER,reg);   //sys_ctrl : 0x47;                         //'D'
		
        reg = REG_READ(SCTRL_ROSC_TIMER);
        reg |= ROSC_TIMER_ENABLE_BIT;                                                                              
        REG_WRITE(SCTRL_ROSC_TIMER,reg);  //sys_ctrl : 0x47;                             //'B'

        if(deep_param->lpo_32k_src == LPO_SELECT_32K_XTAL)
        {
    		REG_WRITE(SCTRL_CONTROL, 0x330100);
            REG_WRITE(SCTRL_BLOCK_EN_CFG, (0x15D|(0xA5C<<20)));
            REG_WRITE(SCTRL_ROSC_CAL, 0x30);
            REG_WRITE(SCTRL_LOW_PWR_CLK, 0x01);
            REG_WRITE(SCTRL_MODEM_CORE_RESET_PHY_HCLK, 0x03);
            REG_WRITE(SCTRL_CLK_GATING, 0x1ff);
        }
    }

    if ((deep_param->wake_up_way & PS_DEEP_WAKEUP_GPIO))
    {
        for (i = 0; i < BITS_INT; i++)
        {
#if(BITS_INT > GPIONUM)
            if(i >= GPIONUM)
            {
                break;
            }
#endif

#if (CFG_SOC_NAME == SOC_BK7231N)
            if(((i > GPIO1) && (i < GPIO6)) 
            || ((i > GPIO11) && (i < GPIO14)) 
            || ((i > GPIO17) && (i < GPIO20)) 
            || ((i > GPIO24) && (i < GPIO26)) 
            || ((i > GPIO26) && (i < GPIO28)))
            {
                continue;
            }
#endif
            if (deep_param->gpio_index_map & (0x01UL << i))			/*set gpio 0~31 mode*/
            {
                if( deep_param->gpio_edge_map & (0x01UL << i))      //0:high,1:low.
                {
                    param = GPIO_CFG_PARAM(i, GMODE_INPUT_PULLUP);
                    sddev_control(GPIO_DEV_NAME, CMD_GPIO_CFG, &param);
                    if(0x1 != (UINT32)gpio_ctrl( CMD_GPIO_INPUT, &i))
                    {   /*check gpio really input value,to correct wrong edge setting*/
                        param = GPIO_CFG_PARAM(i, GMODE_INPUT);
                        sddev_control(GPIO_DEV_NAME, CMD_GPIO_CFG, &param);
                        deep_param->gpio_edge_map &= ~(0x01UL << i);
                    }
                }
                else
                {
                    param = GPIO_CFG_PARAM(i, GMODE_INPUT_PULLDOWN);
                    sddev_control(GPIO_DEV_NAME, CMD_GPIO_CFG, &param);
                    if(0x0 != (UINT32)gpio_ctrl( CMD_GPIO_INPUT, &i))
                    {   /*check gpio really input value,to correct wrong edge setting*/
                        param = GPIO_CFG_PARAM(i, GMODE_INPUT);
                        sddev_control(GPIO_DEV_NAME, CMD_GPIO_CFG, &param);
                        deep_param->gpio_edge_map |= (0x01UL << i);
                    }
                }
            }
        }

#if (CFG_SOC_NAME != SOC_BK7231N)
        for (i = 0; i < (GPIONUM - BITS_INT); i++)
        {  
            if (deep_param->gpio_last_index_map & (0x01UL << i))				/*set gpio 32~39 mode*/
            {	
                if( deep_param->gpio_last_edge_map  & (0x01UL << i))
                {
                    param = GPIO_CFG_PARAM(i + BITS_INT, GMODE_INPUT_PULLUP);
                    sddev_control(GPIO_DEV_NAME, CMD_GPIO_CFG, &param); 
                    reg = i + BITS_INT;
                    if(0x1 != (UINT32)gpio_ctrl( CMD_GPIO_INPUT, &reg))
                    {   /*check gpio really input value,to correct wrong edge setting*/
                        param = GPIO_CFG_PARAM(i + BITS_INT, GMODE_INPUT);
                        sddev_control(GPIO_DEV_NAME, CMD_GPIO_CFG, &param);
                        deep_param->gpio_last_edge_map &= ~(0x01UL << i);
                    }
                }
                else
                {
                    param = GPIO_CFG_PARAM(i + BITS_INT, GMODE_INPUT_PULLDOWN);
                    sddev_control(GPIO_DEV_NAME, CMD_GPIO_CFG, &param);  
                    reg = i + BITS_INT;
                    if(0x0 != (UINT32)gpio_ctrl( CMD_GPIO_INPUT, &reg))
                    {   /*check gpio really input value,to correct wrong edge setting*/
                        param = GPIO_CFG_PARAM(i + BITS_INT, GMODE_INPUT);
                        sddev_control(GPIO_DEV_NAME, CMD_GPIO_CFG, &param);
                        deep_param->gpio_last_edge_map |= (0x01UL << i);
                    }
                }
            }			
        }
#endif
		/* set gpio 0~31 mode*/
        reg = 0xFFFFFFFF;
        REG_WRITE(SCTRL_GPIO_WAKEUP_INT_STATUS,reg);        
        reg = deep_param->gpio_edge_map;
        REG_WRITE(SCTRL_GPIO_WAKEUP_TYPE,reg);
        reg = deep_param->gpio_index_map;
        REG_WRITE(SCTRL_GPIO_WAKEUP_EN,reg); 
		
		/* set gpio 31~32 mode*/
#if (CFG_SOC_NAME != SOC_BK7231N)
        reg = 0xFF;
        REG_WRITE(SCTRL_GPIO_WAKEUP_INT_STATUS1,reg);        

        reg = deep_param->gpio_last_edge_map;
        REG_WRITE(SCTRL_GPIO_WAKEUP_TYPE1,reg);

        reg = deep_param->gpio_last_index_map;
        REG_WRITE(SCTRL_GPIO_WAKEUP_EN1,reg);
#else
        reg = 0xFFFFFFFF;
        REG_WRITE(SCTRL_GPIO_WAKEUP_TYPE_SELECT,reg);
#endif
    }	

#if (CFG_SOC_NAME != SOC_BK7231N)
    REG_WRITE(SCTRL_USB_PLUG_WAKEUP,USB_PLUG_IN_INT_BIT|USB_PLUG_OUT_INT_BIT);
    if(deep_param->wake_up_way & PS_DEEP_WAKEUP_USB)
    {
        REG_WRITE(SCTRL_USB_PLUG_WAKEUP,USB_PLUG_IN_EN_BIT|USB_PLUG_OUT_EN_BIT);
    }
#endif
   
#ifdef BK_DEEP_SLEEP_DEBUG
	BK_DEEP_SLEEP_PRT("SCTRL_CONTROL=0x%08X\r\n", REG_READ(SCTRL_CONTROL)); 
	BK_DEEP_SLEEP_PRT("SCTRL_SLEEP=0x%08X\r\n", REG_READ(SCTRL_SLEEP)); 
	BK_DEEP_SLEEP_PRT("SCTRL_ROSC_TIMER=0x%08X\r\n", REG_READ(SCTRL_ROSC_TIMER));
	BK_DEEP_SLEEP_PRT("SCTRL_BLOCK_EN_CFG=0x%08X\r\n", REG_READ(SCTRL_BLOCK_EN_CFG));
	BK_DEEP_SLEEP_PRT("SCTRL_ROSC_CAL=0x%08X\r\n", REG_READ(SCTRL_ROSC_CAL));
	BK_DEEP_SLEEP_PRT("SCTRL_BLOCK_EN_MUX=0x%08X\r\n", REG_READ(SCTRL_BLOCK_EN_MUX));
	BK_DEEP_SLEEP_PRT("SCTRL_LOW_PWR_CLK=0x%08X\r\n", REG_READ(SCTRL_LOW_PWR_CLK));
	BK_DEEP_SLEEP_PRT("SCTRL_PWR_MAC_MODEM=0x%08X\r\n", REG_READ(SCTRL_PWR_MAC_MODEM));
	BK_DEEP_SLEEP_PRT("SCTRL_MODEM_CORE_RESET_PHY_HCLK=0x%08X\r\n", REG_READ(SCTRL_MODEM_CORE_RESET_PHY_HCLK));
	BK_DEEP_SLEEP_PRT("SCTRL_CLK_GATING=0x%08X\r\n", REG_READ(SCTRL_CLK_GATING));
	BK_DEEP_SLEEP_PRT("SCTRL_GPIO_WAKEUP_INT_STATUS=0x%08X\r\n", REG_READ(SCTRL_GPIO_WAKEUP_INT_STATUS));
	BK_DEEP_SLEEP_PRT("SCTRL_GPIO_WAKEUP_TYPE=0x%08X\r\n", REG_READ(SCTRL_GPIO_WAKEUP_TYPE));
	BK_DEEP_SLEEP_PRT("SCTRL_GPIO_WAKEUP_EN=0x%08X\r\n", REG_READ(SCTRL_GPIO_WAKEUP_EN));
	BK_DEEP_SLEEP_PRT("SCTRL_GPIO_WAKEUP_INT_STATUS1=0x%08X\r\n", REG_READ(SCTRL_GPIO_WAKEUP_INT_STATUS1));
	BK_DEEP_SLEEP_PRT("SCTRL_GPIO_WAKEUP_TYPE1=0x%08X\r\n", REG_READ(SCTRL_GPIO_WAKEUP_TYPE1));
	BK_DEEP_SLEEP_PRT("SCTRL_GPIO_WAKEUP_EN1=0x%08X\r\n", REG_READ(SCTRL_GPIO_WAKEUP_EN1));	
#endif      

    /* enter deep_sleep mode */
    reg = REG_READ(SCTRL_SLEEP);
    reg &= ~(SLEEP_MODE_MASK << SLEEP_MODE_POSI);
    reg = reg | SLEEP_MODE_CFG_DEEP_WORD;
    REG_WRITE(SCTRL_SLEEP, reg);  
  
    delay(5);
}

RESET_SOURCE_STATUS sctrl_get_deep_sleep_wake_soure(void)
{
    RESET_SOURCE_STATUS waked_source = 0;

    if(REG_READ(SCTRL_ROSC_TIMER) & ROSC_TIMER_INT_STATUS_BIT)
    {
        waked_source = RESET_SOURCE_DEEPPS_RTC;
    }
    else if(REG_READ(SCTRL_GPIO_WAKEUP_INT_STATUS)
#if (CFG_SOC_NAME != SOC_BK7231N)
    || REG_READ(SCTRL_GPIO_WAKEUP_INT_STATUS1)
#endif
    )
    {
        waked_source = RESET_SOURCE_DEEPPS_GPIO;
    }

    return waked_source;
}


#endif

#if (CFG_SOC_NAME != SOC_BK7231)
static int sctrl_read_efuse(void *param)
{
    UINT32 reg, ret = -1;
    EFUSE_OPER_PTR efuse;
    efuse = (EFUSE_OPER_PTR)param;
    
    if(efuse) {
        reg = REG_READ(SCTRL_EFUSE_CTRL);
        reg &= ~(EFUSE_OPER_ADDR_MASK << EFUSE_OPER_ADDR_POSI);
        reg &= ~(EFUSE_OPER_DIR);            
      
        reg |= ((efuse->addr & EFUSE_OPER_ADDR_MASK) << EFUSE_OPER_ADDR_POSI);
        reg |= (EFUSE_OPER_EN);
        REG_WRITE(SCTRL_EFUSE_CTRL, reg);

        do {
            reg = REG_READ(SCTRL_EFUSE_CTRL);
        }while(reg & EFUSE_OPER_EN);

        reg = REG_READ(SCTRL_EFUSE_OPTR);
        if(reg & EFUSE_OPER_RD_DATA_VALID) {
            efuse->data = ((reg >> EFUSE_OPER_RD_DATA_POSI) & EFUSE_OPER_RD_DATA_MASK);
            ret = 0;
        } else {
            efuse->data = 0xFF;
        }
    }
    return ret;
}

static int sctrl_write_efuse(void *param)
{
    UINT32 reg, ret = -1;
    EFUSE_OPER_ST *efuse, efuse_bak;
    
    efuse = (EFUSE_OPER_PTR)param;
    if(efuse) {
        efuse_bak.addr = efuse->addr;
        efuse_bak.data = efuse->data;        
        if(sctrl_read_efuse(&efuse_bak) == 0) {
             //read before write, ensure this byte no wrote
             if(EFUSE_INIT_VAL != efuse_bak.data)
                return -1; 
        }      

        // enable vdd2.5v first
        reg = REG_READ(SCTRL_CONTROL);
        reg |= EFUSE_VDD25_EN;
        REG_WRITE(SCTRL_CONTROL, reg);
        
        reg = REG_READ(SCTRL_EFUSE_CTRL);
        reg &= ~(EFUSE_OPER_ADDR_MASK << EFUSE_OPER_ADDR_POSI);
        reg &= ~(EFUSE_OPER_WR_DATA_MASK << EFUSE_OPER_WR_DATA_POSI);
        
        reg |= EFUSE_OPER_DIR;
        reg |= ((efuse->addr & EFUSE_OPER_ADDR_MASK) << EFUSE_OPER_ADDR_POSI);            
        reg |= ((efuse->data & EFUSE_OPER_WR_DATA_MASK) << EFUSE_OPER_WR_DATA_POSI);
        reg |= EFUSE_OPER_EN;
        REG_WRITE(SCTRL_EFUSE_CTRL, reg);

        do {
            reg = REG_READ(SCTRL_EFUSE_CTRL);
        }while(reg & EFUSE_OPER_EN);

        // disable vdd2.5v at last
        reg = REG_READ(SCTRL_CONTROL);
        reg &= ~EFUSE_VDD25_EN;
        REG_WRITE(SCTRL_CONTROL, reg);

        // check, so read
        reg = efuse->data;
        efuse->data = 0;
        if(sctrl_read_efuse(param) == 0) {
            if(((UINT8)reg) == efuse->data)
               ret = 0; 
        }        
    }
    return ret;
}
#endif // (CFG_SOC_NAME != SOC_BK7231)

UINT32 sctrl_ctrl(UINT32 cmd, void *param)
{
    UINT32 ret;
    UINT32 reg;
	GLOBAL_INT_DECLARATION();

    ret = SCTRL_SUCCESS;
	GLOBAL_INT_DISABLE();
    switch(cmd)
    {
#if PS_SUPPORT_MANUAL_SLEEP
    case CMD_SCTRL_NORMAL_SLEEP:
        sctrl_rf_sleep();
        sctrl_hw_sleep(*(UINT32 *)param);
        break;

    case CMD_SCTRL_NORMAL_WAKEUP:
        sctrl_hw_wakeup();
        delay(50);
        sctrl_rf_wakeup();
        break;
#endif
#if CFG_USE_LOW_IDLE_PS
    case CMD_SCTRL_RTOS_IDLE_SLEEP:
        sctrl_enter_rtos_idle_sleep(*(UINT32 *)param);
        break;

    case CMD_SCTRL_RTOS_IDLE_WAKEUP:
        sctrl_exit_rtos_idle_sleep();
        break;
#endif

#if CFG_USE_DEEP_PS
    case CMD_SCTRL_RTOS_DEEP_SLEEP:
        sctrl_enter_rtos_deep_sleep((PS_DEEP_CTRL_PARAM *)param);
        break;
#endif

    case CMD_GET_CHIP_ID:
        ret = REG_READ(SCTRL_CHIP_ID);
        break;

    case CMD_SCTRL_SET_FLASH_DPLL:
        reg = REG_READ(SCTRL_CONTROL);
        reg |= FLASH_26M_MUX_BIT;
        REG_WRITE(SCTRL_CONTROL, reg);
        break;

    case CMD_SCTRL_SET_FLASH_DCO:
        reg = REG_READ(SCTRL_CONTROL);
        reg &= ~FLASH_26M_MUX_BIT;
        REG_WRITE(SCTRL_CONTROL, reg);
        break;

    case CMD_SCTRL_DSP_POWERDOWN:
    case CMD_SCTRL_USB_POWERDOWN:
    case CMD_SCTRL_MODEM_POWERDOWN:
    case CMD_SCTRL_MAC_POWERDOWN:
    case CMD_SCTRL_DSP_POWERUP:
    case CMD_SCTRL_USB_POWERUP:
    case CMD_SCTRL_MAC_POWERUP:
    case CMD_SCTRL_MODEM_POWERUP:
    case CMD_SCTRL_BLE_POWERDOWN:
    case CMD_SCTRL_BLE_POWERUP:
        sctrl_subsys_power(cmd);
        break;

    case CMD_GET_DEVICE_ID:
        ret = REG_READ(SCTRL_DEVICE_ID);
        break;

    case CMD_GET_SCTRL_CONTROL:
        *((UINT32 *)param) = REG_READ(SCTRL_CONTROL);
        break;

    case CMD_SET_SCTRL_CONTROL:
        REG_WRITE(SCTRL_CONTROL, *((UINT32 *)param));
        break;

    case CMD_SCTRL_MCLK_SELECT:
        reg = REG_READ(SCTRL_CONTROL);
        reg &= ~(MCLK_MUX_MASK << MCLK_MUX_POSI);
        reg |= ((*(UINT32 *)param) & MCLK_MUX_MASK) << MCLK_MUX_POSI;
        REG_WRITE(SCTRL_CONTROL, reg);
        break;

    case CMD_SCTRL_MCLK_DIVISION:
        reg = REG_READ(SCTRL_CONTROL);
        reg &= ~(MCLK_DIV_MASK << MCLK_DIV_POSI);
        reg |= ((*(UINT32 *)param) & MCLK_DIV_MASK) << MCLK_DIV_POSI;
        REG_WRITE(SCTRL_CONTROL, reg);
        break;

    case CMD_SCTRL_MCLK_MUX_GET:
        reg = ((REG_READ(SCTRL_CONTROL) >> MCLK_MUX_POSI) & MCLK_MUX_MASK);
        *(UINT32 *)param = reg;
        break;

    case CMD_SCTRL_MCLK_DIV_GET:
        reg = ((REG_READ(SCTRL_CONTROL) >> MCLK_DIV_POSI) & MCLK_DIV_MASK);
        *(UINT32 *)param = reg;
        break;

    case CMD_SCTRL_MCLK_LIMIT_FREQ_BIT_SET:
        limit_freq_status |= (*(UINT32 *)param);
        os_null_printf("l:%x %x\r\n",limit_freq_status,(*(UINT32 *)param));
        if(limit_freq_status)
        {
            reg = REG_READ(SCTRL_CONTROL);
            reg &= ~(MCLK_DIV_MASK << MCLK_DIV_POSI);
            #if (CFG_SOC_NAME == SOC_BK7221U)
            reg &= ~HCLK_DIV2_EN_BIT;
            #endif
            reg |= ((MCLK_DIV_7 & MCLK_DIV_MASK) << MCLK_DIV_POSI);
            reg &= ~(MCLK_MUX_MASK << MCLK_MUX_POSI);
            reg |= ((MCLK_FIELD_DPLL & MCLK_MUX_MASK) << MCLK_MUX_POSI);
            REG_WRITE(SCTRL_CONTROL, reg);
        }
        break;

    case CMD_SCTRL_MCLK_LIMIT_FREQ_BIT_CLR:
        limit_freq_status &= ~(*(UINT32 *)param);
        os_null_printf("f:%x %x\r\n",limit_freq_status,(*(UINT32 *)param));
        if(0 == limit_freq_status)
        {
            #if (USE_DCO_CLK_POWON )
            sctrl_set_cpu_clk_dco();
            #else
            reg = REG_READ(SCTRL_CONTROL);
            reg &= ~(MCLK_DIV_MASK << MCLK_DIV_POSI);
            #if (CFG_SOC_NAME == SOC_BK7221U)
            reg |= HCLK_DIV2_EN_BIT;
            #endif
#if (CFG_SOC_NAME == SOC_BK7231N)
            reg |= ((MCLK_DIV_5 & MCLK_DIV_MASK) << MCLK_DIV_POSI);
#else
            reg |= ((MCLK_DIV_3 & MCLK_DIV_MASK) << MCLK_DIV_POSI);
#endif
            REG_WRITE(SCTRL_CONTROL, reg);
            #endif
        }
        break;

    case CMD_SCTRL_RESET_SET:
        reg = REG_READ(SCTRL_RESET);
        reg |= ((*(UINT32 *)param) & SCTRL_RESET_MASK);
        REG_WRITE(SCTRL_RESET, reg);
        break;

    case CMD_SCTRL_RESET_CLR:
        reg = REG_READ(SCTRL_RESET);
        reg &= ~((*(UINT32 *)param) & SCTRL_RESET_MASK);
        REG_WRITE(SCTRL_RESET, reg);
        break;

    case CMD_SCTRL_MODEM_SUBCHIP_RESET:
    case CMD_SCTRL_MAC_SUBSYS_RESET:
    case CMD_SCTRL_USB_SUBSYS_RESET:
    case CMD_SCTRL_DSP_SUBSYS_RESET:
        sctrl_subsys_reset(cmd);
        break;

    case CMD_SCTRL_MODEM_CORE_RESET:
        ret = REG_READ(SCTRL_MODEM_CORE_RESET_PHY_HCLK);
        ret = ret & (~((MODEM_CORE_RESET_MASK) << MODEM_CORE_RESET_POSI));
        reg = ret | ((MODEM_CORE_RESET_WORD & MODEM_CORE_RESET_MASK)
                     << MODEM_CORE_RESET_POSI);
        REG_WRITE(SCTRL_MODEM_CORE_RESET_PHY_HCLK, reg);

        delay(1);
        reg = ret;
        REG_WRITE(SCTRL_MODEM_CORE_RESET_PHY_HCLK, reg);

        /*resetting, and waiting for done*/
        reg = REG_READ(SCTRL_RESET);
        while(reg & MODEM_CORE_RESET_BIT)
        {
            delay(10);
            reg = REG_READ(SCTRL_RESET);
        }
        ret = SCTRL_SUCCESS;
        break;

    case CMD_SCTRL_MPIF_CLK_INVERT:
        reg = REG_READ(SCTRL_CONTROL);
        reg |= MPIF_CLK_INVERT_BIT;
        REG_WRITE(SCTRL_CONTROL, reg);
        break;

    case CMD_SCTRL_BLK_ENABLE:
        reg = REG_READ(SCTRL_BLOCK_EN_CFG);
        reg &= (~(BLOCK_EN_WORD_MASK << BLOCK_EN_WORD_POSI));
        reg |= (BLOCK_EN_WORD_PWD & BLOCK_EN_WORD_MASK) << BLOCK_EN_WORD_POSI;
        reg |= ((*(UINT32 *)param) & BLOCK_EN_VALID_MASK);
        REG_WRITE(SCTRL_BLOCK_EN_CFG, reg);
        break;

    case CMD_SCTRL_BLK_DISABLE:
        CHECK_OPERATE_RF_REG_IF_IN_SLEEP();
        reg = REG_READ(SCTRL_BLOCK_EN_CFG);
		reg &= (~(BLOCK_EN_WORD_MASK << BLOCK_EN_WORD_POSI));
		reg |= (BLOCK_EN_WORD_PWD & BLOCK_EN_WORD_MASK) << BLOCK_EN_WORD_POSI;        
        reg &= ~((*(UINT32 *)param) & BLOCK_EN_VALID_MASK);
        REG_WRITE(SCTRL_BLOCK_EN_CFG, reg);
        CHECK_OPERATE_RF_REG_IF_IN_SLEEP_END();
        break;

    case CMD_SCTRL_BIAS_REG_SET:
        reg = REG_READ(SCTRL_BIAS);
        reg |= (*(UINT32 *)param);
        REG_WRITE(SCTRL_BIAS, reg);
        break;

    case CMD_SCTRL_BIAS_REG_CLEAN:
        reg = REG_READ(SCTRL_BIAS);
        reg &= ~(*(UINT32 *)param);
        REG_WRITE(SCTRL_BIAS, reg);
        break;

    case CMD_SCTRL_BIAS_REG_READ:
        ret = REG_READ(SCTRL_BIAS);
        break;

    case CMD_SCTRL_BIAS_REG_WRITE:
        REG_WRITE(SCTRL_BIAS, *(UINT32 *)param);
        break;

    case CMD_SCTRL_ANALOG_CTRL4_SET:
        reg = sctrl_analog_get(SCTRL_ANALOG_CTRL4);
        reg |= (*(UINT32 *)param);
        sctrl_analog_set(SCTRL_ANALOG_CTRL4, reg);
        break;

    case CMD_SCTRL_ANALOG_CTRL4_CLEAN:
        reg = sctrl_analog_get(SCTRL_ANALOG_CTRL4);
        reg &= ~(*(UINT32 *)param);
        sctrl_analog_set(SCTRL_ANALOG_CTRL4, reg);
        break;

    case CMD_SCTRL_CALI_DPLL:
        sctrl_cali_dpll(0);
        break;

#if (CFG_SOC_NAME != SOC_BK7231)
    case CMD_SCTRL_SET_XTALH_CTUNE:
        reg = sctrl_analog_get(SCTRL_ANALOG_CTRL2);
        reg &= ~(XTALH_CTUNE_MASK<< XTALH_CTUNE_POSI);
        reg |= (((*(UINT32 *)param) &XTALH_CTUNE_MASK) << XTALH_CTUNE_POSI);
        sctrl_analog_set(SCTRL_ANALOG_CTRL2, reg);
        break;

    case CMD_SCTRL_GET_XTALH_CTUNE:
        reg = sctrl_analog_get(SCTRL_ANALOG_CTRL2);
        ret = ((reg >> XTALH_CTUNE_POSI) & XTALH_CTUNE_MASK);
        break;

    case CMD_BLE_RF_BIT_SET:
        reg = REG_READ(SCTRL_CONTROL);
        reg |= BLE_RF_EN_BIT;
        REG_WRITE(SCTRL_CONTROL, reg);
        break;

    case CMD_BLE_RF_BIT_CLR:
        reg = REG_READ(SCTRL_CONTROL);
        reg &= ~(BLE_RF_EN_BIT);
        REG_WRITE(SCTRL_CONTROL, reg);
        break;
#if (CFG_SOC_NAME == SOC_BK7231N)
	case CMD_BLE_RF_PTA_EN:
        reg = REG_READ(SCTRL_CONTROL);
        reg |= (BLE_RF_PTA_EN_BIT);
        REG_WRITE(SCTRL_CONTROL, reg);
        break; 
	case CMD_BLE_RF_PTA_DIS:
        reg = REG_READ(SCTRL_CONTROL);
        reg &= ~(BLE_RF_PTA_EN_BIT);
        REG_WRITE(SCTRL_CONTROL, reg);
        break; 
#endif

	case CMD_BLE_RF_BIT_GET:
		reg = REG_READ(SCTRL_CONTROL);
		*((UINT32 *)param) = reg & (BLE_RF_EN_BIT);
		break;
        
    case CMD_EFUSE_WRITE_BYTE: 
        ret = sctrl_write_efuse(param);
        break;
        
    case CMD_EFUSE_READ_BYTE:
        ret = sctrl_read_efuse(param);
        break; 
        
#endif // (CFG_SOC_NAME != SOC_BK7231)

#if (CFG_SOC_NAME == SOC_BK7221U)
    case CMD_SCTRL_OPEN_DAC_ANALOG:
        reg = sctrl_analog_get(SCTRL_ANALOG_CTRL9);
        reg |= EN_AUD_DAC_L | EN_AUD_DAC_R 
              | DAC_PA_OUTPUT_EN | DAC_DRIVER_OUTPUT_EN
              | AUD_DAC_DGA_EN;
        sctrl_analog_set(SCTRL_ANALOG_CTRL9, reg);

        reg = sctrl_analog_get(SCTRL_ANALOG_CTRL10);
        reg |= DAC_N_END_OUPT_L | DAC_N_END_OUPT_R;
        sctrl_analog_set(SCTRL_ANALOG_CTRL10, reg);
        break;

    case CMD_SCTRL_CLOSE_DAC_ANALOG:
        reg = sctrl_analog_get(SCTRL_ANALOG_CTRL9);
        reg &= ~(EN_AUD_DAC_L | EN_AUD_DAC_R 
              | DAC_PA_OUTPUT_EN | DAC_DRIVER_OUTPUT_EN
              | AUD_DAC_DGA_EN);
        sctrl_analog_set(SCTRL_ANALOG_CTRL9, reg);

        reg = sctrl_analog_get(SCTRL_ANALOG_CTRL10);
        reg &= ~(DAC_N_END_OUPT_L | DAC_N_END_OUPT_R);
        sctrl_analog_set(SCTRL_ANALOG_CTRL10, reg);
        break;

    case CMD_SCTRL_OPEN_ADC_MIC_ANALOG:
        reg = sctrl_analog_get(SCTRL_ANALOG_CTRL8);
        reg &= ~(SPI_PWD_AUD_ADC_L | SPI_PWD_AUD_ADC_R);
        sctrl_analog_set(SCTRL_ANALOG_CTRL8, reg);
        break;
        
    case CMD_SCTRL_CLOSE_ADC_MIC_ANALOG:
        reg = sctrl_analog_get(SCTRL_ANALOG_CTRL8);
        reg |= (SPI_PWD_AUD_ADC_L | SPI_PWD_AUD_ADC_R);
        sctrl_analog_set(SCTRL_ANALOG_CTRL8, reg);
        break;

    case CMD_SCTRL_ENALBLE_ADC_LINE_IN:
        reg = sctrl_analog_get(SCTRL_ANALOG_CTRL8);
        reg |= LINE_IN_EN;
        sctrl_analog_set(SCTRL_ANALOG_CTRL8, reg);
        break; 

    case CMD_SCTRL_DISALBLE_ADC_LINE_IN:
        reg = sctrl_analog_get(SCTRL_ANALOG_CTRL8);
        reg &= ~LINE_IN_EN;
        sctrl_analog_set(SCTRL_ANALOG_CTRL8, reg);
        break;  

    case CMD_SCTRL_SET_DAC_VOLUME_ANALOG:
        reg = sctrl_analog_get(SCTRL_ANALOG_CTRL8);
        reg &= ~(AUD_DAC_GAIN_MASK << AUD_DAC_GAIN_POSI);
        reg |= (((*(UINT32 *)param) & AUD_DAC_GAIN_MASK) << AUD_DAC_GAIN_POSI);
        sctrl_analog_set(SCTRL_ANALOG_CTRL8, reg);
        break;

    case CMD_SCTRL_SET_LINEIN_VOLUME_ANALOG:
        reg = sctrl_analog_get(SCTRL_ANALOG_CTRL8);
        reg &= ~(LINE_IN_GAIN_MASK << LINE_IN_GAIN_POSI);
        reg |= (((*(UINT32 *)param) & LINE_IN_GAIN_MASK) << LINE_IN_GAIN_POSI);
        sctrl_analog_set(SCTRL_ANALOG_CTRL8, reg);
        break;
        
#endif // (CFG_SOC_NAME == SOC_BK7221U)
	case CMD_SCTRL_SET_ANALOG0:
		sctrl_analog_set(SCTRL_ANALOG_CTRL0, (*(UINT32 *)param));
		break;
	case CMD_SCTRL_SET_ANALOG1:
		sctrl_analog_set(SCTRL_ANALOG_CTRL1, (*(UINT32 *)param));
		break;
	case CMD_SCTRL_SET_ANALOG2:
		sctrl_analog_set(SCTRL_ANALOG_CTRL2, (*(UINT32 *)param));
		break;
	case CMD_SCTRL_SET_ANALOG3:
		sctrl_analog_set(SCTRL_ANALOG_CTRL3, (*(UINT32 *)param));
		break;
	case CMD_SCTRL_SET_ANALOG4:
		sctrl_analog_set(SCTRL_ANALOG_CTRL4, (*(UINT32 *)param));
		break;
	case CMD_SCTRL_SET_ANALOG5:
		sctrl_analog_set(SCTRL_ANALOG_CTRL5, (*(UINT32 *)param));
		break;
	case CMD_SCTRL_GET_ANALOG0:
		ret = sctrl_analog_get(SCTRL_ANALOG_CTRL0);
		break;
	case CMD_SCTRL_GET_ANALOG1:
		ret = sctrl_analog_get(SCTRL_ANALOG_CTRL1);
		break;
	case CMD_SCTRL_GET_ANALOG2:
		ret = sctrl_analog_get(SCTRL_ANALOG_CTRL2);
		break;
	case CMD_SCTRL_GET_ANALOG3:
		ret = sctrl_analog_get(SCTRL_ANALOG_CTRL3);
		break;
	case CMD_SCTRL_GET_ANALOG4:
		ret = sctrl_analog_get(SCTRL_ANALOG_CTRL4);
		break;
	case CMD_SCTRL_GET_ANALOG5:
		ret = sctrl_analog_get(SCTRL_ANALOG_CTRL5);
		break;
    case CMD_SCTRL_SET_LOW_PWR_CLK:
        reg = REG_READ(SCTRL_LOW_PWR_CLK);
        reg &=~(LPO_CLK_MUX_MASK);
        reg |=((*(UINT32 *)param) << LPO_CLK_MUX_POSI);
        REG_WRITE(SCTRL_LOW_PWR_CLK, reg);
        break;
    case CMD_SCTRL_SET_GADC_SEL:
        #if (CFG_SOC_NAME == SOC_BK7231N)
        reg = sctrl_analog_get(SCTRL_ANALOG_CTRL4);
        reg &= ~(GADC_CAL_SEL_MASK << GADC_CAL_SEL_POSI);
        reg |= (((*(UINT32 *)param) & GADC_CAL_SEL_MASK) << GADC_CAL_SEL_POSI);
        sctrl_analog_set(SCTRL_ANALOG_CTRL4, reg);
        #endif
        break;
    case CMD_SCTRL_SET_VDD_VALUE:
    	reg = REG_READ(SCTRL_DIGTAL_VDD);
    	reg &= (~(DIG_VDD_ACTIVE_MASK << DIG_VDD_ACTIVE_POSI));
        reg |=((*(UINT32 *)param) << DIG_VDD_ACTIVE_POSI);
    	REG_WRITE(SCTRL_DIGTAL_VDD, reg);
        break;
    case CMD_SCTRL_GET_VDD_VALUE:
        reg = REG_READ(SCTRL_DIGTAL_VDD);
        ret = (reg >> DIG_VDD_ACTIVE_POSI) & DIG_VDD_ACTIVE_MASK;
        break;

    case CMD_GET_SCTRL_RETETION:
        *((UINT32 *)param) = REG_READ(SCTRL_SW_RETENTION);
        break;

    case CMD_SET_SCTRL_RETETION:
        REG_WRITE(SCTRL_SW_RETENTION, *((UINT32 *)param));
        break;
    
    default:
        ret = SCTRL_FAILURE;
        break;
    }
	GLOBAL_INT_RESTORE();

    return ret;
}

// EOF
