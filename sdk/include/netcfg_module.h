#ifndef __NETCFG_MODULE__
#define __NETCFG_MODULE__

#include "tuya_hal_wifi.h"
#include "uni_pointer.h"
#ifdef __cplusplus
	extern "C" {
#endif

	

/*
	配网类型的宏定义由32bits整型表示:
	| high 16bits 	| low 16bits 	|
	|	vendor type	|	cfg type	|
	
	1)固定前缀netcfg
	2)协议提供方：ty,jd,
	3)链路方式wifi,ble,camera等，品牌
	4)特性后缀，如smartcfg,ap,airkiss
	
	NETCFG_TY_WIFI_SMARTCFG
	NETCFG_TY_WIFI_AP
	NETCFG_JD_WIFI_SMART
	NETCFG_WX_WIFI_AIRKISS
	NETCFG_TY_BLE
	NETCFG_TY_CAMERA_QR
*/

/*
	配网提供方：
	tuya, jd, weixin

*/
#define VENDOR_TUYA			(0x1 << 0)
#define VENDOR_JD			(0x1 << 1)
#define VENDOR_WEIXIN		(0x1 << 2)
#define VENDOR_AMAZON		(0x1 << 3)

#define WIFI_SMARTCFG 				(0x1 << 0)
#define	WIFI_AP						(0x1 << 1)
#define BLE							(0x1 << 2)
#define CAMERA_QR					(0x1 << 3)
#define WIFI_SMARTCFG_COEXIST 		(0x1 << 4)
#define WIFI_AP_COEXIST 			(0x1 << 5)
#define WIFI_FFS					(0x1 << 6)
#define WIFI_PEGASUS				(0x1 << 7)//无感配网
#define API_USER    				(0x1 << 8)//用户通过api配网

#define NETCFG_TY_WIFI_SMARTCFG 				((VENDOR_TUYA << 16) | WIFI_SMARTCFG)
#define NETCFG_TY_WIFI_SMARTCFG_COEXIST 		((VENDOR_TUYA << 16) | WIFI_SMARTCFG_COEXIST)

#define NETCFG_TY_WIFI_AP				((VENDOR_TUYA << 16) | WIFI_AP)
#define NETCFG_TY_WIFI_AP_COEXIST		((VENDOR_TUYA << 16) | WIFI_AP_COEXIST)

#define NETCFG_TY_BLE					((VENDOR_TUYA << 16) | BLE)
#define NETCFG_TY_CAMERA_QR				((VENDOR_TUYA << 16) | CAMERA_QR)

#define NETCFG_TY_API_USER  			((VENDOR_TUYA << 16) | API_USER)

#define NETCFG_JD_WIFI_SMART			((VENDOR_JD << 16) | WIFI_SMARTCFG)
#define NETCFG_WX_WIFI_AIRKISS			((VENDOR_WEIXIN << 16) | WIFI_SMARTCFG)

#define NETCFG_AMAZON_WIFI_FFS			((VENDOR_AMAZON << 16) | (WIFI_FFS))

#define NETCFG_TY_WIFI_PEGASUS			((VENDOR_TUYA << 16) | WIFI_PEGASUS)

#define NETCFG_STOP_ALL_CFG_MODULE		(0)

#define DEF_AP_SSID_PF "SmartLife"

typedef struct{
	int enable_station_ap;
	WF_AP_CFG_IF_S *pDevApCfg;
}AP_NETCFG_ARGS, *P_AP_NETCFG_ARGS;

#define WL_TOKEN_LEN (16)

typedef struct{
    uint8_t ssid[WIFI_SSID_LEN+1];       ///< ssid
    uint8_t s_len;                       ///< len of ssid
    uint8_t passwd[WIFI_PASSWD_LEN+1];   ///< passwd
    uint8_t p_len;                       ///< len of passwd
    uint8_t token[WL_TOKEN_LEN+1]; // tuya link:region(2BYTE) + token(8BYTE) + key(4BYTE) useless // airkiss:useless
    uint8_t t_len;
}SSID_PASSWORD_TOKEN, *PTR_SSID_PASSWORD_TOKEN;

typedef struct apSsidPasswd_s{
	uint8_t* pSsid;
	uint8_t* pPasswd;
}ApSsidPasswd_t, *ptrApSsidPasswd;


typedef int (* FN_NETCFG_CB)( PTR_SSID_PASSWORD_TOKEN info,  int type);
typedef int (* FN_NETCFG_START)(FN_NETCFG_CB cb, void * args, int type);
typedef int (* FN_NETCFG_STOP)(int type);

typedef struct{
	LIST_HEAD 			node;
    int				 	type;
    FN_NETCFG_START 	start;
    FN_NETCFG_STOP 		stop;
    FN_NETCFG_CB        netcfg_finish_cb;    
    void * 				args;    
	bool 				isStarted;/*是否已经开启配网，执行过了start*/
	
	//interface of configing other device
	//fn_netcfg_others_start
	//fn_netcfg_others_stop
}NETCFG_HANDLER, *PTR_NETCFG_HANDLER;

/*
	netcfg_policy:配网策略，具体如下：
	WF_START_AP_ONLY,
	WF_START_SMART_ONLY
	WF_START_AP_FIRST
	WF_START_SMART_FIRST
	WF_START_SMART_AP_CONCURRENT
	根据每种策略，执行不同的配网模块初始化

*/
int netcfg_module_init(int netcfg_policy, ApSsidPasswd_t pApInfo, bool isApCfg);
int netcfg_module_uninit();

/*
	配网注册接口，tuya smartcfg, ap netcfg, bt netcfg等都注册到该模块
*/
int netcfg_module_register(int 	type, FN_NETCFG_START 	start, FN_NETCFG_STOP stop);
int netcfg_module_unregister(int 	type);

/*
	启动配网：
	param：
	vendor: 配网提供方
	type:配网方式，从上述宏定义中选择，可以单选或多选
*/
int netcfg_start(int type, FN_NETCFG_CB netcfg_finish_cb, void *args);

/*
	func desc:
		关闭配网
	param：
		type:
			0 : 关闭所有类型的配网
			!=0:关闭指定类型的配网
		

*/
int netcfg_stop(int type);

/*
	func desc:
	停止其他配网活动
	func params:
	type:
	!=0 :停止type外其他配网

*/
int netcfg_stop_other_all(int type);

/*
    fun_desc:    
    开启除了type之外所有配网
*/
int netcfg_start_other_all(int type);


#ifdef __cplusplus
	}
#endif


#endif
