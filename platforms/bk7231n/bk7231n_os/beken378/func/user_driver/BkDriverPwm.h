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

#ifndef __BEKENDRIVERPWM_H__
#define __BEKENDRIVERPWM_H__

#pragma once
#include "include.h"
#include "rtos_pub.h"

/** @addtogroup BK_PLATFORM
  * @{
  */

/** @defgroup BK_PWM _BK_ PWM Driver
  * @brief  Pulse-Width Modulation (PWM) Functions
  * @{
  */

/******************************************************
 *                   Enumerations
 ******************************************************/
typedef enum
{
    BK_PWM_0,
    BK_PWM_1,
    BK_PWM_2,
    BK_PWM_3,
    BK_PWM_4,
    BK_PWM_5,
    BK_PWM_MAX, /* Denotes the total number of PWM port aliases. Not a valid PWM alias */
    BK_PWM_NONE,
} bk_pwm_t;

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
*                 Function Declarations
******************************************************/


#if (CFG_SOC_NAME != SOC_BK7231N)
/**@brief Initialises a PWM pin
 *
 * @note  Prepares a Pulse-Width Modulation pin for use.
 * Does not start the PWM output (use @ref bk_pwm_start).
 *
 * @param pwm        : the PWM interface which should be initialised
 * @param frequency  : Output signal frequency in Hertz
 * @param duty_cycle : Duty cycle of signal as a floating-point percentage (0.0 to 100.0)
 *
 * @return    kNoErr        : on success.
 * @return    kGeneralErr   : if an error occurred with any step
 */
OSStatus bk_pwm_initialize(bk_pwm_t pwm, uint32_t frequency, uint32_t duty_cycle);

OSStatus bk_pwm_capture_initialize(bk_pwm_t pwm, uint8_t cap_mode);

#else
/**@brief Initialises a PWM pin
 *
 * @note  Prepares a Pulse-Width Modulation pin for use.
 * Does not start the PWM output (use @ref bk_pwm_start).
 *
 * @param pwm         : the PWM interface which should be initialised
 * @param frequency   : Output signal frequency in Hertz
 * @param duty_cycle1 : Set pwm first level reversal time
 * @param duty_cycle2 : Set pwm 2nd level reversal time
 * @param duty_cycle3 : Set pwm 3th level reversal time
 
 * @return    kNoErr        : on success.
 * @return    kGeneralErr   : if an error occurred with any step
 */
 
OSStatus bk_pwm_initialize(bk_pwm_t pwm, uint32_t frequency, uint32_t duty_cycle1,uint32_t duty_cycle2,uint32_t duty_cycle3);


OSStatus bk_pwm_capture_initialize(bk_pwm_t pwm, uint8_t cap_mode);

#endif

/**@brief Starts PWM output on a PWM interface
 *
 * @note  Starts Pulse-Width Modulation signal output on a PWM pin
 *
 * @param pwm        : the PWM interface which should be started
 *
 * @return    kNoErr        : on success.
 * @return    kGeneralErr   : if an error occurred with any step
 */
OSStatus bk_pwm_start(bk_pwm_t pwm);


/**@brief Stops output on a PWM pin
 *
 * @note  Stops Pulse-Width Modulation signal output on a PWM pin
 *
 * @param pwm        : the PWM interface which should be stopped
 *
 * @return    kNoErr        : on success.
 * @return    kGeneralErr   : if an error occurred with any step
 */
OSStatus bk_pwm_stop(bk_pwm_t pwm);


/**@brief Initialises a PWM pin
 *
 * @note  Update pwm cycle and duty_cycle when pwm working.
 *
 * @param pwm         : the PWM interface which should be initialised
 * @param frequency   : Output signal frequency in Hertz
 * @param duty_cycle1 : Set pwm first level reversal time
 * @param duty_cycle2 : Set pwm 2nd level reversal time
 * @param duty_cycle3 : Set pwm 3th level reversal time
 
 * @return    kNoErr        : on success.
 * @return    kGeneralErr   : if an error occurred with any step
 */


#if (CFG_SOC_NAME == SOC_BK7231N)

/**@brief Set PWM as group output
 *
 * @note  Start pwm group mode ,pwm0/1  pwm2/3  pwm4/5 
 *
 * @param pwm1        		: the PWM1 interface which should be started
 * @param  pwm2        		: the PWM2 interface which should be started
 * @param frequency         : pwm frequency
 * @param duty_cycle1       : set first level change time
 * @param duty_cycle2       : set 2nd level change time
  * @param dead_band        : set pwm grounp dead band time
 *
 * @return    kNoErr        : on success.
 * @return    kGeneralErr   : if an error occurred with any step
 */

OSStatus bk_pwm_group_initialize(bk_pwm_t pwm1,  bk_pwm_t pwm2, uint32_t frequency, uint32_t duty_cycle1,uint32_t duty_cycle2, uint32_t dead_band);

OSStatus bk_pwm_update_param(bk_pwm_t pwm, uint32_t frequency, uint32_t duty_cycle1, uint32_t duty_cycle2, uint32_t duty_cycle3);


/**@brief Starts PWM output on a PWM interface
 *
 * @note   Start pwm group mode ,when set pwm0 pwm0/pwm1 as a group output-levl:pwm0:high pwm1-low
 *
 * @param pwm        : the PWM interface which should be started
 *
 * @return    kNoErr        : on success.
 * @return    kGeneralErr   : if an error occurred with any step
 */
OSStatus bk_pwm_group_mode_enable(bk_pwm_t pwm);



/**@brief Starts PWM output on a PWM interface
 *
 * @note   Disable pwm group mode
 *
 * @param pwm        : the PWM interface which should be started
 *
 * @return    kNoErr        : on success.
 * @return    kGeneralErr   : if an error occurred with any step
 */
OSStatus bk_pwm_group_mode_disable(bk_pwm_t pwm);



/**@brief Starts PWM output on a PWM interface
 *
 * @note   Set pwm init output level as low
 *
 * @param pwm        : the PWM interface which should be started
 *
 * @return    kNoErr        : on success.
 * @return    kGeneralErr   : if an error occurred with any step
 */
OSStatus bk_pwm_initlevl_set_low(bk_pwm_t pwm);


/**@brief Starts PWM output on a PWM interface
 *
 * @note   Set pwm init output level as high
 *
 * @param pwm        : the PWM interface which should be started
 *
 * @return    kNoErr        : on success.
 * @return    kGeneralErr   : if an error occurred with any step
 */
OSStatus bk_pwm_initlevl_set_high(bk_pwm_t pwm);

UINT32 bk_pwm_get_capvalue(bk_pwm_t pwm);

OSStatus bk_pwm_cw_initialize(bk_pwm_t pwm1, bk_pwm_t pwm2,uint32_t frequency, uint32_t duty_cycle1, uint32_t duty_cycle, uint32_t dead_band);

OSStatus bk_pwm_cw_start(bk_pwm_t pwm1, bk_pwm_t pwm2);

OSStatus bk_pwm_cw_stop(bk_pwm_t pwm1, bk_pwm_t pwm2);

OSStatus bk_pwm_cw_update_param(bk_pwm_t pwm1, bk_pwm_t pwm2,uint32_t frequency, uint32_t duty_cycle1, uint32_t duty_cycle, uint32_t dead_band);

#endif




/** @} */
/** @} */

#endif
