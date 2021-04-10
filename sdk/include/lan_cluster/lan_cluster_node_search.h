
#ifndef __LAN_CLUSTER_NODE_SEARCH_H
#define __LAN_CLUSTER_NODE_SEARCH_H

#include "tuya_cloud_com_defs.h"
#include "uni_network.h"

#ifdef __cplusplus
extern "C" {
#endif


OPERATE_RET lan_cluster_udp_search_init();

typedef OPERATE_RET (*RECV_UDP_SEARCH_MSG_CB)(UNW_IP_ADDR_T ip, IN CONST CHAR_T *data);

VOID lan_cluster_recv_udp_search_msg_rsg(RECV_UDP_SEARCH_MSG_CB udp_search_msg_cb);


#ifdef __cplusplus
}
#endif
#endif

