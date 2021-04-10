/**
 * @file: user_timer.h
 * @author: wuls@tuya.com
 * @brief: 软硬件定时实现模块
 * @history: 
 *          2020-12-07: style modify
 *          2019-05-22: create
 * @Copyright (c) tuya.inc 2020
 */


#include "user_timer.h"
#include "light_types.h"
#include "light_printf.h"

/**
 * @brief: 硬件定时器启动, 用户调用接口
 * @param[in] cycle_us 循环定时时间, 单位: us
 * @param[in] call_back 定时回调函数指针
 * @retval: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_user_hardware_timer_start(IN UINT_T cycle_us, IN VOID* call_back)
{
    OPERATE_LIGHT ret = -1;

    //必须要有定时回调函数
    if (NULL == call_back) {
        PR_ERR("user hardware time callback can't be null!");
        return LIGHT_INVALID_PARM;
    }

    //启动硬件定时器
    ret = tuya_soc_hardware_timer_start(cycle_us, call_back);
    if (ret != LIGHT_OK) {
        PR_ERR("User hardware start error!");
        return ret;
    }

    return LIGHT_OK;
}

/**
 * @brief: 硬件定时器停止, 用户调用接口
 * @param: none
 * @retval: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_user_hardware_timer_stop(VOID)
{
    OPERATE_LIGHT ret = -1;

    //停止硬件定时器
    ret = tuya_soc_hardware_timer_stop();

    return ret;
}

/**
 * @brief: 软件定时器启动, 用户调用接口
 * @param[in]: timer_id  软件定时器的ID
 * @param[in]: cyclye_ms 定时时间, 单位: ms
 * @param[in]: call_back 定时回调函数指针
 * @note: 
 *         此软件定时可以在任何情况情况下进行重载reload
 *         此软件定时仅仅实现了单次定时，如果需要循环执行，请再回调中再次start定时器id
 * @retval: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_user_software_timer_start(IN UCHAR_T timer_id, IN UINT_T cyclye_ms, IN VOID* call_back)
{
    OPERATE_LIGHT ret = -1;

    if (NULL == call_back) {
        PR_ERR("user software time callback can't be null!");
        return LIGHT_INVALID_PARM;
    }

    //启动软件定时器
    ret = tuya_soc_software_timer_start(timer_id, cyclye_ms, call_back);
    if (ret != LIGHT_OK) {
        PR_ERR("User software start error!");
        return ret;
    }

    return LIGHT_OK;
}

/**
 * @brief: 软件定时器停止, 用户调用接口
 * @param[in] timer_id 软件定时器ID
 * @return: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_user_software_timer_stop(IN UCHAR_T timer_id)
{
    OPERATE_LIGHT ret = -1;

    //软件定时器停止
    ret = tuya_soc_software_timer_stop(timer_id);
    if (ret != LIGHT_OK) {
        PR_ERR("User software stop error!");
        return ret;
    }

    return LIGHT_OK;
}

/**
 * @brief: 检查软件定时器是否有效(正在计时或者正在生效), 用户调用接口
 * @param[in]: timer_id 软件timer ID号
 * @return: TURE-> 软件定时器有效, FALSE-> 当前无效
 */
BOOL_T tuya_user_software_timer_active_check(IN UCHAR_T timer_id)
{
    return tuya_soc_software_timer_active_check(timer_id);
}


