/**
 ******************************************************************************
 * @file    BkDriverPwm.h
 * @brief   This file provides all the headers of PWM operation functions.
 ******************************************************************************
 *
 *  The MIT License
 *  Copyright (c) 2017 BEKEN Inc.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is furnished
 *  to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
 *  IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 ******************************************************************************
 */

#include <string.h>

#include "include.h"
#include "rtos_pub.h"
#include "BkDriverPwm.h"
#include "drv_model_pub.h"
#include "rtos_error.h"
#include "pwm_pub.h"
#include "uart_pub.h"

#if (CFG_SOC_NAME != SOC_BK7231N)
OSStatus bk_pwm_initialize(bk_pwm_t pwm, uint32_t frequency, uint32_t duty_cycle)
{
    UINT32 ret;
    pwm_param_t param;

    /*init pwm*/
    param.channel         = (uint8_t)pwm;

    param.cfg.bits.en     = PWM_INT_DIS;
    param.cfg.bits.int_en = PWM_INT_DIS;//PWM_INT_EN;

    param.cfg.bits.mode   = PWM_PWM_MODE;
    param.cfg.bits.clk    = PWM_CLK_26M;
    param.p_Int_Handler   = 0;
    param.duty_cycle      = duty_cycle;
    param.end_value       = frequency;  // ?????

    ret = sddev_control(PWM_DEV_NAME, CMD_PWM_INIT_PARAM, &param);
    ASSERT(PWM_SUCCESS == ret);
    return kNoErr;
}

OSStatus bk_pwm_update_param(bk_pwm_t pwm, uint32_t frequency, uint32_t duty_cycle)
{
    UINT32 ret;
    pwm_param_t param;

    /*init pwm*/
    param.channel         = (uint8_t)pwm;
    param.cfg.bits.en     = PWM_INT_EN;
    param.cfg.bits.int_en = PWM_INT_DIS;//PWM_INT_EN;
    param.cfg.bits.mode   = PWM_PWM_MODE;
    param.cfg.bits.clk    = PWM_CLK_26M;
    param.p_Int_Handler   = 0;
    param.duty_cycle      = duty_cycle;
    param.end_value       = frequency;  // ?????

    ret = sddev_control(PWM_DEV_NAME, CMD_PWM_INIT_PARAM, &param);
    ASSERT(PWM_SUCCESS == ret);

    return kNoErr;
}

OSStatus bk_pwm_capture_initialize(bk_pwm_t pwm, uint8_t cap_mode)
{
    UINT32 ret;
    pwm_param_t param;

    /*init pwm*/
    param.channel         = (uint8_t)pwm;
    param.cfg.bits.en     = PWM_INT_EN;
    param.cfg.bits.int_en = PWM_INT_EN;
    
    param.cfg.bits.clk    = PWM_CLK_26M;
    param.p_Int_Handler   = 0;
    param.duty_cycle      = 0;
    param.end_value       = 0;
    if(cap_mode == 0x01)
    {
        param.cfg.bits.mode   = PWM_CAP_POS_MODE;
    }
    else if(cap_mode == 0x02)
    {
        param.cfg.bits.mode   = PWM_CAP_NEG_MODE;
    }
    else
    {
        param.cfg.bits.mode   = PWM_PWM_MODE;
    }

    ret = sddev_control(PWM_DEV_NAME, CMD_PWM_INIT_PARAM, &param);
    ASSERT(PWM_SUCCESS == ret);

    return kNoErr;
}

#else
#include "pwm_new.h"

OSStatus bk_pwm_initialize(bk_pwm_t pwm, uint32_t frequency, uint32_t duty_cycle1,uint32_t duty_cycle2,uint32_t duty_cycle3)
{
    UINT32 ret;
    pwm_param_t param;

    /*init pwm*/
    param.channel         = (uint8_t)pwm;
    param.cfg.bits.int_en = PWM_INT_DIS;
    param.cfg.bits.mode   = PWM_PWM_MODE;
    param.cfg.bits.clk    = PWM_CLK_26M;
    param.p_Int_Handler   = 0;
    param.duty_cycle1     = duty_cycle1;
	param.duty_cycle2     = duty_cycle2;
	param.duty_cycle3     = duty_cycle3;
    param.end_value       = frequency;  

	if(!duty_cycle1) {
        bk_pwm_initlevl_set_low(pwm);
    } else {
        bk_pwm_initlevl_set_high(pwm);
    }

	bk_printf("bk pwm initial:mode = %x\r\n",param.cfg.val);
	bk_printf("bk pwm initial:duty_cycle1 = %x\r\n",duty_cycle1);
    ret = sddev_control(PWM_DEV_NAME, CMD_PWM_INIT_PARAM, &param);
    ASSERT(PWM_SUCCESS == ret);
	
    return kNoErr;
}

OSStatus bk_pwm_capture_initialize(bk_pwm_t pwm, uint8_t cap_mode)
{
    UINT32 ret;
    pwm_param_t param;

    /*init pwm*/
    param.channel         = (uint8_t)pwm;
    param.cfg.bits.int_en = PWM_INT_EN;
    param.cfg.bits.clk    = PWM_CLK_26M;
    param.p_Int_Handler   = 0;
    param.duty_cycle1     = 0;
	param.duty_cycle2     = 0;
	param.duty_cycle3     = 0;
    param.end_value       = 0;
    param.cfg.bits.mode   = cap_mode;
    
    ret = sddev_control(PWM_DEV_NAME, CMD_PWM_INIT_PARAM, &param);
    ASSERT(PWM_SUCCESS == ret);

    return kNoErr;
}
#endif


OSStatus bk_pwm_start(bk_pwm_t pwm)
{
    UINT32 ret;
    UINT32 param;

    param = pwm;

    ret = sddev_control(PWM_DEV_NAME, CMD_PWM_UNIT_ENABLE, &param);
    ASSERT(PWM_SUCCESS == ret);

    return kNoErr;
}

OSStatus bk_pwm_stop(bk_pwm_t pwm)
{
    UINT32 ret;
    UINT32 param;

    param = pwm;
    ret = sddev_control(PWM_DEV_NAME, CMD_PWM_UNIT_DISABLE, &param);
    ASSERT(PWM_SUCCESS == ret);

    return kNoErr;
}

#if (CFG_SOC_NAME == SOC_BK7231N)

static UINT8 group_flag=0;
static UINT8 pwm1_set_high_flag=0;
static UINT8 pwm2_set_low_flag=0;

OSStatus bk_pwm_group_initialize(bk_pwm_t pwm1, bk_pwm_t pwm2,uint32_t frequency, uint32_t duty_cycle1,uint32_t duty_cycle2,uint32_t dead_band)
{
    pwm_param_t param;
	if(pwm1 >= pwm2)
	{
		bk_printf("pwm channel:%d param error\r\n", pwm2);
		return kParamErr;
	}

    /*init pwm1*/
	param.channel = pwm1 ;
    param.cfg.bits.int_en = PWM_INT_DIS;
    param.cfg.bits.mode   = PWM_PWM_MODE;
    param.cfg.bits.clk    = PWM_CLK_26M;
    param.p_Int_Handler   = 0;
	if (duty_cycle2) {
		param.duty_cycle1 = duty_cycle1 + dead_band + duty_cycle2;
		param.duty_cycle2 = duty_cycle1 + dead_band;
		param.duty_cycle3 = 0;
	}
	else {
		param.duty_cycle1 = 0;
		param.duty_cycle2 = 0;
		param.duty_cycle3 = 0;
	}
    param.end_value       = frequency;  

	init_pwm_param(&param, 1);
	pwm_init_levl_set_low(pwm1);
	
    /*init pwm2*/
	param.channel = pwm2 ;
    param.cfg.bits.int_en = PWM_INT_DIS;
    param.cfg.bits.mode   = PWM_PWM_MODE;
    param.cfg.bits.clk    = PWM_CLK_26M;
    param.p_Int_Handler   = 0;
	if (duty_cycle1) {
		param.duty_cycle1 = duty_cycle1;
		param.duty_cycle2 = duty_cycle1;
		param.duty_cycle3 = 0;
	}
	else {
		param.duty_cycle1 = 0;
		param.duty_cycle2 = 0;
		param.duty_cycle3 = 0;
	}
    param.end_value       = frequency;  

	init_pwm_param(&param, 1);
	if (duty_cycle1) {
		pwm_init_levl_set_high(pwm2);
	}
	else {
		pwm_init_levl_set_low(pwm2);
	}
	 
	if((pwm1+1) == pwm2)
	{
		//grounp enable
		pwm_group_mode_enable(pwm1);
	}
	else
	{
		pwm_unit_enable(pwm1);
		pwm_unit_enable(pwm2);
	}
	
    return kNoErr;
}

OSStatus bk_pwm_update_param(bk_pwm_t pwm, uint32_t frequency, uint32_t duty_cycle1, uint32_t duty_cycle2, uint32_t duty_cycle3)
{
    UINT32 ret;
	UINT32 init_level = 0;
    pwm_param_t param;

	param.channel         = (uint8_t)pwm;
    param.duty_cycle1	  = duty_cycle1;
	param.duty_cycle2     = duty_cycle2;
	param.duty_cycle3     = duty_cycle3;
    param.end_value       = frequency;  
	

    if(!duty_cycle1) {
        init_level = 0;
    } else {
        init_level = 1;

    }
    
    ret = sddev_control(PWM_DEV_NAME, CMD_PWM_SINGLE_UPDATA_PARAM, &param);

	pwm_single_update_param_enable(pwm,init_level);
	(void)ret;

    return kNoErr;
}

OSStatus bk_pwm_group_mode_enable(bk_pwm_t pwm)
{
    UINT32 ret;
    UINT32 param;

    param = pwm;
    ret = sddev_control(PWM_DEV_NAME, CMD_PWM_GROUP_ENABLE, &param);
    ASSERT(PWM_SUCCESS == ret);

    return kNoErr;
}

OSStatus bk_pwm_group_mode_disable(bk_pwm_t pwm)
{
    UINT32 ret;
    UINT32 param;

    param = pwm;
    ret = sddev_control(PWM_DEV_NAME, CMD_PWM_GROUP_DISABLE, &param);
    ASSERT(PWM_SUCCESS == ret);

    return kNoErr;
}

OSStatus bk_pwm_initlevl_set_low(bk_pwm_t pwm)
{
    UINT32 ret;
    UINT32 param;

    param = pwm;
    ret = sddev_control(PWM_DEV_NAME, CMD_PWM_INIT_LEVL_SET_LOW, &param);
    ASSERT(PWM_SUCCESS == ret);

    return kNoErr;
}

OSStatus bk_pwm_initlevl_set_high(bk_pwm_t pwm)
{
    UINT32 ret;
    UINT32 param;

    param = pwm;
    ret = sddev_control(PWM_DEV_NAME, CMD_PWM_INIT_LEVL_SET_HIGH, &param);
    ASSERT(PWM_SUCCESS == ret);

    return kNoErr;
}

UINT32 bk_pwm_get_capvalue(bk_pwm_t pwm)
{
    UINT32 ret;
	
    pwm_capture_t pwm_cap;
	
    pwm_cap.ucChannel = pwm;
	
    ret = sddev_control(PWM_DEV_NAME, CMD_PWM_CAP_GET, (VOID *)&pwm_cap);
    ASSERT(PWM_SUCCESS == ret);

    return pwm_cap.value;
}

OSStatus bk_pwm_cw_initialize(bk_pwm_t pwm1, bk_pwm_t pwm2,uint32_t frequency, uint32_t duty_cycle1,uint32_t duty_cycle2,  uint32_t dead_band)
{
    pwm_param_t param;

    memset(&param, 0, sizeof(pwm_param_t));

	group_flag = pwm_check_group(pwm1,pwm2);

    /*init pwm1*/
	param.channel = pwm1 ;
    param.cfg.bits.int_en = PWM_INT_DIS;
    param.cfg.bits.mode   = PWM_PWM_MODE;
    param.cfg.bits.clk    = PWM_CLK_26M;
    param.p_Int_Handler   = 0;
    param.duty_cycle1     = frequency -duty_cycle1-dead_band;
	param.duty_cycle2     = frequency  - dead_band;
	param.duty_cycle3     = 0;
    param.end_value       = frequency;  

	init_pwm_param(&param, 1);
	pwm_init_levl_set_low(pwm1);
	
    /*init pwm2*/
	param.channel = pwm2 ;
    param.cfg.bits.int_en = PWM_INT_DIS;
    param.cfg.bits.mode   = PWM_PWM_MODE;
    param.cfg.bits.clk    = PWM_CLK_26M;
    param.p_Int_Handler   = 0;
    param.duty_cycle1     = duty_cycle2;
	param.duty_cycle2     = frequency;
	param.duty_cycle3     = 0;
    param.end_value       = frequency;  

	init_pwm_param(&param, 1);

	if(duty_cycle2 ==0)
	{
		pwm2_set_low_flag = 1;
		bk_pwm_initlevl_set_low(pwm2);

	}
	else
	{
		pwm2_set_low_flag = 0;
		pwm_init_levl_set_high(pwm2);
	}

	if(duty_cycle1 ==frequency)
	{
		pwm1_set_high_flag = 1;
		bk_pwm_initlevl_set_high(pwm1);

	}
	else
	{
		pwm1_set_high_flag = 0;
		pwm_init_levl_set_low(pwm1);
	}
	 
    return kNoErr;
}

OSStatus bk_pwm_cw_start(bk_pwm_t pwm1, bk_pwm_t pwm2)
{
	if(group_flag == 1)
	{
		//grounp enable
		pwm_group_mode_enable(pwm1);
	}
	else
	{
		pwm_unit_enable(pwm1);
		pwm_unit_enable(pwm2);
	}
	
	return kNoErr;
}

OSStatus bk_pwm_cw_stop(bk_pwm_t pwm1, bk_pwm_t pwm2)
{
	if(group_flag == 1)
	{
		//grounp enable
		pwm_group_mode_disable(pwm1);
	}
	else
	{
		pwm_unit_disable(pwm1);
		pwm_unit_disable(pwm2);
	}
	
	return kNoErr;
}

OSStatus bk_pwm_cw_update_param(bk_pwm_t pwm1, bk_pwm_t pwm2,uint32_t frequency, uint32_t duty_cycle1,uint32_t duty_cycle2,  uint32_t dead_band)
{
    UINT32 ret = 0;
    pwm_param_t param;
	
	if(frequency <(duty_cycle1 + duty_cycle2 + 2*dead_band))
	{
		bk_printf("pwm param set error:freq:%x ,cycle1:%x,cycle2:%x,dead_band:%x\r\n",frequency, duty_cycle1, duty_cycle2, dead_band);
		return -1;
	}

	if(duty_cycle2 <= 0)
	{
		duty_cycle2 = 0;
	}
	
	if(duty_cycle1 <= 0)
	{
		duty_cycle1 = 0;
		dead_band = 0;
	}

	//bk_printf("pwm channel :%d :freq:%x ,cycle1:%x,cycle2:%x,dead_band:%x\r\n",pwm1,frequency, duty_cycle1, duty_cycle2, dead_band);
	group_flag = pwm_check_group(pwm1,pwm2);

	/*init pwm1*/
	param.channel 		= pwm1 ;
    param.duty_cycle1   = frequency -duty_cycle1-dead_band;
	param.duty_cycle2   = frequency  - dead_band;
	param.duty_cycle3   = 0;
    param.end_value     = frequency;  
	
	ret = sddev_control(PWM_DEV_NAME, CMD_PWM_UPDATA_PARAM, &param);

    /*init pwm2*/
	param.channel 		= pwm2 ;
	param.duty_cycle1	= duty_cycle2;
	param.duty_cycle2	= frequency;
	param.duty_cycle3	= 0;
    param.end_value		= frequency;  
	
    ret = sddev_control(PWM_DEV_NAME, CMD_PWM_UPDATA_PARAM, &param);

	if(pwm1_set_high_flag == 1)
	{
		if(duty_cycle1 == frequency)
		{
			pwm1_set_high_flag = 1;
			param.init_level0 = 1;
		}
		else
		{
			pwm1_set_high_flag = 0;
			param.init_level0 = 0;
		}
	}	

	if(pwm2_set_low_flag == 1)
	{
		if(duty_cycle2 == 0)
		{
			pwm2_set_low_flag = 1;
			param.init_level1 = 0 ;
		}
		else
		{
			pwm2_set_low_flag=0;
			param.init_level1 = 1 ;
		}		
	}

	if(duty_cycle2 == 0)
	{	
		if(pwm_init_levl_get(pwm2) ==1)
		{
			pwm2_set_low_flag = 1;
			param.init_level1 = 0 ;
		}	
	}
	else
	{
		param.init_level1 = 1 ;
	}
	
	if(duty_cycle1 == frequency)
	{
		if(pwm_init_levl_get(pwm1) ==0)
		{
			pwm1_set_high_flag = 1;
			param.init_level0 = 1;
		}
	}
	else
	{
		param.init_level0 = 0;
	}
	
	if(group_flag)
	{
		pwm_group_update_param_enable(pwm1,pwm2,&param);
	}
	else
	{
		pwm_nogroup_update_param_enable(pwm1,pwm2,&param);
	}
	
    return ret;
}

#endif
// eof

