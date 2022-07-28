/**
* @file light_prod.h
* @author www.tuya.com
* @brief light production test proc
* @version 0.2
* @date 2022-03-25
*
* @copyright Copyright (c) tuya.inc 2022
*
*/

#ifndef __LIHGT_PROD_H__
#define __LIHGT_PROD_H__


#include "light_types.h"
#include "light_cfg.h"
#include "light_set_color.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

// Weak signal breath display change time(unit ms)
#define WEAK_SINGAL_BREATHR_TIME_INTERVAL           1500
// Weak signal on/off display change time(unit ms)
#define WEAK_SINGAL_ONOFF_TIME_INTERVAL             250
// Weak signal on/off blink cnt
#define WEAK_SINGAL_ONOFF_CNT                       2


// Production detected signal weak judge value
#define AUZ_TEST_WEAK_SIGNAL                        -60 
// Production unauthorized display on/off time(unit ms)
#define AUZ_TEST_FAIL_TIME_INTERVAL                 3000    

// Productoin oeverall function single led blink time(unit ms)
#define PORD_CHECK_ONOF_TIMER_INTERVAL              1000
// Production oberall fuction test time(unit ms)
#define PROD_CHECK_TIMER_INTERVAL                   (2*60*1000)


// Production aging restart signle led blink timer(unit ms)
#define PROD_AGING_START_ONOFF_TIMER_INTERVAL       500
// Production aging restart loop cnt()
#define PROD_AGING_START_LOOP_CNT                   5


// Production repeat test signle led up/down timer(unit ms)
#define PROD_REPEAT_ONOFF_TIMER_INTERVAL        1000

/**
 * @brief: Production factory step
 */
typedef BYTE_T PROD_STEP_E;
#define PROD_CHECK      0
#define PROD_AGING      1
#define PROD_REPEAT     2


/**
 * @brief Light way type enum 
 *          1~5 ways
 */
typedef BYTE_T PROD_LIGHT_WAY_E;
#define PROD_LIGHT_C        1
#define PROD_LIGHT_CW       2
#define PROD_LIGHT_RGB      3
#define PROD_LIGHT_RGBC     4
#define PROD_LIGHT_RGBCW    5
#define PROD_LIGHT_MAX      6


/**
 * @brief Light control drive mode enum
 *          BRIGHT_MODE_CW  -> CW drive by pwm totally
 *          BRIGHT_MODE_CCT -> C value is bright setting essentially , w is the scale of C&W .
 *                          the light will send warm, when w is set by zero.
 */
typedef BYTE_T PORD_BRIGHT_MODE_E;
#define PROD_MODE_CW        0
#define PROD_MODE_CCT       1
#define PROD_MODE_MAX       2


#pragma pack(4)
typedef struct {
    PROD_LIGHT_WAY_E    light_way;
    PORD_BRIGHT_MODE_E  bright_mode;
    UCHAR_T             limit_cw_max;   //CW limit Max value
    UCHAR_T             limit_cw_min;   //CW limit Min value 
    UCHAR_T             limit_rgb_max;  //RGB limit Max value 
    UCHAR_T             limit_rgb_min;  //RGB limit Max value 

#if (LIGHT_CFG_PROD_DRIVER_NEED_INIT == 1)
    driver_config_t     drive_cfg;
#endif
}prod_init_t;
#pragma pack()


/**
 * @brief: prodution test process cb
 * @param {BOOL_T bAuthorized -> authorized flag}
 * @param {CHAR_T cSignalStrength -> singal strength}
 * @return: none
 * @retval: none
 */
VOID tuya_prod_test_callback(BOOL_T authorized, CHAR_T signal_strength);


/**
 * @brief: 产测初始化接口
 * @param: none
 * @retval: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_light_prod_init(VOID);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif  /* __LIHGT_PROD_H__ */
