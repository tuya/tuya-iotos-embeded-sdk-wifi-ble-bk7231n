/**
 * @file tuya_hal_rtc.h
 * @brief RTC操作接口
 * 
 * @copyright Copyright(C),2018-2020, 涂鸦科技 www.tuya.com
 * 
 */

#ifndef __TUYA_HAL_RTC_H__
#define __TUYA_HAL_RTC_H__


#include <time.h>


#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief RTC 初始化
 * 
 */
void tuya_hal_rtc_init(void);

/**
 * @brief 设置 RTC 时间
 * 
 * @param[in]       write_time  写入的 RTC 时间值
 */
void tuya_hal_rtc_set_time(time_t write_time);

/**
 * @brief 获取 RTC 时间
 * 
 * @return  获取的 RTC 时间值
 */
time_t tuya_hal_rtc_get_time(void);


#ifdef __cplusplus
}
#endif


#endif // __TUYA_HAL_RTC_H__

