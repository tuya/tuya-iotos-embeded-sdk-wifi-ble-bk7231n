/**
* @file light_system.c
* @author www.tuya.com
* @brief system adapter process
* @version 0.2
* @date 2022-03-25
*
* @copyright Copyright (c) tuya.inc 2022
*
*/

#include "light_system.h"
#include "device_config_load.h"
#include "light_init.h"
#include "light_printf.h"
#include "light_control.h"
#include "light_prod.h"
#include "user_flash.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "tuya_iot_wifi_api.h"
#include "gw_intf.h"
#include "tuya_ws_db.h"
#include "user_timer.h"
#include "light_tools.h"
#include "uni_log.h"
#include "base_event_info.h"
#include "base_event.h"
#include "tuya_hal_semaphore.h"


#define COLOR_DATA_LEN      12

STATIC BOOL_T sg_fastboot_init = FALSE;    //fastboot 初始化标志  

STATIC UINT_T sg_countdown_value = 0;    //倒计时值

STATIC UCHAR_T sg_can_blink_flag = TRUE;    //是否闪烁标志

//VOID vLightDpUpload(UCHAR_T ucDPNum, VOID *pData);
STATIC VOID _light_all_dp_upload(VOID);
STATIC VOID _light_shade_thread(PVOID_T arg);
STATIC UCHAR_T *_light_get_dp_name(IN DP_CNTL_S *dp_cntl, IN UCHAR_T enum_id);

SEM_HANDLE g_semaphore_shade = NULL;

/**
 * @brief: 灯软件系统初始化
 * @param: none
 * @return: OPERATE_LIGHT 0表示成功，非0请参照tuya error code描述文档 
 */
OPERATE_LIGHT tuya_light_sys_software_init(VOID)
{
    OPERATE_LIGHT ret = -1;

    //创建一个二值信号量
    ret = tuya_hal_semaphore_create_init(&g_semaphore_shade, 0, 1); 
    if (ret != OPRT_OK) {
        PR_ERR("Create Binary Semaphore error!: %d", ret);
        return ret;
    }

    ret = tuya_hal_thread_create(NULL, "thread_shade", 512 * 4, TRD_PRIO_0, _light_shade_thread, NULL);
    if (ret != OPRT_OK) {
        PR_ERR("Create thread error!: %d", ret); 
        return ret;
    }

    return OPRT_OK;
}


/**
 * @brief: light shade change thread
 * @param: PVOID_T arg -> NULL
 * @return: none
 */
STATIC VOID _light_shade_thread(PVOID_T arg)
{  
    tuya_hal_semaphore_post(g_semaphore_shade);

    while (1) {
 
        tuya_hal_semaphore_wait(g_semaphore_shade);

        tuya_light_ctrl_shade_gradually();  //
    }

    return;
}

/**
 * @brief: 灯硬件定时器回调函数
 * @param: none
 * @return: none
 */
VOID tuya_light_sys_hardware_timer_callback(VOID)
{
    tuya_hal_semaphore_post(g_semaphore_shade);

    return;

}

/**
 * @brief: 判断硬件系统是否重启
 * @param: none
 * @retval: BOOL_T TRUE -> 硬件系统重启 FALSE -> 硬件系统没重启
 */
BOOL_T tuya_light_sys_hardware_reboot_judge(VOID)
{
    TY_RST_REASON_E rest_info = tuya_hal_system_get_rst_info();    //获取硬件重启原因

    PR_DEBUG("reset info -> reason is %d>>>", rest_info);

    if (TY_RST_POWER_OFF == rest_info) {
        return TRUE;
    }

    return FALSE;
}

/**
 * @brief: light reset(re-distribute) proc
 * @param: none
 * @retval: OPERATE_LIGHT 0表示成功，非0请参照tuya error code描述文档 
 */
OPERATE_LIGHT tuya_light_sys_reset_cnt_over_callback(VOID)
{
    OPERATE_LIGHT ret = -1;
    UCHAR_T connect_mode = 0;
    //PR_NOTICE("**********************");
    connect_mode = tuya_light_ctrl_get_connect_mode();    //获取wifi的连接模式

    ret = tuya_iot_wf_gw_fast_unactive(connect_mode, WF_START_SMART_FIRST);
    if (ret != OPRT_OK) {
        PR_ERR("tuya_iot_wf_gw_fast_unactive error");
    }

    return ret;
}

/**
 * @brief: wifi连接,指示灯闪烁显示
 * @param: IN CONST GW_WIFI_NW_STAT_E stat -> wifi 连接状态
 * @retval: none
 */
STATIC VOID _wifi_status_display_callback(IN CONST GW_WIFI_NW_STAT_E stat)
{
    OPERATE_LIGHT ret = -1;
    STATIC GW_WIFI_NW_STAT_E s_last_wifi_stat = 0xFF;    //最近一次WiFi连接状态
    STATIC UCHAR_T s_connect_flag = FALSE;    //连接标志

    //PR_NOTICE("wifi display callback %d  %d  %d %d", s_last_wifi_stat,  stat, s_connect_flag, sg_can_blink_flag);

    if (s_last_wifi_stat != stat) {

        PR_DEBUG("last wifi stat:%d, wifi stat %d",s_last_wifi_stat,stat);
        PR_DEBUG("size:%d", tuya_hal_system_getheapsize());

        if (!sg_can_blink_flag) {
            //when app remove, can't blink
            PR_DEBUG("app remove proc....,can't blink!");
            return;
        }

        switch (stat) {
        case STAT_LOW_POWER:
            PR_NOTICE("start to lowpower display!");

            //根据配对提醒模式停止闪烁或呼吸
            ret = tuya_light_ctrl_auto_blink_stop(); 
            if (ret != OPRT_OK) {
                PR_ERR("Stop auto blink timer error!");
            }

            break;

        case STAT_UNPROVISION:
            s_connect_flag = TRUE;    //已经开始分配网络                

            //设置灯的默认控制数据
            tuya_light_ctrl_data_reset();    

            PR_NOTICE("start ez config auto blink");

            //根据配对提醒模式开始闪烁
            ret = tuya_light_ctrl_auto_blink_start(PAIRING_NORMAL_BLINK_FREQ);  
            if (ret != OPRT_OK) {
                PR_ERR("start auto blink timer error!");
            }
            break;

        case STAT_AP_STA_UNCFG:

            s_connect_flag = TRUE;    //已经开始分配网络                

            //设置灯的默认控制数据
            tuya_light_ctrl_data_reset();

            PR_NOTICE("start AP config auto blink");

            //根据配对提醒模式开始闪烁
            ret = tuya_light_ctrl_auto_blink_start(PAIRING_SLOW_BLINK_FREQ);
            if (ret != OPRT_OK) {
                PR_ERR("start auto blink timer error!");
            }
            break;

        case STAT_AP_STA_DISC: 
            //do nothing
            break;

        case STAT_AP_STA_CONN:  
            //priority turn down
            //do nothing
            break;


        case STAT_STA_DISC:

            // only distribution network, need to stop and run ctrl proc
            if (s_connect_flag != TRUE) {
                break;
            }

            s_connect_flag = FALSE;    // only distribution network, need to stop and run ctrl proc
            PR_DEBUG("Blink stop!!!!");

            //根据配对提醒模式停止闪烁或呼吸
            ret = tuya_light_ctrl_auto_blink_stop();
            if (ret != OPRT_OK) {
                PR_ERR("Stop blink timer error!");
            }
            break;

        case STAT_STA_CONN:     //priority turn down
            //only distribution network, need to stop and run ctrl proc
            if (s_connect_flag != TRUE) {    
                break;
            }

            s_connect_flag = FALSE;
            
            PR_NOTICE("Blink stop!!!!");

            //根据配对提醒模式停止闪烁或呼吸
            ret = tuya_light_ctrl_auto_blink_stop();
            if (ret != OPRT_OK) {
                PR_ERR("Stop blink timer error!");
            }
            break;

        case STAT_CLOUD_CONN:
        case STAT_AP_CLOUD_CONN:

            //灯的所有dp上传
            _light_all_dp_upload();

            break;

        default:
            break;
        }
        s_last_wifi_stat = stat; //更新WiFi的连接状态
    }
    return;
}

/**
 * @brief: 颜色格式转换color --> RGB
 * @param: IN CHAR_T *date -> 颜色数据
 * @param: OUT USHORT_T *red -> 保存R格式
数据
 * @param: OUT USHORT_T *green -> 保存G格式
数据
 * @param: OUT USHORT_T *blue -> 保存B格式
数据
 * @retval: none
*/
STATIC VOID _light_sys_color_to_rgb(IN CHAR_T *date, OUT USHORT_T *red, OUT USHORT_T *green, OUT USHORT_T *blue)
{
    USHORT_T h;    //色调
    USHORT_T s;    //饱和度
    USHORT_T v;    //明度
    USHORT_T max = 0;
    USHORT_T min = 0;

    h = tuya_light_tool_str_to_ushort(tuya_light_tool_asc_to_hex(date[0]), tuya_light_tool_asc_to_hex(date[1]), tuya_light_tool_asc_to_hex(date[2]), tuya_light_tool_asc_to_hex(date[3]));    //提取色调的值

    s = tuya_light_tool_str_to_ushort(tuya_light_tool_asc_to_hex(date[4]), tuya_light_tool_asc_to_hex(date[5]), tuya_light_tool_asc_to_hex(date[6]), tuya_light_tool_asc_to_hex(date[7]));    //提取饱和度的值

    v = tuya_light_tool_str_to_ushort(tuya_light_tool_asc_to_hex(date[8]), tuya_light_tool_asc_to_hex(date[9]), tuya_light_tool_asc_to_hex(date[10]), tuya_light_tool_asc_to_hex(date[11]));    //提取明度的值

    PR_DEBUG("HSV value %d %d %d", h, s, v);

    max = 1000 * ((FLOAT_T) tuya_light_ctrl_get_color_max() / 100.0);    //max放大10倍
    min = 1000 * ((FLOAT_T) tuya_light_ctrl_get_color_min() / 100.0);	 //min放大10倍

    v = (v - 10) * (max - min) / (1000 - 10) + min;	   //?

    tuya_light_tool_hsv_to_rgb(h, s, v, red, green, blue);    //hsv转换为rgb值

    return;

}

/**
 * @brief: reponse switch property process, realtek
 * @param {OUT BOOL_T on_off -> switch status, TRUE is ON}
 * @retval: none
 */
VOID tuya_light_ctrl_data_switch_respone(OUT BOOL_T on_off)
{
    OPERATE_RET op_ret = OPRT_OK;

    INT_T dp_cnt = 1; /* update DP number */

    TY_OBJ_DP_S *dp_arr = (TY_OBJ_DP_S *)Malloc(dp_cnt*SIZEOF(TY_OBJ_DP_S));
    if(NULL == dp_arr) {
        PR_ERR("malloc failed");
        return;
    }

    /* initialize requested memory space */
    memset(dp_arr, 0, dp_cnt*SIZEOF(TY_OBJ_DP_S));

    dp_arr[0].dpid = DPID_SWITCH; /* DP ID */
    dp_arr[0].type = PROP_BOOL; /* DP type */
    dp_arr[0].time_stamp = 0;
    dp_arr[0].value.dp_bool = on_off; /* DP data */

    /* report DP */
    op_ret = dev_report_dp_json_async(NULL ,dp_arr, dp_cnt);

    /* free requested memory space */
    Free(dp_arr);
    dp_arr = NULL;
    if(OPRT_OK != op_ret) {
        PR_ERR("dev_report_dp_json_async relay_config data error,err_num",op_ret);
    }

    return;
}

/**
 * @brief: reponse mode property process, realtek
 * @param: OUT LIGHT_MODE_E Mode 
 * @retval: none
 */
VOID tuya_light_ctrl_data_mode_response(OUT LIGHT_MODE_E Mode)
{
    OPERATE_RET op_ret = OPRT_OK;

    INT_T dp_cnt = 1; /* update DP number */

    TY_OBJ_DP_S *dp_arr = (TY_OBJ_DP_S *)Malloc(dp_cnt*SIZEOF(TY_OBJ_DP_S));
    if(NULL == dp_arr) {
        PR_ERR("malloc failed");
        return;
    }

    /* initialize requested memory space */
    memset(dp_arr, 0, dp_cnt*SIZEOF(TY_OBJ_DP_S));

    dp_arr[0].dpid = DPID_MODE; /* DP ID */
    dp_arr[0].type = PROP_ENUM; /* DP type */
    dp_arr[0].time_stamp = 0;
    dp_arr[0].value.dp_enum = Mode; /* DP data */

    /* report DP */
    op_ret = dev_report_dp_json_async(NULL ,dp_arr, dp_cnt);

    /* free requested memory space */
    Free(dp_arr);
    dp_arr = NULL;
    if(OPRT_OK != op_ret) {
        PR_ERR("dev_report_dp_json_async relay_config data error,err_num",op_ret);
    }

    return;
}

/**
 * @brief: reponse bright property process
 * @param {OUT LIGHT_MODE_E Mode} 
 * @param {OUT USHORT_T bright} 
 * @attention: need reponse mode property,as set bright value, will auto set the Mode to WHITE_MODE!
 * @retval: none
 */
VOID tuya_light_ctrl_data_bright_response(OUT LIGHT_MODE_E mode, OUT USHORT_T bright)
{
    OPERATE_RET op_ret = OPRT_OK;

    INT_T dp_cnt = 2; /* update DP number */

    TY_OBJ_DP_S *dp_arr = (TY_OBJ_DP_S *)Malloc(dp_cnt*SIZEOF(TY_OBJ_DP_S));
    if(NULL == dp_arr) {
        PR_ERR("malloc failed");
        return;
    }

    /* initialize requested memory space */
    memset(dp_arr, 0, dp_cnt*SIZEOF(TY_OBJ_DP_S));

    dp_arr[0].dpid = DPID_MODE; /* DP ID */
    dp_arr[0].type = PROP_ENUM; /* DP type */
    dp_arr[0].time_stamp = 0;
    dp_arr[0].value.dp_enum = mode; /* DP data */
    dp_arr[1].dpid = DPID_BRIGHT; /* DP ID */
    dp_arr[1].type = PROP_VALUE; /* DP type */
    dp_arr[1].time_stamp = 0;
    dp_arr[1].value.dp_value = bright; /* DP data */

    /* report DP */
    op_ret = dev_report_dp_json_async(NULL ,dp_arr, dp_cnt);

    /* free requested memory space */
    Free(dp_arr);
    dp_arr = NULL;
    if(OPRT_OK != op_ret) {
        PR_ERR("dev_report_dp_json_async relay_config data error,err_num",op_ret);
    }

    return;
}

/**
 * @brief: reponse temperature property process, realtek
 * @param {OUT LIGHT_MODE_E Mode}
 * @param {OUT USHORT_T temperature}
 * @attention: need reponse mode property,as set temperature value, will auto set the Mode to WHITE_MODE!
 * @retval: none
 */
VOID tuya_light_ctrl_data_temperature_response(OUT LIGHT_MODE_E mode, OUT USHORT_T temperature)
{
    OPERATE_RET op_ret = OPRT_OK;

    INT_T dp_cnt = 2; /* update DP number */

    TY_OBJ_DP_S *dp_arr = (TY_OBJ_DP_S *)Malloc(dp_cnt*SIZEOF(TY_OBJ_DP_S));
    if(NULL == dp_arr) {
        PR_ERR("malloc failed");
        return;
    }

    /* initialize requested memory space */
    memset(dp_arr, 0, dp_cnt*SIZEOF(TY_OBJ_DP_S));

    dp_arr[0].dpid = DPID_MODE; /* DP ID */
    dp_arr[0].type = PROP_ENUM; /* DP type */
    dp_arr[0].time_stamp = 0;
    dp_arr[0].value.dp_enum = mode; /* DP data */
    dp_arr[1].dpid = DPID_TEMPR; /* DP ID */
    dp_arr[1].type = PROP_VALUE; /* DP type */
    dp_arr[1].time_stamp = 0;
    dp_arr[1].value.dp_value = temperature; /* DP data */

    /* report DP */
    op_ret = dev_report_dp_json_async(NULL ,dp_arr, dp_cnt);

    /* free requested memory space */
    Free(dp_arr);
    dp_arr = NULL;
    if(OPRT_OK != op_ret) {
        PR_ERR("dev_report_dp_json_async relay_config data error,err_num",op_ret);
    }

    return;
}

/**
 * @brief: reponse RGB property process
 * @param {OUT color_origin_t *ColorOrigin} 
 * @retval: none
 */
VOID tuya_light_ctrl_data_rgb_response(OUT color_origin_t *color_origin)
{
    OPERATE_RET op_ret = OPRT_OK;

    INT_T dp_cnt = 1; /* update DP number */

    TY_OBJ_DP_S *dp_arr = (TY_OBJ_DP_S *)Malloc(dp_cnt*SIZEOF(TY_OBJ_DP_S));
    if(NULL == dp_arr) {
        PR_ERR("malloc failed");
        return;
    }

    /* initialize requested memory space */
    memset(dp_arr, 0, dp_cnt*SIZEOF(TY_OBJ_DP_S));

    dp_arr[0].dpid = DPID_COLOR; /* DP ID */
    dp_arr[0].type = PROP_STR; /* DP type */
    dp_arr[0].time_stamp = 0;
    dp_arr[0].value.dp_str = color_origin->color_str; /* DP data */

    /* report DP */
    op_ret = dev_report_dp_json_async(NULL ,dp_arr, dp_cnt);

    /* free requested memory space */
    Free(dp_arr);
    dp_arr = NULL;
    if(OPRT_OK != op_ret) {
        PR_ERR("dev_report_dp_json_async relay_config data error,err_num",op_ret);
    }

    return;
}


/**
 * @brief: reponse scene property process, realtek
 * @param {OUT UCHAR_T *SceneData} 
 * @retval: none
 */
VOID tuya_light_ctrl_data_scene_response(OUT CHAR_T *scene_data)
{
    OPERATE_RET op_ret = OPRT_OK;

    INT_T dp_cnt = 1; /* update DP number */

    TY_OBJ_DP_S *dp_arr = (TY_OBJ_DP_S *)Malloc(dp_cnt*SIZEOF(TY_OBJ_DP_S));
    if(NULL == dp_arr) {
        PR_ERR("malloc failed");
        return;
    }

    /* initialize requested memory space */
    memset(dp_arr, 0, dp_cnt*SIZEOF(TY_OBJ_DP_S));

    dp_arr[0].dpid = DPID_SCENE; /* DP ID */
    dp_arr[0].type = PROP_STR; /* DP type */
    dp_arr[0].time_stamp = 0;
    dp_arr[0].value.dp_str = scene_data; /* DP data */

    /* report DP */
    op_ret = dev_report_dp_json_async(NULL ,dp_arr, dp_cnt);

    /* free requested memory space */
    Free(dp_arr);
    dp_arr = NULL;
    if(OPRT_OK != op_ret) {
        PR_ERR("dev_report_dp_json_async relay_config data error,err_num",op_ret);
    }

    return;
}

/**
 * @brief: Light countdown proc
 * @param {OUT UINT_T RemainTimeSec -> remain countdown time,unit:s}
 * @retval: none
 */
VOID tuya_light_ctrl_data_countdown_response(OUT UINT_T remain_time)
{
    OPERATE_RET op_ret = OPRT_OK;

    INT_T dp_cnt = 1; /* update DP number */

    TY_OBJ_DP_S *dp_arr = (TY_OBJ_DP_S *)Malloc(dp_cnt*SIZEOF(TY_OBJ_DP_S));
    if(NULL == dp_arr) {
        PR_ERR("malloc failed");
        return;
    }

    /* initialize requested memory space */
    memset(dp_arr, 0, dp_cnt*SIZEOF(TY_OBJ_DP_S));

    dp_arr[0].dpid = DPID_COUNTDOWN; /* DP ID */
    dp_arr[0].type = PROP_VALUE; /* DP type */
    dp_arr[0].time_stamp = 0;
    dp_arr[0].value.dp_value = remain_time; /* DP data */

    /* report DP */
    op_ret = dev_report_dp_json_async(NULL ,dp_arr, dp_cnt);

    /* free requested memory space */
    Free(dp_arr);
    dp_arr = NULL;
    if(OPRT_OK != op_ret) {
        PR_ERR("dev_report_dp_json_async relay_config data error,err_num",op_ret);
    }

    return;
}

/**
 * @brief: 退出低功耗模式
 * @param: none
 * @retval: none
 */
VOID tuya_light_sys_strl_low_power_exist(VOID)
{
    OPERATE_LIGHT ret = -1;

    //失能wifi低功耗模式
    tuya_wifi_lp_disable();    

    //使能硬件定时器
    ret = tuya_user_hardware_timer_start(HW_TIMER_CYCLE_US, (VOID *)tuya_light_ctrl_hardware_timer_callback);
    if (ret != OPRT_OK) {
        PR_ERR("Light exist lowpower start hardware timer error!");
    }

    return;
}

/**
 * @brief: 进入低功耗模式
 * @param: none
 * @retval: none
 */
VOID tuya_light_sys_ctrl_low_power_enter(VOID)
{
    OPERATE_LIGHT ret = -1;

    PR_DEBUG("enter lowpower!");

    //使能wifi低功耗模式
    tuya_wifi_lp_enable(); 

    //关闭硬件定时器
    ret = tuya_user_hardware_timer_stop();   
    if (ret != OPRT_OK) {
        PR_ERR("Light enter lowpower stop hardware timer error!");
    }

    return;
}

/**
 * @brief: 灯dp控制程序
 * @param: CONST TY_OBJ_DP_S *root -> 要控制的dp点
 * @retval: BOOL_T TURE or FALSE
 */
STATIC BOOL_T _light_dp_process(CONST TY_OBJ_DP_S *root)
{
    OPERATE_LIGHT ret = -1;
    UINT_T len;
    UCHAR_T dpid;
    BOOL_T active_flag = FALSE; //是否执行的flag
    color_rgb_t color_data; //颜色的RGB数据
    color_origin_t color_origin; //本身的颜色


    dpid = root->dpid; //dp点的id

    PR_DEBUG("light_light_dp_proc dpid=%d",dpid);

    switch (dpid) {

    case DPID_SWITCH:
        PR_DEBUG("set switch %d",root->value.dp_bool);  //bool dp点值

        //设置灯开关的控制数据
        ret = tuya_light_ctrl_data_switch_set(root->value.dp_bool);
        if (OPRT_OK == ret) {
            active_flag = TRUE;

            //设置灯光倒计时值
            ret = tuya_light_ctrl_data_countdown_set(0);
            if (ret != OPRT_OK) {
            }
        }
        break;

    case DPID_MODE:
        if (root->type != PROP_ENUM) {
            break;
        }
        PR_DEBUG("mode is %d", root->value.dp_enum);

        //设置灯的模式
        ret = tuya_light_ctrl_data_mode_set(root->value.dp_enum);
        if (OPRT_OK == ret) {
            active_flag = TRUE;
        }
        break;

    case DPID_BRIGHT:
        if (root->type != PROP_VALUE) {
            break;
        }
        PR_DEBUG("bright set value %d", root->value.dp_value);

        //设置灯的亮度
        ret = tuya_light_ctrl_data_bright_set(root->value.dp_value);
        if (OPRT_OK == ret) {
            active_flag = TRUE;
        }
        break;

    case DPID_TEMPR:
        if (root->type != PROP_VALUE) {
            break;
        }
        PR_DEBUG("temper set value %d", root->value.dp_value);

        //设置灯的色温
        ret = tuya_light_ctrl_data_temperature_set(root->value.dp_value);
        if (OPRT_OK == ret) {
            active_flag = TRUE;
        }
        break;

    case DPID_COLOR:
        if (root->type != PROP_STR) {
            break;
        }

        len = strlen(root->value.dp_str);
        if (len != COLOR_DATA_LEN) {
            PR_ERR("the data length is wrong: %d", len);
            break;
        }

        PR_DEBUG("color data %s", root->value.dp_str);

        //灯要显示的颜色转换为RGB数据
        _light_sys_color_to_rgb(root->value.dp_str, &color_data.red, &color_data.green, &color_data.blue);
        memcpy(color_origin.color_str, root->value.dp_str, SIZEOF(color_origin.color_str));

        //设置灯的RGB数据 
        ret = tuya_light_ctrl_data_rgb_set(&color_data, &color_origin);
        if (OPRT_OK == ret) {
            active_flag = TRUE;
        }
        break;

    case DPID_SCENE:
        if (root->type != PROP_STR) {
            break;
        }

        PR_DEBUG("scene data %s",root->value.dp_str);

        //设置灯的场景模式数据
        ret = tuya_light_ctrl_data_scene_set(root->value.dp_str);
        if (OPRT_OK == ret) {
            active_flag = TRUE;
        }
        break;

    case DPID_COUNTDOWN:
        if (root->type != PROP_VALUE) {
            break;
        }

        //设置灯的倒计时值
        ret = tuya_light_ctrl_data_countdown_set(root->value.dp_value);
        if (ret != OPRT_OK) {
            PR_ERR("count down set error!");
        } else {
            ; //do nothing
        }
        break;

    case DPID_MUSIC:
        if (root->type != PROP_STR) {
            break;
        }

        PR_DEBUG("music ctrl data %s", root->value.dp_str);

        //
        ret = tuya_light_ctrl_data_realtime_adjust_set(TRUE, root->value.dp_str);
        if (OPRT_OK != ret) {
            break;
        }

        //
        ret = tuya_light_realtime_ctrl_process();
        if (ret != OPRT_OK) {
            PR_ERR("real time control error!");
        }
        break;

    case DPID_CONTROL:
        if (root->type != PROP_STR) {
            break;
        }

        PR_NOTICE("realtime ctrl data %s", root->value.dp_str);

        //设置灯实时控制数据
        ret = tuya_light_ctrl_data_realtime_adjust_set(FALSE, root->value.dp_str);
        if (OPRT_OK != ret) {
            break;
        }

        //灯实时控制程序
        ret = tuya_light_realtime_ctrl_process();
        if (ret != OPRT_OK) {
            PR_ERR("real time control error!");
        }
        break;

    default:
        break;
    }
    PR_DEBUG("ctrl data need to proc flag =%d", active_flag);

    return active_flag;
}

/**
 * @brief: 获取dp点的名字
 * @param: IN TY_OBJ_DP_S *dp_cntl -> control DP data
 * @param: IN UCHAR_T enum_id -> DP enum id
 * @return: none
 * @retval: dp点的名字字符串
 */
STATIC UCHAR_T *_light_get_dp_name(IN DP_CNTL_S *dp_cntl, IN UCHAR_T enum_id)
{
    if (dp_cntl == NULL) {
        return NULL;
    }

    if (enum_id >= dp_cntl->prop.prop_enum.cnt) {
        return NULL;
    }

    return dp_cntl->prop.prop_enum.pp_enum[enum_id];
}

/**
 * @brief: 灯的所有dp点上传
 * @param: none
 * @retval: none
 */
STATIC VOID _light_all_dp_upload(VOID)
{
    OPERATE_LIGHT ret = -1;
    INT_T dp_cnt = 6; /* update DP number */
    light_ctrl_data_t* update = NULL;

    update = (light_ctrl_data_t*)Malloc(SIZEOF(light_ctrl_data_t));
    if (NULL == update) {
        PR_ERR("update malloc failed");
        return;
    }

    /* switch status */
    ret = tuya_light_ctrl_data_switch_get(&update->switch_status);
    if (ret != LIGHT_OK) {
        PR_ERR("get switch status error, %d", ret);
        Free(update);
        return;
    }

    /* mode */
    ret = tuya_light_ctrl_data_mode_get(&update->mode);
    if (ret != LIGHT_OK) {
        PR_ERR("get mode error, %d", ret);
        Free(update);
        return;
    }

    /* bright */
    ret = tuya_light_ctrl_data_bright_get(&update->bright);
    if (ret != LIGHT_OK) {
        PR_ERR("get bright error, %d", ret);
        Free(update);
        return;
    }

    /* temperature */
    ret = tuya_light_ctrl_data_temperature_get(&update->temper);
    if (ret != LIGHT_OK) {
        PR_ERR("get temperature error, %d", ret);
        Free(update);
        return;
    }

    /* color */
    ret = tuya_light_ctrl_data_origin_get(&update->color_origin);
    if (ret != LIGHT_OK) {
        PR_ERR("get color error, %d", ret);
        Free(update);
        return;
    }

    /* scene */
    ret = tuya_light_ctrl_data_scene_get(update->scene);
    if (ret != LIGHT_OK) {
        PR_ERR("get scene error, %d", ret);
        Free(update);
        return;
    }

    TY_OBJ_DP_S *dp_arr = (TY_OBJ_DP_S *)Malloc(dp_cnt*SIZEOF(TY_OBJ_DP_S));
    if(NULL == dp_arr) {
        PR_ERR("malloc dp array failed");
        return;
    }
    /* initialize requested memory space */
    memset(dp_arr, 0, dp_cnt*SIZEOF(TY_OBJ_DP_S));

    dp_arr[0].dpid = DPID_SWITCH; 
    dp_arr[0].type = PROP_BOOL; 
    dp_arr[0].time_stamp = 0;
    dp_arr[0].value.dp_bool = update->switch_status; 

    dp_arr[1].dpid = DPID_MODE; 
    dp_arr[1].type = PROP_ENUM;
    dp_arr[1].time_stamp = 0;
    dp_arr[1].value.dp_enum = update->mode;

    dp_arr[2].dpid = DPID_BRIGHT; 
    dp_arr[2].type = PROP_VALUE;
    dp_arr[2].time_stamp = 0;
    dp_arr[2].value.dp_value = update->bright;

    dp_arr[3].dpid = DPID_TEMPR; 
    dp_arr[3].type = PROP_VALUE;
    dp_arr[3].time_stamp = 0;
    dp_arr[3].value.dp_value = update->temper;

    dp_arr[4].dpid = DPID_COLOR; 
    dp_arr[4].type = PROP_STR;
    dp_arr[4].time_stamp = 0;
    dp_arr[4].value.dp_str = update->color_origin.color_str;

    dp_arr[5].dpid = DPID_SCENE; 
    dp_arr[5].type = PROP_STR;
    dp_arr[5].time_stamp = 0;
    dp_arr[5].value.dp_str = update->scene;

    /* report DP */
    ret = dev_report_dp_json_async(NULL ,dp_arr, dp_cnt);
    if(OPRT_OK != ret) {
        PR_ERR("dev_report_dp_json_async relay_config data error,err_num", ret);
    }

    /* free requested memory space */
    Free(dp_arr);
    dp_arr = NULL;

    Free(update);
    return;
}


/**
 * @brief: 灯的dp控制程序回调函数
 * @param: IN CONST TY_RECV_OBJ_DP_S *dp -> dp控制数据
 * @retval: none
 */
STATIC VOID _device_callback(IN CONST TY_RECV_OBJ_DP_S *dp)
{
    OPERATE_LIGHT ret = -1;
    BOOL_T actice_flag = FALSE;
    BOOL_T update_flag = FALSE;
    UCHAR_T i = 0;

    if (NULL == dp) {
        PR_ERR("dp error");
        return;
    }

    UCHAR_T cnt = dp->dps_cnt; //dp数
    PR_DEBUG("dp_cnt:%d", cnt);

    for (i = 0; i < cnt; i++) {
        //dp控制程序
        actice_flag = _light_dp_process(&(dp->dps[i]));
        if (actice_flag == TRUE) {
            update_flag = TRUE;
        }
    }

    if (update_flag == TRUE) {

        //灯控制程序
        ret = tuya_light_ctrl_process();
        if (ret != OPRT_OK) {
            PR_ERR("ctrl proc deal error!");
        }

        //灯控制数据延迟5s保存
        ret = tuya_light_ctrl_data_auto_save_start(5000);
        if (ret != OPRT_OK) {
            PR_ERR("Save Dp data error!");
        }
        actice_flag = FALSE;
    }

    return;
}

/**
 * @brief: gw 状态回调
 * @param: none
 * @retval: none
 */
STATIC VOID _gw_status_callback(IN CONST GW_STATUS_E status)
{
    if (status == GW_RESET) {
        PR_DEBUG("APP remove status");
        sg_can_blink_flag = FALSE;

        //tuya_hal_system_getheapsize用于获取堆大小/剩余内存大小
        PR_NOTICE("gw reset status heap stack %d", tuya_hal_system_getheapsize());
    }

    //tuya_hal_system_getheapsize用于获取堆大小/剩余内存大小
    PR_NOTICE("gw status heap stack %d", tuya_hal_system_getheapsize());

    return;
}

/**
 * @brief: 重置程序回调
 * @param: GW_RESET_TYPE_E type -> 重置的原因
 * @retval: none
 */
STATIC VOID _reset_callback(GW_RESET_TYPE_E type)
{
    //tuya_hal_system_getheapsize用于获取堆大小/剩余内存大小
    PR_NOTICE("gw reset cb heap stack %d", tuya_hal_system_getheapsize());

    //attention: 重启之前，需要保存到flash中
    switch (type) {
        case GW_LOCAL_RESET_FACTORY:
        case GW_LOCAL_UNACTIVE:
            tuya_light_ctrl_data_reset();    //本地重置，只需重置控制数据
            break;

        case GW_REMOTE_UNACTIVE:
        case GW_REMOTE_RESET_FACTORY:
            tuya_light_ctrl_data_reset();
            tuya_light_ctrl_data_auto_save();
            break;

        default:
            break;
    }

    return;
}

/**
 * @brief: query dp process
 * @param: none
 * @retval: none
 */
STATIC VOID _query_callback(IN CONST TY_DP_QUERY_S *dp_qry)
{
    //上传所有的dp点
    _light_all_dp_upload();

    return;
}

/**
 * @brief: wifi smart 模式初始化
 * @param：IN CHAR_T *sw_ver -> bin version
 * @return: none
 * @retval: OPERATE_LIGHT 0表示成功，非0请参照tuya error code描述文档 
 */
STATIC OPERATE_LIGHT _light_sys_smart_frame_init(IN CHAR_T *sw_ver)
{
    OPERATE_LIGHT ret = -1;
    UCHAR_T connect_mode = 0;

    //WIFI回调函数集
    TY_IOT_CBS_S wifi_callback = {
        .gw_status_cb = _gw_status_callback,
        .gw_ug_cb = NULL,
        .gw_reset_cb = _reset_callback,
        .dev_obj_dp_cb = _device_callback,
        .dev_raw_dp_cb = NULL,
        .dev_dp_query_cb = _query_callback,
        .dev_ug_cb = NULL,
        .dev_reset_cb = NULL,
    #if defined(TUYA_GW_OPERATOR) && (TUYA_GW_OPERATOR==1)
        .ope_get_chcode_cb = NULL,
    #endif
    };

    PR_NOTICE("frame goto init!");

    //获取连接模式
    connect_mode = tuya_light_ctrl_get_connect_mode();

    //wifi smart fram init
    ret = tuya_iot_wf_soc_dev_init_param(connect_mode, WF_START_SMART_FIRST, &wifi_callback, NULL, PRODUCT_KEY, sw_ver);
    if (OPRT_OK != ret) {
        PR_ERR("tuya_iot_wf_soc_dev_init err:%02x", ret);
        return ret;
    }

    PR_NOTICE("frame init out!");

    //注册 wifi 状态回调函数
    //指示灯连接闪烁显示
    ret = tuya_iot_reg_get_wf_nw_stat_cb(_wifi_status_display_callback);
    if (OPRT_OK != ret) {
        PR_ERR("tuya_iot_reg_get_wf_nw_stat_cb err:%02x", ret);
        return ret;
    }

    PR_NOTICE("frame init ok!");

    return OPRT_OK;
}

/**
 * @brief: 灯fast boot 程序 被 early init event 调用
 * @param: none
 * @retval: none
 */
VOID tuya_light_fast_boot(VOID)
{
    OPERATE_LIGHT ret = -1;
    
    SetLogManageAttr(TY_LOG_LEVEL_DEBUG);
    PR_NOTICE("go to pre device!!!!!!!!!!!!!!!!!!!!!!!!!");

    //灯初始化
    ret = tuya_light_init();
    if (ret != LIGHT_OK) {
        PR_NOTICE("Fast boot error!");
        return;
    }

    sg_fastboot_init = TRUE;     //fastboot init 成功

    PR_NOTICE("goto first bright up!");

    //灯的控制程序
    ret = tuya_light_ctrl_process();
    if (ret != LIGHT_OK) {
        PR_ERR("Pre device init set RGBCW error!");
        return;
    }

    return;
}


/**
 * @brief: wifi fast初始化程序
 * @param：none 
 * @attention: 此部分无法操作kv flash等wifi系统服务
 * @attention: 被 pre_device_init() 调用
 * @retval: none
 */
VOID tuya_light_sys_pre_device_init(VOID)
{
    //灯硬件控制重启
    // attention: to make sure light up in 500ms!
    // write recnt count! reload ctrl data!
    // write cnt into flash will take 220ms time
    tuya_light_ctrl_hardware_reboot_process();                       
    
    if (TRUE == sg_fastboot_init) {   
        //设置应用前初始化以判断是否未重置
        ty_subscribe_event(EVENT_SDK_DB_INIT_OK, "reset_cnt", tuya_light_ctrl_reset_cnt_process, FALSE);    
    }

    tuya_cpu_set_lp_mode(TRUE);    //设置低功耗模式  

    tuya_wifi_lp_disable();     //不使能wifi低功耗模式

    return;
}

/**
 * @brief: 应用常规初始化
 * @param: none
 * @attention: 被tuya_app_init()调用
 * @retval: none
 */
VOID tuya_light_sys_app_init(VOID)
{
    OPERATE_LIGHT ret = -1;
    UINT_T len = 0;
    UCHAR_T *p_config = NULL;
    UCHAR_T connect_mode = 0;
    light_prod_test_data_flash_t prod_result;    //产测结果

    PR_NOTICE("%s", tuya_iot_get_sdk_info());	//sdk输出信息

    PR_NOTICE("%s:%s", APP_BIN_NAME, USER_SW_VER);    //output bin information

    if (sg_fastboot_init != TRUE) {    
        // fast初始化失败, 需要从kvs读oem cfg

        ret = wd_user_param_read(&p_config, &len);
        if (ret != LIGHT_OK) {
            PR_ERR("read kvs oem cfg error!");
            if (LIGHT_NOT_FOUND == ret) {
                if (p_config != NULL) {
                    Free(p_config);
                    p_config = NULL;
                }

                //设备配置
                ret = tuya_device_cfg_set_string(strlen(DEFAULT_CONFIG), DEFAULT_CONFIG);
                if (ret != LIGHT_OK) {
                    PR_ERR("Default oem config error!");
                }

                len = strlen(DEFAULT_CONFIG);
                if (len > 1024) {
                    PR_ERR("default oem cfg too large!");
                }
                PR_DEBUG("default oem config len %d", strlen(DEFAULT_CONFIG));

                p_config = (CHAR_T *)Malloc(1024);
                if (p_config != NULL) {
                    memcpy(p_config, DEFAULT_CONFIG, strlen(DEFAULT_CONFIG));
                }
                /* load default oem cfg!!!! */
            }
            else {
                return;
            }
        }

        PR_DEBUG("kv oem cfg %s", p_config);

        //保存灯的应用信息
        ret = tuya_user_write_oem_cfg_data(len, p_config);
        Free(p_config);
        p_config = NULL;
        if (ret != LIGHT_OK) {
            PR_ERR("move kvs oem cfg to uf_file error!");
            return;
        }

        PR_NOTICE("oem cfg move to uf file ok!");

        //设备配置项装载处理，配置文件可以通过配置文件获取或者是DEFAULT_CONFIG宏定义设置
        ret = tuya_device_cfg_data_load();
        if (ret != LIGHT_OK) {
            PR_ERR("oem cfg load error!");
            return;
        }

        //灯初始化
        ret = tuya_light_init();
        if (ret != LIGHT_OK) {
            PR_ERR("Light init error again!");
            return;
        }

        //灯的控制程序
        ret = tuya_light_ctrl_process();
        if (ret != LIGHT_OK) {
            PR_ERR("Pre device init set RGBCW error!");
            return;
        }
    }

#ifdef _IS_OEM
    tuya_iot_oem_set(TRUE);
#endif

    //产测初始化
    ret = tuya_light_prod_init();
    if (ret != LIGHT_OK) {
        PR_ERR("Prod init error!");
        return;
    }

    //获取wifi连接模式
    connect_mode = tuya_light_ctrl_get_connect_mode();

    PR_NOTICE("connect mode is %d", connect_mode);

    //set spcl or spcl_auto connect pairing time
    tuya_iot_wf_timeout_set(tuya_light_ctrl_get_connect_time() * 60);      

    //注册产测工作模式和回调函数
    app_cfg_set(connect_mode, tuya_prod_test_callback);                  

    //读产测数据
    ret = tuya_user_flash_read_prod_data(&prod_result);
    if (ret != LIGHT_OK) {
        return;
    }

    if (PROD_REPEAT == prod_result.test_mode) {
        PR_DEBUG("Prod test ssid chang to test2!");

        //应用设置产测默认的ssid
        set_prod_ssid("tuya_mdev_test2");
    }

    return;
}

/**
 * @brief: 设备初始化
 * @param: none
 * @attention: 被device_init()调用
 * @retval: OPERATE_LIGHT 0表示成功，非0请参照tuya error code描述文档 
 */
OPERATE_LIGHT tuya_light_sys_device_init(VOID)
{
    OPERATE_LIGHT ret = -1;

    //wifi smart 模式初始化
    ret = _light_sys_smart_frame_init(USER_SW_VER);
    if (ret != OPRT_OK) {
        PR_ERR("smart frame init error");
    }

    return ret;
}

/**
 * @brief: 授权时擦出用户数据
 * @param: none
 * @attention: none
 * @retval: none
 */
VOID tuya_light_sys_erase_flash_data(VOID)
{
    OPERATE_LIGHT ret = -1;

    //擦出用户数据
    ret = tuya_user_flash_data_erase();
    if (ret != OPRT_OK) {
        PR_ERR("Erase user flash error!");
    }
}

/**
 * @brief: 转到mf测试时停止并清除计时器
 * @param: none
 * @attention: none
 * @retval: OPERATE_LIGHT 0表示成功，非0请参照tuya error code描述文档 
 */
OPERATE_LIGHT tuya_light_sys_mf_user_enter_callback(VOID)
{   
    
    OPERATE_LIGHT ret = -1;

    //停止软件定时器
    ret = tuya_user_software_timer_stop(CLEAR_RESET_CNT_SW_TIMER);
    if (ret != OPRT_OK) {
        PR_ERR("software time stop error!");
    }
    return ret;
}

