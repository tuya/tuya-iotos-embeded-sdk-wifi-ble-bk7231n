#include "rwip_config.h"

#if (BLE_COMM_SERVER)
#include "attm.h"
#include "comm.h"
#include "comm_task.h"
#include "prf_utils.h"
#include "prf.h"
#include "kernel_mem.h"

static uint8_t bk_ble_service_init (struct prf_task_env* env, uint16_t* start_hdl, uint16_t app_task, uint8_t sec_lvl,  struct bk_ble_db_cfg* params)
{
    uint16_t shdl;
    struct bk_ble_env_tag* ble_env = NULL;
    // Status
    uint8_t status = GAP_ERR_NO_ERROR;
    
    //-------------------- allocate memory required for the profile  ---------------------
    ble_env = (struct bk_ble_env_tag* ) kernel_malloc(sizeof(struct bk_ble_env_tag), KERNEL_MEM_ATT_DB);
    memset(ble_env, 0 , sizeof(struct bk_ble_env_tag));
   
    // Service content flag
    uint16_t cfg_flag =  0xFFFF;
   
    shdl = *start_hdl;

    //Create FFF0 in the DB
    //------------------ create the attribute database for the profile -------------------
    status = attm_svc_create_db_128(&(shdl), params->uuid, (uint8_t *)&cfg_flag,
            params->att_db_nb, NULL, env->task, (struct attm_desc_128 *)params->att_db,
            (sec_lvl & (PERM_MASK_SVC_DIS | PERM_MASK_SVC_AUTH | PERM_MASK_SVC_EKS | PERM_MASK_SVC_UUID_LEN)));
						
    //-------------------- Update profile task information  ---------------------
    if (status == ATT_ERR_NO_ERROR)
    {

        // allocate BASS required environment variable
        env->env = (prf_env_t*) ble_env;
        *start_hdl = shdl;
        ble_env->start_hdl = *start_hdl;
        ble_env->prf_env.app_task = app_task
                | (PERM_GET(sec_lvl, SVC_MI) ? PERM(PRF_MI, ENABLE) : PERM(PRF_MI, DISABLE));
        ble_env->prf_env.prf_task = env->task | PERM(PRF_MI, DISABLE);

        ble_env->att_db_nb = params->att_db_nb;

        // initialize environment variable
        env->id                     = params->prf_task_id;
        comm_task_init(&(env->desc), ble_env->state);

        bk_printf("ble_env->start_hdl = 0x%x\r\n",ble_env->start_hdl);

        // service is ready, go into an Idle state
        kernel_state_set(env->task, BLE_IDLE);
    }
    else if(ble_env != NULL)
    {
        kernel_free(ble_env);
    }
     
    return (status);
}

static void bk_ble_service_destroy(struct prf_task_env* env)
{
    struct bk_ble_env_tag* ble_env = (struct bk_ble_env_tag*) env->env;

    // clear on-going operation
    if(ble_env->operation != NULL)
    {
        kernel_free(ble_env->operation);
    }

    // free profile environment variables
    env->env = NULL;
    kernel_free(ble_env);
}

static void bk_ble_service_create(struct prf_task_env* env, uint8_t conidx)
{
    struct bk_ble_env_tag* ble_env = (struct bk_ble_env_tag*) env->env;
    BLE_ASSERT_ERR(conidx < BLE_CONNECTION_MAX);

    // force notification config to zero when peer device is connected
    ble_env->ntf_cfg[conidx] = 0;
    ble_env->ind_cfg[conidx] = 0;
}


static void bk_ble_service_cleanup(struct prf_task_env* env, uint8_t conidx, uint8_t reason)
{
    struct bk_ble_env_tag* ble_env = (struct bk_ble_env_tag*) env->env;

    BLE_ASSERT_ERR(conidx < BLE_CONNECTION_MAX);
    // force notification config to zero when peer device is disconnected
    ble_env->ntf_cfg[conidx] = 0;
    ble_env->ind_cfg[conidx] = 0;
}

///  Task interface required by profile manager
const struct prf_task_cbs bk_ble_itf =
{
        (prf_init_fnct) bk_ble_service_init,
        bk_ble_service_destroy,
        bk_ble_service_create,
        bk_ble_service_cleanup,
};

const struct prf_task_cbs* bk_ble_prf_itf_get(void)
{
   return &bk_ble_itf;
}

#endif

