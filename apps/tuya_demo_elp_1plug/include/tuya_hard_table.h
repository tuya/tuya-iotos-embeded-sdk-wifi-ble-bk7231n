/*
 * @File name: tuya_hard_tabble.h
 * @Author: caojq
 * @Version: V1.0.0
 * @Date: 2020-10-27 11:39:57
 * @Description: 硬件相关操作，包括硬件结构体初始化、硬件控制等，主要有按键、继电器、指示灯的操作；
 * @Others: 
 * @LastEditTime: 2020-10-28 16:14:19
 * @LastEditors:  
 */
#ifndef __TUYA_HARD_TABLE_H
#define __TUYA_HARD_TABLE_H

#include "tuya_key.h"
#include "tuya_cloud_types.h"
#include "tuya_cloud_wifi_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __TUYA_HARD_TABLE_GLOBAL
#define __TUYA_HARD_TABLE_EXT
#else
#define __TUYA_HARD_TABLE_EXT extern
#endif

/*IO config  effective level*/
typedef enum
{
    IO_INIT_LEVEL,          ///< 初始化电平
    IO_LOW_LEVEL,           ///< 有效电平低电平
    IO_HIGH_LEVEL,          ///< 有效电平高有效
}IO_LEVEL_TYPE_E;

/*IO config info,include io_pin and io_level*/
typedef struct
{
    UCHAR_T io_pin;      //io_pin num
    IO_LEVEL_TYPE_E io_level;      //io_level effective 
}IO_CONFIG_S;

/*dp control moudle*/
typedef struct
{
    BOOL_T is_exist;                        ///<dp_index is exist or not exist
    UINT_T dpid_num;                            ///<device dp_index
}DP_MOUDLE_S;

/*device app send dp，include switch dp and countdown dp*/
typedef struct
{
    DP_MOUDLE_S switch_dp;                       ///< switch control dp
    DP_MOUDLE_S switch_cd_dp;                   ///< switch countdown control dp
    DP_MOUDLE_S app_relay_stat;                 ///< app set relay stat
}DEVICE_DP_S;

/*user set key moudle param*/
typedef struct
{
    IO_CONFIG_S io_config;                  ///<key config param
    UINT_T press_time;                      ///<key press time
    KEY_CALLBACK key_call_back;             ///<key touch call_back
}KEY_MOUDLE_S;

/*set device config param*/
typedef struct
{
    GW_WF_CFG_MTHD_SEL wifi_mode;            ///< tuya sdk definition of wifi work mode
    IO_CONFIG_S wifi_led;                   ///< wifi status display led info
    KEY_MOUDLE_S key_control;                ///< key control relay on or off and press long enter linking netwoork
    IO_CONFIG_S relay;                      ///< relay control io config
    IO_CONFIG_S relay_led;                  ///< relay led display relay on/off status
    DEVICE_DP_S dev_dp;                     ///< app send dp control device
}DEVICE_PARAM_S;

/*led display type*/
typedef enum
{
    PROD_TEST_FAIL,                         ///<prod_test fail,led flash 100ms
    ENTER_PROD_TEST,                        ///<prod_test,led flash 250ms
    EZ_LINKING,                             ///<ez linking,led flash 250ms
    AP_LINKING,                             ///<ap linking,led flash 1500ms
    NORMAL_ON,                              ///<relay control,led is on
    NORMAL_OFF,                             ///<relay control,led is off
}LED_STAT_E;

/*device dpid type*/
typedef enum
{
    DP_NOT_EXIST = 0,                       ///<dpid num is not exist
    SW_TYPE,                            ///<switch dp type
    SW_CD_TYPE,                         ///<switch countdown dp type
    RELAY_ST_TYPE,                      ///<relay stat dp type
}DEV_DP_TYPE_E;

/*channel data upload type*/
typedef enum
{
    SINGLE_TYPE,                        ///<count down sec upload
    DOUBLE_TYPE,                        ///<relay stat and count down sec upload
}CHAN_UPLOAD_TYPE_E;

/**
 * @Function: tuya_hard_init
 * @Description: 硬件初始化
 * @Input: dev_info:设备信息初始化，包括wifi工作模式、硬件I/O引脚和有效电平等
 * @Output: none
 * @Return: none
 * @Others: none
 */
__TUYA_HARD_TABLE_EXT \
VOID tuya_hard_init(DEVICE_PARAM_S *dev_info);

/**
 * @Function: tuya_hard_get_wifi_mode
 * @Description: 获取设置的涂鸦wifi工作模式
 * @Input: none
 * @Output: none
 * @Return: GW_WF_CFG_MTHD_SEL:涂鸦wifi工作模式
 * @Others: none
 */
__TUYA_HARD_TABLE_EXT \
GW_WF_CFG_MTHD_SEL tuya_hard_get_wifi_mode(VOID);

/**
 * @Function: tuya_hard_set_led_state
 * @Description: 指示灯状态显示，主要包括产测、wifi状态、状态指示；
 * @Input: led_type:指示灯类型
 * @Output: none
 * @Return: none
 * @Others: none
 */
__TUYA_HARD_TABLE_EXT \
VOID tuya_hard_set_led_state(LED_STAT_E led_type);

/**
 * @Function: tuya_hard_prod_flag
 * @Description: 产测标识设置
 * @Input: prod_flag:产测标识
 * @Output: none
 * @Return: none
 * @Others: none
 */
__TUYA_HARD_TABLE_EXT \
VOID tuya_hard_prod_flag(BOOL_T prod_flag);

/**
 * @Function: tuya_hard_judge_key_pin
 * @Description: 
 * @Input: key_touch_pin:按键触发回调pin引脚
 * @Output: none
 * @Return: none
 * @Others: none
 */
__TUYA_HARD_TABLE_EXT \
BOOL_T tuya_hard_judge_key_pin(IN UCHAR_T key_touch_pin);

/**
 * @Function: tuya_hard_key_control
 * @Description: 设置短按按键操作控制
 * @Input: none
 * @Output: none
 * @Return: none
 * @Others: 通道状态设置、倒计时关闭、数据上报及存储
 */
__TUYA_HARD_TABLE_EXT \
VOID tuya_hard_key_control(VOID);

/**
 * @Function: tuya_hard_wifi_status
 * @Description: wifi状态对应指示灯显示
 * @Input: cur_stat：当前wifi状态
 * @Output: none
 * @Return: none
 * @Others: none
 */
__TUYA_HARD_TABLE_EXT \
VOID tuya_hard_wifi_status(IN CONST GW_WIFI_NW_STAT_E cur_stat);

/**
 * @Function: tuya_hard_upload_all_data
 * @Description: 数据dp点上报
 * @Input: none
 * @Output: none
 * @Return: OPRT_OK:操作成功；其他：失败返回错误码
 * @Others: none
 */
__TUYA_HARD_TABLE_EXT \
OPERATE_RET tuya_hard_upload_all_data(VOID);

/**
 * @Function: tuya_hard_clear_flash_data
 * @Description: 删除操作过程中flash存储的数据，恢复到出厂设置状态
 * @Input: none
 * @Output: none
 * @Return: none
 * @Others: 删除通道状态和上电状态
 */
__TUYA_HARD_TABLE_EXT \
VOID tuya_hard_clear_flash_data(VOID);

/**
 * @Function: tuya_hard_switch_dpid_num
 * @Description: 获取开关dpid
 * @Input: none
 * @Output: none
 * @Return: UINT_T:dpid点
 * @Others: none
 */
__TUYA_HARD_TABLE_EXT \
UINT_T tuya_hard_switch_dpid_num(VOID);

/**
 * @Function: tuya_hard_channel_upload
 * @Description: 通道数据上报
 * @Input: upload_type:数据上报类型
 * @Output: none
 * @Return: OPRT_OK: success  Other: fail
 * @Others: 通道数据包括继电器状态和倒计时状态；
 */
 __TUYA_HARD_TABLE_EXT \
OPERATE_RET tuya_hard_channel_upload(CHAN_UPLOAD_TYPE_E upload_type);

/**
 * @Function: tuya_hard_get_channel_stat
 * @Description: 获取通道状态
 * @Input: none
 * @Output: none
 * @Return: BOOL_T:通道状态
 * @Others: none
 */
 __TUYA_HARD_TABLE_EXT \
BOOL_T tuya_hard_get_channel_stat(VOID);

/**
 * @Function: tuya_hard_dpid_type
 * @Description: 根据app下发dp点判断类型
 * @Input: app_send_dpid:app send dpid num
 * @Output: none
 * @Return: DEV_DP_TYPE_E:dpid type
 * @Others: none
 */
 __TUYA_HARD_TABLE_EXT \
DEV_DP_TYPE_E tuya_hard_dpid_type(UINT_T app_send_dpid);

/**
 * @Function: tuya_hard_channel_control
 * @Description: app下发状态对通道状态设置
 * @Input: app_send_switch_stat：app下发通道状态
 * @Output: none
 * @Return: none
 * @Others: none
 */
 __TUYA_HARD_TABLE_EXT \
VOID tuya_hard_channel_control(BOOL_T app_send_switch_stat);

/**
 * @Function: tuya_hard_channel_control
 * @Description: app下发状态对通道状态设置
 * @Input: app_send_switch_stat：app下发通道状态
 * @Output: none
 * @Return: none
 * @Others: none
 */
 __TUYA_HARD_TABLE_EXT \
VOID tuya_hard_channel_control(BOOL_T app_send_switch_stat);

/**
 * @Function: tuya_hard_clear_flash_data
 * @Description: 删除操作过程中flash存储的数据，恢复到出厂设置状态
 * @Input: none
 * @Output: none
 * @Return: none
 * @Others: none
 */
 __TUYA_HARD_TABLE_EXT \
VOID tuya_hard_relay_power_stat(UINT_T app_send_relay_stat);

/**
 * @Function: tuya_hard_channel_cd_control
 * @Description: app下发通道倒计时设置
 * @Input: none
 * @Output: none
 * @Return: none
 * @Others: 
 */
 __TUYA_HARD_TABLE_EXT \
VOID tuya_hard_channel_save(VOID);

/**
 * @Function: tuya_hard_channel_cd_control
 * @Description: app下发通道倒计时设置
 * @Input: app_send_cd_sec:app下发倒计时时间
 * @Output: none
 * @Return: none
 * @Others: none
 */
 __TUYA_HARD_TABLE_EXT \
VOID tuya_hard_channel_cd_control(UINT_T app_send_cd_sec);

/**
 * @Function: tuya_hard_soft_timer_init
 * @Description: 设备软件定时器初始化
 * @Input: none
 * @Output: none
 * @Return: OPRT_OK: success  Other: fail
 * @Others: none
 */
 __TUYA_HARD_TABLE_EXT \
OPERATE_RET tuya_hard_soft_timer_init(VOID);

/**
 * @Function: tuya_hard_dpid_type
 * @Description: 根据app下发dp点判断类型
 * @Input: app_send_dpid:app send dpid num
 * @Output: none
 * @Return: DEV_DP_TYPE_E:dpid type
 * @Others: none
 */
 __TUYA_HARD_TABLE_EXT \
DEV_DP_TYPE_E tuya_hard_dpid_type(UINT_T app_send_dpid);

#ifdef __cplusplus
}
#endif

#endif