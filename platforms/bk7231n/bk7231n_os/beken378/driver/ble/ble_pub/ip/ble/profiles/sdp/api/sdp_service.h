/**
 ****************************************************************************************
 *
 * @file sdp_service.h
 *
 * @brief Header file - sdp  Role.
 *
 * Copyright (C) Beken 2009-2015
 *
 *
 ****************************************************************************************
 */

#ifndef _SDP_SERVICE_H_
#define _SDP_SERVICE_H_

/**
 ****************************************************************************************
 * @addtogroup SDP_SERVICE  
 * @ingroup SDP
 * @brief SDP Service 
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "rwip_config.h"
#if 1 //(BLE_SDP_CLIENT)

#include <stdint.h>
#include <stdbool.h>
#include "kernel_task.h"
#include "common_utils.h"
#include "prf_types.h"
#include "prf_utils.h"
#include "sdp_service_task.h"

/*
 * DEFINES
 ****************************************************************************************
 */

#define INVALID_STATUS   (0xFFFF)
#define VALID_STATUS     (0xA5A5)
/*
 * ENUMERATIONS
 ****************************************************************************************
 */


/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

struct sdp_db_env
{
	 ///Number of  instances added
		uint8_t prf_nb;
    /// on-going operation
    struct sdp_cnx_env *cnx_env;
    
};

/// Environment variable for each Connections
struct sdp_cnx_env
{   
	
		
    ///SDP characteristics
    struct sdp_content sdp;
	
    ///Number of BAS instances found
};

/// SDP 'Profile' Client environment variable
struct sdp_env_tag
{
    /// profile environment
    prf_env_t prf_env;
	
	uint16_t use_status;
	/// on-going operation
    struct kernel_msg * operation;
	
    /// Environment variable pointer for each connections
    struct sdp_db_env db_env[SDP_IDX_MAX];
    /// State of different task instances
    kernel_state_t state[SDP_IDX_MAX];
};

/*
 * GLOBAL VARIABLE DECLARATIONS
 ****************************************************************************************
 */

extern uint8_t sdp_need_dis_flag;
/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */


/**
 ****************************************************************************************
 * @brief Retrieve BAS client profile interface
 *
 * @return BAS client profile interface
 ****************************************************************************************
 */
const struct prf_task_cbs* sdp_prf_itf_get(void);

/**
 ****************************************************************************************
 * @brief Send ATT DB discovery results to BASC host.
 ****************************************************************************************
 */
 
void sdp_discover_all_service(void);
void sdp_service_init_env(void);
void sdp_add_profiles(struct sdp_db_env *env);
void sdp_extract_svc_info(struct gattc_sdp_svc_ind const *param);
void sdp_enable_rsp_send(struct sdp_env_tag *basc_env, uint8_t conidx, uint8_t status);

#endif /* (BLE_BATT_CLIENT) */

/// @} BASC

#endif /* _BASC_H_ */
