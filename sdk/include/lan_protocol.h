#ifndef  __LAN_PROTOCOL__
#define __LAN_PROTOCOL__

#include "tuya_cloud_types.h"
#include "crc32i.h"
#include "uni_log.h"
#include "mem_pool.h" 
#include "aes_inf.h"
#include "mix_method.h"
#include "crc32i.h"
#ifdef __cplusplus
	extern "C" {
#endif 

// lan protocol
#define FRM_TP_CFG_WF 1 // only used for ap 3.0 network config
// #define FRM_TP_ACTV 2 // discard
// #define FRM_TP_BIND_DEV 3
// #define FRM_TP_UNBIND_DEV 6
#define FRM_TP_CMD 7
#define FRM_TP_STAT_REPORT 8
#define FRM_TP_HB 9
#define FRM_QUERY_STAT 0x0a
// #define FRM_SSID_QUERY 0x0b // discard
#define FRM_USER_BIND_REQ 0x0c
#define FRM_TP_NEW_CMD 0x0d
#define FRM_ADD_SUB_DEV_CMD 0x0e
#define FRM_CFG_WIFI_INFO 0x0f
#define FRM_QUERY_STAT_NEW 0x10
#define FRM_SCENE_EXEC 0x11
#define FRM_LAN_QUERY_DP 0x12

#if defined(ENABLE_LAN_ENCRYPTION) && (ENABLE_LAN_ENCRYPTION==1)
#define FR_TYPE_ENCRYPTION 0x13
#define FRM_AP_CFG_WF_V40 0x14
#endif

#if defined(ENABLE_IPC) && (ENABLE_IPC == 1)
#define FRM_LAN_P2P 0x20
#endif



typedef int AP_CFG_ERR_CODE;
#define AP_CFG_SUCC  0
#define AP_CFG_ERR_PCK  1
#define AP_CFG_AP_NOT_FOUND 2
#define AP_CFG_ERR_PASSWD 3
#define AP_CFG_CANT_CONN_AP 4
#define AP_CFG_DHCP_FAILED 5
#define AP_CFG_CONN_CLOUD_FAILED 6


#pragma pack(1)
// lan protocol app head
typedef struct
{
    UINT_T head; // 0x55aa
    UINT_T fr_num;
    UINT_T fr_type;
    UINT_T len;
    BYTE_T data[0];
}LAN_PRO_HEAD_APP_S;

typedef struct {
    UINT_T crc;
    UINT_T tail; // 0xaa55
}LAN_PRO_TAIL_S;

// lan protocol gateway head
typedef struct
{
    UINT_T head; // 0x55aa
    UINT_T fr_num;
    UINT_T fr_type;
    UINT_T len;
    UINT_T ret_code;
    BYTE_T data[0];
}LAN_PRO_HEAD_GW_S;
#pragma pack()


#if defined(ENABLE_LAN_ENCRYPTION) && (ENABLE_LAN_ENCRYPTION==1)

STATIC OPERATE_RET __lan_parse_data(IN CONST BYTE_T *data,IN CONST INT_T len,IN CONST BYTE_T *key,OUT CHAR_T **out_data)
{
    BYTE_T *ec_data = NULL;
    UINT_T ec_len = 0;
    OPERATE_RET op_ret = OPRT_OK;

    op_ret = aes128_ecb_decode(data,len,&ec_data,&ec_len,key);
    if(OPRT_OK != op_ret)
    {
        PR_ERR("aes128_ecb_decode error:%d",op_ret);
        return op_ret;
    }

    // delete useless char
    int offset = str_revr_find_ch((CHAR_T *)ec_data,0,'}');
    if(offset < 0)
    {
        PR_ERR("find last } fails %d %s", offset, (CHAR_T *)ec_data);
        Free(ec_data);
        return OPRT_COM_ERROR;
    }
    ec_data[offset+1] = 0;
    PR_TRACE("lan parse decode buf:%s", ec_data);

    *out_data = (CHAR_T *)ec_data;

    return OPRT_OK;
}


STATIC OPERATE_RET encrpt_lan_msg(IN CONST BYTE_T *data,IN CONST UINT_T len,
                                      OUT BYTE_T **ec_data,OUT UINT_T *ec_len,IN CONST BYTE_T *key)
{
    OPERATE_RET op_ret = OPRT_OK;
    op_ret = aes128_ecb_encode(data,len,ec_data,ec_len,key);

    if(OPRT_OK != op_ret)
    {
        PR_ERR("aes128_ecb_encode error:%d",op_ret);
        return op_ret;
    }

    return OPRT_OK;
}

#endif

/* 数据前后封装55aa */
STATIC BYTE_T *__mlp_gw_send_da(IN CONST UINT_T fr_num,IN CONST UINT_T fr_type,\
                              IN CONST UINT_T ret_code,IN CONST BYTE_T *data,\
                              IN CONST UINT_T len,OUT UINT_T *s_len)
{
    UINT_T send_da_len = sizeof(LAN_PRO_HEAD_GW_S) + len + sizeof(LAN_PRO_TAIL_S);
    BYTE_T *send_da = Malloc(send_da_len);
    if(send_da == NULL)
    {
        PR_ERR("Malloc Fails %d", send_da_len);
        return NULL;
    }

    UINT_T p = 0x55aa;
    LAN_PRO_HEAD_GW_S *head_gw = (LAN_PRO_HEAD_GW_S *)send_da;
    head_gw->head = UNI_HTONL(p);
    head_gw->fr_num = UNI_HTONL(fr_num);
    head_gw->fr_type = UNI_HTONL(fr_type);
    head_gw->len = UNI_HTONL(len+sizeof(LAN_PRO_TAIL_S)+sizeof(ret_code));
    head_gw->ret_code = UNI_HTONL(ret_code);
    memcpy(head_gw->data,data,len);

    UINT_T crc = 0;
    crc = hash_crc32i_total(send_da,(send_da_len-sizeof(LAN_PRO_TAIL_S)));

    LAN_PRO_TAIL_S *pro_tail = (LAN_PRO_TAIL_S *)(send_da + sizeof(LAN_PRO_HEAD_GW_S) + len);
    pro_tail->crc = UNI_HTONL(crc);
    p = 0xaa55;
    pro_tail->tail = UNI_HTONL(p);
    *s_len = send_da_len;

    return send_da;
}

#ifdef __cplusplus
		}
#endif 


#endif
