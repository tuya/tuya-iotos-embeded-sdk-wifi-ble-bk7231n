#if (CFG_SOC_NAME == SOC_BK7231N)
#include "include.h"
#include "arm_arch.h"
#include <stdlib.h>

#include "pwm_pub.h"

#include "drv_model_pub.h"
#include "intc_pub.h"
#include "icu_pub.h"
#include "gpio_pub.h"
#include "uart_pub.h"
#include "pwm_new.h"

UINT8 current_channel;
UINT8 current_group;

static SDD_OPERATIONS pwm_op =
{
    pwm_ctrl
};

void (*p_PWM_Int_Handler[CHANNEL_NO])(UINT8);

UINT8 get_current_group(UINT8 channel)
{
	GLOBAL_INT_DECLARATION();

	GLOBAL_INT_DISABLE();
	
    if(channel >= PWM_COUNT)
    {
    	bk_printf("PWM:channel error\r\n");
        return;
    }
			
	if (channel < 2)
    {
        current_group = 0;
		current_channel = channel;
    }
	else  if (channel < 4)
    {
        current_group = 1;
		current_channel = channel-2;
    }
	else
    {
        current_group = 2;
		current_channel = channel-4;
    }
	
	GLOBAL_INT_RESTORE();

	return current_group;
}

UINT32 pwm_check_group(UINT8 channel1,UINT8 channel2)
{
	UINT32 group;
	
	if(abs(channel2-channel1) ==1)
	{
		if(channel1==0)
		{
			if(channel2 ==1)
			{
				group= 1;
			}
			else
			{
				group= 0;
			}
		}
		else if(channel1==1)
		{
			if(channel2 ==0)
			{
				group= 1;
			}
			else
			{
				group= 0;
			}
		}
		else if(channel1==2)
		{
			if(channel2 ==3)
			{
				group= 1;
			}
			else
			{
				group= 0;
			}
		}
		else if(channel1==3)
		{
			if(channel2 ==2)
			{
				group= 1;
			}
			else
			{
				group= 0;
			}
		}
		else if(channel1==4)
		{
			if(channel2 ==5)
			{
				group= 1;
			}
			else
			{
				group= 0;
			}
		}
		else
		{
			if(channel2 ==4)
			{
				group= 1;
			}
			else
			{
				group= 0;
			}
		}
	}
	else
	{
		group= 0;
	}

	return group;
}


static void pwm_gpio_configuration(UINT8 chan, UINT8 enable)
{
    UINT32 ret;
    UINT32 param;

    switch(chan)
    {
    case PWM0:
        param = GFUNC_MODE_PWM0;
        break;

    case PWM1:
        param = GFUNC_MODE_PWM1;
        break;

    case PWM2:
        param = GFUNC_MODE_PWM2;
        break;

    case PWM3:
        param = GFUNC_MODE_PWM3;
        break;

    case PWM4:
        param = GFUNC_MODE_PWM4;
        break;

    case PWM5:
        param = GFUNC_MODE_PWM5;
        break;

    default:
        break;
    }

	if(enable)
	{
    ret = sddev_control(GPIO_DEV_NAME, CMD_GPIO_ENABLE_SECOND, &param);
	}
	else
	{
		param = GPIO_CFG_PARAM(param, GMODE_INPUT);
    	ret = sddev_control(GPIO_DEV_NAME, CMD_GPIO_CFG, &param);
	}
    ASSERT(GPIO_SUCCESS == ret);
}

static void pwm_icu_configuration(pwm_param_t *pwm_param, UINT8 enable)
{
    UINT32 ret;
    UINT32 prm;

    /* set clock power down of icu module*/
    switch(pwm_param->channel)
    {
    case PWM0:
        prm = PWD_PWM0_CLK_BIT;
        break;

    case PWM1:
        prm = PWD_PWM0_CLK_BIT;
        break;

    case PWM2:
        prm = PWD_PWM2_CLK_BIT;
        break;

    case PWM3:
        prm = PWD_PWM2_CLK_BIT;
        break;

    case PWM4:
        prm = PWD_PWM4_CLK_BIT;
        break;

    case PWM5:
        prm = PWD_PWM4_CLK_BIT;
        break;

    default:
		PWM_WARN("pwm_iconfig_fail\r\n");
        goto exit_icu;
    }

	if(enable)
	{
	    ret = sddev_control(ICU_DEV_NAME, CMD_CLK_PWR_UP, (void *)&prm);
	    ASSERT(ICU_SUCCESS == ret);

	    if(PWM_CLK_32K == pwm_param->cfg.bits.clk)
	    {
	        prm = pwm_param->channel;
	        ret = sddev_control(ICU_DEV_NAME, CMD_CONF_PWM_LPOCLK, (void *)&prm);
	    }
	    else if (PWM_CLK_26M == pwm_param->cfg.bits.clk)
	    {
	    	prm = PCLK_POSI_PWMS;
	        ret = sddev_control(ICU_DEV_NAME, CMD_CONF_PCLK_26M, (void *)&prm);
				
	        prm = pwm_param->channel;
	        ret = sddev_control(ICU_DEV_NAME, CMD_CONF_PWM_PCLK, (void *)&prm);
	    }
		else
		{
		    prm = PCLK_POSI_PWMS;
	        ret = sddev_control(ICU_DEV_NAME, CMD_CONF_PCLK_DCO, (void *)&prm);
				
	        prm = pwm_param->channel;
	        ret = sddev_control(ICU_DEV_NAME, CMD_CONF_PWM_PCLK, (void *)&prm);		

		}
	    ASSERT(ICU_SUCCESS == ret);
	}
	else
	{
    	ret = sddev_control(ICU_DEV_NAME, CMD_CLK_PWR_DOWN, (void *)&prm);
		ASSERT(ICU_SUCCESS == ret);
	}

    if(PWM_INT_EN == pwm_param->cfg.bits.int_en)
    {
        prm = IRQ_PWM_BIT;
        ret = sddev_control(ICU_DEV_NAME, CMD_ICU_INT_ENABLE, (void *)&prm);
    }

exit_icu:

    return;
}

UINT8 init_pwm_param(pwm_param_t *pwm_param, UINT8 enable)
{
    UINT32 value;

    if((pwm_param == NULL)
            || (pwm_param->channel >= PWM_COUNT)
            || (pwm_param->duty_cycle1 > pwm_param->end_value))
    {
        return 1;
    }
			
	if (pwm_param->channel < 2)
    {
        current_group = 0;
		current_channel = pwm_param->channel;
    }
	else  if (pwm_param->channel < 4)
    {
        current_group = 1;
		current_channel = pwm_param->channel-2;
    }
	else
    {
        current_group = 2;
		current_channel = pwm_param->channel-4;
    }

	if (pwm_param->cfg.bits.mode != PWM_TIMER_MODE)
	{
#if (CFG_SOC_NAME == SOC_BK7231)
		pwm_gpio_configuration(pwm_param->channel, enable);
#else
		pwm_gpio_configuration(pwm_param->channel, enable);
#endif
	}

	//pwm mode set
    value = REG_READ(REG_PWM_GROUP_CTRL_ADDR(current_group));

	
    value = (value & (~(0x07 << PWM_GROUP_MODE_SET_BIT(current_channel))))
            | (pwm_param->cfg.bits.mode  <<  PWM_GROUP_MODE_SET_BIT(current_channel));

	REG_WRITE(REG_PWM_GROUP_CTRL_ADDR(current_group), value);
	
	PWM_PRT("value:0x%x, group:%x, channel:%x\r\n",value,current_group,  current_channel);
	PWM_PRT("ctrl:0x%lx\r\n",REG_READ(REG_PWM_GROUP_CTRL_ADDR(current_group)));

	//pwm freq set
#if (CFG_SOC_NAME == SOC_BK7231)
    value = (((UINT32)pwm_param->duty_cycle & 0x0000FFFF) << 16)
		            + ((UINT32)pwm_param->end_value & 0x0000FFFF);
    REG_WRITE(REG_APB_BK_PWMn_CNT_ADDR(pwm_param->channel), value);
#else
	if(current_channel == 0)
	{
		value = (UINT32) pwm_param->duty_cycle1;
		REG_WRITE(REG_GROUP_PWM0_T1_ADDR(current_group), value);
		
		value = (UINT32) pwm_param->duty_cycle2;
		REG_WRITE(REG_GROUP_PWM0_T2_ADDR(current_group), value);
		
		value = (UINT32) pwm_param->duty_cycle3;
		REG_WRITE(REG_GROUP_PWM0_T3_ADDR(current_group), value);
		
		value = (UINT32) pwm_param->end_value;
		REG_WRITE(REG_GROUP_PWM0_T4_ADDR(current_group), value);
	}
	else
	{
		value = (UINT32) pwm_param->duty_cycle1;
		REG_WRITE(REG_GROUP_PWM1_T1_ADDR(current_group), value);
		
		value = (UINT32) pwm_param->duty_cycle2;
		REG_WRITE(REG_GROUP_PWM1_T2_ADDR(current_group), value);
		
		value = (UINT32) pwm_param->duty_cycle3;
		REG_WRITE(REG_GROUP_PWM1_T3_ADDR(current_group), value);
		
		value = (UINT32) pwm_param->end_value;
		REG_WRITE(REG_GROUP_PWM1_T4_ADDR(current_group), value);

	}
#endif

	//clear int status
	value = REG_READ(REG_PWM_GROUP_CTRL_ADDR(current_group));
    value = value | PWM_GROUP_PWM_INT_STAT_CLEAR(current_channel);
	REG_WRITE(REG_PWM_GROUP_CTRL_ADDR(current_group), value);
	
    p_PWM_Int_Handler[pwm_param->channel] = pwm_param->p_Int_Handler;

	//pwm int set 
    value = REG_READ(REG_PWM_GROUP_CTRL_ADDR(current_group));
    value = (value & (~(0x01 << PWM_GROUP_PWM_INT_ENABLE_BIT(current_channel))))
            | (pwm_param->cfg.bits.int_en << PWM_GROUP_PWM_INT_ENABLE_BIT(current_channel));
    REG_WRITE(REG_PWM_GROUP_CTRL_ADDR(current_group), value);

	PWM_PRT("mode: %x, REG_PWM_GROUP_CTRL= 0x%lx\r\n",pwm_param->cfg.val ,REG_READ (REG_PWM_GROUP_CTRL_ADDR(current_group)));	
    pwm_icu_configuration(pwm_param, enable);
}

void pwm_unit_enable(UINT8 ucChannel)
{
	UINT32 value;
	
	if (ucChannel > PWM_CHANNEL_NUMBER_MAX)
    {
        return;
    }
	
	if (ucChannel < 2)
	{
	   current_group = 0;  
	   current_channel = ucChannel;
	}
	else  if (ucChannel < 4)
	{
	   current_group = 1;
	   current_channel = ucChannel-2;
	}
	else
	{
	   current_group = 2;
	   current_channel = ucChannel-4;
	}

	value = REG_READ(REG_PWM_GROUP_CTRL_ADDR(current_group));
	value |= PWM_GROUP_PWM_ENABLE_MASK(current_channel);
	REG_WRITE(REG_PWM_GROUP_CTRL_ADDR(current_group), value);
}

void pwm_unit_disable(UINT8 ucChannel)
{
	UINT32 value;
	if (ucChannel > PWM_CHANNEL_NUMBER_MAX)
	{
		return;
	}
	
	if (ucChannel < 2)
	{
	   current_group = 0;  
	   current_channel = ucChannel;
	}
	else  if (ucChannel < 4)
	{
	   current_group = 1;
	   current_channel = ucChannel-2;
	}
	else
	{
	   current_group = 2;
	   current_channel = ucChannel-4;
	}

	value = REG_READ(REG_PWM_GROUP_CTRL_ADDR(current_group));
	value &= ~(PWM_GROUP_PWM_ENABLE_MASK(current_channel));
	REG_WRITE(REG_PWM_GROUP_CTRL_ADDR(current_group), value);
}

 UINT32 pwm_capture_value_get(UINT8 ucChannel)
{
	UINT32 value,state;
	
	if (ucChannel < 2)
	{
	   current_group = 0;
	   current_channel = ucChannel;
	}
	else  if (ucChannel < 4)
	{
	   current_group = 1;
	   current_channel = ucChannel-2;
	}
	else
	{
	   current_group = 2;
	   current_channel = ucChannel-4;
	}


	value = REG_READ(REG_GROUP_PWM_CPU_ADDR(current_group));
	value |=  1 << current_channel;
	REG_WRITE(REG_GROUP_PWM_CPU_ADDR(current_group), value);
	
	PWM_PRT("channel: %x, REG_PWM_GROUP_CTRL= 0x%lx\r\n",current_channel ,REG_READ (REG_PWM_GROUP_CTRL_ADDR(current_group)));	
	
	state = REG_READ(REG_GROUP_PWM_CPU_ADDR(current_group));
	while ((state & (1 << ucChannel) )== 0 )
    {
    	PWM_PRT("channel: ");
    }

	if(current_channel == 1)
	{
		value = REG_READ(REG_GROUP_PWM1_RD_DATA_ADDR(current_group));
		return value;
	}
	else
	{
		value = REG_READ(REG_GROUP_PWM0_RD_DATA_ADDR(current_group));
		return value;
	}

}

static void pwm_set_duty_cycle(UINT8 ucChannel, UINT32 u32DutyCycle)
{
	 UINT32 value;

    if (ucChannel < 2)
    {
       current_group = 0;
	   current_channel = ucChannel;   
    }
	else  if (ucChannel < 4)
    {
       current_group = 1;
       current_channel = ucChannel-2;
    }
	else
    {
       current_group = 2;
       current_channel = ucChannel-4;
    }

	//check last opreation work
	value = REG_READ(REG_PWM_GROUP_CTRL_ADDR(current_group));
	if (value & (PWM_GROUP_PWM_CFG_UPDATA_MASK(current_channel)))
	{
		PWM_FATAL("[bk_error]:wait updata pwm param");
	}

	if (current_channel ==0 )
	{
		REG_WRITE(REG_GROUP_PWM0_T1_ADDR(current_group), u32DutyCycle);
	}
	else
	{
		REG_WRITE(REG_GROUP_PWM1_T1_ADDR(current_group), u32DutyCycle);
	}
	
	// bit 7/15 :cfg_updata enable
	value = REG_READ(REG_PWM_GROUP_CTRL_ADDR(current_group));
	value |= PWM_GROUP_PWM_CFG_UPDATA_MASK(current_channel);
	REG_WRITE(REG_PWM_GROUP_CTRL_ADDR(current_group), value);

}

static void pwm_set_end_value(UINT8 ucChannel, UINT32 u32EndValue)
{
	UINT32 value;

	if (ucChannel < 2)
	{
	   current_group = 0;  
	   current_channel = ucChannel;	   
	}
	else  if (ucChannel < 4)
	{
	   current_group = 1;
	   current_channel = ucChannel-2;
	}
	else
	{
	   current_group = 2;
	   current_channel = ucChannel-4;
	}
	
	//check last opreation work
	value = REG_READ(REG_PWM_GROUP_CTRL_ADDR(current_group));
	if (value & (PWM_GROUP_PWM_CFG_UPDATA_MASK(current_channel)))
	{
		PWM_FATAL("[bk_error]:wait updata pwm param");
	}

	if (current_channel ==0 )
	{
		REG_WRITE(REG_GROUP_PWM0_T4_ADDR(current_group), u32EndValue);
	}
	else
	{
		REG_WRITE(REG_GROUP_PWM1_T4_ADDR(current_group), u32EndValue);
	}
	
	// bit 7/15 :cfg_updata enable
	value = REG_READ(REG_PWM_GROUP_CTRL_ADDR(current_group));
	value |= PWM_GROUP_PWM_CFG_UPDATA_MASK(current_channel);
	REG_WRITE(REG_PWM_GROUP_CTRL_ADDR(current_group), value);

}

void pwm_group_enable(UINT8 ucChannel)
{
	UINT32 value;
	
	if (ucChannel > PWM_CHANNEL_NUMBER_MAX)
    {
        return;
    }
	
	if (ucChannel < 2)
	{
	   current_group = 0;  
	}
	else  if (ucChannel < 4)
	{
	   current_group = 1;
	}
	else
	{
	   current_group = 2;
	}

	value = REG_READ(REG_PWM_GROUP_CTRL_ADDR(current_group));
	value |= PWM_GROUP_PWM_ENABLE_MASK(0)|PWM_GROUP_PWM_ENABLE_MASK(1) ;
	REG_WRITE(REG_PWM_GROUP_CTRL_ADDR(current_group), value);
}

void pwm_group_mode_disable(UINT8 ucChannel)
{
	UINT32 value;
	
	if (ucChannel > PWM_CHANNEL_NUMBER_MAX)
    {
        return;
    }
	
	if (ucChannel < 2)
	{
	   current_group = 0;  
	}
	else  if (ucChannel < 4)
	{
	   current_group = 1;
	}
	else
	{
	   current_group = 2;
	}

	value = REG_READ(REG_PWM_GROUP_CTRL_ADDR(current_group));
	value &=( ~( PWM_GROUP_PWM_ENABLE_MASK(0)|PWM_GROUP_PWM_ENABLE_MASK(1) 
			|PWM_GROUP_PWM_GROUP_MODE_MASK | PWM_GROUP_PWM_GROUP_MODE_ENABLE_MASK));
	REG_WRITE(REG_PWM_GROUP_CTRL_ADDR(current_group), value);
}

void pwm_group_mode_enable(UINT8 ucChannel)
{
 	UINT32 value;
	
	if (ucChannel < 2)
	{
	   current_group = 0;  
	   current_channel = ucChannel;	   
	}
	else  if (ucChannel < 4)
	{
	   current_group = 1;
	}
	else
	{
	   current_group = 2;
	}
	
	value = REG_READ(REG_PWM_GROUP_CTRL_ADDR(current_group));

	//enable grounp pwm mode
	value |= PWM_GROUP_PWM_GROUP_MODE_MASK |PWM_GROUP_PWM_GROUP_MODE_ENABLE_MASK;
	REG_WRITE(REG_PWM_GROUP_CTRL_ADDR(current_group), value);	
	pwm_group_enable(ucChannel);
	
	PWM_PRT("ctrl:0x%lx\r\n",REG_READ(REG_PWM_GROUP_CTRL_ADDR(current_group)));	
}

UINT8 pwm_init_levl_set_low(UINT8 ucChannel)
{
	UINT32 value;
	if (ucChannel > PWM_CHANNEL_NUMBER_MAX)
	{
		return 1;
	}
	
	if (ucChannel < 2)
	{
	   current_group = 0;  
	   current_channel = ucChannel;	   
	}
	else  if (ucChannel < 4)
	{
	   current_group = 1;
	   current_channel = ucChannel-2;
	}
	else
	{
	   current_group = 2;
	   current_channel = ucChannel-4;
	}
	
	value = REG_READ(REG_PWM_GROUP_CTRL_ADDR(current_group));
	value &= ~(PWM_GROUP_PWM_INT_LEVL_MASK(current_channel));
	REG_WRITE(REG_PWM_GROUP_CTRL_ADDR(current_group), value);

	return 0;

}

UINT8 pwm_init_levl_set_high(UINT8 ucChannel)
{
	UINT32 value;
	if (ucChannel > PWM_CHANNEL_NUMBER_MAX)
	{
		return 1;
	}
	
	if (ucChannel < 2)
	{
	   current_group = 0;  
	   current_channel = ucChannel;	   
	}
	else  if (ucChannel < 4)
	{
	   current_group = 1;
	   current_channel = ucChannel-2;
	}
	else
	{
	   current_group = 2;
	   current_channel = ucChannel-4;
	}
	
	value = REG_READ(REG_PWM_GROUP_CTRL_ADDR(current_group));
	value |= PWM_GROUP_PWM_INT_LEVL_MASK(current_channel);
	REG_WRITE(REG_PWM_GROUP_CTRL_ADDR(current_group), value);

	return 0;
}

UINT8 pwm_init_levl_get(UINT8 ucChannel)
{
	UINT32 value,ret;
	if (ucChannel > PWM_CHANNEL_NUMBER_MAX)
	{
		return 1;
	}
	
	if (ucChannel < 2)
	{
	   current_group = 0;  
	   current_channel = ucChannel;	   
	}
	else  if (ucChannel < 4)
	{
	   current_group = 1;
	   current_channel = ucChannel-2;
	}
	else
	{
	   current_group = 2;
	   current_channel = ucChannel-4;
	}
	
	value = REG_READ(REG_PWM_GROUP_CTRL_ADDR(current_group));
	
	ret = value & PWM_GROUP_PWM_INT_LEVL_MASK(current_channel);
	if(ret !=0 )
	{
		return 1;
	}
	else 
	{
		return 0;
	}

}

UINT8 pwm_update_param(pwm_param_t *pwm_param)
{
	UINT32 value;

	current_group = get_current_group(pwm_param->channel);
	
	/*value = REG_READ(REG_PWM_GROUP_CTRL_ADDR(current_group));
	if (value & (PWM_GROUP_PWM_CFG_UPDATA_MASK(current_channel)))
	{
		bk_printf("[bk_error]:wait updata pwm param\r\n");
		return 0;
	}*/
	
	if(current_channel == 0)
	{
		value = (UINT32) pwm_param->duty_cycle1;
		REG_WRITE(REG_GROUP_PWM0_T1_ADDR(current_group), value);
		
		value = (UINT32) pwm_param->duty_cycle2;
		REG_WRITE(REG_GROUP_PWM0_T2_ADDR(current_group), value);
		
		value = (UINT32) pwm_param->duty_cycle3;
		REG_WRITE(REG_GROUP_PWM0_T3_ADDR(current_group), value);
		
		value = (UINT32) pwm_param->end_value;
		REG_WRITE(REG_GROUP_PWM0_T4_ADDR(current_group), value);
	}
	else
	{
		value = (UINT32) pwm_param->duty_cycle1;
		REG_WRITE(REG_GROUP_PWM1_T1_ADDR(current_group), value);
		
		value = (UINT32) pwm_param->duty_cycle2;
		REG_WRITE(REG_GROUP_PWM1_T2_ADDR(current_group), value);
		
		value = (UINT32) pwm_param->duty_cycle3;
		REG_WRITE(REG_GROUP_PWM1_T3_ADDR(current_group), value);
		
		value = (UINT32) pwm_param->end_value;
		REG_WRITE(REG_GROUP_PWM1_T4_ADDR(current_group), value);
	}
}

UINT8 pwm_update_param_enable(UINT8 channel)
{
	UINT32 value;
	
    if(channel >= PWM_COUNT)
    {
        return 0;
    }
			
	if (channel < 2)
    {
        current_group = 0;
		current_channel = channel;
    }
	else  if (channel < 4)
    {
        current_group = 1;
		current_channel = channel-2;
    }
	else
    {
        current_group = 2;
		current_channel = channel-4;
    }

	// bit 7/15 :cfg_updata enable
	value = REG_READ(REG_PWM_GROUP_CTRL_ADDR(current_group));
	value |= PWM_GROUP_PWM_CFG_UPDATA_MASK(current_channel);
	REG_WRITE(REG_PWM_GROUP_CTRL_ADDR(current_group), value);
}

UINT8 pwm_group_update_param_enable(UINT8 channel1,UINT8 channel2,pwm_param_t *pwm_param)
{
	UINT32 value;
	
    if(channel1 >= PWM_COUNT)
    {
        return 0;
    }
			
	if (channel1 < 2)
    {
        current_group = 0;
		current_channel = channel1;
    }
	else  if (channel1 < 4)
    {
        current_group = 1;
		current_channel = channel1-2;
    }
	else
    {
        current_group = 2;
		current_channel = channel1-4;
    }

	// bit 7/15 :cfg_updata and init_level enable 
	value = REG_READ(REG_PWM_GROUP_CTRL_ADDR(current_group));
	value &= ~ (PWM_GROUP_PWM_INT_LEVL_MASK(1)|PWM_GROUP_PWM_INT_LEVL_MASK(0));
    if(current_channel == 0) {
    	value |= PWM_GROUP_PWM_CFG_UPDATA_MASK(0) | PWM_GROUP_PWM_CFG_UPDATA_MASK(1)
    		  | (pwm_param->init_level1 << PWM_GROUP_PWM_INT_LEVL_BIT(1))
    		  | (pwm_param->init_level0 << PWM_GROUP_PWM_INT_LEVL_BIT(0));
    } else {
        value |= PWM_GROUP_PWM_CFG_UPDATA_MASK(0) | PWM_GROUP_PWM_CFG_UPDATA_MASK(1)
              | (pwm_param->init_level0 << PWM_GROUP_PWM_INT_LEVL_BIT(1))
              | (pwm_param->init_level1 << PWM_GROUP_PWM_INT_LEVL_BIT(0));

    }
	
	REG_WRITE(REG_PWM_GROUP_CTRL_ADDR(current_group), value);
}

UINT8 pwm_nogroup_update_param_enable(UINT8 channel1,UINT8 channel2,pwm_param_t *pwm_param)
{
	UINT32 value1,value2,group1,group2,chan1,chan2;

	group1= get_current_group(channel1);
	chan1 = current_channel;
	
	group2= get_current_group(channel2);
	chan2 = current_channel;

	// bit 7/15 :cfg_updata and init_level enable 
	value1 = REG_READ(REG_PWM_GROUP_CTRL_ADDR(group1));
	value2 = REG_READ(REG_PWM_GROUP_CTRL_ADDR(group2));
	
	value1 &= ~ PWM_GROUP_PWM_INT_LEVL_MASK(chan1);
	value1 |= PWM_GROUP_PWM_CFG_UPDATA_MASK(chan1)
		   | (pwm_param->init_level0 << PWM_GROUP_PWM_INT_LEVL_BIT(chan1));

	value2 &= ~ PWM_GROUP_PWM_INT_LEVL_MASK(chan2);
	value2 |= PWM_GROUP_PWM_CFG_UPDATA_MASK(chan2)
		   | (pwm_param->init_level1 << PWM_GROUP_PWM_INT_LEVL_BIT(chan2));		   
	
	REG_WRITE(REG_PWM_GROUP_CTRL_ADDR(group1), value1);
	REG_WRITE(REG_PWM_GROUP_CTRL_ADDR(group2), value2);
}

UINT8 pwm_param_clear(UINT8 ucChannel)
{	
	if (ucChannel < 2)
    {
        current_group = 0;
		current_channel = ucChannel;
    }
	else  if (ucChannel < 4)
    {
        current_group = 1;
		current_channel = ucChannel-2;
    }
	else
    {
        current_group = 2;
		current_channel = ucChannel-4;
    }

	
	if(current_channel == 0)
	{
		REG_WRITE(REG_GROUP_PWM0_T1_ADDR(current_group), 0);	
		REG_WRITE(REG_GROUP_PWM0_T2_ADDR(current_group), 0);
		REG_WRITE(REG_GROUP_PWM0_T3_ADDR(current_group), 0);
		REG_WRITE(REG_GROUP_PWM0_T4_ADDR(current_group), 0);
	}
	else
	{
		REG_WRITE(REG_GROUP_PWM1_T1_ADDR(current_group), 0);	
		REG_WRITE(REG_GROUP_PWM1_T2_ADDR(current_group), 0);
		REG_WRITE(REG_GROUP_PWM1_T3_ADDR(current_group), 0);
		REG_WRITE(REG_GROUP_PWM1_T4_ADDR(current_group), 0);
	}
	
}


static void pwm_int_handler_clear(UINT8 ucChannel)
{
    p_PWM_Int_Handler[ucChannel] = NULL;
}

void pwm_init(void)
{
	REG_WRITE(REG_PWM_GROUP_CTRL_ADDR(0), 0x0);
	REG_WRITE(REG_PWM_GROUP_CTRL_ADDR(1), 0x0);
	REG_WRITE(REG_PWM_GROUP_CTRL_ADDR(2), 0x0);

    intc_service_register(IRQ_PWM, PRI_IRQ_PWM, pwm_isr);

    sddev_register_dev(PWM_DEV_NAME, &pwm_op);
}

void pwm_exit(void)
{
    sddev_unregister_dev(PWM_DEV_NAME);
}


UINT32 pwm_ctrl(UINT32 cmd, void *param)
{
    UINT32 ret = PWM_SUCCESS;
    UINT32 ucChannel;
    UINT32 value;
    pwm_param_t *p_param;
    pwm_capture_t *p_capture;

    switch(cmd)
    {
    case CMD_PWM_UNIT_ENABLE:
        ucChannel = (*(UINT32 *)param);
        if(ucChannel > 5)
        {
            ret = PWM_FAILURE;
            break;
        }
		pwm_unit_enable(ucChannel);
        break;
    case CMD_PWM_UNIT_DISABLE:
        ucChannel = (*(UINT32 *)param);
        if(ucChannel > 5)
        {
            ret = PWM_FAILURE;
            break;
        }
		pwm_unit_disable(ucChannel);
		pwm_param_clear(ucChannel);
        break;
    case CMD_PWM_IR_ENABLE:

        break;
    case CMD_PWM_IR_DISABLE:

        break;
    case CMD_PWM_IR_CLEAR:
        ucChannel = (*(UINT32 *)param);
        if(ucChannel > 5)
        {
            ret = PWM_FAILURE;
            break;
        }
        pwm_int_handler_clear(ucChannel);
        break;
    case CMD_PWM_INIT_PARAM:
        p_param = (pwm_param_t *)param;
        init_pwm_param(p_param, 1);
        break;
    case CMD_PWM_SET_DUTY_CYCLE:
        p_param = (pwm_param_t *)param;
        if(p_param->channel >= PWM_COUNT)
        {
            ret = PWM_FAILURE;
            break;
        }
        pwm_set_duty_cycle(p_param->channel, p_param->duty_cycle1);
        break;
    case CMD_PWM_SET_END_VALUE:
        p_param = (pwm_param_t *)param;
        if(p_param->channel >= PWM_COUNT)
        {
            ret = PWM_FAILURE;
            break;
        }
        pwm_set_end_value(p_param->channel, p_param->end_value);
        break;
    case CMD_PWM_CAP_GET:
        p_capture = (pwm_capture_t *)param;
        if(p_capture->ucChannel >= PWM_COUNT)
        {
            ret = PWM_FAILURE;
            break;
        }
        p_capture->value = pwm_capture_value_get(p_capture->ucChannel);
        break;
	case CMD_PWM_DEINIT_PARAM:
        p_param = (pwm_param_t *)param;
        init_pwm_param(p_param, 0);
		break;
	case CMD_PWM_GROUP_ENABLE:
        ucChannel = (*(UINT32 *)param);
        if(ucChannel > 5)
        {
            ret = PWM_FAILURE;
            break;
        }
		pwm_group_mode_enable(ucChannel);
        break;
	case CMD_PWM_GROUP_DISABLE:
        ucChannel = (*(UINT32 *)param);
        if(ucChannel > 5)
        {
            ret = PWM_FAILURE;
            break;
        }
		pwm_group_mode_disable(ucChannel);
        break;
	case CMD_PWM_UPDATA_PARAM:
        p_param = (pwm_param_t *)param;
		pwm_update_param(p_param);
        break;
	case CMD_PWM_UPDATA_PARAM_ENABLE:
        ucChannel = (*(UINT32 *)param);
        if(ucChannel > 5)
        {
            ret = PWM_FAILURE;
            break;
        }
		pwm_update_param_enable(p_param);
        break;
	case CMD_PWM_INIT_LEVL_SET_LOW:
        ucChannel = (*(UINT32 *)param);
        if(ucChannel > 5)
        {
            ret = PWM_FAILURE;
            break;
        }
		pwm_init_levl_set_low(ucChannel);
        break;	
	case CMD_PWM_INIT_LEVL_SET_HIGH:
        ucChannel = (*(UINT32 *)param);
        if(ucChannel > 5)
        {
            ret = PWM_FAILURE;
            break;
        }
		pwm_init_levl_set_high(ucChannel);
        break;	
    default:
        ret = PWM_FAILURE;
        break;
    }

    return ret;
}

//note:pwm mode no isr
void pwm_isr(void)
{
    int i;
    UINT32 status;

    status = REG_READ(REG_PWM_GROUP_CTRL_ADDR(current_group) );

    for(i = 0; i < 3; i++)
    {
        if(status &  PWM_GROUP_PWM_INT_STAT_MASK(i))
        {
            if(p_PWM_Int_Handler[i])
            {
                p_PWM_Int_Handler[i]((UINT8)i);
                do
                {
                    REG_WRITE(REG_PWM_GROUP_CTRL_ADDR(current_group), PWM_GROUP_PWM_INT_STAT_CLEAR(i));
                }
                while(REG_READ(REG_PWM_GROUP_CTRL_ADDR(current_group)) & PWM_GROUP_PWM_INT_STAT_MASK(i));
            }
        }
    }
    REG_WRITE(REG_PWM_GROUP_CTRL_ADDR(current_group), status);
}

#endif
