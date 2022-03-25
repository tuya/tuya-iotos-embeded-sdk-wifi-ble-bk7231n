/**
* @file soc_timer.c
* @author www.tuya.com
* @brief soc timer proc
* @version 0.2
* @date 2022-03-25
*
* @copyright Copyright (c) tuya.inc 2022
*
*/

#include "uni_time_queue.h"
#include "light_types.h"
#include "light_printf.h"
#include "tuya_timer.h"
#include "tuya_driver.h"
#include "sys_timer.h"
#define SOFTWARE_TIMER_MAX      20


typedef struct {
    TIMER_ID id;    //实际的timer id
    BOOL_T flag;      //该id是否注册
}timer_item_t;

//timer item 数组
STATIC timer_item_t sg_timers[SOFTWARE_TIMER_MAX];

STATIC BOOL_T sg_timers_flag = FALSE;    //sg_timers是否初始化标志

STATIC tuya_timer_t *sg_hw_timer;

/**
 * @brief: SOC 开启硬件定时器
 * @param[in]: cycle_us 循环定时时间, 单位us
 * @param[in]: callback 定时回调函数指针
 * @retval: OPERATE_LIGHT 0表示成功，非0请参照tuya error code描述文档 
 */
OPERATE_LIGHT tuya_soc_hardware_timer_start(IN UINT_T cycle_us, IN VOID* callback)
{
    INT_T ret = 0;
    sg_hw_timer = tuya_driver_find(TUYA_DRV_TIMER, 0);
    if (sg_hw_timer == NULL) {
        PR_ERR("HW timer0 find error");
        return OPRT_COM_ERROR;
    }

    TUYA_TIMER_CFG(sg_hw_timer, TUYA_TIMER_MODE_PERIOD, callback, NULL);  

    ret = tuya_timer_start(sg_hw_timer, cycle_us);
    if (ret != OPRT_OK) {
        PR_ERR("HW timer start error");
        return OPRT_COM_ERROR;
    }

    return OPRT_OK;
}

/**
 * @brief: SOC 停止硬件定时器
 * @param: none 
 * @retval: OPERATE_LIGHT 0表示成功，非0请参照tuya error code描述文档 
 */
OPERATE_LIGHT tuya_soc_hardware_timer_stop(VOID)
{   
    INT_T ret = 0;

    ret = tuya_timer_stop(sg_hw_timer);
    if (ret != OPRT_OK) {
        PR_ERR("HW timer stop error");
        return OPRT_COM_ERROR;
    }

    return OPRT_OK;
}

/**
 * @berief: SOC 开启软件定时器
 * @param[in]: timer_id 软件timer ID号
 * @param[in]: time_ms 定时的时间, 单位ms 
 * @param[in]: callback 定时回调函数指针
 * @note: 此函数为软件定时器的适配实现，可以依照现有资源进行实现，
 *        如使用RTOS的软件timer，或者涂鸦SDK中提供的软件timer实现
 * @return: OPERATE_LIGHT 0表示成功，非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_soc_software_timer_start(IN UCHAR_T timer_id, IN UINT_T cycle_ms, IN VOID* callback)
{
    OPERATE_LIGHT ret = 0;

    if (!sg_timers_flag) {
        //sg_timers初始化
        memset(sg_timers, 0, sizeof(sg_timers));
        sg_timers_flag = TRUE;
    }

    if (timer_id >= SOFTWARE_TIMER_MAX) {
        PR_ERR("Software timer is overflow,max timer count is %d", SOFTWARE_TIMER_MAX);
        return OPRT_INVALID_PARM;
    }


    if (sg_timers[timer_id].flag == FALSE) {
        //创建一个新的软件定时器

        ret = sys_add_timer((P_TIMER_FUNC)callback, NULL, (TIMER_ID *)&sg_timers[timer_id].id);
        if (ret != OPRT_OK) {
            PR_ERR("Create software time %d error", sg_timers[timer_id].id);
            return OPRT_COM_ERROR;
        } 

        sg_timers[timer_id].flag = TRUE;
    

        ret = sys_start_timer((TIMER_ID)sg_timers[timer_id].id, (TIME_MS)(cycle_ms), TIMER_ONCE);
        if (ret != OPRT_OK) {
            PR_ERR("Start software timer %d error : %d", sg_timers[timer_id].id, ret);
            return OPRT_COM_ERROR;
        }
    
    } else {
        //改变已有的软件定时器的周期

        ret = sys_start_timer((TIMER_ID)sg_timers[timer_id].id, (TIME_MS)(cycle_ms), TIMER_ONCE);
        if (ret != OPRT_OK) {
            PR_ERR("Start software timer %d error", sg_timers[timer_id].id);
            return OPRT_COM_ERROR;
        }
    
    }
    return OPRT_OK;
}

/**
 * @brief: SOC 停止软件定时器
 * @param[in]: timer_id  软件timer ID号
 * @retval: OPERATE_LIGHT 0表示成功，非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_soc_software_timer_stop(IN UCHAR_T timer_id)
{
    OPERATE_LIGHT ret = 0;

    if (timer_id >= SOFTWARE_TIMER_MAX) {
        PR_ERR("Software timer is overflow,max timer count is %d", SOFTWARE_TIMER_MAX);
        return OPRT_INVALID_PARM;
    }

    if (sg_timers[timer_id].flag == FALSE) {

        return OPRT_OK;
    }


    if (IsThisSysTimerRun((TIMER_ID)sg_timers[timer_id].id) != OPRT_OK) { //?
        //删除该定时器
        ret = sys_delete_timer((TIMER_ID)sg_timers[timer_id].id);
        if (ret != OPRT_OK) {
            PR_ERR("Software timer %d delte error!", sg_timers[timer_id].id);
            return OPRT_COM_ERROR;
        }
        sg_timers[timer_id].flag = FALSE;
    } else {
        //停止该定时器

        ret = sys_stop_timer((TIMER_ID)sg_timers[timer_id].id);
        if (ret != OPRT_OK) {
            PR_ERR("Software timer %d stop error!", sg_timers[timer_id].id);
            return OPRT_COM_ERROR;
        }

    }

    return OPRT_OK;
}

/**
 * @brief: 软件定时器是否有效（正在计时或者正在生效）判断适配模块
 * @param[in]: timer_id 软件timer ID号
 * @return: TURE-> 软件定时器有效, FALSE-> 当前无效
 */
BOOL_T tuya_soc_software_timer_active_check(IN UCHAR_T timer_id)
{
    if (!sg_timers_flag) {
        //sg_timers初始化
        memset(sg_timers, 0, sizeof(sg_timers));
        sg_timers_flag = TRUE;
    }

    return sg_timers[timer_id].flag;
}




