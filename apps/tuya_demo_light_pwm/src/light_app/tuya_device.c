/**
 * @file: tuya_device.c
 * @author: wuls@tuya.com
 * @brief: 设备初始化流程处理模块
 * @history: 
 *          2022-03-25 update reporte interface
 *          2020-12-07 style modify
 *          2019-05-22 create  
 * @Copyright (c) tuya.inc 2020
 */

#define _TUYA_DEVICE_GLOBAL
 
#include "tuya_device.h"
#include "light_system.h"
#include "base_event.h"
#include "base_event_info.h"
#include "gpio_test.h"

/**
 * @brief: 应用快速初始化程序
 * @param: none
 * @return: none
 */
VOID pre_app_init(VOID)
{
    //灯开速启动的事件
    ty_subscribe_event(EVENT_SDK_EARLY_INIT_OK, "early_init", tuya_light_fast_boot, FALSE);

    return;
}

/**
 * @brief: 设备初始化前置准备工作
 * @param: none
 * Note: to initialize device before device_init
 * Return: none
 */
VOID pre_device_init(VOID)
{
    //wifi fast初始化程序
    tuya_light_sys_pre_device_init();

    return;
}

/**
 * @brief: wifi常规初始化程序, 被user_main调用
 * @param: none
 * @retval: none
 */
VOID app_init(VOID)
{
    //wifi 常规初始化
    tuya_light_sys_app_init();

    return;
}

/**
 * @brief: gpio口测试
 * @param: none
 * @return: TURN or FALSE
 */
BOOL_T gpio_test(IN CONST CHAR_T *in, OUT CHAR_T *out)
{
    //gpio口测试 
    return gpio_test_all(in, out);
}

/**
 * @brief: 授权时擦出用户数据的回调函数, 被user_main调用
 * @param: none 
 * @return: none
 */
VOID mf_user_callback(VOID)
{
    //授权时擦出用户数据
    tuya_light_sys_erase_flash_data();

    return;
}

/**
 * @brief: 设备初始化, 被user_main调用
 * @param: none 
 * @return: none
 */
OPERATE_RET device_init(VOID)
{
    OPERATE_RET ret = OPRT_OK;

    //设备初始化
    ret = tuya_light_sys_device_init();
    if (ret != OPRT_OK) {
        return ret;
    }
    return OPRT_OK;
}

/**
 * @brief: 转到mf测试时停止并清除计时器, 被user_main调用
 * @param: none 
 * @return: none
 */
VOID mf_user_enter_callback(VOID)
{
    OPERATE_LIGHT ret = -1; 

    ret = tuya_light_sys_mf_user_enter_callback();
    
    return;
}

/**
 * @brief: gpio测试回调函数，可以关闭gpio测试或进行其他处理, 被user_main调用
 * @param: none
 * @return: none
 */
VOID mf_user_pre_gpio_test_cb(VOID)
{
    ;
}

/**
 * @brief: 产测回调函数, 被user_main调用
 * @param: none
 * @return: none
 */
OPERATE_RET mf_user_product_test_cb(USHORT_T cmd,UCHAR_T *data, UINT_T len, OUT UCHAR_T **ret_data,OUT USHORT_T *ret_len)
{
    return OPRT_OK;
}



