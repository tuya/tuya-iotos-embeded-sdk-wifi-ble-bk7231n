#ifndef __BT_NETCFG__
#define __BT_NETCFG__

#include "tuya_ble_config.h"

#if defined(TY_BT_MOD) && (TY_BT_MOD==1)
#if defined(BT_NETCFG) && (BT_NETCFG==1)


typedef OPERATE_RET (*FN_BT_NET_CFG_CB)(IN CONST CHAR_T *ssid,IN CONST CHAR_T *passwd, IN CONST CHAR_T *token);
int bt_netcfg_init();

#endif
#endif
#endif
