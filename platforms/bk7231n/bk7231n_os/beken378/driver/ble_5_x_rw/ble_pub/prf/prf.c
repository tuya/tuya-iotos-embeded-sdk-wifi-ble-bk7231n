/**
 ****************************************************************************************
 *
 * @file prf.c
 *
 * @brief Entry point of profile source file.
 *
 * Used to manage life cycle of profiles
 *
 * Copyright (C) RivieraWaves 2009-2016
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup PRF
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"
#include "rwprf_config.h"
#if (BLE_PROFILES)
#include "rwip.h"
#include "prf.h"
#include "att.h"

#if (BLE_ISO_MODE_0_PROFILE)
extern const struct prf_task_cbs* am0_has_prf_itf_get(void);
#endif // (BLE_ISO_MODE_0_PROFILE)

#if (BLE_MESH)
extern const struct prf_task_cbs* mal_prf_itf_get(void);
#endif // (BLE_MESH)

#if (BLE_COMM_SERVER)
extern const struct prf_task_cbs* bk_ble_prf_itf_get(void);
#endif // (BLE_COMM_SERVER)

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */


/*
 * DEFINES
 ****************************************************************************************
 */

/*
 * MACROS
 ****************************************************************************************
 */


/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */
struct prf_env_tag prf_env;

/*
 * LOCAL FUNCTIONS DEFINITIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Retrieve profile interface
 ****************************************************************************************
 */
static const struct prf_task_cbs * prf_itf_get(uint16_t task_id)
{
    const struct prf_task_cbs* prf_cbs = NULL;
    //uart_printf("~~~~~~~~~~~~~~~~~~~~~~~task id=%d",KERNEL_TYPE_GET(task_id));

	uint16_t id = KERNEL_TYPE_GET(task_id);

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

    switch(id)
    {         	
		#if (BLE_COMM_SERVER)
		case TASK_BLE_ID_COMMON:
			prf_cbs = bk_ble_prf_itf_get();
			break;
		#endif

        #if (BLE_ISO_MODE_0_PROFILE)
        case TASK_BLE_ID_AM0_HAS:
            prf_cbs = am0_has_prf_itf_get();
            break;
        #endif // (BLE_ISO_MODE_0_PROFILE)

        #if (BLE_MESH)
        case TASK_BLE_ID_MESH:
            prf_cbs = mal_prf_itf_get();
            break;
        #endif //(BLE_MESH)

        default: /* Nothing to do */ break;
    }

    return prf_cbs;
}

/*
 * EXPORTED FUNCTIONS DEFINITIONS
 ****************************************************************************************
 */
void prf_init(uint8_t init_type)
{
    uint8_t i;
    uart_printf("!!!!!!init_type=%x\n",init_type);
    for(i = 0; i < BLE_NB_PROFILES ; i++)
    {
        switch (init_type)
        {
            case RWIP_INIT:
            {
                // FW boot profile initialization
                prf_env.prf[i].task = TASK_BLE_GAPC + i +1;
                kernel_task_create(prf_env.prf[i].task, &(prf_env.prf[i].desc));
            }
            break;

            case RWIP_RST:
            {
                // FW boot profile destruction
                // Get Profile API
                const struct prf_task_cbs * cbs = prf_itf_get(prf_env.prf[i].id);

                if(cbs != NULL)
                {
                    // request to destroy profile
                    cbs->destroy(&(prf_env.prf[i]));
                }
                // Request kernel to flush task messages
                kernel_task_msg_flush(KERNEL_TYPE_GET(prf_env.prf[i].task));
            }
            // No break

            case RWIP_1ST_RST:
            {
                // FW boot profile destruction
                prf_env.prf[i].env  = NULL;
                // unregister profile
                prf_env.prf[i].id   = TASK_BLE_ID_INVALID;
                prf_env.prf[i].desc.msg_handler_tab = NULL;
                prf_env.prf[i].desc.state           = NULL;
                prf_env.prf[i].desc.idx_max         = 0;
                prf_env.prf[i].desc.msg_cnt         = 0;
            }
            break;

            default:
            {
                // Do nothing
            }
            break;
        }
    }
}


uint8_t prf_add_profile(struct gapm_profile_task_add_cmd * params, kernel_task_id_t* prf_task)
{
    uint8_t i;
    uint8_t status = GAP_ERR_NO_ERROR;
    // retrieve profile callback
    const struct prf_task_cbs * cbs = prf_itf_get(params->prf_task_id);
    if(cbs == NULL)
    {    
        // profile API not available
        status = GAP_ERR_INVALID_PARAM;
    }

    // check if profile not already present in task list
    if(status == GAP_ERR_NO_ERROR)
    {      
        for(i = 0; i < BLE_NB_PROFILES ; i++)
        {
            if(prf_env.prf[i].id == params->prf_task_id)
            {
                status = GAP_ERR_NOT_SUPPORTED;
                break;
            }
        }
    }

    if(status == GAP_ERR_NO_ERROR)
    {
        // find first available task
        for(i = 0; i < BLE_NB_PROFILES ; i++)
        {
            // available task found
            if(prf_env.prf[i].id == TASK_BLE_ID_INVALID)
            {
                // initialize profile
                status = cbs->init(&(prf_env.prf[i]), &(params->start_hdl), params->app_task, params->sec_lvl, params->param);

                // initialization succeed
                if(status == GAP_ERR_NO_ERROR)
                {
                    // register profile
                    prf_env.prf[i].id = params->prf_task_id;
                    *prf_task = prf_env.prf[i].task;
                }
                break;
            }
        }

        if(i == BLE_NB_PROFILES)
        {
            status = GAP_ERR_INSUFF_RESOURCES;
        }
    }

    return (status);
}



void prf_create(uint8_t conidx)
{
    uint8_t i;
    /* simple connection creation handler, nothing to do. */

    // execute create function of each profiles
    for(i = 0; i < BLE_NB_PROFILES ; i++)
    {
        // Get Profile API
        const struct prf_task_cbs * cbs = prf_itf_get(prf_env.prf[i].id);
        if(cbs != NULL)
        {
            // call create callback
            cbs->create(&(prf_env.prf[i]), conidx);
        }
    }
}


void prf_cleanup(uint8_t conidx, uint8_t reason)
{
    uint8_t i;
    /* simple connection creation handler, nothing to do. */

    // execute create function of each profiles
    for(i = 0; i < BLE_NB_PROFILES ; i++)
    {
        // Get Profile API
        const struct prf_task_cbs * cbs = prf_itf_get(prf_env.prf[i].id);
        if(cbs != NULL)
        {
            // call cleanup callback
            cbs->cleanup(&(prf_env.prf[i]), conidx, reason);
        }
    }
}


prf_env_t* prf_env_get(uint16_t prf_id)
{
    prf_env_t* env = NULL;
    uint8_t i;
    // find if profile present in profile tasks
    for(i = 0; i < BLE_NB_PROFILES ; i++)
    {
        // check if profile identifier is known
        if(prf_env.prf[i].id == prf_id)
        {
            env = prf_env.prf[i].env;
            break;
        }
    }

    return env;
}

kernel_task_id_t prf_src_task_get(prf_env_t* env, uint8_t conidx)
{
    kernel_task_id_t task = PERM_GET(env->prf_task, PRF_TASK);

    if(PERM_GET(env->prf_task, PRF_MI))
    {
        task = KERNEL_BUILD_ID(task, conidx);
    }

    return task;
}

kernel_task_id_t prf_dst_task_get(prf_env_t* env, uint8_t conidx)
{
    kernel_task_id_t task = PERM_GET(env->app_task, PRF_TASK);

    if(PERM_GET(env->app_task, PRF_MI))
    {
        task = KERNEL_BUILD_ID(task, conidx);
    }

    return task;
}


kernel_task_id_t prf_get_id_from_task(kernel_msg_id_t task)
{
    kernel_task_id_t id = TASK_BLE_ID_INVALID;
    uint8_t idx = KERNEL_IDX_GET(task);
    uint8_t i;
    task = KERNEL_TYPE_GET(task);

    // find if profile present in profile tasks
    for(i = 0; i < BLE_NB_PROFILES ; i++)
    {
        // check if profile identifier is known
        if(prf_env.prf[i].task == task)
        {
            id = prf_env.prf[i].id;
            break;
        }
    }

    return KERNEL_BUILD_ID(id, idx);
}

kernel_task_id_t prf_get_task_from_id(kernel_msg_id_t id)
{
    kernel_task_id_t task = TASK_BLE_NONE;
    uint8_t idx = KERNEL_IDX_GET(id);
    uint8_t i;
    id = KERNEL_TYPE_GET(id);

    // find if profile present in profile tasks
    for(i = 0; i < BLE_NB_PROFILES ; i++)
    {
        // check if profile identifier is known
        if(prf_env.prf[i].id == id)
        {
            task = prf_env.prf[i].task;
            break;
        }
    }

    return KERNEL_BUILD_ID(task, idx);
}

#if 0
struct prf_env_t * prf_env_get_from_uuid(uint8_t conidx,uint8_t uuid_len,const uint8_t *uuid)
{
	struct sdp_env_tag* sdp_env;
	struct prf_env_t* env = NULL;
    uint8_t i;
	uint8_t j;
	 
    // find if profile present in profile tasks
    for(i = 0; i < BLE_NB_PROFILES ; i++)
    {	
		//UART_PRINTF("prf_env.prf[%d].id = 0x%x \r\n",i,prf_env.prf[i].id);
		if(prf_env.prf[i].role == PRF_CLIENT)
		{
			sdp_env = (struct sdp_env_tag *) prf_env.prf[i].env;
            if(sdp_env->conidx == conidx)
            {           
                // check if profile identifier is known
                for(j = 0;j < sdp_env->prf_db_env->sdp_cont->chars_nb;j++)
                {
                    if(memcmp(sdp_env->prf_db_env->sdp_cont->chars_descs_inf.chars_inf[j].uuid,uuid,uuid_len) == 0)
                    {
                        env = (struct prf_env_t*) prf_env.prf[i].env;
                        sdp_env->prf_db_env->sdp_cont->char_idx = j;
                        return env;
                    }
                }
            }
			
		}    
    }
   return env;	 
}
struct prf_env_t * prf_env_get_from_handle(uint8_t conidx,uint16_t handle)
{
	struct sdp_env_tag* sdp_env;
	struct prf_env_t* env = NULL;
    uint8_t i;
	uint8_t j;
	 
    // find if profile present in profile tasks
    for(i = 0; i < BLE_NB_PROFILES ; i++)
    {	
		//UART_PRINTF("prf_env.prf[%d].id = 0x%x \r\n",i,prf_env.prf[i].id);
		if(prf_env.prf[i].role == PRF_CLIENT)
		{
			sdp_env = (struct sdp_env_tag *) prf_env.prf[i].env;
            if(sdp_env->conidx == conidx)
            {           
                // check if profile identifier is known
                for(j = 0;j < sdp_env->prf_db_env->sdp_cont->chars_nb;j++)
                {
                    if((sdp_env->prf_db_env->sdp_cont->chars_descs_inf.chars_inf[j].char_hdl == handle) || (sdp_env->prf_db_env->sdp_cont->chars_descs_inf.chars_inf[j].val_hdl == handle))
                    {
                        env = (struct prf_env_t*) prf_env.prf[i].env;
                        sdp_env->prf_db_env->sdp_cont->char_idx = j;
                        return env;
                    }
                }
                
                // check if profile identifier is known
                for(j = 0;j < sdp_env->prf_db_env->sdp_cont->descs_nb;j++)
                {
                    if(sdp_env->prf_db_env->sdp_cont->chars_descs_inf.descs_inf[j].desc_hdl == handle)
                    {
                        env = (struct prf_env_t*) prf_env.prf[i].env;
                        sdp_env->prf_db_env->sdp_cont->char_idx = j;
                        return env;
                    }
                }
            }
   
  }    
    }

   return env;
	 
}
#endif
#endif // (BLE_PROFILES)

/// @} PRF
