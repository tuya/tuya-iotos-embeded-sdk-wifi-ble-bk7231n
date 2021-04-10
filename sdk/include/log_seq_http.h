/***********************************************************
*  File: log_seq_http.h
*  Author: nzy
*  Date: 20200327
***********************************************************/
#ifndef __LOG_SEQ_HTTP_H
#define __LOG_SEQ_HTTP_H

#include "log_seq.h"


// 定义了日志序对象为 “http”，包含了此头文件的文件，使用日志序会记录到 “http” 日志序模块
#define LOGSEQ_OBJ "http"

// 定义了http的详细日志序信息，包括点的序号，以及描述信息，通过点、类型、值、位置，可以问题发生的位置
typedef enum {
    // generic
    LOGSEQ_HTTP_TIME                = 0,    // 类型：字符串，值：系统时间，位置：NULL
    LOGSEQ_HTTP_TOKEN               = 1,    // 类型：字符串，值：设备Token，位置：NULL

    // URL config
    LOGSEQ_HTTP_URL_CFG             = 11,   // 类型：错误码，值：调用URL config接口获取URL，位置：http_iot_dns_get_cloud_url

    // request
    LOGSEQ_HTTP_API                 = 20,   // 类型：字符串，值：API名称，位置：__iot_common_post
    LOGSEQ_HTTP_URL                 = 21,   // 类型：字符串，值：API名称，位置：__iot_common_post
    LOGSEQ_HTTP_DATA                = 22,   // 类型：字符串，值：API名称，位置：__iot_common_post
    LOGSEQ_HTTP_OPEN_SESSION        = 23,   // 类型：错误码，值：HTTP建立连接seesion失败，位置：__http_inf_com_hanle->http_open_session
    LOGSEQ_HTTP_PREPARE_REQ         = 24,   // 类型：错误码，值：HTTP准备请求数据失败，位置：__http_inf_com_hanle->http_prepare_req
    LOGSEQ_HTTP_SEND_REQUEST        = 25,   // 类型：错误码，值：HTTP发生请求失败，位置：__http_inf_com_hanle->http_send_request

    // response
    LOGSEQ_HTTP_GET_RESP_HEAD       = 30,   // 类型：错误码，值：HTTP接受应答头部数据失败，位置：http_get_response_hdr
    LOGSEQ_HTTP_CHK_RESP_RET        = 31,   // 类型：错误码，值：HTTP应答返回值错误，位置：NULL
    LOGSEQ_HTTP_RECV_RESP           = 32,   // 类型：错误码，值：HTTP接受应答数据失败，位置：__httpc_return_json_cb和__httpc_gw_common_cb
    LOGSEQ_HTTP_RECV_BODY_CJSON     = 33,   // 类型：错误码，值：HTTP接受应答数据解析失败，位置：__httpc_return_json_cb和__httpc_gw_common_cb
    LOGSEQ_HTTP_RECV_DECODE         = 34,   // 类型：错误码，值：HTTP解密失败，位置：__httpc_gw_common_cb
    LOGSEQ_HTTP_RECV_CONTENT        = 35,   // 类型：错误码，值：HTTP内容错误，位置：NULL
    LOGSEQ_HTTP_RECV_DATA           = 36,   // 类型：错误码，值：HTTP错误数据，位置：NULL
}HTTP_LOG_SEQ_E;

#endif
