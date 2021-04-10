
#ifndef __LAN_CLUSTER_MASTER_CONFIRM_H
#define __LAN_CLUSTER_MASTER_CONFIRM_H

#include "tuya_cloud_com_defs.h"
#include "uni_network.h"

#ifdef __cplusplus
extern "C" {
#endif

OPERATE_RET lan_cluster_master_confirm_init();

OPERATE_RET lan_cluster_send_master_confirm_msg(IN CONST CHAR_T *data);


typedef VOID (*RECV_MASTER_CONFIRM_CB)(UNW_IP_ADDR_T  ip, IN CONST CHAR_T *data);

VOID lan_cluster_recv_master_confirm_msg_rsg(RECV_MASTER_CONFIRM_CB mst_cfm_cb);


#ifdef __cplusplus
}
#endif
#endif

