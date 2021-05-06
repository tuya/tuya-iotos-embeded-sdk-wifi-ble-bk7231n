#ifndef _ROLE_LAUNCH_H_
#define _ROLE_LAUNCH_H_

#include "wlan_ui_pub.h"

#define JL_DEBUG            0

#if JL_DEBUG
#define JL_PRT              os_printf
#define JL_WARN             warning_prf
#define JL_FATAL            fatal_prf
#else
#define JL_PRT              null_prf
#define JL_WARN             null_prf
#define JL_FATAL            null_prf
#endif

#define RL_LAUNCH_PERIOD           250   
#define RL_ENTER_PERIOD            150   

#define RL_SUPPORT_FAST_CONNECT    1

typedef network_InitTypeDef_st LAUNCH_DESCR;

enum
{
    LAUNCH_REQ_STA = 1,
    LAUNCH_REQ_AP,
    LAUNCH_REQ_PURE_STA_SCAN,
    LAUNCH_REQ_DELIF_STA,
    LAUNCH_REQ_DELIF_AP,
    LAUNCH_REQ_MAX
};

typedef struct _launch_req_
{
    LAUNCH_DESCR descr;
    uint32_t req_type;
}LAUNCH_REQ, *LAUNCH_REQ_PTR;

enum
{
    LAUNCH_TYPE_ASAP = 1,  /*ASAP: as soon as possible*/
    LAUNCH_TYPE_RECONN
};

enum
{
    LAUNCH_STATUS_OVER = 0,
    LAUNCH_STATUS_CONT
};

typedef struct _rl_socket_cache_
{
	uint32_t sta_req_flag;
	LAUNCH_REQ sta_param;
	FUNC_1PARAM_PTR sta_completion;
}RL_SOCKET_CACHE_T;

typedef struct _rl_socket_
{
	uint32_t sta_req_flag;
	LAUNCH_REQ sta_param;
	FUNC_1PARAM_PTR sta_completion;
	
	uint32_t ap_req_flag;
	LAUNCH_REQ ap_param;
	FUNC_1PARAM_PTR ap_completion;
}RL_SOCKET_T;

typedef struct _rlaunch_entity_
{
    uint32_t launch_type;
    
    uint32_t relaunch_limit;
    uint32_t launch_count;
    
    LAUNCH_REQ rlaunch;
    FUNC_1PARAM_PTR completion_cb;
}RL_ENTITY_T, *RL_ENTITY_PTR;

#if RL_SUPPORT_FAST_CONNECT
typedef struct _rl_bssid_info_
{
	uint8_t ssid[33];
	uint8_t bssid[6];
	uint8_t security;
	uint8_t channel;
	uint8_t psk[65];
	uint8_t pwd[65];
}RL_BSSID_INFO_T, *RL_BSSID_INFO_PTR;
#endif

#define RL_STATUS_CANCEL            0x8000
#define RL_STATUS_CANCEL_MASK       0x8000
#define RL_STATUS_OTHER_MASK        0x7FFF

enum
{
    RL_STATUS_UNKNOWN = 0,
    RL_STATUS_STA_INITING = 1,
    RL_STATUS_STA_SCANNING = 2,
    RL_STATUS_STA_SCAN_VAIN = 3,
    RL_STATUS_STA_SCAN_OVER = 4,
    RL_STATUS_STA_CONNECTING = 5,
    RL_STATUS_STA_DHCPING = 6,    
    RL_STATUS_STA_CHANNEL_SWITCHING = 7, 
    RL_STATUS_STA_CHANNEL_SWITCHED = 8,
    RL_STATUS_STA_CSA_LAUNCHED_UNCERTAINTY = 9,
    RL_STATUS_STA_LAUNCHED = 0xa,
    RL_STATUS_STA_LAUNCH_FAILED = 0xb,
    RL_STATUS_STA_PS_SETTING = 0xc,
    
    RL_STATUS_AP_INITING = 0x11,
    RL_STATUS_AP_UPDATING_BCN = 0x12,
    RL_STATUS_AP_TRANSMITTING_BCN = 0x13,
    RL_STATUS_AP_TRANSMITTED_BCN = 0x14,
    RL_STATUS_AP_LAUNCHED
};

enum
{
    RL_TIMER_UNINIT = 0,
    RL_TIMER_INIT,
    RL_TIMER_START,
    RL_TIMER_STOP
};

enum
{
    PRE_ENTITY_IDLE = 0,
    PRE_ENTITY_STA,
    PRE_ENTITY_AP,
    PRE_ENTITY_MESH
};

typedef struct _role_launch_
{
    beken2_timer_t enter_timer;
    beken2_timer_t rl_timer;
    uint32_t rl_timer_flag;
    
    uint32_t pre_entity_type;
    uint32_t pre_sta_cancel;
    uint32_t pre_sta_status;    
    uint32_t pre_ap_cancel;
    uint32_t pre_ap_status;
    
    RL_ENTITY_T *jl_previous_sta;
    RL_ENTITY_T *jl_following_sta;
    
    RL_ENTITY_T *jl_previous_ap;
    RL_ENTITY_T *jl_following_ap;
}RL_T, *RL_PTR;

/*******************************************************************************
* Function Declarations
*******************************************************************************/
extern void rl_init(void);
extern void rl_uninit(void);
extern void rl_start(void);
extern void rl_stop(void);
extern void rl_pre_ap_init(void);
extern void rl_pre_sta_init(void);
extern uint32_t rl_launch_sta(void);
extern uint32_t rl_launch_ap(void);
extern uint32_t rl_relaunch_chance(void);
extern void rl_launch_handler(void *left, void *right);
extern void rl_free_entity(RL_ENTITY_T *d);
extern uint32_t rl_pre_sta_stop_launch(void);
extern uint32_t rl_pre_ap_stop_launch(void);
extern uint32_t rl_sta_may_next_launch(void);
extern uint32_t rl_ap_may_next_launch(uint32_t pre_cancel);
extern uint32_t rl_pre_sta_get_status(void);
extern uint32_t rl_pre_ap_get_status(void);
extern uint32_t rl_pre_sta_set_status(uint32_t status);
extern uint32_t rl_pre_ap_set_status(uint32_t status);
extern void rl_pre_sta_set_cancel(void);
extern uint32_t rl_pre_sta_get_cancel(void);
extern void rl_pre_sta_clear_cancel(void);
extern uint32_t fl_get_pre_sta_cancel_status(void);
extern void rl_pre_ap_set_cancel(void);
extern void rl_pre_ap_clear_cancel(void);
extern uint32_t rl_pre_ap_disable_autobcn(void);
extern uint32_t fl_get_pre_ap_cancel_status(void);
extern RL_ENTITY_T *rl_alloc_entity(LAUNCH_REQ *param, FUNC_1PARAM_PTR completion);
extern void rl_sta_request_start(LAUNCH_REQ *param);
extern void rl_ap_request_start(LAUNCH_REQ *param);
extern void rl_sta_request_enter(LAUNCH_REQ *param, FUNC_1PARAM_PTR completion);
extern void rl_ap_request_enter(LAUNCH_REQ *param, FUNC_1PARAM_PTR completion);
extern uint32_t rl_sta_cache_request_enter(void);
extern uint32_t rl_sta_req_is_null(void);
#endif //_ROLE_LAUNCH_H_ 

