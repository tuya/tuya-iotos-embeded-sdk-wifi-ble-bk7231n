/*
 * @Author: yj 
 * @email: shiliu.yang@tuya.com
 * @LastEditors: yj 
 * @file name: tuya_device.c
 * @Description: template demo for SDK WiFi & BLE for BK7231T
 * @Copyright: HANGZHOU TUYA INFORMATION TECHNOLOGY CO.,LTD
 * @Company: http://www.tuya.com
 * @Date: 2021-01-23 16:33:00
 * @LastEditTime: 2021-01-27 17:00:00
 */

#define _TUYA_DEVICE_GLOBAL

/* Includes ------------------------------------------------------------------*/
#include "uni_log.h"
#include "tuya_iot_wifi_api.h"
#include "tuya_hal_system.h"
#include "tuya_iot_com_api.h"
#include "tuya_cloud_com_defs.h"
#include "gw_intf.h"
#include "gpio_test.h"
#include "tuya_gpio.h"
#include "tuya_key.h"
#include "tuya_led.h"
#include "wlan_ui_pub.h"

#include "lwip/sockets.h"
#include "lwip/ip_addr.h"
#include "lwip/inet.h"

#include "mem_pub.h"
#include "str_pub.h"
#include "ethernet_intf.h"


#include "../../beken378/func/key/multi_button.h"
#include "../../beken378/app/config/param_config.h"
#include "lwip/netdb.h"

static int g_secondsElapsed = 0;
static int openAPAfterDelay = 5;

beken_timer_t led_timer;

const char *CFG_GetDeviceName() {
	return "obk_demo_accessPoint";
}

static int setup_wifi_open_access_point(void)
{
    #define APP_DRONE_DEF_SSID          "OpenBK2731N_WiFi_Demo"
    #define APP_DRONE_DEF_NET_IP        "192.168.4.1"
    #define APP_DRONE_DEF_NET_MASK      "255.255.255.0"
    #define APP_DRONE_DEF_NET_GW        "192.168.4.1"
    #define APP_DRONE_DEF_CHANNEL       1    
    
    general_param_t general;
    ap_param_t ap_info;
    network_InitTypeDef_st wNetConfig;
    int len;
    u8 *mac;
    
    os_memset(&general, 0, sizeof(general_param_t));
    os_memset(&ap_info, 0, sizeof(ap_param_t)); 
    os_memset(&wNetConfig, 0x0, sizeof(network_InitTypeDef_st));  
    
        general.role = 1,
        general.dhcp_enable = 1,

        os_strcpy((char *)wNetConfig.local_ip_addr, APP_DRONE_DEF_NET_IP);
        os_strcpy((char *)wNetConfig.net_mask, APP_DRONE_DEF_NET_MASK);
        os_strcpy((char *)wNetConfig.dns_server_ip_addr, APP_DRONE_DEF_NET_GW);
 

        mac = (u8*)&ap_info.bssid.array;
		    // this is MAC for Access Point, it's different than Client one
		    // see wifi_get_mac_address source
        wifi_get_mac_address((char *)mac, CONFIG_ROLE_AP);
        ap_info.chann = APP_DRONE_DEF_CHANNEL;
        ap_info.cipher_suite = 0;
        os_memcpy(ap_info.ssid.array, APP_DRONE_DEF_SSID, os_strlen(APP_DRONE_DEF_SSID));
		
        ap_info.key_len = 0;
        os_memset(&ap_info.key, 0, 65);   
  

    bk_wlan_ap_set_default_channel(ap_info.chann);

    len = os_strlen((char *)ap_info.ssid.array);

    os_strncpy((char *)wNetConfig.wifi_ssid, (char *)ap_info.ssid.array, sizeof(wNetConfig.wifi_ssid));
    os_strncpy((char *)wNetConfig.wifi_key, (char *)ap_info.key, sizeof(wNetConfig.wifi_key));
    
    wNetConfig.wifi_mode = SOFT_AP;
    wNetConfig.dhcp_mode = DHCP_SERVER;
    wNetConfig.wifi_retry_interval = 100;
    
	bk_wlan_start(&wNetConfig);

  return 0;    
}


static void app_led_timer_handler(void *data)
{
	g_secondsElapsed ++;
    bk_printf("ObkAccessDemo - timer is %i free mem %d\r\n", g_secondsElapsed, xPortGetFreeHeapSize());
	if(openAPAfterDelay>0)
	{
		openAPAfterDelay--;
		if(openAPAfterDelay == 0)
		{
			
			setup_wifi_open_access_point();
		}
	}
}


// receives status change notifications about wireless - could be useful
// ctxt is pointer to a rw_evt_type
void wl_status( void *ctxt ){


}

void user_main(void)
{
    OSStatus err;

    bk_printf("[OpenBK2731T Access Point Demo]!\r\n");


	setup_wifi_open_access_point();

    err = rtos_init_timer(&led_timer,
                          1 * 1000,
                          app_led_timer_handler,
                          (void *)0);
    ASSERT(kNoErr == err);

    err = rtos_start_timer(&led_timer);
    ASSERT(kNoErr == err);
	bk_printf("[ObkAccessDemo] started timer\r\n");
}
