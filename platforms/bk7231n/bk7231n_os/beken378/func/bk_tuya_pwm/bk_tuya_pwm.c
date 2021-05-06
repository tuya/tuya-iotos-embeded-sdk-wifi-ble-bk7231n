#include "include.h"
#include "icu_pub.h"
#include "pwm_pub.h"
#include "uart_pub.h"
#include "bk_tuya_pwm.h"
#include "drv_model_pub.h"
#if (CFG_SOC_NAME == SOC_BK7231N)
#include "BkDriverPwm.h"
#endif

UINT32 bk_cw_pwm_init(uint8 channel_num, uint32 end_value, uint32 duty_cycle)
{
    UINT32 ret;
#if (CFG_SOC_NAME != SOC_BK7231N)
    pwm_param_t param;

    TPWM_PRT("tuya_pwm_init: %d %ld %ld\r\n", channel_num, end_value, duty_cycle);

    *(volatile unsigned long *) (0x00802A0C) |= ( (1 << 1));

    param.channel = channel_num;
    param.cfg.bits.en = 0;
    param.cfg.bits.int_en = 0;
    param.cfg.bits.clk = 1;
    param.cfg.bits.mode = 0;
    param.end_value = end_value;
    param.duty_cycle = duty_cycle;
    param.p_Int_Handler = NULL;
    ret = sddev_control(PWM_DEV_NAME, CMD_PWM_INIT_PARAM, &param);
    ASSERT(PWM_SUCCESS == ret);
#else
    OSStatus status;
    status = bk_pwm_initialize(channel_num, end_value, duty_cycle, 0, 0);
    ret = (kNoErr == status) ? PWM_SUCCESS : PWM_FAILURE;
#endif

    return ret;
}

void bk_cw_pwm_set_duty_cycle(uint8 channel_num, uint32 duty_cycle)
{
    UINT32 ret;
    pwm_param_t param;

    TPWM_PRT("tuya_pwm_set_duty_cycle: %d %ld\r\n", channel_num, duty_cycle);

    param.channel = channel_num;
#if (CFG_SOC_NAME != SOC_BK7231N)
    param.duty_cycle = duty_cycle;
#else
    param.duty_cycle1 = duty_cycle;
#endif
    ret = sddev_control(PWM_DEV_NAME, CMD_PWM_SET_DUTY_CYCLE, &param);
    if (PWM_SUCCESS != ret)
    {
        TPWM_FATAL("tuya_pwm_set_duty_cycle: error param!\r\n");
    }
    ASSERT(PWM_SUCCESS == ret);
}

void bk_cw_pwm_set_end_value(uint8 channel_num, uint32 end_value)
{
    UINT32 ret;
    pwm_param_t param;

    TPWM_PRT("tuya_pwm_set_end_value: %d %ld\r\n", channel_num, end_value);

    param.channel = channel_num;
    param.end_value = end_value;
    ret = sddev_control(PWM_DEV_NAME, CMD_PWM_SET_END_VALUE, &param);
    if (PWM_SUCCESS != ret)
    {
        TPWM_FATAL("tuya_pwm_set_end_value: error param!\r\n");
    }
    ASSERT(PWM_SUCCESS == ret);
}

void bk_cw_pwm_stop(uint8 channel_num_1, uint8 channel_num_2)
{
    UINT32 ret;
    UINT32 PWM_channel;

    TPWM_PRT("tuya_pwm_stop: %d %d\r\n", channel_num_1, channel_num_2);

    PWM_channel = channel_num_1;
    ret = sddev_control(PWM_DEV_NAME, CMD_PWM_UNIT_DISABLE, &PWM_channel);
    ASSERT(PWM_SUCCESS == ret);

    PWM_channel = channel_num_2;
    ret = sddev_control(PWM_DEV_NAME, CMD_PWM_UNIT_DISABLE, &PWM_channel);
    ASSERT(PWM_SUCCESS == ret);
}

void bk_cw_pwm_reset_duty_cycle(uint8 channel_num_1, uint8 channel_num_2,
										uint32 duty_cycle_1, uint32 duty_cycle_2,
										uint32 end_value, uint32 dead_band_1)
{
#if (CFG_SOC_NAME != SOC_BK7231N)
	GLOBAL_INT_DECLARATION();
	UINT32 i_time_out1, i_time_out2, status;

	TPWM_PRT("tuya_pwm_reset_duty_cycle: %d %d %ld %ld %ld %ld\r\n", 
	channel_num_1, channel_num_2, duty_cycle_1, duty_cycle_2, end_value, dead_band_1);

	if ((channel_num_1 >= PWM_COUNT) || (channel_num_2 >= PWM_COUNT))
	{
		TPWM_FATAL("tuya_pwm_reset_duty_cycle param 1 error!\r\n");
		return;
	}

	if (end_value < duty_cycle_1 + duty_cycle_2 + dead_band_1*2)
	{
		TPWM_FATAL("tuya_pwm_reset_duty_cycle param 2 error!\r\n");
		return;
	}

	// set PWM 1/2 end value
	*(volatile unsigned long *) (0x00802A80 + 0x08 + (channel_num_1 * 3 * 4)) = end_value;
	*(volatile unsigned long *) (0x00802A80 + 0x08 + (channel_num_2 * 3 * 4)) = end_value;

	// set PWM 1/2 duty cycle
	*(volatile unsigned long *) (0x00802A80 + 0x0C + (channel_num_1 * 3 * 4)) = duty_cycle_1;
	*(volatile unsigned long *) (0x00802A80 + 0x0C + (channel_num_2 * 3 * 4)) = duty_cycle_2;


	//enable pwm1 and pwm1 int
	*(volatile unsigned long *) (0x00802040) |= (1 << 9);
	*(volatile unsigned long *) (0x00802A80) |= (1 << (channel_num_1 * 4));
	*(volatile unsigned long *) (0x00802A80) |= (1 << (channel_num_1 * 4+1));

	GLOBAL_INT_DISABLE();

	while(1)
	{  	
		//wait pwm int
		status = *(volatile unsigned long *) (0x0080204C);
		if(status & (1 << 9))
		{	
			break;
		}
	}

	// disable PWM 2 channel
	*(volatile unsigned long *) (0x00802A80) &= (~((1 << (channel_num_2 * 4))));
					   
	// cache tuya_pwm_set_duty_cycle function
	do
	{
		char *text, temp;
		int i;

		text = (char *)bk_cw_pwm_reset_duty_cycle;
		for (i = 0; i < 0x400; i ++)
		{
			temp = text[i];
		}
	} while(0);

	// delay
	{
		uint32 slot1, t;
		volatile uint32 slot2;

		*(volatile unsigned long *) (0x00802A10) = (1 << 2) | (1 << 0);
		i_time_out1 = 0;
		while ((*(volatile unsigned long *) (0x00802A10)) & (1 << 0))
		{
			i_time_out1 ++;
			if (i_time_out1 > (120 * 1000))
			{
				break;
			}
		}
		slot1 = *(volatile unsigned long *) (0x00802A14);

		//pwm int time about 5us
		t = duty_cycle_1 + dead_band_1 + slot1;
		i_time_out1 = 0;
		while (1)
		{
			*(volatile unsigned long *) (0x00802A10) = (1 << 2) | (1 << 0);
			i_time_out2 = 0;
			while ((*(volatile unsigned long *) (0x00802A10)) & (1 << 0))
			{
				i_time_out2 ++;
				if (i_time_out2 > (120 * 1000))
				{
					break;
				}
			}
			slot2 = *(volatile unsigned long *) (0x00802A14);
			if (slot2 >= t)
			{
				break;
			}

			i_time_out1 ++;
			if (i_time_out1 > (120 * 1000))
			{
				break;
			}
		}
	}

	// enable PWM channel 1/2
	*(volatile unsigned long *) (0x00802A80) |= (1 << (channel_num_1 * 4))
					   						  | (1 << (channel_num_2 * 4));
	GLOBAL_INT_RESTORE();
#endif
}
