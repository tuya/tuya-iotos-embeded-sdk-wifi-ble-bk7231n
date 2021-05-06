/**
 * @file tuya_os_adapt_storage.h
 * @brief STORAGE设备操作接口
 * 
 * @copyright Copyright(C),2018-2020, 涂鸦科技 www.tuya.com
 * 
 */

#ifndef __TUYA_OS_ADAPT_STORAGE_H__
#define __TUYA_OS_ADAPT_STORAGE_H__


#include <stdint.h>
#include "tuya_os_adapter.h"

#ifdef __cplusplus
	extern "C" {
#endif

/**
 * @brief read data from flash
 * 
 * @param[in]       addr        flash address
 * @param[out]      dst         pointer of buffer
 * @param[in]       size        size of buffer
 * @return int 0=成功，非0=失败
 */
int tuya_os_adapt_flash_read(const unsigned int addr, unsigned char *dst, const unsigned int size);


/**
 * @brief write data to flash
 * 
 * @param[in]       addr        flash address
 * @param[in]       src         pointer of buffer
 * @param[in]       size        size of buffer
 * @return int 0=成功，非0=失败
 */
int tuya_os_adapt_flash_write(const unsigned int addr, const unsigned char *src, const unsigned int size);


/**
 * @brief erase flash block
 * 
 * @param[in]       addr        flash block address
 * @param[in]       size        size of flash block
 * @return int 0=成功，非0=失败
 */
int tuya_os_adapt_flash_erase(const unsigned int addr, const unsigned int size);


/**
 * @brief get description of storage
 * 
 * @return  pointer to storage description structure
 */
UNI_STORAGE_DESC_S* tuya_os_adapt_storage_get_desc(void);

/**
 * @brief flash 设置保护,enable 设置ture为全保护，false为半保护
 * 
 * @return OPRT_OS_ADAPTER_OK 
 */
int tuya_os_adapt_flash_set_protect(const bool enable);

/**
 * @brief get UF file description
 * 
 * @return  pointer to descrtion of UF file
 */
UF_PARTITION_TABLE_S* tuya_os_adapt_uf_get_desc(void);

OPERATE_RET tuya_os_adapt_reg_storage_intf(void);



#ifdef __cplusplus
}
#endif

#endif // __TUYA_OS_ADAPT_STORAGE_H__

