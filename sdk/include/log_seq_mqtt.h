/***********************************************************
*  File: log_seq_mqtt.h
*  Author: Maht
*  Date: 20200327
***********************************************************/
#ifndef __LOG_SEQ_MQTT_H
#define __LOG_SEQ_MQTT_H

#include "log_seq.h"

// 定义了日志序对象为 “mqtt”，包含了此头文件的文件，使用日志序会记录到 “mqtt” 日志序模块
#define LOGSEQ_OBJ "mqtt"

// 定义了mqtt的详细日志序信息，包括点的序号，以及描述信息，通过点、类型、值、位置，可以问题发生的位置
typedef enum {
    // generic
    LOGSEQ_MQTT_TIME            = 0,    // 类型：字符串，值：系统时间，位置：无确定位置
    LOGSEQ_MQTT_RSSI            = 9,    // 类型：字符串，值：RSSI信号强度，位置：__mqtt_log_seq_err

    // DNS
    LOGSEQ_MQTT_DNS_RESV        = 10,   // 类型：错误码，值：DNS解析，位置：__mq_ctrl_task

    // connect
    LOGSEQ_MQTT_SOCK_CONN       = 20,   // 类型：错误码，值：SOCKET连接，位置：__mq_ctrl_task
    LOGSEQ_MQTT_CONN            = 21,   // 类型：系统错误码，值：MQTT连接，位置：__mq_ctrl_task
    LOGSEQ_MQTT_CONN_ACK        = 22,   // 类型：系统错误码，值：MQTT连接ACK，位置：mqtt_connect

    // subscirbe
    LOGSEQ_MQTT_SUB             = 30,   // 类型：系统错误码，值：MQTT订阅，位置：__mq_ctrl_task
    LOGSEQ_MQTT_SUB_ACK         = 31,   // 类型：系统错误码，值：MQTT订阅ACK，位置：mqtt_subscribe

    // publish
    LOGSEQ_MQTT_PUB             = 40,   // 类型：系统错误码，值：MQTT发布，位置：mqtt_publish_async
    LOGSEQ_MQTT_PUB_ACK         = 41,   // 类型：系统错误码，值：MQTT发布ACK，位置：mqtt_publish
    LOGSEQ_MQTT_PUB_ACK_CLOSE   = 42,   // 类型：字符串，值：MQTT发布ACK失败3次关闭，位置：LOGSEQ_MQTT_PUB_ACK_CLOSE

    // process logic
    LOGSEQ_MQTT_RECV            = 50,   // 类型：系统错误码，值：MQTT接收，位置：__mq_ctrl_task
    LOGSEQ_MQTT_LOGIC_PROC      = 51,   // 类型：错误码，值：MQTT逻辑处理，位置：__mq_ctrl_task
    LOGSEQ_MQTT_CONN_RECV       = 52,   // 类型：错误码，值：MQTT连接接收，位置：__mq_ctrl_task

    // ping
    LOGSEQ_MQTT_PING            = 60,   // 类型：系统错误码，值：MQTT PING失败，位置：__alive_tm_msg_cb
    LOGSEQ_MQTT_PING_TIMEOUT    = 61,   // 类型：字符串，值：为记录发生的时间点，位置：__resp_tm_cb
}MQTT_LOG_SEQ_E;



#endif
