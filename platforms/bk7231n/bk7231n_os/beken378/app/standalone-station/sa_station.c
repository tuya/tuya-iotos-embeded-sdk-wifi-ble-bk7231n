#include "include.h"

#include "schedule_pub.h"

#include "sa_station.h"
#include "drv_model_pub.h"
#include "uart_pub.h"
#include "scanu_task.h"
#include "scan_task.h"
#include "rxu_task.h"
#include "mm_task.h"
#include "me_task.h"
#include "sm_task.h"
#include "rw_msg_tx.h"
#include "mac_ie.h"
#include "vif_mgmt.h"

#include "arm_arch.h"
#include "mem_pub.h"
#include "rw_pub.h"
#include "common.h"

#if CFG_USE_LWIP_NETSTACK
#include "lwip_intf.h"
#endif

#if CFG_USE_TEMPERATURE_DETECT
#include "temp_detect_pub.h"
#endif

#include "param_config.h"

#if (!CFG_SUPPORT_ALIOS)
#include "sys_rtos.h"
#endif
#include "rtos_pub.h"
#include "errno.h"
#include "rtos_error.h"
#include "errno-base.h"
#include "rw_ieee80211.h"

extern struct mac_scan_result *scanu_search_by_ssid(struct mac_ssid const *ssid);

/*---------------------------------------------------------------------------*/
int sa_station_send_associate_cmd(CONNECT_PARAM_T *connect_param)
{
    int ret;
    struct mac_scan_result *desired_ap_ptr;
    struct sm_connect_cfm sm_connect_cfm;

    if(g_sta_param_ptr->fast_connect_set)
    {
        g_sta_param_ptr->fast_connect_set = 0;
        if ((g_sta_param_ptr->key_len != g_sta_param_ptr->orig_key_len) 
			|| !os_strncmp((char *)g_sta_param_ptr->key, (char *)g_sta_param_ptr->orig_key, g_sta_param_ptr->key_len))
        {
            os_null_printf("%s:%d %d vs %d\n", __FUNCTION__, __LINE__, g_sta_param_ptr->key_len, g_sta_param_ptr->orig_key_len);
            g_sta_param_ptr->key_len = g_sta_param_ptr->orig_key_len;
            os_memcpy(g_sta_param_ptr->key, g_sta_param_ptr->orig_key, g_sta_param_ptr->orig_key_len);
        }
        connect_param->chan.freq = rw_ieee80211_get_centre_frequency(g_sta_param_ptr->fast_connect.chann);
        connect_param->chan.band = 0;
        connect_param->chan.flags = 0;
        connect_param->chan.tx_power = 10;
    }
    else
    {
        desired_ap_ptr = scanu_search_by_ssid((void *)&connect_param->ssid);
        if(NULL == desired_ap_ptr)
        {
            return -1;
        }
        connect_param->chan = *(desired_ap_ptr->chan);
        if(0 == connect_param->chan.tx_power)
        {
            connect_param->chan.tx_power = 10;
        }
    }

    if(rw_ieee80211_is_scan_rst_in_countrycode(rw_ieee80211_get_chan_id(connect_param->chan.freq)) == 0)
    {
        os_printf("ch:%d not in countrycode\r\n", connect_param->chan.freq);
        return -1;
    }

    ret = rw_msg_send_sm_connect_req(connect_param, &sm_connect_cfm);
    if(ret)
        return ret;

    switch (sm_connect_cfm.status)
    {
    case CO_OK:
        ret = 0;
        break;

    case CO_BUSY:
        ret = -ERRINPROGRESS;
        break;

    case CO_OP_IN_PROGRESS:
        ret = -ERRALREADY;
        break;

    default:
        ret = -EERIO;
        break;
    }

    return ret;
}

/*---------------------------------------------------------------------------*/
static void sa_station_cfg80211_init(void)
{
    if (rwm_mgmt_is_vif_first_used() == NULL)
    {
        SASTA_PRT("[sa_sta]MM_RESET_REQ\r\n");
        rw_msg_send_reset();

        SASTA_PRT("[sa_sta]ME_CONFIG_REQ\r\n");
        rw_msg_send_me_config_req();

        SASTA_PRT("[sa_sta]ME_CHAN_CONFIG_REQ\r\n");
        rw_msg_send_me_chan_config_req();

        SASTA_PRT("[sa_sta]MM_START_REQ\r\n");
        rw_msg_send_start();
    }
}

void sa_station_init(void)
{
    sa_station_cfg80211_init();
}

void sa_station_uninit(void)
{
}

// eof
