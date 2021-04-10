#ifndef _COAP_SERVER_H
#define _COAP_SERVER_H

#include "tuya_cloud_types.h"
#include "tuya_os_adapter.h"

#include "coap.h"

#ifdef __cplusplus
    extern "C" {
#endif

typedef PVOID_T TY_COAP_SERVER_HANLDER;

OPERATE_RET ty_coap_add_resource(IN TY_COAP_SERVER_HANLDER handler, IN CONST CHAR_T *p_uri, \
                                 coap_method_handler_t post_cb, coap_method_handler_t get_cb, \
                                 coap_method_handler_t put_cb, coap_method_handler_t delete_cb );

typedef OPERATE_RET (*ty_coap_init_resource_cb)(IN TY_COAP_SERVER_HANLDER handler);

OPERATE_RET ty_coap_server_start(OUT TY_COAP_SERVER_HANLDER *p_handler, IN CONST UINT_T port, ty_coap_init_resource_cb init_res_cb);

typedef struct
{
    SHORT_T type;
    UINT_T length;
    CHAR_T *p_data;
}TY_COAP_OPTION_S;

/*
upload_mode: COAP_REQUEST_POST
msg_type: COAP_MESSAGE_CON
*/

OPERATE_RET ty_coap_send_custom_msg(IN TY_COAP_SERVER_HANLDER server_handler,
                                    CHAR_T *url, UCHAR_T upload_mode, UCHAR_T msg_type,
                                    CHAR_T *data, TY_COAP_OPTION_S *option_arr, UINT_T arr_size,
                                    coap_response_handler_t handler);

#ifdef __cplusplus
}
#endif
#endif
