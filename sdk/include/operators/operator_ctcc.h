#ifndef _OPERATOR_CTCC_H_
#define _OPERATOR_CTCC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "tuya_cloud_types.h"
#include "tuya_operator.h"

#define CODE_MAX_LEN  5
#define KEY_MAX_LEN   17
#define VER_MAX_LEN   6

typedef struct dev_change_info{
    CHAR_T up_code[CODE_MAX_LEN+1]; //manufacturerId
    CHAR_T up_key[KEY_MAX_LEN+1];   //key
    CHAR_T up_ver[VER_MAX_LEN+1];   //version
}dev_change_info_t;


OPERATE_RET set_dev_up_info(dev_change_info_t data);

OPERATE_RET operator_ctcc_start_send(CHAR_T *report_code, CHAR_T *sn, CHAR_T* ctei_code, IN CONST TY_OPERATOR_INFO_S *p_info);

#ifdef __cplusplus
}
#endif

#endif
