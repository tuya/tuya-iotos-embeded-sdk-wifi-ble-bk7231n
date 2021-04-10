/**
 * @file svc_online_log.h
 * @author 愚者(maht@tuya.com)
 * @brief log服务
 * @version 0.1
 * @date 2019-08-29
 * 
 * @copyright Copyright (c) tuya.inc 2019
 * 
 */

#ifndef __SVC_ONLINE_LOG_H__
#define __SVC_ONLINE_LOG_H__

#include "tuya_cloud_types.h"
//#include "svc_devos_components.h"
#include "ty_cJSON.h"

#ifdef __cplusplus
extern "C" {
#endif

//devos_component_options_t *svc_online_log_get_desc(void);

/**
 * @brief tuya online log service初始化接口
 * 
 * @param[in] init_param: 初始化参数
 *
 * @return int: 0成功，非0，请参照tuya error code描述文档
 */
//int online_log_init(const void *init_param);
int online_log_init(const char *p_env, const char *p_log_seq_path);


/**
 * @brief tuya online log service激活接口
 * 
 * @param[in] active_param: 激活参数
 *
 * @return int: 0成功，非0，请参照tuya error code描述文档
 */
//int online_log_active(const void *active_param);
int online_log_active(void);


/**
 * @brief tuya online log service 上传runtime status接口
 * 
 * @param[in] log: 需要上传的日志字符串
 *
 * @return int: 0成功，非0，请参照tuya error code描述文档
 */
int online_log_upload_runstat(const char *log);

/**
 * @brief tuya online log service上传实时信息接口
 * 
 * @param[in] log: 需要上传的日志字符串
 *
 * @return int: 0成功，非0，请参照tuya error code描述文档
 */
int online_log_upload_realtime(const char *log);

/**
 * @brief tuya online log service 处理mqtt日志配置接口
 * 
 * @param[in] mqtt_cmd_json: mqtt 命令json字符串
 *
 * @return int: 0成功，非0，请参照tuya error code描述文档
 */
int online_log_proc_mqtt_log_config(ty_cJSON *mqtt_cmd_json);
    

#ifdef __cplusplus
}
#endif
#endif

