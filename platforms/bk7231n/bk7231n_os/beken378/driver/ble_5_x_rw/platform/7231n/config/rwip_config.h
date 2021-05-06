/**
 ****************************************************************************************
 *
 * @file rwip_config.h
 *
 * @brief Configuration of the RW IP SW
 *
 * Copyright (C) RivieraWaves 2009-2015
 *
 *
 ****************************************************************************************
 */

#ifndef RWIP_CONFIG_H_
#define RWIP_CONFIG_H_

/**
 ****************************************************************************************
 * @addtogroup ROOT
 * @{
 *
 *  Information about RW SW IP options and flags
 *
 *        BT_DUAL_MODE             BT/BLE Dual Mode
 *        BT_STD_MODE              BT Only
 *        BLE_STD_MODE             BLE Only
 *
 *        BT_EMB_PRESENT           BT controller exists
 *        BLE_EMB_PRESENT          BLE controller exists
 *        BLE_HOST_PRESENT         BLE host exists
 *
 * @name RW Stack Configuration
 * @{
 ****************************************************************************************
 */
#include "architect.h"
#include "ble.h"
#include "user_config.h"
/*
 * DEFINES
 ****************************************************************************************
 */

/******************************************************************************************/
/* --------------------------   GENERAL SETUP       --------------------------------------*/
/* -------------------------    don't change  format       -------------------------------*/
/******************************************************************************************/

// <<< Use Configuration Wizard in Context Menu >>>

// <h>  CW_CFG_RW_IP_VALUE
//   <e> CFG_APP
//   </e>
#if ( 1 )
  #define CFG_APP
#endif

//   <o> CFG_BLE Role
// 		<0=> BROADCASTER
// 		<1=> OBSERVER
// 		<2=> PERIPHERAL
// 		<3=> CENTRAL
// 		<4=> ALLROLES
//    <i> Select Role
#define CFG_ROLE 4

#if ( CFG_ROLE == 0)
  #define CFG_BROADCASTER
#endif

#if ( CFG_ROLE == 1)
  #define CFG_OBSERVER
#endif

#if ( CFG_ROLE == 2)
  #define CFG_PERIPHERAL
#endif

#if ( CFG_ROLE == 3)
  #define CFG_CENTRAL
#endif

#if ( CFG_ROLE == 4)
  #define CFG_ALLROLES
#endif

//   <e> CFG_EMB
//   <i> enable Exchange Memeory
//   </e>
#if ( 1 )
  #define CFG_EMB
#endif

//   <e> CFG_BT
//   	<i> BT MODE ENABLE
//   </e>
#if ( 0 )
  #define CFG_BT
#endif

//   <e> CFG_BLE
//   	<i> BLE MODE ENABLE
//   </e>
#if ( 1 )
  #define CFG_BLE
#endif

//   <e> CFG_BLE_2MBPS
//   	<i> select BLE at 2MBPS Mode
//   </e>
#if ( 1 )
  #define CFG_BLE_2MBPS
#endif

//   <e> CFG_CHNL_ASSESS
//   <i> select BLE CHNL_ASSESS
//   </e>
#if ( 0 )
  #define CFG_CHNL_ASSESS
#endif

//   <e> CFG_SEC_CON
//   	<i> select BLE SEC CON
//   </e>
#if ( 0 )
  #define CFG_SEC_CON
#endif

//   <e> CFG_HOST
//   	<i> enable BLE HOST
//   </e>
#if ( 1 )
  #define CFG_HOST
#endif

//   <e> CFG_AHITL
//   	<i> enable Application Host Interface
//   </e>
#if ( 0 )
  #define CFG_AHITL
#endif

//   <e> CFG_HCITL
//   	<i> enable Host Controller Interface Support
//   </e>
#if ( 1 )
  #define CFG_HCITL
#endif

//   <e> CFG_HW_AUDIO
//   	<i> enable HW_AUDIO
//   </e>
#if ( 0 )
  #define CFG_HW_AUDIO
#endif

//   <e> CFG_NVDS
//   	<i> enable NVDS
//   </e>
#if ( 0 )
  #define CFG_NVDS
#endif

//   <e> CFG_PRF
//   	<i> enable PRF
//   </e>
#if ( 1 )
  #define CFG_PRF
#endif

//  	 <o>  CFG_NB_PRF <0-10>
//  	 <i>  CFG_NB_PRF  (0 -- 10)
#define CFG_NB_PRF			10

//   <e> CFG_BLE_TESTER(DUT)
//   	<i> enable BLE_TESTER
//   </e>
#if ( 0 )
  #define CFG_BLE_TESTER
#endif

//   <e> CFG_SLEEP
//   <i> enable DEEP_SLEEP
//   </e>
#if ( 1 )
  #define CFG_SLEEP
#endif

#if ( 1 )
  #define CFG_GAIA
#endif


//	 <o> CFG_CON <1-10>
//   <i> CFG MAX CONNECT NUM (1 -- 10)
#define CFG_CON			1

//	 <o> CFG_RAL <1-10>
//   <i> CFG NUMBER OF DEVICE IN RAL (1 -- 10)
#define CFG_RAL			4

//	 <o> CFG_ACT <1-10>
//   <i> CFG NUMBER OF ACTIVITIES IN BLE SIMULTANEOUS (1 -- 10)
#define CFG_ACT			3

//	 <o> CFG_CON_SCO <0-2>
//   <i> CFG NUMBER OF SYNCHRONOUS CONNECTIONS (0 -- 2)
#define CFG_CON_SCO			0

//   <e> CFG_BLE_MESH
//   <i> enable CFG_BLE_MESH
//   </e>
#if ( 0 )
  #define CFG_BLE_MESH
#endif

// </h>

#define CFG_ECC_P256_ACC	0
#define CFG_BLE_PER_SYNC	0
#define CFG_BLE_PER_ADV		0
#define CFG_BLE_MASTER      0
#define CFG_BLE_EXT_ADV		1
#define CFG_BLE_EXT_SCAN	1


/////POKI Test
// <h> Config User App/Test function
// <e> CFG_USER_APP
// <i> enable CFG_USER_APP
// </e>
#if ( 1 )
	#define CFG_USER_APP
#endif
// </h>

#ifdef CFG_BLE_TESTER
	#define BLE_DUT_TEST 0
#else
	#define BLE_DUT_TEST 0
#endif

/******************************************************************************************/
/* --------------------------   GENERAL SETUP       --------------------------------------*/
/* -------------------------    don't change  format       -------------------------------*/
/******************************************************************************************/

/// Flag indicating if stack is compiled in dual or single mode
#if defined(CFG_BT)
#define BT_EMB_PRESENT              1
#else // defined(CFG_BT)
#define BT_EMB_PRESENT              0
#endif // defined(CFG_BT)

#if defined(CFG_BLE) && defined(CFG_EMB)
#define BLE_EMB_PRESENT             1
#else // defined(CFG_BLE) && defined(CFG_EMB)
#define BLE_EMB_PRESENT             0
#endif // defined(CFG_BLE) && defined(CFG_EMB)

#if defined(CFG_BLE) && defined(CFG_HOST)
#define BLE_HOST_PRESENT            1
#else // defined(CFG_BLE) && defined(CFG_HOST)
#define BLE_HOST_PRESENT            0
#endif // defined(CFG_BLE) && defined(CFG_HOST)

#if defined(CFG_BLE) && defined(CFG_APP)
#define BLE_APP_PRESENT             1
#else // defined(CFG_BLE) && defined(CFG_APP)
#define BLE_APP_PRESENT             0
#endif // defined(CFG_BLE) && defined(CFG_APP)

#define BT_DUAL_MODE                (BT_EMB_PRESENT && BLE_EMB_PRESENT)
#define BT_STD_MODE                 (BT_EMB_PRESENT)
#define BLE_STD_MODE                (BLE_EMB_PRESENT)
#define HCI_PRESENT                 1

/// Flag indicating that Mesh is supported
#if defined(CFG_BLE_MESH)
#define BLE_MESH      1
#else  // !defined(CFG_BLE_MESH)
#define BLE_MESH      0
#endif // defined(CFG_BLE_MESH)

/******************************************************************************************/
/* -------------------------   INTERFACES DEFINITIONS      -------------------------------*/
/******************************************************************************************/

/// Application Host Interface
#if defined(CFG_AHITL)
#define AHI_TL_SUPPORT       1
#else // defined(CFG_AHITL)
#define AHI_TL_SUPPORT       0
#endif // defined(CFG_AHITL)


/// Host Controller Interface Support (defines if HCI parser is present or not)
#if defined(CFG_HCITL)
#define HCI_TL_SUPPORT      1
#else //defined(CFG_HCITL)
#define HCI_TL_SUPPORT      0
#endif //defined(CFG_HCITL)

#if BLE_HOST_PRESENT
#define H4TL_SUPPORT      ((AHI_TL_SUPPORT) + (HCI_TL_SUPPORT))
#else // !BLE_HOST_PRESENT
#define H4TL_SUPPORT      (HCI_TL_SUPPORT)
#endif // BLE_HOST_PRESENT

/// Number of HCI commands the stack can handle simultaneously
#define HCI_NB_CMD_PKTS   (5 * HCI_TL_SUPPORT)

/******************************************************************************************/
/* --------------------------   BLE COMMON DEFINITIONS      ------------------------------*/
/******************************************************************************************/
/// Kernel Heap memory sized reserved for allocate dynamically connection environment
#define KERNEL_HEAP_MEM_RESERVED        (4)

#if defined(CFG_BLE)

/// Broadcaster
#if (defined(CFG_BROADCASTER) || defined(CFG_PERIPHERAL) || defined(CFG_ALLROLES))
#define BLE_BROADCASTER      1
#else
#define BLE_BROADCASTER      0
#endif // (defined(CFG_BROADCASTER) || defined(CFG_PERIPHERAL) || defined(CFG_ALLROLES))

/// Observer
#if (defined(CFG_OBSERVER) || defined(CFG_CENTRAL) || defined(CFG_ALLROLES))
#define BLE_OBSERVER      1
#else
#define BLE_OBSERVER      0
#endif // (defined(CFG_OBSERVER) || defined(CFG_CENTRAL) || defined(CFG_ALLROLES))

/// Central
#if (defined(CFG_CENTRAL) || defined(CFG_ALLROLES))
#define BLE_CENTRAL      1
#else
#define BLE_CENTRAL      0
#endif // (defined(CFG_CENTRAL) || defined(CFG_ALLROLES))

/// Peripheral
#if (defined(CFG_PERIPHERAL) || defined(CFG_ALLROLES))
#define BLE_PERIPHERAL      1
#else
#define BLE_PERIPHERAL      0
#endif // (defined(CFG_PERIPHERAL) || defined(CFG_ALLROLES))

#if ((BLE_BROADCASTER+BLE_OBSERVER+BLE_PERIPHERAL+BLE_CENTRAL) == 0)
    #error "No application role defined"
#endif // ((BLE_BROADCASTER+BLE_OBSERVER+BLE_PERIPHERAL+BLE_CENTRAL) == 0)

/// Maximum number of devices in RAL
#define BLE_RAL_MAX          (CFG_RAL)

/// Maximum number of simultaneous BLE activities (scan, connection, advertising, initiating)
#define BLE_ACTIVITY_MAX          (CFG_ACT)

#if (BLE_HOST_PRESENT)
/// Maximum number of simultaneous connections
#if (BLE_CENTRAL || BLE_PERIPHERAL)
    #define BLE_CONNECTION_MAX      (CFG_CON)
#else
    #define BLE_CONNECTION_MAX      (0)
#endif /* #if (BLE_CENTRAL || BLE_PERIPHERAL) */

#if (BLE_CONNECTION_MAX >= BLE_ACTIVITY_MAX)
    #error "Number of connections must be strictly less than number of activities"
#endif // (BLE_CONNECTION_MAX >= BLE_ACTIVITY_MAX)
#endif // (BLE_HOST_PRESENT)

/// Max advertising reports before sending the info to the host
#define BLE_ADV_REPORTS_MAX              (1)

#if (BLE_EMB_PRESENT)
/// Maximum number of ADV reports in the HCI queue to Host
#define BLE_MAX_NB_ADV_REP_FRAG       (4 * HCI_TL_SUPPORT)
/// Maximum number of IQ reports in the HCI queue to Host
#define BLE_MAX_NB_IQ_REP             (4 * HCI_TL_SUPPORT)
#endif // (BLE_EMB_PRESENT)
#endif //defined(CFG_BLE)


/******************************************************************************************/
/* --------------------------   Direction finding (AoA/AoD)      -------------------------*/
/******************************************************************************************/

#if defined(CFG_CON_CTE_REQ)
    #define BLE_CON_CTE_REQ                (1)
#else // defined(CFG_CON_CTE_REQ)
    #define BLE_CON_CTE_REQ                (0)
#endif // defined(CFG_CON_CTE_REQ)

#if defined(CFG_CON_CTE_RSP)
    #define BLE_CON_CTE_RSP                (1)
#else // defined(CFG_CON_CTE_RSP)
    #define BLE_CON_CTE_RSP                (0)
#endif // defined(CFG_CON_CTE_RSP)

#if defined(CFG_CONLESS_CTE_TX)
    #define BLE_CONLESS_CTE_TX             (1)
#else // defined(CFG_CONLESS_CTE_TX)
    #define BLE_CONLESS_CTE_TX             (0)
#endif // defined(CFG_CONLESS_CTE_TX)

#if defined(CFG_CONLESS_CTE_RX)
    #define BLE_CONLESS_CTE_RX             (1)
#else // defined(CFG_CONLESS_CTE_RX)
    #define BLE_CONLESS_CTE_RX             (0)
#endif // defined(CFG_CONLESS_CTE_RX)

#if defined(CFG_AOD)
    #define BLE_AOD                        (1)
#else // defined(CFG_AOD)
    #define BLE_AOD                        (0)
#endif // defined(CFG_AOD)

#if defined(CFG_AOA)
    #define BLE_AOA                        (1)
#else // defined(CFG_AOA)
    #define BLE_AOA                        (0)
#endif // defined(CFG_AOA)

/******************************************************************************************/
/* --------------------------   ISOCHRONOUS CONFIGURATION        -------------------------*/
/******************************************************************************************/

// check if isochronous is enabled or not
#if  (defined(CFG_ISO_CON) && (CFG_ISO_CON > 0) && (defined(CFG_ISO_MODE_0)))
    #define BLE_ISO_PRESENT                (1)
#else // !(defined(CFG_ISO_CON) && (CFG_ISO_CON > 0))
    #define BLE_ISO_PRESENT                (0)
#endif // (defined(CFG_ISO_CON) && (CFG_ISO_CON > 0))

//  *** Definition of supported isochronous mode ***

// Isochronous Mode 0 - Proprietary mode
#if  (BLE_ISO_PRESENT && defined(CFG_ISO_MODE_0))
    #define BLE_ISO_MODE_0                (BLE_CENTRAL | BLE_PERIPHERAL)
    #define BLE_ISO_MODE_0_PROTOCOL       (BLE_ISO_MODE_0 & BLE_HOST_PRESENT)
    #define BLE_ISO_MODE_0_PROFILE        (BLE_ISO_MODE_0 & BLE_HOST_PRESENT)
    #define BLE_RSA                       (BLE_ISO_MODE_0_PROFILE)
#else
    #define BLE_ISO_MODE_0                (0)
    #define BLE_ISO_MODE_0_PROTOCOL       (0)
    #define BLE_ISO_MODE_0_PROFILE        (0)
    #define BLE_RSA                       (0)
#endif // (BLE_ISO_PRESENT && defined(CFG_AUDIO))

#if (BLE_ISO_PRESENT)
    /// Maximum number of ISO channel / streams
    #define BLE_ISO_CHANNEL_MAX      (CFG_ISO_CON)
    #define BLE_ISO_STREAM_MAX       (CFG_ISO_CON)

    /// Maximum number of octets that can be received/transmitted over Isochronous channels
    #define BLE_MAX_ISO_OCTETS  (251) // number of octets

    /// Define number of ISO TX/RX buffers per isochronous channel
    #define BLE_NB_ISO_BUFF_PER_CHAN            (4)

    /// Define number of ISO descriptors per isochronous channel
    /// Must be equal to max(BLE_NB_ISODESC_PER_ICL_CHAN, BLE_NB_RX_ISODESC_PER_ICO_CHAN + BLE_NB_TX_ISODESC_PER_ICO_CHAN)
    #define BLE_NB_ISODESC_PER_CHAN             (4)

    /// Number of ISO Descriptors - one descriptor required for update sub-event: 1 per stream
    #define BLE_ISO_DESC_NB                     ((BLE_ISO_CHANNEL_MAX * (BLE_NB_ISODESC_PER_CHAN)) + BLE_ISO_STREAM_MAX)
    /// Number of ISO buffers
    #define BLE_ISO_BUF_NB                      (BLE_ISO_CHANNEL_MAX * BLE_NB_ISO_BUFF_PER_CHAN)
#endif // (BLE_ISO_PRESENT)


/// Check status of Isochronous Data path drivers

/// Proprietary ISO over HCI
#if defined(CFG_ISOOHCI)
    #define BLE_ISOOHCI                  (BLE_ISO_PRESENT)
#else
    #define BLE_ISOOHCI                  (0)
#endif

/// Internal ISO generator for validation purpose
#if defined(CFG_ISOGEN)
    #define BLE_ISOGEN                   (BLE_ISO_PRESENT)
#else
    #define BLE_ISOGEN                   (0)
#endif

/// Platform PCM
#if defined(CFG_PCM)
    #define BLE_ISO_PCM                  (BLE_ISO_PRESENT)
#else // !defined(CFG_PCM)
    #define BLE_ISO_PCM                  (0)
#endif // defined(CFG_PCM)

/******************************************************************************************/
/* --------------------------   GPIO SETUP        -------------------------------------*/
/******************************************************************************************/

/// Display controller enable/disable
#if defined(CFG_GPIO)
#define GPIO_SUPPORT      1
#else
#define GPIO_SUPPORT      0
#endif //CFG_GPIO

/******************************************************************************************/
/* --------------------------      RTC SETUP         -------------------------------------*/
/******************************************************************************************/

/// RTC enable/disable
#if defined(CFG_RTC)
#define RTC_SUPPORT      1
#else
#define RTC_SUPPORT      0
#endif //CFG_RTC

/******************************************************************************************/
/* --------------------------      PS2 SETUP         -------------------------------------*/
/******************************************************************************************/

/// PS2 enable/disable
#if defined(CFG_PS2)
#define PS2_SUPPORT      1
#else
#define PS2_SUPPORT      0
#endif //CFG_PS2

/******************************************************************************************/
/* --------------------------      GAIA SETUP        -------------------------------------*/
/******************************************************************************************/

/// GAIA enable/disable
#if defined(CFG_GAIA)
#define GAIA_SUPPORT      1
#else
#define GAIA_SUPPORT      0
#endif //CFG_GAIA

/******************************************************************************************/
/* --------------------------      TRACER SETUP      -------------------------------------*/
/******************************************************************************************/

/// tracer enable/disable
#if defined(CFG_TRC_EN)
    #define TRACER_PRESENT                   1
    #include "dbg_trc_config.h"
#else
    #define TRACER_PRESENT                   0
#endif // CFG_TRC_EN

/******************************************************************************************/
/* -------------------------   DEEP SLEEP SETUP      -------------------------------------*/
/******************************************************************************************/

/// Use 32K Hz Clock if set to 1 else 32,768k is used
#define HZ32000                                     (GAIA_SUPPORT)

/// Time to wake-up Radio Module (in us)
#define SLEEP_RM_WAKEUP_DELAY                       625
/// Time for stabilization of the high frequency oscillator following a sleep-timer expiry (in us)
#define SLEEP_OSC_NORMAL_WAKEUP_DELAY               5000
/// Time for stabilization of the high frequency oscillator following an external wake-up request (in us)
#define SLEEP_OSC_EXT_WAKEUP_DELAY                  5000

/******************************************************************************************/
/* --------------------------   BASEBAND SETUP       -------------------------------------*/
/******************************************************************************************/

/// Default programming delay, margin for programming the baseband in advance of each activity (in half-slots)
#define IP_PROG_DELAY_DFT  (6)

/**
 * Prefetch time (in us)
 *  - Radio power up: 110us (worst case)
 *  - EM fetch: 30us (worst case at 26Mhz)
 *  - HW logic: 10us (worst case at 26Mhz)
 */
#define IP_PREFETCH_TIME_US       (150)

/**
 * Prefetch Abort time (in us)
 *
 * - EM fetch:
 *    - HW CS Update is 18 access
 *    - HW Tx Desc Update is 1 access
 *    - HW Rx Desc Update is 5 access
 *        => EM update at 26MHz Tx, Rx and CS is (18+1+5)*0.04*4 = 4us
 * - HW logic: 10us (worst case)
 * - Radio power down: 26 us for Ripple
 *
 * Prefetch abort time = prefetch time + 4 + 10 + 26
 */
#define IP_PREFETCHABORT_TIME_US  (190)

/******************************************************************************************/
/* --------------------------   RADIO SETUP       ----------------------------------------*/
/******************************************************************************************/

/// Power control features
#define RF_TXPWR                            1
/// Class of device
#define RF_CLASS1                           0


/******************************************************************************************/
/* --------------------------      BEKEN TEST SETUP  -------------------------------------*/
/******************************************************************************************/
#if defined(CFG_USER_APP)
    #define BEKEN_TASK_ENABLE                1
#else
    #define BEKEN_TASK_ENABLE                0
#endif // CFG_USER_APP



/******************************************************************************************/
/* -------------------------   SUPPORTED RADIO PHY    ------------------------------------*/
/******************************************************************************************/

#if defined(CFG_RF_ATLAS)
#define BLE_PHY_1MBPS_SUPPORT                       1
#define BLE_PHY_2MBPS_SUPPORT                       1
#define BLE_PHY_CODED_SUPPORT                       1
#define BLE_STABLE_MOD_IDX_TX_SUPPORT               0
#define BLE_STABLE_MOD_IDX_RX_SUPPORT               0
#elif defined(CFG_RF_BTIPT)
#define BLE_PHY_1MBPS_SUPPORT                       1
#define BLE_PHY_2MBPS_SUPPORT                       1
#define BLE_PHY_CODED_SUPPORT                       1
#define BLE_STABLE_MOD_IDX_TX_SUPPORT               0
#define BLE_STABLE_MOD_IDX_RX_SUPPORT               0
#elif defined(CFG_RF_EXTRC)
#define BLE_PHY_1MBPS_SUPPORT                       1
#define BLE_PHY_2MBPS_SUPPORT                       1
#define BLE_PHY_CODED_SUPPORT                       1
#define BLE_STABLE_MOD_IDX_TX_SUPPORT               0
#define BLE_STABLE_MOD_IDX_RX_SUPPORT               0
#else // RIPPLE
#define BLE_PHY_1MBPS_SUPPORT                       1
#define BLE_PHY_2MBPS_SUPPORT                       0
#define BLE_PHY_CODED_SUPPORT                       1
#define BLE_STABLE_MOD_IDX_TX_SUPPORT               0
#define BLE_STABLE_MOD_IDX_RX_SUPPORT               0
#endif

/******************************************************************************************/
/* -------------------------   COEXISTENCE SETUP      ------------------------------------*/
/******************************************************************************************/

/// WLAN Coexistence
#if defined(CFG_WLAN_COEX)
    #define RW_WLAN_COEX                 1
    #define RW_WLAN_COEX_TEST            (defined(CFG_WLAN_COEX_TEST))
#else
    #define RW_WLAN_COEX                 0
    #define RW_WLAN_COEX_TEST            0
#endif // defined(CFG_WLAN_COEX)

/// MWS Coexistence
#if defined(CFG_MWS_COEX)
    #define RW_MWS_COEX                 1
    #define RW_MWS_COEX_TEST            (defined(CFG_MWS_COEX_TEST))
#else
    #define RW_MWS_COEX                 0
    #define RW_MWS_COEX_TEST            0
#endif // defined(CFG_MWS_COEX)

/******************************************************************************************/
/* -----------------------   SLOT AVAILABILITY MASKS   -----------------------------------*/
/******************************************************************************************/

/// Maximum support peer SAM map size
#define RW_MAX_PEER_SAM_MAP_SLOTS      (256)
#define RW_PEER_SAM_MAP_MAX_LEN      (RW_MAX_PEER_SAM_MAP_SLOTS/4) // 2-bit field per slot

/******************************************************************************************/
/* --------------------   SECURE CONNECTIONS SETUP  --------------------------------------*/
/******************************************************************************************/
#if defined(CFG_ECC_16_BITS_ALGO)
#define ECC_MULT_ALGO_TYPE        (16)
#else // !defined(CFG_ECC_16_BITS_ALGO)
#define ECC_MULT_ALGO_TYPE        (32)
#endif // defined(CFG_ECC_16_BITS_ALGO)
#if defined(CFG_CRYPTO_UT)
#define CRYPTO_UT                 (1)
#else //defined(CFG_CRYPTO_UT)
#define CRYPTO_UT                 (0)
#endif //defined(CFG_CRYPTO_UT)

/******************************************************************************************/
/* --------------------------   DEBUG SETUP       ----------------------------------------*/
/******************************************************************************************/

/// Flag indicating if debug mode is activated or not
#if defined(CFG_BLE_DBG)
    #define RW_DEBUG                        ((BLE_EMB_PRESENT) || (BT_EMB_PRESENT) || (BLE_HOST_PRESENT))
#if (BLE_EMB_PRESENT || BT_EMB_PRESENT)
    #define RW_SWDIAG                       1
#else
    #define RW_SWDIAG                       0
#endif
    #define KERNEL_PROFILING                    1
#else
    #define RW_DEBUG                        0
    #define RW_SWDIAG                       0
    #define KERNEL_PROFILING                    0
#endif /* CFG_DBG */

/// Flag indicating if Read/Write memory commands are supported or not
#if defined(CFG_DBG_MEM)
    #define RW_DEBUG_MEM               1
#else //CFG_DBG_MEM
    #define RW_DEBUG_MEM               0
#endif //CFG_DBG_MEM

/// Flag indicating if Flash debug commands are supported or not
#if defined(CFG_DBG_FLASH)
    #define RW_DEBUG_FLASH                  1
#else //CFG_DBG_FLASH
    #define RW_DEBUG_FLASH                  0
#endif //CFG_DBG_FLASH

/// Flag indicating if CPU stack profiling commands are supported or not
#if defined(CFG_DBG_STACK_PROF)
    #define RW_DEBUG_STACK_PROF             1
#else
    #define RW_DEBUG_STACK_PROF             0
#endif // defined (CFG_DBG_STACK_PROF)

/// Scheduling Planner unit test (HCI debug commands to test scheduling planner functions)
#define SCH_PLAN_UT                 (1)

/// BLE I&Q sample Generator control interface
#define BLE_IQ_GEN                  (RW_DEBUG && (BLE_CON_CTE_REQ | BLE_CONLESS_CTE_RX))


/******************************************************************************************/
/* --------------------------      VS SETUP         --------------------------------------*/
/******************************************************************************************/

/// Read piconnect clock
#if defined (CFG_BT_READ_PICONET_CLOCK)
#define BT_READ_PICONET_CLOCK   1
#else
#define BT_READ_PICONET_CLOCK   0
#endif //defined (CFG_BT_READ_PICONET_CLOCK)

/******************************************************************************************/
/* --------------------------      NVDS SETUP       --------------------------------------*/
/******************************************************************************************/

/// Flag indicating if NVDS feature is supported or not
#if defined(CFG_NVDS)
    #define NVDS_SUPPORT                    1
#else //CFG_DBG_NVDS
    #define NVDS_SUPPORT                    0
#endif //CFG_DBG_NVDS

/******************************************************************************************/
/* --------------------------      MISC SETUP       --------------------------------------*/
/******************************************************************************************/
/// Manufacturer: RivieraWaves SAS
#define RW_COMP_ID                           0x0060

/// Bluetooth technologies version
#define RW_BT40_VERSION                      (6)
#define RW_BT41_VERSION                      (7)
#define RW_BT42_VERSION                      (8)
#define RW_BT50_VERSION                      (9)
#define RW_BT51_VERSION                      (10)

/******************************************************************************************/
/* -------------------------   BT / BLE / BLE HL CONFIG    -------------------------------*/
/******************************************************************************************/

#if (BT_EMB_PRESENT)
#include "rwbt_config.h"    // bt stack configuration
#endif //BT_EMB_PRESENT

#if (BLE_EMB_PRESENT)
#include "rwble_config.h"   // ble stack configuration
#endif //BLE_EMB_PRESENT

#if (BLE_HOST_PRESENT)
#include "rwble_hl_config.h"  // ble Host stack configuration
#endif //BLE_HOST_PRESENT

#if defined(CFG_APP)
#include "rwapp_config.h"     // Application configuration
#endif // defined(CFG_APP)



/******************************************************************************************/
/* -------------------------   KERNEL SETUP          -------------------------------------*/
/******************************************************************************************/

/// Event types definition
/*@TRACE*/
enum KERNEL_EVENT_TYPE
{
    #if RTC_SUPPORT
    KERNEL_EVENT_RTC_1S_TICK,
    #endif //RTC_SUPPORT

    #if BLE_RSA
    KERNEL_EVENT_RSA_SIGN,
    #endif //BLE_RSA
    
    KERNEL_EVENT_ECC_MULTIPLICATION,

    #if BT_EMB_PRESENT
    KERNEL_EVENT_P192_PUB_KEY_GEN,
    #endif // BT_EMB_PRESENT

    #if (BLE_MESH)
    KERNEL_EVENT_BLE_MESH_DJOB ,
    #endif // (BLE_MESH)

    #if (BLE_EMB_PRESENT || BT_EMB_PRESENT)
    KERNEL_EVENT_AES_END,
    #endif // (BLE_EMB_PRESENT || BT_EMB_PRESENT)

    KERNEL_EVENT_KERNEL_MESSAGE,
    KERNEL_EVENT_KERNEL_TIMER,

    #if (TRACER_PRESENT)
    KERNEL_EVENT_TRC,
    #endif /*(TRACER_PRESENT)*/

    #if H4TL_SUPPORT
    KERNEL_EVENT_H4TL_TX,
    #if (BLE_EMB_PRESENT || BT_EMB_PRESENT)
    KERNEL_EVENT_H4TL_CMD_HDR_RX,
    KERNEL_EVENT_H4TL_CMD_PLD_RX,
    #endif //(BLE_EMB_PRESENT || BT_EMB_PRESENT)
    #if (((BLE_EMB_PRESENT || BLE_HOST_PRESENT) && (BLE_CENTRAL || BLE_PERIPHERAL)) || BT_EMB_PRESENT)
    KERNEL_EVENT_H4TL_ACL_HDR_RX,
    #endif //(((BLE_EMB_PRESENT || BLE_HOST_PRESENT) && (BLE_CENTRAL || BLE_PERIPHERAL)) || BT_EMB_PRESENT)
    #endif //H4TL_SUPPORT

    #if (BLE_HOST_PRESENT)
    #if (BLE_L2CC)
    KERNEL_EVENT_L2CAP_TX,
    #endif //(BLE_L2CC)
    #endif// (BLE_HOST_PRESENT)

    #if BT_EMB_PRESENT
    KERNEL_EVENT_BT_PSCAN_PROC,
    #endif //BT_EMB_PRESENT

    #if (BLE_ISOOHCI)
    KERNEL_EVENT_ISOOHCI_IN_DEFER,
    KERNEL_EVENT_ISOOHCI_OUT_DEFER,
    #endif //(BLE_ISOOHCI)

  #if(BEKEN_TASK_ENABLE)
	KERNEL_EVENT_BEKEN_TEST,
  #endif

    KERNEL_EVENT_MAX,
};

/// Tasks types definition
/*@TRACE*/
enum KERNEL_TASK_TYPE
{
    #if (BT_EMB_PRESENT)
    // BT Controller Tasks
    TASK_BLE_LM,
    TASK_BLE_LC,
    TASK_BLE_LB,
    #endif // (BT_EMB_PRESENT)

    #if (BLE_EMB_PRESENT)
    // Link Layer Tasks
    TASK_BLE_LLM,
    TASK_BLE_LLC,
    #if (BLE_ISO_PRESENT)
    TASK_BLE_LLI,
    #endif // (BLE_ISO_PRESENT)
    #endif // (BLE_EMB_PRESENT)

#if ((BLE_EMB_PRESENT) || (BT_EMB_PRESENT))
    TASK_BLE_DBG,
#endif // ((BLE_EMB_PRESENT) || (BT_EMB_PRESENT))

#if (BLE_APP_PRESENT)
    TASK_BLE_APP,
#endif // (BLE_APP_PRESENT)

#if (BLE_HOST_PRESENT)
    TASK_BLE_L2CC,    // L2CAP Controller Task
    TASK_BLE_GATTM,   // Generic Attribute Profile Manager Task
    TASK_BLE_GATTC,   // Generic Attribute Profile Controller Task
    TASK_BLE_GAPM,    // Generic Access Profile Manager
    TASK_BLE_GAPC,    // Generic Access Profile Controller

    // allocate a certain number of profiles task
    TASK_BLE_PRF_MAX = (TASK_BLE_GAPC + BLE_NB_PROFILES),

    #if (BLE_ISO_MODE_0_PROTOCOL)
    TASK_BLE_AM0,     // BLE Audio Mode 0 Task
    #endif // (BLE_ISO_MODE_0_PROTOCOL)
#endif // (BLE_HOST_PRESENT)

#if (AHI_TL_SUPPORT)
    TASK_BLE_AHI,
#endif // (AHI_TL_SUPPORT)

  #if(BEKEN_TASK_ENABLE)
	TASK_BLE_BEKEN,
  #endif

    /// Maximum number of tasks
    TASK_BLE_MAX,

    TASK_BLE_NONE = 0xFF,
};

/// Kernel memory heaps types.
/*@TRACE*/
enum KERNEL_MEM_HEAP
{
    /// Memory allocated for environment variables
    KERNEL_MEM_ENV,
    #if (BLE_HOST_PRESENT)
    /// Memory allocated for Attribute database
    KERNEL_MEM_ATT_DB,
    #endif // (BLE_HOST_PRESENT)
    /// Memory allocated for kernel messages
    KERNEL_MEM_KERNEL_MSG,
    /// Non Retention memory block
    KERNEL_MEM_NON_RETENTION,
    KERNEL_MEM_BLOCK_MAX,
};



#if (BT_EMB_PRESENT)
#define BT_HEAP_MSG_SIZE_      BT_HEAP_MSG_SIZE
#define BT_HEAP_ENV_SIZE_      BT_HEAP_ENV_SIZE
#else
#define BT_HEAP_MSG_SIZE_      0
#define BT_HEAP_ENV_SIZE_      0
#endif //BT_EMB_PRESENT

#if (BLE_EMB_PRESENT)
#define BLE_HEAP_MSG_SIZE_     BLE_HEAP_MSG_SIZE
#define BLE_HEAP_ENV_SIZE_     BLE_HEAP_ENV_SIZE
#else
#define BLE_HEAP_MSG_SIZE_     0
#define BLE_HEAP_ENV_SIZE_     0
#endif //BLE_EMB_PRESENT

#if (BLE_HOST_PRESENT)

#define BLEHL_HEAP_MSG_SIZE_   BLEHL_HEAP_MSG_SIZE
#define BLEHL_HEAP_ENV_SIZE_   BLEHL_HEAP_ENV_SIZE
#define BLEHL_HEAP_DB_SIZE_    BLEHL_HEAP_DB_SIZE
#else
#define BLEHL_HEAP_MSG_SIZE_   0
#define BLEHL_HEAP_ENV_SIZE_   0
#define BLEHL_HEAP_DB_SIZE_    0
#endif //BLE_HOST_PRESENT

#if (BT_EMB_PRESENT || BLE_EMB_PRESENT)
#define ECC_HEAP_NON_RET_SIZE_   (328*2) // Could only have 2 ECC computations simultaneously
#else // (BT_EMB_PRESENT || BLE_EMB_PRESENT)
#define ECC_HEAP_NON_RET_SIZE_   0
#endif // (BT_EMB_PRESENT || BLE_EMB_PRESENT)

/// Kernel Message Heap
#define RWIP_HEAP_MSG_SIZE         (  BT_HEAP_MSG_SIZE_      + \
                                      BLE_HEAP_MSG_SIZE_     + \
                                      BLEHL_HEAP_MSG_SIZE_      )

/// Size of Environment heap
#define RWIP_HEAP_ENV_SIZE         ( BT_HEAP_ENV_SIZE_       + \
                                     BLE_HEAP_ENV_SIZE_      + \
                                     BLEHL_HEAP_ENV_SIZE_       )


/// Size of Attribute database heap
#define RWIP_HEAP_DB_SIZE          (  BLEHL_HEAP_DB_SIZE_  )

/**
 * Size of non-retention heap
 *
 * This heap can be used to split the RAM into 2 parts:
 *    - an always-on part that can handle a certain number of links
 *    - a secondary memory that could be powered-off when not used, and retained only when used
 *
 * With such mechanism, the previous heaps need to be reduced so that they can contain all required data
 * in a light scenario (few connections, few profiles). Then the non-retention heap is sized in order to
 * cover the worst case scenario (max connections, max profiles, etc ...)
 *
 * The current size show what is already known as not needing to be retained during deep sleep.
 */
#define RWIP_HEAP_NON_RET_SIZE    ( ECC_HEAP_NON_RET_SIZE_ )

/// Minimum sleep time to enter in deep sleep (in half slot).
#define RWIP_MINIMUM_SLEEP_TIME                (6)

/******************************************************************************************/
/* -------------------------     CONFIGURABLE PARAMETERS     -----------------------------*/
/******************************************************************************************/

/// Bit field definitions for channel assessment enable parameter
enum param_ch_ass_en
{
    /// Indicate channel assessment enabled for BLE
    CH_ASS_EN_BLE_POS           = 0,
    CH_ASS_EN_BLE_BIT           = (0x1),
    /// Indicate channel assessment enabled for BT
    CH_ASS_EN_BT_POS            = 1,
    CH_ASS_EN_BT_BIT            = (0x2),
};

/// List of parameters identifiers
enum PARAM_ID
{
    /// Definition of the tag associated to each parameters
    /// Local Bd Address
    PARAM_ID_BD_ADDRESS                 = 0x01,
    /// Device Name
    PARAM_ID_DEVICE_NAME                = 0x02,
    /// Radio Drift
    PARAM_ID_LPCLK_DRIFT                = 0x07,
    /// Radio Jitter
    PARAM_ID_LPCLK_JITTER               = 0x08,
    /// External wake-up time
    PARAM_ID_EXT_WAKEUP_TIME            = 0x0D,
    /// Oscillator wake-up time
    PARAM_ID_OSC_WAKEUP_TIME            = 0x0E,
    /// Radio wake-up time
    PARAM_ID_RM_WAKEUP_TIME             = 0x0F,
    /// UART baudrate
    PARAM_ID_UART_BAUDRATE              = 0x10,
    /// Enable sleep mode
    PARAM_ID_SLEEP_ENABLE               = 0x11,
    /// Enable External Wakeup
    PARAM_ID_EXT_WAKEUP_ENABLE          = 0x12,
    /// SP Private Key 192
    PARAM_ID_SP_PRIVATE_KEY_P192        = 0x13,
    /// SP Public Key 192
    PARAM_ID_SP_PUBLIC_KEY_P192         = 0x14,

    /// Activity Move Configuration (enables/disables activity move for BLE connections and BT (e)SCO links)
    PARAM_ID_ACTIVITY_MOVE_CONFIG       = 0x15,

    /// Enable/disable scanning for extended advertising PDUs
    PARAM_ID_SCAN_EXT_ADV               = 0x16,

    /// Duration of the schedule reservation for long activities such as scan, inquiry, page, HDC advertising
    PARAM_ID_SCHED_SCAN_DUR             = 0x17,

    /// Programming delay, margin for programming the baseband in advance of each activity (in half-slots)
    PARAM_ID_PROG_DELAY                 = 0x18,

    /// Enable/disable channel assessment for BT and/or BLE
    PARAM_ID_CH_ASS_EN                  = 0x19,

    /// Synchronous links configuration
    PARAM_ID_SYNC_CONFIG                = 0x2C,
    /// PCM Settings
    PARAM_ID_PCM_SETTINGS               = 0x2D,
    /// Sleep algorithm duration
    PARAM_ID_SLEEP_ALGO_DUR             = 0x2E,
    /// Tracer configuration
    PARAM_ID_TRACER_CONFIG              = 0x2F,

    /// Diagport configuration
    PARAM_ID_DIAG_BT_HW                 = 0x30,
    PARAM_ID_DIAG_BLE_HW                = 0x31,
    PARAM_ID_DIAG_SW                    = 0x32,
    PARAM_ID_DIAG_DM_HW                 = 0x33,
    PARAM_ID_DIAG_PLF                   = 0x34,

    /// IDC selection (for audio demo)
    PARAM_ID_IDCSEL_PLF                 = 0x37,

    /// RSSI threshold tags
    PARAM_ID_RSSI_HIGH_THR              = 0x3A,
    PARAM_ID_RSSI_LOW_THR               = 0x3B,
    PARAM_ID_RSSI_INTERF_THR            = 0x3C,

    /// RF BTIPT
    PARAM_ID_RF_BTIPT_VERSION          = 0x3E,
    PARAM_ID_RF_BTIPT_XO_SETTING       = 0x3F,
    PARAM_ID_RF_BTIPT_GAIN_SETTING     = 0x40,

    PARAM_ID_BT_LINK_KEY_FIRST          = 0x60,
    PARAM_ID_BT_LINK_KEY_LAST           = 0x67,

    PARAM_ID_BLE_LINK_KEY_FIRST         = 0x70,
    PARAM_ID_BLE_LINK_KEY_LAST          = 0x7F,
    /// SC Private Key (Low Energy)
    PARAM_ID_LE_PRIVATE_KEY_P256        = 0x80,
    /// SC Public Key (Low Energy)
    PARAM_ID_LE_PUBLIC_KEY_P256         = 0x81,
    /// SC Debug: Used Fixed Private Key from NVDS (Low Energy)
    PARAM_ID_LE_DBG_FIXED_P256_KEY      = 0x82,
    /// SP Private Key (classic BT)
    PARAM_ID_SP_PRIVATE_KEY_P256        = 0x83,
    /// SP Public Key (classic BT)
    PARAM_ID_SP_PUBLIC_KEY_P256         = 0x84,

    /// LE Coded PHY 500 Kbps selection
    PARAM_ID_LE_CODED_PHY_500           = 0x85,

    /// Application specific
    PARAM_ID_APP_SPECIFIC_FIRST         = 0x90,
    PARAM_ID_APP_SPECIFIC_LAST          = 0xAF,

    /// Mesh NVDS values
    PARAM_ID_MESH_SPECIFIC_FIRST        = 0xB0,
    PARAM_ID_MESH_SPECIFIC_LAST         = 0xF0,
};

/// List of parameters lengths
enum PARAM_LEN
{
     // Definition of length associated to each parameters
     /// Local Bd Address
     PARAM_LEN_BD_ADDRESS                 = 6,
     /// Device Name
     PARAM_LEN_DEVICE_NAME                = 248,
     /// Low power clock drift
     PARAM_LEN_LPCLK_DRIFT                = 2,
     /// Low power clock jitter
     PARAM_LEN_LPCLK_JITTER               = 1,
     /// External wake-up time
     PARAM_LEN_EXT_WAKEUP_TIME            = 2,
     /// Oscillator wake-up time
     PARAM_LEN_OSC_WAKEUP_TIME            = 2,
     /// Radio wake-up time
     PARAM_LEN_RM_WAKEUP_TIME             = 2,
     /// UART baudrate
     PARAM_LEN_UART_BAUDRATE              = 4,
     /// Enable sleep mode
     PARAM_LEN_SLEEP_ENABLE               = 1,
     /// Enable External Wakeup
     PARAM_LEN_EXT_WAKEUP_ENABLE          = 1,
     /// SP Private Key 192
     PARAM_LEN_SP_PRIVATE_KEY_P192        = 24,
     /// SP Public Key 192
     PARAM_LEN_SP_PUBLIC_KEY_P192         = 48,

     /// Activity Move Configuration
     PARAM_LEN_ACTIVITY_MOVE_CONFIG       = 1,

     /// Enable/disable scanning for extended advertising PDUs
     PARAM_LEN_SCAN_EXT_ADV               = 1,

     /// Duration of the schedule reservation for long activities such as scan, inquiry, page, HDC advertising
     PARAM_LEN_SCHED_SCAN_DUR             = 2,

     /// Programming delay, margin for programming the baseband in advance of each activity (in half-slots)
     PARAM_LEN_PROG_DELAY                 = 1,

     /// Enable/disable channel assessment for BT and/or BLE
     PARAM_LEN_CH_ASS_EN                  = 1,

     /// Synchronous links configuration
     PARAM_LEN_SYNC_CONFIG                = 2,
     /// PCM Settings
     PARAM_LEN_PCM_SETTINGS               = 8,
     /// Tracer configuration
     PARAM_LEN_TRACER_CONFIG              = 4,

     /// Diagport configuration
     PARAM_LEN_DIAG_BT_HW                 = 4,
     PARAM_LEN_DIAG_BLE_HW                = 4,
     PARAM_LEN_DIAG_SW                    = 4,
     PARAM_LEN_DIAG_DM_HW                 = 4,
     PARAM_LEN_DIAG_PLF                   = 4,

     /// IDC selection (for audio demo)
     PARAM_LEN_IDCSEL_PLF                 = 4,

     /// RSSI thresholds
     PARAM_LEN_RSSI_THR                   = 1,

     /// RF BTIPT
     PARAM_LEN_RF_BTIPT_VERSION          = 1,
     PARAM_LEN_RF_BTIPT_XO_SETTING       = 1,
     PARAM_LEN_RF_BTIPT_GAIN_SETTING     = 2,

     /// Link keys
     PARAM_LEN_BT_LINK_KEY                = 22,
     PARAM_LEN_BLE_LINK_KEY               = 48,

     /// P256
     PARAM_LEN_PRIVATE_KEY_P256           = 32,
     PARAM_LEN_PUBLIC_KEY_P256            = 64,
     PARAM_LEN_DBG_FIXED_P256_KEY         = 1,

     /// LE Coded PHY 500 Kbps selection
     PARAM_LEN_LE_CODED_PHY_500           = 1,
};

/******************************************************************************************/
/* -------------------------        BT-BLE COEX        -----------------------------------*/
/******************************************************************************************/

///To let the HW using the default values set in the registers
#define RW_BLE_PTI_PRIO_AUTO    15

#if (BLE_EMB_PRESENT || BT_EMB_PRESENT)
/// Enable and diable definition for the PTI
///Enable TX busy signal
#define RWIP_PTI_TXEN           1
///Disable TX busy signal
#define RWIP_PTI_TXDIS          0
/// Tx busy position
#define RWIP_TXBSY_POS          0

///Enable RX busy signal
#define RWIP_PTI_RXEN           1
///Disable RX busy signal
#define RWIP_PTI_RXDIS          0
/// Rx busy position
#define RWIP_RXBSY_POS          1

///Enable do not abort TX
#define RWIP_PTI_DNABORTEN      1
///Disable do not abort TX
#define RWIP_PTI_DNABORTDIS     0
/// Do not abort busy position
#define RWIP_DNABORT_POS        2

/// SAM disabled
#define RWIP_SAM_DIS            0
/// SAM enabled
#define RWIP_SAM_EN             1
/// SAM enable position
#define RWIP_SAMEN_POS          3

/// Bit masking
#define RWIP_COEX_BIT_MASK      1

/// Coex configuration index
enum rwip_coex_config_idx
{
    #if (BT_EMB_PRESENT)
    RWIP_COEX_MSSWITCH_IDX,
    RWIP_COEX_SNIFFATT_IDX,
    RWIP_COEX_PAGE_IDX,
    RWIP_COEX_PSCAN_IDX,
    RWIP_COEX_INQ_IDX,
    RWIP_COEX_INQRES_IDX,
    RWIP_COEX_SCORSVD_IDX,
    RWIP_COEX_BCAST_IDX,
    RWIP_COEX_CONNECT_IDX,
    #endif //#if (BT_EMB_PRESENT)
    #if (BLE_EMB_PRESENT)
    RWIP_COEX_CON_IDX,
    RWIP_COEX_CON_DATA_IDX,
    RWIP_COEX_ADV_IDX,
    RWIP_COEX_SCAN_IDX,
    RWIP_COEX_INIT_IDX,
    #endif // #if (BLE_EMB_PRESENT)
    /// Max configuration index
    RWIP_COEX_CFG_MAX,
};
#endif //(BLE_EMB_PRESENT || BT_EMB_PRESENT)

/******************************************************************************************/
/* -------------------------     BT-BLE PRIORITIES     -----------------------------------*/
/******************************************************************************************/
#if (BLE_EMB_PRESENT || BT_EMB_PRESENT)
/// Priority index definition
enum rwip_prio_idx
{
    #if (BT_EMB_PRESENT)
    /// ACL event default priority
    RWIP_PRIO_ACL_DFT_IDX,
    /// ACL event priority with activity
    RWIP_PRIO_ACL_ACT_IDX,
    /// ACL Role Switch event default priority
    RWIP_PRIO_ACL_RSW_IDX,
    /// ACL sniff event default priority
    RWIP_PRIO_ACL_SNIFF_DFT_IDX,
    /// ACL sniff transition event default priority
    RWIP_PRIO_ACL_SNIFF_TRANS_IDX,
    #if MAX_NB_SYNC
    /// SCO event default priority
    RWIP_PRIO_SCO_DFT_IDX,
    #endif //MAX_NB_SYNC
    /// Broadcast ACL event default priority
    RWIP_PRIO_BCST_DFT_IDX,
    /// Broadcast ACL event with LMP activity priority
    RWIP_PRIO_BCST_ACT_IDX,
    /// CSB RX event default priority
    RWIP_PRIO_CSB_RX_DFT_IDX,
    /// CSB TX event default priority
    RWIP_PRIO_CSB_TX_DFT_IDX,
    /// Inquiry event default priority
    RWIP_PRIO_INQ_DFT_IDX,
    /// Inquiry Scan event default priority
    RWIP_PRIO_ISCAN_DFT_IDX,
    /// Page event default priority
    RWIP_PRIO_PAGE_DFT_IDX,
    /// Page event default priority
    RWIP_PRIO_PAGE_1ST_PKT_IDX,
    /// Page first packet event default priority
    RWIP_PRIO_PCA_DFT_IDX,
    /// Page scan event default priority
    RWIP_PRIO_PSCAN_DFT_IDX,
    /// Page scan event priority increment when canceled
    RWIP_PRIO_PSCAN_1ST_PKT_IDX,
    /// Synchronization Scan event default priority
    RWIP_PRIO_SSCAN_DFT_IDX,
    /// Synchronization Train event default priority
    RWIP_PRIO_STRAIN_DFT_IDX,
    #endif //#if (BT_EMB_PRESENT)
    #if (BLE_EMB_PRESENT)
    /// Default priority for scanning events
    RWIP_PRIO_SCAN_IDX,
    /// Default priority for auxillary scan/init (no_asap) rx events
    RWIP_PRIO_AUX_RX_IDX,
    /// Default priority for periodic adv rx events
    RWIP_PRIO_PER_ADV_RX_DFT_IDX,
    /// Default priority for initiating events
    RWIP_PRIO_INIT_IDX,
    /// LE connection events default priority
    RWIP_PRIO_CONNECT_DFT_IDX,
    /// LE connection events priority with activity
    RWIP_PRIO_CONNECT_ACT_IDX,
    /// Default priority for advertising events
    RWIP_PRIO_ADV_IDX,
    /// Default priority for advertising high duty cycle events
    RWIP_PRIO_ADV_HDC_IDX,
    /// Default priority for aux advertising events
    RWIP_PRIO_ADV_AUX_IDX,
    /// Default priority for periodic advertising events
    RWIP_PRIO_PER_ADV_IDX,
    /// Default priority for resolvable private addresses renewal event
    RWIP_PRIO_RPA_RENEW_IDX,
    #endif // #if (BLE_EMB_PRESENT)
    RWIP_PRIO_IDX_MAX
};
/// Default priority value definition
enum rwip_prio_dft
{
    #if (BT_EMB_PRESENT)
    /// ACL event default priority
    RWIP_PRIO_ACL_DFT               = 5,
    /// ACL event priority with activity
    RWIP_PRIO_ACL_ACT               = 10,
    /// ACL Role Switch event default priority
    RWIP_PRIO_ACL_RSW               = 20,
    /// ACL sniff event default priority
    RWIP_PRIO_ACL_SNIFF_DFT         = 15,
    /// ACL sniff transition event default priority
    RWIP_PRIO_ACL_SNIFF_TRANS       = 10,
    #if MAX_NB_SYNC
    /// SCO event default priority
    RWIP_PRIO_SCO_DFT               = 18,
    #endif //MAX_NB_SYNC
    /// Broadcast ACL event default priority
    RWIP_PRIO_BCST_DFT              = 5,
    /// Broadcast ACL event with LMP activity priority
    RWIP_PRIO_BCST_ACT              = 10,
    /// CSB RX event default priority
    RWIP_PRIO_CSB_RX_DFT            = 10,
    /// CSB TX event default priority
    RWIP_PRIO_CSB_TX_DFT            = 10,
    /// Inquiry event default priority
    RWIP_PRIO_INQ_DFT               = 5,
    /// Inquiry Scan event default priority
    RWIP_PRIO_ISCAN_DFT             = 5,
    /// Page event default priority
    RWIP_PRIO_PAGE_DFT              = 8,
    /// Page first packet event default priority
    RWIP_PRIO_PAGE_1ST_PKT          = 20,
    /// PCA event default priority
    RWIP_PRIO_PCA_DFT               = 20,
    /// Page scan event default priority
    RWIP_PRIO_PSCAN_DFT             = 8,
    /// Page scan event priority increment when canceled
    RWIP_PRIO_PSCAN_1ST_PKT         = 20,
    /// Synchronization Scan event default priority
    RWIP_PRIO_SSCAN_DFT             = 10,
    /// Synchronization Train event default priority
    RWIP_PRIO_STRAIN_DFT            = 10,
    #endif //#if (BT_EMB_PRESENT)
    #if (BLE_EMB_PRESENT)
    /// Default priority for scanning events
    RWIP_PRIO_SCAN_DFT              = 5,
    /// Default priority for auxillary scan/init (no_asap) rx events
    RWIP_PRIO_AUX_RX_DFT            = 12,
    /// Default priority for periodic adv rx events
    RWIP_PRIO_PER_ADV_RX_DFT        = 16,
    /// Active priority for periodic adv rx events
    RWIP_PRIO_INIT_DFT              = 10,
    /// LE connection events default priority
    RWIP_PRIO_CONNECT_DFT           = 14,
    /// LE connection events priority with activity
    RWIP_PRIO_CONNECT_ACT           = 16,
    /// Default priority for advertising events
    RWIP_PRIO_ADV_DFT               = 5,
    /// Default priority for advertising high duty cycle events
    RWIP_PRIO_ADV_HDC_DFT           = 10,
    /// Default priority for aux advertising events
    RWIP_PRIO_ADV_AUX_DFT           = 12,
    /// Default priority for periodic advertising events
    RWIP_PRIO_PER_ADV_DFT           = 10,
    /// Default priority for resolvable private addresses renewal event
    RWIP_PRIO_RPA_RENEW_DFT         = 10,
    #endif // #if (BLE_EMB_PRESENT)
    /// Max priority
    RWIP_PRIO_MAX                   = 31,
};
/// Default increment value definition
enum rwip_incr_dft
{
    #if (BT_EMB_PRESENT)
    /// ACL event default increment
    RWIP_INCR_ACL_DFT               = 1,
    /// ACL event increment with activity
    RWIP_INCR_ACL_ACT               = 1,
    /// ACL Role Switch event default increment
    RWIP_INCR_ACL_RSW               = 1,
    /// ACL sniff event default increment
    RWIP_INCR_ACL_SNIFF_DFT         = 1,
    /// ACL sniff transition event default increment
    RWIP_INCR_ACL_SNIFF_TRANS       = 1,
    #if MAX_NB_SYNC
    /// SCO event default increment
    RWIP_INCR_SCO_DFT               = 1,
    #endif //MAX_NB_SYNC
    /// Broadcast ACL event default increment
    RWIP_INCR_BCST_DFT              = 1,
    /// Broadcast ACL event with LMP activity increment
    RWIP_INCR_BCST_ACT              = 1,
    /// CSB RX event default increment
    RWIP_INCR_CSB_RX_DFT            = 1,
    /// CSB TX event default increment
    RWIP_INCR_CSB_TX_DFT            = 1,
    /// Inquiry event default increment
    RWIP_INCR_INQ_DFT               = 1,
    /// Inquiry Scan event default increment
    RWIP_INCR_ISCAN_DFT             = 1,
    /// Page event default increment
    RWIP_INCR_PAGE_DFT              = 1,
    /// Page event default increment
    RWIP_INCR_PAGE_1ST_PKT          = 2,
    /// Page first packet event default increment
    RWIP_INCR_PCA_DFT               = 1,
    /// Page scan event default increment
    RWIP_INCR_PSCAN_DFT             = 1,
    /// Page scan event increment increment when canceled
    RWIP_INCR_PSCAN_1ST_PKT         = 1,
    /// Synchronization Scan event default increment
    RWIP_INCR_SSCAN_DFT             = 1,
    /// Synchronization Train event default increment
    RWIP_INCR_STRAIN_DFT            = 1,
    #endif //#if (BT_EMB_PRESENT)
    #if (BLE_EMB_PRESENT)
    /// Default increment for scanning events
    RWIP_INCR_SCAN_DFT              = 1,
    /// Default increment for auxillary scan/init (no_asap) rx events
    RWIP_INCR_AUX_RX_DFT            = 1,
    /// Default increment for periodic adv rx events
    RWIP_INCR_PER_ADV_RX_DFT        = 1,
    /// Default increment for initiating events
    RWIP_INCR_INIT_DFT              = 1,
    /// LE connection events default increment
    RWIP_INCR_CONNECT_DFT           = 1,
    /// LE connection events increment with activity
    RWIP_INCR_CONNECT_ACT           = 1,
    /// Default increment for advertising events
    RWIP_INCR_ADV_DFT               = 1,
    /// Default increment for advertising high duty cycle events
    RWIP_INCR_ADV_HDC_PRIO_DFT      = 1,
    /// Default increment for aux advertising events
    RWIP_INCR_ADV_AUX_DFT           = 1,
    /// Default increment for periodic advertising events
    RWIP_INCR_PER_ADV_DFT           = 1,
    /// Default increment for resolvable private addresses renewal event
    RWIP_INCR_RPA_RENEW_DFT         = 1,
    #endif // #if (BLE_EMB_PRESENT)
};
#endif //#if (BLE_EMB_PRESENT || BT_EMB_PRESENT)
/// @} BT Stack Configuration
/// @} ROOT

#endif //RWIP_CONFIG_H_
