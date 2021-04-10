#ifndef TUYA_TLS_H
#define TUYA_TLS_H

/* mbedtls 仅仅用于通道加密通讯，不再用于通道建立。*/

#include "tuya_cloud_types.h"
#include "ssl.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    CONST UINT8_T *pub_perm;
    CONST UINT8_T *pri_perm;
    UINT16_T pub_perm_len;
    UINT16_T pri_perm_len;
}tuya_tls_cert_perm_t;

typedef PVOID_T tuya_tls_hander;

typedef INT_T (*tuya_tls_send_cb)( VOID *p_custom_net_ctx, CONST BYTE_T *buf, SIZE_T len);
typedef INT_T (*tuya_tls_recv_cb)( VOID *p_custom_net_ctx, BYTE_T *buf, SIZE_T len);

VOID tuya_tls_load_der_info(VOID);

VOID tuya_tls_register_constant(IN CHAR_T *p_uuid,  IN CHAR_T *p_authkey, IN CHAR_T *p_psk_key);

/*
VERIFY_NONE       0
VERIFY_OPTIONAL   1
VERIFY_REQUIRED   2
*/
VOID tuya_tls_set_ssl_verify(IN CONST INT_T verify_mode);
INT_T tuya_tls_register_x509_crt_der(VOID *p_ctx, UCHAR_T *p_der, UINT_T der_len);

VOID tuya_tls_set_load_cert(BOOL_T load);
void tuya_tls_set_coustom_cert(CHAR_T *cert, INT_T len);
void tuya_tls_set_client_cert_pkey(CHAR_T *cert, INT_T len, CHAR_T *client_pkey, INT_T pkey_len);

typedef enum
{
    TY_TLS_CERT_EXPIRED,
}TY_TLS_EVENT_E;
typedef VOID (*tuya_tls_event_cb)(TY_TLS_EVENT_E event, VOID *p_args);
OPERATE_RET tuya_tls_init(IN tuya_tls_event_cb event_cb);

OPERATE_RET tuya_tls_connect(OUT tuya_tls_hander *p_tls_handler, IN CHAR_T *hostname,IN INT_T port_num, IN INT_T sethostname,
                             IN VOID *p_custom_net_ctx, IN tuya_tls_send_cb send_cb, IN tuya_tls_recv_cb recv_cb,
                             IN INT_T socket_fd, IN INT_T overtime_s);

INT_T tuya_tls_write(IN tuya_tls_hander tls_handler, BYTE_T *buf, UINT_T len);
INT_T tuya_tls_read(IN tuya_tls_hander tls_handler, BYTE_T *buf, UINT_T len);
OPERATE_RET tuya_tls_disconnect(IN tuya_tls_hander tls_handler);

int __tuya_tls_random(void *p_rng, unsigned char *output, size_t output_len);
#ifdef __cplusplus
} // extern "C"
#endif


#endif//TUYA_TLS_H


