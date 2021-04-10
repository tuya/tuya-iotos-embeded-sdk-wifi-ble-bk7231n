#ifndef __LAN_CLUSTER_MASTER_CORE_H
#define __LAN_CLUSTER_MASTER_CORE_H


#include "tuya_cloud_com_defs.h"
#include "lan_cluster_base.h"


#ifdef __cplusplus
extern "C" {
#endif

//master core模块初始化
OPERATE_RET lan_cluster_master_core_init(IN CONST	   LAN_NODE_S* node);


//注册本节点的状态变化回调
OPERATE_RET lan_cluster_master_reg_state_cb(IN STATE_CHANGE_CALLBACK cb);


//注册集群中节点状态变化回调，只有master节点会调用，当前是全量
OPERATE_RET lan_cluster_master_reg_node_cb(IN CLS_NODE_CHANGE_CALLBACK cb);

//注册集群中节点状态变化回调，只有master节点会调用，当前是增量
OPERATE_RET lan_cluster_master_reg_node_single_cb(IN CLS_NODE_CHANGE_SINGLE_CALLBACK cb);


#ifdef __cplusplus
}
#endif
#endif

