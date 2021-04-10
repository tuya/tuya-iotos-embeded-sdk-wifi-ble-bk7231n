/**
 * @file base_event.h
 * @author maht@tuya.com
 * @brief tuya event,基于timer queue、message queue、work queue实现的事件中心
 * @version 0.1
 * @date 2019-10-30
 * 
 * @copyright Copyright (c) tuya.inc 2019
 * 
 */

#ifndef __BASE_EVENT_H__
#define __BASE_EVENT_H__

#include "tuya_os_adapter.h"
#include "tuya_base_utilities.h"
#include "base_event_info.h"

#ifdef __cplusplus
extern "C" {
#endif

#define EVENT_NAME_MAX_LEN (16)
#define EVENT_DESC_MAX_LEN (32)

// 事件传递数据，tlv结构，信息暂由事件发送、接收双方自行协商
typedef struct {
    int type;           // 发送数据类型
    int len;            // 发送数据总长度，包括type和len
    char value[0];      // 占位，用于动态扩展后续的空间
}event_raw_data_t;

// 定义了事件回调函数类型，所有的订阅回调都必须基于此类型实现
typedef int (*event_subscribe_cb)(void *data);

// 定义了订阅节点
typedef struct {
    char name[EVENT_NAME_MAX_LEN+1];// 名称，用于记录该node关注的事件信息
    char desc[EVENT_DESC_MAX_LEN+1];// 描述，用于记录该node的相关信息用于定位问题
    int emergency;                  // 是否紧急标志
    event_subscribe_cb cb;          // 事件处理函数
    struct tuya_list_head node;          // 用于挂接到事件subscribe_root
}subscribe_node_t;

// 定义了事件节点
typedef struct {
    MUTEX_HANDLE mutex;                         // mutex, 用于保证event发布及订阅的一致性		

    char name[EVENT_NAME_MAX_LEN+1];            // 事件名称，唯一标识符    
    struct tuya_list_head node;                      // 用于挂接到 event_manage
    struct tuya_list_head subscribe_root;            // 订阅根，保存订阅者    
}event_node_t;

// 定义了事件管理
typedef struct {
    int inited;
    MUTEX_HANDLE mutex;                     // mutex, 用于保证  event manage的一致性
    int event_cnt;                          // 当前事件数量
    struct tuya_list_head event_root;            // 事件链表
    struct tuya_list_head free_subscribe_root;   // 空闲订阅者链表
}event_manage_t;

/** 
 * @brief 事件初始化函数，将会根据事件描述表，支持动态未定义事件
 * 
 * 
 * @return int: 0成功，非0，请参照tuya error code描述文档 
 */
int ty_event_init(void);

/** 
 * @brief: 发布指定事件，会通知所有订阅该事件的订阅者处理
 *
 * @param[in] name: 事件名，事件标识，字符串，16字节长的
 * @param[in] data: 事件数据，数据和事件类型绑定，发布事件和订阅事件必须使用同一事件数据类型定义
 * @param[in] len: 事件数据长度
 *
 * @return int: 0成功，非0，请参照tuya error code描述文档 
 */
int ty_publish_event(const char* name, void *data);

/** 
 * @brief: 订阅指定事件，会通过回调函数处理消息发布内容
 *
 * @param[in] name: 事件名，事件标识，字符串，16字节长的
 * @param[in] desc: 描述信息，表面订阅者身份、目的，32字节长度，方便定位问题
 * @param[in] cb: 事件处理回调函数
 * @param[in] is_emergency: 紧急事件，必须第一个处理
 *
 * @note： desc、cb构成了一个二元组，这个二元组标识一个唯一订阅者，同一个desc不同的cb，也会认为是不同的
 *      订阅。在事件发布之前就订阅的，可以收到事件；在事件发布之后订阅的，收不到之前发布的事件。
 *
 * @return int: 0成功，非0，请参照tuya error code描述文档 
 */
int ty_subscribe_event(const char *name, const char *desc, const event_subscribe_cb cb, int is_emergency);

/** 
 * @brief: 订阅指定事件，会通过回调函数处理消息发布内容
 *
 * @param[in] name: 事件名，事件标识，字符串，16字节长的
 * @param[in] desc: 描述信息，订阅者身份、目的，32字节长度，方便定位问题
 * @param[in] cb: 事件处理回调函数
 *
 * @note： desc、cb构成了一个二元组，这个二元组标识一个唯一订阅者，同一个desc不同的cb，也会认为是不同的
 *      订阅
 *
 * @return int: 0成功，非0，请参照tuya error code描述文档 
 */
int ty_unsubscribe_event(const char *name, const char *desc, event_subscribe_cb cb);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  /*__BASE_EVENT_H__ */


