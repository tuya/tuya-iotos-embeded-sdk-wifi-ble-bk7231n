/**
 * @file base_event_info.c
 * @author maht@tuya.com
 * @brief tuya event,基于timer queue、message queue、work queue实现的事件中心
 *		本文件主要是用于登记相关事件以及事件数据结构
 * @version 0.1
 * @date 2019-10-30
 * 
 * @copyright Copyright (c) tuya.inc 2019
 * 
 */

#ifndef __BASE_EVENT_INFO_H__
#define __BASE_EVENT_INFO_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef int event_test_data_t;

/**
 * @brief data of EVENT_ID_MQC_DENIED
 * 
 */
typedef struct {
    int deny_times;     /**< 拒绝次数 @see MQ_CONN_DENY_CB */
} event_mqc_deny_data_t;

/**
 * @brief data of EVENT_ID_MQC_PERMITTED
 * 
 */
typedef struct {
    int permit;     /**< 允许状态 1-允许 0-不允许，随机重连 1-不允许，短时重连 @see MQ_PERMIT_CONN_CB */
    int errcode;    /**< 不允许时的错误码 */
} event_mqc_permit_data_t;

// devos event
#define EVENT_RESET         "dev.reset"
#define EVENT_REBOOT_REQ     "dev.reboot.req"
#define EVENT_REBOOT_ACK     "dev.reboot.ack"
#define EVENT_RECOVER_DB     "dev.recover.db"    // db is lost, device need recover from cloud

// http
#define EVENT_HTTP_FAILED   "http.failed"

// init
#define EVENT_SDK_EARLY_INIT_OK     "early.init"
#define EVENT_SDK_DB_INIT_OK        "db.init"

// schema update
#define EVENT_SCHEMA_UPDATE "schema.update"

// ota
#define EVENT_OTA_START_NOTIFY      "ota.start"
#define EVENT_OTA_PROCESS_NOTIFY    "ota.process"
#define EVENT_OTA_FAILED_NOTIFY     "ota.failed"
#define EVENT_OTA_FINISHED_NOTIFY   "ota.finished"


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  /*__BASE_EVENT_H__ */


