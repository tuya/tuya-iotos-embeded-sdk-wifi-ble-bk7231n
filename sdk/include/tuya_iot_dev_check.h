/**
 * @file ty_dev_cloud_check.h
 * @brief 
 * @date 2019-12-19
 * 
 * copyright Copyright (c) {2018-2020} 涂鸦科技 www.tuya.com
 * 
 */
#ifndef __TUYA_IOT_DEV_CHECK__
#define __TUYA_IOT_DEV_CHECK__
#include "tuya_cloud_types.h"

#define TY_DEV_RANDOM_LENGTH 16
#define TY_DEV_KEY_LENGTH 16 //取authkey前16字节
#define TY_DEV_SHA256_DIGEST_SIZE 32 //uint8 type 512 = 32*8
#define TY_DEV_RANDOM_AES_LEN 32 //16 byte->aes128ecb->32 byte
#define TY_DEV_SESSION_KEY_LEN 16
#define ENCRYPT_AUTHKEY_HMACRA_NAME "hmacRandomA"
#define ENCRYPT_AUTHKEY_HMACRB_NAME "hmacRandomB"
#define ENCRYPT_AUTHKEY_AESRB_NAME  "aesRandomB"
#define ENCRYPT_AUTHKEY_SESSIONKEY_NAME "sessionKey"
#define TY_DEV_IS_INDEPENDENT_NAME "isIndependent"
#define TY_DEV_IS_SUB_NAME "isSub"
#define TY_DEV_IS_FAMILY_NAME "isFamily"

/**
 * @brief dev生成RA,ra_aes,hmac(ra+uuid),hmac(ra)，用于三向认证
 * 
 * @param BYTE_T RA
 * @param BYTE_T ra_en_hex
 * @param BYTE_T ra_uuid_hmac
 * @param BYTE_T ra_hmac
 * @return OPERATE_RET 
 */
OPERATE_RET tuya_dev_gen_ra_check(OUT BYTE_T RA[TY_DEV_RANDOM_LENGTH],OUT BYTE_T ra_en_hex[TY_DEV_RANDOM_AES_LEN],
   OUT BYTE_T ra_uuid_hmac[TY_DEV_SHA256_DIGEST_SIZE], OUT BYTE_T ra_hmac[TY_DEV_SHA256_DIGEST_SIZE]);

/**
 * @brief gw通过uuid+ra_aes+hmac(ra+uuid)到云端交换ra_hmac,rb_aes
 *  
 */
OPERATE_RET tuya_dev_http_ra_check_exchange(IN CONST CHAR_T* uuid, IN CONST BYTE_T* ra_aes_hex, UINT_T ra_aes_hex_len,
    IN CONST BYTE_T* ra_uuid_hmac_hex, UINT_T ra_uuid_hmac_hex_len, OUT BYTE_T* ra_hmac, OUT BYTE_T* rb_aes,
    OUT BYTE_T* rb_hmac, OUT BYTE_T* session_key);

/**
 * @brief dev由RA,rb_aes,authkey计算rb_hmac,session_key(16字节)
 * 
 */
OPERATE_RET tuya_dev_rarb_session_key_calc(IN CONST BYTE_T* auth_key, IN BYTE_T rb_aes[TY_DEV_RANDOM_AES_LEN],
    IN BYTE_T RA[TY_DEV_RANDOM_LENGTH], OUT BYTE_T rb_hmac[TY_DEV_SHA256_DIGEST_SIZE],
    OUT BYTE_T session_key[TY_DEV_SESSION_KEY_LEN]);

/**
 * @brief dev生成hmac(uuid)
 * 
 * @param BYTE_T 
 * @return OPERATE_RET 
 */
OPERATE_RET tuya_dev_gen_uuid_hmac(OUT BYTE_T uuid_hmac[TY_DEV_SHA256_DIGEST_SIZE]);

/**
 * @brief gw由uuid+hmac(uuid)到云端交换dev和gw的关系
 * isIndependent  是否独立 1 是 0 否 
 * isSub 是否父子设备 1 是 0 否
 * isFamily 是否同一家庭 1 是  0 否
 * 
 */
OPERATE_RET tuya_dev_http_relation_exchange(IN CONST CHAR_T* uuid, IN CONST BYTE_T* hmacAuthKey_hex, UINT_T hmacAuthKey_hex_len,
    OUT INT_T *isIndependent, OUT INT_T *isSub, OUT INT_T *isFamily);
#endif