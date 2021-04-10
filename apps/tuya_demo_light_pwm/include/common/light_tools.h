/*
 * @Author: wls
 * @email: wuls@tuya.com
 * @LastEditors:   
 * @file name: light_toolkit.h
 * @Description: light common tool include file
 * @Copyright: HANGZHOU TUYA INFORMATION TECHNOLOGY CO.,LTD
 * @Company: http://www.tuya.com
 * @Date: 2019-04-26 13:55:40
 * @LastEditTime: 2019-05-28 15:24:01
 */

#ifndef __LIHGT_TOOLS_H__
#define __LIHGT_TOOLS_H__

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#include "light_types.h"
#include "light_cfg.h"

#define size_get(x) (sizeof(x) / sizeof(x[0]))

/**
 * @brief: 获取五个数中的最大值
 * @param[in]: a -> number1 
 * @param[in]: b -> number2 
 * @param[in]: c -> number3
 * @param[in]: d -> number4
 * @param[in]: e -> number5 
 * @return: 最大值
 */
UINT_T tuya_light_tool_get_max_value(IN UINT_T a, IN UINT_T b, IN UINT_T c, IN UINT_T d, IN UINT_T e);

/**
 * @brief: 获取一个浮点数的绝对值
 * @param[in]: value 浮点数值  
 * @return: value的绝对值
 */
FLOAT_T tuya_light_tool_get_abs_float_value(IN FLOAT_T value);

/**
 * @brief: 获取一个整型绝对值
 * @param[in] value 整型值 
 * @return: value的绝对值
 */
UINT_T tuya_light_tool_get_abs_value(IN INT_T value);

/**
 * @brief: 获取value的第bit位是0 or 1
 * @param: value bit  
 * @return: 0 or 1
 */
UCHAR_T tuya_light_tool_is_bit_valid(IN UCHAR_T value, IN UCHAR_T bit);

/**
 * @brief: convert dec string to dec hex 字符串型十进制转整型十进制
 * @param[in]:  dec string , like: "123"; 
 * @param[in]:  dec string len; 
 * @param[out]: dec hex, like 0x7B. 
 * @return: dec hex length, 0 ~ 4
 */
UCHAR_T tuya_light_tool_str_to_dec(IN CHAR_T *dec_str, IN UCHAR_T str_len, OUT UINT_T	*p_dec);

/**
 * @brief: ASSIC change to hex
 * @param[in]: asc_code -> ASSIC code
 * @return: hex value
 */
UCHAR_T tuya_light_tool_asc_to_hex(IN CHAR_T asc_code);

/**
 * @brief: two unsigned char merge into unsigned char
 * @param[in] H -> high 4 bit  
 * @param[in] L -> low 4 bit 
 * @return: UCHAR_T
 */
UCHAR_T tuya_light_tool_str_to_uchar(IN UCHAR_T H, IN UCHAR_T L);

/**
 * @brief: four unsigned char merge into unsigned short
 * @param[in]: HH -> USHORT Hight hight 4bit 
 * @param[in]: HL -> USHORT Hight low 4bit   
 * @param[in]: LH -> USHORT low hight 4bit   
 * @param[in]: LL -> USHORT low low 4bit     
 * @retval: USHORT_T
 */
USHORT_T tuya_light_tool_str_to_ushort(IN UCHAR_T HH, IN UCHAR_T HL, IN UCHAR_T LH, IN UCHAR_T LL);

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
VOID tuya_light_tool_hsv_to_rgb(IN USHORT_T h, IN USHORT_T s, IN USHORT_T v, OUT USHORT_T *r, OUT USHORT_T *g, OUT USHORT_T *b);

/**
 * @brief: get the max data in 2 float number
 * @param[in] FLOAT_T a
 * @param[in] FLOAT_T b
 * @return: MAX FLOAT_T
 */
FLOAT_T tuya_light_tool_get_max_float(IN FLOAT_T a, IN FLOAT_T b);

/**
 * @brief: get the min data in 2 float number
 * @param[in]: FLOAT_T a
 * @param[in]: FLOAT_T b
 * @return: MIN FLOAT_T
 */
FLOAT_T tuya_light_tool_get_min_float(FLOAT_T a, FLOAT_T b);

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
VOID tuya_light_tool_rgb_to_hsv(IN USHORT_T R, IN USHORT_T G, IN USHORT_T B, OUT USHORT_T *H, OUT USHORT_T *S, OUT USHORT_T *V);

/**
 * @brief: 比较两个字符串是否相等
 * @param[in]: str1 -> string 1
 * @param[in]: str2 -> string 2
 * @return: 0: if these two string is not same,
 *          1: these two string is all same
 */
BOOL_T tuya_string_compare(IN CHAR_T *str1, IN CHAR_T *str2);

/**
 * @brief: change number to str
 * @param[in]: mode -> change format( 0 -> %d, 4-> %4x)}
 * @param[in]: num -> number(unsigned int)}
 * @param[in]: len -> buf len(sizeof)}
 * @param[out]: str -> string}
 * @return: string
 */
VOID tuya_num_to_str(IN CHAR_T mode, IN UINT_T num, IN UCHAR_T len, OUT CHAR_T *str);

/**
 * @brief: compress scene data,from string (max 2+26*8=210 bytes) to hex (8+8+64*8)bits = 66 bytes).
 * compressed data format:
 * scene id + mode + uint*8
 * @param[in]: input_str --> scene data string format
 * @param[out] output_buf --> Output buffer address, compressed data.
 * @param[out] output_buf_len --> output compressed data length
 * @return: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code文档
 */
OPERATE_LIGHT tuya_scene_data_compress(CONST IN CHAR_T *input_str, OUT UCHAR_T *output_buf, OUT UINT_T *output_buf_len);

/**
 * @brief: decompress scene data to string format
 * @param[in]: input_buf --> compressed scene data
 * @param[in]: input_buf_len --> compressed scene data length
 * @param[in]: output_str --> decompressed scene data,string format
 * @return: OPERATE_LIGHT 0表示成功, 非0请参照tuya error code文档
 */
OPERATE_LIGHT tuya_scene_data_decompress(CONST IN UCHAR_T* input_buf, CONST IN UINT_T input_buf_len, OUT CHAR_T* output_str);

#if (LIGHT_CFG_GAMMA_CAL == 1)
/**
 * @brief: 
 * @param[in]: color_bri -> gamma index}
 * @param[in]: FLOAT_T gama_gain -> gamma calc gain
 * @param[in]: FLOAT_T gama_white -> gamma calc white balance param
 * @return: UCHAR_T
 */
UCHAR_T tuya_light_tool_calculate_gamma(IN UCHAR_T color_bri, IN FLOAT_T gama_gain, IN FLOAT_T gama_white);
#endif


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif  /* __LIHGT_TOOLS_H__ */
