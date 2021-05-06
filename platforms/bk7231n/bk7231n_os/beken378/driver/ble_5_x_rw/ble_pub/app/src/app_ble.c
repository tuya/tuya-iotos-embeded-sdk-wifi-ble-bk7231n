/**
 ****************************************************************************************
 *
 * @file app.c
 *
 * @brief Application entry point
 *
 * Copyright (C) RivieraWaves 2009-2015
 *
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup APP
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "user_config.h"  

#include "rwip_config.h"             // SW configuration

#if (BLE_APP_PRESENT)
#include "rwapp_config.h"
#include <string.h>

#include "rwip.h"
#include "app_task.h"                // Application task Definition
#include "app_ble.h"                     // Application Definition
#include "gap.h"                     // GAP Definition
#include "gapm_task.h"               // GAP Manager Task API
#include "gapc_task.h"               // GAP Controller Task API
#include "gattc_task.h"

#include "common_bt.h"                   // Common BT Definition
#include "common_math.h"                 // Common Maths Definition

#if (NVDS_SUPPORT)
#include "nvds.h"                    // NVDS Definitions
#endif //(NVDS_SUPPORT)
/*
 * DEFINES
 ****************************************************************************************
 */

#define DEVICE_NAME        "BK DEVICE"

#define DEVICE_NAME_SIZE    sizeof(DEVICE_NAME)

/*
 * LOCAL VARIABLES DEFINITIONS
 ****************************************************************************************
 */

/// Application Task Descriptor
extern const struct kernel_task_desc TASK_BLE_DESC_APP;

/// Application Environment Structure
struct app_env_tag app_ble_ctx;

#if CFG_USE_BLE_PS
static uint32_t ble_sleep_enable = 1;
#else
static uint32_t ble_sleep_enable = 0;
#endif

/*
 * LOCAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */
void set_app_ble_adv_actv_idx(unsigned char idx)
{
	app_ble_ctx.adv_actv_idx = idx;
}

int get_app_ble_adv_actv_idx(void)
{
	return app_ble_ctx.adv_actv_idx;
}

void set_app_ble_conidx(unsigned char idx)
{
	app_ble_ctx.conidx = idx;
}

int get_app_ble_conidx(void)
{
	return app_ble_ctx.conidx;
}

void set_app_ble_adv_state(int adv_state)
{
	app_ble_ctx.adv_state = adv_state;
	bk_printf("adv_state:%d\r\n",adv_state);
}

int get_app_ble_adv_state(void)
{
	return app_ble_ctx.adv_state;
}

void set_app_ble_adv_op(int adv_op)
{
	app_ble_ctx.adv_op = adv_op;
}

int get_app_ble_adv_op(void)
{
	return app_ble_ctx.adv_op;
}

void set_app_ble_scan_actv_idx(unsigned char idx)
{
	app_ble_ctx.scan_actv_idx = idx;
}

int get_app_ble_scan_actv_idx(void)
{
	return app_ble_ctx.scan_actv_idx;
}

void set_app_ble_scan_state(int scan_state)
{
	app_ble_ctx.scan_state = scan_state;
}

int get_app_ble_scan_state(void)
{
	return app_ble_ctx.scan_state;
}

void ble_appm_create_advertising(unsigned char chnl_map,uint32_t intv_min,uint32_t intv_max)
{
	bk_printf("[%s]\r\n",__func__);
    // Prepare the GAPM_ACTIVITY_CREATE_CMD message
    struct gapm_activity_create_adv_cmd *p_cmd = KERNEL_MSG_ALLOC(GAPM_ACTIVITY_CREATE_CMD,
                                                              TASK_BLE_GAPM, TASK_BLE_APP,
                                                              gapm_activity_create_adv_cmd);

    // Set operation code
    p_cmd->operation = GAPM_CREATE_ADV_ACTIVITY;

    // Fill the allocated kernel message
    p_cmd->own_addr_type = GAPM_STATIC_ADDR;
    p_cmd->adv_param.type = GAPM_ADV_TYPE_LEGACY;//GAPM_ADV_TYPE_EXTENDED;//;
    p_cmd->adv_param.prop = GAPM_ADV_PROP_UNDIR_CONN_MASK ;//| GAPM_ADV_PROP_SCAN_REQ_NTF_EN_BIT;//GAPM_ADV_PROP_BROADCAST_NON_SCAN_MASK;//GAPM_ADV_PROP_UNDIR_CONN_MASK;
    p_cmd->adv_param.filter_pol = ADV_ALLOW_SCAN_ANY_CON_ANY;
    p_cmd->adv_param.prim_cfg.chnl_map = chnl_map;
    p_cmd->adv_param.prim_cfg.phy = GAP_PHY_LE_1MBPS;

    p_cmd->adv_param.disc_mode = GAPM_ADV_MODE_GEN_DISC;
    p_cmd->adv_param.prim_cfg.adv_intv_min = intv_min;
    p_cmd->adv_param.prim_cfg.adv_intv_max = intv_max;
	set_app_ble_adv_op(GAPM_CREATE_ADV_ACTIVITY);

    // Send the message
    kernel_msg_send(p_cmd);
}

void ble_appm_start_advertising(unsigned char adv_actv_idx,uint16 duration)
{
    bk_printf("[%s]\r\n",__func__);
    // Prepare the GAPM_ACTIVITY_START_CMD message
    struct gapm_activity_start_cmd *p_cmd = KERNEL_MSG_ALLOC(GAPM_ACTIVITY_START_CMD,
                                                         TASK_BLE_GAPM, TASK_BLE_APP,
                                                         gapm_activity_start_cmd);

    p_cmd->operation = GAPM_START_ACTIVITY;
    p_cmd->actv_idx = adv_actv_idx;
    p_cmd->u_param.adv_add_param.duration = duration;
    p_cmd->u_param.adv_add_param.max_adv_evt = 0;
	
	// And the next expected operation code for the command completed event
	set_app_ble_adv_op(GAPM_START_ACTIVITY);

    // Send the message
    kernel_msg_send(p_cmd);
}

void ble_appm_stop_advertising(unsigned char adv_actv_idx)
{
    bk_printf("[%s]\r\n",__func__);
    // Prepare the GAPM_ACTIVITY_STOP_CMD message
    struct gapm_activity_stop_cmd *cmd = KERNEL_MSG_ALLOC(GAPM_ACTIVITY_STOP_CMD,
                                                      TASK_BLE_GAPM, TASK_BLE_APP,
                                                      gapm_activity_stop_cmd);

    // Fill the allocated kernel message
    cmd->operation = GAPM_STOP_ACTIVITY;
    cmd->actv_idx = adv_actv_idx;
	
	// And the next expected operation code for the command completed event
	set_app_ble_adv_op(GAPM_STOP_ACTIVITY);

    // Send the message
    kernel_msg_send(cmd);
}

void ble_appm_delete_advertising(unsigned char adv_actv_idx)
{
    // Prepare the GAPM_ACTIVITY_CREATE_CMD message
    struct gapm_activity_delete_cmd *p_cmd = KERNEL_MSG_ALLOC(GAPM_ACTIVITY_DELETE_CMD,
                                                              TASK_BLE_GAPM, TASK_BLE_APP,
                                                              gapm_activity_delete_cmd);

    // Set operation code
    p_cmd->operation = GAPM_DELETE_ACTIVITY;
    p_cmd->actv_idx = adv_actv_idx;

	// Keep the current operation
    // And the next expected operation code for the command completed event
	set_app_ble_adv_op(GAPM_DELETE_ACTIVITY);
	
    // Send the message
    kernel_msg_send(p_cmd);
}

int ble_appm_set_adv_data(unsigned char adv_actv_idx,unsigned char* adv_buff, unsigned char adv_len)
{
	// Remaining Length
	uint8_t rem_len = ADV_DATA_LEN - 3;// GAP will use 3 bytes for the AD Type
	
	if (/*kernel_state_get(TASK_BLE_APP) == APPM_ADVERTISING
		   &&*/ (adv_len > ADV_DATA_LEN) && (rem_len < adv_len))	
	{
		return -1;
	}

    // Prepare the GAPM_SET_ADV_DATA_CMD message
    struct gapm_set_adv_data_cmd *p_cmd = KERNEL_MSG_ALLOC_DYN(GAPM_SET_ADV_DATA_CMD,
                                                           TASK_BLE_GAPM, TASK_BLE_APP,
                                                           gapm_set_adv_data_cmd,
                                                           ADV_DATA_LEN);

    // Fill the allocated kernel message
    p_cmd->operation = GAPM_SET_ADV_DATA;
    p_cmd->actv_idx = adv_actv_idx;

    p_cmd->length = 0;
    ///if(rem_len >= adv_len)
    {
        // Copy adv data
        memcpy(&p_cmd->data[0], adv_buff, adv_len);
        // Update advertising data length
        p_cmd->length = adv_len;
    }
    
    bk_printf("[%s]set data",__FUNCTION__);
    for(int i = 0;i < p_cmd->length;i++)
    {
        bk_printf(":%02x",p_cmd->data[i]);
    }bk_printf("\r\n");
	
	// And the next expected operation code for the command completed event
	set_app_ble_adv_op(GAPM_SET_ADV_DATA);
    // Send the message
    kernel_msg_send(p_cmd);

    return 0;
}

int ble_appm_set_scan_rsp_data(unsigned char adv_actv_idx,unsigned char* scan_buff, unsigned char scan_len)
{
    bk_printf("[%s]\r\n",__func__);
	
	if(!(scan_len <= ADV_DATA_LEN)/* || (!(kernel_state_get(TASK_BLE_APP) == APPM_ADVERTISING
        ))*/)
	{
		return -1;
	}

    // Prepare the GAPM_SET_ADV_DATA_CMD message
    struct gapm_set_adv_data_cmd *p_cmd = KERNEL_MSG_ALLOC_DYN(GAPM_SET_ADV_DATA_CMD,
                                                           TASK_BLE_GAPM, TASK_BLE_APP,
                                                           gapm_set_adv_data_cmd,
                                                           ADV_DATA_LEN);

    // Fill the allocated kernel message
    p_cmd->operation = GAPM_SET_SCAN_RSP_DATA;
    p_cmd->actv_idx = adv_actv_idx;

    p_cmd->length = scan_len;
    memcpy(&p_cmd->data[0], scan_buff, scan_len);
	
	// And the next expected operation code for the command completed event
	set_app_ble_adv_op(GAPM_SET_SCAN_RSP_DATA);

    // Send the message
    kernel_msg_send(p_cmd);

    return 0;
}

int ble_appm_update_adv_data(unsigned char adv_actv_idx,unsigned char* adv_buff, unsigned char adv_len)
{
	// Remaining Length
	uint8_t rem_len = ADV_DATA_LEN - 3;// GAP will use 3 bytes for the AD Type
	
    if (/*kernel_state_get(TASK_BLE_APP) == APPM_ADVERTISING
        &&*/ (adv_len <= ADV_DATA_LEN) && (rem_len >= adv_len))  
    {
    	bk_printf("!!!!!!!!!!ble_appm_update_adv_data updata adv data\r\n");
		// Prepare the GAPM_SET_ADV_DATA_CMD message
        struct gapm_set_adv_data_cmd *p_cmd = KERNEL_MSG_ALLOC_DYN(GAPM_SET_ADV_DATA_CMD,
                                                               TASK_BLE_GAPM, TASK_BLE_APP,
                                                               gapm_set_adv_data_cmd,
                                                               ADV_DATA_LEN);

        // Fill the allocated kernel message
        p_cmd->operation = GAPM_SET_ADV_DATA;
        p_cmd->actv_idx = adv_actv_idx;

        p_cmd->length = 0;

        // Check if additional data can be added to the Advertising data - 2 bytes needed for type and length
        ///if(rem_len >= adv_len)
        {
            // Copy adv data
            memcpy(&p_cmd->data[0], adv_buff, adv_len);
            // Update advertising data length
            p_cmd->length = adv_len;
        }

		set_app_ble_adv_op(GAPM_SET_ADV_DATA);

        // Send the message
        kernel_msg_send(p_cmd);

        return 0;
    }
	return -1;
}

int ble_appm_updata_scan_rsp_data(unsigned char adv_actv_idx,unsigned char* scan_buff, unsigned char scan_len)
{
	return ble_appm_set_scan_rsp_data(adv_actv_idx,scan_buff,scan_len);
}


static int ble_app_gapm_adv_is_advtype_unique(uint8_t type)
{
    // Advertising type check which shall be unique
    switch (type)
    {
        case GAP_AD_TYPE_MORE_16_BIT_UUID:
        case GAP_AD_TYPE_COMPLETE_LIST_16_BIT_UUID:
        case GAP_AD_TYPE_MORE_32_BIT_UUID:
        case GAP_AD_TYPE_COMPLETE_LIST_32_BIT_UUID:
        case GAP_AD_TYPE_MORE_128_BIT_UUID:
        case GAP_AD_TYPE_COMPLETE_LIST_128_BIT_UUID:
        case GAP_AD_TYPE_SHORTENED_NAME:
        case GAP_AD_TYPE_COMPLETE_NAME:
        case GAP_AD_TYPE_APPEARANCE:
        case GAP_AD_TYPE_ADV_INTV:
        case GAP_AD_TYPE_PUB_TGT_ADDR:
        case GAP_AD_TYPE_RAND_TGT_ADDR:
        case GAP_AD_TYPE_LE_BT_ADDR:
        case GAP_AD_TYPE_LE_ROLE:
        case GAP_AD_TYPE_FLAGS:
        {
            return 1;
        }

        default:
        {
            return 0;
        }
    }
}

int ble_app_gapm_adv_check_data_sanity(uint16_t length, uint8_t *p_data)
{
    // Returned status
    int status = 1;
    // Cursor
    uint8_t *p_cursor = p_data;
    // End of data
    uint8_t *p_end_cursor = p_data + length;
    // Check for duplicate information in advertising or scan response data.
    uint8_t dup_filter[GAP_AD_TYPE_BITFIELD_BYTES];

    // Clear presence status of unique advertising type
    memset(&dup_filter[0], 0, GAP_AD_TYPE_BITFIELD_BYTES);

    // AD type flags must not be set by application
    GAP_AD_TYPE_SET_BIT(dup_filter, GAP_AD_TYPE_FLAGS);

    while (p_cursor < p_end_cursor)
    {
        // Extract AD type
        uint8_t ad_type = *(p_cursor + 1);

        // Check if it's AD Type which shall be unique
        if (ble_app_gapm_adv_is_advtype_unique(ad_type))
        {
            if (!GAP_AD_TYPE_CHECK_BIT(dup_filter, ad_type))
            {
                // Mark the advertising type as found
                GAP_AD_TYPE_SET_BIT(dup_filter, ad_type);
            }
            else
            {
                // Advertising type has been found twice
                break;
            }
        }

        /* Go to next advertising info */
        p_cursor += (*p_cursor + 1);
    }

    // Check if total advertising length is valid with advertising data info
    if (p_cursor != p_end_cursor)
    {
        status = 0;
    }

    return (status);
}

void ble_appm_update_param(unsigned char conidx,struct gapc_conn_param *conn_param)
{
	// Prepare the GAPC_PARAM_UPDATE_CMD message
	struct gapc_param_update_cmd *cmd = KERNEL_MSG_ALLOC(GAPC_PARAM_UPDATE_CMD,
													 KERNEL_BUILD_ID(TASK_BLE_GAPC, conidx), TASK_BLE_APP,
													 gapc_param_update_cmd);

	cmd->operation	= GAPC_UPDATE_PARAMS;
	cmd->intv_min	= conn_param->intv_min;
	cmd->intv_max	= conn_param->intv_max;
	cmd->latency	= conn_param->latency;
	cmd->time_out	= conn_param->time_out;

	// not used by a slave device
	cmd->ce_len_min = 0x10;  ///0xFFFF
	cmd->ce_len_max = 0x20;  ///0xFFFF

	// Send the message
	kernel_msg_send(cmd);
}

void ble_appm_disconnect(uint8_t conidx,uint8_t reason)
{
    struct gapc_disconnect_cmd *cmd = KERNEL_MSG_ALLOC(GAPC_DISCONNECT_CMD,
                                                   KERNEL_BUILD_ID(TASK_BLE_GAPC, conidx), 
                                                   KERNEL_BUILD_ID(TASK_BLE_APP, conidx),
                                                   gapc_disconnect_cmd);

    cmd->operation = GAPC_DISCONNECT;
    cmd->reason    = reason;

    // Send the message
    kernel_msg_send(cmd);
}

///////////////////scan
void ble_appm_create_scaning(void)
{
    // Prepare the GAPM_ACTIVITY_CREATE_CMD message
    struct gapm_activity_create_cmd *p_cmd = KERNEL_MSG_ALLOC(GAPM_ACTIVITY_CREATE_CMD,
                                                              TASK_BLE_GAPM, TASK_BLE_APP,
                                                              gapm_activity_create_cmd);

    // Set operation code
    p_cmd->operation = GAPM_CREATE_SCAN_ACTIVITY;

    // Fill the allocated kernel message
    p_cmd->own_addr_type = GAPM_STATIC_ADDR;
     
    // Send the message
    kernel_msg_send(p_cmd);
}

void ble_appm_start_scaning(void)
{
    if (app_ble_ctx.scan_state == APP_SCAN_STATE_CREATED)
    {
        // Prepare the GAPM_ACTIVITY_START_CMD message
        struct gapm_activity_start_cmd *p_cmd = KERNEL_MSG_ALLOC(GAPM_ACTIVITY_START_CMD,
                                                             TASK_BLE_GAPM, TASK_BLE_APP,
                                                             gapm_activity_start_cmd);

        p_cmd->operation = GAPM_START_ACTIVITY;
        p_cmd->actv_idx = app_ble_ctx.scan_actv_idx;
        
        p_cmd->u_param.scan_param.type = GAPM_SCAN_TYPE_OBSERVER;//GAPM_SCAN_TYPE_GEN_DISC;//GAPM_SCAN_TYPE_OBSERVER;//;
        
        p_cmd->u_param.scan_param.prop = GAPM_SCAN_PROP_PHY_1M_BIT ;//| GAPM_SCAN_PROP_ACTIVE_1M_BIT;
        
        //p_cmd->u_param.scan_param.scan_param_1m.scan_intv = app_env.scan_intv;
        //p_cmd->u_param.scan_param.scan_param_1m.scan_wd = app_env.scan_wd;
        
        p_cmd->u_param.scan_param.scan_param_1m.scan_intv = 20;//app_env.scan_intv;
        p_cmd->u_param.scan_param.scan_param_1m.scan_wd = 10;//app_env.scan_wd;
        
        p_cmd->u_param.scan_param.dup_filt_pol = 0;
        
        p_cmd->u_param.scan_param.duration = 0;
        
        p_cmd->u_param.scan_param.period = 10;

		// Keep the current operation
        app_ble_ctx.scan_state = APP_SCAN_STATE_STARTING;
        // And the next expected operation code for the command completed event
        app_ble_ctx.scan_op = GAPM_START_ACTIVITY;

        // Send the message
        kernel_msg_send(p_cmd);
    }
}

void ble_appm_stop_scaning(void)
{
    // Prepare the GAPM_ACTIVITY_STOP_CMD message
    struct gapm_activity_stop_cmd *cmd = KERNEL_MSG_ALLOC(GAPM_ACTIVITY_STOP_CMD,
                                                      TASK_BLE_GAPM, TASK_BLE_APP,
                                                      gapm_activity_stop_cmd);

    // Fill the allocated kernel message
    cmd->operation = GAPM_STOP_ACTIVITY;
    cmd->actv_idx = app_ble_ctx.scan_actv_idx;

	// Update advertising state
    app_ble_ctx.scan_state = APP_SCAN_STATE_STOPPING;
    // And the next expected operation code for the command completed event
    app_ble_ctx.scan_op = GAPM_STOP_ACTIVITY;

    // Send the message
    kernel_msg_send(cmd);
}

///////////////other
void ble_appm_send_gapm_reset_cmd(void)
{
    // Reset the stack
    struct gapm_reset_cmd *p_cmd = KERNEL_MSG_ALLOC(GAPM_RESET_CMD,
                                                TASK_BLE_GAPM, 
                                                TASK_BLE_APP,
                                                gapm_reset_cmd);

    p_cmd->operation = GAPM_RESET;

    kernel_msg_send(p_cmd);
    
    bk_printf("[%s]\r\n",__FUNCTION__);
}

void ble_appc_gatt_mtu_change(unsigned char conidx)
{
    // Prepare the GAPC_PARAM_UPDATE_CMD message
    struct gattc_exc_mtu_cmd  *cmd = KERNEL_MSG_ALLOC(GATTC_EXC_MTU_CMD,
                                                 KERNEL_BUILD_ID(TASK_BLE_GATTC, conidx), 
                                                 KERNEL_BUILD_ID(TASK_BLE_APP, conidx),
                                                 gattc_exc_mtu_cmd);

    cmd->operation  = GATTC_MTU_EXCH;

    // Send the message
    kernel_msg_send(cmd);
}

void  __attribute__((weak)) app_ble_task_init(void)
{
	
}

void appm_init( void )
{
    // Reset the application manager environment
    memset(&app_ble_ctx, 0, sizeof(struct app_env_tag));
	
    // Create APP task
    kernel_task_create(TASK_BLE_APP, &TASK_BLE_DESC_APP);

    // Initialize Task state
    kernel_state_set(TASK_BLE_APP, APPM_INIT);

    /*------------------------------------------------------
     * INITIALIZE ALL MODULES
     *------------------------------------------------------*/
	app_ble_task_init();

    // Reset the stack
    ble_appm_send_gapm_reset_cmd();
}

void ble_ps_enable_set(void)
{
    GLOBAL_INT_DECLARATION();
    GLOBAL_INT_DISABLE();
    ble_sleep_enable = 1;
    GLOBAL_INT_RESTORE();
}

void ble_ps_enable_clear(void)
{
    GLOBAL_INT_DECLARATION();
    GLOBAL_INT_DISABLE();
    ble_sleep_enable = 0;
    GLOBAL_INT_RESTORE();
}

UINT32 ble_ps_enabled(void )
{
    uint32_t value = 0;
    GLOBAL_INT_DECLARATION();
    GLOBAL_INT_DISABLE();
    value =  ble_sleep_enable;
    GLOBAL_INT_RESTORE();
    return value;
}

#endif //(BLE_APP_PRESENT)

/// @} APP
