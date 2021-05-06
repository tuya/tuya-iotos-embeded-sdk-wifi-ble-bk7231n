/**
 ****************************************************************************************
 *
 * @file rwprf_config.h
 *
 * @brief Header file - Profile Configuration
 *
 * Copyright (C) RivieraWaves 2009-2016
 *
 ****************************************************************************************
 */


#ifndef _RWPRF_CONFIG_H_
#define _RWPRF_CONFIG_H_


/**
 ****************************************************************************************
 * @addtogroup PRF_CONFIG
 * @ingroup PROFILE
 * @brief Profile configuration
 *
 * @{
 ****************************************************************************************
 */
#include "rwip_config.h"

//ATT DB,Testing and Qualification related flags
#if (BLE_CENTRAL || BLE_PERIPHERAL)

#if (1)
#define CFG_PRF_COMM
#endif

#if defined(CFG_PRF_COMM)
#define BLE_COMM_SERVER        1
#else
#define BLE_COMM_SERVER        0
#endif

/// BLE_CLIENT_PRF indicates if at least one client profile is present
#if (BLE_PROX_MONITOR || BLE_FINDME_LOCATOR || BLE_HT_COLLECTOR || BLE_BP_COLLECTOR \
        || BLE_HR_COLLECTOR || BLE_DIS_CLIENT || BLE_TIP_CLIENT || BLE_SP_CLIENT \
        || BLE_BATT_CLIENT || BLE_GL_COLLECTOR || BLE_HID_BOOT_HOST || BLE_HID_REPORT_HOST \
        || BLE_RSC_COLLECTOR || BLE_CSC_COLLECTOR || BLE_CP_COLLECTOR || BLE_LN_COLLECTOR || BLE_AN_CLIENT \
        || BLE_PAS_CLIENT || BLE_IPS_CLIENT || BLE_ENV_CLIENT || BLE_WSC_CLIENT \
        || BLE_UDS_CLIENT || BLE_BCS_CLIENT || BLE_WPT_CLIENT || BLE_PLX_CLIENT \
        || BLE_CGM_CLIENT || BLE_DBG_THPP||BLE_ANCS_CLIENT)
#define BLE_CLIENT_PRF          1
#else
#define BLE_CLIENT_PRF          0
#endif //

/// BLE_SERVER_PRF indicates if at least one server profile is present
#if (BLE_COMM_SERVER)
#define BLE_SERVER_PRF          1
#else
#define BLE_SERVER_PRF          0
#endif //BLE_COMM_SERVER

//Force ATT parts depending on profile roles or compile options
/// Attribute Client
#if (BLE_CLIENT_PRF)
#define BLE_ATTC                    1
#endif //(BLE_CLIENT_PRF)

/// Attribute Server
#if (BLE_SERVER_PRF)
#define BLE_ATTS                    1
#endif //(BLE_SERVER_PRF)


#elif (BLE_OBSERVER || BLE_BROADCASTER)
//Force ATT parts to 0
/// External database management
#define BLE_EXT_ATTS_DB             0
/// Profile Server
#define BLE_SERVER_PRF              0
/// Profile Client
#define BLE_CLIENT_PRF              0
#endif //(BLE_OBSERVER || BLE_BROADCASTER)


/// @} PRF_CONFIG

#endif /* _RWPRF_CONFIG_H_ */
