/**
 * @file svc_online_log_mqtt.h
 * @author 愚者(maht@tuya.com)
 * @brief log服务防腐层MQTT接口
 * @version 0.1
 * @date 2019-08-29
 * 
 * @copyright Copyright (c) tuya.inc 2019
 * 
 */

#ifndef __SVC_ONLINE_LOG_MQTT_H__
#define __SVC_ONLINE_LOG_MQTT_H__

#include "ty_cJSON.h"

#ifdef __cplusplus
extern "C" {
#endif

int online_log_mqtt_init();
int online_log_mqtt_proc_LOG_CFG_55(ty_cJSON *mqtt_cmd_json);
int online_log_mqtt_response(char *response_str);
int online_log_mqtt_publish_async(const char *data, const int len, const char *topic, const int qos, const unsigned int time_out, void *cb, void *prv_data);
int online_log_mqtt_publish_sync(const char *data, const int len, const char *topic, const int qos, const unsigned int time_out);



#ifdef __cplusplus
}
#endif
#endif

