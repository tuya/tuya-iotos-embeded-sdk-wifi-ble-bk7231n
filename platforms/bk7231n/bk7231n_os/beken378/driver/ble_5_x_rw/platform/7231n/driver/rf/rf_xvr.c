/**
****************************************************************************************
*
* @file rf_xvras.c
*
* @brief Atlas radio initialization and specific functions
*
* Copyright (C) Beken 2009-2015
*
* $Rev: $
*
****************************************************************************************
*/

/**
****************************************************************************************
* @addtogroup RF_XVR
* @ingroup RF
* @brief Radio Driver
*
* This is the driver block for radio
* @{
****************************************************************************************
*/

/**
 *****************************************************************************************
 * INCLUDE FILES
 *****************************************************************************************
 */
#include "ble_reg_ipcore.h"        // DM core registers
#include "rwip_config.h"        // RW SW configuration
#include <string.h>             // for memcpy
#include "common_utils.h"           // common utility definition
#include "common_math.h"            // common math functions
#include "rf.h"                 // RF interface
#include "rwip.h"               // for RF API structure definition
#include "em_map.h"
#if (BLE_EMB_PRESENT)
#include "ble_reg_blecore.h"        // ble core registers
#include "ble_reg_em_ble_cs.h"      // control structure definitions
#endif //BLE_EMB_PRESENT
#if (BT_EMB_PRESENT)
#include "ble_reg_btcore.h"         // bt core registers
#include "ble_reg_em_bt_cs.h"       // control structure definitions
#endif //BT_EMB_PRESENT
#include "ble_reg_access.h"			//// added
#include "BK3633_RegList.h"           // Platform register
#include "user_config.h"
/**
 ****************************************************************************************
 * DEFINES
 ****************************************************************************************
 **/
 #define RPL_GAIN_TBL_SIZE           0x0F
//#define bk_printf uart2_printf

// EM RF SPI address
#define RF_EM_SPI_ADRESS        (EM_BASE_ADDR + EM_RF_SW_SPI_OFFSET)

#define RPL_SPIRD                   0x00
#define RPL_SPIWR                   0x80
#define RPL_RFPLL_TBL_SIZE          0x50
#define RPL_PWR_TBL_SIZE            0x0F

/* The offset value given below is the offset to add to the frequency table index to
   get the value to be programmed in the radio for each channel                      */
#define RPL_FREQTAB_OFFSET          0   // Offset for Ripple radio

/// Radio skew compensation (round trip delay)
#define RPL_RADIO_SKEW              12L

#define RFLOIF                      0x00

#define RPL_RSSI_20dB_THRHLD        -20
#define RPL_RSSI_45dB_THRHLD        -45
#define RPL_RSSI_48dB_THRHLD        -48
#define RPL_RSSI_55dB_THRHLD        -55
#define RPL_RSSI_60dB_THRHLD        -60
#define RPL_RSSI_70dB_THRHLD        -70

// EDR Control value
#define RPL_EDRCNTL                 18 // Default value is set to 18us


#define RPL_POWER_MAX               0x0c	//// ????
////#define RPL_POWER_MAX               0x06
#define RPL_POWER_MIN               0x01	//// ????
#define RPL_POWER_MSK               0x07	//// ????

// Generic RSSI Threshold
#define RF_RPL_RSSI_THR             0x29


/**
****************************************************************************************
* MACROS
*****************************************************************************************
*/




/**
 ****************************************************************************************
 * GLOBAL VARIABLE DEFINITIONS
 *****************************************************************************************
 **/
//// IP ????
// PLL Lock Table
///static uint8_t RFPLL_LOCK_TABLE[RPL_RFPLL_TBL_SIZE] = {0};

// PLL VCOFC table
//static uint8_t RFPLL_VCOFC_TABLE[RPL_RFPLL_TBL_SIZE] = {0};

// PLL ICP table
//static uint8_t RFPLL_ICP_TABLE[RPL_RFPLL_TBL_SIZE] = {0};

// Power table
static const int8_t RF_RPL_TX_PW_CONV_TBL[RPL_PWR_TBL_SIZE] = {
        [0] = -23,
        [1] = -20,
        [2] = -17,
        [3] = -14,
        [4] = -11,
        [5] = -8,
        [6] = -5,
        [7] = -2};
// Gain table
static const uint8_t RF_RPL_RX_GAIN_TBL[RPL_GAIN_TBL_SIZE] = {
        [0] = 43,
        [1] = 37,
        [2] = 31,
        [3] = 25,
        [4] = 19,
        [5] = 13,
        [6] = 7,
        [7] = 1};
////IP ????


extern uint8_t system_mode;

/**
 ****************************************************************************************
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

void CLK32K_AutoCali_init(void);


/**
 *****************************************************************************************
 * @brief Init RF sequence after reset.
 *****************************************************************************************
 */

static void rf_reset(void)
{
}

/**
 ****************************************************************************************
 * @brief ISR to be called in BLE ISR routine when RF Interrupt occurs.
 *****************************************************************************************
 */


//// IP
/**
 *****************************************************************************************
 * @brief Get the TX power as control structure TX power field from a value in dBm.
 *
 * @param[in] txpwr_dbm   TX power in dBm
 * @param[in] high        If true, return index equal to or higher than requested
 *                        If false, return index equal to or lower than requested
 *
 * @return The index of the TX power
 *
 *****************************************************************************************
 */
static uint8_t rf_txpwr_cs_get (int8_t txpwr_dbm, bool high)
{
    uint8_t i;

    for (i = RPL_POWER_MIN; i <= RPL_POWER_MAX; i++)
    {
        // Loop until we find a power just higher or equal to the requested one
        if (RF_RPL_TX_PW_CONV_TBL[i] >= txpwr_dbm)
            break;
    }

    // If equal to value requested, do nothing
    // Else if 'high' is false and index higher than the minimum one, decrement by one
    if ((RF_RPL_TX_PW_CONV_TBL[i] != txpwr_dbm) && (!high) && (i > RPL_POWER_MIN))
    {
        i--;
    }

    return(i);
}





/**
 *****************************************************************************************
 * @brief Get TX power in dBm from the index in the control structure
 *
 * @param[in] txpwr_idx  Index of the TX power in the control structure
 * @param[in] modulation Modulation: 1 or 2 or 3 MBPS
 *
 * @return The TX power in dBm
 *
 *****************************************************************************************
 */

static uint8_t rf_txpwr_dbm_get(uint8_t txpwr_idx, uint8_t modulation)
{
    // Power table should be provided
    return(0);
}

/**
 *****************************************************************************************
 * @brief Sleep function for  RF.
 *****************************************************************************************
 */

static void rf_sleep(void)
{
	ip_deepslcntl_set(ip_deepslcntl_get() | 0x04);//// IP_DEEP_SLEEP_ON_BIT | IP_RADIO_SLEEP_EN_BIT | IP_OSC_SLEEP_EN_BIT
}


/**
 *****************************************************************************************
 * @brief Convert RSSI to dBm
 *
 * @param[in] rssi_reg RSSI read from the HW registers
 *
 * @return The converted RSSI
 *
 *****************************************************************************************
 */

static int8_t rf_rssi_convert (uint8_t rssi_reg)
{
    uint8_t RssidBm = 0;

    RssidBm = ((rssi_reg) >> 1) - 118;

    return(RssidBm);
}


static uint32_t rf_rpl_reg_rd (uint32_t addr)
{
	uint32_t ret;

	ret = REG_PL_RD(addr);

	return ret;
}

static void rf_rpl_reg_wr (uint32_t addr, uint32_t value)
{
	REG_PL_WR(addr, value);
}

static void rf_em_init(void)
{
    uint8_t idx = 0;
    uint8_t temp_freq_tbl[EM_RF_FREQ_TABLE_LEN];

    #if BT_EMB_PRESENT
    // First half part of frequency table is for the even frequencies
    while(idx < EM_RF_FREQ_TABLE_LEN/2)
    {
      temp_freq_tbl[idx] = 2*idx + RPL_FREQTAB_OFFSET;
      idx++;
    }
    while(idx < EM_RF_FREQ_TABLE_LEN)
    {
      temp_freq_tbl[idx] = 2*(idx-(EM_RF_FREQ_TABLE_LEN/2)) + 1 + RPL_FREQTAB_OFFSET;
      idx++;
    }
   // em_wr(&temp_freq_tbl[0], EM_FT_OFFSET, EM_RF_FREQ_TABLE_LEN);
    #elif BLE_EMB_PRESENT
    while(idx < EM_RF_FREQ_TABLE_LEN)
    {
      temp_freq_tbl[idx] = 2*idx + RPL_FREQTAB_OFFSET;
      idx++;
    }
    em_wr(&temp_freq_tbl[0], EM_FT_OFFSET, EM_RF_FREQ_TABLE_LEN);
    #endif // BT_EMB_PRESENT/BLE_EMB_PRESENT
}

void xvr_reg_init(void)
{
	// TODO:Use hardware powertable
#if 0
	*((volatile unsigned long *)(0x01050200)) = 0x200;
	unsigned long temp = *((volatile unsigned long *)(0x01050200));
	bk_printf("temp:%08x\r\n", temp);
#endif

#if 1
	REG_WRITE(BLE_XVR_REG00, 0x0426684E);
	REG_WRITE(BLE_XVR_REG01, 0x925B28A1);
	REG_WRITE(BLE_XVR_REG02, 0x2F00EFF2);
	REG_WRITE(BLE_XVR_REG03, 0x0AA00000);
	REG_WRITE(BLE_XVR_REG04, 0x9C697067);
	REG_WRITE(BLE_XVR_REG05, 0xB13B1133);
	REG_WRITE(BLE_XVR_REG06, 0x078A0C5A);
	REG_WRITE(BLE_XVR_REG07, 0x3DA87860);
	REG_WRITE(BLE_XVR_REG08, 0x23BC6000);
	REG_WRITE(BLE_XVR_REG09, 0x03D7F000);
	REG_WRITE(BLE_XVR_REG0A, 0x0003007C);
	REG_WRITE(BLE_XVR_REG0B, 0x00000E5B);
	REG_WRITE(BLE_XVR_REG0C, 0x000101E0);
	REG_WRITE(BLE_XVR_REG0D, 0x08006300);
	REG_WRITE(BLE_XVR_REG0E, 0x00000000);
	REG_WRITE(BLE_XVR_REG0F, 0x00000000);
	REG_WRITE(BLE_XVR_REG10, 0x00003011);
	REG_WRITE(BLE_XVR_REG11, 0x3EED021C);
	REG_WRITE(BLE_XVR_REG12, 0x00000097);
	REG_WRITE(BLE_XVR_REG13, 0x00180000);
	REG_WRITE(BLE_XVR_REG14, 0x02690F8A);
	REG_WRITE(BLE_XVR_REG15, 0x07233AE7);
	REG_WRITE(BLE_XVR_REG16, 0x000103A5);
	REG_WRITE(BLE_XVR_REG17, 0x00000016);
	REG_WRITE(BLE_XVR_REG18, 0x00000000);
	REG_WRITE(BLE_XVR_REG20, 0x3BA7A940);
	REG_WRITE(BLE_XVR_REG21, 0x96000000);
	REG_WRITE(BLE_XVR_REG22, 0x78000000);
	REG_WRITE(BLE_XVR_REG23, 0xA0000000);
	REG_WRITE(BLE_XVR_REG24, 0x001E0782);
	REG_WRITE(BLE_XVR_REG25, 0x00000000);
	REG_WRITE(BLE_XVR_REG26, 0x10200400);
	REG_WRITE(BLE_XVR_REG27, 0x0008C900);
	REG_WRITE(BLE_XVR_REG28, 0x01011010);
	REG_WRITE(BLE_XVR_REG29, 0x7C104E00);
	REG_WRITE(BLE_XVR_REG2A, 0x12084044);
	REG_WRITE(BLE_XVR_REG2B, 0x00000408);
	REG_WRITE(BLE_XVR_REG2C, 0x00000000);
	REG_WRITE(BLE_XVR_REG2D, 0x082AC444);
	//REG_WRITE(BLE_XVR_REG2E, 0x00000000);
	//REG_WRITE(BLE_XVR_REG2F, 0x00000000);
	REG_WRITE(BLE_XVR_REG30, 0xA0419980);//<11:10>=2 by qunshan20201228 for rx
	REG_WRITE(BLE_XVR_REG31, 0x00000000);
	REG_WRITE(BLE_XVR_REG32, 0x00000000);
	REG_WRITE(BLE_XVR_REG33, 0x00000000);
	//REG_WRITE(BLE_XVR_REG34, 0x00000000);
	//REG_WRITE(BLE_XVR_REG35, 0x00000000);
	//REG_WRITE(BLE_XVR_REG36, 0x00000000);
	//REG_WRITE(BLE_XVR_REG37, 0x00000000);
	REG_WRITE(BLE_XVR_REG38, 0x00000000);
	REG_WRITE(BLE_XVR_REG39, 0x00000000);
	REG_WRITE(BLE_XVR_REG3A, 0x01028000);
	REG_WRITE(BLE_XVR_REG3B, 0x3F341448); //<11:9>=2 by qunshan20201224 for rx
	REG_WRITE(BLE_XVR_REG3C, 0x26771C01);
	REG_WRITE(BLE_XVR_REG3D, 0x00000000);
	REG_WRITE(BLE_XVR_REG3E, 0x8908B950);//0908B942
	//REG_WRITE(BLE_XVR_REG3F, 0x00000000);
#if 0
	REG_WRITE(BLE_XVR_REG40, 0x02000100);
	REG_WRITE(BLE_XVR_REG41, 0x04000300);
	REG_WRITE(BLE_XVR_REG42, 0x06000500);
	REG_WRITE(BLE_XVR_REG43, 0x08000700);
	REG_WRITE(BLE_XVR_REG44, 0x0A000900);
	REG_WRITE(BLE_XVR_REG45, 0x0C000B00);
	REG_WRITE(BLE_XVR_REG46, 0x0E000D00);
	REG_WRITE(BLE_XVR_REG47, 0x10000F00);
	REG_WRITE(BLE_XVR_REG48, 0x12001100);
	REG_WRITE(BLE_XVR_REG49, 0x14001300);
	REG_WRITE(BLE_XVR_REG4A, 0x16001500);
	REG_WRITE(BLE_XVR_REG4B, 0x18001700);
	REG_WRITE(BLE_XVR_REG4C, 0x1A001900);
	REG_WRITE(BLE_XVR_REG4D, 0x1C001B00);
	REG_WRITE(BLE_XVR_REG4E, 0x1E001D00);
	REG_WRITE(BLE_XVR_REG4F, 0x20001F00);
#endif

#if 0
	REG_WRITE(BLE_XVR_REG40, 0x03000180);
	REG_WRITE(BLE_XVR_REG41, 0x06000480);
	REG_WRITE(BLE_XVR_REG42, 0x09000780);
	REG_WRITE(BLE_XVR_REG43, 0x0C000A80);
	REG_WRITE(BLE_XVR_REG44, 0x0F000D80);
	REG_WRITE(BLE_XVR_REG45, 0x12001080);
	REG_WRITE(BLE_XVR_REG46, 0x15001380);
	REG_WRITE(BLE_XVR_REG47, 0x18001680);
	REG_WRITE(BLE_XVR_REG48, 0x1B001980);
	REG_WRITE(BLE_XVR_REG49, 0x1E001C80);
	REG_WRITE(BLE_XVR_REG4A, 0x21001F80);
	REG_WRITE(BLE_XVR_REG4B, 0x24002280);
	REG_WRITE(BLE_XVR_REG4C, 0x27002580);
	REG_WRITE(BLE_XVR_REG4D, 0x2A002880);
	REG_WRITE(BLE_XVR_REG4E, 0x2D002B80);
	REG_WRITE(BLE_XVR_REG4F, 0x30002E80);
#endif

#if 0
	REG_WRITE(BLE_XVR_REG40, 0x01200090);
	REG_WRITE(BLE_XVR_REG41, 0x024001B0);
	REG_WRITE(BLE_XVR_REG42, 0x036002D0);
	REG_WRITE(BLE_XVR_REG43, 0x048003F0);
	REG_WRITE(BLE_XVR_REG44, 0x05A00510);
	REG_WRITE(BLE_XVR_REG45, 0x06C00630);
	REG_WRITE(BLE_XVR_REG46, 0x07E00750);
	REG_WRITE(BLE_XVR_REG47, 0x09000870);
	REG_WRITE(BLE_XVR_REG48, 0x0A200990);
	REG_WRITE(BLE_XVR_REG49, 0x0B400AB0);
	REG_WRITE(BLE_XVR_REG4A, 0x0C600BD0);
	REG_WRITE(BLE_XVR_REG4B, 0x0D800CF0);
	REG_WRITE(BLE_XVR_REG4C, 0x0EA00E10);
	REG_WRITE(BLE_XVR_REG4D, 0x0FC00F30);
	REG_WRITE(BLE_XVR_REG4E, 0x10E01050);
	REG_WRITE(BLE_XVR_REG4F, 0x12001170);
#endif
#endif
}

#if defined(CFG_BLE)

static void rf_force_agc_enable(bool en)
{
	#if defined(CFG_BT)
    ip_radiocntl1_forceagc_en_setf(en);
    #else
    ip_radiocntl1_forceagc_en_setf(en);
    #endif //CFG_BLE

    ble_radiocntl1_forceagc_en_setf(en);

}
#endif //CFG_BLE

void rf_init(struct rwip_rf_api *api)
{
	// Initialize the RF driver API structure
	api->reg_rd = rf_rpl_reg_rd;
	api->reg_wr = rf_rpl_reg_wr;
	api->txpwr_dbm_get = rf_txpwr_dbm_get;
	api->txpwr_min = RPL_POWER_MIN;
	api->txpwr_max = RPL_POWER_MAX;
	api->sleep = rf_sleep;
	api->reset = rf_reset;

#if defined(CFG_BLE)
	api->force_agc_enable = rf_force_agc_enable;
#endif //CFG_BLE

	api->rssi_convert = rf_rssi_convert;
	api->txpwr_cs_get = rf_txpwr_cs_get;

#if defined(CFG_BT)
	api->txpwr_dec = rf_txpwr_dec;
	api->txpwr_inc = rf_txpwr_inc;
	api->txpwr_max_set = txpwr_max_set;
	// Initialize the RSSI thresholds (high, low, interference)
	// These are 'real' signed values in dBm
	if (   (rwip_param.get(PARAM_ID_RSSI_HIGH_THR, &length, (uint8_t*)&api->rssi_high_thr) != PARAM_OK)
		|| (rwip_param.get(PARAM_ID_RSSI_LOW_THR, &length, (uint8_t*)&api->rssi_low_thr) != PARAM_OK)
		|| (rwip_param.get(PARAM_ID_RSSI_INTERF_THR, &length, (uint8_t*)&api->rssi_interf_thr) != PARAM_OK) )
	{
		api->rssi_high_thr = (int8_t)RPL_RSSI_20dB_THRHLD;
		api->rssi_low_thr = (int8_t)RPL_RSSI_60dB_THRHLD;
		api->rssi_interf_thr = (int8_t)RPL_RSSI_70dB_THRHLD;
	}
#endif //CFG_BT

	bk_printf("xvr_reg_init\r\n");
	rf_em_init();
	xvr_reg_init();

	ip_radiocntl1_pack(/*uint8_t  forceagcen*/		 0,
							/*uint8_t  forceiq*/		 0,
							/*uint8_t  rxdnsl*/ 		 0,
							/*uint8_t  txdnsl*/ 		 0,
							/*uint16_t forceagclength*/  0,
							/*uint8_t  syncpulsemode*/	 0,
							/*uint8_t  syncpulsesrc*/	 0,
							/*uint8_t  dpcorren*/		 0,
							/*uint8_t  jefselect*/		 0,
							/*uint8_t  xrfsel*/ 		 0x02,
							/*uint8_t  subversion*/ 	 0x0);
                            
        
	ip_radiocntl1_set(0x00000020);
	//uart_printf("ip RADIOCNTL1 addr:0x%08x,val:0x%08x\r\n",ip_RADIOCNTL1_ADDR,ip_radiocntl1_get());
	ip_timgencntl_set(0x01df0120);		////Beken,
	//uart_printf("ip_TIMGENCNTL addr:0x%08x,val:0x%08x\r\n",ip_TIMGENCNTL_ADDR,ip_timgencntl_get());
#if defined(CFG_BLE)
	//uart_printf("RW BLE reg init\r\n");
	/* BLE RADIOCNTL2 */
	ble_radiocntl2_pack(/*uint8_t  lrsynccompmode*/ 0x0,
						/*uint8_t  rxcitermbypass*/ 0x0,
						/*uint8_t  lrvtbflush*/ 	0x8,
						/*uint8_t  phymsk*/ 		0x2, // mark that Coded phy are supported
						/*uint8_t  lrsyncerr*/		0,
						/*uint8_t  syncerr*/		0,
						/*uint16_t freqtableptr*/	(EM_FT_OFFSET >> 2));
	ble_radiocntl2_set(0x00C000C0);
	//uart_printf("BLE_RADIOCNTL2 addr:0x%08x,val:0x%08x\r\n",BLE_RADIOCNTL2_ADDR,ble_radiocntl2_get());

	/* BLE RADIOPWRUPDN0 */
	ble_radiopwrupdn0_pack(/*uint8_t syncposition0*/ 0,
						   /*uint8_t rxpwrup0*/ 	 0x50,
						   /*uint8_t txpwrdn0*/ 	 0x07,
						   /*uint8_t txpwrup0*/ 	 0x55);
	ble_radiopwrupdn0_set(0x00650065);
	//uart_printf("BLE_RADIOPWRUPDN0 addr:0x%08x,val:0x%08x\r\n",BLE_RADIOPWRUPDN0_ADDR,ble_radiopwrupdn0_get());

	/* BLE RADIOPWRUPDN1 */
	ble_radiopwrupdn1_pack(/*uint8_t syncposition1*/ 0,
						   /*uint8_t rxpwrup1*/ 	 0x70,
						   /*uint8_t txpwrdn0*/ 	 0x00,
						   /*uint8_t txpwrup1*/ 	 0x65);
	ble_radiopwrupdn1_set(0x00700065);
	// uart_printf("BLE_RADIOPWRUPDN1 addr:0x%08x,val:0x%08x\r\n",BLE_RADIOPWRUPDN1_ADDR,ble_radiopwrupdn1_get());

	/* BLE RADIOPWRUPDN2 */      
	ble_radiopwrupdn2_pack(/*uint8_t syncposition2*/ 0,
						   /*uint8_t rxpwrup2*/ 	 0x50, // 50
						   /*uint8_t txpwrdn2*/ 	 0x07,
						   /*uint8_t txpwrup2*/ 	 0x55);
	ble_radiopwrupdn2_set(0x00650065);
	//uart_printf("BLE_RADIOPWRUPDN2 addr:0x%08x,val:0x%08x\r\n",BLE_RADIOPWRUPDN2_ADDR,ble_radiopwrupdn2_get());


	/* BLE RADIOPWRUPDN3 */
	ble_radiopwrupdn3_pack(/*uint8_t txpwrdn3*/ 	 0x07,
						   /*uint8_t txpwrup3*/ 	 0x55);
	ble_radiopwrupdn3_set(0x00000065);
	//uart_printf("BLE_RADIOPWRUPDN3 addr:0x%08x,val:0x%08x\r\n",BLE_RADIOPWRUPDN3_ADDR,ble_radiopwrupdn3_get());

	/* BLE RADIOTXRXTIM0 */
	ble_radiotxrxtim0_pack(/*uint8_t rfrxtmda0*/   0,
						   /*uint8_t rxpathdly0*/  0x6,
						   /*uint8_t txpathdly0*/  0x6);
	ble_radiotxrxtim0_set(0x00000606);  //0x00001007
	//uart_printf("BLE_RADIOTXRXTIM0 addr:0x%08x,val:0x%08x\r\n",BLE_RADIOTXRXTIM0_ADDR,ble_radiotxrxtim0_get());

	/* BLE RADIOTXRXTIM1 */
	ble_radiotxrxtim1_pack(/*uint8_t rfrxtmda1*/ 0x00,
						   /*uint8_t rxpathdly1*/	   0x04,
						   /*uint8_t txpathdly1*/	   0x04);
	ble_radiotxrxtim1_set(0x00000404);
	//uart_printf("BLE_RADIOTXRXTIM1 addr:0x%08x,val:0x%08x\r\n",BLE_RADIOTXRXTIM1_ADDR,ble_radiotxrxtim1_get());


	/* BLE RADIOTXRXTIM2 */
	// uart_printf("# 09\r\n");
	ble_radiotxrxtim2_pack(/*uint8_t rxflushpathdly2*/ 0x10,
						   /*uint8_t rfrxtmda2*/	   0x00,
						   /*uint8_t rxpathdly2*/	   0x49,
						   /*uint8_t txpathdly2*/	   0x03);
	ble_radiotxrxtim2_set(0x00002020);
	//uart_printf("BLE_RADIOTXRXTIM2 addr:0x%08x,val:0x%08x\r\n",BLE_RADIOTXRXTIM2_ADDR,ble_radiotxrxtim2_get());

	/* BLE RADIOTXRXTIM3 */
	ble_radiotxrxtim3_pack(/*uint8_t rxflushpathdly3*/ 0x10,
						   /*uint8_t rfrxtmda3*/	   0x00,
						   /*uint8_t txpathdly3*/	   0x03);
	ble_radiotxrxtim3_set(0x00000020);                       
	//uart_printf("BLE_RADIOTXRXTIM3 addr:0x%08x,val:0x%08x\r\n",BLE_RADIOTXRXTIM3_ADDR,ble_radiotxrxtim3_get());

#if (BLE_CON_CTE_REQ | BLE_CONLESS_CTE_RX)
	// Init the DF CNTL
	ble_dfcntl0_1us_pack(/*uint8_t rxsampstinst01us*/ 0x08, /*uint8_t rxswstinst01us*/ 0x18, /*uint8_t txswstinst01us*/ 0x19);
	ble_dfcntl0_2us_pack(/*uint8_t rxsampstinst02us*/ 0x08, /*uint8_t rxswstinst02us*/ 0x18, /*uint8_t txswstinst02us*/ 0x19);
	ble_dfcntl1_1us_pack(/*uint8_t rxsampstinst11us*/ 0x08, /*uint8_t rxswstinst11us*/ 0x18, /*uint8_t txswstinst11us*/ 0x19);
	ble_dfcntl1_2us_pack(/*uint8_t rxsampstinst12us*/ 0x08, /*uint8_t rxswstinst12us*/ 0x18, /*uint8_t txswstinst12us*/ 0x19);
	ble_dfantcntl_pack(/*uint8_t rxprimidcntlen*/ 1, /*uint8_t rxprimantid*/ 0, /*uint8_t txprimidcntlen*/ 1, /*uint8_t txprimantid*/ 0);
#endif // (BLE_CON_CTE_REQ | BLE_CONLESS_CTE_RX)
#endif // defined CFG_BLE

#if defined(CFG_BT)

	/* EDRCNTL */
#if 1		////BEKEN
	bt_rwbtcntl_set(0x0000010A);
#else
	bt_rwbtcntl_nwinsize_setf(0);
#endif
	bt_edrcntl_rxgrd_timeout_setf(RPL_EDRCNTL);

	/* BT RADIOPWRUPDN */
#if 1		//// BEKEN
	bt_radiopwrupdn_set(0x00710271);
#else
	bt_radiopwrupdn_rxpwrupct_setf(0x42);
	bt_radiopwrupdn_txpwrdnct_setf(0x07);
	bt_radiopwrupdn_txpwrupct_setf(0x56);
#endif
	/* BT RADIOCNTL 2 */
#if 1		////BEKEN
	//uart_printf("# 14\r\n");
	bt_radiocntl2_set(0x04070100);
#else
	bt_radiocntl2_freqtable_ptr_setf((EM_FT_OFFSET >> 2));
	bt_radiocntl2_syncerr_setf(0x7);
#endif
	/* BT RADIOTXRXTIM */
#define PRL_TX_PATH_DLY 4
#define PRL_RX_PATH_DLY (RPL_RADIO_SKEW - PRL_TX_PATH_DLY)
#if 1		////BEKEN
	//uart_printf("# 15\r\n");
	bt_radiotxrxtim_set(0x00000403);
#else
	bt_radiotxrxtim_rxpathdly_setf(PRL_RX_PATH_DLY);
	bt_radiotxrxtim_txpathdly_setf(PRL_TX_PATH_DLY);
	bt_radiotxrxtim_sync_position_setf(0x38); // Default is 0x10
#endif
	/* BT RADIOCNTL 3*/
#if 1		////BEKEN
	//uart_printf("# 16\r\n");
	bt_radiocntl3_set(0x39003900);
#else
	bt_radiocntl3_pack( /*uint8_t rxrate2cfg*/	  3,
						/*uint8_t rxrate1cfg*/	  2,
						/*uint8_t rxrate0cfg*/	  1,
						/*uint8_t rxserparif*/	  0,
						/*uint8_t rxsyncrouting*/ 0,
						/*uint8_t rxvalidbeh*/	  0,
						/*uint8_t txrate2cfg*/	  3,
						/*uint8_t txrate1cfg*/	  2,
						/*uint8_t txrate0cfg*/	  1,
						/*uint8_t txserparif*/	  0,
						/*uint8_t txvalidbeh*/	  0);
#endif
#endif //CFG_BT

	// Settings for proper reception
#if defined(CFG_BLE)
	ip_radiocntl1_forceiq_setf(1);
	ip_radiocntl1_dpcorr_en_setf(0x0);
	BLE_ASSERT_ERR(ip_radiocntl1_dpcorr_en_getf() == 0x0);
#endif // CFG_BLE

#if defined(CFG_BT)
	ip_radiocntl1_dpcorr_en_setf(0x1);
	BLE_ASSERT_ERR(ip_radiocntl1_dpcorr_en_getf() == 0x1);
#endif // CFG_BT

#if defined(CFG_BLE)
	// Force IQ mode for BLE only
	ip_radiocntl1_forceiq_setf(1);
#endif //CFG_BLE
	
}

void Delay_us(int num)
{
    int x, y;
    for(y = 0; y < num; y ++ )
    {
        for(x = 0; x < 10; x++);
    }
}

void Delay(int num)
{
    int x, y;
    for(y = 0; y < num; y ++ )
    {
        for(x = 0; x < 50; x++);
    }
}

void Delay_ms(int num) //sync from svn revision 18
{
    int x, y;
    for(y = 0; y < num; y ++ )
    {
        for(x = 0; x < 3260; x++);
    }

}

