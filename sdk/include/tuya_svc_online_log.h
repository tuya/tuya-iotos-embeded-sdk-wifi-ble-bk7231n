/**
 * @file tuya_svc_online_log.h
 * @author 愚者(maht@tuya.com)
 * @brief log服务应用接口描述文件
 * @version 0.1
 * @date 2019-08-29
 * 
 * @copyright Copyright (c) tuya.inc 2019
 * 
 */

#ifndef __TUYA_SVC_ONLINE_LOG_H__
#define __TUYA_SVC_ONLINE_LOG_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 用户运行状态log上传
 * 
 * @param[in] p_log: log字符串 
 * @return int: 0成功，非0，请参照tuya error code描述文档 
 */
int tuya_svc_online_log_upload_runstat(const char *p_log);

/**
 * @brief 用户实时log上传
 * 
 * @param[in] p_log: log字符串  
 * @return int: 0成功，非0，请参照tuya error code描述文档 
 */
int tuya_svc_online_log_upload_realtime(const char *p_log);


/**
 * @brief 用户log seq上传
 * 
 * @param[in] p_log: custom seq log字符串  
 * @return int: 0成功，非0，请参照tuya error code描述文档 
 */
int tuya_svc_online_log_upload_custom_seq_log(const char *p_log);

#ifdef __cplusplus
}
#endif
#endif

