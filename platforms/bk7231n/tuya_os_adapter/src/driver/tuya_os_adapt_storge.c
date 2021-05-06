/**
 * @file tuya_os_adapt_storage.c
 * @brief STORAGE设备操作接口
 * 
 * @copyright Copyright(C),2018-2020, 涂鸦科技 www.tuya.com
 * 
 */
#include "tuya_os_adapt_storge.h"
#include "tuya_os_adapter_error_code.h"

#include "drv_model_pub.h"
#include "flash_pub.h"


/***********************************************************
*************************micro define***********************
***********************************************************/
#define PARTITION_SIZE (1 << 12) /* 4KB */
#define FLH_BLOCK_SZ PARTITION_SIZE

// flash map 
#define SIMPLE_FLASH_START (0x200000 - 0x3000 - 0xE000)
#define SIMPLE_FLASH_SIZE 0xE000 // 56k

#define SIMPLE_FLASH_SWAP_START (0x200000 - 0x3000)
#define SIMPLE_FLASH_SWAP_SIZE 0x3000 // 12k

#define SIMPLE_FLASH_KEY_ADDR  (0x200000 - 0x3000 - 0xE000 - 0x1000)            //4k

#define UF_PARTITION_START     (0x200000 - 0x3000 - 0xE000 - 0x1000) - 0x3000 - 0x1000 - 0x18000
#define UF_PARTITION_SIZE      0x18000                                          //96k


/***********************************************************
*************************variable define********************
***********************************************************/

static const TUYA_OS_STORAGE_INTF m_tuya_os_storage_intfs = {
    .read  = tuya_os_adapt_flash_read, 
    .write = tuya_os_adapt_flash_write,
    .erase = tuya_os_adapt_flash_erase,
    .get_storage_desc = tuya_os_adapt_storage_get_desc,
    .get_uf_desc = tuya_os_adapt_uf_get_desc
};

static UNI_STORAGE_DESC_S storage = {
    SIMPLE_FLASH_START,
    SIMPLE_FLASH_SIZE,
    FLH_BLOCK_SZ,
    SIMPLE_FLASH_SWAP_START,
    SIMPLE_FLASH_SWAP_SIZE,
    SIMPLE_FLASH_KEY_ADDR
};

static UF_PARTITION_TABLE_S uf_file = {
    .sector_size = PARTITION_SIZE,
    .uf_partition_num = 1,
    .uf_partition = {
        {UF_PARTITION_START, UF_PARTITION_SIZE},                        //96K
    }
};


/***********************************************************
*************************function define********************
***********************************************************/
extern int hal_flash_lock(void);
extern int hal_flash_unlock(void);
/**
 * @brief read data from flash
 * 
 * @param[in]       addr        flash address
 * @param[out]      dst         pointer of buffer
 * @param[in]       size        size of buffer
 * @return int 0=成功，非0=失败
 */
int tuya_os_adapt_flash_read(const unsigned int addr, unsigned char *dst, const unsigned int size)
{
    unsigned int status;
    
    if(NULL == dst) {
        return OPRT_OS_ADAPTER_INVALID_PARM;
    }
    
    hal_flash_lock();

    DD_HANDLE flash_handle;
    flash_handle = ddev_open(FLASH_DEV_NAME, &status, 0);
    ddev_read(flash_handle, (char *)dst, size, addr);
    ddev_close(flash_handle);
    
    hal_flash_unlock();

    return OPRT_OS_ADAPTER_OK;
}

static unsigned int __uni_flash_is_protect_all(void)
{
    DD_HANDLE flash_handle;
    unsigned int status;
    unsigned int param;

    flash_handle = ddev_open(FLASH_DEV_NAME, &status, 0);
    ddev_control(flash_handle, CMD_FLASH_GET_PROTECT, (void *)&param);
    ddev_close(flash_handle);

    return (FLASH_PROTECT_ALL == param);
}

/**
 * @brief write data to flash
 * 
 * @param[in]       addr        flash address
 * @param[in]       src         pointer of buffer
 * @param[in]       size        size of buffer
 * @return int 0=成功，非0=失败
 */
int tuya_os_adapt_flash_write(const unsigned int addr, const unsigned char *src, const unsigned int size)
{
    DD_HANDLE flash_handle;
    unsigned int protect_flag;
    unsigned int status;
    unsigned int param;

    if(NULL == src) 
    {
        return OPRT_INVALID_PARM;
    }

    hal_flash_lock();

    protect_flag = __uni_flash_is_protect_all();
    flash_handle = ddev_open(FLASH_DEV_NAME, &status, 0);

    if(protect_flag)
    {
        param = FLASH_PROTECT_HALF;
        ddev_control(flash_handle, CMD_FLASH_SET_PROTECT, (void *)&param);
    }
    
    ddev_write(flash_handle, (char *)src, size, addr);

    protect_flag = __uni_flash_is_protect_all();

    if(protect_flag)
    {
        param = FLASH_PROTECT_ALL;
        ddev_control(flash_handle, CMD_FLASH_SET_PROTECT, (void *)&param);
    }

    ddev_close(flash_handle);
    hal_flash_unlock();

    return OPRT_OS_ADAPTER_OK;
}

/**
 * @brief erase flash block
 * 
 * @param[in]       addr        flash block address
 * @param[in]       size        size of flash block
 * @return int 0=成功，非0=失败
 */
int tuya_os_adapt_flash_erase(const unsigned int addr, const unsigned int size)
{
    unsigned short start_sec = (addr/PARTITION_SIZE);
    unsigned short end_sec = ((addr+size-1)/PARTITION_SIZE);
    unsigned int status;
    unsigned int i = 0;
    unsigned int sector_addr;
    DD_HANDLE flash_handle;
    unsigned int  param;
    unsigned int protect_flag;

    hal_flash_lock();

    protect_flag = __uni_flash_is_protect_all();
    flash_handle = ddev_open(FLASH_DEV_NAME, &status, 0);

    if(protect_flag)
    {
        param = FLASH_PROTECT_HALF;
        ddev_control(flash_handle, CMD_FLASH_SET_PROTECT, (void *)&param);
    }

    for(i = start_sec;i <= end_sec;i++) {
        sector_addr = PARTITION_SIZE*i;
        ddev_control(flash_handle, CMD_FLASH_ERASE_SECTOR,(void*)(&sector_addr));
    }

    protect_flag = __uni_flash_is_protect_all();

    if(protect_flag)
    {
        param = FLASH_PROTECT_ALL;
        ddev_control(flash_handle, CMD_FLASH_SET_PROTECT, (void *)&param);
    }
    
    ddev_close(flash_handle);

    hal_flash_unlock();
     
    return OPRT_OS_ADAPTER_OK;
}

/**
 * @brief get UF file description
 * 
 * @return  pointer to descrtion of UF file
 */
UF_PARTITION_TABLE_S *tuya_os_adapt_uf_get_desc(void)
{
    return &uf_file;
}

/**
 * @brief get description of storage
 * 
 * @return  pointer to storage description structure
 */
UNI_STORAGE_DESC_S *tuya_os_adapt_storage_get_desc(void)
{
    return &storage;
}

/**
 * @brief flash 设置保护,enable 设置ture为全保护，false为半保护
 * 
 * @return OPRT_OS_ADAPTER_OK 
 */
int tuya_os_adapt_flash_set_protect(const bool enable)
{
    DD_HANDLE flash_handle;
    unsigned int  param;
    unsigned int status;

    flash_handle = ddev_open(FLASH_DEV_NAME, &status, 0);

    if(enable)
    {
        param = FLASH_PROTECT_ALL;
        ddev_control(flash_handle, CMD_FLASH_SET_PROTECT, (void *)&param);
    }
    else
    {
        param = FLASH_PROTECT_HALF;
        ddev_control(flash_handle, CMD_FLASH_SET_PROTECT, (void *)&param);
    }
    
    ddev_close(flash_handle);
    return OPRT_OS_ADAPTER_OK;
}

OPERATE_RET tuya_os_adapt_reg_storage_intf(void)
{
    return tuya_os_adapt_reg_intf(INTF_STORAGE, (void *)&m_tuya_os_storage_intfs);
}

