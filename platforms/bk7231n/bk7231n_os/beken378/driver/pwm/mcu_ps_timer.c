#include "include.h"
#include "arm_arch.h"

#include "pwm.h"
#include "pwm_pub.h"
#include "bk_timer_pub.h"
#include "bk_timer.h"
#include "icu_pub.h"
#include "fake_clock_pub.h"
#include "mcu_ps_pub.h"
#include "target_util_pub.h"
#include "wlan_ui_pub.h"
#include "intc_pub.h"

#if (CFG_SOC_NAME == SOC_BK7231)
void ps_pwm_enable(void)
{
    UINT32 reg = 0;
    reg = REG_READ(PWM_CTL);
    reg &= (~(0xf<<(PS_PWM_ID*4)));
    reg |= (0x7<<(PS_PWM_ID*4));
    REG_WRITE(PWM_CTL, reg);
}

void ps_pwm_disable(void )
{
    UINT32 reg;
    reg = REG_READ(PWM_CTL);
    REG_WRITE(PWM_CTL, reg & (~(0xf<<(PS_PWM_ID*4))));
}

void ps_pwm_set_period(UINT32 period, UINT8 clk_mux)
{
    UINT32 reg = 0,ch;

    ch = PS_PWM_ID;
    if(PWM_MUX_LPO == clk_mux)
    {
        icu_ctrl(CMD_CONF_PWM_LPOCLK, (void *)&ch);
    }
    else
    {
        icu_ctrl(CMD_CONF_PWM_PCLK, (void *)&ch);
    }
    
#if (CFG_SOC_NAME == SOC_BK7231)
    reg = 0;
    reg |= period << PWM0_END_POSI | 0x0 << PWM0_DC_POSI;
    REG_WRITE(MCU_PS_PWM_COUNTER , reg);
#else
	reg = period;
    REG_WRITE(MCU_PS_PWM_COUNTER , reg);
    reg = 0;
    REG_WRITE(MCU_PS_PWM_DUTY_CYCLE , reg);   
#endif
}


void ps_pwm_reconfig(UINT32 period, UINT8 clk_mux)
{
    UINT32 param;
    //disable
#if (CFG_SOC_NAME == SOC_BK7231)	
    ps_pwm_disable();
    delay(5);
    //new
    ps_pwm_set_period(period, clk_mux);
    delay(1);
    //reenable
    ps_pwm_enable();
    
    REG_WRITE(PWM_INTERRUPT_STATUS,0x1);
#else
    ps_pwm_disable();
    //new
    ps_pwm_set_period(period, clk_mux);
    //reenable
    ps_pwm_enable();
    delay(5);

    REG_WRITE(PWM_INTERRUPT_STATUS,0x3f);
#endif
	
}

void ps_pwm_suspend_tick(UINT32 period)
{
    ps_pwm_reconfig(period, PWM_MUX_LPO);
}

void ps_pwm_resume_tick(void)
{
    ps_pwm_reconfig(fclk_cal_endvalue(PWM_CLK_26M), PWM_MUX_PCLK);
}

UINT32 ps_pwm_int_status(void )
{
    return ((REG_READ(ICU_INT_STATUS) & (CO_BIT(IRQ_PWM)))&& (REG_READ(PWM_INTERRUPT_STATUS) & 0x1));
}
#endif


#if (CFG_SOC_NAME != SOC_BK7231)
void ps_timer3_enable(UINT32 period)
{
    UINT32 reg;

#if (CFG_SOC_NAME == SOC_BK7231U) || (SOC_BK7231N == CFG_SOC_NAME)
    reg = REG_READ(TIMER3_5_READ_CTL);
    reg &= ~(TIMER3_5_READ_INDEX_MASK << TIMER3_5_READ_INDEX_POSI);
    reg |= (TIMER3_5_READ_INDEX_3 << TIMER3_5_READ_INDEX_POSI);
    REG_WRITE(TIMER3_5_READ_CTL,reg);
#endif

    reg = REG_READ(TIMER3_5_CTL);
    reg &= (~TIMERCTL3_EN_BIT);
    REG_WRITE(TIMER3_5_CTL,reg);
    delay(2);
    REG_WRITE(TIMER3_CNT,period);
    delay(2);
    reg = REG_READ(TIMER3_5_CTL);
    reg |= (TIMERCTL3_EN_BIT);
    reg &= ~(0x7 << TIMERCTLB_INT_POSI);
    REG_WRITE(TIMER3_5_CTL,reg);
}

void ps_timer3_measure_prepare(void)
{
#if (CFG_SOC_NAME == SOC_BK7231U) || (SOC_BK7231N == CFG_SOC_NAME)
    UINT32 reg;
    if(! (REG_READ(TIMER3_5_CTL)&(TIMERCTL3_INT_BIT)))
    {
        reg = REG_READ(TIMER3_5_READ_CTL);
        reg |= TIMER3_5_READ_OP_BIT;
        REG_WRITE(TIMER3_5_READ_CTL,reg);
    }
#endif
}

UINT32 ps_timer3_disable(void)
{
    UINT32 reg,less;
    if(REG_READ(TIMER3_5_CTL)&(TIMERCTL3_INT_BIT))
    {
        less = REG_READ(TIMER3_CNT);
    }
    else
    {
#if (CFG_SOC_NAME == SOC_BK7231U) || (SOC_BK7231N == CFG_SOC_NAME)
        while(REG_READ(TIMER3_5_READ_CTL) & TIMER3_5_READ_OP_BIT);
        less = REG_READ(TIMER3_5_READ_VALUE);
#else
        less = REG_READ(TIMER3_CNT);
#endif
    }

    reg = REG_READ(TIMER3_5_CTL);
    reg &= (~TIMERCTL3_EN_BIT);
    reg &= ~(0x7 << TIMERCTLB_INT_POSI);
    REG_WRITE(TIMER3_5_CTL,reg);
    delay(2);
    REG_WRITE(TIMER3_CNT,FCLK_DURATION_MS*32);
    delay(2);
    reg = REG_READ(TIMER3_5_CTL);
    reg |= (TIMERCTL3_EN_BIT);
    REG_WRITE(TIMER3_5_CTL,reg);

    return (less/32);
}
#endif

