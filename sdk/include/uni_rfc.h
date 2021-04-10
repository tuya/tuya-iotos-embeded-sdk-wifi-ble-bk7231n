/***********************************************************
*  File: uni_rfc.h
*  Author: nzy
*  Date: 20160823
***********************************************************/
#ifndef _UNI_RFC_H
#define _UNI_RFC_H

    #include "tuya_cloud_types.h"
    #include "tuya_hal_mutex.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef  __UNI_RFC_GLOBALS
    #define __UNI_RFC_EXT
#else
    #define __UNI_RFC_EXT extern
#endif

/***********************************************************
*************************micro define***********************
***********************************************************/
#define RFC_MUTEX 1
typedef struct {
    #if RFC_MUTEX
    MUTEX_HANDLE mutex;
    #endif
    BOOL_T malk;
    UINT_T rfc;
    UINT_T data_len;
    VOID *data;
}RFC_DATA_S;

/***********************************************************
*************************variable define********************
***********************************************************/

/***********************************************************
*************************function define********************
***********************************************************/
__UNI_RFC_EXT \
RFC_DATA_S *cr_rfc_data(IN VOID *data,IN CONST UINT_T data_len,\
                        IN CONST BOOL_T malk,IN CONST UINT_T cnt);

__UNI_RFC_EXT \
VOID get_rfc_data(INOUT RFC_DATA_S *rfc_data);

__UNI_RFC_EXT \
VOID put_rfc_data(INOUT RFC_DATA_S *rfc_data);

__UNI_RFC_EXT \
VOID del_rfc_data(INOUT RFC_DATA_S *rfc_data);

__UNI_RFC_EXT \
UINT_T check_rfc_data(INOUT RFC_DATA_S *rfc_data);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
