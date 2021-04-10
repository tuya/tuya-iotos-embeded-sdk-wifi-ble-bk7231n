/*
mix_method.h
Copyright(C),2018-2020, 涂鸦科技 www.tuya.comm
*/
#ifndef _MIX_METHOD_H
#define _MIX_METHOD_H

#include "tuya_cloud_types.h"

#ifdef __cplusplus
    extern "C" {
#endif

#ifdef  __MIX_METHOD_GLOBALS
    #define __MIX_METHOD_EXT
#else
    #define __MIX_METHOD_EXT extern
#endif

/***********************************************************
*************************micro define***********************
***********************************************************/
#define STRCASE_CMP

/***********************************************************
*************************variable define********************
***********************************************************/

/***********************************************************
*************************function define********************
***********************************************************/
#ifndef STRCASE_CMP
/***********************************************************
*  Function: strcasecmp
*  Input && Output && Desc: refer to std strcasecmp
***********************************************************/
__MIX_METHOD_EXT \
int strcasecmp(const char *s1, const char *s2);
#endif

/***********************************************************
*  Function: tuya_strncasecmp
*  Input && Output && Desc: refer to std strncasecmp
***********************************************************/
__MIX_METHOD_EXT \
int tuya_strncasecmp(const char *s1, const char *s2, size_t n);

/***********************************************************
*  Function: asc2hex
*  Desc:     change the character to hex
*            if '0'-'f'/'F' -> 0-15, else 0
*  Input:   asccode: the input character
*  Return:   nLen :  the hex value
***********************************************************/
__MIX_METHOD_EXT \
unsigned char asc2hex(char asccode);

/***********************************************************
*  Function: ascs2hex
*  Desc:     change the character array to hex array.
*            refer asc2hex
*  Output:  hex: the out hex array
*  Input:  ascs: the input character array
*  Input:srclen: the length of input character array
***********************************************************/
__MIX_METHOD_EXT \
void ascs2hex(unsigned char *hex,unsigned char *ascs,int srclen);

/***********************************************************
*  Function: hex2str
*  Desc:     change the input hex array <pbSrc> to string array
*            <pbDest>. the size of <pbDest> must >= nLen * 2
*            change example 00-ff -> "00"-"FF"
*  Output: pbDest: the out string array
*  Input:   pbSrc: the input hex array
*  Input:   nLen : the length of input hex array
***********************************************************/
__MIX_METHOD_EXT \
void hex2str(unsigned char *pbDest, unsigned char *pbSrc, int nLen);

/***********************************************************
*  Function: str_revr_find_ch
*  Desc:     find <ch> in <str>, start find in index <revr_index>,
*            find in reverse order.
*  Input: str: the string for find.
*  Input: revr_index: the start index to find.
*  Input: ch : the character to find.
*  return:
***********************************************************/
__MIX_METHOD_EXT \
int str_revr_find_ch(const char *str,const int revr_index,const char ch);

/***********************************************************
*  Function: byte_sort
*  Desc:     sort the char in the buf.
*  Input: is_ascend : is ascend
*  Input && Output  : buf: the header of the buf array.
*  Input: buf_cnt   : the size of the buf array.
***********************************************************/
__MIX_METHOD_EXT \
void byte_sort(unsigned char is_ascend, unsigned char *buf, int buf_cnt);

/***********************************************************
*  Function: mm_strdup
*  Desc:     duplicate input string and return. use function Malloc
*  Input && Return: refer to std strdup
***********************************************************/
__MIX_METHOD_EXT \
char *mm_strdup(const char *str);

/***********************************************************
*  Function: is_valid_version
*  Desc:     check the version input is valid
*            valid version: xx.xx.xx / xx.xx
*  Input:    ver
*  Return:   1: valid  0: invalid
***********************************************************/
__MIX_METHOD_EXT \
int is_valid_version(char *ver);

/***********************************************************
*  Function: string2mac
*  Desc:     string to mac.
*            valid str: xxxxxxxxxxxx / xx-xx-xx-xx-xx-xx
*  Input:    str
*  Return:   -1: error  0: success
***********************************************************/
__MIX_METHOD_EXT \
int string2mac(const char *str, char mac[6]);

/***********************************************************
*  Function: string2mac
*  Desc:     string to mac.
*            valid str: xxxxxxxxxxxx / xx-xx-xx-xx-xx-xx
*  Input:    str
*  Return:   -1: error  0: success
***********************************************************/
__MIX_METHOD_EXT \
int string2mac(const char *str, char mac[6]);



#ifdef __cplusplus
}
#endif
#endif

