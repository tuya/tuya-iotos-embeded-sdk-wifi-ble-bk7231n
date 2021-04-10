/**
 * @file tuya_svc_upgrade.h
 * @author maht@tuya.com
 * @brief tuya OTA服务
 * @version 0.1
 * @date 2019-08-28
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef __TUYA_SVC_UPGRADE_H__
#define __TUYA_SVC_UPGRADE_H__

#include "tuya_cloud_types.h"
#include "tuya_cloud_com_defs.h"
#include "tuya_os_adapter.h"
#include "uni_msg_queue.h"
#include "uni_thread.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef BYTE_T TI_UPGRD_STAT_S;
#define TUS_RD 1
#define TUS_UPGRDING 2
#define TUS_UPGRD_FINI 3
#define TUS_UPGRD_EXEC 4

#define TUS_DOWNLOAD_START 10
#define TUS_DOWNLOAD_COMPLETE 11
#define TUS_UPGRADE_START 12
#define TUS_UPGRADE_SUCCESS 3

#define TUS_DOWNLOAD_ERROR_UNKONW 40
#define TUS_DOWNLOAD_ERROR_LOW_BATTERY 41
#define TUS_DOWNLOAD_ERROR_STORAGE_NOT_ENOUGH 42
#define TUS_DOWNLOAD_ERROR_MALLOC_FAIL 43
#define TUS_DOWNLOAD_ERROR_TIMEOUT 44
#define TUS_DOWNLOAD_ERROR_HMAC 45
#define TUS_UPGRADE_ERROR_LOW_BATTERY 46
#define TUS_UPGRADE_ERROR_MALLOC_FAIL 47
#define TUS_UPGRADE_ERROR_VERSION 48
#define TUS_UPGRADE_ERROR_HMAC 49

typedef struct {
    BOOL_T upload_upgrade_percent;
    GET_FILE_DATA_CB get_file_cb;
    UPGRADE_NOTIFY_CB upgrd_nofity_cb;
    PVOID_T pri_data;

    THRD_HANDLE upgrade_thrd;
    THRD_PARAM_S thrd_param;

    FW_UG_S fw;
    CHAR_T dev_id[DEV_ID_LEN+1];
    UINT_T download_buf_size;

    TIMER_ID mqc_upd_progess_timer;
    INT_T download_percent;
}tuya_upgrade_process_t, *tuya_upgrade_process_p;

typedef struct {
    INT_T exectime;                 // specify the upgrade time
    FW_UG_S ug;
    DEV_TYPE_T tp;        	// 云端固件类型
    CHAR_T dev_id[DEV_ID_LEN+1]; 	// if gateway upgrade then dev_id[0] == 0
}tuya_upgrade_notify_t, *tuya_upgrade_notify_p;

typedef int (*dev_upgrade_inform_cb)(const FW_UG_S *fw);
typedef VOID (*dev_upgrade_pre_inform_cb)(BOOL_T *handled, const FW_UG_S *fw);
typedef int (*subdev_upgrade_inform_cb)(const char *dev_id, const FW_UG_S *fw);

typedef struct {	
	char inited;

	// upgrade process 
	tuya_upgrade_process_t *upgrade_proc;

	// mqc mess proc message
    MSG_ID mid_upgrade_msg;

	// auto detect timer, will trigger work check if there have valid veriosn to upgrade
    TM_MSG_S *tmm_dev_fw_ug;
    TM_MSG_S *tmm_subdev_fw_ug;	

	// callbacks, registered by app
    dev_upgrade_inform_cb dev_upgrade_cb;
    subdev_upgrade_inform_cb subdev_upgrade_cb;	
    dev_upgrade_inform_cb pre_dev_upgrade_cb;
    subdev_upgrade_inform_cb pre_subdev_upgrade_cb;	

	// gateway 
	dev_upgrade_pre_inform_cb dev_upgrade_pre_cb;
}tuya_upgrade_t, *tuya_upgrade_p;

typedef struct {
    dev_upgrade_inform_cb dev_upgrade_cb;
    subdev_upgrade_inform_cb subdev_upgrade_cb;
    dev_upgrade_inform_cb pre_dev_upgrade_cb;
    subdev_upgrade_inform_cb pre_subdev_upgrade_cb;
}tuya_upgrade_param_t, *tuya_upgrade_param_p;

int tuya_svc_upgrade_init(tuya_upgrade_param_t *ota_param);
int tuya_svc_upgrade_active(void);


void tuya_svc_upgrade_register_pre_cb(dev_upgrade_pre_inform_cb pre_ug_cb);
int tuya_svc_upgrade_detect_reset(const int upgrade_interval);
int tuya_svc_upgrade_refuse(const FW_UG_S *fw, const char *dev_id);
int tuya_svc_upgrade_result_report(const char *dev_id, const DEV_TYPE_T type, const int result);	
int tuya_svc_upgrade_start(const char *dev_id,
						   const FW_UG_S *fw,
						   const GET_FILE_DATA_CB get_file_cb,
						   void *pri_data,
						   const UPGRADE_NOTIFY_CB upgrd_nofity_cb, 
						   const bool upload_upgrade_percent, 
						   const unsigned int download_buf_size);

int tuya_svc_upgrade_schema_notify(VOID);

#ifdef __cplusplus
}
#endif

#endif
