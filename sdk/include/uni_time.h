/*
uni_time.h
Copyright(C),2018-2020, 涂鸦科技 www.tuya.comm
*/

#ifndef _UNI_TIME_H
#define _UNI_TIME_H

#include "tuya_cloud_types.h"
#include "tuya_hal_system.h"


#ifdef __cplusplus
extern "C" {
#endif

#ifdef  __UNI_TIME_GLOBALS
    #define __UNI_TIME_EXT
#else
    #define __UNI_TIME_EXT extern
#endif

/* tuya sdk definition of struct tm */
typedef struct {
    INT_T tm_sec;     /* seconds [0-59] */
    INT_T tm_min;     /* minutes [0-59] */
    INT_T tm_hour;    /* hours [0-23] */
    INT_T tm_mday;    /* day of the month [1-31] */
    INT_T tm_mon;     /* month [0-11] */
    INT_T tm_year;    /* year. The number of years since 1900 */
    INT_T tm_wday;    /* day of the week [0-6] 0-Sunday...6-Saturday */
}POSIX_TM_S;

/* summer time zone function */
#define SUM_ZONE_TAB_LMT 6
typedef struct {
    TIME_T posix_min;
    TIME_T posix_max;
}SUM_ZONE_S;

/***********************************************************
*  Function: uni_time_init
*  Desc:     init time-management module
*  Return:   OPRT_OK: success  Other: fail
***********************************************************/
__UNI_TIME_EXT \
OPERATE_RET uni_time_init(VOID);

/***********************************************************
*  Function: uni_mktime
*  Desc:     change tm to time_t, redefine the std C func mktime
*  Input:    tm
*  Return:   the changed time_t
***********************************************************/
__UNI_TIME_EXT \
TIME_T uni_mktime(IN CONST POSIX_TM_S *tm);

/***********************************************************
*  Function: uni_gmtime_r
*  Desc:     change tm_t to tm, redefine the std C func gmtime_r
*  Input:    tm_t
*  Output && Return: the changed tm
***********************************************************/
__UNI_TIME_EXT \
POSIX_TM_S *uni_gmtime_r(IN CONST TIME_T *tm, OUT POSIX_TM_S *result);

/***********************************************************
*  Function: uni_time_set
*  Desc:     update tuya-sdk inside time, tm format
*  Input:    tm
*  Return:   OPRT_OK: success  Other: fail
***********************************************************/
__UNI_TIME_EXT \
OPERATE_RET uni_time_set(IN CONST POSIX_TM_S *tm);

/***********************************************************
*  Function: uni_time_get
*  Desc:     get tuya-sdk inside time, tm format
*  Output:   tm
*  Return:   OPRT_OK: success  Other: fail
***********************************************************/
__UNI_TIME_EXT \
OPERATE_RET uni_time_get(OUT POSIX_TM_S *tm);

/***********************************************************
*  Function: uni_http_date_to_time
*  Desc:     change http-GMT format time to time_t
*  Input:    date: http-GMT format time
*  Return:   time_t
***********************************************************/
__UNI_TIME_EXT \
TIME_T uni_http_date_to_time(IN CONST CHAR_T *date);

/***********************************************************
*  Function: uni_time_set_posix
*  Desc:     update tuya-sdk inside time, time_t format
*  Input:    time
*  Input:    update_source(0:rtc 1:cloud 2:other)
*  Return:   OPRT_OK: success  Other: fail
***********************************************************/
__UNI_TIME_EXT \
OPERATE_RET uni_time_set_posix(IN CONST TIME_T time, IN CONST INT_T update_source);

__UNI_TIME_EXT \
TIME_T uni_time_get_cur_posix(VOID);

/***********************************************************
*  Function: uni_time_check_time_sync
*  Desc:     check tuya-sdk inside time is synced with tuya-cloud
*  Return:   OPRT_OK: synced  Other: fail
***********************************************************/
__UNI_TIME_EXT \
OPERATE_RET uni_time_check_time_sync(VOID);

/***********************************************************
*  Function: uni_time_check_time_zone_sync
*  Desc:     check tuya-sdk inside time zone is synced with tuya-cloud
*  Return:   OPRT_OK: success  Other: fail
***********************************************************/
__UNI_TIME_EXT \
OPERATE_RET uni_time_check_time_zone_sync(VOID);


/***********************************************************
*  Function: uni_time_get_posix
*  Desc:     get tuya-sdk inside time, time_t format
*  Return:   time_t
***********************************************************/
__UNI_TIME_EXT \
TIME_T uni_time_get_posix(VOID);

__UNI_TIME_EXT \
SYS_TICK_T uni_time_get_posix_ms(VOID);
__UNI_TIME_EXT uint32_t uni_time_get_posix_ms_remain_tick(VOID);

/***********************************************************
*  Function: uni_get_system_time
*  Desc:     get the system running time
*  Output:   pSecTime: s pMsTime: ms
***********************************************************/
__UNI_TIME_EXT \
VOID uni_get_system_time(OUT TIME_S *pSecTime,OUT TIME_MS *pMsTime);

/***********************************************************
*  Function: uni_set_time_zone
*  Desc:     set tuya-sdk inside timezone
*  Input:    time_zone: "+/-hh:mm"
*  Return:   OPRT_OK: success  Other: fail
***********************************************************/
__UNI_TIME_EXT \
OPERATE_RET uni_set_time_zone(IN CONST CHAR_T *time_zone);

/***********************************************************
*  Function: uni_get_time_zone_seconds
*  Desc:     get tuya-sdk inside timezone, in second format
*  Output:   time_zone: timezone in second format
*  Return:   OPRT_OK: success  Other: fail
***********************************************************/
__UNI_TIME_EXT \
OPERATE_RET uni_get_time_zone_seconds(OUT INT_T *time_zone);


__UNI_TIME_EXT \
OPERATE_RET uni_set_time_zone_seconds(IN INT_T time_zone_sec);

/***********************************************************
*  Function: uni_local_time_get
*  Desc:     change the current time to tm_s,including the
*            timezone and the summer time zone info
*  Output:   tm
*  Return:   OPRT_OK: success  Other: fail
***********************************************************/
__UNI_TIME_EXT \
OPERATE_RET uni_local_time_get(OUT POSIX_TM_S *tm);

/***********************************************************
*  Function: uni_local_time_get_custom
*  Desc:     change the time_t to tm_s,including the timezone and the summer time zone info
*  Input:    in_time: if in_time is 0, then in_time = uni_time_get_posix()
*  Output:   tm
*  Return:   OPRT_OK: success  Other: fail
***********************************************************/
__UNI_TIME_EXT \
OPERATE_RET uni_local_time_get_custom(IN TIME_T in_time, OUT POSIX_TM_S *tm);

/***********************************************************
*  Function: uni_sum_time_get
*  Desc:     change tuya-sdk inside time to tm_s,including the summer time zone info
*  Output:   tm
*  Return:   OPRT_OK: success  Other: fail
***********************************************************/
__UNI_TIME_EXT \
OPERATE_RET uni_sum_time_get(OUT POSIX_TM_S *tm);

/***********************************************************
*  Function: uni_set_sum_zone_tbl
*  Desc:     update tuya-sdk summer time zone info
*  Input:    zone cnt
***********************************************************/
__UNI_TIME_EXT \
VOID uni_set_sum_zone_tbl(IN CONST SUM_ZONE_S *zone,IN CONST UINT_T cnt);

/***********************************************************
*  Function: uni_sum_time_get_posix
*  Input: none
*  Output: none
*  Return: TIME_T
***********************************************************/
__UNI_TIME_EXT \
TIME_T uni_sum_time_get_posix(VOID);

__UNI_TIME_EXT \
BOOL_T uni_is_in_sum_zone(TIME_T time);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _UNI_TIME_H */


