/**
 * @file tuya_hal_ota.h
 * @brief ota底层操作接口
 * 
 * @copyright Copyright (c) {2018-2020} 涂鸦科技 www.tuya.com
 * 
 */
#ifndef _TUYA_HAL_OTA_H_
#define _TUYA_HAL_OTA_H_
#include <stdint.h>
#include <stdbool.h>
#include "tuya_os_adapter.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 升级开始通知函数
 * 
 * @param file_size 升级固件大小
 * @param type 升级类型，串口升级还是OTA
 * 
 * @retval  =0      成功
 * @retval  <0      错误码
 */
int tuya_hal_ota_start_inform(unsigned int file_size, OTA_TYPE type);

/**
 * @brief ota数据包处理
 * 
 * @param total_len ota升级包总大小
 * @param offset 当前data在升级包中的偏移
 * @param data ota数据buffer指针
 * @param len ota数据buffer长度
 * @param remain_len [out] 内部已经下发但该函数还未处理的数据长度
 * @param pri_data 保留参数
 *
 * @retval  =0      成功
 * @retval  <0      错误码
 */
int tuya_hal_ota_data_process(const unsigned int total_len, const unsigned int offset,
                              const unsigned char* data, const unsigned int len, unsigned int* remain_len, void* pri_data);

/**
 * @brief 固件ota数据传输完毕通知
 *        用户可以做固件校验以及设备重启
 * param[in]        reset       是否需要重启
 * @retval  =0      成功
 * @retval  <0      错误码
 */
int tuya_hal_ota_end_inform(bool reset);
VOID_T tuya_os_ota_intf_init(VOID_T);

#ifdef __cplusplus
}
#endif

#endif
