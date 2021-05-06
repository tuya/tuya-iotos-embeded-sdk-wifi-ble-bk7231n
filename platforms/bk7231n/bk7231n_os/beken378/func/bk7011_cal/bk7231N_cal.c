
#include "include.h"
#if (CFG_SOC_NAME == SOC_BK7231N)

#include "arm_arch.h"

#include "target_util_pub.h"
#include "mem_pub.h"

#include "drv_model_pub.h"
#include "sys_ctrl_pub.h"
#include "phy.h"

#include "bk7011_cal_pub.h"
#include "bk7011_cal.h"

#include <string.h>
#include "flash_pub.h"
#ifdef INCLUDE_OS
#include "cmd_evm.h"
#include "temp_detect_pub.h"
#endif
#include "power_save_pub.h"
#include "gpio.h"
#include "gpio_pub.h"

#include "icu_pub.h"
#include "saradc_pub.h"
#include "sys_ctrl.h"

#define CAL_RESULT_TO_FLASH		0
#define CAL_RESULT_FLASH_ADDR		0x000F1000UL

#define RCB_POWER_TABLE_ADDR        0x01050200

/* 12 bits, [-2^11, 2^11]==>[0x800, 0x7FF]==>[-2048,2047] */
#define BK_TX_DAC_SIGNED_MIN 0x800
#define BK_TX_DAC_SIGNED_MAX 0x7FF
#define BK_TX_DAC_UNSIGNED_MAX 0x1000
#define BK_TX_DAC_UNSIGNED_MASK 0xFFF
#define BK_TX_DAC_UNSIGNED_MASK_GAIN 0xFA0
#define BK_TX_DAC_COEF 4 // 2^(12-10) from 10 bits to 12 bits

#if CFG_SUPPORT_CALIBRATION

#define TX_WANTED_POWER_CAL            0
#define TX_IQ_POWER_CAL            1
#define TX_IQ_LOOPBACK_POWER_CAL            2
#define TX_DC_CAL       0
#define TX_DC_CAL_IQ       1
#define TX_DC_LOOPBACK_CAL_IQ            2
#define TX_GAIN_IMB_CAL            0
#define TX_GAIN_LOOPBACK_IMB_CAL            1
#define TX_PHASE_IMB_CAL            0
#define TX_PHASE_LOOPBACK_IMB_CAL            1


#define CAL_DEBUG          1
#include "uart_pub.h"
#if CAL_DEBUG
#define CAL_PRT      null_prf
#define CAL_WARN     null_prf// warning_prf
#define CAL_FATAL    null_prf
#define CAL_TIM_PRT null_prf
#define CAL_FLASH_PRT os_printf
#else
#define CAL_PRT      null_prf
#define CAL_WARN     null_prf
#define CAL_FATAL    null_prf
#define CAL_TIM_PRT null_prf
#define CAL_FLASH_PRT null_prf
#endif

extern void bk7011_cal_pll(void);
static void bk7011_cal_dcormod_save_base(INT32 mod);
static void bk7011_cal_dcormod_do_fitting(void);
static UINT8 bk7011_cal_dcormod_get(void);
void rwnx_set_tpc_txpwr_by_tmpdetect(INT16 shift_b, INT16 shift_g);
extern uint32_t get_ate_mode_state(void);

//static UINT8 pwr_nstep = 0;

#define BK7231_56PIN_BOARD  1
#define BK7231_32PIN_BOARD  2
#define BK7231_32PIN_TZH_BOARD  3
#define BK7231U_BOARD  4

//#define BOARD  BK7231_32PIN_TZH_BOARD
#define BOARD  BK7231U_BOARD

#define TRX_REG_0XA_VAL         0x80631DFF
#define TRX_REG_0XB_VAL         0xA9758ACE//0xAF758FCE//AF758FCE   A9758ACE //yongguang modify
#define TRX_REG_0XC_VAL         0x06782EAA //0x067827CC
#define TRX_REG_0XD_VAL         0xDDF90339//0xD9FB0339
#define TSSI_POUT_TH_G          0x7D//0xB5 actually, yongguang20200918 tune it for workaround
#define TSSI_POUT_TH_B          0xD0//0xE0 actually, yongguang20200918 tune it for workaround

#define POWER_TABLE_11B_1DB_STEP (2) /* refer to cfg_tab_b, 0.50db for each step */
#define POWER_TABLE_11G_1DB_STEP (4) /* refer to cfg_tab_g, 0.25db for each step */

#define POWER_CALI_11B_SHIFT (0) /* add 0db for 11b when auto calibration for tuya, should adjust this for each batch */
#define POWER_CALI_11G_SHIFT (0) /* add 2db for 11g when auto calibration for tuya, should adjust this for each batch */

#define TXIQ_IMB_TSSI_TH       0x35B// (0x840-0x180)   

#define TXIQ_IMB_TSSI_TH_LOOPBACK             60

#define No_TXPOWERCAL   0
#define TXIQ_TSSI_TH             0x40
#define TRX_REG_0XD_TX_IQ_VAL    0xDDFB0339   //0xD9FF0338   //0214  //wyg1/17
#define TRX_REG_0XD_TX_IQ_VAL_GAIN_PHASE    0xD9FB0339   //0xD9FF0338   //wyg1/17
#define TRX_REG_0XD_TX_LOOPBACK_IQ_VAL    0xD9FE7FF1   //0214   0xD9FE7FF1
#define TRX_REG_0XD_TX_FILTER_IQ_VAL    0xFC4E03B9

#define TRX_REG_0XC_TXLO_LEAKAGE_VAL     TRX_REG_0XC_VAL//0x5228100

#define TRX_REG_0XC_RXIQ_VAL    TRX_REG_0XC_VAL  // 1A244ED    //0215 increasse gain due to load antenna   01A244DD
#define TRX_REG_0XE_RXIQ_VAL    0xD9F87FF1   //0214  FDF87FF1


#define gi_dc_tx_pa_dgainPA30  1
#define gi_dc_tx_pa_dgainbuf30  7
#define gi_gain_tx_pa_dgainPA30  1
#define gi_gain_tx_pa_dgainbuf30  1

#define gi_dc_tx_loopback_pa_dgainPA30  7
#define gi_dc_tx_loopback_pa_dgainbuf30  6
#define gi_gain_tx_loopback_pa_dgainPA30  6
#define gi_gain_tx_loopback_pa_dgainbuf30  4
#define gi_phase_tx_loopback_pa_dgainPA30  6
#define gi_phase_tx_loopback_pa_dgainbuf30 4

#define gi_cal_rx_iq_pa_dgainPA30  4
#define gi_cal_rx_iq_pa_dgainbuf30  5

#define CALI_MODE_UNKNOWN       -1

static BK7011_CALI_CONTEXT gcali_context =
{
    2,              /* gtx_dc_n, the times of first dc cal. (64 * BK_TX_DAC_COEF)=2^8 */
    CAL_DELAY100US, /* gst_rx_adc */
    CAL_DELAY05US,  /* gst_sar_adc */

    CALI_MODE_UNKNOWN, /* cali_mode */
    TSSI_POUT_TH_B,   /* gtx_tssi_thred_b */
    TSSI_POUT_TH_G,   /* gtx_tssi_thred_g */

    0,              /* is_tpc_used */
        
#if CFG_USE_TEMPERATURE_DETECT
    0,              /* ble_pwr_indx */
    0,              /* ble_pwr_shift */
#endif
};

static BK7011_CALI_RESULT gcali_result =
{
    0,                       /* gbias_after_cal */
    0,                       /* gav_tssi */

    BK_TX_DAC_SIGNED_MIN,    /* gtx_i_dc_comp */
    BK_TX_DAC_SIGNED_MIN,    /* gtx_q_dc_comp */
    BK_TX_DAC_SIGNED_MIN + (BK_TX_DAC_COEF * 2),    /* gtx_i_dc_comp_loopback */
    BK_TX_DAC_SIGNED_MAX,    /* gtx_q_dc_comp_loopback */

    BK_TX_DAC_UNSIGNED_MASK, /* gtx_i_gain_comp */
    BK_TX_DAC_UNSIGNED_MASK, /* gtx_q_gain_comp */
    BK_TX_DAC_UNSIGNED_MASK, /* gtx_i_gain_comp_loopback */
    BK_TX_DAC_UNSIGNED_MASK - 12 * BK_TX_DAC_COEF, /* gtx_q_gain_comp_loopback */

    BK_TX_DAC_SIGNED_MIN,    /* gtx_phase_comp */
    BK_TX_DAC_SIGNED_MIN,    /* gtx_phase_ty2 */
    BK_TX_DAC_SIGNED_MIN,    /* gtx_phase_comp_loopback */
    BK_TX_DAC_SIGNED_MIN,    /* gtx_phase_ty2_loopback */

    0x10,                    /* gtx_ifilter_corner */
    0x10,                    /* gtx_qfilter_corner */

    0x400,                   /* const_iqcal_p */ //0x3B0

    BK_TX_DAC_SIGNED_MIN,    /* grx_amp_err_wr */
    0x041 * BK_TX_DAC_COEF,  /* grx_phase_err_wr */

#ifdef SCAN_IQ_FILTER_CORNER
    0,                       /* g_sctrl_bias_reg */
    0,                       /* grx_avg_i_14M */
    0,                       /* grx_avg_i_14M2 */
    0,                       /* grx_avg_q_14M */
    0,                       /* grx_avg_q_14M2 */
    {
        0,
        0,
        0,
        0,
        0,
    },                       /* index_test */
    {
        0.0,
        0.0,
        0.0,
        0.0,
        0.0,
    },                       /* float2_test */
#endif
    
    0,                       /* rx_amp_err_rd */
    0,                       /* rx_phase_err_rd */
    0,                       /* rx_ty2_rd */

    {
        0x807C807C,
        0x80788078,
        0x80788078,
        0x88688870,
        0x88688868,
        0x88688868,
        0x88688868,
        0x88688868
    },                       /* g_rx_dc_gain_tab */
};

const static UINT32 BK7231N_TRX_ROM[] = {
    0x04138462,                      /* 00-R0x00, //       */
#if (CFG_XTAL_FREQUENCE == CFG_XTAL_FREQUENCE_40M)
    0xAFBBC311,                      /* 01-R0x01, //AE09C081   26M spur    0xF259347B;*/
    0xCFA28802,                      /* 02-R0x02, //       0xC3A28800*/
    0x000023E4,                      /* 03-R0x03, //   0x00002164  0x000023E4  */
    0x65C89655,                      /* 04-R0x04, //       */
    0x18C031AA,                      /* 05-R0x05, //       */
#else
    0xFE09C189,                      /* 01-R0x01, //AE09C081   26M spur    0xF259347B;*/ //wanjian20200907 0xAE09C181->0xFE09C189
    0xCFA28800,                      /* 02-R0x02, //       0xC3A28800*/
    0x000023E4,                      /* 03-R0x03, //   0x00002164  0x000023E4  */ //wangjian20200828 0x00002164->0x000023E4
    0x65C89659,                      /* 04-R0x04, //       */ //wanjian20200907 0x65C89655->0x65C89659
    0xA8C030AA,                      /* 05-R0x05, //       */
#endif
    0x727A0004,                      /* 06-R0x06, //       */
    0x0226826A,                      /* 07-R0x07, // 00F5:4th;20F5:2nd      *///wyg F7E6026B //desheng20200827 0x57E6826B //guo20200831 <26:22>=8
    0x28A8088A,                      /* 08-R0x08, //       */ //wangjian20200822 0x082C091F->0x08AC091F //wangjian20200828 0x08AC088A //huamin20200902 <29>=1
    0x001E077E,                      /* 09-R0x09, //       */ //wangjian20200822 0x001E676F->0x001E076F //huamin20200915 <4:3>=3
    TRX_REG_0XA_VAL,                 /* 10-R0x0A, //       */
    TRX_REG_0XB_VAL,                 /* 11-R0x0B, //       */
    TRX_REG_0XC_VAL,                 /* 12-R0x0C, //       */
    TRX_REG_0XD_VAL,                 /* 13-R0x0D, //       */
    0xDA01BCF0,                      /* 14-R0x0E, //       */
    0x00008000,                      /* 15-R0x0F, //       */
    0xD8C00130,                      /* 16-R0x10, //0xD8800130       */
    0x00000000,                      /* 17-R0x11, //       */
    0x47390195,                      /* 18-R0x12, // 0xF0090481      */ //wangjiang20200822 0x473AC015->0x473AC195  //desheng20200827 0x473AC195->47390195
    0x7B305ECC,                      /* 19-R0x13, //       */
    0x807C807C,                      /* 20-R0x14, //       */
    0x80788078,                      /* 21-R0x15, //       */
    0x80788078,                      /* 22-R0x16, //       */
    0x88688870,                      /* 23-R0x17, //       */
    0x88688868,                      /* 24-R0x18, //       */
    0x88688868,                      /* 25-R0x19, //       */
    0x88688868,                      /* 26-R0x1A, //       */
    0x88688868,                      /* 27-R0x1B, //       */
    0x10F8FFA7                       /* 28-R0x1C, //       */
};

const static UINT32 BK7231N_RC_ROM[] = {
    0x00000009,                      /* 00-R0x0,  0x0000;  */
    0xF0000000,                      /* 01-R0x1,  0x0004;  */
    0x00000030,                      /* 02-R0x5,  0x0014;  */
    0x00010001,                      /* 03-R0x8,  0x0020;  */
    0x000100e0,                      /* 04-R0xB,  0x002C;  */
    0x00010070,                      /* 05-R0xE,  0x0038;  */
    0x00010001,                      /* 06-R0x11, 0x0044;  */
    0x00010005,                      /* 07-R0x19, 0x0064;  */
    TRX_REG_0XD_VAL,                 /* 07-R0x1A, 0x0068; the same with BK7231N_TRX_RAM.REG0xD */
    0x00000002,                      /* 08-R0x1C, 0x0070;  */
    0x0000012c,                      /* 09-R0x1E, 0x0078;  */
    0x10F8FFA7,                      /* 10-R0x3C, 0x00F0;  */
    0x00F8FFA7,                      /* 11-R0x3E, 0x00F8;  */
    0x00BA0201,                      /* 12-R0x3F, 0x00FC;  */
    0x00000095,                      /* 13-R0x40, 0x0100;  */
    0x00000000,                      /* 14-R0x41, 0x0104;  */
    0x03570108,                      /* 15-R0x42, 0x0108;  */
    0x00000200,                      /* 15-R0x43, 0x010C;  */
    0x08000800,                      /* 16-R0x4C, 0x0130;  */
    0x2CC02000,                      /* 17-R0x4D, 0x0134;  */
    0x00000001,                      /* 25-R0x4E, 0x0138;  */
    0x07B507DE,                      /* 18-R0x4F, 0x013C;  */
    0x0FFF0FFF,                      /* 19-R0x50, 0x0140;  */
    0x08000800,                      /* 20-R0x51, 0x0144;  */
    0xC4008800,                      /* 21-R0x52, 0x0148;  */
   (0x007456A8|((TSSI_POUT_TH_G)<<1)), /* 22-R0x54, 0x0150;  */
    0x00000000,                      /* 23-R0x55, 0x0154;  */
    0x00000000,                      /* 24-R0x56, 0x0158;  */
    0x80B2E065,                      /* 25-R0x57, 0x015C;  */
    0x00000000,                      /* 26-R0x5A, 0x0168;  */
    0x00000000,                      /* 27-R0x5B, 0x016C;  */
    0x80000064,                      /* 24-R0x5C, 0x0170;  */
    0x00000000,                      /* 28-R0x6A, 0x01A8;  */
};

static BK7231N_TRX_TypeDef BK7231N_TRX_RAM;
static BK7231N_RC_TypeDef BK7231N_RC_RAM;

INT32 gconst_pout = BK_TX_DAC_SIGNED_MIN + BK_TX_DAC_COEF * 0x10;

//INT32 gpll_lock_err = 1;

//INT32 gadda_map[6];
//INT32 gdc_goldval[32]= {0};
//INT32 g_tri_offset = 0;

INT32 gtx_dcorMod = 0x4;
INT32 gtx_dcorPA = 0x8;
INT32 gtx_pre_gain = 0x10;

INT32 gtx_dcorMod_temp = 0x4;
INT32 gtx_dcorPA_temp = 0x8;
INT32 gtx_pre_gain_temp = 0x7;

INT32 gtx_dcorMod_temp_loopback = 0x4;
INT32 gtx_dcorPA_temp_loopback = 0x8;
INT32 gtx_pre_gain_temp_loopback = 0x7;

struct cal_pwr_st 
{
    UINT8 idx;
    UINT8 mode;    
};

struct cal_pwr_st g_pwr_current;

#include "net_param_pub.h"

#define PWRI(gain, rb_28_31, pactrl, padctrl, rc_0_2, ra_8_13, ra_4_7, ra_0_1)      \
{                                     \
     .unuse      = 0,            \
     .pregain    = gain,         \
     .regc_8_10  = pactrl,      \
     .regc_4_6   = padctrl,       \
} 

#define TCP_PAMAP_TAB_B_LEN             (48)
const static PWR_REGS cfg_tab_b[TCP_PAMAP_TAB_B_LEN] = {
    // pregain REGB<31:28> REGC<10:8> REGC<6:4> REGC<2:0> REGA<13:8> REGA<7:4> REGA<1:0>
PWRI(  0x83C ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 0   7.5  dBm
PWRI(  0x83F ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 1   8  dBm
PWRI(  0x843 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 2   8.5  dBm
PWRI(  0x847 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 3   9  dBm
PWRI(  0x84B ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 4   9.5  dBm
PWRI(  0x84F ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 5   10  dBm
PWRI(  0x854 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 6   10.5  dBm
PWRI(  0x859 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 7   11  dBm
PWRI(  0x85E ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 8   11.5  dBm
PWRI(  0x864 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 9   12  dBm
PWRI(  0x86A ,      0xA,       7,        4,         3,      0x20,     0x0,     0x2   ),   // 10  12.5  dBm
PWRI(  0x870 ,      0xA,       7,        4,         3,      0x20,     0x0,     0x2   ),   // 11  13  dBm
PWRI(  0x877 ,      0xA,       7,        4,         3,      0x20,     0x0,     0x2   ),   // 12  13.5  dBm
PWRI(  0x87E ,      0xA,       7,        4,         3,      0x20,     0x0,     0x2   ),   // 13  14  dBm
PWRI(  0x885 ,      0xA,       7,        4,         3,      0x20,     0x0,     0x2   ),   // 14  14.5  dBm
PWRI(  0x88D ,      0xA,       7,        4,         3,      0x20,     0x0,     0x2   ),   // 15  15  dBm
PWRI(  0x896 ,      0xA,       7,        5,         5,      0x20,     0x0,     0x2   ),   // 16  15.5  dBm
PWRI(  0x89F ,      0xA,       7,        5,         5,      0x20,     0x0,     0x2   ),   // 17  16  dBm
PWRI(  0x8A8 ,      0xA,       7,        5,         5,      0x20,     0x0,     0x2   ),   // 18  16.5  dBm
PWRI(  0x8B2 ,      0xA,       7,        5,         5,      0x20,     0x0,     0x2   ),   // 19  17  dBm
PWRI(  0x8BC ,      0xA,       7,        5,         5,      0x20,     0x0,     0x2   ),   // 20  17.5  dBm
PWRI(  0x8C8 ,      0xA,       7,        5,         5,      0x20,     0x0,     0x2   ),   // 21  18  dBm
PWRI(  0x8D3 ,      0xA,       7,        5,         5,      0x20,     0x0,     0x2   ),   // 22  18.5  dBm
PWRI(  0x8E0 ,      0xA,       7,        5,         5,      0x20,     0x0,     0x2   ),   // 23  19  dBm
PWRI(  0x8ED ,      0xA,       7,        5,         5,      0x20,     0x0,     0x2   ),   // 24  19.5  dBm
PWRI(  0x8FB ,      0xA,       7,        5,         5,      0x20,     0x0,     0x2   ),   // 25  20  dBm
PWRI(  0x90A ,      0xA,       7,        5,         5,      0x20,     0x0,     0x2   ),   // 26  20.5  dBm
PWRI(  0x91A ,      0xA,       7,        5,         5,      0x20,     0x0,     0x2   ),   // 27  21  dBm
PWRI(  0x92B ,      0xA,       7,        5,         5,      0x20,     0x0,     0x2   ),   // 28  21.5  dBm
PWRI(  0x93C ,      0xA,       7,        5,         5,      0x20,     0x0,     0x2   ),   // 29  22  dBm
PWRI(  0x94F ,      0xA,       7,        6,         5,      0x20,     0x0,     0x2   ),   // 30  22.5  dBm
PWRI(  0x963 ,      0xA,       7,        6,         6,      0x20,     0x0,     0x2   ),   // 31  23  dBm
PWRI(  0x978 ,     0xA,       7,        4,         4,      0x20,       0x1,     0x1   ),   // 32  23.5  dBm
PWRI(  0x98E ,     0xA,       7,        4,         4,      0x20,       0x1,     0x1   ),   // 33  24  dBm
PWRI(  0x9A6 ,     0xA,       7,        5,         4,      0x20,       0x1,     0x1   ),   // 34  24.5  dBm
PWRI(  0x9BF ,     0xA,       7,        5,         4,      0x20,       0x1,     0x1   ),   // 35  25  dBm
PWRI(  0x9D9 ,     0xA,       7,        5,         4,      0x20,       0x1,     0x1   ),   // 36  25.5  dBm
PWRI(  0x9F5 ,     0xA,       7,        5,         4,      0x20,       0x1,     0x1   ),   // 37  26  dBm
PWRI(  0xA13 ,     0xA,       7,        5,         5,      0x20,       0x1,     0x1   ),   // 38  26.5  dBm
PWRI(  0xA33 ,     0xA,       7,        5,         5,      0x20,       0x1,     0x1   ),   // 39  27  dBm
PWRI(  0xA54 ,     0xA,       7,        5,         5,      0x20,       0x1,     0x1   ),   // 40  27.5  dBm
PWRI(  0xA77 ,     0xA,       7,        5,         5,      0x20,       0x1,     0x1   ),   // 41  28  dBm
PWRI(  0xA9D ,     0xA,       7,        5,         5,      0x20,       0x1,     0x1   ),   // 42  28.5  dBm
PWRI(  0xAC4 ,     0xA,       7,        5,         5,      0x20,       0x1,     0x1   ),   // 43  29  dBm
PWRI(  0xAEE ,     0xA,       7,        5,         5,      0x20,       0x1,     0x1   ),   // 44  29.5  dBm
PWRI(  0xB1B ,     0xA,       7,        5,         5,      0x20,       0x1,     0x1   ),   // 45  30  dBm
PWRI(  0xB4A ,     0xA,       7,        5,         5,      0x20,       0x1,     0x1   ),   // 46  30.5  dBm
PWRI(  0xB7C ,     0xA,       7,        5,         5,      0x20,       0x1,     0x1   ),   // 47  31  dBm
};

#define TCP_PAMAP_TAB_G_LEN             (80)
const static PWR_REGS cfg_tab_g[TCP_PAMAP_TAB_G_LEN] = {
    // pregain REGB<31:28> REGC<10:8> REGC<6:4> REGC<2:0> REGA<13:8> REGA<7:4> REGA<1:0>
PWRI(  0x85E ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 0    2.5  dBm
PWRI(  0x860 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 1    2.75  dBm
PWRI(  0x863 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 2    3  dBm
PWRI(  0x866 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 3    3.25  dBm
PWRI(  0x869 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 4    3.5  dBm
PWRI(  0x86C ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 5    3.75  dBm
PWRI(  0x86F ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 6    4  dBm
PWRI(  0x873 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 7    4.25  dBm
PWRI(  0x876 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 8    4.5  dBm
PWRI(  0x879 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 9    4.75  dBm
PWRI(  0x87D ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 10   5  dBm
PWRI(  0x881 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 11   5.25  dBm
PWRI(  0x884 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 12   5.5  dBm
PWRI(  0x888 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 13   5.75  dBm
PWRI(  0x88C ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 14   6  dBm
PWRI(  0x890 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 15   6.25  dBm
PWRI(  0x895 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 16   6.5  dBm
PWRI(  0x899 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 17   6.75  dBm
PWRI(  0x89D ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 18   7  dBm
PWRI(  0x8A2 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 19   7.25  dBm
PWRI(  0x8A7 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 20   7.5  dBm
PWRI(  0x8AC ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 21   7.75  dBm
PWRI(  0x8B1 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 22   8  dBm
PWRI(  0x8B6 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 23   8.25  dBm
PWRI(  0x8BB ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 24   8.5  dBm
PWRI(  0x8C0 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 25   8.75  dBm
PWRI(  0x8C6 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 26   9  dBm
PWRI(  0x8CC ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 27   9.25  dBm
PWRI(  0x8D2 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 28   9.5  dBm
PWRI(  0x8D8 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 29   9.75  dBm
PWRI(  0x8DE ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 30   10  dBm
PWRI(  0x8E5 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 31   10.25  dBm
PWRI(  0x8EB ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 32   10.5  dBm
PWRI(  0x8F2 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 33   10.75  dBm
PWRI(  0x8F9 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 34   11  dBm

PWRI(  0x901 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 35   11.25  dBm
PWRI(  0x908 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 36   11.5  dBm
PWRI(  0x910 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 37   11.75  dBm
PWRI(  0x918 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 38   12  dBm
PWRI(  0x920 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 39   12.25  dBm
PWRI(  0x928 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 40   12.5  dBm
PWRI(  0x931 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 41   12.75  dBm
PWRI(  0x93A ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 42   13  dBm
PWRI(  0x943 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 43   13.25  dBm
PWRI(  0x94D ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 44   13.5  dBm
PWRI(  0x956 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 45   13.75  dBm
PWRI(  0x960 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 46   14  dBm
PWRI(  0x96B ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 47   14.25  dBm
PWRI(  0x975 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 48   14.5  dBm
PWRI(  0x980 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 49   14.75  dBm
PWRI(  0x98B ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 50   15  dBm
PWRI(  0x997 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 51   15.25  dBm
PWRI(  0x9A3 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 52   15.5  dBm
PWRI(  0x9AF ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 53   15.75  dBm
PWRI(  0x9BB ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 54   16  dBm
PWRI(  0x9C8 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 55   16.25  dBm
PWRI(  0x9D6 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 56   16.5  dBm
PWRI(  0x9E0 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 57   16.75  dBm
PWRI(  0x9F2 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 58   17  dBm
PWRI(  0xA00 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 59   17.25  dBm
PWRI(  0xA0F ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 60   17.5  dBm
PWRI(  0xA1E ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 61   17.75  dBm
PWRI(  0xA2E ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 62   18  dBm
PWRI(  0xA3F ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 63   18.25  dBm
PWRI(  0xA4F ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 64   18.5  dBm
PWRI(  0xA61 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 65   18.75  dBm
PWRI(  0xA72 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 66   19  dBm
PWRI(  0xA85 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 67   19.25  dBm
PWRI(  0xA98 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 68   19.5  dBm
PWRI(  0xAAB ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 69   19.75  dBm
PWRI(  0xABF ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 70   20  dBm
PWRI(  0xAD3 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 71   20.25  dBm
PWRI(  0xAE9 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 72   20.5  dBm
PWRI(  0xAFE ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 73   20.75  dBm
PWRI(  0xB15 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 74   21  dBm
PWRI(  0xB2C ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 75   21.25  dBm
PWRI(  0xB43 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 76   21.5  dBm
PWRI(  0xB5C ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 77   21.75  dBm
PWRI(  0xB75 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 78   22  dBm
PWRI(  0xB8F ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 79   22.25  dBm
};

#define TCP_PAMAP_TAB_BLE_LEN			(80)
const static PWR_REGS cfg_tab_ble[TCP_PAMAP_TAB_BLE_LEN] = 
{
    // pregain REGB<31:28> REGC<10:8> REGC<6:4> REGC<2:0> REGA<13:8> REGA<7:4> REGA<1:0>
PWRI(  0x80B ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 0   -7.5  dBm
PWRI(  0x81F ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 1   21  dBm
PWRI(  0x820 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 2   21.25  dBm
PWRI(  0x821 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 3   21.5  dBm
PWRI(  0x822 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 4   21.75  dBm
PWRI(  0x823 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 5   22  dBm
PWRI(  0x824 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 6   2.5  dBm
PWRI(  0x825 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 7   2.75  dBm
PWRI(  0x826 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 8   3  dBm
PWRI(  0x828 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 9   3.25  dBm
PWRI(  0x829 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 10   3.5  dBm
PWRI(  0x82A ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 11   3.75  dBm
PWRI(  0x82B ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 12   4  dBm
PWRI(  0x82C ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 13   4.25  dBm
PWRI(  0x82E ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 14   4.5  dBm
PWRI(  0x82F ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 15   4.75  dBm
PWRI(  0x830 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 16   5  dBm
PWRI(  0x832 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 17   5.25  dBm
PWRI(  0x833 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 18   5.5  dBm
PWRI(  0x835 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 19   5.75  dBm
PWRI(  0x836 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 20   6  dBm
PWRI(  0x838 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 21   6.25  dBm
PWRI(  0x839 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 22   6.5  dBm
PWRI(  0x83B ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 23   6.75  dBm
PWRI(  0x83D ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 24   7  dBm
PWRI(  0x83F ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 25   7.25  dBm
PWRI(  0x840 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 26   7.5  dBm
PWRI(  0x842 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 27   7.75  dBm
PWRI(  0x844 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 28   8  dBm
PWRI(  0x846 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 29   8.25  dBm
PWRI(  0x848 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 30   8.5  dBm
PWRI(  0x84A ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 31   8.75  dBm
PWRI(  0x84D ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 32   9  dBm
PWRI(  0x84F ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 33   9.25  dBm
PWRI(  0x851 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 34   9.5  dBm
PWRI(  0x854 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 35   9.75  dBm
PWRI(  0x856 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 36   10  dBm
PWRI(  0x858 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 37   10.25  dBm
PWRI(  0x85B ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 38   10.5  dBm
PWRI(  0x85E ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 39   10.75  dBm
PWRI(  0x860 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 40   11  dBm
PWRI(  0x863 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 41   11.25  dBm
PWRI(  0x866 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 42   11.5  dBm
PWRI(  0x869 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 43   11.75  dBm
PWRI(  0x86C ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 44   12  dBm
PWRI(  0x86F ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 45   12.25  dBm
PWRI(  0x873 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 46   12.5  dBm
PWRI(  0x876 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 47   12.75  dBm
PWRI(  0x879 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 48   13  dBm
PWRI(  0x87D ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 49   13.25  dBm
PWRI(  0x881 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 50   13.5  dBm
PWRI(  0x884 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 51   13.75  dBm
PWRI(  0x888 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 52   14  dBm
PWRI(  0x88C ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 53   14.25  dBm
PWRI(  0x890 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 54   14.5  dBm
PWRI(  0x895 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 55   14.75  dBm
PWRI(  0x899 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 56   15  dBm
PWRI(  0x89D ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 57   15.25  dBm
PWRI(  0x8A2 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 58   15.5  dBm
PWRI(  0x8A7 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 59   15.75  dBm
PWRI(  0x8AC ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 60   16  dBm
PWRI(  0x8B1 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 61   16.25  dBm
PWRI(  0x8B6 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 62   16.5  dBm
PWRI(  0x8BB ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 63   16.75  dBm
PWRI(  0x8C0 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 64   17  dBm
PWRI(  0x8C6 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 65   17.25  dBm
PWRI(  0x8CC ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 66   17.5  dBm
PWRI(  0x8D2 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 67   17.75  dBm
PWRI(  0x8D8 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 68   18  dBm
PWRI(  0x8DE ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 69   18.25  dBm
PWRI(  0x8E5 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 70   18.5  dBm
PWRI(  0x8EB ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 71   18.75  dBm
PWRI(  0x8F2 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 72   19  dBm
PWRI(  0x8F9 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 73   19.25  dBm
PWRI(  0x901 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 74   19.5  dBm
PWRI(  0x908 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 75   19.75  dBm
PWRI(  0x910 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 76   20  dBm
PWRI(  0x918 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 77   20.25  dBm
PWRI(  0x920 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 78   20.5  dBm
PWRI(  0x928 ,      0xA,       7,        3,         2,      0x20,     0x0,     0x2   ),   // 79   20.75  dBm
};

#define TCP_PAMAP_TAB_LEN               (TCP_PAMAP_TAB_B_LEN+TCP_PAMAP_TAB_G_LEN)
#define TCP_PAMAP_DEF_PREGAIN           (0x0)
#define TCP_PAMAP_DEF_A13_8             (0x20)
#define TCP_PAMAP_DEF_A7_4              (0x1)
#define TCP_PAMAP_DEF_A1_0              (0x1)

#define TPCI(rb_12_31, rc_0_11)  (rb_12_31<<12)|(rc_0_11)

const PWR_REGS_TPC cfg_tab_tpc[TCP_PAMAP_TAB_LEN] = {
    // 802.11b        index dbM
    TPCI(0xE0A2A, 0x83C),   // 0  5.00
    TPCI(0xE0A2A, 0x83F),   // 1  5.50
    TPCI(0xE0A2A, 0x843),   // 2  6.00
    TPCI(0xE0A2A, 0x847),   // 3  6.50
    TPCI(0xE0A2A, 0x84B),   // 4  7.00
    TPCI(0xE0A2A, 0x84F),   // 5  7.50
    TPCI(0xE0A2A, 0x854),   // 6  8.00
    TPCI(0xE0A2A, 0x859),   // 7  8.50
    TPCI(0xE0A2A, 0x85E),   // 8  9.00
    TPCI(0xE0A2A, 0x864),   // 9  9.50
    TPCI(0xE0A2A, 0x86A),   // 10 10.00
    TPCI(0xE0A2A, 0x870),   // 11 10.50
    TPCI(0xE0A2A, 0x877),   // 12 11.00
    TPCI(0xE0A2A, 0x87E),   // 13 11.50
    TPCI(0xE0A2A, 0x885),   // 14 12.00
    TPCI(0xE0A2A, 0x88D),   // 15 12.50
    TPCI(0xE0A2A, 0x896),   // 16 13.00
    TPCI(0xE0A2A, 0x89F),   // 17 13.50
    TPCI(0xE0A2A, 0x8A8),   // 18 14.00
    TPCI(0xE0A2A, 0x8B2),   // 19 14.50
    TPCI(0xE0A2A, 0x8BC),   // 20 15.00
    TPCI(0xE0A2A, 0x8C8),   // 21 15.50
    TPCI(0xE0A2A, 0x8D3),   // 22 16.00
    TPCI(0xE0A2A, 0x8E0),   // 23 16.50
    TPCI(0xE0A2A, 0x8ED),   // 24 17.00
    TPCI(0xE0A2A, 0x8FB),   // 25 17.50
    TPCI(0xE0A2A, 0x90A),   // 26 18.00
    TPCI(0xE0A2A, 0x91A),   // 27 18.50
    TPCI(0xE0A2A, 0x92B),   // 28 19.00
    TPCI(0xE0A2A, 0x93C),   // 29 19.50
    TPCI(0xE0A2A, 0x94F),   // 30 20.00
    TPCI(0xE0A2A, 0x963),   // 31 20.50
    TPCI(0xE0A2A, 0x978),   // 32 21.00
    TPCI(0xE0A2A, 0x98E),   // 33 21.50
    TPCI(0xE0A2A, 0x9A6),   // 34 22.00
    TPCI(0xE0A2A, 0x9BF),   // 35 22.50
    TPCI(0xE0A2A, 0x9D9),   // 36 23.00
    TPCI(0xE0A2A, 0x9F5),   // 37 23.50
    TPCI(0xE0A2A, 0xA13),   // 38 24.00
    TPCI(0xE0A2A, 0xA33),   // 39 24.50
    TPCI(0xE0A2A, 0xA54),   // 40 25.00
    TPCI(0xE0A2A, 0xA77),   // 41 25.50
    TPCI(0xE0A2A, 0xA9D),   // 42 26.00
    TPCI(0xE0A2A, 0xAC4),   // 43 26.50
    TPCI(0xE0A2A, 0xAEE),   // 44 27.00
    TPCI(0xE0A2A, 0xB1B),   // 45 27.50
    TPCI(0xE0A2A, 0xB4A),   // 46 28.00
    TPCI(0xE0A2A, 0xB7C),   // 47 28.50
    
    // 802.11g        index dbM
    TPCI(0xE0AB1, 0x85E),  //  0  2.5  dBm
    TPCI(0xE0AB1, 0x860),  //  1  2.75  dBm
    TPCI(0xE0AB1, 0x863),  //  2  3  dBm
    TPCI(0xE0AB1, 0x866),  //  3  3.25  dBm
    TPCI(0xE0AB1, 0x869),  //  4  3.5  dBm
    TPCI(0xE0AB1, 0x86C),  //  5  3.75  dBm
    TPCI(0xE0AB1, 0x86F),  //  6  4  dBm
    TPCI(0xE0AB1, 0x873),  //  7  4.25  dBm
    TPCI(0xE0AB1, 0x876),  //  8  4.5  dBm
    TPCI(0xE0AB1, 0x879),  //  9  4.75  dBm
    TPCI(0xE0AB1, 0x87D),  //  10  5  dBm
    TPCI(0xE0AB1, 0x881),  //  11  5.25  dBm
    TPCI(0xE0AB1, 0x884),  //  12  5.5  dBm
    TPCI(0xE0AB1, 0x888),  //  13  5.75  dBm
    TPCI(0xE0AB1, 0x88C),  //  14  6  dBm
    TPCI(0xE0AB1, 0x890),  //  15  6.25  dBm
    TPCI(0xE0AB1, 0x895),  //  16  6.5  dBm
    TPCI(0xE0AB1, 0x899),  //  17  6.75  dBm
    TPCI(0xE0AB1, 0x89D),  //  18  7  dBm
    TPCI(0xE0AB1, 0x8A2),  //  19  7.25  dBm
    TPCI(0xE0AB1, 0x8A7),  //  20  7.5  dBm
    TPCI(0xE0AB1, 0x8AC),  //  21  7.75  dBm
    TPCI(0xE0AB1, 0x8B1),  //  22  8  dBm
    TPCI(0xE0AB1, 0x8B6),  //  23  8.25  dBm
    TPCI(0xE0AB1, 0x8BB),  //  24  8.5  dBm
    TPCI(0xE0AB1, 0x8C0),  //  25  8.75  dBm
    TPCI(0xE0AB1, 0x8C6),  //  26  9  dBm
    TPCI(0xE0AB1, 0x8CC),  //  27  9.25  dBm
    TPCI(0xE0AB1, 0x8D2),  //  28  9.5  dBm
    TPCI(0xE0AB1, 0x8D8),  //  29  9.75  dBm
    TPCI(0xE0AB1, 0x8DE),  //  30  10  dBm
    TPCI(0xE0AB1, 0x8E5),  //  31  10.25  dBm
    TPCI(0xE0AB1, 0x8EB),  //  32  10.5  dBm
    TPCI(0xE0AB1, 0x8F2),  //  33  10.75  dBm
    TPCI(0xE0AB1, 0x8F9),  //  34  11  dBm

    TPCI(0xE0AB1, 0x901),  //  35  11.25  dBm
    TPCI(0xE0AB1, 0x908),  //  36  11.5  dBm
    TPCI(0xE0AB1, 0x910),  //  37  11.75  dBm
    TPCI(0xE0AB1, 0x918),  //  38  12  dBm
    TPCI(0xE0AB1, 0x920),  //  39  12.25  dBm
    TPCI(0xE0AB1, 0x928),  //  40  12.5  dBm
    TPCI(0xE0AB1, 0x931),  //  41  12.75  dBm
    TPCI(0xE0AB1, 0x93A),  //  42  13  dBm
    TPCI(0xE0AB1, 0x943),  //  43  13.25  dBm
    TPCI(0xE0AB1, 0x94D),  //  44  13.5  dBm
    TPCI(0xE0AB1, 0x956),  //  45  13.75  dBm
    TPCI(0xE0AB1, 0x960),  //  46  14  dBm
    TPCI(0xE0AB1, 0x96B),  //  47  14.25  dBm
    TPCI(0xE0AB1, 0x975),  //  48  14.5  dBm
    TPCI(0xE0AB1, 0x980),  //  49  14.75  dBm
    TPCI(0xE0AB1, 0x98B),  //  50  15  dBm
    TPCI(0xE0AB1, 0x997),  //  51  15.25  dBm
    TPCI(0xE0AB1, 0x9A3),  //  52  15.5  dBm
    TPCI(0xE0AB1, 0x9AF),  //  53  15.75  dBm
    TPCI(0xE0AB1, 0x9BB),  //  54  16  dBm
    TPCI(0xE0AB1, 0x9C8),  //  55  16.25  dBm
    TPCI(0xE0AB1, 0x9D6),  //  56  16.5  dBm
    TPCI(0xE0AB1, 0x9E0),  //  57  16.75  dBm
    TPCI(0xE0AB1, 0x9F2),  //  58  17  dBm
    TPCI(0xE0AB1, 0xA00),  //  59  17.25  dBm
    TPCI(0xE0AB1, 0xA0F),  //  60  17.5  dBm
    TPCI(0xE0AB1, 0xA1E),  //  61  17.75  dBm
    TPCI(0xE0AB1, 0xA2E),  //  62  18  dBm
    TPCI(0xE0AB1, 0xA3F),  //  63  18.25  dBm
    TPCI(0xE0AB1, 0xA4F),  //  64  18.5  dBm
    TPCI(0xE0AB1, 0xA61),  //  65  18.75  dBm
    TPCI(0xE0AB1, 0xA72),  //  66  19  dBm
    TPCI(0xE0AB1, 0xA85),  //  67  19.25  dBm
    TPCI(0xE0AB1, 0xA98),  //  68  19.5  dBm
    TPCI(0xE0AB1, 0xAAB),  //  69  19.75  dBm
    TPCI(0xE0AB1, 0xABF),  //  70  20  dBm
    TPCI(0xE0AB1, 0xAD3),  //  71  20.25  dBm
    TPCI(0xE0AB1, 0xAE9),  //  72  20.5  dBm
    TPCI(0xE0AB1, 0xAFE),  //  73  20.75  dBm
    TPCI(0xE0AB1, 0xB15),  //  74  21  dBm
    TPCI(0xE0AB1, 0xB2C),  //  75  21.25  dBm
    TPCI(0xE0AB1, 0xB43),  //  76  21.5  dBm
    TPCI(0xE0AB1, 0xB5C),  //  77  21.75  dBm
    TPCI(0xE0AB1, 0xB75),  //  78  22  dBm
    TPCI(0xE0AB1, 0xB8F),  //  79  22.25  dBm
};

struct BK7011RCBEKEN_TypeDef BK7231N_RC_REG =
{
    (volatile BK7011_RC_BEKEN_REG0x0_TypeDef *) (RC_BEKEN_BASE + 0 * 4),
    (volatile BK7011_RC_BEKEN_REG0x1_TypeDef *) (RC_BEKEN_BASE + 1 * 4),
    (volatile BK7011_RC_BEKEN_REG0x5_TypeDef *) (RC_BEKEN_BASE + 5 * 4),
    (volatile BK7011_RC_BEKEN_REG0x8_TypeDef *) (RC_BEKEN_BASE + 8 * 4),
    (volatile BK7011_RC_BEKEN_REG0xB_TypeDef *) (RC_BEKEN_BASE + 11 * 4),
    (volatile BK7011_RC_BEKEN_REG0xE_TypeDef *) (RC_BEKEN_BASE + 14 * 4),
    (volatile BK7011_RC_BEKEN_REG0x11_TypeDef *)(RC_BEKEN_BASE + 17 * 4),
    (volatile BK7011_RC_BEKEN_REG0x19_TypeDef *)(RC_BEKEN_BASE + 25 * 4),
    (volatile BK7011_RC_BEKEN_REG0x1A_TypeDef *)(RC_BEKEN_BASE + 0x1A * 4),
    (volatile BK7011_RC_BEKEN_REG0x1C_TypeDef *)(RC_BEKEN_BASE + 28 * 4),
    (volatile BK7011_RC_BEKEN_REG0x1E_TypeDef *)(RC_BEKEN_BASE + 30 * 4),
    (volatile BK7011_RC_BEKEN_REG0x3C_TypeDef *)(RC_BEKEN_BASE + 60 * 4),
    (volatile BK7011_RC_BEKEN_REG0x3E_TypeDef *)(RC_BEKEN_BASE + 62 * 4),
    (volatile BK7011_RC_BEKEN_REG0x3F_TypeDef *)(RC_BEKEN_BASE + 63 * 4),
    (volatile BK7011_RC_BEKEN_REG0x40_TypeDef *)(RC_BEKEN_BASE + 64 * 4),
    (volatile BK7011_RC_BEKEN_REG0x41_TypeDef *)(RC_BEKEN_BASE + 65 * 4),
    (volatile BK7011_RC_BEKEN_REG0x42_TypeDef *)(RC_BEKEN_BASE + 66 * 4),
    (volatile BK7011_RC_BEKEN_REG0x43_TypeDef *)(RC_BEKEN_BASE + 0x43 * 4),
    (volatile BK7011_RC_BEKEN_REG0x4C_TypeDef *)(RC_BEKEN_BASE + 76 * 4),
    (volatile BK7011_RC_BEKEN_REG0x4D_TypeDef *)(RC_BEKEN_BASE + 77 * 4),
    (volatile BK7011_RC_BEKEN_REG0x4E_TypeDef *)(RC_BEKEN_BASE + 78 * 4),
    (volatile BK7011_RC_BEKEN_REG0x4F_TypeDef *)(RC_BEKEN_BASE + 79 * 4),
    (volatile BK7011_RC_BEKEN_REG0x50_TypeDef *)(RC_BEKEN_BASE + 80 * 4),
    (volatile BK7011_RC_BEKEN_REG0x51_TypeDef *)(RC_BEKEN_BASE + 81 * 4),
    (volatile BK7011_RC_BEKEN_REG0x52_TypeDef *)(RC_BEKEN_BASE + 82 * 4),
    (volatile BK7011_RC_BEKEN_REG0x54_TypeDef *)(RC_BEKEN_BASE + 84 * 4),
    (volatile BK7011_RC_BEKEN_REG0x55_TypeDef *)(RC_BEKEN_BASE + 85 * 4),
    (volatile BK7011_RC_BEKEN_REG0x56_TypeDef *)(RC_BEKEN_BASE + 0x56 * 4),
    (volatile BK7011_RC_BEKEN_REG0x57_TypeDef *)(RC_BEKEN_BASE + 0x57 * 4),
    (volatile BK7011_RC_BEKEN_REG0x5A_TypeDef *)(RC_BEKEN_BASE + 90 * 4),
    (volatile BK7011_RC_BEKEN_REG0x5B_TypeDef *)(RC_BEKEN_BASE + 91 * 4),
    (volatile BK7011_RC_BEKEN_REG0x5C_TypeDef *)(RC_BEKEN_BASE + 92 * 4),
    (volatile BK7011_RC_BEKEN_REG0x6A_TypeDef *)(RC_BEKEN_BASE + 106 * 4),
};

struct BK7231N_TRX_REG_TypeDef BK7231N_TRX_REG =
{
    (volatile BK7011_TRxV2A_REG0x0_TypeDef *) (TRX_BEKEN_BASE + 0 * 4),
    (volatile BK7011_TRxV2A_REG0x1_TypeDef *) (TRX_BEKEN_BASE + 1 * 4),
    (volatile BK7011_TRxV2A_REG0x2_TypeDef *) (TRX_BEKEN_BASE + 2 * 4),
    (volatile BK7011_TRxV2A_REG0x3_TypeDef *) (TRX_BEKEN_BASE + 3 * 4),
    (volatile BK7011_TRxV2A_REG0x4_TypeDef *) (TRX_BEKEN_BASE + 4 * 4),
    (volatile BK7011_TRxV2A_REG0x5_TypeDef *) (TRX_BEKEN_BASE + 5 * 4),
    (volatile BK7011_TRxV2A_REG0x6_TypeDef *) (TRX_BEKEN_BASE + 6 * 4),
    (volatile BK7011_TRxV2A_REG0x7_TypeDef *) (TRX_BEKEN_BASE + 7 * 4),
    (volatile BK7011_TRxV2A_REG0x8_TypeDef *) (TRX_BEKEN_BASE + 8 * 4),
    (volatile BK7011_TRxV2A_REG0x9_TypeDef *) (TRX_BEKEN_BASE + 9 * 4),
    (volatile BK7011_TRxV2A_REG0xA_TypeDef *) (TRX_BEKEN_BASE + 10 * 4),
    (volatile BK7011_TRxV2A_REG0xB_TypeDef *) (TRX_BEKEN_BASE + 11 * 4),
    (volatile BK7011_TRxV2A_REG0xC_TypeDef *) (TRX_BEKEN_BASE + 12 * 4),
    (volatile BK7011_TRxV2A_REG0xD_TypeDef *) (TRX_BEKEN_BASE + 13 * 4),
    (volatile BK7011_TRxV2A_REG0xE_TypeDef *) (TRX_BEKEN_BASE + 14 * 4),
    (volatile BK7011_TRxV2A_REG0xF_TypeDef *) (TRX_BEKEN_BASE + 15 * 4),
    (volatile BK7011_TRxV2A_REG0x10_TypeDef *)(TRX_BEKEN_BASE + 16 * 4),
    (volatile BK7011_TRxV2A_REG0x11_TypeDef *)(TRX_BEKEN_BASE + 17 * 4),
    (volatile BK7011_TRxV2A_REG0x12_TypeDef *)(TRX_BEKEN_BASE + 18 * 4),
    (volatile BK7011_TRxV2A_REG0x13_TypeDef *)(TRX_BEKEN_BASE + 19 * 4),
    (volatile BK7011_TRxV2A_REG0x14_TypeDef *)(TRX_BEKEN_BASE + 20 * 4),
    (volatile BK7011_TRxV2A_REG0x15_TypeDef *)(TRX_BEKEN_BASE + 21 * 4),
    (volatile BK7011_TRxV2A_REG0x16_TypeDef *)(TRX_BEKEN_BASE + 22 * 4),
    (volatile BK7011_TRxV2A_REG0x17_TypeDef *)(TRX_BEKEN_BASE + 23 * 4),
    (volatile BK7011_TRxV2A_REG0x18_TypeDef *)(TRX_BEKEN_BASE + 24 * 4),
    (volatile BK7011_TRxV2A_REG0x19_TypeDef *)(TRX_BEKEN_BASE + 25 * 4),
    (volatile BK7011_TRxV2A_REG0x1A_TypeDef *)(TRX_BEKEN_BASE + 26 * 4),
    (volatile BK7011_TRxV2A_REG0x1B_TypeDef *)(TRX_BEKEN_BASE + 27 * 4),
    (volatile BK7011_TRxV2A_REG0x1C_TypeDef *)(TRX_BEKEN_BASE + 28 * 4),
};

struct temp_cal_pwr_st g_temp_pwr_current = {16, EVM_DEFUALT_RATE, 0, 0};

void delay05us(INT32 num)
{
    volatile INT32 i, j;
    //REG_WRITE((0x0802800+(18*4)), 0x02);

    for(i = 0; i < num; i ++)
    {
        for(j = 0; j < 5; j ++)
            ;
    }
    //REG_WRITE((0x0802800+(18*4)), 0x00);
}

void delay100us(INT32 num)
{
    volatile INT32 i, j;
    //REG_WRITE((0x0802800+(18*4)), 0x02);

    for(i = 0; i < num; i ++)
    {
        for(j = 0; j < 1050; j ++)
            ;
    }
    //REG_WRITE((0x0802800+(18*4)), 0x00);
}


#define CAL_WR_TRXREGS(reg)    do{\
                                    CHECK_OPERATE_RF_REG_IF_IN_SLEEP();\
                                    while(BK7231N_RC_REG.REG0x1->value & (0x1 << reg));\
                                    BK7231N_TRX_REG.REG##reg->value = BK7231N_TRX_RAM.REG##reg.value;\
                                    cal_delay(6);\
                                    while(BK7231N_RC_REG.REG0x1->value & (0x1 << reg));\
                                    CHECK_OPERATE_RF_REG_IF_IN_SLEEP_END();\
                                }while(0)


void rwnx_cal_load_default_result(void)
{
    //gtx_dcorMod = BK7231N_TRX_RAM.REG0xB.bits.padCa; //changed
    //gtx_dcorPA = BK7231N_TRX_RAM.REG0xC.bits.Rgm; //changed
   // gtx_pre_gain = BK7231N_RC_RAM.REG0x52.bits.Reserved_; //changed

    gcali_result.gtx_i_dc_comp = BK7231N_RC_RAM.REG0x4F.bits.TXIDCCOMP;
    gcali_result.gtx_q_dc_comp = BK7231N_RC_RAM.REG0x4F.bits.TXQDCCOMP;

    gcali_result.gtx_i_gain_comp = BK7231N_RC_RAM.REG0x50.bits.TXIGAINCOMP;
    gcali_result.gtx_q_gain_comp = BK7231N_RC_RAM.REG0x50.bits.TXQGAINCOMP;

    //gcali_result.gtx_ifilter_corner = BK7231N_TRX_RAM.REG0x6.bits.lpfcapcali50;
    //gcali_result.gtx_ifilter_corner += BK7231N_TRX_RAM.REG0x7.bits.lpfcapcali << 6;
    //gcali_result.gtx_qfilter_corner = BK7231N_TRX_RAM.REG0x6.bits.lpfcapcalq50;
    //gcali_result.gtx_qfilter_corner += BK7231N_TRX_RAM.REG0x7.bits.lpfcapcalq << 6;
    gcali_result.gtx_phase_comp = BK7231N_RC_RAM.REG0x51.bits.TXPHASECOMP;
    gcali_result.gtx_phase_ty2 = BK7231N_RC_RAM.REG0x51.bits.TXTY2;

    gcali_result.g_rx_dc_gain_tab[0] = BK7231N_TRX_RAM.REG0x14.value;
    gcali_result.g_rx_dc_gain_tab[1] = BK7231N_TRX_RAM.REG0x15.value;
    gcali_result.g_rx_dc_gain_tab[2] = BK7231N_TRX_RAM.REG0x16.value;
    gcali_result.g_rx_dc_gain_tab[3] = BK7231N_TRX_RAM.REG0x17.value;
    gcali_result.g_rx_dc_gain_tab[4] = BK7231N_TRX_RAM.REG0x18.value;
    gcali_result.g_rx_dc_gain_tab[5] = BK7231N_TRX_RAM.REG0x19.value;
    gcali_result.g_rx_dc_gain_tab[6] = BK7231N_TRX_RAM.REG0x1A.value;
    gcali_result.g_rx_dc_gain_tab[7] = BK7231N_TRX_RAM.REG0x1B.value;

    gcali_result.grx_amp_err_wr = BK7231N_RC_RAM.REG0x42.bits.RXAMPERRWR;
    gcali_result.grx_phase_err_wr = BK7231N_RC_RAM.REG0x42.bits.RXPHASEERRWR;

    //gstat_cal = (BK7231N_RC_RAM[16] >> 29) & 0x1;
}

void rwnx_cal_read_current_cal_result(BK7011_CALI_RESULT *cali_result)
{
    memcpy(cali_result, &gcali_result, sizeof(gcali_result));
}

void calibration_print_result()
{
    CAL_FATAL("*********** finally result 7231N **********\r\n");
    CAL_FATAL("gbias_after_cal: 0x%x\r\n", gcali_result.gbias_after_cal);
    CAL_FATAL("gav_tssi: 0x%x\r\n", gcali_result.gav_tssi);
    CAL_FATAL("gtx_q_dc_comp:0x%x\r\n", gcali_result.gtx_q_dc_comp);
    CAL_FATAL("gtx_i_dc_comp:0x%x\r\n", gcali_result.gtx_i_dc_comp);
    CAL_FATAL("gtx_i_gain_comp:0x%x\r\n", gcali_result.gtx_i_gain_comp);
    CAL_FATAL("gtx_q_gain_comp:0x%x\r\n", gcali_result.gtx_q_gain_comp);
    CAL_FATAL("gtx_phase_comp:0x%x\r\n", gcali_result.gtx_phase_comp);
    CAL_FATAL("gtx_phase_ty2:0x%x\r\n", gcali_result.gtx_phase_ty2);
    CAL_FATAL("gtx_ifilter_corner over: 0x%x\r\n", gcali_result.gtx_ifilter_corner);
    CAL_FATAL("gtx_qfilter_corner over: 0x%x\r\n", gcali_result.gtx_qfilter_corner);
    CAL_FATAL("const_iqcal_p over: 0x%x\r\n", gcali_result.const_iqcal_p);
    CAL_FATAL("gtx_dcorMod:0x%x, gtx_dcorPA:0x%x\r\n", gtx_dcorMod, gtx_dcorPA);
    CAL_FATAL("gtx_pre_gain:0x%x\r\n", gtx_pre_gain);
    CAL_FATAL("g_rx_dc_gain_tab 0 over: 0x%x\r\n", gcali_result.g_rx_dc_gain_tab[0]);
    CAL_FATAL("g_rx_dc_gain_tab 1 over: 0x%x\r\n", gcali_result.g_rx_dc_gain_tab[1]);
    CAL_FATAL("g_rx_dc_gain_tab 2 over: 0x%x\r\n", gcali_result.g_rx_dc_gain_tab[2]);
    CAL_FATAL("g_rx_dc_gain_tab 3 over: 0x%x\r\n", gcali_result.g_rx_dc_gain_tab[3]);
    CAL_FATAL("g_rx_dc_gain_tab 4 over: 0x%x\r\n", gcali_result.g_rx_dc_gain_tab[4]);
    CAL_FATAL("g_rx_dc_gain_tab 5 over: 0x%x\r\n", gcali_result.g_rx_dc_gain_tab[5]);
    CAL_FATAL("g_rx_dc_gain_tab 6 over: 0x%x\r\n", gcali_result.g_rx_dc_gain_tab[6]);
    CAL_FATAL("g_rx_dc_gain_tab 7 over: 0x%x\r\n", gcali_result.g_rx_dc_gain_tab[7]);

    CAL_FATAL("grx_amp_err_wr:0x%03x\r\n", gcali_result.grx_amp_err_wr);
    CAL_FATAL("grx_phase_err_wr:0x%03x\r\n", gcali_result.grx_phase_err_wr);
    CAL_FATAL("**************************************\r\n");
}

void calibration_print_results(BK7011_CALI_RESULT *cali_result, int calibrate_time)
{
    int i, j;
    int tx_filter_corner_max, tx_filter_corner_min;
    int tx_filter_corner;
    BK7011_CALI_RESULT max;
    BK7011_CALI_RESULT min;

    os_printf("gtx_dcorMod_temp: 0x%x\r\n", gtx_dcorMod_temp);
    os_printf("gtx_dcorPA_temp: 0x%x\r\n", gtx_dcorPA_temp);
    os_printf("gtx_pre_gain_temp: 0x%x\r\n", gtx_pre_gain_temp);

    os_printf("gtx_dcorMod_temp_loopback: 0x%x\r\n", gtx_dcorMod_temp_loopback);
    os_printf("gtx_dcorPA_temp_loopback: 0x%x\r\n", gtx_dcorPA_temp_loopback);
    os_printf("gtx_pre_gain_temp_loopback: 0x%x\r\n", gtx_pre_gain_temp_loopback);
    
    tx_filter_corner_max = -1100 * BK_TX_DAC_COEF;
    tx_filter_corner_min = 1100 * BK_TX_DAC_COEF;

    max.gbias_after_cal = -1100 * BK_TX_DAC_COEF;
    min.gbias_after_cal = 1100 * BK_TX_DAC_COEF;
    max.gav_tssi = -1100 * BK_TX_DAC_COEF;
    min.gav_tssi = 1100 * BK_TX_DAC_COEF;
    max.gtx_ifilter_corner = -1100 * BK_TX_DAC_COEF;
    min.gtx_ifilter_corner = 1100 * BK_TX_DAC_COEF;
    max.gtx_qfilter_corner = -1100 * BK_TX_DAC_COEF;
    min.gtx_qfilter_corner = 1100 * BK_TX_DAC_COEF;
    max.gtx_i_dc_comp = -1100 * BK_TX_DAC_COEF;
    min.gtx_i_dc_comp = 1100 * BK_TX_DAC_COEF;
    max.gtx_q_dc_comp = -1100 * BK_TX_DAC_COEF;
    min.gtx_q_dc_comp = 1100 * BK_TX_DAC_COEF;
    max.gtx_i_gain_comp = -1100 * BK_TX_DAC_COEF;
    min.gtx_i_gain_comp = 1100 * BK_TX_DAC_COEF;
    max.gtx_q_gain_comp = -1100 * BK_TX_DAC_COEF;
    min.gtx_q_gain_comp = 1100 * BK_TX_DAC_COEF;
    max.gtx_phase_comp = -1100 * BK_TX_DAC_COEF;
    min.gtx_phase_comp = 1100 * BK_TX_DAC_COEF;
    max.gtx_phase_ty2 = -1100 * BK_TX_DAC_COEF;
    min.gtx_phase_ty2 = 1100 * BK_TX_DAC_COEF;
    max.gtx_i_dc_comp_loopback = -1100 * BK_TX_DAC_COEF;
    min.gtx_i_dc_comp_loopback = 1100 * BK_TX_DAC_COEF;
    max.gtx_q_dc_comp_loopback = -1100 * BK_TX_DAC_COEF;
    min.gtx_q_dc_comp_loopback = 1100 * BK_TX_DAC_COEF;
    max.gtx_i_gain_comp_loopback = -1100 * BK_TX_DAC_COEF;
    min.gtx_i_gain_comp_loopback = 1100 * BK_TX_DAC_COEF;
    max.gtx_q_gain_comp_loopback = -1100 * BK_TX_DAC_COEF;
    min.gtx_q_gain_comp_loopback = 1100 * BK_TX_DAC_COEF;
    max.gtx_phase_comp_loopback = -1100 * BK_TX_DAC_COEF;
    min.gtx_phase_comp_loopback = 1100 * BK_TX_DAC_COEF;
    max.gtx_phase_ty2_loopback = -1100 * BK_TX_DAC_COEF;
    min.gtx_phase_ty2_loopback = 1100 * BK_TX_DAC_COEF;
    max.rx_amp_err_rd = -1100 * BK_TX_DAC_COEF;
    min.rx_amp_err_rd = 1100 * BK_TX_DAC_COEF;
    max.rx_phase_err_rd = -1100 * BK_TX_DAC_COEF;
    min.rx_phase_err_rd = 1100 * BK_TX_DAC_COEF;
    max.rx_ty2_rd = -1100 * BK_TX_DAC_COEF;
    min.rx_ty2_rd = 1100 * BK_TX_DAC_COEF;

    for (i = 0; i < calibrate_time; i++)
    {
        //bias_after_cal:
        if (max.gbias_after_cal < cali_result[i].gbias_after_cal)
        {
            max.gbias_after_cal = cali_result[i].gbias_after_cal;
        }
        if (min.gbias_after_cal > cali_result[i].gbias_after_cal)
        {
            min.gbias_after_cal = cali_result[i].gbias_after_cal;
        }

        //av_tssi:
        if (max.gav_tssi < cali_result[i].gav_tssi)
        {
            max.gav_tssi = cali_result[i].gav_tssi;
        }
        if (min.gav_tssi > cali_result[i].gav_tssi)
        {
            min.gav_tssi = cali_result[i].gav_tssi;
        }

        //tx_ifilter_corner:
        if (max.gtx_ifilter_corner < cali_result[i].gtx_ifilter_corner)
        {
            max.gtx_ifilter_corner = cali_result[i].gtx_ifilter_corner;
        }
        if (min.gtx_ifilter_corner > cali_result[i].gtx_ifilter_corner)
        {
            min.gtx_ifilter_corner = cali_result[i].gtx_ifilter_corner;
        }
#ifdef SCAN_IQ_FILTER_CORNER
#endif

        //gtx_qfilter_corner:
        if (max.gtx_qfilter_corner < cali_result[i].gtx_qfilter_corner)
        {
            max.gtx_qfilter_corner = cali_result[i].gtx_qfilter_corner;
        }
        if (min.gtx_qfilter_corner > cali_result[i].gtx_qfilter_corner)
        {
            min.gtx_qfilter_corner = cali_result[i].gtx_qfilter_corner;
        }
#ifdef SCAN_IQ_FILTER_CORNER
                    cali_result[i].grx_avg_i_14M, cali_result[i].grx_avg_i_14M2,
                    cali_result[i].grx_avg_q_14M, cali_result[i].grx_avg_q_14M2);
        for (j=0; j<5; j++)
        {
        }
#endif

        tx_filter_corner = cali_result[i].gtx_ifilter_corner - cali_result[i].gtx_qfilter_corner;
        if (tx_filter_corner_max < tx_filter_corner)
        {
            tx_filter_corner_max = tx_filter_corner;
        }
        if (tx_filter_corner_min > tx_filter_corner)
        {
            tx_filter_corner_min = tx_filter_corner;
        }

        //tx_i_dc_comp:
        if (max.gtx_i_dc_comp < cali_result[i].gtx_i_dc_comp)
        {
            max.gtx_i_dc_comp = cali_result[i].gtx_i_dc_comp;
        }
        if (min.gtx_i_dc_comp > cali_result[i].gtx_i_dc_comp)
        {
            min.gtx_i_dc_comp = cali_result[i].gtx_i_dc_comp;
        }

        //tx_q_dc_comp:
        if (max.gtx_q_dc_comp < cali_result[i].gtx_q_dc_comp)
        {
            max.gtx_q_dc_comp = cali_result[i].gtx_q_dc_comp;
        }
        if (min.gtx_q_dc_comp > cali_result[i].gtx_q_dc_comp)
        {
            min.gtx_q_dc_comp = cali_result[i].gtx_q_dc_comp;
        }

        //tx_i_gain_comp:
        if (max.gtx_i_gain_comp < cali_result[i].gtx_i_gain_comp)
        {
            max.gtx_i_gain_comp = cali_result[i].gtx_i_gain_comp;
        }
        if (min.gtx_i_gain_comp > cali_result[i].gtx_i_gain_comp)
        {
            min.gtx_i_gain_comp = cali_result[i].gtx_i_gain_comp;
        }

        //tx_q_gain_comp:
        if (max.gtx_q_gain_comp < cali_result[i].gtx_q_gain_comp)
        {
            max.gtx_q_gain_comp = cali_result[i].gtx_q_gain_comp;
        }
        if (min.gtx_q_gain_comp > cali_result[i].gtx_q_gain_comp)
        {
            min.gtx_q_gain_comp = cali_result[i].gtx_q_gain_comp;
        }

        //tx_phase_comp:
        if (max.gtx_phase_comp < cali_result[i].gtx_phase_comp)
        {
            max.gtx_phase_comp = cali_result[i].gtx_phase_comp;
        }
        if (min.gtx_phase_comp > cali_result[i].gtx_phase_comp)
        {
            min.gtx_phase_comp = cali_result[i].gtx_phase_comp;
        }

        //gtx_phase_ty2:
        if (max.gtx_phase_ty2 < cali_result[i].gtx_phase_ty2)
        {
            max.gtx_phase_ty2 = cali_result[i].gtx_phase_ty2;
        }
        if (min.gtx_phase_ty2 > cali_result[i].gtx_phase_ty2)
        {
            min.gtx_phase_ty2 = cali_result[i].gtx_phase_ty2;
        }

        //tx_i_dc_comp_temp:
        if (max.gtx_i_dc_comp_loopback < cali_result[i].gtx_i_dc_comp_loopback)
        {
            max.gtx_i_dc_comp_loopback = cali_result[i].gtx_i_dc_comp_loopback;
        }
        if (min.gtx_i_dc_comp_loopback > cali_result[i].gtx_i_dc_comp_loopback)
        {
            min.gtx_i_dc_comp_loopback = cali_result[i].gtx_i_dc_comp_loopback;
        }

        //tx_q_dc_comp_temp:
        if (max.gtx_q_dc_comp_loopback < cali_result[i].gtx_q_dc_comp_loopback)
        {
            max.gtx_q_dc_comp_loopback = cali_result[i].gtx_q_dc_comp_loopback;
        }
        if (min.gtx_q_dc_comp_loopback > cali_result[i].gtx_q_dc_comp_loopback)
        {
            min.gtx_q_dc_comp_loopback = cali_result[i].gtx_q_dc_comp_loopback;
        }

        //tx_i_gain_comp_temp:
        if (max.gtx_i_gain_comp_loopback < cali_result[i].gtx_i_gain_comp_loopback)
        {
            max.gtx_i_gain_comp_loopback = cali_result[i].gtx_i_gain_comp_loopback;
        }
        if (min.gtx_i_gain_comp_loopback > cali_result[i].gtx_i_gain_comp_loopback)
        {
            min.gtx_i_gain_comp_loopback = cali_result[i].gtx_i_gain_comp_loopback;
        }

        //tx_q_gain_comp_temp:
        if (max.gtx_q_gain_comp_loopback < cali_result[i].gtx_q_gain_comp_loopback)
        {
            max.gtx_q_gain_comp_loopback = cali_result[i].gtx_q_gain_comp_loopback;
        }
        if (min.gtx_q_gain_comp_loopback > cali_result[i].gtx_q_gain_comp_loopback)
        {
            min.gtx_q_gain_comp_loopback = cali_result[i].gtx_q_gain_comp_loopback;
        }

        //tx_phase_comp_temp:
        if (max.gtx_phase_comp_loopback < cali_result[i].gtx_phase_comp_loopback)
        {
            max.gtx_phase_comp_loopback = cali_result[i].gtx_phase_comp_loopback;
        }
        if (min.gtx_phase_comp_loopback > cali_result[i].gtx_phase_comp_loopback)
        {
            min.gtx_phase_comp_loopback = cali_result[i].gtx_phase_comp_loopback;
        }

        //tx_phase_ty2_temp:
        if (max.gtx_phase_ty2_loopback < cali_result[i].gtx_phase_ty2_loopback)
        {
            max.gtx_phase_ty2_loopback = cali_result[i].gtx_phase_ty2_loopback;
        }
        if (min.gtx_phase_ty2_loopback > cali_result[i].gtx_phase_ty2_loopback)
        {
            min.gtx_phase_ty2_loopback = cali_result[i].gtx_phase_ty2_loopback;
        }

        //rx_amp_err_rd:
        if (max.rx_amp_err_rd < cali_result[i].rx_amp_err_rd)
        {
            max.rx_amp_err_rd = cali_result[i].rx_amp_err_rd;
        }
        if (min.rx_amp_err_rd > cali_result[i].rx_amp_err_rd)
        {
            min.rx_amp_err_rd = cali_result[i].rx_amp_err_rd;
        }

        //rx_phase_err_rd:
        if (max.rx_phase_err_rd < cali_result[i].rx_phase_err_rd)
        {
            max.rx_phase_err_rd = cali_result[i].rx_phase_err_rd;
        }
        if (min.rx_phase_err_rd > cali_result[i].rx_phase_err_rd)
        {
            min.rx_phase_err_rd = cali_result[i].rx_phase_err_rd;
        }

        //rx_ty2_rd:
        if (max.rx_ty2_rd < cali_result[i].rx_ty2_rd)
        {
            max.rx_ty2_rd = cali_result[i].rx_ty2_rd;
        }
        if (min.rx_ty2_rd > cali_result[i].rx_ty2_rd)
        {
            min.rx_ty2_rd = cali_result[i].rx_ty2_rd;
        }
    }


    for (j = 0; j < 8; j++)
    {
        int iTemp;
        os_printf("\r\n");

        max.g_rx_dc_gain_tab[j] = -1100 * BK_TX_DAC_COEF;
        min.g_rx_dc_gain_tab[j] = 1100 * BK_TX_DAC_COEF;
        for (i = 0; i < calibrate_time; i++)
        {
            iTemp = cali_result[i].g_rx_dc_gain_tab[j];
            if (max.g_rx_dc_gain_tab[j] < (iTemp & 0x00FF))
            {
                max.g_rx_dc_gain_tab[j] = iTemp & 0x00FF;
            }
            if (min.g_rx_dc_gain_tab[j] > (iTemp & 0x00FF))
            {
                min.g_rx_dc_gain_tab[j] = iTemp & 0x00FF;
            }
        }
        os_printf("g_rx_dc_gain_tab[%d].i_%ddb: min = %d, max = %d, max-min = %d\r\n", j, j * 6, min.g_rx_dc_gain_tab[j], max.g_rx_dc_gain_tab[j], max.g_rx_dc_gain_tab[j] - min.g_rx_dc_gain_tab[j]);

        max.g_rx_dc_gain_tab[j] = -1100 * BK_TX_DAC_COEF;
        min.g_rx_dc_gain_tab[j] = 1100 * BK_TX_DAC_COEF;
        for (i = 0; i < calibrate_time; i++)
        {
            iTemp = cali_result[i].g_rx_dc_gain_tab[j] >> 8;
            if (max.g_rx_dc_gain_tab[j] < (iTemp & 0x00FF))
            {
                max.g_rx_dc_gain_tab[j] = iTemp & 0x00FF;
            }
            if (min.g_rx_dc_gain_tab[j] > (iTemp & 0x00FF))
            {
                min.g_rx_dc_gain_tab[j] = iTemp & 0x00FF;
            }
        }

        max.g_rx_dc_gain_tab[j] = -1100 * BK_TX_DAC_COEF;
        min.g_rx_dc_gain_tab[j] = 1100 * BK_TX_DAC_COEF;
        for (i = 0; i < calibrate_time; i++)
        {
            iTemp = cali_result[i].g_rx_dc_gain_tab[j] >> 16;
            if (max.g_rx_dc_gain_tab[j] < (iTemp & 0x00FF))
            {
                max.g_rx_dc_gain_tab[j] = iTemp & 0x00FF;
            }
            if (min.g_rx_dc_gain_tab[j] > (iTemp & 0x00FF))
            {
                min.g_rx_dc_gain_tab[j] = iTemp & 0x00FF;
            }
        }

        max.g_rx_dc_gain_tab[j] = -1100 * BK_TX_DAC_COEF;
        min.g_rx_dc_gain_tab[j] = 1100 * BK_TX_DAC_COEF;
        for (i = 0; i < calibrate_time; i++)
        {
            iTemp = cali_result[i].g_rx_dc_gain_tab[j] >> 24;
            if (max.g_rx_dc_gain_tab[j] < (iTemp & 0x00FF))
            {
                max.g_rx_dc_gain_tab[j] = iTemp & 0x00FF;
            }
            if (min.g_rx_dc_gain_tab[j] > (iTemp & 0x00FF))
            {
                min.g_rx_dc_gain_tab[j] = iTemp & 0x00FF;
            }
        }
    }
}

void calibration_auto_test(unsigned long ul_calibrate_times)
{
    BK7011_CALI_RESULT *cali_results;
    uint32_t start, end;
    int index;

    if (ul_calibrate_times == 0)
    {
        ul_calibrate_times = 100;
    }

    cali_results = (BK7011_CALI_RESULT *)malloc(ul_calibrate_times * sizeof(BK7011_CALI_RESULT));
    if (NULL == cali_results)
    {
        CAL_FATAL("malloc(%d*sizeof(%d)) failed\r\n", ul_calibrate_times, sizeof(BK7011_CALI_RESULT));
        return;
    }

    CAL_FATAL("\r\n***********************************************************************\r\n"
              "calibration_auto_test: \r\n");
    start = rtos_get_time();
    for (index=0; index<ul_calibrate_times; index++)
    {
        calibration_main();

        rwnx_cal_read_current_cal_result(cali_results + index);
    }
    end = rtos_get_time();
    calibration_print_results(cali_results, ul_calibrate_times);
    CAL_FATAL("***********************************************************************\r\n");
    CAL_FATAL("tick total=%d,avg=%d\n", end - start, (end - start)/ul_calibrate_times);
    free(cali_results);
}

void rwnx_cal_set_lpfcap_iq(UINT32 lpfcap_i, UINT32 lpfcap_q)
{   
    BK7231N_TRX_RAM.REG0x6.bits.lpfcapcalq50 = lpfcap_q & 0x3F;
    BK7231N_TRX_RAM.REG0x7.bits.lpfcapcalq = (lpfcap_q >> 6) & 0x3;
    BK7231N_TRX_RAM.REG0x6.bits.lpfcapcali50 = lpfcap_i & 0x3F; 
    BK7231N_TRX_RAM.REG0x7.bits.lpfcapcali = (lpfcap_i >> 6) & 0x3; 
    CAL_WR_TRXREGS(0x6);
    CAL_WR_TRXREGS(0x7);
    CAL_TIM_PRT("[%d]v=0x%x,0x%x ram=0x%x,0x%x reg=0x%x,0x%x\n", __LINE__, lpfcap_i, lpfcap_q, BK7231N_TRX_RAM.REG0x6.bits.lpfcapcali50, BK7231N_TRX_RAM.REG0x6.bits.lpfcapcalq50, BK7231N_TRX_REG.REG0x6->bits.lpfcapcali50, BK7231N_TRX_REG.REG0x6->bits.lpfcapcalq50);
}

#if CFG_SUPPORT_MANUAL_CALI
static UINT32 rwnx_cal_translate_tx_rate(UINT32 rate)
{
    UINT32 param;

    switch(rate)
    {
    case 0 :
        param = 1;
        break;  // 1Mbps
    case 1 :
        param = 2;
        break;  // 2Mbps
    case 2 :
        param = 5;
        break;	// 5.5Mbps
    case 3:
        param = 11;
        break;	// 11Mbps
    case 4:
        param = 6;
        break;	// 6Mbps
    case 5 :
        param = 9;
        break;	// 9Mbps
    case 6:
        param = 12;
        break;	// 12Mbps
    case 7:
        param = 18;
        break;	// 18Mbps
    case 8:
        param = 24;
        break;	// 24Mbps
    case 9:
        param = 36;
        break;	// 36Mbps
    case 10:
        param = 48;
        break;	// 48Mbps
    case 11:
        param = 54;
        break;	// 54Mbps
    default:
        param = rate;
        break;	// 54Mbps
    }

    return param;
}

static UINT32 rwnx_cal_translate_tx_rate_for_n(UINT32 rate, UINT32 bandwidth)
{
    UINT32 param;
   
    switch(rate)
    {
    case 128 :
    case 129 :
    case 130 :
    case 131 :
    case 132 :
    case 133 :        
        param = rate - 123;
        break;	// MCS0-5 the same rate indx as 9M to 36M
		
    case 134:
    case 135:
        if(bandwidth == PHY_CHNL_BW_20)
            param = 11;  // MCS6-7the same rate indx as to 54M  - band:20M
        else 
            param = 12;  // MCS6-7the same rate indx as to 135M - band:40M
        break;	
    
    default:
        param = rate;
        break;	
    }

    return param;
}
#endif

//static INT32 cur_rate;
void rwnx_cal_set_txpwr_by_rate(INT32 rate, UINT32 test_mode)
{
#if CFG_SUPPORT_MANUAL_CALI     
    UINT32 ret;
    UINT32 pwr_gain;

    struct phy_channel_info info;
    UINT32 channel, bandwidth;   // PHY_CHNL_BW_20,  PHY_CHNL_BW_40:

    phy_get_channel(&info, 0);
    bandwidth = (info.info1 >> 8) & 0xff;

    channel = (((info.info2 & 0xffff) - 2400) - 7)/5;
    if(channel > 14)
        channel = 14;
    
    if(!manual_cal_get_txpwr(rwnx_cal_translate_tx_rate(rate), 
        channel, bandwidth, &pwr_gain)) 
    {
        // unable get txpwr from manual cal
        return;
    }

    if (test_mode == 0)
    {
        ret = manual_cal_get_pwr_idx_shift(rate, bandwidth, &pwr_gain);
    }
    else
    {
        ret = 2;
        if((bandwidth == PHY_CHNL_BW_20) && (rate <= 3))
        {
            ret = 1;
        }
    }
    os_null_printf("txpwr info- r:%d, c:%d, b:%d -- idx1:%d, idx2: %d\r\n", 
            rate, channel, bandwidth, pwr_gain, ret);
    if(!ret ){
        // unable get txpwr from manual cal
        return;
    } else if(ret == 1) {
        rwnx_cal_set_txpwr(pwr_gain, EVM_DEFUALT_B_RATE);
    } else if(ret == 2) {
        rwnx_cal_set_txpwr(pwr_gain, EVM_DEFUALT_RATE);
    }

    if ((test_mode) && ((rate <= 4) || (rate == 128)))
    {
        // just for 11b/11g_6Mbps/MCS0
        BK7231N_RC_REG.REG0x5A->bits.TXCALCAPI = 0xff;
        BK7231N_RC_REG.REG0x5B->bits.TXCALCAPQ = 0xff;
    }
    else if (rate <= 1)
    {
        // just for 11b_1Mbps/11b_2Mbps
        BK7231N_RC_REG.REG0x5A->bits.TXCALCAPI = 0x7f;
        BK7231N_RC_REG.REG0x5B->bits.TXCALCAPQ = 0xff;
    }
    else if (rate <= 3)
    {
        // just for 11b_5.5Mbps/11b_11Mbps
        BK7231N_RC_REG.REG0x5A->bits.TXCALCAPI = 0xff;
        BK7231N_RC_REG.REG0x5B->bits.TXCALCAPQ = 0xff;
    }
    else
    {
        // restore iq for 11g/11n
        extern UINT32 g_lpf_cal_i;
        extern UINT32 g_lpf_cal_q;
        //BK7231N_RC_REG.REG0x5A->bits.TXCALCAPI = gcali_result.gtx_ifilter_corner;
        //BK7231N_RC_REG.REG0x5B->bits.TXCALCAPQ = gcali_result.gtx_qfilter_corner;
        BK7231N_RC_REG.REG0x5A->bits.TXCALCAPI = g_lpf_cal_i;
        BK7231N_RC_REG.REG0x5B->bits.TXCALCAPQ = g_lpf_cal_q;
    }

    if(test_mode)
    {
        if (BK7231N_TRX_RAM.REG0x4.bits.Itune_vco_spi != 0x2C)
        {
            BK7231N_TRX_RAM.REG0x4.bits.Itune_vco_spi = 0x2C;
            CAL_WR_TRXREGS(0x4);
        }
		#if 0
        os_printf("add extral movement in test\r\n"); 

         #if CFG_USE_TEMPERATURE_DETECT
         temp_detect_uninit();
         #endif

        BK7231N_TRX_RAM.REG0xB->bits.gctrlmod30 = 0xF;
        CAL_WR_TRXREGS(0xB);   

        if(rate <= 3) {
            BK7231N_TRX_RAM.REG0xB->bits.gctrlmod30 = 0xC;
            CAL_WR_TRXREGS(0xB);  
        } 
		#endif
    } else {
        CAL_WR_TRXREGS(0xB);   
    }
#endif
}

void ble_cal_set_txpwr(uint8_t idx)
{
	UINT32 pwr_gain_base = 0xE12B1000;
	const PWR_REGS *pcfg;

	*((volatile unsigned int *)0XC0010004) = 3 << 18;

	if (idx > TCP_PAMAP_TAB_BLE_LEN) {
		idx = TCP_PAMAP_TAB_BLE_LEN;
	}

	if (check_large_singal_status())
		phy_enable_rx_switch();

	if(rwnx_sys_is_enable_hw_tpc())
        rwnx_no_use_tpc_set_pwr();
	
	pcfg = cfg_tab_ble + idx;

	BK7231N_TRX_RAM.REG0x8.bits.rssith50 = 0xf; 
    CAL_WR_TRXREGS(0x8);

	BK7231N_RC_REG.REG0x3C->bits.RXHPFBYPASS = 0x1;

	REG_WRITE((RCB_POWER_TABLE_ADDR + (0x7F * 4)), pwr_gain_base | pcfg->pregain);
}

void ble_cal_recover_txpwr(void)
{
	BK7231N_TRX_RAM.REG0x8.value = BK7231N_TRX_ROM[8];
    CAL_WR_TRXREGS(0x8);

	*((volatile unsigned int *)0XC0010004) = 0;

	if (check_large_singal_status())
		phy_disable_rx_switch();

	if (rwnx_sys_is_enable_hw_tpc()) {
        rwnx_use_tpc_set_pwr();
    }

	BK7231N_RC_REG.REG0x3C->bits.RXHPFBYPASS = 0x0;
}

extern void tpc_init(void);
extern void tpc_deinit(void);
struct temp_cal_pwr_st g_temp_pwr_current_tpc = {0, EVM_DEFUALT_RATE, 0, 0};
void rwnx_set_tpc_txpwr_by_tmpdetect(INT16 shift_b, INT16 shift_g)
{
    g_temp_pwr_current_tpc.shift_g = shift_g;
    g_temp_pwr_current_tpc.shift = shift_b;

#ifdef ATE_PRINT_DEBUG
    os_printf("td set tpc pwr: shift_b:%d, shift_g:%d\r\n", 
        shift_b, shift_g);
#endif
}  

UINT32 rwnx_tpc_pwr_idx_translate(UINT32 pwr_gain, UINT32 rate, UINT32 print_log)
{
    INT16 idx = TCP_PAMAP_TAB_LEN - 1;
    INT16 shift;
    INT16 idx_max;
    
    if(rate == EVM_DEFUALT_B_RATE) {
    // for b
        idx_max = TCP_PAMAP_TAB_B_LEN;
        shift = g_temp_pwr_current_tpc.shift;
    } else if(rate == EVM_DEFUALT_RATE) {
    // for g
        idx_max = TCP_PAMAP_TAB_G_LEN;
        shift = g_temp_pwr_current_tpc.shift_g;
    } else {
        os_printf("no support :%d\r\n", rate);
        return idx;
    }

    idx = pwr_gain + shift;

    if(idx >= idx_max) {
	    idx = idx_max - 1; 
	} else if (idx < 0)
	    idx = 0;

#if CFG_USE_TEMPERATURE_DETECT
    g_temp_pwr_current_tpc.idx = idx;
#endif

    if (print_log)
    {
        os_printf("translate idx1:%d, td_shift:%d, b/g:%d --- idx2:%d\r\n", pwr_gain, 
            shift, rate, idx);
    }

    return idx;
}

UINT32 rwnx_tpc_get_pwridx_by_rate(UINT32 rate, UINT32 print_log)
{
    UINT32 ret, ret_bak;
    UINT32 pwr_gain;
    static UINT32 last_ret = 0;

    struct phy_channel_info info;
    UINT32 channel, bandwidth;   // PHY_CHNL_BW_20,  PHY_CHNL_BW_40:

    phy_get_channel(&info, 0);
    bandwidth = (info.info1 >> 8) & 0xff;
    if(rate <= 128)
        bandwidth = 0;

    channel = (((info.info2 & 0xffff) - 2400) - 7)/5;

    if(channel > 14)
        channel = 14;

    if(manual_cal_get_txpwr(rwnx_cal_translate_tx_rate(rate), 
        channel, bandwidth, &pwr_gain) == 0) 
    {
        os_printf("unable get txpwr %d, %d, %d\r\n", rate, channel, bandwidth);
        return 0;
    }

    ret_bak = ret = manual_cal_get_pwr_idx_shift(rate, bandwidth, &pwr_gain);
    
    if(!ret ){
        os_printf("unable get txpwr shift %d, %d, %d\r\n", rate, channel, bandwidth);
        return 0;
    } else if(ret == 1) {
        ret = rwnx_tpc_pwr_idx_translate(pwr_gain, EVM_DEFUALT_B_RATE, 0);
    } else if(ret == 2) {
        ret = rwnx_tpc_pwr_idx_translate(pwr_gain, EVM_DEFUALT_RATE, 0);
        ret += TCP_PAMAP_TAB_B_LEN;
    }

    if (last_ret != ret || print_log)
    {
        INT16 shift = g_temp_pwr_current.shift;
        if(ret_bak == 2) {
            shift = g_temp_pwr_current.shift_g;
        }
        
        CAL_FATAL("tpc info- r:%d, c:%d, b:%d -- idx1:%d+(%d), idx2: %d\r\n", 
            rate, channel, bandwidth, pwr_gain, shift, ret);

        const PWR_REGS_TPC value = cfg_tab_tpc[ret];
        CAL_PRT("b[31-12]:0x%05x, c[11-0]:0x%03x\r\n", value >> 12, value & 0xFFF);
    }
    last_ret = ret;
    
    return ret;
    
}

void rwnx_use_tpc_set_pwr(void)
{
    #if CFG_SUPPORT_TPC_PA_MAP
    //tpc_init();

    BK7231N_RC_REG.REG0x4C->bits.TXPOWTBLEN = 1;
    BK7231N_RC_RAM.REG0x4C.bits.TXPOWTBLEN = 1;
    //BK7231N_TRX_RAM.REG0xC.bits.pamapen = 1;
    //CAL_WR_TRXREGS(0xC);
    
    gcali_context.is_tpc_used = 1;
    #endif
}

void rwnx_no_use_tpc_set_pwr(void)
{
	#if CFG_SUPPORT_TPC_PA_MAP
    //tpc_deinit();
    
    BK7231N_RC_REG.REG0x4C->bits.TXPOWTBLEN = 0;
    BK7231N_RC_RAM.REG0x4C.bits.TXPOWTBLEN = 0;
    //BK7231N_TRX_RAM.REG0xC.bits.pamapen = 0;
    //CAL_WR_TRXREGS(0xC);
	#endif
}

UINT32 rwnx_is_tpc_bit_on(void)
{
    return (BK7231N_RC_REG.REG0x4C->bits.TXPOWTBLEN == 1)? 1: 0;
}

UINT32 rwnx_sys_is_enable_hw_tpc(void)
{
    return (gcali_context.is_tpc_used == 1)? 1: 0;
}

void rwnx_tpc_pa_map_init(void)
{
    UINT32 index;

    BK7231N_RC_REG.REG0x4C->bits.TXPOWTBLEN = 0;
    for (index = 0; index < TCP_PAMAP_TAB_LEN; index++)
    {
        REG_WRITE((RCB_POWER_TABLE_ADDR + (index * 4)), cfg_tab_tpc[index]);
    }
    rwnx_use_tpc_set_pwr();

    BK7231N_TRX_RAM.REG0xC.bits.pamapen = 1;
    CAL_WR_TRXREGS(0xC);
}

void rwnx_cal_initial_calibration(void)
{
    rwnx_cal_set_txpwr(40, EVM_DEFUALT_RATE);

    rwnx_tpc_pa_map_init();
}

void rwnx_cal_set_reg_adda_ldo(UINT32 val)
{
//    os_printf("set_reg_adda_ldo:%d \r\n", val);
    
    val = val & 0x3;
	
    BK7231N_TRX_RAM.REG0x12.bits.ldoadda = val;
    CAL_WR_TRXREGS(0x12);
}

void rwnx_cal_en_extra_txpa(void)
{
    BK7231N_TRX_RAM.REG0x10.value = BK7231N_TRX_REG.REG0xD->value;
    BK7231N_TRX_RAM.REG0x10.bits.entxfebias = 0;
    CAL_WR_TRXREGS(0x10);
}

void rwnx_cal_dis_extra_txpa(void)
{
    BK7231N_TRX_RAM.REG0x10.value = BK7231N_TRX_ROM[0x10];
    CAL_WR_TRXREGS(0x10);
}

void rwnx_cal_set_bw_i2v(int enable)
{
    BK7231N_TRX_RAM.REG0x9.bits.bw_i2v = enable ? 1 : 0;
    CAL_WR_TRXREGS(0x9);
}

void rwnx_cal_en_rx_filter_offset(void)
{
    BK7231N_RC_RAM.REG0x5B.bits.RXCALCAPQ = gcali_result.gtx_qfilter_corner+0x64;
    BK7231N_RC_RAM.REG0x5A.bits.RXCALCAPI = gcali_result.gtx_ifilter_corner+0x64;
    BK7231N_RC_REG.REG0x5A->value = BK7231N_RC_RAM.REG0x5A.value;
    BK7231N_RC_REG.REG0x5B->value = BK7231N_RC_RAM.REG0x5B.value;
}

void rwnx_cal_dis_rx_filter_offset(void)
{
    BK7231N_RC_RAM.REG0x5B.bits.RXCALCAPQ = gcali_result.gtx_qfilter_corner;
    BK7231N_RC_RAM.REG0x5A.bits.RXCALCAPI = gcali_result.gtx_ifilter_corner;
    BK7231N_RC_REG.REG0x5A->value = BK7231N_RC_RAM.REG0x5A.value;
    BK7231N_RC_REG.REG0x5B->value = BK7231N_RC_RAM.REG0x5B.value;
}

void rwnx_cal_set_reg_rx_ldo(void)
{
//    os_printf("set_reg_adda_ldo:%d \r\n", val);

    BK7231N_TRX_RAM.REG0x9.bits.vsrxlnaldo10 = 0x3;
    CAL_WR_TRXREGS(0x9);

    //BK7231N_TRX_RAM.REG0x5->bits.cp_ldo = 0x0;
    //BK7231N_TRX_RAM.REG0x5->bits.pll_ldo = 0x0;
    //BK7231N_TRX_RAM.REG0x5->bits.vco_ldo = 0x0;
    //CAL_WR_TRXREGS(0x5);
}

void rwnx_cal_set_40M_extra_setting(UINT8 val)
{
    if (1==val)
    {
        BK7231N_TRX_RAM.REG0xF.bits.clkadc_sel = 1;
        CAL_WR_TRXREGS(0xF);
    }
    else
    {
        BK7231N_TRX_RAM.REG0xF.bits.clkadc_sel = 0;
        CAL_WR_TRXREGS(0xF);
    }
}

void rwnx_cal_set_40M_setting(void)
{
    //BK7231N_TRX_RAM.REG0x12.bits.adcrefbwsel = 1;
    //BK7231N_TRX_RAM.REG0x12.bits.adciselc20 = 0x4;
    //BK7231N_TRX_RAM.REG0x12.bits.adciselr20 = 0x4;
    BK7231N_TRX_RAM.REG0x12.bits.fictrl30 = 7;
    CAL_WR_TRXREGS(0x12);

    BK7231N_TRX_RAM.REG0xD.bits.lpfrxbw = 1;
    BK7231N_TRX_RAM.REG0xD.bits.lpftxbw = 1;
    CAL_WR_TRXREGS(0xD);
    BK7231N_RC_REG.REG0x1A->value = BK7231N_TRX_RAM.REG0xD.value;
   
    BK7231N_TRX_RAM.REG0xE.bits.lpfrxbw = 1;
    BK7231N_TRX_RAM.REG0xE.bits.lpftxbw = 1;
    CAL_WR_TRXREGS(0xE);

    BK7231N_TRX_RAM.REG0x10.bits.lpfrxbw = 1;
    BK7231N_TRX_RAM.REG0x10.bits.lpftxbw = 1;
    CAL_WR_TRXREGS(0x10);

    BK7231N_TRX_RAM.REG0xF.bits.clkdac_sel = 1;
    BK7231N_TRX_RAM.REG0xF.bits.clkadc_sel = 1;
    CAL_WR_TRXREGS(0xF);
}

void rwnx_cal_set_20M_setting(void)
{
    //BK7231N_TRX_RAM.REG0x12.bits.adcrefbwsel = 1;
    //BK7231N_TRX_RAM.REG0x12.bits.adciselc20 = 0x4;
    //BK7231N_TRX_RAM.REG0x12.bits.adciselr20 = 0x4;
    BK7231N_TRX_RAM.REG0x12.bits.fictrl30 = 7;
    CAL_WR_TRXREGS(0x12);

    BK7231N_TRX_RAM.REG0xD.bits.lpfrxbw = 0;
    BK7231N_TRX_RAM.REG0xD.bits.lpftxbw = 0;
    CAL_WR_TRXREGS(0xD);
    BK7231N_RC_REG.REG0x1A->value = BK7231N_TRX_RAM.REG0xD.value;
   
    BK7231N_TRX_RAM.REG0xE.bits.lpfrxbw = 0;
    BK7231N_TRX_RAM.REG0xE.bits.lpftxbw = 0;
    CAL_WR_TRXREGS(0xE);

    BK7231N_TRX_RAM.REG0x10.bits.lpfrxbw = 0;
    BK7231N_TRX_RAM.REG0x10.bits.lpftxbw = 0;
    CAL_WR_TRXREGS(0x10);

    BK7231N_TRX_RAM.REG0xF.bits.clkdac_sel = 0;
    BK7231N_TRX_RAM.REG0xF.bits.clkadc_sel = 0;
    CAL_WR_TRXREGS(0xF);
}

#if CFG_SUPPORT_MANUAL_CALI
void rwnx_cal_set_txpwr(UINT32 pwr_gain, UINT32 grate)
{
    const PWR_REGS *pcfg;
	UINT32 pwr_gain_base = 0;

	g_temp_pwr_current.idx = pwr_gain;
	g_temp_pwr_current.mode = grate;
    
#if CFG_USE_TEMPERATURE_DETECT
    INT16 shift = g_temp_pwr_current.shift;

    if(g_temp_pwr_current.mode == EVM_DEFUALT_RATE)
        shift = g_temp_pwr_current.shift_g;

#ifdef ATE_PRINT_DEBUG
    os_printf("-----pwr_gain:%d, rate=%d, g_idx:%d, shift_b:%d, shift_g:%d\n",
    pwr_gain,
    grate,
    g_temp_pwr_current.idx,
    g_temp_pwr_current.shift,
    g_temp_pwr_current.shift_g);
#endif
    
    pwr_gain = g_temp_pwr_current.idx + shift;

#endif // CFG_USE_TEMPERATURE_DETECT

    if(grate == EVM_DEFUALT_B_RATE) {
    // for b
        if(pwr_gain > TCP_PAMAP_TAB_B_LEN - 1) 
        {
            pwr_gain = TCP_PAMAP_TAB_B_LEN - 1; 
            g_temp_pwr_current.idx = pwr_gain;
        }
        pcfg = cfg_tab_b + pwr_gain;
	pwr_gain_base = 0xE0A2A000;  //E0A2A800
    } else if(grate == EVM_DEFUALT_RATE) {
    // for g
        if(pwr_gain > TCP_PAMAP_TAB_G_LEN - 1) 
        {
            pwr_gain = TCP_PAMAP_TAB_G_LEN - 1; 
            g_temp_pwr_current.idx = pwr_gain;
        }
        pcfg = cfg_tab_g + pwr_gain;
	pwr_gain_base = 0xE0AB1000  ;//  E0AB1800
    } else {
        os_printf("set_txpwr unknow rate:%d \r\n", grate);  
        return;
    }

    if(get_ate_mode_state()) {
#ifdef ATE_PRINT_DEBUG
        os_printf("idx:%02d,r:%03d- pg:0x%02x, %01x, %01x, %01x, %01x, %02x, %02x, %01x,\r\n", pwr_gain, grate,
            pcfg->pregain, pcfg->regb_28_31, pcfg->regc_8_10,pcfg->regc_4_6, pcfg->regc_0_2, 
            pcfg->rega_8_13, pcfg->rega_4_7, pcfg->rega_0_1);
        os_printf("Xtal C: %d\r\n", manual_cal_get_xtal());
#else
        os_printf("idx:%02d\r\n", pwr_gain);
#endif
        #if DIFFERENCE_PIECES_CFG
        os_printf("Mod :0x%x\r\n", bk7011_cal_dcormod_get());
        #endif
    }

    CHECK_OPERATE_RF_REG_IF_IN_SLEEP();

    if (get_ate_mode_state()) {
        REG_WRITE((RCB_POWER_TABLE_ADDR + (0x34 * 4)), pwr_gain_base | pcfg->pregain); //0xE98B7150
    } else if(grate == EVM_DEFUALT_B_RATE) {
        REG_WRITE((RCB_POWER_TABLE_ADDR + (pwr_gain * 4)), pwr_gain_base | pcfg->pregain); //0xE98B7150
    } else {
        REG_WRITE((RCB_POWER_TABLE_ADDR + TCP_PAMAP_TAB_B_LEN * 4 + (pwr_gain * 4)), pwr_gain_base | pcfg->pregain); //0xE98B7150
    }
    //BK7231N_TRX_RAM.REG0xA.bits.dbpab30 = pcfg->rega_4_7;
    //BK7231N_TRX_RAM.REG0xA.bits.dbpaa30 = pcfg->rega_8_13;
    //BK7231N_TRX_RAM.REG0xA.bits.disrefPA10 = pcfg->rega_0_1;
    //CAL_WR_TRXREGS(0xA);

    //BK7231N_TRX_RAM.REG0xB.bits.gctrlmod30 = pcfg->regb_28_31;
#if DIFFERENCE_PIECES_CFG
	//BK7231N_TRX_RAM.REG0xB.bits.dcorMod30 = bk7011_cal_dcormod_get();
#endif
    CAL_WR_TRXREGS(0xB);

    if(grate == EVM_DEFUALT_B_RATE)
    {
        BK7231N_TRX_RAM.REG0xA.bits.Dvb=0xA;     //   0X5063DDFF;//0X8063DDFF
        BK7231N_TRX_RAM.REG0xC.bits.Gmgain=0x8;     //0X04783E98;//0X04782EA8
        //BK7231N_TRX_RAM.REG0x10.value=0XD8C00030;       //enif enlpf for 11b sem
        BK7231N_TRX_RAM.REG0x0.bits.tspi = 0x4;
        BK7231N_TRX_RAM.REG0x0.bits.Dtau = 0x3;
    }
    else if(grate == EVM_DEFUALT_RATE)
    {
        BK7231N_TRX_RAM.REG0xA.bits.Dvb=0x6;       //0X5063DDFF;//0X8063DDFF
        BK7231N_TRX_RAM.REG0xC.bits.Gmgain=0xA;    //0X04783E98;//0X04782EA8
        //BK7231N_TRX_RAM.REG0x10.value=0XD8C00130;     //enif enlpf for 11b sem

        BK7231N_TRX_RAM.REG0x0.bits.tspi = 0x5;
        BK7231N_TRX_RAM.REG0x0.bits.Dtau = 0x8;
    }
    CAL_WR_TRXREGS(0xC);
    CAL_WR_TRXREGS(0xA);
    //CAL_WR_TRXREGS(0x10);
    CAL_WR_TRXREGS(0x0);  
    os_null_printf("%s:%d padctrl=0x%x->0xC,pactrl=0x%x->0xE pregain=0x%x\r\n", __FUNCTION__, __LINE__, pcfg->regc_4_6, pcfg->regc_8_10, pcfg->pregain);
	CHECK_OPERATE_RF_REG_IF_IN_SLEEP_END();    
    g_pwr_current.idx = pwr_gain;
    g_pwr_current.mode = grate;
} 

#if CFG_USE_TEMPERATURE_DETECT
extern UINT32 ble_in_dut_mode(void);
void rwnx_cal_set_txpwr_by_tmpdetect(INT16 shift_b, INT16 shift_g)
{
    UINT32 should_do = 0;
    
    if(g_temp_pwr_current.shift != shift_b)
    {
        g_temp_pwr_current.shift = shift_b;
        should_do = 1;
    }
    
    if(g_temp_pwr_current.shift_g!= shift_g)
    {
        g_temp_pwr_current.shift_g = shift_g;
        should_do = 1;
    }

    if( should_do)
    {
#ifdef ATE_PRINT_DEBUG
        os_printf("td set pwr: shift_b:%d, shift_g:%d, rate:%d\r\n", 
            g_temp_pwr_current.shift,
            g_temp_pwr_current.shift_g, 
            g_temp_pwr_current.mode);
#endif

        if ((rwnx_is_tpc_bit_on() == 0) && (ble_in_dut_mode() == 0))
        {
            rwnx_cal_set_txpwr(g_temp_pwr_current.idx, g_temp_pwr_current.mode);
        }
        else
        {

        }
    }
}  
#endif  // CFG_USE_TEMPERATURE_DETECT

void rwnx_cal_set_reg_mod_pa(UINT16 reg_mod, UINT16 reg_pa)
{
    CHECK_OPERATE_RF_REG_IF_IN_SLEEP();

    gtx_dcorMod = (INT32)reg_mod,
    gtx_dcorPA = (INT32)reg_pa;
    //BK7231N_TRX_REG.REG0xB->bits.dcorMod30 = gtx_dcorMod;
    //BK7231N_TRX_REG.REG0xC->bits.dcorPA30 = gtx_dcorPA;    
    CHECK_OPERATE_RF_REG_IF_IN_SLEEP_END();
}

void rwnx_cal_recover_txpwr_for_wifi(void)
{
    if(gcali_context.is_tpc_used)
    {
        rwnx_use_tpc_set_pwr();
    } 
    else
    {
        rwnx_cal_set_txpwr(g_pwr_current.idx, g_pwr_current.mode);
    }
}

#endif

#if CFG_USE_TEMPERATURE_DETECT
#ifdef INCLUDE_OS
extern void bk7011_cal_rx_adc_restore(int enter_or_exit);
extern void bk7011_cal_rx_adc_dlym();
void rwnx_cal_do_temp_detect(UINT16 cur_val, UINT16 thre, UINT16 *last)
{
    static UINT16 last_val;
    TMP_PWR_PTR tmp_pwr_ptr;

    if (cur_val == 0)
    {
        /* 0 is invalid, but saradc may return 0 in power save mode */
        return;
    }
    tmp_pwr_ptr = manual_cal_set_tmp_pwr(cur_val, thre, last);
    if(tmp_pwr_ptr) 
    {
        if ((last_val >= cur_val + ADC_TMEP_LSB_PER_10DEGREE) || (cur_val >= last_val + ADC_TMEP_LSB_PER_10DEGREE))
        {
            bk7011_cal_rx_adc_restore(1);
            bk7011_cal_rx_adc_dlym();
            bk7011_cal_rx_adc_restore(0);
            last_val = cur_val;
        }
        #if CFG_TEMP_DETECT_VERSION == CFG_TEMP_DETECT_VERSION1
		#if CFG_TEMP_DIFF_PWR_FREQOFFSET
		//BK7231N_TRX_RAM.REG0xC.bits.dcorPA30 = tmp_pwr_ptr->trx0x0c_12_15;
		CAL_WR_TRXREGS(0xC);
        
		manual_cal_do_xtal_temp_delta_set(tmp_pwr_ptr->xtal_c_dlta);
        manual_cal_do_xtal_cali(cur_val, 0, 0, 0);
        
		rwnx_cal_set_txpwr_by_tmpdetect((INT16)tmp_pwr_ptr->p_index_delta, (INT16)tmp_pwr_ptr->p_index_delta_g);
        rwnx_set_tpc_txpwr_by_tmpdetect((INT16)tmp_pwr_ptr->p_index_delta, (INT16)tmp_pwr_ptr->p_index_delta_g);
        #endif
	    #else
        rwnx_cal_set_reg_mod_pa(tmp_pwr_ptr->mod, tmp_pwr_ptr->pa);
	    #endif
        
    }
}
#endif
#endif // CFG_USE_TEMPERATURE_DETECT

void rwnx_tx_cal_save_cal_result(void)
{
    // Manual calibration not used PA & MOD, but use pre_gain
    #if CFG_SUPPORT_MANUAL_CALI 
   // gtx_dcorMod = 0xc;//0x8;
    //gtx_dcorPA = 0x5;//0x8;
    #if CFG_USE_TEMPERATURE_DETECT
    manual_cal_tmp_pwr_init_reg(gtx_dcorMod, gtx_dcorPA);
    #endif  // CFG_USE_TEMPERATURE_DETECT
    #endif

    //BK7231N_TRX_RAM.REG0xB.bits.padCa = gtx_dcorMod; //changed
    //BK7231N_TRX_RAM.REG0xC.bits.Rgm = gtx_dcorPA; //changed
    //BK7231N_RC_RAM.REG0x52.bits.Reserved_ = gtx_pre_gain;

    BK7231N_RC_RAM.REG0x4F.bits.TXIDCCOMP = gcali_result.gtx_i_dc_comp;
    BK7231N_RC_RAM.REG0x4F.bits.TXQDCCOMP = gcali_result.gtx_q_dc_comp;

    BK7231N_RC_RAM.REG0x50.bits.TXIGAINCOMP = gcali_result.gtx_i_gain_comp;
    BK7231N_RC_RAM.REG0x50.bits.TXQGAINCOMP = gcali_result.gtx_q_gain_comp;

    BK7231N_TRX_RAM.REG0x6.bits.lpfcapcalq50 = gcali_result.gtx_qfilter_corner & 0x3F;
    BK7231N_TRX_RAM.REG0x7.bits.lpfcapcalq = (gcali_result.gtx_qfilter_corner >> 6) & 0x3;
    BK7231N_TRX_RAM.REG0x6.bits.lpfcapcali50 = gcali_result.gtx_ifilter_corner & 0x3F; 
    BK7231N_TRX_RAM.REG0x7.bits.lpfcapcali = (gcali_result.gtx_ifilter_corner >> 6) & 0x3; 

    BK7231N_RC_RAM.REG0x51.bits.TXPHASECOMP = gcali_result.gtx_phase_comp;
    BK7231N_RC_RAM.REG0x51.bits.TXTY2 = gcali_result.gtx_phase_ty2;

    BK7231N_RC_RAM.REG0x5A.bits.TXCALCAPI = gcali_result.gtx_ifilter_corner;
    BK7231N_RC_RAM.REG0x5A.bits.RXCALCAPI = gcali_result.gtx_ifilter_corner+0x64;
    BK7231N_RC_RAM.REG0x5A.bits.STANDBYCALCAPI = gcali_result.gtx_ifilter_corner;

    BK7231N_RC_RAM.REG0x5B.bits.STANDBYCALCAPQ = gcali_result.gtx_qfilter_corner;
    BK7231N_RC_RAM.REG0x5B.bits.RXCALCAPQ = gcali_result.gtx_qfilter_corner+0x64;
    BK7231N_RC_RAM.REG0x5B.bits.TXCALCAPQ = gcali_result.gtx_qfilter_corner;

    //if(gstat_cal)
    //BK7231N_RC_RAM[16] = BK7231N_RC_RAM[16] | (1 << 29);
    //else
    //BK7231N_RC_RAM[16] = BK7231N_RC_RAM[16] & (~(1 << 29));
}

void rwnx_rx_cal_save_cal_result(void)
{
    BK7231N_TRX_RAM.REG0x14.value = gcali_result.g_rx_dc_gain_tab[0];
    BK7231N_TRX_RAM.REG0x15.value = gcali_result.g_rx_dc_gain_tab[1];
    BK7231N_TRX_RAM.REG0x16.value = gcali_result.g_rx_dc_gain_tab[2];
    BK7231N_TRX_RAM.REG0x17.value = gcali_result.g_rx_dc_gain_tab[3];
    BK7231N_TRX_RAM.REG0x18.value = gcali_result.g_rx_dc_gain_tab[4];
    BK7231N_TRX_RAM.REG0x19.value = gcali_result.g_rx_dc_gain_tab[5];
    BK7231N_TRX_RAM.REG0x1A.value = gcali_result.g_rx_dc_gain_tab[6];
    BK7231N_TRX_RAM.REG0x1B.value = gcali_result.g_rx_dc_gain_tab[7];

    BK7231N_RC_RAM.REG0x42.bits.RXAMPERRWR = gcali_result.grx_amp_err_wr;
    BK7231N_RC_RAM.REG0x42.bits.RXPHASEERRWR = gcali_result.grx_phase_err_wr;
}

/*******************************************************************************
* Function Implemantation
*******************************************************************************/
void bk7011_read_cal_param(void)
{
    gcali_context.gtx_dc_n = 2;//gst_sar_adc=4  gst_rx_adc=2
#if 0
    gcali_context.gst_sar_adc = ((BK7231N_RC_REG.REG0x54->bits.STSARADC & 0x03) + 1) * CAL_DELAY05US;//CAL_DELAY05US= 2
    gcali_context.gst_rx_adc = ((BK7231N_RC_REG.REG0x54->bits.STRXADC & 0x03) + 1) *  CAL_DELAY100US;//CAL_DELAY100US=1
#endif
    gconst_pout = BK7231N_RC_REG.REG0x52->bits.IQCONSTANTPOUT;

    return;
}

static void rwnx_cal_recover_rcbeken_reg_val(void)
{
    BK7231N_RC_REG.REG0x0->value  = BK7231N_RC_RAM.REG0x0.value;
    BK7231N_RC_REG.REG0x1->value  = BK7231N_RC_RAM.REG0x1.value;
    BK7231N_RC_REG.REG0x5->value  = BK7231N_RC_RAM.REG0x5.value;
    BK7231N_RC_REG.REG0x8->value  = BK7231N_RC_RAM.REG0x8.value;
    BK7231N_RC_REG.REG0xB->value  = BK7231N_RC_RAM.REG0xB.value;
    BK7231N_RC_REG.REG0xE->value  = BK7231N_RC_RAM.REG0xE.value;
    BK7231N_RC_REG.REG0x11->value = BK7231N_RC_RAM.REG0x11.value;
    BK7231N_RC_REG.REG0x19->value = BK7231N_RC_RAM.REG0x19.value;
    //BK7231N_RC_REG.REG0x1A->value = BK7231N_RC_RAM.REG0x1A.value;
    BK7231N_RC_REG.REG0x1C->value = BK7231N_RC_RAM.REG0x1C.value;
    BK7231N_RC_REG.REG0x1E->value = BK7231N_RC_RAM.REG0x1E.value;

    /**********NEW ADDED************/
    BK7231N_RC_REG.REG0x3C->value = BK7231N_RC_RAM.REG0x3C.value;
    BK7231N_RC_REG.REG0x3E->value = BK7231N_RC_RAM.REG0x3E.value;
    BK7231N_RC_REG.REG0x3F->value = BK7231N_RC_RAM.REG0x3F.value;
    BK7231N_RC_REG.REG0x40->value = BK7231N_RC_RAM.REG0x40.value;
    BK7231N_RC_REG.REG0x41->value = BK7231N_RC_RAM.REG0x41.value;
    BK7231N_RC_REG.REG0x42->value = BK7231N_RC_RAM.REG0x42.value;
    //BK7231N_RC_REG.REG0x43->value = BK7231N_RC_RAM.REG0x43.value;
    BK7231N_RC_REG.REG0x4C->value = BK7231N_RC_RAM.REG0x4C.value;
    BK7231N_RC_REG.REG0x4D->value = BK7231N_RC_RAM.REG0x4D.value;
    BK7231N_RC_REG.REG0x4F->value = BK7231N_RC_RAM.REG0x4F.value;
    BK7231N_RC_REG.REG0x50->value = BK7231N_RC_RAM.REG0x50.value;
    BK7231N_RC_REG.REG0x51->value = BK7231N_RC_RAM.REG0x51.value;
    BK7231N_RC_REG.REG0x52->value = BK7231N_RC_RAM.REG0x52.value;
    BK7231N_RC_REG.REG0x54->value = BK7231N_RC_RAM.REG0x54.value;
    BK7231N_RC_REG.REG0x55->value = BK7231N_RC_RAM.REG0x55.value;
    BK7231N_RC_REG.REG0x5C->value = BK7231N_RC_RAM.REG0x5C.value;

    BK7231N_RC_REG.REG0x4E->value = BK7231N_RC_RAM.REG0x4E.value;
    //BK7231N_RC_REG.REG0x56->value = BK7231N_RC_RAM.REG0x56.value;
    //BK7231N_RC_REG.REG0x57->value = BK7231N_RC_RAM.REG0x57.value;
    BK7231N_RC_REG.REG0x5A->value = BK7231N_RC_RAM.REG0x5A.value;
    BK7231N_RC_REG.REG0x5B->value = BK7231N_RC_RAM.REG0x5B.value;
    BK7231N_RC_REG.REG0x6A->value = BK7231N_RC_RAM.REG0x6A.value;
}

static void rwnx_cal_recover_trx_reg_val(void)
{
    BK7231N_TRX_REG.REG0x0->value = BK7231N_TRX_RAM.REG0x0.value;
    BK7231N_TRX_REG.REG0x1->value = BK7231N_TRX_RAM.REG0x1.value;
    BK7231N_TRX_REG.REG0x2->value = BK7231N_TRX_RAM.REG0x2.value;
    BK7231N_TRX_REG.REG0x3->value = BK7231N_TRX_RAM.REG0x3.value;
    BK7231N_TRX_REG.REG0x4->value = BK7231N_TRX_RAM.REG0x4.value;
    BK7231N_TRX_REG.REG0x5->value = BK7231N_TRX_RAM.REG0x5.value;
    BK7231N_TRX_REG.REG0x6->value = BK7231N_TRX_RAM.REG0x6.value;
    BK7231N_TRX_REG.REG0x7->value = BK7231N_TRX_RAM.REG0x7.value;
    BK7231N_TRX_REG.REG0x8->value = BK7231N_TRX_RAM.REG0x8.value;
    BK7231N_TRX_REG.REG0x9->value = BK7231N_TRX_RAM.REG0x9.value;
    BK7231N_TRX_REG.REG0xA->value = BK7231N_TRX_RAM.REG0xA.value;
    BK7231N_TRX_REG.REG0xB->value = BK7231N_TRX_RAM.REG0xB.value;
    BK7231N_TRX_REG.REG0xC->value = BK7231N_TRX_RAM.REG0xC.value;
    BK7231N_TRX_REG.REG0xD->value = BK7231N_TRX_RAM.REG0xD.value;
    BK7231N_TRX_REG.REG0xE->value = BK7231N_TRX_RAM.REG0xE.value;
    BK7231N_TRX_REG.REG0xF->value = BK7231N_TRX_RAM.REG0xF.value;
    BK7231N_TRX_REG.REG0x10->value = BK7231N_TRX_RAM.REG0x10.value;
    BK7231N_TRX_REG.REG0x11->value = BK7231N_TRX_RAM.REG0x11.value;
    BK7231N_TRX_REG.REG0x12->value = BK7231N_TRX_RAM.REG0x12.value;
    BK7231N_TRX_REG.REG0x13->value = BK7231N_TRX_RAM.REG0x13.value;
    BK7231N_TRX_REG.REG0x14->value = BK7231N_TRX_RAM.REG0x14.value;
    BK7231N_TRX_REG.REG0x15->value = BK7231N_TRX_RAM.REG0x15.value;
    BK7231N_TRX_REG.REG0x16->value = BK7231N_TRX_RAM.REG0x16.value;
    BK7231N_TRX_REG.REG0x17->value = BK7231N_TRX_RAM.REG0x17.value;
    BK7231N_TRX_REG.REG0x18->value = BK7231N_TRX_RAM.REG0x18.value;
    BK7231N_TRX_REG.REG0x19->value = BK7231N_TRX_RAM.REG0x19.value;
    BK7231N_TRX_REG.REG0x1A->value = BK7231N_TRX_RAM.REG0x1A.value;
    BK7231N_TRX_REG.REG0x1B->value = BK7231N_TRX_RAM.REG0x1B.value;

    BK7231N_RC_REG.REG0x1A->value = BK7231N_TRX_RAM.REG0xD.value;

}

void rwnx_cal_recover_rf_setting(void)
{
    rwnx_cal_recover_rcbeken_reg_val();
    rwnx_cal_recover_trx_reg_val();
}

void rwnx_cal_recover_wifi_setting(void)
{
    //rwnx_cal_recover_trx_reg_val();
    rwnx_tpc_pa_map_init();
}

INT32 rwnx_cal_load_trx_rcbekn_reg_val(void)
{
#if (CFG_RUNNING_PLATFORM == FPGA_PLATFORM)

#else
    rwnx_cal_recover_rcbeken_reg_val();
#endif

    while(BK7231N_RC_REG.REG0x1->value & 0x0FFFFFFF)
    {
        cpu_delay(1);
    }
    rwnx_cal_recover_trx_reg_val();
    while(BK7231N_RC_REG.REG0x1->value & 0x0FFFFFFF)
    {
        cpu_delay(1);
    }

#if No_TXPOWERCAL
    BK7231N_TRX_RAM.REG0xB.value = TRX_REG_0XB_VAL;
    CAL_WR_TRXREGS(0xB);
    BK7231N_TRX_RAM.REG0xC.value = TRX_REG_0XC_VAL;
    CAL_WR_TRXREGS(0xC);

    //BK7231N_RC_REG.REG0x52->bits.TXPREGAIN = 7;
#endif

    // cal rf pll when reload trx and rc beken value
    bk7011_cal_pll();

    return 0;
}

INT32 rwnx_cal_save_trx_rcbekn_reg_val(void)
{
#if (CFG_RUNNING_PLATFORM == FPGA_PLATFORM)

#else
    BK7231N_RC_RAM.REG0x0.value = BK7231N_RC_REG.REG0x0->value;
    BK7231N_RC_RAM.REG0x1.value = BK7231N_RC_REG.REG0x1->value;
    BK7231N_RC_RAM.REG0x5.value = BK7231N_RC_REG.REG0x5->value;
    BK7231N_RC_RAM.REG0x8.value = BK7231N_RC_REG.REG0x8->value;
    BK7231N_RC_RAM.REG0xB.value = BK7231N_RC_REG.REG0xB->value;
    BK7231N_RC_RAM.REG0xE.value = BK7231N_RC_REG.REG0xE->value;
    BK7231N_RC_RAM.REG0x11.value = BK7231N_RC_REG.REG0x11->value;
    BK7231N_RC_RAM.REG0x19.value = BK7231N_RC_REG.REG0x19->value;
    BK7231N_RC_RAM.REG0x1C.value = BK7231N_RC_REG.REG0x1C->value;
    BK7231N_RC_RAM.REG0x1E.value = BK7231N_RC_REG.REG0x1E->value;

    /**********NEW ADDED************/
    BK7231N_RC_RAM.REG0x3C.value = BK7231N_RC_REG.REG0x3C->value;
    BK7231N_RC_RAM.REG0x3E.value = BK7231N_RC_REG.REG0x3E->value;
    BK7231N_RC_RAM.REG0x3F.value = BK7231N_RC_REG.REG0x3F->value;
    BK7231N_RC_RAM.REG0x40.value = BK7231N_RC_REG.REG0x40->value;
    BK7231N_RC_RAM.REG0x41.value = BK7231N_RC_REG.REG0x41->value;
    BK7231N_RC_RAM.REG0x42.value = BK7231N_RC_REG.REG0x42->value;
    BK7231N_RC_RAM.REG0x4C.value = BK7231N_RC_REG.REG0x4C->value;
    BK7231N_RC_RAM.REG0x4D.value = BK7231N_RC_REG.REG0x4D->value;
    BK7231N_RC_RAM.REG0x4F.value = BK7231N_RC_REG.REG0x4F->value;
    BK7231N_RC_RAM.REG0x50.value = BK7231N_RC_REG.REG0x50->value;
    BK7231N_RC_RAM.REG0x51.value = BK7231N_RC_REG.REG0x51->value;
    BK7231N_RC_RAM.REG0x52.value = BK7231N_RC_REG.REG0x52->value;
    BK7231N_RC_RAM.REG0x54.value = BK7231N_RC_REG.REG0x54->value;
    BK7231N_RC_RAM.REG0x55.value = BK7231N_RC_REG.REG0x55->value;
    BK7231N_RC_RAM.REG0x5C.value = BK7231N_RC_REG.REG0x5C->value;

    BK7231N_RC_RAM.REG0x4E.value = BK7231N_RC_REG.REG0x4E->value;
    BK7231N_RC_RAM.REG0x5A.value = BK7231N_RC_REG.REG0x5A->value;
    BK7231N_RC_RAM.REG0x5B.value = BK7231N_RC_REG.REG0x5B->value;
    BK7231N_RC_RAM.REG0x6A.value = BK7231N_RC_REG.REG0x6A->value;
#if 0
    BK7231N_RC_RAM[29] = BK7231N_RC_REG.REG0x70->value;
    BK7231N_RC_RAM[30] = BK7231N_RC_REG.REG0x71->value;
    BK7231N_RC_RAM[31] = BK7231N_RC_REG.REG0x72->value;
    BK7231N_RC_RAM[32] = BK7231N_RC_REG.REG0x73->value;
    BK7231N_RC_RAM[33] = BK7231N_RC_REG.REG0x74->value;
    BK7231N_RC_RAM[34] = BK7231N_RC_REG.REG0x75->value;
    BK7231N_RC_RAM[35] = BK7231N_RC_REG.REG0x76->value;
    BK7231N_RC_RAM[36] = BK7231N_RC_REG.REG0x77->value;
#endif
    //BK7231N_RC_REG.REG0x3C->bits.RXIQSWAP = 1; /* I/Q SWAP*/

#endif

    BK7231N_TRX_RAM.REG0x0.value  = BK7231N_TRX_REG.REG0x0->value ;
    BK7231N_TRX_RAM.REG0x1.value  = BK7231N_TRX_REG.REG0x1->value ;
    BK7231N_TRX_RAM.REG0x2.value  = BK7231N_TRX_REG.REG0x2->value ;
    BK7231N_TRX_RAM.REG0x3.value  = BK7231N_TRX_REG.REG0x3->value ;
    BK7231N_TRX_RAM.REG0x4.value  = BK7231N_TRX_REG.REG0x4->value ;
    BK7231N_TRX_RAM.REG0x5.value  = BK7231N_TRX_REG.REG0x5->value ;
    BK7231N_TRX_RAM.REG0x6.value  = BK7231N_TRX_REG.REG0x6->value ;
    BK7231N_TRX_RAM.REG0x7.value  = BK7231N_TRX_REG.REG0x7->value ;
    BK7231N_TRX_RAM.REG0x8.value  = BK7231N_TRX_REG.REG0x8->value ;
    BK7231N_TRX_RAM.REG0x9.value  = BK7231N_TRX_REG.REG0x9->value ;
    BK7231N_TRX_RAM.REG0xA.value = BK7231N_TRX_REG.REG0xA->value ;
    BK7231N_TRX_RAM.REG0xB.value = BK7231N_TRX_REG.REG0xB->value ;
    BK7231N_TRX_RAM.REG0xC.value = BK7231N_TRX_REG.REG0xC->value ;
    BK7231N_TRX_RAM.REG0xD.value = BK7231N_TRX_REG.REG0xD->value ;
    BK7231N_TRX_RAM.REG0xE.value = BK7231N_TRX_REG.REG0xE->value ;
    BK7231N_TRX_RAM.REG0xF.value = BK7231N_TRX_REG.REG0xF->value ;
    BK7231N_TRX_RAM.REG0x10.value = BK7231N_TRX_REG.REG0x10->value;
    BK7231N_TRX_RAM.REG0x11.value = BK7231N_TRX_REG.REG0x11->value;
    BK7231N_TRX_RAM.REG0x12.value = BK7231N_TRX_REG.REG0x12->value;
    BK7231N_TRX_RAM.REG0x13.value = BK7231N_TRX_REG.REG0x13->value;
    BK7231N_TRX_RAM.REG0x14.value = BK7231N_TRX_REG.REG0x14->value;
    BK7231N_TRX_RAM.REG0x15.value = BK7231N_TRX_REG.REG0x15->value;
    BK7231N_TRX_RAM.REG0x16.value = BK7231N_TRX_REG.REG0x16->value;
    BK7231N_TRX_RAM.REG0x17.value = BK7231N_TRX_REG.REG0x17->value;
    BK7231N_TRX_RAM.REG0x18.value = BK7231N_TRX_REG.REG0x18->value;
    BK7231N_TRX_RAM.REG0x19.value = BK7231N_TRX_REG.REG0x19->value;
    BK7231N_TRX_RAM.REG0x1A.value = BK7231N_TRX_REG.REG0x1A->value;
    BK7231N_TRX_RAM.REG0x1B.value = BK7231N_TRX_REG.REG0x1B->value;

    while(BK7231N_RC_REG.REG0x1->value & 0x0FFFFFFF)
    {
        cpu_delay(1);
    }

    return 0;
}

typedef struct
{
    DD_HANDLE handle;
    saradc_desc_t desc;
    UINT16 buffer[32];
} cali_saradc_desc_t;
static cali_saradc_desc_t *cali_saradc_desc;

void bk7011_cal_saradc_close(cali_saradc_desc_t *cali_saradc_ptr)
{
    UINT32 param;

    if (NULL != cali_saradc_ptr)
    {
        INT32 value = 0;
        ddev_control(cali_saradc_desc->handle, SARADC_CMD_SET_BYPASS_CALIB, &value);

        ddev_close(cali_saradc_ptr->handle);
        cali_saradc_ptr->handle = DD_HANDLE_UNVALID;
    }

    param = PWD_SARADC_CLK_BIT;
    sddev_control(ICU_DEV_NAME, CMD_CLK_PWR_DOWN, &param);

    //param = (1 << 30); //the default value is 0 in bk7231n
    //sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_ANALOG_CTRL4_SET, &param);

    param = BLK_BIT_SARADC;
    sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_BLK_DISABLE, &param);

    //param = sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_GET_ANALOG2, &param);
    //param &= ~(1 << 12);
    //sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_SET_ANALOG2, &param);

    free(cali_saradc_ptr);
    cali_saradc_desc = NULL;

    //BK7231N_TRX_RAM.REG0x2.bits.ampctrl_m = 0;
    BK7231N_TRX_RAM.REG0x9.bits.agcrxfeEn = 1;
    BK7231N_TRX_RAM.REG0x12.bits.buftstselection = 0;
    BK7231N_TRX_RAM.REG0x12.bits.entstbufldo = 0;
    BK7231N_TRX_RAM.REG0x6.bits.capcal_sel = 1;
	
    //CAL_WR_TRXREGS(0x2);
    CAL_WR_TRXREGS(0x9);
    CAL_WR_TRXREGS(0x12);
    CAL_WR_TRXREGS(0x6);

}

cali_saradc_desc_t *bk7011_cal_saradc_open()
{
    UINT32 param;
	
    //BK7231N_TRX_RAM.REG0x2.bits.ampctrl_m = 1;
    BK7231N_TRX_RAM.REG0xD.bits.entssi = 1;
    BK7231N_TRX_RAM.REG0x12.bits.buftstselection = 3;
    BK7231N_TRX_RAM.REG0x12.bits.entstbufldo = 1;
    //CAL_WR_TRXREGS(0x2);
    CAL_WR_TRXREGS(0xD);
    CAL_WR_TRXREGS(0x12);
    BK7231N_RC_REG.REG0x1A->value = BK7231N_TRX_RAM.REG0xD.value;

    if (NULL != cali_saradc_desc)
    {
        return cali_saradc_desc;
    }

    cali_saradc_desc = (cali_saradc_desc_t *)malloc(sizeof(cali_saradc_desc_t));
    if (NULL == cali_saradc_desc)
    {
        return cali_saradc_desc;
    }

    param = PWD_SARADC_CLK_BIT;
    sddev_control(ICU_DEV_NAME, CMD_CLK_PWR_UP, &param);//REG_0x2£º00FFFD7C

    //param = (1 << 30); //the default value is 0 in bk7231n
    //sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_ANALOG_CTRL4_CLEAN, &param);
    //delay100us(100);

    param = BLK_BIT_SARADC;
    sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_BLK_ENABLE, &param);

    //param = sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_GET_ANALOG2, &param);
    //param |= (1 << 12);
    //sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_SET_ANALOG2, &param);

    memset(cali_saradc_desc, 0x00, sizeof(cali_saradc_desc_t));
    cali_saradc_desc->desc.channel = 8;
    cali_saradc_desc->desc.data_buff_size = sizeof(cali_saradc_desc->buffer) / sizeof(cali_saradc_desc->buffer[0]);
    cali_saradc_desc->desc.mode = (ADC_CONFIG_MODE_CONTINUE << 0)
                          |(ADC_CONFIG_MODE_4CLK_DELAY << 2);
    cali_saradc_desc->desc.has_data                = 0;
    cali_saradc_desc->desc.current_read_data_cnt   = 0;
    cali_saradc_desc->desc.current_sample_data_cnt = 0;
    cali_saradc_desc->desc.pre_div = 0x04;//0x10; /* 26/[2*(4+1)]=2.6 < 120/[2*(16+1)]=3.53 */
    cali_saradc_desc->desc.samp_rate = 0x20;
    cali_saradc_desc->desc.pData = cali_saradc_desc->buffer;

    cali_saradc_desc->handle = ddev_open(SARADC_DEV_NAME, &param, (UINT32)&cali_saradc_desc->desc);
    if (DD_HANDLE_UNVALID == cali_saradc_desc->handle)
    {
        bk_printf("ddev_open(%s) failed\n", SARADC_DEV_NAME);
        bk7011_cal_saradc_close(cali_saradc_desc);
        cali_saradc_desc = NULL;
    }

    {
        INT32 value = 1;
        ddev_control(cali_saradc_desc->handle, SARADC_CMD_SET_BYPASS_CALIB, &value);
    }

    return cali_saradc_desc;
}

UINT32 bk7011_cal_saradc_runorstop(cali_saradc_desc_t *cali_saradc_ptr, UINT8 run_stop)
{
    if (NULL == cali_saradc_ptr)
    {
        return DRV_FAILURE;
    }
    return ddev_control(cali_saradc_ptr->handle, SARADC_CMD_RUN_OR_STOP_ADC, (VOID *)&run_stop);
}

UINT32 bk7011_cal_saradc_read(cali_saradc_desc_t *cali_saradc_ptr)
{
    INT32 index;
    INT32 count = 20;
    UINT32 sum = 0;

    delay05us(15);
    for (index = 0; index < count; index++) {
        sum += REG_READ(SCTRL_BASE + 0x22 * 4);
    }

	return sum / count;
}

void bk7011_cal_ready(void)
{
    rwnx_cal_load_trx_rcbekn_reg_val();

    bk7011_cal_saradc_open();

    bk7011_read_cal_param();
    rwnx_cal_load_default_result();

    cpu_delay(1000);
    /*step 4.2*/
    BK7231N_RC_REG.REG0x4C->bits.TXCOMPDIS = 0;
    //gstat_cal = 0; // calibration start

    return;
}

void bk7011_cal_dpll(void)
{
    sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_CALI_DPLL, NULL);
}

void bk7011_update_tx_power_when_cal_dpll(int start_or_stop)
{
    extern UINT32 gtest_mode;
    static UINT8 TRX_Oxc_pactrl;
    static UINT32 RCB_PWTBL_0x34;

    if (!gtest_mode)
    {
        return;
    }

    //bk_printf("[in] TRX_Oxc_pactrl=0x%x,RCB_PWTBL_0x34=0x%x\n", BK7231N_TRX_REG.REG0xC->value, REG_READ(RCB_POWER_TABLE_ADDR + (0x34 * 4)));
    if (start_or_stop)
    {
        /* start */
        /* 1. save TRX_Oxc, RCB_PWTBL_0x34 */
        TRX_Oxc_pactrl = BK7231N_TRX_RAM.REG0xC.bits.pactrl;
        RCB_PWTBL_0x34 = REG_READ(RCB_POWER_TABLE_ADDR + (0x34 * 4));
        /* 2. set TRX_Oxc<11:8>=0, RCB_PWTBL_0x34<31:30>=0 */
        BK7231N_TRX_RAM.REG0xC.bits.pactrl = 0x0;
        REG_WRITE((RCB_POWER_TABLE_ADDR + (0x34 * 4)), RCB_PWTBL_0x34 & 0x3FFFFFFF);
    }
    else
    {
        /* stop */
        /* 3. restore TRX_Oxc, RCB_PWTBL_0x34 */
        BK7231N_TRX_RAM.REG0xC.bits.pactrl = TRX_Oxc_pactrl;
        REG_WRITE((RCB_POWER_TABLE_ADDR + (0x34 * 4)), RCB_PWTBL_0x34);
    }
    CAL_WR_TRXREGS(0xC);
    //bk_printf("[out] TRX_Oxc_pactrl=0x%x,RCB_PWTBL_0x34=0x%x\n", BK7231N_TRX_REG.REG0xC->value, REG_READ(RCB_POWER_TABLE_ADDR + (0x34 * 4)));
}

#define BAND_CAL_GPIO_TIMES            10
#define BAND_CAL_ADD_STEP              8
#define BAND_CAL_VAL_MAX               0x7FU
#define BAND_CAL_VAL_MIN               0x00U

#define BIAS_DIFF_VAL1       (6u)
#define BIAS_DIFF_VAL2       (2u)
void bk7011_cal_bias(void)
{
    extern void bk7011_update_tx_power_when_cal_dpll(int start_or_stop);
    UINT32 param, param2;
    bk7011_update_tx_power_when_cal_dpll(1);
    //    BK7231N_TRX_RAM.REG0xF->bits.biascalmanual = 0;
    param = PARAM_BIAS_CAL_MANUAL_BIT;
    sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_BIAS_REG_CLEAN, &param);

    //    BK7231N_TRX_RAM.REG0xF->bits.biascaltrig = 0;
    param = PARAM_BIAS_CAL_TRIGGER_BIT;
    sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_BIAS_REG_CLEAN, &param);
    //trx_reg_is_write(st_TRXREG0F);
    //BK7231N_TRX_REG.REG0xF->value = BK7231N_TRX_RAM.REG0xF->value;
    cpu_delay(100);
    //    BK7231N_TRX_RAM.REG0xF->bits.biascaltrig = 1;
    param = PARAM_BIAS_CAL_TRIGGER_BIT;
    sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_BIAS_REG_SET, &param);

    //trx_reg_is_write(st_TRXREG0F);
    //BK7231N_TRX_REG.REG0xF->value = BK7231N_TRX_RAM.REG0xF->value;
    cpu_delay(DELAY1US * 40);//40us = 30 + 10;

    //Read SYS_CTRL.REG0x4C->bias_cal_out
    param = sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_BIAS_REG_READ, &param);
    param = (param >> PARAM_BIAS_CAL_OUT_POSI) & PARAM_BIAS_CAL_OUT_MASK;

    //First, Write SYS_CTRL.REG0x4C->ldo_val_man = bias_cal_out + BIAS_DIFF_VAL1
    param += BIAS_DIFF_VAL1;
    param2 = param;
    if (param > 0x1f) param = 0x1f;
    param = ((param & PARAM_BIAS_CAL_OUT_MASK) << PARAM_LDO_VAL_MANUAL_POSI)
            | PARAM_BIAS_CAL_MANUAL_BIT;
    sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_BIAS_REG_WRITE, &param);

    //Second, Write SYS_CTRL.REG0x4C->ldo_val_man = ldo_val_man - BIAS_DIFF_VAL2
    param = param2 - BIAS_DIFF_VAL2;
    if (param > 0x1f) param = 0x1f;
    gcali_result.gbias_after_cal = param;
    param = ((param & PARAM_BIAS_CAL_OUT_MASK) << PARAM_LDO_VAL_MANUAL_POSI)
            | PARAM_BIAS_CAL_MANUAL_BIT;
    sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_BIAS_REG_WRITE, &param);


    //param = ((0x3 & PARAM_VSEL_SYS_LDO_MASK)<< PARAM_VSEL_SYS_LDO_POSI);
    //sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_ANALOG_CTRL4_SET, &param);
    bk7011_update_tx_power_when_cal_dpll(0);

    return;
}

extern UINT32 gtest_mode;
static UINT8 bandgap_calm_in_efuse = 0xFF;
static temperature_type last_temperature_type = TEMPERATURE_TYPE_UNKNOWN; /* uninitialized when power up */
void bk7011_cal_vdddig_by_temperature(temperature_type new_temperature_type)
{
    UINT32 param;

    if (new_temperature_type == last_temperature_type)
    {
        return;
    }

    if (gtest_mode)
    {
        return;
    }

    bk_printf("temperature_type=%d\n", new_temperature_type);
    if (TEMPERATURE_TYPE_LOW == new_temperature_type)
    {
        param = 5;
        BK7231N_TRX_RAM.REG0x4.bits.Itune_vco_spi = 0x19;
    }
    else if (TEMPERATURE_TYPE_HIGH == new_temperature_type)
    {
        param = 5;
        BK7231N_TRX_RAM.REG0x4.bits.Itune_vco_spi = 0x2C;
    }
    else
    {
        param = 4;
        BK7231N_TRX_RAM.REG0x4.bits.Itune_vco_spi = 0x19;
    }
    CAL_WR_TRXREGS(0x4);
    /* keep vdddig all the time */
    //sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_SET_VDD_VALUE, &param);

    last_temperature_type = new_temperature_type;
}

void manual_cal_load_bandgap_calm(void)
{
#define VDDIG_MAX 6
#define VDDIG_MIN 3
    int32_t old_vddig;
    int32_t new_vddig;
    uint32_t analog2;
    uint32_t old_bandgap_calm;
    uint32_t new_bandgap_calm;

    bandgap_calm_in_efuse = wifi_read_efuse(17);
    if ((0xFF == bandgap_calm_in_efuse) || (0x00 == bandgap_calm_in_efuse))
    {
        bandgap_calm_in_efuse = 0xFF;
        return;
    }
    bk_printf("bandgap_calm_in_efuse=0x%x\r\n", bandgap_calm_in_efuse);
    analog2 = sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_GET_ANALOG2, &analog2);
    old_bandgap_calm = (analog2 >> BANDGAP_CAL_MANUAL_POSI) & BANDGAP_CAL_MANUAL_MASK;
    old_vddig = (int32_t)sctrl_ctrl(CMD_SCTRL_GET_VDD_VALUE, NULL);
    new_bandgap_calm = (uint32_t)(bandgap_calm_in_efuse & 0x3F);
    new_vddig = (int32_t)((bandgap_calm_in_efuse >> 6) & 0x3);
    if (3 != new_vddig)
    {
        new_vddig += 4;
    }
    if ((old_bandgap_calm != new_bandgap_calm) || (old_vddig != new_vddig))
    {
        /* keep bandgap_calm in [0x10, 0x30) */
        if ((new_bandgap_calm >= 0x30) && (new_vddig < VDDIG_MAX))
        {
            new_vddig = new_vddig + 1;
            new_bandgap_calm = new_bandgap_calm - 0x20;
        }
        else if ((new_bandgap_calm < 0x10) && (new_vddig > VDDIG_MIN))
        {
            new_vddig = new_vddig - 1;
            new_bandgap_calm = new_bandgap_calm + 0x20;
        }

        if ((new_vddig < VDDIG_MIN) || (new_vddig > VDDIG_MAX))
        {
            new_vddig = old_vddig;
            new_bandgap_calm = (uint32_t)old_bandgap_calm;
        }

        bk_printf("[load]bandgap_calm=0x%x->0x%x,vddig=%d->%d\r\n", bandgap_calm_in_efuse, new_bandgap_calm, old_vddig, new_vddig);

        analog2 &= ~(BANDGAP_CAL_MANUAL_MASK << BANDGAP_CAL_MANUAL_POSI);
        analog2 |= (new_bandgap_calm << BANDGAP_CAL_MANUAL_POSI);
        sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_SET_ANALOG2, &analog2);
        if (old_vddig != new_vddig)
        {
            sctrl_ctrl(CMD_SCTRL_SET_VDD_VALUE, (void *)&new_vddig);
        }
    }
}

void bk7011_cal_pll(void)
{
#if 1
    //    uint32_t loop = 0, val;

    //    do
    {
#if 0

#else

        /*reg0x10 enrfpll = 1*/
        BK7231N_TRX_RAM.REG0x10.bits.enrfpll = 1;
        BK7231N_TRX_RAM.REG0x10.bits.endpll = 1;
        CAL_WR_TRXREGS(0x10);

        /*reg0x00 spitrig = 0->1->0*/
/*        BK7231N_TRX_RAM.REG0x0->bits.spitrig = 0;
        CAL_WR_TRXREGS(0x0);
        BK7231N_TRX_RAM.REG0x0->bits.spitrig = 1;
        CAL_WR_TRXREGS(0x0);
        BK7231N_TRX_RAM.REG0x0->bits.spitrig = 0;
        CAL_WR_TRXREGS(0x0);*/

        /*reg0x05 spitrigger = 0->1->0*/
//        BK7231N_TRX_RAM.REG0x5->bits.spitrigger = 0;
//        BK7231N_TRX_RAM.REG0x5->bits.errdetspien = 0;
//        CAL_WR_TRXREGS(0x5);

//        BK7231N_TRX_RAM.REG0x5->bits.spitrigger = 1;
//        CAL_WR_TRXREGS(0x5);
//        BK7231N_TRX_RAM.REG0x5->bits.spitrigger = 0;
//        CAL_WR_TRXREGS(0x5);
//        BK7231N_TRX_RAM.REG0x5->bits.errdetspien = 1;
//        CAL_WR_TRXREGS(0x5);
        //BK7231N_TRX_RAM.REG0x3->bits.spi_trigger = 1;
        //CAL_WR_TRXREGS(0x3);
        //BK7231N_TRX_RAM.REG0x3->bits.spi_trigger = 0;
        //CAL_WR_TRXREGS(0x3);
        //BK7231N_TRX_RAM.REG0x3->bits.errdet_spien = 1;
        //CAL_WR_TRXREGS(0x3);
#endif

        cpu_delay(DELAY1US * 10);

    }

//    BK7231N_TRX_RAM[5] = (BK7231N_TRX_RAM[5] & (~(0x3))) | (0x1); //7231 for pll unlock detection,7231u for doubler

#endif
}

void bk7011_set_rfcali_mode(int mode, int save)
{
    if((mode != CALI_MODE_AUTO) && (mode != CALI_MODE_MANUAL))
    {
        CAL_FATAL("rfcali_mode 0/1\r\n");
        return;
    }
        
    gcali_context.cali_mode = mode;

    if (save)
    {
        save_info_item(RF_CFG_MODE_ITEM, (UINT8 *)&gcali_context.cali_mode, NULL, NULL);
    }

    os_printf("set rfcali_mode:%d\r\n", gcali_context.cali_mode);
}

UINT32 rwnx_cal_load_user_rfcali_mode(int *rfcali_mode)
{
    #define RF_MODE_GPIO_ID                 (GPIO1)
    #define RF_MODE_AUTOCALI_GPIO_LEVEL     (0)
    #define RF_MODE_MANULCALI_GPIO_LEVEL    (1)

    UINT32 gpio_level, second_func, ret, mode = 0, txpwr_state;
    GLOBAL_INT_DECLARATION();

    GLOBAL_INT_DISABLE();
    /* checking RX of UART2 */
    bk_gpio_config_input_pup(RF_MODE_GPIO_ID);

    txpwr_state = manual_cal_txpwr_tab_ready_in_flash();

    gpio_level = bk_gpio_input(RF_MODE_GPIO_ID);

    second_func = GFUNC_MODE_UART2;
    gpio_ctrl(CMD_GPIO_ENABLE_SECOND, &second_func);
    GLOBAL_INT_RESTORE();

    ret = 0;
    bk_printf("gpio_level=%d,txpwr_state=%d\n", gpio_level, txpwr_state);
    if(gpio_level == (UINT32)RF_MODE_AUTOCALI_GPIO_LEVEL)
    {
        if(txpwr_state != TXPWR_NONE_RD)
        {
            mode = CALI_MODE_MANUAL; // manual rf cali
        }
        else
        {
            mode = CALI_MODE_AUTO; // auto rf cali
        }
    }
    else if(gpio_level == (UINT32)RF_MODE_MANULCALI_GPIO_LEVEL)
    {
        mode = CALI_MODE_MANUAL; // manual rf cali
    }

    if(rfcali_mode)
    {
        ret = 1;
        *rfcali_mode = mode;
    }

    return ret;
}

void bk7011_get_rfcali_mode(void)
{
    int cali_mode, in_valid = 1;
    GLOBAL_INT_DECLARATION();

    GLOBAL_INT_DISABLE();
    if(get_info_item(RF_CFG_MODE_ITEM, (UINT8 *)&cali_mode, NULL, NULL))
    {
        if((cali_mode == CALI_MODE_AUTO) || (cali_mode == CALI_MODE_MANUAL))
        {
            gcali_context.cali_mode = cali_mode;
            os_printf("load flash rfcali mode:%d \r\n", cali_mode);
            in_valid = 0;
        }
        else
        {
            os_printf("rfcali_mode other:%d\r\n", cali_mode);
            in_valid = 1;
        }
    }

    if(in_valid == 1)
    {
        if(rwnx_cal_load_user_rfcali_mode)
        {
            UINT32 is_used = rwnx_cal_load_user_rfcali_mode(&cali_mode);
            if(is_used)
            {
                in_valid = 0;
                gcali_context.cali_mode = cali_mode;
                os_printf("user define rfcali mode:%d \r\n", cali_mode);
            }
        }
    }

    if(in_valid == 1)
    {
        if(manual_cal_txpwr_tab_ready_in_flash() != TXPWR_NONE_RD)
        {
            // found rfcali flag in flash, turn it to manual mode
            gcali_context.cali_mode = CALI_MODE_MANUAL;
            in_valid = 0;
        }
    }
    GLOBAL_INT_RESTORE();

    os_printf("get rfcali_mode:%d\r\n", gcali_context.cali_mode);
}

int bk7011_is_rfcali_mode_auto(void)
{
    if (CALI_MODE_UNKNOWN == gcali_context.cali_mode)
    {
        bk7011_get_rfcali_mode();
    }
    return (gcali_context.cali_mode == CALI_MODE_AUTO) ? 1 : 0;
}

void bk7011_set_rf_config_tssithred_b(int tssi_thred_b)
{
    if((tssi_thred_b < 0) || (tssi_thred_b > 0xffU))
    {
        CAL_FATAL("b tssi range:0-255, %d\r\n", tssi_thred_b);
        return;
    }
        
    gcali_context.gtx_tssi_thred_b = tssi_thred_b;

    save_info_item(RF_CFG_TSSI_ITEM, (UINT8 *)&gcali_context.gtx_tssi_thred_b, NULL, NULL);

    os_printf("set b_tssi_thred:0x%x\r\n", gcali_context.gtx_tssi_thred_b);
}

void bk7011_set_rf_config_tssithred_g(int tssi_thred_g)
{
    if((tssi_thred_g < 0) || (tssi_thred_g > 0xff))
    {
        os_printf("g tssi range:0-255, %d\r\n", tssi_thred_g);
        return;
    }

    gcali_context.gtx_tssi_thred_g = tssi_thred_g;

    save_info_item(RF_CFG_TSSI_ITEM, (UINT8 *)&gcali_context.gtx_tssi_thred_g, NULL, NULL);

    os_printf("set g_tssi_thred:%d\r\n", gcali_context.gtx_tssi_thred_g);
}

void bk7011_get_txpwr_config_reg(void)
{
    int tssi_thred;
    
#if CFG_ENABLE_ATE_FEATURE
    // load from flash first
    if(get_info_item(RF_CFG_TSSI_ITEM, (UINT8 *)&tssi_thred, NULL, NULL))
    {
        gcali_context.gtx_tssi_thred_g = tssi_thred;
        os_printf("load flash tssi_th:g-%d \r\n", gcali_context.gtx_tssi_thred_g);
    }
    // otherwise check if user set default value
    else 
#endif
    if(rwnx_cal_load_user_g_tssi_threshold)
    {
        UINT32 is_used = rwnx_cal_load_user_g_tssi_threshold(&tssi_thred);
        if(is_used)
        {
            gcali_context.gtx_tssi_thred_g = tssi_thred;
            os_printf("user define tssi_th:g-%d \r\n", gcali_context.gtx_tssi_thred_g);
        }
    }

#if CFG_ENABLE_ATE_FEATURE
    if(get_info_item(RF_CFG_TSSI_ITEM, (UINT8 *)&tssi_thred, NULL, NULL))
    {
        gcali_context.gtx_tssi_thred_b = tssi_thred;
        os_printf("load flash tssi_th:b-%d \r\n", gcali_context.gtx_tssi_thred_b);
    }
    else 
#endif
    if(rwnx_cal_load_user_b_tssi_threshold)
    {
        UINT32 is_used = rwnx_cal_load_user_b_tssi_threshold(&tssi_thred);
        if(is_used)
        {
            gcali_context.gtx_tssi_thred_b = tssi_thred;
            os_printf("user define tssi_th:b-%d \r\n", gcali_context.gtx_tssi_thred_b);
        }
    }
    
    os_printf("tssi_th:b-%d, g-%d\r\n", gcali_context.gtx_tssi_thred_b, gcali_context.gtx_tssi_thred_g);
}

void bk7011_tx_cal_en(void)
{
    BK7231N_RC_REG.REG0x0->bits.forceenable = 1;
    cpu_delay(1);
    BK7231N_RC_REG.REG0x19->bits.FCH0EN = 1;
    BK7231N_RC_REG.REG0x19->bits.FCH0SHDN = 1;
    BK7231N_RC_REG.REG0x19->bits.FCH0RXEN = 0;
    BK7231N_RC_REG.REG0x19->bits.FCH0TXEN = 1;
    BK7231N_RC_REG.REG0x1C->bits.FRXON = 0;
    BK7231N_RC_REG.REG0x1C->bits.FTXON = 1;
    //BK7231N_RC_REG.REG0x4D->bits.TXSINF = 0x1A;

    //BK7011ICU.REG0x3->value = 0x00010002;
    return;
}

static INT32 bk7011_get_tx_output_power(INT32 tx_power_cal_mode, INT32 gav_tssi_temp)
{
    INT32 tssioutpower = bk7011_cal_saradc_read(cali_saradc_desc);

    if(tx_power_cal_mode == TX_WANTED_POWER_CAL) //
    {
	#if DIFFERENCE_PIECES_CFG
		tssioutpower = tssioutpower - gcali_context.gtx_tssi_thred_g - gav_tssi_temp;
		//bk_printf("tssioutpower:%d\n",tssioutpower);
	#else
        tssioutpower = tssioutpower - TSSI_POUT_TH_G - gav_tssi_temp;
		
	#endif
    }
    else if(tx_power_cal_mode == TX_IQ_POWER_CAL) //
    {

        //tssioutpower = tssioutpower - TXIQ_IMB_TSSI_TH - gav_tssi_temp;
        //tssioutpower = tssioutpower - TXIQ_IMB_TSSI_TH;
        tssioutpower = abs(tssioutpower - TXIQ_IMB_TSSI_TH - gav_tssi_temp);

    }
    else
    {
        tssioutpower = tssioutpower - TXIQ_IMB_TSSI_TH_LOOPBACK - gav_tssi_temp;
    }


    tssioutpower = abs(tssioutpower);

    return tssioutpower;
}


static INT32 bk7011_set_tx_pa(INT32 val1, INT32 val2, INT32 val3, INT32 val4)
{
    BK7231N_TRX_RAM.REG0xC.bits.pactrl = val1;
    BK7231N_TRX_RAM.REG0xC.bits.padctrl = val2;
    //BK7231N_TRX_RAM.REG0xC->bits.dgainpga = val3;
    //BK7231N_TRX_RAM.REG0xB->bits.gctrlmod30 = val4;
    CAL_WR_TRXREGS(0xC);
    //CAL_WR_TRXREGS(0xB);

    return 0;
}

INT32 bk7011_cal_tx_output_power(INT32 tx_power_cal_mode)
{
    INT32 gold_index = 0;
    INT32 tssilow = 0;
    INT32 tssihigh = 0;
    INT32 index = 0;
    INT16 high, low;
 ///   INT32 cnt = 0;
    INT32 gav_tssi_temp = 0;

    bk7011_cal_saradc_runorstop(cali_saradc_desc, 1);

    //BK7011ADDAMAP.REG0x5->bits.vc40 = 0x1f;
    //BK7011ADDA.REG0x5->value = BK7011ADDAMAP.REG0x5->value;

    // BK7231N_RC_RAM[12]-16:20=7,    0x53479D40,   21 REG_0x52
    //BK7231N_RC_REG.REG0x52->bits.TXPREGAIN = 7;

    BK7231N_TRX_RAM.REG0xB.value = TRX_REG_0XB_VAL;
    CAL_WR_TRXREGS(0xB);

    BK7231N_RC_REG.REG0x4C->bits.ICONSTANT = BK_TX_DAC_SIGNED_MIN;
    BK7231N_RC_REG.REG0x4C->bits.QCONSTANT = BK_TX_DAC_SIGNED_MIN;
    BK7231N_TRX_RAM.REG0xC.value = TRX_REG_0XC_TXLO_LEAKAGE_VAL;
    BK7231N_RC_REG.REG0x4C->bits.TESTPATTERN = 1;
    BK7231N_RC_REG.REG0x43->bits.PRE_GAIN_2ND = 0x90;//wyg for iqpower


	
    if (tx_power_cal_mode == TX_WANTED_POWER_CAL)
    {
        BK7231N_TRX_RAM.REG0xD.value = TRX_REG_0XD_VAL;
        BK7231N_TRX_RAM.REG0xA.value = TRX_REG_0XA_VAL;//by yiming 20170315;
        BK7231N_TRX_RAM.REG0xB.value = TRX_REG_0XB_VAL;//;
        BK7231N_TRX_RAM.REG0xC.value = TRX_REG_0XC_VAL;//;
	    CAL_WR_TRXREGS(0xC);
        BK7231N_TRX_RAM.REG0x0.bits.TSSIsel = 1;
        BK7231N_TRX_RAM.REG0x0.bits.enDCcal = 0;
        BK7231N_TRX_RAM.REG0x0.bits.enIQcal = 0;
        BK7231N_TRX_RAM.REG0x0.bits.enPcal = 1;



		
    }
    else if(tx_power_cal_mode == TX_IQ_POWER_CAL)
    {
        BK7231N_TRX_RAM.REG0xD.value = TRX_REG_0XD_TX_IQ_VAL_GAIN_PHASE;
        BK7231N_TRX_RAM.REG0xA.value = TRX_REG_0XA_VAL;//0x036F0757;//by yiming 20170315;
        BK7231N_TRX_RAM.REG0xB.value = TRX_REG_0XB_VAL;//0x17248757;//;
        BK7231N_TRX_RAM.REG0xC.value =0x0678211A; //second iqpower
        
        //bk7011_set_tx_pa(gi_gain_tx_pa_dgainPA30, gi_gain_tx_pa_dgainbuf30, 0, 8);
        BK7231N_TRX_RAM.REG0x0.bits.TSSIsel = 0;
        BK7231N_TRX_RAM.REG0x0.bits.enDCcal = 0;
        BK7231N_TRX_RAM.REG0x0.bits.enIQcal = 1;
        BK7231N_TRX_RAM.REG0x0.bits.enPcal = 0;
        BK7231N_TRX_RAM.REG0x0.value =0x04238462; //second iqpower
        
	    CAL_WR_TRXREGS(0xA);
	    CAL_WR_TRXREGS(0xB);
	    CAL_WR_TRXREGS(0xC);
	    CAL_WR_TRXREGS(0xD);
        BK7231N_RC_REG.REG0x1A->value = BK7231N_TRX_RAM.REG0xD.value;
	    CAL_WR_TRXREGS(0x0);
        //BK7231N_TRX_RAM.REG0x0.bits.tssi_selrange = 0;           
    }
    else  //tx_power_cal_mode == TX_IQ_LOOPBACK_POWER_CAL
    {
        BK7231N_TRX_RAM.REG0xD.value = TRX_REG_0XD_TX_LOOPBACK_IQ_VAL;
        BK7231N_TRX_RAM.REG0xA.value = TRX_REG_0XA_VAL;//0x036F0757;//by yiming 20170315;
        BK7231N_TRX_RAM.REG0xB.value = TRX_REG_0XB_VAL;//0x17248757;//;
        BK7231N_TRX_RAM.REG0xC.value = TRX_REG_0XC_VAL;//0x05228765;//;
	    CAL_WR_TRXREGS(0xC);
        bk7011_set_tx_pa(gi_gain_tx_loopback_pa_dgainPA30, gi_gain_tx_loopback_pa_dgainbuf30, 0, 8);
        BK7231N_TRX_RAM.REG0x0.bits.TSSIsel = 0;
        BK7231N_TRX_RAM.REG0x0.bits.enDCcal = 0;
        BK7231N_TRX_RAM.REG0x0.bits.enIQcal = 1;
        BK7231N_TRX_RAM.REG0x0.bits.enPcal = 0;
        //BK7231N_TRX_RAM.REG0x0.bits.tssi_selrange = 0;        
    }
    
#if DIFFERENCE_PIECES_CFG
	if (tx_power_cal_mode == TX_WANTED_POWER_CAL)
    {
		//BK7231N_TRX_RAM.REG0x0.bits.tssi_statectrl = 0;
    }
	else{
    	//BK7231N_TRX_RAM.REG0x0.bits.tssi_statectrl = 1;
    }
#else
	//BK7231N_TRX_RAM.REG0x0.bits.tssi_statectrl = 1;
#endif

    BK7231N_TRX_RAM.REG0xD.bits.entssi = 1;
    BK7231N_TRX_RAM.REG0xD.bits.entssiadc = 1;//20170503 enable TSSI
//    BK7231N_TRX_RAM.REG0x0.bits.tssi_selrange = 1;
//    BK7231N_TRX_RAM.REG0x0.bits.enPcaliGm = 0;
    BK7231N_TRX_RAM.REG0xF.bits.tssi_cal_en = 1;

    BK7231N_TRX_RAM.REG0x12.bits.buftstselection = 3;
    BK7231N_TRX_RAM.REG0x12.bits.entstbufldo = 1;

	
    CAL_WR_TRXREGS(0x12);
    
    CAL_WR_TRXREGS(0x0);
    CAL_WR_TRXREGS(0xA);
    CAL_WR_TRXREGS(0xD);
    BK7231N_RC_REG.REG0x1A->value = BK7231N_TRX_RAM.REG0xD.value;
    CAL_WR_TRXREGS(0xC);
    CAL_WR_TRXREGS(0xF);
    //BK7231N_TRX_RAM.REG0xB.bits.dcorMod30 = 0;
    CAL_WR_TRXREGS(0xB);
    cal_delay(150);//for reg C,D,F write spi=80/16=5M,total time is 21.6us
    cal_delay(5 * gcali_context.gst_sar_adc);

    delay05us(10);
    gav_tssi_temp = bk7011_cal_saradc_read(cali_saradc_desc);//Get small power tssi of each sample to remove tssi dc
	
    if (tx_power_cal_mode == TX_WANTED_POWER_CAL)
    {
        gcali_result.gav_tssi = gav_tssi_temp;
		///bk_printf("gav_tssi:%d\n",gav_tssi);
    }

    if (tx_power_cal_mode == TX_WANTED_POWER_CAL)
    {
	#if DIFFERENCE_PIECES_CFG
	/*
        BK7231N_RC_REG.REG0x4C->bits.ICONSTANT = 0x280;//gconst_pout;
        BK7231N_RC_REG.REG0x4C->bits.QCONSTANT = 0x280;//gconst_pout;
        BK7231N_TRX_RAM.REG0xC.value = TRX_REG_0XC_VAL;//;
        
        BK7231N_TRX_RAM.REG0xC.bits.pactrl = 2;//;
        BK7231N_TRX_RAM.REG0xC.bits.padctrl = 2;//;
        
        CAL_WR_TRXREGS(0xC);
        BK7231N_TRX_RAM.REG0x0.bits.TSSIsel = 1;
        BK7231N_TRX_RAM.REG0x0.bits.enDCcal = 0;
        BK7231N_TRX_RAM.REG0x0.bits.enIQcal = 0;
        BK7231N_TRX_RAM.REG0x0.bits.enPcal = 1;
        BK7231N_TRX_RAM.REG0x0.bits.tssi_selrange = 0;  
      */
      	BK7231N_RC_REG.REG0x4C->bits.ICONSTANT = BK_TX_DAC_SIGNED_MIN + BK_TX_DAC_COEF * 0x80;///0x200;
    	BK7231N_RC_REG.REG0x4C->bits.QCONSTANT = BK_TX_DAC_SIGNED_MIN + BK_TX_DAC_COEF * 0x80;///0x200;
    	BK7231N_RC_REG.REG0x0->bits.ch0en = 1;  ///add
    	
        BK7231N_TRX_RAM.REG0xA.value = TRX_REG_0XA_VAL;///TRX_REG_0XA_VAL;//by yiming 20170315;
        BK7231N_TRX_RAM.REG0xB.value = TRX_REG_0XB_VAL;///TRX_REG_0XB_VAL;//;
        BK7231N_TRX_RAM.REG0xC.value = TRX_REG_0XC_VAL;///TRX_REG_0XC_VAL;//;

		CAL_WR_TRXREGS(0xC);
		CAL_WR_TRXREGS(0xB);
        //BK7231N_RC_REG.REG0x52->bits.TXPREGAIN = 0x07;  ///ADD
        
        BK7231N_TRX_RAM.REG0xD.bits.entxsw = 1;
        BK7231N_TRX_RAM.REG0xD.bits.enrxsw = 0;
        
	    //CAL_WR_TRXREGS(0xD);
		
        BK7231N_TRX_RAM.REG0x0.bits.TSSIsel = 1;
        BK7231N_TRX_RAM.REG0x0.bits.enDCcal = 0;
        BK7231N_TRX_RAM.REG0x0.bits.enIQcal = 0;
        BK7231N_TRX_RAM.REG0x0.bits.enPcal = 1;
        //BK7231N_TRX_RAM.REG0x0->bits.tssi_selrange = 0; 
		BK7231N_TRX_RAM.REG0x0.bits.PcalATT = 2;
	#else
        BK7231N_RC_REG.REG0x4C->bits.ICONSTANT =BK_TX_DAC_SIGNED_MIN-0x200 ;//wyg
        BK7231N_RC_REG.REG0x4C->bits.QCONSTANT = BK_TX_DAC_SIGNED_MIN-0x200 ;//wyg
        BK7231N_TRX_RAM.REG0xC.value = TRX_REG_0XC_VAL;//;
        CAL_WR_TRXREGS(0xC);
        BK7231N_TRX_RAM.REG0x0.bits.TSSIsel = 1;
        BK7231N_TRX_RAM.REG0x0.bits.enDCcal = 0;
        BK7231N_TRX_RAM.REG0x0.bits.enIQcal = 0;
        BK7231N_TRX_RAM.REG0x0.bits.enPcal = 1;
        //BK7231N_TRX_RAM.REG0x0.bits.tssi_selrange = 0;        
	#endif
    }
    else if(tx_power_cal_mode == TX_IQ_POWER_CAL)
    {
        //BK7231N_RC_REG.REG0x4C->bits.ICONSTANT = BK_TX_DAC_SIGNED_MIN + BK_TX_DAC_COEF * 0x30;
        //BK7231N_RC_REG.REG0x4C->bits.QCONSTANT = BK_TX_DAC_SIGNED_MIN + BK_TX_DAC_COEF * 0x30;
        //BK7231N_TRX_RAM.REG0xC.value = TRX_REG_0XC_VAL;//0x05228245;//;
        BK7231N_RC_REG.REG0x4C->bits.ICONSTANT = BK_TX_DAC_SIGNED_MIN ;
        BK7231N_RC_REG.REG0x4C->bits.QCONSTANT = BK_TX_DAC_SIGNED_MIN ;

        CAL_WR_TRXREGS(0xC);
        //bk7011_set_tx_pa(gi_gain_tx_pa_dgainPA30, gi_gain_tx_pa_dgainbuf30, 0, 8);
        BK7231N_TRX_RAM.REG0x0.bits.TSSIsel = 0;
        BK7231N_TRX_RAM.REG0x0.bits.enDCcal = 0;
        BK7231N_TRX_RAM.REG0x0.bits.enIQcal = 1;
        BK7231N_TRX_RAM.REG0x0.bits.enPcal = 0;
        //BK7231N_TRX_RAM.REG0x0.bits.tssi_selrange = 0;          
    }
    else  //tx_power_cal_mode == TX_IQ_LOOPBACK_POWER_CAL
    {
        BK7231N_RC_REG.REG0x4C->bits.ICONSTANT = BK_TX_DAC_SIGNED_MIN + BK_TX_DAC_COEF * 0x60;//by yiming 20170315;  250
        BK7231N_RC_REG.REG0x4C->bits.QCONSTANT = BK_TX_DAC_SIGNED_MIN + BK_TX_DAC_COEF * 0x60;
        BK7231N_TRX_RAM.REG0xC.value = TRX_REG_0XC_VAL;//0x05228245;//;
        CAL_WR_TRXREGS(0xC);
        bk7011_set_tx_pa(gi_gain_tx_loopback_pa_dgainPA30, gi_gain_tx_loopback_pa_dgainbuf30, 0, 8);
        BK7231N_TRX_RAM.REG0x0.bits.TSSIsel = 0;
        BK7231N_TRX_RAM.REG0x0.bits.enDCcal = 0;
        BK7231N_TRX_RAM.REG0x0.bits.enIQcal = 1;
        BK7231N_TRX_RAM.REG0x0.bits.enPcal = 0;
        //BK7231N_TRX_RAM.REG0x0.bits.tssi_selrange = 0;
    }

//    BK7231N_TRX_RAM.REG0x0->bits.enPcaliGm = 0;
    BK7231N_TRX_RAM.REG0xF.bits.tssi_cal_en = 1;
    //BK7231N_TRX_RAM.REG0xC->bits.dcorPA30 = 8;

    CAL_WR_TRXREGS(0x0);
    CAL_WR_TRXREGS(0xA);
    CAL_WR_TRXREGS(0xD);
    BK7231N_RC_REG.REG0x1A->value = BK7231N_TRX_RAM.REG0xD.value;
    CAL_WR_TRXREGS(0xC);
    CAL_WR_TRXREGS(0xF);
    BK7231N_RC_REG.REG0x4C->bits.TESTPATTERN = 1;

#if DIFFERENCE_PIECES_CFG
 	if(tx_power_cal_mode == TX_WANTED_POWER_CAL)
    {
        index = 0;
        tssilow = 0;
        tssihigh = 0;
        cnt = 0;

        //BK7231N_TRX_RAM.REG0xB.bits.dcorMod30 = cnt;
        CAL_WR_TRXREGS(0xB);
        tssilow = bk7011_get_tx_output_power(tx_power_cal_mode, gav_tssi_temp);
        index = cnt;
        
        CAL_PRT("cnt:%d, index:%d, tssilow:%d, tssihigh:%d\r\n",
            cnt, index, tssilow, tssihigh);
        
        cnt ++;
        
        while(cnt <= 15)
        {
            //BK7231N_TRX_RAM.REG0xB.bits.dcorMod30 = cnt;
            CAL_WR_TRXREGS(0xB);
            tssihigh = bk7011_get_tx_output_power(tx_power_cal_mode, gav_tssi_temp);

            if(tssihigh < tssilow)
            {
                index = cnt;
                tssilow = tssihigh;
            }
            
            CAL_PRT("cnt:%d, index:%d, tssilow:%d, tssihigh:%d\r\n",
                cnt, index, tssilow, tssihigh);

            cnt ++;
        }
    }
	else  
#endif
    if(tx_power_cal_mode == TX_IQ_POWER_CAL)
    {
        low = 0x800;//170703,for TZH limited from 2 to 6
        high = 0x0;

   // BK7231N_TRX_RAM.REG0xC.bits.padctrl=low;
        CAL_WR_TRXREGS(0xC);

        //BK7231N_TRX_RAM.REG0xB.bits.dcorMod30 = low;
        CAL_WR_TRXREGS(0xB);
        BK7231N_RC_REG.REG0x4C->bits.ICONSTANT = low;
        BK7231N_RC_REG.REG0x4C->bits.QCONSTANT = low;
        cal_delay(CAL_TX_NUM);//first sar dac delay needs double time
        tssilow = bk7011_get_tx_output_power(tx_power_cal_mode, gav_tssi_temp);

        CAL_WR_TRXREGS(0xC);

        BK7231N_RC_REG.REG0x4C->bits.ICONSTANT = high;
        BK7231N_RC_REG.REG0x4C->bits.QCONSTANT = high;

        cal_delay(CAL_TX_NUM);//first sar dac delay needs double time
	
        tssihigh = bk7011_get_tx_output_power(tx_power_cal_mode, gav_tssi_temp);

        do
        {
            if(tssilow < tssihigh)
            {
                index = low;
                high = ((low + high) >> 1) + (((low + high) & 0x01) ? 1 : 0);
				
        BK7231N_RC_REG.REG0x4C->bits.ICONSTANT = high;
        BK7231N_RC_REG.REG0x4C->bits.QCONSTANT = high;
				
                //BK7231N_TRX_RAM.REG0xB.bits.dcorMod30 = high;
                CAL_WR_TRXREGS(0xB);
                tssihigh = bk7011_get_tx_output_power(tx_power_cal_mode, gav_tssi_temp);
            }
            else
            {
                index = high;
                low = ((low + high) >> 1) + (((low + high) & 0x01) ? 1 : 0);

        BK7231N_RC_REG.REG0x4C->bits.ICONSTANT = low;
        BK7231N_RC_REG.REG0x4C->bits.QCONSTANT = low;
				
                //BK7231N_TRX_RAM.REG0xB.bits.dcorMod30 = low;
                CAL_WR_TRXREGS(0xB);
                tssilow = bk7011_get_tx_output_power(tx_power_cal_mode, gav_tssi_temp);
            }
        }
        while((low - high) > 1);
        index = ((tssilow < tssihigh) ? low : high);
        gcali_result.const_iqcal_p=0x800-index;
    }
    
    if (tx_power_cal_mode == TX_WANTED_POWER_CAL)
    {
     //   gtx_dcorMod = index;
	//	CAL_PRT("gtx_dcorMod=%d\n",gtx_dcorMod);
    }
	else if(tx_power_cal_mode == TX_IQ_POWER_CAL)
	{
    //   CAL_PRT("bk7011 TX IQ Cal. Output Power: \r\ntx_dcorMod= %x\n, ", index);
    //	gtx_dcorMod_temp = index;
	}
	else 
	{
    //    CAL_PRT("bk7011 TX LOOPBACK IQ Cal. Output Power: \r\ntx_dcorMod= %d, ", index);
    	gtx_dcorMod_temp_loopback = index;
	}		

   // CAL_PRT("gtx_dcorMod over: 0x%x\r\n", gtx_dcorMod);
    //CAL_PRT("cnt:%d, index:%d, tssilow:0x%x-%d, tssihigh:0x%x-%d\r\n",
    //        cnt++, index, tssilow, low, tssihigh, high);

    //BK7231N_TRX_RAM.REG0xB.bits.dcorMod30 = index;

    //BK7231N_TRX_RAM.REG0xC.bits.padctrl= index;
   
    CAL_WR_TRXREGS(0xC);
    gold_index = index << 8;
    cal_delay(6);

    /* restore changed registers */
    BK7231N_TRX_RAM.REG0x0.bits.TSSIsel = 0;
    BK7231N_TRX_RAM.REG0x0.bits.enIQcal = 1;
    BK7231N_TRX_RAM.REG0x0.bits.enPcal = 0;
    BK7231N_TRX_RAM.REG0xC.value = TRX_REG_0XC_VAL;
    BK7231N_TRX_RAM.REG0xD.value = TRX_REG_0XD_VAL;
    BK7231N_TRX_RAM.REG0xF.bits.tssi_cal_en = 0;
    BK7231N_RC_REG.REG0x43->bits.PRE_GAIN_2ND = 0x200;//
    CAL_WR_TRXREGS(0x0);
    CAL_WR_TRXREGS(0xC);
    CAL_WR_TRXREGS(0xD);
    BK7231N_RC_REG.REG0x1A->value = BK7231N_TRX_RAM.REG0xD.value;
    CAL_WR_TRXREGS(0xF);

    bk7011_cal_saradc_runorstop(cali_saradc_desc, 0);

    return (gold_index);
}

static INT32 bk7011_get_tx_tssi(INT32 tssi_thred, INT32 tssi_offset)
{
    INT32 tssioutpower = bk7011_cal_saradc_read(cali_saradc_desc);

    tssioutpower = tssioutpower - tssi_thred - tssi_offset;

    tssioutpower = abs(tssioutpower);

    return tssioutpower;
}

#define AUTO_CAL_PTR        null_prf
#define AUTO_CAL_NPTR       null_prf
static void bk7011_do_atuo_tx_cal_print(const char *fmt, ...)
{
    UINT32 is_on = 1;
    if (rwnx_cal_is_auto_rfcali_printf_on)
    {
        is_on = rwnx_cal_is_auto_rfcali_printf_on();
    }
    if(is_on)
    {
        va_list ap;
        int length;
        char string[128];
    
        va_start(ap, fmt);
        length = vsnprintf(string, sizeof(string) - 1, fmt, ap);
        if (length > sizeof(string) - 1)
        {
            length = sizeof(string) - 1;
        }
        string[length] = '\0';
        va_end(ap);
        bk_printf("%s", string);
    }
}

static UINT32 bk7011_do_atuo_tx_cal(UINT32 channel, UINT32 rate, INT32 tssi_thred, INT32 tssi_offset)
{
    INT32 tssilow = 0;
    INT32 tssihigh = 0;
    INT32 index = 0;
///    UINT32 pwr_idx;
    INT16 pwr_idx_high, pwr_idx_low;
    const PWR_REGS *pcfg;

    if ((channel < 1) || (channel > 14))
    {
        AUTO_CAL_NPTR("wrong channel:%d\r\n", channel);
        return 16;
    }
    else
    {
        AUTO_CAL_NPTR("\r\n ******** do chan:%d ********\r\n", channel);
        if (channel != 14)
        {
            BK7231N_TRX_RAM.REG0x5.bits.chspi = 12 + 5 * (channel - 1);
        }
        else
        {
            BK7231N_TRX_RAM.REG0x5.bits.chspi = 84;
        }
    }

    // do for channel
    CAL_WR_TRXREGS(0x5);
    delay100us(1);

    pwr_idx_low = 0x000;

    if (EVM_DEFUALT_B_RATE == rate)
    {
        pcfg = cfg_tab_b;
        pwr_idx_high = TCP_PAMAP_TAB_B_LEN - 1;
    }
    else
    {
        pcfg = cfg_tab_g;
        pwr_idx_high = TCP_PAMAP_TAB_G_LEN - 1;
    }

    BK7231N_RC_REG.REG0x43->bits.PRE_GAIN_2ND = (pcfg[pwr_idx_low].pregain & 0x3FF);
    cal_delay(CAL_TX_NUM);//first sar dac delay needs double time
    tssilow = bk7011_get_tx_tssi(tssi_thred, tssi_offset);
    CAL_WR_TRXREGS(0xC);

    BK7231N_RC_REG.REG0x43->bits.PRE_GAIN_2ND = (pcfg[pwr_idx_high].pregain & 0x3FF);

    cal_delay(CAL_TX_NUM);//first sar dac delay needs double time

    tssihigh = bk7011_get_tx_tssi(tssi_thred, tssi_offset);
    AUTO_CAL_PTR("pwr_gain_low[%d]=0x%x,pwr_gain_high[%d]=0x%x,tssilow=0x%x,tssihigh=0x%x\n", pwr_idx_low, pcfg[pwr_idx_low].pregain, pwr_idx_high, pcfg[pwr_idx_high].pregain, tssilow, tssihigh);        
    do
    {
        if (tssilow < tssihigh)
        {
            if (tssilow <= 3)
            {
                break;
            }
            index = ((pwr_idx_high-pwr_idx_low) >> 3);
            if (0 == index)
            {
                index = 1;
            }
            pwr_idx_high -= index;

            BK7231N_RC_REG.REG0x43->bits.PRE_GAIN_2ND = (pcfg[pwr_idx_high].pregain & 0x3FF);

            tssihigh = bk7011_get_tx_tssi(tssi_thred, tssi_offset);
        }
        else
        {
            if (tssihigh <= 3)
            {
                break;
            }
            index = ((pwr_idx_high-pwr_idx_low) >> 3);
            if (0 == index)
            {
                index = 1;
            }
            pwr_idx_low += index;

            BK7231N_RC_REG.REG0x43->bits.PRE_GAIN_2ND = (pcfg[pwr_idx_low].pregain & 0x3FF);

            tssilow = bk7011_get_tx_tssi(tssi_thred, tssi_offset);
        }
        AUTO_CAL_PTR("pwr_gain_low[%d]=0x%x,pwr_gain_high[%d]=0x%x,tssilow=0x%x,tssihigh=0x%x\n", pwr_idx_low, pcfg[pwr_idx_low].pregain, pwr_idx_high, pcfg[pwr_idx_high].pregain, tssilow, tssihigh);        
    }
    while ((pwr_idx_high-pwr_idx_low) > 1);
    index = ((tssilow < tssihigh) ? pwr_idx_low : pwr_idx_high);
    bk7011_do_atuo_tx_cal_print("rate=%d,channel=%d,tssi_thred=0x%x,tssi_offset=0x%x,index=%d\n", rate, channel, tssi_thred, tssi_offset, index);		   

    return (index);
}

INT32 bk7011_cal_auto_tx_power(void)
{
    extern UINT32 g_dif_g_n40;
    extern void manual_cal_11b_2_ble(void);
    UINT32 channel, rate, pwr_idx, tssi_thred;
    INT32 tssi_offset;
    INT32 tssi_index;
    UINT32 tssi_sum = 0;

    bk7011_cal_saradc_runorstop(cali_saradc_desc, 1);

    BK7231N_RC_REG.REG0x4C->bits.TESTPATTERN = 1;
    BK7231N_RC_REG.REG0x1A->value = BK7231N_TRX_RAM.REG0xD.value;

    // tx chanin gain setting
    BK7231N_TRX_RAM.REG0xD.value = TRX_REG_0XD_VAL;
    BK7231N_TRX_RAM.REG0xA.value = 0x8063DDFF;//by yg 20200915;
    BK7231N_TRX_RAM.REG0xB.value = TRX_REG_0XB_VAL;//;
    BK7231N_TRX_RAM.REG0xC.value = TRX_REG_0XC_VAL;//;
    BK7231N_TRX_RAM.REG0x0.bits.TSSIsel = 1;
    BK7231N_TRX_RAM.REG0x0.bits.enDCcal = 0;
    BK7231N_TRX_RAM.REG0x0.bits.enIQcal = 0;
    BK7231N_TRX_RAM.REG0x0.bits.enPcal = 1;
    BK7231N_TRX_RAM.REG0x0.value = 0x528826F;
    
    BK7231N_TRX_RAM.REG0xD.bits.entssi = 1;
    BK7231N_TRX_RAM.REG0xD.bits.entssiadc = 1;//20170503 enable TSSI
    BK7231N_TRX_RAM.REG0xF.bits.tssi_cal_en = 1;
    BK7231N_TRX_RAM.REG0x12.bits.buftstselection = 3;
    BK7231N_TRX_RAM.REG0x12.bits.entstbufldo = 1;

    CAL_WR_TRXREGS(0x0);
    CAL_WR_TRXREGS(0xA);
    CAL_WR_TRXREGS(0xB);
    CAL_WR_TRXREGS(0xC);
    CAL_WR_TRXREGS(0xD);
    CAL_WR_TRXREGS(0xF);
    CAL_WR_TRXREGS(0x12);
    cal_delay(150);//for reg C,D,F write spi=80/16=5M,total time is 21.6us
    cal_delay(5 * gcali_context.gst_sar_adc);

    // Set tssi offset
    BK7231N_RC_REG.REG0x4C->bits.ICONSTANT = BK_TX_DAC_SIGNED_MIN ;///0x200;
    BK7231N_RC_REG.REG0x4C->bits.QCONSTANT = BK_TX_DAC_SIGNED_MIN ;///0x200;
    BK7231N_RC_REG.REG0x43->bits.PRE_GAIN_2ND = 0x0;//
    for (tssi_index = 0; tssi_index < 20; tssi_index++)
    {
        tssi_sum += bk7011_cal_saradc_read(cali_saradc_desc);//Get small power tssi of each sample to remove tssi dc
    }
    tssi_offset = tssi_sum / tssi_index;

    // tx sinewave setting
    rate = EVM_DEFUALT_RATE;
    tssi_thred = gcali_context.gtx_tssi_thred_g;
    BK7231N_RC_REG.REG0x4C->bits.ICONSTANT = 0x627;
    BK7231N_RC_REG.REG0x4C->bits.QCONSTANT = 0x627;

    /* add 4db power gain for 11g */
    channel = 1;
    pwr_idx = bk7011_do_atuo_tx_cal(channel, rate, tssi_thred, tssi_offset);
    pwr_idx += POWER_TABLE_11G_1DB_STEP * POWER_CALI_11G_SHIFT;
    manual_cal_save_txpwr(rate, channel, pwr_idx);
    pwr_idx = (pwr_idx > g_dif_g_n40) ? (pwr_idx - g_dif_g_n40) : 0;
    manual_cal_save_txpwr(135, 3, pwr_idx);

    channel = 7;
    pwr_idx = bk7011_do_atuo_tx_cal(channel, rate, tssi_thred, tssi_offset);
    pwr_idx += POWER_TABLE_11G_1DB_STEP * POWER_CALI_11G_SHIFT;
    manual_cal_save_txpwr(rate, channel, pwr_idx);
    pwr_idx = (pwr_idx > g_dif_g_n40) ? (pwr_idx - g_dif_g_n40) : 0;
    manual_cal_save_txpwr(135, 7, pwr_idx);

    channel = 13;
    pwr_idx = bk7011_do_atuo_tx_cal(channel, rate, tssi_thred, tssi_offset);
    pwr_idx += POWER_TABLE_11G_1DB_STEP * POWER_CALI_11G_SHIFT;
    manual_cal_save_txpwr(rate, channel, pwr_idx);
    pwr_idx = (pwr_idx > g_dif_g_n40) ? (pwr_idx - g_dif_g_n40) : 0;
    manual_cal_save_txpwr(135, 11, pwr_idx);

    rate = EVM_DEFUALT_B_RATE;
    tssi_thred = gcali_context.gtx_tssi_thred_b;
    BK7231N_RC_REG.REG0x4C->bits.ICONSTANT = 0x380;
    BK7231N_RC_REG.REG0x4C->bits.QCONSTANT = 0x380;

    /* add 2db power gain for 11b */
    channel = 1;
    pwr_idx = bk7011_do_atuo_tx_cal(channel, rate, tssi_thred, tssi_offset);
    manual_cal_save_txpwr(rate, channel, pwr_idx + POWER_TABLE_11B_1DB_STEP * POWER_CALI_11B_SHIFT);

    channel = 7;
    pwr_idx = bk7011_do_atuo_tx_cal(channel, rate, tssi_thred, tssi_offset);
    manual_cal_save_txpwr(rate, channel, pwr_idx + POWER_TABLE_11B_1DB_STEP * POWER_CALI_11B_SHIFT);
    
    channel = 13;
    pwr_idx = bk7011_do_atuo_tx_cal(channel, rate, tssi_thred, tssi_offset);
    manual_cal_save_txpwr(rate, channel, pwr_idx + POWER_TABLE_11B_1DB_STEP * POWER_CALI_11B_SHIFT);

    manual_cal_11b_2_ble();

    manual_cal_fitting_txpwr_tab();

    // exit, recover setting
    BK7231N_TRX_RAM.REG0x0.bits.TSSIsel = 0;
    BK7231N_TRX_RAM.REG0x0.bits.enIQcal = 1;
    BK7231N_TRX_RAM.REG0x0.bits.enPcal = 0;
    BK7231N_TRX_RAM.REG0xC.value = TRX_REG_0XC_VAL;
    BK7231N_TRX_RAM.REG0xD.value = TRX_REG_0XD_VAL;
    BK7231N_TRX_RAM.REG0xF.bits.tssi_cal_en = 0;
    CAL_WR_TRXREGS(0x0);
    CAL_WR_TRXREGS(0xC);
    CAL_WR_TRXREGS(0xD);
    CAL_WR_TRXREGS(0xF);

    BK7231N_RC_REG.REG0x1A->value = BK7231N_TRX_RAM.REG0xD.value;
    BK7231N_RC_REG.REG0x43->bits.PRE_GAIN_2ND = 0x200;
    BK7231N_RC_REG.REG0x4C->bits.TESTPATTERN = 0;

    bk7011_cal_saradc_runorstop(cali_saradc_desc, 0);

    return 0;
}

void bk7011_set_const_iqcal_p(INT32 const_iqcal_p)
{
    gcali_result.const_iqcal_p = const_iqcal_p;
}

void bk7011_micopwr_config_tssi_read_prepare(void)
{
    //BK7231N_TRX_RAM.REG0x0.bits.TSSIsel = 1;
    BK7231N_TRX_RAM.REG0x0.bits.enDCcal = 0;
    BK7231N_TRX_RAM.REG0x0.bits.enIQcal = 0;
    BK7231N_TRX_RAM.REG0x0.bits.enPcal = 1;
    BK7231N_TRX_RAM.REG0xD.bits.entssi = 1;
    //BK7231N_TRX_RAM.REG0xF.bits.tssi_cal_en = 1;
    
    BK7231N_TRX_RAM.REG0x12.bits.buftstselection = 3;
    BK7231N_TRX_RAM.REG0x12.bits.entstbufldo = 1;

    CAL_WR_TRXREGS(0x0);
    CAL_WR_TRXREGS(0xD);
    BK7231N_RC_REG.REG0x1A->value = BK7231N_TRX_RAM.REG0xD.value;
    //CAL_WR_TRXREGS(0xF);
    CAL_WR_TRXREGS(0x12);
}

static INT32 bk7011_update_tx_power(void)
{
    bk7011_set_tx_pa(gi_dc_tx_pa_dgainPA30, gi_dc_tx_pa_dgainbuf30, 0, 8);
    return 0;
}

static INT32 bk7011_update_tx_loopback_power(void)
{
    bk7011_set_tx_pa(gi_dc_tx_loopback_pa_dgainPA30, gi_dc_tx_loopback_pa_dgainbuf30, 0, 8);
    return 0;
}

static INT32 bk7011_get_tx_dc(void)
{
    return bk7011_cal_saradc_read(cali_saradc_desc);
}

INT32 bk7011_cal_tx_dc_new(INT32 tx_dc_cal_mode)
{
    INT32 detect_dc_low = 0;
    INT32 detect_dc_high = 0;
    INT16 high, low;
    INT32 gold_index = 0;
    INT32 i_index, q_index;
    INT32 srchcnt = 0;
    INT16 search_thrd = 64 * BK_TX_DAC_COEF;//128;//DC search range search_thrd=512 gtx_dc_n=3,    search_thrd=256 gtx_dc_n=2,    search_thrd=128 gtx_dc_n=1
    /*step 4*/

    bk7011_cal_saradc_runorstop(cali_saradc_desc, 1);

    BK7231N_TRX_RAM.REG0x7.bits.txif_2rd = 0x0;
    BK7231N_TRX_RAM.REG0x7.bits.abws_en = 0x0;
    BK7231N_TRX_RAM.REG0xD.bits.entssi = 1;
    BK7231N_TRX_RAM.REG0x12.bits.buftstselection = 3;
    BK7231N_TRX_RAM.REG0x12.bits.entstbufldo = 1;
    BK7231N_RC_REG.REG0x43->bits.PRE_GAIN_2ND = 0x200;//
    CAL_WR_TRXREGS(0x7);
    CAL_WR_TRXREGS(0xD);
    BK7231N_RC_REG.REG0x1A->value = BK7231N_TRX_RAM.REG0xD.value;
    CAL_WR_TRXREGS(0x12);
    if(tx_dc_cal_mode == TX_DC_CAL)
    {
        BK7231N_TRX_RAM.REG0xD.value = TRX_REG_0XD_TX_IQ_VAL;  //BK7011TRXREG0xD;//
        CAL_WR_TRXREGS(0xD);
        BK7231N_RC_REG.REG0x1A->value = BK7231N_TRX_RAM.REG0xD.value;
        bk7011_update_tx_power();
        //BK7231N_TRX_RAM.REG0xB.bits.gctrlmod30 = (TRX_REG_0XB_VAL >> 28) & 0x0F;
        //CAL_WR_TRXREGS(0xB);
    }
    else if (tx_dc_cal_mode == TX_DC_CAL_IQ)
    {
        BK7231N_TRX_RAM.REG0xD.value = TRX_REG_0XD_TX_IQ_VAL;  //BK7011TRXREG0xD;//
        CAL_WR_TRXREGS(0xD);
        BK7231N_RC_REG.REG0x1A->value = BK7231N_TRX_RAM.REG0xD.value;
        bk7011_update_tx_power();

    }
    else if (tx_dc_cal_mode == TX_DC_LOOPBACK_CAL_IQ)
    {
        BK7231N_TRX_RAM.REG0xD.value = TRX_REG_0XD_TX_LOOPBACK_IQ_VAL;
        CAL_WR_TRXREGS(0xD);
        BK7231N_RC_REG.REG0x1A->value = BK7231N_TRX_RAM.REG0xD.value;
        bk7011_update_tx_loopback_power();
    }
    
    if(tx_dc_cal_mode == TX_DC_CAL)
    {
        CAL_PRT("\r\nbk7011_cal_tx_dc: \r\n");
    }
    if(tx_dc_cal_mode == TX_DC_CAL_IQ)
    {
        CAL_PRT("\r\nbk7011_cal_tx_dc_iq: \r\n");
    }
    else if (tx_dc_cal_mode == TX_DC_LOOPBACK_CAL_IQ)
    {
        CAL_PRT("\r\nbk7011_cal_tx_loopback_dc_iq: \r\n");
    }
    CAL_PRT("pactrl  = %d\r\n", BK7231N_TRX_RAM.REG0xC.bits.pactrl );
    CAL_PRT("padctrl =%d\r\n",  BK7231N_TRX_RAM.REG0xC.bits.padctrl);
    //[-512 511]---->[0,BK_TX_DAC_UNSIGNED_MASK];

    //20170330
    BK7231N_RC_REG.REG0x4F->bits.TXIDCCOMP = 0x800;//wyg
    BK7231N_RC_REG.REG0x4F->bits.TXQDCCOMP = 0x800;//wyg
    BK7231N_RC_REG.REG0x50->bits.TXIGAINCOMP=0xfff;//wyg
    BK7231N_RC_REG.REG0x50->bits.TXQGAINCOMP=0xfff;//wyg
    BK7231N_RC_REG.REG0x51->bits.TXPHASECOMP =  0x800;//wyg
    BK7231N_RC_REG.REG0x51->bits.TXTY2 = 0x800;//wyg

    BK7231N_RC_REG.REG0x4C->bits.ICONSTANT = UNSIGNEDOFFSET10  - 1;
    BK7231N_RC_REG.REG0x4C->bits.QCONSTANT = UNSIGNEDOFFSET10 - 1 ;
    BK7231N_RC_REG.REG0x4C->bits.TESTPATTERN = 1;

    BK7231N_TRX_RAM.REG0x0.bits.enDCcal = 1;
    //BK7231N_TRX_RAM.REG0x0->bits.tssi_selrange = 1;//inverse 20141014
    //BK7231N_TRX_RAM.REG0x0->bits.tssi_statectrl = 1;
    BK7231N_TRX_RAM.REG0x0.bits.enIQcal = 0;
    BK7231N_TRX_RAM.REG0x0.bits.enPcal = 0;
    BK7231N_TRX_RAM.REG0x0.bits.tssiDC_gc = 0;//wyg
//    BK7231N_TRX_RAM.REG0x0.bits.enPcaliGm = 0;
    BK7231N_TRX_RAM.REG0xF.bits.tssi_cal_en = 1;
    //20170330
    BK7231N_TRX_RAM.REG0xC.bits.pamapen = 0;

    BK7231N_TRX_RAM.REG0x5.bits.chspi = 5;
    CAL_WR_TRXREGS(0x5);

    CAL_WR_TRXREGS(0x0);
    CAL_WR_TRXREGS(0xC);
    CAL_WR_TRXREGS(0xF);
    cal_delay(CAL_TX_NUM);//first sar dac delay needs double time

if (1) //fix default Q, calibrate I
{
    // I DC calibration;
    BK7231N_RC_REG.REG0x4F->bits.TXQDCCOMP = UNSIGNEDOFFSET10 + 0; //default

    if(tx_dc_cal_mode == TX_DC_CAL)
    {
        low = UNSIGNEDOFFSET10 - MINOFFSET ;
        high = UNSIGNEDOFFSET10 + MINOFFSET ;
    }
    else if (tx_dc_cal_mode == TX_DC_CAL_IQ)
    {
        low = UNSIGNEDOFFSET10 - MINOFFSET ;
        high = UNSIGNEDOFFSET10 + MINOFFSET ;
    }
    else if (tx_dc_cal_mode == TX_DC_LOOPBACK_CAL_IQ)
    {
        low = UNSIGNEDOFFSET10 - 3 * MINOFFSET ;
        high = UNSIGNEDOFFSET10 + 3 * MINOFFSET ;
    }
	else
	{
		low = BK7231N_RC_REG.REG0x4F->bits.TXIDCCOMP;
		high = BK7231N_RC_REG.REG0x4F->bits.TXIDCCOMP;
	}
    BK7231N_RC_REG.REG0x4F->bits.TXIDCCOMP = low;

    detect_dc_low = bk7011_get_tx_dc();


    BK7231N_RC_REG.REG0x4F->bits.TXIDCCOMP = high;
    detect_dc_high = bk7011_get_tx_dc();
    //Step 1 3~6 search;
    srchcnt = 0;

    CAL_TIM_PRT("%d:(low=0x%x) detect_dc_low=%d,(high=0x%x) detect_dc_high=%d\n", __LINE__, low, detect_dc_low, high, detect_dc_high);

    if (detect_dc_low < detect_dc_high)
    {
        high = BK_TX_DAC_SIGNED_MAX;
        low = high - search_thrd;
    }
    else
    {
        low = BK_TX_DAC_SIGNED_MIN;
        high = low + search_thrd;
    }

    BK7231N_RC_REG.REG0x4F->bits.TXIDCCOMP = high;
    detect_dc_high = bk7011_get_tx_dc();
    BK7231N_RC_REG.REG0x4F->bits.TXIDCCOMP = low;
    detect_dc_low = bk7011_get_tx_dc();
	
    CAL_TIM_PRT("%d:(low=0x%x) detect_dc_low=%d,(high=0x%x) detect_dc_high=%d\n", __LINE__, low, detect_dc_low, high, detect_dc_high);
    do
    {
        if(detect_dc_low < detect_dc_high)
        {
            high = ((low + high) >> 1) + (((low + high) & 0x01) ? 1 : 0);
            BK7231N_RC_REG.REG0x4F->bits.TXIDCCOMP = high;
            detect_dc_high = bk7011_get_tx_dc();

        }
        else
        {
            low = ((low + high) >> 1) + (((low + high) & 0x01) ? 1 : 0);
            BK7231N_RC_REG.REG0x4F->bits.TXIDCCOMP = low;
            detect_dc_low = bk7011_get_tx_dc();
        }
    CAL_TIM_PRT("%d:(low=0x%x) detect_dc_low=%d,(high=0x%x) detect_dc_high=%d\n", __LINE__, low, detect_dc_low, high, detect_dc_high);
    }
    while((high - low) > 1);
    i_index = ((detect_dc_low < detect_dc_high) ? low : high);
}

if (2) //fix calibrated I, calibrated Q
{
    // Q DC calibration;
    //Step 1 3~6 search;
    if ((tx_dc_cal_mode == TX_DC_CAL) || (tx_dc_cal_mode == TX_DC_CAL_IQ))
    {
        low = UNSIGNEDOFFSET10 - MINOFFSET ;
        high = UNSIGNEDOFFSET10 + MINOFFSET ;
    }
    else if (tx_dc_cal_mode == TX_DC_LOOPBACK_CAL_IQ)
    {
        low = UNSIGNEDOFFSET10 - 3 * MINOFFSET ;
        high = UNSIGNEDOFFSET10 + 3 * MINOFFSET ;
    }

    //20170330
    srchcnt = 0;
    BK7231N_RC_REG.REG0x4F->bits.TXIDCCOMP = i_index; //default

    BK7231N_RC_REG.REG0x4F->bits.TXQDCCOMP = low;
    detect_dc_low = bk7011_get_tx_dc();


    BK7231N_RC_REG.REG0x4F->bits.TXQDCCOMP = high;
    detect_dc_high = bk7011_get_tx_dc();

    CAL_TIM_PRT("%d:(low=0x%x) detect_dc_low=%d,(high=0x%x) detect_dc_high=%d\n", __LINE__, low, detect_dc_low, high, detect_dc_high);
    if(detect_dc_low < detect_dc_high)
    {

        high = BK_TX_DAC_SIGNED_MAX;
        low = high - search_thrd;
    }
    else
    {
        low = BK_TX_DAC_SIGNED_MIN;
        high = low + search_thrd;
    }

    BK7231N_RC_REG.REG0x4F->bits.TXQDCCOMP = high;
    detect_dc_high = bk7011_get_tx_dc();
    BK7231N_RC_REG.REG0x4F->bits.TXQDCCOMP = low;
    detect_dc_low = bk7011_get_tx_dc();

    CAL_TIM_PRT("%d:(low=0x%x) detect_dc_low=%d,(high=0x%x) detect_dc_high=%d\n", __LINE__, low, detect_dc_low, high, detect_dc_high);
    do
    {
        if(detect_dc_low < detect_dc_high)
        {
            high = ((low + high) >> 1) + (((low + high) & 0x01) ? 1 : 0);
            BK7231N_RC_REG.REG0x4F->bits.TXQDCCOMP = high;
            detect_dc_high = bk7011_get_tx_dc();

        }
        else
        {
            low = ((low + high) >> 1) + (((low + high) & 0x01) ? 1 : 0);
            BK7231N_RC_REG.REG0x4F->bits.TXQDCCOMP = low;
            detect_dc_low = bk7011_get_tx_dc();
        }
    CAL_TIM_PRT("%d:(low=0x%x) detect_dc_low=%d,(high=0x%x) detect_dc_high=%d\n", __LINE__, low, detect_dc_low, high, detect_dc_high);
    }
    while((high - low) > 1);
    q_index = ((detect_dc_low < detect_dc_high) ? low : high);
}

if (3) //fix calibrated Q, calibrated I again
{
    // 2nd  I DC calibration;
    //Step 1 3~6 search;
    if ((tx_dc_cal_mode == TX_DC_CAL) || (tx_dc_cal_mode == TX_DC_CAL_IQ))
    {
        low = UNSIGNEDOFFSET10 - MINOFFSET ;
        high = UNSIGNEDOFFSET10 + MINOFFSET ;
    }
    else// (tx_dc_cal_mode == TX_DC_LOOPBACK_CAL_IQ)
    {
        low = UNSIGNEDOFFSET10 - 3 * MINOFFSET ;
        high = UNSIGNEDOFFSET10 + 3 * MINOFFSET ;
    }
    //20170330

    //BK7231N_TRX_RAM.REG0x0.bits.tssi_selrange = 1;//inverse 20141014
    //BK7231N_TRX_RAM.REG0x0.value =  0x00079042;   
    //CAL_WR_TRXREGS(0x0);

    //20170330


    BK7231N_TRX_RAM.REG0x0.bits.tssiDC_gc = 3;//wyg
    CAL_WR_TRXREGS(0x0);
    srchcnt = 0;
    BK7231N_RC_REG.REG0x4F->bits.TXQDCCOMP = q_index; // optimum

    BK7231N_RC_REG.REG0x4F->bits.TXIDCCOMP = low;
    detect_dc_low = bk7011_get_tx_dc();


    BK7231N_RC_REG.REG0x4F->bits.TXIDCCOMP = high;
    detect_dc_high = bk7011_get_tx_dc();
    //low = 0;
    //high = 1023;

    CAL_TIM_PRT("%d:(low=0x%x) detect_dc_low=%d,(high=0x%x) detect_dc_high=%d\n", __LINE__, low, detect_dc_low, high, detect_dc_high);

    if(detect_dc_low < detect_dc_high)
    {
        high = BK_TX_DAC_SIGNED_MAX;
        low = high - search_thrd;
    }
    else
    {
        low = BK_TX_DAC_SIGNED_MIN;
        high = low + search_thrd;
    }

    BK7231N_RC_REG.REG0x4F->bits.TXIDCCOMP = high;
    detect_dc_high = bk7011_get_tx_dc();
    BK7231N_RC_REG.REG0x4F->bits.TXIDCCOMP = low;
    detect_dc_low = bk7011_get_tx_dc();

    CAL_TIM_PRT("%d:adc(0x%x)=%d,adc(0x%x)=%d\n", __LINE__, low, detect_dc_low, high, detect_dc_high);
    do
    {
        if(detect_dc_low < detect_dc_high)
        {
            high = ((low + high) >> 1) + (((low + high) & 0x01) ? 1 : 0);
            BK7231N_RC_REG.REG0x4F->bits.TXIDCCOMP = high;
            detect_dc_high = bk7011_get_tx_dc();
        }
        else
        {
            low = ((low + high) >> 1) + (((low + high) & 0x01) ? 1 : 0);
            BK7231N_RC_REG.REG0x4F->bits.TXIDCCOMP = low;
            detect_dc_low = bk7011_get_tx_dc();
        }
    CAL_TIM_PRT("%d:(low=0x%x) detect_dc_low=%d,(high=0x%x) detect_dc_high=%d\n", __LINE__, low, detect_dc_low, high, detect_dc_high);
    }
    while((high - low) > 1);
    i_index = ((detect_dc_low < detect_dc_high) ? low : high);
}

/*
if (4) //fix calibrated I, calibrated Q again
{
    BK7231N_RC_REG.REG0x4F->bits.TXIDCCOMP = i_index;
    
    // Q DC calibration;
    //Step 1 3~6 search;
    if ((tx_dc_cal_mode == TX_DC_CAL) || (tx_dc_cal_mode == TX_DC_CAL_IQ))
    {
        low = UNSIGNEDOFFSET10 - MINOFFSET ;
        high = UNSIGNEDOFFSET10 + MINOFFSET ;
    }
    else if (tx_dc_cal_mode == TX_DC_LOOPBACK_CAL_IQ)
    {
        low = UNSIGNEDOFFSET10 - 3 * MINOFFSET ;
        high = UNSIGNEDOFFSET10 + 3 * MINOFFSET ;
    }

    //20170330
    srchcnt = 0;
    BK7231N_RC_REG.REG0x4F->bits.TXIDCCOMP = i_index; //default

    BK7231N_RC_REG.REG0x4F->bits.TXQDCCOMP = low;
    detect_dc_low = bk7011_get_tx_dc();


    BK7231N_RC_REG.REG0x4F->bits.TXQDCCOMP = high;
    detect_dc_high = bk7011_get_tx_dc();


    CAL_TIM_PRT("%d:(low=0x%x) detect_dc_low=%d,(high=0x%x) detect_dc_high=%d\n", __LINE__, low, detect_dc_low, high, detect_dc_high);

    if(detect_dc_low < detect_dc_high)
    {
        high = BK_TX_DAC_SIGNED_MAX;
        low = high - search_thrd;
    }
    else
    {
        low = BK_TX_DAC_SIGNED_MIN;
        high = low + search_thrd;
    }

    BK7231N_RC_REG.REG0x4F->bits.TXQDCCOMP = high;
    detect_dc_high = bk7011_get_tx_dc();
    BK7231N_RC_REG.REG0x4F->bits.TXQDCCOMP = low;
    detect_dc_low = bk7011_get_tx_dc();

    CAL_TIM_PRT("444444%d:adc(0x%x)=%d,adc(0x%x)=%d\n", __LINE__, low, detect_dc_low, high, detect_dc_high);
    do
    {
        if(detect_dc_low < detect_dc_high)
        {
            high = ((low + high) >> 1) + (((low + high) & 0x01) ? 1 : 0);
            BK7231N_RC_REG.REG0x4F->bits.TXQDCCOMP = high;
            detect_dc_high = bk7011_get_tx_dc();
        }
        else
        {
            low = ((low + high) >> 1) + (((low + high) & 0x01) ? 1 : 0);
            BK7231N_RC_REG.REG0x4F->bits.TXQDCCOMP = low;
            detect_dc_low = bk7011_get_tx_dc();
        }
    CAL_TIM_PRT("%d:(low=0x%x) detect_dc_low=%d,(high=0x%x) detect_dc_high=%d\n", __LINE__, low, detect_dc_low, high, detect_dc_high);
    }
    while((high - low) > 1);
    q_index = ((detect_dc_low < detect_dc_high) ? low : high);
}


*/
    if(tx_dc_cal_mode == TX_DC_CAL)
    {
        gcali_result.gtx_i_dc_comp = i_index ;//190614 I compensate
        CAL_PRT("gtx_i_dc_comp:0x%x\r\n", gcali_result.gtx_i_dc_comp); 	 
    }
    else if(tx_dc_cal_mode == TX_DC_CAL_IQ)
    {
        gcali_result.gtx_i_dc_comp = i_index;
        CAL_PRT("gtx_i_dc_comp_iq:0x%x\r\n", gcali_result.gtx_i_dc_comp); 	 
    }
    else if (tx_dc_cal_mode == TX_DC_LOOPBACK_CAL_IQ)
    {
        gcali_result.gtx_i_dc_comp_loopback = i_index;      		
        CAL_PRT("gtx_i_dc_comp_loopback_iq:0x%x\r\n", gcali_result.gtx_i_dc_comp_loopback); 	
    }

    if(tx_dc_cal_mode == TX_DC_CAL)
    {
        gcali_result.gtx_q_dc_comp = q_index ;//190614 Q compensate
        CAL_PRT("gtx_q_dc_comp:0x%x\r\n", gcali_result.gtx_q_dc_comp);
    }
    else if(tx_dc_cal_mode == TX_DC_CAL_IQ)
    {
        gcali_result.gtx_q_dc_comp = q_index;
        CAL_PRT("gtx_q_dc_comp_iq:0x%x\r\n", gcali_result.gtx_q_dc_comp);
    }
    else if (tx_dc_cal_mode == TX_DC_LOOPBACK_CAL_IQ)
    {
        gcali_result.gtx_q_dc_comp_loopback = q_index;
        CAL_PRT("gtx_q_dc_comp_loopback_iq:0x%x\r\n", gcali_result.gtx_q_dc_comp_loopback);
    }

    gold_index = q_index + (i_index << 16);

    /* restore changed registers */
    BK7231N_TRX_RAM.REG0x0.bits.enIQcal = 1;
    BK7231N_TRX_RAM.REG0x0.bits.enDCcal = 0;
    BK7231N_TRX_RAM.REG0x5.bits.chspi = 0;
    BK7231N_TRX_RAM.REG0xC.bits.padctrl = 0xC;
    BK7231N_TRX_RAM.REG0xD.value = TRX_REG_0XD_VAL;
    BK7231N_TRX_RAM.REG0xF.bits.tssi_cal_en = 0;

    CAL_WR_TRXREGS(0x0);
    CAL_WR_TRXREGS(0x5);
    CAL_WR_TRXREGS(0xC);
    CAL_WR_TRXREGS(0xD);
    BK7231N_RC_REG.REG0x1A->value = BK7231N_TRX_RAM.REG0xD.value;
    CAL_WR_TRXREGS(0xF);

    bk7011_cal_saradc_runorstop(cali_saradc_desc, 0);

    return gold_index;
}


#define TSSI_RD_TIMES		1//8
static INT32 bk7011_get_tx_i_gain(void)
{
///   int i;
    INT32 detector_i_gain_p, detector_i_gain_n, detector_i_gain;

    BK7231N_RC_REG.REG0x4C->bits.ICONSTANT = UNSIGNEDOFFSET10 + (gcali_result.const_iqcal_p+0x100);
    BK7231N_RC_REG.REG0x4C->bits.QCONSTANT = UNSIGNEDOFFSET10 + 0;
    detector_i_gain_p = bk7011_cal_saradc_read(cali_saradc_desc) * TSSI_RD_TIMES;

    BK7231N_RC_REG.REG0x4C->bits.ICONSTANT = UNSIGNEDOFFSET10 - (gcali_result.const_iqcal_p+0x100);//wyg
    BK7231N_RC_REG.REG0x4C->bits.QCONSTANT = UNSIGNEDOFFSET10 + 0;//wyg
    detector_i_gain_n = bk7011_cal_saradc_read(cali_saradc_desc) * TSSI_RD_TIMES;//wyg

    detector_i_gain = detector_i_gain_p + detector_i_gain_n;

    return detector_i_gain;
}

static INT32 bk7011_get_tx_q_gain(void)
{
 ///   int i;
    INT32 detector_q_gain_p, detector_q_gain_n, detector_q_gain;

    BK7231N_RC_REG.REG0x4C->bits.ICONSTANT = UNSIGNEDOFFSET10 + 0;
    BK7231N_RC_REG.REG0x4C->bits.QCONSTANT = UNSIGNEDOFFSET10 + (gcali_result.const_iqcal_p+0x100);
    detector_q_gain_p = bk7011_cal_saradc_read(cali_saradc_desc) * TSSI_RD_TIMES;

    BK7231N_RC_REG.REG0x4C->bits.ICONSTANT = UNSIGNEDOFFSET10 + 0;//wyg
    BK7231N_RC_REG.REG0x4C->bits.QCONSTANT = UNSIGNEDOFFSET10 - (gcali_result.const_iqcal_p+0x100);//wyg
    detector_q_gain_n = bk7011_cal_saradc_read(cali_saradc_desc) * TSSI_RD_TIMES;//wyg

    detector_q_gain = detector_q_gain_p + detector_q_gain_n;

    return detector_q_gain;
}

static INT32 bk7011_get_tx_i_phase(void)
{
  ///  int i;
    INT32 detector_i_phase_n, detector_i_phase_p, detector_i_phase;


    BK7231N_RC_REG.REG0x4C->bits.ICONSTANT = UNSIGNEDOFFSET10 + gcali_result.const_iqcal_p;
    BK7231N_RC_REG.REG0x4C->bits.QCONSTANT = UNSIGNEDOFFSET10 - gcali_result.const_iqcal_p;
    detector_i_phase_p = bk7011_cal_saradc_read(cali_saradc_desc) * TSSI_RD_TIMES;

    BK7231N_RC_REG.REG0x4C->bits.ICONSTANT = UNSIGNEDOFFSET10 - (gcali_result.const_iqcal_p);
    BK7231N_RC_REG.REG0x4C->bits.QCONSTANT = UNSIGNEDOFFSET10 + (gcali_result.const_iqcal_p);
    detector_i_phase_n = bk7011_cal_saradc_read(cali_saradc_desc) * TSSI_RD_TIMES;
	
    detector_i_phase = detector_i_phase_p + detector_i_phase_n;//wyg

    return detector_i_phase;
}

static INT32 bk7011_get_tx_q_phase(void)
{
///    int i;
    INT32 detector_q_phase_n, detector_q_phase_p, detector_q_phase;

    BK7231N_RC_REG.REG0x4C->bits.ICONSTANT = UNSIGNEDOFFSET10 + gcali_result.const_iqcal_p;
    BK7231N_RC_REG.REG0x4C->bits.QCONSTANT = UNSIGNEDOFFSET10 + gcali_result.const_iqcal_p;
    detector_q_phase_p = bk7011_cal_saradc_read(cali_saradc_desc) * TSSI_RD_TIMES;

    BK7231N_RC_REG.REG0x4C->bits.ICONSTANT = UNSIGNEDOFFSET10 - (gcali_result.const_iqcal_p);
    BK7231N_RC_REG.REG0x4C->bits.QCONSTANT = UNSIGNEDOFFSET10 - (gcali_result.const_iqcal_p);
    detector_q_phase_n = bk7011_cal_saradc_read(cali_saradc_desc) * TSSI_RD_TIMES;
	
    detector_q_phase = detector_q_phase_p + detector_q_phase_n;//wyg

    return detector_q_phase;
}

static INT32 bk7011_get_rx_i_avg_signed(void)
{
    INT32 val;
    val = BK7231N_RC_REG.REG0x3C->bits.RXAVGIRD;

    return val;
//    if(val & 0x00000800)
//    {
//        val |= 0xfffff000;
//    }

//    return abs(val);
}

static INT32 bk7011_get_rx_q_avg_signed(void)
{
    INT32 val;

    val = BK7231N_RC_REG.REG0x3C->bits.RXAVGQRD;

    return val;

//    if(val & 0x00000800)
//    {
//        val |= 0xfffff000;
//    }
//    return abs(val);
}

INT32 bk7011_cal_tx_gain_imbalance(INT32 tx_gain_imb_cal_mode)
{
    INT32 detect_gain_low = 0;
    INT32 detect_gain_high = 0;
    INT16 high, low;
    INT32 index = 0, gold_index = 0;
    INT32 detector_i_gain;
    INT32 detector_q_gain;

    bk7011_cal_saradc_runorstop(cali_saradc_desc, 1);
    BK7231N_RC_REG.REG0x50->bits.TXIGAINCOMP = BK_TX_DAC_UNSIGNED_MASK_GAIN;//1023;
    BK7231N_RC_REG.REG0x50->bits.TXQGAINCOMP = BK_TX_DAC_UNSIGNED_MASK_GAIN;//1023;
    BK7231N_RC_REG.REG0x4C->bits.TESTPATTERN = 1;


    BK7231N_TRX_RAM.REG0xD.bits.entssi = 1;
    //BK7231N_TRX_RAM.REG0xF.bits.tssi_cal_en = 1;
    BK7231N_TRX_RAM.REG0x12.bits.buftstselection = 3;
    BK7231N_TRX_RAM.REG0x12.bits.entstbufldo = 1;

    CAL_WR_TRXREGS(0xD);
    BK7231N_RC_REG.REG0x1A->value = BK7231N_TRX_RAM.REG0xD.value;
    //CAL_WR_TRXREGS(0xF);
    CAL_WR_TRXREGS(0x12);

    BK7231N_TRX_RAM.REG0x0.bits.TSSIsel = 0;
    BK7231N_TRX_RAM.REG0x0.bits.enDCcal = 0;
    BK7231N_TRX_RAM.REG0x0.bits.tssiIQ_gc = 0;
    BK7231N_TRX_RAM.REG0x0.bits.enIQcal = 1;
    BK7231N_TRX_RAM.REG0x0.bits.enPcal = 0;
//    BK7231N_TRX_RAM.REG0x0.bits.enPcaliGm = 0;

    BK7231N_TRX_RAM.REG0xF.bits.tssi_cal_en = 1;
    BK7231N_TRX_RAM.REG0xD.bits.entxsw= 0;
    BK7231N_TRX_RAM.REG0xC.bits.padctrl= 0x1;
    BK7231N_TRX_RAM.REG0xC.bits.pactrl= 1;
    BK7231N_RC_REG.REG0x43->bits.PRE_GAIN_2ND = 0x90;
    CAL_WR_TRXREGS(0x0);
    CAL_WR_TRXREGS(0xC);
    CAL_WR_TRXREGS(0xD);
    BK7231N_RC_REG.REG0x1A->value = BK7231N_TRX_RAM.REG0xD.value;
    CAL_WR_TRXREGS(0xF);
    if(tx_gain_imb_cal_mode == TX_GAIN_IMB_CAL)
    {
        BK7231N_RC_REG.REG0x4F->bits.TXIDCCOMP = gcali_result.gtx_i_dc_comp;
        BK7231N_RC_REG.REG0x4F->bits.TXQDCCOMP = gcali_result.gtx_q_dc_comp;
        BK7231N_TRX_RAM.REG0xD.value = TRX_REG_0XD_TX_IQ_VAL_GAIN_PHASE;//wyg
        CAL_WR_TRXREGS(0xD);    
        BK7231N_RC_REG.REG0x1A->value = BK7231N_TRX_RAM.REG0xD.value;
    }
    else
    {
        BK7231N_RC_REG.REG0x4F->bits.TXIDCCOMP = gcali_result.gtx_i_dc_comp_loopback;
        BK7231N_RC_REG.REG0x4F->bits.TXQDCCOMP = gcali_result.gtx_q_dc_comp_loopback;
        BK7231N_TRX_RAM.REG0xD.value = TRX_REG_0XD_TX_LOOPBACK_IQ_VAL;
        CAL_WR_TRXREGS(0xD);
        BK7231N_RC_REG.REG0x1A->value = BK7231N_TRX_RAM.REG0xD.value;
    }
    

    if(tx_gain_imb_cal_mode == TX_GAIN_IMB_CAL)
    {
       // bk7011_set_tx_pa(gi_gain_tx_pa_dgainPA30, gi_gain_tx_pa_dgainbuf30, 3, 8);
    }
    else
    {
        //bk7011_set_tx_pa(gi_gain_tx_loopback_pa_dgainPA30, gi_gain_tx_loopback_pa_dgainbuf30, 3, 8);
    }
    if(tx_gain_imb_cal_mode == TX_GAIN_IMB_CAL)
    {
        CAL_PRT("\r\nbk7011_cal_tx_gain_imbalance\r\n");
    }
    else
    {
        CAL_PRT("\r\nbk7011_cal_tx_loopback_gain_imbalance\r\n");
    }
    
    CAL_PRT("pactrl  = %d\r\n", BK7231N_TRX_RAM.REG0xC.bits.pactrl );      //??
    CAL_PRT("padctrl =%d\r\n",  BK7231N_TRX_RAM.REG0xC.bits.padctrl);      //??

    if(tx_gain_imb_cal_mode == TX_GAIN_IMB_CAL)
    {
        gcali_result.gtx_i_gain_comp = BK7231N_RC_REG.REG0x50->bits.TXIGAINCOMP;
        gcali_result.gtx_q_gain_comp = BK7231N_RC_REG.REG0x50->bits.TXQGAINCOMP;
    }
    else
    {
        gcali_result.gtx_i_gain_comp_loopback = BK7231N_RC_REG.REG0x50->bits.TXIGAINCOMP;
        gcali_result.gtx_q_gain_comp_loopback = BK7231N_RC_REG.REG0x50->bits.TXQGAINCOMP;
    }

    cpu_delay(500);
    detector_i_gain = bk7011_get_tx_i_gain();
    detector_q_gain = bk7011_get_tx_q_gain();
	
    CAL_TIM_PRT("%d:(i_comp=0x%x) detector_i_gain=%d,(q_comp=0x%x) detector_q_gain=%d\n", __LINE__, BK7231N_RC_REG.REG0x50->bits.TXIGAINCOMP, detector_i_gain, BK7231N_RC_REG.REG0x50->bits.TXQGAINCOMP, detector_q_gain);
    if(abs(detector_q_gain - detector_i_gain) < 3)
    {
        if(tx_gain_imb_cal_mode == TX_GAIN_IMB_CAL)
        {
            gcali_result.gtx_i_gain_comp = BK7231N_RC_REG.REG0x50->bits.TXIGAINCOMP;
            gcali_result.gtx_q_gain_comp = BK7231N_RC_REG.REG0x50->bits.TXQGAINCOMP;
        }
        else
        {
            gcali_result.gtx_i_gain_comp_loopback = BK7231N_RC_REG.REG0x50->bits.TXIGAINCOMP;
            gcali_result.gtx_q_gain_comp_loopback = BK7231N_RC_REG.REG0x50->bits.TXQGAINCOMP;
        }

    bk7011_cal_saradc_runorstop(cali_saradc_desc, 0);
        return (BK7231N_RC_REG.REG0x50->bits.TXQGAINCOMP + (BK7231N_RC_REG.REG0x50->bits.TXIGAINCOMP  << 16));
    }

    if(detector_i_gain > detector_q_gain) // TX_Q_GAIN_COMP NOT CHANGED
    {
        BK7231N_RC_REG.REG0x50->bits.TXQGAINCOMP = BK_TX_DAC_UNSIGNED_MASK_GAIN;//1023;
        low = 0;
        BK7231N_RC_REG.REG0x50->bits.TXIGAINCOMP = low;
        detector_i_gain = bk7011_get_tx_i_gain();
        detector_q_gain = bk7011_get_tx_q_gain();

        detect_gain_low = abs(detector_i_gain - detector_q_gain);

        high = BK_TX_DAC_UNSIGNED_MASK;
        BK7231N_RC_REG.REG0x50->bits.TXIGAINCOMP = high;
        detector_i_gain = bk7011_get_tx_i_gain();
        detector_q_gain = bk7011_get_tx_q_gain();

        detect_gain_high = abs(detector_i_gain - detector_q_gain);

        do
        {
            if(detect_gain_low < detect_gain_high)
            {
                index = low;
                high = ((low + high) >> 1) + (((low + high) & 0x01) ? 1 : 0);
                BK7231N_RC_REG.REG0x50->bits.TXIGAINCOMP = high;
                detector_i_gain = bk7011_get_tx_i_gain();
                detector_q_gain = bk7011_get_tx_q_gain();
                detect_gain_high = abs(detector_i_gain - detector_q_gain);
            }
            else
            {
                index = high;
                low = ((low + high) >> 1) + (((low + high) & 0x01) ? 1 : 0);
                BK7231N_RC_REG.REG0x50->bits.TXIGAINCOMP = low;
                detector_i_gain = bk7011_get_tx_i_gain();
                detector_q_gain = bk7011_get_tx_q_gain();
                detect_gain_low = abs(detector_i_gain - detector_q_gain);
            }

        }
        while((high - low) > 1);
        index = ((detect_gain_low < detect_gain_high) ? low : high);
        BK7231N_RC_REG.REG0x50->bits.TXIGAINCOMP = index;

        if(tx_gain_imb_cal_mode == TX_GAIN_IMB_CAL)
        {
            gcali_result.gtx_i_gain_comp = index;    
        }
        else
        {
            gcali_result.gtx_i_gain_comp_loopback= index;    
        }       
        gold_index = (index << 16) + BK_TX_DAC_UNSIGNED_MASK_GAIN;//1023;
    }
    else  //// TX_I_GAIN_COMP NOT CHANGED
    {
        BK7231N_RC_REG.REG0x50->bits.TXIGAINCOMP = BK_TX_DAC_UNSIGNED_MASK;//1023;
        low = 0;
        BK7231N_RC_REG.REG0x50->bits.TXQGAINCOMP = low;
        detector_i_gain = bk7011_get_tx_i_gain();
        detector_q_gain = bk7011_get_tx_q_gain();

        detect_gain_low = abs(detector_i_gain - detector_q_gain);

        high = BK_TX_DAC_UNSIGNED_MASK;
        BK7231N_RC_REG.REG0x50->bits.TXQGAINCOMP = high;
        detector_i_gain = bk7011_get_tx_i_gain();
        detector_q_gain = bk7011_get_tx_q_gain();

        detect_gain_high = abs(detector_i_gain - detector_q_gain);
        do
        {
            if(detect_gain_low < detect_gain_high)
            {
                index = low;
                high = ((low + high) >> 1) + (((low + high) & 0x01) ? 1 : 0);
                BK7231N_RC_REG.REG0x50->bits.TXQGAINCOMP = high;
                detector_i_gain = bk7011_get_tx_i_gain();
                detector_q_gain = bk7011_get_tx_q_gain();
                detect_gain_high = abs(detector_i_gain - detector_q_gain);
            }
            else
            {
                index = high;
                low = ((low + high) >> 1) + (((low + high) & 0x01) ? 1 : 0);
                BK7231N_RC_REG.REG0x50->bits.TXQGAINCOMP = low;
                detector_i_gain = bk7011_get_tx_i_gain();
                detector_q_gain = bk7011_get_tx_q_gain();
                detect_gain_low = abs(detector_i_gain - detector_q_gain);
            }

        }
        while((high - low) > 1);
        index = ((detect_gain_low < detect_gain_high) ? low : high);
        BK7231N_RC_REG.REG0x50->bits.TXQGAINCOMP = index;
        if(tx_gain_imb_cal_mode == TX_GAIN_IMB_CAL)
        {
            gcali_result.gtx_q_gain_comp = index;    
        }
        else
        {
            gcali_result.gtx_q_gain_comp_loopback= index;    
        }
        gold_index += (BK_TX_DAC_UNSIGNED_MASK_GAIN << 16) + index;//(1023 << 16) + index;
    }

    if(tx_gain_imb_cal_mode == TX_GAIN_IMB_CAL)
    {
        CAL_PRT("gtx_i_gain_comp:0x%x\r\n", gcali_result.gtx_i_gain_comp);
        CAL_PRT("gtx_q_gain_comp:0x%x\r\n", gcali_result.gtx_q_gain_comp);
    }

    /* restore changed registers */
    BK7231N_TRX_RAM.REG0x0.bits.tssiIQ_gc = 1;
    BK7231N_TRX_RAM.REG0xC.value = TRX_REG_0XC_VAL;
    BK7231N_TRX_RAM.REG0xD.value = TRX_REG_0XD_VAL;
    BK7231N_TRX_RAM.REG0xF.bits.tssi_cal_en = 0;
    BK7231N_RC_REG.REG0x43->bits.PRE_GAIN_2ND = 0x200;//
    CAL_WR_TRXREGS(0x0);
    CAL_WR_TRXREGS(0xC);
    CAL_WR_TRXREGS(0xD);
    BK7231N_RC_REG.REG0x1A->value = BK7231N_TRX_RAM.REG0xD.value;
    CAL_WR_TRXREGS(0xF);

    bk7011_cal_saradc_runorstop(cali_saradc_desc, 0);
    return gold_index;
}

static INT32 bk7011_cal_tx_ty2(INT32 tx_phase_comp)
{
    float ty1, ty1_sqr, ty2;
    INT32 tx_ty2;

    ty1 = (-1.0 * (tx_phase_comp - BK_TX_DAC_SIGNED_MIN) * (tx_phase_comp - BK_TX_DAC_SIGNED_MIN)) / (BK_TX_DAC_UNSIGNED_MAX * BK_TX_DAC_UNSIGNED_MAX);
    ty1_sqr = ty1 * ty1;
    ty2 = 1 - ty1 / 2 + 3 * ty1_sqr / 8;
    tx_ty2 = (INT32)((ty2 - 0.5) * BK_TX_DAC_UNSIGNED_MAX + 0.5);

    return tx_ty2;
}

INT32 bk7011_cal_tx_phase_imbalance(INT32 tx_phase_imb_cal_mode)
{
    INT32 detect_phase_low = 0;
    INT32 detect_phase_high = 0;
    INT16 high, low;
    INT32 index = 0, gold_index = 0;
    INT32 detector_i_phase;
    INT32 detector_q_phase;

    bk7011_cal_saradc_runorstop(cali_saradc_desc, 1);

    if(tx_phase_imb_cal_mode == TX_PHASE_IMB_CAL)
    {
        BK7231N_RC_REG.REG0x4F->bits.TXIDCCOMP = gcali_result.gtx_i_dc_comp;
        BK7231N_RC_REG.REG0x4F->bits.TXQDCCOMP = gcali_result.gtx_q_dc_comp;
        BK7231N_RC_REG.REG0x50->bits.TXIGAINCOMP = gcali_result.gtx_i_gain_comp;
        BK7231N_RC_REG.REG0x50->bits.TXQGAINCOMP = gcali_result.gtx_q_gain_comp;
        BK7231N_TRX_RAM.REG0xD.value = TRX_REG_0XD_TX_IQ_VAL_GAIN_PHASE;//wyg//BK7011TRXREG0xD;//0214 close the TX switch to ellimite the antenna infect
        CAL_WR_TRXREGS(0xD);	
        BK7231N_RC_REG.REG0x1A->value = BK7231N_TRX_RAM.REG0xD.value;
    }
    else
    {
        BK7231N_RC_REG.REG0x4F->bits.TXIDCCOMP = gcali_result.gtx_i_dc_comp_loopback;
        BK7231N_RC_REG.REG0x4F->bits.TXQDCCOMP = gcali_result.gtx_q_dc_comp_loopback;
        BK7231N_RC_REG.REG0x50->bits.TXIGAINCOMP = gcali_result.gtx_i_gain_comp_loopback;
        BK7231N_RC_REG.REG0x50->bits.TXQGAINCOMP = gcali_result.gtx_q_gain_comp_loopback;
       BK7231N_TRX_RAM.REG0xD.value = TRX_REG_0XD_TX_LOOPBACK_IQ_VAL;
        CAL_WR_TRXREGS(0xD);
        BK7231N_RC_REG.REG0x1A->value = BK7231N_TRX_RAM.REG0xD.value;
    }	
	

    BK7231N_RC_REG.REG0x51->bits.TXPHASECOMP = BK_TX_DAC_SIGNED_MIN;
    BK7231N_RC_REG.REG0x51->bits.TXTY2 = BK_TX_DAC_SIGNED_MIN;

    BK7231N_RC_REG.REG0x4C->bits.TESTPATTERN = 1;

	if(tx_phase_imb_cal_mode == TX_PHASE_IMB_CAL)
	{
	  //  bk7011_set_tx_pa(gi_gain_tx_pa_dgainPA30, gi_gain_tx_pa_dgainbuf30, 3, 8);
	}
	else
	{
	  //  bk7011_set_tx_pa(gi_phase_tx_loopback_pa_dgainPA30, gi_phase_tx_loopback_pa_dgainbuf30,3, 8);
			
	}	

    if(tx_phase_imb_cal_mode == TX_PHASE_IMB_CAL)
    {
        CAL_PRT("\r\nbk7011_cal_tx_phase_imbalance: \r\n");
    }
    else
    {
        CAL_PRT("\r\nbk7011_cal_tx_loopback_phase_imbalance: \r\n");
    
    }
    
    CAL_PRT("pactrl  = %d\r\n", BK7231N_TRX_RAM.REG0xC.bits.pactrl );      //??
    CAL_PRT("padctrl =%d\r\n",  BK7231N_TRX_RAM.REG0xC.bits.padctrl);      //??

    BK7231N_TRX_RAM.REG0x0.bits.TSSIsel = 0;
    BK7231N_TRX_RAM.REG0x0.bits.enDCcal = 0;
    BK7231N_TRX_RAM.REG0x0.bits.tssiIQ_gc = 0;
    BK7231N_TRX_RAM.REG0x0.bits.enIQcal = 1;
    BK7231N_TRX_RAM.REG0x0.bits.enPcal = 0;
//    BK7231N_TRX_RAM.REG0x0->bits.enPcaliGm = 0;
    BK7231N_TRX_RAM.REG0xF.bits.tssi_cal_en = 1;
    BK7231N_TRX_RAM.REG0xD.bits.entxsw= 0;
    BK7231N_TRX_RAM.REG0xC.bits.padctrl= 0x1;
    BK7231N_TRX_RAM.REG0xC.bits.pactrl= 1;
    BK7231N_RC_REG.REG0x43->bits.PRE_GAIN_2ND = 0x90;//

    CAL_WR_TRXREGS(0x0);
    CAL_WR_TRXREGS(0xC);
    CAL_WR_TRXREGS(0xD);
    BK7231N_RC_REG.REG0x1A->value = BK7231N_TRX_RAM.REG0xD.value;
    CAL_WR_TRXREGS(0xF);

    //low = bk7011_cal_tx_ty2(BK_TX_DAC_SIGNED_MIN);
    low = (1 + 256) * BK_TX_DAC_COEF;
    BK7231N_RC_REG.REG0x51->bits.TXPHASECOMP =  low;
    BK7231N_RC_REG.REG0x51->bits.TXTY2 = bk7011_cal_tx_ty2( low);
    detector_i_phase = bk7011_get_tx_i_phase();
    detector_q_phase = bk7011_get_tx_q_phase();
    detect_phase_low = abs(detector_i_phase - detector_q_phase);

    high = BK_TX_DAC_UNSIGNED_MASK - 1024;//1023 - 256;
    BK7231N_RC_REG.REG0x51->bits.TXPHASECOMP =  high;
    BK7231N_RC_REG.REG0x51->bits.TXTY2 = bk7011_cal_tx_ty2( high);
    detector_i_phase = bk7011_get_tx_i_phase();
    detector_q_phase = bk7011_get_tx_q_phase();
    detect_phase_high = abs(detector_i_phase - detector_q_phase);

    CAL_TIM_PRT("%d:low(0x%x)=%d,high(0x%x)=%d\n", __LINE__, low, detect_phase_low, high, detect_phase_high);

    do
    {
        if(detect_phase_low < detect_phase_high)
        {
            index = low;
            high = ((low + high) >> 1) + (((low + high) & 0x01) ? 1 : 0);
            BK7231N_RC_REG.REG0x51->bits.TXPHASECOMP =  high;
            BK7231N_RC_REG.REG0x51->bits.TXTY2 = bk7011_cal_tx_ty2( high);
            detector_i_phase = bk7011_get_tx_i_phase();
            detector_q_phase = bk7011_get_tx_q_phase();
            detect_phase_high = abs(detector_i_phase - detector_q_phase);
            CAL_TIM_PRT("%d:low(0x%x)=%d,high(0x%x)=%d\n", __LINE__, low, detect_phase_low, high, detect_phase_high);
        }
        else
        {
            index = high;
            low = ((low + high) >> 1) + (((low + high) & 0x01) ? 1 : 0);
            BK7231N_RC_REG.REG0x51->bits.TXPHASECOMP =  low;
            BK7231N_RC_REG.REG0x51->bits.TXTY2 = bk7011_cal_tx_ty2( low);
            detector_i_phase = bk7011_get_tx_i_phase();
            detector_q_phase = bk7011_get_tx_q_phase();
            detect_phase_low = abs(detector_i_phase - detector_q_phase);
            CAL_TIM_PRT("%d:low(0x%x)=%d,high(0x%x)=%d\n", __LINE__, low, detect_phase_low, high, detect_phase_high);		
        }
    }
    while((high - low) > 1);
    index = ((detect_phase_low < detect_phase_high) ? low : high);
    BK7231N_RC_REG.REG0x51->bits.TXPHASECOMP = index;

	if(tx_phase_imb_cal_mode == TX_PHASE_IMB_CAL)
	{
	    gcali_result.gtx_phase_comp =  BK7231N_RC_REG.REG0x51->bits.TXPHASECOMP;
	    gcali_result.gtx_phase_ty2 = BK7231N_RC_REG.REG0x51->bits.TXTY2;

	    CAL_PRT("gtx_phase_comp:0x%x\r\n", gcali_result.gtx_phase_comp);
	    CAL_PRT("gtx_phase_ty2:0x%x\r\n", gcali_result.gtx_phase_ty2);

        gold_index = (gcali_result.gtx_phase_comp << 16) + gcali_result.gtx_phase_ty2;
	}
	else
	{
	    gcali_result.gtx_phase_comp_loopback =  BK7231N_RC_REG.REG0x51->bits.TXPHASECOMP;
	    gcali_result.gtx_phase_ty2_loopback = BK7231N_RC_REG.REG0x51->bits.TXTY2;

	    CAL_PRT("tx_phase_comp:0x%x\r\n", gcali_result.gtx_phase_comp_loopback);
	    CAL_PRT("tx_phase_ty2:0x%x\r\n", gcali_result.gtx_phase_ty2_loopback);

        gold_index = (gcali_result.gtx_phase_comp_loopback << 16) + gcali_result.gtx_phase_ty2_loopback;
	}

    /* restore changed registers */
    BK7231N_TRX_RAM.REG0x0.bits.tssiIQ_gc = 1;
    BK7231N_TRX_RAM.REG0xC.value = TRX_REG_0XC_VAL;
    BK7231N_TRX_RAM.REG0xD.value = TRX_REG_0XD_VAL;
    BK7231N_TRX_RAM.REG0xF.bits.tssi_cal_en = 0;

    CAL_WR_TRXREGS(0x0);
    CAL_WR_TRXREGS(0xC);
    CAL_WR_TRXREGS(0xD);
    BK7231N_RC_REG.REG0x1A->value = BK7231N_TRX_RAM.REG0xD.value;
    CAL_WR_TRXREGS(0xF);
    BK7231N_RC_REG.REG0x43->bits.PRE_GAIN_2ND = 0x200;//
    bk7011_cal_saradc_runorstop(cali_saradc_desc, 0);
    return gold_index;
}

static float bk7011_get_tx_filter_i_ratio(void)
{
    INT32 rx_avg_i_14M, rx_avg_i_500K;
    float rx_avg_ratio;
    BK7231N_RC_REG.REG0x4D->bits.TXSINF = 250; // 9.7MHz
    cal_delay_100us(3*gcali_context.gst_rx_adc);
    rx_avg_i_14M = bk7011_get_rx_i_avg_signed();
    BK7231N_RC_REG.REG0x4D->bits.TXSINF = 125; // 4.85MHz;
    cal_delay_100us(3*gcali_context.gst_rx_adc);
    rx_avg_i_500K = bk7011_get_rx_i_avg_signed();
//    if(rx_avg_i_14M > 0)
    {
        rx_avg_ratio = abs(1.0 * rx_avg_i_500K / rx_avg_i_14M - 1.377);
        return rx_avg_ratio;
    }
//    else
//    {
//        return -1.0;
//    }
}


static float bk7011_get_tx_filter_i_ratio1(void)
{
    int i;
    INT32 rx_avg_i_14M_array[100], rx_avg_i_500K_array[100];
    INT32 rx_avg_i_14M = 0, rx_avg_i_500K = 0;
    float rx_avg_ratio;
    BK7231N_RC_REG.REG0x4D->bits.TXSINF = 250; // 9.7MHz  20180308 BY YIMING;
    
    cal_delay_100us(2*gcali_context.gst_rx_adc);
    for (i=0; i<40; i++)
    {
        cal_delay_100us(gcali_context.gst_rx_adc);
        rx_avg_i_14M_array[i] = bk7011_get_rx_i_avg_signed();
        rx_avg_i_14M += rx_avg_i_14M_array[i];
    }
    BK7231N_RC_REG.REG0x4D->bits.TXSINF = 125; // 4.85MHz;

    cal_delay_100us(2*gcali_context.gst_rx_adc);
    for (i=0; i<40; i++)
    {
        cal_delay_100us(gcali_context.gst_rx_adc);
        rx_avg_i_500K_array[i] = bk7011_get_rx_i_avg_signed();
        rx_avg_i_500K += rx_avg_i_500K_array[i];
    }
//    if(rx_avg_i_14M > 0)
    {
        rx_avg_ratio = abs(1.0 * rx_avg_i_500K / rx_avg_i_14M - 1.377);
        return rx_avg_ratio;
    }
//    else
//    {
//        return -1.0;
//    }
}


static float bk7011_get_tx_filter_q_ratio(void)
{
    INT32 rx_avg_q_14M = 0, rx_avg_q_500K = 0;
    float rx_avg_ratio;
    BK7231N_RC_REG.REG0x4D->bits.TXSINF = 250; // 9.7MHz;
    cal_delay_100us(3*gcali_context.gst_rx_adc);
    rx_avg_q_14M = bk7011_get_rx_q_avg_signed();
    BK7231N_RC_REG.REG0x4D->bits.TXSINF = 125; // 4.85MHz;
    cal_delay_100us(3*gcali_context.gst_rx_adc);
    rx_avg_q_500K = bk7011_get_rx_q_avg_signed();
//    if(rx_avg_i_14M > 0)
    {
        rx_avg_ratio = abs(1.0 * rx_avg_q_500K / rx_avg_q_14M - 1.377);
        return rx_avg_ratio;
    }
//    else
//    {
//        return -1.0;
//    }
}

static float bk7011_get_tx_filter_q_ratio1(void)
{
    int i;
    INT32 rx_avg_q_14M_array[100], rx_avg_q_500K_array[100];
    INT32 rx_avg_q_14M = 0, rx_avg_q_500K = 0;
    float rx_avg_ratio;
    BK7231N_RC_REG.REG0x4D->bits.TXSINF = 250; // 9.7MHz;
    
    cal_delay_100us(2*gcali_context.gst_rx_adc);
    for (i=0; i<40; i++)
    {
        cal_delay_100us(gcali_context.gst_rx_adc);
        rx_avg_q_14M_array[i] = bk7011_get_rx_q_avg_signed();
        rx_avg_q_14M += rx_avg_q_14M_array[i];
    }
    BK7231N_RC_REG.REG0x4D->bits.TXSINF = 125; // 4.85MHz;
    
    cal_delay_100us(2*gcali_context.gst_rx_adc);
    for (i=0; i<40; i++)
    {
        cal_delay_100us(gcali_context.gst_rx_adc);
        rx_avg_q_500K_array[i] = bk7011_get_rx_q_avg_signed();
        rx_avg_q_500K += rx_avg_q_500K_array[i];
    }
//    if(rx_avg_q_14M > 0)
    {
        rx_avg_ratio = abs(1.0 * rx_avg_q_500K / rx_avg_q_14M - 1.377);
        return rx_avg_ratio;
    }
//    else
//    {
//        return -1.0;
//    }
}

void bk7011_get_tx_filter_corner(INT32 *tx_ifilter_corner, INT32 *tx_qfilter_corner)
{
    *tx_ifilter_corner = gcali_result.gtx_ifilter_corner;
    *tx_qfilter_corner = gcali_result.gtx_qfilter_corner;
}

void bk7011_get_tx_dc_comp(INT32 *tx_i_dc_comp, INT32 *tx_q_dc_comp)
{
    *tx_i_dc_comp = gcali_result.gtx_i_dc_comp;
    *tx_q_dc_comp = gcali_result.gtx_q_dc_comp;
}

void bk7011_get_tx_gain_comp(INT32 *tx_i_gain_comp, INT32 *tx_q_gain_comp)
{
    *tx_i_gain_comp = gcali_result.gtx_i_gain_comp;
    *tx_q_gain_comp = gcali_result.gtx_q_gain_comp;
}

void bk7011_get_tx_phase(INT32 *tx_phase_comp, INT32 *tx_phase_ty2)
{
    *tx_phase_comp = gcali_result.gtx_phase_comp;
    *tx_phase_ty2 = gcali_result.gtx_phase_ty2;
}

void bk7011_get_rx_err_wr(INT32 *rx_amp_err_wr, INT32 *rx_phase_err_wr, INT32 *rx_dc_gain_tab)
{
    *rx_phase_err_wr = gcali_result.grx_phase_err_wr;
    *rx_amp_err_wr = gcali_result.grx_amp_err_wr;
    os_memcpy(rx_dc_gain_tab, gcali_result.g_rx_dc_gain_tab, sizeof(gcali_result.g_rx_dc_gain_tab));
}

void bk7011_get_tx_tssi_thred(INT32 *tx_tssi_thred_b, INT32 *tx_tssi_thred_g)
{
    *tx_tssi_thred_b = gcali_context.gtx_tssi_thred_b;
    *tx_tssi_thred_g = gcali_context.gtx_tssi_thred_g;
}

INT32 bk7011_cal_tx_filter_corner()
{
    int i;
    float float_1 = 1100.00;
    float float_2 = 1100.00;
    float tx_avg_ratio_low = 0.0;
    float tx_avg_ratio_high = 0.0;
    INT16 high, low;
    INT32 index = 0, gold_index = 0;
    INT32 index1 = 0;
    INT32 index2 = 0;
///    INT32 index3 = 0;
    BK7231N_TRX_RAM.REG0x6.bits.capcal_sel = 0;
    CAL_WR_TRXREGS(0x6);

    BK7231N_RC_REG.REG0x52->bits.TXIQSWAP = 0; /* I/Q SWAP*/
    BK7231N_RC_REG.REG0x1C->bits.FRXON = 1;
    // I CAL
    BK7231N_TRX_RAM.REG0x6.bits.lpfcapcali50 = 0x00;
    BK7231N_TRX_RAM.REG0x7.bits.lpfcapcali = 0x02;
    BK7231N_TRX_RAM.REG0x7.bits.txif_2rd = 0x0;
    BK7231N_TRX_RAM.REG0x7.bits.abws_en = 0x0;


	
    BK7231N_TRX_RAM.REG0xD.value = TRX_REG_0XD_TX_FILTER_IQ_VAL;//BK7011TRXREG0xD;//0xE00F02B9;//0xFC4E03B9;//
    CAL_WR_TRXREGS(0x7);
    CAL_WR_TRXREGS(0x6);
    CAL_WR_TRXREGS(0xD);
    BK7231N_RC_REG.REG0x1A->value = BK7231N_TRX_RAM.REG0xD.value;

    //12/10/2014 for D version
    //BK7231N_TRX_RAM.REG0x0->bits.tssi_statectrl = 1;
    BK7231N_TRX_RAM.REG0xF.bits.tssi_cal_en = 0;
    BK7231N_TRX_RAM.REG0xF.bits.sinad_rx_en = 0;
    CAL_WR_TRXREGS(0x0);
    CAL_WR_TRXREGS(0xF);


    BK7231N_RC_REG.REG0x4C->bits.TESTPATTERN = 2;
    BK7231N_RC_REG.REG0x4D->bits.TXSINMODE = 1;
    BK7231N_RC_REG.REG0x4D->bits.TXSINAMP = 6;
    BK7231N_RC_REG.REG0x3C->bits.RXDCCALEN = 1;
    BK7231N_RC_REG.REG0x3C->bits.RXAVGMODE = 1;


    //cpu_delay(gcali_context.gst_rx_adc);

    low = 0;
    BK7231N_TRX_RAM.REG0x6.bits.lpfcapcali50 = low & 0x3F;
    BK7231N_TRX_RAM.REG0x7.bits.lpfcapcali = (low >> 6) & 0x3;
    CAL_WR_TRXREGS(0x6);
    CAL_WR_TRXREGS(0x7);

    tx_avg_ratio_low = bk7011_get_tx_filter_i_ratio();

    high = 255;
    BK7231N_TRX_RAM.REG0x6.bits.lpfcapcali50 = high & 0x3F;
    BK7231N_TRX_RAM.REG0x7.bits.lpfcapcali = (high >> 6) & 0x3;
    CAL_WR_TRXREGS(0x6);
    CAL_WR_TRXREGS(0x7);

    tx_avg_ratio_high = bk7011_get_tx_filter_i_ratio();
	

    do
    {
        if(tx_avg_ratio_low < tx_avg_ratio_high)
        {
            index = low;
            high = high - ((high - low) >> 2);
            //high = ((low + high) >> 1) + (((low + high) & 0x01) ? 1 : 0);
            BK7231N_TRX_RAM.REG0x6.bits.lpfcapcali50 = high & 0x3F;
            BK7231N_TRX_RAM.REG0x7.bits.lpfcapcali = (high >> 6) & 0x3;
            CAL_WR_TRXREGS(0x6);
            CAL_WR_TRXREGS(0x7);
            tx_avg_ratio_high = bk7011_get_tx_filter_i_ratio();
        }
        else
        {
            index = high;
            low = low + ((high - low) >> 2);
            //low = ((low + high) >> 1) + (((low + high) & 0x01) ? 1 : 0);
            BK7231N_TRX_RAM.REG0x6.bits.lpfcapcali50 = low & 0x3F;
            BK7231N_TRX_RAM.REG0x7.bits.lpfcapcali = (low >> 6) & 0x3;
            CAL_WR_TRXREGS(0x6);
            CAL_WR_TRXREGS(0x7);
            tx_avg_ratio_low = bk7011_get_tx_filter_i_ratio();
        }
    }
    while((high - low) > 3);
    index = ((tx_avg_ratio_low < tx_avg_ratio_high) ? low : high);
	gcali_result.gtx_ifilter_corner = index ;
	index1=index;
    gold_index = index << 8;
//    gtx_ifilter_corner = index;

    for (i=0; i<7; i++)
    {
        if (((index - 2 + i) >= 0) && ((index - 2 + i) < 256))
        {
            BK7231N_TRX_RAM.REG0x6.bits.lpfcapcali50 = (index - 2 + i) & 0x3F;
            BK7231N_TRX_RAM.REG0x7.bits.lpfcapcali = ((index - 2 + i) >> 6) & 0x3;
            CAL_WR_TRXREGS(0x6);
            CAL_WR_TRXREGS(0x7);
#ifdef SCAN_IQ_FILTER_CORNER
            gcali_result.index_test[i] = index - 2 + i;
#endif
            float_2 = bk7011_get_tx_filter_i_ratio1();
#ifdef SCAN_IQ_FILTER_CORNER
            gcali_result.float2_test[i] = float_2;
#endif
            if (float_1 > float_2)
            {
                float_1 = float_2;
                gcali_result.gtx_ifilter_corner = index - 2 + i;
            }
//            if (gtx_ifilter_corner == 11)
//            {
//                gtx_ifilter_corner = 11;
//            }
        }
    }
                gcali_result.gtx_ifilter_corner = gcali_result.gtx_ifilter_corner - 1;
    
#ifdef SCAN_IQ_FILTER_CORNER
    gcali_result.g_sctrl_bias_reg = sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_BIAS_REG_READ, &gcali_result.g_sctrl_bias_reg);
    gcali_result.g_sctrl_bias_reg = (gcali_result.g_sctrl_bias_reg >> PARAM_LDO_VAL_MANUAL_POSI) & PARAM_LDO_VAL_MANUAL_MASK;


    BK7231N_TRX_RAM.REG0x6.bits.lpfcapcali50 = gcali_result.gtx_ifilter_corner & 0x3F;
    BK7231N_TRX_RAM.REG0x7.bits.lpfcapcali = (gcali_result.gtx_ifilter_corner >> 6) & 0x3;
    CAL_WR_TRXREGS(0x6);
    CAL_WR_TRXREGS(0x7);

    gcali_result.grx_avg_i_14M = 0;
    for (i=0; i<8; i++)
    {
        gcali_result.grx_avg_i_14M += bk7011_get_rx_i_avg_signed();
    }

    gcali_result.grx_avg_i_14M2 = 0;
    BK7231N_RC_REG.REG0x4D->bits.TXSINF = 255; // 14MHz;
    for (i=0; i<8; i++)
    {
        cal_delay_100us(3*gcali_context.gst_rx_adc);
        gcali_result.grx_avg_i_14M2 += bk7011_get_rx_i_avg_signed();
    }
#endif
    
    // Q CAL
    BK7231N_TRX_RAM.REG0x6.bits.lpfcapcalq50 = 0x00;
    BK7231N_TRX_RAM.REG0x7.bits.lpfcapcalq = 0x02;
    BK7231N_TRX_RAM.REG0xD.value = TRX_REG_0XD_TX_FILTER_IQ_VAL;//BK7011TRXREG0xD;//0xE00F02B9;//0xFC4E03B9;//
    CAL_WR_TRXREGS(0x6);
    CAL_WR_TRXREGS(0x7);
    CAL_WR_TRXREGS(0xD);
    BK7231N_RC_REG.REG0x1A->value = BK7231N_TRX_RAM.REG0xD.value;

    //12/10/2014 for D version
    //BK7231N_TRX_RAM.REG0x0->bits.tssi_statectrl = 1;
    BK7231N_TRX_RAM.REG0xF.bits.tssi_cal_en = 0;
    BK7231N_TRX_RAM.REG0xF.bits.sinad_rx_en = 0;
    CAL_WR_TRXREGS(0x0);
    CAL_WR_TRXREGS(0xF);


    BK7231N_RC_REG.REG0x4C->bits.TESTPATTERN = 2;
    BK7231N_RC_REG.REG0x4D->bits.TXSINMODE = 2;
    BK7231N_RC_REG.REG0x4D->bits.TXSINAMP = 6;
    BK7231N_RC_REG.REG0x3C->bits.RXDCCALEN = 1;
    BK7231N_RC_REG.REG0x3C->bits.RXAVGMODE = 1;


    //cpu_delay(gcali_context.gst_rx_adc);

    low = 0;
    BK7231N_TRX_RAM.REG0x6.bits.lpfcapcalq50 = low & 0x3F;
    BK7231N_TRX_RAM.REG0x7.bits.lpfcapcalq = (low >> 6) & 0x3;
    CAL_WR_TRXREGS(0x6);
    CAL_WR_TRXREGS(0x7);

    tx_avg_ratio_low = bk7011_get_tx_filter_q_ratio();
    high = 255;
    BK7231N_TRX_RAM.REG0x6.bits.lpfcapcalq50 = high & 0x3F;
    BK7231N_TRX_RAM.REG0x7.bits.lpfcapcalq = (high >> 6) & 0x3;
    CAL_WR_TRXREGS(0x6);
    CAL_WR_TRXREGS(0x7);

    tx_avg_ratio_high = bk7011_get_tx_filter_q_ratio();
    do
    {
        if(tx_avg_ratio_low < tx_avg_ratio_high)
        {
            index = low;
            high = high - ((high - low) >> 2);
            //high = ((low + high) >> 1) + (((low + high) & 0x01) ? 1 : 0);
            BK7231N_TRX_RAM.REG0x6.bits.lpfcapcalq50 = high & 0x3F;
            BK7231N_TRX_RAM.REG0x7.bits.lpfcapcalq = (high >> 6) & 0x3;
            CAL_WR_TRXREGS(0x6);
            CAL_WR_TRXREGS(0x7);
            tx_avg_ratio_high = bk7011_get_tx_filter_q_ratio();
        }
        else
        {
            index = high;
            low = low + ((high - low) >> 2);
            //low = ((low + high) >> 1) + (((low + high) & 0x01) ? 1 : 0);
            BK7231N_TRX_RAM.REG0x6.bits.lpfcapcalq50 = low & 0x3F;
            BK7231N_TRX_RAM.REG0x7.bits.lpfcapcalq = (low >> 6) & 0x3;
            CAL_WR_TRXREGS(0x6);
            CAL_WR_TRXREGS(0x7);
            tx_avg_ratio_low = bk7011_get_tx_filter_q_ratio();
        }
    }
    while((high - low) > 3);
    index = ((tx_avg_ratio_low < tx_avg_ratio_high) ? low : high);
	  gcali_result.gtx_qfilter_corner = index ;
	index2=index;
    gold_index += index;
//    gtx_qfilter_corner = index;
    float_1 = 1100;
    for (i=0; i<7; i++)
    {
        if (((index - 2 + i) >= 0) && ((index - 2 + i) < 256))
        {
            BK7231N_TRX_RAM.REG0x6.bits.lpfcapcalq50 = (index - 2 + i) & 0x3F;
            BK7231N_TRX_RAM.REG0x7.bits.lpfcapcalq = ((index - 2 + i) >> 6) & 0x3;
            CAL_WR_TRXREGS(0x6);
            CAL_WR_TRXREGS(0x7);
            float_2 = bk7011_get_tx_filter_q_ratio1();
            if (float_1 > float_2)
            {
                float_1 = float_2;
                gcali_result.gtx_qfilter_corner = index - 2 + i;
            }
        }
    }
    
#ifdef SCAN_IQ_FILTER_CORNER

    BK7231N_TRX_RAM.REG0x6.bits.lpfcapcalq50 = gcali_result.gtx_qfilter_corner & 0x3F;
    BK7231N_TRX_RAM.REG0x7.bits.lpfcapcalq = (gcali_result.gtx_qfilter_corner >> 6) & 0x3;
    CAL_WR_TRXREGS(0x6);
    CAL_WR_TRXREGS(0x7);
    
    gcali_result.grx_avg_q_14M = bk7011_get_rx_q_avg_signed();

    
        BK7231N_RC_REG.REG0x4D->bits.TXSINF = 255; // 14MHz;
    cal_delay_100us(3*gcali_context.gst_rx_adc);
    gcali_result.grx_avg_q_14M2 = bk7011_get_rx_q_avg_signed();
#endif
    
 //    gcali_result.gtx_qfilter_corner=(gcali_result.gtx_ifilter_corner+gcali_result.gtx_qfilter_corner)/2;
 //    gcali_result.gtx_ifilter_corner=gcali_result.gtx_qfilter_corner;
	 
    rwnx_cal_set_lpfcap_iq(gcali_result.gtx_ifilter_corner, gcali_result.gtx_qfilter_corner);
    //gtx_qfilter_corner = gtx_ifilter_corner;
    CAL_PRT("gtx_ifilter_corner over: 0x%x\r\n", gcali_result.gtx_ifilter_corner);
    CAL_PRT("gtx_qfilter_corner over: 0x%x\r\n", gcali_result.gtx_qfilter_corner);

    /* restore changed registers */
    //rwnx_cal_set_lpfcap_iq(0, 0);
	BK7231N_RC_REG.REG0x52->bits.TXIQSWAP = 1; /* I/Q SWAP*/
 //  index3 = (index2+index1)/2;
  
 //  gold_index = index3;
    return (gold_index);
}

void bk7011_rx_cal_en(void)
{
    BK7231N_RC_REG.REG0x3E->bits.RXCOMPEN = 0;

    BK7231N_RC_REG.REG0x1C->bits.FRXON = 1;
    BK7231N_RC_REG.REG0x1C->bits.FTXON = 0;

    BK7231N_RC_REG.REG0x0->bits.forceenable = 1;
    cpu_delay(1);
    BK7231N_RC_REG.REG0x19->bits.FCH0SHDN = 1;
    BK7231N_RC_REG.REG0x19->bits.FCH0TXEN = 0;
    BK7231N_RC_REG.REG0x19->bits.FCH0RXEN = 1;
    BK7231N_RC_REG.REG0x19->bits.FCH0EN = 1;

    // ADC clock change to 80M
    BK7231N_TRX_RAM.REG0xF.bits.clkadc_sel = 0;//20170331   1  --> 0
    CAL_WR_TRXREGS(0xF);

    //BK7231N_TRX_RAM.REG0x12->bits.adcrefbwsel = 0;//20170331  1-->0
    //CAL_WR_TRXREGS(0x12);
}

void bk7011_cal_rx_adc_dlym()
{
    UINT32 state;
    UINT32 REG0x7_value = BK7231N_TRX_RAM.REG0x7.value;

    BK7231N_TRX_RAM.REG0xF.bits.clkadc_sel = 1;
    CAL_WR_TRXREGS(0xF);

    BK7231N_TRX_RAM.REG0x7.bits.adc_dly_men = 0;
    BK7231N_TRX_RAM.REG0x7.bits.autorxifgen = 0;
    BK7231N_TRX_RAM.REG0x7.bits.spilpfrxg30 = 0;
    CAL_WR_TRXREGS(0x7);

    BK7231N_TRX_RAM.REG0x12.bits.pwd_adccal = 1;
    CAL_WR_TRXREGS(0x12);
    BK7231N_TRX_RAM.REG0x12.bits.pwd_adccal = 0;
    CAL_WR_TRXREGS(0x12);
    delay05us(10);

    state = REG_READ(SCTRL_ANALOG_STATE);
    state = (state >> 8) & 0x7F;
    BK7231N_TRX_RAM.REG0x12.bits.adc_dlym = state;
    CAL_WR_TRXREGS(0x12);

    BK7231N_TRX_RAM.REG0x7.value = REG0x7_value;
    CAL_WR_TRXREGS(0x7);

    BK7231N_TRX_RAM.REG0xF.bits.clkadc_sel = 0;
    CAL_WR_TRXREGS(0xF);
}

void bk7011_cal_rx_adc_restore(int enter_or_exit)
{
    static UINT32 REG0x0_value;
    static UINT32 REG0x19_value;
    static UINT32 REG0x1C_value;
    static UINT32 REG0x3E_value;
    static UINT32 REG0xF_value;

    if (enter_or_exit)
    {
        /* enter */
        REG0x0_value = BK7231N_RC_REG.REG0x0->bits.forceenable;
        REG0x19_value = BK7231N_RC_REG.REG0x19->value;
        REG0x1C_value = BK7231N_RC_REG.REG0x1C->value;
        REG0x3E_value = BK7231N_RC_REG.REG0x3E->value;
        REG0xF_value = BK7231N_TRX_RAM.REG0xF.value;

        BK7231N_RC_REG.REG0x3E->bits.RXCOMPEN = 0;
        
        BK7231N_RC_REG.REG0x1C->bits.FRXON = 1;
        BK7231N_RC_REG.REG0x1C->bits.FTXON = 0;
        
        BK7231N_RC_REG.REG0x0->bits.forceenable = 1;
        cpu_delay(1);
        BK7231N_RC_REG.REG0x19->bits.FCH0SHDN = 1;
        BK7231N_RC_REG.REG0x19->bits.FCH0TXEN = 0;
        BK7231N_RC_REG.REG0x19->bits.FCH0RXEN = 1;
        BK7231N_RC_REG.REG0x19->bits.FCH0EN = 1;
        
        // ADC clock change to 80M
        BK7231N_TRX_RAM.REG0xF.bits.clkadc_sel = 0;//20170331   1  --> 0
        CAL_WR_TRXREGS(0xF);
    }
    else
    {
        /* exit */
        BK7231N_RC_REG.REG0x3E->value = REG0x3E_value;
        
        BK7231N_RC_REG.REG0x1C->value = REG0x1C_value;
        
        BK7231N_RC_REG.REG0x0->bits.forceenable = REG0x0_value;
        cpu_delay(1);
        BK7231N_RC_REG.REG0x19->value = REG0x19_value;
        
        // ADC clock change to 80M
        BK7231N_TRX_RAM.REG0xF.value = REG0xF_value;//20170331   1  --> 0
        CAL_WR_TRXREGS(0xF);
    }
}

INT32 bk7011_cal_rx_dc(void)
{
    INT32 index = 0;
    INT32 i, j, k, t, curr, value;
    UINT32 rx_dc_gain_tab_temp[8];
    UINT32 rx_dc_gain_tab_temp1[8];

    BK7231N_TRX_RAM.REG0x7.bits.dig_dcoen = 1; //dco input using digital
    BK7231N_TRX_RAM.REG0x7.bits.autorxifgen = 1;
    CAL_WR_TRXREGS(0x7);

     BK7231N_TRX_RAM.REG0x14.bits.dc_i_0db=0x80;
     BK7231N_TRX_RAM.REG0x14.bits.dc_q_0db=0x80;
     BK7231N_TRX_RAM.REG0x14.bits.dc_i_3db=0x80;
     BK7231N_TRX_RAM.REG0x14.bits.dc_q_3db=0x80;

     BK7231N_TRX_RAM.REG0x15.bits.dc_i_6db=0x80;
     BK7231N_TRX_RAM.REG0x15.bits.dc_q_6db=0x80;
     BK7231N_TRX_RAM.REG0x15.bits.dc_i_9db=0x80;
     BK7231N_TRX_RAM.REG0x15.bits.dc_q_9db=0x80;

     BK7231N_TRX_RAM.REG0x16.bits.dc_i_12db=0x80;
     BK7231N_TRX_RAM.REG0x16.bits.dc_q_12db=0x80;
     BK7231N_TRX_RAM.REG0x16.bits.dc_i_15db=0x80;
     BK7231N_TRX_RAM.REG0x16.bits.dc_q_15db=0x80;

     BK7231N_TRX_RAM.REG0x17.bits.dc_i_18db=0x80;
     BK7231N_TRX_RAM.REG0x17.bits.dc_q_18db=0x80;
     BK7231N_TRX_RAM.REG0x17.bits.dc_i_21db=0x80;
     BK7231N_TRX_RAM.REG0x17.bits.dc_q_21db=0x80;

     BK7231N_TRX_RAM.REG0x18.bits.dc_i_24db=0x80;
     BK7231N_TRX_RAM.REG0x18.bits.dc_q_24db=0x80;
     BK7231N_TRX_RAM.REG0x18.bits.dc_i_27db=0x80;
     BK7231N_TRX_RAM.REG0x18.bits.dc_q_27db=0x80;

     BK7231N_TRX_RAM.REG0x19.bits.dc_i_30db=0x80;
     BK7231N_TRX_RAM.REG0x19.bits.dc_q_30db=0x80;
     BK7231N_TRX_RAM.REG0x19.bits.dc_i_33db=0x80;
     BK7231N_TRX_RAM.REG0x19.bits.dc_q_33db=0x80;

     BK7231N_TRX_RAM.REG0x1A.bits.dc_i_36db=0x80;
     BK7231N_TRX_RAM.REG0x1A.bits.dc_q_36db=0x80;
     BK7231N_TRX_RAM.REG0x1A.bits.dc_i_39db=0x80;
     BK7231N_TRX_RAM.REG0x1A.bits.dc_q_39db=0x80;

     BK7231N_TRX_RAM.REG0x1B.bits.dc_i_42db=0x80;
     BK7231N_TRX_RAM.REG0x1B.bits.dc_q_42db=0x80;
     BK7231N_TRX_RAM.REG0x1B.bits.dc_i_45db=0x80;
     BK7231N_TRX_RAM.REG0x1B.bits.dc_q_45db=0x80;
    /*step 2*/
    BK7231N_RC_REG.REG0x3C->bits.RXDCCALEN = 1;
    BK7231N_RC_REG.REG0x3C->bits.RXAVGMODE = 0;
    BK7231N_RC_REG.REG0x19->bits.FCH0RXHP = 0; //huaming.jiang 20141017
    BK7231N_RC_REG.REG0x1C->bits.FTXON = 0;
    BK7231N_RC_REG.REG0x1C->bits.FRXON = 1;

     BK7231N_TRX_RAM.REG0x5.bits.chspi = 0x0;//2400MHz, 20181120 from 0x64 to 0x0;
    CAL_WR_TRXREGS(0x5);
    delay100us(1);//delay 100us for RFPLL
    
    BK7231N_TRX_RAM.REG0xE.value = 0xD801BCF0;//0xD8013CF0;//0xDA01BCF0; /// D801BCF0;//170217 
    CAL_WR_TRXREGS(0xE);	

    BK7231N_RC_REG.REG0x52->bits.TXIQSWAP = 0; /* I/Q SWAP*/

    for(i = 0; i < 16; i ++)
    {
        BK7231N_RC_REG.REG0x19->bits.FCH0B = (0x70 | i);
        for(j = 0; j < 2; j ++)
        {
            index = 128;
            k = 6;
            do
            {
                CAL_PRT("%d:REG0x%x,bits[%d-%d]=0x%x\n", __LINE__, (0x14 + i / 2),
                            16 * (i % 2) + 8 * j + 7,
                            16 * (i % 2) + 8 * j,
                            index << (16 * (i % 2) + 8 * j));
                
                //set dc offset
                value = (*((volatile unsigned long *)(TRX_BEKEN_BASE + (0x14 + i / 2) * 4)));
                curr = ~(0xff << (16 * (i % 2) + 8 * j));
                value &= curr;
                curr = (index << (16 * (i % 2) + 8 * j));
                value |= curr;
                (*((volatile unsigned long *)(TRX_BEKEN_BASE + (0x14 + i / 2) * 4))) = value;
                while(BK7231N_RC_REG.REG0x1->value & 0xfffffff);
                cal_delay_100us(gcali_context.gst_rx_adc);

                //read dc avg, and calc mean
                CAL_PRT("%d:RXAVG_RD=", __LINE__);
                value = 0;
                for(t = 0; t < 1; t ++)
                {
                    if(j == 0)  curr = BK7231N_RC_REG.REG0x3C->bits.RXAVGIRD;
                    else        curr = BK7231N_RC_REG.REG0x3C->bits.RXAVGQRD;
                    CAL_PRT("0x%x,", curr);
                    if(curr >= 2048) curr -= 4096;
                    value += curr;
                    cpu_delay(100);
                }
                curr = value / 1;
                CAL_PRT(" RXAVG=0x%x\n", curr);

                //calc new dc offset
                if(curr > 0) index += (0x1 << k);
                else         index -= (0x1 << k);
                k --;
            }
            while((k >= 0) && ((curr >= 16) || (curr <= -16)));
            if(k < 0)
            {
                value = (*((volatile unsigned long *)(TRX_BEKEN_BASE + (0x14 + i / 2) * 4)));
                curr = ~(0xff << (16 * (i % 2) + 8 * j));
                value &= curr;
                curr = (index << (16 * (i % 2) + 8 * j));
                value |= curr;
                (*((volatile unsigned long *)(TRX_BEKEN_BASE + (0x14 + i / 2) * 4))) = value;
                while(BK7231N_RC_REG.REG0x1->value & 0xfffffff);
            }
        }
    }

    rx_dc_gain_tab_temp[0] = BK7231N_TRX_REG.REG0x14->value;
    rx_dc_gain_tab_temp[1] = BK7231N_TRX_REG.REG0x15->value;
    rx_dc_gain_tab_temp[2] = BK7231N_TRX_REG.REG0x16->value;
    rx_dc_gain_tab_temp[3] = BK7231N_TRX_REG.REG0x17->value;
    rx_dc_gain_tab_temp[4] = BK7231N_TRX_REG.REG0x18->value;
    rx_dc_gain_tab_temp[5] = BK7231N_TRX_REG.REG0x19->value;
    rx_dc_gain_tab_temp[6] = BK7231N_TRX_REG.REG0x1A->value;
    rx_dc_gain_tab_temp[7] = BK7231N_TRX_REG.REG0x1B->value;

    BK7231N_TRX_RAM.REG0x5.bits.chspi = 0x55;//2485MHz;
    CAL_WR_TRXREGS(0x5);
    delay100us(1);//delay 100us for RFPLL
    
    BK7231N_TRX_RAM.REG0xE.value = 0xD801BCF0;//0xD8013CF0;//0xDA01BCF0; /// D801BCF0;//170217 
    CAL_WR_TRXREGS(0xE);	

    for(i = 0; i < 16; i ++)
    {
        BK7231N_RC_REG.REG0x19->bits.FCH0B = (0x70 | i);
        for(j = 0; j < 2; j ++)
        {
            index = 128;
            k = 6;
            do
            {
                //set dc offset
                value = (*((volatile unsigned long *)(TRX_BEKEN_BASE + (0x14 + i / 2) * 4)));
                curr = ~(0xff << (16 * (i % 2) + 8 * j));
                value &= curr;
                curr = (index << (16 * (i % 2) + 8 * j));
                value |= curr;
                   (*((volatile unsigned long *)(TRX_BEKEN_BASE + (0x14 + i / 2) * 4))) = value;
                while(BK7231N_RC_REG.REG0x1->value & 0xfffffff);
                cal_delay_100us(gcali_context.gst_rx_adc);

                //read dc avg, and calc mean
                value = 0;
                for(t = 0; t < 1; t ++)
                {
                    if(j == 0)  curr = BK7231N_RC_REG.REG0x3C->bits.RXAVGIRD;
                    else        curr = BK7231N_RC_REG.REG0x3C->bits.RXAVGQRD;
					
                    if(curr >= 2048) curr -= 4096;
                    value += curr;
                    cpu_delay(100);
                }
                curr = value / 1;
                //calc new dc offset
                if(curr > 0) index += (0x1 << k);
                else         index -= (0x1 << k);
                k --;
            }
            while((k >= 0) && ((curr >= 16) || (curr <= -16)));
            if(k < 0)
            {
                value = (*((volatile unsigned long *)(TRX_BEKEN_BASE + (0x14 + i / 2) * 4)));
				
                curr = ~(0xff << (16 * (i % 2) + 8 * j));
                value &= curr;
				
                curr = (index << (16 * (i % 2) + 8 * j));
                value |= curr;
				
                (*((volatile unsigned long *)(TRX_BEKEN_BASE + (0x14 + i / 2) * 4))) = value;
                while(BK7231N_RC_REG.REG0x1->value & 0xfffffff);
            }
        }
    }

    rx_dc_gain_tab_temp1[0] = BK7231N_TRX_REG.REG0x14->value;
    rx_dc_gain_tab_temp1[1] = BK7231N_TRX_REG.REG0x15->value;
    rx_dc_gain_tab_temp1[2] = BK7231N_TRX_REG.REG0x16->value;
    rx_dc_gain_tab_temp1[3] = BK7231N_TRX_REG.REG0x17->value;
    rx_dc_gain_tab_temp1[4] = BK7231N_TRX_REG.REG0x18->value;
    rx_dc_gain_tab_temp1[5] = BK7231N_TRX_REG.REG0x19->value;
    rx_dc_gain_tab_temp1[6] = BK7231N_TRX_REG.REG0x1A->value;
    rx_dc_gain_tab_temp1[7] = BK7231N_TRX_REG.REG0x1B->value;

    for (i=0; i<8; i++)
    {
        gcali_result.g_rx_dc_gain_tab[i] = ((((rx_dc_gain_tab_temp[i] & 0x000000FF) + (rx_dc_gain_tab_temp1[i] & 0x000000FF)) / 2) & 0x000000FF)
                            | ((((rx_dc_gain_tab_temp[i] & 0x0000FF00) + (rx_dc_gain_tab_temp1[i] & 0x0000FF00)) / 2) & 0x0000FF00)
                            | ((((rx_dc_gain_tab_temp[i] & 0x00FF0000) + (rx_dc_gain_tab_temp1[i] & 0x00FF0000)) / 2) & 0x00FF0000)
                            | ((((((rx_dc_gain_tab_temp[i] >> 8) & 0x00FF0000) + ((rx_dc_gain_tab_temp1[i] >> 8) & 0x00FF0000)) / 2) & 0x00FF0000) << 8);
    }

    //hengzh:should be BK7231N_TRX_RAM
    BK7231N_TRX_RAM.REG0x14.value = gcali_result.g_rx_dc_gain_tab[0];
    CAL_WR_TRXREGS(0x14);
    BK7231N_TRX_RAM.REG0x15.value = gcali_result.g_rx_dc_gain_tab[1];
    CAL_WR_TRXREGS(0x15);
    BK7231N_TRX_RAM.REG0x16.value = gcali_result.g_rx_dc_gain_tab[2];
    CAL_WR_TRXREGS(0x16);
    BK7231N_TRX_RAM.REG0x17.value = gcali_result.g_rx_dc_gain_tab[3];
    CAL_WR_TRXREGS(0x17);
    BK7231N_TRX_RAM.REG0x18.value = gcali_result.g_rx_dc_gain_tab[4];
    CAL_WR_TRXREGS(0x18);
    BK7231N_TRX_RAM.REG0x19.value = gcali_result.g_rx_dc_gain_tab[5];
    CAL_WR_TRXREGS(0x19);
    BK7231N_TRX_RAM.REG0x1A.value = gcali_result.g_rx_dc_gain_tab[6];
    CAL_WR_TRXREGS(0x1A);
    BK7231N_TRX_RAM.REG0x1B.value = gcali_result.g_rx_dc_gain_tab[7];
    CAL_WR_TRXREGS(0x1B);
	

	/*
    CAL_PRT("%d:g_rx_dc_gain_tab 0 over: 0x%x\r\n", __LINE__,gcali_result.g_rx_dc_gain_tab[0]);
    CAL_PRT("%d:g_rx_dc_gain_tab 1 over: 0x%x\r\n", __LINE__,gcali_result.g_rx_dc_gain_tab[1]);
    CAL_PRT("%d:g_rx_dc_gain_tab 2 over: 0x%x\r\n",__LINE__, gcali_result.g_rx_dc_gain_tab[2]);
    CAL_PRT("%d:g_rx_dc_gain_tab 3 over: 0x%x\r\n", __LINE__,gcali_result.g_rx_dc_gain_tab[3]);
    CAL_PRT("%d:g_rx_dc_gain_tab 4 over: 0x%x\r\n", __LINE__,gcali_result.g_rx_dc_gain_tab[4]);
    CAL_PRT("%d:g_rx_dc_gain_tab 5 over: 0x%x\r\n", __LINE__,gcali_result.g_rx_dc_gain_tab[5]);
    CAL_PRT("%d:g_rx_dc_gain_tab 6 over: 0x%x\r\n", __LINE__,gcali_result.g_rx_dc_gain_tab[6]);
    CAL_PRT("%d:g_rx_dc_gain_tab 7 over: 0x%x\r\n", __LINE__,gcali_result.g_rx_dc_gain_tab[7]);
*/
    BK7231N_RC_REG.REG0x3C->bits.RXDCCALEN = 0;

    BK7231N_TRX_RAM.REG0x7.bits.dig_dcoen = 1;
    BK7231N_TRX_RAM.REG0x7.bits.autorxifgen = 1;
    CAL_WR_TRXREGS(0x7);
    BK7231N_TRX_RAM.REG0x5.bits.chspi = 0xc;
    CAL_WR_TRXREGS(0x5);
    delay100us(1);//delay 100us for RFPLL
    
    BK7231N_RC_REG.REG0x52->bits.TXIQSWAP = 1; /* I/Q SWAP*/
    return 0;
}

INT32 bk7011_cal_rx_iq()
{
    //   INT32 rx_dc_i_rd, rx_dc_q_rd;
    INT32 rx_amp_err_rd, rx_phase_err_rd, rx_ty2_rd;
    INT32 rx_amp_err_wr;
    INT32 rx_phase_err_wr;
    float amp_err, phase_err, ty2_err;
    INT32 gold_index = 0;
    INT32 i, curr, value, value1, value2;
    //bk7011_load_rxiq_init_cfg(); //v1.5

    /*step 1*/
    BK7231N_RC_REG.REG0x1C->bits.FRXON = 1;
    BK7231N_RC_REG.REG0x1C->bits.FTXON = 1;

    //BK7011ADDAMAP.REG0x5->bits.vc40 = 0x1f;
    //BK7011ADDA.REG0x5->value = BK7011ADDAMAP.REG0x5->value;

    //BK7231N_TRX_RAM.REG0x0.bits.tssi_statectrl = 1;
    BK7231N_TRX_RAM.REG0xF.bits.tssi_cal_en = 0;
    BK7231N_TRX_RAM.REG0xF.bits.sinad_rx_en = 0;
    CAL_WR_TRXREGS(0x0);
    CAL_WR_TRXREGS(0xF);

#if 0
    BK7231N_TRX_RAM.REG0x9->bits.grxi2vspi10 = 1;  //12dB
    BK7231N_TRX_RAM.REG0x9->bits.agcrxfeEn = 0; //disable agc  

    BK7231N_TRX_RAM.REG0x7->bits.autorxifgen = 0;//disable agc 
    BK7231N_TRX_RAM.REG0x7->bits.spilpfrxg30 = 0x0a;  //33dB;

    //BK7231N_TRX_RAM.REG0x7->bits.digdcoen = 0;
    CAL_WR_TRXREGS(0x7);
    CAL_WR_TRXREGS(0x9);
#endif

    BK7231N_TRX_RAM.REG0xE.value = TRX_REG_0XE_RXIQ_VAL;
    CAL_WR_TRXREGS(0xE);
  //BK7231N_TRX_RAM.REG0xC.value = TRX_REG_0XC_RXIQ_VAL;
    bk7011_set_tx_pa(gi_cal_rx_iq_pa_dgainPA30, gi_cal_rx_iq_pa_dgainbuf30, 4, 4);	

    BK7231N_RC_REG.REG0x19->bits.FCH0B = 0x1a;//decrease 6dB


    /*searching...*/
    BK7231N_RC_REG.REG0x3E->bits.RXCALEN = 1;
    BK7231N_RC_REG.REG0x4C->bits.TESTPATTERN = 2;
    BK7231N_RC_REG.REG0x4D->bits.TXSINMODE = 0;
    BK7231N_RC_REG.REG0x4D->bits.TXSINAMP = 0x04;  //  increase 6dB
    //BK7231N_RC_REG.REG0x3C->bits.RXAVGMODE = 0;
    //BK7231N_RC_REG.REG0x3C->bits.RXDCCALEN = 1;
    BK7231N_RC_REG.REG0x4D->bits.TXSINF = 179; // 7MHz;
    cal_delay_100us(6*gcali_context.gst_rx_adc);
    cpu_delay(500 * DELAY1US);

    //rx_dc_i_rd = BK7231N_RC_REG.REG0x3C->bits.RXAVGIRD;
    //rx_dc_q_rd = BK7231N_RC_REG.REG0x3C->bits.RXAVGQRD;

    BK7231N_RC_REG.REG0x41->bits.RXDCIWR = 0x0;
    BK7231N_RC_REG.REG0x41->bits.RXDCQWR = 0x0;

    //BK7231N_RC_REG.REG0x3C->bits.RXAVGMODE = 1;
    //BK7231N_RC_REG.REG0x3C->bits.RXDCCALEN = 0;

    value = 0;
    value1 = 0;
    value2 = 0;
    for(i = 0; i < 2; i ++)
    {
        curr = BK7231N_RC_REG.REG0x3F->bits.RXAMPERRRD;
        value += curr - ((curr < BK_TX_DAC_SIGNED_MIN) ? 0: BK_TX_DAC_UNSIGNED_MAX);
        curr = BK7231N_RC_REG.REG0x3F->bits.RXPHASEERRRD;
        value1 += curr - ((curr < BK_TX_DAC_SIGNED_MIN) ? 0: BK_TX_DAC_UNSIGNED_MAX);
        curr = BK7231N_RC_REG.REG0x40->bits.RXTY2RD;
        value2 += curr - ((curr < BK_TX_DAC_SIGNED_MIN) ? 0: BK_TX_DAC_UNSIGNED_MAX);
        cpu_delay(gcali_context.gst_rx_adc);
    }
    rx_amp_err_rd = value / 2;
    rx_phase_err_rd = value1 / 2;
    rx_ty2_rd = value2 / 2;
/* rx_amp_err_rd = value;
 rx_phase_err_rd = value1;
 rx_ty2_rd = value2;*/

    if (1)      // by gwf
    {
        int rx_amp_err_rd_temp;
        int rx_phase_err_rd_temp;
        int rx_ty2_rd_temp;
        if (rx_amp_err_rd & 0x200)
        {
            rx_amp_err_rd_temp = rx_amp_err_rd | 0xFFFFFC00;
        }
        else
        {
            rx_amp_err_rd_temp = rx_amp_err_rd & 0x000003FF;
        }
        if (rx_phase_err_rd & 0x200)
        {
            rx_phase_err_rd_temp = rx_phase_err_rd | 0xFFFFFC00;
        }
        else
        {
            rx_phase_err_rd_temp = rx_phase_err_rd & 0x000003FF;
        }
        if (rx_ty2_rd & 0x200)
        {
            rx_ty2_rd_temp = rx_ty2_rd | 0xFFFFFC00;
        }
        else
        {
            rx_ty2_rd_temp = rx_ty2_rd & 0x000003FF;
        }
        gcali_result.rx_amp_err_rd = rx_amp_err_rd_temp;
        gcali_result.rx_phase_err_rd = rx_phase_err_rd_temp;
        gcali_result.rx_ty2_rd = rx_ty2_rd_temp;
    }
    else
    {
        CAL_FATAL("[rx_iq]rx_amp_err_rd: 0x%03x\r\n", rx_amp_err_rd );
        CAL_FATAL("[rx_iq]rx_phase_err_rd: 0x%03x\r\n", rx_phase_err_rd );    
        CAL_FATAL("[rx_iq]rx_ty2_rd: 0x%03x\r\n", rx_ty2_rd );    
    }

    amp_err = 1.0 * rx_amp_err_rd / 1024;
    phase_err = 1.0 * rx_phase_err_rd / 1024;
    ty2_err = 1.0 * rx_ty2_rd / 1024;

    rx_amp_err_wr = (INT32) (512 * (ty2_err + 1) / (amp_err + 1));
    rx_phase_err_wr = (INT32) (512 * phase_err * (ty2_err + 1));



    BK7231N_RC_REG.REG0x42->bits.RXPHASEERRWR = rx_phase_err_wr;
    BK7231N_RC_REG.REG0x42->bits.RXAMPERRWR = rx_amp_err_wr;
    BK7231N_RC_REG.REG0x3E->bits.RXCOMPEN = 1;
    BK7231N_RC_REG.REG0x3E->bits.RXCALEN = 0;

    gcali_result.grx_amp_err_wr = rx_amp_err_wr;
    gcali_result.grx_phase_err_wr = rx_phase_err_wr;

    CAL_PRT("grx_amp_err_wr:0x%03x\r\n", rx_amp_err_wr);
    CAL_PRT("grx_phase_err_wr:0x%03x\r\n", rx_phase_err_wr);

    gold_index = (rx_amp_err_wr << 16 ) + rx_phase_err_wr;
    /* restore changed registers */
    BK7231N_TRX_RAM.REG0xC.value = TRX_REG_0XC_VAL;
    BK7231N_TRX_RAM.REG0x9.bits.agcrxfeEn = 1; //enable agc  
    BK7231N_TRX_RAM.REG0x7.bits.autorxifgen = 1;//ensable agc 

    CAL_WR_TRXREGS(0x7);
    CAL_WR_TRXREGS(0x9);
    CAL_WR_TRXREGS(0xC);

    return gold_index;
}

void bk7011_set_rx_avg_dc(void)
{
    INT32 rx_dc_i_rd, rx_dc_q_rd;

    BK7231N_RC_REG.REG0x3E->bits.RXCOMPEN = 0;
    BK7231N_RC_REG.REG0x3C->bits.RXAVGMODE = 0;
    BK7231N_RC_REG.REG0x3C->bits.RXDCCALEN = 1;
    cal_delay_100us(gcali_context.gst_rx_adc);
    BK7231N_RC_REG.REG0x3C->bits.RXDCCALEN = 0;

    rx_dc_i_rd = BK7231N_RC_REG.REG0x3C->bits.RXAVGIRD;
    rx_dc_q_rd = BK7231N_RC_REG.REG0x3C->bits.RXAVGQRD;
    BK7231N_RC_REG.REG0x41->bits.RXDCIWR = rx_dc_i_rd;
    BK7231N_RC_REG.REG0x41->bits.RXDCQWR = rx_dc_q_rd;

    BK7231N_RC_REG.REG0x3E->bits.RXCOMPEN = 1;

    return;
}

INT32 bk7011_load_calibration_cfg(void)
{
    BK7231N_RC_REG.REG0x0->value  = BK7231N_RC_REG.REG0x0->value;
    BK7231N_RC_REG.REG0x1->value  = BK7231N_RC_REG.REG0x1->value;
    BK7231N_RC_REG.REG0x5->value  = BK7231N_RC_REG.REG0x5->value ;
    BK7231N_RC_REG.REG0x8->value  = BK7231N_RC_REG.REG0x8->value;
    BK7231N_RC_REG.REG0xB->value  = BK7231N_RC_REG.REG0xB->value;
    BK7231N_RC_REG.REG0x11->value = BK7231N_RC_REG.REG0x11->value;
    BK7231N_RC_REG.REG0x19->value = BK7231N_RC_REG.REG0x19->value;
    BK7231N_RC_REG.REG0x1E->value = BK7231N_RC_REG.REG0x1E->value;

    /**********NEW ADDED************/
    BK7231N_RC_REG.REG0x3C->value = BK7231N_RC_REG.REG0x3C->value;
    BK7231N_RC_REG.REG0x3E->value = BK7231N_RC_REG.REG0x3E->value;
    BK7231N_RC_REG.REG0x3F->value = BK7231N_RC_REG.REG0x3F->value;
    BK7231N_RC_REG.REG0x40->value = BK7231N_RC_REG.REG0x40->value;
    BK7231N_RC_REG.REG0x41->value = BK7231N_RC_REG.REG0x41->value;
    BK7231N_RC_REG.REG0x42->value = BK7231N_RC_REG.REG0x42->value ;
    BK7231N_RC_REG.REG0x4C->value = BK7231N_RC_REG.REG0x4C->value;
    BK7231N_RC_REG.REG0x4D->value = BK7231N_RC_REG.REG0x4D->value;
    BK7231N_RC_REG.REG0x4F->value = BK7231N_RC_REG.REG0x4F->value;
    BK7231N_RC_REG.REG0x50->value = BK7231N_RC_REG.REG0x50->value;
    BK7231N_RC_REG.REG0x51->value = BK7231N_RC_REG.REG0x51->value;
    BK7231N_RC_REG.REG0x52->value = BK7231N_RC_REG.REG0x52->value;
    BK7231N_RC_REG.REG0x54->value = BK7231N_RC_REG.REG0x54->value;
    BK7231N_RC_REG.REG0x5C->value = BK7231N_RC_REG.REG0x5C->value;

    while(BK7231N_RC_REG.REG0x1->value & 0x0FFFFFFF)
    {
        cpu_delay(1);
    }

    CAL_WR_TRXREGS(0x0);
    CAL_WR_TRXREGS(0x1);
    CAL_WR_TRXREGS(0x2);
    CAL_WR_TRXREGS(0x3);
    CAL_WR_TRXREGS(0x4);
    CAL_WR_TRXREGS(0x5);
    CAL_WR_TRXREGS(0x6);
    CAL_WR_TRXREGS(0x7);
    CAL_WR_TRXREGS(0x8);
    CAL_WR_TRXREGS(0x9);
    CAL_WR_TRXREGS(0xA);
    CAL_WR_TRXREGS(0xB);
    CAL_WR_TRXREGS(0xC);
    CAL_WR_TRXREGS(0xD);
    BK7231N_RC_REG.REG0x1A->value = BK7231N_TRX_RAM.REG0xD.value;
    CAL_WR_TRXREGS(0xE);
    CAL_WR_TRXREGS(0xF);
    CAL_WR_TRXREGS(0x10);
    CAL_WR_TRXREGS(0x11);
    CAL_WR_TRXREGS(0x12);
    CAL_WR_TRXREGS(0x13);
    CAL_WR_TRXREGS(0x14);
    CAL_WR_TRXREGS(0x15);
    CAL_WR_TRXREGS(0x16);
    CAL_WR_TRXREGS(0x17);
    CAL_WR_TRXREGS(0x18);
    CAL_WR_TRXREGS(0x19);
    CAL_WR_TRXREGS(0x1A);
    CAL_WR_TRXREGS(0x1B);

    BK7231N_TRX_RAM.REG0x7.bits.autorxifgen = 1;
    BK7231N_TRX_RAM.REG0x7.bits.dig_dcoen = 1;
    BK7231N_TRX_RAM.REG0x7.bits.spilpfrxg30 = 5;
    CAL_WR_TRXREGS(0x7);

    CAL_PRT("[%s:%d]v=0x%x,0x%x\n", __FUNCTION__, __LINE__, gcali_result.gtx_ifilter_corner, gcali_result.gtx_qfilter_corner);
    rwnx_cal_set_lpfcap_iq(gcali_result.gtx_ifilter_corner, gcali_result.gtx_qfilter_corner);

    bk7011_set_rx_avg_dc(); // 11/11/2014

    return 0;
}

void bk7011_set_tx_after_cal(void)
{
    BK7231N_RC_REG.REG0x0->bits.forceenable = 0;
    cpu_delay(1);
    BK7231N_RC_REG.REG0x19->bits.FCH0EN = 1;
    BK7231N_RC_REG.REG0x19->bits.FCH0SHDN = 1;
    BK7231N_RC_REG.REG0x19->bits.FCH0RXEN = 0;
    BK7231N_RC_REG.REG0x19->bits.FCH0TXEN = 1;
    BK7231N_RC_REG.REG0x1C->bits.FRXON = 0;
    BK7231N_RC_REG.REG0x1C->bits.FTXON = 1;
    BK7231N_RC_REG.REG0x4C->bits.TESTPATTERN = 0;
#if DIFFERENCE_PIECES_CFG
	BK7231N_RC_REG.REG0x0->bits.ch0en = 0;
#endif
    //BK7011ICU.REG0x1->bits.txon = 1;
    //BK7011ICU.REG0x1->bits.rxon = 0;
}

void bk7011_set_rx_after_cal(void)
{
#if 0
    BK7231N_RC_REG.REG0x0->bits.forceenable = 0;
    cpu_delay(1);
    BK7231N_RC_REG.REG0x19->bits.FCH0EN = 1;
    BK7231N_RC_REG.REG0x19->bits.FCH0SHDN = 1;
    BK7231N_RC_REG.REG0x19->bits.FCH0RXEN = 1;
    BK7231N_RC_REG.REG0x19->bits.FCH0TXEN = 0;
    BK7231N_RC_REG.REG0x1C->bits.FRXON = 1;
    BK7231N_RC_REG.REG0x1C->bits.FTXON = 0;
    BK7231N_RC_REG.REG0x4C->bits.TESTPATTERN = 0;
#endif
#if DIFFERENCE_PIECES_CFG
    BK7231N_RC_REG.REG0x0->bits.ch0en = 0;
#endif
    BK7231N_RC_RAM.REG0x3C.bits.RXHPFBYPASS = 0;
    BK7231N_TRX_RAM.REG0xE.value = 0xDA01BCF0;
    CAL_WR_TRXREGS(0xE);

    //BK7231N_TRX_RAM.REG0xC->value = TRX_REG_0XC_VAL;  //170217  delete
    //CAL_WR_TRXREGS(0xC);
    
    //BK7011ICU.REG0x1->bits.txon = 0;
    //BK7011ICU.REG0x1->bits.rxon = 1;

#if 0
    // ADC clock change to 40M
    BK7231N_TRX_RAM.REG0xF.bits.clkadc_sel = 0;
    CAL_WR_TRXREGS(0xF);

    //BK7231N_TRX_RAM.REG0x12.bits.adcrefbwsel = 0;
    CAL_WR_TRXREGS(0x12);
#endif
}

extern void mpb_tx_mode(void);

#define CALI_DPD_TEST       0
#if CALI_DPD_TEST
#define I_TABLE_ADDR        0x01050400
#define Q_TABLE_ADDR        0x01050600

static UINT16 i_table_val[256] =
{
    0, 6, 13, 19, 26, 35, 40, 47, 52, 57, 68, 73, 76, 82, 88, 91, 96, 102, 107, 107, 118, 118, 120, 127, 132, 134, 139, 141, 146, 149, 152, 158, 161, 161, 163, 164, 168, 172, 172, 176, 181, 177, 179, 181, 185, 187, 189, 185, 191, 195, 196, 195, 196, 197, 203, 198, 204, 201, 207, 199, 206, 207, 207, 207, 207, 210, 210, 212, 214, 215, 215, 215, 206, 216, 215, 221, 217, 219, 215, 219, 222, 222, 225, 229, 225, 223, 228, 226, 226, 229, 229, 226, 225, 227, 226, 226, 228, 232, 230, 229, 230, 231, 230, 231, 234, 235, 236, 238, 241, 244, 245, 247, 248, 251, 252, 255, 255, 258, 259, 262, 263, 265, 267, 268, 271, 272, 275, 275, 278, 280, 282, 284, 287, 288, 291, 293, 295, 297, 299, 301, 304, 306, 308, 310, 312, 314, 317, 319, 321, 323, 325, 327, 330, 332, 334, 336, 338, 341, 343, 345, 347, 349, 351, 354, 356, 358, 360, 362, 364, 367, 369, 371, 373, 375, 377, 380, 382, 384, 386, 388, 390, 393, 395, 397, 399, 401, 403, 406, 408, 410, 412, 414, 416, 419, 421, 423, 425, 427, 429, 432, 434, 436, 438, 440, 442, 445, 447, 449, 451, 453, 455, 458, 460, 462, 464, 466, 468, 471, 473, 475, 477, 479, 481, 484, 486, 488, 490, 492, 495, 497, 499, 501, 503, 505, 508, 510, 512, 514, 516, 518, 521, 523, 525, 527, 529, 531, 534, 536, 538, 540, 542, 544, 547, 549, 551, 562
};

static UINT16 q_table_val[256] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 2, 1, 5, 8, 5, 9, 6, 5, 7, 4, 7, 8, 17, 13, 12, 14, 15, 12, 12, 18, 12, 13, 16, 16, 17, 19, 20, 24, 22, 30, 23, 21, 24, 30, 27, 26, 24, 27, 26, 30, 28, 30, 32, 31, 31, 32, 32, 33, 35, 35, 33, 35, 34, 32, 32, 32, 34, 33, 32, 31, 32, 30, 33, 29, 30, 29, 30, 29, 29, 28, 27, 29, 27, 28, 26, 26, 26, 27, 27, 27, 27, 27, 28, 28, 28, 28, 28, 29, 29, 29, 29, 29, 30, 30, 30, 30, 30, 31, 31, 31, 31, 31, 32, 32, 32, 32, 32, 33, 33, 33, 33, 33, 34, 34, 34, 34, 34, 35, 35, 35, 35, 35, 36, 36, 36, 36, 36, 37, 37, 37, 37, 38, 38, 38, 38, 38, 39, 39, 39, 39, 39, 40, 40, 40, 40, 40, 41, 41, 41, 41, 41, 42, 42, 42, 42, 42, 43, 43, 43, 43, 43, 44, 44, 44, 44, 44, 45, 45, 45, 45, 45, 46, 46, 46, 46, 46, 47, 47, 47, 47, 48, 48, 48, 48, 48, 49, 49, 49, 49, 49, 50, 50, 50, 50, 50, 51, 51, 51, 51, 51, 52, 52, 48
};

UINT32 bk7211_cal_tx_dpd_load(void)
{

    UINT16 *i_tbl_addr = (UINT16 *)I_TABLE_ADDR;
    UINT16 *q_tbl_addr = (UINT16 *)Q_TABLE_ADDR;
    UINT16 k;

    BK7231N_RC_REG.REG0x4C->bits.DPDEN = 0;

    os_memcpy(i_tbl_addr, (UINT16 *)&i_table_val[0], 256 * 2);
    os_memcpy(q_tbl_addr, (UINT16 *)&q_table_val[0], 256 * 2);

#if 1
    for(k = 0; k < 256; k++)
    {
        i_tbl_addr[k] = 1;
        q_tbl_addr[k] = 0;
    }
#endif
    return 0;

}
#endif

#if CAL_RESULT_TO_FLASH
void write_cal_result_to_flash(void)
{
	UINT32 param;
	UINT32 param1;
	char cTemp[sizeof(BK7231N_RC_RAM) + sizeof(BK7231N_TRX_RAM)];
	char cTemp1[sizeof(BK7231N_RC_RAM) + sizeof(BK7231N_TRX_RAM)];
	
	CAL_FLASH_PRT("write cal result to flash\r\n");

//	flash_ctrl(CMD_FLASH_READ_SR, &param);
//	CAL_FLASH_PRT("param = 0x%x\r\n", param);
	#if CFG_SUPPORT_ALIOS
	hal_flash_dis_secure(0, 0, 0);
	#else
	bk_flash_enable_security(NONE);
	#endif
//	CAL_FLASH_PRT("param1 = 0x%x\r\n", param1);
	
//	param = CAL_RESULT_FLASH_ADDR;
//	flash_ctrl(CMD_FLASH_ERASE_SECTOR, &param);
	flash_erase_sector(CAL_RESULT_FLASH_ADDR);

	memcpy(cTemp, BK7231N_RC_RAM, sizeof(BK7231N_RC_RAM));
	memcpy(cTemp+sizeof(BK7231N_RC_RAM), BK7231N_TRX_RAM, sizeof(BK7231N_TRX_RAM));
//	memset(cTemp, 0, sizeof(cTemp));
	flash_write(cTemp, sizeof(cTemp), CAL_RESULT_FLASH_ADDR);
	flash_read(cTemp1, sizeof(cTemp1), CAL_RESULT_FLASH_ADDR);
	if (memcmp(cTemp, cTemp1,  sizeof(cTemp1)) == 0)
	{
		CAL_FLASH_PRT("memcmp OK\r\n");
	}
	else
	{
		CAL_FLASH_PRT("memcmp fail\r\n");
	}
	
//	param = 2 | ((param&0x00FFFF)<<8);
	#if CFG_SUPPORT_ALIOS
	hal_flash_enable_secure(0, 0, 0);
	#else
	bk_flash_enable_security(FLASH_UNPROTECT_LAST_BLOCK);
	#endif
//	param = 0;
//	flash_ctrl(CMD_FLASH_READ_SR, &param);
//	CAL_FLASH_PRT("param = 0x%x\r\n", param);
	
	CAL_FLASH_PRT("write cal result to flash OK\r\n");
}
#endif

char read_cal_result_from_flash(void)
{
#if CAL_RESULT_TO_FLASH
	char cTemp[sizeof(BK7231N_RC_RAM) + sizeof(BK7231N_TRX_RAM)];

	CAL_FLASH_PRT("read_cal_result_from_flash\r\n");
	memset(cTemp, 0, sizeof(cTemp));
	flash_read(cTemp, sizeof(cTemp), CAL_RESULT_FLASH_ADDR);
	if ((cTemp[0] == 0xFF) && (cTemp[4] == 0xFF) && (cTemp[8] == 0xFF) && (cTemp[12] == 0xFF))
	{
		return 0;
	}
	else
	{
		memcpy(BK7231N_RC_RAM, cTemp, sizeof(BK7231N_RC_RAM));
		memcpy(BK7231N_TRX_RAM, cTemp+sizeof(BK7231N_RC_RAM), sizeof(BK7231N_TRX_RAM));
		rwnx_cal_load_default_result();
		rwnx_cal_load_trx_rcbekn_reg_val();
//		printf_trx_rc_value();
		CAL_FLASH_PRT("read cal result from flash OK\r\n");
		return 1;
	}
#else
	return 0;
#endif
}

#if CAL_RESULT_TO_FLASH
void flash_test(void)
{
	UINT32 param;
	char cTemp[0x1000];
	char cTemp1[0x1000];
	int i;

	flash_read(cTemp, 0x1000, 0xF4000);
	CAL_FLASH_PRT("cTemp:\r\n");
	for (i=0; i<0x1000; i++)
	{
	    CAL_FLASH_PRT("%x ", cTemp[i]);
	}
	CAL_FLASH_PRT("\r\n");

	#if CFG_SUPPORT_ALIOS
	hal_flash_dis_secure(0, 0, 0);
	#else
	bk_flash_enable_security(FLASH_PROTECT_NONE);
	#endif
	param = 0xF4000;
	flash_ctrl(CMD_FLASH_ERASE_SECTOR, &param);

	flash_read(cTemp, 0x1000, 0xF4000);
	CAL_FLASH_PRT("cTemp:\r\n");
	for (i=0; i<0x1000; i++)
	{
	    CAL_FLASH_PRT("%x ", cTemp[i]);
	}
	CAL_FLASH_PRT("\r\n");

	for (i=0; i<0x1000; i++)
	{
	    cTemp[i] = i;
	}
	CAL_FLASH_PRT("cTemp:\r\n");
	for (i=0; i<0x1000; i++)
	{
	    CAL_FLASH_PRT("%x ", cTemp[i]);
	}
	CAL_FLASH_PRT("\r\n");

	flash_write(cTemp, 0x1000, 0xF4000);

	memset(cTemp1, 0, 0x1000);
	CAL_FLASH_PRT("cTemp1:\r\n");
	for (i=0; i<0x1000; i++)
	{
	    CAL_FLASH_PRT("%x ", cTemp1[i]);
	}
	CAL_FLASH_PRT("\r\n");
	flash_read(cTemp1, 0x1000, 0xF4000);

	CAL_FLASH_PRT("cTemp1:\r\n");
	for (i=0; i<0x1000; i++)
	{
	    CAL_FLASH_PRT("%x ", cTemp1[i]);
	}

	CAL_FLASH_PRT("\r\n");
	if (memcmp(cTemp, cTemp1, sizeof(cTemp)) == 0)
	{
		CAL_FLASH_PRT("memcmp OK\r\n");
	}
	else
	{
		CAL_FLASH_PRT("memcmp ERROR\r\n");
	}
}
#endif

void load_reg (void)
{
    BK7231N_RC_REG.REG0x0->value  = BK7231N_RC_RAM.REG0x0.value;
    BK7231N_RC_REG.REG0x1->value  = BK7231N_RC_RAM.REG0x1.value | 0xFFFFFFF;
    BK7231N_RC_REG.REG0x5->value  = BK7231N_RC_RAM.REG0x5.value;
    BK7231N_RC_REG.REG0x8->value  = BK7231N_RC_RAM.REG0x8.value;
    BK7231N_RC_REG.REG0xB->value  = BK7231N_RC_RAM.REG0xB.value;
    BK7231N_RC_REG.REG0xE->value  = BK7231N_RC_RAM.REG0xE.value;
    BK7231N_RC_REG.REG0x11->value = BK7231N_RC_RAM.REG0x11.value;
    BK7231N_RC_REG.REG0x19->value = BK7231N_RC_RAM.REG0x19.value;
    BK7231N_RC_REG.REG0x1C->value = BK7231N_RC_RAM.REG0x1C.value;
    BK7231N_RC_REG.REG0x1E->value = BK7231N_RC_RAM.REG0x1E.value;

    /**********NEW ADDED************/
    BK7231N_RC_REG.REG0x3C->value = BK7231N_RC_RAM.REG0x3C.value;
    BK7231N_RC_REG.REG0x3E->value = BK7231N_RC_RAM.REG0x3E.value;
    BK7231N_RC_REG.REG0x3F->value = BK7231N_RC_RAM.REG0x3F.value;
    BK7231N_RC_REG.REG0x40->value = BK7231N_RC_RAM.REG0x40.value;
    BK7231N_RC_REG.REG0x41->value = BK7231N_RC_RAM.REG0x41.value;
    BK7231N_RC_REG.REG0x42->value = BK7231N_RC_RAM.REG0x42.value;
    BK7231N_RC_REG.REG0x4C->value = BK7231N_RC_RAM.REG0x4C.value;
    BK7231N_RC_REG.REG0x4D->value = BK7231N_RC_RAM.REG0x4D.value;
    BK7231N_RC_REG.REG0x4F->value = BK7231N_RC_RAM.REG0x4F.value;
    BK7231N_RC_REG.REG0x50->value = BK7231N_RC_RAM.REG0x50.value;
    BK7231N_RC_REG.REG0x51->value = BK7231N_RC_RAM.REG0x51.value;
    BK7231N_RC_REG.REG0x52->value = BK7231N_RC_RAM.REG0x52.value;
    BK7231N_RC_REG.REG0x54->value = BK7231N_RC_RAM.REG0x54.value;
    BK7231N_RC_REG.REG0x55->value = BK7231N_RC_RAM.REG0x55.value;
    BK7231N_RC_REG.REG0x5C->value = BK7231N_RC_RAM.REG0x5C.value;

    BK7231N_RC_REG.REG0x4E->value = BK7231N_RC_RAM.REG0x4E.value;
    BK7231N_RC_REG.REG0x5A->value = BK7231N_RC_RAM.REG0x5A.value;
    BK7231N_RC_REG.REG0x5B->value = BK7231N_RC_RAM.REG0x5B.value;
    BK7231N_RC_REG.REG0x6A->value = BK7231N_RC_RAM.REG0x6A.value;

    while(BK7231N_RC_REG.REG0x1->value & 0x0FFFFFFF)
    {
        cpu_delay(1);
    }

    BK7231N_TRX_REG.REG0x0->value = BK7231N_TRX_RAM.REG0x0.value;
    BK7231N_TRX_REG.REG0x1->value = BK7231N_TRX_RAM.REG0x1.value;
    BK7231N_TRX_REG.REG0x2->value = BK7231N_TRX_RAM.REG0x2.value;
    BK7231N_TRX_REG.REG0x3->value = BK7231N_TRX_RAM.REG0x3.value;
    BK7231N_TRX_REG.REG0x4->value = BK7231N_TRX_RAM.REG0x4.value;
    BK7231N_TRX_REG.REG0x5->value = BK7231N_TRX_RAM.REG0x5.value;
    BK7231N_TRX_REG.REG0x6->value = BK7231N_TRX_RAM.REG0x6.value;
    BK7231N_TRX_REG.REG0x7->value = BK7231N_TRX_RAM.REG0x7.value;
    BK7231N_TRX_REG.REG0x8->value = BK7231N_TRX_RAM.REG0x8.value;
    BK7231N_TRX_REG.REG0x9->value = BK7231N_TRX_RAM.REG0x9.value;
    BK7231N_TRX_REG.REG0xA->value = BK7231N_TRX_RAM.REG0xA.value;
    BK7231N_TRX_REG.REG0xB->value = BK7231N_TRX_RAM.REG0xB.value;
    BK7231N_TRX_REG.REG0xC->value = BK7231N_TRX_RAM.REG0xC.value;
    BK7231N_TRX_REG.REG0xD->value = BK7231N_TRX_RAM.REG0xD.value;
    BK7231N_TRX_REG.REG0xE->value = BK7231N_TRX_RAM.REG0xE.value;
    BK7231N_TRX_REG.REG0xF->value = BK7231N_TRX_RAM.REG0xF.value;
    BK7231N_TRX_REG.REG0x10->value = BK7231N_TRX_RAM.REG0x10.value;
    BK7231N_TRX_REG.REG0x11->value = BK7231N_TRX_RAM.REG0x11.value;
    BK7231N_TRX_REG.REG0x12->value = BK7231N_TRX_RAM.REG0x12.value;
    BK7231N_TRX_REG.REG0x13->value = BK7231N_TRX_RAM.REG0x13.value;
    BK7231N_TRX_REG.REG0x14->value = BK7231N_TRX_RAM.REG0x14.value;
    BK7231N_TRX_REG.REG0x15->value = BK7231N_TRX_RAM.REG0x15.value;
    BK7231N_TRX_REG.REG0x16->value = BK7231N_TRX_RAM.REG0x16.value;
    BK7231N_TRX_REG.REG0x17->value = BK7231N_TRX_RAM.REG0x17.value;
    BK7231N_TRX_REG.REG0x18->value = BK7231N_TRX_RAM.REG0x18.value;
    BK7231N_TRX_REG.REG0x19->value = BK7231N_TRX_RAM.REG0x19.value;
    BK7231N_TRX_REG.REG0x1A->value = BK7231N_TRX_RAM.REG0x1A.value;
    BK7231N_TRX_REG.REG0x1B->value = BK7231N_TRX_RAM.REG0x1B.value;

    while(BK7231N_RC_REG.REG0x1->value & 0x0FFFFFFF)
    {
        cpu_delay(1);
    }
    return ;
}

void sctrl_dpll_int_open(void);
void calibration_main(void)
{
    INT32 goldval[32] = {0};

	if (CALI_MODE_UNKNOWN == gcali_context.cali_mode)
	{
		bk7011_get_rfcali_mode();
	}
#if DIFFERENCE_PIECES_CFG
	bk7011_get_txpwr_config_reg();
#endif

    REG_WRITE((RCB_POWER_TABLE_ADDR + (0x34 * 4)), 0xE0B318E0);//E98B7150

    /* remove this workaround after using 26MHz as saradc source */
#if 0
    analog1 = sctrl_ctrl(CMD_SCTRL_GET_ANALOG1, NULL);
    analog1 |= (DCO_AMSEL_BIT);
    sctrl_ctrl(CMD_SCTRL_SET_ANALOG1, (void *)&analog1);
#endif

    memcpy(&BK7231N_TRX_RAM, BK7231N_TRX_ROM, sizeof(BK7231N_TRX_RAM));
    memcpy(&BK7231N_RC_RAM, BK7231N_RC_ROM, sizeof(BK7231N_RC_RAM));

    //REG_WRITE((0x0802800 + (18 * 4)), 0x02);
    BK7231N_RC_REG.REG0x4C->bits.TXCOMPDIS = 0;    
    bk7011_cal_ready();
    bk7011_cal_bias();

    //bk7011_doubler_cal();
    //bk7011_band_cal();

    //bk7011_cal_pll();
    //bk7011_rfpll_ld();

    bk7011_cal_dpll();
    sctrl_dpll_int_open();

    BK7231N_TRX_RAM.REG0xA.value = TRX_REG_0XA_VAL;
    CAL_WR_TRXREGS(0xA);
    BK7231N_TRX_RAM.REG0xB.value = TRX_REG_0XB_VAL;
    CAL_WR_TRXREGS(0xB);
    BK7231N_TRX_RAM.REG0xC.value = TRX_REG_0XC_VAL;
    CAL_WR_TRXREGS(0xC);
    BK7231N_RC_REG.REG0x3C->bits.RXHPFBYPASS = 1;// Disable HPF for calibration 	
    

    if (read_cal_result_from_flash() == 1)
    {
    	return;
    }
	

//    BK7231N_TRX_RAM.REG0x7->bits.chin60 = 0x00;//2400MHz  descrease the external interference
//    CAL_WR_TRXREGS(0x7);
    BK7231N_TRX_RAM.REG0x5.bits.chspi = 0x00;//2400MHz  descrease the external interference
    CAL_WR_TRXREGS(0x5);
    delay100us(1);//delay 100us for RFPLL
	
    bk7011_tx_cal_en();
    bk7011_cal_bias();

    *goldval = bk7011_cal_tx_dc_new(TX_DC_CAL);
   // *goldval = bk7011_cal_tx_dc_new(TX_DC_CAL_IQ);
    //*goldval = bk7011_cal_tx_filter_corner();    
    
    *goldval = bk7011_cal_tx_output_power(TX_IQ_POWER_CAL);  // First cal just to enable BK7231 TX work for temp. balance
    
    //*goldval = bk7011_cal_tx_dc_new(TX_DC_CAL_IQ);

    *goldval = bk7011_cal_tx_gain_imbalance(TX_GAIN_IMB_CAL);
	
    *goldval = bk7011_cal_tx_phase_imbalance(TX_PHASE_IMB_CAL);

    bk7011_cal_tx_filter_corner();
//    BK7231N_TRX_RAM.REG0xB->value = TRX_REG_0XB_VAL;

//    BK7231N_TRX_RAM.REG0x7->bits.chin60 = 0x5d;//2495MHz
//    CAL_WR_TRXREGS(0x7);
    BK7231N_TRX_RAM.REG0x5.bits.chspi = 0x5d;//2495MHz
    CAL_WR_TRXREGS(0x5);
    delay100us(1);//delay 100us for RFPLL
    
    
    bk7011_cal_bias();

    if(bk7011_is_rfcali_mode_auto())
    {
        bk7011_cal_auto_tx_power();
    }

//    BK7231N_TRX_RAM.REG0x7->bits.chin60 = 0xc;//fix to this channel for power cal.
//    CAL_WR_TRXREGS(0x7);
    BK7231N_TRX_RAM.REG0x5.bits.chspi = 0xc;//fix to this channel for power cal.
    CAL_WR_TRXREGS(0x5);
    delay100us(1);//delay 100us for RFPLL	

    *goldval = bk7011_cal_tx_output_power(TX_WANTED_POWER_CAL);  // Actual Power cal.

    bk7011_set_tx_after_cal();
    rwnx_tx_cal_save_cal_result();//by yiming 20170315

//    BK7231N_TRX_RAM.REG0x7->bits.chin60 = 0x5d;//2495MHz
//    CAL_WR_TRXREGS(0x7);
    BK7231N_TRX_RAM.REG0x5.bits.chspi = 0x5d;//2495MHz
    CAL_WR_TRXREGS(0x5);
    delay100us(1);//delay 100us for RFPLL	

    bk7011_rx_cal_en();
    bk7011_cal_rx_adc_dlym();//add by desheng
    bk7011_cal_rx_dc();

    bk7011_tx_cal_en();

    BK7231N_TRX_RAM.REG0x5.bits.chspi = 0x5d;//2495MHz
    CAL_WR_TRXREGS(0x5);
    delay100us(1);//delay 100us for RFPLL

    
  //20170804 by yiming 
    //*goldval = bk7011_cal_tx_output_power(TX_IQ_LOOPBACK_POWER_CAL);
    bk7011_cal_bias();
    bk7011_cal_pll();
    delay100us(1);//delay 100us for RFPLL
    
    //*goldval = bk7011_cal_tx_output_power(TX_IQ_LOOPBACK_POWER_CAL);      

   // *goldval = bk7011_cal_tx_dc_new(TX_DC_LOOPBACK_CAL_IQ);//Loopback DC cal.   

    //*goldval = bk7011_cal_tx_gain_imbalance(TX_GAIN_LOOPBACK_IMB_CAL);

    //*goldval = bk7011_cal_tx_phase_imbalance(TX_PHASE_LOOPBACK_IMB_CAL);

    //bk7011_rx_cal_en();
    //bk7011_cal_rx_iq();


    bk7011_load_calibration_cfg();
    bk7011_set_rx_after_cal();
    //REG_WRITE((0x0802800 + (18 * 4)), 0x00);

    //rwnx_rx_cal_save_cal_result();
    rwnx_cal_load_default_result();
    //rwnx_cal_read_current_cal_result();
    BK7231N_RC_REG.REG0x4C->bits.TXCOMPDIS = 0;
	
    rwnx_tx_cal_save_cal_result();//by yiming 20170315
    rwnx_cal_load_trx_rcbekn_reg_val();

#if CALI_DPD_TEST
    bk7211_cal_tx_dpd_load();
#endif
    bk7011_cal_dpll();
    sctrl_dpll_int_open();
/*	
    cpu_delay(100);
    *goldval = bk7011_cal_tx_output_power(TX_WANTED_POWER_CAL);  // 
*/    
#if CAL_RESULT_TO_FLASH
    write_cal_result_to_flash();
//    printf_trx_rc_value();
#endif

    bk7011_cal_saradc_close(cali_saradc_desc);

    bk_printf("calibration_main over\n");


    BK7231N_TRX_RAM.REG0x0.bits.tssi_atten = 0x2;
    CAL_WR_TRXREGS(0x0);
    BK7231N_TRX_RAM.REG0xA.bits.Dvncref=0x3;
    CAL_WR_TRXREGS(0xA);

#if 0
    analog1 = sctrl_ctrl(CMD_SCTRL_GET_ANALOG1, NULL);
    analog1 &= ~(DCO_AMSEL_BIT);
    sctrl_ctrl(CMD_SCTRL_SET_ANALOG1, (void *)&analog1);
#endif

    return ;
}

void do_calibration_in_temp_dect(void)
{
    INT32 goldval[32] = {0};

//    BK7231N_TRX_RAM.REG0x7->bits.chin60 = 0xc;//fix to this channel for power cal.
//    CAL_WR_TRXREGS(0x7);
    BK7231N_TRX_RAM.REG0x5.bits.chspi = 0xc;//fix to this channel for power cal.
    CAL_WR_TRXREGS(0x5);
    delay100us(1);//delay 100us for RFPLL

    bk7011_read_cal_param();
    rwnx_cal_load_default_result();
    cpu_delay(10);//cpu_delay(10);  20160801

    bk7011_cal_bias();

    BK7231N_RC_REG.REG0x4C->bits.TXCOMPDIS = 0;

    bk7011_tx_cal_en();
    //gtxoutput = 1;
    //*goldval = bk7011_cal_tx_output_power(gtx_power_cal_mode);  // first time

    *goldval = bk7011_cal_tx_output_power(TX_WANTED_POWER_CAL);  // second time

    bk7011_set_tx_after_cal();
    //REG_WRITE((0x0802800 + (19 * 4)), 0x00);

    rwnx_tx_cal_save_cal_result();
    rwnx_cal_load_default_result();
    //rwnx_cal_read_current_cal_result();
    BK7231N_RC_REG.REG0x4C->bits.TXCOMPDIS = 0;      

    bk7011_cal_dpll();
    sctrl_dpll_int_open();

    return ;
}

UINT32 dgainpga = 0;
void turnoff_PA_in_temp_dect(void)
{
    CHECK_OPERATE_RF_REG_IF_IN_SLEEP();
    //dgainpga = BK7231N_TRX_REG.REG0xC->bits.dgainpga;
    //BK7231N_TRX_RAM.REG0xC->bits.dgainpga = 0;

    //CAL_WR_TRXREGS(0xC);    
    CHECK_OPERATE_RF_REG_IF_IN_SLEEP_END();
}

void turnon_PA_in_temp_dect(void)
{
    CHECK_OPERATE_RF_REG_IF_IN_SLEEP();
    //BK7231N_TRX_RAM.REG0xC.bits.dgainpga = dgainpga;
    //CAL_WR_TRXREGS(0xC);   
    CHECK_OPERATE_RF_REG_IF_IN_SLEEP_END();
}

#define MPB_ADDR_BASE  (0x01060000)
#define SCTRL_BASE     (0x00800000)
#define LA_ADDR        (0x00808000)
void bk7011_la_sample_print(UINT32 isrx)
{
 	UINT32 reg_val, i, len;
    UINT8 *buf;

    // please do tx / rx  before call the function
    // tx, use txevm 
    // rx, use rxsens

    #define LA_SAMPLE_BUF_LEN           (96 *1024)
    buf = os_malloc(LA_SAMPLE_BUF_LEN);
    if(!buf) {
        os_printf("la_sample_print no buffer\r\n");
        return;
    }
    len = LA_SAMPLE_BUF_LEN / 4;

    REG_WRITE((LA_ADDR + 0x4*4), (UINT32)buf); 

    if(1 == isrx) 
    {
        //RX ADC Data
        REG_WRITE((SCTRL_BASE + 0xd*4), 0x00040000);
        REG_WRITE((LA_ADDR + 0x2*4), 0xfe000000);
        REG_WRITE((LA_ADDR + 0x1*4), 0x0a000000);
    }
    else if(2 == isrx) 
    {
        //RX DAC Data
        REG_WRITE((SCTRL_BASE + 0xd*4), 0x00040000);
        REG_WRITE((LA_ADDR + 0x2*4), 0xfe000000);
        REG_WRITE((LA_ADDR + 0x1*4), 0x0c000000);
    }
    else 
    {
        //TX DAC Data
        REG_WRITE((SCTRL_BASE + 0xd*4), 0x00030000);
        REG_WRITE((LA_ADDR + 0x2*4), 0x00700000);
        REG_WRITE((LA_ADDR + 0x1*4), 0x00600000);

        BK7231N_TRX_RAM.REG0xF.bits.clkdac_inv = 0;
        CAL_WR_TRXREGS(0xC);
    }

    reg_val = 0x15;
    reg_val |= (len << 12);
    REG_WRITE((LA_ADDR + 0x0*4), reg_val); 
   
	do 
    {
         reg_val = REG_READ((LA_ADDR + 0x0*4));
         os_printf("abc:%x\r\n",reg_val&0x8);
    } while((reg_val & 0x8) != 0x8);
    
	delay100us(100);	
    
    reg_val = REG_READ((LA_ADDR + 0x0*4));
    reg_val = (reg_val & (~0x3)) | 0x00;
    REG_WRITE((LA_ADDR + 0x0*4), reg_val); 
	
	for(i = 0; i < len; i ++)
	{
		os_printf("%08x\r\n", *((uint32_t *)(buf+i*4)));
	}

    os_free(buf);
}

void bk7011_max_rxsens_setting(void)
{    
	BK7231N_TRX_RAM.REG0x8.bits.isrxref10 = 3;    
	BK7231N_TRX_RAM.REG0x8.bits.isrxlna30 = 15;
	CAL_WR_TRXREGS(0x8);    
}

void bk7011_normal_rxsens_setting(void)
{    
	BK7231N_TRX_RAM.REG0x8.bits.isrxref10 = 2;    
	BK7231N_TRX_RAM.REG0x8.bits.isrxlna30 = 7;
	CAL_WR_TRXREGS(0x8);    
}

void bk7011_default_rxsens_setting(void)
{    
	BK7231N_TRX_RAM.REG0x8.bits.isrxref10 = 2;    
	BK7231N_TRX_RAM.REG0x8.bits.isrxlna30 = 7;    
	CAL_WR_TRXREGS(0x8);    
}

UINT8 gtx_dcorMod_tab[14] = {0};
void bk7011_cal_dcormod_show(void)
{
    os_printf("\r\n dcormod tab:\r\n");
    for(int i=0; i<14; i++) {
        os_printf("ch:%2d: mod:%02x\r\n", i+1, gtx_dcorMod_tab[i]);
    }
}

static void bk7011_cal_dcormod_save_base(INT32 mod)
{
    UINT32 channel = BK7231N_TRX_REG.REG0x5->bits.chspi;
    
    if(channel < 12)
        channel = 1;
    else
        channel = (channel - 7)/5;
    
    if(channel > 14)
        channel = 14;
    
    if((channel == 1) || (channel == 7) || (channel == 13))
    {
        if((mod >= 0) && (mod <= 15))
        {
            gtx_dcorMod_tab[channel-1] = mod;
        }
        else
        {
            os_printf("save base failed:ch:%d, mod:%x\r\n", channel, mod);
        }
    }
}

static void bk7011_cal_dcormod_do_fitting(void)
{
    UINT8 mod_ch;
    // ch4, use ch1 & ch7
    mod_ch = gtx_dcorMod_tab[0] + gtx_dcorMod_tab[6];
    gtx_dcorMod_tab[3] = mod_ch >> 1;

    // ch10, use ch7 & ch13
    mod_ch = gtx_dcorMod_tab[6] + gtx_dcorMod_tab[12];
    gtx_dcorMod_tab[9] = mod_ch >> 1;

    mod_ch = gtx_dcorMod_tab[0];
    for(int i=1; i<14; i++)
    {
        // ch 1 4 7 10 13
        if((i == 0) || (i == 3) || (i == 6) || (i == 9) || (i == 12))
            mod_ch = gtx_dcorMod_tab[i];
        else
            gtx_dcorMod_tab[i] = mod_ch;
    }
}

static UINT8 bk7011_cal_dcormod_get(void)
{
    if(bk7011_is_rfcali_mode_auto())
    {
        UINT32 channel = BK7231N_TRX_REG.REG0x5->bits.chspi;
        
        if(channel < 12)
            channel = 1;
        else
            channel = (channel - 7)/5;
        
        if(channel > 14)
            channel = 14;

        return gtx_dcorMod_tab[channel-1];
    }
    else
        return gtx_dcorMod;
}

#include "str_pub.h"
static int rfcali_cfg_tssi_b(int argc, char **argv)
{
    int tssi_thred_b;

    if(argc != 2)
    {
        os_printf("rfcali_cfg_tssi 0-255(for b)\r\n");
        return 0;
    }
    
    tssi_thred_b = os_strtoul(argv[1], NULL, 10);

    os_printf("cmd set tssi b_thred:%d\r\n", tssi_thred_b);

    bk7011_set_rf_config_tssithred_b(tssi_thred_b);
    return 0; 
}

static int rfcali_cfg_tssi_g(int argc, char **argv)
{
    int tssi_thred_g;

    if(argc != 2)
    {
        os_printf("rfcali_cfg_tssi 0-255(for b)\r\n");
        return 0;
    }
    
    tssi_thred_g = os_strtoul(argv[1], NULL, 10);

    os_printf("cmd set tssi g_thred:%d\r\n", tssi_thred_g);

    bk7011_set_rf_config_tssithred_g(tssi_thred_g);
    return 0; 
}

static int rfcali_cfg_rate_dist(int argc, char **argv)
{
    int dist_b, dist_g, dist_n40, dist_ble;

    if(argc != 5)
    {
        os_printf("rfcali_cfg_rate_dist b g n40 ble (0-31)\r\n");
        return 0;
    }
    
    dist_b = os_strtoul(argv[1], NULL, 10);
    dist_g = os_strtoul(argv[2], NULL, 10);
    dist_n40 = os_strtoul(argv[3], NULL, 10);
    dist_ble = os_strtoul(argv[4], NULL, 10);

    if((dist_b > 31) || (dist_g > 31) || (dist_n40 > 31) || (dist_ble > 31))
    {
        os_printf("rate_dist range:-31 - 31\r\n");
        return 0;
    }

    if((dist_b < -31) || (dist_g < -31) || (dist_n40 < -31) || (dist_ble < -31))
    {
        os_printf("rate_dist range:-31 - 31\r\n");
        return 0;
    }
    
    //manual_cal_set_rate_dist_for_txpwr(dist_b, dist_g, dist_n40, dist_ble);
    
    return 0; 
}

static int rfcali_cfg_mode(int argc, char **argv)
{
    int rfcali_mode = 0;

    if(argc != 2)
    {
        os_printf("rfcali_mode 0/1\r\n");
        return 0;
    }
    
    rfcali_mode = os_strtoul(argv[1], NULL, 10);

    if((rfcali_mode != CALI_MODE_AUTO) && (rfcali_mode != CALI_MODE_MANUAL))
    {
        os_printf("rfcali_mode 0/1, %d\r\n", rfcali_mode);
        return 0;
    }

    bk7011_set_rfcali_mode(rfcali_mode, 1);
    
    return 0; 
}

static int rfcali_show_data(int argc, char **argv)
{
    manual_cal_show_txpwr_tab();
    
    return 0; 
}

#if CFG_SUPPORT_RTT
FINSH_FUNCTION_EXPORT_ALIAS(rfcali_cfg_tssi_b, __cmd_rfcali_cfg_tssi_b, rfcali cfg tssi);
FINSH_FUNCTION_EXPORT_ALIAS(rfcali_cfg_mode, __cmd_rfcali_cfg_mode, rfcali cfg mode);
FINSH_FUNCTION_EXPORT_ALIAS(rfcali_cfg_rate_dist, __cmd_rfcali_cfg_rate_dist, rfcali cfg rate_dist);
FINSH_FUNCTION_EXPORT_ALIAS(rfcali_cfg_tssi_g, __cmd_rfcali_cfg_tssi_g, rfcali cfg tssi);
FINSH_FUNCTION_EXPORT_ALIAS(rfcali_show_data, __cmd_rfcali_show_data, rfcali show data);
#else
void cmd_rfcali_cfg_mode(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    rfcali_cfg_mode(argc, argv);
}

void cmd_rfcali_cfg_rate_dist(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    rfcali_cfg_rate_dist(argc, argv);
}

void cmd_rfcali_cfg_tssi_g(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    rfcali_cfg_tssi_g(argc, argv);
}

void cmd_rfcali_cfg_tssi_b(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    rfcali_cfg_tssi_b(argc, argv);
}

void cmd_rfcali_show_data(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    rfcali_show_data(argc, argv);
}
#endif

#else  /* CFG_SUPPORT_CALIBRATION */
/*INT32 rwnx_cal_load_trx_rcbekn_reg_val(void)
{
}
INT32 rwnx_cal_save_trx_rcbekn_reg_val(void)
{
}
void rwnx_cal_set_txpwr_by_rate(INT32 rate)
{
}
void calibration_main(void)
{
}
void calibration_auto_test(void)
{
}*/
#endif  /* CFG_SUPPORT_CALIBRATION */

#include "sys_ctrl.h"
void bk_dump_regs()
{
    bk_printf("0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n",
    BK7231N_RC_REG.REG0x0->value,
    BK7231N_RC_REG.REG0x1->value,
    BK7231N_RC_REG.REG0x5->value,
    BK7231N_RC_REG.REG0x8->value,
    BK7231N_RC_REG.REG0xB->value,
    BK7231N_RC_REG.REG0xE->value,
    BK7231N_RC_REG.REG0x11->value,
    BK7231N_RC_REG.REG0x19->value);
    bk_printf("0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n",
    BK7231N_RC_REG.REG0x1C->value,
    BK7231N_RC_REG.REG0x1E->value,
    BK7231N_RC_REG.REG0x3C->value,
    BK7231N_RC_REG.REG0x3E->value,
    BK7231N_RC_REG.REG0x3F->value,
    BK7231N_RC_REG.REG0x40->value,
    BK7231N_RC_REG.REG0x41->value,
    BK7231N_RC_REG.REG0x42->value);
    bk_printf("0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n",
    BK7231N_RC_REG.REG0x4C->value,
    BK7231N_RC_REG.REG0x4D->value,
    BK7231N_RC_REG.REG0x4F->value,
    BK7231N_RC_REG.REG0x50->value,
    BK7231N_RC_REG.REG0x51->value,
    BK7231N_RC_REG.REG0x52->value,
    BK7231N_RC_REG.REG0x54->value,
    BK7231N_RC_REG.REG0x55->value);
    bk_printf("0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n",
    BK7231N_RC_REG.REG0x5C->value,
    BK7231N_RC_REG.REG0x4E->value,
    BK7231N_RC_REG.REG0x5A->value,
    BK7231N_RC_REG.REG0x5B->value,
    BK7231N_RC_REG.REG0x6A->value, 0, 0, 0);

    bk_printf("0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n",
        BK7231N_TRX_REG.REG0x0->value,
        BK7231N_TRX_REG.REG0x1->value,
        BK7231N_TRX_REG.REG0x2->value,
        BK7231N_TRX_REG.REG0x3->value,
        BK7231N_TRX_REG.REG0x4->value,
        BK7231N_TRX_REG.REG0x5->value,
        BK7231N_TRX_REG.REG0x6->value,
        BK7231N_TRX_REG.REG0x7->value);
    bk_printf("0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n",
        BK7231N_TRX_REG.REG0x8->value,
        BK7231N_TRX_REG.REG0x9->value,
        BK7231N_TRX_REG.REG0xA->value,
        BK7231N_TRX_REG.REG0xB->value,
        BK7231N_TRX_REG.REG0xC->value,
        BK7231N_TRX_REG.REG0xD->value,
        BK7231N_TRX_REG.REG0xE->value,
        BK7231N_TRX_REG.REG0xF->value);
    bk_printf("0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n",
        BK7231N_TRX_REG.REG0x10->value,
        BK7231N_TRX_REG.REG0x11->value,
        BK7231N_TRX_REG.REG0x12->value,
        BK7231N_TRX_REG.REG0x13->value,
        BK7231N_TRX_REG.REG0x14->value,
        BK7231N_TRX_REG.REG0x15->value,
        BK7231N_TRX_REG.REG0x16->value,
        BK7231N_TRX_REG.REG0x17->value);
    bk_printf("0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n",
        BK7231N_TRX_REG.REG0x18->value,
        BK7231N_TRX_REG.REG0x19->value,
        BK7231N_TRX_REG.REG0x1A->value,
        BK7231N_TRX_REG.REG0x1B->value, 0, 0, 0, 0);

    bk_printf("0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n",
        sctrl_analog_get(SCTRL_ANALOG_CTRL0),
        sctrl_analog_get(SCTRL_ANALOG_CTRL1),
        sctrl_analog_get(SCTRL_ANALOG_CTRL2),
        sctrl_analog_get(SCTRL_ANALOG_CTRL3),
        sctrl_analog_get(SCTRL_ANALOG_CTRL4),
        sctrl_analog_get(SCTRL_ANALOG_CTRL5),
        sctrl_analog_get(SCTRL_ANALOG_CTRL6),
        0);
}

#endif //  (CFG_SOC_NAME != SOC_BK7231)
// eof

