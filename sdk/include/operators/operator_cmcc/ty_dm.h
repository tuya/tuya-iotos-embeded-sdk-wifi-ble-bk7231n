#ifndef _TY_DM_H
#define _TY_DM_H

#include "tuya_operator.h"

#ifdef __cplusplus
    extern "C" {
#endif

#define AES_KEY     "P73NB4Va1p5te677ze978I29l3b1sB8p"
#define APP_KEY     "M100000298"
#define PRO_VER     "v2.0"
#define OBJ_DATA    "</1/1>,</2/1>,</3/0>"
#define COAP_CMCC_PORT      5683     /* CoAP default UDP/TCP port */

#define DEV_ROM        "2M"
#define DEV_RAM        "256K"
#define DEV_CPU        "ARM"
#define DEV_SYS_VER    "FreeRTOS v8.1.2"
//#define DEV_SOFT_VER   USER_SW_VER
//#define DEV_SOFT_NAME  APP_BIN_NAME
#define DEV_NET_TYPE   "WIFI"
#define BS_PATH        "rd"


#define DEVICE_INFO   6601
#define APP_INFO      6602
#define DM_MAC        6603
#define DM_ROM        6604
#define DM_RAM        6605
#define DM_CPU        6606
#define SYS_VER       6607
#define SOFT_VER      6608
#define SOFT_NAME     6609
#define DM_VOLTE      6610
#define NET_TYPE      6611
#define DM_ACCOUNT    6612
#define PHONE_NUM     6613
#define LOCATION      6614



//#define DM_TEST


#ifdef  DM_TEST

#define DM_URI     "coap://shipei.fxltsbl.com:5683"
#define HEAR_TIMER_CYCLE   (60*1000)
#define QUERY_BIND  "b=U"
#define BEAT_TIME   60
#define QUERY_TIME  "lt=60"

#else

#define DM_URI      "coap://m.fxltsbl.com:5683"
#define HEAR_TIMER_CYCLE   (24*60*60*1000)
#define QUERY_BIND  "b=U"
#define BEAT_TIME   86400
#define QUERY_TIME  "lt=86400"

#endif
OPERATE_RET operator_cmcc_start_send(IN CHAR_T *cmei_code, IN CONST TY_OPERATOR_INFO_S *p_info);

#ifdef __cplusplus
}
#endif
#endif

