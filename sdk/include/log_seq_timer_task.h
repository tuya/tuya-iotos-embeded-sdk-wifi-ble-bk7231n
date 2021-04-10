/***********************************************************
*  File: log_seq_timer_task.h
*  Author: nzy
*  Date: 20200327
***********************************************************/
#ifndef __LOG_SEQ_TIMER_TASK_H
#define __LOG_SEQ_TIMER_TASK_H

#include "log_seq.h"


// 定义了日志序对象为 “timer_task”，包含了此头文件的文件，使用日志序会记录到 “timer_task” 日志序模块
#define LOGSEQ_OBJ "svc_timer_task"

// 定义了timer_task的详细日志序信息，包括点的序号，以及描述信息，通过点、类型、值、位置，可以问题发生的位置
typedef enum {
    // run timer task
    LOGSEQ_TIMER_TASK_EXECUTE              = 0,    // 类型：字符串，值：定时ID和执行结果，位置：NULL
    
    // generic
    LOGSEQ_TIMER_TASK_TIME                 = 1,    // 类型：字符串，值：系统时间，位置：NULL
    LOGSEQ_TIMER_TASK_TOKEN                = 2,    // 类型：字符串，值：设备Token，位置：NULL

    // load from disk
    LOGSEQ_TIMER_TASK_LOAD                 = 10,    // 类型：错误码，值：从flash加载定时任务失败，位置：__load_timer_task_from_disk
    LOGSEQ_TIMER_TASK_LOAD_DPS_FAILD       = 11,    // 类型：十进制数值型，值：申请DPS内存大小，位置：__load_timer_task_from_disk
    LOGSEQ_TIMER_TASK_LOAD_TIMER_ID_FAILD  = 12,    // 类型：十进制数值型，值：申请Timer ID内存大小，位置：__load_timer_task_from_disk

    // save to disk
    LOGSEQ_TIMER_TASK_SAVE                 = 20,    // 类型：错误码，值：把定时任务存储到flash失败，位置：__save_time_task_to_disk

    // update from cloud
    LOGSEQ_TIMER_TASK_UPDATE               = 30,    // 类型：错误码，值：更新定时任务失败，位置：__update_time_task_from_cloud
    LOGSEQ_TIMER_TASK_UPDATE_GET           = 31,    // 类型：错误码，值：更新定时内容任务失败，位置：__update_time_task_from_cloud
    LOGSEQ_TIMER_TASK_UPDATE_GET_ITEM      = 32,    // 类型：十进制数值型，值：获取出错timer index，位置：__update_time_task_from_cloud
    LOGSEQ_TIMER_TASK_UPDATE_INCREASE      = 33,    // 类型：十进制数值型，值：总共的timer数量，位置：__increase_arr_size
    LOGSEQ_TIMER_TASK_UPDATE_ADD           = 34,    // 类型：十进制数值型，值：添加出错timer index，位置：__add_task_to_arr
    

}TIMER_TASK_LOG_SEQ_E;

#endif
