/*
 * @File name: tuya_dp_upload.h
 * @Author: caojq
 * @Version: V1.0.0
 * @Date: 2020-10-28 14:50:40
 * @Description: dp点数据上报相关函数接口，主要包括上报数据结构创建、删除、各种类型数据赋值等
 * @Others: dp点数据上报相关数据结构
 * @LastEditTime: 2020-10-28 16:49:58
 * @LastEditors:  
 */

#ifndef __TUYA_DP_UPLOAD_H
#define __TUYA_DP_UPLOAD_H

#include "tuya_cloud_types.h"
#include "tuya_cloud_com_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __TUYA_DP_UPLOAD_GLOBAL
    #define __TUYA_DP_UPLOAD_EXT
#else
    #define __TUYA_DP_UPLOAD_EXT extern
#endif

/*dp upload data moudle*/
typedef struct {
    UINT_T dp_num;                              ///<dp num ,who needed upload
    UINT_T cur_index;                           //<current index
    TY_OBJ_DP_S *dp_buf;                        ///<upload dp buffer
}DP_UPLOAD_S;

/**
 * @Function: tuya_upload_alloc_space
 * @Description: dp点上报数据结构申请内存空间
 * @Input: dp_num：上报dp点个数
 * @Output: none
 * @Return: DP_UPLOAD_S:返回数据上报结构指针
 * @Others: none
 */
__TUYA_DP_UPLOAD_EXT \
DP_UPLOAD_S* tuya_upload_alloc_space(UINT_T dp_num);

/**
 * @Function: tuya_upload_delete_space
 * @Description: 删除dp点上报结构体内存空间
 * @Input: buf：dp点数据结构体指针
 * @Output: none
 * @Return: none
 * @Others: none
 */
__TUYA_DP_UPLOAD_EXT \
VOID tuya_upload_delete_space(DP_UPLOAD_S *buf);

/**
 * @Function: tuya_upload_dp_enum_frame
 * @Description:枚举型上报数据赋值
 * @Input: dpid：上报数据dp点；dp_enum：枚举数据
 * @Output: upload_dp：dp点上报数据结构
 * @Return: none
 * @Others: none
 */
__TUYA_DP_UPLOAD_EXT \
VOID tuya_upload_dp_enum_frame(IN OUT DP_UPLOAD_S *upload_dp,UCHAR_T dpid, UINT_T dp_enum);

/**
 * @Function: tuya_upload_dp_value_frame
 * @Description: 数值型上报数据赋值
 * @Input: dpid：上报数据dp点；dp_value：数值型数据
 * @Output: upload_dp：dp点上报数据结构
 * @Return: none
 * @Others: none
 */
__TUYA_DP_UPLOAD_EXT \
VOID tuya_upload_dp_value_frame(IN OUT DP_UPLOAD_S *upload_dp,UCHAR_T dpid, INT_T dp_value);

/**
 * @Function: tuya_upload_dp_bool_frame
 * @Description: bool型上报数据赋值
 * @Input: dpid：上报数据dp点；dp_bool：bool数据
 * @Output: upload_dp：dp点上报数据结构
 * @Return: none
 * @Others: none
 */
__TUYA_DP_UPLOAD_EXT \
VOID tuya_upload_dp_bool_frame(IN OUT DP_UPLOAD_S *upload_dp,UCHAR_T dpid, BOOL_T dp_bool);

/**
 * @Function: tuya_upload_dp_string_frame
 * @Description: 字符串型上报数据赋值
 * @Input: dpid：上报数据dp点；dp_str：字符型数据
 * @Output: upload_dp：dp点上报数据结构
 * @Return: none
 * @Others: none
 */
 __TUYA_DP_UPLOAD_EXT \
VOID tuya_upload_dp_string_frame(IN OUT DP_UPLOAD_S *upload_dp,IN UCHAR_T dpid, IN CHAR_T *dp_str);

/**
 * @Function: tuya_upload_dp_bitmap_frame
 * @Description: 位图型上报数据赋值
 * @Input: dpid：上报数据dp点
 * @Input: dp_bitmap:位图型数据
 * @Output: upload_dp：dp点上报数据结构
 * @Return: none
 * @Others: none
 */
 __TUYA_DP_UPLOAD_EXT \
VOID tuya_upload_dp_bitmap_frame(IN OUT DP_UPLOAD_S *upload_dp,IN UCHAR_T dpid, IN UINT_T dp_bitmap);

#ifdef __cplusplus
}
#endif
#endif