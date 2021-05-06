/**
 ****************************************************************************************
 *
 * @file app_sec.c
 *
 * @brief Application Security Entry Point
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

#include "rwip_config.h"

#if (BLE_APP_SEC)

#include <string.h>
#include "common_math.h"
#include "gapc_task.h"      // GAP Controller Task API Definition
#include "gap.h"            // GAP Definition
#include "gapc.h"           // GAPC Definition
#include "prf_types.h"
#include "llc_llcp.h"

#include "application.h"            // Application API Definition
#include "app_sec.h"        // Application Security API Definition
#include "app_task.h"       // Application Manager API Definitionde 
#include "kernel_timer.h"

#if (BLE_APP_ANCSC)
#include "app_ancsc.h"
#endif //(BLE_APP_ANCSC)

#if (DISPLAY_SUPPORT)
#include "app_display.h"    // Display Application Definitions
#endif //(DISPLAY_SUPPORT)

#if (NVDS_SUPPORT)
#include "nvds.h"           // NVDS API Definitions
#endif //(NVDS_SUPPORT)

#ifdef BLE_APP_AM0
#include "am0_application.h"
#endif // BLE_APP_AM0
#include "common_utils.h"
#include "uart.h"
/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

#define DIRECT_ADV_EN				0  //test

/// Application Security Environment Structure
struct app_sec_env_tag app_sec_env;




bool app_sec_peer_pairing_recv(void);

bool app_sec_peer_encrypt_recv(void);


/*
 * GLOBAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

void app_sec_init(void)
{
    //UART_PRINTF("%s \r\n",__func__);
	#if (NVDS_SUPPORT && DIRECT_ADV_EN)
	uint8_t length = NVDS_LEN_PERIPH_BONDED;
	if (nvds_get(NVDS_TAG_PERIPH_BONDED, &length, (uint8_t *)&app_sec_env.bonded) != NVDS_OK)
	{
		//UART_PRINTF("read app_sec_env.bonded = %x\r\n",app_sec_env.bonded);
		// If read value is invalid, set status to not bonded
		if ((app_sec_env.bonded != true) && (app_sec_env.bonded != false))
		{
			app_sec_env.bonded = false;
		}
	}
	app_sec_env.peer_pairing_recv = false;
	app_sec_env.peer_encrypt_recv = false;	
	#else
	app_sec_env.bonded = false;
	app_sec_env.peer_pairing_recv = false;
	app_sec_env.peer_encrypt_recv = false;	
	#endif
		
}

bool app_sec_get_bond_status(void)
{
    //UART_PRINTF("bonded = %d\r\n",app_sec_env.bonded);
    return app_sec_env.bonded;
}


bool app_sec_peer_pairing_recv(void)
{
	//UART_PRINTF("peer pairing = %d\r\n",app_sec_env.peer_pairing_recv);
	return app_sec_env.peer_pairing_recv;
}


bool app_sec_peer_encrypt_recv(void)
{
	//UART_PRINTF("peer encrypt = %d\r\n",app_sec_env.peer_encrypt_recv);
	return app_sec_env.peer_encrypt_recv;
}


#if (NVDS_SUPPORT )
void app_sec_remove_bond(void)
{
    #if (BLE_APP_HID)
    uint16_t ntf_cfg = PRF_CLI_STOP_NTFIND;
    #endif //(BLE_APP_HID)

    // Check if we are well bonded
    if (app_sec_env.bonded == true)
    {
        // Update the environment variable
        app_sec_env.bonded = false;

        if (nvds_put(NVDS_TAG_PERIPH_BONDED, NVDS_LEN_PERIPH_BONDED,
                     (uint8_t *)&app_sec_env.bonded) != NVDS_OK)
        {
            ASSERT_ERR(0);
        }

        #if (BLE_APP_HID)
        if (nvds_put(NVDS_TAG_MOUSE_NTF_CFG, NVDS_LEN_MOUSE_NTF_CFG,
                     (uint8_t *)&ntf_cfg) != NVDS_OK)
        {
            ASSERT_ERR(0);
        }
        #endif //(BLE_APP_HID)
    }
	app_sec_env.bonded = false;
	app_sec_env.peer_pairing_recv = false;
	app_sec_env.peer_encrypt_recv = false;
}
#endif 

void app_sec_send_security_req(uint8_t conidx)
{
	if(!app_sec_peer_encrypt_recv() && !app_sec_peer_pairing_recv())
	{
		//UART_PRINTF("app_sec_send_security_req\r\n");
		
		// Send security request
	    struct gapc_security_cmd *cmd = KERNEL_MSG_ALLOC(GAPC_SECURITY_CMD,
	                                                 KERNEL_BUILD_ID(TASK_GAPC, conidx), TASK_APP,
	                                                 gapc_security_cmd);

	    cmd->operation = GAPC_SECURITY_REQ;

	    #if (BLE_APP_HID || BLE_APP_HT)
	    //cmd->auth      = GAP_AUTH_REQ_MITM_BOND;
	    cmd->auth      = GAP_AUTH_REQ_NO_MITM_BOND;
	    #elif defined(BLE_APP_AM0)
	    cmd->auth      = GAP_AUTH_REQ_NO_MITM_BOND;
	    #else
	    cmd->auth      = GAP_AUTH_REQ_NO_MITM_NO_BOND;
	    #endif //(BLE_APP_HID || BLE_APP_HT)

	    // Send the message
	    kernel_msg_send(cmd);
	}
	else
	{
		kernel_timer_clear(APP_SEND_SECURITY_REQ,TASK_APP); 
	}
}

/*
 * MESSAGE HANDLERS
 ****************************************************************************************
 */

static int gapc_bond_req_ind_handler(kernel_msg_id_t const msgid,
                                     struct gapc_bond_req_ind const *param,
                                     kernel_task_id_t const dest_id,
                                     kernel_task_id_t const src_id)
{
	//UART_PRINTF("%s \r\n",__func__);
    // Prepare the GAPC_BOND_CFM message
    struct gapc_bond_cfm *cfm = KERNEL_MSG_ALLOC(GAPC_BOND_CFM,
                                             src_id, TASK_APP,
                                             gapc_bond_cfm);

    switch (param->request)
    {
        case (GAPC_PAIRING_REQ):
        {
	    	//UART_PRINTF("gapc_pairing req\r\n");
	    	app_sec_env.peer_pairing_recv = true;
            cfm->request = GAPC_PAIRING_RSP;

            #ifndef BLE_APP_AM0
            cfm->accept  = false;

            // Check if we are already bonded (Only one bonded connection is supported)
            if (!app_sec_env.bonded)
            #endif // !BLE_APP_AM0
            {
                cfm->accept  = true;

                #if (BLE_APP_HID || BLE_APP_HT)
                // Pairing Features
				cfm->data.pairing_feat.auth      = GAP_AUTH_REQ_NO_MITM_BOND;
                #elif defined(BLE_APP_AM0)
                cfm->data.pairing_feat.auth      = GAP_AUTH_REQ_NO_MITM_BOND;
                #else
                cfm->data.pairing_feat.auth      = GAP_AUTH_REQ_NO_MITM_NO_BOND;
                #endif //(BLE_APP_HID || BLE_APP_HT)

                #if (BLE_APP_HT)
                cfm->data.pairing_feat.iocap     = GAP_IO_CAP_DISPLAY_ONLY;
                #else
                cfm->data.pairing_feat.iocap     = GAP_IO_CAP_NO_INPUT_NO_OUTPUT;
                #endif //(BLE_APP_HT)

                cfm->data.pairing_feat.key_size  = 16;
                cfm->data.pairing_feat.oob       = GAP_OOB_AUTH_DATA_NOT_PRESENT;
                cfm->data.pairing_feat.sec_req   = GAP_NO_SEC;
				//cfm->data.pairing_feat.ikey_dist = GAP_KDIST_ENCKEY | GAP_KDIST_IDKEY | GAP_KDIST_SIGNKEY;
                //cfm->data.pairing_feat.rkey_dist = GAP_KDIST_ENCKEY | GAP_KDIST_IDKEY | GAP_KDIST_SIGNKEY;
                
				//cfm->data.pairing_feat.ikey_dist = GAP_KDIST_IDKEY | GAP_KDIST_SIGNKEY;
                //cfm->data.pairing_feat.rkey_dist = GAP_KDIST_ENCKEY;
                cfm->data.pairing_feat.ikey_dist = GAP_KDIST_ENCKEY| GAP_KDIST_IDKEY;
                cfm->data.pairing_feat.rkey_dist = GAP_KDIST_ENCKEY| GAP_KDIST_IDKEY;
				/*	
			#if (defined(BLE_APP_AM0))
				cfm->data.pairing_feat.ikey_dist = GAP_KDIST_IDKEY;
				cfm->data.pairing_feat.rkey_dist = GAP_KDIST_ENCKEY | GAP_KDIST_IDKEY;
			#else
				cfm->data.pairing_feat.ikey_dist = GAP_KDIST_NONE;
				cfm->data.pairing_feat.rkey_dist = GAP_KDIST_ENCKEY;
			#endif // (defined(BLE_APP_AM0))
				*/    
            }
        } break;

        case (GAPC_LTK_EXCH):
        {
					//	UART_PRINTF("gapc_ltk req\r\n");
            // Counter
            uint8_t counter;
            cfm->accept  = true;
            cfm->request = GAPC_LTK_EXCH;

            // Generate all the values
            cfm->data.ltk.ediv = (uint16_t)common_rand_word();

            for (counter = 0; counter < RAND_NB_LEN; counter++)
            {
                cfm->data.ltk.ltk.key[counter]    = (uint8_t)common_rand_word();
                cfm->data.ltk.randnb.nb[counter] = (uint8_t)common_rand_word();
            }

            for (counter = RAND_NB_LEN; counter < KEY_LEN; counter++)
            {
                cfm->data.ltk.ltk.key[counter]    = (uint8_t)common_rand_word();
            }

            #if (NVDS_SUPPORT)
            // Store the generated value in NVDS
            if (nvds_put(NVDS_TAG_LTK, NVDS_LEN_LTK,
                         (uint8_t *)&cfm->data.ltk) != NVDS_OK)
            {
                ASSERT_ERR(0);
            }
			#endif// #if (NVDS_SUPPORT)
        } break;


        case (GAPC_IRK_EXCH):
        {
						//UART_PRINTF("gapc_lrk exch\r\n");

            cfm->accept  = true;
            cfm->request = GAPC_IRK_EXCH;

            // Load IRK
            memcpy(cfm->data.irk.irk.key, app_env.loc_irk, KEY_LEN);
            // load device address
            cfm->data.irk.addr.addr_type = ADDR_PUBLIC;
            memcpy(cfm->data.irk.addr.addr.addr,(uint8_t *)&common_default_bdaddr,BD_ADDR_LEN);
        } break;


        #if (BLE_APP_HT)
        case (GAPC_TK_EXCH):
        {
            // Generate a PIN Code- (Between 100000 and 999999)
            uint32_t pin_code = (100000 + (common_rand_word()%900000));

            #if DISPLAY_SUPPORT
            // Display the PIN Code
            app_display_pin_code(pin_code);
            #endif //DISPLAY_SUPPORT

            cfm->accept  = true;
            cfm->request = GAPC_TK_EXCH;

            // Set the TK value
            memset(cfm->data.tk.key, 0, KEY_LEN);

            cfm->data.tk.key[0] = (uint8_t)((pin_code & 0x000000FF) >>  0);
            cfm->data.tk.key[1] = (uint8_t)((pin_code & 0x0000FF00) >>  8);
            cfm->data.tk.key[2] = (uint8_t)((pin_code & 0x00FF0000) >> 16);
            cfm->data.tk.key[3] = (uint8_t)((pin_code & 0xFF000000) >> 24);
        } break;
        #endif //(BLE_APP_HT)

        default:
        {
            ASSERT_ERR(0);
        } break;
    }

    // Send the message
    kernel_msg_send(cfm);

    return (KERNEL_MSG_CONSUMED);
}


static int gapc_bond_ind_handler(kernel_msg_id_t const msgid,
                                 struct gapc_bond_ind const *param,
                                 kernel_task_id_t const dest_id,
                                 kernel_task_id_t const src_id)
{
    //UART_PRINTF("%s param->info = 0x%x\r\n",__func__,param->info);
    switch (param->info)
    {
        case (GAPC_PAIRING_SUCCEED):
        {
            // Update the bonding status in the environment
            app_sec_env.bonded = true;
	    	   //UART_PRINTF("gapc pairing success\r\n");
		
            // Update the bonding status in the environment
            #if (PLF_NVDS)
            if (nvds_put(NVDS_TAG_PERIPH_BONDED, NVDS_LEN_PERIPH_BONDED,
                         (uint8_t *)&app_sec_env.bonded) != NVDS_OK)
            {
                // An error has occurred during access to the NVDS
                ASSERT_ERR(0);
            }

            // Set the BD Address of the peer device in NVDS
            if (nvds_put(NVDS_TAG_PEER_BD_ADDRESS, NVDS_LEN_PEER_BD_ADDRESS,
                         (uint8_t *)gapc_get_bdaddr(0, SMPC_INFO_PEER)) != NVDS_OK)
            {
                // An error has occurred during access to the NVDS
                ASSERT_ERR(0);
            }
            #endif //(PLF_NVDS)
			
            #if (DISPLAY_SUPPORT)
            // Update the bond status screen value
            app_display_set_bond(app_sec_env.bonded);
            #endif //(DISPLAY_SUPPORT)
            #ifdef BLE_APP_AM0
            am0_app_send_audio_init(KERNEL_IDX_GET(src_id));
            #endif // BLE_APP_AM0
        } break;

        case (GAPC_REPEATED_ATTEMPT):
        {
            appm_disconnect(0x13);
        } break;

        case (GAPC_IRK_EXCH):
        {
					//UART_PRINTF("gapc pairing GAPC_IRK_EXCH\r\n");
           #if (NVDS_SUPPORT)
           // Store peer identity in NVDS
           if (nvds_put(NVDS_TAG_PEER_IRK, NVDS_LEN_PEER_IRK, (uint8_t *)&param->data.irk) != NVDS_OK)
           {
               ASSERT_ERR(0);
           }
           #endif // (NVDS_SUPPORT)
					 
					memcpy(&app_env.peer_irk,&param->data.irk.irk.key[0],sizeof(struct gapc_irk));
					for(int i = 0;i<sizeof(struct gap_sec_key);i++)
					{
						//UART_PRINTF("irk.key[%d]  = %x\r\n",i,param->data.irk.irk.key[i]);
					}
					
					//UART_PRINTF("addr tyep = %x\r\n",param->data.irk.addr.addr_type);
					for(int i = 0;i<sizeof(struct bd_addr);i++)
					{
					//UART_PRINTF("addr.addr[%d]  = %x\r\n",i,param->data.irk.addr.addr.addr[i]);
					}
					 
					 
					 
					 
        } break;

        case (GAPC_PAIRING_FAILED):
        {
	    	//UART_PRINTF("gapc pairing failed\r\n");
	    	app_sec_env.peer_pairing_recv = false;
	    	app_sec_env.peer_encrypt_recv = false;
	    	appm_disconnect(0x13);
        	//app_sec_send_security_req(0);
        } break;

        default:
        {
            ASSERT_ERR(0);
        } break;
    }

    return (KERNEL_MSG_CONSUMED);
}

static int gapc_encrypt_req_ind_handler(kernel_msg_id_t const msgid,
                                        struct gapc_encrypt_req_ind const *param,
                                        kernel_task_id_t const dest_id,
                                        kernel_task_id_t const src_id)
{
	if(kernel_timer_active(APP_SEND_SECURITY_REQ, TASK_APP))
	{
		kernel_timer_clear(APP_SEND_SECURITY_REQ, TASK_APP);
	}
	
	//UART_PRINTF("%s \r\n",__func__);
	app_sec_env.peer_encrypt_recv = true;
    #if (NVDS_SUPPORT)
    // LTK value
    struct gapc_ltk ltk;
    // Length
    uint8_t length = NVDS_LEN_LTK;
    #endif // #if (NVDS_SUPPORT)

    // Prepare the GAPC_ENCRYPT_CFM message
    struct gapc_encrypt_cfm *cfm = KERNEL_MSG_ALLOC(GAPC_ENCRYPT_CFM,
                                                src_id, TASK_APP,
                                                gapc_encrypt_cfm);

    cfm->found    = false;

#if (NVDS_SUPPORT && DIRECT_ADV_EN)
    if (app_sec_env.bonded)  
#else	
    if(1)
#endif	
    {
        #if (NVDS_SUPPORT)
        // Retrieve the required informations from NVDS
        if (nvds_get(NVDS_TAG_LTK, &length, (uint8_t *)&ltk) == NVDS_OK)
        {
            // Check if the provided EDIV and Rand Nb values match with the stored values
            if ((param->ediv == ltk.ediv) &&
                !memcmp(&param->rand_nb.nb[0], &ltk.randnb.nb[0], sizeof(struct rand_nb)))
            {
                cfm->found    = true;
                cfm->key_size = 16;
                memcpy(&cfm->ltk, &ltk.ltk, sizeof(struct gap_sec_key));
            }
            /*
             * else we are bonded with another device, disconnect the link
             */
        }
        else
        {
        	//UART_PRINTF("read ltk fail\r\n");
            ASSERT_ERR(0);
        }
        #endif // #if (NVDS_SUPPORT)

    }
    /*
     * else the peer device is not known, an error should trigger a new pairing procedure.
     */

    // Send the message
    kernel_msg_send(cfm);

    return (KERNEL_MSG_CONSUMED);
}


static int gapc_encrypt_ind_handler(kernel_msg_id_t const msgid,
                                    struct gapc_encrypt_ind const *param,
                                    kernel_task_id_t const dest_id,
                                    kernel_task_id_t const src_id)
{
    // encryption/ re-encryption succeeded
	//UART_PRINTF("%s \r\n",__func__);
    #ifdef BLE_APP_AM0
    am0_app_send_audio_init(KERNEL_IDX_GET(src_id));
    #endif // BLE_APP_AM0

    return (KERNEL_MSG_CONSUMED);
}

static int app_sec_msg_dflt_handler(kernel_msg_id_t const msgid,
                                    void *param,
                                    kernel_task_id_t const dest_id,
                                    kernel_task_id_t const src_id)
{
    // Drop the message

    return (KERNEL_MSG_CONSUMED);
}

/*
 * LOCAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/// Default State handlers definition
const struct kernel_msg_handler app_sec_msg_handler_list[] =
{
    // Note: first message is latest message checked by kernel so default is put on top.
    {KERNEL_MSG_DEFAULT_HANDLER,  (kernel_msg_func_t)app_sec_msg_dflt_handler},

    {GAPC_BOND_REQ_IND,       (kernel_msg_func_t)gapc_bond_req_ind_handler},
    {GAPC_BOND_IND,           (kernel_msg_func_t)gapc_bond_ind_handler},

    {GAPC_ENCRYPT_REQ_IND,    (kernel_msg_func_t)gapc_encrypt_req_ind_handler},
    {GAPC_ENCRYPT_IND,        (kernel_msg_func_t)gapc_encrypt_ind_handler},
};

const struct kernel_state_handler app_sec_table_handler =
    {&app_sec_msg_handler_list[0], (sizeof(app_sec_msg_handler_list)/sizeof(struct kernel_msg_handler))};

#endif //(BLE_APP_SEC)

/// @} APP
