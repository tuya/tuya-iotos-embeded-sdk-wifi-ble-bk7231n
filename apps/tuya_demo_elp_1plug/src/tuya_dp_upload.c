/*
 * @File name: tuya_dp_upload.c
 * @Author: caojq
 * @Version: V1.0.0
 * @Date: 2020-10-28 14:46:09
 * @Description: dp点数据上报相关函数功能实现，主要包括上报数据结构创建、删除、各种类型数据赋值等；
 * @Others: 
 * @LastEditTime: 2020-10-28 16:49:38
 * @LastEditors:  
 */

#define __TUYA_DP_UPLOAD_GLOBAL
#include "tuya_dp_upload.h"
#include "uni_log.h"

/**
 * @Function: tuya_upload_alloc_space
 * @Description: dp点上报数据结构申请内存空间
 * @Input: dp_num：上报dp点个数
 * @Output: none
 * @Return: DP_UPLOAD_S:返回数据上报结构指针
 * @Others: none
 */
DP_UPLOAD_S* tuya_upload_alloc_space(UINT_T dp_num)
{
    TY_OBJ_DP_S *obj_dp = NULL;
    DP_UPLOAD_S *upload_dp = NULL;

    if (0 == dp_num)
    {
        PR_NOTICE("dp num is 0,do not upload");
        return NULL;
    }

    upload_dp = (DP_UPLOAD_S*)Malloc(SIZEOF(DP_UPLOAD_S));
    if (NULL == upload_dp)
    {
        PR_ERR("malloc upload_dp failed!");
        return NULL;
    }
    memset(upload_dp,0, SIZEOF(DP_UPLOAD_S));

    obj_dp = (TY_OBJ_DP_S*)Malloc(dp_num*SIZEOF(TY_OBJ_DP_S));
    if(NULL == obj_dp)
    {
        PR_ERR("malloc obj_dp failed!");
        Free(upload_dp);
        return NULL;
    }
    memset(obj_dp, 0, dp_num*SIZEOF(TY_OBJ_DP_S));

    upload_dp->dp_num  = dp_num;
    upload_dp->cur_index = 0;
    upload_dp->dp_buf  = obj_dp;

    return upload_dp;
}

/**
 * @Function: tuya_upload_delete_space
 * @Description: 删除dp点上报结构体内存空间
 * @Input: buf：dp点数据结构体指针
 * @Output: none
 * @Return: none
 * @Others: none
 */
VOID tuya_upload_delete_space(DP_UPLOAD_S *buf)
{
    if(NULL == buf)
    {
        PR_NOTICE("buf null,returned");
        return;
    }

    UINT_T index = 0;
    TY_OBJ_DP_S *obj_dp = NULL;

    for(index = 0; index < buf->dp_num; index++) {
        obj_dp = &(buf->dp_buf[index]);
        if(NULL != obj_dp) {
            if((PROP_STR == obj_dp->type) && (NULL != obj_dp->value.dp_str)) {
                Free(obj_dp->value.dp_str);
            }
        }
    }

    Free(buf->dp_buf);
    buf->dp_buf = NULL;
    Free(buf);
    buf = NULL;

    return;
}

/**
 * @Function: tuya_upload_dp_enum_frame
 * @Description:枚举型上报数据赋值
 * @Input: dpid：上报数据dp点；dp_enum：枚举数据
 * @Output: upload_dp：dp点上报数据结构
 * @Return: none
 * @Others: none
 */
VOID tuya_upload_dp_enum_frame(IN OUT DP_UPLOAD_S *upload_dp,UCHAR_T dpid, UINT_T dp_enum)
{
    TY_OBJ_DP_S *obj_dp = NULL;

    if(NULL == upload_dp)
    {
      return;
    }

    if (upload_dp->cur_index >= upload_dp->dp_num)
    {
      PR_ERR("upload_dp buf is full!");
      return;
    }

    obj_dp = &(upload_dp->dp_buf[upload_dp->cur_index]);
    obj_dp->type    = PROP_ENUM;
    obj_dp->dpid    = dpid;
    obj_dp->value.dp_enum = dp_enum;
    upload_dp->cur_index++;

    return;
}

/**
 * @Function: tuya_upload_dp_bool_frame
 * @Description: bool型上报数据赋值
 * @Input: dpid：上报数据dp点；dp_bool：bool数据
 * @Output: upload_dp：dp点上报数据结构
 * @Return: none
 * @Others: none
 */
VOID tuya_upload_dp_bool_frame(IN OUT DP_UPLOAD_S *upload_dp,UCHAR_T dpid, BOOL_T dp_bool)
{
    TY_OBJ_DP_S *obj_dp = NULL;

    if (NULL == upload_dp)
    {
        return;
    }

    if (upload_dp->cur_index >= upload_dp->dp_num)
    {
        PR_ERR("upload_dp buf is full!!");
        return;
    }

    obj_dp = &(upload_dp->dp_buf[upload_dp->cur_index]);
    obj_dp->type    = PROP_BOOL;
    obj_dp->dpid    = dpid;
    obj_dp->value.dp_bool = dp_bool;
    upload_dp->cur_index++;

    return;
}

/**
 * @Function: tuya_upload_dp_value_frame
 * @Description: 数值型上报数据赋值
 * @Input: dpid：上报数据dp点；dp_value：数值型数据
 * @Output: upload_dp：dp点上报数据结构
 * @Return: none
 * @Others: none
 */
VOID tuya_upload_dp_value_frame(IN OUT DP_UPLOAD_S *upload_dp,UCHAR_T dpid, INT_T dp_value)
{
    TY_OBJ_DP_S *obj_dp = NULL;

    if(NULL == upload_dp)
    {
        return;
    }

    if(upload_dp->cur_index >= upload_dp->dp_num)
    {
        PR_ERR("upload_dp buf is full!!");
        return;
    }

    obj_dp = &(upload_dp->dp_buf[upload_dp->cur_index]);
    obj_dp->type    = PROP_VALUE;
    obj_dp->dpid    = dpid;
    obj_dp->value.dp_value = dp_value;
    upload_dp->cur_index++;

    return;
}

/**
 * @Function: tuya_upload_dp_string_frame
 * @Description: 字符串型上报数据赋值
 * @Input: dpid：上报数据dp点；dp_str：字符型数据
 * @Output: upload_dp：dp点上报数据结构
 * @Return: none
 * @Others: none
 */
VOID tuya_upload_dp_string_frame(IN OUT DP_UPLOAD_S *upload_dp,IN UCHAR_T dpid, IN CHAR_T *dp_str)
{
    TY_OBJ_DP_S *obj_dp = NULL;

    if(NULL == upload_dp) {
        return;
    }

    if(upload_dp->cur_index >= upload_dp->dp_num) {
        PR_ERR("upload_dp buf is full!!");
        return;
    }

    obj_dp = &(upload_dp->dp_buf[upload_dp->cur_index]);
    obj_dp->type    = PROP_STR;
    obj_dp->dpid    = dpid;

    if(NULL != dp_str) {
        obj_dp->value.dp_str = Malloc(strlen(dp_str)+1);
        if(NULL == obj_dp->value.dp_str) {
            PR_ERR("malloc failed");
            return;
        }
        strcpy(obj_dp->value.dp_str, dp_str);
    }else{
        obj_dp->value.dp_str = NULL;
   }

    upload_dp->cur_index++;

    return;
}

/**
 * @Function: tuya_upload_dp_bitmap_frame
 * @Description: 位图型上报数据赋值
 * @Input: dpid：上报数据dp点
 * @Input: dp_bitmap:位图型数据
 * @Output: upload_dp：dp点上报数据结构
 * @Return: none
 * @Others: none
 */
VOID tuya_upload_dp_bitmap_frame(IN OUT DP_UPLOAD_S *upload_dp,IN UCHAR_T dpid, IN UINT_T dp_bitmap)
{
    TY_OBJ_DP_S *obj_dp = NULL;

    if(NULL == upload_dp) {
        return;
    }

    if(upload_dp->cur_index >= upload_dp->dp_num) {
        PR_ERR("upload_dp buf is full!!");
        return;
    }

    obj_dp = &(upload_dp->dp_buf[upload_dp->cur_index]);
    obj_dp->type    = PROP_BITMAP;
    obj_dp->dpid    = dpid;
    obj_dp->value.dp_bitmap = dp_bitmap;

    upload_dp->cur_index++;

    return;
}

