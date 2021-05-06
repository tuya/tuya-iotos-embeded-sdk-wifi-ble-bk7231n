#include "rwip_config.h"             // SW configuration
#if (BLE_APP_SDP)
#include "app_task.h"                // Application task Definition
#include "app_sdp.h"              // Application Definition
#include "gap.h"                     // GAP Definition
#include "gapm_task.h"               // GAP Manager Task API
#include "gapc_task.h"               // GAP Controller Task API
#include "gattc_task.h"
#include "common_bt.h"                   // Common BT Definition
#include "common_math.h"                 // Common Maths Definition
#include "sdp_service_task.h"
#include "sdp_service.h"
#include "uart.h"
#include "gpio.h"
#include "llm_util.h"


uint32_t con_peer_idx;
struct gap_bdaddr  con_bdaddr;
extern uint8_t max_scan_numbers;

scan_info_t scan_info = {
    .filter_en = SCAN_FILT_DUPLIC_DIS,
    .channel_map = 7,
    .interval = 100,
    .window = 20
};

void appm_set_max_scan_nums(uint8_t max)
{
    if(max > MAX_SCAN_NUM)
    {
        max_scan_numbers = MAX_SCAN_NUM;
    }
    else
    {
	    max_scan_numbers = max;
    }
}

uint8_t appm_get_max_scan_nums(void)
{
	return max_scan_numbers;
}


ble_err_t appm_start_scanning(void)
{
    ble_err_t status = ERR_SUCCESS;
    
    if(BLE_ROLE_SLAVE == ble_get_role_mode())
    {
        bk_printf("current role should not be slave\r\n");
        return ERR_SCAN_FAIL;
    }
    if (kernel_state_get(TASK_APP) == APPM_READY)
    {
        // Prepare the GAPM_START_SCAN_CMD message
        struct gapm_start_scan_cmd *cmd = KERNEL_MSG_ALLOC(GAPM_START_SCAN_CMD,
                                          TASK_GAPM, TASK_APP,
                                          gapm_start_scan_cmd);
        cmd->op.addr_src = GAPM_STATIC_ADDR;
        cmd->op.code = GAPM_SCAN_PASSIVE;
        cmd->interval = scan_info.interval;//50;
        cmd->window  = scan_info.window;//40;
        cmd->mode = GAP_OBSERVER_MODE;
        cmd->filt_policy = SCAN_ALLOW_ADV_ALL;
        cmd->filter_duplic = scan_info.filter_en;
        cmd->channel_map = scan_info.channel_map; /*APP_ADV_CHMAP*/
        adv_count = 0;
        ble_set_role_mode(BLE_ROLE_MASTER);
        // Send the message
        kernel_msg_send(cmd);
        // Set the state of the task to APPM_SCANNING
        kernel_state_set(TASK_APP, APPM_SCANNING);

    }
    else
    {
        bk_printf("CURRENT STATE= %x\r\n",kernel_state_get(TASK_APP));
		status = ERR_SCAN_FAIL;
    }

    return status;
}

ble_err_t appm_stop_scanning(void)
{
    ble_err_t status = ERR_SUCCESS;
    
    if (kernel_state_get(TASK_APP) == APPM_SCANNING)
    {
        // Prepare the GAPM_CANCEL_CMD message
        struct gapm_cancel_cmd *cmd = KERNEL_MSG_ALLOC(GAPM_CANCEL_CMD,
                                      TASK_GAPM, TASK_APP,
                                      gapm_cancel_cmd);
        cmd->operation = GAPM_CANCEL;

        ble_set_role_mode(BLE_ROLE_NONE);
        // Send the message
        kernel_msg_send(cmd);
        // Go in ready state
        kernel_state_set(TASK_APP, APPM_READY);
    }
    else
    {
	    bk_printf("CURRENT STATE= %x\r\n",kernel_state_get(TASK_APP));
        status = ERR_STOP_SCAN_FAIL;
    }
	return status;
}


ble_err_t appm_start_connenct_by_id(uint8_t id)
{
    ble_err_t status = ERR_SUCCESS;

    if(BLE_ROLE_SLAVE == ble_get_role_mode())
    {
        bk_printf("current role should not be slave\r\n");
        return ERR_CONN_FAIL;
    }
    if (kernel_state_get(TASK_APP) == APPM_READY)
    {
        // Prepare the GAPM_START_SCAN_CMD message
        struct gapm_start_connection_cmd *cmd = KERNEL_MSG_ALLOC_DYN(
        										GAPM_START_CONNECTION_CMD,
                                                TASK_GAPM, TASK_APP,
                                                gapm_start_connection_cmd, 
                                                sizeof(struct gap_bdaddr));
        cmd->op.addr_src = GAPM_STATIC_ADDR;
        cmd->op.code = GAPM_CONNECTION_DIRECT;
        cmd->scan_interval = 50;
        cmd->scan_window  = 40;
        cmd->con_intv_min = 16;
        cmd->con_intv_max = 20;
        cmd->con_latency = 0;
        cmd->superv_to = 200;
        cmd->ce_len_min = 100;
        cmd->ce_len_max = 200;
        cmd->nb_peers = 1;

        cmd->peers[0].addr_type = 0;
        memcpy(&cmd->peers[0].addr.addr[0],&ble_scan_list.info[id].adv_addr.addr[0],6);

		bk_printf("conn addr = %02x:%02x:%02x:%02x:%02x:%02x\r\n",cmd->peers[0].addr.addr[0],
			cmd->peers[0].addr.addr[1],cmd->peers[0].addr.addr[2],
			cmd->peers[0].addr.addr[3],cmd->peers[0].addr.addr[4],
			cmd->peers[0].addr.addr[5]);

        // Send the message
        kernel_msg_send(cmd);

        ble_set_role_mode(BLE_ROLE_MASTER);
        // Set the state of the task to APPM_SCANNING
        kernel_state_set(TASK_APP, APPM_CONNECTING);
    }
    else
    {
	    bk_printf("CURRENT STATE= %x\r\n",kernel_state_get(TASK_APP));
        status = ERR_CONN_FAIL;
    }
	return status;
}


ble_err_t appm_start_connenct_by_addr(uint8_t* bdaddr)
{
    ble_err_t status = ERR_SUCCESS;

    if(BLE_ROLE_SLAVE == ble_get_role_mode())
    {
        bk_printf("current role should not be slave\r\n");
        return ERR_CONN_FAIL;
    }
    if (kernel_state_get(TASK_APP) == APPM_READY)
    {
        // Prepare the GAPM_START_SCAN_CMD message
        struct gapm_start_connection_cmd *cmd = KERNEL_MSG_ALLOC_DYN(
        										GAPM_START_CONNECTION_CMD,
                                                TASK_GAPM, TASK_APP,
                                                gapm_start_connection_cmd, 
                                                sizeof(struct gap_bdaddr));
        cmd->op.addr_src = GAPM_STATIC_ADDR;
        cmd->op.code = GAPM_CONNECTION_DIRECT;
        cmd->scan_interval = 50;
        cmd->scan_window  = 40;
        cmd->con_intv_min = 16;
        cmd->con_intv_max = 20;
        cmd->con_latency = 0;
        cmd->superv_to = 200;
        cmd->ce_len_min = 100;
        cmd->ce_len_max = 200;
        cmd->nb_peers = 1;

        cmd->peers[0].addr_type = 0;
        memcpy(&cmd->peers[0].addr.addr[0],bdaddr,6);

		bk_printf("conn addr = %02x:%02x:%02x:%02x:%02x:%02x\r\n",cmd->peers[0].addr.addr[0],
		cmd->peers[0].addr.addr[1],cmd->peers[0].addr.addr[2],
		cmd->peers[0].addr.addr[3],cmd->peers[0].addr.addr[4],
		cmd->peers[0].addr.addr[5]);

        // Send the message
        kernel_msg_send(cmd);

        ble_set_role_mode(BLE_ROLE_MASTER);
        // Set the state of the task to APPM_SCANNING
        kernel_state_set(TASK_APP, APPM_CONNECTING);

    }
    else
    {
	    bk_printf("CURRENT STATE= %x\r\n",kernel_state_get(TASK_APP));
        status = ERR_CONN_FAIL;
    }
	return status;
}


ble_err_t appm_stop_connencting(void)
{
    ble_err_t status = ERR_SUCCESS;
    
    if (kernel_state_get(TASK_APP) == APPM_CONNECTING)
    {
        // Go in ready state
        // Prepare the GAPM_CANCEL_CMD message
        struct gapm_cancel_cmd *cmd = KERNEL_MSG_ALLOC(
        							  GAPM_CANCEL_CMD,
                                      TASK_GAPM, TASK_APP,
                                      gapm_cancel_cmd);
        cmd->operation = GAPM_CANCEL;
        // Send the message

        ble_set_role_mode(BLE_ROLE_NONE);
        
        kernel_msg_send(cmd);
    }
    else
    {
	    bk_printf("CURRENT STATE= %x\r\n",kernel_state_get(TASK_APP));
        status = ERR_STOP_CONN_FAIL;
    }
	return status;
}


ble_err_t appm_disconnect_link(void)
{
    ble_err_t status = ERR_SUCCESS;
    
    if((kernel_state_get(TASK_APP) == APPM_CONNECTED) 
		||(kernel_state_get(TASK_APP) == APPM_LINK_CONNECTED)
		||(kernel_state_get(TASK_APP) == APPM_SDP_DISCOVERING))
    {
        struct gapc_disconnect_cmd *cmd = KERNEL_MSG_ALLOC(GAPC_DISCONNECT_CMD,
                                          KERNEL_BUILD_ID(TASK_GAPC, 0), 
                                          TASK_APP,
                                          gapc_disconnect_cmd);
        cmd->operation = GAPC_DISCONNECT;
        cmd->reason    = COMMON_ERROR_REMOTE_USER_TERM_CON;

        // Send the message
        kernel_msg_send(cmd);
    }
    else
    {
	    bk_printf("CURRENT STATE= %x\r\n",kernel_state_get(TASK_APP));
        status = ERR_DISCONN_FAIL;
    }
	return status;
}


uint8_t appm_adv_data_decode(uint8_t len,const uint8_t *data,uint8_t *find_str,uint8_t str_len)
{
    uint8_t find = 0;
    uint8_t index;
	uint8_t i;
    for(index = 0; index < len;)
    {
        switch(data[index + 1])
        {
        case GAP_AD_TYPE_FLAGS:
        {
            bk_printf("AD_TYPE : ");
            for(i = 0; i < data[index] - 1; i++)
            {
                bk_printf("%02x ",data[index + 2 + i]);
            }
            bk_printf("\r\n");
            index +=(data[index] + 1);
        }
        break;
        case GAP_AD_TYPE_SHORTENED_NAME:
        case GAP_AD_TYPE_COMPLETE_NAME:
        {
            if(strncmp((char*)&data[index + 2],(char*)find_str,str_len) == 0)
            {
                find = 1;
            }
            bk_printf("ADV_NAME : ");
            for(i = 0; i < data[index] - 1; i++)
            {
                bk_printf("%c",data[index + 2 + i]);
            }
            bk_printf("\r\n");
            index +=(data[index] + 1);
        }
        break;
        case GAP_AD_TYPE_MORE_16_BIT_UUID:
        {
            bk_printf("UUID : ");
            for(i = 0; i < data[index] - 1;)
            {
                bk_printf("%02x%02x  ",data[index + 2 + i],data[index + 3 + i]);
                i+=2;
            }
            bk_printf("\r\n");
            index +=(data[index] + 1);
        }
        break;
        default:
        {
            index +=(data[index] + 1);
        }
        break;
        }
    }
    return find;
}


ble_err_t appm_write_data_by_uuid(uint16_t uuid,uint8_t len,uint8_t *data)
{
    ble_err_t status = ERR_SUCCESS;
    
    if(BLE_ROLE_MASTER != ble_get_role_mode())
    {
        bk_printf("current role should be master\r\n");
        return ERR_WRITE_FAIL;
    }
    if(kernel_state_get(TASK_APP) == APPM_CONNECTED)
    {
        struct sdp_write_info_req *req = KERNEL_MSG_ALLOC_DYN(SDP_WRITE_VALUE_INFO_REQ,
                                         prf_get_task_from_id(TASK_ID_SDP), 
                                         TASK_APP,
                                         sdp_write_info_req, len);
        // Fill in the parameter structure
        req->uuid = uuid;
        req->length = len;
        memcpy(req->data,data,len);
        // Send the message
        kernel_msg_send(req);
    }
    else
    {
        status = ERR_WRITE_FAIL;
    }
    
	return status;
}

ble_err_t appm_write_ntf_cfg(uint16_t uuid,uint16_t number,uint16_t cfg)
{
    ble_err_t status = ERR_SUCCESS;
    bk_printf("uuid = 0x%04x,cfg = 0x%x\r\n",uuid, cfg);
    if(kernel_state_get(TASK_APP) == APPM_CONNECTED )
    {
        struct sdp_w_ntf_cfg_req *req = KERNEL_MSG_ALLOC(SDP_WRITE_NTF_CFG_REQ,
                                        prf_get_task_from_id(TASK_ID_SDP), 
                                        TASK_APP,
                                        sdp_w_ntf_cfg_req);
        // Fill in the parameter structure
        req->uuid = uuid;
		req->char_num = number;
        req->ntf_cfg = cfg;
		
        // Send the message
        kernel_msg_send(req);
    }
    else
    {
        status = ERR_WRITE_FAIL;
    }
    
	return status;
}


ble_err_t appm_read_data_by_uuid(uint16_t uuid)
{
    ble_err_t status = ERR_SUCCESS;
    
    if(BLE_ROLE_MASTER != ble_get_role_mode())
    {
        bk_printf("current role should be master\r\n");
        return ERR_READ_FAIL;
    }
    if(kernel_state_get(TASK_APP) == APPM_CONNECTED)
    {
        struct sdp_read_info_req *req = KERNEL_MSG_ALLOC(SDP_READ_INFO_REQ,
                                        prf_get_task_from_id(TASK_ID_SDP), 
                                        TASK_APP,
                                        sdp_read_info_req);
        // Fill in the parameter structure
        req->uuid = uuid;
        req->info = SDPC_CHAR_VAL;
		
        // Send the message
        kernel_msg_send(req);
    }
    else
    {
        status = ERR_READ_FAIL;
    }
    
	return status;
}

ble_err_t appm_read_cfg_by_uuid(uint16_t uuid)
{
    ble_err_t status = ERR_SUCCESS;
    
    if(kernel_state_get(TASK_APP) == APPM_CONNECTED )
    {
        struct sdp_read_info_req *req = KERNEL_MSG_ALLOC(SDP_READ_INFO_REQ,
                                        prf_get_task_from_id(TASK_ID_SDP), 
                                        TASK_APP,
                                        sdp_read_info_req);
        // Fill in the parameter structure
        req->uuid = uuid;
        req->info = SDPC_CHAR_NTF_CFG;
		
        // Send the message
        kernel_msg_send(req);
    }
    else
    {
        status = ERR_READ_FAIL;
    }
   
	return status;
}

ble_err_t appm_read_user_desc_by_uuid(uint16_t uuid)
{
    ble_err_t status = ERR_SUCCESS;
    
    if(kernel_state_get(TASK_APP) == APPM_CONNECTED )
    {
        struct sdp_read_info_req *req = KERNEL_MSG_ALLOC(SDP_READ_INFO_REQ,
                                        prf_get_task_from_id(TASK_ID_SDP), 
                                        TASK_APP,
                                        sdp_read_info_req);
        // Fill in the parameter structure
        req->uuid = uuid;
        req->info = SDPC_CHAR_USER_DESC_VAL;
		
        // Send the message
        kernel_msg_send(req);
    }
    else
    {
        status = ERR_READ_FAIL;
    }
    
	return status;
}

extern struct sdp_env_tag adp_serv_env[SDP_NB_SERVICE_INSTANCES_MAX];
uint8_t  sdp_enable_all_server_ntf_ind(uint8_t  reset)
{
    bool more_enable = false;
    static uint8_t server_num = 0;
	static uint8_t chars_num = 0;

	bk_printf("%s\r\n",__func__);
	
    if(reset == 1)
    {
        server_num = 0;
        chars_num = 0;
    }
    for(; server_num < SDP_NB_SERVICE_INSTANCES_MAX;)
    {
        if( adp_serv_env[server_num].use_status == VALID_STATUS)
        {
            for(; chars_num < adp_serv_env[server_num].db_env[0].cnx_env->sdp.chars_nb;)
            {
                bk_printf("server_num = %d,chars_num = %d,prop = 0x%x\r\n",server_num,chars_num,adp_serv_env[server_num].db_env[0].cnx_env->sdp.chars[chars_num].prop);
                if(adp_serv_env[server_num].db_env[0].cnx_env->sdp.chars[chars_num].prop & 0x20)
                {
                    if(adp_serv_env[server_num].db_env[0].cnx_env->sdp.chars[chars_num].prop )
                    {
                        appm_write_ntf_cfg(adp_serv_env[server_num].db_env[0].cnx_env->sdp.chars[chars_num].uuid,chars_num,0x02);
                    }
                    more_enable = true;
                    chars_num++;
                    return more_enable;
                }
                else if(adp_serv_env[server_num].db_env[0].cnx_env->sdp.chars[chars_num].prop & 0x10)
                {
                    if(adp_serv_env[server_num].db_env[0].cnx_env->sdp.chars[chars_num].prop )
                    {
                        appm_write_ntf_cfg(adp_serv_env[server_num].db_env[0].cnx_env->sdp.chars[chars_num].uuid,chars_num,0x01);
                    }
                    more_enable = true;
                    chars_num++;
                    return more_enable;
                }
                else
                {
                    chars_num++;
                }
            }
            server_num++;
            chars_num = 0;
        }
        else
        {
            more_enable = false;
            return more_enable;
        }
    }
    return more_enable;
}

void sdp_prf_register_all_atthdl2gatt(void)
{
    uint8_t idx = 0;
    for(idx = 0;idx < SDP_NB_SERVICE_INSTANCES_MAX;idx++)
    {
        struct sdp_env_tag* sdp_env = &adp_serv_env[idx];

        if(0 != sdp_env->db_env[0].cnx_env->sdp.descs_nb)
        {
            bk_printf("prf_register_atthdl2gatt start_hdl = 0x%x,end_hdl = 0x%x\r\n",sdp_env->db_env[0].cnx_env->sdp.svc.shdl,sdp_env->db_env[0].cnx_env->sdp.svc.ehdl);
            prf_register_atthdl2gatt(&(sdp_env->prf_env), 0, &sdp_env->db_env[0].cnx_env->sdp.svc);
        }
    }
}

#endif  

