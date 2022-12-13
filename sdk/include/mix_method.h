/**
 * @file mix_method.h
 * @brief tuya common api module
 * @version 1.0
 * @date 2019-10-30
 * 
 * @copyright Copyright (c) tuya.inc 2019
 * 
 */
#ifndef _MIX_METHOD_H
#define _MIX_METHOD_H

#include "tuya_cloud_types.h"

#ifdef __cplusplus
    extern "C" {
#endif

/**
 * @brief compare the first n characters of two string
 * 
 * @param[in] s1 the string 1
 * @param[in] s2 the string 2
 * @param[in] n the first n characters
 * 
 * @return 0 one equal, 1 on s1 longer than s2, -1 on s2 longer than s1
 */
int tuya_strncasecmp(const char *s1, const char *s2, size_t n);

/**
 * @brief convert character to hex, if '0'-'f'/'F' -> 0-15, else 0
 * 
 * @param[in] asccode the input character
 * @return the hex value
 */
unsigned char asc2hex(char asccode);

/**
 * @brief convert the character array to hex array
 * 
 * @param[out] hex the out hex array
 * @param[in] ascs the input character array
 * @param[in] srclen the length of input character array
 */
void ascs2hex(unsigned char *hex,unsigned char *ascs,int srclen);

/**
 * @brief convert the input hex array to string array
 * 
 * @param[out] pbDest the out string array
 * @param[in] pbSrc the input hex array
 * @param[in] nLen the length of input hex array
 * 
 * @note the size of <pbDest> must >= nLen * 2, convert example 00-ff -> "00"-"FF"
 */
void hex2str(unsigned char *pbDest, unsigned char *pbSrc, int nLen);
void byte2str(unsigned char *pbDest, unsigned char *pbSrc, int nLen, bool_t upper);

/**
 * @brief find <ch> in <str>, start find in index <revr_index>, find in reverse order.
 * 
 * @param[in] str the string for find
 * @param[in] revr_index the start index to find
 * @param[in] ch the character to find
 * @return -2 on not found, -1 on invalid parameter, >=0 on the position of the character to find 
 */
int str_revr_find_ch(const char *str,const int revr_index,const char ch);

/**
 * @brief sort the char in the buf according ascii value
 * 
 * @param[in] is_ascend sort in sacend order
 * @param[in] buf the buffer need to sort
 * @param[in] buf_cnt the character counts in buffer
 */
void byte_sort(unsigned char is_ascend, unsigned char *buf, int buf_cnt);

/**
 * @brief  duplicate input string, will malloc a new block of memory
 * 
 * @param[in] str the input string need to duplicate
 * @return new string  
 */
char *mm_strdup(const char *str);

/**
 * @brief check the version input is valid
 * 
 * @param[in] ver the version string
 * @return 1 on valid, 0 on invalid 
 */
int is_valid_version(char *ver);

/**
 * @brief convert mac address string to mac address array
 * 
 * @param[in] str the input mac address string
 * @param[out] mac the mac address array
 * @return 0 on success, others on failed.
 * 
 * @warning now only support valid mac address string: xxxxxxxxxxxx / xx-xx-xx-xx-xx-xx
 */
int string2mac(const char *str, char mac[6]);

/**
 * @brief convert version string to integer
 * 
 * @param[in] versionString the input version string
 * @return a integer version
 * 
 * @note  can support version string format x.y.z or x.y, for example: "1.0.1" to 0x010001 and "40.00" t0 0x2800

 */
int versionString2int(const char* versionString);

int tuya_isalpha(int c);
int tuya_tolower(int c);
int tuya_toupper(int c);

#ifdef __cplusplus
}
#endif
#endif

