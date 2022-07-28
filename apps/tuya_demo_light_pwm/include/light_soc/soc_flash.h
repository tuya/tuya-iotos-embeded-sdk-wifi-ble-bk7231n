/**
* @file soc_flash.h
* @author www.tuya.com
* @brief soc flash proc
* @version 0.2
* @date 2022-03-25
*
* @copyright Copyright (c) tuya.inc 2022
*
*/

#ifndef __SOC_FLASH_H__
#define __SOC_FLASH_H__


#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#include "light_types.h"

/**
 * @brief soc flash save type enum
 */
typedef BYTE_T SOC_FLASH_SAVE_TYPE_E;
#define SAVE_TYP1       0       // reset cnt
#define SAVE_TYP2       1       // app data  
#define SAVE_TYP3       2       // prod test
#define SAVE_TYP4       3       //rhythm
#define SAVE_TYP5       4       //sleep
#define SAVE_TYP6       5       //wake
#define SAVE_TYP_MAX    6


/**
 * @brief: flash数据写入
 * @param: IN SOC_FLASH_SAVE_TYPE_E data_type -> 数据的种类(uf file文件名)
 * @param: IN UINT_T addr -> 数据的地址（可以自行根据数据存储方式来决定是否使用）
 * @param: IN UCHAR_T *data -> 数据指针
 * @param: IN USHORT_T len -> 要写入的数据长度
 * @note: 可以更加flash存储的方式自行决定使用data_type还是addr的方式
 *        addr的方式适合使用在flash裸写的方式中
 *        data_type使用在已经有uf file\kv等组件的情况下
 * @retval: OPERATE_LIGHT 0表示成功，非0请参照tuya error code描述文档 
 */
OPERATE_LIGHT tuya_soc_flash_write(IN SOC_FLASH_SAVE_TYPE_E data_type, IN UINT_T addr, IN UCHAR_T *data, IN USHORT_T len);


/**
 * @brief: soc flash 读取
 * @param: IN SOC_FLASH_SAVE_TYPE_E data_type -> 数据的种类（uf file文件名）
 * @param: IN UINT_T addr -> 数据的地址（可以自行根据数据存储方式来决定是否使用）
 * @param: IN USHORT_T Len -> 数据长度
 * @param: OUT UCHAR_T *data -> 数据
 * @note: 可以更加flash存储的方式自行决定使用data_type还是addr的方式
 *        addr的方式适合使用在flash裸写的方式中
 *        data_type使用在已经有uf file\kv等组件的情况下
 * @retval: 读取到的数据长度
 */
INT_T tuya_soc_flash_read(IN SOC_FLASH_SAVE_TYPE_E data_type, IN UINT_T addr, IN USHORT_T len, OUT UCHAR_T *data);

/**
 * @brief: oem配置文件写入适配
 * @param: IN USHORT_T len -> write数据长度
 * @param: IN UCHAR_T *data -> write数据
 * @return: OPERATE_LIGHT 0表示成功，非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_soc_cfg_write(IN USHORT_T len, IN UCHAR_T *data);


/**
 * @brief: oem配置文件读取适配
 * @para: OUT USHORT_T *len -> 读取数据的长度
 * @param: OUT UCHAR_T *data -> 存放数据
 * @retval: OPERATE_LIGHT
 */
OPERATE_LIGHT tuya_soc_cfg_read(OUT USHORT_T *len, OUT UCHAR_T *data);

/**
 * @brief: 擦除特定flash文件适配
 * @param: IN SOC_FLASH_SAVE_TYPE_E data_type -> 数据的种类（uf file文件名）
 * @param: IN UINT_T addr -> 数据的地址（可以自行根据数据存储方式来决定是否使用
 * @retval: OPERATE_LIGHT 0表示成功，非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_soc_flash_earse_special(IN SOC_FLASH_SAVE_TYPE_E data_type, IN UINT_T addr);

/**
 * @brief: 擦除所有flash文件适配
 * @param: none
 * @retval: OPERATE_LIGHT 0表示成功，非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_soc_flash_earse(VOID);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif  /* __SOC_FLASH_H__ */
