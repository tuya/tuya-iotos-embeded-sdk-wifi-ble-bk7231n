/***********************************************************
*  File: log_seq_netcfg.h
*  Author: nzy
*  Date: 20200327
***********************************************************/
#ifndef __LOG_SEQ_NETCFG_H
#define __LOG_SEQ_NETCFG_H

#include "log_seq.h"

// 定义了日志序对象为 “netcfg”，包含了此头文件的文件，使用日志序会记录到 “netcfg” 日志序模块
#define LOGSEQ_OBJ "netcfg"

// 定义了NETCFG的详细日志序信息，包括点的序号，以及描述信息，通过点、类型、值、位置，可以问题发生的位置
typedef enum {
    // generic
    LOGSEQ_NETCFG_TIME              = 0,    // 类型：字符串，值：系统时间，位置：NULL
    LOGSEQ_NETCFG_TOKEN             = 1,    // 类型：字符串，值：设备Token，位置：NULL
    LOGSEQ_NETCFG_TYPE              = 9,    // 类型：字符串，值：配网类型，位置：NULL

    // netcfg 
    LOGSEQ_NETCFG_FAIL              = 10,   // 类型：错误码，值：配网失败原因，位置：NULL

    // route (后面会进行修改，移到合适的地方，目前处理是为了兼容)
    LOGSEQ_NETCFG_LAST_STATUS       = 20,   // 类型：十进制数值型，值：上一个网络状态(后面会移到合适的地方)，位置：get_wf_gw_nw_status
    LOGSEQ_NETCFG_NEW_STATUS        = 21,   // 类型：十进制数值型，值：新的网络状态(后面会到合适的地方)，位置：get_wf_gw_nw_status
    LOGSEQ_NETCFG_OTHERS_START      = 22,   // 类型：十进制数值型，值：退出类型(后面会移到合适的地方)，位置：__gw_net_conn_cb
}NETCFG_LOG_SEQ_E;



#endif
