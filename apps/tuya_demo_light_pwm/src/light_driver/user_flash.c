/**
 * @file: user_flash.c
 * @author: wuls@tuya.com
 * @brief: 设备数据存储、读取和删除处理
 * @history:  
 *          2020-12-07: style modify
 *          2019-05-22: create
 * @Copyright (c) tuya.inc 2020
 */


#include "soc_flash.h"
#include "user_flash.h"
#include "light_types.h"
#include "light_printf.h"

/**
 * @brief: 保存灯的上电次数
 * @param[in]: cnt 次数值  
 * @retval: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_user_flash_write_reset_cnt(IN UCHAR_T cnt)
{
    OPERATE_LIGHT ret = -1;

    //将上电次数写入flash中
    ret = tuya_soc_flash_write(SAVE_TYP1, RESET_CNT_OFFSET, &cnt, 1);
    if (ret != LIGHT_OK) {
        PR_ERR("Reset cnt write error %d!", ret);
    return LIGHT_COM_ERROR;
    }

    return LIGHT_OK;
}

/**
 * @brief: 保存灯的应用数据
 * @param[in]: data 应用数据的指针 
 * @retval: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_user_flash_write_app_data(IN light_app_data_flash_t *data)
{
    OPERATE_LIGHT ret;

    //将灯的应用数据写入flash中
    ret = tuya_soc_flash_write(SAVE_TYP2, LIGHT_APP_DATA_OFFSET, (UCHAR_T *)data, SIZEOF(light_app_data_flash_t));
    if (ret != LIGHT_OK) {
        PR_ERR("Application data write error %d!", ret);
        return LIGHT_COM_ERROR;
    }

    return LIGHT_OK;
}

/**
 * @brief: 保存灯的产测数据
 * @param[in]: data 产测数据的指针
 * @retval: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_user_flash_write_prod_data(IN light_prod_test_data_flash_t *data)
{
    OPERATE_LIGHT ret = -1;

    //将灯的产测数据写入flash中
    ret = tuya_soc_flash_write(SAVE_TYP3, PROD_TEST_DATA_OFFSET, (UCHAR_T *)data, SIZEOF(light_prod_test_data_flash_t));
    if (ret != LIGHT_OK) {
        PR_ERR("Production test data write error %d!", ret);
        return LIGHT_COM_ERROR;
    }
    return LIGHT_OK;
}

/**
 * @brief: 保存灯的应用数据到"oem_cfg"中
 * @param[in]: len 数据长度
 * @param[in]: data 应用数据指针
 * @retval: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_user_write_oem_cfg_data(IN USHORT_T len, IN UCHAR_T *data)
{
    OPERATE_LIGHT ret = -1;

    //将应用数据写入"oem_cfg"中
    ret = tuya_soc_cfg_write(len, data);
    if (ret != LIGHT_OK) {
        PR_ERR("oem cfg move to uf file error!");
        return LIGHT_COM_ERROR;
    }

    return LIGHT_OK;
}

/**
 * @brief: 读灯的上电次数
 * @param[out]: data 指向保存上电次数的指针 
 * @retval: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_user_flash_read_reset_cnt(OUT UCHAR_T *data)
{
    INT_T read_cnt = 0;

    //从flash中读取灯的上电次数
    read_cnt = tuya_soc_flash_read(SAVE_TYP1, RESET_CNT_OFFSET, 1, (UCHAR_T *)data);
    if (read_cnt <= 0) {
        PR_ERR("Reset cnt read error!");
        return LIGHT_COM_ERROR;
    }

    return LIGHT_OK;
}

/**
 * @brief: 读取灯的应用数据
 * @param[out]: data 指向保存灯应用数据的指针  
 * @retval: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_user_flash_read_app_data(OUT light_app_data_flash_t *data)
{
    INT_T read_cnt = 0;

    //从flash中读取灯的应用数据
    read_cnt = tuya_soc_flash_read(SAVE_TYP2, LIGHT_APP_DATA_OFFSET, SIZEOF(light_app_data_flash_t), (UCHAR_T *)data);
    if (read_cnt <= 0) {
        PR_ERR("Application data read error!");
        return LIGHT_COM_ERROR;
    }
    
    return LIGHT_OK;
}

/**
 * @brief: 读取灯的产测数据
 * @param[out]: data 指向保存灯产测数据的指针  
 * @return: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_user_flash_read_prod_data(OUT light_prod_test_data_flash_t *data)
{
    INT_T read_cnt = 0;

    //从flash中读取灯的产测数据
    read_cnt = tuya_soc_flash_read(SAVE_TYP3, PROD_TEST_DATA_OFFSET, SIZEOF(light_prod_test_data_flash_t), (UCHAR_T *)data);
    if (read_cnt <= 0) {
        PR_ERR("Production data read error!");
        return LIGHT_COM_ERROR;
    }

    return LIGHT_OK;
}

/**
 * @brief: 读oem cfg数据
 * @param[out]: len 读取到的数据长度 
 * @param[out]: data 指向保存oem_cfg数据的指针 
 * @retval: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_user_read_oem_cfg_data(OUT USHORT_T *len, OUT UCHAR_T *data)
{
    OPERATE_LIGHT ret = -1;

    //从"oem cfg"中读取数据
    ret = tuya_soc_cfg_read(len, data);
    if (ret != LIGHT_OK) {
        PR_ERR("oem cfg data read error!");
        return LIGHT_COM_ERROR;
    }

    return LIGHT_OK;
}

/**
 * @brief: 擦除灯的产测数据
 * @param: none 
 * @retval: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_user_flash_earse_prod_data(VOID)
{
    OPERATE_LIGHT ret = -1;

    //从flash擦除产测数据
    ret = tuya_soc_flash_earse_special(SAVE_TYP3, PROD_TEST_DATA_OFFSET);
    if (ret != LIGHT_OK) {
        PR_ERR("Production test data earse error %d!", ret);
        return LIGHT_COM_ERROR;
    }

    return LIGHT_OK;
}

/**
 * @brief: 擦除所有的用户数据
 * @param: none 
 * @retval: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_user_flash_data_erase(VOID)
{

    //擦除flash中所有的数据
    OPERATE_LIGHT ret = tuya_soc_flash_earse();  
    if (ret!= LIGHT_OK) {
        return LIGHT_COM_ERROR; 
    }
    return LIGHT_OK;
}


