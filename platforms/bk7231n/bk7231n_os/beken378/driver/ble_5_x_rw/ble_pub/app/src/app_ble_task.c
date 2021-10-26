#include "app_ble_task.h"

#if (BLE_APP_PRESENT)
#include "rw_pub.h"

#include "ble_rf_port.h"

#define APP_SCNRSP_DATA         "\x09\xFF\x00\x60\x52\x57\x2D\x42\x4C\x45"
#define APP_SCNRSP_DATA_LEN     (10)

adv_info_t adv_info = {
    .channel_map = APP_ADV_CHMAP,
    .interval_min = APP_ADV_INT_MIN,
    .interval_max = APP_ADV_INT_MAX,
    .respData = {APP_SCNRSP_DATA},
    .respDataLen = APP_SCNRSP_DATA_LEN,
    .advData = {0},
    .advDataLen = 0
};

static ble_event_cb_t ble_app_event_cb = NULL;
void ble_app_set_event_cb(ble_event_cb_t func)
{
	ble_app_event_cb = func;
}

static void ble_app_event_cb_handler(ble_event_t event, void *param)
{
	ble_event_cb_t func = ble_app_event_cb;
	
	if(func != NULL)
	{
		func(event,param);
	}
}
/*
 * LOCAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */
void appm_create_advertising(void)
{
	bk_printf("[%s]\r\n",__func__);
	
	if (get_app_ble_adv_state() == APP_ADV_STATE_IDLE)
	{
		// And the next expected operation code for the command completed event
		ble_appm_create_advertising(adv_info.channel_map,adv_info.interval_min,adv_info.interval_max);
		// Keep the current operation
		set_app_ble_adv_state(APP_ADV_STATE_CREATING);
	}
}

void appm_set_adv_data(void)
{
	int ret;

	// Update advertising state
	set_app_ble_adv_state(APP_ADV_STATE_SETTING_ADV_DATA);
	ret = ble_appm_set_adv_data(get_app_ble_adv_actv_idx(),adv_info.advData,adv_info.advDataLen);
	
	bk_printf("[%s]ret:%d\r\n",__func__,ret);
}

void appm_set_scan_rsp_data(void)
{
    bk_printf("[%s]\r\n",__func__);

	// Update advertising state
    set_app_ble_adv_state(APP_ADV_STATE_SETTING_SCAN_RSP_DATA);
	ble_appm_set_scan_rsp_data(get_app_ble_adv_actv_idx(),adv_info.respData,adv_info.respDataLen);
}

void appm_start_advertising_cmd(void)
{
    bk_printf("[%s]\r\n",__func__);
    if((get_app_ble_adv_state() == APP_ADV_STATE_CREATED) 
		|| (get_app_ble_adv_state() == APP_ADV_STATE_SETTING_SCAN_RSP_DATA))
    {
    	// Keep the current operation
        set_app_ble_adv_state(APP_ADV_STATE_STARTING);

		ble_appm_start_advertising(get_app_ble_adv_actv_idx(),0);
    }
}

/* 设备发起广播函数*/
ble_err_t appm_start_advertising(void)
{
    ble_err_t status = ERR_SUCCESS;
	/*
#if !(DEEP_SLEEP)
    kernel_msg_send_basic(APP_PERIOD_TIMER,TASK_APP,TASK_APP);
#endif
	*/
    /*if(BLE_ROLE_MASTER == ble_get_role_mode())
    {
        bk_printf("current role should not be master\r\n");
        return ERR_ADV_FAIL;
    }
	*/
    // Check if the advertising procedure is already is progress
    ///if (kernel_state_get(TASK_APP) == APPM_READY)
    if (APP_ADV_STATE_IDLE == get_app_ble_adv_state())
    {
    	appm_create_advertising();
       /*
        bk_printf("appm start advertising\r\n");

        ble_init_over = 1;
      ///  mcu_prevent_set(MCU_PS_BLE_FROBID);
        ble_set_role_mode(BLE_ROLE_SLAVE);

        // Set the state of the task to APPM_ADVERTISING
        kernel_state_set(TASK_APP, APPM_ADVERTISING);
        */
    }
    else
    {
        status = ERR_ADV_FAIL;
    }
	
    return status;
    // else ignore the request
}

/* 设备主动停止广播函数*/
ble_err_t appm_stop_advertising(void)
{
	ble_err_t status = ERR_SUCCESS;
	bk_printf("[%s]\r\n",__func__);

    if (APP_ADV_STATE_STARTED == get_app_ble_adv_state())
    {
        // Go in ready state
        ///kernel_state_set(TASK_APP, APPM_READY);
        // Update advertising state
		set_app_ble_adv_state(APP_ADV_STATE_STOPPING);

		ble_appm_stop_advertising(get_app_ble_adv_actv_idx());

        ///ble_set_role_mode(BLE_ROLE_NONE);	
        ///mcu_prevent_clear(MCU_PS_BLE_FROBID);
    }
    else if(get_app_ble_adv_state() != APP_ADV_STATE_IDLE )
    {
        status = ERR_STOP_ADV_FAIL;	
    }

    return  status;
}

void appm_delete_advertising(void)
{
    bk_printf("[%s]\r\n",__func__);

	set_app_ble_adv_state(APP_ADV_STATE_DELETEING);
	ble_appm_delete_advertising(get_app_ble_adv_actv_idx());
}

/* 设备发起定向广播函数*/
void appm_start_direct_dvertising(void)
{
    appm_start_advertising();	
	
    return;
#if 0
	// Check if the advertising procedure is already is progress
    if (kernel_state_get(TASK_APP) == APPM_READY)
    {
        struct gapm_start_advertise_cmd *cmd = KERNEL_MSG_ALLOC(GAPM_START_ADVERTISE_CMD,
                                                            TASK_GAPM, TASK_APP,
                                                            gapm_start_advertise_cmd);

        cmd->op.addr_src    = GAPM_STATIC_ADDR;
        cmd->channel_map    = APP_ADV_CHMAP;

        cmd->intv_min = APP_ADV_FAST_INT;
        cmd->intv_max = APP_ADV_FAST_INT;

        cmd->op.code        = GAPM_ADV_UNDIRECT;
		
        cmd->info.host.mode = GAP_GEN_DISCOVERABLE;
		 
		/*
		 * If the peripheral is already bonded with a central device, use the direct advertising
		 * procedure (BD Address of the peer device is stored in NVDS.
		 */
		if(app_sec_get_bond_status())
		{
			#if (NVDS_SUPPORT)
			uint8_t bd_len = NVDS_LEN_PEER_BD_ADDRESS;
			#endif
			cmd->op.code   = GAPM_ADV_DIRECT_LDC;
			//cmd->info.direct.addr_type = 1;
			
	
			#if (NVDS_SUPPORT)
			if (nvds_get(NVDS_TAG_PEER_BD_ADDRESS, &bd_len,
	            		(uint8_t *)cmd->info.direct.addr.addr) != NVDS_OK)
			{
			    // An error has occurred during access to the NVDS
			    ASSERT_INFO(0,NVDS_TAG_PEER_BD_ADDRESS,bd_len);
			}
			#endif

			kernel_msg_send(cmd);

			#if !(DEEP_SLEEP)
			kernel_msg_send_basic(APP_PERIOD_TIMER,TASK_APP,TASK_APP);
			#endif

		//	UART_PRINTF("appm start direct advertising\r\n");
		}
		else
		{
			kernel_msg_free(kernel_param2msg(cmd));
            appm_start_advertising();		
 		//	UART_PRINTF("appm start general advertising\r\n");
		}

	    kernel_state_set(TASK_APP, APPM_ADVERTISING);	
	}		
#endif
}

/*设备主动断开连接函数*/
void appm_disconnect(unsigned char reason)
{
	///if (kernel_state_get(TASK_BLE_APP) == APPM_CONNECTED)
	////if(app_ble_ctx.init_state == APP_INIT_STATE_CONECTED)
	{
	    ble_appm_disconnect(get_app_ble_conidx(),reason);///0x13
	}
}

/////////////////////////////////////////////
uint8_t appm_updata_adv_data(uint16_t length, uint8_t *p_data)
{
    // Returned status
    uint8_t status = GAP_ERR_INVALID_PARAM;
	
	bk_printf("[%s]\r\n",__func__);
    if(!ble_app_gapm_adv_check_data_sanity(length,p_data))
    {
        status = GAP_ERR_ADV_DATA_INVALID;
        return status;
    }
	
    if((get_app_ble_adv_state() == APP_ADV_STATE_CREATED) 
		|| (get_app_ble_adv_state() == APP_ADV_STATE_STARTED))
    {
    	// Update advertising state
        set_app_ble_adv_state(APP_ADV_STATE_UPDATA_ADV_DATA);
		ble_appm_update_adv_data(get_app_ble_adv_actv_idx(),p_data,length);

        status = GAP_ERR_NO_ERROR;
    }
	else
    {
        status = GAP_ERR_COMMAND_DISALLOWED;
    }
	
    return status;
}

uint8_t appm_updata_scan_rsp_data(uint16_t length, uint8_t *p_data)
{
    // Returned status
    uint8_t status = GAP_ERR_INVALID_PARAM;
	
	bk_printf("[%s]\r\n",__func__);
	
    if((get_app_ble_adv_state() == APP_ADV_STATE_CREATED) 
		|| (get_app_ble_adv_state() == APP_ADV_STATE_STARTED))
    {
    	// Update advertising state
        set_app_ble_adv_state(APP_ADV_STATE_UPDATA_SCAN_RSP_DATA);
		
		ble_appm_updata_scan_rsp_data(get_app_ble_adv_actv_idx(),p_data,length);
		
        status = GAP_ERR_NO_ERROR;
    }
	else
    {
        status = GAP_ERR_COMMAND_DISALLOWED;
    }
	
    return status;
}


int appm_update_adv_data( uint8_t* adv_buff, uint8_t adv_len, uint8_t* scan_buff, uint8_t scan_len)
{
	int advstate = get_app_ble_adv_state();
	int ret;
	
	if (/*(advstate == APP_ADV_STATE_CREATED) 
		|| */(advstate == APP_ADV_STATE_STARTED))
	{
	   set_app_ble_adv_state(APP_ADV_STATE_UPDATA2_ADV_DATA);
	   ret = ble_appm_update_adv_data(get_app_ble_adv_actv_idx(),adv_buff,adv_len);
	   return ret;
	}
	
	return -1;
}

void appm_update_adv_scan_handler(void)
{
	set_app_ble_adv_state(APP_ADV_STATE_UPDATA2_SCAN_RSP_DATA);
	ble_appm_set_scan_rsp_data(get_app_ble_adv_actv_idx(),adv_info.respData,adv_info.respDataLen);
}

void appm_update_param(struct gapc_conn_param *conn_param)
{
    ble_appm_update_param(get_app_ble_conidx(),conn_param);
}


//////////////////Application ///////////////////////////
void appm_adv_fsm_next(char flag)
{
    bk_printf("[%s] cur adv_state:%x\r\n",__func__,get_app_ble_adv_state());
    switch (get_app_ble_adv_state())
    {
        case (APP_ADV_STATE_IDLE): // 0
	        {
	            // Create advertising
	            appm_create_advertising();
	        }
			break;
        case (APP_ADV_STATE_CREATING)://1
	        {
				if(flag == 0)
				{
					// Set advertising data
					appm_set_adv_data();
				}
				else
				{
					set_app_ble_adv_state(APP_ADV_STATE_IDLE);
				}
	        } 
			break;
        case (APP_ADV_STATE_SETTING_ADV_DATA): //2
	        {
				if(flag == 0)
				{
					// Set scan response data
					appm_set_scan_rsp_data();
				}
				else
				{
					appm_delete_advertising();
				}
	        }
			break;
        case (APP_ADV_STATE_CREATED)://5
        case (APP_ADV_STATE_SETTING_SCAN_RSP_DATA)://3
	        {
				if((get_app_ble_adv_state() == APP_ADV_STATE_SETTING_SCAN_RSP_DATA) && (flag == 1))
				{
					appm_delete_advertising();
				}
				else
				{
					// Start advertising activity
					appm_start_advertising_cmd();
				}
	        }
			break;
        case (APP_ADV_STATE_UPDATA_ADV_DATA)://4
        case APP_ADV_STATE_UPDATA_SCAN_RSP_DATA:
        case (APP_ADV_STATE_STARTING)://6
	        {
				if((get_app_ble_adv_state() == APP_ADV_STATE_STARTING) && (flag == 1))
				{
					appm_delete_advertising();
				}
				else
				{
					// Go to started state
					set_app_ble_adv_state(APP_ADV_STATE_STARTED);
				}
	        }
			break;
        case (APP_ADV_STATE_STARTED)://7
	        {
	            // Stop advertising activity
	            appm_stop_advertising();
	        }
			break;
        case (APP_ADV_STATE_STOPPING)://8
	        {
	            // Go delete state
	            appm_delete_advertising();
	        }
			break;
        case (APP_ADV_STATE_DELETEING)://
			set_app_ble_adv_state(APP_ADV_STATE_IDLE);
       		break;
		case APP_ADV_STATE_UPDATA2_ADV_DATA:
			if(!(flag & 1)){
				appm_update_adv_scan_handler();
			}else if(flag &0x01){
				set_app_ble_adv_state(APP_ADV_STATE_STARTED);
			}
			break;
		case APP_ADV_STATE_UPDATA2_SCAN_RSP_DATA:
			set_app_ble_adv_state(APP_ADV_STATE_STARTED);
			break;
        default:
            ASSERT_ERR(0);
        	break;
    }
    
    bk_printf("end adv_state:%x\r\n",get_app_ble_adv_state());
}

void appm_create_scaning(void)
{
	bk_printf("[%s]\r\n",__func__);
	
	if (get_app_ble_scan_state() == APP_SCAN_STATE_IDLE)
	{
		// Keep the current operation
		set_app_ble_scan_state(APP_SCAN_STATE_CREATING);
		// And the next expected operation code for the command completed event
		ble_appm_create_scaning();
	}
	else if (get_app_ble_scan_state() == APP_SCAN_STATE_CREATED)
	{
		appm_scan_fsm_next();
	}
}

ble_err_t appm_start_scaning(void)
{
	bk_printf("%s\r\n", __FUNCTION__);
	ble_err_t status = ERR_SUCCESS;
	if ((get_app_ble_scan_state() == APP_SCAN_STATE_IDLE) || (get_app_ble_scan_state() == APP_SCAN_STATE_CREATED)) {
		appm_create_scaning();
	} else {
		status = ERR_SCAN_FAIL;
	}

	return status;
}

ble_err_t appm_stop_scaning(void)
{
	ble_err_t status = ERR_SUCCESS;
	if (get_app_ble_scan_state() == APP_SCAN_STATE_STARTED) {
		// Stop scaning activity  
        ble_appm_stop_scaning();
	} else {
		status = ERR_SCAN_FAIL;
	}

	return status;
}

void appm_scan_fsm_next(void)
{
    //  return;
    switch (get_app_ble_scan_state())
    {
        case (APP_SCAN_STATE_IDLE)://0
        {
            // Create advertising
            appm_create_scaning();
        } break;

        case (APP_SCAN_STATE_CREATING):  //1      
        case (APP_SCAN_STATE_CREATED):   //2    
        {
            // Start scaning activity
            ble_appm_start_scaning();
        } break;

        case (APP_SCAN_STATE_STARTING)://3
        {
            // Go to started state
            set_app_ble_scan_state(APP_SCAN_STATE_STARTED);
        } break;

        case (APP_SCAN_STATE_STARTED)://4
        {
            // Stop scaning activity  
            ble_appm_stop_scaning();
        } break;

        case (APP_SCAN_STATE_STOPPING)://5
        {
            // Go created state
            set_app_ble_scan_state(APP_SCAN_STATE_CREATED);
        } break;

        default:
        {
            ASSERT_ERR(0);
        } break;
    }
    bk_printf("end scan_state:%x\r\n",get_app_ble_scan_state());
}

void appm_update_adv_state(char start)
{
    // TODO [LT] - Check current advertising state

    // Start or stop advertising
    appm_adv_fsm_next(0);
}

static void ble_common_event_callback_handler(void *parama)
{
	struct gapm_cmp_evt *param = (struct gapm_cmp_evt *)parama;
	uint8_t cur_adv_state;
	
	cur_adv_state = get_app_ble_adv_state();
	switch(param->operation)
	{
		case (GAPM_CREATE_ADV_ACTIVITY)://0xA0              
        case (GAPM_SET_ADV_DATA)://0xA9
        case (GAPM_SET_SCAN_RSP_DATA)://0xAA
        {
			/// Sanity checks
            ASSERT(get_app_ble_adv_op() == param->operation);///, get_app_ble_adv_op(), param->operation);
            ///ASSERT(param->status == GAP_ERR_NO_ERROR);///, param->status, get_app_ble_adv_op());
            // Perform next operation
            
            if((param->status == GAP_ERR_NO_ERROR)
				///if ind status is ERROR,recover adv fsm to APP_ADV_STATE_STARTED
				|| (APP_ADV_STATE_UPDATA_ADV_DATA == cur_adv_state)
				|| (APP_ADV_STATE_UPDATA_SCAN_RSP_DATA == cur_adv_state)
				|| (APP_ADV_STATE_UPDATA2_ADV_DATA == cur_adv_state)
				|| (APP_ADV_STATE_UPDATA2_SCAN_RSP_DATA == cur_adv_state))
            {
                 appm_adv_fsm_next((param->status == GAP_ERR_NO_ERROR)?0:1);
            }
			else
			{
				appm_adv_fsm_next((param->status == GAP_ERR_NO_ERROR)?0:1);
			}
			
			////Notice APP User updata adv or scan response Data status
			if((APP_ADV_STATE_UPDATA_ADV_DATA == cur_adv_state)
				|| (APP_ADV_STATE_UPDATA_SCAN_RSP_DATA == cur_adv_state)
				|| (APP_ADV_STATE_UPDATA2_ADV_DATA == cur_adv_state)
				|| (APP_ADV_STATE_UPDATA2_SCAN_RSP_DATA == cur_adv_state))
			{
				ble_event_t event = BLE_EVENT_MAX;
				
				switch(param->operation)
				{
					case GAPM_SET_ADV_DATA:
						event = BLE_UPDATA_ADV_DATA_IND;
						break;
					case GAPM_SET_SCAN_RSP_DATA:
						event = BLE_UPDATA_SCAN_RSP_IND;
						break;
					default:
						break;
				}
				
				if(event != BLE_EVENT_MAX)
				{
					ble_app_event_cb_handler(event,&param->status);
				}
			}
        }
        break;
		case (GAPM_START_ACTIVITY)://0xA4
        {
            if(param->status == GAP_ERR_NO_ERROR)
            {
                if(get_app_ble_adv_state() == APP_ADV_STATE_STARTING)
                {
                    appm_adv_fsm_next(0); 
                }
                #if (BLE_OBSERVER || BLE_CENTRAL)
                if(get_app_ble_scan_state() == APP_SCAN_STATE_STARTING)
                {
                    appm_scan_fsm_next(); 
                }
                #endif
            }
			else if(get_app_ble_adv_state() == APP_ADV_STATE_STARTING)
			{
				if(param->status == GAP_ERR_COMMAND_DISALLOWED)
				{
					appm_adv_fsm_next(0); 
				}
				else
				{
					appm_adv_fsm_next(1); 
				}
			}
        }
		break;
		case (GAPM_STOP_ACTIVITY)://0xA5
        {
            if(param->status == GAP_ERR_NO_ERROR)
            {
                if(get_app_ble_adv_state() == APP_ADV_STATE_STOPPING)
                {
                    appm_adv_fsm_next(0); 
                }
                #if (BLE_OBSERVER || BLE_CENTRAL)
                if(get_app_ble_scan_state() == APP_SCAN_STATE_STOPPING)
                {
                    appm_scan_fsm_next(); 
                }
                #endif
            }
        }break;
		case (GAPM_DELETE_ACTIVITY)://0xA7
        {
            if(param->status == GAP_ERR_NO_ERROR)
            {
                if(get_app_ble_adv_state() == APP_ADV_STATE_DELETEING)
                {
                     appm_adv_fsm_next(0); 
                }            
            }
        
        }break;
	#if (BLE_OBSERVER || BLE_CENTRAL)
        case (GAPM_CREATE_SCAN_ACTIVITY)://0xA1           
        {
            #if (BLE_OBSERVER || BLE_CENTRAL)
             if(param->status == GAP_ERR_NO_ERROR)
             {
                 appm_scan_fsm_next();
             }           
            #endif
        }break;
    #endif
	    case (GAPM_PROFILE_TASK_ADD)://0x1b
        {                 

        }
        break;
		case (GAPM_DELETE_ALL_ACTIVITIES) :
        {
            // Re-Invoke activty
            set_app_ble_adv_state(APP_ADV_STATE_IDLE);
            #if (BLE_OBSERVER || BLE_CENTRAL )
			set_app_ble_scan_state(APP_SCAN_STATE_IDLE);
            #endif
        } break;
		default:
			break;
	}
}

static void ble_activity_stopped_event_callback_handler(void *parama)
{
	struct gapm_activity_stopped_ind *p_param = (struct gapm_activity_stopped_ind *)parama;
	
	if ((get_app_ble_adv_state() == APP_ADV_STATE_STARTED) 
		&& (p_param->actv_type == GAPM_ACTV_TYPE_ADV))
    {
        // Act as if activity had been stopped by the application
        set_app_ble_adv_state(APP_ADV_STATE_STOPPING);

        // Perform next operation
        appm_adv_fsm_next(0);
    }
    else if ((get_app_ble_adv_state() == APP_ADV_STATE_WAITING_DELETE) 
		&& (p_param->actv_type == GAPM_ACTV_TYPE_ADV))
    {
        // Act as if activity had been stopped by the application
       // set_app_ble_adv_state(APP_ADV_STATE_DELETEING);

        // Perform next operation
        appm_adv_fsm_next(0);
    }
	else if((get_app_ble_scan_state() == APP_SCAN_STATE_STARTED) 
		&& (p_param->actv_type == GAPM_ACTV_TYPE_SCAN))
    {
        {
            // Act as if activity had been stopped by the application
            set_app_ble_scan_state(APP_SCAN_STATE_STOPPING);
            // Perform next operation
            appm_scan_fsm_next();
        }
    }

}

static void ble_actiove_created_ind_handler(struct ble_activity_created_ind *p_param)
{
	switch(p_param->actv_type)
	{
		case BLE_ACTV_TYPE_ADV:
			if(get_app_ble_adv_state() == APP_ADV_STATE_CREATING){
				// Store the advertising activity index
				set_app_ble_adv_actv_idx( p_param->actv_idx );
				bk_printf("adv_actv_idx:%d,tx_pwr:%d\r\n",p_param->actv_idx,p_param->tx_pwr);
			}
			break;
		case BLE_ACTV_TYPE_SCAN:
			if((get_app_ble_scan_state() == APP_SCAN_STATE_CREATING))
			{
				// Store the scaning activity index
				set_app_ble_scan_actv_idx(p_param->actv_idx);
				set_app_ble_scan_state(APP_SCAN_STATE_CREATED);
				bk_printf("scan_actv_idx:%d,scan_state:%d\r\n",get_app_ble_scan_actv_idx(),get_app_ble_scan_state());
			}
			break;
		default:
			break;
	}
}

static void ble_event_callback_handler(ble_event_t event, void *param)
{
    switch(event)
    {
		case BLE_ACTIVITY_CREATED_IND:
			ble_actiove_created_ind_handler((struct ble_activity_created_ind *)param);
			break;
		case BLE_COMMON_EVT:
			ble_common_event_callback_handler(param);
			break;
		case BLE_ACTIVITY_STOPPED_IND:
			ble_activity_stopped_event_callback_handler(param);
			break;
		default:
			break;
    }
	ble_app_event_cb_handler(event,param);
}

void app_ble_task_init(void)
{
	ble_set_event_cb(ble_event_callback_handler);
}

void appm_build_adv_data(uint16_t max_length, uint16_t *p_length, uint8_t *p_buf)
{
    // Remaining Length
    uint8_t rem_len = max_length;

    // Sanity check
    ASSERT_ERR(rem_len >= max_length);

    // Get remaining space in the Advertising Data - 2 bytes are used for name length/flag
    rem_len -= *p_length;

    // Check if additional data can be added to the Advertising data - 2 bytes needed for type and length
    if (rem_len > 2)
    {
        uint8_t dev_name_length = common_min(app_ble_ctx.dev_name_len, (rem_len - 2));

        // Device name length
        *p_buf = dev_name_length + 1;
        // Device name flag (check if device name is complete or not)
        *(p_buf + 1) = (dev_name_length == app_ble_ctx.dev_name_len) ? '\x09' : '\x08';
        // Copy device name
        memcpy(p_buf + 2, app_ble_ctx.dev_name, dev_name_length);

        // Update advertising data length
        *p_length += (dev_name_length + 2);
    }
}


uint8_t appm_get_app_status(void)
{
	int status = 0;
	////return kernel_state_get(TASK_APP);
	if(kernel_state_get(TASK_BLE_APP) ==  APPM_CONNECTED){
		status = 9;
	}else if(get_app_ble_adv_state() == APP_ADV_STATE_STARTED){
		status = 3;
	}
	///return get_app_ble_adv_state();
	return status;
}

#endif

