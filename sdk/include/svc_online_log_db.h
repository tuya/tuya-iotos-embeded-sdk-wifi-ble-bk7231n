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

#ifndef __SVC_ONLINE_LOG_DB_H__
#define __SVC_ONLINE_LOG_DB_H__

#ifdef __cplusplus
extern "C" {
#endif

int online_log_db_write(const char *key, const char *value, const unsigned int len);
int online_log_db_read(const char *key, char **value, unsigned int *len);
int online_log_db_free(char *data);


#ifdef __cplusplus
}
#endif
#endif

