/***********************************************************
*  File: tuya_key.h
*  Author: nzy
*  Date: 20171117
***********************************************************/
#ifndef __TUYA_KEY_H
    #define __TUYA_KEY_H
	#include <stdint.h>

    #include "tuya_cloud_types.h"
    #include "tuya_pin.h"

#ifdef __cplusplus
	extern "C" {
#endif

#ifdef  __TUYA_KEY_GLOBALS
    #define __TUYA_KEY_EXT
#else
    #define __TUYA_KEY_EXT extern
#endif

/***********************************************************
*************************micro define***********************
***********************************************************/
typedef enum {
    NORMAL_KEY = 0, // 普通单击触发
    SEQ_KEY, // 连击计数键
    LONG_KEY, // 长按键
}PUSH_KEY_TYPE_E;

typedef enum {
    LP_INVALID = 0, // long press invalid
    LP_ONCE_TRIG,// long press once trigger
    LP_MORE_NORMAL_TRIG, // long press more normal trigger
    FALLING_EDGE_TRIG,//press key immedialtely trigger
    FALLING_LONG_TRIG,//press key immedialtely trigger & LONG
}KEY_LONG_PRESS_TP_E;

typedef VOID(* KEY_CALLBACK)(tuya_pin_name_t port,PUSH_KEY_TYPE_E type,INT_T cnt);

// (cnt >= 2) ==> SEQ_KEY
// time < long_key_time && (cnt == 1) ==> NORMAL_KEY
// time >= long_key_time && (cnt == 1) ==> LONG_KEY
typedef struct { // user define
    tuya_pin_name_t port;
    BOOL_T low_level_detect; // 低电平检测
    KEY_LONG_PRESS_TP_E lp_tp;
    USHORT_T long_key_time; // ms (lp_tp == LP_ONCE_TRIG then valid and must >= 1000ms)
    USHORT_T seq_key_detect_time; // ms 0:disable default:400ms
    KEY_CALLBACK call_back;
}KEY_USER_DEF_S;


/***********************************************************
*************************variable define********************
***********************************************************/

/***********************************************************
*************************function define********************
***********************************************************/
OPERATE_RET reg_proc_key(IN CONST KEY_USER_DEF_S *key_ud);
OPERATE_RET key_init(IN CONST KEY_USER_DEF_S *p_tbl,IN CONST INT_T cnt,\
                         IN CONST INT_T timer_space);


#ifdef __cplusplus
}
#endif
#endif

