/*
 * @File name: tuya_hard_table.c
 * @Author: caojq
 * @Version: V1.0.0
 * @Date: 2020-10-27 11:36:01
 * @Description: 硬件初始化、控制、存储和上报等操作，主要包含按键、指示灯等；
 * @Others: 
 * @LastEditTime: 2020-10-28 16:53:39
 * @LastEditors:  
 */
#define __TUYA_HARD_TABLE_GLOBAL
#include "tuya_hard_table.h"

#include "gw_intf.h"
#include "uni_log.h"
#include "ty_cJSON.h"
#include "tuya_gpio.h"
#include "tuya_led.h"
#include "tuya_iot_com_api.h"
#include "tuya_iot_internal_api.h"
#include "uni_time_queue.h"

#include "tuya_dp_upload.h"

/*device dp num*/
#define DEV_DP_NUM 3
/*BK pin max num*/
#define GPIO_PIN_MAX 32
/*countdown upload cycle time is 30s*/
#define CD_CYCLE_UPLOAD_TIME 30
/*power stat save in flash*/
#define POWER_STAT_STORAGE "power_stat"
/*channel stat save in falsh*/
#define CHANNEL_STAT_STORAGE "channel_stat"
/*led io level init*/
#define IO_INIT_EN(io_level) ((io_level == IO_LOW_LEVEL)?(FALSE):(TRUE))
/*key low level effective,low_pin_effective is true*/
#define KEY_LOW_LEVEL_EN(io_level) ((io_level == IO_HIGH_LEVEL)?(FALSE):(TRUE))
/*led flash active enable*/
#define IO_ACTIVE_FLASH_EN(io_level) ((io_level == IO_LOW_LEVEL)?(OL_FLASH_LOW):(OL_FLASH_HIGH))
/*led active enable*/
#define IO_ACTIVE_EN(io_level) ((io_level == IO_LOW_LEVEL)?(OL_LOW):(OL_HIGH))
/*led active disable*/
#define IO_ACTIVE_DIS(io_level) ((io_level == IO_LOW_LEVEL)?(OL_HIGH):(OL_LOW))
/*led time set low or high level enable*/
#define IO_LED_TIME_ACTIVE_EN(io_level) ((io_level == IO_LOW_LEVEL)?(OL_LOW):(OL_HIGH))
/*led time set low or high level disable*/
#define IO_LED_TIME_ACTIVE_DIS(io_level) ((io_level == IO_LOW_LEVEL)?(OL_HIGH):(OL_LOW))
/*gpio enable set output level*/
#define LEVEL_TO_VALUE_EN(io_level) ((io_level == IO_LOW_LEVEL)?(FALSE):(TRUE))
/*gpio  disable set output level*/
#define LEVEL_TO_VALUE_DIS(io_level) ((io_level == IO_HIGH_LEVEL)?(FALSE):(TRUE))

/*device user timer*/
typedef struct
{
    TIMER_ID count_down_timer;            ///<countdown timer id
    TIMER_ID channel_save_timer;            ///<channel stat save timer id
}DEV_TIMER_S;

/*timer moudle*/
typedef struct 
{
    TIMER_ID *timer_id;                      ///<register timer id
    P_TIMER_FUNC timer_cb;                  ///<timer call back
}TIMER_MOUDLE_S;

/*LED control moudle  include pin、effective level and handle*/
typedef struct
{
    IO_CONFIG_S io_config;                  ///< led io config
    LED_HANDLE io_handle;                   ///< led io handle control
}LED_MOUDLE_S;

/*device info copy device param*/
typedef struct
{
    GW_WF_CFG_MTHD_SEL wifi_mode;            ///< tuya sdk definition of wifi work mode
    LED_MOUDLE_S wifi_led;                   ///< wifi status display led info
    KEY_USER_DEF_S button;                  ///< key info,include press time,port pin,effective level and so on
    IO_CONFIG_S relay;                      ///< relay control io config
    LED_MOUDLE_S relay_led;                  ///< relay led display relay on/off status
    DEVICE_DP_S dev_dp;                     ///< app send dp control device
}DEVICE_INFO_S;

/*wifi led type*/
typedef enum
{
    LED_FLASH,                                  ///<指示灯闪烁
    LED_ON_OFF,                                 ///<指示灯亮灭
}LED_TYPE_E;

/*继电器状态，默认为断电，由app面板进行设置*/
typedef enum
{
    RELAY_OFF,                               ///<relay is open on power
    RELAY_ON,                              ///<relay is close on power
    RELAY_MEM,                              ///<relay stat keep poweroff stat 
}RELAY_POWER_STAT;

/*设备状态*/
typedef struct
{
    BOOL_T relay_stat;                      ///<current relay stat
    UINT_T relay_cd_sec;                    ///<relay count down sec
    BOOL_T prod_test_flag;                  ///<current dev is prod_test flag
    RELAY_POWER_STAT relay_power_stat;      ///<power on relay_stat
}DEVICE_STAT_S;

/*globe device stat info*/
STATIC DEVICE_STAT_S g_dev_stat = {0};
/*设备信息结构体*/
STATIC DEVICE_INFO_S g_dev_info = {0};
/*goble timer struct*/
STATIC DEV_TIMER_S g_dev_timer = {0};

STATIC OPERATE_RET tuya_hard_get_power_state(RELAY_POWER_STAT *power_stat);
STATIC OPERATE_RET tuya_hard_get_channel_state(BOOL_T *channel_stat);
STATIC VOID tuya_hard_set_channel_stat(BOOL_T channel_stat);
STATIC OPERATE_RET tuya_hard_power_stat_upload(RELAY_POWER_STAT power_stat);
STATIC VOID tuya_hard_power_stat_save(RELAY_POWER_STAT power_stat);

STATIC VOID tuya_hard_cd_timer_cb(UINT_T timerID,PVOID_T pTimerArg);
STATIC VOID tuya_hard_channel_save_cb(UINT_T timerID,PVOID_T pTimerArg);
STATIC VOID tuya_hard_start_soft_timer(TIMER_ID timer_id,UINT_T cycle_time,TIMER_TYPE timer_type);
STATIC VOID tuya_hard_stop_soft_timer(TIMER_ID timer_id);

/*register timer moudle*/
STATIC TIMER_MOUDLE_S g_user_timer[] = {
    {&g_dev_timer.count_down_timer,tuya_hard_cd_timer_cb},
    {&g_dev_timer.channel_save_timer,tuya_hard_channel_save_cb},
};

/**
 * @Function: tuya_hard_led_init
 * @Description: 指示灯初始化设置
 * @Input: led_driver：led信息，包括pin、有效电平和控制句柄
 * @Output: none
 * @Return: OPERATE_RET:初始化结果
 * @Others: none
 */
OPERATE_RET tuya_hard_led_init(IN      LED_MOUDLE_S *led_driver)
{
    OPERATE_RET op_ret = OPRT_OK;

    if(NULL == led_driver || led_driver->io_config.io_pin > GPIO_PIN_MAX) 
    {
        PR_ERR("%s param is null",__FUNCTION__);
        return OPRT_INVALID_PARM;
    }

    if(led_driver->io_config.io_level != IO_INIT_LEVEL)
    {
        op_ret = tuya_create_led_handle_select(led_driver->io_config.io_pin,IO_INIT_EN(led_driver->io_config.io_level),&led_driver->io_handle);
        if(OPRT_OK != op_ret) 
        {
            PR_ERR("tuya_create_led_handle_select fail,fail_num:%d",op_ret);
            return op_ret;
        }
        PR_DEBUG("led init is sucess!!!!!!!!!!!!!!!!");
    }
    else 
    {
        PR_NOTICE("led_driver io_level is IO_INIT_LEVEL");
    }

    return OPRT_OK;
}

/**
 * @Function: tuya_hard_key_init
 * @Description: 
 * @Input: key_driver:按键参数设置，包括按键引脚 、有效电平、长按时间等 
 * @Output: none
 * @Return: OPERATE_RET：函数完成结果
 * @Others: none
 */
OPERATE_RET tuya_hard_key_init(IN CONST KEY_MOUDLE_S *key_driver)
{
    if (NULL == key_driver)
    {
        PR_ERR("%s param is null",__FUNCTION__);
        return OPRT_INVALID_PARM;
    }
    OPERATE_RET op_ret = OPRT_OK;

    op_ret = key_init(NULL,0,20);
    if (OPRT_OK != op_ret)
    {
        PR_ERR("key_init fail,fail_num:%d",op_ret);
        return op_ret;
    }

    g_dev_info.button.port = key_driver->io_config.io_pin;
    if (IO_INIT_LEVEL != key_driver->io_config.io_level)
    {
        g_dev_info.button.low_level_detect = KEY_LOW_LEVEL_EN(key_driver->io_config.io_level);
        g_dev_info.button.lp_tp = LP_ONCE_TRIG;
        g_dev_info.button.long_key_time = key_driver->press_time;
        g_dev_info.button.seq_key_detect_time = 0;
        g_dev_info.button.call_back = key_driver->key_call_back;

        op_ret = reg_proc_key(&g_dev_info.button);
        if (OPRT_OK != op_ret)
        {
            PR_ERR("reg_proc_key fail,fail_num:%d",op_ret);
            return op_ret;
        }
    }
    else
    {
        PR_ERR("dev_info->key_control,io_level is not exit,io_level:%d",key_driver->io_config.io_level);
        return OPRT_COM_ERROR;
    }

    return OPRT_OK;
}

/**
 * @Function: tuya_hard_relay_init
 * @Description: 继电器初始化
 * @Input: relay_driver:继电器参数，包括继电器引脚和有效电平
 * @Output: none
 * @Return: OPERATE_RET:函数返回结果
 * @Others: none
 */
OPERATE_RET tuya_hard_relay_init(IN CONST IO_CONFIG_S *relay_driver)
{
    if (NULL == relay_driver || relay_driver->io_pin > GPIO_PIN_MAX)
    {
        PR_ERR("%s param is null",__FUNCTION__);
        return OPRT_INVALID_PARM;
    }

    OPERATE_RET op_ret = OPRT_OK;

    if(relay_driver->io_level != IO_INIT_LEVEL)
    {
        op_ret = tuya_gpio_inout_set(relay_driver->io_pin,FALSE);
        if(OPRT_OK != op_ret)
        {
            PR_ERR("tuya_gpio_inout_set fail,fail_num:%d",op_ret);
            return op_ret;
        }
        PR_DEBUG("relay init sucess!!!!!!!!!!!!!!!!");
    }
    else
    {
        PR_NOTICE("relay_driver io_level is IO_INIT_LEVEL");
    }
    
    return OPRT_OK;
}

/**
 * @Function: tuya_hard_init
 * @Description: 硬件初始化
 * @Input: dev_info:设备信息初始化，包括wifi工作模式、硬件I/O引脚和有效电平等
 * @Output: none
 * @Return: none
 * @Others: none
 */
VOID tuya_hard_init_dev_stat(VOID)
{
    OPERATE_RET op_ret = OPRT_OK;

    op_ret = tuya_hard_get_power_state(&g_dev_stat.relay_power_stat);
    if (OPRT_OK != op_ret)
    {
        g_dev_stat.relay_power_stat = RELAY_OFF;
        PR_ERR("flash get relay stat on power fail,fail_num:%d",op_ret);
    }

    if (RELAY_MEM == g_dev_stat.relay_power_stat)
    {
        op_ret = tuya_hard_get_channel_state(&g_dev_stat.relay_stat);
        if (OPRT_OK != op_ret)
        {
            g_dev_stat.relay_stat = FALSE;
            PR_DEBUG("flash get relay_stat fail,fail_num:%d",op_ret);
        }
    }
    else
    {
        g_dev_stat.relay_stat = ((RELAY_ON == g_dev_stat.relay_power_stat)?(TRUE):(FALSE));
    }
    tuya_hard_channel_control(g_dev_stat.relay_stat);
    PR_DEBUG("power_on relay_power_stat:%d,init relay_stat:%d",g_dev_stat.relay_power_stat,g_dev_stat.relay_stat);
    return;
}

/**
 * @Function: tuya_hard_init
 * @Description: 硬件初始化
 * @Input: dev_info:设备信息初始化，包括wifi工作模式、硬件I/O引脚和有效电平等
 * @Output: none
 * @Return: none
 * @Others: none
 */
VOID tuya_hard_init(DEVICE_PARAM_S *dev_info)
{
    if(NULL == dev_info)
    {
        PR_ERR("%s invail param is null");
        return;
    }

    OPERATE_RET op_ret = OPRT_OK;

    g_dev_info.wifi_mode = dev_info->wifi_mode;
    g_dev_info.dev_dp.app_relay_stat.dpid_num = dev_info->dev_dp.app_relay_stat.dpid_num;
    g_dev_info.dev_dp.app_relay_stat.is_exist = dev_info->dev_dp.app_relay_stat.is_exist;
    g_dev_info.dev_dp.switch_cd_dp.dpid_num = dev_info->dev_dp.switch_cd_dp.dpid_num;
    g_dev_info.dev_dp.switch_cd_dp.is_exist = dev_info->dev_dp.switch_cd_dp.is_exist;
    g_dev_info.dev_dp.switch_dp.dpid_num = dev_info->dev_dp.switch_dp.dpid_num;
    g_dev_info.dev_dp.switch_dp.is_exist = dev_info->dev_dp.switch_dp.is_exist;
    g_dev_info.wifi_led.io_config.io_pin = dev_info->wifi_led.io_pin;
    g_dev_info.wifi_led.io_config.io_level = dev_info->wifi_led.io_level;
    op_ret = tuya_hard_led_init(&g_dev_info.wifi_led);
    if (OPRT_OK != op_ret)
    {
        return;
    }

    op_ret = tuya_hard_key_init(&dev_info->key_control);
    if (OPRT_OK != op_ret)
    {
        return;
    }

    g_dev_info.relay.io_pin = dev_info->relay.io_pin;
    g_dev_info.relay.io_level = dev_info->relay.io_level;
    op_ret = tuya_hard_relay_init(&g_dev_info.relay);
    if (OPRT_OK != op_ret)
    {
        return;
    }

    g_dev_info.relay_led.io_config.io_pin = dev_info->relay_led.io_pin;
    g_dev_info.relay_led.io_config.io_level = dev_info->relay_led.io_level;
    op_ret = tuya_hard_led_init(&g_dev_info.relay_led);
    if (OPRT_OK != op_ret)
    {
        return;
    }

    tuya_hard_init_dev_stat();

    PR_NOTICE("%s is success",__FUNCTION__);
    return;
}

/**
 * @Function: tuya_hard_get_wifi_mode
 * @Description: 获取设置的涂鸦wifi工作模式
 * @Input: none
 * @Output: none
 * @Return: GW_WF_CFG_MTHD_SEL:涂鸦wifi工作模式
 * @Others: none
 */
GW_WF_CFG_MTHD_SEL tuya_hard_get_wifi_mode(VOID)
{
    return g_dev_info.wifi_mode;
}

/**
 * @Function: tuya_hard_set_led
 * @Description: 
 * @Input: 
 * @Output: 
 * @Return:  
 * @Others: 
 */
VOID tuya_hard_set_led(IN const LED_MOUDLE_S *led_param,LED_TYPE_E led_type,UINT_T led_time)
{
    if (NULL == led_param || led_param->io_config.io_level == IO_INIT_LEVEL)
    {
        PR_ERR("%s param is null",__FUNCTION__);
        return;
    }
    PR_DEBUG("led_time:%d,led_io:%d!",led_time,IO_ACTIVE_FLASH_EN(led_param->io_config.io_level));
    if (led_type == LED_FLASH)
    {
        tuya_set_led_light_type(led_param->io_handle,IO_ACTIVE_FLASH_EN(led_param->io_config.io_level),led_time,LED_TIMER_UNINIT);
        PR_DEBUG("led flash!!");
    }
    else if (led_type == LED_ON_OFF)
    {
        PR_DEBUG("led on off!");
        if(led_time == 0)
        {
            tuya_set_led_light_type(led_param->io_handle,IO_LED_TIME_ACTIVE_DIS(led_param->io_config.io_level),0,0);
            PR_DEBUG("led off!");
        }
        else
        {
            tuya_set_led_light_type(led_param->io_handle,IO_LED_TIME_ACTIVE_EN(led_param->io_config.io_level),0,0);
            PR_DEBUG("led on!");
        }
    }
    else
    {
        PR_ERR("%s led_type is error");
    }
    
    return;
}

/**
 * @Function: tuya_hard_set_led_state
 * @Description: 指示灯状态显示，主要包括产测、wifi状态、状态指示；
 * @Input: led_type:指示灯类型
 * @Output: none
 * @Return: none
 * @Others: none
 */
VOID tuya_hard_set_led_state(LED_STAT_E led_type)
{
    switch (led_type)
    {
        case PROD_TEST_FAIL:
            tuya_hard_set_led(&g_dev_info.wifi_led,LED_FLASH,100);
        break;

        case ENTER_PROD_TEST:
        case EZ_LINKING:
            tuya_hard_set_led(&g_dev_info.wifi_led,LED_FLASH,250);
        break;

        case AP_LINKING:
            tuya_hard_set_led(&g_dev_info.wifi_led,LED_FLASH,1500);
        break;

        case NORMAL_ON:
            tuya_hard_set_led(&g_dev_info.wifi_led,LED_ON_OFF,1);
        break;

        case NORMAL_OFF:
            tuya_hard_set_led(&g_dev_info.wifi_led,LED_ON_OFF,0);
        break;

        default:
        break;
    }
}

/**
 * @Function: tuya_hard_prod_flag
 * @Description: 产测标识设置
 * @Input: prod_flag:产测标识
 * @Output: none
 * @Return: none
 * @Others: none
 */
VOID tuya_hard_prod_flag(BOOL_T prod_flag)
{
    g_dev_stat.prod_test_flag = prod_flag;
}

/**
 * @Function: tuya_hard_judge_key_pin
 * @Description: 
 * @Input: key_touch_pin:按键触发回调pin引脚
 * @Output: none
 * @Return: none
 * @Others: none
 */
BOOL_T tuya_hard_judge_key_pin(IN UCHAR_T key_touch_pin)
{
    if (key_touch_pin == g_dev_info.button.port)
    {
        return TRUE;
    }

    return FALSE;
}

/**
 * @Function: tuya_hard_key_control
 * @Description: 设置短按按键操作控制
 * @Input: none
 * @Output: none
 * @Return: none
 * @Others: 通道状态设置、倒计时关闭、数据上报及存储
 */
VOID tuya_hard_key_control(VOID)
{
    g_dev_stat.relay_stat = !g_dev_stat.relay_stat;
    PR_DEBUG("key control channel status,channel_stat:%d",g_dev_stat.relay_stat);
    g_dev_stat.relay_cd_sec = 0;
    tuya_hard_set_channel_stat(g_dev_stat.relay_stat);
    tuya_hard_channel_upload(DOUBLE_TYPE);
}

/**
 * @Function: tuya_hard_wifi_status
 * @Description: wifi状态对应指示灯显示
 * @Input: cur_stat：当前wifi状态
 * @Output: none
 * @Return: none
 * @Others: none
 */
VOID tuya_hard_wifi_status(IN CONST GW_WIFI_NW_STAT_E cur_stat)
{
    switch (cur_stat)
    {
        case STAT_LOW_POWER:
            tuya_hard_set_led_state(NORMAL_OFF);
        break;
        case STAT_UNPROVISION:
            tuya_hard_set_led_state(EZ_LINKING);
        break;
        case STAT_AP_STA_UNCFG:
            tuya_hard_set_led_state(AP_LINKING);
        break;
        case STAT_AP_STA_DISC:
        case STAT_STA_DISC:
            tuya_hard_set_led_state(NORMAL_OFF);
        break;
        case STAT_CLOUD_CONN:
        case STAT_AP_CLOUD_CONN:
            tuya_hard_set_led_state(NORMAL_ON);
        break;
        default:
        break;
    }
}

/**
 * @Function: tuya_hard_upload_all_data
 * @Description: 设备所有数据dp点上报
 * @Input: none
 * @Output: none
 * @Return: OPRT_OK:操作成功；其他：失败返回错误码
 * @Others: none
 */
OPERATE_RET tuya_hard_upload_all_data(VOID)
{
    OPERATE_RET op_ret = OPRT_OK;
    DP_UPLOAD_S *upload_dp = NULL;
    UINT_T dp_cnt = DEV_DP_NUM;

    upload_dp = tuya_upload_alloc_space(dp_cnt);
    if (NULL == upload_dp)
    {
        PR_ERR("tuya_upload_alloc_space fail");
        return OPRT_COM_ERROR;
    }

    if (g_dev_info.dev_dp.switch_dp.is_exist)
    {
        tuya_upload_dp_bool_frame(upload_dp,g_dev_info.dev_dp.switch_dp.dpid_num,g_dev_stat.relay_stat);
    }

    if (g_dev_info.dev_dp.switch_cd_dp.is_exist)
    {
        tuya_upload_dp_value_frame(upload_dp,g_dev_info.dev_dp.switch_cd_dp.dpid_num,g_dev_stat.relay_cd_sec);
    }

    if (g_dev_info.dev_dp.app_relay_stat.is_exist)
    {
        tuya_upload_dp_enum_frame(upload_dp,g_dev_info.dev_dp.app_relay_stat.dpid_num,g_dev_stat.relay_power_stat);
    }
//            dev_report_dp_json_async(get_gw_cntl()->gw_if.id, upload_dp->dp_buf, upload_dp->cur_idx)
    op_ret = dev_report_dp_json_async(get_gw_cntl()->gw_if.id,upload_dp->dp_buf,upload_dp->cur_index);
    tuya_upload_delete_space(upload_dp);
    if (OPRT_OK != op_ret)
    {
        PR_ERR("dev_report_dp_json_async all_data is fail,fail_num:%d",op_ret);
        return op_ret;
    }
    PR_DEBUG("upload all dp data sucess");
    return OPRT_OK;
}

/**
 * @Function: tuya_hard_clear_flash_data
 * @Description: 删除操作过程中flash存储的数据，恢复到出厂设置状态
 * @Input: none
 * @Output: none
 * @Return: none
 * @Others: none
 */
VOID tuya_hard_relay_power_stat(UINT_T app_send_relay_stat)
{
    OPERATE_RET op_ret = OPRT_OK;

    if (app_send_relay_stat == RELAY_ON)
    {
        g_dev_stat.relay_power_stat = RELAY_ON;
    }
    else if (app_send_relay_stat == RELAY_OFF)
    {
        g_dev_stat.relay_power_stat = RELAY_OFF;
    }
    else if (app_send_relay_stat == RELAY_MEM)
    {
        g_dev_stat.relay_power_stat = RELAY_MEM;
    }
    else
    {
        PR_DEBUG("app_send_relay_stat is error,stat:%d",app_send_relay_stat);
        return;
    }

    op_ret = tuya_hard_power_stat_upload(g_dev_stat.relay_power_stat);
    if (OPRT_OK != op_ret)
    {
        PR_ERR("tuya_hard_power_stat_upload fail,fail_num:%d",op_ret);
    }
    tuya_hard_power_stat_save(g_dev_stat.relay_power_stat);
    return;
}

/**
 * @Function: tuya_hard_power_stat_upload
 * @Description: 上电状态数据上报
 * @Input: power_stat:上电通道状态
 * @Output: none
 * @Return: OPRT_OK: success  Other: fail
 * @Others: none
 */
STATIC OPERATE_RET tuya_hard_power_stat_upload(RELAY_POWER_STAT power_stat)
{
    OPERATE_RET op_ret = OPRT_OK;
    DP_UPLOAD_S *upload_dp = NULL;
    UINT_T dp_cnt = 1;

    upload_dp = tuya_upload_alloc_space(dp_cnt);
    if (NULL == upload_dp)
    {
        PR_ERR("tuya_upload_alloc_space fail");
        return OPRT_COM_ERROR;
    }

    tuya_upload_dp_enum_frame(upload_dp,g_dev_info.dev_dp.app_relay_stat.dpid_num,power_stat);

    op_ret = dev_report_dp_json_async(get_gw_cntl()->gw_if.id,upload_dp->dp_buf,upload_dp->cur_index);
    tuya_upload_delete_space(upload_dp);
    if (OPRT_OK != op_ret)
    {
        PR_ERR("dev_report_dp_json_async all_data is fail,fail_num:%d",op_ret);
        return op_ret;
    }
    PR_DEBUG("upload all dp data sucess");
    return OPRT_OK;
}

/**
 * @Function: tuya_hard_dpid_type
 * @Description: 根据app下发dp点判断类型
 * @Input: app_send_dpid:app send dpid num
 * @Output: none
 * @Return: DEV_DP_TYPE_E:dpid type
 * @Others: none
 */
DEV_DP_TYPE_E tuya_hard_dpid_type(UINT_T app_send_dpid)
{
    PR_DEBUG("app_send_dpid:%d!",app_send_dpid);
    if (app_send_dpid == g_dev_info.dev_dp.switch_dp.dpid_num)
    {
        return SW_TYPE;
    }
    else if (app_send_dpid == g_dev_info.dev_dp.switch_cd_dp.dpid_num)
    {
        return SW_CD_TYPE;
    }
    else if (app_send_dpid == g_dev_info.dev_dp.app_relay_stat.dpid_num)
    {
        return RELAY_ST_TYPE;
    }
    else
    {
        return DP_NOT_EXIST;
    }
}

/**
 * @Function: tuya_hard_set_relay
 * @Description: 根据app下发dp点判断类型
 * @Input: relay_param:继电器模块，引脚、电平和句柄控制
 * @Input: relay_stat:设置继电器模块状态
 * @Output: none
 * @Return: none
 * @Others: none
 */
VOID tuya_hard_set_relay(IO_CONFIG_S *relay_param,BOOL_T relay_stat)
{
    if(NULL == relay_param || relay_param->io_level == IO_INIT_LEVEL)
    {
        PR_ERR("[%s] param fail",__FUNCTION__);
        return;
    }

    if (relay_stat == FALSE)
    {
        tuya_gpio_write(relay_param->io_pin,LEVEL_TO_VALUE_DIS(relay_param->io_level));
        PR_DEBUG("off off off off");
    }
    else
    {
        tuya_gpio_write(relay_param->io_pin,LEVEL_TO_VALUE_EN(relay_param->io_level));
        PR_DEBUG("on on on on");
    }
}

/**
 * @Function: tuya_hard_set_channel_stat
 * @Description: 通道状态设置，包括继电器状态和继电器指示灯状态
 * @Input: channel_stat：通道状态
 * @Output: none
 * @Return: none
 * @Others: none
 */
STATIC VOID tuya_hard_set_channel_stat(BOOL_T channel_stat)
{
    tuya_hard_set_relay(&g_dev_info.relay,channel_stat);
    tuya_hard_set_led(&g_dev_info.relay_led,LED_ON_OFF,(channel_stat == TRUE)?(1):(0));
    tuya_hard_channel_save();
}

/**
 * @Function: tuya_hard_channel_control
 * @Description: app下发状态对通道状态设置
 * @Input: app_send_switch_stat：app下发通道状态
 * @Output: none
 * @Return: none
 * @Others: none
 */
VOID tuya_hard_channel_control(BOOL_T app_send_switch_stat)
{
    g_dev_stat.relay_stat = app_send_switch_stat;
    g_dev_stat.relay_cd_sec = 0;
    if (IsThisSysTimerRun(g_dev_timer.count_down_timer))
    {
        tuya_hard_stop_soft_timer(g_dev_timer.count_down_timer);
    }
    tuya_hard_set_channel_stat(g_dev_stat.relay_stat);
}

/**
 * @Function: tuya_hard_channel_cd_control
 * @Description: app下发通道倒计时设置
 * @Input: app_send_cd_sec:app下发倒计时时间
 * @Output: none
 * @Return: none
 * @Others: none
 */
VOID tuya_hard_channel_cd_control(UINT_T app_send_cd_sec)
{
    g_dev_stat.relay_cd_sec = app_send_cd_sec;
    if (app_send_cd_sec == 0)
    {
        g_dev_stat.relay_cd_sec = 0;
        tuya_hard_stop_soft_timer(g_dev_timer.count_down_timer);
    }
    else
    {
        tuya_hard_start_soft_timer(g_dev_timer.count_down_timer,1000,TIMER_CYCLE);
        tuya_hard_channel_upload(SINGLE_TYPE);
    }
}

/**
 * @Function: tuya_hard_cd_timer_cb
 * @Description:倒计时回调函数
 * @Input: timerID:count_down timer id
 * @Input: pTimerArg:timer callback other param
 * @Output: none
 * @Return: none
 * @Others: 倒计时处理逻辑
 */
STATIC VOID tuya_hard_cd_timer_cb(UINT_T timerID,PVOID_T pTimerArg)
{
    OPERATE_RET op_ret = OPRT_OK;

    g_dev_stat.relay_cd_sec--;
    if (g_dev_stat.relay_cd_sec == 0)
    {
        g_dev_stat.relay_stat = !g_dev_stat.relay_stat;
        tuya_hard_channel_control(g_dev_stat.relay_stat);
        op_ret = tuya_hard_channel_upload(DOUBLE_TYPE);
        if (OPRT_OK != op_ret)
        {
            PR_ERR("[%s] tuya_hard_channel_upload fail,fail_num:%d",op_ret);
        }
        tuya_hard_channel_save();
    }
    else
    {
        if (g_dev_stat.relay_cd_sec % CD_CYCLE_UPLOAD_TIME == 0)
        {
            PR_DEBUG("30s upload count_down_sec is:%d",g_dev_stat.relay_cd_sec);
            tuya_hard_channel_upload(SINGLE_TYPE);
        }
    }
}

/**
 * @Function: tuya_hard_channel_cd_control
 * @Description: app下发通道倒计时设置
 * @Input: none
 * @Output: none
 * @Return: none
 * @Others: 
 */
VOID tuya_hard_channel_save(VOID)
{
    if (g_dev_stat.relay_power_stat == RELAY_MEM)
    {
        tuya_hard_start_soft_timer(g_dev_timer.channel_save_timer,5000,TIMER_ONCE);
    }

    PR_DEBUG("relay_power_stat is not need save,stat:%d",g_dev_stat.relay_power_stat);
    return;
}

/**
 * @Function: tuya_hard_channel_upload
 * @Description: 通道数据上报
 * @Input: upload_type:数据上报类型
 * @Output: none
 * @Return: OPRT_OK: success  Other: fail
 * @Others: 通道数据包括继电器状态和倒计时状态；
 */
OPERATE_RET tuya_hard_channel_upload(CHAN_UPLOAD_TYPE_E upload_type)
{
    OPERATE_RET op_ret = OPRT_OK;
    DP_UPLOAD_S *upload_dp = NULL;
    UINT_T dp_cnt = 0;

    if (upload_type == SINGLE_TYPE)
    {
        dp_cnt = 1;
    }
    else
    {
        dp_cnt = 2;
    }
    upload_dp = tuya_upload_alloc_space(dp_cnt);
    if (NULL == upload_dp)
    {
        PR_ERR("tuya_upload_alloc_space fail");
        return OPRT_COM_ERROR;
    }

    if (g_dev_info.dev_dp.switch_dp.is_exist && upload_type == DOUBLE_TYPE)
    {
        tuya_upload_dp_bool_frame(upload_dp,g_dev_info.dev_dp.switch_dp.dpid_num,g_dev_stat.relay_stat);
    }

    if (g_dev_info.dev_dp.switch_cd_dp.is_exist)
    {
        tuya_upload_dp_value_frame(upload_dp,g_dev_info.dev_dp.switch_cd_dp.dpid_num,g_dev_stat.relay_cd_sec);
    }

    op_ret = dev_report_dp_json_async(get_gw_cntl()->gw_if.id,upload_dp->dp_buf,upload_dp->cur_index);
    tuya_upload_delete_space(upload_dp);
    if (OPRT_OK != op_ret)
    {
        PR_ERR("dev_report_dp_json_async all_data is fail,fail_num:%d",op_ret);
        return op_ret;
    }
    PR_DEBUG("upload all dp data sucess");
    return OPRT_OK;
}

/**
 * @Function: tuya_hard_soft_timer_init
 * @Description: 设备软件定时器初始化
 * @Input: none
 * @Output: none
 * @Return: OPRT_OK: success  Other: fail
 * @Others: none
 */
OPERATE_RET tuya_hard_soft_timer_init(VOID)
{
    OPERATE_RET op_ret = OPRT_OK;
    UINT_T timer_index = 0;

    for (timer_index = 0;timer_index < CNTSOF(g_user_timer);timer_index++)
    {
        op_ret = sys_add_timer(g_user_timer[timer_index].timer_cb,NULL,g_user_timer[timer_index].timer_id);
        if (OPRT_OK != op_ret)
        {
            PR_ERR("add timer_moudle[%d] fail,fail_num:%d",timer_index,op_ret);
            return op_ret;
        }
    }

    return OPRT_OK;
}

/**
 * @Function: tuya_hard_start_soft_timer
 * @Description: 软件定时器使能
 * @Input: timer_id:定时器id
 * @Input: cycle_time:延时时间
 * @Input: timer_type:定时器单次/循环
 * @Output: none
 * @Return: none
 * @Others: none
 */
STATIC VOID tuya_hard_start_soft_timer(TIMER_ID timer_id,UINT_T cycle_time,TIMER_TYPE timer_type)
{
    OPERATE_RET op_ret = OPRT_OK;

    op_ret = sys_start_timer(timer_id,cycle_time,timer_type);
    if (OPRT_OK != op_ret)
    {
        PR_ERR("[%s] start timer fail,fail_num:%d",__FUNCTION__,op_ret);
    }

    return;
}

/**
 * @Function: tuya_hard_stop_soft_timer
 * @Description: soft timer stop
 * @Input: timer_id:定时器id
 * @Output: none
 * @Return: none
 * @Others: none
 */
STATIC VOID tuya_hard_stop_soft_timer(TIMER_ID timer_id)
{
    if (IsThisSysTimerRun(timer_id))
    {
        sys_stop_timer(timer_id);
    }

    return;
}

/**
 * @Function: tuya_hard_get_power_state
 * @Description: flash中获取上电状态
 * @Input: none
 * @Output: power_stat:上电通道状态
 * @Return: OPRT_OK: success  Other: fail
 * @Others: none
 */
STATIC OPERATE_RET tuya_hard_get_power_state(RELAY_POWER_STAT *power_stat)
{
    OPERATE_RET op_ret = OPRT_OK;
    ty_cJSON *root = NULL;
    CHAR_T *buff = NULL;
    UINT_T buff_len = 0;

    op_ret = iot_wd_common_read(POWER_STAT_STORAGE,&buff,&buff_len);
    if (OPRT_OK != op_ret)
    {
        PR_ERR("[%s] iot_wd_common_read fail,fail_num:%d",__FUNCTION__,op_ret);
        return OPRT_COM_ERROR;
    }
    PR_DEBUG("[%s] read_power_stat:%s",__FUNCTION__,buff);

    root = ty_cJSON_Parse(buff);
    Free(buff);
    buff = NULL;
    if (NULL == root)
    {
        PR_ERR("[%s] ty_cJSON_Parse fail",__FUNCTION__);
        return OPRT_CJSON_PARSE_ERR;
    }

    UCHAR_T dp_str[10] = {0};
    sprintf(dp_str,"%d",g_dev_info.dev_dp.app_relay_stat.dpid_num);
    ty_cJSON *js_power = ty_cJSON_GetObjectItem(root,dp_str);
    if (NULL == js_power)
    {
        ty_cJSON_Delete(root);
        root = NULL;
        return OPRT_CJSON_GET_ERR;
    }
    *power_stat = js_power->valueint;
    ty_cJSON_Delete(root);
    root = NULL;

    return OPRT_OK;
}

/**
 * @Function: tuya_hard_power_stat_save
 * @Description: 上电状态存储
 * @Input: power_stat:上电通道状态
 * @Output: none
 * @Return: none
 * @Others: 上电状态组成json格式的字符串存储在flash中，格式为{“dpid”:value}
 */
STATIC VOID tuya_hard_power_stat_save(RELAY_POWER_STAT power_stat)
{
    OPERATE_RET op_ret = OPRT_OK;

    ty_cJSON *root = NULL;
    CHAR_T *out = NULL;

    root = ty_cJSON_CreateObject();
    if (NULL == root)
        {
        PR_ERR("[%s] ty_cJSON_CreateObject fail");
        return;
    }

    CHAR_T dp_str[10] = {0};
    sprintf(dp_str,"%d",g_dev_info.dev_dp.app_relay_stat.dpid_num);
    ty_cJSON_AddNumberToObject(root, dp_str,power_stat);
    out = ty_cJSON_PrintUnformatted(root);
    ty_cJSON_Delete(root);
    root = NULL;
    if (NULL == out)
    {
        PR_ERR("[%s] ty_cJSON_PrintUnformatted fail",__FUNCTION__);
        return;
    }
    PR_DEBUG("[%s] power_stat_string:%s",__FUNCTION__,out);
    op_ret = iot_wd_common_write(POWER_STAT_STORAGE,out,strlen(out));
    Free(out);
    out = NULL;
    if (OPRT_OK != op_ret)
    {
        PR_ERR("[%s] iot_wd_common_write fail,fail_num:%d",__FUNCTION__,op_ret);
        return;
    }

    return;
}

/**
 * @Function: tuya_hard_get_channel_state
 * @Description: flash中获取通道状态
 * @Input: none
 * @Output: channel_stat：通道 状态设置
 * @Return: OPRT_OK: success  Other: fail
 * @Others: none
 */
STATIC OPERATE_RET tuya_hard_get_channel_state(BOOL_T *channel_stat)
{
    OPERATE_RET op_ret = OPRT_OK;
    ty_cJSON *root = NULL;
    CHAR_T *buff = NULL;
    UINT_T buff_len = 0;

    op_ret = iot_wd_common_read(CHANNEL_STAT_STORAGE,&buff,&buff_len);
    if (OPRT_OK != op_ret)
    {
        PR_ERR("[%s] iot_wd_common_read fail,fail_num:%d",__FUNCTION__,op_ret);
        return OPRT_COM_ERROR;
    }
    PR_DEBUG("[%s] read_channel_stat:%s",__FUNCTION__,buff);

    root = ty_cJSON_Parse(buff);
    Free(buff);
    buff = NULL;
    if (NULL == root)
    {
        PR_ERR("[%s] ty_cJSON_Parse fail",__FUNCTION__);
        return OPRT_CJSON_PARSE_ERR;
    }

    UCHAR_T dp_str[10] = {0};
    sprintf(dp_str,"%d",g_dev_info.dev_dp.switch_dp.dpid_num);
    ty_cJSON *js_channel = ty_cJSON_GetObjectItem(root,dp_str);
    if (NULL == js_channel)
    {
        ty_cJSON_Delete(root);
        root = NULL;
        return OPRT_CJSON_GET_ERR;
    }
    *channel_stat = js_channel->valueint;
    ty_cJSON_Delete(root);
    root = NULL;

    return OPRT_OK;
}
/**
 * @Function: tuya_hard_channel_save_cb
 * @Description: 通道存储回调函数
 * @Input: timerID:channel_save timer id
 * @Input: pTimerArg:timer callback other param
 * @Output: none
 * @Return: none
 * @Others: 存储通道状态，格式为{“dpid”:TRUE/FALSE}
 */
STATIC VOID tuya_hard_channel_save_cb(UINT_T timerID,PVOID_T pTimerArg)
{
    OPERATE_RET op_ret = OPRT_OK;

    ty_cJSON *root = NULL;
    CHAR_T *out = NULL;

    root = ty_cJSON_CreateObject();
    if (NULL == root)
        {
        PR_ERR("[%s] ty_cJSON_CreateObject fail");
        return;
    }

    CHAR_T dp_str[10] = {0};
    sprintf(dp_str,"%d",g_dev_info.dev_dp.switch_dp.dpid_num);
    ty_cJSON_AddBoolToObject(root, dp_str,g_dev_stat.relay_stat);
    out = ty_cJSON_PrintUnformatted(root);
    ty_cJSON_Delete(root);
    root = NULL;
    if (NULL == out)
    {
        PR_ERR("[%s] ty_cJSON_PrintUnformatted fail",__FUNCTION__);
        return;
    }
    PR_DEBUG("[%s] channel_stat_string:%s",__FUNCTION__,out);
    op_ret = iot_wd_common_write(CHANNEL_STAT_STORAGE,out,strlen(out));
    Free(out);
    out = NULL;
    if (OPRT_OK != op_ret)
    {
        PR_ERR("[%s] iot_wd_common_write fail,fail_num:%d",__FUNCTION__,op_ret);
        return;
    }

    return;
}

/**
 * @Function: tuya_hard_clear_flash_data
 * @Description: 删除操作过程中flash存储的数据，恢复到出厂设置状态
 * @Input: none
 * @Output: none
 * @Return: none
 * @Others: 删除通道状态和上电状态
 */
VOID tuya_hard_clear_flash_data(VOID)
{
    OPERATE_RET op_ret = OPRT_OK;

    op_ret = iot_wd_common_delete(POWER_STAT_STORAGE);
    if (OPRT_OK != op_ret)
        {
        PR_ERR("[%s] delete power_stat fail",__FUNCTION__);
    }

    op_ret = iot_wd_common_delete(CHANNEL_STAT_STORAGE);
    if (OPRT_OK != op_ret)
        {
        PR_ERR("[%s] delete channel_stat fail",__FUNCTION__);
    }

    return;
}

/**
 * @Function: tuya_hard_get_channel_stat
 * @Description: 获取通道状态
 * @Input: none
 * @Output: none
 * @Return: BOOL_T:通道状态
 * @Others: none
 */
BOOL_T tuya_hard_get_channel_stat(VOID)
{
    return g_dev_stat.relay_stat;
}