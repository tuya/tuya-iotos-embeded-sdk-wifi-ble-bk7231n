#ifndef _BK7231U_CAL_H_
#define _BK7231U_CAL_H_

#include "uart_pub.h"

#define CAL_DEBUG          1
#if CAL_DEBUG
#define CAL_PRT       null_prf
#define CAL_WARN      null_prf// warning_prf
#define CAL_FATAL     null_prf
#define CAL_TIM_PRT   null_prf
#define CAL_FLASH_PRT os_printf
#else
#define CAL_PRT       null_prf
#define CAL_WARN      null_prf
#define CAL_FATAL     null_prf
#define CAL_TIM_PRT   null_prf
#define CAL_FLASH_PRT null_prf
#endif

#if (CFG_SOC_NAME != SOC_BK7231)

#define INCLUDE_OS

//#define SCAN_IQ_FILTER_CORNER

////Difference between pieces
#define DIFFERENCE_PIECES_CFG        0

//#define _11MBPS_MAX_POWER

void delay05us(INT32 num);

#define trx_reg_is_write(st_trxreg)     while(BK7231N_RC_REG.REG0x1->value & st_trxreg) 	{cpu_delay(1);}

#define DETECT_LOOPCNT		        10
#define GOLD_OUTPUT_POWER		    56
#define UNSIGNEDOFFSET10		    0x800
#define I_Q_CAP_DIF					32
#define CONSTANT_RCIQ				117
#define SUMNUMBERS					1
#define MINOFFSET			    	64//16

//#define DPDCALILEN				  256

#define cpu_delay(val)            delay(MAX(1, val/100))
#define DELAY1US				  100
//#define DELAY05US				  1

#define cal_delay(val)            delay05us(MAX(1, val))	// 8us
//#define CAL_DELAY1US			  2
//#define CAL_DELAY05US			  20 //20160804  1:0.5us 2:1us
#define CAL_DELAY05US			  2		// 20170503 2:1.5us     2 to 20  for debug 20180227
#define CAL_TX_NUM                50
#define CAL_RX_NUM                5

#define cal_delay_100us(val)      delay100us(MAX(1, val))	// 200us
#define CAL_DELAY100US			  1  //20160804  1:100us 2:200us		// 20170503 1:150us 2:300us


//#define BK7011TRXREG0xD 		  0xDDFF0339
//#define BK7011TRXREG0xC		      0x01A147EE//0x01A183FD
/*
#define DGAINPA20 				  3
#define DGAINBUF20				  3
#define GCTRLPGA40				  0xf
#define GCTRLMOD30        		  0x04
#define TSSI_DELTA				 (2)  // 10
#define TSSI_IS_VALID(val)	  (((val)  0xf0 ) && ((val) > 0x20))?1:0)
#define TSSI_IS_TOO_LOW(val)  (((val)<(0x00 * SUMNUMBERS))?1:0)  //0x37
#define TSSI_IS_TOO_HIGH(val) (((val)> (0xff * SUMNUMBERS))?1:0) //0xe0
*/
#define st_TRXREG00			(1<<0)
#define st_TRXREG01			(1<<1)
#define st_TRXREG02			(1<<2)
#define st_TRXREG03			(1<<3)
#define st_TRXREG04			(1<<4)
#define st_TRXREG05			(1<<5)
#define st_TRXREG06			(1<<6)
#define st_TRXREG07			(1<<7)
#define st_TRXREG08			(1<<8)
#define st_TRXREG09			(1<<9)
#define st_TRXREG0A			(1<<10)
#define st_TRXREG0B			(1<<11)
#define st_TRXREG0C			(1<<12)
#define st_TRXREG0D			(1<<13)
#define st_TRXREG0E			(1<<14)
#define st_TRXREG0F			(1<<15)
#define st_TRXREG10			(1<<16)
#define st_TRXREG11			(1<<17)
#define st_TRXREG12			(1<<18)
#define st_TRXREG13			(1<<19)
#define st_TRXREG14			(1<<20)
#define st_TRXREG15			(1<<21)
#define st_TRXREG16			(1<<22)
#define st_TRXREG17			(1<<23)
#define st_TRXREG18			(1<<24)
#define st_TRXREG19			(1<<25)
#define st_TRXREG1A			(1<<26)
#define st_TRXREG1B			(1<<27)

#define abs(a)                ((a) < 0 ?(-1*(a)):(a))

#ifndef __BK7011RCBEKEN_H__
#define __BK7011RCBEKEN_H__

#define RC_BEKEN_BASE		0x01050000

/// REG0x0
typedef union
{
    struct
    {
        volatile unsigned int ch0en         : 1;  /**< Enable BK7011 (1: Enable) */
        volatile unsigned int Reserved      : 2;  /**< NC */
        volatile unsigned int rcen          : 1;  /**< Enable BK7011 (1: Enable) */
        volatile unsigned int Reserved_     : 4;  /**< NC */
        volatile unsigned int ch0ld         : 1;  /**< BK7011 LD输出值（是否锁定） */
        volatile unsigned int lnarssi       : 1;  /**< BK7011 LNA RSSI状态 */
        volatile unsigned int Reserved__    : 2;  /**< NC */
        volatile unsigned int ch0shdnstat   : 1;  /**< BK7011是否处于shutdown状态 */
        volatile unsigned int Reserved___   : 3;  /**< NC */
        volatile unsigned int rcstate       : 3;  /**< 当前RC的状态;0x00=SPI_RESET ;0x01=SHUTDOWN ;0x02=WAIT_SPI (SPI正在操作);0x03=WAIT_LOCK（RFPLL正在Lock）;0x04=ACTIVE */
        volatile unsigned int Reserved____  : 11; /**< NC */
        volatile unsigned int spireset      : 1;  /**< Reset BK7011的SPI寄存器 */
        volatile unsigned int forceenable   : 1;  /**< 强制控制BK7011的接口信号 */
    } bits;
    volatile unsigned int value;
} BK7011_RC_BEKEN_REG0x0_TypeDef;

/// REG0x1
typedef union
{
    struct
    {
        volatile unsigned int trxregstat    : 28; /**< TRx Register Stat.;0x0: register is idle;0x1: register is updating. Can not write register again */
        volatile unsigned int prescaler     : 4;  /**< SPI时钟频率控制=RC_Clock (80 MHz)/2/PRESCALE */
    } bits;
    volatile unsigned int value;
} BK7011_RC_BEKEN_REG0x1_TypeDef;

/// REG0x5
typedef union
{
    struct
    {
        volatile unsigned int ch0outpower   : 8;  /**< No description */
        volatile unsigned int Reserved      : 24; /**< NC */
    } bits;
    volatile unsigned int value;
} BK7011_RC_BEKEN_REG0x5_TypeDef;

/// REG0x8
typedef union
{
    struct
    {
        volatile unsigned int CH0RXONDELAY  : 8; /**< No description */
        volatile unsigned int Reserved      : 8; /**< NC */
        volatile unsigned int CH0RXOFFDELAY : 8; /**< No description */
        volatile unsigned int Reserved_     : 8; /**< NC */
    } bits;
    volatile unsigned int value;
} BK7011_RC_BEKEN_REG0x8_TypeDef;

/// REG0xB
typedef union
{
    struct
    {
        volatile unsigned int CH0TXONDELAY  : 8; /**< No description */
        volatile unsigned int Reserved      : 8; /**< NC */
        volatile unsigned int CH0TXOFFDELAY : 8; /**< No description */
        volatile unsigned int Reserved_     : 8; /**< NC */
    } bits;
    volatile unsigned int value;
} BK7011_RC_BEKEN_REG0xB_TypeDef;

/// REG0xE
typedef union
{
    struct
    {
        volatile unsigned int CH0PAONDELAY  : 8; /**< No description */
        volatile unsigned int Reserved      : 8; /**< NC */
        volatile unsigned int CH0PAOFFDELAY : 8; /**< No description */
        volatile unsigned int Reserved_     : 8; /**< NC */
    } bits;
    volatile unsigned int value;
} BK7011_RC_BEKEN_REG0xE_TypeDef;

/// REG0x11
typedef union
{
    struct
    {
        volatile unsigned int CH0SHDNONDELAY  : 8; /**< No description */
        volatile unsigned int Reserved        : 8; /**< NC */
        volatile unsigned int CH0SHDNOFFDELAY : 8; /**< No description */
        volatile unsigned int Reserved_       : 8; /**< NC */
    } bits;
    volatile unsigned int value;
} BK7011_RC_BEKEN_REG0x11_TypeDef;

/// REG0x19
typedef union
{
    struct
    {
        volatile unsigned int FCH0SHDN  : 1;  /**< 强制模式下的SHDN */
        volatile unsigned int FCH0RXEN  : 1;  /**< 强制模式下的RXEN */
        volatile unsigned int FCH0TXEN  : 1;  /**< 强制模式下的TXEN */
        volatile unsigned int FCH0RXHP  : 1;  /**< 强制模式下的RXHP */
        volatile unsigned int Reserved  : 4;  /**< NC */
        volatile unsigned int FCH0B     : 8;  /**< 强制模式下增益控制 */
        volatile unsigned int FCH0EN    : 1;  /**< 强制控制RF接口信号 */
        volatile unsigned int Reserved_ : 15; /**< NC */
    } bits;
    volatile unsigned int value;
} BK7011_RC_BEKEN_REG0x19_TypeDef;

/// REG0x1A
typedef union
{
    volatile unsigned int value;
} BK7011_RC_BEKEN_REG0x1A_TypeDef;

/// REG0x1C
typedef union
{
    struct
    {
        volatile unsigned int FRXON    : 1;  /**< 强制模式下RX ON */
        volatile unsigned int FTXON    : 1;  /**< 强制模式下TX ON */
        volatile unsigned int Reserved : 30; /**< NC */
    } bits;
    volatile unsigned int value;
} BK7011_RC_BEKEN_REG0x1C_TypeDef;

/// REG0x1E
typedef union
{
    struct
    {
        volatile unsigned int FERXONDEL : 12; /**< No description */
        volatile unsigned int Reserved  : 20; /**< NC */
    } bits;
    volatile unsigned int value;
} BK7011_RC_BEKEN_REG0x1E_TypeDef;

/// REG0x3C
typedef union
{
    struct
    {
        volatile unsigned int RXAVGQRD    : 12; /**< Q路计算结果 */
        volatile unsigned int RXAVGIRD    : 12; /**< I路计算结果 */
        volatile unsigned int Reserved    : 3;  /**< NC */
        volatile unsigned int RX_ATTEN_SWITCH_COEX : 1;  /**< 0x0: RF Attenuation and Rx Switch controlled sperately 0x1: RF Attenuation and Rx Switch controlled at the same time */
        volatile unsigned int RXHPFBYPASS : 1;  /**< RX HPF bypass */
        volatile unsigned int RXIQSWAP    : 1;  /**< RX IQ SWAP */
        volatile unsigned int RXAVGMODE   : 1;  /**< 0：求取信号的平均值;1：求取信号绝对值得平均值 */
        volatile unsigned int RXDCCALEN   : 1;  /**< 使能RX DC计算(1) */
    } bits;
    volatile unsigned int value;
} BK7011_RC_BEKEN_REG0x3C_TypeDef;

/// REG0x3E
typedef union
{
    struct
    {
        volatile unsigned int RXDCQRD  : 12; /**< Q路DC计算结果 */
        volatile unsigned int RXDCIRD  : 12; /**< I路DC计算结果 */
        volatile unsigned int Reserved : 6;  /**< NC */
        volatile unsigned int RXCOMPEN : 1;  /**< 使能Mismatch补偿(1) */
        volatile unsigned int RXCALEN  : 1;  /**< 使能Mismatch计算(1) */
    } bits;
    volatile unsigned int value;
} BK7011_RC_BEKEN_REG0x3E_TypeDef;

/// REG0x3F
typedef union
{
    struct
    {
        volatile unsigned int RXPHASEERRRD : 10; /**< 估计出来的相位误差，phase_err_est*2^9，有符号 */
        volatile unsigned int Reserved     : 6;  /**< NC */
        volatile unsigned int RXAMPERRRD   : 10; /**< 估计出来的幅度误差，amp_err_est*2^9，有符号 */
        volatile unsigned int Reserved_    : 6;  /**< NC */
    } bits;
    volatile unsigned int value;
} BK7011_RC_BEKEN_REG0x3F_TypeDef;

/// REG0x40
typedef union
{
    struct
    {
        volatile unsigned int RXTY2RD  : 10; /**< 估计出来的TY2，(ty2-0.5)*2^10，有符号 */
        volatile unsigned int Reserved : 22; /**< NC */
    } bits;
    volatile unsigned int value;
} BK7011_RC_BEKEN_REG0x40_TypeDef;

/// REG0x41
typedef union
{
    struct
    {
        volatile unsigned int RXDCQWR  : 12; /**< 补偿时使用的Q路DC */
        volatile unsigned int RXDCIWR  : 12; /**< 补偿时使用的I路DC */
        volatile unsigned int Reserved : 8;  /**< NC */
    } bits;
    volatile unsigned int value;
} BK7011_RC_BEKEN_REG0x41_TypeDef;

/// REG0x42
typedef union
{
    struct
    {
        volatile unsigned int RXPHASEERRWR : 10; /**< 补偿时使用的phase_err*ty2*2^9 */
        volatile unsigned int Reserved     : 6;  /**< NC */
        volatile unsigned int RXAMPERRWR   : 10; /**< 补偿时使用的ty2/(1+amp_err)*2^9 */
        volatile unsigned int Reserved_    : 6;  /**< NC */
    } bits;
    volatile unsigned int value;
} BK7011_RC_BEKEN_REG0x42_TypeDef;

/// REG0x43
typedef union
{
    struct
    {
        volatile unsigned int PRE_GAIN_2ND   : 10; /**< 2nd stage PreGain */
        volatile unsigned int Reserved       : 2;  /**< NC */
        volatile unsigned int TRIANGLE_AMP   : 1; /**< 三角波幅度范围：0： 2048~2560 1： 2048~3072 */
        volatile unsigned int DPD_ADDR_SCALE : 1;  /**< PD 地址缩放：0：1/2 1: 1/4 */
        volatile unsigned int Reserved_      : 18;  /**< NC */
    } bits;
    volatile unsigned int value;
} BK7011_RC_BEKEN_REG0x43_TypeDef;

/// REG0x4C
typedef union
{
    struct
    {
        volatile unsigned int QCONSTANT   : 12; /**< 当发射常数时，Q的值 */
        volatile unsigned int Reserved    : 3;  /**< NC */
        volatile unsigned int DPD_EN      : 1;  /**< DPD enable */
        volatile unsigned int ICONSTANT   : 12; /**< 当发射常数时，I的值 */
        volatile unsigned int TXPOWTBLEN  : 1;  /**< Tx Power Table Enable;  0x0: 软件控制Table读写;  0x1: 硬件控制Table读 */
        volatile unsigned int TXCOMPDIS   : 1;  /**< 发送补偿功能禁止：;0x0: 使能发送补偿;0x1: 禁止发送补偿 */
        volatile unsigned int TESTPATTERN : 2;  /**< 0：正常模式，发射正常来自Modem的数据; 1：I/Q发射常数; 2：I/Q发射满幅度正弦波; 3：I/Q发射三角波 */
    } bits;
    volatile unsigned int value;
} BK7011_RC_BEKEN_REG0x4C_TypeDef;

/// REG0x4D
typedef union
{
    struct
    {
        volatile unsigned int Reserved  : 12; /**< NC */
        volatile unsigned int TXSINAMP  : 4;  /**< 正弦波幅度调整系数。小数表示，x.yyy。范围是[0 15/8] */
        volatile unsigned int TXSINMODE : 2;  /**< 0：I/Q路发射复数正弦波;1：仅仅I路发射，Q路发射0;2：仅仅Q路发射，I路发射0;3：保留 */
        volatile unsigned int Reserved_ : 4;  /**< NC */
        volatile unsigned int TXSINF    : 10; /**< 发射正弦波频率;TX_SIN_F=2*pi*F/80e6*2^8，F是发射频率 */
    } bits;
    volatile unsigned int value;
} BK7011_RC_BEKEN_REG0x4D_TypeDef;

/// REG0x4E
typedef union
{
    struct
    {
        volatile unsigned int hbf40sel  : 1;  /**< 40M模式上采样滤波器系数  1： beken 系数 0：rw 系数 */
        volatile unsigned int hbf40bp   : 1;  /**< 40M模式上采样滤波器 bypass */
        volatile unsigned int hbf20sel  : 1;  /**< 20M模式上采样滤波器系数  1： beken 系数 0：rw 系数 */
        volatile unsigned int hbf20bp   : 1;  /**< 20M模式上采样滤波器 bypass */
        volatile unsigned int Reserved_ : 28; /**< NC */
    } bits;
    volatile unsigned int value;
} BK7011_RC_BEKEN_REG0x4E_TypeDef;

/// REG0x4F
typedef union
{
    struct
    {
        volatile unsigned int TXQDCCOMP : 12; /**< Q DC offset，[-512 511] */
        volatile unsigned int Reserved  : 4;  /**< NC */
        volatile unsigned int TXIDCCOMP : 12; /**< I DC offset，[-512 511] */
        volatile unsigned int Reserved_ : 4;  /**< NC */
    } bits;
    volatile unsigned int value;
} BK7011_RC_BEKEN_REG0x4F_TypeDef;

/// REG0x50
typedef union
{
    struct
    {
        volatile unsigned int TXQGAINCOMP : 12; /**< 0 ~ 1023/1024，step:1/1024 */
        volatile unsigned int Reserved    : 4;  /**< NC */
        volatile unsigned int TXIGAINCOMP : 12; /**<  0 ~ 1023/1024，step:1/1024 */
        volatile unsigned int Reserved_   : 4;  /**< NC */
    } bits;
    volatile unsigned int value;
} BK7011_RC_BEKEN_REG0x50_TypeDef;

/// REG0x51
typedef union
{
    struct
    {
        volatile unsigned int TXTY2       : 12; /**< 512/1024 ~ (512+1023)/1024，step:1/1024 */
        volatile unsigned int Reserved    : 4;  /**< NC */
        volatile unsigned int TXPHASECOMP : 12; /**< -512/1024~ 511/1024，step:1/1024 */
        volatile unsigned int Reserved_   : 4;  /**< NC */
    } bits;
    volatile unsigned int value;
} BK7011_RC_BEKEN_REG0x51_TypeDef;

/// REG0x52
typedef union
{
    struct
    {
        volatile unsigned int Reserved         : 6;  /**< NC */
        volatile unsigned int IQCONSTANTIQCALP : 10; /**< IQ校准，IQ CONSTANT 输入值正值 */
        volatile unsigned int Reserved_        : 5;  /**< IQ信号增益控制;0：1.75 dB；;…. 31：-6dB;; */
        volatile unsigned int IQCONSTANTPOUT   : 10; /**< 功率校准，IQ CONSTANT 输入值 */
        volatile unsigned int TXIQSWAP         : 1;  /**<  IQ Swap */
    } bits;
    volatile unsigned int value;
} BK7011_RC_BEKEN_REG0x52_TypeDef;

/// REG0x54
typedef union
{
    struct
    {
        volatile unsigned int STRXADCIQ  : 1; /**< RX ADC IQ校准 Settling time 0:300us;1:600us */
        volatile unsigned int TSSIPOUTTH : 8; /**< 功率校准，最佳输出功率对应的TSSI门限 */
        volatile unsigned int DSELVA     : 1; /**< TSSI or AGC gain setting for versiong A, output 0: TSSI 1:AGC gain setting */
        volatile unsigned int TXDCN      : 2; /**< TX DC 两分法第一次执行步骤 0:3次，1:4次，2:5次，3:6次 */
        volatile unsigned int TSSIRD     : 12; /**< TSSI Value. Just valid during tssi_cal_en assert */
        volatile unsigned int AGCPGARD   : 4; /**< AGC的PGA结果 */
        volatile unsigned int AGCBUFRD   : 1; /**< AGC的BUF结果 */
        volatile unsigned int AGCLNARD   : 2; /**< AGC的LNA结果 */
        volatile unsigned int AGCMAPMODE : 1; /**< 0：映射方式一;1：映射方式二 */
    } bits;
    volatile unsigned int value;
} BK7011_RC_BEKEN_REG0x54_TypeDef;

/// REG0x55
typedef union
{
    struct
    {
        volatile unsigned int RXSNRNOISE : 9; /**< rx noise for sinar test */
        volatile unsigned int Reserved   : 7; /**< NC */
        volatile unsigned int RXSNRSIG   : 9; /**< rx signal for sinar test */
        volatile unsigned int Reserved_  : 7; /**< NC */
    } bits;
    volatile unsigned int value;
} BK7011_RC_BEKEN_REG0x55_TypeDef;

/// REG0x56
typedef union
{
    struct
    {
        volatile unsigned int rf_bias_cout : 5; /**< rf bias calib result */
        volatile unsigned int Reserved   : 27; /**< NC */
    } bits;
    volatile unsigned int value;
} BK7011_RC_BEKEN_REG0x56_TypeDef;

/// REG0x57
typedef union
{
    struct
    {
        volatile unsigned int adc_rssi         : 12; /**< rf lna sat for record */
        volatile unsigned int adc_rssi_rec_max : 12; /**< rf gain is according to maxium rssi */
        volatile unsigned int rf_gain_rec_max  : 7; /**< maxium adc rssi be recorded */
        volatile unsigned int rf_lna_sat_rec   : 1; /**< rx adc rssi */
    } bits;
    volatile unsigned int value;
} BK7011_RC_BEKEN_REG0x57_TypeDef;

/// REG0x5A
typedef union
{
    struct
    {
        volatile unsigned int TXCALCAPI        : 8; /**< I path calib capacity for transmit mode */
        volatile unsigned int RXCALCAPI        : 8; /**< I path calib capacity for receive mode */
        volatile unsigned int STANDBYCALCAPI   : 8; /**< I path calib capacity for standby mode */
        volatile unsigned int Reserved_        : 8; /**< NC */
    } bits;
    volatile unsigned int value;
} BK7011_RC_BEKEN_REG0x5A_TypeDef;

/// REG0x5B
typedef union
{
    struct
    {
        volatile unsigned int TXCALCAPQ        : 8; /**< Q path calib capacity for transmit mode */
        volatile unsigned int RXCALCAPQ        : 8; /**< Q path calib capacity for receive mode */
        volatile unsigned int STANDBYCALCAPQ   : 8; /**< Q path calib capacity for standby mode */
        volatile unsigned int Reserved_        : 8; /**< NC */
    } bits;
    volatile unsigned int value;
} BK7011_RC_BEKEN_REG0x5B_TypeDef;

/// REG0x5C
typedef union
{
    struct
    {
        volatile unsigned int trxspiintval : 10; /**< TRX SPI Transfer Interval. Unit is 12.5ns */
        volatile unsigned int Reserved     : 14; /**< NC */
        volatile unsigned int Reserved_    : 7;  /**< Reserved */
        volatile unsigned int trxbankrpt   : 1;  /**< TRX Bank Report. 0x0:bank0; 0x1:bank1 */
    } bits;
    volatile unsigned int value;
} BK7011_RC_BEKEN_REG0x5C_TypeDef;

/// REG0x6A
typedef union
{
    struct
    {
        volatile unsigned int trxaddaregstat : 6;  /**< ADC&DAC Register Status.;0x0: register is idle;0x1: register is updating. Can not write register again */
        volatile unsigned int Reserved       : 26; /**< NC */
    } bits;
    volatile unsigned int value;
} BK7011_RC_BEKEN_REG0x6A_TypeDef;

#if 0
/// REG0x70
typedef union
{
    struct
    {
        volatile unsigned int palevel0map    : 16; /**< PA Level0 Mapping Value */
        volatile unsigned int palevel1map    : 16; /**< PA Level1 Mapping Value */
    } bits;
    volatile unsigned int value;
} BK7011_RC_BEKEN_REG0x70_TypeDef;

/// REG0x71
typedef union
{
    struct
    {
        volatile unsigned int palevel2map    : 16; /**< PA Level2 Mapping Value */
        volatile unsigned int palevel3map    : 16; /**< PA Level3 Mapping Value */
    } bits;
    volatile unsigned int value;
} BK7011_RC_BEKEN_REG0x71_TypeDef;

/// REG0x72
typedef union
{
    struct
    {
        volatile unsigned int palevel4map    : 16; /**< PA Level4 Mapping Value */
        volatile unsigned int palevel5map    : 16; /**< PA Level5 Mapping Value */
    } bits;
    volatile unsigned int value;
} BK7011_RC_BEKEN_REG0x72_TypeDef;

/// REG0x73
typedef union
{
    struct
    {
        volatile unsigned int palevel6map    : 16; /**< PA Level6 Mapping Value */
        volatile unsigned int palevel7map    : 16; /**< PA Level7 Mapping Value */
    } bits;
    volatile unsigned int value;
} BK7011_RC_BEKEN_REG0x73_TypeDef;

/// REG0x74
typedef union
{
    struct
    {
        volatile unsigned int palevel8map    : 16; /**< PA Level8 Mapping Value */
        volatile unsigned int palevel9map    : 16; /**< PA Level9 Mapping Value */
    } bits;
    volatile unsigned int value;
} BK7011_RC_BEKEN_REG0x74_TypeDef;

/// REG0x75
typedef union
{
    struct
    {
        volatile unsigned int palevel10map    : 16; /**< PA Level10 Mapping Value */
        volatile unsigned int palevel11map    : 16; /**< PA Level11 Mapping Value */
    } bits;
    volatile unsigned int value;
} BK7011_RC_BEKEN_REG0x75_TypeDef;

/// REG0x76
typedef union
{
    struct
    {
        volatile unsigned int palevel12map    : 16; /**< PA Level12 Mapping Value */
        volatile unsigned int palevel13map    : 16; /**< PA Level13 Mapping Value */
    } bits;
    volatile unsigned int value;
} BK7011_RC_BEKEN_REG0x76_TypeDef;

/// REG0x77
typedef union
{
    struct
    {
        volatile unsigned int palevel4map    : 16; /**< PA Level4 Mapping Value */
        volatile unsigned int palevel5map    : 16; /**< PA Level5 Mapping Value */
    } bits;
    volatile unsigned int value;
} BK7011_RC_BEKEN_REG0x77_TypeDef;
#endif

/// BK7231N_RC_REG
struct BK7011RCBEKEN_TypeDef
{
    volatile BK7011_RC_BEKEN_REG0x0_TypeDef  *REG0x0;
    volatile BK7011_RC_BEKEN_REG0x1_TypeDef  *REG0x1;
    volatile BK7011_RC_BEKEN_REG0x5_TypeDef  *REG0x5;
    volatile BK7011_RC_BEKEN_REG0x8_TypeDef  *REG0x8;
    volatile BK7011_RC_BEKEN_REG0xB_TypeDef  *REG0xB;
    volatile BK7011_RC_BEKEN_REG0xE_TypeDef  *REG0xE;
    volatile BK7011_RC_BEKEN_REG0x11_TypeDef *REG0x11;
    volatile BK7011_RC_BEKEN_REG0x19_TypeDef *REG0x19;
    volatile BK7011_RC_BEKEN_REG0x1A_TypeDef *REG0x1A;
    volatile BK7011_RC_BEKEN_REG0x1C_TypeDef *REG0x1C;
    volatile BK7011_RC_BEKEN_REG0x1E_TypeDef *REG0x1E;
    volatile BK7011_RC_BEKEN_REG0x3C_TypeDef *REG0x3C;
    volatile BK7011_RC_BEKEN_REG0x3E_TypeDef *REG0x3E;
    volatile BK7011_RC_BEKEN_REG0x3F_TypeDef *REG0x3F;
    volatile BK7011_RC_BEKEN_REG0x40_TypeDef *REG0x40;
    volatile BK7011_RC_BEKEN_REG0x41_TypeDef *REG0x41;
    volatile BK7011_RC_BEKEN_REG0x42_TypeDef *REG0x42;
    volatile BK7011_RC_BEKEN_REG0x43_TypeDef *REG0x43;
    volatile BK7011_RC_BEKEN_REG0x4C_TypeDef *REG0x4C;
    volatile BK7011_RC_BEKEN_REG0x4D_TypeDef *REG0x4D;
    volatile BK7011_RC_BEKEN_REG0x4E_TypeDef *REG0x4E;
    volatile BK7011_RC_BEKEN_REG0x4F_TypeDef *REG0x4F;
    volatile BK7011_RC_BEKEN_REG0x50_TypeDef *REG0x50;
    volatile BK7011_RC_BEKEN_REG0x51_TypeDef *REG0x51;
    volatile BK7011_RC_BEKEN_REG0x52_TypeDef *REG0x52;
    volatile BK7011_RC_BEKEN_REG0x54_TypeDef *REG0x54;
    volatile BK7011_RC_BEKEN_REG0x55_TypeDef *REG0x55;
    volatile BK7011_RC_BEKEN_REG0x56_TypeDef *REG0x56;
    volatile BK7011_RC_BEKEN_REG0x57_TypeDef *REG0x57;
    volatile BK7011_RC_BEKEN_REG0x5A_TypeDef *REG0x5A;
    volatile BK7011_RC_BEKEN_REG0x5B_TypeDef *REG0x5B;
    volatile BK7011_RC_BEKEN_REG0x5C_TypeDef *REG0x5C;
    volatile BK7011_RC_BEKEN_REG0x6A_TypeDef *REG0x6A;
    #if 0
    volatile BK7011_RC_BEKEN_REG0x70_TypeDef *REG0x70;
    volatile BK7011_RC_BEKEN_REG0x71_TypeDef *REG0x71;
    volatile BK7011_RC_BEKEN_REG0x72_TypeDef *REG0x72;
    volatile BK7011_RC_BEKEN_REG0x73_TypeDef *REG0x73;
    volatile BK7011_RC_BEKEN_REG0x74_TypeDef *REG0x74;
    volatile BK7011_RC_BEKEN_REG0x75_TypeDef *REG0x75; 
    volatile BK7011_RC_BEKEN_REG0x76_TypeDef *REG0x76;
    volatile BK7011_RC_BEKEN_REG0x77_TypeDef *REG0x77;
    #endif
};

typedef struct
{
    BK7011_RC_BEKEN_REG0x0_TypeDef  REG0x0;
    BK7011_RC_BEKEN_REG0x1_TypeDef  REG0x1;
    BK7011_RC_BEKEN_REG0x5_TypeDef  REG0x5;
    BK7011_RC_BEKEN_REG0x8_TypeDef  REG0x8;
    BK7011_RC_BEKEN_REG0xB_TypeDef  REG0xB;
    BK7011_RC_BEKEN_REG0xE_TypeDef  REG0xE;
    BK7011_RC_BEKEN_REG0x11_TypeDef REG0x11;
    BK7011_RC_BEKEN_REG0x19_TypeDef REG0x19;
    BK7011_RC_BEKEN_REG0x1A_TypeDef REG0x1A;
    BK7011_RC_BEKEN_REG0x1C_TypeDef REG0x1C;
    BK7011_RC_BEKEN_REG0x1E_TypeDef REG0x1E;
    BK7011_RC_BEKEN_REG0x3C_TypeDef REG0x3C;
    BK7011_RC_BEKEN_REG0x3E_TypeDef REG0x3E;
    BK7011_RC_BEKEN_REG0x3F_TypeDef REG0x3F;
    BK7011_RC_BEKEN_REG0x40_TypeDef REG0x40;
    BK7011_RC_BEKEN_REG0x41_TypeDef REG0x41;
    BK7011_RC_BEKEN_REG0x42_TypeDef REG0x42;
    BK7011_RC_BEKEN_REG0x43_TypeDef REG0x43;
    BK7011_RC_BEKEN_REG0x4C_TypeDef REG0x4C;
    BK7011_RC_BEKEN_REG0x4D_TypeDef REG0x4D;
    BK7011_RC_BEKEN_REG0x4E_TypeDef REG0x4E;
    BK7011_RC_BEKEN_REG0x4F_TypeDef REG0x4F;
    BK7011_RC_BEKEN_REG0x50_TypeDef REG0x50;
    BK7011_RC_BEKEN_REG0x51_TypeDef REG0x51;
    BK7011_RC_BEKEN_REG0x52_TypeDef REG0x52;
    BK7011_RC_BEKEN_REG0x54_TypeDef REG0x54;
    BK7011_RC_BEKEN_REG0x55_TypeDef REG0x55;
    BK7011_RC_BEKEN_REG0x56_TypeDef REG0x56;
    BK7011_RC_BEKEN_REG0x57_TypeDef REG0x57;
    BK7011_RC_BEKEN_REG0x5A_TypeDef REG0x5A;
    BK7011_RC_BEKEN_REG0x5B_TypeDef REG0x5B;
    BK7011_RC_BEKEN_REG0x5C_TypeDef REG0x5C;
    BK7011_RC_BEKEN_REG0x6A_TypeDef REG0x6A;
} BK7231N_RC_TypeDef;
#endif

#ifndef __BK7011TRxV2A_H__
#define __BK7011TRxV2A_H__

#define TRX_BEKEN_BASE		0x01050080

/// REG0x0
typedef union
{
    struct
    {
        volatile unsigned long tssi_atten       : 3; //Signal attenuation programming in power calibration: 000:6dB; 111:12dB; ~1dB/LSB;
        volatile unsigned long tssiIQ_gc        : 2; //Gain selection in IQcal when gctune_en=0;
        volatile unsigned long tssiDC_gc        : 2; //Gain selection in Dccal when gctune_en=0;
        volatile unsigned long tssi_gc          : 2; //Fixed gain in Dccal and Iqcal; <1>: stage 1, <0>: stage 2; 1:-3dB; 0: 0dB;
        volatile unsigned long enPcal           : 1; //1: Enable output power calibration; 0 : otherwise;
        volatile unsigned long enIQcal          : 1; //1: Enable IQ mismatch calibration; 0 : otherwise;
        volatile unsigned long gctune_en        : 1; //0: Separate Dccal and Iqcal gain settling; 0 : fixed gain for both;
        volatile unsigned long enDCcal          : 1; //1: Enable LO leakage calibration; 0 : otherwise;
        volatile unsigned long TSSIsel          : 1; //No internal connection
        volatile unsigned long NC               : 2;
        volatile unsigned long Dtau             : 4; //PA bias settling time programming; 1000:400ns; ~50n/LSB
        volatile unsigned long Dvldo            : 2; //Gm LDO output programming: (00,01,10) =>  [1.2, 1.3, 1.4]
        volatile unsigned long Dvnlo            : 2; //Mixer LO dc bias programming: 0.8*Vrefm + Vrefm*Dvnlo/16;
        volatile unsigned long tspi             : 8; //Reserved control bits;
    } bits;
    volatile unsigned int value;
}BK7011_TRxV2A_REG0x0_TypeDef;

/// REG0x1
typedef union
{
    struct
    {
        volatile unsigned long Icpmis_en        : 1; //enable the Icp mismatch function
        volatile unsigned long cp_ictrl_mode    : 1; //controlling the current bias mode in the CP
        volatile unsigned long cphalf_spi       : 1; //set the cp out to vdd/2
        volatile unsigned long Icp_offset       : 4; //controlling the offset current in the CP
        volatile unsigned long Icp_core         : 5; //controlling the CP current in the high power mode
        volatile unsigned long Cp2              : 2; //controlling the loop of filter of PLL
        volatile unsigned long Rp2              : 3; //controlling the loop of filter of PLL
        volatile unsigned long loopRz_lp        : 4; //controlling resistor of zero point in the loop filter in the high power mode
        volatile unsigned long Cp1              : 2; //controlling the loop of filter of PLL
        volatile unsigned long Rp3              : 3; //controlling the loop of filter of PLL
        volatile unsigned long Cp3              : 1; //controlling the loop of filter of PLL
        volatile unsigned long open             : 1; //enable the OP in the CP,help to improve the mismatch between UP and DOWN
        volatile unsigned long Icp_bias         : 4; //tunning the bias current in the CP of RFPLL
    } bits;
    volatile unsigned int value;
} BK7011_TRxV2A_REG0x1_TypeDef;

/// REG0x2
typedef union
{
    struct
    {
        volatile unsigned long NA1              : 1;
        volatile unsigned long clksel_cal       : 1; //choosing the clock of calibration
        volatile unsigned long bypass_opt       : 1; //by pass the option in the frequency calibration block
        volatile unsigned long manual_band      : 1; //enable the band controlling mannualy
        volatile unsigned long bandcal_spi      : 8; //bandcal controlling by spi
        volatile unsigned long band_spi_trgger  : 1; //trigger the band calibration
        volatile unsigned long bandcal_force    : 1; //force the bandcal_en signal to '1' or '0' by spi
        volatile unsigned long NA2              : 1;
        volatile unsigned long hvref            : 2; //set the detected votage in the locked detector
        volatile unsigned long lvref            : 2; //set the detected votage in the locked detector
        volatile unsigned long done_delay       : 4; //delay time of band callibration
        volatile unsigned long ampautocal       : 1; //enable the amplitude calibration of VCO
        volatile unsigned long ampcal_en        : 1; //enable the calibration function of amplitude
        volatile unsigned long ampctrl_m        : 1; //controlling the amplitude of VCO mannualy
        volatile unsigned long errdet_en        : 1; //enable the lock detector block
        volatile unsigned long errdet_spien     : 1; //enable output the lock detector function
        volatile unsigned long NA3              : 2;
        volatile unsigned long pllen            : 1; //enable PLL loop in the top by spi
        volatile unsigned long vcoen            : 1; //enable VCO in the top by spi
    } bits;
    volatile unsigned int value;
} BK7011_TRxV2A_REG0x2_TypeDef;

/// REG0x3
typedef union
{
    struct
    {
        volatile unsigned long amp_txbuf        : 3;  //
        volatile unsigned long lobuf_ampctrl    : 3;  //controlling the amplitude of LO buffer
        volatile unsigned long itune_LODIV      : 2;  //controlling the bias current in the divier-by-2 of LO chain
        volatile unsigned long vbn_LODIV        : 2;  //controlling the bias voltage in the divier-by-2 of LO chain
        volatile unsigned long NA1              : 1;
        volatile unsigned long ictrl_mode       : 1;  //change the working mode of bias current in the VCO
        volatile unsigned long ampsel           : 3;  //controlling the amplitude value of amplitude calibration in the normal working
        volatile unsigned long NA2              : 17;
    } bits;
    volatile unsigned int value;
} BK7011_TRxV2A_REG0x3_TypeDef;

/// REG0x4
typedef union
{
    struct
    {
        volatile unsigned long Itune_vco_spi    : 6;  //controlling the bias current of VCO
        volatile unsigned long Rvco             : 4;  //controlling the bias current of VCO
        volatile unsigned long vnw              : 4;  //controlling the bias voltage of Kmod varactor array
        volatile unsigned long reset_spi        : 1;  //Reset spi of SDM in PLL
        volatile unsigned long Nrsten           : 1;  //enable the reset function both PFD and Ncounter
        volatile unsigned long tristate_spi     : 1;  //enable the tristate in the PFD
        volatile unsigned long selvcopol        : 1;  //change the direction of PFD between positive and negative
        volatile unsigned long Nclken_spi       : 1;  //enable the Nclk output
        volatile unsigned long Nint             : 10; //The feedback N value in manual control mode
        volatile unsigned long pfddly           : 2;  //pfd delay time controlling
        volatile unsigned long int_mode         : 1;  //loop int mode enable
    } bits;
    volatile unsigned int value;
} BK7011_TRxV2A_REG0x4_TypeDef;

/// REG0x5
typedef union
{
    struct
    {
        volatile unsigned long nsdmlsb          : 8; //amplitude of noise input of SDM
        volatile unsigned long cksel_sdm        : 3; //set the clock frequency of SDM,0:26M;1:40M,
        volatile unsigned long NA1              : 1;
        volatile unsigned long pnen             : 1; //enable the phase noise improvement in the SDM
        volatile unsigned long sdm_rstn         : 1; //reset the SDM in the RFPLL
        volatile unsigned long NA2              : 5;
        volatile unsigned long podge_sel        : 1; //choosing the clock edge of SDM
        volatile unsigned long vcoldo_sel       : 3; //set the output voltage of vco ldo
        volatile unsigned long NA3              : 2;
        volatile unsigned long chspi            : 7; //set the frequency channel by spi: (2400+chspi)MHZ
    } bits;
    volatile unsigned int value;
} BK7011_TRxV2A_REG0x5_TypeDef;

/// REG0x6
typedef union
{
    struct
    {
        volatile unsigned long vtsel            : 1; //temp_det output voltage slelction 0:high resolution/1:low resolution
        volatile unsigned long md_ten           : 1; //adc internal signal test enable
        volatile unsigned long capcal_sel       : 1; //filter cap control selection, 1:from dig; 0 :from spi 
        volatile unsigned long dpd_en           : 1; //dpd cal path enable
        volatile unsigned long lpfcapcalq50     : 6; //lpf Q path calibiration input (low 6bits)
        volatile unsigned long lpfcapcali50     : 6; //lpf I path calibiration input  (low 6bits)
        volatile unsigned long dcocq            : 8; //dcoc Q path input
        volatile unsigned long dcoci            : 8; //dcoc I path input
    } bits;
    volatile unsigned int value;
} BK7011_TRxV2A_REG0x6_TypeDef;

/// REG0x7
typedef union
{
    struct
    {
        volatile unsigned long dig_dcoen        : 1; //dcoc input selection,1: digital   0: spi
        volatile unsigned long spilpfrxg30      : 4; //rxif  gain, 0000-1111 0dB  - 45dB step:3dB
        volatile unsigned long autorxifgen      : 1; //rxif gain control selection, 1: auto control  0: spi 
        volatile unsigned long dcoc_ctl         : 2; //dcoc gain contrl
        volatile unsigned long txgs             : 1; //txif gain option
        volatile unsigned long NC1              : 1;
        volatile unsigned long lpftxTest        : 1; //lpf tx test control bit,1 : tx test mode  0: default
        volatile unsigned long lpen             : 1; //IF filter low power mode enable
        volatile unsigned long iqsw             : 1; //IF filter output iq swap
        volatile unsigned long txif_2rd         : 1; //Tx filter 2nd mode enable
        volatile unsigned long abws_en          : 1; //Tx filter control mode selection 0: from dig/1:from spi
        volatile unsigned long adc_dly_men      : 1; //adc delay manual selection enable
        volatile unsigned long dac_isel_buf     : 3; //dac buffer bias current selection
        volatile unsigned long dac_isel_opa     : 3; //dac opamp bias current selection
        volatile unsigned long dac_delay        : 5; //dac delay selection
        volatile unsigned long dac_ck_edge      : 1; //dac clock edge selection
        volatile unsigned long lpfcapcalq       : 2; //lpf q path calibiration input (high 2bits)
        volatile unsigned long lpfcapcali       : 2; //lpf I path calibiration input (high 2bits)
    } bits;
    volatile unsigned int value;
} BK7011_TRxV2A_REG0x7_TypeDef;

/// REG0x8
typedef union
{
    struct
    {
        volatile unsigned long isrxref10        : 2; //rx ref ib selection 00-11 10-15uA
        volatile unsigned long isrxlna30        : 4; //rx lna ib selection 000-111 50-110uA
        volatile unsigned long NC1              : 1;
        volatile unsigned long isv2i            : 2; //v2i bias current selection
        volatile unsigned long isrxlnac10       : 2; //rx lnacali ib selection 00-11 40-70uA
        volatile unsigned long isrxdiv10        : 2; //rx div ib selection 00-11  10-25uA
        volatile unsigned long NC2              : 1;
        volatile unsigned long rssith50         : 6; //rx rssi threshhold  Pant=-29dBm(th=000111)  0:-37dBm  step:1dB
        volatile unsigned long isrssigm10       : 2; //rx rssi gm ib selection 00-11 40-55uA
        volatile unsigned long rssiten          : 1; //rssi analog v output test enable
        volatile unsigned long cloadlna         : 2; //LNA load  capacitor tunning
        volatile unsigned long lnacali_tune     : 1; //lna_cali phase tune
        volatile unsigned long NC3              : 3;
        volatile unsigned long att_g            : 1; //lna att level selection 0:-12dB/1:-15dB
        volatile unsigned long auto_att_en      : 1; //lna input attenuation mode  0:from spi/1:from dig
        volatile unsigned long atten_spi        : 1; //lna input attenuation by spi  0:0dB/1:-12dB
    } bits;
    volatile unsigned int value;
} BK7011_TRxV2A_REG0x8_TypeDef;

/// REG0x9
typedef union
{
    struct
    {
        volatile unsigned long agcrxfeEn        : 1; //enble rxfe agc by mcu, 0/1 gctr by spi/mcu
        volatile unsigned long grxlnaspi10      : 2; //rx lna gain 00-11 0/+6/+12/+18dBr
        volatile unsigned long grxi2vspi10      : 2; //rx i2v gain 00-11 0/+6/+6/+12dBr
        volatile unsigned long vsrxlnaldo10     : 3; //rx lna LDO vout 1~5 /1.0V~1.2V
        volatile unsigned long NC1              : 1;
        volatile unsigned long vsrxmixldo10     : 3; //rx mixer LDO vout 1~5 /1.0V~1.2V
        volatile unsigned long vbsrxlo20        : 3; //rx lo biasing voltage: 000-111 0.4-0.75V
        volatile unsigned long NC2              : 1;
        volatile unsigned long ibsrxi2v10       : 2; //rx i2v ib selection 00-11 5-15uA
        volatile unsigned long vcmsrxi2v        : 1; //rx i2v vcm selction 0-1 0.6-0.5V
        volatile unsigned long NC3              : 1;
        volatile unsigned long grxi2v_opt       : 1; //i2v optional gain control (4dB)
        volatile unsigned long abws_en_i2v      : 1; //i2v bandwith auto selection enable
        volatile unsigned long bw_i2v           : 1;
        volatile unsigned long NC4              : 9;
    } bits;
    volatile unsigned int value;
} BK7011_TRxV2A_REG0x9_TypeDef;

/// REG0xA
typedef union
{
    struct
    {
        volatile unsigned long Dvncmix          : 4; //Mixer cascode device gate bias voltage programming: Vnc=Vrefm+|Vthp|+30mV*Dvncmix
        volatile unsigned long Dvncpad          : 4; //PA driver cascode device bias voltage programming: Vnc = Vncref+33mV*Dncpad
        volatile unsigned long Drefmixlo        : 3; //Mixer switching quad gate bias reference voltage programming: Vref=1.1V+120mV*(1+Drefmixlo)
        volatile unsigned long Drefmix          : 3; //Mixer cascode device gate bias reference voltage programming: Vrefm=1.1V+120mV*(1+Drefmix)
        volatile unsigned long Dvncref          : 2; //PA and PAD Vncref programming; Vncref=V(50uA->ndiode)+500mV+60mV*Dvncref
        volatile unsigned long Dvnc             : 4; //PA and PAD cascode device gate bias voltage programming: Vnc_mV=Vncref+33*Dvnc
        volatile unsigned long Dvb              : 4; //PA Class-B path bias voltage programming, Vnb_mV=25*(1+Dvb)
        volatile unsigned long Dib              : 4; //No internal connection
        volatile unsigned long Dia              : 4; //PA Class-A path bias current programming, Ibias_uA=200 + 50 * Dia
    } bits;
    volatile unsigned int value;
} BK7011_TRxV2A_REG0xA_TypeDef;

/// REG0xB
typedef union
{
    struct
    {
        volatile unsigned long padCd            : 4; //PAD  output capacitor tunning; Group-D; For channels (100:101)_####;
        volatile unsigned long padCc            : 4; //PAD output capacitor tunning; Group-C; For channels 100_####;
        volatile unsigned long padCb            : 4; //PAD output capacitor tunning; Group-B; For channels 010_####;
        volatile unsigned long padCa            : 4; //PAD output capacitor tunning; Group-A; For channels (000:001)_####;
        volatile unsigned long modCd            : 4; //Modulator output capacitor tunning; Group-D; For channels (100:101)_####;
        volatile unsigned long modCc            : 4; //Modulator output capacitor tunning; Group-C; For channels 100_####;
        volatile unsigned long modCb            : 4; //Modulator output capacitor tunning; Group-B; For channels 010_####;
        volatile unsigned long modCa            : 4; //Modulator output capacitor tunning; Group-A; For channels (000:001)_####;
    } bits;
    volatile unsigned int value;
} BK7011_TRxV2A_REG0xB_TypeDef;

/// REG0xC
typedef union
{
    struct
    {
        volatile unsigned long Gmgain           : 4; //V2I Gm gain programming: Active units = 1 + Dgmgain
        volatile unsigned long padctrl          : 4; //PAD gain control by units: <3>=16; <2>=8; <1>= 4; <0>=4;
        volatile unsigned long pactrl           : 4; //PA output power control; (one PA slice)/(one bit)
        volatile unsigned long Rgm              : 4; //V2I input resistor programming: 0000: by-pass; 0001:max_gain; 1111:min_gain; gain step: -0.5dB/LSB;
        volatile unsigned long Dgmdc            : 4; //Baseband output (V2I) DC current programming: 20uA*Dgmdc
        volatile unsigned long Dipad            : 4; //PAD bias current programming, Ibias_uA=50 + 50*Dipad;
        volatile unsigned long Dibias           : 4; //TXFE signal path unit current programming (to compensate corner variation)
        volatile unsigned long NC               : 3;
        volatile unsigned long pamapen          : 1; //0: spi power control option;  1: direct digital power control (per packet);
    } bits;
    volatile unsigned int value;
} BK7011_TRxV2A_REG0xC_TypeDef;

/// REG0xD
typedef union
{
    struct
    {
        volatile unsigned long dacselection     : 1; //0: external dac, 1: internal dac
        volatile unsigned long lpfrxbw          : 1; //lpf rx bandwidth selection 1: high bandwidth(19MHz) 0: low bandwidth(9MHz);
        volatile unsigned long lpftxbw          : 1; //lpf tx bandwidth selection 1: high bandwidth(29MHz) 0: low bandwidth(14MHz);
        volatile unsigned long lpftrxsw         : 1; //LPF tx rx switch(1:tx,0:rx)
        volatile unsigned long enlpf            : 1; //LPF enable
        volatile unsigned long enif             : 1; //ADC, DAC Ldo enable
        volatile unsigned long endcoc           : 1; //DCOC DAC enable
        volatile unsigned long enrxadc          : 1; //RX ADC enable
        volatile unsigned long entxdac          : 1; //TX dac enable
        volatile unsigned long entxdacbias      : 1; //TX dac bias enable
        volatile unsigned long enrxrssi         : 1; //RX RF Rssi enable
        volatile unsigned long enrxref          : 1; //RX RF Vref enable
        volatile unsigned long enrxi2v          : 1; //RX I2V enable
        volatile unsigned long enrxmix          : 1; //RX mixer enable
        volatile unsigned long enlnacal         : 1; //RX look back lna input enable
        volatile unsigned long enlna            : 1; //Rxfe lna enable
        volatile unsigned long txvinsel         : 1; //Txfe input selection(0,cal when need bypass TX filter,1:normal)
        volatile unsigned long entssi           : 1; //Txfe tssi enable
        volatile unsigned long entssiadc        : 1; //Txfe tssi adc enable
        volatile unsigned long entxferef        : 1; //Txfe reference enable
        volatile unsigned long entxfebias       : 1; //Txfe bias enable
        volatile unsigned long entxv2i          : 1; //Tx v2i enable
        volatile unsigned long entxlo           : 1; //Tx LO enable
        volatile unsigned long entxpga          : 1; //TX PGA enable
        volatile unsigned long enpa             : 1; //Tx PA enable
        volatile unsigned long enrxsw           : 1; //RX switch enable
        volatile unsigned long entxsw           : 1; //TX switch enable
        volatile unsigned long trswpll          : 1; //rf pll tx rx switch
        volatile unsigned long enrfpll          : 1; //rf pll enable
        volatile unsigned long endobuler        : 1; //dobuler enable
        volatile unsigned long endpll           : 1; //digital pll enable
        volatile unsigned long enxtal           : 1; //xtal enable signal
    } bits;
    volatile unsigned int value;
} BK7011_TRxV2A_REG0xD_TypeDef;

/// REG0xE
typedef union
{
    struct
    {
        volatile unsigned long dacselection     : 1; //0: external dac, 1: internal dac
        volatile unsigned long lpfrxbw          : 1; //lpf rx bandwidth selection 1: high bandwidth(19MHz) 0: low bandwidth(9MHz);
        volatile unsigned long lpftxbw          : 1; //lpf tx bandwidth selection 1: high bandwidth(29MHz) 0: low bandwidth(14MHz);
        volatile unsigned long lpftrxsw         : 1; //LPF tx rx switch(1:tx,0:rx)
        volatile unsigned long enlpf            : 1; //LPF enable
        volatile unsigned long enif             : 1; //ADC, DAC Ldo enable
        volatile unsigned long endcoc           : 1; //DCOC DAC enable
        volatile unsigned long enrxadc          : 1; //RX ADC enable
        volatile unsigned long entxdac          : 1; //TX dac enable
        volatile unsigned long entxdacbias      : 1; //TX dac bias enable
        volatile unsigned long enrxrssi         : 1; //RX RF Rssi enable
        volatile unsigned long enrxref          : 1; //RX RF Vref enable
        volatile unsigned long enrxi2v          : 1; //RX I2V enable
        volatile unsigned long enrxmix          : 1; //RX mixer enable
        volatile unsigned long enlnacal         : 1; //RX look back lna input enable
        volatile unsigned long enlna            : 1; //Rxfe lna enable
        volatile unsigned long txvinsel         : 1; //Txfe input selection(0,cal when need bypass TX filter,1:normal)
        volatile unsigned long entssi           : 1; //Txfe tssi enable
        volatile unsigned long entssiadc        : 1; //Txfe tssi adc enable
        volatile unsigned long entxferef        : 1; //Txfe reference enable
        volatile unsigned long entxfebias       : 1; //Txfe bias enable
        volatile unsigned long entxv2i          : 1; //Tx v2i enable
        volatile unsigned long entxlo           : 1; //Tx LO enable
        volatile unsigned long entxpga          : 1; //TX PGA enable
        volatile unsigned long enpa             : 1; //Tx PA enable
        volatile unsigned long enrxsw           : 1; //RX switch enable
        volatile unsigned long entxsw           : 1; //TX switch enable
        volatile unsigned long trswpll          : 1; //rf pll tx rx switch
        volatile unsigned long enrfpll          : 1; //rf pll enable
        volatile unsigned long endobuler        : 1; //dobuler enable
        volatile unsigned long endpll           : 1; //digital pll enable
        volatile unsigned long enxtal           : 1; //xtal enable signal
    } bits;
    volatile unsigned int value;
} BK7011_TRxV2A_REG0xE_TypeDef;

/// REG0xF
typedef union
{
    struct
    {
        volatile unsigned long NC               : 1; //
        volatile unsigned long clkdac_sel       : 1; //DAC Clock Select.  0x0: 80MHz;  0x1: 160MHz
        volatile unsigned long clkadc_sel       : 1; //ADCClock Select.  0x0: 40MHz;  0x1: 80MHz
        volatile unsigned long NC1              : 2; //
        volatile unsigned long sinad_tx_en      : 1; //TX Sinad Enable
        volatile unsigned long sinad_rx_en      : 1; //RX Sinad Detect Enable
        volatile unsigned long tssi_cal_en      : 1; //TSSI Calibration Enable, enable 40M clock
        volatile unsigned long NC2              : 5; //
        volatile unsigned long sinad_hpf_coef   : 2; //HPF Coefficence for RX Siand Detect.  0x0: 1/8  0x1: 1/32;  0x2: 1/128; 0x3: 1/1024
        volatile unsigned long clkdac_inv       : 1; //DAC Clock Invert
        volatile unsigned long clkadc_inv       : 1; //ADC Clock Invert
#if 0
        volatile unsigned long NC3              : 11;
        volatile unsigned long sysldoen         : 1;
        volatile unsigned long sysldolp         : 1;
        volatile unsigned long endspvdd         : 1;
        volatile unsigned long digldolp         : 1;
        #endif
    } bits;
    volatile unsigned int value;
} BK7011_TRxV2A_REG0xF_TypeDef;

/// REG0x10
typedef union
{
    struct
    {
        volatile unsigned long dacselection     : 1; //0: external dac, 1: internal dac
        volatile unsigned long lpfrxbw          : 1; //lpf rx bandwidth selection 1: high bandwidth(19MHz) 0: low bandwidth(9MHz);
        volatile unsigned long lpftxbw          : 1; //lpf tx bandwidth selection 1: high bandwidth(29MHz) 0: low bandwidth(14MHz);
        volatile unsigned long lpftrxsw         : 1; //LPF tx rx switch(1:tx,0:rx)
        volatile unsigned long enlpf            : 1; //LPF enable
        volatile unsigned long enif             : 1; //ADC, DAC Ldo enable
        volatile unsigned long endcoc           : 1; //DCOC DAC enable
        volatile unsigned long enrxadc          : 1; //RX ADC enable
        volatile unsigned long entxdac          : 1; //TX dac enable
        volatile unsigned long entxdacbias      : 1; //TX dac bias enable
        volatile unsigned long enrxrssi         : 1; //RX RF Rssi enable
        volatile unsigned long enrxref          : 1; //RX RF Vref enable
        volatile unsigned long enrxi2v          : 1; //RX I2V enable
        volatile unsigned long enrxmix          : 1; //RX mixer enable
        volatile unsigned long enlnacal         : 1; //RX look back lna input enable
        volatile unsigned long enlna            : 1; //Rxfe lna enable
        volatile unsigned long txvinsel         : 1; //Txfe input selection(0,cal when need bypass TX filter,1:normal)
        volatile unsigned long entssi           : 1; //Txfe tssi enable
        volatile unsigned long entssiadc        : 1; //Txfe tssi adc enable
        volatile unsigned long entxferef        : 1; //Txfe reference enable
        volatile unsigned long entxfebias       : 1; //Txfe bias enable
        volatile unsigned long entxv2i          : 1; //Tx v2i enable
        volatile unsigned long entxlo           : 1; //Tx LO enable
        volatile unsigned long entxpga          : 1; //TX PGA enable
        volatile unsigned long enpa             : 1; //Tx PA enable
        volatile unsigned long enrxsw           : 1; //RX switch enable
        volatile unsigned long entxsw           : 1; //TX switch enable
        volatile unsigned long trswpll          : 1; //rf pll tx rx switch
        volatile unsigned long enrfpll          : 1; //rf pll enable
        volatile unsigned long endobuler        : 1; //dobuler enable
        volatile unsigned long endpll           : 1; //digital pll enable
        volatile unsigned long enxtal           : 1; //xtal enable signal
    } bits;
    volatile unsigned int value;
} BK7011_TRxV2A_REG0x10_TypeDef;

/// REG0x11
typedef union
{
    struct
    {
        volatile unsigned long tx_sinad_table   : 11; //TX Sinad Table;  D[0] = 0: Write I Path Table Data;  D[0] = 1: Write Q Path Table Data;  LSB￡oD[1] ;  Both I and Q need 16 number data
        volatile unsigned long NC               : 21;
    } bits;
    volatile unsigned int value;
} BK7011_TRxV2A_REG0x11_TypeDef;

/// REG0x12
typedef union
{
    struct
    {
        volatile unsigned long lpfouttsten      : 1; //lpf output test enable,0:disenable,1:enable
        volatile unsigned long lpfintsten       : 1; //lpf input test enable,0:disenable,1:enable
        volatile unsigned long ifbufic20        : 3; //test buffer bias current control,0:low, 1:high
        volatile unsigned long entstbufldo      : 1; //enable test buffer :disenable,1:enable
        volatile unsigned long adc_vref_sel     : 1; //adc refere voltage selection 0:vref/1:avdd
        volatile unsigned long ldoadda          : 2; //adc ldo voltage selection,1.0~1.2
        volatile unsigned long buftstselection  : 2; //buffer input selection:1:lpf, 0:rssidactst,rssitst,tssitst,adcstst1
        volatile unsigned long pwd_adccal       : 1; //power down adc calibration
        volatile unsigned long adc_dlym         : 7; //adc delay time selection
        volatile unsigned long adc_cksel        : 2; //adc clock  pulse width selection
        volatile unsigned long adciselc         : 3; //adc core bias current selection,0:5x,1:0.6x,2:0.7X,3:0.8x,4:1.0x,5:1.2X,6:1.4X,7:1.7X
        volatile unsigned long fictrl30         : 4; //lpf bias current selection, 0:low, 15:high
        volatile unsigned long flvcm10          : 2; //lpf common mode voltage selection,00:low , 11: high
        volatile unsigned long ldoifsel3v10     : 2; //If ldo vout selection:00:low, 11: high
    } bits;
    volatile unsigned int value;
} BK7011_TRxV2A_REG0x12_TypeDef;

/// REG0x13
typedef union
{
    struct
    {
        volatile unsigned long r2_rx            : 2; //rx mode loopfilter R2 control
        volatile unsigned long c1_rx            : 2; //rx mode loopfilter C1 control
        volatile unsigned long rz_rx            : 5; //rx mode loopfilter Rz control
        volatile unsigned long icp_rx           : 5; //rx mode cp current control
        volatile unsigned long icpoff20         : 3; //cp offset current control
        volatile unsigned long icpsel           : 1; //icp bias select: 0, internal; 1, external
        volatile unsigned long r2_tx            : 2; //tx mode loopfilter R2 control
        volatile unsigned long c1_tx            : 2; //tx mode loopfilter C1 control
        volatile unsigned long rz_tx            : 5; //tx mode loopfilter Rz control
        volatile unsigned long icp_tx           : 5; //tx mode cp current control
    } bits;
    volatile unsigned int value;
} BK7011_TRxV2A_REG0x13_TypeDef;

/// REG0x14
typedef union
{
    struct
    {
        volatile unsigned long dc_i_0db         : 8; //RX dcoc I path input for 0dB of PGA
        volatile unsigned long dc_q_0db         : 8; //RX dcoc Q path input for 0dB of PGA
        volatile unsigned long dc_i_3db         : 8; //RX dcoc I path input for 3dB of PGA
        volatile unsigned long dc_q_3db         : 8; //RX dcoc Q path input for 3dB of PGA
    } bits;
    volatile unsigned int value;
} BK7011_TRxV2A_REG0x14_TypeDef;

/// REG0x15
typedef union
{
    struct
    {
        volatile unsigned long dc_i_6db         : 8; //RX dcoc I path input for 6dB of PGA
        volatile unsigned long dc_q_6db         : 8; //RX dcoc Q path input for 6dB of PGA
        volatile unsigned long dc_i_9db         : 8; //RX dcoc I path input for 9dB of PGA
        volatile unsigned long dc_q_9db         : 8; //RX dcoc Q path input for 9dB of PGA
    } bits;
    volatile unsigned int value;
} BK7011_TRxV2A_REG0x15_TypeDef;

/// REG0x16
typedef union
{
    struct
    {
        volatile unsigned long dc_i_12db        : 8; //RX dcoc I path input for 12dB of PGA
        volatile unsigned long dc_q_12db        : 8; //RX dcoc Q path input for 12dB of PGA
        volatile unsigned long dc_i_15db        : 8; //RX dcoc I path input for 15dB of PGA
        volatile unsigned long dc_q_15db        : 8; //RX dcoc Q path input for 15dB of PGA
    } bits;
    volatile unsigned int value;
} BK7011_TRxV2A_REG0x16_TypeDef;

/// REG0x17
typedef union
{
    struct
    {
        volatile unsigned long dc_i_18db        : 8; //RX dcoc I path input for 18dB of PGA
        volatile unsigned long dc_q_18db        : 8; //RX dcoc Q path input for 18dB of PGA
        volatile unsigned long dc_i_21db        : 8; //RX dcoc I path input for 21dB of PGA
        volatile unsigned long dc_q_21db        : 8; //RX dcoc Q path input for 21dB of PGA
    } bits;
    volatile unsigned int value;
} BK7011_TRxV2A_REG0x17_TypeDef;

/// REG0x18
typedef union
{
    struct
    {
        volatile unsigned long dc_i_24db        : 8; //RX dcoc I path input for 24dB of PGA
        volatile unsigned long dc_q_24db        : 8; //RX dcoc Q path input for 24dB of PGA
        volatile unsigned long dc_i_27db        : 8; //RX dcoc I path input for 27dB of PGA
        volatile unsigned long dc_q_27db        : 8; //RX dcoc Q path input for 27dB of PGA
    } bits;
    volatile unsigned int value;
} BK7011_TRxV2A_REG0x18_TypeDef;

/// REG0x19
typedef union
{
    struct
    {
        volatile unsigned long dc_i_30db        : 8; //RX dcoc I path input for 30dB of PGA
        volatile unsigned long dc_q_30db        : 8; //RX dcoc Q path input for 30dB of PGA
        volatile unsigned long dc_i_33db        : 8; //RX dcoc I path input for 33dB of PGA
        volatile unsigned long dc_q_33db        : 8; //RX dcoc Q path input for 33dB of PGA
    } bits;
    volatile unsigned int value;
} BK7011_TRxV2A_REG0x19_TypeDef;

/// REG0x1A
typedef union
{
    struct
    {
        volatile unsigned long dc_i_36db        : 8; //RX dcoc I path input for 36dB of PGA
        volatile unsigned long dc_q_36db        : 8; //RX dcoc Q path input for 36dB of PGA
        volatile unsigned long dc_i_39db        : 8; //RX dcoc I path input for 39dB of PGA
        volatile unsigned long dc_q_39db        : 8; //RX dcoc Q path input for 39dB of PGA
    } bits;
    volatile unsigned int value;
} BK7011_TRxV2A_REG0x1A_TypeDef;

/// REG0x1B
typedef union
{
    struct
    {
        volatile unsigned long dc_i_42db        : 8; //RX dcoc I path input for 42dB of PGA
        volatile unsigned long dc_q_42db        : 8; //RX dcoc Q path input for 42dB of PGA
        volatile unsigned long dc_i_45db        : 8; //RX dcoc I path input for 45dB of PGA
        volatile unsigned long dc_q_45db        : 8; //RX dcoc Q path input for 45dB of PGA
    } bits;
    volatile unsigned int value;
} BK7011_TRxV2A_REG0x1B_TypeDef;

/// REG0x1C
typedef union
{
    struct
    {
        volatile unsigned long dac_out_qspi     : 10; //DAC Q channel input in analog test only;
        volatile unsigned long NC1              : 2;
        volatile unsigned long dac_out_ispi     : 10; //DAC I channel input in analog test only;
        volatile unsigned long NC2              : 10;
    } bits;
    volatile unsigned int value;
} BK7011_TRxV2A_REG0x1C_TypeDef;


/// BK7011TRxV2A
struct BK7231N_TRX_REG_TypeDef
{
    volatile BK7011_TRxV2A_REG0x0_TypeDef  *REG0x0;
    volatile BK7011_TRxV2A_REG0x1_TypeDef  *REG0x1;
    volatile BK7011_TRxV2A_REG0x2_TypeDef  *REG0x2;
    volatile BK7011_TRxV2A_REG0x3_TypeDef  *REG0x3;
    volatile BK7011_TRxV2A_REG0x4_TypeDef  *REG0x4;
    volatile BK7011_TRxV2A_REG0x5_TypeDef  *REG0x5;
    volatile BK7011_TRxV2A_REG0x6_TypeDef  *REG0x6;
    volatile BK7011_TRxV2A_REG0x7_TypeDef  *REG0x7;
    volatile BK7011_TRxV2A_REG0x8_TypeDef  *REG0x8;
    volatile BK7011_TRxV2A_REG0x9_TypeDef  *REG0x9;
    volatile BK7011_TRxV2A_REG0xA_TypeDef  *REG0xA;
    volatile BK7011_TRxV2A_REG0xB_TypeDef  *REG0xB;
    volatile BK7011_TRxV2A_REG0xC_TypeDef  *REG0xC;
    volatile BK7011_TRxV2A_REG0xD_TypeDef  *REG0xD;
    volatile BK7011_TRxV2A_REG0xE_TypeDef  *REG0xE;
    volatile BK7011_TRxV2A_REG0xF_TypeDef  *REG0xF;
    volatile BK7011_TRxV2A_REG0x10_TypeDef *REG0x10;
    volatile BK7011_TRxV2A_REG0x11_TypeDef *REG0x11;
    volatile BK7011_TRxV2A_REG0x12_TypeDef *REG0x12;
    volatile BK7011_TRxV2A_REG0x13_TypeDef *REG0x13;
    volatile BK7011_TRxV2A_REG0x14_TypeDef *REG0x14;
    volatile BK7011_TRxV2A_REG0x15_TypeDef *REG0x15;
    volatile BK7011_TRxV2A_REG0x16_TypeDef *REG0x16;
    volatile BK7011_TRxV2A_REG0x17_TypeDef *REG0x17;
    volatile BK7011_TRxV2A_REG0x18_TypeDef *REG0x18;
    volatile BK7011_TRxV2A_REG0x19_TypeDef *REG0x19;
    volatile BK7011_TRxV2A_REG0x1A_TypeDef *REG0x1A;
    volatile BK7011_TRxV2A_REG0x1B_TypeDef *REG0x1B;
    volatile BK7011_TRxV2A_REG0x1C_TypeDef *REG0x1C;
};

typedef struct
{
    BK7011_TRxV2A_REG0x0_TypeDef  REG0x0;
    BK7011_TRxV2A_REG0x1_TypeDef  REG0x1;
    BK7011_TRxV2A_REG0x2_TypeDef  REG0x2;
    BK7011_TRxV2A_REG0x3_TypeDef  REG0x3;
    BK7011_TRxV2A_REG0x4_TypeDef  REG0x4;
    BK7011_TRxV2A_REG0x5_TypeDef  REG0x5;
    BK7011_TRxV2A_REG0x6_TypeDef  REG0x6;
    BK7011_TRxV2A_REG0x7_TypeDef  REG0x7;
    BK7011_TRxV2A_REG0x8_TypeDef  REG0x8;
    BK7011_TRxV2A_REG0x9_TypeDef  REG0x9;
    BK7011_TRxV2A_REG0xA_TypeDef  REG0xA;
    BK7011_TRxV2A_REG0xB_TypeDef  REG0xB;
    BK7011_TRxV2A_REG0xC_TypeDef  REG0xC;
    BK7011_TRxV2A_REG0xD_TypeDef  REG0xD;
    BK7011_TRxV2A_REG0xE_TypeDef  REG0xE;
    BK7011_TRxV2A_REG0xF_TypeDef  REG0xF;
    BK7011_TRxV2A_REG0x10_TypeDef REG0x10;
    BK7011_TRxV2A_REG0x11_TypeDef REG0x11;
    BK7011_TRxV2A_REG0x12_TypeDef REG0x12;
    BK7011_TRxV2A_REG0x13_TypeDef REG0x13;
    BK7011_TRxV2A_REG0x14_TypeDef REG0x14;
    BK7011_TRxV2A_REG0x15_TypeDef REG0x15;
    BK7011_TRxV2A_REG0x16_TypeDef REG0x16;
    BK7011_TRxV2A_REG0x17_TypeDef REG0x17;
    BK7011_TRxV2A_REG0x18_TypeDef REG0x18;
    BK7011_TRxV2A_REG0x19_TypeDef REG0x19;
    BK7011_TRxV2A_REG0x1A_TypeDef REG0x1A;
    BK7011_TRxV2A_REG0x1B_TypeDef REG0x1B;
    BK7011_TRxV2A_REG0x1C_TypeDef REG0x1C;
} BK7231N_TRX_TypeDef;
#endif

/// POWER
typedef struct
{
    unsigned int regc_4_6   : 4;
    unsigned int regc_8_10  : 4;
    unsigned int pregain    : 12;
    unsigned int unuse      : 12;
} PWR_REGS;

typedef unsigned int PWR_REGS_TPC;

typedef struct
{
    INT32 gtx_dc_n;//the times of first dc cal. (64 * BK_TX_DAC_COEF)=2^8
    UINT32 gst_rx_adc;
    UINT32 gst_sar_adc;

    UINT32 cali_mode;
    INT32 gtx_tssi_thred_b;
    INT32 gtx_tssi_thred_g;

    UINT32 is_tpc_used;

#if CFG_USE_TEMPERATURE_DETECT
    INT16 ble_pwr_indx;
    INT16 ble_pwr_shift;
#endif
    UINT32 device_id;
} BK7011_CALI_CONTEXT;

typedef struct
{
    INT32 gbias_after_cal;
    INT32 gav_tssi;

    INT32 gtx_i_dc_comp;
    INT32 gtx_q_dc_comp;
    INT32 gtx_i_dc_comp_loopback;
    INT32 gtx_q_dc_comp_loopback;

    INT32 gtx_i_gain_comp;
    INT32 gtx_q_gain_comp;
    INT32 gtx_i_gain_comp_loopback;
    INT32 gtx_q_gain_comp_loopback;

    INT32 gtx_phase_comp;
    INT32 gtx_phase_ty2;
    INT32 gtx_phase_comp_loopback;
    INT32 gtx_phase_ty2_loopback;

    INT32 gtx_ifilter_corner;
    INT32 gtx_qfilter_corner;

    INT32 const_iqcal_p;

    INT32 grx_amp_err_wr;
    INT32 grx_phase_err_wr;

#ifdef SCAN_IQ_FILTER_CORNER
    UINT32 g_sctrl_bias_reg;
    INT32  grx_avg_i_14M;
    INT32  grx_avg_i_14M2;
    INT32  grx_avg_q_14M;
    INT32  grx_avg_q_14M2;
    INT32  index_test[5];
    float  float2_test[5];
#endif

    INT32  rx_amp_err_rd;
    INT32  rx_phase_err_rd;
    INT32  rx_ty2_rd;

    INT32 g_rx_dc_gain_tab[8];
} BK7011_CALI_RESULT;

/*******************************************************************************
* Function Declarations
*******************************************************************************/
void rwnx_cal_en_rx_filter_offset(void);
void rwnx_cal_set_bw_i2v(int enable);
void rwnx_cal_dis_rx_filter_offset(void);

#endif // (CFG_SOC_NAME != SOC_BK7231)

#endif // _BK7231U_CAL_H_
