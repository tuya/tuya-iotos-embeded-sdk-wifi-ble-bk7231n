/**
* @file light_control.c
* @author www.tuya.com
* @brief light control process
* @version 0.2
* @date 2022-03-25
*
* @copyright Copyright (c) tuya.inc 2022
*
*/

#include "light_control.h"
#include "light_printf.h"
#include "light_tools.h"
#include "user_flash.h"
#include "user_timer.h"

// hardware timer cycle (unit:ms)
#define HW_TIMER_CYCLE_MS           HW_TIMER_CYCLE_US/1000

// light shade change cycle (unit:ms) enable to change
#define LIGHT_SHADE_CYCLE           (HW_TIMER_CYCLE_US/1000)

// Control calculate range 0 ~ 1000
#define CTRL_CAL_VALUE_RANGE        1000
// Control cw calc max bright value
#define CTRL_CW_BRIGHT_VALUE_MAX    CTRL_CAL_VALUE_RANGE
// Control cw calc min bright value (max value * 0.01)
#define CTRL_CW_BRIGHT_VALUE_MIN    (CTRL_CW_BRIGHT_VALUE_MAX * 0.01)

// Control RGB calc max bright value
#define CTRL_RGB_BRIGHT_VALUE_MAX   CTRL_CAL_VALUE_RANGE
// Control RGB calc min bright value
#define CTRL_RGB_BRIGHT_VALUE_MIN   (CTRL_RGB_BRIGHT_VALUE_MAX * 0.01)

// control power gain (100 --> gain = 1 times )
#define CTRL_POEWER_GAIN_MIN        100
// control power gain (200 --> gain = 2 times )
#define CTRL_POEWER_GAIN_MAX        200

// control CW&RGB bright limit max
#define CTRL_BRIGHT_LIMIT_MAX       100
// control CW&RGB bright limit min
#define CTRL_BRIGHT_LIMIT_MIN       0

// control CW&RGB temper limit max
#define CTRL_TEMPER_LIMIT_MAX       100
// control CW&RGB temper limit min
#define CTRL_TEMPER_LIMIT_MIN       0

#define SHADE_STEP_GAIN_DEFAULT     (CTRL_CAL_VALUE_RANGE * LIGHT_SHADE_CYCLE / SHADE_CHANG_MAX_TIME)

#define CHECK_INIT_OK();        if (_light_check_init() != LIGHT_OK) {\
                                    return LIGHT_COM_ERROR;\
                                }

/**
 * @brief hardware timer drive actively control structure
 */
typedef struct
{
    UINT_T target_cnt;     //shade time per one change 
    UINT_T cnt;            //hardware time 
    BOOL_T enable;         //hardware time deal with shade change flag
}hardware_timer_param_t;

// hardware time set param
STATIC hardware_timer_param_t sg_hw_timer_param = {
    .target_cnt = 0xFFFFFF,
    .cnt = 0,
    .enable = FALSE,
};

// light control initlize flag
STATIC UCHAR_T sg_light_ctrl_init_flag = FALSE;

// light ctrl configuration
STATIC light_ctrl_cfg_t sg_light_cfg_data;

// light ctrl data(status)
STATIC light_ctrl_data_t sg_light_ctrl_data;

// light ctrl handle(process)
STATIC light_ctrl_handle_t sg_light_ctrl_handle;

// Light shade change step gain
STATIC UINT_T sg_step_gain = SHADE_STEP_GAIN_DEFAULT;

STATIC VOID _light_ctrl_scene_auto_restart_time_callback(VOID);
STATIC VOID _light_ctrl_secne_change_stop(VOID);
STATIC VOID _light_ctrl_shade_stop(VOID);
#if (LIGHT_CFG_SUPPORT_LOWPOWER == 1)
STATIC OPERATE_LIGHT _light_ctrl_low_power_process(IN UCHAR_T on_off);
#endif


/**
 * @brief: 检查灯是否已经初始化
 * @param: none
 * @return: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
STATIC OPERATE_LIGHT _light_check_init(VOID)
{
    if (!sg_light_ctrl_init_flag) {
        PR_ERR("light control don't initlize, can't active!");
        return LIGHT_COM_ERROR;
    }

    return LIGHT_OK;
}

#if (LIGHT_CFG_INIT_PARAM_CHECK == 1)
/**
 * @brief: 检查灯对应控制方式的默认颜色
 * @param[in] light_way -> 灯控制方式的类型
 * @param[in] color 默认颜色
 * @return: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
STATIC OPERATE_LIGHT _light_ctrl_check_color_param(IN CTRL_LIGHT_WAY_E light_way, IN CTRL_DEF_COLOR_E color)
{
    switch (light_way) {
        case LIGHT_C:
            if (color != DEF_COLOR_C ) {
                return LIGHT_INVALID_PARM;
            }
            break;

        case LIGHT_CW:
            if ((color != DEF_COLOR_C) && (color != DEF_COLOR_W)) {
                return LIGHT_INVALID_PARM;
            }
            break;

        case LIGHT_RGB:
            if ((color == DEF_COLOR_C) || (color == DEF_COLOR_W)) {
                return LIGHT_INVALID_PARM;
            }
            break;

        case LIGHT_RGBC:
            if (color == DEF_COLOR_W) {
                return LIGHT_INVALID_PARM;
            }
            break;

        case LIGHT_RGBCW:
        case LIGHT_MAX:
        default:
            break;
    }

    return LIGHT_OK;
}
#endif

/**
 * @brief:根据灯的配置数据设置灯的默认控制数据
 * @param: none
 * @return: none
 */
VOID tuya_light_ctrl_data_reset(VOID)
{
    CHAR_T str_temp[5] = {0};
    CHAR_T hsv_temp[13] = {0};
    USHORT_T red_temp = 0;
    USHORT_T green_temp = 0;
    USHORT_T blue_temp = 0;
    USHORT_T h_temp = 0;
    USHORT_T s_temp = 0;
    USHORT_T v_temp = 0;

    tuya_light_shade_ctrl_disable(); //关闭灯的渐变控制

    memset(&sg_light_ctrl_data, 0, SIZEOF(light_ctrl_data_t));

    sg_light_ctrl_data.switch_status = TRUE;

    if ((LIGHT_RGB == sg_light_cfg_data.light_way) || (sg_light_cfg_data.def_color >= DEF_COLOR_R)) { //RGB 3 way light,just have color mode
        sg_light_ctrl_data.mode = COLOR_MODE;
    } else {
        sg_light_ctrl_data.mode = WHITE_MODE;
    }

    PR_DEBUG("def bright %d", sg_light_cfg_data.def_bright);
    PR_DEBUG("def temper %d", sg_light_cfg_data.def_temper);
    sg_light_ctrl_data.bright = sg_light_cfg_data.def_bright;
    sg_light_ctrl_data.temper = sg_light_cfg_data.def_temper;

    USHORT_T max = 0;
    USHORT_T min = 0; 
    USHORT_T bright_temp = 0;

    max = CTRL_RGB_BRIGHT_VALUE_MAX * ((FLOAT_T) sg_light_cfg_data.limit_rgb_max / 100.0);
    min = CTRL_RGB_BRIGHT_VALUE_MAX * ((FLOAT_T) sg_light_cfg_data.limit_rgb_min / 100.0);

    bright_temp = (sg_light_cfg_data.def_bright - CTRL_RGB_BRIGHT_VALUE_MIN) * (max - min) / \
                (CTRL_RGB_BRIGHT_VALUE_MAX - CTRL_RGB_BRIGHT_VALUE_MIN) + min;

    /* don't save RGB data to original ctrl data1\2\3, to avoid mistake */
    /* just need save rgb limit data, gamma proc will deal in calcRGBCW */
    if (DEF_COLOR_R == sg_light_cfg_data.def_color) {    
        sg_light_ctrl_data.color.red = bright_temp;  
        red_temp = sg_light_cfg_data.def_bright ;
    } else if (DEF_COLOR_G == sg_light_cfg_data.def_color) {
        sg_light_ctrl_data.color.green = bright_temp;
        green_temp = sg_light_cfg_data.def_bright ;
    } else if (DEF_COLOR_B == sg_light_cfg_data.def_color) {
        sg_light_ctrl_data.color.blue = bright_temp;
        blue_temp = sg_light_cfg_data.def_bright ;
    } else {
        sg_light_ctrl_data.color.red = bright_temp;
        red_temp = sg_light_cfg_data.def_bright ;
    }

    tuya_light_tool_rgb_to_hsv(red_temp, green_temp, blue_temp, &h_temp, &s_temp, &v_temp);

    PR_DEBUG("default color HSV %d %d %d ", h_temp, s_temp, v_temp);

    tuya_num_to_str(4, h_temp, 5, str_temp);
    strcpy(hsv_temp, str_temp);

    tuya_num_to_str(4, s_temp, 5, str_temp);
    strcat(hsv_temp, str_temp);

    tuya_num_to_str(4, v_temp, 5, str_temp);
    strcat(hsv_temp, str_temp);
    strcpy((CHAR_T*)&sg_light_ctrl_data.color_origin.color_str, hsv_temp);

    PR_DEBUG("default color origin string %s", sg_light_ctrl_data.color_origin.color_str);

    sg_light_ctrl_data.color_origin.hue = h_temp;
    sg_light_ctrl_data.color_origin.sat = s_temp;
    sg_light_ctrl_data.color_origin.value = v_temp;

    PR_DEBUG("default color data HSV %d %d %d", sg_light_ctrl_data.color_origin.hue, sg_light_ctrl_data.color_origin.sat,sg_light_ctrl_data.color_origin.value);

    switch (sg_light_cfg_data.light_way) {
        case LIGHT_C:
            memcpy(sg_light_ctrl_data.scene, SCENE_DATA_DEFAULT_C, strlen(SCENE_DATA_DEFAULT_C));
            break;

        case LIGHT_CW:
            memcpy(sg_light_ctrl_data.scene, SCENE_DATA_DEFAULT_CW, strlen(SCENE_DATA_DEFAULT_CW));
            break;

        case LIGHT_RGB:
            memcpy(sg_light_ctrl_data.scene, SCENE_DATA_DEFAULT_RGB, strlen(SCENE_DATA_DEFAULT_RGB));
            break;

        case LIGHT_RGBC:
            memcpy(sg_light_ctrl_data.scene, SCENE_DATA_DEFAULT_C, strlen(SCENE_DATA_DEFAULT_C));
            break;

        case LIGHT_RGBCW:
            memcpy(sg_light_ctrl_data.scene, SCENE_DATA_DEFAULT_CW, strlen(SCENE_DATA_DEFAULT_CW));
            break;

        default:
            break;
    }
    PR_DEBUG("default scene data %s", sg_light_ctrl_data.scene);

    sg_light_ctrl_data.scene_first_set = FALSE;

    sg_light_ctrl_data.countdown = 0;

    return;
}

/**
 * @brief: 硬件定时器回调函数
 * @param: none
 * @attention: this function need to implement by system,
 *              decide how to call tuya_light_ctrl_shade_gradually function.
 * @return: none
 */
WEAK VOID tuya_light_sys_hardware_timer_callback(VOID)
{
    ;
}

/**
 * @brief: 硬件定时器回调函数, 灯控制适配接口
 * @param: none
 * @attention: tuya_light_sys_hardware_timer_callback() func need to implement by system
 * @return: none
 */
VOID tuya_light_ctrl_hardware_timer_callback(VOID)
{
    if (sg_hw_timer_param.enable != TRUE) {
        return;
    }

    sg_hw_timer_param.cnt += HW_TIMER_CYCLE_MS;

    if (sg_hw_timer_param.cnt >= sg_hw_timer_param.target_cnt) {
        tuya_light_sys_hardware_timer_callback();
        sg_hw_timer_param.cnt = 0;
    }

    return;
}

/**
 * @brief: 灯控制初始化接口
 * @param[in]: config_data 初始化的数据指针 
 * @attention: this function need apply tuya_light_sys_hardware_reboot_judge();
 * @return: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_light_ctrl_init(IN light_ctrl_cfg_t *config_data)
{
    OPERATE_LIGHT ret = -1;
    light_app_data_flash_t app_data ;
    BOOL_T hw_reboot_flag;

    if (sg_light_ctrl_init_flag) {
        return LIGHT_OK;
    }

#if (LIGHT_CFG_INIT_PARAM_CHECK == 1)
    //light way cfg
    if (config_data->light_way >= LIGHT_MAX) {
        PR_ERR("light way set error, max way is 5-way!");
        return LIGHT_INVALID_PARM;
    }

    //CW / CCT Drive mode setting
    if (config_data->bright_mode >= BRIGHT_MODE_MAX) {
        PR_ERR("light drive mode set error!!");
        return LIGHT_INVALID_PARM;
    }

    //turn on/off change directly or gradually setting */
    if (config_data->switch_mode >= SWITCH_MAX) {
        PR_ERR("light turn on/off change mode set error!!");
        return LIGHT_INVALID_PARM;
    }

    //light def color 
    ret = _light_ctrl_check_color_param(config_data->light_way, config_data->def_color);
    if (ret != LIGHT_OK) {
        PR_ERR("Default color set not match to light way!");
        return LIGHT_INVALID_PARM;
    }

    //light def bright, enable config max is 100
    if (config_data->def_bright > CTRL_BRIGHT_LIMIT_MAX) {
        PR_ERR("light def bright set error!");
        return LIGHT_INVALID_PARM;
    }

    //light def temperature, enable config max is 100
    if (config_data->def_temper > CTRL_TEMPER_LIMIT_MAX) {
        PR_ERR("light def temperature set error!");
        return LIGHT_INVALID_PARM;
    }

    //power gain setting, enable config range: 100~200 
    if ((config_data->power_gain < CTRL_POEWER_GAIN_MIN) || \
        (config_data->power_gain > CTRL_POEWER_GAIN_MAX)) {
        PR_ERR("light power gain set error!!");
        return LIGHT_INVALID_PARM;
    }

    //light CW bright limit max&min setting 
    if ((config_data->limit_cw_max > CTRL_BRIGHT_LIMIT_MAX) || \
        (config_data->limit_cw_min > config_data->limit_cw_max)) {
        PR_ERR("light CW white bright max&min limit set error!!");
        return LIGHT_INVALID_PARM;
    }

    //light RGB color bright limit max&min setting
    if ((config_data->limit_rgb_max > CTRL_BRIGHT_LIMIT_MAX) || \
        (config_data->limit_rgb_min > config_data->limit_rgb_max)) {
        PR_ERR("light RGB color bright max&min limit set error!!");
        return LIGHT_INVALID_PARM;
    }

    //reset cnt set
    if (config_data->reset_cnt <= 0) {
        PR_ERR("light reset cnt set invaild!");
        return LIGHT_INVALID_PARM;
    }

    //the color when connecting
    PR_DEBUG("light net color %d", config_data->net_color);
    ret = _light_ctrl_check_color_param(config_data->light_way, config_data->net_color);
    if (ret != LIGHT_OK) {
        PR_ERR("Default net color set not match to light way!");
        return LIGHT_INVALID_PARM;
    }

    //the bright when connecting
    if (config_data->net_bright > CTRL_BRIGHT_LIMIT_MAX) {
        PR_ERR("light net match bright set error!");
        return LIGHT_INVALID_PARM;
    }

    //the temperature when connecting */
    if (config_data->net_temper > CTRL_TEMPER_LIMIT_MAX) {
        PR_ERR("light net match bright set error!");
        return LIGHT_INVALID_PARM;
    }
#endif

    // attention oem cfg defbright range from 0~100,need to chang 0~1000
    config_data->def_bright = (USHORT_T) (CTRL_CW_BRIGHT_VALUE_MAX * ((FLOAT_T) config_data->def_bright / CTRL_BRIGHT_LIMIT_MAX));

    // attention oem cfg deftemper range from 0~100,need to chang 0~1000
    config_data->def_temper = (USHORT_T) (CTRL_CW_BRIGHT_VALUE_MAX * ((FLOAT_T) config_data->def_temper / CTRL_TEMPER_LIMIT_MAX));

    // attention oem cfg netbright range from 0~100,need to chang 0~1000 
    config_data->net_bright = CTRL_CW_BRIGHT_VALUE_MAX * ((FLOAT_T) config_data->net_bright / CTRL_BRIGHT_LIMIT_MAX);
    config_data->net_temper = CTRL_CW_BRIGHT_VALUE_MAX * ((FLOAT_T) config_data->net_temper / CTRL_TEMPER_LIMIT_MAX);

    // the hardware drive cfg & init!
    ret = tuya_light_drive_init(&(config_data->drive_cfg));
    if (ret != LIGHT_OK) {
        PR_ERR("Light drive init error!");
        return LIGHT_COM_ERROR;
    }

    memset(&sg_light_cfg_data, 0, SIZEOF(light_ctrl_cfg_t));
    memcpy(&sg_light_cfg_data, config_data, SIZEOF(light_ctrl_cfg_t));
    memset(&sg_light_ctrl_handle, 0, SIZEOF(light_ctrl_handle_t));

    memset(&app_data, 0, SIZEOF(light_app_data_flash_t));

    //read app flash data!
    ret = tuya_user_flash_read_app_data(&app_data);       
    if (ret != LIGHT_OK) {
        PR_NOTICE("No application data!");
        tuya_light_ctrl_data_reset();
    } else {
        hw_reboot_flag = tuya_light_sys_hardware_reboot_judge();
        sg_light_ctrl_data.switch_status = hw_reboot_flag ? TRUE : app_data.power;
        sg_light_ctrl_data.mode = app_data.mode;
        sg_light_ctrl_data.bright = app_data.bright;
        sg_light_ctrl_data.temper = app_data.temper;
        sg_light_ctrl_data.color.red = app_data.color.red;
        sg_light_ctrl_data.color.green = app_data.color.green;
        sg_light_ctrl_data.color.blue = app_data.color.blue;
        sg_light_ctrl_data.color_origin.hue = app_data.color_origin.hue;
        sg_light_ctrl_data.color_origin.sat = app_data.color_origin.sat;
        sg_light_ctrl_data.color_origin.value = app_data.color_origin.value;
        strcpy((CHAR_T*)&sg_light_ctrl_data.color_origin.color_str, (CHAR_T*)&app_data.color_origin.color_str);
        memcpy(&sg_light_ctrl_data.scene, &app_data.scene, SIZEOF(app_data.scene));
        sg_light_ctrl_data.scene_first_set = TRUE;
        sg_light_ctrl_data.countdown = 0;
    }

    //start shade need hardware timer!
    ret = tuya_user_hardware_timer_start(HW_TIMER_CYCLE_US, (VOID *)tuya_light_ctrl_hardware_timer_callback);    
    if (ret != LIGHT_OK) {
        PR_ERR("Light hardware timer init error!");
        return ret;
    }
    sg_light_ctrl_init_flag = TRUE;

    return LIGHT_OK;
}

/**
 * @brief: 获取灯rgbcw中渐变的最大值
 * @param[in]: target_val 目标值
 * @param[in]: current_val 当前值
 * @return: USHORT_T 灯rgbcw渐变的最大值  
 */
STATIC USHORT_T _light_get_shade_max(IN bright_data_t *target_val, IN bright_data_t *current_val)
{
    USHORT_T max_value = 0;

    max_value = (USHORT_T) tuya_light_tool_get_max_value(\
                tuya_light_tool_get_abs_value(target_val->red - current_val->red), \
                tuya_light_tool_get_abs_value(target_val->green - current_val->green),\
                tuya_light_tool_get_abs_value(target_val->blue - current_val->blue),\
                tuya_light_tool_get_abs_value(target_val->white - current_val->white),\
                tuya_light_tool_get_abs_value(target_val->warm - current_val->warm)\
                );

    return max_value;
}

/**
 * @brief: 启动灯的渐变效果
 * @param[in]: shade_time_ms 渐变周期 单位:ms}
 * @return: none
 */
STATIC VOID _light_ctrl_shade_start(IN UINT_T shade_time_ms)
{
    sg_hw_timer_param.enable = FALSE;
    sg_hw_timer_param.cnt = 0;

    if (shade_time_ms < HW_TIMER_CYCLE_MS) {
        PR_NOTICE("start shade time less than the minimum setting");
        shade_time_ms = HW_TIMER_CYCLE_MS;
    }

    sg_hw_timer_param.target_cnt = shade_time_ms;
    sg_hw_timer_param.enable = TRUE;
    sg_light_ctrl_handle.first_change = TRUE;

    PR_DEBUG("start shade >>>");
}

/**
 * @brief: 停止灯的渐变效果
 * @param: none
 * @return: none
 */
STATIC VOID _light_ctrl_shade_stop(VOID)
{
    sg_hw_timer_param.enable = FALSE;
    sg_hw_timer_param.cnt = 0;
    sg_hw_timer_param.target_cnt = 0xFFFFFF;
}

/**
 * @brief: 设置灯渐变的步进增益
 * @param[in]: gain 步进增益
 * @return: none
 */
STATIC VOID _light_ctrl_shade_step_gain_set(IN UINT_T gain)
{
    sg_step_gain = gain;

    //增益不能太小
    if (sg_step_gain < 1) {    
        sg_step_gain = 1;
    }
}

/**
 * @brief: 获取渐变的步进增益
 * @param: none
 * @return: sg_step_gain 
 */
STATIC UINT_T _light_ctrl_shade_step_gain_get(VOID)
{
    return (sg_step_gain);
}

/**
 * @brief: cct延时1s关闭回调函数
 * @param: none
 * @attention: only active as CCT
 * @return: none
 */
STATIC VOID _light_ctrl_cct_shut_down_time_callback(VOID)
{
    OPERATE_LIGHT ret = -1;

    //stop timer dominantly
    ret = tuya_user_software_timer_stop(CCT_DELAY_SHUT_DOWN_TIMER);   
    if (ret != LIGHT_OK) {
        PR_ERR("CCT delay timer stop error!");
    }

    if (sg_light_ctrl_data.switch_status) {     
        // if turn on again ,don't active 
        return;
    }

    memset(&sg_light_ctrl_handle.current_val, 0, SIZEOF(bright_data_t));

    //when CCT shut down, need to turn off CCT
    PR_DEBUG("CCT turn off!");
    ret = tuya_light_set_rgbcw(0, 0, 0, 0, 0);
    if (ret != LIGHT_OK) {
        PR_ERR("shut down CCT error!");
    }

    return;
}

/**
 * @brief: 灯渐变控制接口
 * @param: none
 * @attention: this func need to called by period
 * @return: none
 */
VOID tuya_light_ctrl_shade_gradually(VOID)
{
    OPERATE_LIGHT ret = -1;

    INT_T error_red = 0;
    INT_T error_green = 0;
    INT_T error_blue = 0;
    INT_T error_white = 0;
    INT_T error_warm = 0;

    USHORT_T max_error = 0;

    STATIC FLOAT_T s_scale_red = 0;
    STATIC FLOAT_T s_scale_green = 0;
    STATIC FLOAT_T s_scale_blue = 0;
    STATIC FLOAT_T s_scale_white = 0;
    STATIC FLOAT_T s_scale_warm = 0;

    UINT_T step_red = 0;
    UINT_T step_green = 0;
    UINT_T step_blue = 0;
    UINT_T step_white = 0;
    UINT_T step_warm = 0;

    UINT_T gain = 0;
    UINT_T delta = 0;

    //获取增益
    gain = _light_ctrl_shade_step_gain_get();
    if (memcmp(&sg_light_ctrl_handle.target_val, &sg_light_ctrl_handle.current_val, SIZEOF(bright_data_t)) != 0) {  
        //exist error

        // calulate the error between current value and target value
        error_red = sg_light_ctrl_handle.target_val.red - sg_light_ctrl_handle.current_val.red;
        error_green = sg_light_ctrl_handle.target_val.green - sg_light_ctrl_handle.current_val.green;
        error_blue = sg_light_ctrl_handle.target_val.blue - sg_light_ctrl_handle.current_val.blue;
        error_white = sg_light_ctrl_handle.target_val.white - sg_light_ctrl_handle.current_val.white;
        error_warm = sg_light_ctrl_handle.target_val.warm - sg_light_ctrl_handle.current_val.warm;

        max_error = _light_get_shade_max(&sg_light_ctrl_handle.target_val, &sg_light_ctrl_handle.current_val);

        if (TRUE == sg_light_ctrl_handle.first_change) {    
            //calculate change scale
            s_scale_red = tuya_light_tool_get_abs_value(error_red) / 1.0 / max_error;
            s_scale_green = tuya_light_tool_get_abs_value(error_green) / 1.0 / max_error;
            s_scale_blue = tuya_light_tool_get_abs_value(error_blue) / 1.0 / max_error;
            s_scale_white = tuya_light_tool_get_abs_value(error_white) / 1.0 / max_error;
            s_scale_warm = tuya_light_tool_get_abs_value(error_warm) / 1.0 / max_error;
            sg_light_ctrl_handle.first_change = FALSE;
        }

        if (max_error == tuya_light_tool_get_abs_value(error_red)) {
            step_red = 1 * gain;
        } else {
            delta = tuya_light_tool_get_abs_value(error_red) - max_error * s_scale_red + s_scale_red * gain;
            step_red = (delta < gain) ? (delta % gain) : gain;
        }

        if (max_error == tuya_light_tool_get_abs_value(error_green)) {
            step_green = 1 * gain;
        } else {
            delta = tuya_light_tool_get_abs_value(error_green) - max_error * s_scale_green + s_scale_green * gain;
            step_green = (delta < gain) ? (delta % gain) : gain;
        }

        if (max_error == tuya_light_tool_get_abs_value(error_blue)) {
            step_blue = 1 * gain;
        } else {
            delta = tuya_light_tool_get_abs_value(error_blue) - max_error *s_scale_blue + s_scale_blue * gain;
            step_blue = (delta < gain) ? (delta % gain) : gain;
        }

        if (max_error == tuya_light_tool_get_abs_value(error_white)) {
            step_white = 1 * gain;
        } else {
            delta = tuya_light_tool_get_abs_value(error_white) - max_error *s_scale_white + s_scale_white * gain;
            step_white = (delta < gain) ? (delta % gain) : gain;
        }

        if (max_error == tuya_light_tool_get_abs_value(error_warm)) {
            step_warm = 1 * gain;
        } else {
            delta = tuya_light_tool_get_abs_value(error_warm) - max_error *s_scale_warm + s_scale_warm * gain;
            step_warm = (delta < gain) ? (delta % gain) : gain;
        }

        if (error_red != 0) {
            if (tuya_light_tool_get_abs_value(error_red) < step_red) {
                sg_light_ctrl_handle.current_val.red += error_red;
            } else {
                if (error_red < 0) {
                    sg_light_ctrl_handle.current_val.red -= step_red;
                } else {
                    sg_light_ctrl_handle.current_val.red += step_red;
                }
            }
        }

        if (error_green != 0) {
            if (tuya_light_tool_get_abs_value(error_green) < step_green) {
                sg_light_ctrl_handle.current_val.green += error_green;
            } else {
                if (error_green < 0) {
                    sg_light_ctrl_handle.current_val.green -= step_green;
                } else {
                    sg_light_ctrl_handle.current_val.green += step_green;
                }
            }
        }

        if (error_blue != 0) {
            if (tuya_light_tool_get_abs_value(error_blue) < step_blue) {
                sg_light_ctrl_handle.current_val.blue += error_blue;
            } else {
                if (error_blue < 0) {
                    sg_light_ctrl_handle.current_val.blue -= step_blue;
                } else {
                    sg_light_ctrl_handle.current_val.blue += step_blue;
                }
            }
        }

        if (error_white != 0) {
            if (tuya_light_tool_get_abs_value(error_white) < step_white) {
                sg_light_ctrl_handle.current_val.white += error_white;
            } else {
                if (error_white < 0) {
                    sg_light_ctrl_handle.current_val.white -= step_white;
                } else {
                    sg_light_ctrl_handle.current_val.white += step_white;
                }
            }
        }

        if (error_warm!= 0) {
            if (tuya_light_tool_get_abs_value(error_warm) < step_warm) {
                sg_light_ctrl_handle.current_val.warm += error_warm;
            } else {
                if (error_warm < 0) {
                    sg_light_ctrl_handle.current_val.warm -= step_warm;
                } else {
                    sg_light_ctrl_handle.current_val.warm += step_warm;
                }
            }
        }

        if (sg_hw_timer_param.enable != TRUE) {
            //PR_DEBUG("light shade can't not output set!");
            return;
        }

        //改变rgbcw值
        ret = tuya_light_set_rgbcw(sg_light_ctrl_handle.current_val.red, sg_light_ctrl_handle.current_val.green, sg_light_ctrl_handle.current_val.blue, \
                                    sg_light_ctrl_handle.current_val.white, sg_light_ctrl_handle.current_val.warm);
        if (ret != LIGHT_OK) {
            //PR_ERR("light shade set RGBCW error %d!",ret);
            return;
        }
    } else { 
        // no error between target and current

        //PR_DEBUG("shade change no need, stop");

        //停止渐变
        _light_ctrl_shade_stop();    

        if ((BRIGHT_MODE_CCT == sg_light_cfg_data.bright_mode) && (FALSE == sg_light_ctrl_data.switch_status)) {
            //开启cct延时1s关闭软件定时器
            ret = tuya_user_software_timer_start(CCT_DELAY_SHUT_DOWN_TIMER, 1000, _light_ctrl_cct_shut_down_time_callback);
            if (ret != LIGHT_OK) {
                PR_ERR("CCT shut down err!");
                //when CCT shut down, need to turn off CCT
                ret = tuya_light_set_rgbcw(0, 0, 0, 0, 0);
            }
        }
    }
}

/**
 * @brief: 限制CW亮度值
 * @param[in]: bright 亮度值
 * @retval: result 限制后的值
 */
STATIC USHORT_T _light_ctrl_data_cw_limit(IN USHORT_T bright)
{
    USHORT_T max = 0;
    USHORT_T min = 0;
    USHORT_T result = 0;

    max = (USHORT_T) (CTRL_CW_BRIGHT_VALUE_MAX * ((FLOAT_T) sg_light_cfg_data.limit_cw_max / CTRL_BRIGHT_LIMIT_MAX));
    min = (USHORT_T) (CTRL_CW_BRIGHT_VALUE_MAX * ((FLOAT_T) sg_light_cfg_data.limit_cw_min / CTRL_BRIGHT_LIMIT_MAX));

    result = (USHORT_T)((bright - CTRL_CW_BRIGHT_VALUE_MIN) * (max - min) / 1.0 / \
                        (CTRL_CW_BRIGHT_VALUE_MAX - CTRL_CW_BRIGHT_VALUE_MIN) + min);

    return result;
}

/**
 * @brief: 根据Light_Data值计算CW Light_Handle.TargetVal，并进行CW输出限制和放大过程！ 
 * @param[in]: bright Light_Data bright
 * @param[in]: temperature Light_Data temperature
 * @param[in]: result calc result
 * @return: none
 */
STATIC VOID _light_ctrl_data_calculate_cw(IN USHORT_T bright, IN USHORT_T temperatue, OUT bright_data_t *result)
{
    USHORT_T bright_temp = 0;

    PR_DEBUG("Bright %d", bright);

    if (bright < CTRL_CW_BRIGHT_VALUE_MIN) {
        bright_temp = 0;
    } else {
        PR_DEBUG("cw limit proc");
        //限制CW亮度值
        bright_temp = _light_ctrl_data_cw_limit(bright);     
    }
    PR_DEBUG("usBrightTmep %d", bright_temp);

    if (BRIGHT_MODE_CW == sg_light_cfg_data.bright_mode) {

        //power amplification
        bright_temp = (USHORT_T)(((FLOAT_T)(sg_light_cfg_data.power_gain / CTRL_POEWER_GAIN_MIN)) * bright_temp);

        PR_DEBUG("usBrightTmep %d", bright_temp);

        result->white = (USHORT_T) (bright_temp * ((FLOAT_T)temperatue / CTRL_CAL_VALUE_RANGE));
        result->warm = bright_temp - result->white;

        //output max limit --  power amplification, convert overflow power to another
        if (result->white > CTRL_CAL_VALUE_RANGE) {
            result->white = CTRL_CAL_VALUE_RANGE;
        }
        if (result->warm > CTRL_CAL_VALUE_RANGE) {
            result->warm = CTRL_CAL_VALUE_RANGE;
        }
    } else if (BRIGHT_MODE_CCT == sg_light_cfg_data.bright_mode) {
        result->white = bright_temp;
        result->warm = temperatue;
    }

    return;
}

/**
 * @brief: calculate HSV limit v.
 * @param[in]: value HSV值
 * @return: limit adjust value
 */
STATIC USHORT_T _light_ctrl_data_calculate_hsv_limit(IN USHORT_T value)
{
    USHORT_T limit_min = 10;
    USHORT_T limit_max = 1000;

    if (value < CTRL_RGB_BRIGHT_VALUE_MIN) {
        // value is illegal, HSV v must be 0 
        return 0;       
    }

    limit_min = (USHORT_T)(CTRL_RGB_BRIGHT_VALUE_MAX * ((FLOAT_T) sg_light_cfg_data.limit_rgb_min / 100.0));	//RGB scene data color limit
    limit_max = (USHORT_T)(CTRL_RGB_BRIGHT_VALUE_MAX * ((FLOAT_T) sg_light_cfg_data.limit_rgb_max / 100.0));

    return (USHORT_T)((value - CTRL_RGB_BRIGHT_VALUE_MIN) * (limit_max - limit_min) / (CTRL_RGB_BRIGHT_VALUE_MAX - CTRL_RGB_BRIGHT_VALUE_MIN) + limit_min);
}

/**
 * @brief: 计算 RGB 伽马值, format:0~1000
 * @param[in]: color R\G\B
 * @param[in]: gamma before gamma param
 * @attention:
 * @return: gamma
 */
STATIC USHORT_T _light_ctrl_data_calculate_gamma(IN CTRL_DEF_COLOR_E  color, IN USHORT_T gamma)
{
#if (LIGHT_CFG_ENABLE_GAMMA == 1)
    USHORT_T result;
    UCHAR_T index;

    result = gamma;
    index = (UCHAR_T)(gamma * 1.0 / CTRL_CAL_VALUE_RANGE * 255);

    switch (color) {
        case DEF_COLOR_R:
            result = (USHORT_T)(tuya_cfg_get_gamma_red(index) * CTRL_CAL_VALUE_RANGE / 255.0);
            break;

        case DEF_COLOR_G:
            result = (USHORT_T)(tuya_cfg_get_gamma_green(index) * CTRL_CAL_VALUE_RANGE / 255.0);
            break;

        case DEF_COLOR_B:
            result = (USHORT_T)(tuya_cfg_get_gamma_blue(index) * CTRL_CAL_VALUE_RANGE / 255.0);
            break;

        case DEF_COLOR_C:
        case DEF_COLOR_W:
        default:
            break;
    }

    return result;
#else
    PR_DEBUG("gamma adjust don't active");
    return gamma;
#endif
}

/**
 * @brief: calculate the RGB tLight_Handle.target_val according to
 *          the tLight_Data value.
 * @param[in]: color_rgb_t *color -> RGB ctrl data
 * @param[OUT]: bright_data_t *result -> result handle data
 * @attention: gamma adjust proc
 * @return: none
 */
STATIC VOID _light_ctrl_data_calculate_rgb(IN color_rgb_t *color, OUT bright_data_t *result)
{

    PR_DEBUG("before gamma adjust RGB %d %d %d", color->red, color->green, color->blue);

    result->red = _light_ctrl_data_calculate_gamma(DEF_COLOR_R, color->red);

    result->green = _light_ctrl_data_calculate_gamma(DEF_COLOR_G, color->green);

    result->blue = _light_ctrl_data_calculate_gamma(DEF_COLOR_B, color->blue);

    PR_DEBUG(" After gamma adjust RGB %d %d %d", result->red, result->green, result->blue);

    return;
}

/**
 * @brief: Calc RGBCW handle data
 * @param[in]: LIGHT_MODE_E mode -> current mode
 * @param[in]: light_ctrl_data_t *ctrl_data -> current ctrl data
 * @param[out]: bright_data_t *result -> output handle data
 * @attention: support white and color mode only
 * @return: none
 */
STATIC VOID _light_ctrl_data_calculate_rgbcw(IN LIGHT_MODE_E mode, IN light_ctrl_data_t *ctrl_data, OUT bright_data_t *result)
{
    STATIC LIGHT_MODE_E last_mode = MODE_MAX ;

    memset(result, 0, SIZEOF(bright_data_t));

    switch (mode) {
        case WHITE_MODE:
            _light_ctrl_data_calculate_cw(ctrl_data->bright, ctrl_data->temper, result);
            PR_DEBUG(" set bright %d temper %d,calc ctrl data %d %d %d %d %d", ctrl_data->bright, ctrl_data->temper,\
                result->red, result->green, result->blue, result->white, result->warm);
            if ((BRIGHT_MODE_CCT == sg_light_cfg_data.bright_mode) && (last_mode != mode)) {
                sg_light_ctrl_handle.current_val.warm = result->warm;
            }
            break;

        case COLOR_MODE:
            if (BRIGHT_MODE_CCT == sg_light_cfg_data.bright_mode) {
            //to aviod the CCT temperature change, when CCT set as white mode, make sure color mode not change CCT!
                PR_DEBUG("current warm %d", sg_light_ctrl_handle.current_val.warm);
                PR_DEBUG("target warm %d", sg_light_ctrl_handle.target_val.warm);
                result->warm = sg_light_ctrl_handle.current_val.warm;
            }
            _light_ctrl_data_calculate_rgb(&(ctrl_data->color), result);
            PR_DEBUG(" set bright %d temper %d,calc ctrl data %d %d %d %d %d", ctrl_data->bright, ctrl_data->temper,\
                result->red, result->green, result->blue, result->white, result->warm);
            break;

        case SCENE_MODE:
            _light_ctrl_data_calculate_rgb(&(ctrl_data->color), result);
            _light_ctrl_data_calculate_cw(ctrl_data->bright, ctrl_data->temper, result);
            PR_DEBUG("mode is scene mode!");

            PR_DEBUG(" set bright %d temper %d,calc ctrl data %d %d %d %d %d", ctrl_data->bright, ctrl_data->temper,\
                result->red, result->green, result->blue, result->white, result->warm);
            break;

        case MUSIC_MODE:
            _light_ctrl_data_calculate_rgb(&(ctrl_data->color), result);
            PR_DEBUG("mode is music mode!");
            PR_DEBUG(" set bright %d temper %d,calc ctrl data %d %d %d %d %d", ctrl_data->bright, ctrl_data->temper,\
                result->red, result->green, result->blue, result->white, result->warm);
            if (BRIGHT_MODE_CCT == sg_light_cfg_data.bright_mode) {
                result->warm = sg_light_ctrl_handle.current_val.warm;
            }
            break;
        default:
            break;
    }

    last_mode = mode;

    return;
}

/**
 * @brief: light HSVBT data analysize
 * @param[in]: CHAR_T *hsv_bt -> HSVBT string 
 * @param[out]: val_h -> Hue
 * @param[out]: val_s -> Sat
 * @param[out]: val_v -> value
 * @param[out]: val_b -> Brightness
 * @param[out]: val_t -> Temperature
 * @return: none
 */
STATIC VOID _light_hsvbt_analysize(IN CHAR_T *hsv_bt, OUT USHORT_T *val_h, OUT USHORT_T *val_s, OUT USHORT_T *val_v, OUT USHORT_T *val_b, OUT USHORT_T *val_t)
{
    UCHAR_T i = 0;
    USHORT_T temp_buf[5] = {0};

    for (i = 0; i < 5; i++) {
        temp_buf[i] = tuya_light_tool_str_to_ushort(tuya_light_tool_asc_to_hex(*(hsv_bt + 4*i + 0)), 
                                                    tuya_light_tool_asc_to_hex(*(hsv_bt + 4*i + 1)),
                                                    tuya_light_tool_asc_to_hex(*(hsv_bt + 4*i + 2)), 
                                                    tuya_light_tool_asc_to_hex(*(hsv_bt + 4*i + 3)));
    }

    PR_DEBUG("HSVBT %d %d %d %d", temp_buf[0],temp_buf[1],temp_buf[2],temp_buf[3],temp_buf[4]);

    *val_h = temp_buf[0];
    *val_s = temp_buf[1];
    *val_v = temp_buf[2];
    *val_b = temp_buf[3];
    *val_t = temp_buf[4];

    return;
}


/**
 * @brief: Calc Light scene handle data
 * @param[in]: UCHAR_T *scene_data -> scene data string 
 * @param[out]: light_scene_ctrl_t *ctrl_param -> scene ctr param(change mode, time, speed)
 * @param[out]: bright_data_t *result -> result(already changed to RGBCW data)
 * @attention: times & speed unit:ms
 * @return: none
 */
STATIC VOID _light_ctrl_data_calculate_scene(IN CHAR_T *scene_data, OUT light_scene_ctrl_t *ctrl_param, OUT bright_data_t *result)
{
    USHORT_T brightness = 0;
    USHORT_T tempature = 0;
    USHORT_T val_h = 0;
    USHORT_T val_s = 0;
    USHORT_T val_v = 0;
    USHORT_T val_r = 0;
    USHORT_T val_g = 0;
    USHORT_T val_b = 0;
    color_rgb_t rgb_value;


    //APP setting in constrast ,APP data is rang from 40~ 100
    // attention:(105 - x) is attentin to make when the APP send the fastest speed 100, ucTimes isn't equal to zeoro!
    // attention: APP send the times is equal to speed set !
    // attention: times and speed unit:ms !
    ctrl_param->change_mode = tuya_light_tool_str_to_uchar(tuya_light_tool_asc_to_hex(scene_data[4]), tuya_light_tool_asc_to_hex(scene_data[5]));

    if (ctrl_param->change_mode != SCENE_SHADE) {
        ctrl_param->times = (105 - tuya_light_tool_str_to_uchar(tuya_light_tool_asc_to_hex(scene_data[0]), tuya_light_tool_asc_to_hex(scene_data[1]))) * 100;
        ctrl_param->speed = (105 - tuya_light_tool_str_to_uchar(tuya_light_tool_asc_to_hex(scene_data[2]), tuya_light_tool_asc_to_hex(scene_data[3]))) * 100;
    } else {
        ctrl_param->times = (110 - tuya_light_tool_str_to_uchar(tuya_light_tool_asc_to_hex(scene_data[0]), tuya_light_tool_asc_to_hex(scene_data[1]))) * 100;
        ctrl_param->speed = ctrl_param->times / 2;          //scene change speed is time / 2 
    }


    PR_DEBUG("Speed is %d, time is %d", ctrl_param->speed , ctrl_param->times);

    _light_hsvbt_analysize((scene_data + 6), &val_h, &val_s, &val_v, &brightness, &tempature);

    val_v = _light_ctrl_data_calculate_hsv_limit(val_v);
    tuya_light_tool_hsv_to_rgb(val_h, val_s, val_v, &val_r, &val_g, &val_b);

    rgb_value.red = val_r;
    rgb_value.green = val_g;
    rgb_value.blue = val_b;
    _light_ctrl_data_calculate_rgb(&rgb_value, result);

    _light_ctrl_data_calculate_cw(brightness, tempature, result);

}

/**
 * @brief: reponse switch property process,
 *          this need to implement by system.
 * @param[out]: BOOL_T on_off -> switch status, TRUE is ON
 * @return: none
 */
WEAK VOID tuya_light_ctrl_data_switch_respone(OUT BOOL_T on_off)
{
    ;
}

/**
 * @brief:  设置开关状态, adapte control data issued by system
 *          to control data format.
 * @param[in]: on_off 开关值，TURE表示打开
 * @return: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_light_ctrl_data_switch_set(IN BOOL_T on_off)
{
    BOOL_T last_status;

    last_status = sg_light_ctrl_data.switch_status;

    if (TRUE == on_off) {
        sg_light_ctrl_data.switch_status = TRUE;
        if (SCENE_MODE == sg_light_ctrl_data.mode) {
            //restart scene mode
            sg_light_ctrl_data.scene_first_set = TRUE;
        }
    } else {
        sg_light_ctrl_data.switch_status = FALSE;
    }

    //reponse property
    tuya_light_ctrl_data_switch_respone(on_off);      

    if (on_off == last_status) {
        PR_DEBUG("the same switch set");
        return LIGHT_INVALID_PARM;
    }

    return LIGHT_OK;
}

/**
 * @brief: reponse mode property process,
 *          this need to implement by system.
 * @param[out]: LIGHT_MODE_E mode
 * @return: none
 */
WEAK VOID tuya_light_ctrl_data_mode_response(OUT LIGHT_MODE_E mode)
{
    ;
}

/**
 * @brief: 设置灯的模式数据
 * @param[in]: LIGHT_MODE_E mode
 * @attention: mode value is below:
 *                                  WHITE_MODE = 0,
 *                                  COLOR_MODE = 1,
 *                                  SCENE_MODE = 2,
 *                                  MUSIC_MODE = 3,
 * @return: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_light_ctrl_data_mode_set(IN LIGHT_MODE_E mode)
{
    OPERATE_LIGHT ret = -1;
    LIGHT_MODE_E last_mode ;

    last_mode = sg_light_ctrl_data.mode;

    //mode set need limit when light way is RGB, or CW or C 
    if (((LIGHT_RGB == sg_light_cfg_data.light_way) && (WHITE_MODE == mode)) \
       || ((sg_light_cfg_data.light_way <= LIGHT_CW) && (COLOR_MODE == mode)) \
       || (mode > MODE_MAX)) {

        PR_ERR("mode is illegal,set error");
        return LIGHT_INVALID_PARM;
    }

    sg_light_ctrl_data.mode = mode;
    tuya_light_ctrl_data_mode_response(mode);

    if ((mode == last_mode) && (mode != SCENE_MODE)) { 
        //only scene mode can be set many time
        PR_DEBUG("the same mode set");
        return LIGHT_INVALID_PARM;
    }

    //don't need to proc, as the adjusted new scene control data will issued later
    if ((mode == SCENE_MODE) && (sg_light_ctrl_data.realtime_flag == TRUE)) {
        PR_DEBUG("this scene mode don't need proc!");

        ret = tuya_user_software_timer_start(SCENE_AUTO_RESTART_TIMER, 1500, _light_ctrl_scene_auto_restart_time_callback);
        if (ret != LIGHT_OK) {
            PR_ERR("scene auto restart timer init error!");
        }
        return LIGHT_INVALID_PARM;
    }

    if (SCENE_MODE == sg_light_ctrl_data.mode) {             
        //restart scene mode 
        sg_light_ctrl_data.scene_first_set = TRUE;
    }

    return LIGHT_OK;
}

/**
 * @brief: reponse bright property process,
 *          this need to implement by system.
 * @param[out]: LIGHT_MODE_E mode
 * @param[out] USHORT_T bright
 * @attention: need reponse mode property,as set bright value, will auto set the mode to WHITE_MODE!
 * @return: none
 */
WEAK VOID tuya_light_ctrl_data_bright_response(OUT LIGHT_MODE_E mode, OUT USHORT_T bright)
{
    ;
}

/**
 * @brief: 设置灯的亮度, adapte control data issued by system
 *          to control data format.
 * @param[in] USHORT_T bright 亮度值
 * @attention: acceptable range:10~1000
 * @attention: set bright value, will auto set the mode to WHITE_MODE !
 * @return: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_light_ctrl_data_bright_set(IN USHORT_T bright)
{

    if (((bright < CTRL_CW_BRIGHT_VALUE_MIN) || (bright > CTRL_CW_BRIGHT_VALUE_MAX)) \
        || (LIGHT_RGB == sg_light_cfg_data.light_way)) {
        PR_ERR("bright value is exceed range,set error");
        return LIGHT_INVALID_PARM;
    }

    sg_light_ctrl_data.bright = bright;
    sg_light_ctrl_data.mode = WHITE_MODE;    //change mode to white mode forcibly

    tuya_light_ctrl_data_bright_response(sg_light_ctrl_data.mode, bright);

    return LIGHT_OK;
}

/**
 * @brief: reponse temperature property process,
 *          this need to implement by system.
 * @param[out]: LIGHT_MODE_E mode
 * @param[out]: USHORT_T temperature
 * @attention: need reponse mode property,as set temperature value, will auto set the mode to WHITE_MODE!
 * @return: none
 */
WEAK VOID tuya_light_ctrl_data_temperature_response(OUT LIGHT_MODE_E mode, OUT USHORT_T temperature)
{
    ;
}

/**
 * @brief: 设置灯色温, adapte control data issued by system
 *          to control data format.
 * @param[in]: USHORT_T temperature 色温
 * @attention: acceptable range:0~1000
 * @return: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_light_ctrl_data_temperature_set(IN USHORT_T temperature)
{

    if ((temperature > CTRL_CW_BRIGHT_VALUE_MAX) \
        || (LIGHT_RGB == sg_light_cfg_data.light_way)) {
        PR_ERR("temperature value is exceed range,set error");
        return LIGHT_INVALID_PARM;
    }

    sg_light_ctrl_data.temper = temperature;
    sg_light_ctrl_data.mode = WHITE_MODE;    //change mode to white mode forcibly

    tuya_light_ctrl_data_temperature_response(sg_light_ctrl_data.mode, temperature);

    return LIGHT_OK;
}

/**
 * @brief: reponse RGB property process,
 *          this need to implement by system
 * @param[out]: color_origin_t *color_origin
 * @return: none
 */
WEAK VOID tuya_light_ctrl_data_rgb_response(OUT color_origin_t *color_origin)
{
    ;
}

/**
 * @brief: 设置灯的RGB值
 * @param[in]: color 指向RGB数据
 * @param[in]: color_origin 指向原始的颜色数据 
 * @attention: acceptable format is RGB module, R,G,B range：0~1000
 * @return: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_light_ctrl_data_rgb_set(IN color_rgb_t *color, IN color_origin_t *color_origin)
{
    if ((color->red > CTRL_CAL_VALUE_RANGE) ||
        (color->green > CTRL_CAL_VALUE_RANGE) ||
        (color->blue > CTRL_CAL_VALUE_RANGE)) {
        PR_ERR("RGB color value is exceed range, set error!");
        return LIGHT_INVALID_PARM;
    }

    if(sg_light_cfg_data.light_way <= LIGHT_CW) {
        return LIGHT_INVALID_PARM;
    }

    //to avoid the situation:adjust color data ver fast and turn off the light with remote cmd,
    //as exist realtime control dp, the ightCtrlData.color will equal to the control data
    if (color_origin != NULL) {
        memcpy(&sg_light_ctrl_data.color_origin, color_origin, SIZEOF(color_origin_t));    //must save the origin data 
    }

    memcpy(&sg_light_ctrl_data.color, color, SIZEOF(color_rgb_t));

    tuya_light_ctrl_data_rgb_response(color_origin);

    return LIGHT_OK;
}

/**
 * @brief: reponse scene property process,
 *          this need to implement by system
 * @param[out]: CHAR_T *scene_data
 * @return: none
 */
WEAK VOID tuya_light_ctrl_data_scene_response(OUT CHAR_T *scene_data)
{
    ;
}

/**
 * @brief: 设置灯的场景数据
 * @param[in]: scene_data 场景数据
 * @attention: SceneData format: please reference to DP protocol
 * @return: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_light_ctrl_data_scene_set(IN CHAR_T *scene_data)
{
    CHAR_T last_scene_data[SCENE_MAX_LENGTH + 1] = {0};

    strcpy(last_scene_data, (CHAR_T *)&sg_light_ctrl_data.scene);

    if ((strlen(scene_data) < SCENE_MIN_LENGTH) || (strlen(scene_data) > SCENE_MAX_LENGTH)) {
        PR_ERR("Scene data is error! please chek!");
        return LIGHT_INVALID_PARM;
    }

    strcpy((CHAR_T*)&sg_light_ctrl_data.scene, scene_data);
    PR_DEBUG("light ctrl scene buf %s", sg_light_ctrl_data.scene);

    tuya_light_ctrl_data_scene_response(scene_data);

    if (TRUE == sg_light_ctrl_data.realtime_flag) {
        PR_DEBUG("this commad need to save and proc!");
        sg_light_ctrl_data.realtime_flag = FALSE;
    } else {
        if(strcmp(scene_data, last_scene_data) == 0) {
            PR_DEBUG("the same scene set");
            return LIGHT_INVALID_PARM;
        }
    }

    sg_light_ctrl_data.scene_first_set = TRUE;

    return LIGHT_OK;
}

/**
 * @brief: 获取开关状态
 * @param[out]: on_off 获取开关状态的指针 
 * @return: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_light_ctrl_data_switch_get(OUT BOOL_T *on_off)
{
    if (NULL == on_off) {
        return LIGHT_INVALID_PARM;
    }

    *on_off = sg_light_ctrl_data.switch_status;

    return LIGHT_OK;
}

/**
 * @berief: get light time Count Down data
 * @param[out] countdown  time count down value
 * @return: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
OPERATE_RET tuya_light_ctrl_data_countdown_get(OUT UINT_T *countdown)
{
    *countdown = sg_light_ctrl_data.countdown;

    return OPRT_OK;
}

/**
 * @brief: 获取灯光模式
 * @param[out] mode 获取灯光模式的指针
 * @return: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_light_ctrl_data_mode_get(OUT LIGHT_MODE_E *mode)
{
    if (NULL == mode) {
        return LIGHT_INVALID_PARM;
    }

    *mode = sg_light_ctrl_data.mode;

    return LIGHT_OK;
}

/**
 * @brief: 获取灯的明亮值
 * @param[out]: bright 获取灯明亮值的指针
 * @return: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_light_ctrl_data_bright_get(OUT USHORT_T *bright)
{
    if (NULL == bright) {
        return LIGHT_INVALID_PARM;
    }

    if (sg_light_cfg_data.light_way == LIGHT_RGB) {
        return LIGHT_COM_ERROR;
    }

    *bright = sg_light_ctrl_data.bright;

    return LIGHT_OK;
}

/**
 * @brief: 获取灯的色温值
 * @param[out]: temperature 获取灯色温值的指针 
 * @return: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_light_ctrl_data_temperature_get(OUT USHORT_T *temperature)
{
    if (NULL == temperature) {
        return LIGHT_INVALID_PARM;
    }

    if ((sg_light_cfg_data.light_way != LIGHT_RGBCW) && (sg_light_cfg_data.light_way != LIGHT_CW)) {
        return LIGHT_COM_ERROR;
    }

    *temperature = sg_light_ctrl_data.temper;

    return LIGHT_OK;
}

/**
 * @brief: 获取灯原始的颜色数据
 * @param[out]: original_color 获取灯原始颜色数据的指针
 * @return: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_light_ctrl_data_origin_get(OUT color_origin_t *original_color)
{
    if (NULL == original_color) {
        return LIGHT_INVALID_PARM;
    }

    if (sg_light_cfg_data.light_way < LIGHT_RGB) {
        return LIGHT_COM_ERROR;
    }

    PR_DEBUG("color original %s", sg_light_ctrl_data.color_origin.color_str);
    memcpy(original_color, &sg_light_ctrl_data.color_origin, SIZEOF(color_origin_t));

    return LIGHT_OK;
}

/**
 * @brief: 获取场景数据
 * @param[out]: scene_data 获取场景数据的指针 
 * @return: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_light_ctrl_data_scene_get(OUT CHAR_T *scene_data)
{
    if (NULL == scene_data) {
        return LIGHT_INVALID_PARM;
    }

    PR_DEBUG("str len %d", strlen(sg_light_ctrl_data.scene));

    strcpy(scene_data, (CHAR_T*)&sg_light_ctrl_data.scene);

    return LIGHT_OK;
}

/**
 * @brief: scene change proc
 *          get scene ctrl data and change to RGBCW value form,
 *          start to scene unitX(X:1~8);
 * @param {OUT BOOL_T *cycle_enable -> need to cycle}
 * @param {OUT UINT_T *pCycleTime_ms -> unit change time, ms}
 * @attention: this func need to called period, period time is transfered by pCycletime,
 *              when cycle_enable flag equal to TRUE;
 * @retval: OPERATE_LIGHT
 */
STATIC OPERATE_LIGHT _light_ctrl_scene_change(OUT BOOL_T *cycle_enable, OUT UINT_T *cycle_time_ms)
{
    OPERATE_LIGHT ret = -1;
    light_scene_ctrl_t light_scene_ctrl_parm;
    UINT_T gain_temp = 0;
    STATIC USHORT_T s_cct_warm = 0xFFFF;         // save CCT mode, warm set!
    STATIC UCHAR_T s_cct_last_rgb_flag = FALSE;
    UCHAR_T color_to_scene_mode_flag = FALSE;
    UCHAR_T white_to_scene_mode_flag = FALSE;

    if (sg_light_ctrl_handle.scene_stop_flag) {       
        //don't permit to change!
        PR_NOTICE("Scene don't allow to change!");

        *cycle_enable = FALSE;

        if (sg_light_ctrl_data.switch_status == FALSE) {
            // to avoid can't shut down! 
            ret = tuya_light_ctrl_process();
            PR_DEBUG("scene shut down again!");
            if (ret != LIGHT_OK) {
                PR_ERR("scene shut down again error!");
            }
        }
        return LIGHT_OK;
    }

    *cycle_enable = TRUE;

    //PR_DEBUG("firstly stop shade !");
    _light_ctrl_shade_stop();  //stop other shade firstly

    memset(&light_scene_ctrl_parm, 0, SIZEOF(light_scene_ctrl_t));

    sg_light_ctrl_handle.scene_unit = strlen(sg_light_ctrl_data.scene + 2) / SCENE_UNIT_LENGTH;

    PR_DEBUG("Scene total unit %d  current unit %d", sg_light_ctrl_handle.scene_unit, sg_light_ctrl_handle.unit_cnt);

    PR_DEBUG("before RGBCW: %d %d %d %d %d", sg_light_ctrl_handle.target_val.red, sg_light_ctrl_handle.target_val.green, sg_light_ctrl_handle.target_val.blue,\
                    sg_light_ctrl_handle.target_val.white, sg_light_ctrl_handle.target_val.warm);

    //attention!!!!
    sg_light_ctrl_handle.scene_set_first_flag = sg_light_ctrl_data.scene_first_set;

    if (sg_light_ctrl_handle.scene_set_first_flag) {
        s_cct_warm = 0xFFFF;	//when scene data is new, clear CCT warm set!

        sg_light_ctrl_handle.unit_cnt = 0;

        //color mode or music mode change to scene!
        if ((sg_light_ctrl_handle.current_val.red != 0) || (sg_light_ctrl_handle.current_val.green != 0) || (sg_light_ctrl_handle.current_val.blue != 0)) {
            PR_DEBUG("color mode or music mode change to scene mode!");
            color_to_scene_mode_flag = TRUE;
        }

        //white mode change to scene!
        if ((sg_light_ctrl_handle.current_val.white != 0) || (sg_light_ctrl_handle.current_val.warm != 0)) {
            PR_DEBUG("color mode or music mode change to scene mode!");
            white_to_scene_mode_flag = TRUE;
        }
    }

    sg_light_ctrl_data.scene_first_set = FALSE;
    sg_light_ctrl_handle.scene_set_first_flag = FALSE;     //attention:scene first set，don't display the first unit!!! 

    _light_ctrl_data_calculate_scene(sg_light_ctrl_data.scene + sg_light_ctrl_handle.unit_cnt * SCENE_UNIT_LENGTH + SCNE_HRAD_LENGTH, \
                                &light_scene_ctrl_parm, &sg_light_ctrl_handle.target_val);

    if (BRIGHT_MODE_CCT == sg_light_cfg_data.bright_mode) {      
        //CCT drive mode make sure don't change warm
        if (sg_light_ctrl_handle.target_val.white != 0) {    
            //unit data is white led set cfg
            if ((s_cct_last_rgb_flag) || (color_to_scene_mode_flag)) {
                //last unit is RGB data, this unit is white data
                sg_light_ctrl_handle.current_val.warm = sg_light_ctrl_handle.target_val.warm;

                //set the warm set when unit firstly set as white led
                color_to_scene_mode_flag = FALSE;
            }

            s_cct_warm = sg_light_ctrl_handle.target_val.warm;

            s_cct_last_rgb_flag = FALSE;
        } else {
            if (s_cct_warm != 0XFFFF) {       
                //when RGB... + C or RGB... + W or RGB... + C + W 
                sg_light_ctrl_handle.target_val.warm = s_cct_warm;
            }

            if(white_to_scene_mode_flag) {  
                // white mode chang to scene, and first unit is color!
                sg_light_ctrl_handle.target_val.warm = sg_light_ctrl_handle.current_val.warm;
                s_cct_warm = sg_light_ctrl_handle.target_val.warm;
                white_to_scene_mode_flag = FALSE;
            }

            s_cct_last_rgb_flag = TRUE;		//last unit data is RGB
        }
    }

    PR_DEBUG("before RGBCW: %d %d %d %d %d", sg_light_ctrl_handle.current_val.red, sg_light_ctrl_handle.current_val.green, sg_light_ctrl_handle.current_val.blue,\
                        sg_light_ctrl_handle.current_val.white, sg_light_ctrl_handle.current_val.warm);

    PR_DEBUG("after RGBCW: %d %d %d %d %d", sg_light_ctrl_handle.target_val.red, sg_light_ctrl_handle.target_val.green, sg_light_ctrl_handle.target_val.blue,\
                    sg_light_ctrl_handle.target_val.white, sg_light_ctrl_handle.target_val.warm);

    *cycle_time_ms = light_scene_ctrl_parm.times;

    sg_light_ctrl_handle.unit_cnt++;        //attention:turn on/off on APP,will change the unit num++!
	if (sg_light_ctrl_handle.unit_cnt >= sg_light_ctrl_handle.scene_unit) {
        sg_light_ctrl_handle.unit_cnt = 0;
    }

    PR_DEBUG("scene mode %d",light_scene_ctrl_parm.change_mode);

	switch (light_scene_ctrl_parm.change_mode) {
        case SCENE_STATIC:
            _light_ctrl_shade_step_gain_set(SHADE_STEP_GAIN_DEFAULT);
            *cycle_enable = FALSE;   //only one unit,do't need cycle
            _light_ctrl_shade_start(LIGHT_SHADE_CYCLE);
            break;

        case SCENE_JUMP:
            memcpy(&sg_light_ctrl_handle.current_val, &sg_light_ctrl_handle.target_val, SIZEOF(sg_light_ctrl_handle.target_val));

            ret = tuya_light_set_rgbcw(sg_light_ctrl_handle.current_val.red, sg_light_ctrl_handle.current_val.green, sg_light_ctrl_handle.current_val.blue, \
                                        sg_light_ctrl_handle.current_val.white, sg_light_ctrl_handle.current_val.warm);
            if (ret != LIGHT_OK) {
                PR_ERR("Light ctrl turn on set RGBCW error!");
                return ret;
            }
            break;

        case SCENE_SHADE:
            gain_temp = (UINT_T)(CTRL_CAL_VALUE_RANGE * LIGHT_SHADE_CYCLE / 1.0 / light_scene_ctrl_parm.speed + 0.5);

            _light_ctrl_shade_step_gain_set(gain_temp);

            if (sg_light_ctrl_handle.scene_set_first_flag == TRUE) {
                PR_DEBUG("send first unit firstly!");

                memcpy(&sg_light_ctrl_handle.current_val, &sg_light_ctrl_handle.target_val, SIZEOF(sg_light_ctrl_handle.target_val));

                ret = tuya_light_set_rgbcw(sg_light_ctrl_handle.target_val.red, sg_light_ctrl_handle.target_val.green, sg_light_ctrl_handle.target_val.blue, \
                                            sg_light_ctrl_handle.target_val.white, sg_light_ctrl_handle.target_val.warm);
                if (ret != LIGHT_OK) {
                    PR_ERR("Light ctrl turn on set RGBCW error!");
                    return ret;
                }

                sg_light_ctrl_handle.scene_set_first_flag = FALSE;
            } else {
                PR_DEBUG("send second unit continue!");
                _light_ctrl_shade_start(LIGHT_SHADE_CYCLE);
            }
            break;

        default:
            PR_ERR("Scene change mode err!");
            break;
    }

    return LIGHT_OK;
}

/**
 * @brief: 灯场景变化定时器回调函数
 * @param: none
 * @return: none
 */
STATIC VOID _light_ctrl_scene_change_timer_callback(VOID)
{
    OPERATE_LIGHT ret = -1;
    UINT_T cycle_time = 0;
    BOOL_T eable_flag = 0;

    ret = _light_ctrl_scene_change(&eable_flag, &cycle_time);
    if (ret != LIGHT_OK) {
        PR_ERR("Scene change err!");
    }

    if (eable_flag == FALSE) {
        PR_DEBUG("No need next scene change!");

        ret = tuya_user_software_timer_stop(SCENE_SW_TIMER);      // stop timer dominantly
        if (ret != LIGHT_OK) {
            PR_ERR("stop scene timer error!");
        }
    }

    if (TRUE == eable_flag) {
        ret = tuya_user_software_timer_start(SCENE_SW_TIMER, cycle_time, (VOID*)_light_ctrl_scene_change_timer_callback);
        if (ret != LIGHT_OK) {
            PR_ERR("Scene timer start error!");
        }
    }

    PR_DEBUG("current %d %d %d %d %d", sg_light_ctrl_handle.current_val.red, sg_light_ctrl_handle.current_val.green, \
                sg_light_ctrl_handle.current_val.blue,sg_light_ctrl_handle.current_val.white, sg_light_ctrl_handle.current_val.warm);

    return;
}

/**
 * @brief: 启动灯场景变化
 * @param {IN UINT_T change_time_ms -> change time, unit:ms}
 * @return: none
 */
STATIC VOID _light_ctrl_secne_change_start(IN UINT_T change_time_ms)
{
    OPERATE_LIGHT ret = -1;

    ret = tuya_user_software_timer_start(SCENE_SW_TIMER, change_time_ms, (VOID*)_light_ctrl_scene_change_timer_callback);
    if (ret != LIGHT_OK) {
        PR_ERR("Scene timer start error!");
    }

    return;
}

/**
 * @brief: 停止灯场景变化
 * @param: none
 * @return: none
 */
STATIC VOID _light_ctrl_secne_change_stop(VOID)
{
    OPERATE_LIGHT ret = -1;

    PR_DEBUG("stop scene time!!!!!");

    ret = tuya_user_software_timer_stop(SCENE_SW_TIMER);
    if (ret != LIGHT_OK) {
        PR_ERR("Scene timer start error!");
    }

    return;
}

/**
 * @brief: scene auto restart
 * @param: none
 * @attention: to restart scene when app don't submit the scene data!!
 * @return: none
 */
STATIC VOID _light_ctrl_scene_auto_restart_time_callback(VOID)
{
    OPERATE_LIGHT ret = -1;
    UINT_T cycle_time = 0;
    BOOL_T eable_flag = 0;

    if ((sg_light_ctrl_data.realtime_flag) && (sg_light_ctrl_data.switch_status)) {
        sg_light_ctrl_data.realtime_flag = FALSE;

        sg_light_ctrl_handle.scene_stop_flag = FALSE;

        //start
        ret = _light_ctrl_scene_change(&eable_flag, &cycle_time);   
        if (ret != LIGHT_OK){
            PR_ERR("Scene change err!");
        }

        if (eable_flag == TRUE) {
            _light_ctrl_secne_change_start(cycle_time);
        } else {
            PR_DEBUG("Scene don't need change!");

            //stop timer dominantly
            ret = tuya_user_software_timer_stop(SCENE_SW_TIMER);      
            if (ret != LIGHT_OK) {
                PR_ERR("stop scene timer error!");
            }
        }
   } else {
        //don't need to proc! 
   }
   
    //stop timer dominantly
    ret = tuya_user_software_timer_stop(SCENE_AUTO_RESTART_TIMER);
    if (ret != LIGHT_OK) {
        PR_ERR("scene auto restart timer stop error!");
    }

    return;
}

/**
 * @brief:设置灯的数据
 * @param: none
 * @attention: this function only deal white or color mode when light is on,
 *              light will set value directly!
 * @return: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
OPERATE_RET tuya_light_ctrl_data_extend_active(VOID)
{
    OPERATE_RET ret = 0;

    if (sg_light_ctrl_data.switch_status == FALSE) {
        ret = tuya_light_ctrl_process();
    } else { 
        // only deal white and color mode

        //first stop shade change!
        _light_ctrl_shade_stop();          

        sg_light_ctrl_handle.scene_stop_flag = TRUE;  //stop scene change to make sure don't change scene again

        //stop scene change firstly
        _light_ctrl_secne_change_stop();

        _light_ctrl_data_calculate_rgbcw(sg_light_ctrl_data.mode, &sg_light_ctrl_data, &sg_light_ctrl_handle.target_val);

        PR_DEBUG("%d %d %d %d %d", sg_light_ctrl_handle.target_val.red, sg_light_ctrl_handle.target_val.green, sg_light_ctrl_handle.target_val.blue,\
                                                sg_light_ctrl_handle.target_val.white, sg_light_ctrl_handle.target_val.warm);

        ret = tuya_light_set_rgbcw(sg_light_ctrl_handle.target_val.red, sg_light_ctrl_handle.target_val.green, sg_light_ctrl_handle.target_val.blue, \
                                                sg_light_ctrl_handle.target_val.white, sg_light_ctrl_handle.target_val.warm);

        //make sure current value = target value
        memcpy(&sg_light_ctrl_handle.current_val, &sg_light_ctrl_handle.target_val, SIZEOF(bright_data_t));
        if (ret != OPRT_OK) {
                PR_ERR("Light ctrl turn on set RGBCW error!");
                return ret;
        }
    }

    return ret;
}

/**
 * @brief: light extern set response
 * @param: none
 * @attention: sysytem need apply all dp response or need dp
 * @return: none
 */
WEAK VOID tuya_light_ctrl_data_extend_respone(VOID)
{

}

/**
 * @brief: extern set light data
 * @param[in]: extend_data --> extern set param
 * @param[in]: need_upload --> need upload
 * @param[in]: active_immed --> actice immediately, don't need call opLightCtrl() function
 * @param[in]: need_save --> need to save to flash 
 * @attention: when turn off light, mode bright temperature and rgb will not accept!
 *                                only set white mode or color mode.
 * @return: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
OPERATE_RET tuya_light_ctrl_data_extend_set(IN light_ctrl_extend_data_t *extend_data, IN BOOL_T need_upload, IN BOOL_T active_immed, IN BOOL_T need_save)
{
    OPERATE_RET ret = -1;

    if (WHITE_MODE == extend_data->mode) {    
        // adjust CW 
        if ((extend_data->bright < CTRL_CW_BRIGHT_VALUE_MIN) || (extend_data->bright > CTRL_CW_BRIGHT_VALUE_MAX)) {
            PR_ERR("bright value is exceed range,set error");
            return OPRT_INVALID_PARM;
        }

        if (extend_data->temper > CTRL_CW_BRIGHT_VALUE_MAX) {
            PR_ERR("temperature value is exceed range,set error:%d", extend_data->temper);
            return OPRT_INVALID_PARM;
        }

        sg_light_ctrl_data.switch_status = extend_data->switch_status;

        sg_light_ctrl_data.bright = extend_data->bright;

        sg_light_ctrl_data.temper = extend_data->temper;

        sg_light_ctrl_data.mode = WHITE_MODE;    //change mode to white mode forcibly

        if (active_immed) {
            ret = tuya_light_ctrl_data_extend_active();
            if (ret != OPRT_OK) {
                PR_ERR("extern active Immediately error");
                return ret;
            }
        }
    } else if (COLOR_MODE == extend_data->mode) {

        if ((extend_data->color.red > CTRL_CAL_VALUE_RANGE) ||
            (extend_data->color.green > CTRL_CAL_VALUE_RANGE) ||
            (extend_data->color.blue > CTRL_CAL_VALUE_RANGE)) {

            PR_ERR("RGB color value is exceed range,set error");
            return OPRT_INVALID_PARM;
        }

        sg_light_ctrl_data.switch_status = extend_data->switch_status;

        sg_light_ctrl_data.mode = COLOR_MODE;    //change mode to color mode

        memcpy(&sg_light_ctrl_data.color_origin, &(extend_data->color_origin), SIZEOF(color_origin_t));    //must save the origin data

        memcpy(&sg_light_ctrl_data.color, &(extend_data->color), SIZEOF(color_rgb_t));

        if (active_immed) {
            ret = tuya_light_ctrl_data_extend_active();
            if (ret != OPRT_OK) {
                PR_ERR("extern active Immediately error");
                return ret;
            }
        }

    }else{
        PR_DEBUG("tuya_light_ctrl_data_extend_set scene don't need deal!");

        sg_light_ctrl_data.switch_status = extend_data->switch_status;

        sg_light_ctrl_data.mode = extend_data->mode;

    }


    if (need_upload) {
        tuya_light_ctrl_data_extend_respone();
    }

    if (need_save) {
        ret = tuya_light_ctrl_data_auto_save_start(5000);
        if (ret != OPRT_OK) {
            PR_ERR("extern auto save time start error");
            return ret;
        }
    }

    return OPRT_OK;
}

/**
 * @berief: Light control proc
 * @param: none
 * @return: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_light_ctrl_process(VOID)
{
    OPERATE_LIGHT ret = -1;
    UINT_T cycle_time = 0;
    BOOL_T eable_flag = 0;
    STATIC BOOL_T s_last_switch_status = FALSE;

    //检查灯是否已经初始化
    CHECK_INIT_OK();

#if (LIGHT_CFG_SUPPORT_LOWPOWER == 1)
    if (sg_light_cfg_data.title20) {      
        //使能低功耗的程序

        //低功耗程序
        ret = _light_ctrl_low_power_process(sg_light_ctrl_data.switch_status);    
        if (ret != LIGHT_OK) {
            PR_ERR("Lowpower proc error!");
        }
    }
#endif

    if (sg_light_ctrl_data.switch_status == FALSE) { 
        //onoff ctrl state -- turn off

        //停止所有的渐变程序!
        tuya_light_shade_ctrl_disable();      

        memset(&sg_light_ctrl_handle.target_val, 0, sizeof(bright_data_t));  //set target contol data!!!

        if (BRIGHT_MODE_CCT == sg_light_cfg_data.bright_mode) {  
            //CCT drive mode make sure don't change warm
            sg_light_ctrl_handle.target_val.warm = sg_light_ctrl_handle.current_val.warm;
        }

        if (SWITCH_GRADUAL == sg_light_cfg_data.switch_mode) {
            //渐变turn on/off
            switch (sg_light_ctrl_data.mode) {
                case WHITE_MODE:
                case COLOR_MODE:
                    PR_DEBUG("start to shut down!");

                    _light_ctrl_shade_step_gain_set(SHADE_STEP_GAIN_DEFAULT);

                    _light_ctrl_shade_start(LIGHT_SHADE_CYCLE);

                    PR_NOTICE("current %d %d %d %d %d", sg_light_ctrl_handle.current_val.red, sg_light_ctrl_handle.current_val.green, \
                                sg_light_ctrl_handle.current_val.blue, sg_light_ctrl_handle.current_val.white, sg_light_ctrl_handle.current_val.warm);

                    PR_NOTICE("target %d %d %d %d %d", sg_light_ctrl_handle.target_val.red, sg_light_ctrl_handle.target_val.green, \
                                sg_light_ctrl_handle.target_val.blue, sg_light_ctrl_handle.target_val.white, sg_light_ctrl_handle.target_val.warm);
                    break;

                case SCENE_MODE:

                    PR_DEBUG("current %d %d %d %d %d", sg_light_ctrl_handle.current_val.red, sg_light_ctrl_handle.current_val.green, \
                        sg_light_ctrl_handle.current_val.blue, sg_light_ctrl_handle.current_val.white, sg_light_ctrl_handle.current_val.warm);

                    PR_DEBUG("turn off disable scene timer flag!");

                    _light_ctrl_shade_step_gain_set(SHADE_STEP_GAIN_DEFAULT);

                    _light_ctrl_shade_start(LIGHT_SHADE_CYCLE);
                    break;

                case MUSIC_MODE:
                    ret = tuya_light_set_rgbcw(sg_light_ctrl_handle.target_val.red, sg_light_ctrl_handle.target_val.green, sg_light_ctrl_handle.target_val.blue, \
                                                sg_light_ctrl_handle.target_val.white, sg_light_ctrl_handle.target_val.warm);

                    memset(&sg_light_ctrl_handle.current_val, 0, sizeof(sg_light_ctrl_handle.current_val));

                    if (ret != LIGHT_OK) {
                        PR_ERR("Light ctrl music mode shutdown set RGBCW error!");
                        return ret;
                    }
                    break;

                default:
                    break;
            }
        } else if(SWITCH_DIRECT == sg_light_cfg_data.switch_mode) {   
            //直接turn on/off

            sg_light_ctrl_handle.scene_stop_flag = TRUE;

            _light_ctrl_secne_change_stop();

            ret = tuya_light_set_rgbcw(sg_light_ctrl_handle.target_val.red, sg_light_ctrl_handle.target_val.green, sg_light_ctrl_handle.target_val.blue, \
                                        sg_light_ctrl_handle.target_val.white, sg_light_ctrl_handle.target_val.warm);

            memset(&sg_light_ctrl_handle.current_val, 0, sizeof(sg_light_ctrl_handle.current_val));

            if (ret != LIGHT_OK) {
                PR_ERR("Light ctrl music mode shutdown set RGBCW error!");
                return ret;
            }

            //make sure current value to clear
            memcpy(&sg_light_ctrl_handle.current_val, &sg_light_ctrl_handle.target_val, SIZEOF(bright_data_t));

            if ((BRIGHT_MODE_CCT == sg_light_cfg_data.bright_mode) && (FALSE == sg_light_ctrl_data.switch_status)) {
                ret = tuya_user_software_timer_start(CCT_DELAY_SHUT_DOWN_TIMER, 1000, _light_ctrl_cct_shut_down_time_callback);
                if (ret != LIGHT_OK) {
                    PR_ERR("CCT shut down err!");

                    //when CCT shut down, need to turn off CCT
                    ret = tuya_light_set_rgbcw(0, 0, 0, 0, 0);
                }
            }
        } else {   
            //LightCfgData.Switch_mode 
            ;       
        }
    } else {    
        //onoff ctrl state -- turn on 

        ret = tuya_user_software_timer_stop(CCT_DELAY_SHUT_DOWN_TIMER);
        if (ret != OPRT_OK) {
            PR_ERR("CCT shut down delay timer stop!");
        }
        PR_DEBUG("current mode %d", sg_light_ctrl_data.mode);

        if (sg_light_ctrl_data.mode == SCENE_MODE) {
            sg_light_ctrl_handle.scene_stop_flag = FALSE;

            ret = _light_ctrl_scene_change(&eable_flag, &cycle_time);   //start
            if (ret != LIGHT_OK){
                PR_ERR("Scene change err!");
            }

            if (eable_flag == TRUE) {
                _light_ctrl_secne_change_start(cycle_time);
            } else {
                PR_DEBUG("Scene don't need change!");

                ret = tuya_user_software_timer_stop(SCENE_SW_TIMER);      //stop timer dominantly
                if (ret != LIGHT_OK) {
                    PR_ERR("stop scene timer error!");
                }
            }
        } else if (sg_light_ctrl_data.mode == MUSIC_MODE) {   
            //mode is music mode!!!! 
            //this mode only appear turn on firstly

            sg_light_ctrl_handle.scene_stop_flag = TRUE;      //stop scene
            _light_ctrl_secne_change_stop();

            light_ctrl_data_t music_ctrl_data;
            memset(&music_ctrl_data, 0, sizeof(light_ctrl_data_t));
            memcpy(&music_ctrl_data, &sg_light_ctrl_data, SIZEOF(light_ctrl_data_t));     //make sure music mode restart as red color

            music_ctrl_data.color.red = CTRL_RGB_BRIGHT_VALUE_MAX;
            music_ctrl_data.color.green = 0;
            music_ctrl_data.color.blue = 0;

            _light_ctrl_data_calculate_rgbcw(sg_light_ctrl_data.mode, &music_ctrl_data, &sg_light_ctrl_handle.target_val);

            PR_NOTICE("%d %d %d %d %d",sg_light_ctrl_handle.target_val.red, sg_light_ctrl_handle.target_val.green, sg_light_ctrl_handle.target_val.blue,\
                                sg_light_ctrl_handle.target_val.white, sg_light_ctrl_handle.target_val.warm);

            if (BRIGHT_MODE_CCT == sg_light_cfg_data.bright_mode) {   
                //CCT drive mode make sure don't change warm
                sg_light_ctrl_handle.current_val.warm = sg_light_ctrl_handle.target_val.warm;
            }

            _light_ctrl_shade_step_gain_set(SHADE_STEP_GAIN_DEFAULT);

            _light_ctrl_shade_start(LIGHT_SHADE_CYCLE);

        } else {    
            // mode is not scene and music mode ,mode is white or color mode !

            sg_light_ctrl_handle.scene_stop_flag = TRUE;  //stop scene 
            _light_ctrl_secne_change_stop();

            if (s_last_switch_status != FALSE) {        
                //already turn on

                _light_ctrl_data_calculate_rgbcw(sg_light_ctrl_data.mode, &sg_light_ctrl_data, &sg_light_ctrl_handle.target_val);

                PR_DEBUG("%d %d %d %d %d",sg_light_ctrl_handle.target_val.red, sg_light_ctrl_handle.target_val.green, sg_light_ctrl_handle.target_val.blue,\
                            sg_light_ctrl_handle.target_val.white, sg_light_ctrl_handle.target_val.warm);

                _light_ctrl_shade_step_gain_set(SHADE_STEP_GAIN_DEFAULT);

                _light_ctrl_shade_start(LIGHT_SHADE_CYCLE);

            } else {       
                //firstly turn on

                if (SWITCH_GRADUAL == sg_light_cfg_data.switch_mode) {
                    _light_ctrl_data_calculate_rgbcw(sg_light_ctrl_data.mode, &sg_light_ctrl_data, &sg_light_ctrl_handle.target_val);

                    PR_DEBUG("%d %d %d %d %d",sg_light_ctrl_handle.target_val.red, sg_light_ctrl_handle.target_val.green, sg_light_ctrl_handle.target_val.blue,\
                                        sg_light_ctrl_handle.target_val.white, sg_light_ctrl_handle.target_val.warm);

                    if (BRIGHT_MODE_CCT == sg_light_cfg_data.bright_mode) {  
                        //CCT drive mode make sure don't change warm 

                        PR_DEBUG("CCT need to proc!");

                        sg_light_ctrl_handle.current_val.warm = sg_light_ctrl_handle.target_val.warm;
                    }

                    _light_ctrl_shade_step_gain_set(SHADE_STEP_GAIN_DEFAULT);

                    _light_ctrl_shade_start(LIGHT_SHADE_CYCLE);

                } else if (SWITCH_DIRECT == sg_light_cfg_data.switch_mode) {
                    _light_ctrl_data_calculate_rgbcw(sg_light_ctrl_data.mode,  &sg_light_ctrl_data, &sg_light_ctrl_handle.target_val);

                    PR_DEBUG("%d %d %d %d %d",sg_light_ctrl_handle.target_val.red,sg_light_ctrl_handle.target_val.green,sg_light_ctrl_handle.target_val.blue,\
                                        sg_light_ctrl_handle.target_val.white,sg_light_ctrl_handle.target_val.warm);

                    if (BRIGHT_MODE_CCT == sg_light_cfg_data.bright_mode) {  
                        //CCT drive mode make sure don't change warm

                        sg_light_ctrl_handle.current_val.warm = sg_light_ctrl_handle.target_val.warm;
                    }

                    ret = tuya_light_set_rgbcw(sg_light_ctrl_handle.target_val.red, sg_light_ctrl_handle.target_val.green, sg_light_ctrl_handle.target_val.blue, \
                                               sg_light_ctrl_handle.target_val.white, sg_light_ctrl_handle.target_val.warm);

                    //make sure current value = target value
                    memcpy(&sg_light_ctrl_handle.current_val, &sg_light_ctrl_handle.target_val, SIZEOF(bright_data_t));

                    if (ret != LIGHT_OK) {
                        PR_ERR("Light ctrl turn on set RGBCW error!");
                        return ret;
                    }
                } else {   
                    //LightCfgData.Switch_mode
                    ;      
                }
            }
       }
    }

    s_last_switch_status = sg_light_ctrl_data.switch_status;

    return LIGHT_OK;
}

/**
 * @brief: 失能灯的控制
 * @param: none
 * @return: none
 */
VOID tuya_light_ctrl_disable(VOID)
{
    OPERATE_LIGHT ret = -1;
    UCHAR_T i = 0;

    ret = tuya_user_hardware_timer_stop();
    if (ret != LIGHT_OK) {
        PR_ERR("hardware timer stop error!");
    }

    for (i = 0; i < SW_TIMER_MAX; i++) {
        ret = tuya_user_software_timer_stop(i);
        if (ret != LIGHT_OK) {
           PR_ERR("stop %d timer error!", i);
        }
    }

    //重置并保存上电次数
    ret = tuya_user_flash_write_reset_cnt(0);
    if (ret != LIGHT_OK) {
        PR_ERR("Reset cnt clear error!");
    }

#if (LIGHT_CFG_SUPPORT_LOWPOWER == 1)
    if (sg_light_cfg_data.title20) {
        tuya_light_sys_ctrl_low_power_exist();
    }
#endif

    return;
}

/**
 * @brief: 失能渐变程序
 * @param: none
 * @return: none
 */
VOID tuya_light_shade_ctrl_disable(VOID)
{
    //先停止渐变!
    _light_ctrl_shade_stop();                 

    //停止场景变化
    sg_light_ctrl_handle.scene_stop_flag = TRUE;
    _light_ctrl_secne_change_stop();   

    return;
}

/**
 * @brief: countdown lave time return proc
 * @param[out] remain_time_sec -> left time,unit:sec
 * @attention: this function need to implement in system
 * @return: none
 */
WEAK VOID tuya_light_ctrl_data_countdown_response(OUT UINT_T remain_time_sec)
{
    ;
}

/**
 * @brief: light ctrl breath proc
 * @param: none
 * @return: none
 */
STATIC VOID _light_ctrl_countdown_timer_callback(VOID)
{
    OPERATE_LIGHT ret = -1;

    if (sg_light_ctrl_data.countdown > 1) {   
         //to avoid CountDown = 0 %60 also equal to 0 

        sg_light_ctrl_data.countdown--;

        if ((sg_light_ctrl_data.countdown % 60) == 0) {     
            //upload countdown value per min
            tuya_light_ctrl_data_countdown_response(sg_light_ctrl_data.countdown);
        }

    } else {
        sg_light_ctrl_data.countdown = 0;

        sg_light_ctrl_data.switch_status = (sg_light_ctrl_data.switch_status != FALSE) ? FALSE : TRUE;

        ret = tuya_light_ctrl_process();
        if (ret != LIGHT_OK) {
            PR_ERR("CountDown process error!");
        }

        //opload after ctrl deal with
        tuya_light_ctrl_data_countdown_response(0);      

        //stop timer 
        ret = tuya_user_software_timer_stop(COUNTDOWN_SW_TIMER); 
        if (ret != LIGHT_OK) {
           PR_ERR("stop countdown software timer error!");
        }

        ret = tuya_light_ctrl_data_auto_save_start(5000);
        if (ret != LIGHT_OK) {
            PR_ERR("coutdown auto save time start error");
        }

        return;

    }

    ret = tuya_user_software_timer_start(COUNTDOWN_SW_TIMER, 1000, (VOID*)_light_ctrl_countdown_timer_callback);
    if (ret != LIGHT_OK) {
        PR_ERR("CountDown timer restart error!");
    }

}

/**
 * @brief: set light countdown value
 * @param[in]  countdown_sec -> unit:second
 * @attention: countdown lave time will return with
 *              calling vLightDataCountDownResponse function every minutes.
 *              switch status will return with calling
 *              tuya_light_ctrl_data_switch_respone function when countdown active.
 * @return: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_light_ctrl_data_countdown_set(IN INT_T countdown_sec)
{
    OPERATE_LIGHT ret = -1;

    if ((countdown_sec < 0)|| (countdown_sec > 86400)) {
        PR_ERR("Set countdwon value error!");
        return LIGHT_INVALID_PARM;
    }

    sg_light_ctrl_data.countdown = countdown_sec;
    if(countdown_sec <= 0) {

        //cancel coutdown proc
        ret = tuya_user_software_timer_stop(COUNTDOWN_SW_TIMER);     
        if (ret != LIGHT_OK) {
            PR_ERR("Stop countdown timer error!");
        }

        // don't need response switch status, because user set initiactive 
        tuya_light_ctrl_data_countdown_response(0);                 

        return LIGHT_OK;

    } else {
        ret = tuya_user_software_timer_start(COUNTDOWN_SW_TIMER, 1000, (VOID*)_light_ctrl_countdown_timer_callback);
        if (ret != LIGHT_OK) {
            PR_ERR("Start countdown timer error!");
        }
    }

    //upload countdown value
    tuya_light_ctrl_data_countdown_response(countdown_sec);     

    return LIGHT_OK;
}

/**
 * @brief: set light realtime control data
 * @param[in]: music_flag
 * @param[in]: realtime_data
 * @attention: data format: please reference to DP protocol
 * @return: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_light_ctrl_data_realtime_adjust_set(IN BOOL_T music_flag, IN CHAR_T *realtime_data)
{
    if (music_flag) {
        if (sg_light_ctrl_data.mode != MUSIC_MODE) {
            PR_DEBUG("Music data don't accpected, bacause mode is not music mode!");
            return LIGHT_INVALID_PARM;
        }
    }

    if (strcmp(realtime_data, sg_light_ctrl_data.realtime_data) == 0) {
        PR_DEBUG("the same realtime adjust data");
        return LIGHT_INVALID_PARM;
    }

    if (strlen(realtime_data) != 21) {
        PR_ERR("Real time adjust data is error! please chek!");
        return LIGHT_INVALID_PARM;
    }

    strcpy((CHAR_T*)&sg_light_ctrl_data.realtime_data, realtime_data);

    PR_DEBUG("light real time adjust ctrl data buf %s", sg_light_ctrl_data.realtime_data);

    return LIGHT_OK;
}

/**
 * @brief: Light realtime ctrl process
 * @param: none
 * @return: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_light_realtime_ctrl_process(VOID)
{
    UCHAR_T change_mode = 0;
    USHORT_T bright = 0;
    USHORT_T temper = 0;
    USHORT_T hue = 0;
    USHORT_T sat = 0;
    USHORT_T val = 0;
    light_ctrl_data_t ctrl_data_temp;
    OPERATE_LIGHT ret = -1;

    if (!sg_light_ctrl_data.switch_status) {       
        //make sure realtime adjust dp can't active!

        return LIGHT_OK;
    }

    //stop all shade process!
    tuya_light_shade_ctrl_disable();

    _light_hsvbt_analysize(&sg_light_ctrl_data.realtime_data[1], &hue, &sat, &val, &bright, &temper);

    val = _light_ctrl_data_calculate_hsv_limit(val);

    PR_DEBUG("hsv %d %d %d", hue, sat, val);

    memset(&ctrl_data_temp, 0, SIZEOF(light_ctrl_data_t));

    if ((WHITE_MODE == sg_light_ctrl_data.mode) || (COLOR_MODE == sg_light_ctrl_data.mode)) {

        //mix group proc!!!!
        //attention：only recevice correct data according to lightway
        switch (sg_light_cfg_data.light_way) {
            case LIGHT_C:
                if (val > 0) {    
                    //when mix group, if receive color realtime data,just stand
                    ctrl_data_temp.bright = sg_light_ctrl_data.bright;
                } else {
                    ctrl_data_temp.bright = bright;
                }
                ctrl_data_temp.temper = CTRL_CW_BRIGHT_VALUE_MAX;
                break;

            case LIGHT_CW:
                if (val > 0 ) { 
                    //when mix group, if receive color realtime data,just stand
                    ctrl_data_temp.bright = sg_light_ctrl_data.bright;
                    ctrl_data_temp.temper = sg_light_ctrl_data.temper;
                } else {
                    ctrl_data_temp.bright = bright;
                    ctrl_data_temp.temper = temper;
                }
                break;

            case LIGHT_RGB:
                if (bright >= CTRL_CW_BRIGHT_VALUE_MIN) { 
                    //when mix group, if receive white realtime data,just stand
                    ctrl_data_temp.color.red = sg_light_ctrl_data.color.red;
                    ctrl_data_temp.color.green = sg_light_ctrl_data.color.green;
                    ctrl_data_temp.color.blue = sg_light_ctrl_data.color.blue;
                } else {
                    tuya_light_tool_hsv_to_rgb(hue, sat, val, &ctrl_data_temp.color.red, &ctrl_data_temp.color.green, &ctrl_data_temp.color.blue);
                }
                break;

            case LIGHT_RGBC:
                ctrl_data_temp.bright = bright;
                ctrl_data_temp.temper = CTRL_CW_BRIGHT_VALUE_MAX;
                tuya_light_tool_hsv_to_rgb(hue, sat, val, &ctrl_data_temp.color.red, &ctrl_data_temp.color.green, &ctrl_data_temp.color.blue);
                break;

            case LIGHT_RGBCW:
                ctrl_data_temp.bright = bright;
                ctrl_data_temp.temper = temper;
                tuya_light_tool_hsv_to_rgb(hue, sat, val, &ctrl_data_temp.color.red, &ctrl_data_temp.color.green, &ctrl_data_temp.color.blue);
                break;

            default:
                break;

        }

        _light_ctrl_data_calculate_cw(ctrl_data_temp.bright, ctrl_data_temp.temper, &sg_light_ctrl_handle.target_val);

        _light_ctrl_data_calculate_rgb(&(ctrl_data_temp.color), &sg_light_ctrl_handle.target_val);

        PR_DEBUG("realtime RGBCW %d %d %d %d %d", sg_light_ctrl_handle.target_val.red, sg_light_ctrl_handle.target_val.green, sg_light_ctrl_handle.target_val.blue,\
                  sg_light_ctrl_handle.target_val.white, sg_light_ctrl_handle.target_val.warm);

    } else if(SCENE_MODE == sg_light_ctrl_data.mode)  {   
        //scene mode 

        //have set realtime data
        sg_light_ctrl_data.realtime_flag = TRUE;   

        memset(&ctrl_data_temp, 0, SIZEOF(light_ctrl_data_t));

        ctrl_data_temp.bright = bright;
        ctrl_data_temp.temper = temper;

        PR_DEBUG(" scene hsv %d %d %d", hue, sat, val);

        tuya_light_tool_hsv_to_rgb(hue, sat, val, &ctrl_data_temp.color.red, &ctrl_data_temp.color.green, &ctrl_data_temp.color.blue);

        _light_ctrl_data_calculate_rgbcw(sg_light_ctrl_data.mode, &ctrl_data_temp, &sg_light_ctrl_handle.target_val);

    } else {  
        //music mode

        memset(&ctrl_data_temp, 0, SIZEOF(light_ctrl_data_t));

        ctrl_data_temp.bright = bright;
        ctrl_data_temp.temper = temper;

        PR_DEBUG(" scene hsv %d %d %d", hue, sat, val);

        tuya_light_tool_hsv_to_rgb(hue, sat, val, &ctrl_data_temp.color.red, &ctrl_data_temp.color.green, &ctrl_data_temp.color.blue);

        _light_ctrl_data_calculate_cw(ctrl_data_temp.bright, ctrl_data_temp.temper, &sg_light_ctrl_handle.target_val);
        _light_ctrl_data_calculate_rgb(&(ctrl_data_temp.color), &sg_light_ctrl_handle.target_val);

        if (BRIGHT_MODE_CCT == sg_light_cfg_data.bright_mode) { 
            //as music mode realtime will send jump, also need keep warm when cct 

            sg_light_ctrl_handle.target_val.warm = sg_light_ctrl_handle.current_val.warm ;
        }
    }

    change_mode = sg_light_ctrl_data.realtime_data[0] - '0';


    if (REALTIME_CHANGE_JUMP == change_mode) {

        memcpy(&sg_light_ctrl_handle.current_val, &sg_light_ctrl_handle.target_val, SIZEOF(sg_light_ctrl_handle.target_val));

        PR_DEBUG("%d %d %d %d %d",sg_light_ctrl_handle.target_val.red, sg_light_ctrl_handle.target_val.green, sg_light_ctrl_handle.target_val.blue,\
                                  sg_light_ctrl_handle.target_val.white, sg_light_ctrl_handle.target_val.warm);

        ret = tuya_light_set_rgbcw(sg_light_ctrl_handle.target_val.red, sg_light_ctrl_handle.target_val.green, sg_light_ctrl_handle.target_val.blue, \
                                   sg_light_ctrl_handle.target_val.white, sg_light_ctrl_handle.target_val.warm);
        if(ret != LIGHT_OK) {
            PR_ERR("Light ctrl turn on set RGBCW error!");
            return ret;
        }
    } else {
        USHORT_T max_error;
        max_error = _light_get_shade_max(&sg_light_ctrl_handle.target_val, &sg_light_ctrl_handle.current_val);

        // 150ms APP issue realtime control dp
        _light_ctrl_shade_step_gain_set((UINT_T)((max_error * LIGHT_SHADE_CYCLE / 300.0)));   

        _light_ctrl_shade_start(LIGHT_SHADE_CYCLE);
    }

    return LIGHT_OK;
}

/**
 * @brief: Light reset cnt clear timercallback
 * @param: none
 * @return: none
 */
STATIC VOID _light_ctrl_reset_cnt_clear_timer_callback(VOID)
{
    OPERATE_LIGHT ret = -1;

    PR_DEBUG("reset cnt clear!");

    //重置并保存上电次数
    ret = tuya_user_flash_write_reset_cnt(0);     
    if (ret != LIGHT_OK) {
        PR_ERR("Reset cnt clear error!");
        return ;
    }

    //stop timer dominantly
    ret = tuya_user_software_timer_stop(CLEAR_RESET_CNT_SW_TIMER);       
    if (ret != LIGHT_OK) {
        PR_ERR("stop reset clear timer error!");
    }

    return;
}

/**
 * @brief: system reboot as hardware mode judge proc
 * @param: none
 * @attention: this function need to implement by each plantform.
 * @retval: BOOL_T TRUE -> system reboot
 */
WEAK BOOL_T tuya_light_sys_hardware_reboot_judge(VOID)
{
    return TRUE;
}

/**
 * @berief: Light hardware reboot judge & proc
 *          process detail:
 *                  1. hardware reset judge;
 *                  2. load reboot cnt data;
 *                  3. reboot cnt data increase;
 *                  4. start software time to clear reboot cnt;
 * @param: none
 * @attention: this function need tuya_light_sys_hardware_reboot_judge();
 * @return: none
 */
VOID tuya_light_ctrl_hardware_reboot_process(VOID)
{
    OPERATE_LIGHT ret = -1;
    BOOL_T hw_reboot_flag = FALSE;
    UCHAR_T cnt = 0;

    hw_reboot_flag = tuya_light_sys_hardware_reboot_judge();
    if (TRUE != hw_reboot_flag) {
        return;
    }

    PR_DEBUG("Light hardware reboot, turn on light!");

    //read cnt from flash
    ret = tuya_user_flash_read_reset_cnt(&cnt);     
    if (ret != LIGHT_OK) {
        PR_ERR("Read reset cnt error!");
    }

    PR_DEBUG("read reset cnt %d",cnt);

    cnt++;

    //更新上电次数
    ret = tuya_user_flash_write_reset_cnt(cnt);
    if (ret != LIGHT_OK) {
        PR_ERR("Reset cnt add write error!");
        return ;
    }

    PR_DEBUG("start reset cnt clear timer!!!!!");

    //start clear reset cnt timer
    ret = tuya_user_software_timer_start(CLEAR_RESET_CNT_SW_TIMER, 5000, (VOID*)_light_ctrl_reset_cnt_clear_timer_callback);
    if (ret != LIGHT_OK) {
        PR_ERR("start reset clear timer error!");
    }

    return;
}

/**
 * @brief: 保存灯的控制数据
 * @param: none
 * @attention: this function directly save ctrl data.
 * @return: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_light_ctrl_data_auto_save(VOID)
{
    OPERATE_LIGHT ret = -1;
    light_app_data_flash_t save_data;

    if (sg_light_cfg_data.memory != TRUE) {
        return LIGHT_OK;
    }

    memset(&save_data, 0, SIZEOF(light_app_data_flash_t));

    save_data.power = sg_light_ctrl_data.switch_status;
    save_data.mode = sg_light_ctrl_data.mode;
    save_data.bright = sg_light_ctrl_data.bright;
    save_data.temper = sg_light_ctrl_data.temper;
    save_data.color.red = sg_light_ctrl_data.color.red;
    save_data.color.green = sg_light_ctrl_data.color.green;
    save_data.color.blue = sg_light_ctrl_data.color.blue;
    save_data.color_origin.hue = sg_light_ctrl_data.color_origin.hue;
    save_data.color_origin.sat = sg_light_ctrl_data.color_origin.sat;
    save_data.color_origin.value  = sg_light_ctrl_data.color_origin.value;
    strcpy((CHAR_T*)&save_data.color_origin.color_str, (CHAR_T*)&sg_light_ctrl_data.color_origin.color_str);
    memcpy(&save_data.scene, &sg_light_ctrl_data.scene, SIZEOF(sg_light_ctrl_data.scene));

#if 0
    PR_DEBUG("flash mode: %d",save_data.mode);
    PR_DEBUG("flash on/off: %d",save_data.power);
    PR_DEBUG("flash R: %d",save_data.color.red);
    PR_DEBUG("flash G: %d",save_data.color.green);
    PR_DEBUG("flash B: %d",save_data.color.blue);
    PR_DEBUG("flash B: %d",save_data.bright);
    PR_DEBUG("flash T: %d",save_data.temper);
    PR_DEBUG("flash raw data1: %d",save_data.color_origin.usColorData1);
    PR_DEBUG("flash raw data2: %d",save_data.color_origin.usColorData2);
    PR_DEBUG("flash raw data3: %d",save_data.color_origin.usColorData3);
    PR_DEBUG("flash scene data: %s",save_data.scene);
#endif
    //将save_data数据写入flash中
    ret = tuya_user_flash_write_app_data(&save_data);
    if (ret != LIGHT_OK) {
        PR_ERR("Write app data ERROR!");
        return LIGHT_COM_ERROR;
    }

    return LIGHT_OK;
}

/**
 * @brief: system reset proc
 * @param: none
 * @attention: this function need implememt by system,
 *              need to deal with different thing in each plantform.
 * @return: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
WEAK OPERATE_LIGHT tuya_light_sys_reset_cnt_over_callback(VOID)
{
    return LIGHT_OK;
}

/**
 * @brief: system CCT drive mode delya timer cb
 * @param: none
 * @attention: this active will increase reset time!
 * @return: none
 */
STATIC VOID _light_ctrl_cct_delay_reset_time_callback(VOID)
{
    OPERATE_LIGHT ret = -1;

    ret = tuya_user_software_timer_stop(CCT_DELAY_RESET_TIMER); 
    if (ret != LIGHT_OK) {
        PR_ERR("CCT delay reset timer stop error!");
    }

    PR_DEBUG("CCT delay 200ms reset factory!");

    //system reset deal proc
    ret = tuya_light_sys_reset_cnt_over_callback();     
    if (ret != LIGHT_OK) {
        PR_ERR("Light reset proc error!");
    }

    return;
}

/**
 * @berief: 灯上电次数清零
 * @param: none
 * @attention: this func will call in tuya_light_ctrl_reset_cnt_process
 *              when no tuya_light_sys_reset_cnt_over_callback finish callback, otherwise
 *               call in system!!!
 * @return: none
 */
VOID tuya_light_ctrl_reset_cnt_clear(VOID)
{
    OPERATE_RET ret = -1;

    //重置并保存上电次数
    ret = tuya_user_flash_write_reset_cnt(0);    
    if (ret != OPRT_OK) {
        PR_ERR("reset cnt set error!");
    }

    return;
}

/**
 * @brief: Light reset to re-distribute proc
 * @param: none
 * @attention: this func will call tuya_light_sys_reset_cnt_over_callback()
 *              tuya_light_sys_reset_cnt_over_callback() need to implement by system
 * @return: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_light_ctrl_reset_cnt_process(VOID)
{
    OPERATE_LIGHT ret = -1;
    UCHAR_T cnt = 0;

    //从flash中读取上电次数
    ret = tuya_user_flash_read_reset_cnt(&cnt);    
    if (ret != LIGHT_OK) {
        PR_ERR("Read reset cnt error!");
        return LIGHT_COM_ERROR;
    }

    if (cnt < sg_light_cfg_data.reset_cnt) {
        PR_DEBUG("Don't reset ctrl data!");
        return LIGHT_OK;
    }

    PR_DEBUG("Reset ctrl data!");

    //重置并保存灯的控制数据
    tuya_light_ctrl_data_reset();                  
    ret = tuya_light_ctrl_data_auto_save();
    if (ret != LIGHT_OK) {
        PR_ERR("reset ctrl data error!");
    }

    //attention：this function please inmplement by function note!
    //灯上电次数清零
    tuya_light_ctrl_reset_cnt_clear();

    ret = tuya_user_software_timer_stop(CLEAR_RESET_CNT_SW_TIMER);
    if (ret != OPRT_OK) {
        PR_ERR("clear cnt time stop error!");
    }

    if (BRIGHT_MODE_CCT == sg_light_cfg_data.bright_mode) {
        ret = tuya_light_set_rgbcw(0, 0, 0, 0, sg_light_ctrl_data.temper);
        if (ret != LIGHT_OK) {
            PR_ERR("CCT reset shut down bright firstly error!");
        }

        ret = tuya_user_software_timer_start(CCT_DELAY_RESET_TIMER, 200, (VOID*)_light_ctrl_cct_delay_reset_time_callback);
        if (ret != LIGHT_OK) {
            PR_ERR("CCT reset delay start timer error!");
        } else {
            return LIGHT_OK;
        }
    }

    PR_NOTICE("Light will reset!");

    //system reset deal proc
    ret = tuya_light_sys_reset_cnt_over_callback();     
    if (ret != LIGHT_OK) { 
        PR_ERR("Light reset proc error!");
    }

    return ret;
}

/**
 * @brief: get connect mode cfg
 * @param: none
 * @return: connect mode
 */
UCHAR_T tuya_light_ctrl_get_connect_mode(VOID)
{
    return (sg_light_cfg_data.connect_mode);
}

/**
 * @brief: get connect time cfg
 * @param: none
 * @return: pair_time
 */
UCHAR_T tuya_light_ctrl_get_connect_time(VOID)
{
    return (sg_light_cfg_data.pair_time);
}

/**
 * @brief: get color max limit
 * @param: none
 * @return: limit_rgb_max
 */
UCHAR_T tuya_light_ctrl_get_color_max(VOID)
{
    return (sg_light_cfg_data.limit_rgb_max);
}

/**
 * @brief: get color min limit
 * @param: none
 * @return: limit_rgb_min
 */
UCHAR_T tuya_light_ctrl_get_color_min(VOID)
{
    return (sg_light_cfg_data.limit_rgb_min);
}

/* @brief: light ctrl data autosave timer callback
 * @param: none
 * @return: none
 */
STATIC VOID _ligth_ctrl_data_auto_save_timer_callback(VOID)
{
    OPERATE_LIGHT ret = -1;

    PR_DEBUG("auto save dp ctrl!");

    ret = tuya_light_ctrl_data_auto_save();
    if (ret != LIGHT_OK) {
        PR_ERR("Light ctrl data auto save error!");
    } else {
        PR_DEBUG("Light ctrl app auto data save OK !");
    }

    ret = tuya_user_software_timer_stop(AUTOSAVE_SW_TIMER);
    if (ret != LIGHT_OK) {
       PR_ERR("stop autosave software timer error!");
    }

    return;
}

/**
 * @brief: light ctrl data auto save proc
 * @param: delay_time_ms -> ctrl data auto save delay time, uint:ms
 * @attention: save data proc will do after the last call actually!   
 * @return: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_light_ctrl_data_auto_save_start(IN UINT_T delay_time_ms)
{
    OPERATE_LIGHT ret = -1;

    CHECK_INIT_OK();

    ret = tuya_user_software_timer_start(AUTOSAVE_SW_TIMER, delay_time_ms, (VOID*)_ligth_ctrl_data_auto_save_timer_callback);
    if (ret != LIGHT_OK) {
        PR_ERR("data save timer start error!");
    }

    return LIGHT_OK;
}

/**
 * @brief: calculate def bright temper and color, according to param
 * @param[in]: color -> default color
 * @param[in]: bright -> default bright
 * @param[in]: temper -> default temperature
 * @param[out]: bright_data_t *ctrl_result -> calc result
 * @return: none
 */
STATIC VOID _light_ctrl_def_color_bright_calculate(IN CTRL_DEF_COLOR_E color, IN USHORT_T bright, IN USHORT_T temper, OUT bright_data_t *ctrl_result)
{
    IN USHORT_T bright_temp = 0;

    if ((color >= DEF_COLOR_R) && (color <= DEF_COLOR_RGB)) {
        bright_temp = _light_ctrl_data_calculate_hsv_limit(bright);
    }

    switch (color) {
        case DEF_COLOR_C:
        case DEF_COLOR_W:
            _light_ctrl_data_calculate_cw(bright, temper, ctrl_result);
            break;

        case DEF_COLOR_R:
            ctrl_result ->red = _light_ctrl_data_calculate_gamma(DEF_COLOR_R, bright_temp);
            break;

        case DEF_COLOR_G:
            ctrl_result ->green = _light_ctrl_data_calculate_gamma(DEF_COLOR_G, bright_temp);
            break;

        case DEF_COLOR_B:
            ctrl_result ->blue = _light_ctrl_data_calculate_gamma(DEF_COLOR_B, bright_temp);
            break;

        case DEF_COLOR_RGB:
            ctrl_result ->red = _light_ctrl_data_calculate_gamma(DEF_COLOR_R, bright_temp);
            ctrl_result ->green = _light_ctrl_data_calculate_gamma(DEF_COLOR_G, bright_temp);
            ctrl_result ->blue = _light_ctrl_data_calculate_gamma(DEF_COLOR_B, bright_temp);
            break;

        case DEF_COLOR_MAX:
        default:
            break;
    }

    return;
}

/**
 * @brief: 灯的常规状态(白光常亮)控制接口
 * @param: none
 * @return: none
 */
STATIC VOID _light_ctrl_normal_display(VOID)
{
    OPERATE_LIGHT ret = -1;
    bright_data_t ctrl_data;

    PR_DEBUG("normal display....");
    
    memset(&ctrl_data, 0, SIZEOF(bright_data_t));

    //stop all shade process!
    tuya_light_shade_ctrl_disable();       

    _light_ctrl_def_color_bright_calculate(sg_light_cfg_data.def_color, sg_light_cfg_data.def_bright, sg_light_cfg_data.def_temper, &ctrl_data);

    PR_DEBUG("normal display %d %d %d %d %d", ctrl_data.red, ctrl_data.green, ctrl_data.blue, ctrl_data.white, ctrl_data.warm);

    ret = tuya_light_set_rgbcw(ctrl_data.red, ctrl_data.green, ctrl_data.blue, ctrl_data.white, ctrl_data.warm);
    if (ret != LIGHT_OK){
        PR_ERR("normal display set RGBCW error!");
    }

    return;
}

/**
 * @brief: 配网闪烁程序
 * @param: none
 * @return: none
 */
STATIC VOID _light_ctrl_blink_display(VOID)
{
    OPERATE_LIGHT ret = -1;
    STATIC UCHAR_T s_cnt = 0;       //first blink off
    bright_data_t ctrl_data;
    USHORT_T bright_temp = 0;

    memset(&ctrl_data, 0, SIZEOF(bright_data_t));

    //stop all shade process!
    tuya_light_shade_ctrl_disable();       

    if ((sg_light_cfg_data.net_color >= DEF_COLOR_R) && (sg_light_cfg_data.net_color <= DEF_COLOR_RGB)) {
        bright_temp = _light_ctrl_data_calculate_hsv_limit(sg_light_cfg_data.net_bright);
    }

    switch (sg_light_cfg_data.net_color) {
        case DEF_COLOR_C:
        case DEF_COLOR_W:
            _light_ctrl_data_calculate_cw(sg_light_cfg_data.net_bright, sg_light_cfg_data.net_temper, &ctrl_data);

            if (BRIGHT_MODE_CW == sg_light_cfg_data.bright_mode) {
                ctrl_data.white = (s_cnt % 2) ? ctrl_data.white : 0;
                ctrl_data.warm = (s_cnt % 2) ? ctrl_data.warm : 0;
            } else if (BRIGHT_MODE_CCT == sg_light_cfg_data.bright_mode) {
                ctrl_data.white = (s_cnt % 2) ? ctrl_data.white : 0;
                ctrl_data.warm = ctrl_data.warm ;
            }
            break;

        case DEF_COLOR_R:
            ctrl_data.red = (s_cnt % 2) ? (_light_ctrl_data_calculate_gamma(DEF_COLOR_R, bright_temp)) : 0;
            break;

        case DEF_COLOR_G:
            ctrl_data.green = (s_cnt % 2) ? (_light_ctrl_data_calculate_gamma(DEF_COLOR_G, bright_temp)) : 0;
            break;

        case DEF_COLOR_B:
            ctrl_data.blue = (s_cnt % 2) ? (_light_ctrl_data_calculate_gamma(DEF_COLOR_B, bright_temp)) : 0;
            break;

        case DEF_COLOR_RGB:
            ctrl_data.red = (s_cnt % 2) ? (_light_ctrl_data_calculate_gamma(DEF_COLOR_R, bright_temp)) : 0;
            ctrl_data.green = (s_cnt % 2) ? (_light_ctrl_data_calculate_gamma(DEF_COLOR_G, bright_temp)) : 0;
            ctrl_data.blue = (s_cnt % 2) ? (_light_ctrl_data_calculate_gamma(DEF_COLOR_B, bright_temp)) : 0;
            break;

        case DEF_COLOR_MAX:
        default:
            break;
    }

    ret = tuya_light_set_rgbcw(ctrl_data.red, ctrl_data.green, ctrl_data.blue, ctrl_data.white, ctrl_data.warm);
    if (ret != LIGHT_OK) {
        PR_ERR("blink set RGBCW error!");
    }

    s_cnt++;

    return;
}


// Light blink cycletime
STATIC UINT_T sg_blink_time_ms = 0;

/**
 * @brief: 闪烁定时回调函数
 * @param: none
 * @return: none
 */
STATIC VOID _light_ctrl_blink_timer_callback(VOID)
{
    OPERATE_LIGHT ret = -1;

    _light_ctrl_blink_display();

    ret = tuya_user_software_timer_start(BLINK_SW_TIMER, sg_blink_time_ms, (VOID*)_light_ctrl_blink_timer_callback);
    if (ret != LIGHT_OK) {
        PR_ERR("blink timer start error!");
    }

    return;
}

/**
 * @brief: 开启闪烁功能的接口
 * @param[in]: blink_time_ms 闪烁周期, 单位:ms
 * @attention: blink display will as the parm
 *             -- net_color, net_bright, usNetTempr in configuration.
 * @return: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_light_ctrl_blink_start(IN UINT_T blink_time_ms)
{
    OPERATE_LIGHT ret = -1;

    CHECK_INIT_OK();

    sg_blink_time_ms = blink_time_ms;

    ret = tuya_user_software_timer_start(BLINK_SW_TIMER, blink_time_ms, (VOID*)_light_ctrl_blink_timer_callback);
    if (ret != LIGHT_OK) {
        PR_ERR("blink timer start error!");
    }

    return LIGHT_OK;
}

/**
 * @brief: 停止闪烁
 * @param: none
 * @attention: blink stop will directly go to normal status display
 *              normal status will bright as default bright parm
 *              -- DefColor, def_bright, def_temper in configuration.
 * @return: none
 */
OPERATE_LIGHT tuya_light_ctrl_blink_stop(VOID)
{
    OPERATE_LIGHT ret = -1;

    CHECK_INIT_OK();

    ret = tuya_user_software_timer_stop(BLINK_SW_TIMER);
    if (ret != LIGHT_OK) {
        PR_ERR("blink timer stop error!");
    }

    _light_ctrl_normal_display();

    return LIGHT_OK;
}

#define BREATH_UNIT_TIME             2  //2s @attention: accurate time is 2100ms
#define BREATH_TIMER_CYCLE_MS        10 //ms

// Light breath cycle cnt
STATIC UINT_T sg_breath_cnt = 0;
STATIC UCHAR_T sg_breath_rate = 0;

typedef BYTE_T BREATH_STEP_E;
#define BREATH_INIT             0
#define BREATH_WAIT_INIT_OK     1
#define BREATH_CHANGE           2
#define BREATH_OK               3

/**
 * @brief: 呼吸灯控制程序 light ctrl breath proc(breath as net config color&bright)
 * @param: none
 * @return: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档	
 */
STATIC OPERATE_RET _light_ctrl_breathing(VOID)
{
   //y= 0.1x*x
   /* CONST SHORT_T breath_parm[] = {  0,   0,   0,   0,   0,   1,   1,   1,   2,  2,
                                       3,   3,   4,   4,   5,   6,   6,   7,   8,  9,
                                      10,  11,  12,  13,  14,  16,  17,  18,  20, 21,
                                      23,  24,  26,  27,  29,  31,  32,  34,  36, 38,
                                      40,  42,  44,  46,  48,  51,  53,  55,  58, 60,
                                      63,  65,  68,  70,  73,  76,  78,  81,  84, 87,
                                      90,  93,  96,  99, 102, 106, 109, 112, 116, 119,
                                      123, 126, 130, 133, 137, 141, 144, 148, 152, 156,
                                      160, 164, 168, 172, 176, 181, 185, 189, 194, 198,
                                      203, 207, 212, 216, 221, 226, 230, 235, 240, 245,
                                      250, 255, 260, 265, 270, 276, 281, 286, 292, 297,
                                      303, 308, 314, 319, 325, 331, 336, 342, 348, 354,
                                      360, 366, 372, 378, 384, 391, 397, 403, 410, 416,
                                      423, 429, 436, 442, 449, 456, 462, 469, 476, 483,
                                      490, 497, 504, 511, 518, 526, 533, 540, 548, 555,
                                      563, 570, 578, 585, 593, 601, 608, 616, 624, 632,
                                      640, 648, 656, 664, 672, 681, 689, 697, 706, 714,
                                      723, 731, 740, 748, 757, 766, 774, 783, 792, 801,
                                      810, 819, 828, 837, 846, 856, 865, 874, 884, 893,
                                      903, 912, 922, 931, 941, 951, 960, 970, 980, 990,
                                     1000};
    */

    //Gaussian distribution
    CONST SHORT_T breath_parm[]  = {1000, 999, 997, 994, 989, 982, 975, 966, 956, 944,
                                    932,  918, 903, 887, 870, 853, 834, 815, 795, 774,
                                    753,  731, 709, 687, 665, 642, 619, 596, 573, 551,
                                    528,  506, 484, 462, 440, 419, 399, 379, 359, 340,
                                    322,  304, 286, 269, 253, 238, 223, 209, 195, 182,
                                    170,  158, 147, 136, 126, 117, 108, 100,  92,  85,
                                     78,   71,  65,  60,  55,  50,  46,  41,  38,  34,
                                     31,   28,  25,  23,  21,  19,  17,  15,  13,  12,
                                     11,   10,   8,   8,   7,   6,   5,   5,   4,   4,
                                      3,    3,   2,   2,   2,   2,   1,   1,   1,   1, 1};

    OPERATE_RET ret = -1;
    STATIC UCHAR_T s_breathing_step = 0;
    STATIC USHORT_T s_stand_cnt = 0;
    STATIC USHORT_T s_stand_index = 0;
    STATIC UCHAR_T s_stand_again_cnt = 0;
    bright_data_t ctrl_data;
    //USHORT_T max = 0;
    STATIC USHORT_T s_min = 0;
    USHORT_T bright_temp = 0;
    USHORT_T temp_index = 0;

    ret = tuya_user_software_timer_start(BREATH_SW_TIMER, sg_breath_rate * BREATH_TIMER_CYCLE_MS, (VOID*)_light_ctrl_breathing);
    if (ret != OPRT_OK) {
        PR_ERR("breath timer restart error!");
        return LIGHT_COM_ERROR;
    }

    if (BREATH_INIT == s_breathing_step) {  
        //change to mid brightness firstly

        //stop all shade process!
        tuya_light_shade_ctrl_disable();       

        if ((sg_light_cfg_data.net_color >= DEF_COLOR_R) && (sg_light_cfg_data.net_color <= DEF_COLOR_RGB)) {
            s_min = CTRL_RGB_BRIGHT_VALUE_MAX * ((FLOAT_T) sg_light_cfg_data.limit_rgb_min / 100.0);
        } else {
            s_min = CTRL_CW_BRIGHT_VALUE_MAX * ((FLOAT_T) sg_light_cfg_data.limit_cw_min / 100.0);
        }

        memset(&sg_light_ctrl_handle.current_val, 0, SIZEOF(bright_data_t));

        _light_ctrl_def_color_bright_calculate(sg_light_cfg_data.net_color, s_min, sg_light_cfg_data.net_temper, &sg_light_ctrl_handle.current_val);

        ret = tuya_light_set_rgbcw(sg_light_ctrl_handle.current_val.red, sg_light_ctrl_handle.current_val.green, sg_light_ctrl_handle.current_val.blue, 
                                   sg_light_ctrl_handle.current_val.white, sg_light_ctrl_handle.current_val.warm);
        if (ret != LIGHT_OK) {
            PR_ERR("breath set RGBCW error!");
        }

        s_breathing_step = BREATH_WAIT_INIT_OK;
    }

    if (BREATH_WAIT_INIT_OK == s_breathing_step) {

       /* s_stand_index ++;
        if(s_stand_index >= (SHADE_CHANG_MAX_TIME/BREATH_TIMER_CYCLE_MS)) {   // wait change to breath start brightness firstly
            s_stand_index = 0;
            s_breathing_step = 2;
        }
        */
        s_stand_index = 0;
        s_breathing_step = BREATH_CHANGE;
    } else if (BREATH_CHANGE == s_breathing_step) {

        memset(&ctrl_data, 0, SIZEOF(bright_data_t));

        if (s_stand_again_cnt < 1) {
            temp_index = ((SIZEOF(breath_parm)/SIZEOF(USHORT_T)) - s_stand_index - 1);  
        } else {
            temp_index = s_stand_index;
        }

        bright_temp = (USHORT_T) (breath_parm[temp_index] /1000.0 * (sg_light_cfg_data.net_bright - s_min)) + s_min ;
        _light_ctrl_def_color_bright_calculate(sg_light_cfg_data.net_color, bright_temp, sg_light_cfg_data.net_temper, &ctrl_data);

        s_stand_index ++;

        if (s_stand_index >= (SIZEOF(breath_parm) / SIZEOF(USHORT_T))) {
            s_stand_index = 0;
            s_stand_again_cnt++;

            if (s_stand_again_cnt >= 2) {
                s_breathing_step = BREATH_OK;
                s_stand_again_cnt = 0;
            } else {
                 s_breathing_step = BREATH_CHANGE;
            }
        }

         /* PR_NOTICE("target %d %d %d %d %d", ctrl_data.red, \
                ctrl_data.green,\
                ctrl_data.blue,\
                ctrl_data.white,\
                ctrl_data.warm);
         */


        ret = tuya_light_set_rgbcw(ctrl_data.red, ctrl_data.green, ctrl_data.blue, ctrl_data.white, ctrl_data.warm);
        if (ret != LIGHT_OK) {
            PR_ERR("breath set RGBCW error!");
        }

    }else if (s_breathing_step == BREATH_OK) {
        s_stand_index ++;

        if (s_stand_index < 2) {  
            //wait 2*BREATH_TIMER_CYCLE_MS (ms)
            ;
        } else {
            s_stand_index = 1;
            s_stand_cnt++;

            if (s_stand_cnt >= sg_breath_cnt) { 
                // breath unit cnt ok
                //@attention: if set alway breath, the sg_breath_cnt will set 0xFFFFFFFF or (0xFFFFFFFF / 3) (bigger than 65535)

                s_breathing_step = BREATH_INIT;
                s_stand_cnt = 0;
                s_stand_index = 0;

                memset(&sg_light_ctrl_handle.target_val, 0, SIZEOF(bright_data_t));

                _light_ctrl_def_color_bright_calculate(sg_light_cfg_data.net_color, sg_light_cfg_data.def_bright, sg_light_cfg_data.net_temper, &sg_light_ctrl_handle.target_val);

                _light_ctrl_shade_step_gain_set(SHADE_STEP_GAIN_DEFAULT);

                _light_ctrl_shade_start(LIGHT_SHADE_CYCLE);

                PR_DEBUG("Breath ok!!! stop!!!!!");

                ret = tuya_user_software_timer_stop(BREATH_SW_TIMER);
                if (ret != LIGHT_OK) {
                    PR_ERR("blink timer stop error!");
                }
                return LIGHT_OK;
            } else {
                s_breathing_step = BREATH_CHANGE;
            }
        }
    }
    return LIGHT_OK;
}

/**
 * @brief: 启动呼吸灯功能
 * @param[in]: breath_rate 
 * @param[in]: breath_cnt  
 * @attention: breath display will as the parm
 *             -- NetColor, net_bright, net_temper in configuration.
 * @return: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档	
 */
OPERATE_RET tuya_light_ctrl_breathing_start(IN UCHAR_T breath_rate, IN UINT_T breath_cnt)
{
    OPERATE_RET ret = -1;

    CHECK_INIT_OK();

    sg_breath_rate = (breath_rate ==  PAIRING_NORMAL_BLINK_FREQ) ? 1 : 3;
    sg_breath_cnt = breath_cnt / sg_breath_rate;

    PR_DEBUG("breath cnt %d",sg_breath_cnt);

    ret = tuya_user_software_timer_start(BREATH_SW_TIMER, BREATH_TIMER_CYCLE_MS, (VOID*)_light_ctrl_breathing);
    if (ret != LIGHT_OK) {
        PR_ERR("breath timer stop error!");
        return LIGHT_COM_ERROR;
    }

    return LIGHT_OK;
}

/**
 * @brief: 停止呼吸灯功能
 * @param: none
 * @attention: Breathing stop will directly go to normal status display
 *             normal status will bright as default bright parm
 *              -- DefColor, def_bright, def_temper in configuration.
 * @return: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
OPERATE_RET tuya_light_ctrl_breathing_stop(VOID)
{
    OPERATE_RET ret = -1;

    CHECK_INIT_OK();

    ret = tuya_user_software_timer_stop(BREATH_SW_TIMER);
    if (ret != LIGHT_OK) {
        PR_ERR("breath timer stop error!");
        return LIGHT_COM_ERROR;
    }

    _light_ctrl_normal_display();
    
    return LIGHT_OK;
}

/**
 * @brief: 根据配网提醒模式自动启动呼吸灯或者是闪烁灯
 * @param[in]: blink_time_ms 闪烁周期, 单位:ms 
 * @attention: blink or breath display will as the parm
 *             -- NetColor, net_bright, usNetTempr in configuration.
 * @attention: breath time is don't set by BlinkTimeMs, but set by LightCfgData.remind_time
 * @return: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档	
 */
OPERATE_RET tuya_light_ctrl_auto_blink_start(IN UINT_T blink_time_ms)
{
    OPERATE_RET ret = -1;

    CHECK_INIT_OK();

    if (!sg_light_cfg_data.remind_mode) { 
        //blink when pairing
        ret = tuya_light_ctrl_blink_start(blink_time_ms);
        if (ret != OPRT_OK) {
            PR_ERR("start blink timer error!");
        }

    } else {    
        //breath when pairing

        if (sg_light_cfg_data.remind_time >= BREATH_ALWAY_ON_TIME) {
            ret = tuya_light_ctrl_breathing_start(blink_time_ms, BREATH_ALWAY_ON_CNT);
        } else {
            ret = tuya_light_ctrl_breathing_start(blink_time_ms, sg_light_cfg_data.remind_time / BREATH_UNIT_TIME);
        }

        if (ret != OPRT_OK) {
            PR_ERR("start breath timer error!");
        }
    }

    return ret;
}


/**
 * @brief: 根据配网提醒模式自动停止呼吸灯或者是闪烁灯
 * @param: none
 * @return: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
OPERATE_RET tuya_light_ctrl_auto_blink_stop(VOID)
{
    OPERATE_RET ret = -1;

    CHECK_INIT_OK();

    if (!sg_light_cfg_data.remind_mode) {    
        //blink
        ret = tuya_light_ctrl_blink_stop();
        if (ret != OPRT_OK) {
            PR_ERR("blink timer stop error!");
            return OPRT_COM_ERROR;
        }
    } else {
        //breath
        ret = tuya_light_ctrl_breathing_stop();
        if (ret != OPRT_OK) {
            PR_ERR("breath timer stop error!");
            return OPRT_COM_ERROR;
        }
    }

    return OPRT_OK;
}

#if (LIGHT_CFG_SUPPORT_LOWPOWER == 1)

// Light low power active flag
STATIC BOOL_T sg_low_power_active = FALSE;

/**
 * @brief: get low power mode cfg
 * @param: none
 * @return: title20
 */
UCHAR_T tuya_light_ctrl_get_low_power_mode(VOID)
{
    return (UCHAR_T)sg_light_cfg_data.title20;
}

/*
 * @brief: 退出低功耗模式
 * @param: none
 * @attention: this function need to implement in system
 *              if system need to realize lowpower mode,
 *              system need to inmplement the flow function.
 *              exist lowpower function should turn on hardware time and
 *              disable system special lowpower API.
 * @return: none
 */
WEAK VOID tuya_light_sys_ctrl_low_power_exist(VOID)
{
    ;
}

/*
* @brief: 进入低功耗模式
* @param: none
* @attention: this function need to implement in system
*              if system need to realize lowpower mode,
*              system need to inmplement the flow function.
*              enter lowpower function should shut down hardware time and
*              enable system special lowpower API.
* @return: none
*/
WEAK VOID tuya_light_sys_ctrl_low_power_enter(VOID)
{
    ;
}

/**
 * @brief: 进入低功耗模式的定时回调函数
 * @param: none
 * @return: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档	
 */
STATIC OPERATE_LIGHT _light_ctrl_low_power_enter_callback(VOID)
{
    OPERATE_LIGHT ret = -1;

    if (BRIGHT_MODE_CCT == sg_light_cfg_data.bright_mode) {   
        //CCT drive mode make sure don't change warm
        ret = tuya_light_set_rgbcw(0,0,0,0,0);
        if (ret != LIGHT_OK) {
            PR_ERR("CCT shut down error!");
        }
    }

    //system special process, tickless processs for example
    tuya_light_sys_ctrl_low_power_enter();       

    PR_DEBUG("enter lowpower!");

    //attention: as base code use counting semaphore to paired
    sg_low_power_active = TRUE;             

    //stop timer dominantly 
    ret = tuya_user_software_timer_stop(LOWPOWER_SW_TIMER);  
    if (ret != LIGHT_OK) {
        PR_ERR("lowpower timer stop error!");
        return LIGHT_COM_ERROR;
    }

    return LIGHT_OK;
}

/**
 * @brief: 低功耗处理程序接口
 * @param[in]: UCHAR_T on_off
 * @return: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
STATIC OPERATE_LIGHT _light_ctrl_low_power_process(IN UCHAR_T on_off)
{
    OPERATE_LIGHT ret = -1;

    CHECK_INIT_OK();

    if (on_off == TRUE) {       
        //开的状态

        ret = tuya_user_software_timer_stop(LOWPOWER_SW_TIMER);
        if (ret != LIGHT_OK) {
            PR_ERR("lowpower timer stop error!");
            return LIGHT_COM_ERROR;
        }

        if (sg_low_power_active) {
            tuya_light_sys_ctrl_low_power_exist();

            PR_DEBUG("exist lowpower!");

            sg_low_power_active = FALSE;
        }
    } else {                  
        //关的状态 

        PR_DEBUG("start lowpower timer!");

        ret = tuya_user_software_timer_start(LOWPOWER_SW_TIMER, LOWPOWER_TIMER_CYCLE_MS, (VOID*)_light_ctrl_low_power_enter_callback);
        if (ret != LIGHT_OK) {
            PR_ERR("lowpower timer start error!");
            return LIGHT_COM_ERROR;
        }
    }

    return LIGHT_OK;
}
#endif



