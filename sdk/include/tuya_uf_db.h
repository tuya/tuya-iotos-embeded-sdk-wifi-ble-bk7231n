/**
 * @file tuya_uf_db.h
 * @brief 
 * @date 2019-04-25
 * 
 * copyright Copyright (c) {2018-2020} 涂鸦科技 www.tuya.com
 * 
 */
#ifndef _TUYA_UF_DB_H_
#define _TUYA_UF_DB_H_
#include "tuya_cloud_types.h"


#ifdef __cplusplus
extern "C" {
#endif


OPERATE_RET uf_db_init(VOID);

OPERATE_RET uf_db_user_param_write(IN CONST BYTE_T *data,IN CONST UINT_T len);
OPERATE_RET uf_db_user_param_read(OUT BYTE_T **buf,OUT UINT_T *len);


#ifdef __cplusplus
} // extern "C"
#endif


#endif

