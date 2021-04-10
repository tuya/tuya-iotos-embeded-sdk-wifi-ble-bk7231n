/**
 * @file tuya_hal_memory.h
 * @author sunkz@tuya.com
 * @brief 内存操作接口封装
 * @version 0.1
 * @date 2020-05-9
 * 
 * @copyright Copyright (c) tuya.inc 2019
 * 
 */

#ifndef __TUYA_HAL_MEMORY_H__
#define __TUYA_HAL_MEMORY_H__

#include "tuya_os_adapter.h"
#include "tuya_cloud_types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief tuya_hal_system_malloc用于分配内存
 * 
 * @param[in]       size        需要分配的内存带下
 * @return  分配得到的内存指针
 */
VOID_T *tuya_hal_system_malloc(CONST SIZE_T size);

/**
 * @brief tuya_hal_system_free用于释放内存
 * 
 * @param[in]       ptr         需要释放的内存指针
 */
VOID_T tuya_hal_system_free(VOID_T* ptr);


/* add begin: by sunkz, interface regist */
VOID_T tuya_os_memory_intf_init(VOID_T);
/* add end */



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __TUYA_HAL_MEMORY_H__

