/*
 * @Author: wls
 * @email: wuls@tuya.com
 * @LastEditors:   
 * @file name: soc_flash.c
 * @Description: soc flash proc
 * @Copyright: HANGZHOU TUYA INFORMATION TECHNOLOGY CO.,LTD
 * @Company: http://www.tuya.com
 * @Date: 2019-05-06 10:00:26
 * @LastEditTime: 2019-10-21 14:15:59
 */

#include "soc_flash.h"
#include "uf_file.h"
#include "light_types.h"
#include "light_printf.h"
#include "light_tools.h"
#include "soc_timer.h"

/**
 * @brief: wifi uf write(a+ mode)
 * @param: IN CHAR_T *file_name -> 文件名
 * @param: IN UCHAR_T *data -> 待写入的数据
 * @param: IN USHORT_T len -> 写入数据的长度
 * @retval: OPERATE_LIGHT 0表示成功，非0请参照tuya error code描述文档 
 */
STATIC OPERATE_LIGHT _soc_flash_file_write(IN CHAR_T *file_name, IN UCHAR_T *data, IN USHORT_T len)
{
    OPERATE_LIGHT ret = -1;
    uFILE * fp = NULL; 
    UINT_T off_set = 0;
    UINT_T write_cnt = 0;

    //打开filename指向的文件
    fp = ufopen(file_name, "a+");
    if (NULL == fp) {
        PR_ERR("uf file %s can't open and write data!", file_name);
        return LIGHT_COM_ERROR;
    }

    //重定位光标
    off_set = ufseek(fp, 0, UF_SEEK_SET);
    if (off_set != 0) {
        PR_ERR("uf file %s Set file offset to 0 error!", file_name);
        return LIGHT_COM_ERROR;
    }

    //将data指向的数据写入fp代表的文件中
    write_cnt = ufwrite(fp, data, len);
    if (write_cnt != len) {
        PR_ERR("uf file %s write data error!", file_name);
        return LIGHT_COM_ERROR;
    }

    //关闭文件
    ret = ufclose(fp);
    if (ret != OPRT_OK) {
        PR_ERR("uf file %s close error!", file_name);
        return ret;
    }

    return OPRT_OK;
}

/**
 * @brief: wifi uf read
 * @param: IN CHAR_T *file_name -> 文件名
 * @param: IN UCHAR_T *data -> 存放读入的数据
 * @param: IN USHORT_T len -> 要读的个数
 * @retval: 读取数据的个数
 */
STATIC INT_T _soc_flash_file_read(IN CHAR_T *file_name, IN USHORT_T len, OUT UCHAR_T *data)
{
    OPERATE_LIGHT ret = -1;
    uFILE * fp = NULL;
    INT_T read_cnt = 0;

    //打开filename指向的文件
    fp = ufopen(file_name, "r+");
    if (NULL == fp) {
        PR_ERR("uf file %s can't open and read data!", file_name);
        return LIGHT_COM_ERROR;
    }

    PR_DEBUG("uf open OK");

    //读取数据
    read_cnt = ufread(fp, data, len);
    PR_DEBUG("uf file %s read data %d!", file_name, read_cnt);

    //关闭文件
    ret = ufclose(fp);
    if (ret != OPRT_OK) {
        PR_ERR("uf file %s close error!", file_name);
        return ret;
    }
    
    return read_cnt;
}

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
OPERATE_LIGHT tuya_soc_flash_write(IN SOC_FLASH_SAVE_TYPE_E data_type, IN UINT_T addr, IN UCHAR_T *data, IN USHORT_T len)
{
    OPERATE_LIGHT ret = -1;
    CHAR_T temp[4] = {0};
    
    if (data_type >= SAVE_TYP_MAX) {
        PR_ERR("Write soc flash type error!");
        return OPRT_INVALID_PARM;
    }

    //data_type保存到temp中
    tuya_num_to_str(0, data_type, 4, temp);

    //将data指向的数据写入flash中
    ret = _soc_flash_file_write(temp, data, len);
    if (ret != OPRT_OK) {
        return ret;
    }
    
    return OPRT_OK;
}

/**
 * @brief: soc flash 读取
 * @param: IN SOC_FLASH_SAVE_TYPE_E data_type -> 数据的种类（uf file文件名）
 * @param: IN UINT_T addr -> 数据的地址（可以自行根据数据存储方式来决定是否使用）
 * @param: IN USHORT_T Len -> 数据长度
 * @param: OUT UCHAR_T *data -> 数据
 * @note: 可以更加flash存储的方式自行决定使用data_type还是addr的方式
 *        addr的方式适合使用在flash裸写的方式中
 *        data_type使用在已经有uf file\kv等组件的情况下
 * @return: 读取到的数据长度
 */
INT_T tuya_soc_flash_read(IN SOC_FLASH_SAVE_TYPE_E data_type, IN UINT_T addr, IN USHORT_T len, OUT UCHAR_T *data)
{
    INT_T read_cnt = 0;
    CHAR_T temp[4] = {0};

    if (data_type >= SAVE_TYP_MAX) {
        PR_ERR("Read soc flash type error!");
        return OPRT_INVALID_PARM;
    }
    
    tuya_num_to_str(0, data_type, 4, temp);
    PR_NOTICE("file name %s",temp);
    read_cnt = _soc_flash_file_read(temp, len, data);

    return read_cnt;
}

/**
 * @brief: oem配置文件写入适配
 * @param: IN USHORT_T len -> write数据长度
 * @param: IN UCHAR_T *data -> write数据
 * @return: OPERATE_LIGHT 0表示成功，非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_soc_cfg_write(IN USHORT_T len, IN UCHAR_T *data)
{
    OPERATE_LIGHT ret = -1;
    uFILE * fp = NULL;
    UINT_T write_cnt = 0;
   
    fp = ufopen("oem_cfg", "w+");
    if (NULL == fp) {
        PR_ERR("oem cfg uf file can't open and write data!");
        return LIGHT_COM_ERROR;
    }

    write_cnt = ufwrite(fp, data, len);
    if (write_cnt != len) {
        PR_ERR("oem cfg uf file write data error!");
        return LIGHT_COM_ERROR;
    }

    ret = ufclose(fp);
    if (ret != OPRT_OK) {
        PR_ERR("oem cfg uf file close error!");
        return ret;
    }
    PR_NOTICE("save oem cfg into uf ok");
    return OPRT_OK;
}

/**
 * @brief: oem配置文件读取适配
 * @para: OUT USHORT_T *len -> 读取数据的长度
 * @param: OUT UCHAR_T *data -> 存放数据
 * @retval: OPERATE_LIGHT 0表示成功，非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_soc_cfg_read(OUT USHORT_T *len, OUT UCHAR_T *data)
{
    OPERATE_LIGHT ret = -1;
    uFILE * fp = NULL;
    UINT_T read_cnt = 0;
    
    fp = ufopen("oem_cfg", "r+");
    if (NULL == fp) {
        PR_ERR("uf file can't open and read data!");
        return LIGHT_COM_ERROR;
    }

    read_cnt = ufread(fp, data, 1024);
    PR_DEBUG("oem cfg uf file read data %d!", read_cnt);
    PR_DEBUG("oem cfg uf file %s", data);
    *len = read_cnt;
    
    ret = ufclose(fp);
    if (ret != OPRT_OK) {
        PR_ERR("oem cfg uf file close error!");
        return ret;
    }

    PR_DEBUG("oem read OK");
    return OPRT_OK;
}

/**
 * @brief: 擦除特定flash文件适配
 * @param: IN SOC_FLASH_SAVE_TYPE_E data_type -> 数据的种类（uf file文件名）
 * @param: IN UINT_T addr -> 数据的地址（可以自行根据数据存储方式来决定是否使用
 * @retval: OPERATE_LIGHT 0表示成功，非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_soc_flash_earse_special(IN SOC_FLASH_SAVE_TYPE_E data_type, IN UINT_T addr)
{
    OPERATE_LIGHT ret = 0;
    CHAR_T temp[4] = {0};

    tuya_num_to_str(0, data_type, 4, temp);
    
    if (!ufexist(temp)) {
        return OPRT_OK;
    }
    
    ret = ufdelete(temp);
    if (ret != OPRT_OK) {
        PR_ERR("Delete %s file error!", temp);
        return ret;
    }

    return OPRT_OK;
}

/**
 * @brief: 擦除所有flash文件适配
 * @param: none
 * @retval: OPERATE_LIGHT 0表示成功，非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_soc_flash_earse(VOID)
{
    OPERATE_LIGHT ret = -1;
    UCHAR_T i = 0;
    UCHAR_T earse_cnt = 0;  //擦除次数

    for (i = 0; i < SAVE_TYP_MAX; i++) {
        ret = tuya_soc_flash_earse_special(i, 0);
        if (OPRT_OK == ret) {
            earse_cnt++;
        }
    }
    
    if (ufexist("oem_cfg")) {
        ret = ufdelete("oem_cfg");
        if (ret != OPRT_OK) {
            PR_ERR("Delete oem_cfg file error!");
            return OPRT_COM_ERROR;
        }
    }

    if (earse_cnt < SAVE_TYP_MAX) {
        return OPRT_COM_ERROR;
    }
    
    return OPRT_OK;
    
}


