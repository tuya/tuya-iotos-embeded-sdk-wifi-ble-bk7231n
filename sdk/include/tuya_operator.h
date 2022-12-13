/**
* @file tuya_operator.h
* @brief TUYA OTA service
* @version 0.1
* @date 2016-08-28
*
* @copyright Copyright 2016-2021 Tuya Inc. All Rights Reserved.
*/

#ifndef _TUYA_OPERATOR_H_
#define _TUYA_OPERATOR_H_

#include <stdio.h>
#include "tuya_cloud_types.h"
#include "tuya_cloud_com_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(TUYA_OPERATOR_TYPE) && ((TUYA_OPERATOR_TYPE&TUYA_OPERATOR_CUCC)==TUYA_OPERATOR_CUCC)
#define TY_CUCC_UPLOAD_RESULT_OK            0 // upload ok
#define TY_CUCC_UPLOAD_ERR                  1 // upload error

#if (ENABLE_AI_SPEAKER == 1)
#define TY_CUCC_IS_UNICOM_NETWORK           0 // is unicom network
#define TY_CUCC_NOT_UNICOM_NETWORK          1 // not unicom network

#define TY_CUCC_NUMBER_ATTR_OK              0 // number attr ok
#define TY_CUCC_NUMBER_NOT_BIND             1 // number not bind
#define TY_CUCC_LOCATION_ERR                2 // location error

/**
 * @brief Definition of CUCC state
 */
typedef enum {
    /** INIT */
    TY_CUCC_STAT_INIT = 0,
    /** HTTP error */   
    TY_CUCC_STAT_HTTP_ERR = 1,
    /** HTTP ok */
    TY_CUCC_STAT_HTTP_OK = 2
} TY_CUCC_STAT_E;

/**
 * @brief Definition of CUCC state info
 */
typedef struct {
    /** see TY_CUCC_STAT_E */
    TY_CUCC_STAT_E  stat;
    /** serial code */
    CHAR_T          cuei[20];
    /** report result */
    UINT_T          report_result;
    /** is unicom network */
    UINT_T          is_unicom_network;
    /** is home location */
    UINT_T          is_telephone_home_location;
}TY_CUCC_STAT_INFO_S;

/**
 * @brief Get operator(CUCC) state info
 * 
 * @param[in] p_stat State info, see TY_CUCC_STAT_INFO_S
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_operator_get_cucc_stat(TY_CUCC_STAT_INFO_S *p_stat);
#endif
#endif

#if defined(TUYA_OPERATOR_TYPE) && (((TUYA_OPERATOR_TYPE&TUYA_OPERATOR_CMCC)==TUYA_OPERATOR_CMCC)||((TUYA_OPERATOR_TYPE&TUYA_OPERATOR_CMCC_ANDLINK)==TUYA_OPERATOR_CMCC_ANDLINK) )
/**
 * @brief Set operator initial params
 * 
 * @param[in] p_appkey Key of application
 * @param[in] p_aeskey AES key
 * @param[in] p_deviceType Device type
 * @param[in] p_productToken Product token
 * @param[in] localport Local port
 * @param[in] p_appBinName Applicaiton bin name
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_operator_cmcc_reg_params(IN CONST CHAR_T *p_appkey, IN CONST CHAR_T *p_aeskey,
                                          IN CHAR_T *p_deviceType,IN CHAR_T *p_productToken, UINT_T localport, IN CHAR_T *p_appBinName);
#endif

/**
 * @brief Definition of operator info
 */
typedef struct {              /*   CTCC     CMCC    CUCC  */
    CHAR_T *fwver;            /*    Y                Y    */ //
    CHAR_T *device_info;      /*             Y            */ //备用(不上报)
    CHAR_T *app_info;         /*             Y            */ //应用信息(App集合，每条日志用\r\n分隔，每条日志包含 App名称、包名、App使用时长(毫秒)、App打开次数 并由’|’分隔 )
    CHAR_T *rom;              /*             Y            */ //rom信息
    CHAR_T *ram;              /*             Y            */ //ram信息
    CHAR_T *cpu;              /*             Y            */ //cpu信息
    CHAR_T *sys_ver;          /*             Y            */ //操作系统版本号
    CHAR_T *soft_ver;         /*             Y            */ //固件版本号
    CHAR_T *soft_name;        /*             Y            */ //固件名称
    CHAR_T *volte;            /*             Y            */ //volte开关状态
    CHAR_T *nettype;          /*             Y            */ //当前网络类型
    CHAR_T *account;          /*             Y            */ //宽带账号
    CHAR_T *p_num;            /*             Y            */ //本机号码
    CHAR_T *location;         /*             Y            */ //位置信息(包含经度，维度，定位模式，以逗号‘,’分隔)
    BYTE_T mac[6];            /*    Y                Y    */ //设备mac地址
    BOOL_T initial;            /*   Y                     */ //是否开机后首次上报，子设备使用
    CHAR_T *devid;            /*    Y                     */ //子设备ID/MAC
}TY_OPERATOR_INFO_S;

/**
 * @brief Upload operator info manually(once a time)
 * 
 * @param[in] p_info Operator info, see TY_OPERATOR_INFO_S
 * @param[in] async Sync or not to upload operator info
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_operator_upload_manual(IN CONST TY_OPERATOR_INFO_S *p_info, IN CONST BOOL_T async);

/**
 * @brief Start to upload operator info
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_operator_upload(VOID);

/**
 * @brief Definition of operator type
 */
typedef enum operat_ctei_type {
    /** TUYA */
    OPERATOR_TUYA = 1,
    /** China Telecom */
    OPERATOR_CTCC,
    /** China Mobile */
    OPERATOR_CMCC,
    /** China Unicom */
    OPERATOR_CUCC,
} TY_OPERATOR_TYPE_E;

/**
 * @brief Get current operator type
 * 
 * @param[in] p_type Operator type, see TY_OPERATOR_TYPE_E
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_operator_get_type(OUT TY_OPERATOR_TYPE_E *p_type);

/**
 * @brief Reset saved operator info
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_operator_reset(VOID);

#define EVENT_OPERATOR_INFO_UPLOAD "EVT_OPR_UPLOAD" // event definition

/**
 * @brief Retrieve operator info
 * 
 * @param[in] dev_id ID of sub-device, or NULL for GW
 * @param[in] ch_code Operator info, see CH_CODE_ST
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_operator_code_get(IN CONST CHAR_T *devid, OUT CH_CODE_ST *ch_code);

/**
 * @brief Upload operator info
 * 
 * @param[in] dev_id ID of sub-device, or NULL for GW
 * @param[in] ch_code Operator info, see CH_CODE_ST
 * @param[in] initial Whether the first time to upload
 * 
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tuya_operator_inf_upload(IN CONST CHAR_T *devid, IN CONST CH_CODE_ST *ch_code, IN BOOL_T initial);

#ifdef __cplusplus
}
#endif

#endif
