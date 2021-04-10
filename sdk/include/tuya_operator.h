#ifndef _TUYA_OPERATOR_H_
#define _TUYA_OPERATOR_H_

#include "tuya_cloud_types.h"
#include "tuya_cloud_com_defs.h"


#ifdef __cplusplus
extern "C" {
#endif

#if defined(TUYA_OPERATOR_TYPE) && ((TUYA_OPERATOR_TYPE&TUYA_OPERATOR_CUCC)==TUYA_OPERATOR_CUCC)
#define TY_CUCC_UPLOAD_RESULT_OK            0
#define TY_CUCC_UPLOAD_ERR                  1

#if (ENABLE_AI_SPEAKER == 1)
#define TY_CUCC_IS_UNICOM_NETWORK           0
#define TY_CUCC_NOT_UNICOM_NETWORK          1

#define TY_CUCC_NUMBER_ATTR_OK              0
#define TY_CUCC_NUMBER_NOT_BIND             1
#define TY_CUCC_LOCATION_ERR                2

typedef enum {
    TY_CUCC_STAT_INIT = 0,        
    TY_CUCC_STAT_HTTP_ERR = 1,
    TY_CUCC_STAT_HTTP_OK = 2,
    
} TY_CUCC_STAT_E;

typedef struct {
    TY_CUCC_STAT_E  stat;               
    CHAR_T          cuei[20];    
    UINT_T          report_result;
    UINT_T          is_unicom_network;
    UINT_T          is_telephone_home_location;
    
}TY_CUCC_STAT_INFO_S;
OPERATE_RET tuya_operator_get_cucc_stat(TY_CUCC_STAT_INFO_S *p_stat);

#endif
#endif

#if defined(TUYA_OPERATOR_TYPE) && (((TUYA_OPERATOR_TYPE&TUYA_OPERATOR_CMCC)==TUYA_OPERATOR_CMCC)||((TUYA_OPERATOR_TYPE&TUYA_OPERATOR_CMCC_ANDLINK)==TUYA_OPERATOR_CMCC_ANDLINK) )
OPERATE_RET tuya_operator_cmcc_reg_params(IN CONST CHAR_T *p_appkey, IN CONST CHAR_T *p_aeskey,
                                          IN CHAR_T *p_deviceType,IN CHAR_T *p_productToken, UINT_T localport, IN CHAR_T *p_appBinName);
#endif

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

//Used by GW
OPERATE_RET tuya_operator_upload_manual(IN CONST TY_OPERATOR_INFO_S *p_info, IN CONST BOOL_T async);
OPERATE_RET tuya_operator_upload(VOID);


typedef enum operat_ctei_type {
    OPERATOR_TUYA = 1,
    OPERATOR_CTCC,		//电信
    OPERATOR_CMCC,		//移动
    OPERATOR_CUCC,		//联通
} TY_OPERATOR_TYPE_E;

OPERATE_RET tuya_operator_get_type(INOUT TY_OPERATOR_TYPE_E *p_type);

OPERATE_RET tuya_operator_reset(VOID);

#define EVENT_OPERATOR_INFO_UPLOAD "EVT_OPR_UPLOAD"

/*
* 获取运营商编码及相关信息
* @devid NULL indicates GW, Not-NULL indicates sub-dev
* @ch_code see @CH_CODE_ST
*/
OPERATE_RET tuya_operator_code_get(IN CONST CHAR_T *devid, OUT CH_CODE_ST *ch_code);

/*
* 终端信息上报
* @devid NULL indicates GW, Not-NULL indicates sub-dev
* @ch_code see @CH_CODE_ST
* @initial TRUE indicates first time report, FALSE for following reports
*/
OPERATE_RET tuya_operator_inf_upload(IN CONST CHAR_T *devid, IN CONST CH_CODE_ST *ch_code, IN BOOL_T initial);

#ifdef __cplusplus
}
#endif

#endif
