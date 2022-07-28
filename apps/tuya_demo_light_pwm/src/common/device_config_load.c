/**
* @file device_config_load.c
* @author www.tuya.com
* @brief Light device configuration item processing module
* @version 0.2
* @date 2022-03-25
*
* @copyright Copyright (c) tuya.inc 2022
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "device_config_load.h"
#include "user_flash.h"
#include "light_tools.h"
#include "light_printf.h"

#define CONFIG_DATA_LEN_MAX    1024
#define MAX_VALUE_SIZE         15 //+ 512
#define CHECK_LOAD_FINISH()    if (_device_cfg_init_check() != LIGHT_OK) { \
                                    return LIGHT_COM_ERROR;\
                                }

#define PROD_AGAING_INVAILD_VALUE 	0xFF

STATIC UCHAR_T sg_oem_config_load_flag = FALSE;
STATIC device_config_t sg_device_cfg;


typedef BYTE_T VALUE_TYPE_E; 
#define TYPE_HEX        0
#define TYPE_STRING     1

typedef struct {
    CHAR_T *string;
    UCHAR_T enum_value;
}string_to_enum_t;

string_to_enum_t g_cmode_table[] = 
{
    {"c", CMOD_C},
    {"cw", CMOD_CW},
    {"rgb", CMOD_RGB},
    {"rgbc", CMOD_RGBC},
    {"rgbcw", CMOD_RGBCW}
};

string_to_enum_t g_wf_cfg_table[] = 
{
    {"old", GWCM_OLD},
    {"low", GWCM_LOW_POWER},
    {"spcl", GWCM_SPCL_MODE},
    {"prod", GWCM_OLD_PROD},
    {"spcl_auto", GWCM_SPCL_AUTOCFG}
};

string_to_enum_t g_color_table[] = 
{
    {"c", COLOR_C},
    {"w", COLOR_W},
    {"r", COLOR_R},
    {"g", COLOR_G},
    {"b", COLOR_B},
};

#if (LIGHT_CFG_ENABLE_GAMMA == 1)
/// RED gamma 0.8 ~ 100% 
STATIC UCHAR_T sg_gamma_red[] = {
                                        0,  0,  1,  1,  1,  2,  2,  3,  3,  4,  4,  5,  6,  6,  7,  7,  8,  9,  9,  10,
                                        11, 11, 12, 13, 13, 14, 15, 15, 16, 17, 18, 18, 19, 20, 21, 21, 22, 23, 24, 24,
                                        25, 26, 27, 28, 28, 29, 30, 31, 32, 32, 33, 34, 35, 36, 37, 37, 38, 39, 40, 41,
                                        42, 43, 44, 44, 45, 46, 47, 48, 49, 50, 51, 52, 52, 53, 54, 55, 56, 57, 58, 59,
                                        60, 61, 62, 63, 64, 65, 66, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78,
                                        79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98,
                                        99, 100,101,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,
                                        121,122,123,124,125,126,127,128,129,130,131,132,134,135,136,137,138,139,140,141,
                                        142,144,145,146,147,148,149,150,151,152,154,155,156,157,158,159,160,162,163,164,
                                        165,166,167,168,170,171,172,173,174,175,177,178,179,180,181,182,184,185,186,187,
                                        188,189,191,192,193,194,195,196,198,199,200,201,202,204,205,206,207,208,210,211,
                                        212,213,214,216,217,218,219,220,222,223,224,225,227,228,229,230,231,233,234,235,
                                        236,238,239,240,241,243,244,245,246,248,249,250,251,253,254,255
};

/// GREEN gamma 0.6 ~ 70%
STATIC UCHAR_T sg_gamma_green[] = {
                                        0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,
                                        3,  3,  3,  3,  3,  4,  4,  4,  4,  5,  5,  5,  6,  6,  6,  7,  7,  7,  7,  8,
                                        8,  8,  9,  9, 10, 10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16,
                                        16, 16, 17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 23, 24, 24, 25, 25,
                                        26, 26, 27, 27, 28, 29, 29, 30, 30, 31, 31, 32, 33, 33, 34, 34, 35, 36, 36, 37,
                                        38, 38, 39, 39, 40, 41, 41, 42, 43, 43, 44, 45, 45, 46, 47, 47, 48, 49, 49, 50,
                                        51, 52, 52, 53, 54, 54, 55, 56, 57, 57, 58, 59, 60, 60, 61, 62, 63, 63, 64, 65,
                                        66, 66, 67, 68, 69, 70, 70, 71, 72, 73, 74, 75, 75, 76, 77, 78, 79, 80, 80, 81,
                                        82, 83, 84, 85, 86, 86, 87, 88, 89, 90, 91, 92, 93, 94, 94, 95, 96, 97, 98, 99,
                                        100,101,102,103,104,105,106,106,107,108,109,110,111,112,113,114,115,116,117,118,
                                        119,120,121,122,123,124,125,126,127,128,129,130,131,132,133,134,135,136,137,139,
                                        140,141,142,143,144,145,146,147,148,149,150,151,152,154,155,156,157,158,159,160,
                                        161,162,164,165,166,167,168,169,170,172,173,174,175,176,177,179
};

//BLUE gama 0.6-75%
STATIC UCHAR_T sg_gamma_blue[] = {
                                        0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  3,
                                        3,  3,  3,  3,  4,  4,  4,  5,  5,  5,  5,  6,  6,  6,  7,  7,  7,  8,  8,  8,
                                        9,  9,  9, 10, 10, 11, 11, 11, 12, 12, 13, 13, 14, 14, 14, 15, 15, 16, 16, 17,
                                        17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 23, 24, 24, 25, 25, 26, 27, 27,
                                        28, 28, 29, 29, 30, 31, 31, 32, 32, 33, 34, 34, 35, 36, 36, 37, 38, 38, 39, 40,
                                        40, 41, 42, 42, 43, 44, 44, 45, 46, 46, 47, 48, 49, 49, 50, 51, 51, 52, 53, 54,
                                        54, 55, 56, 57, 58, 58, 59, 60, 61, 61, 62, 63, 64, 65, 65, 66, 67, 68, 69, 70,
                                        70, 71, 72, 73, 74, 75, 76, 76, 77, 78, 79, 80, 81, 82, 83, 83, 84, 85, 86, 87,
                                        88, 89, 90, 91, 92, 93, 94, 94, 95, 96, 97, 98, 99,100,101,102,103,104,105,106,
                                        107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,127,
                                        128,129,130,131,132,133,134,135,136,137,138,139,141,142,143,144,145,146,147,148,
                                        150,151,152,153,154,155,156,158,159,160,161,162,163,165,166,167,168,169,170,172,
                                        173,174,175,176,178,179,180,181,183,184,185,186,188,189,190,191
};
#endif

typedef struct {
    CHAR_T  *key;
    VOID    *value;
    USHORT_T value_size;
    VALUE_TYPE_E type;
}key_value_t;

key_value_t g_json_table[] = {
    {",Jsonver:",    &sg_device_cfg.Jsonver,   SIZEOF(sg_device_cfg.Jsonver),        TYPE_STRING},
    {",category:",   &sg_device_cfg.category,  SIZEOF(sg_device_cfg.category),       TYPE_STRING},
    {",module:",     &sg_device_cfg.module,    SIZEOF(sg_device_cfg.module),         TYPE_STRING}, 
    
    {",cmod:",       &sg_device_cfg.cmod,      SIZEOF(sg_device_cfg.cmod),           TYPE_STRING}, 
    {",dmod:",       &sg_device_cfg.dmod,      SIZEOF(sg_device_cfg.dmod),           TYPE_HEX}, 
    {",cwtype:",     &sg_device_cfg.cwtype,    SIZEOF(sg_device_cfg.cwtype),         TYPE_HEX}, 
    {",onoffmode:",  &sg_device_cfg.onoffmode, SIZEOF(sg_device_cfg.onoffmode),      TYPE_HEX}, 
    {",pmemory:",    &sg_device_cfg.pmemory,   SIZEOF(sg_device_cfg.pmemory),        TYPE_HEX},
    {",ctrl_pin:",   &sg_device_cfg.ctrl_pin,  SIZEOF(sg_device_cfg.ctrl_pin),       TYPE_HEX},
    {",ctrl_lv:",    &sg_device_cfg.ctrl_lv,   SIZEOF(sg_device_cfg.ctrl_lv),        TYPE_HEX},
      
    {",defcolor:",   &sg_device_cfg.defcolor,  SIZEOF(sg_device_cfg.defcolor),       TYPE_STRING},
    {",defbright:",  &sg_device_cfg.defbright, SIZEOF(sg_device_cfg.defbright),      TYPE_HEX},
    {",deftemp:",    &sg_device_cfg.deftemp,   SIZEOF(sg_device_cfg.deftemp),        TYPE_HEX},
    {",cwmaxp:",     &sg_device_cfg.cwmaxp,    SIZEOF(sg_device_cfg.cwmaxp),         TYPE_HEX},
    {",brightmin:",  &sg_device_cfg.brightmin, SIZEOF(sg_device_cfg.brightmin),      TYPE_HEX},
    {",brightmax:",  &sg_device_cfg.brightmax, SIZEOF(sg_device_cfg.brightmax),      TYPE_HEX},
    {",colormax:",   &sg_device_cfg.colormax,  SIZEOF(sg_device_cfg.colormax),       TYPE_HEX},
    {",colormin:",   &sg_device_cfg.colormin,  SIZEOF(sg_device_cfg.colormin),       TYPE_HEX},

    {",wfcfg:",      &sg_device_cfg.wfcfg,     SIZEOF(sg_device_cfg.wfcfg),          TYPE_STRING},
    {",remdmode:",   &sg_device_cfg.remdmode,   SIZEOF(sg_device_cfg.remdmode),      TYPE_HEX},
    {",rstnum:",     &sg_device_cfg.rstnum,    SIZEOF(sg_device_cfg.rstnum),         TYPE_HEX},
    {",rstcor:",     &sg_device_cfg.rstcor,    SIZEOF(sg_device_cfg.rstcor),         TYPE_STRING},
    {",rstbr:",      &sg_device_cfg.rstbr,     SIZEOF(sg_device_cfg.rstbr),          TYPE_HEX},
    {",rsttemp:",    &sg_device_cfg.rsttemp,   SIZEOF(sg_device_cfg.rsttemp),        TYPE_HEX},
    {",pairt:",      &sg_device_cfg.remdtime,  SIZEOF(sg_device_cfg.remdtime),       TYPE_HEX},
    {",wfct:",       &sg_device_cfg.wfptime,   SIZEOF(sg_device_cfg.wfptime),        TYPE_HEX},

    {",pwmhz:",      &sg_device_cfg.pwmhz,     SIZEOF(sg_device_cfg.pwmhz),          TYPE_HEX},
    {",r_pin:",      &sg_device_cfg.r_pin,     SIZEOF(sg_device_cfg.r_pin),          TYPE_HEX},
    {",r_lv:",       &sg_device_cfg.r_lv,      SIZEOF(sg_device_cfg.r_lv),           TYPE_HEX},
    {",g_pin:",      &sg_device_cfg.g_pin,     SIZEOF(sg_device_cfg.g_pin),          TYPE_HEX},
    {",g_lv:",       &sg_device_cfg.g_lv,      SIZEOF(sg_device_cfg.g_lv),           TYPE_HEX},
    {",b_pin:",      &sg_device_cfg.b_pin,     SIZEOF(sg_device_cfg.b_pin),          TYPE_HEX},
    {",b_lv:",       &sg_device_cfg.b_lv,      SIZEOF(sg_device_cfg.b_lv),           TYPE_HEX},
    {",c_pin:",      &sg_device_cfg.c_pin,     SIZEOF(sg_device_cfg.c_pin),          TYPE_HEX},
    {",c_lv:",       &sg_device_cfg.c_lv,      SIZEOF(sg_device_cfg.c_lv),           TYPE_HEX},
    {",w_pin:",      &sg_device_cfg.w_pin,     SIZEOF(sg_device_cfg.w_pin),          TYPE_HEX},
    {",w_lv:",       &sg_device_cfg.w_lv,      SIZEOF(sg_device_cfg.w_lv),           TYPE_HEX},

#if (LIGHT_CFG_SUPPORT_LOWPOWER == 1)
    {",title20:",    &sg_device_cfg.title20,   SIZEOF(sg_device_cfg.title20),        TYPE_HEX},
#endif

#if ((LIGHT_CFG_ENABLE_GAMMA == 1 ) && (LIGHT_CFG_GAMMA_CAL == 0))
    {",gmr:",        &sg_gamma_red,           SIZEOF(sg_gamma_red),                 TYPE_STRING},
    {",gmg:",        &sg_gamma_green,         SIZEOF(sg_gamma_green),               TYPE_STRING},
    {",gmb:",        &sg_gamma_blue,          SIZEOF(sg_gamma_blue),                TYPE_STRING},
#endif

#if ((LIGHT_CFG_ENABLE_GAMMA == 1) && (LIGHT_CFG_GAMMA_CAL == 1))
    {",gmkr:",       &sg_device_cfg.gammakr,   SIZEOF(sg_device_cfg.gammakr),        TYPE_HEX},
    {",gmkg:",       &sg_device_cfg.gammakg,   SIZEOF(sg_device_cfg.gammakg),        TYPE_HEX},
    {",gmkb:",       &sg_device_cfg.gammakb,   SIZEOF(sg_device_cfg.gammakb),        TYPE_HEX},

    {",gmwr:",       &sg_device_cfg.gammawr,   SIZEOF(sg_device_cfg.gammawr),        TYPE_HEX},
    {",gmwg:",       &sg_device_cfg.gammawg,   SIZEOF(sg_device_cfg.gammawg),        TYPE_HEX},
    {",gmwb:",       &sg_device_cfg.gammawb,   SIZEOF(sg_device_cfg.gammawb),        TYPE_HEX},
#endif    
};

#define GET_ARRAY_LEN(x)    (SIZEOF(x) / SIZEOF(x[0]))

#define JSON_TABLE_LEN      GET_ARRAY_LEN(g_json_table)
#define CMOD_TABLE_LEN      GET_ARRAY_LEN(g_cmode_table)
#define WFCFG_TABLE_LEN     GET_ARRAY_LEN(g_wf_cfg_table)
#define COLOR_TABLE_LEN     GET_ARRAY_LEN(g_color_table)



/**
 * @brief: Device configuaration init check
 * @param: none
 * @return: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
STATIC OPERATE_LIGHT _device_cfg_init_check(VOID)
{    
    if (!sg_oem_config_load_flag) { 
        PR_ERR("oem config data not load !");
        return LIGHT_COM_ERROR;
    }

    return LIGHT_OK;
}

/**
 * @brief: get all config data from ucDeviceConfigTable[], and install it to
            the sg_device_cfg struct. make sure all keys of g_json_table[] can be find in ucDeviceConfigTable[]
 * @param[in]: max_len -> string len
 * @param[in]: p_buf -> 
 * @return: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code描述文档
 */
STATIC OPERATE_LIGHT _device_cfg_analysis(IN USHORT_T max_len, IN CHAR_T *p_buf)
{
    UCHAR_T i = 0;
    CHAR_T *p_start;
    CHAR_T *p_end;
    CHAR_T temp_value[MAX_VALUE_SIZE];
    USHORT_T len;

    
    *(p_buf + 0) = ',';          // to make sure don't match error, such as code "abcd" and "abc"
    
    for (i = 0; i < JSON_TABLE_LEN; i++) {
        p_start = strstr(p_buf, g_json_table[i].key);
        if (p_start == NULL) {
            continue;
        }

        p_start += strlen(g_json_table[i].key);
        p_end = p_start;
        
        while (*p_end != ',') {
            p_end ++;
            if (p_end - p_buf >= max_len) {        
                //if the last key don't has "," end! 
                break;
            }
        }

        len = p_end - p_start;
        
        if (len > MAX_VALUE_SIZE - 1) {      
            //attention: key-value, max len is 15!! 
            len = g_json_table[i].value_size;
        }
        
        memcpy(temp_value, p_start, len);
        temp_value[len] = '\0';

        if (g_json_table[i].type == TYPE_HEX) {
            UINT_T temp = 0;
            UCHAR_T dec_len = 0;

            //attention: default little-ending format 
            dec_len = tuya_light_tool_str_to_dec(temp_value, len, &temp);     

            memcpy(g_json_table[i].value, &temp, dec_len);
        } else {
    #if ((LIGHT_CFG_ENABLE_GAMMA == 1) & (LIGHT_CFG_GAMMA_CAL == 0))
            if ((strcmp(g_json_table[i].key, "gmr:") == 0) || (strcmp(g_json_table[i].key, "gmg:") == 0) || (strcmp(g_json_table[i].key, "gmb:") == 0)) {
                UCHAR_T temp;
                USHORT_T j;

                for (j = 0; j < (len / 2); j++) {
                    temp = tuya_light_tool_str_to_uchar(tuya_light_tool_asc_to_hex(*(p_start + 2*j)), tuya_light_tool_asc_to_hex(*(p_start + 2*j + 1)));
                    *((UCHAR_T *)g_json_table[i].value + j) = temp;
                    //*((UCHAR_T *)(g_json_table[i].value + j)) = temp;
                }   
            } else {
                memcpy(g_json_table[i].value, temp_value, len);
            } 
            
    #else
            memcpy(g_json_table[i].value, temp_value, len);
    #endif
        }  
    }
    
    return LIGHT_OK;
}

/**
 * @brief: device configuration data init operate.
 * @param: none
 * @return: none
 */
STATIC VOID _device_cfg_data_init(VOID)
{
    sg_oem_config_load_flag = FALSE;

    memset(&sg_device_cfg, 0, SIZEOF(device_config_t));
    
    sg_device_cfg.ctrl_pin = PIN_NOEXIST;

    return;
}

/**
 * @brief: 设备配置设置接口(struct)
 * @param[in]: p_config 指向要配置的结构体
 * @return: OPERATE_RET 0表示成功，非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_device_cfg_set_struct(IN device_config_t *p_config)
{
    if (p_config == NULL) {
        PR_ERR("Default config set param invalid!");
        return LIGHT_INVALID_PARM;
    }

    if (sg_oem_config_load_flag) {
        PR_NOTICE("device config data already load! Don't load again!!");
        return LIGHT_OK;
    }
    
    _device_cfg_data_init();

    memcpy(&sg_device_cfg, p_config, SIZEOF(device_config_t));

    sg_oem_config_load_flag = TRUE;
    
    return LIGHT_OK;   
}

/**
 * @brief: 设备配置设置接口(string)
 * @param[in]: len 配置字符串长度, p_config 指向要配置的字符串
 * @return: OPERATE_RET 0表示成功，非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_device_cfg_set_string(IN USHORT_T len, IN CONST CHAR_T *p_config)
{
    OPERATE_LIGHT ret = -1;

    if (sg_oem_config_load_flag) {
        PR_NOTICE("device config data already load! Don't load again!!");
        return LIGHT_OK;
    }
    
    if (len >= CONFIG_DATA_LEN_MAX) {
        PR_ERR("oem config set ERROR!");
        return LIGHT_COM_ERROR;
    }

    _device_cfg_data_init();

    ret = _device_cfg_analysis(len, (CHAR_T *)p_config);
    if (ret != LIGHT_OK) {
         PR_ERR("oem data vaild!");
         return LIGHT_COM_ERROR;
    }

#if ((LIGHT_CFG_ENABLE_GAMMA == 1) && (LIGHT_CFG_GAMMA_CAL == 1)) 
    USHORT_T i;  
    if ((sg_device_cfg.gammakr != 0) || (sg_device_cfg.gammawr != 0) || \
        (sg_device_cfg.gammakg != 0) || (sg_device_cfg.gammawg != 0) || \
        (sg_device_cfg.gammakb != 0) || (sg_device_cfg.gammawb != 0)) {
        for(i = 0; i < 256 ; i++) {
            sg_gamma_red[i] = tuya_light_tool_calculate_gamma(i, (FLOAT_T)(sg_device_cfg.gammakr / 100.0), (FLOAT_T)(sg_device_cfg.gammawr / 100.0));
            sg_gamma_green[i] = tuya_light_tool_calculate_gamma(i, (FLOAT_T)(sg_device_cfg.gammakg / 100.0), (FLOAT_T)(sg_device_cfg.gammawg / 100.0));
            sg_gamma_blue[i] = tuya_light_tool_calculate_gamma(i, (FLOAT_T)(sg_device_cfg.gammakb / 100.0), (FLOAT_T)(sg_device_cfg.gammawb / 100.0));
        }
    }
    
#endif

    sg_oem_config_load_flag = TRUE;
    
    return LIGHT_OK;
}

/**
 * @brief: 设备配置项装载处理，配置文件可以通过配置文件获取或者是DEFAULT_CONFIG宏定义设置
 * @param[in]: none
 * @return: OPERATE_RET 0表示成功，非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_device_cfg_data_load(VOID)
{
    USHORT_T len = 0;
    OPERATE_LIGHT ret = -1;
    UCHAR_T * p_buf = NULL;
    
    if (sg_oem_config_load_flag) {
        PR_NOTICE("device config data already load! Don't load again!!");
        return LIGHT_OK;
    }

    p_buf = Malloc(2 * CONFIG_DATA_LEN_MAX);
    if (p_buf == NULL) {
        PR_ERR("Malloc failure!");
        return LIGHT_COM_ERROR;
    }

    //读oem cfg数据
    ret = tuya_user_read_oem_cfg_data(&len, p_buf);
    if ((ret != LIGHT_OK) || (len <= 0) || (len >= CONFIG_DATA_LEN_MAX)) {
        PR_ERR("oem data ERROR");

        //Free !!
        Free(p_buf);     
        p_buf = NULL;

        return LIGHT_COM_ERROR;
    }
    
    PR_DEBUG("oem len: %d", len);
    
    _device_cfg_data_init();
    
    ret = _device_cfg_analysis(len, p_buf);
    if (ret != LIGHT_OK) {
        PR_ERR("oem data ERROR");
        return LIGHT_COM_ERROR;
    }
    
    if (p_buf != NULL) {
        Free(p_buf);
        p_buf = NULL;
    }
    
#if ((LIGHT_CFG_ENABLE_GAMMA == 1) && (LIGHT_CFG_GAMMA_CAL == 1))  
    USHORT_T i;
    if((sg_device_cfg.gammakr != 0) || (sg_device_cfg.gammawr != 0) || \
        (sg_device_cfg.gammakg != 0) || (sg_device_cfg.gammawg != 0) || \
        (sg_device_cfg.gammakb != 0) || (sg_device_cfg.gammawb != 0)) {
        for(i = 0; i < 256 ; i++) {
            sg_gamma_red[i] = tuya_light_tool_calculate_gamma(i, (FLOAT_T)(sg_device_cfg.gammakr / 100.0), (FLOAT_T)(sg_device_cfg.gammawr / 100.0));
            sg_gamma_green[i] = tuya_light_tool_calculate_gamma(i, (FLOAT_T)(sg_device_cfg.gammakg / 100.0), (FLOAT_T)(sg_device_cfg.gammawg / 100.0));
            sg_gamma_blue[i] = tuya_light_tool_calculate_gamma(i, (FLOAT_T)(sg_device_cfg.gammakb / 100.0), (FLOAT_T)(sg_device_cfg.gammawg / 100.0));
        }
    }
#endif  

    sg_oem_config_load_flag = TRUE;

    return LIGHT_OK;
}

/**
 * @brief: 获取json配置的版本号
 * @param[out]: p_json_ver json 版本号字符串
 * @return: p_json_ver json 版本号字符串
 */
CHAR_T tuya_cfg_get_json_ver(OUT CHAR_T *p_json_ver)
{   
    CHECK_LOAD_FINISH();

    memcpy(p_json_ver, sg_device_cfg.Jsonver, SIZEOF(sg_device_cfg.Jsonver));
    
    return LIGHT_OK;
}


/**
 * @brief: 获取设备的集合属性
 * @param[out]: p_category 集合字符串
 * @note: 该集合属性可以不用关注
 * @return: p_category 集合字符串
 */
CHAR_T tuya_cfg_get_category(OUT CHAR_T *p_category)
{
    CHECK_LOAD_FINISH();

    memcpy(p_category, sg_device_cfg.category, 5);
    
    return LIGHT_OK;
}

/**
 * @brief: 获取设备soc使用模组型号
 * @param[out]: p_module 模组型号字符串, p_len 模组型号字符串长度
 * @return: p_module 模组型号字符串, p_len 模组型号字符串长度
 */
CHAR_T tuya_cfg_get_module_code(OUT CHAR_T *p_module, OUT UCHAR_T *p_len)
{
    CHECK_LOAD_FINISH();
    
    *p_len = strlen(sg_device_cfg.module);

    memcpy(p_module, &sg_device_cfg.module[0], strlen(sg_device_cfg.module));

    return LIGHT_OK;
}

/**
 * @brief: 获取灯的类型，如RGBCW5路灯，CW两路灯。
 *         种类有：RGBCW/RGBC/RGB/CW/C
 * @param: none
 * @return: 0x01 -> C, 
 *          0x02 -> CW
 *          0x03 -> RGB
 *          0x04 -> RGBC
 *          0x05 -> RGBCW
 *          -1   -> not find light way,error
 */
CHAR_T tuya_cfg_get_color_mode(VOID)
{
    CHECK_LOAD_FINISH();
    
    UCHAR_T i;

    for (i = 0; i < CMOD_TABLE_LEN; i++) {
        if (tuya_string_compare(&sg_device_cfg.cmod[0], g_cmode_table[i].string)) {
            return g_cmode_table[i].enum_value;
        }
    }
    
    return LIGHT_COM_ERROR;
}

/**
 * @brief: 获取灯驱动模式
 * @param: none
 * @note: 此demo仅支持pwm驱动方式，其他驱动可联系涂鸦获取或者自行实现
 * @return: 
 *          0x00 -> PWM
 *          0x01 -> SM16726B
 *          0x02 -> SM2135e
 *          0x03->sm2135eh
 *          0x04->sm2135ej
 *          0x05->bp1658cj;
 *          -1   -> configure not load, error
 */
CHAR_T tuya_cfg_get_driver_mode(VOID)
{
    CHECK_LOAD_FINISH();
    
    return sg_device_cfg.dmod;
}

/**
 * @brief: 获取灯冷暖光驱动方式
 *          cw 驱动是纯软件计算互补的方式
 *          cct 驱动是硬件实现互补
 * @param: none
 * @return: 
 *          0x00 -> CW drive
 *          0x01 -> CCT drive
 */
CHAR_T tuya_cfg_get_cw_type(VOID)
{
    CHECK_LOAD_FINISH();
    
    return sg_device_cfg.cwtype;
}

/**
 * @brief: 获取设备上电点灯灯光变化模式
 * @param: none
 * @return:  
 *          0x00 -> turn on/off change gradually（上电点灯和模式切换是渐变的）
 *          0x01 -> turn on/off change directly（上电点灯是跳变，模式切换是渐变）
 *          -1   -> configure not load, error
 */
CHAR_T tuya_cfg_get_onoff_change_mode(VOID)
{
    CHECK_LOAD_FINISH();
    
    return sg_device_cfg.onoffmode;
}

/**
 * @brief: 获取设备断电记忆模式
 * @param: none
 * @return: 
 *          0x00 -> don't save app data in flash
 *          0x01 -> save app data in flash
 *          -1   -> configure not load, error
 */
CHAR_T tuya_cfg_get_memory_mode(VOID)
{
    CHECK_LOAD_FINISH();
    
    return sg_device_cfg.pmemory;
}

/**
 * @brief: 获取设备控制管脚IO
 * @param: none
 * @note: 控制管脚可以结合硬件设计用来做为驱动IC的电源控制或者灯开关的指示
 * @return: 控制管脚IO, -1 -> configure not load, error
 */
CHAR_T tuya_cfg_get_ctrl_pin(VOID)
{
    CHECK_LOAD_FINISH();
    
    return sg_device_cfg.ctrl_pin;
}

/**
 * @brief: 获取设备控制管脚有效电平
 * @param: none
 * @return: 1 -> 高电平有效, 0 -> 低电平有效, -1 -> not find,error
 */
CHAR_T tuya_cfg_get_ctrl_pin_active_level(VOID)
{
    CHECK_LOAD_FINISH();
    
    return sg_device_cfg.ctrl_lv;
}

/**
 * @brief: 获取灯（未配网状态下或者配网上初始）默认颜色
 * @param[in]: none
 * @return: 
 *          0x00 -> default bright is C when connected.
 *          0x01 -> default bright is W when connected.
 *          0x02 -> default bright is R when connected.
 *          0x03 -> default bright is G when connected.
 *          0x04 -> default bright is B when connected.
 *          -1   -> configure not load, error
 */
CHAR_T tuya_cfg_get_default_color(VOID)
{
    CHECK_LOAD_FINISH();
    
    UCHAR_T i;

    CHAR_T temp_color[2] = {0,0}; //the second value is end of string: "\0"
    
    temp_color[0] = sg_device_cfg.defcolor;

    for (i = 0; i < COLOR_TABLE_LEN; i++) { 
       if(tuya_string_compare(temp_color, g_color_table[i].string)) {
          return g_color_table[i].enum_value;
       }
    }
    
    return LIGHT_COM_ERROR;
}

/**
 * @brief: 获取灯（未配网状态下或者配网上初始）默认亮度值
 * @param: none
 * @note: 默认亮度值不光影响CW白光的亮度，也是RGB彩光的亮度
 * @return: 默认亮度值, 范围: 0~100, -1 -> configure not load, error
 */
CHAR_T tuya_cfg_get_default_brightness(VOID)
{
    CHECK_LOAD_FINISH();
    
    return  sg_device_cfg.defbright;
}

/**
 * @brief: 获取灯（未配网状态下或者配网上初始）默认色温值
 * @param: none
 * @return: 默认色温值, 范围: 0~100, -1 -> configure not load, error
 */
CHAR_T tuya_cfg_get_default_temperature(VOID)
{
    CHECK_LOAD_FINISH();
    
    return sg_device_cfg.deftemp;
}

/**
 * @brief: 获取灯cw两路功率放大倍数
 * @param: none
 * @return: 功率放大倍数, 范围: 100~200
 */
CHAR_T tuya_cfg_get_cw_max_power(VOID)
{
    CHECK_LOAD_FINISH();
    
    return sg_device_cfg.cwmaxp;
}

/**
 * @brief: 获取灯白光最小亮度值 
 * @param: none
 * @return: 白光最小亮度值, 范围：1~max, -1 -> configure not load, error
 */
CHAR_T tuya_cfg_get_white_min(VOID)
{
    CHECK_LOAD_FINISH();
    
    return sg_device_cfg.brightmin;
}

/**
 * @brief: 获取灯白光最大亮度值 
 * @param: none
 * @return: 白光最大亮度值, 范围：min~100, -1 -> configure not load, error
 */
CHAR_T tuya_cfg_get_white_max(VOID)
{
    CHECK_LOAD_FINISH();
    
    return sg_device_cfg.brightmax;
}

/**
 * @brief: 获取灯RGB彩光最低亮度值 
 * @param: none
 * @note: 此亮度值并不是意味着R、G、B三色最低值，而是HSV中V的最低值
 * @return: RGB彩光最低亮度值, 范围：1~max
 */
CHAR_T tuya_cfg_get_color_min(VOID)
{
    CHECK_LOAD_FINISH();
    
    return sg_device_cfg.colormin;
}

/**
 * @brief: 获取灯RGB彩光最大亮度值 
 * @param: none
 * @note: 此亮度值并不是意味着R、G、B三色最大值，而是HSV中V的最大值
 * @return: RGB彩光最大亮度值, 范围：min~100
 */
CHAR_T tuya_cfg_get_color_max(VOID)
{
    CHECK_LOAD_FINISH();
    
    return sg_device_cfg.colormax;
}

/**
 * @brief: 获取设备配网方式
 * @param: none
 * @return: 
 *      配网方式详见配网说明文档，简述如下：
 *          GWCM_OLD -> 主要用于通用对接，进入产测由mcu发起，任何时刻均进入产测。
 *          GWCM_LOW_POWER -> 主要用于SOC电工产品，上电常亮，需要手动切换到配网状态
 *          GWCM_SPCL_MODE -> 主要用于SOC照明产品，上电常亮，需要手动切换到配网状态
 *          GWCM_OLD_PROD -> 上电快闪（上电即进入配网状态）
 *          LOW_POWER_AUTO_CFG -> 闪烁默认3min后常亮，不具有防误触模式
 *          GWCM_SPCL_AUTOCFG -> 闪烁默认3min后常亮，具有防误触模式
 *          -1 -> not find, error
 */
CHAR_T tuya_cfg_get_wifi_cfg_mode(VOID)
{
    CHECK_LOAD_FINISH();
    
    UCHAR_T i;
    
    for (i = 0;i < WFCFG_TABLE_LEN; i++){
        if (tuya_string_compare(&sg_device_cfg.wfcfg[0], g_wf_cfg_table[i].string)) {
            return g_wf_cfg_table[i].enum_value;
        }
    }
    
    return LIGHT_COM_ERROR;
}

/**
 * @brief: 获取设备配网提醒式样的方式
 *         当前支持的配网提醒式样有：闪烁、呼吸
 * @param: none
 * @return: 配网提醒式样, 0 -> 闪烁配网, 1 -> 呼吸配网
 */
CHAR_T tuya_cfg_get_remind_mode(VOID)
{
    CHECK_LOAD_FINISH(); 
       
    return sg_device_cfg.remdmode;   //default remind mode is 0 --> blink 
}

/**
 * @brief: 获取开关开重置的次数
 * @param: none
 * @return: reset number, -1 -> configure not load, error
 */
CHAR_T tuya_cfg_get_reset_num(VOID)
{
    CHECK_LOAD_FINISH();
    
    return sg_device_cfg.rstnum;
}

/**
 * @brief: 获取重置时（配网状态下）灯光颜色
 * @param: none
 * @return: 
 *          0x00 -> default bright is C when pairing.
 *          0x01 -> default bright is W when pairing.
 *          0x02 -> default bright is R when pairing.
 *          0x03 -> default bright is G when pairing.
 *          0x04 -> default bright is B when pairing.
 *          -1   -> not find, error
 */
CHAR_T tuya_cfg_get_reset_color(VOID)
{
    CHECK_LOAD_FINISH();
    
    UCHAR_T i;
    CHAR_T color[2] = {0,0};

    color[0] = sg_device_cfg.rstcor;

	for (i = 0; i < COLOR_TABLE_LEN; i++){ 
       if (tuya_string_compare(color, g_color_table[i].string)){
          return g_color_table[i].enum_value;
       }
    }
    
    return LIGHT_COM_ERROR;
}

/**
 * @brief: 获取重置时（配网状态下）亮度值
 * @param: none
 * @return: 白光模式下亮度值，范围：0~100, -1 -> not find, error
 */
CHAR_T tuya_cfg_get_reset_brightness_value(VOID)
{
    CHECK_LOAD_FINISH();
    
    return sg_device_cfg.rstbr;
}

/**
 * @brief: 获取重置时（配网状态下）色温值
 * @param: none
 * @return: 白光模式下色温值, 范围：0~100, -1 -> not find, error
 */
CHAR_T tuya_cfg_get_reset_temperature_value(VOID)
{   
    CHECK_LOAD_FINISH();
    
    return sg_device_cfg.rsttemp;
}

/**
 * @brief: 获取设备提醒配网式样持续时长
 * @param: none
 * @note: 提醒配网式样持续时长仅在呼吸配网方式中生效，快闪的方式不生效
 * @return: 设备提醒配网式样持续时长, 单位：sec
 */
SHORT_T tuya_cfg_get_remind_time_sec(VOID)
{
    CHECK_LOAD_FINISH();  

    return sg_device_cfg.remdtime;   //remind time only use as breathing
}

/**
 * @brief: 获取设备配网时长
 * @param: none
 * @note: 配网时长是指除去上电快闪的配网方式除外，进入配网状态时，配网状态的维持的时间
 *        配网方式具体详见WiFi配网方式文档
 * @return: 配网时长, 单位：min
 */
CHAR_T tuya_cfg_get_pairing_time_min(VOID)
{
    CHECK_LOAD_FINISH();

    if(0 == sg_device_cfg.wfptime) { 
        //to avoid ota, 3min
        sg_device_cfg.wfptime = 3;
    }
    
    return sg_device_cfg.wfptime;
}

/**
 * @brief: 获取pwm驱动频率
 * @param: none
 * @return: pwm频率, 单位：Hz, -1 -> not find, error 
 */
USHORT_T tuya_cfg_get_pwm_frequency(VOID)
{
    CHECK_LOAD_FINISH();
    
    return sg_device_cfg.pwmhz;
}

/**
 * @brief: 获取红光IO口
 * @param: none
 * @return: 红光驱动IO号, -1 -> not find, error
 */
CHAR_T tuya_cfg_get_red_pin(VOID)
{
    CHECK_LOAD_FINISH();
    
    return sg_device_cfg.r_pin;
}

/**
 * @brief: 获取红光驱动IO口有效电平
 * @param: none
 * @note: RGBCW驱动有效电平在IOT平台上配置不允许有不相同的情况，
 *          在定制固件中可以进行个性定制修改
 * @return: 1 -> 高电平有效, 0 -> 低电平有效, -1 -> not find,error
 */
CHAR_T tuya_cfg_get_red_pin_active_level(VOID)
{
    CHECK_LOAD_FINISH();
    
    return sg_device_cfg.r_lv;
}

/**
 * @brief: 获取绿光IO口
 * @param: none
 * @return: 绿光驱动IO号, -1 -> not find, error
 */
CHAR_T tuya_cfg_get_green_pin(VOID)
{
    CHECK_LOAD_FINISH();
    
    return sg_device_cfg.g_pin;
}

/**
 * @brief: 获取绿光驱动IO口有效电平
 * @param: none
 * @note: RGBCW驱动有效电平在IOT平台上配置不允许有不相同的情况，
 *          在定制固件中可以进行个性定制修改
 * @return: 1 -> 高电平有效, 0 -> 低电平有效,  -1 -> not find, error
 */
CHAR_T tuya_cfg_get_green_pin_active_level(VOID)
{
    CHECK_LOAD_FINISH();
    
    return sg_device_cfg.g_lv;
}

/**
 * @brief: 获取蓝光IO口
 * @param: none
 * @return: 蓝光驱动IO号, -1 -> not find, error
 */
CHAR_T tuya_cfg_get_blue_pin(VOID)
{
    CHECK_LOAD_FINISH();
    
    return sg_device_cfg.b_pin;
}

/**
 * @brief: 获取蓝光驱动IO口有效电平
 * @param: none
 * @note: RGBCW驱动有效电平在IOT平台上配置不允许有不相同的情况，
 *          在定制固件中可以进行个性定制修改
 * @return: 1 -> 高电平有效, 0 -> 低电平有效,  -1 -> not find,error
 */
CHAR_T tuya_cfg_get_blue_pin_active_level(VOID)
{
    CHECK_LOAD_FINISH();
    
    return sg_device_cfg.b_lv;
}

/**
 * @brief: 获取冷光IO口
 * @param: none
 * @return: 冷光驱动IO号, -1 -> not find, error
 */
CHAR_T tuya_cfg_get_cold_pin(VOID)
{
    CHECK_LOAD_FINISH();
    
    return sg_device_cfg.c_pin;
}

/**
 * @brief: 获取冷光驱动IO口有效电平
 * @param: none
 * @note: RGBCW驱动有效电平在IOT平台上配置不允许有不相同的情况，
 *          在定制固件中可以进行个性定制修改
 * @return: 1 -> 高电平有效, 0 -> 低电平有效, -1 -> not find,error
 */
CHAR_T tuya_cfg_get_cold_pin_active_level(VOID)
{
    CHECK_LOAD_FINISH();
    
    return sg_device_cfg.c_lv;
}

/**
 * @brief: 获取暖光IO口
 * @param: none
 * @return: 暖光驱动IO号, -1 -> not find, error
 */
CHAR_T tuya_cfg_get_warm_pin(VOID)
{
    CHECK_LOAD_FINISH();
    
    return sg_device_cfg.w_pin;
}

/**
 * @brief: 获取暖光驱动IO口有效电平
 * @param: none
 * @note: RGBCW驱动有效电平在IOT平台上配置不允许有不相同的情况，
 *          在定制固件中可以进行个性定制修改
 * @return: 1 -> 高电平有效, 0 -> 低电平有效, -1 -> not find,error
 */
CHAR_T tuya_cfg_get_warm_pin_active_level(VOID)
{
    CHECK_LOAD_FINISH();
    
    return sg_device_cfg.w_lv;
}


#if (LIGHT_CFG_SUPPORT_LOWPOWER == 1)
/**
 * @brief: 获取设备是否支持T20低功耗标志位
 * @param: none
 * @note: T20 低功耗功能是指开启之后，可以实现单模组在联网状态下，平均电流小于15mA以下
 * @return: 
 *          0x00 -> don't support lowpower mode
 *          0x01 -> support lowpower mode
 *          -1   -> configure not load, error
 */
CHAR_T tuya_cfg_get_title20_flag(VOID)
{
    CHECK_LOAD_FINISH();
    
    return sg_device_cfg.title20;
}
#endif

#if (LIGHT_CFG_ENABLE_GAMMA == 1)
/**
 * @brief: 获取红光的伽马值
 * @param[in]: index 伽马表的下标，范围0~255
 * @return: 过伽马校准的伽马值
 */
UCHAR_T tuya_cfg_get_gamma_red(IN UCHAR_T index)
{
    CHECK_LOAD_FINISH();
    
    return sg_gamma_red[index];
}

/**
 * @brief: 获取绿光的伽马值
 * @param[in]: 伽马表的下标，范围0~255
 * @return: 经过伽马校准的伽马值
 */
UCHAR_T tuya_cfg_get_gamma_green(IN UCHAR_T index)
{
    CHECK_LOAD_FINISH();
    
    return sg_gamma_green[index];
}

/**
 * @brief: 获取蓝光的伽马值
 * @param[in]: 伽马表的下标，范围0~255
 * @return: 经过伽马校准的伽马值
 */
UCHAR_T tuya_cfg_get_gamma_blue(IN UCHAR_T index)
{
    CHECK_LOAD_FINISH();
    
    return sg_gamma_blue[index];
}

#endif

