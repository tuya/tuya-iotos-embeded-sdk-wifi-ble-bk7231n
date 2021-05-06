/**
 ****************************************************************************************
 *
 * @file appm_task.c
 *
 * @brief RW APP Task implementation
 *
 * Copyright (C) RivieraWaves 2009-2015
 *
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup APPTASK
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"          // SW configuration

#if (BLE_APP_PRESENT)

#include "rwapp_config.h"
#include "app_task.h"             // Application Manager Task API
#include "app_ble.h"                  // Application Manager Definition
#include "gapc_task.h"            // GAP Controller Task API
#include "gapm_task.h"            // GAP Manager Task API
#include "architect.h"                 // Platform Definitions
#include <string.h>
#include "common_utils.h"
#include "kernel_timer.h"             // Kernel timer
#include "ble_api.h"

#if (BLE_APP_COMM)
#include "app_comm.h"
#endif

#ifdef __func__
#undef __func__
#endif
#define __func__   __FUNCTION__

/*
 * LOCAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

static uint8_t app_get_handler(const struct app_subtask_handlers *handler_list_desc,
                               kernel_msg_id_t msgid,
                               void *param,
                               kernel_task_id_t src_id)
{
    // Counter
    uint8_t counter;

    // Get the message handler function by parsing the message table
    for (counter = handler_list_desc->msg_cnt; 0 < counter; counter--)
    {
        struct kernel_msg_handler handler
                = (struct kernel_msg_handler)(*(handler_list_desc->p_msg_handler_tab + counter - 1));

        if ((handler.id == msgid) ||
            (handler.id == KERNEL_MSG_DEFAULT_HANDLER))
        {
            // If handler is NULL, message should not have been received in this state
            BLE_ASSERT_ERR(handler.func);

            return (uint8_t)(handler.func(msgid, param, TASK_BLE_APP, src_id));
        }
    }

    // If we are here no handler has been found, drop the message
    return (KERNEL_MSG_CONSUMED);
}

/*
 * MESSAGE HANDLERS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Handles GAPM_ACTIVITY_CREATED_IND event
 *
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance.
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gapm_activity_created_ind_handler(kernel_msg_id_t const msgid,
                                             struct gapm_activity_created_ind const *p_param,
                                             kernel_task_id_t const dest_id,
                                             kernel_task_id_t const src_id)
{
	struct ble_activity_created_ind ind;
	
	ind.actv_idx = p_param->actv_idx;
	switch(p_param->actv_type){
		case GAPM_ACTV_TYPE_ADV:
			ind.actv_type = BLE_ACTV_TYPE_ADV;
			break;
		case GAPM_ACTV_TYPE_SCAN:
			ind.actv_type = BLE_ACTV_TYPE_SCAN;
			break;
		case GAPM_ACTV_TYPE_INIT:
			ind.actv_type = BLE_ACTV_TYPE_INIT;
			break;
		case GAPM_ACTV_TYPE_PER_SYNC:
			ind.actv_type = BLE_ACTV_TYPE_PER_SYNC;
			break;
		default:
			ind.actv_type = p_param->actv_type;
			break;
	}
	ind.tx_pwr = p_param->tx_pwr;
	
	ble_event_cb_handler(BLE_ACTIVITY_CREATED_IND,&ind);

    return (KERNEL_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles GAPM_ACTIVITY_STOPPED_IND event.
 *
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance.
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gapm_activity_stopped_ind_handler(kernel_msg_id_t const msgid,
                                             struct gapm_activity_stopped_ind const *p_param,
                                             kernel_task_id_t const dest_id,
                                             kernel_task_id_t const src_id)
{   
    ble_event_cb_handler(BLE_ACTIVITY_STOPPED_IND,(void*)p_param);

    return (KERNEL_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles GAPM_PROFILE_ADDED_IND event
 *
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance.
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gapm_profile_added_ind_handler(kernel_msg_id_t const msgid,
                                          struct gapm_profile_added_ind *param,
                                          kernel_task_id_t const dest_id,
                                          kernel_task_id_t const src_id)
{ 
    // Current State
    kernel_state_t state = kernel_state_get(dest_id);
    uint8_t conidx = KERNEL_IDX_GET(dest_id);
	uint16_t id = param->prf_task_id; 

#if (BLE_SDP_CLIENT)
    if((id >= TASK_BLE_ID_SDP) && (id <= TASK_BLE_ID_SDP + BLE_NB_PROFILES) )
    {
        id = TASK_BLE_ID_SDP;
    }
#endif

#if (BLE_COMM_SERVER)
    if((id >= TASK_BLE_ID_COMMON) && (id <= TASK_BLE_ID_COMMON + BLE_NB_PROFILES))
    {
        id = TASK_BLE_ID_COMMON;
    }
#endif
	
	switch (id)
	{
#if (BLE_COMM_SERVER)
		case TASK_BLE_ID_COMMON:
		{
			kernel_state_set(TASK_BLE_APP, APPM_READY);
			
			if(param->status == GAP_ERR_NO_ERROR)
			{
				ble_event_cb_handler(BLE_CREATE_DB_OK, NULL);
			}
			else
			{
				ble_event_cb_handler(BLE_CREATE_DB_FAIL, NULL);
			}
		}
		break;
#endif

#if (BLE_SDP_CLIENT)
		case TASK_BLE_ID_SDP:
		{
			kernel_state_set(dest_id, APPM_CONNECTED);
			uint8_t find;
			find = sdp_enable_all_server_ntf_ind(get_app_ble_conidx(),1);
			bk_printf("find = %d\r\n",find);
		}
		break;
#endif
#if BLE_MESH
		case TASK_BLE_ID_MESH:
		{
			kernel_state_set(TASK_BLE_BLE_APP, APPM_READY);

			if(param->status == GAP_ERR_NO_ERROR)
			{
				if(ble_mesh_event_cb)
				{
					ble_mesh_event_cb(BLE_MESH_CREATE_DB_OK, NULL);
				}
			}
			else
			{
				if(ble_mesh_event_cb)
				{
					ble_mesh_event_cb(BLE_MESH_CREATE_DB_FAIL, NULL);
				}
			}
		}
		break;
#endif
		default:
		{

		}
		break;
	}

    return KERNEL_MSG_CONSUMED;
}

/**
 ****************************************************************************************
 * @brief Handles GAP manager command complete events.
 *
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance (TASK_BLE_GAP).
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gapm_cmp_evt_handler(kernel_msg_id_t const msgid,
                                struct gapm_cmp_evt const *param,
                                kernel_task_id_t const dest_id,
                                kernel_task_id_t const src_id)
{
    #if (NVDS_SUPPORT)
    uint8_t key_len = KEY_LEN;
    #endif //(NVDS_SUPPORT)
    uint8_t conidx = KERNEL_IDX_GET(dest_id);
    bk_printf("[%s] conidx:%d,operation:0x%x,status:0x%x\r\n",__func__,conidx,param->operation,param->status);
    switch(param->operation)
    {
        // Reset completed
        case (GAPM_RESET)://0
        {

            if(param->status == GAP_ERR_NO_ERROR)
            {
                #if (NVDS_SUPPORT)
                nvds_tag_len_t len = 6;
                #endif //(NVDS_SUPPORT)

                // Set Device configuration
                struct gapm_set_dev_config_cmd* cmd = KERNEL_MSG_ALLOC(GAPM_SET_DEV_CONFIG_CMD,
                                                                   TASK_BLE_GAPM, TASK_BLE_APP,
                                                                   gapm_set_dev_config_cmd);
                // Set the operation
                cmd->operation = GAPM_SET_DEV_CONFIG;
                // Set the device role - Peripheral / central
                cmd->role      = GAP_ROLE_ALL;//GAP_ROLE_PERIPHERAL;
                
                #if (BLE_APP_SEC_CON)
                // The Max MTU is increased to support the Public Key exchange
                // HOWEVER, with secure connections enabled you cannot sniff the 
                // LEAP and LEAS protocols
                cmd->max_mtu = 160;
                cmd->pairing_mode = GAPM_PAIRING_SEC_CON | GAPM_PAIRING_LEGACY;
                #else // !(BLE_APP_SEC_CON)
                // Do not support secure connections
                cmd->pairing_mode = GAPM_PAIRING_LEGACY;
                #endif //(BLE_APP_SEC_CON)
                
                // Set Data length parameters
                cmd->sugg_max_tx_octets = LE_MAX_OCTETS;//LE_MIN_OCTETS;//LE_MAX_OCTETS;
                cmd->sugg_max_tx_time   = LE_MAX_TIME;//LE_MIN_TIME;//LE_MAX_TIME;
                
                cmd->max_mtu = 527;//ATT_DEFAULT_MTU;
               
                #if (BLE_APP_HID)
                // Enable Slave Preferred Connection Parameters present 
                cmd->att_cfg = GAPM_MASK_ATT_SLV_PREF_CON_PAR_EN;
                #endif //(BLE_APP_HID)

                // Host privacy enabled by default
                cmd->privacy_cfg = 0;
                

                #if (NVDS_SUPPORT)
                if (rwip_param.get(PARAM_ID_BD_ADDRESS, &len, &cmd->addr.addr[0]) == PARAM_OK)
                {
                    // Check if address is a static random address
                    if (cmd->addr.addr[5] & 0xC0)
                    {
                        // Host privacy enabled by default
                        cmd->privacy_cfg |= GAPM_PRIV_CFG_PRIV_ADDR_BIT;
                    }
                }else
                {
                    memcpy(&cmd->addr.addr[0],&co_default_bdaddr.addr[0],BD_ADDR_LEN);
                    if (cmd->addr.addr[5] & 0xC0)
                    {
                        // Host privacy enabled by default
                        cmd->privacy_cfg |= GAPM_PRIV_CFG_PRIV_ADDR_BIT;
                    }
                }
                #endif //(NVDS_SUPPORT)
                bk_printf("cmd->addr.addr[5] :%x\r\n",cmd->addr.addr[5]);
               
                #if (NVDS_SUPPORT)
                if ((app_sec_get_bond_status()==true) &&
                    (nvds_get(NVDS_TAG_LOC_IRK, &key_len, app_ble_ctx.loc_irk) == NVDS_OK))
                {
                    memcpy(cmd->irk.key, app_ble_ctx.loc_irk, 16);
                }
                else
                #endif //(NVDS_SUPPORT)
                {
                    memset((void *)&cmd->irk.key[0], 0x00, KEY_LEN);
                }
                // Send message
                kernel_msg_send(cmd);
            }
            else
            {
                BLE_ASSERT_ERR(0);
            }
        }
        break;

        case (GAPM_GEN_RAND_NB) ://0x1a
        {
        	bk_printf("gapm_cmp_evt:GAPM_GEN_RAND_NB\r\n");
            if (app_ble_ctx.rand_cnt == 1)
            {
                // Generate a second random number
                app_ble_ctx.rand_cnt++;
                struct gapm_gen_rand_nb_cmd *cmd = KERNEL_MSG_ALLOC(GAPM_GEN_RAND_NB_CMD,
                                                                TASK_BLE_GAPM, TASK_BLE_APP,
                                                                gapm_gen_rand_nb_cmd);
                cmd->operation = GAPM_GEN_RAND_NB;
                kernel_msg_send(cmd);
            }
            else
            {
                struct gapm_set_irk_cmd *cmd = KERNEL_MSG_ALLOC(GAPM_SET_IRK_CMD,
                                                        TASK_BLE_GAPM, TASK_BLE_APP,
                                                        gapm_set_irk_cmd);
                app_ble_ctx.rand_cnt=0;
                ///  - GAPM_SET_IRK
                cmd->operation = GAPM_SET_IRK;
                memcpy(&cmd->irk.key[0], &app_ble_ctx.loc_irk[0], KEY_LEN);
                kernel_msg_send(cmd);
            }
        }
        break;

        case (GAPM_SET_IRK):
        {
            // BLE_ASSERT_INFO(param->status == GAP_ERR_NO_ERROR, param->operation, param->status);

            #if (BLE_APP_SEC)
            // If not Bonded already store the generated value in NVDS
            if (app_sec_get_bond_status()==false)
            {
                #if (NVDS_SUPPORT)
                if (nvds_put(NVDS_TAG_LOC_IRK, KEY_LEN, (uint8_t *)&app_ble_ctx.loc_irk) != NVDS_OK)
                #endif //(NVDS_SUPPORT)
                {
                    BLE_ASSERT_INFO(0, 0, 0);
                }
            }
            #endif //(BLE_APP_SEC)
            app_ble_ctx.rand_cnt = 0;
             
            // Go to the create db state
            kernel_state_set(TASK_BLE_APP, APPM_READY);
			bk_printf("gapm_cmp_evt:BLE_STACK_OK\r\n");
			ble_event_cb_handler(BLE_STACK_OK, NULL);
        }
        break;

        // Device Configuration updated
        case (GAPM_SET_DEV_CONFIG):
        {
            BLE_ASSERT_INFO(param->status == GAP_ERR_NO_ERROR, param->operation, param->status);
			bk_printf("gapm_cmp_evt:GAPM_SET_DEV_CONFIG\r\n");

            #if (BLE_APP_SEC)
            if (app_sec_get_bond_status()==true) 
            {
                #if (NVDS_SUPPORT)
                // If Bonded retrieve the local IRK from NVDS
                if (nvds_get(NVDS_TAG_LOC_IRK, &key_len, app_ble_ctx.loc_irk) == NVDS_OK)
                {
                    // Set the IRK in the GAP
                    struct gapm_set_irk_cmd *cmd = KERNEL_MSG_ALLOC(GAPM_SET_IRK_CMD,
                                                                TASK_BLE_GAPM, TASK_BLE_APP,
                                                                gapm_set_irk_cmd);
                    ///  - GAPM_SET_IRK: 
                    cmd->operation = GAPM_SET_IRK;
                    memcpy(&cmd->irk.key[0], &app_ble_ctx.loc_irk[0], KEY_LEN);
                    kernel_msg_send(cmd);
					bk_printf("gapm_cmp_evt:wait GAPM_SET_IRK\r\n");
                }
                else
                #endif //(NVDS_SUPPORT)
               
                {
                     BLE_ASSERT_ERR(0);
                }
            }
            else // Need to start the generation of new IRK
            #endif //(BLE_APP_SEC)
            {
                struct gapm_gen_rand_nb_cmd *cmd = KERNEL_MSG_ALLOC(GAPM_GEN_RAND_NB_CMD,
                                                                TASK_BLE_GAPM, TASK_BLE_APP,
                                                                gapm_gen_rand_nb_cmd);
                cmd->operation   = GAPM_GEN_RAND_NB;
                app_ble_ctx.rand_cnt = 1;
                kernel_msg_send(cmd);
				bk_printf("gapm_cmp_evt:wait GAPM_GEN_RAND_NB\r\n");
            }
           
        }
        break;
    
#if (BLE_CENTRAL )
		case (GAPM_CREATE_INIT_ACTIVITY)://0xA2
#endif
#if (BLE_OBSERVER || BLE_CENTRAL)
		case (GAPM_CREATE_SCAN_ACTIVITY)://0xA1 
#endif
        case (GAPM_CREATE_ADV_ACTIVITY)://0xA0              
        case (GAPM_SET_ADV_DATA)://0xA9
        case (GAPM_SET_SCAN_RSP_DATA)://0xAA
        case (GAPM_START_ACTIVITY):
		case (GAPM_STOP_ACTIVITY)://0xA5
		case (GAPM_DELETE_ACTIVITY)://0xA7
		case (GAPM_DELETE_ALL_ACTIVITIES) :
        {
 			ble_event_cb_handler(BLE_COMMON_EVT,(void*)param);
        }
		break;
		
        default:
        {
            // Drop the message
        }
        break;
    }

    return (KERNEL_MSG_CONSUMED);
}

static int gapc_get_dev_info_req_ind_handler(kernel_msg_id_t const msgid,
        struct gapc_get_dev_info_req_ind const *param,
        kernel_task_id_t const dest_id,
        kernel_task_id_t const src_id)
{
    switch(param->req)
    {
        case GAPC_DEV_NAME:
        {
            struct gapc_get_dev_info_cfm * cfm = KERNEL_MSG_ALLOC_DYN(GAPC_GET_DEV_INFO_CFM,
                                                    src_id, dest_id,
                                                    gapc_get_dev_info_cfm, APP_DEVICE_NAME_MAX_LEN);
            cfm->req = param->req;
            cfm->info.name.length = ble_appm_get_dev_name(cfm->info.name.value);

            // Send message
            kernel_msg_send(cfm);
        } break;

        case GAPC_DEV_APPEARANCE:
        {
            // Allocate message
            struct gapc_get_dev_info_cfm *cfm = KERNEL_MSG_ALLOC(GAPC_GET_DEV_INFO_CFM,
                                                             src_id, dest_id,
                                                             gapc_get_dev_info_cfm);
            cfm->req = param->req;
            // Set the device appearance
            #if (BLE_APP_HT)
            // Generic Thermometer - TODO: Use a flag
            cfm->info.appearance = 728;
            #elif (BLE_APP_HID)
            // HID Mouse
            cfm->info.appearance = 962;
            #else
            // No appearance
            cfm->info.appearance = 0;
            #endif

            // Send message
            kernel_msg_send(cfm);
        } break;

        case GAPC_DEV_SLV_PREF_PARAMS:
        {
            // Allocate message
            struct gapc_get_dev_info_cfm *cfm = KERNEL_MSG_ALLOC(GAPC_GET_DEV_INFO_CFM,
                    src_id, dest_id,
                                                            gapc_get_dev_info_cfm);
            cfm->req = param->req;
            // Slave preferred Connection interval Min
            cfm->info.slv_pref_params.con_intv_min = 8;
            // Slave preferred Connection interval Max
            cfm->info.slv_pref_params.con_intv_max = 10;
            // Slave preferred Connection latency
            cfm->info.slv_pref_params.slave_latency  = 0;
            // Slave preferred Link supervision timeout
            cfm->info.slv_pref_params.conn_timeout    = 200;  // 2s (500*10ms)

            // Send message
            kernel_msg_send(cfm);
        } break;

        default: /* Do Nothing */ break;
    }


    return (KERNEL_MSG_CONSUMED);
}
/**
 ****************************************************************************************
 * @brief Handles GAPC_SET_DEV_INFO_REQ_IND message.
 *
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance (TASK_BLE_GAP).
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gapc_set_dev_info_req_ind_handler(kernel_msg_id_t const msgid,
        struct gapc_set_dev_info_req_ind const *param,
        kernel_task_id_t const dest_id,
        kernel_task_id_t const src_id)
{
    // Set Device configuration
    struct gapc_set_dev_info_cfm* cfm = KERNEL_MSG_ALLOC(GAPC_SET_DEV_INFO_CFM, src_id, dest_id,
                                                     gapc_set_dev_info_cfm);
    // Reject to change parameters
    cfm->status = GAP_ERR_REJECTED;
    cfm->req = param->req;
    // Send message
    kernel_msg_send(cfm);

    return (KERNEL_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles connection complete event from the GAP. Enable all required profiles
 *
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance (TASK_BLE_GAP).
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gapc_connection_req_ind_handler(kernel_msg_id_t const msgid,
                                           struct gapc_connection_req_ind const *param,
                                           kernel_task_id_t const dest_id,
                                           kernel_task_id_t const src_id)
{
	unsigned char conidx = KERNEL_IDX_GET(src_id);
    app_ble_ctx.conidx = conidx;
	set_app_ble_conidx(conidx);
    
    // Check if the received Connection Handle was valid
    if (get_app_ble_conidx() != GAP_INVALID_CONIDX)
    {
        // Retrieve the connection info from the parameters
        app_ble_ctx.conhdl = param->conhdl;
                
        app_ble_ctx.conn_intv = param->con_interval;
        app_ble_ctx.con_dev_addr[get_app_ble_conidx()].addr_type = param->peer_addr_type;
        memcpy(app_ble_ctx.con_dev_addr[get_app_ble_conidx()].addr.addr,param->peer_addr.addr,6);
        app_ble_ctx.role[get_app_ble_conidx()] = param->role;
        
        // Send connection confirmation
        struct gapc_connection_cfm *cfm = KERNEL_MSG_ALLOC(GAPC_CONNECTION_CFM,
                KERNEL_BUILD_ID(TASK_BLE_GAPC, get_app_ble_conidx()), KERNEL_BUILD_ID(TASK_BLE_APP,get_app_ble_conidx()),
                gapc_connection_cfm);

        #if(BLE_APP_SEC)
        cfm->auth      = app_sec_get_bond_status() ? GAP_AUTH_REQ_NO_MITM_BOND : GAP_AUTH_REQ_NO_MITM_NO_BOND; // TODO [FBE] restore valid data
        #else // !(BLE_APP_SEC)
        cfm->auth      = GAP_AUTH_REQ_NO_MITM_NO_BOND;
        #endif // (BLE_APP_SEC)
        // Send the message
        kernel_msg_send(cfm);
        
        ///if(param->role == 0)
        {
           /// sdp_discover_all_service(get_app_ble_conidx());
        }

        // We are now in connected State
        ///kernel_state_set(KERNEL_BUILD_ID(TASK_BLE_APP,get_app_ble_conidx()), APPM_CONNECTED);
		kernel_state_set(TASK_BLE_APP, APPM_CONNECTED);
		ble_event_cb_handler(BLE_CONNECT, (void *)param);
    }
    else
    {
        // No connection has been established, restart advertising
     //   appm_update_adv_state(true);
    }

    return (KERNEL_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles connection complete event from the GAP. Enable all required profiles
 *
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance (TASK_BLE_GAP).
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gapc_param_update_req_ind_handler(kernel_msg_id_t const msgid,
                                           struct gapc_param_update_req_ind const *param,
                                           kernel_task_id_t const dest_id,
                                           kernel_task_id_t const src_id)
{
    app_ble_ctx.conidx = KERNEL_IDX_GET(src_id);

    // Check if the received Connection Handle was valid
    if (app_ble_ctx.conidx != GAP_INVALID_CONIDX)
    {

        // Send connection confirmation
        struct gapc_param_update_cfm *cfm = KERNEL_MSG_ALLOC(GAPC_PARAM_UPDATE_CFM,
                KERNEL_BUILD_ID(TASK_BLE_GAPC, app_ble_ctx.conidx), TASK_BLE_APP,
                gapc_param_update_cfm);

        cfm->accept = 1;//true;
        cfm->ce_len_min = 10;
        cfm->ce_len_max = 20;

        // Send message
        kernel_msg_send(cfm);

    }
    else
    {
        // No connection has been established, restart advertising
        appm_update_adv_state(true);
    }

    return (KERNEL_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief  GAPC_PARAM_UPDATED_IND
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gapc_param_updated_ind_handler (kernel_msg_id_t const msgid, 
									const struct gapc_param_updated_ind  *param,
                 					kernel_task_id_t const dest_id, kernel_task_id_t const src_id)
{
    uint8_t conidx = KERNEL_IDX_GET(src_id);

    app_ble_ctx.conn_intv = param->con_interval;

	return KERNEL_MSG_CONSUMED;
}

/**
 ****************************************************************************************
 * @brief Handles GAP controller command complete events.
 *
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance (TASK_BLE_GAP).
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gapc_cmp_evt_handler(kernel_msg_id_t const msgid,
                                struct gapc_cmp_evt const *param,
                                kernel_task_id_t const dest_id,
                                kernel_task_id_t const src_id)
{
    uint8_t conidx = KERNEL_IDX_GET(dest_id);
    bk_printf("%s conidx:%d,operation:0x%x,status:%x\r\n",__func__,conidx,param->operation,param->status);
 
    switch(param->operation)
    {
        case (GAPC_UPDATE_PARAMS):
        {
            if (param->status != GAP_ERR_NO_ERROR)
            {
//                appm_disconnect();
            }
        } break;

        default:
        {
        } break;
    }

    return (KERNEL_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles disconnection complete event from the GAP.
 *
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance (TASK_BLE_GAP).
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gapc_disconnect_ind_handler(kernel_msg_id_t const msgid,
                                      struct gapc_disconnect_ind const *param,
                                      kernel_task_id_t const dest_id,
                                      kernel_task_id_t const src_id)
{
    
    uint8_t conidx = KERNEL_IDX_GET(dest_id);
	struct ble_disconnect_ind ind;
    bk_printf("[%s],reason:0x%x\r\n",__func__,param->reason);
    // Go to the ready state
    ////kernel_state_set(KERNEL_BUILD_ID(TASK_BLE_APP, conidx), APPM_INIT);
	ind.conhdl = param->conhdl;
	ind.reason = param->reason;
	kernel_state_set(TASK_BLE_APP, APPM_INIT);
	ble_event_cb_handler(BLE_DISCONNECT, &ind);

    return (KERNEL_MSG_CONSUMED);
	(void)conidx;
}

/**
 ****************************************************************************************
 * @brief Handles reception of all messages sent from the lower layers to the application
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int appm_msg_handler(kernel_msg_id_t const msgid,
                            void *param,
                            kernel_task_id_t const dest_id,
                            kernel_task_id_t const src_id)
{
    // Retrieve identifier of the task from received message
    kernel_task_id_t src_task_id = BLE_MSG_T(msgid);
    // Message policy
    uint8_t msg_pol = KERNEL_MSG_CONSUMED;
    switch (src_task_id)
    {
        case (TASK_BLE_ID_GAPC):
        {
            #if (BLE_APP_SEC)
            if ((msgid >= GAPC_BOND_CMD) &&
                (msgid <= GAPC_SECURITY_IND))
            {
                // Call the Security Module
                msg_pol = app_get_handler(&app_sec_handlers, msgid, param, src_id);
            }
            #endif //(BLE_APP_SEC)
            // else drop the message
        } break;

        case (TASK_BLE_ID_GATTC):
        {
            // Service Changed - Drop
        } break;

#if (BLE_APP_COMM)
        case (TASK_BLE_ID_COMMON):
        {
            // Call the Health Thermometer Module
            msg_pol = app_get_handler(&app_comm_table_handler, msgid, param, src_id);
        } break;
#endif //(BLE_APP_COMM)

        default:
        {
        } break;
    }
 
    return (msg_pol);
}

/**
 ****************************************************************************************
 * @brief Handles reception of random number generated message
 *
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gapm_gen_rand_nb_ind_handler(kernel_msg_id_t const msgid, struct gapm_gen_rand_nb_ind *param,
                                        kernel_task_id_t const dest_id, kernel_task_id_t const src_id)
{
    if (app_ble_ctx.rand_cnt==1)      // First part of IRK
    {
        memcpy(&app_ble_ctx.loc_irk[0], &param->randnb.nb[0], 8);
    }
    else if (app_ble_ctx.rand_cnt==2) // Second part of IRK
    {
        memcpy(&app_ble_ctx.loc_irk[8], &param->randnb.nb[0], 8);
    }

    return KERNEL_MSG_CONSUMED;
}
										
static int gapm_gen_dh_key_ind_handler(kernel_msg_id_t const msgid,
											struct gapm_gen_dh_key_ind const *param,
											kernel_task_id_t const dest_id,
											kernel_task_id_t const src_id)
{
	int msg_status = KERNEL_MSG_CONSUMED;
	struct ble_gen_dh_key_ind ind;

	ind.result =  (uint8_t *)&(param->result[0]);
	ind.len =  GAP_P256_KEY_LEN;

	ble_event_cb_handler(BLE_GEN_DH_KEY, (void *)&ind);

	return (msg_status);
}
											
static int gapm_get_key_ind_handler(kernel_msg_id_t const msgid,
											struct gapm_pub_key_ind const *param,
											kernel_task_id_t const dest_id,
											kernel_task_id_t const src_id)
{
	int msg_status = KERNEL_MSG_CONSUMED;
	struct ble_get_key_ind ind;

	ind.pub_key_x =  (uint8_t *)&(param->pub_key_x[0]);
	ind.pub_x_len =  GAP_P256_KEY_LEN;
	ind.pub_key_y =  (uint8_t *)&(param->pub_key_y[0]);
	ind.pub_y_len =  GAP_P256_KEY_LEN;

	ble_event_cb_handler(BLE_GET_KEY, (void *)&ind);

	return (msg_status);
}

										
#if (BLE_OBSERVER || BLE_CENTRAL )

static int gapm_ext_adv_report_ind_handler(kernel_msg_id_t const msgid, struct gapm_ext_adv_report_ind *param,
                                        kernel_task_id_t const dest_id, kernel_task_id_t const src_id)
{
#if 0
    bk_printf("%s\r\n",__func__);
    bk_printf("actv_idx:%x\r\n",param->actv_idx);
    bk_printf("info:%x\r\n",param->info);
    bk_printf("ch:%d\r\n",param->channel_idx);
    bk_printf("rssi:%d\r\n",param->rssi);
    bk_printf("addr_type:%x\r\n",param->trans_addr.addr_type);
    bk_printf("addr");
    for(int i = 0;i < 6;i++)
    {
        bk_printf(":%02x",param->trans_addr.addr.addr[i]);
    }bk_printf("\r\n");
    
    bk_printf("data len:%d\r\n",param->length);
    bk_printf("data");
    for(int i = 0;i < param->length;i++)
    {
        bk_printf(":%02x",param->data[i]);
    }bk_printf("\r\n");
    
    //appm_adv_data_decode(param->length,param->data,NULL,0);
	bk_printf("!!!![%s] recv adv\r\n",__FUNCTION__);
#endif
	recv_adv_t adv_param;

	adv_param.evt_type = param->info;
	adv_param.data = &(param->data[0]);
	adv_param.data_len = param->length;
	adv_param.rssi = param->rssi;
	adv_param.adv_addr_type = param->trans_addr.addr_type;
	memcpy(adv_param.adv_addr, param->trans_addr.addr.addr, GAP_BD_ADDR_LEN);

    ble_recv_adv_cb_handler(&adv_param);
    //bk_printf("****************************************\r\n");
    
    return KERNEL_MSG_CONSUMED;
}
#endif

/**
 ****************************************************************************************
 * @brief  GATTC_MTU_CHANGED_IND
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int app_gattc_mtu_changed_ind_handler(kernel_msg_id_t const msgid,
                                     struct gattc_mtu_changed_ind const *ind,
                                     kernel_task_id_t const dest_id,
                                     kernel_task_id_t const src_id)
{
    uint8_t conidx = KERNEL_IDX_GET(src_id);
	ble_event_cb_handler(BLE_MTU_CHANGE, (void*)&(ind->mtu));
	
 	return (KERNEL_MSG_CONSUMED);
}

static int gattc_cmp_evt_handler(kernel_msg_id_t const msgid,
                                 struct gattc_cmp_evt const *param,
                                 kernel_task_id_t const dest_id,
                                 kernel_task_id_t const src_id)
{
    uint8_t state = kernel_state_get(dest_id);
    
    uint8_t conidx = KERNEL_IDX_GET(src_id);
    bk_printf("app %s dest_id = %x,conidx:%d\r\n",__func__,dest_id,conidx);
    bk_printf("operation = 0x%x,status = 0x%x,seq_num = 0x%x\r\n",param->operation,param->status,param->seq_num);
    
    if(((param->operation == GATTC_WRITE_NO_RESPONSE) || (param->operation == GATTC_WRITE)) && (param->seq_num != 0xa5))
	{
		
	}
    if((param->operation == GATTC_SDP_DISC_SVC_ALL))
	{
    	///kernel_state_set(KERNEL_BUILD_ID(TASK_BLE_APP,conidx),APPC_SERVICE_CONNECTED);
        bk_printf("\r\nAPPC_SERVICE_CONNECTED\r\n");

     //   sdp_enable_all_server_ntf_ind(conidx,1);
	}
    
    if((param->operation == GATTC_WRITE) && (param->seq_num == 0xaa))
	{
    	
        bk_printf("\r\nGATTC_WRITE\r\n");
        ////sdp_enable_all_server_ntf_ind(conidx,0);
	}    
    if((param->operation == GATTC_MTU_EXCH))
	{
    	
        bk_printf("\r\nGATTC_MTU_EXCH STATUS:0x%x\r\n",param->status);
     //   sdp_enable_all_server_ntf_ind(conidx,1);
	}
    	   
    return (KERNEL_MSG_CONSUMED);
	(void)state;
}

static int app_deviation_clac_timer_handler(kernel_msg_id_t const msgid,
                                     void const *ind,
                                     kernel_task_id_t const dest_id,
                                     kernel_task_id_t const src_id)
{

    bk_printf("%s\r\n",__func__);
    uint8_t conidx = KERNEL_IDX_GET(dest_id);
	
    ///app_deviation_info_send_end(conidx);
    
    return (KERNEL_MSG_CONSUMED);
	(void)conidx;
}

/*
 * GLOBAL VARIABLES DEFINITION
 ****************************************************************************************
 */

/* Default State handlers definition. */
KERNEL_MSG_HANDLER_TAB(appm)
{
    // Note: first message is latest message checked by kernel so default is put on top.
    {KERNEL_MSG_DEFAULT_HANDLER,    (kernel_msg_func_t)appm_msg_handler},

    // GAPM messages
    {GAPM_PROFILE_ADDED_IND,    (kernel_msg_func_t)gapm_profile_added_ind_handler},
    {GAPM_ACTIVITY_CREATED_IND, (kernel_msg_func_t)gapm_activity_created_ind_handler},
    {GAPM_ACTIVITY_STOPPED_IND, (kernel_msg_func_t)gapm_activity_stopped_ind_handler},
    {GAPM_CMP_EVT,              (kernel_msg_func_t)gapm_cmp_evt_handler},
    {GAPM_GEN_RAND_NB_IND,      (kernel_msg_func_t)gapm_gen_rand_nb_ind_handler},
    
    #if (BLE_OBSERVER || BLE_CENTRAL )
    {GAPM_EXT_ADV_REPORT_IND,   (kernel_msg_func_t)gapm_ext_adv_report_ind_handler},
    #endif

	#if (SECURE_CONNECTIONS)
    {GAPM_GEN_DH_KEY_IND,       (kernel_msg_func_t)gapm_gen_dh_key_ind_handler},
    {GAPM_PUB_KEY_IND,          (kernel_msg_func_t)gapm_get_key_ind_handler},
	#endif

    // GAPC messages
    {GAPC_GET_DEV_INFO_REQ_IND, (kernel_msg_func_t)gapc_get_dev_info_req_ind_handler},
    {GAPC_SET_DEV_INFO_REQ_IND, (kernel_msg_func_t)gapc_set_dev_info_req_ind_handler},
    {GAPC_CONNECTION_REQ_IND,   (kernel_msg_func_t)gapc_connection_req_ind_handler},
    {GAPC_PARAM_UPDATE_REQ_IND, (kernel_msg_func_t)gapc_param_update_req_ind_handler},
    {GAPC_PARAM_UPDATED_IND,    (kernel_msg_func_t)gapc_param_updated_ind_handler},
    {GAPC_CMP_EVT,              (kernel_msg_func_t)gapc_cmp_evt_handler},
    {GAPC_DISCONNECT_IND,       (kernel_msg_func_t)gapc_disconnect_ind_handler},
    {GATTC_MTU_CHANGED_IND,     (kernel_msg_func_t)app_gattc_mtu_changed_ind_handler},
    {GATTC_CMP_EVT,             (kernel_msg_func_t)gattc_cmp_evt_handler},
};

/* Defines the place holder for the states of all the task instances. */
kernel_state_t appm_state[APP_IDX_MAX];

// Application task descriptor
const struct kernel_task_desc TASK_BLE_DESC_APP = {appm_msg_handler_tab, appm_state, APP_IDX_MAX, ARRAY_LEN(appm_msg_handler_tab)};

#endif //(BLE_APP_PRESENT)

/// @} APPTASK
