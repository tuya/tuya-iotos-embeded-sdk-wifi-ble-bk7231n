/***********************************************************
*  File: app_agent.h
*  Author: nzy
*  Date: 20150618
***********************************************************/
#ifndef _APP_AGENT_H
#define _APP_AGENT_H

#include "tuya_cloud_types.h"
#include "uni_network.h"

#ifdef __cplusplus
    extern "C" {
#endif

#ifdef  __APP_AGENT_GLOBALS
    #define __APP_AGENT_EXT
#else
    #define __APP_AGENT_EXT extern
#endif



//group test
#define FRM_GRP_OPER_ENGR 0xd0
#define FRM_GRP_CMD 0xd1

#if defined(ENABLE_IPC) && (ENABLE_IPC == 1)
/* 局域网P2P信令通道，由P2P模块注册回调, 非线程安全 */
typedef INT_T (*LAN_IPC_FRM_RECV_CB)(IN CONST INT_T socket,IN CONST CHAR_T *dev_id, IN CONST CHAR_T * buffer, IN CONST INT_T len);

__APP_AGENT_EXT \
OPERATE_RET lan_ipc_frm_register_cb(IN CONST LAN_IPC_FRM_RECV_CB recv_cb);

__APP_AGENT_EXT \
OPERATE_RET lan_ipc_frm_unregister_cb();

__APP_AGENT_EXT \
OPERATE_RET lan_ipc_tcp_send(IN BYTE_T *data, IN UINT_T len);
#endif

__APP_AGENT_EXT \
OPERATE_RET lan_pro_cntl_init(BOOL_T wechat);

__APP_AGENT_EXT \
OPERATE_RET lan_pro_cntl_exit(VOID);

__APP_AGENT_EXT \
OPERATE_RET lan_pro_cntl_disable(VOID);

__APP_AGENT_EXT \
OPERATE_RET lan_pro_cntl_enable(VOID);

__APP_AGENT_EXT \
OPERATE_RET lan_dp_sata_report(IN CONST VOID *data,IN CONST UINT_T len);

__APP_AGENT_EXT \
OPERATE_RET lan_data_report_cb(IN CONST UINT_T fr_type,IN CONST UINT_T ret_code, \
                                     IN CONST BYTE_T *data,IN CONST UINT_T len);

__APP_AGENT_EXT \
INT_T lan_pro_cntl_get_valid_connect_cnt(VOID);

__APP_AGENT_EXT \
OPERATE_RET lan_disconnect_all_sockets(VOID_T);


#define LSTN_SVR_MAX_FD_NUM 2
typedef OPERATE_RET (*LSTN_SVR_GET_FDS_CB)(OUT INT_T *fds, IN OUT INT_T *fd_num);
typedef OPERATE_RET (*LSTN_SVR_ON_READ_CB)(IN CONST UNW_FD_SET_T *readfds, OUT INT_T *hdl_num);
typedef OPERATE_RET (*LSTN_SVR_ON_ERR_CB)(IN CONST UNW_FD_SET_T *errfds,  OUT INT_T *hdl_num);

typedef struct
{
	LSTN_SVR_GET_FDS_CB cb_get_fd;
	LSTN_SVR_ON_READ_CB cb_read;
	LSTN_SVR_ON_ERR_CB cb_err;
} LAN_LSTN_SVR_CB_S;

#if defined(ENABLE_LAN_LINKAGE) && (ENABLE_LAN_LINKAGE==1)

__APP_AGENT_EXT \
VOID lan_linkage_set_cap(IN USHORT_T cap);

//set seq for every time join the lan cluster
__APP_AGENT_EXT \
VOID lan_linkage_set_seqno(IN INT_T seqno);

__APP_AGENT_EXT \
OPERATE_RET lan_listen_server_rsg(IN CONST LAN_LSTN_SVR_CB_S *svr_cb);


//encrprt data to a frame use key2
//must Free ec_data
__APP_AGENT_EXT \
OPERATE_RET lan_encrpt_frame_key2(IN CONST BYTE_T *data,IN UINT_T in_len,  OUT BYTE_T **ec_data,OUT UINT_T *ec_len);


//decrprt data from a frame use key2
//must Free ec_data
__APP_AGENT_EXT \
OPERATE_RET lan_decrpt_frame_key2(IN CONST BYTE_T *data,IN UINT_T in_len,  OUT BYTE_T **ret_dec_data,OUT UINT_T *ret_dec_len);

#endif
#ifdef __cplusplus
}
#endif
#endif

