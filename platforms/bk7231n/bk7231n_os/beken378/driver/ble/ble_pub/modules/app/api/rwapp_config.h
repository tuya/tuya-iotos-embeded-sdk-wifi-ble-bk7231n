/**
 ****************************************************************************************
 *
 * @file rwapp_config.h
 *
 * @brief Application configuration definition
 *
 * Copyright (C) RivieraWaves 2009-2016
 *
 ****************************************************************************************
 */


#ifndef _RWAPP_CONFIG_H_
#define _RWAPP_CONFIG_H_

/**
 ****************************************************************************************
 * @addtogroup app
 * @brief Application configuration definition
 *
 * @{
 ****************************************************************************************
 */

#include "rwip_config.h"
/*
 * INCLUDE FILES
 ****************************************************************************************
 */

/*
 * DEFINES
 ****************************************************************************************
 */

/******************************************************************************************/
/* -------------------------   BLE APPLICATION SETTINGS      -----------------------------*/
/* -------------------------    don't change  format       -------------------------------*/
/******************************************************************************************/

// <<< Use Configuration Wizard in Context Menu >>> 

// <h> BLE APPLICATION SETTINGS


// 	<e> BLE_APP_OADS
// 	<i> BLE_APP_OADS enable /disable 
//  </e>
#if ( 0 )
#define CFG_APP_OADS
#endif

// 	<e> BLE_APP_FFF0
// 	<i> BLE_APP_FFF0 enable /disable 
//  </e>
#if ( 0 )
#define CFG_APP_FFF0
#endif


// 	<e> BLE_APP_FFE0
// 	<i> BLE_APP_FFE0 enable /disable 
//  </e>
#if ( 0 )
#define CFG_APP_FFE0
#endif

// 	<e> BLE_APP_COMM
// 	<i> BLE_APP_COMM enable /disable 
//  </e>
#if ( 1 )
#define CFG_APP_COMM
#endif

// 	<e> BLE_APP_SDP
// 	<i> BLE_APP_SDP enable /disable 
//  </e>
#if ( 1 )
#define CFG_APP_SDP
#endif

/// HID Application
// 	<e> BLE_APP_HID
// 	<i> BLE_APP_HID enable /disable 
//  </e>
#if ( 0 )
#define CFG_APP_HID
#endif



// 	<e> BLE_APP_PXPR
// 	<i> BLE_APP_PXPR enable /disable 
//  </e>
#if ( 0 )
#define CFG_APP_PXPR
#endif

// 	<e> BLE_APP_ANCSC
// 	<i> BLE_APP_ANCS enable /disable
//  </e>
#if ( 0 )
#define CFG_APP_ANCSC
#endif


// 	<e> BLE_APP_SEC
// 	<i> BLE_APP_SEC enable /disable
//  </e>
#if ( 0 )
#define CFG_APP_SEC
#endif

// 	<e> BLE_APP_FINDT
// 	<i> BLE_APP_FINDT enable /disable 
//  </e>
#if ( 0 )
#define CFG_APP_FINDT
#endif


// 	<e> BLE_APP_DIS
// 	<i> BLE_APP_DIS enable /disable 
//  </e>
#if ( 0 )
#define CFG_APP_DIS
#endif

// 	<e> CFG_APP_BATT
// 	<i> CFG_APP_BATT enable /disable 
//  </e>
#if ( 0 )
#define CFG_APP_BATT
#endif

/// OAD Service Application
#if defined(CFG_APP_OADS)
#define BLE_APP_OADS          1
#else
#define BLE_APP_OADS          0
#endif // (BLE_APP_FFF0)


/// FFF0 Service Application
#if defined(CFG_APP_FFF0)
#define BLE_APP_FFF0          1
#else
#define BLE_APP_FFF0          0
#endif // (BLE_APP_FFF0)


/// FFE0 Service Application
#if defined(CFG_APP_FFE0)
#define BLE_APP_FFE0          1
#else
#define BLE_APP_FFE0          0
#endif // (BLE_APP_FFF0)

/// COMM Service Application
#if defined(CFG_APP_COMM)
#define BLE_APP_COMM               1
#else
#define BLE_APP_COMM               0
#endif // defined(BLE_APP_COMM)

// SDP Client Application
#if defined(CFG_APP_SDP)
#define BLE_APP_SDP          1
#else
#define BLE_APP_SDP          0
#endif // (CFG_APP_SDP)

/// Proximity  Service Application
#if defined(CFG_APP_PXPR)
#define BLE_APP_PXPR          1
#else
#define BLE_APP_PXPR          0
#endif // (CFG_APP_PXPR)

/// Find me Locator role Application
#if defined(CFG_APP_ANCSC)
#define BLE_APP_ANCSC           1
#else // defined(CFG_APP_ANCSC)
#define BLE_APP_ANCSC           0
#endif // defined(CFG_APP_ANCSC)

/// Find me Target role Application
#if defined(CFG_APP_FINDT)
#define BLE_APP_FINDT           1
#else // defined(CFG_APP_HT)
#define BLE_APP_FINDT           0
#endif // defined(CFG_APP_HT)


/// Find me Locator role Application
#if defined(CFG_APP_FINDL)
#define BLE_APP_FINDL           1
#else // defined(CFG_APP_HT)
#define BLE_APP_FINDL           0
#endif // defined(CFG_APP_HT)


/// Heart Rate Profile Sensor Application
#if defined(CFG_APP_HRPS)
#define BLE_APP_HRPS           1
#else // defined(CFG_APP_HRPS)
#define BLE_APP_HRPS           0
#endif // defined(CFG_APP_HRPS)

/// Health Thermometer Application
#if defined(CFG_APP_HT)
#define BLE_APP_HT           1
#else // defined(CFG_APP_HT)
#define BLE_APP_HT           0
#endif // defined(CFG_APP_HT)

/// HID Application
#if defined(CFG_APP_HID)
#define BLE_APP_HID          1
#else // defined(CFG_APP_HID)
#define BLE_APP_HID          0
#endif // defined(CFG_APP_HID)

/// DIS Application
#if (defined(CFG_APP_DIS) || BLE_APP_HRPS)
#define BLE_APP_DIS          1

#else // defined(CFG_APP_DIS)
#define BLE_APP_DIS          0
#endif // defined(CFG_APP_DIS)

/// Time Application
#if defined(CFG_APP_TIME)
#define BLE_APP_TIME         1
#else // defined(CFG_APP_TIME)
#define BLE_APP_TIME         0
#endif // defined(CFG_APP_TIME)

/// Battery Service Application
//#if (defined(CFG_APP_HID) || defined(CFG_APP_BATT))
#if defined(CFG_APP_BATT)
#define BLE_APP_BATT          1
#else
#define BLE_APP_BATT          0
#endif // (BLE_APP_HID)

/// Security Application
#if (defined(CFG_APP_SEC) || BLE_APP_HID || defined(BLE_APP_AM0))
#define BLE_APP_SEC          1
#else // defined(CFG_APP_SEC)
#define BLE_APP_SEC          0
#endif // defined(CFG_APP_SEC)

#if (BLE_APP_FFE0 || BLE_APP_FFF0 || BLE_APP_COMM)
#define BLE_APP_SERVICE      1
#else
#define BLE_APP_SERVICE      0
#endif

#if (BLE_APP_SDP)
#define BLE_APP_CLIENT       1
#else
#define BLE_APP_CLIENT       0
#endif

/// @} rwapp_config

#endif /* _RWAPP_CONFIG_H_ */
