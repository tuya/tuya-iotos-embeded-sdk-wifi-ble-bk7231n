/**
* @file light_prod.c
* @author www.tuya.com
* @brief light production test proc
* @version 0.2
* @date 2022-03-25
*
* @copyright Copyright (c) tuya.inc 2022
*
*/

#include "light_control.h"
#include "light_printf.h"
#include "light_prod.h"
#include "user_flash.h"
#include "device_config_load.h"
#include "user_timer.h"

/// Control calculate range 0 ~ 1000
#define PROD_CAL_VALUE_RANGE                        1000
/// Control cw calc max bright value
#define PROD_CW_BRIGHT_VALUE_MAX                    PROD_CAL_VALUE_RANGE
/// Control rgb calc max bright value
#define PROD_RGB_BRIGHT_VALUE_MAX                   PROD_CAL_VALUE_RANGE


/// Weak signal display change base time(unit ms)
#define WEAK_SINGAL_BASE_INTERVAL                   10

/// Unauthorized display change base time(unit ms)
#define UNAUTHOR_BASE_TIME_INTERVAL                 10

/// Production overall function check base time(unit ms)
#define PROD_CHECK_BASE_INTERVAL                    10

/// Production aging restart check base time(unit ms)
#define PROD_AGING_START_BASE_INTERVAL              10

/// Production aging check base time(unit ms)
#define PROD_AGING_BASE_INTERVAL                    500

/// Production repeat test base time(unit ms)
#define PROD_REPEAT_BASE_INERVAL                    10

/// Production overall function check can repeat timer(unit ms)
#define PROD_CHECK_REPEAT_INTERVAL                  5000

/// Production aging test C or W or RGB max aging time (unit min)
#define PROD_AGING_TIME_MAX                         250

/**
 * @brief software timer use id enum
 */
typedef BYTE_T PROD_SW_TIMER_ID_E;
#define WEAKSINGAL_SW_TIME      0
#define UNAUTHORIZED_SW_TIME    1
#define OVERALLCHECK_SW_TIME    2
#define AGINGSTART_SW_TIME      3
#define AGINGTEST_SW_TIME       4
#define REPEATTEST_SW_TIME      5
#define RESET_CNT_SW_TIME       6



/**
 * @brief Light LED enum
 */
typedef BYTE_T LIGHT_LED_E;
#define LED_R       0
#define LED_G       1
#define LED_B       2
#define LED_C       3
#define LED_W       4
#define LED_OFF     5
#define LED_MAX     6
#define LED_CW      7       //CW AGING END
#define LED_RGB     8       //RGB AGING


/**
 * @brief Light Aging time enum
 */
typedef BYTE_T LIGHT_AGING_E;
#define AGING_C     0
#define AGING_W     1
#define AGING_RGB   2
#define AGING_MAX   3


/**
 * @brief: Production weak singal display status
 */
typedef BYTE_T WEAKSINGAL_STATUS_E;
#define WS_BREATH_UP    0
#define WS_BREATH_DOWN  1
#define WS_BLINK_ON     2
#define WS_BLINK_OFF    3



STATIC prod_init_t sg_prod_config_data;    //产测配置数据

STATIC UCHAR_T sg_prod_init_flag = FALSE;   //产测初始化标志


STATIC VOID _aging_start_process(VOID);
STATIC VOID _prod_aging_process(VOID);


/**
 * @brief: 产测输出RGBCW设置
 * @param[in]: bright_led 需要点灯的LED, 支持的类型详见LIGHT_LED_E枚举值
 * @param[in]: bright_value led值 
 * @return: none 
 */
STATIC VOID _prod_set_rgbcw(LIGHT_LED_E bright_led, USHORT_T bright_value)
{

    switch (bright_led) {
    case LED_R:
        tuya_light_set_rgbcw(bright_value, 0, 0, 0, 0);
        break;

    case LED_G:
        tuya_light_set_rgbcw(0, bright_value, 0, 0, 0);
        break;

    case LED_B:
        tuya_light_set_rgbcw(0, 0, bright_value, 0, 0);
        break;

    case LED_C:
        if (PROD_MODE_CCT == sg_prod_config_data.bright_mode) {
            tuya_light_set_rgbcw(0, 0, 0, bright_value, PROD_CAL_VALUE_RANGE);    //CCT Drive mode bright C LED!
        } else {
            tuya_light_set_rgbcw(0, 0, 0, bright_value, 0);
        }
        break;

    case LED_W:
        if (PROD_MODE_CCT == sg_prod_config_data.bright_mode) {
            tuya_light_set_rgbcw(0, 0, 0, bright_value, 0);     //CCT Drive mode bright W LED!
        } else {
            tuya_light_set_rgbcw(0, 0, 0, 0, bright_value);
        }

        break;

    case LED_CW:
        if (PROD_MODE_CCT == sg_prod_config_data.bright_mode) {
            tuya_light_set_rgbcw(0, 0, 0, bright_value, PROD_CAL_VALUE_RANGE/2);  //CCT Drive mode C&W LED bright mix
        } else {
            tuya_light_set_rgbcw(0, 0, 0, bright_value, bright_value);
        }
        break;

    case LED_RGB:
        tuya_light_set_rgbcw(bright_value, bright_value, bright_value, 0, 0);
        break;

    case LED_OFF:
        tuya_light_set_rgbcw(0, 0, 0, 0, 0);    //shut down!
        break;

    case LED_MAX:
    default:
        PR_ERR("Production send led error!");
        tuya_light_set_rgbcw(0, 0, 0, 0, 0);    //shut down!
        break;
    }

	return;
}

/**
 * @brief: 产测弱信号处理回调函数
 *          display styple
 *              C     ->  C breath up -> breath down -> blink
 *              CW    ->  C breath up -> breath down -> blink
 *              RGB   ->  R breath up -> breath down -> blink
 *              RGBC  ->  C breath up -> breath down -> blink
 *              RGBCW ->  C breath up -> breath down -> blink
 *          above display base on WEAK_SINGAL_BASE_INTERVAL frequency
 * @param: none
 * @return: none
 */
STATIC VOID _weak_singal_time_callback(VOID)
{
    OPERATE_LIGHT ret = -1;
    STATIC UCHAR_T s_init_flag = FALSE;
    STATIC UCHAR_T s_status = 0;
    STATIC USHORT_T s_bright_min = 0;
    STATIC USHORT_T s_bright_max = 0;
    STATIC USHORT_T s_change_step = 0;
    STATIC SHORT_T s_send_value = 0;
    STATIC UCHAR_T s_led_list = 0;
    STATIC USHORT_T s_stand_cnt = 0;
    STATIC UCHAR_T s_onoff_cnt = 0;

    if (FALSE == s_init_flag) {
        if (sg_prod_config_data.light_way != PROD_LIGHT_RGB) {
            s_led_list = LED_C;
            s_bright_min = PROD_CW_BRIGHT_VALUE_MAX * (sg_prod_config_data.limit_cw_min / 100.0);          //calc max bright 
            s_bright_max = PROD_CW_BRIGHT_VALUE_MAX * (sg_prod_config_data.limit_cw_max / 100.0);          //calc min bright 
            s_send_value = s_bright_min;
        } else {
            s_led_list = LED_R;
            s_bright_min = PROD_RGB_BRIGHT_VALUE_MAX * (sg_prod_config_data.limit_rgb_min / 100.0);          //calc max bright 
            s_bright_max = PROD_RGB_BRIGHT_VALUE_MAX * (sg_prod_config_data.limit_rgb_max / 100.0);          //calc min bright 
            s_send_value = s_bright_min;
        }

        s_change_step = (s_bright_max - s_bright_min) / (WEAK_SINGAL_BREATHR_TIME_INTERVAL / WEAK_SINGAL_BASE_INTERVAL);
        s_init_flag = TRUE;
    }

    switch (s_status) {
    case WS_BREATH_UP:
        _prod_set_rgbcw(s_led_list, s_send_value);     //breath up
        s_send_value += s_change_step;
        if (s_send_value >= s_bright_max) {
            s_send_value = s_bright_max;
            s_status = WS_BREATH_DOWN;
        }
        break;

    case WS_BREATH_DOWN:                             //breath down
        _prod_set_rgbcw(s_led_list, s_send_value);
        s_send_value -= s_change_step;
        if (s_send_value <= s_bright_min) {   //limit min brightness
            s_send_value = s_bright_min;
            s_status = WS_BLINK_ON;
        }
        break;

    case WS_BLINK_ON:                                //blink on off
        s_stand_cnt++;
        _prod_set_rgbcw(s_led_list, s_bright_max);
        if ((s_stand_cnt * WEAK_SINGAL_BASE_INTERVAL) >= WEAK_SINGAL_ONOFF_TIME_INTERVAL) {
            s_stand_cnt = 0;
            s_status = WS_BLINK_OFF;
        }
        break;

    case WS_BLINK_OFF:
        s_stand_cnt++;
        _prod_set_rgbcw(s_led_list, 0);
        if ((s_stand_cnt * WEAK_SINGAL_BASE_INTERVAL) >= WEAK_SINGAL_ONOFF_TIME_INTERVAL) {
            if (s_onoff_cnt < (WEAK_SINGAL_ONOFF_CNT - 1)) {
                s_stand_cnt = 0;
                s_onoff_cnt++;
                s_status = WS_BLINK_ON;
            } else {
                s_stand_cnt = 0;
                s_onoff_cnt = 0;
                s_status = WS_BREATH_UP;
            }
        }
        break;

    default:
        break;
    }

    ret = tuya_user_software_timer_start(WEAKSINGAL_SW_TIME, WEAK_SINGAL_BASE_INTERVAL, _weak_singal_time_callback);
    if (LIGHT_OK != ret) {

        PR_ERR("weak signal display restart failure,light shut down!");

        ret = tuya_light_set_rgbcw(0,0,0,0,0);             //light shut down
        if (LIGHT_OK != ret) {
            PR_ERR("weak signal display restart failure,light shut down error!");
        }
    }

    return;
}

/**
 * @brief: 扫描到信标弱的提醒处理
 * @param: none
 * @return: none
 */
STATIC VOID _weak_signal_process(VOID)
{
    OPERATE_LIGHT ret = -1;
	
    ret = tuya_user_software_timer_start(WEAKSINGAL_SW_TIME, WEAK_SINGAL_BASE_INTERVAL, _weak_singal_time_callback);
    if (LIGHT_OK != ret) {

        PR_ERR("weak signal display init failure,light shut down!");

        ret = tuya_light_set_rgbcw(0,0,0,0,0);             //light shut down
        if (LIGHT_OK != ret) {
            PR_ERR("weak signal display init failure,light shut down error!");
        }
    }

	return;
}

/**
 * @brief: 未授权提醒处理回调函数
 *          display styple
 *              C     ->  C on/off mimimus brightness
 *              CW    ->  C on/off mimimus brightness
 *              RGB   ->  R on/off mimimus brightness
 *              RGBC  ->  C on/off mimimus brightness
 *              RGBCW ->  C on/off mimimus brightness
 *          above display on/off as AUZ_TEST_FAIL_TIME_INTERVAL frequency
 * @param: none
 * @return: none
 */
STATIC VOID _unauthorized_time_callback(VOID)
{
    OPERATE_LIGHT ret = -1;
    STATIC UCHAR_T s_init_flag = FALSE;
    STATIC UCHAR_T s_status = FALSE;
    STATIC UCHAR_T s_led_list = 0;
    STATIC USHORT_T s_bright_min = 0xFFFF;
    STATIC USHORT_T s_stand_cnt = 0;

    if (FALSE == s_init_flag) {
        if (sg_prod_config_data.light_way >= PROD_LIGHT_RGB) {
            s_bright_min = PROD_RGB_BRIGHT_VALUE_MAX * (sg_prod_config_data.limit_rgb_min / 100.0);         //calc min bright
            s_led_list = LED_R;
        } else {
            s_bright_min = PROD_CW_BRIGHT_VALUE_MAX * (sg_prod_config_data.limit_cw_min / 100.0);          //calc min bright
            s_led_list = LED_C;
        }

        s_init_flag = TRUE;
    }

    if (TRUE == s_status) {
        s_stand_cnt++;
        _prod_set_rgbcw(s_led_list, s_bright_min);
        if ((s_stand_cnt * UNAUTHOR_BASE_TIME_INTERVAL) >= AUZ_TEST_FAIL_TIME_INTERVAL) {
            s_stand_cnt = 0;
            s_status = FALSE;
        }
    } else {
        s_stand_cnt++;
        _prod_set_rgbcw(s_led_list, 0);
        if ((s_stand_cnt * UNAUTHOR_BASE_TIME_INTERVAL) >= AUZ_TEST_FAIL_TIME_INTERVAL) {
            s_stand_cnt = 0;
            s_status = TRUE;
        }
    }

    ret = tuya_user_software_timer_start(UNAUTHORIZED_SW_TIME, UNAUTHOR_BASE_TIME_INTERVAL, _unauthorized_time_callback);
    if (LIGHT_OK != ret) {

        PR_ERR("unauthorized display restart failure,light shut down!");

        ret = tuya_light_set_rgbcw(0,0,0,0,0);             //light shut down
        if (LIGHT_OK != ret) {
            PR_ERR("unauthorized display restart failure,light shut down!");
        }
    }

    return;
}

/**
 * @brief: 未授权提醒处理
 * @param[in]: none
 * @return: none
 */
STATIC VOID _unauthorized_process(VOID)
{
    OPERATE_LIGHT ret = -1;

    ret = tuya_user_software_timer_start(UNAUTHORIZED_SW_TIME, UNAUTHOR_BASE_TIME_INTERVAL, _unauthorized_time_callback);
    if (LIGHT_OK != ret) {

        PR_ERR("unauthorized display init failure,light shut down!");

        ret = tuya_light_set_rgbcw(0,0,0,0,0);             //light shut down
        if (LIGHT_OK != ret) {
            PR_ERR("unauthorized display init failure,light shut down!");
        }
    }

    return;
}

/**
 * @brief: 产测跑马灯处理回调函数
 *         display styple
 *              C     ->  C blink
 *              CW    ->  C > W blink
 *              RGB   ->  R > G > B blink
 *              RGBC  ->  R > G > B > C blink
 *              RGBCW ->  R > G > B > C > W blink
 *          above display base on PROD_CHECK_BASE_INTERVAL frequency
 * @param[in]: none
 * @return: none
 */
STATIC VOID _prod_check_time_callback(VOID)
{
    OPERATE_LIGHT ret = -1;
    STATIC UCHAR_T s_init_flag = FALSE;
    STATIC UCHAR_T s_led_list[5] = {0};
    STATIC UCHAR_T s_led_num = 0;
    STATIC UCHAR_T s_index = 0;
    STATIC USHORT_T s_stand_cnt = 0;
    STATIC UINT_T s_check_cnt = 0;
    STATIC USHORT_T s_cw_bright_max = 0;
    STATIC USHORT_T s_rgb_bright_max = 0;
    light_prod_test_data_flash_t prod_result = {0};

    if (FALSE == s_init_flag) {
        switch (sg_prod_config_data.light_way) {
        case PROD_LIGHT_C:
            s_led_list[0] = LED_C;
            s_led_list[1] = LED_OFF;
            break;
        case PROD_LIGHT_CW:
            s_led_list[0] = LED_C;
            s_led_list[1] = LED_W;
            break;
        case PROD_LIGHT_RGB:
            s_led_list[0] = LED_R;
            s_led_list[1] = LED_G;
            s_led_list[2] = LED_B;
            break;
        case PROD_LIGHT_RGBC:
            s_led_list[0] = LED_R;
            s_led_list[1] = LED_G;
            s_led_list[2] = LED_B;
            s_led_list[3] = LED_C;
            break;
        case PROD_LIGHT_RGBCW:
            s_led_list[0] = LED_R;
            s_led_list[1] = LED_G;
            s_led_list[2] = LED_B;
            s_led_list[3] = LED_C;
            s_led_list[4] = LED_W;
            break;
        default:
            break;
        }

        s_led_num = sg_prod_config_data.light_way;
        if (PROD_LIGHT_C == sg_prod_config_data.light_way) {
            s_led_num += 1;              //attention: when one way,blink as turn on as C and blink turn off 
        }

        s_cw_bright_max = PROD_CW_BRIGHT_VALUE_MAX * (sg_prod_config_data.limit_cw_max / 100.0);             //calc CW max bright
        s_rgb_bright_max = PROD_RGB_BRIGHT_VALUE_MAX * (sg_prod_config_data.limit_rgb_max / 100.0);          //calc RGB max bright
        s_init_flag = TRUE;
    }

    if (s_led_list[s_index] <= LED_B) {  //R\G\B color led!
        _prod_set_rgbcw(s_led_list[s_index], s_rgb_bright_max);
    } else {
        _prod_set_rgbcw(s_led_list[s_index], s_cw_bright_max);
    }

    s_stand_cnt++;
    if ((s_stand_cnt * PROD_CHECK_BASE_INTERVAL) >= PORD_CHECK_ONOF_TIMER_INTERVAL) {                //blink as PORD_CHECK_ONOF_TIMER_INTERVAL
        s_stand_cnt = 0;
        s_index++;
        if (s_index >= s_led_num) {
            s_index = 0;
        }
    }

    s_check_cnt++;
    if( (s_check_cnt * PROD_CHECK_BASE_INTERVAL) >= PROD_CHECK_TIMER_INTERVAL ) {   //overall check time over,the test mode change to aging mode!

        prod_result.aging_tested_time = 0;
        prod_result.test_mode = PROD_AGING;

        //保存灯的产测数据
        ret = tuya_user_flash_write_prod_data(&prod_result);
        if (LIGHT_OK == ret) {

            _aging_start_process();      //display goto aging process

            ret = tuya_user_software_timer_stop(OVERALLCHECK_SW_TIME);    //stop timer dominantly error
            if (ret != LIGHT_OK) {
                PR_ERR("prod check finish stop timer dominantly error!");
            }
            return;                 //don't restart OVERALLCHECK_SW_TIME software timer
        } else {
            PR_NOTICE("Write prod aging time error!");
        }
    }

    ret = tuya_user_software_timer_start(OVERALLCHECK_SW_TIME, PROD_CHECK_BASE_INTERVAL, _prod_check_time_callback);
    if (LIGHT_OK != ret) {

        PR_ERR("production overall check display init failure,light shut down!");

        ret = tuya_light_set_rgbcw(0,0,0,0,0);             //light shut down 
        if( LIGHT_OK != ret ) {
            PR_ERR("production overall check display init failure,light shut down!");
        }
    }

    return;
}

/**
 * @brief:	产测跑马灯提醒处理
 * @param: none
 * @return: none
 */
STATIC VOID _prod_check_process(VOID)
{
    OPERATE_LIGHT ret = -1;

    ret = tuya_user_software_timer_start(OVERALLCHECK_SW_TIME, PROD_CHECK_BASE_INTERVAL, _prod_check_time_callback);
    if (LIGHT_OK != ret) {

        PR_ERR("production overall check display init failure,light shut down!");

        ret = tuya_light_set_rgbcw(0,0,0,0,0);             //light shut down
        if (LIGHT_OK != ret) {
            PR_ERR("production overall check display init failure,light shut down!");
        }
    }

    return;
}

/**
 * @brief: 灯老化处理回调函数 Production factory test aging function start display timer callback
 *          display styple
 *              C     ->  C blink
 *              CW    ->  C W blink
 *              RGB   ->  R G B blink
 *              RGBC  ->  R G B C blink
 *              RGBCW ->  R G B C W blink
 *          above display base on PROD_AGING_START_ONOFF_TIMER_INTERVAL frequency
 * @param: none
 * @return: none
 */
STATIC VOID _aging_start_time_callback(VOID)
{
    OPERATE_LIGHT ret = -1;
    STATIC UCHAR_T s_init_flag = FALSE;
    STATIC UCHAR_T s_led_list[5] = {0};
    STATIC UCHAR_T s_led_num = 0;
    STATIC UCHAR_T s_index = 0;
    STATIC USHORT_T s_stand_cnt = 0;
    STATIC USHORT_T usCheckCnt = 0;
    STATIC USHORT_T s_cw_bright_max = 0;
    STATIC USHORT_T s_rgb_bright_max = 0;
    light_prod_test_data_flash_t prod_result = {0};

    if (FALSE == s_init_flag) {

        PR_NOTICE("ligth way %d", sg_prod_config_data.light_way);

        switch (sg_prod_config_data.light_way) {
        case PROD_LIGHT_C:
            s_led_list[0] = LED_C;
            s_led_list[1] = LED_OFF;
            break;

        case PROD_LIGHT_CW:
            s_led_list[0] = LED_C;
            s_led_list[1] = LED_W;
            break;

        case PROD_LIGHT_RGB:
            s_led_list[0] = LED_R;
            s_led_list[1] = LED_G;
            s_led_list[2] = LED_B;
            break;

        case PROD_LIGHT_RGBC:
            s_led_list[0] = LED_R;
            s_led_list[1] = LED_G;
            s_led_list[2] = LED_B;
            s_led_list[3] = LED_C;
            break;

        case PROD_LIGHT_RGBCW:
            s_led_list[0] = LED_R;
            s_led_list[1] = LED_G;
            s_led_list[2] = LED_B;
            s_led_list[3] = LED_C;
            s_led_list[4] = LED_W;
            break;

        default:
            break;
        }

        s_led_num = sg_prod_config_data.light_way;
        if (PROD_LIGHT_C == sg_prod_config_data.light_way) {
            s_led_num += 1;              //attention: when one way,blink as turn on as C and blink turn off
        }

        s_cw_bright_max = PROD_CW_BRIGHT_VALUE_MAX * (sg_prod_config_data.limit_cw_max / 100.0);          //calc cw max bright
        s_rgb_bright_max = PROD_RGB_BRIGHT_VALUE_MAX * (sg_prod_config_data.limit_rgb_max / 100.0);       //calc RGB max bright
        s_init_flag = TRUE;
    }

    if (s_led_list[s_index] <= LED_B ) {  //R\G\B color led!
        _prod_set_rgbcw(s_led_list[s_index], s_rgb_bright_max);
    } else {
        _prod_set_rgbcw(s_led_list[s_index], s_cw_bright_max);
    }

    s_stand_cnt++;
    if ((s_stand_cnt * PROD_AGING_START_BASE_INTERVAL) >= PROD_AGING_START_ONOFF_TIMER_INTERVAL) {
        s_stand_cnt = 0;
        s_index++;
        if (s_index >= s_led_num) {
            s_index = 0;
            usCheckCnt++;
        }
    }

    if (usCheckCnt >= PROD_AGING_START_LOOP_CNT) {

        ret = tuya_user_flash_read_prod_data(&prod_result);      //read prodaging time first
        if (LIGHT_OK != ret) {

            PR_ERR("Read prod aging time error! reset aging time -> 0.");

            prod_result.aging_tested_time = 0;
            prod_result.test_mode= PROD_AGING;
        }

        ret = tuya_user_flash_write_prod_data(&prod_result);
        if (LIGHT_OK == ret) {
            PR_NOTICE("goto aging!");

            _prod_aging_process();       //display goto aging process 

            ret = tuya_user_software_timer_stop(AGINGSTART_SW_TIME);    //stop timer dominantly error
            if (ret != LIGHT_OK) {
                PR_ERR("prod aging start display finish stop timer dominantly error!");
            }

            return;     //don't restart OVERALLCHECK_SW_TIME software timer
        } else {
            PR_NOTICE("Write prod aging time error!");
        }
    }

    ret = tuya_user_software_timer_start(AGINGSTART_SW_TIME, PROD_AGING_START_BASE_INTERVAL, _aging_start_time_callback);
    if(LIGHT_OK != ret) {

        PR_ERR("production aging check start display init failure,light shut down!");

        ret = tuya_light_set_rgbcw(0,0,0,0,0);	// light shut down
        if (LIGHT_OK != ret) {
            PR_ERR("production aging check start display init failure,light shut down error!");
        }
    }

    return;
}

/**
 * @brief: 灯老化提醒处理
 * @param: none
 * @return: none
 */
STATIC VOID _aging_start_process(VOID)
{
    OPERATE_LIGHT ret = -1;

    PR_NOTICE("prod aging start!");

    ret = tuya_user_software_timer_start(AGINGSTART_SW_TIME, PROD_AGING_START_BASE_INTERVAL, _aging_start_time_callback);
    if (LIGHT_OK != ret) {

        PR_ERR("production aging check start display init failure,light shut down!");

        ret = tuya_light_set_rgbcw(0,0,0,0,0);             // light shut down
        if (LIGHT_OK != ret) {
            PR_ERR("production aging check start display init failure,light shut down error!");
        }
    }

    return;
}

/**
 * @brief: Production factory test aging function display timer callback
 *          display styple
 *              C     ->  C aging
 *              CW    ->  C aging > W aging
 *              RGB   ->  RGB aging
 *              RGBC  ->  C aging > RGB aging
 *              RGBCW ->  C aging > W aging > RGB aging
 *          above display base on  frequency
 * @param {none}
 * @retval: none
 */
STATIC VOID _prod_aging_time_callback(VOID)
{
    OPERATE_LIGHT ret = -1;
    STATIC UCHAR_T s_init_flag = FALSE;
    STATIC UCHAR_T s_aging_time_list[3] = {0};
    UCHAR_T light_num = 0;
    STATIC USHORT_T s_aging_time = 0;   /* unit:min */
    STATIC UINT_T s_stand_cnt = 0;
    STATIC UCHAR_T s_index = 0;
    STATIC USHORT_T s_cw_bright_max = PROD_CW_BRIGHT_VALUE_MAX;
    STATIC USHORT_T s_cw_bright_min = PROD_CW_BRIGHT_VALUE_MAX * 0.1;
    STATIC USHORT_T s_rgb_bright_max = PROD_RGB_BRIGHT_VALUE_MAX;
    STATIC USHORT_T s_rgb_bright_min = PROD_CW_BRIGHT_VALUE_MAX * 0.1;
    light_prod_test_data_flash_t prod_result = {0};

    CONST UCHAR_T target_time[5][3] = {
            /* C    W       RGB aging time*/
            { 40,   0 ,     0 },          /* 1 way light  */
            { 20,   20,     0 },          /* 2 way light  */
            { 0 ,   0 ,     40},          /* 3 way light  */
            { 30,   0 ,     10},          /* 4 way light  */
            { 20,   20,     10}           /* 5 way light  */
    };

    if (FALSE == s_init_flag) {

        light_num = sg_prod_config_data.light_way - 1;     //attention: light way range from 1 to 5!
        memcpy(s_aging_time_list, target_time[light_num], 3 * (SIZEOF(UCHAR_T)));

        PR_NOTICE("Aging time C --> %d W --> %d RGB --> %d", s_aging_time_list[0], s_aging_time_list[1], s_aging_time_list[2]);

        ret = tuya_user_flash_read_prod_data(&prod_result);      //read prodaging time first
        if (LIGHT_OK != ret) {
            PR_ERR("Read prod aging time error! reset aging time -> 0.");
            prod_result.aging_tested_time = 0;
            prod_result.test_mode = PROD_AGING;
        }

        s_aging_time = prod_result.aging_tested_time;
        PR_NOTICE("already aging time:%d(min)!", s_aging_time);

        s_cw_bright_max = PROD_CW_BRIGHT_VALUE_MAX * (sg_prod_config_data.limit_cw_max / 100.0);          //calc CW max bright
        s_cw_bright_min = PROD_CW_BRIGHT_VALUE_MAX * (sg_prod_config_data.limit_cw_min / 100.0);          //calc CW min bright 

        s_rgb_bright_max = PROD_RGB_BRIGHT_VALUE_MAX * (sg_prod_config_data.limit_rgb_max / 100.0);        //calc RGB max bright 
        s_rgb_bright_min = PROD_RGB_BRIGHT_VALUE_MAX * (sg_prod_config_data.limit_rgb_min / 100.0);        //calc RGB min bright 

        if (PROD_LIGHT_RGB == sg_prod_config_data.light_way) {
            s_index = AGING_RGB;        //if light is RGB aging test need to aging RGB directly
        } else {
            ;
        }

        s_init_flag = TRUE;
    }

    s_stand_cnt++;

    if ((s_stand_cnt * PROD_AGING_BASE_INTERVAL) >= (60 * 1000)) {      //save aging time per minutes
        s_aging_time++;

        prod_result.aging_tested_time = s_aging_time;
        prod_result.test_mode = PROD_AGING;

        ret = tuya_user_flash_write_prod_data(&prod_result);
        if (LIGHT_OK == ret) {
            s_stand_cnt = 0;
            PR_NOTICE("aging has pass %d min!", s_aging_time);
        } else {
            s_aging_time--;
            PR_ERR("Write prod aging time error!");
        }
    }

    if( s_aging_time >= (s_aging_time_list[0] + s_aging_time_list[1] + s_aging_time_list[2])) {   //aging time satisfied!

        prod_result.aging_tested_time = s_aging_time;
        prod_result.test_mode = PROD_REPEAT;

        ret = tuya_user_flash_write_prod_data(&prod_result);
        if (LIGHT_OK == ret) {

            switch (sg_prod_config_data.light_way) {
            case PROD_LIGHT_C:
                _prod_set_rgbcw(LED_C, s_cw_bright_min);
                break;

            case PROD_LIGHT_CW:
                _prod_set_rgbcw(LED_CW, s_cw_bright_min);
                break;

            case PROD_LIGHT_RGB:
            case PROD_LIGHT_RGBC:
            case PROD_LIGHT_RGBCW:
                _prod_set_rgbcw(LED_G, s_rgb_bright_min);
                break;

            default:
                break;
            }

            ret = tuya_user_software_timer_stop(AGINGTEST_SW_TIME);    //stop timer dominantly error
            if (ret != LIGHT_OK) {
                PR_ERR("prod aging check finish stop timer dominantly error!");
            }
            return;
        } else {
            PR_ERR("Prod aging finish,but write flash error! try again!");
        }
    }

    switch (s_index)
    {
        case AGING_C:   
            _prod_set_rgbcw(LED_C, s_cw_bright_max);
            if (s_aging_time >= s_aging_time_list[0]) {
                s_index = AGING_W;
            }
            break;

        case AGING_W:
            _prod_set_rgbcw(LED_W, s_cw_bright_max);
            if (s_aging_time >= (s_aging_time_list[0] + s_aging_time_list[1])) {
                s_index = AGING_RGB;
            }
            break;

        case AGING_RGB:
            _prod_set_rgbcw(LED_RGB, s_rgb_bright_max);
            if (s_aging_time >= (s_aging_time_list[0] + s_aging_time_list[1] + s_aging_time_list[2])) {
                ;
            }
            break;

        default:
            break;
    }

    ret = tuya_user_software_timer_start(AGINGTEST_SW_TIME, PROD_AGING_BASE_INTERVAL, _prod_aging_time_callback);
    if( LIGHT_OK != ret ) {

        PR_ERR("production aging check display init failure,light shut down!");

        ret = tuya_light_set_rgbcw(0,0,0,0,0);      //light shut down
        if (LIGHT_OK != ret) {
            PR_ERR("production aging check display init failure,light shut down error!");
        }
    }

    return;
}

/**
 * @brief: production aging check
 * @param {none}
 * @retval: none
 */
STATIC VOID _prod_aging_process(VOID)
{
    OPERATE_LIGHT ret = -1;

    ret = tuya_user_software_timer_start(AGINGTEST_SW_TIME, PROD_AGING_BASE_INTERVAL, _prod_aging_time_callback);
    if (LIGHT_OK != ret) {

        PR_ERR("production aging check display init failure,light shut down!");

        ret = tuya_light_set_rgbcw(0,0,0,0,0);      //light shut down
        if (LIGHT_OK != ret) {
            PR_ERR("production aging check display init failure,light shut down error!");
        }
    }

    return;
}

/**
 * @brief: Production factory test repeat function display timer callback
 *          display styple
 *              C     ->  C bringt up then bright down
 *              CW    ->  C bringt up then bright down > W
 *              RGB   ->  R bringt up then bright down > G > B
 *              RGBC  ->  R bringt up then bright down > G > B > C
 *              RGBCW ->  R bringt up then bright down > G > B > C > W
 *          above display base on  frequency
 * @param {none}
 * @retval: none
 */
STATIC VOID _prod_repeat_time_callback(VOID)
{
    OPERATE_LIGHT ret = -1;
    STATIC UCHAR_T s_init_flag = FALSE;
    STATIC UCHAR_T s_led_list[5] = {0};
    STATIC UCHAR_T s_led_num = 0;
    STATIC UCHAR_T s_index = 0;
    STATIC UCHAR_T s_status = TRUE;
    STATIC USHORT_T s_cw_bright_min = 0;
    STATIC USHORT_T s_cw_bright_max = 0;
    STATIC USHORT_T s_cw_change_step = 0;
    STATIC USHORT_T s_rgb_bright_min = 0;
    STATIC USHORT_T s_rgb_bright_max = 0;
    STATIC USHORT_T s_rgb_change_step = 0;
    STATIC SHORT_T s_send_value = 0;

    if (FALSE == s_init_flag) {
        switch (sg_prod_config_data.light_way) {
        case PROD_LIGHT_C:
            s_led_list[0] = LED_C;
            break;
        case PROD_LIGHT_CW:
            s_led_list[0] = LED_C;
            s_led_list[1] = LED_W;
            break;
        case PROD_LIGHT_RGB:
            s_led_list[0] = LED_R;
            s_led_list[1] = LED_G;
            s_led_list[2] = LED_B;
            break;
        case PROD_LIGHT_RGBC:
            s_led_list[0] = LED_R;
            s_led_list[1] = LED_G;
            s_led_list[2] = LED_B;
            s_led_list[3] = LED_C;
            break;
        case PROD_LIGHT_RGBCW:
            s_led_list[0] = LED_R;
            s_led_list[1] = LED_G;
            s_led_list[2] = LED_B;
            s_led_list[3] = LED_C;
            s_led_list[4] = LED_W;
            break;

        case PROD_LIGHT_MAX:
        default:
            break;
        }

        s_led_num = sg_prod_config_data.light_way;

        s_cw_bright_min = PROD_CW_BRIGHT_VALUE_MAX * (sg_prod_config_data.limit_cw_min / 100.0);             //calc cw min bright 
        s_cw_bright_max = PROD_CW_BRIGHT_VALUE_MAX * (sg_prod_config_data.limit_cw_max / 100.0);             //calc cw max bright
        s_cw_change_step = (s_cw_bright_max - s_cw_bright_min) / (PROD_REPEAT_ONOFF_TIMER_INTERVAL / PROD_REPEAT_BASE_INERVAL);

        s_rgb_bright_min = PROD_RGB_BRIGHT_VALUE_MAX * (sg_prod_config_data.limit_rgb_min / 100.0);          //calc rgb min bright 
        s_rgb_bright_max = PROD_RGB_BRIGHT_VALUE_MAX * (sg_prod_config_data.limit_rgb_max / 100.0);          //calc rgb max bright 
        s_rgb_change_step = (s_rgb_bright_max - s_rgb_bright_min) / (PROD_REPEAT_ONOFF_TIMER_INTERVAL / PROD_REPEAT_BASE_INERVAL);

        if (s_led_list[0] <= LED_B) {
            s_send_value = s_rgb_bright_min;
        } else {
            s_send_value = s_cw_bright_min;
        }

        s_init_flag = TRUE;
    }

    if (TRUE == s_status) {

        _prod_set_rgbcw(s_led_list[s_index], s_send_value);
        s_send_value += (s_led_list[s_index] <= LED_B) ? s_rgb_change_step : s_cw_change_step;
        if( s_send_value >= ((s_led_list[s_index] <= LED_B) ? s_rgb_bright_max : s_cw_bright_max)) {
            s_status = FALSE;
            s_send_value = (s_led_list[s_index] <= LED_B) ? s_rgb_bright_max : s_cw_bright_max;
        }

    } else {

        _prod_set_rgbcw(s_led_list[s_index], s_send_value);
        s_send_value -= (s_led_list[s_index] <= LED_B) ? s_rgb_change_step : s_cw_change_step;
        if (s_send_value <= ((s_led_list[s_index] <= LED_B) ? s_rgb_bright_min : s_cw_bright_min)) {
            s_status = TRUE;
            s_index++;
            if (s_index >= s_led_num) {
                s_index = 0;
            }
            s_send_value = (s_led_list[s_index] <= LED_B) ? s_rgb_bright_min : s_cw_bright_min;
        }
    }

    ret = tuya_user_software_timer_start(REPEATTEST_SW_TIME, PROD_REPEAT_BASE_INERVAL, _prod_repeat_time_callback);
    if (LIGHT_OK != ret) {

        PR_ERR("production repeat check display init failure,light shut down!");

        ret = tuya_light_set_rgbcw(0,0,0,0,0);      //light shut down
        if (LIGHT_OK != ret) {
            PR_ERR("production repeat check display init failure,light shut down error!");
        }
    }

    return;
}

/**
 * @brief: production factory repeat test -- test2
 * @param {none}
 * @retval: none
 */
STATIC VOID _Prod_repeat_process(VOID)
{
    OPERATE_LIGHT ret = -1;

    ret = tuya_user_software_timer_start(REPEATTEST_SW_TIME, PROD_REPEAT_BASE_INERVAL, _prod_repeat_time_callback);
    if (LIGHT_OK != ret) {

        PR_ERR("production repeat check display init failure,light shut down!");

        ret = tuya_light_set_rgbcw(0,0,0,0,0);      //light shut down
        if (LIGHT_OK != ret) {
            PR_ERR("production repeat check display init failure,light shut down error!");
        }
    }

    return;
}

/**
 * @brief: prodution test process cb
 * @param: authorized -> authorized flag
 * @param: signal_strength -> singal strength
 * @retval: none
 */
VOID tuya_prod_test_callback(BOOL_T authorized, CHAR_T signal_strength)
{
    OPERATE_LIGHT ret = -1;
    light_prod_test_data_flash_t prod_result;

    //turn off ctrl proc firstly!
    tuya_light_ctrl_disable();       

    if (sg_prod_init_flag != TRUE) {
        PR_ERR("Prod test not init,can't do test!");
        return;
    }

    if (signal_strength < AUZ_TEST_WEAK_SIGNAL) { 
        //weak singal
        PR_ERR("Weak singal!");
        _weak_signal_process();
        return;
    }

    if (FALSE == authorized) {                      
        //unauthorized !
        PR_ERR("Production unauthorized!");
        _unauthorized_process();
        return;
    }

    PR_NOTICE("PROD FACTORY START!");

    //read prodaging time first
    ret = tuya_user_flash_read_prod_data(&prod_result);     
    if (LIGHT_OK != ret) {
        prod_result.test_mode = PROD_CHECK;
        prod_result.aging_tested_time = 0;
        PR_NOTICE("First prod test!");
    }
    PR_NOTICE("Prod test mode -> %d, aging already time %d(min)",prod_result.test_mode, prod_result.aging_tested_time);

    switch (prod_result.test_mode) {
    case PROD_CHECK:  
        // production overall check 
        _prod_check_process();
        break;

    case PROD_AGING:
        _aging_start_process();
        break;

    case PROD_REPEAT:
        _Prod_repeat_process();
        break;

    default:
        break;
    }
}

/**
 * @brief: 产测初始化接口
 * @param: none
 * @retval: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_light_prod_init(VOID)
{
    OPERATE_LIGHT ret = -1;

    //确保设备配置数据已加载
    ret = tuya_device_cfg_data_load();       
    if (ret != LIGHT_OK) {
        PR_ERR("Config Data Load error!");
        return LIGHT_INVALID_PARM;
    }

    sg_prod_config_data.light_way = tuya_cfg_get_color_mode();
    sg_prod_config_data.bright_mode = tuya_cfg_get_cw_type();
    sg_prod_config_data.limit_cw_max = tuya_cfg_get_white_max();
    sg_prod_config_data.limit_cw_min = tuya_cfg_get_white_min();
    sg_prod_config_data.limit_rgb_max = tuya_cfg_get_color_max();
    sg_prod_config_data.limit_rgb_min = tuya_cfg_get_color_min();

#if (LIGHT_CFG_PROD_DRIVER_NEED_INIT == 1)
    sg_prod_config_data.drive_cfg.mode = tuya_cfg_get_driver_mode();

    switch (sg_prod_config_data.drive_cfg.mode)
    {
        case DRIVER_MODE_PWM: {
            sg_prod_config_data.drive_cfg.config.pwm_init.freq = tuya_cfg_get_pwm_frequency();
            sg_prod_config_data.drive_cfg.config.pwm_init.list[0] = tuya_cfg_get_red_pin();
            sg_prod_config_data.drive_cfg.config.pwm_init.list[1] = tuya_cfg_get_green_pin();
            sg_prod_config_data.drive_cfg.config.pwm_init.list[2] = tuya_cfg_get_blue_pin();
            sg_prod_config_data.drive_cfg.config.pwm_init.list[3] = tuya_cfg_get_cold_pin();
            sg_prod_config_data.drive_cfg.config.pwm_init.list[4] = tuya_cfg_get_warm_pin();
            sg_prod_config_data.drive_cfg.config.pwm_init.channel_num = tuya_cfg_get_color_mode();

            if (sg_prod_config_data.light_way == PROD_LIGHT_RGB) {
                sg_prod_config_data.drive_cfg.config.pwm_init.polarity = tuya_cfg_get_red_pin_active_level();
                sg_prod_config_data.drive_cfg.config.pwm_init.duty = (tuya_cfg_get_red_pin_active_level() == 1) ?  0 : 1000;
            } else {
                sg_prod_config_data.drive_cfg.config.pwm_init.polarity = tuya_cfg_get_cold_pin_active_level();
                sg_prod_config_data.drive_cfg.config.pwm_init.duty = (tuya_cfg_get_cold_pin_active_level() == 1) ?  0 : 1000;
            }
            sg_prod_config_data.drive_cfg.config.pwm_init.ctrl_pin = tuya_cfg_get_ctrl_pin();
            sg_prod_config_data.drive_cfg.config.pwm_init.ctrl_level = tuya_cfg_get_ctrl_pin_active_level();
            sg_prod_config_data.drive_cfg.config.pwm_init.cct_flag = tuya_cfg_get_cw_type();
            break;
        }

        default: {
            ret = LIGHT_COM_ERROR;
            PR_ERR("Driver mode ERROR");
            break;
        }

    }

    //灯驱动初始化
    ret = tuya_light_drive_init(&sg_prod_config_data.drive_cfg);
    if (ret != LIGHT_OK) {
        PR_ERR("Light drive init error!");
        return LIGHT_COM_ERROR;
    }
#endif
    sg_prod_init_flag = TRUE;

    return LIGHT_OK;
}


