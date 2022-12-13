
#ifndef __TUYA_FFS_INC_H__
#define __TUYA_FFS_INC_H__


#ifdef __cplusplus
extern "C" {
#endif


#include "tuya_ffs_config.h"
#include "netcfg_module.h"

#include <string.h>

#include "tuya_cloud_com_defs.h"
#include "tuya_cloud_types.h"
#include "http_inf.h"

#include "ty_cJSON.h"
#include "tuya_hal_system.h"
#include "tuya_hal_thread.h"
#include "tuya_hal_wifi.h"
#include "uni_thread.h"

#include "uni_log.h"
//#include "iot_httpc.h"
#include "http_inf.h"
#include "tuya_tls.h"
#include "mem_pool.h"


#if TUYA_FFS_SUPPORT_OPENSSL
#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <openssl/sha.h>
#endif

#if TUYA_FFS_SUPPORT_MBEDTLS
#include "mbedtls/md.h"
#include "mbedtls/ecdh.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/sha256.h"
#include "mbedtls/base64.h"
#include "mbedtls/pk.h"


typedef struct {
    char* pid;          // the length is 4, ref TEST_PRODUCT_INDEX "Q9pp"
    char* pin;
    char* sn;
    char* cert_pem;
    char* private_pem;
    char* public_pem;
	int netcfg_type;
}tuya_ffs_config_t;


#endif

#ifdef __cplusplus
    } // extern "C"
#endif
    
#endif // __TUYA_FFS_INC_H__


