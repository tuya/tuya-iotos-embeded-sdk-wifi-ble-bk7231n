/*
 * @Author: wls
 * @email: wuls@tuya.com
 * @LastEditors:   
 * @file name: light_toolkit.c
 * @Description: light common toolkit
 * @Copyright: HANGZHOU TUYA INFORMATION TECHNOLOGY CO.,LTD
 * @Company: http://www.tuya.com
 * @Date: 2019-05-06 10:57:08
 * @LastEditTime: 2019-06-21 11:18:08
 */

#include "light_types.h"
#include "light_tools.h"
#include "light_printf.h"
#include "math.h"

#define SET_BIT(value, bit) (value |= (1 << bit))
#define CLR_BIT(value, bit) (value &= ~(1 << bit))
#define GET_BIT(value, bit) (value & (1 << bit))

/**
 * @brief: 获取五个数中的最大值
 * @param[in]: a -> number1 
 * @param[in]: b -> number2 
 * @param[in]: c -> number3
 * @param[in]: d -> number4
 * @param[in]: e -> number5 
 * @return: 最大值
 */
UINT_T tuya_light_tool_get_max_value(IN UINT_T a, IN UINT_T b, IN UINT_T c, IN UINT_T d, IN UINT_T e)
{
    UINT_T x = a > b ? a : b;
    UINT_T y = c > d ? c : d;
    UINT_T z = x > y ? x : y;

    return (z > e ? z : e);
}

/**
 * @brief: 获取一个浮点数的绝对值
 * @param[in]: value 浮点数值  
 * @return: value的绝对值
 */
FLOAT_T tuya_light_tool_get_abs_float_value(IN FLOAT_T value)
{
    return (value > 0 ? value : -value);
}

/**
 * @brief: 获取一个整型绝对值
 * @param[in] value 整型值 
 * @return: value的绝对值
 */
UINT_T tuya_light_tool_get_abs_value(IN INT_T value)
{
    return (value > 0 ? value : -value);
}

/**
 * @brief: 获取value的第bit位是0 or 1
 * @param: value bit  
 * @return: 0 or 1
 */
UCHAR_T tuya_light_tool_is_bit_valid(IN UCHAR_T value, IN UCHAR_T bit)
{
    return GET_BIT(value, bit) == 0 ? 0 : 1;
}

/**
 * @brief: 获取10的p幂的值
 * @param[in]: p 幂
 * @return: value 10的p幂 
 */
STATIC UINT_T _light_tool_power10(IN UCHAR_T p)
{
    UINT_T i = 0;
    UINT_T value = 1;
    for (i = 0; i < p; i++) {
        value = value * 10;
    }
    return value;
}

/**
 * @brief: convert dec string to dec hex 字符串型十进制转整型十进制
 * @param[in]:  dec string , like: "123"; 
 * @param[in]:  dec string len; 
 * @param[out]: dec hex, like 0x7B. 
 * @return: dec hex length, 0 ~ 4
 */
UCHAR_T tuya_light_tool_str_to_dec(IN CHAR_T *dec_str, IN UCHAR_T str_len, OUT UINT_T	*p_dec)
{
    if (dec_str == NULL) {
        return 0;
    }
    
    UINT_T i = 0;
    UINT_T dec_value = 0;

    for (i = 0; i < str_len; i++) {
        dec_value += (dec_str[i] - '0') * _light_tool_power10(str_len-i-1);
    }
    
    *p_dec = dec_value;

    if (dec_value <= 0xFF) {
        return 1;
    } else if (dec_value <= 0xFFFF) {
        return 2;
    } else if (dec_value <= 0xFFFFFF) {
        return 3;
    } else if (dec_value <= 0xFFFFFFFF) {
        return 4;
    } else {
        return 0;
    }
}

/**
 * @brief: ASSIC change to hex
 * @param[in]: asc_code -> ASSIC code
 * @return: hex value
 */
UCHAR_T tuya_light_tool_asc_to_hex(IN CHAR_T asc_code)
{
    UCHAR_T result = 0;

    if ('0' <= asc_code && asc_code <= '9') {
        result = asc_code - '0';
    } else if ('a' <= asc_code && asc_code <= 'f') {
        result = asc_code - 'a' + 10;
    } else if ('A' <= asc_code && asc_code <= 'F') {
        result = asc_code - 'A' + 10;
    } else {
        result = 0;
    }

    return result;
}

/**
 * @brief: two unsigned char merge into unsigned char
 * @param[in] H -> high 4 bit  
 * @param[in] L -> low 4 bit 
 * @return: UCHAR_T
 */
UCHAR_T tuya_light_tool_str_to_uchar(IN UCHAR_T H, IN UCHAR_T L)
{
    return((H << 4) | (L & 0x0F));
}

/**
 * @brief: four unsigned char merge into unsigned short
 * @param[in]: HH -> USHORT Hight hight 4bit 
 * @param[in]: HL -> USHORT Hight low 4bit   
 * @param[in]: LH -> USHORT low hight 4bit   
 * @param[in]: LL -> USHORT low low 4bit     
 * @retval: USHORT_T
 */
USHORT_T tuya_light_tool_str_to_ushort(IN UCHAR_T HH, IN UCHAR_T HL, IN UCHAR_T LH, IN UCHAR_T LL)
{
    return ((HH << 12) | (HL << 8) | (LH << 4) | (LL & 0x0F));
}

/**
 * @brief: HSV change to RGB
 * @param[in]:  h -> range 0~360 
 * @param[in]:  s -> range 0~1000
 * @param[in]:  v -> range 0~1000
 * @param[out]: r -> R result,rang from 0~1000
 * @param[out]: g -> G result,rang from 0~1000
 * @param[out]: b -> B result,rang from 0~1000
 * @return: none
 */
VOID tuya_light_tool_hsv_to_rgb(IN USHORT_T h, IN USHORT_T s, IN USHORT_T v, OUT USHORT_T *r, OUT USHORT_T *g, OUT USHORT_T *b)
{
    INT_T i;
    FLOAT_T rgb_min;
    FLOAT_T rgb_max;
    FLOAT_T rgb_adjust;
    INT_T difs;

    if (h >= 360) {
        h = 0;
    }
    
    rgb_max = v * 1.0f;
    rgb_min = rgb_max * (1000 - s) / 1000.0f;

    i = h / 60;
    difs = h % 60; //factorial part of h 


    //RGB adjustment amount by hue
    rgb_adjust = (rgb_max - rgb_min) * difs / 60.0f;

    switch (i) {
        case 0:
            *r = (USHORT_T)rgb_max;
            *g = (USHORT_T)(rgb_min + rgb_adjust);
            *b = (USHORT_T)rgb_min;
            break;
            
        case 1:
            *r = (USHORT_T)(rgb_max - rgb_adjust);
            *g = (USHORT_T)rgb_max;
            *b = (USHORT_T)rgb_min;
            break;
            
        case 2:
            *r = (USHORT_T)rgb_min;
            *g = (USHORT_T)rgb_max;
            *b = (USHORT_T)(rgb_min + rgb_adjust);
            break;
            
        case 3:
            *r = (USHORT_T)rgb_min;
            *g = (USHORT_T)(rgb_max - rgb_adjust);
            *b = (USHORT_T)rgb_max;
            break;
            
        case 4:
            *r = (USHORT_T)(rgb_min + rgb_adjust);
            *g = (USHORT_T)rgb_min;
            *b = (USHORT_T)rgb_max;
            break;
            
        default:        // case 5:
            *r = (USHORT_T)rgb_max;
            *g = (USHORT_T)rgb_min;
            *b = (USHORT_T)(rgb_max - rgb_adjust);
            break;
    }

    if (*r > 1000) {
        *r = 1000;
    }

    if (*g > 1000) {
        *g = 1000;
    }

    if (*b > 1000) {
        *b = 1000;
    }

    return;
}

/**
 * @brief: get the max data in 2 float number
 * @param[in] FLOAT_T a
 * @param[in] FLOAT_T b
 * @return: MAX FLOAT_T
 */
FLOAT_T tuya_light_tool_get_max_float(IN FLOAT_T a, IN FLOAT_T b)
{
    if (a >= b) {
        return a;
    } else {
        return b;
    }
}

/**
 * @brief: get the min data in 2 float number
 * @param[in]: FLOAT_T a
 * @param[in]: FLOAT_T b
 * @return: MIN FLOAT_T
 */
FLOAT_T tuya_light_tool_get_min_float(FLOAT_T a, FLOAT_T b)
{
    if (a <= b) {
        return a;
    } else {
        return b;
    }
}

/**
 * @brief: RGB change to HSV
 * @param[in]:  R -> R,rang from 0~1000
 * @param[in]:  G -> G,rang from 0~1000
 * @param[in]:  B -> B,rang from 0~1000
 * @param[in]:  H -> result, range 0~360
 * @param[out]: S -> result, range 0~1000
 * @param[out]: V -> result, range 0~1000
 * @return: none
 */
VOID tuya_light_tool_rgb_to_hsv(IN USHORT_T R, IN USHORT_T G, IN USHORT_T B, OUT USHORT_T *H, OUT USHORT_T *S, OUT USHORT_T *V)
{
    FLOAT_T r = 0.0f;
    FLOAT_T g = 0.0f;
    FLOAT_T b = 0.0f;
    FLOAT_T min_rgb = 0.0f;
    FLOAT_T max_rgb = 0.0f;
    FLOAT_T delta_rgb = 0.0f;
    FLOAT_T h = 0.0f;
    FLOAT_T s = 0.0f;
    FLOAT_T v = 0.0f;
     
    r = R / 1000.0f;
    g = G / 1000.0f;
    b = B / 1000.0f;

    min_rgb = tuya_light_tool_get_min_float(r,tuya_light_tool_get_min_float(g,b));
    max_rgb = tuya_light_tool_get_max_float(r,tuya_light_tool_get_max_float(g,b));

    delta_rgb = max_rgb - min_rgb;

    v = max_rgb;
    if (max_rgb != 0.0) {
        s = delta_rgb / max_rgb;
    } else {
        s = 0.0;
    }
    
    if (s <= 0.0) {
        h = -1.0f;
    } else {
        if (r == max_rgb) {
            h = (g-b) / delta_rgb;
        } else if (g == max_rgb) {
            h = 2.0 + (b-r) / delta_rgb;
        } else if (b == max_rgb) {
            h = 4.0 + (r-g) / delta_rgb;
        }
    }
    
    h = h * 60.0;
    if (h < 0.0) {
        h += 360;
    }

    *H = (USHORT_T)h;
    *S = (USHORT_T)(s * 1000);
    *V = (USHORT_T)(v * 1000);

    return;
}

/**
 * @brief: 比较两个字符串是否相等
 * @param[in]: str1 -> string 1
 * @param[in]: str2 -> string 2
 * @return: 0: if these two string is not same,
 *          1: these two string is all same
 */
BOOL_T tuya_string_compare(IN CHAR_T *str1, IN CHAR_T *str2)
{
    
    while (*str1 !='\0' && *str2 != '\0') {
        if (*str1 != *str2){
            return 0;
        }
        str1++;
        str2++;
    }
    
    if (*str1 =='\0' && *str2 == '\0') {
        return 1;
    } else {
      return 0;
    }
}

/**
 * @brief: change number to str
 * @param[in]: mode -> change format( 0 -> %d, 4-> %4x)}
 * @param[in]: num -> number(unsigned int)}
 * @param[in]: len -> buf len(sizeof)}
 * @param[out]: str -> string}
 * @return: string
 */
VOID tuya_num_to_str(IN CHAR_T mode, IN UINT_T num, IN UCHAR_T len, OUT CHAR_T *str)
{
    memset(str, 0, len);
    
    switch (mode) {
        case 0:
            snprintf(str, len, "%d", num);
            break;

        case 4:
            snprintf(str, len, "%04x", num);
            break;
            
        default:
            break;
    }
}

/**
 * @brief: compress one uint scene data, from string (26 bytes) to hex (65 bits).
 * compress format:
 * bits: mode:2 bits, time1:7bits, timer2:7bits, H:9bits, S:10 bits, V:10 bits,B:10bits, T:10bits;
 * output_buf[0]: mode_1 + time1
 * output_buf[1]: H_1 + time2
 * output_buf[2]: H_8
 * output_buf[3]: S_2 + V_2 + B_2 + T_2
 * output_buf[4]: S_8
 * output_buf[5]: V_8
 * output_buf[6]: B_8
 * output_buf[7]: T_8
 * @param[in]:  input_str --> one uint data of scene
 * @param[out]: output_buf --> Output buffer address,a unit of compressed data,including speed,transition time,H,S,V,B,T
 * @param[out]: light_mode --> output transition mode of the compressed data
 * @return: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code文档
 */
STATIC OPERATE_LIGHT _scene_data_compress_to_one_uint(CONST IN CHAR_T *input_str, OUT UCHAR_T *output_buf , OUT UCHAR_T *light_mode)
{
    if (NULL == input_str || NULL == output_buf || strlen(input_str) < 26){
        return LIGHT_INVALID_PARM;
    }

    INT_T offset = 0;
    CHAR_T tmp_str[5] = {0};

    //unit  time
    strncpy(tmp_str, input_str + offset, 2);
    USHORT_T time1 = (USHORT_T) strtol(tmp_str, NULL, 16);
    offset += 2;

    //unit stand time
    memset(tmp_str, 0, 5);
    strncpy(tmp_str, input_str + offset, 2);
    USHORT_T time2 = (USHORT_T) strtol(tmp_str, NULL, 16);
    offset += 2;

    //unit change mode
    memset(tmp_str, 0, 5);
    strncpy(tmp_str, input_str + offset, 2);
    USHORT_T mode = (USHORT_T) strtol(tmp_str, NULL, 16);
    offset += 2;

    //Hue
    memset(tmp_str, 0, 5);
    strncpy(tmp_str, input_str + offset, 4);
    USHORT_T val_H = (USHORT_T) strtol(tmp_str, NULL, 16);
    offset += 4;

    //Saturation
    memset(tmp_str, 0, 5);
    strncpy(tmp_str, input_str + offset, 4);
    USHORT_T val_S = (USHORT_T) strtol(tmp_str, NULL, 16);
    offset += 4;

    //Value
    memset(tmp_str, 0, 5);
    strncpy(tmp_str, input_str + offset, 4);
    USHORT_T val_V = (USHORT_T) strtol(tmp_str, NULL, 16);
    offset += 4;

    //Bright
    memset(tmp_str, 0, 5);
    strncpy(tmp_str, input_str + offset, 4);
    USHORT_T val_L = (USHORT_T) strtol(tmp_str, NULL, 16);
    offset += 4;

    //Temperature
    memset(tmp_str, 0, 5);
    strncpy(tmp_str, input_str + offset, 4);
    USHORT_T val_T = (USHORT_T) strtol(tmp_str, NULL, 16);
    offset += 4;
    output_buf[0] = ((mode & 0x1) << 7) | (time1 & 0x7F);
    output_buf[1] = (((val_H >> 8) & 0x1) << 7) | (time2 & 0x7F);
    output_buf[2] = (val_H & 0xFF);
    output_buf[3] = (((val_S >> 8) & 0x3) << 6) | (((val_V >> 8) & 0x3) << 4) | (((val_L >> 8) & 0x3) << 2) | ((val_T >> 8) & 0x3);
    output_buf[4] = (val_S & 0xFF);
    output_buf[5] = (val_V & 0xFF);
    output_buf[6] = (val_L & 0xFF);
    output_buf[7] = (val_T & 0xFF);

    *light_mode = mode;
    
    return LIGHT_OK;
}

/**
 * @brief: compress scene data,from string (max 2+26*8=210 bytes) to hex (8+8+64*8)bits = 66 bytes).
 * compressed data format:
 * scene id + mode + uint*8
 * @param[in]: input_str --> scene data string format
 * @param[out] output_buf --> Output buffer address, compressed data.
 * @param[out] output_buf_len --> output compressed data length
 * @return: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code文档
 */
OPERATE_LIGHT tuya_scene_data_compress(CONST IN CHAR_T *input_str, OUT UCHAR_T *output_buf, OUT UINT_T *output_buf_len)
{
    OPERATE_LIGHT ret = -1;
    INT_T str_len ; 
    INT_T i = 0;
    UCHAR_T light_mode_tmp = 0;
    CHAR_T tmp_str[5] = {0};
    USHORT_T num;
    
    if (input_str == NULL) {
        return LIGHT_INVALID_PARM;
    }

    str_len = strlen(input_str);
    if ((0 != (str_len % 2)) || (2 != (str_len % 26))) {
        return -1;
    }

    for (i = 0; (i * 26 + 2) < str_len; i++) {
        ret = _scene_data_compress_to_one_uint(input_str + 2 + i * 26, output_buf + 2 + i * 8, &light_mode_tmp);
        if (ret != LIGHT_OK) {
            PR_ERR("scene compress error!");
        }
        output_buf[1] |= (((light_mode_tmp >> 1) & 0x1) << i);
        light_mode_tmp = 0;
    }

    //fill scene number
    strncpy(tmp_str, input_str, 2);
    num = (USHORT_T) strtol(tmp_str, NULL, 16);
    output_buf[0] =  (num & 0xFF);

    //calc scene length
    *output_buf_len = i * 8 + 2;

    return LIGHT_OK;
}

/**
 * @brief: decompress one uint scene data, to string format
 * @param[in]: input_buf --> scene compressed data
 * @param[in]: light_mode --> mode value
 * @param[out]: output_str --> output compressed data length
 * @return: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code文档
 */
STATIC OPERATE_LIGHT _decompress_scene_data_one_uint(CONST IN UCHAR_T *input_buf, CONST OUT UCHAR_T light_mode, OUT CHAR_T * output_str)
{
    CHAR_T output_str_tmp[27] = {0};
    
    if (NULL == input_buf || NULL == output_str) {
        return LIGHT_INVALID_PARM;
    }

    USHORT_T mode = ((light_mode & 0x1) << 1) | ((input_buf[0] >> 7) & 0x1);
    USHORT_T time1 = (input_buf[0] & 0x7F);
    USHORT_T time2 = (input_buf[1] & 0x7F);
    USHORT_T val_H = (((input_buf[1] >> 7) & 0x1) << 8) | (input_buf[2] & 0xFF);
    USHORT_T val_S = (input_buf[4] & 0xFF) | (((input_buf[3] >> 6) & 0x3) << 8);
    USHORT_T val_V = (input_buf[5] & 0xFF) | (((input_buf[3] >> 4) & 0x3) << 8);
    USHORT_T val_L = (input_buf[6] & 0xFF) | (((input_buf[3] >> 2) & 0x3) << 8);
    USHORT_T val_T = (input_buf[7] & 0xFF) | ((input_buf[3] & 0x3) << 8);

    memset(output_str_tmp, 0, 27);
    snprintf(output_str_tmp, 27, "%02x%02x%02x%04x%04x%04x%04x%04x", time1, time2, mode, val_H, val_S, val_V, val_L, val_T);
    strncpy(output_str, output_str_tmp, 26);
    
    return LIGHT_OK;
}

/**
 * @brief: decompress scene data to string format
 * @param[in]: input_buf --> compressed scene data
 * @param[in]: input_buf_len --> compressed scene data length
 * @param[in]: output_str --> decompressed scene data,string format
 * @return: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code文档
 */
OPERATE_LIGHT tuya_scene_data_decompress(CONST IN UCHAR_T* input_buf, CONST IN UINT_T input_buf_len, OUT CHAR_T* output_str)
{
    OPERATE_LIGHT ret = -1;
    INT_T i = 0;
    CHAR_T output_str_tmp[27] = {0}; 
    UCHAR_T light_mode_tmp = 0;
    
    if (NULL == input_buf || NULL == output_str || 2 != (input_buf_len % 8)) {
        return LIGHT_INVALID_PARM;
    }
    
    //fill scene number
    snprintf(output_str, 3, "%02x", input_buf[0]);
    
    //fill scene unit
    for (i = 0; (i * 8 + 2) < input_buf_len; i++) {
        memset(output_str_tmp, 0, 27);

        light_mode_tmp = ((input_buf[1] >> i) & 0x1);

        ret = _decompress_scene_data_one_uint(input_buf + 2 + (i * 8), light_mode_tmp, output_str_tmp);
        if (ret != LIGHT_OK) {
            PR_ERR("scene decompress error!");
        }

        strncpy(output_str + 2 + (i * 26), output_str_tmp, 26);

        light_mode_tmp = 0;
    }

    return LIGHT_OK;
}

#if (LIGHT_CFG_GAMMA_CAL == 1)
/**
 * @brief: 
 * @param[in]: color_bri -> gamma index}
 * @param[in]: FLOAT_T gama_gain -> gamma calc gain
 * @param[in]: FLOAT_T gama_white -> gamma calc white balance param
 * @return: UCHAR_T
 */
UCHAR_T tuya_light_tool_calculate_gamma(IN UCHAR_T color_bri, IN FLOAT_T gama_gain, IN FLOAT_T gama_white)
{

    UCHAR_T result;
    DOUBLE_T temp;

    double p = 255 / pow(255, gama_gain);

    p = (double)pow(1 / p, 1 / gama_gain);

    temp = (double)(color_bri);

    temp = p * gama_white * (double)pow(temp, 1 / gama_gain);

    result = temp + 0.5;

    return result;
}
#endif

