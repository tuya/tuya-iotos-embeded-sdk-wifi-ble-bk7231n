#ifndef _COMM_TASK_H_
#define _COMM_TASK_H_

#include "rwprf_config.h"
#if (BLE_COMM_SERVER)
#include <stdint.h>
#include "rwip_task.h" // Task definitions
#include "ble_compiler.h"


struct bk_ble_enable_req
{
    /// connection index
    uint8_t  conidx;
    ///  Configuration
    uint16_t  ind_cfg;
    
  	uint16_t  ntf_cfg;
    
};

struct bk_ble_enable_rsp
{
    /// connection index
    uint8_t conidx;
    uint16_t prf_id;
    ///status
    uint8_t status;
};

struct bk_ble_ntf_upd_req
{
    ///  instance
    uint8_t conidx;

    uint16_t length;

    uint16_t att_id;
    /// ffe1 Level
    uint8_t value[__ARRAY_EMPTY];
};

struct bk_ble_ind_upd_req
{
    ///  instance
    uint8_t conidx;

    uint16_t length;

    uint16_t att_id;
    /// ffe1 Level
    uint8_t value[__ARRAY_EMPTY];
};


struct bk_ble_write_ind
{   
    uint8_t conidx;
    uint16_t prf_id;
    uint16_t att_id;
    uint8_t length;
    uint8_t value[__ARRAY_EMPTY];
};

/// Command complete event data structure
struct bk_ble_gattc_cmp_evt
{
    /// GATT request type
    uint8_t operation;
    /// Status of the request
    uint8_t status;

    uint16_t prf_id;
    /// operation sequence number - provided when operation is started
    uint16_t att_id;
};

enum bk_ble_msg_id
{
	BK_BLE_ENABLE_REQ  =  TASK_FIRST_MSG(TASK_ID_COMMON),
    BK_BLE_ENABLE_RSP,
    BK_BLE_NTF_UPD_REQ,
    BK_BLE_IND_UPD_REQ,
    BK_BLE_WRITE_REQ_IND,
    BK_BLE_GATTC_CMP_EVT,
};

extern const struct kernel_state_handler bk_ble_default_handler;

#endif

#endif
