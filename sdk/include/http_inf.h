/***********************************************************
*  File: http_inf.h
*  Author: nzy
*  Date: 20161028
***********************************************************/
#ifndef _HTTP_INF_H
#define _HTTP_INF_H


#include "tuya_cloud_types.h"
#include "httpc.h"
#include "log_seq.h"


#ifdef __cplusplus
extern "C" {
#endif


#ifdef  __HTTP_INF_GLOBALS
    #define __HTTP_INF_EXT
#else
    #define __HTTP_INF_EXT extern
#endif

/***********************************************************
*************************micro define***********************
***********************************************************/
typedef INT_T (*HTTP_DATA_RECV)(PVOID_T hand,PVOID_T buf,UINT_T len);

typedef struct {
    BOOL_T chunked;
    UINT_T content_len; // if(chunked == FALSE) then valid.
    PVOID_T hand;
    HTTP_DATA_RECV recv; // system auto fill
    PVOID_T *pri_data; // private data

    INT_T status_code;

    CHAR_T *p_decode_key;

}HTTP_INF_H_S;

// user http respond callback
typedef OPERATE_RET (*HTTP_INF_CB)(HTTP_INF_H_S *hand);

typedef struct {
    BYTE_T total;
    BYTE_T cnt;
    CHAR_T *pos[0];
}HTTP_PARAM_H_S;

#define DEF_URL_LEN 1024
typedef struct {
    HTTP_PARAM_H_S *param_h; // param handle
    CHAR_T *param_in; // param insert pos
    USHORT_T head_size; // head_size == "url?" or "url"
    USHORT_T buf_len; // bufer len
    CHAR_T buf[0]; // "url?key=value" + "kev=value statistics"
}HTTP_URL_H_S;

/***********************************************************
*************************variable define********************
***********************************************************/

/***********************************************************
*************************function define********************
***********************************************************/
/***********************************************************
*  Function: http_inf_client_get
*  Input: url callback
*  Output: pri_data
*  Return: OPERATE_RET
***********************************************************/
__HTTP_INF_EXT \
OPERATE_RET http_inf_client_get(IN CONST CHAR_T *url,\
                                IN CONST HTTP_INF_CB callback,\
                                INOUT PVOID_T *pri_data);


/***********************************************************
*  Function: http_inf_client_get
*  Input: url callback
*  Output: pri_data
*  Return: OPERATE_RET
***********************************************************/
__HTTP_INF_EXT \
OPERATE_RET http_inf_client_get_file(IN CONST CHAR_T *url,\
                                IN CONST HTTP_INF_CB callback,\
                                INOUT PVOID_T *pri_data, \
                                IN CONST UINT_T file_offset,\
                                IN CONST UINT_T file_size);

#define http_inf_client_post(url, callback, data, len, add_head_cb, add_head_data, p_decode_key, pri_data) \
    http_inf_client_post_field(url, callback, data, len, add_head_cb, add_head_data, p_decode_key, pri_data, \
        STANDARD_HDR_FLAGS|HDR_ADD_CONN_KEEP_ALIVE|HDR_ADD_CONTENT_TYPE_FORM_URLENCODE)

OPERATE_RET http_inf_client_post_field(IN CONST CHAR_T *url,\
                                       IN CONST HTTP_INF_CB callback,\
                                       IN CONST BYTE_T *data,\
                                       IN CONST UINT_T len,\
                                       IN HTTP_HEAD_ADD_CB add_head_cb, \
                                       IN VOID* add_head_data,\
                                       IN CONST CHAR_T *p_decode_key,\
                                       INOUT PVOID_T *pri_data, \
                                       IN CONST http_hdr_field_sel_t field_flags);

#define http_inf_client_post_raw(url, len, p_inf)   \
    http_inf_client_post_raw_field(url, len, p_inf, \
        HRD_ADD_HTTP_RAW | STANDARD_HDR_FLAGS | HDR_ADD_CONN_KEEP_ALIVE | HDR_ADD_CONTENT_TYPE_JSON)

__HTTP_INF_EXT \
OPERATE_RET http_inf_client_post_raw_field(IN CONST CHAR_T *url,\
                                   IN CONST UINT_T len,\
                                   INOUT HTTP_INF_H_S *p_inf,\
                                   IN http_hdr_field_sel_t field_flags);
__HTTP_INF_EXT \
OPERATE_RET http_inf_client_get_raw(IN CONST CHAR_T *url,\
                                     INOUT HTTP_INF_H_S *p_inf,\
                                    IN UINT_T offset, IN UINT_T total_len);

__HTTP_INF_EXT \
OPERATE_RET http_inf_client_put_raw(IN CONST CHAR_T *url,\
                                    IN INT_T len, \
                                    INOUT HTTP_INF_H_S *p_inf);

__HTTP_INF_EXT \
INT_T http_inf_write_content_raw(INOUT HTTP_INF_H_S *p_inf,\
                                         IN CONST BYTE_T *p_data, \
                                         IN CONST UINT_T len,\
                                         BOOL_T chunked );

__HTTP_INF_EXT \
OPERATE_RET httpc_inf_read_header_raw(INOUT HTTP_INF_H_S *p_inf);

__HTTP_INF_EXT \
INT_T httpc_inf_read_content_raw(IN HTTP_INF_H_S *p_inf, IN BYTE_T *buf, IN CONST UINT_T buf_len);

__HTTP_INF_EXT \
OPERATE_RET http_inf_close_raw(INOUT HTTP_INF_H_S *p_inf);

__HTTP_INF_EXT \
OPERATE_RET http_inf_client_add_head_cb_post(IN CONST char *url,\
                              IN CONST HTTP_INF_CB callback,\
                              IN CONST BYTE_T *data,\
                              IN CONST UINT_T len,\
                              IN HTTP_HEAD_ADD_CB add_head_cb,\
                              IN VOID* add_head_data,\
                              INOUT PVOID_T *pri_data);

/***********************************************************
*  Function: http_inf_client_put
*  Input: url callback
*  Output: none
*  Return: OPERATE_RET
***********************************************************/

OPERATE_RET http_inf_client_put(IN CONST CHAR_T *url, \
                                    IN CONST HTTP_INF_CB callback, \
                                    IN CONST BYTE_T *data, \
                                    IN CONST UINT_T len, \
                                    IN HTTP_HEAD_ADD_CB add_head_cb, \
                                    IN VOID* add_head_data,\
                                    INOUT PVOID_T *pri_data);

OPERATE_RET http_inf_client_put_custom_content(IN CONST CHAR_T *url, \
                                               IN CONST HTTP_INF_CB callback, \
                                               IN http_custom_content_ctx_s *p_ctx,\
                                               IN HTTP_HEAD_ADD_CB add_head_cb, IN VOID* add_head_data,\
                                               INOUT PVOID_T *pri_data);

/***********************************************************
*  Function: create_http_url_h
*  Input: buf_len: if len == 0 then use DEF_URL_LEN
*         param_cnt
*  Output:
*  Return: HTTP_URL_H_S *
***********************************************************/
__HTTP_INF_EXT \
HTTP_URL_H_S *create_http_url_h(IN CONST USHORT_T buf_len,\
                                IN CONST USHORT_T param_cnt);


/***********************************************************
*  Function: del_http_url_h
*  Input: hu_h
*  Output:
*  Return: HTTP_URL_H_S *
***********************************************************/
__HTTP_INF_EXT \
VOID del_http_url_h(IN HTTP_URL_H_S *hu_h);

/***********************************************************
*  Function: fill_url_head
*  Input: hu_h:http url handle
*         url_h:url head
*  Output: hu_h
*  Return: OPERATE_RET
***********************************************************/
__HTTP_INF_EXT \
OPERATE_RET fill_url_head(INOUT HTTP_URL_H_S *hu_h,\
                                 IN CONST CHAR_T *url_h);

/***********************************************************
*  Function: fill_url_param
*  Input: hu_h
*         key
*         value
*  Output: hu_h
*  Return: OPERATE_RET
***********************************************************/
__HTTP_INF_EXT \
OPERATE_RET fill_url_param(INOUT HTTP_URL_H_S *hu_h,\
                           IN CONST CHAR_T *key,\
                           IN CONST CHAR_T *value);
__HTTP_INF_EXT \
VOID is_http_dns_set(UCHAR_T val);

__HTTP_INF_EXT \
UCHAR_T is_http_dns_get(VOID);

#ifdef __cplusplus
}
#endif
#endif

