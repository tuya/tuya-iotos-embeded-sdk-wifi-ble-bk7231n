/**
* @file device_config_load.h
* @author www.tuya.com
* @brief Light device configuration item processing module
* @version 0.2
* @date 2022-03-25
*
* @copyright Copyright (c) tuya.inc 2022
*
*/

#ifndef __DEVICE_CONFIG_LOAD__
#define __DEVICE_CONFIG_LOAD__

#include "stdio.h"
#include "stdlib.h"
#include "light_types.h"
#include "light_cfg.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */


typedef BYTE_T CMODE_E; 
#define CMOD_C      0x01
#define CMOD_CW     0x02
#define CMOD_RGB    0x03
#define CMOD_RGBC   0x04
#define CMOD_RGBCW  0x05

	
typedef BYTE_T DMODE_E; 
#define DMOD_PWM            0x00
#define DMOD_IIC_SM16726    0x01
#define DMOD_IIC_SM2135     0x02
#define DMOD_IIC_SM2135EH   0x03
#define DMOD_IIC_SM2135EJ   0x04
#define DMOD_IIC_NP1658CJ   0x05


typedef BYTE_T WFCFG_E; 
#define GWCM_OLD            0x00
#define GWCM_LOW_POWER      0x01
#define GWCM_SPCL_MODE      0x02
#define GWCM_OLD_PROD       0x03
#define GWCM_SPCL_AUTOCFG   0x04


typedef BYTE_T COLOR_E; 
#define COLOR_C         0x00
#define COLOR_W         0x01
#define COLOR_R         0x02
#define COLOR_G         0x03
#define COLOR_B         0x04
#define COLOR_RGB       0x05


typedef BYTE_T CWTYPE_E; 
#define CW_TYPE     0
#define CCT_TYPE    1


typedef BYTE_T ONOFFMODE_E; 
#define CHANGE_GRADUALLY    0
#define CHANGE_DIRECTLY     1


typedef BYTE_T MEMORY_E; 
#define MEM_SAVE_NOT    0
#define MEM_SAVE        1


#pragma pack(1)
typedef struct {
//common
    CHAR_T Jsonver[5];          // json version
    CHAR_T category[5];         // ble(sigmesh) dedicated

//device function 
    CHAR_T  module[15];         // module choosed for the light
    CHAR_T  cmod[6];            // Color model: 1 -> C; 2 -> CW; 3 -> RGB; 4 -> RGBC; 5 ->RGBCW;
    UCHAR_T dmod;               // Color driver mode: 0->pwm; 1->sm16726b; 2->sm2135;
    UCHAR_T cwtype;             // Color temperature drive mode: 0 -> CW; 1: -> CCT;
    UCHAR_T onoffmode;          // Is there a gradient when switching: 0 -> turn on gradually; 1 -> turn ondirectly;
    UCHAR_T pmemory;            // Is there a power-off memory: 1 -> save app data; 0 -> don't save
    UCHAR_T ctrl_pin;           // CTRL pin: 
    UCHAR_T ctrl_lv;            // CTRL pin level
    
//light config
    CHAR_T  defcolor;           // light color after connected
    UCHAR_T defbright;          // light brightness after connected
    UCHAR_T deftemp;            // light default temperature
    UCHAR_T cwmaxp;             // Maximum Power configuration of Cold and warm Light mixing
    UCHAR_T brightmin;          // Minimum brightness: 1~100
    UCHAR_T brightmax;          // Maximum brightness: 1~100
    UCHAR_T colormax;           // Minimum brightness: 1~100
    UCHAR_T colormin;           // Maximum brightness: 1~100

//connection config
    CHAR_T  wfcfg[10];          // Low power / flash, value: spcl,prod
    UCHAR_T remdmode;           // light reset pairing mode
    UCHAR_T rstnum;             // number of times required to reset by switching on and off
    CHAR_T  rstcor;             // light color while connecting
    UCHAR_T rstbr;              // light brightness while connecting
    UCHAR_T rsttemp;            // light brightness while connecting
    USHORT_T remdtime;          // light reset pairing reminde time,unit:sec
    UCHAR_T wfptime;            // light pairing time,unit:min

//pwm config
    UINT_T  pwmhz;              // PWM frequency
    UCHAR_T r_pin;              // Red color control pin
    UCHAR_T r_lv;               // Red color control pin level
    UCHAR_T g_pin;              // Green color control pin
    UCHAR_T g_lv;               // Green color control pin level
    UCHAR_T b_pin;              // Blue  color control pin
    UCHAR_T b_lv;               // Blue  color control pin level
    UCHAR_T c_pin;              // Cold white color control pin
    UCHAR_T c_lv;               // Cold white color control pin level
    UCHAR_T w_pin;              // Warm white color control pin
    UCHAR_T w_lv;               // Warm white color control pin level

    
#if (LIGHT_CFG_SUPPORT_LOWPOWER == 1)
    UCHAR_T title20;            // title20 flag: 0 -> don't support T20; 1 -> support T20
#endif

#if LIGHT_CFG_ENABLE_GAMMA
//Gamma param
    UCHAR_T gamma_red_buf;
    UCHAR_T gamma_green_buf;
    UCHAR_T gamma_blue_buf;
#endif

#if (LIGHT_CFG_GAMMA_CAL == 1)
    UCHAR_T gammakr;
    UCHAR_T gammakg;
    UCHAR_T gammakb;

    UCHAR_T gammawr;
    UCHAR_T gammawg;
    UCHAR_T gammawb;
#endif

}device_config_t;
#pragma pack()

/**
 * @brief: 设备配置设置接口
 * @param[in]: p_config 配置结构体
 * @return: OPERATE_RET 0表示成功，非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_device_cfg_set_struct(IN device_config_t *p_config);

/**
 * @brief: 设备配置设置接口(string)
 * @param[in]: len 配置字符串长度, p_config 指向要配置的字符串
 * @return: OPERATE_RET 0表示成功，非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_device_cfg_set_string(IN USHORT_T len, IN CONST CHAR_T *p_config);

/**
 * @brief: 设备配置项装载处理，配置文件可以通过配置文件获取或者是DEFAULT_CONFIG宏定义设置
 * @param[in]: none
 * @return: OPERATE_RET 0表示成功，非0请参照tuya error code描述文档
 */
OPERATE_LIGHT tuya_device_cfg_data_load(VOID);

/**
 * @brief: 获取json配置的版本号
 * @param[out]: p_json_ver json 版本号字符串
 * @return: p_json_ver json 版本号字符串
 */
CHAR_T tuya_cfg_get_json_ver(OUT CHAR_T *p_json_ver);

/**
 * @brief: 获取设备的集合属性
 * @param[out]: p_category 集合字符串
 * @note: 该集合属性可以不用关注
 * @return: p_category 集合字符串
 */
CHAR_T tuya_cfg_get_category(OUT CHAR_T *p_category);

/**
 * @brief: 获取设备soc使用模组型号
 * @param[out]: p_module 模组型号字符串, p_len 模组型号字符串长度
 * @return: p_module 模组型号字符串, p_len 模组型号字符串长度
 */
CHAR_T tuya_cfg_get_module_code(OUT CHAR_T *p_module, OUT UCHAR_T *p_len);

/**
 * @brief: 获取灯的类型，如RGBCW5路灯，CW两路灯。
 *         种类有：RGBCW/RGBC/RGB/CW/C
 * @param: none
 * @return: 0x01 -> C, 
 *          0x02 -> CW
 *          0x03 -> RGB
 *          0x04 -> RGBC
 *          0x05 -> RGBCW
 *          -1   -> not find light way,error
 */
CHAR_T tuya_cfg_get_color_mode(VOID);

/**
 * @brief: 获取灯驱动模式
 * @param: none
 * @note: 此demo仅支持pwm驱动方式，其他驱动可联系涂鸦获取或者自行实现
 * @return: 
 *          0x00 -> PWM
 *          0x01 -> SM16726B
 *          0x02 -> SM2135e
 *          0x03->sm2135eh
 *          0x04->sm2135ej
 *          0x05->bp1658cj;
 *          -1   -> configure not load, error
 */
CHAR_T tuya_cfg_get_driver_mode(VOID);

/**
 * @brief: 获取灯冷暖光驱动方式
 *          cw 驱动是纯软件计算互补的方式
 *          cct 驱动是硬件实现互补
 * @param: none
 * @return: 
 *          0x00 -> CW drive
 *          0x01 -> CCT drive
 */
CHAR_T tuya_cfg_get_cw_type(VOID);

/**
 * @brief: 获取设备上电点灯灯光变化模式
 * @param: none
 * @return:  
 *          0x00 -> turn on/off change gradually（上电点灯和模式切换是渐变的）
 *          0x01 -> turn on/off change directly（上电点灯是跳变，模式切换是渐变）
 *          -1   -> configure not load, error
 */
CHAR_T tuya_cfg_get_onoff_change_mode(VOID);

/**
 * @brief: 获取设备断电记忆模式
 * @param: none
 * @return: 
 *          0x00 -> don't save app data in flash
 *          0x01 -> save app data in flash
 *          -1   -> configure not load, error
 */
CHAR_T tuya_cfg_get_memory_mode(VOID);

/**
 * @brief: 获取设备控制管脚IO
 * @param: none
 * @note: 控制管脚可以结合硬件设计用来做为驱动IC的电源控制或者灯开关的指示
 * @return: 控制管脚IO, -1 -> configure not load, error
 */
CHAR_T tuya_cfg_get_ctrl_pin(VOID);

/**
 * @brief: 获取设备控制管脚有效电平
 * @param: none
 * @return: 1 -> 高电平有效, 0 -> 低电平有效, -1 -> not find,error
 */
CHAR_T tuya_cfg_get_ctrl_pin_active_level(VOID);

/**
 * @brief: 获取灯（未配网状态下或者配网上初始）默认颜色
 * @param[in]: none
 * @return: 
 *          0x00 -> default bright is C when connected.
 *          0x01 -> default bright is W when connected.
 *          0x02 -> default bright is R when connected.
 *          0x03 -> default bright is G when connected.
 *          0x04 -> default bright is B when connected.
 *          -1   -> configure not load, error
 */
CHAR_T tuya_cfg_get_default_color(VOID);

/**
 * @brief: 获取灯（未配网状态下或者配网上初始）默认亮度值
 * @param: none
 * @note: 默认亮度值不光影响CW白光的亮度，也是RGB彩光的亮度
 * @return: 默认亮度值, 范围: 0~100, -1 -> configure not load, error
 */
CHAR_T tuya_cfg_get_default_brightness(VOID);

/**
 * @brief: 获取灯（未配网状态下或者配网上初始）默认色温值
 * @param: none
 * @return: 默认色温值, 范围: 0~100, -1 -> configure not load, error
 */
CHAR_T tuya_cfg_get_default_temperature(VOID);

/**
 * @brief: 获取灯cw两路功率放大倍数
 * @param: none
 * @return: 功率放大倍数, 范围: 100~200
 */
CHAR_T tuya_cfg_get_cw_max_power(VOID);

/**
 * @brief: 获取灯白光最小亮度值 
 * @param: none
 * @return: 白光最小亮度值, 范围：1~max, -1 -> configure not load, error
 */
CHAR_T tuya_cfg_get_white_min(VOID);

/**
 * @brief: 获取灯白光最大亮度值 
 * @param: none
 * @return: 白光最大亮度值, 范围：min~100, -1 -> configure not load, error
 */
CHAR_T tuya_cfg_get_white_max(VOID);

/**
 * @brief: 获取灯RGB彩光最低亮度值 
 * @param: none
 * @note: 此亮度值并不是意味着R、G、B三色最低值，而是HSV中V的最低值
 * @return: RGB彩光最低亮度值, 范围：1~max
 */
CHAR_T tuya_cfg_get_color_min(VOID);

/**
 * @brief: 获取灯RGB彩光最大亮度值 
 * @param: none
 * @note: 此亮度值并不是意味着R、G、B三色最大值，而是HSV中V的最大值
 * @return: RGB彩光最大亮度值, 范围：min~100
 */
CHAR_T tuya_cfg_get_color_max(VOID);

/**
 * @brief: 获取设备配网方式
 * @param: none
 * @return: 
 *      配网方式详见配网说明文档，简述如下：
 *          GWCM_OLD -> 主要用于通用对接，进入产测由mcu发起，任何时刻均进入产测。
 *          GWCM_LOW_POWER -> 主要用于SOC电工产品，上电常亮，需要手动切换到配网状态
 *          GWCM_SPCL_MODE -> 主要用于SOC照明产品，上电常亮，需要手动切换到配网状态
 *          GWCM_OLD_PROD -> 上电快闪（上电即进入配网状态）
 *          LOW_POWER_AUTO_CFG -> 闪烁默认3min后常亮，不具有防误触模式
 *          GWCM_SPCL_AUTOCFG -> 闪烁默认3min后常亮，具有防误触模式
 *          -1 -> not find, error
 */
CHAR_T tuya_cfg_get_wifi_cfg_mode(VOID);

/**
 * @brief: 获取设备配网提醒式样的方式
 *         当前支持的配网提醒式样有：闪烁、呼吸
 * @param: none
 * @return: 配网提醒式样, 0 -> 闪烁配网, 1 -> 呼吸配网
 */
CHAR_T tuya_cfg_get_remind_mode(VOID);
/**
 * @brief: 获取开关开重置的次数
 * @param: none
 * @return: reset number, -1 -> configure not load, error
 */
CHAR_T tuya_cfg_get_reset_num(VOID);

/**
 * @brief: 获取重置时（配网状态下）灯光颜色
 * @param: none
 * @return: 
 *          0x00 -> default bright is C when pairing.
 *          0x01 -> default bright is W when pairing.
 *          0x02 -> default bright is R when pairing.
 *          0x03 -> default bright is G when pairing.
 *          0x04 -> default bright is B when pairing.
 *          -1   -> not find, error
 */
CHAR_T tuya_cfg_get_reset_color(VOID);

/**
 * @brief: 获取重置时（配网状态下）亮度值
 * @param: none
 * @return: 白光模式下亮度值，范围：0~100, -1 -> not find, error
 */
CHAR_T tuya_cfg_get_reset_brightness_value(VOID);

/**
 * @brief: 获取重置时（配网状态下）色温值
 * @param: none
 * @return: 白光模式下色温值, 范围：0~100, -1 -> not find, error
 */
CHAR_T tuya_cfg_get_reset_temperature_value(VOID);

/**
 * @brief: 获取设备提醒配网式样持续时长
 * @param: none
 * @note: 提醒配网式样持续时长仅在呼吸配网方式中生效，快闪的方式不生效
 * @return: 设备提醒配网式样持续时长, 单位：sec
 */
SHORT_T tuya_cfg_get_remind_time_sec(VOID);

/**
 * @brief: 获取设备配网时长
 * @param: none
 * @note: 配网时长是指除去上电快闪的配网方式除外，进入配网状态时，配网状态的维持的时间
 *        配网方式具体详见WiFi配网方式文档
 * @return: 配网时长, 单位：min
 */
CHAR_T tuya_cfg_get_pairing_time_min(VOID);

/**
 * @brief: 获取pwm驱动频率
 * @param: none
 * @return: pwm频率, 单位：Hz, -1 -> not find, error 
 */
USHORT_T tuya_cfg_get_pwm_frequency(VOID);

/**
 * @brief: 获取红光IO口
 * @param: none
 * @return: 红光驱动IO号, -1 -> not find, error
 */
CHAR_T tuya_cfg_get_red_pin(VOID);

/**
 * @brief: 获取红光驱动IO口有效电平
 * @param: none
 * @note: RGBCW驱动有效电平在IOT平台上配置不允许有不相同的情况，
 *          在定制固件中可以进行个性定制修改
 * @return: 1 -> 高电平有效, 0 -> 低电平有效, -1 -> not find,error
 */
CHAR_T tuya_cfg_get_red_pin_active_level(VOID);

/**
 * @brief: 获取绿光IO口
 * @param: none
 * @return: 绿光驱动IO号, -1 -> not find, error
 */
CHAR_T tuya_cfg_get_green_pin(VOID);

/**
 * @brief: 获取绿光驱动IO口有效电平
 * @param: none
 * @note: RGBCW驱动有效电平在IOT平台上配置不允许有不相同的情况，
 *          在定制固件中可以进行个性定制修改
 * @return: 1 -> 高电平有效, 0 -> 低电平有效,  -1 -> not find, error
 */
CHAR_T tuya_cfg_get_green_pin_active_level(VOID);

/**
 * @brief: 获取蓝光IO口
 * @param: none
 * @return: 蓝光驱动IO号, -1 -> not find, error
 */
CHAR_T tuya_cfg_get_blue_pin(VOID);

/**
 * @brief: 获取蓝光驱动IO口有效电平
 * @param: none
 * @note: RGBCW驱动有效电平在IOT平台上配置不允许有不相同的情况，
 *          在定制固件中可以进行个性定制修改
 * @return: 1 -> 高电平有效, 0 -> 低电平有效,  -1 -> not find,error
 */
CHAR_T tuya_cfg_get_blue_pin_active_level(VOID);

/**
 * @brief: 获取冷光IO口
 * @param: none
 * @return: 冷光驱动IO号, -1 -> not find, error
 */
CHAR_T tuya_cfg_get_cold_pin(VOID);

/**
 * @brief: 获取冷光驱动IO口有效电平
 * @param: none
 * @note: RGBCW驱动有效电平在IOT平台上配置不允许有不相同的情况，
 *          在定制固件中可以进行个性定制修改
 * @return: 1 -> 高电平有效, 0 -> 低电平有效, -1 -> not find,error
 */
CHAR_T tuya_cfg_get_cold_pin_active_level(VOID);

/**
 * @brief: 获取暖光IO口
 * @param: none
 * @return: 暖光驱动IO号, -1 -> not find, error
 */
CHAR_T tuya_cfg_get_warm_pin(VOID);

/**
 * @brief: 获取暖光驱动IO口有效电平
 * @param: none
 * @note: RGBCW驱动有效电平在IOT平台上配置不允许有不相同的情况，
 *          在定制固件中可以进行个性定制修改
 * @return: 1 -> 高电平有效, 0 -> 低电平有效, -1 -> not find,error
 */
CHAR_T tuya_cfg_get_warm_pin_active_level(VOID);


#if (LIGHT_CFG_SUPPORT_LOWPOWER == 1)
/**
 * @brief: 获取设备是否支持T20低功耗标志位
 * @param: none
 * @note: T20 低功耗功能是指开启之后，可以实现单模组在联网状态下，平均电流小于15mA以下
 * @return: 
 *          0x00 -> don't support lowpower mode
 *          0x01 -> support lowpower mode
 *          -1   -> configure not load, error
 */
CHAR_T tuya_cfg_get_title20_flag(VOID);
#endif

#if (LIGHT_CFG_ENABLE_GAMMA == 1)
/**
 * @brief: 获取红光的伽马值
 * @param[in]: index 伽马表的下标，范围0~255
 * @return: 过伽马校准的伽马值
 */
UCHAR_T tuya_cfg_get_gamma_red(IN UCHAR_T index);

/**
 * @brief: 获取绿光的伽马值
 * @param[in]: 伽马表的下标，范围0~255
 * @return: 经过伽马校准的伽马值
 */
UCHAR_T tuya_cfg_get_gamma_green(IN UCHAR_T index);

/**
 * @brief: 获取蓝光的伽马值
 * @param[in]: 伽马表的下标，范围0~255
 * @return: 经过伽马校准的伽马值
 */
UCHAR_T tuya_cfg_get_gamma_blue(IN UCHAR_T index);

#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __DEVICE_CONFIG_LOAD__ */



