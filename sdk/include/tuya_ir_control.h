/*!
\file tuya_ir_control.h
Copyright(C),2017, 涂鸦科技 www.tuya.comm
*/
#ifndef _TUYA_IR_CONTROL_H
#define _TUYA_IR_CONTROL_H

#include "tuya_cloud_com_defs.h"
#include "uni_log.h"
#include "ty_cJSON.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    TY_IR_STUDY_START = 0,     
    TY_IR_STUDY_EXIT = 1,
    
}TY_IR_STUDY_CTL_E;

typedef struct {
    USHORT_T    *code;
    USHORT_T    code_len;
    BYTE_T      send_count;     //repeat send count, don't need delay
    UINT_T      feq;
    UINT16_T    delay;          //ms
    
}TY_IR_CODE_S;

typedef VOID (*TY_IR_STUDY_CTL_CB)(TY_IR_STUDY_CTL_E mode);
typedef VOID (*TY_IR_SEND_CB)(TY_IR_CODE_S *ir_code, UINT8_T code_num);

typedef struct {
    TY_IR_STUDY_CTL_CB  study_ctl_cb;
    TY_IR_SEND_CB       send_cb;
    
}TY_IR_CONTROL_CBS_S;

OPERATE_RET tuya_ir_control_register(IN CONST TY_IR_CONTROL_CBS_S *p_ir_cbs);
OPERATE_RET tuya_ir_study_code_report(IN CONST USHORT_T *p_code, IN CONST UINT_T code_len);
OPERATE_RET tuya_ir_study_error_report();
OPERATE_RET tuya_ir_code_free(IN       TY_IR_CODE_S *ir_code, IN UINT8_T code_num);
OPERATE_RET tuya_ir_study_stat_report(IN TY_IR_STUDY_CTL_E mode);

#ifdef __cplusplus
}
#endif

#endif  

