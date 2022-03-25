/**
* @file light_init.c
* @author www.tuya.com
* @brief light initialization program
* @version 0.2
* @date 2022-03-25
*
* @copyright Copyright (c) tuya.inc 2022
*
*/

#include "light_printf.h"
#include "light_types.h"
#include "light_control.h"
#include "device_config_load.h"
#include "light_system.h"


STATIC BOOL_T g_light_init_flag = FALSE;    //初始化灯的标志

#define CONFIG_SET_BY_USER  0

#if (CONFIG_SET_BY_USER == 1) /* user set config */

#define USER_SET_CMOD_C          0x01
#define USER_SET_CMOD__CW        0x02
#define USER_SET_CMOD__RGB       0x03
#define USER_SET_CMOD__RGBC      0x04
#define USER_SET_CMOD_RGBCW      0x05


#define USER_SET_DMOD_PWM        0x00
#define USER_SET_DMOD_SM16726    0x01
#define USER_SET_DMOD_SM2135     0x02
#define USER_SET_DMOD_SM2135EH   0x03
#define USER_SET_DMOD_SM2135EJ   0x04
#define USER_SET_DMOD_BP1658CJ   0x05

#define DEMO_LIGHT_WAY USER_SET_CMOD_RGBCW
#define DEMO_LIHGT_DRIVE_MODE USER_SET_DMOD_PWM

/**
 * @brief: 用户设备配置
 * @param：无 
 * @return: OPERATE_LIGHT 0表示成功，非0请参照tuya error code描述文档 
 */
STATIC VOID _light_cfg_set_demo(VOID) 
{
    OPERATE_LIGHT ret = -1;
    device_config_t device_cfg;

    memset(&device_cfg, 0, SIZEOF(device_cfg));

#if (DEMO_LIGHT_WAY == CMOD_C)
    //light C                       
    memcpy(device_cfg.cmod, "c", sizeof(device_cfg.cmod)); // don't modify! 
    device_cfg.dmod = DMOD_PWM;                     // don't modify! 
    device_cfg.cwtype = CW_TYPE;                    // don't modify! 
    device_cfg.onoffmode = CHANGE_GRADUALLY;//CHANGE_DIRECTLY;
    device_cfg.pmemory = MEM_SAVE;//MEM_SAVE_NOT;   

#if (LIGHT_CFG_SUPPORT_LOWPOWER == 1)
    device_cfg.title20 = 0;//1;                     //this function need config support in light_cfg.h
#endif

    device_cfg.defcolor = 'c';                  // don't modify!
    device_cfg.defbright = 100;                     // default brightness
    device_cfg.deftemp = 100;                       // don't modify!
    device_cfg.cwmaxp = 100;                        // don't modify!
    
    device_cfg.brightmin = 10;                      // Minimum brightness: 1~100
    device_cfg.brightmax = 100;                     // Maximum brightness: 1~100
    device_cfg.colormax = 10;                       // don't modify!
    device_cfg.colormin = 100;                      // don't modify!
    
    memset(device_cfg.wfcfg, 0, SIZEOF(device_cfg.wfcfg));
    memcpy(device_cfg.wfcfg, "spcl", strlen("spcl"));                      //GWCM_SPCL_MODE
    device_cfg.remdmode = 0;                        //blink mode
    device_cfg.rstnum = 3;                          //The number of resets triggers the distribution network
    device_cfg.rstcor = 'c';                        //The color of the light in the distribution mode
    device_cfg.rstbr = 50;                          //The brightness of the light during network distribution
    device_cfg.rsttemp = 100;                       //The color temperature of the light during network distribution

    device_cfg.pwmhz = 1000;                        //pwm phase 1k(unit:hz)
    device_cfg.c_pin = 6;                           // this io need adjust by platform, io num
    device_cfg.c_lv = 1;                            // high level active

    device_cfg.ctrl_pin = PIN_NOEXIST;              // don't has ctrl pin

#endif

#if (DEMO_LIGHT_WAY == CMOD_CW)
    // light CW
    memcpy(device_cfg.cmod, "cw", sizeof(device_cfg.cmod));                     // don't modify! 
    device_cfg.dmod = DMOD_PWM;                     // don't modify! 
    device_cfg.cwtype = CW_TYPE;                    //CCT_TYPE;  // CW or CCT 
    device_cfg.onoffmode = CHANGE_GRADUALLY;        //CHANGE_DIRECTLY;
    device_cfg.pmemory = MEM_SAVE;                  //MEM_SAVE_NOT;

    device_cfg.title20 = 0;                         //this function need config support in light_cfg.h

    device_cfg.defcolor = 'c';                  // don't modify!
    device_cfg.defbright = 100;                     // default brightness
    device_cfg.deftemp = 100;                       // don't modify!
    device_cfg.cwmaxp = 100;                        // don't modify!
    
    device_cfg.brightmin = 10;                      // Minimum brightness: 1~100
    device_cfg.brightmax = 100;                     // Maximum brightness: 1~100
    device_cfg.colormax = 10;                       // don't modify!
    device_cfg.colormin = 100;                      // don't modify!

    memset(device_cfg.wfcfg, 0, SIZEOF(device_cfg.wfcfg));
    memcpy(device_cfg.wfcfg, "spcl", strlen("spcl"));                      //GWCM_SPCL_MODE
    device_cfg.remdmode = 0;                        //blink mode
    device_cfg.rstnum = 3;                          //The number of resets triggers the distribution network
    device_cfg.rstcor = 'c';                        //The color of the light in the distribution mode
    device_cfg.rstbr = 50;                          //The brightness of the light during network distribution
    device_cfg.rsttemp = 100;                       //The color temperature of the light during network distribution      

    device_cfg.pwmhz = 1000;                        //pwm phase 1k(unit:hz)
    device_cfg.c_pin = 6;                           // this io need adjust by platform, io num
    device_cfg.c_lv = 1;                            // high level active

    device_cfg.w_pin = 8;                           // this io need adjust by platform, io num
    device_cfg.w_lv = 1;                            // high level active

    device_cfg.ctrl_pin = PIN_NOEXIST;              // don't has ctrl pin
#endif

#if (DEMO_LIGHT_WAY == CMOD_RGBCW)

    #if (DEMO_LIHGT_DRIVE_MODE == DMOD_PWM)
        // light RGB
        memcpy(device_cfg.cmod, "rgbcw", sizeof(device_cfg.cmod));                  // don't modify! 
        device_cfg.dmod = DMOD_PWM;                    
        device_cfg.cwtype = CW_TYPE;//CCT_TYPE;         
        device_cfg.onoffmode = CHANGE_GRADUALLY;//CHANGE_DIRECTLY;
        device_cfg.pmemory = MEM_SAVE;//MEM_SAVE_NOT;

    #if (LIGHT_CFG_SUPPORT_LOWPOWER == 1)
        device_cfg.title20 = 0;//1;                     //this function need config support in light_cfg.h
    #endif

        device_cfg.defcolor = 'c';                  //COLOR_W;//COLOR_R;//COLOR_G;//COLOR_B 
        device_cfg.defbright = 100;
        device_cfg.deftemp = 100;                       
        device_cfg.cwmaxp = 100;                        // don't modify!
        
        device_cfg.brightmin = 10;                      // Minimum brightness: 1~100
        device_cfg.brightmax = 100;                     // Minimum brightness: 1~100
        device_cfg.colormax = 100;                       // Minimum brightness: 1~100
        device_cfg.colormin = 10;                      // Maximum brightness: 1~100

        memset(device_cfg.wfcfg, 0, SIZEOF(device_cfg.wfcfg));
        memcpy(device_cfg.wfcfg, "spcl", strlen("spcl"));                      //GWCM_SPCL_MODE
        device_cfg.remdmode = 0;                        //blink mode
        device_cfg.rstnum = 3;                          //The number of resets triggers the distribution network
        device_cfg.rstcor = 'c';                        //The color of the light in the distribution mode
        device_cfg.rstbr = 50;                          //The brightness of the light during network distribution
        device_cfg.rsttemp = 100;                       //The color temperature of the light during network distribution   
  
        device_cfg.pwmhz = 1000;                        //pwm phase 1k(unit:hz)

        device_cfg.r_pin = 26;                           // this io need adjust by platform, io num
        device_cfg.r_lv = 1;                            // high level active

        device_cfg.g_pin = 24;                          // this io need adjust by platform, io num
        device_cfg.g_lv = 1;                            // high level active

        device_cfg.b_pin = 6;                          // this io need adjust by platform, io num
        device_cfg.b_lv = 1;                            // high level active

        device_cfg.c_pin = 8;                           // this io need adjust by platform, io num
        device_cfg.c_lv = 1;                            // high level active

        device_cfg.w_pin = 7;                           // this io need adjust by platform, io num
        device_cfg.w_lv = 1;                            // high level active
        
        device_cfg.ctrl_pin = PIN_NOEXIST;              // don't has ctrl pin
    #endif
#endif
    //设备配置设置接口
    ret = tuya_device_cfg_set_struct(&device_cfg);
    if(ret != LIGHT_OK) {
        PR_ERR("Default oem config error!");
    }
}
#endif


/**
 * @brief: 灯硬件初始化
 *          get oem json set, and init hardware
 * @param：无 
 * @return: OPERATE_LIGHT 0表示成功，非0请参照tuya error code描述文档 
 */
STATIC OPERATE_LIGHT _light_hardware_init(VOID)
{
    OPERATE_LIGHT ret = -1;
#ifdef _IS_OEM  //load config from flash! 
    //get oem get json!! 
    ret = tuya_device_cfg_data_load();
    if (ret != LIGHT_OK) {
        PR_NOTICE("oem cfg load error!");
        return LIGHT_COM_ERROR;
    }
#else //set configuration by user
    #if (CONFIG_SET_BY_USER == 1)
    _light_cfg_set_demo(); 
    #else
        //设备配置设置接口
        ret = tuya_device_cfg_set_string(strlen(DEFAULT_CONFIG), DEFAULT_CONFIG);
        if (ret != LIGHT_OK) {
            PR_ERR("Default oem config error!");
        }
    #endif
#endif

    light_ctrl_cfg_t light_config_data = {0};
    
    /************ production cfg ************/
    light_config_data.light_way = tuya_cfg_get_color_mode();    //获取灯的类型
    /************ production cfg ************/

    /************ pairing cfg ************/
    light_config_data.connect_mode = tuya_cfg_get_wifi_cfg_mode();  //获取设备配网方式

    light_config_data.reset_cnt = tuya_cfg_get_reset_num();    //获取开关开重置的次数

    light_config_data.net_color = tuya_cfg_get_reset_color();   //获取重置时（配网状态下）灯光颜色

    light_config_data.net_bright = tuya_cfg_get_reset_brightness_value();    //获取重置时（配网状态下）亮度值

    light_config_data.net_temper = tuya_cfg_get_reset_temperature_value();    //获取重置时（配网状态下）色温值

    light_config_data.remind_mode = tuya_cfg_get_remind_mode();    //获取设备配网提醒式样的方式

    light_config_data.remind_time = tuya_cfg_get_remind_time_sec();    //获取设备配网提醒式样持续时长

    light_config_data.pair_time = tuya_cfg_get_pairing_time_min();    //获取设备配网时长

    /************ pairing cfg ************/

    
    /************ production light cfg ************/

    light_config_data.def_color = tuya_cfg_get_default_color();    //获取灯（未配网状态下或者配网上初始）默认颜色

    light_config_data.def_bright = tuya_cfg_get_default_brightness();   //获取灯（未配网状态下或者配网上初始）默认亮度值

    light_config_data.def_temper = tuya_cfg_get_default_temperature();    //获取灯（未配网状态下或者配网上初始）默认色温值    

    light_config_data.switch_mode = tuya_cfg_get_onoff_change_mode();    //获取设备上电点灯灯光变化模式  

    light_config_data.memory = tuya_cfg_get_memory_mode();    //获取设备断电记忆模式
    /************ production light cfg ************/
    
    /************ production hardware cfg ************/

    light_config_data.bright_mode = tuya_cfg_get_cw_type();    //获取灯冷暖光驱动方式

    light_config_data.limit_cw_max = tuya_cfg_get_white_max();    //获取灯白光最大亮度值

    light_config_data.limit_cw_min = tuya_cfg_get_white_min();    //获取灯白光最小亮度值

    light_config_data.limit_rgb_max = tuya_cfg_get_color_max();    //获取灯RGB彩光最大亮度值
    
    light_config_data.limit_rgb_min = tuya_cfg_get_color_min();    //获取灯RGB彩光最低亮度值
    
#if (LIGHT_CFG_SUPPORT_LOWPOWER == 1)    //是否支持低功耗

    light_config_data.title20 = tuya_cfg_get_title20_flag();    //获取设备是否支持T20低功耗标志位

#endif


    light_config_data.drive_cfg.mode = tuya_cfg_get_driver_mode();    //获取灯驱动模式

    light_config_data.power_gain = tuya_cfg_get_cw_max_power();    //获取灯cw两路功率放大倍数
    
    switch (light_config_data.drive_cfg.mode) 
    {
        case DRIVER_MODE_PWM: {  
            light_config_data.drive_cfg.config.pwm_init.freq = tuya_cfg_get_pwm_frequency();    //获取pwm驱动频率
            
            light_config_data.drive_cfg.config.pwm_init.list[0] = tuya_cfg_get_red_pin();    //获取红光IO口
            
            light_config_data.drive_cfg.config.pwm_init.list[1] = tuya_cfg_get_green_pin();    //获取绿光IO口
            
            light_config_data.drive_cfg.config.pwm_init.list[2] = tuya_cfg_get_blue_pin();    //获取蓝光IO口
            
            light_config_data.drive_cfg.config.pwm_init.list[3] = tuya_cfg_get_cold_pin();    //获取冷光IO口
            
            light_config_data.drive_cfg.config.pwm_init.list[4] = tuya_cfg_get_warm_pin();    //获取暖光IO口
            
            light_config_data.drive_cfg.config.pwm_init.channel_num = tuya_cfg_get_color_mode();    //获取灯的类型，如RGBCW5路灯，CW两路灯。
            
            light_config_data.drive_cfg.config.pwm_init.cct_flag = tuya_cfg_get_cw_type();    //获取灯冷暖光驱动方式
            
            if (light_config_data.light_way == LIGHT_RGB) {  
                light_config_data.drive_cfg.config.pwm_init.polarity = tuya_cfg_get_red_pin_active_level();    //获取红光驱动IO口有效电平
                light_config_data.drive_cfg.config.pwm_init.duty = (tuya_cfg_get_red_pin_active_level() == 1) ?  0 : 1000;  //获取pwm占空值  

            } else {
                light_config_data.drive_cfg.config.pwm_init.polarity = tuya_cfg_get_cold_pin_active_level();    //获取冷光驱动IO口有效电平
                light_config_data.drive_cfg.config.pwm_init.duty = (tuya_cfg_get_cold_pin_active_level() == 1) ?  0 : 1000;    //获取pwm占空值  
            }

            light_config_data.drive_cfg.config.pwm_init.ctrl_pin = tuya_cfg_get_ctrl_pin();    //获取设备控制管脚IO

            light_config_data.drive_cfg.config.pwm_init.ctrl_level = tuya_cfg_get_ctrl_pin_active_level();    //获取设备控制管脚有效电平

            break;
        }

        default: {
            PR_ERR("Driver mode ERROR");
            break;
        }
    }

    //灯的控制初始化
    ret = tuya_light_ctrl_init(&light_config_data);
    if (ret != LIGHT_OK) {
        PR_ERR("Light control init error!");
        return LIGHT_COM_ERROR;
    }
    return LIGHT_OK;

}

/**
 * @brief: 灯软件初始化
 *          this func will call tuya_light_sys_software_init()
 * @attention: tuya_light_sys_software_init()需要被系统执行
 * @param: none 
 * @return: OPERATE_LIGHT 0表示成功，非0请参照tuya error code描述文档 
 */
STATIC OPERATE_LIGHT _light_software_init(VOID)
{
    OPERATE_LIGHT ret = -1;

    //light software resource init
    ret = tuya_light_sys_software_init();
    if (ret != LIGHT_OK) {
        PR_ERR("Light software init error!");
    }
    
    return ret;
}

/**
 * @brief : 灯初始化
 * @param : none 
 * @return: OPERATE_LIGHT 0表示成功，非0请参照tuya error code描述文档 
 */
OPERATE_LIGHT tuya_light_init(VOID)
{
    OPERATE_LIGHT ret = -1;

    //灯是否需要初始化
    if (g_light_init_flag) {
        PR_NOTICE("Light already init");
        return LIGHT_OK;
    }

    //灯硬件初始化
    ret = _light_hardware_init();
    if (ret != LIGHT_OK) {
        PR_ERR("Light hardware init error!");
        return LIGHT_COM_ERROR;
    }

    //灯软件初始化
    ret = _light_software_init();
    if (ret != LIGHT_OK) {
        PR_ERR("Light software init error!");
        return LIGHT_COM_ERROR;
    }

    //表示灯已初始化
    g_light_init_flag = TRUE;
    
    return LIGHT_OK;
}


