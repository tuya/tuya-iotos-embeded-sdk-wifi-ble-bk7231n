/**
****************************************************************************************
*
* @file icu.c
*
* @brief icu initialization and specific functions
*
* Copyright (C) Beken 2009-2016
*
* $Rev: $
*
****************************************************************************************
*/

/**
****************************************************************************************
* @addtogroup ICU
* @ingroup ICU
* @brief ICU
*
* This is the driver block for ICU
* @{
****************************************************************************************
*/


#include <stddef.h>     // standard definition

#include "driver_sys_ctrl.h"




void DelayNops(volatile unsigned long nops)
{
    while (nops --)
    {
    }
}


void DelayUS(volatile unsigned long timesUS)
{
	volatile unsigned long i;

	//    unsigned long tick_cnt = 8;       // 24MHz, DelayUS(1000)平均DelayUS(1) = 1.052us
	#if (MCU_CLK == MCU_CLK_26MHz)
	//    unsigned long tick_cnt_us = 3;       // 26MHz, DelayUS(1000)平均DelayUS(1) = 1.028us
	#define tick_cnt_us         3
	#elif (MCU_CLK == MCU_CLK_120MHz)		/* #if (MCU_CLK == MCU_CLK_26MHz) */
	#define tick_cnt_us         28
	#endif		/* #if (MCU_CLK == MCU_CLK_26MHz) */
//    printf("tick_cnt = 0x%x\r\n", tick_cnt);
    while (timesUS --)
	{
		i = 0;
		while (i < tick_cnt_us)
		{
			i++;
		}
	}
}


// set CPU clk to DPLL
void set_CPU_clk_DPLL(void)
{
    REG_SYS_CTRL_BLOCK_EN = (REG_SYS_CTRL_BLOCK_EN
                           & (~SYS_CTRL_BLOCK_EN_VALID_MASK))
                           | SYS_CTRL_BLOCK_EN_VALID_SET
                           | SYS_CTRL_BLOCK_EN_SW_XTAL_26MHz;

    REG_SYS_CTRL_ANALOG_REG0 = 0x0819A54B;
    REG_SYS_CTRL_ANALOG_REG1 = 0x6AC03102;
//    REG_SYS_CTRL_ANALOG_REG2 = 0x24006000;
    REG_SYS_CTRL_ANALOG_REG2 = 0x27006000;
    REG_SYS_CTRL_ANALOG_REG3 = 0x4FE06C50;
    REG_SYS_CTRL_ANALOG_REG4 = 0x59E04520;
    DelayNops(100);
    REG_SYS_CTRL_ANALOG_REG0 = 0x0811A54B;
    DelayNops(100);
    REG_SYS_CTRL_ANALOG_REG0 = 0x0819A54B;
    DelayNops(1000);

    DelayUS(100);
    REG_SYS_CTRL_CLK_SET = (REG_SYS_CTRL_CLK_SET
                         & (~SYS_CTRL_CLK_SET_MCLK_MASK)
                         & (~SYS_CTRL_CLK_SET_MCLK_DIV_MASK))
//                         | (5 << SYS_CTRL_CLK_SET_MCLK_DIV_POSI)
                         | SYS_CTRL_CLK_SET_MCLK_DPLL;
    DelayUS(100);
}


// set PCLK to XTAL 26MHz
void set_PCLK_clk_XTAL_26MHz(void)
{
    REG_ICU_PERI_CLK_MUX = ICU_PERI_CLK_MUX_SEL_XTAL_26M;
}

// set PCLK to DCO clock
void set_PCLK_clk_DCO_CLK(void)
{
    REG_ICU_PERI_CLK_MUX = ICU_PERI_CLK_MUX_SEL_DCO_CLK;
}


void set_LPO_clk_ROSC_32KHz(void)
{
    REG_SYS_CTRL_LPO_CLK = SYS_CTRL_LPO_CLK_MUX_ROSC_32KHz;
    REG_SYS_CTRL_ROSC_CONFIG = 0x37;
}

void set_LPO_clk_XTAL_32KHz(void)
{
    REG_SYS_CTRL_LPO_CLK = SYS_CTRL_LPO_CLK_MUX_XTAL_32KHz;
}

void set_LPO_clk_32KHz_div_from_XTAL_26MHz(void)
{
    REG_SYS_CTRL_LPO_CLK = SYS_CTRL_LPO_CLK_MUX_DIV_32KHz;
}


// set CPU clk to XTAL 26MHz
void set_CPU_clk_XTAL_26MHz(void)
{
    REG_SYS_CTRL_BLOCK_EN = (REG_SYS_CTRL_BLOCK_EN
                           & (~SYS_CTRL_BLOCK_EN_VALID_MASK))
                           | SYS_CTRL_BLOCK_EN_VALID_SET
                           | SYS_CTRL_BLOCK_EN_SW_XTAL_26MHz;
    DelayUS(100);
    REG_SYS_CTRL_CLK_SET = (REG_SYS_CTRL_CLK_SET
                         & (~SYS_CTRL_CLK_SET_MCLK_MASK))
                         | SYS_CTRL_CLK_SET_MCLK_XTAL_26M;
    DelayUS(100);
}





void ble_icu_init(void)
{
/*    REG_SYS_CTRL_BLOCK_EN = (REG_SYS_CTRL_BLOCK_EN
                           & (~SYS_CTRL_BLOCK_EN_VALID_MASK))
                           | SYS_CTRL_BLOCK_EN_VALID_SET
                           | SYS_CTRL_BLOCK_EN_SW_FLASH
                           | SYS_CTRL_BLOCK_EN_SW_DCO_26MHz
                           | SYS_CTRL_BLOCK_EN_SW_DPLL
                           | SYS_CTRL_BLOCK_EN_SW_XTAL_TO_RF
                           | SYS_CTRL_BLOCK_EN_SW_ROSC_32KHz
                           | SYS_CTRL_BLOCK_EN_SW_XTAL_26MHz
                           | SYS_CTRL_BLOCK_EN_SW_XTAL_32KHz;*/
    REG_WIFI_PWD = 0x0;
    REG_DSP_PWD  = 0x0;
    REG_USB_PWD  = 0x0;
    (*((volatile unsigned long *) 0x01050000)) = 0x09;

    DelayUS(100);

    set_LPO_clk_ROSC_32KHz();

#if (MCU_CLK == MCU_CLK_26MHz)
    set_CPU_clk_XTAL_26MHz();
    REG_SYS_CTRL_ANALOG_REG4 = 0x59E04520; 
#elif (MCU_CLK == MCU_CLK_120MHz)
    set_CPU_clk_DPLL();
#endif

#if (PER_CLK == PER_CLK_26MHz)
    set_PCLK_clk_XTAL_26MHz();
#elif (PER_CLK == PER_CLK_120MHz)
    DcoCalib(DCO_CALIB_120M);
    set_PCLK_clk_DCO_CLK();
#endif
    DelayUS(100);
//    REG_SYS_CTRL_CLK_SET |= SYS_CTRL_CLK_SET_AHB_CLK_DIV_MASK;

    REG_SYS_CTRL_BIAS_CALIB = (REG_SYS_CTRL_BIAS_CALIB
                            & (~(SYS_CTRL_BIAS_CALIB_MANUAL_MASK | SYS_CTRL_BIAS_CALIB_SETTING_MASK)))
                            | (0x01 << SYS_CTRL_BIAS_CALIB_MANUAL_POSI)
                            | (0x14 << SYS_CTRL_BIAS_CALIB_SETTING_POSI);

   // REG_ICU_INT_ENABLE = 1<<30;
   // REG_ICU_INT_GLOBAL_ENABLE = (ICU_INT_GLOBAL_ENABLE_IRQ_MASK | ICU_INT_GLOBAL_ENABLE_FIQ_MASK);
  
}









