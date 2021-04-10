/**
 * @file tuya_svc_timer_task.h
 * @author anby (you@domain.com)
 * @brief tuya定时服务外部接口
 * @version 0.1
 * @date 2019-08-29
 * 
 * @copyright Copyright (c) tuya.inc 2019
 * 
 */

#ifndef __TUYA_SVC_TIME_TASK_H__
#define __TUYA_SVC_TIME_TASK_H__

#include "tuya_cloud_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief tuya 本地定时服务初始化
 * 
 * @param increase_unit: 定时任务每次申请increase_unit
 * @return OPERATE_RET： 0成功，非0，请参照tuya error code描述文档 
 */
OPERATE_RET tuya_svc_timer_task_init(UINT_T increase_unit);

/** 
 * @brief tuya 本地定时服务重置，清空
 * 
 * @return OPERATE_RET： 0成功，非0，请参照tuya error code描述文档 
 */
OPERATE_RET tuya_svc_timer_task_reset(VOID_T);

/*
删除flash中已保存的本地定时数据
未初始化timer_task前调用
*/
OPERATE_RET tuya_svc_timer_task_clear_db(VOID_T);

/** 
 * @brief tuya 获取定时任务个数
 * 
 * @return UINT_T：返回定时器任务个数
 */
UINT_T tuya_svc_timer_get_task_count(VOID_T);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  /*__TUYA_SVC_TIME_TASK_H__ */

