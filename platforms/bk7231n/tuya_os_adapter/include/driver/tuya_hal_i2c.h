/**
 * @file tuya_hal_i2c.h
 * @brief IIC总线操作接口
 * 
 * @copyright Copyright(C),2018-2020, 涂鸦科技 www.tuya.com
 * 
 */

#ifndef __TUYA_HAL_IIC_H__
#define __TUYA_HAL_IIC_H__


#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief 打开I2C设备
 * 
 * @param[in]   port    设备编号
 * 
 * @retval  =0      成功
 * @retval  <0      错误码
 */
int tuya_hal_i2c_open(int port);

/**
 * @brief 关闭I2C设备
 * 
 * @param[in]   port    设备编号
 * 
 * @retval  =0      成功
 * @retval  <0      错误码
 */
int tuya_hal_i2c_close(int port);

/**
 * @brief 读数据
 * 
 * @param[in]   port    设备编号
 * @param[in]   addr    从机地址
 * @param[in]   buf     读取的数据缓冲区地址
 * @param[in]   len     缓冲区长度
 * 
 * @retval  >=0     实际读取的字节数
 * @retval  <0      错误码
 */
int tuya_hal_i2c_read(int port, int addr, uint8_t* buf, int len);

/**
 * @brief 写数据
 * 
 * @param[in]   port    设备编号
 * @param[in]   addr    从机地址
 * @param[in]   buf     写入的数据缓冲区地址
 * @param[in]   len     缓冲区长度
 * 
 * @retval  >=0     实际写入的字节数
 * @retval  <0      错误码
 */
int tuya_hal_i2c_write(int port, int addr, uint8_t const* buf, int len);


#ifdef __cplusplus
} // extern "C"
#endif


#endif // __TUYA_HAL_IIC_H__

