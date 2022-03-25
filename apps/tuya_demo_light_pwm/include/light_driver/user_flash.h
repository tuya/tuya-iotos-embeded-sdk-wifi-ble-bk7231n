/**
* @file user_flash.h
* @author www.tuya.com
* @brief flash driver program
* @version 0.2
* @date 2022-03-25
*
* @copyright Copyright (c) tuya.inc 2022
*
*/

#ifndef __USER_FLASH_H__
#define __USER_FLASH_H__

#include "light_types.h"
#include "soc_flash.h"


#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#define LIGHT_SCENE_MAX_LENGTH    210


#pragma pack(1)

/**
 *@brief: 枚举类型LIGHT_MODE_FLASH_E
 */
typedef BYTE_T LIGHT_MODE_FLASH_E;
#define LIGHT_MODE_WHITE    0
#define LIGHT_MODE_COLOR    1
#define LIGHT_MODE_SCENE    2
#define LIGHT_MODE_MUSIC    3
#define LIGHT_MODE_MAX      4


typedef struct
{
    USHORT_T red;        
    USHORT_T green;
    USHORT_T blue;
}color_rgb_flash_t;


typedef struct
{
    USHORT_T hue;
    USHORT_T sat;
    USHORT_T value;
    UCHAR_T  color_str[13];
}color_origin_flash_t;


/**
 * @brief Light save data structure
 */
typedef struct
{
    BOOL_T                  power;
    LIGHT_MODE_FLASH_E      mode;
    USHORT_T                bright;
    USHORT_T                temper;
    color_rgb_flash_t       color;
    color_origin_flash_t    color_origin;
    CHAR_T                  scene[LIGHT_SCENE_MAX_LENGTH + 1];
}light_app_data_flash_t;


/**
 * @brief 枚举类型TEST_MODE_E
 */
typedef BYTE_T TEST_MODE_E;
#define FUC_TEST1   0
#define AGING_TEST  1
#define FUC_TEST2   2


#pragma pack(4)
/**
 * @brief Light prod test save structure
 */
typedef struct {
    TEST_MODE_E  test_mode;
    USHORT_T     aging_tested_time;
}light_prod_test_data_flash_t;
#pragma pack()


#if 0
typedef struct
{
    USHORT_T red;       
    USHORT_T green;
    USHORT_T blue;
}color_rgb_t;


typedef struct
{
    USHORT_T color_data1;
    USHORT_T color_data2;
    USHORT_T color_data3;
    UCHAR_T  color_str[13];
}color_origin_t;
#endif


typedef struct {
    UCHAR_T     valid; 
    UCHAR_T     hour;
    UCHAR_T     min;
    UCHAR_T     h_per[2];
    UCHAR_T     s_per; 
    UCHAR_T     v_per;  
    UCHAR_T     bright_per;
    UCHAR_T     temper_per;
}phythm_node_param_flash_t;


typedef struct {
    UCHAR_T     valid; 
    UCHAR_T     wday;   
    UCHAR_T     step;   
    UCHAR_T     hour;    
    UCHAR_T     min;    
    UCHAR_T     h_per[2]; 
    UCHAR_T     s_per;
    UCHAR_T     v_per;   
    UCHAR_T     bright_per;
    UCHAR_T     temper_per;
}sleep_node_param_flash_t;


typedef struct {
    UCHAR_T     version;
    UCHAR_T     onoff;  
    UCHAR_T     mode; 
    UCHAR_T     wday;  
    UCHAR_T     node_cnt;
    phythm_node_param_flash_t  node[8];
}phythm_protl_flash_t;


typedef struct {
    UCHAR_T     version;
    UCHAR_T     node_cnt;
    sleep_node_param_flash_t  node[4];
}sleep_protl_flash_t;


/**
 * @brief Light oem cfg data structure
 */
#pragma pack()

#define RESET_CNT_OFFSET          0
#define LIGHT_APP_DATA_OFFSET     (RESET_CNT_OFFSET + 2)
#define PROD_TEST_DATA_OFFSET     (LIGHT_APP_DATA_OFFSET + sizeof(light_app_data_flash_t) + 1)

#define RHYTHM_DATASAVE_TYPE      SAVE_TYP4
#define RHYTHM_DATASAVE_OFFSET    (PROD_TEST_DATA_OFFSET + sizeof(light_app_data_flash_t)+1)

#define SLEEP_DATASAVE_TYPE       SAVE_TYP5
#define SLEEP_DATASAVE_OFFSET     (RHYTHM_DATASAVE_OFFSET + sizeof(phythm_protl_flash_t) + 1)

#define WAKE_DATASAVE_TYPE        SAVE_TYP6
#define WAKE_DATASAVE_OFFSET      (SLEEP_DATASAVE_OFFSET + sizeof(sleep_protl_flash_t) + 1)

/**
 * @brief: 保存灯的上电次数
 * @param[in]: cnt 次数值  
 * @retval: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_user_flash_write_reset_cnt(IN UCHAR_T cnt);

/**
 * @brief: 保存灯的应用数据
 * @param[in]: data 待保存数据的指针 
 * @retval: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_user_flash_write_app_data(IN light_app_data_flash_t *data);

/**
 * @brief: 保存灯的产测数据
 * @param[in]: data 产测数据的指针
 * @retval: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_user_flash_write_prod_data(IN light_prod_test_data_flash_t *data);

/**
 * @brief: 保存灯的应用数据到"oem_cfg"中
 * @param[in]: len 数据长度
 * @param[in]: data 应用数据指针
 * @retval: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_user_write_oem_cfg_data(IN USHORT_T len, IN UCHAR_T *data);

/**
 * @brief: 读灯的上电次数
 * @param[out]: data 指向保存上电次数的指针 
 * @retval: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_user_flash_read_reset_cnt(OUT UCHAR_T *data);

/**
 * @brief: 读取灯的应用数据
 * @param[out]: data 指向保存灯应用数据的指针  
 * @retval: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_user_flash_read_app_data(OUT light_app_data_flash_t *data);

/**
 * @brief: 读取灯的产测数据
 * @param[out]: data 指向保存灯产测数据的指针  
 * @retval: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_user_flash_read_prod_data(OUT light_prod_test_data_flash_t *data);

/**
 * @brief: 读oem cfg数据
 * @param[out]: len 读取到的数据长度 
 * @param[out]: data 指向保存oem_cfg数据的指针 
 * @retval: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_user_read_oem_cfg_data(OUT USHORT_T *len, OUT UCHAR_T *data);

/**
 * @brief: 擦除灯的产测数据
 * @param: none 
 * @retval: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_user_flash_earse_prod_data(VOID);

/**
 * @brief: 擦除所有的用户数据
 * @param: none 
 * @retval: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_user_flash_data_erase(VOID);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif  /* __USER_FLASH_H__ */
