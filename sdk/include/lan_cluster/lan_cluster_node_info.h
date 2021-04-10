#ifndef __LAN_CLUSTER_NODE_INFO_H
    #define __LAN_CLUSTER_NODE_INFO_H

    #include "tuya_cloud_types.h"
    #include "lan_cluster_base.h"

#ifdef __cplusplus
    extern "C" {
#endif

#ifdef  __LAN_CLUSTER_NODE_INFO_GLOBALS
    #define __LAN_CLUSTER_NODE_INFO_EXT
#else
    #define __LAN_CLUSTER_NODE_INFO_EXT extern
#endif

/***********************************************************
*************************micro define***********************
***********************************************************/

/***********************************************************
*************************variable define********************
***********************************************************/

/***********************************************************
*************************function define********************
***********************************************************/


__LAN_CLUSTER_NODE_INFO_EXT \
OPERATE_RET lan_cluster_node_info_init(IN CONST LAN_NODE_S* node);	


#ifdef __cplusplus
}
#endif
#endif

