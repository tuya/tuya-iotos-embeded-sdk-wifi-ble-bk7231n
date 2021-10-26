/**
 * @file tuya_devos_health.h
 * @author maht@tuya.com
 * @brief Tuya Device OS设备健康监控
 * @version 0.1
 * @date 2020-05-20
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef __TUYA_DEVOS_HEALTH_H__
#define __TUYA_DEVOS_HEALTH_H__


#include "tuya_cloud_com_defs.h"
#include "tuya_hal_thread.h"
#include "tuya_hal_mutex.h"

#ifdef __cplusplus
extern "C" {
#endif

// 默认health monitor检测时间间隔（暂定）
#define HEALTH_SLEEP_INTERVAL               (5)
// 默认系统上报健康状态时间
#define HEALTH_REPORT_INTERVAL   (60 * 60)
// 默认系统最小free内存门限设置为5K，正常访问云端、FLASH需要4K内存以上（暂定）
#define HEALTH_FREE_MEM_THRESHOLD           (1024*8)
// 默认系统最小内存块门限设置为5K，正常访问云端、FLASH需要一次性分配4K内存以上（暂定）
#define HEALTH_LARGEST_MEM_BLOK_THRESHOLD   (1024*5)
// 默认系统HTTP访问拒绝、sign校验错误次数
#define HEALTH_HTTP_FAILED_THRESHOLD   (5)


// health monitor状态
typedef enum {
    HEALTH_STATUS_INITED = 1,
    HEALTH_STATUS_NORMAL,
    HEALTH_STATUS_WAIT_REBOOT,
    HEALTH_STATUS_REBOOT
}ty_devos_health_status_e;

// health monitor指标
typedef enum {
    HEALTH_RULE_FREE_MEM_SIZE = 0,
    HEALTH_RULE_MAX_MEM_SIZE,
    HEALTH_RULE_ATOP_REFUSE,
    HEALTH_RULE_ATOP_SIGN_FAILED,
    HEALTH_RULE_MAX
}ty_devos_health_monitor_rule_e;

// health monitor检测指标
typedef struct {
    ty_devos_health_monitor_rule_e type;    // health monitor检测指标
    unsigned int value;                     // health monitor指标对应的值
    unsigned int cnt;                       // health monitor指标异常连续次数
    TIME_T ts;                              // 对应指标更新的时间
    bool_t is_need_reset;                   // reset标记
}ty_devos_health_item_t;

// health monitor检测数据
typedef struct {
    ty_devos_health_item_t items[HEALTH_RULE_MAX];
}ty_devos_health_db_t;

// health monitor对象
typedef struct {
    THREAD_HANDLE health_monitor;           // health monitor任务句柄

    MUTEX_HANDLE health_mutex;              // 避免重入
    int watch_dog_interval;                 // 看门狗检测时间间隔，单位sec
    int monitor_detect_interval;            // 健康检测时间间隔，单位sec
    int reason;                             // health状态原因
    ty_devos_health_status_e status;        // health monitor状态
    ty_devos_health_db_t *monitor_db;       // health monitor数据库
}ty_devos_health_monitor_t;

/**
 * @brief tuya Device OS健康状态检测
 * 
 * @param[in] watch_dog_interval: 看门狗的默认时间，单位是秒，必须是20秒钟的倍数
 * @param[in] monitor_detect_interval: 健康监控扫描间隔，单位是秒，必须是20秒钟的倍数
 *
 * @return int: 0成功，非0，请参照tuya error code描述文档
 */
int tuya_devos_health_init_and_start(const int watch_dog_interval, const int monitor_detect_interval);

int tuya_devos_health_disable(VOID);
#ifdef __cplusplus
}
#endif


#endif
