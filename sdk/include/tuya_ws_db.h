/***********************************************************
*  File: tuya_ws_db.h
*  Author: nzy
*  Date: 20150601
***********************************************************/
#ifndef _TUYA_WS_DB_H
#define _TUYA_WS_DB_H

#include "tuya_cloud_types.h"

#ifdef __cplusplus
    extern "C" {
#endif



/***********************************************************
*  Function: ws_db_init
*  Input:
*  Output:
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET ws_db_init(IN CONST CHAR_T *fs_storge_path, IN CONST BYTE_T *p_key);

/***********************************************************
*  Function: ws_db_uninit
*  Input: none
*  Output: none
*  Return: none
***********************************************************/
VOID ws_db_uninit(VOID) ;

/***********************************************************
*  Function: wd_common_write
*  Input:key value len
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET wd_common_write(IN CONST CHAR_T *key, IN CONST BYTE_T *value, IN CONST UINT_T len);

/***********************************************************
*  Function: wd_common_read
*  Input: key
*  Output: value p_len
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET wd_common_read(IN CONST CHAR_T *key, OUT BYTE_T **value, OUT UINT_T *p_len);

OPERATE_RET wd_common_fuzzy_read(IN CONST CHAR_T *fuzzy_name,INOUT UINT_T *index,OUT BYTE_T **data,OUT UINT_T *len);

/***********************************************************
*  Function: wd_common_free_data
*  Input: data
*  Output:
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET wd_common_free_data(IN BYTE_T *data);

/***********************************************************
*  Function: wd_common_delete
*  Input: key
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET wd_common_delete(IN CONST CHAR_T *key);

/***********************************************************
*  Function: wd_common_fuzzy_delete
*  Input: key
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET wd_common_fuzzy_delete(IN CONST CHAR_T *key);

/**
 * @brief tuya protected storage write entry
 * 
 * @param[in] key key of the entry you want to write
 * @param[in] value value buffer you want to write 
 * @param[in] len the numbers of byte you want to write
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h 
 */
OPERATE_RET wd_protected_write(IN CONST CHAR_T *key, IN CONST BYTE_T *value, IN CONST UINT_T len);

/**
 * @brief tuya protected storage read entry
 * 
 * @param[in] key  key of the entry you want to read
 * @param[out] value buffer of the value
 * @param[out] p_len length of the buffer
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h  
 * 
 * @note must free the value buffer with wd_common_free_data when you nolonger need the buffer
 */
OPERATE_RET wd_protected_read(IN CONST CHAR_T *key, OUT BYTE_T **value, OUT UINT_T *p_len);

/**
 * @brief delete the entry from protected storage
 * 
 * @param[in] key key of the entry you want to delete
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h    
 */
OPERATE_RET wd_protected_delete(IN CONST CHAR_T *key);

/***********************************************************
*  Function: wd_user_param_write
*  Input: data len
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET wd_user_param_write(IN CONST BYTE_T *data,IN CONST UINT_T len);

/***********************************************************
*  Function: wd_user_param_read
*  Input: buf len
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET wd_user_param_read(OUT BYTE_T **buf,OUT UINT_T *len);

/***********************************************************
*  Function: ws_db_ffs_keys_write
*  Input: data len
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET ws_db_ffs_keys_write(IN CONST BYTE_T *data,IN CONST UINT_T len);

/***********************************************************
*  Function: ws_db_ffs_keys_read
*  Input: buf len
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET ws_db_ffs_keys_read(OUT BYTE_T **buf,OUT UINT_T *len);


OPERATE_RET wd_encrypt_if_write(IN CONST CHAR_T *dev_id, BYTE_T *p_key);
OPERATE_RET wd_encrypt_if_read(IN CONST CHAR_T *dev_id, BYTE_T *p_key);
OPERATE_RET wd_encrypt_if_delete(IN CONST CHAR_T *dev_id);
OPERATE_RET wd_encrypt_delete_all(VOID);



typedef BYTE_T VAR_TP_T;
#define VT_CHAR 0
#define VT_BYTE 1
#define VT_SHORT 2
#define VT_USHORT 3
#define VT_INT 4
#define VT_BOOL 5
#define VT_STRING 6
#define VT_RAW 7

typedef struct {
    CHAR_T *key;
    VAR_TP_T vt;
    VOID *val;
    USHORT_T len;
}TY_DB_RW_S;

OPERATE_RET wd_utils_serialize(IN CONST TY_DB_RW_S *rw,IN CONST UINT_T rw_cnt,
                               OUT CHAR_T **out,OUT UINT_T *out_len);
OPERATE_RET wd_utils_free_outbuf(IN CHAR_T *out_buf);
OPERATE_RET wd_utils_deserialize(IN CONST CHAR_T *in,INOUT TY_DB_RW_S *rw,IN CONST UINT_T rw_cnt);


OPERATE_RET wd_utils_serialize_save(IN CONST CHAR_T *key, IN CONST TY_DB_RW_S *rw,IN CONST UINT_T rw_cnt);
OPERATE_RET wd_utils_serialize_restore(IN CONST CHAR_T *key,INOUT TY_DB_RW_S *rw,IN CONST UINT_T rw_cnt);



#ifdef __cplusplus
}
#endif
#endif

