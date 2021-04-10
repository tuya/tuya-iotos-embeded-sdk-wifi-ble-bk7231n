/*
 * @Author: wls
 * @email: wuls@tuya.com
 * @LastEditors: wls
 * @file name: light_control.h
 * @Description: light control include file
 * @Copyright: HANGZHOU TUYA INFORMATION TECHNOLOGY CO.,LTD
 * @Company: http://www.tuya.com
 * @Date: 2019-04-26 13:55:40
 * @LastEditTime: 2020-03-06 12:35:19
 */

#ifndef __LIHGT_CONTROL_H__
#define __LIHGT_CONTROL_H__


#include "light_types.h"
#include "light_set_color.h"
#include "light_cfg.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

// hardware timer cycle (unit:us)
#define HW_TIMER_CYCLE_US           4000

// shade change max time(the worst situation),uint:ms @attention: don't change arbitrary
#define SHADE_CHANG_MAX_TIME        800

// scene head(scene Num) length
#define SCNE_HRAD_LENGTH            2
// scene unit length
#define SCENE_UNIT_LENGTH           26
// scene max unit number
#define SCENE_MAX_UNIT              8
// scene data min length, formu -> 2+ 26 = 28
#define SCENE_MIN_LENGTH            (SCNE_HRAD_LENGTH + SCENE_UNIT_LENGTH)
// scene data max length, formu -> 2 + 26*8  = 210
#define SCENE_MAX_LENGTH            (SCNE_HRAD_LENGTH + SCENE_UNIT_LENGTH * SCENE_MAX_UNIT)

// scene C default scene ctrl data
#define SCENE_DATA_DEFAULT_C        "000e0d0000000000000000c803e8"
// scene CW default scene ctrl data
#define SCENE_DATA_DEFAULT_CW       "000e0d0000000000000000c80000"
// scene RGB/RGBC/RGBCW default scene ctrl data
#define SCENE_DATA_DEFAULT_RGB      "000e0d00002e03e802cc00000000"

#if (LIGHT_CFG_SUPPORT_LOWPOWER == 1)
#define LOWPOWER_TIMER_CYCLE_MS     3000 //ms
#endif

#define PAIRING_NORMAL_BLINK_FREQ   250
#define PAIRING_SLOW_BLINK_FREQ     1500
#define BREATH_ALWAY_ON_TIME        602         //@attention: breath alway on config time
#define BREATH_ALWAY_ON_CNT         0xFFFFFFFF   //@attention: this value to avoid breath stop!(bigger than 65535)


/**
 * @brief Light way type enum
 *          1~5 ways
 */
typedef BYTE_T CTRL_LIGHT_WAY_E;
#define LIGHT_C         1
#define LIGHT_CW        2
#define LIGHT_RGB       3
#define LIGHT_RGBC      4
#define LIGHT_RGBCW     5
#define LIGHT_MAX       6


typedef BYTE_T CTRL_DEF_COLOR_E;
#define DEF_COLOR_C         0
#define DEF_COLOR_W         1
#define DEF_COLOR_R         2
#define DEF_COLOR_G         3
#define DEF_COLOR_B         4
#define DEF_COLOR_RGB       5
#define DEF_COLOR_MAX       6


/**
 * @brief Light control switch change mode enum
 *          SWITCH_GRADUAL -> turn on/off gradually
 *          SWITCH_DIRECT  -> turn on/off directly
 */
typedef BYTE_T CTRL_SWITCH_MODE_E;
#define SWITCH_GRADUAL      0
#define SWITCH_DIRECT       1
#define SWITCH_MAX          2


/**
 * @brief Light control drive mode enum
 *          BRIGHT_MODE_CW  -> CW drive by pwm totally
 *          BRIGHT_MODE_CCT -> C value is bright setting essentially , w is the scale of C&W .
 *                          the light will send warm, when w is set by zero.
 */
typedef BYTE_T CTRL_BRIGHT_MODE_E;
#define BRIGHT_MODE_CW      0
#define BRIGHT_MODE_CCT     1
#define BRIGHT_MODE_MAX     2



/**
 * @brief Light control scene change mode enum
 *          SCENE_STATIC    -> scene hold on no change
 *          SCENE_JUMP      -> scene change by directly
 *          SCENE_SHADOW    -> scene change by gradually
 */
typedef BYTE_T CTRL_SCENE_MODE_E;
#define SCENE_STATIC        0
#define SCENE_JUMP          1
#define SCENE_SHADE         2
#define SCENE_MAX           3



typedef BYTE_T CTRL_RESET_MODE_E;
#define RESET_MOD_CNT       0       //this mode can't choose in bulb!

/**
 * @brief Light control need configuration sturct
 * the configuration need to set firstly ,when use the control frame!
 */
typedef struct {
    CTRL_LIGHT_WAY_E    light_way;      //light type -- 1/2/3/4/5 way light 
    CTRL_BRIGHT_MODE_E  bright_mode;    // CCT&CW drive mode -- 0:CW, 1:CCT 
    CTRL_SWITCH_MODE_E  switch_mode;    // turn on/off mode -- 0:gradually, 1:directly 
    BOOL_T              memory;         // ifnot save app control data flag -- 0: not save, 1:save 

#if (LIGHT_CFG_SUPPORT_LOWPOWER == 1)
    BOOL_T              title20;        //ifnot support T20 lowpower mode 
#endif

    CTRL_DEF_COLOR_E    def_color;      // default color 
    USHORT_T            def_bright;     // default bright 
    USHORT_T            def_temper;     // default temper 
    UCHAR_T             power_gain;     // power gain(amplification) 
    UCHAR_T             limit_cw_max;   // CW limit Max value 
    UCHAR_T             limit_cw_min;   // CW limit Min value 
    UCHAR_T             limit_rgb_max;  // RGB limit Max value 
    UCHAR_T             limit_rgb_min;  // RGB limit Min value 

    UCHAR_T             connect_mode;   // connect cfg mode 
    UCHAR_T             reset_cnt;      // re distibute cnt 
    CTRL_DEF_COLOR_E    net_color;      // the color when  
    USHORT_T            net_bright;     // blink bright in connect proc 
    USHORT_T            net_temper;     // blink temper in connect proc 
    UCHAR_T             remind_mode;    // remind mode when pairing 
    USHORT_T            remind_time;    // pairing blink time,unit:sec 
    UCHAR_T             pair_time;      // pairing blink time,unit:min 
    driver_config_t     drive_cfg;      // light drive hardware cfg 
}light_ctrl_cfg_t;


/**
 * @brief Light control data structure
 * storage the light control data(normal issued by app)
 */
typedef BYTE_T LIGHT_MODE_E;
#define WHITE_MODE      0
#define COLOR_MODE      1
#define SCENE_MODE      2
#define MUSIC_MODE      3
#define MODE_MAX        4


/**
 * @brief software timer use id enum
 */
typedef BYTE_T SW_TIMER_ID_E;
#define CLEAR_RESET_CNT_SW_TIMER        0
#define BLINK_SW_TIMER                  1
#define SCENE_SW_TIMER                  2
#define AUTOSAVE_SW_TIMER               3
#define BREATH_SW_TIMER                 4
#define COUNTDOWN_SW_TIMER              5
#define LOWPOWER_SW_TIMER               6
#define SCENE_AUTO_RESTART_TIMER        7
#define CCT_DELAY_SHUT_DOWN_TIMER       8
#define CCT_DELAY_RESET_TIMER           9
#if 1//added by peter for IR light
#define IR_JUMP_TIMER                   10
#endif
/*********advance function************/
#define RHYTHM_QUERY_TIMER              11
#define RHYTHM_AUTO_SAVE_TIMER          12
#define PHYTHM_DIMMING_TIMER            13
#define SLEEP_QUERY_TIMER               14
#define SLEEP_AUTO_SAVE_TIMER           15
#define SLEEP_DIMMING_TIMER             16
#define WAKE_QUERY_TIMER                17
#define WAKE_AUTO_SAVE_TIMER            18
#define WAKE_DIMMING_TIMER              19
#define SW_TIMER_MAX                    20


/**
 * @brief Light control color(RGB) data structure
 * storage the light control color(RGB) data(normal issued by app)
 */
typedef struct {
    USHORT_T red;  
    USHORT_T green;
    USHORT_T blue;
}color_rgb_t;


/**
 * @brief Light control color control original data structure
 */
typedef struct {
    USHORT_T hue;     //hue, range:0 ~ 360 
    USHORT_T sat;     //saturation, range:0 ~ 1000 
    USHORT_T value;   //value, range:0 ~ 1000 
    UCHAR_T  color_str[13];
}color_origin_t;

/**
 * @brief Light control real time control change mode enum
 */
typedef BYTE_T REALTIME_CHANGE_E;
#define REALTIME_CHANGE_JUMP 	 0
#define REALTIME_CHANGE_SHADE	 1


/**
 * @brief Light control data structure
 * storage the light control data(normal issued by app)
 */
typedef struct {
    BOOL_T switch_status;             
    LIGHT_MODE_E mode;
    USHORT_T bright;
    USHORT_T temper;
    color_rgb_t color;
    color_origin_t color_origin;
    CHAR_T scene[SCENE_MAX_LENGTH + 1];
    BOOL_T scene_first_set;
    UINT_T countdown;
    CHAR_T realtime_data[22];
    UCHAR_T realtime_flag;
}light_ctrl_data_t;


/**
 * @brief Light control extend data structure
 */
typedef struct {
    BOOL_T switch_status;       
    LIGHT_MODE_E mode;
    USHORT_T bright;
    USHORT_T temper;
    color_rgb_t color;
    color_origin_t color_origin;
}light_ctrl_extend_data_t;


typedef struct {
    CTRL_SCENE_MODE_E change_mode;
    UINT_T speed;
    UINT_T times;    //unit:ms 
}light_scene_ctrl_t;


/**
 * @brief Bright 5ways value structure
 * Used in light gradually change calculation process
 */
typedef struct {
    USHORT_T red;
    USHORT_T green;
    USHORT_T blue;
    USHORT_T white;
    USHORT_T warm;
}bright_data_t;


/**
 * @brief Light gradually change structure
 * Used in light gradually change calculation process
 */
typedef struct {
    bright_data_t target_val;
    bright_data_t current_val;
    BOOL_T first_change;    //first change flag, need to calculate change step 
    UCHAR_T scene_unit;
    UCHAR_T unit_cnt;
    BOOL_T scene_set_first_flag;
    BOOL_T scene_stop_flag;
}light_ctrl_handle_t;


/**
 * @brief Light time structure
 * Used in light advance function about time
 */
typedef struct {
    INT_T tm_sec;     //seconds [0-59] 
    INT_T tm_min;     // minutes [0-59] 
    INT_T tm_hour;    // hours [0-23] 
    INT_T tm_mday;    // day of the month [1-31] 
    INT_T tm_mon;     // month [0-11] 
    INT_T tm_year;    // year. The number of years since 1900 
    INT_T tm_wday;    // day of the week [0-6] 0-Sunday...6-Saturday 
}light_time_t;


typedef BOOL_T (*bLightGetLocalTime)(OUT light_time_t *result);


/**
 * @brief:根据灯的配置数据设置灯的默认控制数据
 * @param: none
 * @return: none
 */
VOID tuya_light_ctrl_data_reset(VOID);

/**
 * @brief: 硬件定时器回调函数
 * @param: none
 * @attention: this function need to implement by system,
 *              decide how to call vLightCtrlShadeGradually function.
 * @return: none
 */
VOID tuya_light_sys_hardware_timer_callback(VOID);

/**
 * @brief: 硬件定时器回调函数, 灯控制适配接口
 * @param: none
 * @attention: tuya_light_sys_hardware_timer_callback() func need to implement by system
 * @return: none
 */
VOID tuya_light_ctrl_hardware_timer_callback(VOID);

/**
 * @brief: 灯控制初始化接口
 * @param[in]: config_data 初始化的数据指针 
 * @attention: this function need apply tuya_light_sys_hardware_reboot_judge();
 * @return: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_light_ctrl_init(IN light_ctrl_cfg_t *config_data);

/**
 * @brief: 灯渐变控制接口
 * @param: none
 * @attention: this func need to called by period
 * @return: none
 */
VOID tuya_light_ctrl_shade_gradually(VOID);

/**
 * @brief: reponse switch property process,
 *          this need to implement by system.
 * @param[out]: BOOL_T bONOFF -> switch status, TRUE is ON}
 * @return: none
 */
VOID tuya_light_ctrl_data_switch_respone(OUT BOOL_T on_off);

/**
 * @brief:  设置开关状态, adapte control data issued by system
 *          to control data format.
 * @param[in]: on_off 开关值，TURE表示打开
 * @return: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_light_ctrl_data_switch_set(IN BOOL_T on_off);

/**
 * @brief: reponse mode property process,
 *          this need to implement by system.
 * @param[out]: LIGHT_MODE_E mode
 * @return: none
 */
VOID tuya_light_ctrl_data_mode_response(OUT LIGHT_MODE_E mode);

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
OPERATE_LIGHT tuya_light_ctrl_data_mode_set(IN LIGHT_MODE_E mode);

/**
 * @brief: reponse bright property process,
 *          this need to implement by system.
 * @param[out]: LIGHT_MODE_E mode
 * @param[out] USHORT_T bright
 * @attention: need reponse mode property,as set bright value, will auto set the mode to WHITE_MODE!
 * @return: none
 */
VOID tuya_light_ctrl_data_bright_response(OUT LIGHT_MODE_E mode, OUT USHORT_T bright);

/**
 * @brief: 设置灯的亮度, adapte control data issued by system
 *          to control data format.
 * @param[in] USHORT_T bright 亮度值
 * @attention: acceptable range:10~1000
 * @attention: set bright value, will auto set the mode to WHITE_MODE !
 * @return: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_light_ctrl_data_bright_set(IN USHORT_T bright);

/**
 * @brief: reponse temperature property process,
 *          this need to implement by system.
 * @param[out]: LIGHT_MODE_E mode
 * @param[out]: USHORT_T temperature
 * @attention: need reponse mode property,as set temperature value, will auto set the mode to WHITE_MODE!
 * @return: none
 */
VOID tuya_light_ctrl_data_temperature_response(OUT LIGHT_MODE_E mode, OUT USHORT_T temperature);

/**
 * @brief: 设置灯色温, adapte control data issued by system
 *          to control data format.
 * @param[in]: USHORT_T temperature 色温
 * @attention: acceptable range:0~1000
 * @return: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_light_ctrl_data_temperature_set(IN USHORT_T temperature);

/**
 * @brief: reponse RGB property process,
 *          this need to implement by system
 * @param[out]: color_origin_t *color_origin
 * @return: none
 */
VOID tuya_light_ctrl_data_rgb_response(OUT color_origin_t *color_origin);

/**
 * @brief: 设置灯的RGB值
 * @param[in]: color 指向RGB数据
 * @param[in]: color_origin  指向原始的颜色数据  
 * @attention: acceptable format is RGB module, R,G,B range：0~1000
 * @return: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_light_ctrl_data_rgb_set(IN color_rgb_t *color, IN color_origin_t *color_origin);

/**
 * @brief: reponse scene property process,
 *          this need to implement by system
 * @param[out]: CHAR_T *scene_data
 * @return: none
 */
VOID tuya_light_ctrl_data_scene_response(OUT CHAR_T *scene_data);

/**
 * @brief: 设置灯的场景数据
 * @param[in]: scene_data 场景数据
 * @attention: SceneData format: please reference to DP protocol
 * @return: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_light_ctrl_data_scene_set(IN CHAR_T *scene_data);

/**
 * @brief: 获取开关状态
 * @param[out]: on_off 获取开关状态的指针 
 * @return: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_light_ctrl_data_switch_get(OUT BOOL_T *on_off);

/**
 * @berief: get light time Count Down data
 * @param[out] countdown  time count down value
 * @return: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
OPERATE_RET tuya_light_ctrl_data_countdown_get(OUT UINT_T *countdown);

/**
 * @brief: 获取灯光模式
 * @param[out] mode 获取灯光模式的指针
 * @return: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_light_ctrl_data_mode_get(OUT LIGHT_MODE_E *mode);

/**
 * @brief: 获取灯的明亮值
 * @param[out]: bright 获取灯明亮值的指针
 * @return: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_light_ctrl_data_bright_get(OUT USHORT_T *bright);

/**
 * @brief: 获取灯的色温值
 * @param[out]: temperature 获取灯色温值的指针 
 * @return: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_light_ctrl_data_temperature_get(OUT USHORT_T *temperature);


/**
 * @brief: 获取灯原始的颜色数据
 * @param[out]: original_color 获取灯原始颜色数据的指针
 * @return: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_light_ctrl_data_origin_get(OUT color_origin_t *original_color);

/**
 * @brief: 获取场景数据
 * @param[out]: scene_data 获取场景数据的指针 
 * @return: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_light_ctrl_data_scene_get(OUT CHAR_T *scene_data);

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
OPERATE_RET tuya_light_ctrl_data_extend_set(IN light_ctrl_extend_data_t *extend_data, IN BOOL_T need_upload, IN BOOL_T active_immed, IN BOOL_T need_save);

/**
 * @berief: Light control proc
 * @param: none
 * @return: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_light_ctrl_process(VOID);

/**
 * @brief: 失能灯的控制
 * @param: none
 * @return: none
 */
VOID tuya_light_ctrl_disable(VOID);

/**
 * @brief: 失能渐变程序
 * @param: none
 * @return: none
 */
VOID tuya_light_shade_ctrl_disable(VOID);

/**
 * @brief: countdown lave time return proc
 * @param[out] remain_time_sec -> left time,unit:sec
 * @attention: this function need to implement in system
 * @return: none
 */
VOID tuya_light_ctrl_data_countdown_response(OUT UINT_T remain_time_sec);

/**
 * @brief: set light countdown value
 * @param[in]  countdown_sec -> unit:second
 * @attention: countdown lave time will return with
 *              calling vLightDataCountDownResponse function every minutes.
 *              switch status will return with calling
 *              tuya_light_ctrl_data_switch_respone function when countdown active.
 * @return: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_light_ctrl_data_countdown_set(IN INT_T countdown_sec);

/**
 * @brief: set light realtime control data
 * @param[in]: music_flag
 * @param[in]: realtime_data
 * @attention: data format: please reference to DP protocol
 * @return: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_light_ctrl_data_realtime_adjust_set(IN BOOL_T music_flag, IN CHAR_T *realtime_data);

 /**
 * @brief: Light realtime ctrl process
 * @param: none
 * @return: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_light_realtime_ctrl_process(VOID);

/**
 * @brief: system reboot as hardware mode judge proc
 * @param: none
 * @attention: this function need to implement by each plantform.
 * @retval: BOOL_T TRUE -> system reboot
 */
extern BOOL_T tuya_light_sys_hardware_reboot_judge(VOID);

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
VOID tuya_light_ctrl_hardware_reboot_process(VOID);

/**
 * @brief: 保存灯的控制数据
 * @param: none
 * @attention: this function directly save ctrl data.
 * @return: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_light_ctrl_data_auto_save(VOID);

/**
 * @brief: system reset proc
 * @param: none
 * @attention: this function need implememt by system,
 *              need to deal with different thing in each plantform.
 * @return: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_light_sys_reset_cnt_over_callback(VOID);

/**
 * @berief: 灯上电次数清零
 * @param: none
 * @attention: this func will call in opLightCtrlResetCntProcess
 *              when no tuya_light_sys_reset_cnt_over_callback finish callback, otherwise
 *               call in system!!!
 * @return: none
 */
VOID tuya_light_ctrl_reset_cnt_clear(VOID);

/**
 * @brief: Light reset to re-distribute proc
 * @param: none
 * @attention: this func will call tuya_light_sys_reset_cnt_over_callback()
 *              tuya_light_sys_reset_cnt_over_callback() need to implement by system
 * @return: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_light_ctrl_reset_cnt_process(VOID);

/**
 * @brief: get connect mode cfg
 * @param: none
 * @return: connect mode
 */
UCHAR_T tuya_light_ctrl_get_connect_mode(VOID);

/**
 * @brief: get connect time cfg
 * @param: none
 * @return: pair_time
 */
UCHAR_T tuya_light_ctrl_get_connect_time(VOID);

/**
 * @brief: get color max limit
 * @param: none
 * @return: limit_rgb_max
 */
UCHAR_T tuya_light_ctrl_get_color_max(VOID);

/**
 * @brief: get color min limit
 * @param: none
 * @return: limit_rgb_min
 */
UCHAR_T tuya_light_ctrl_get_color_min(VOID);

/**
 * @brief: light ctrl data auto save proc
 * @param: delay_time_ms -> ctrl data auto save delay time, uint:ms
 * @attention: save data proc will do after the last call actually!   
 * @return: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_light_ctrl_data_auto_save_start(IN UINT_T delay_time_ms);


/**
 * @brief: 开启闪烁功能的接口
 * @param[in]: blink_time_ms 闪烁周期, 单位:ms
 * @attention: blink display will as the parm
 *             -- net_color, net_bright, usNetTempr in configuration.
 * @return: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_light_ctrl_blink_start(IN UINT_T blink_time_ms);

/**
 * @brief: 停止闪烁
 * @param: none
 * @attention: blink stop will directly go to normal status display
 *              normal status will bright as default bright parm
 *              -- DefColor, def_bright, def_temper in configuration.
 * @return: none
 */
OPERATE_LIGHT tuya_light_ctrl_blink_stop(VOID);

/**
 * @brief: 启动呼吸灯功能
 * @param[in]: breath_rate 
 * @param[in]: breath_cnt  
 * @attention: breath display will as the parm
 *             -- NetColor, net_bright, net_temper in configuration.
 * @return: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档	
 */
OPERATE_RET tuya_light_ctrl_breathing_start(IN UCHAR_T breath_rate, IN UINT_T breath_cnt);

/**
 * @brief: 停止呼吸灯功能
 * @param: none
 * @attention: Breathing stop will directly go to normal status display
 *             normal status will bright as default bright parm
 *              -- DefColor, def_bright, def_temper in configuration.
 * @return: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
OPERATE_RET tuya_light_ctrl_breathing_stop(VOID);

/**
 * @brief: 根据配网提醒模式自动启动呼吸灯或者是闪烁灯
 * @param[in]: blink_time_ms 闪烁周期, 单位:ms
 * @attention: blink or breath display will as the parm
 *             -- NetColor, net_bright, usNetTempr in configuration.
 * @attention: breath time is don't set by BlinkTimeMs, but set by LightCfgData.remind_time
 * @return: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档	
 */
OPERATE_RET tuya_light_ctrl_auto_blink_start(IN UINT_T blink_time_ms);

/**
 * @brief: 根据配网提醒模式自动停止呼吸灯或者是闪烁灯
 * @param: none
 * @return: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
OPERATE_RET tuya_light_ctrl_auto_blink_stop(VOID);


#if (LIGHT_CFG_SUPPORT_LOWPOWER == 1)
/**
 * @brief: get low power mode cfg
 * @param: none
 * @return: title20
 */
UCHAR_T tuya_light_ctrl_get_low_power_mode(VOID);

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
VOID tuya_light_sys_ctrl_low_power_exist(VOID);

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
VOID tuya_light_sys_ctrl_low_power_enter(VOID);

#endif



#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif  /* __LIHGT_CONTROL_H__ */
