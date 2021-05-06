#include "include.h"
#include "rw_msg_rx.h"
#include "rw_pub.h"
#include "ke_msg.h"
#include "mem_pub.h"
#include "mac_common.h"
#include "scanu_task.h"
#include "sa_station.h"
#include "apm_task.h"
#include "me_task.h"
#include "sm_task.h"
#include "hostapd_intf_pub.h"
#include "mac_ie.h"
#include "ieee802_11_defs.h"
#include "wlan_ui_pub.h"
#include "mcu_ps_pub.h"
#include "role_launch.h"

uint32_t resultful_scan_cfm = 0;
uint8_t *ind_buf_ptr = 0;
struct co_list rw_msg_rx_head;
struct co_list rw_msg_tx_head;

/* transient status is the passing status of station connection,
   abnormal status is exceptional status, which maybe is uploaded to upper layer.
   the application will go with the status.
 */
static rw_evt_type g_connect_transient_status = RW_EVT_STA_IDLE;
static rw_evt_type g_connect_abnormal_status = RW_EVT_STA_IDLE;

SCAN_RST_UPLOAD_T *scan_rst_set_ptr = 0;
IND_CALLBACK_T scan_cfm_cb = {0};
IND_CALLBACK_T assoc_cfm_cb = {0};
IND_CALLBACK_T deassoc_evt_cb = {0};
IND_CALLBACK_T deauth_evt_cb = {0};
IND_CALLBACK_T wlan_connect_user_cb = {0};

extern void app_set_sema(void);
extern int get_security_type_from_ie(u8 *, int, u16);
extern void rwnx_cal_set_txpwr(UINT32 pwr_gain, UINT32 grate);
extern void bk7011_default_rxsens_setting(void);

/* scan result malloc item */
UINT8 *sr_malloc_result_item(UINT32 vies_len)
{
    return os_zalloc(vies_len + sizeof(struct sta_scan_res));
}

/* free scan result item */
void sr_free_result_item(UINT8 *item_ptr)
{
    os_free(item_ptr);
}

UINT8 *sr_malloc_shell(void)
{
    UINT8 *ptr;
    UINT32 layer1_space_len;
    UINT32 layer2_space_len;

    layer1_space_len = sizeof(SCAN_RST_UPLOAD_T);
    layer2_space_len = MAX_BSS_LIST * sizeof(struct sta_scan_res *);
    ptr = os_zalloc(layer1_space_len + layer2_space_len);

    ASSERT(ptr);

    return ptr;
}

void sr_free_shell(UINT8 *shell_ptr)
{
    os_free(shell_ptr);
}

void sr_free_all(SCAN_RST_UPLOAD_T *scan_rst)
{
    UINT32 i;

    for(i = 0; i < scan_rst->scanu_num; i ++)
    {
        sr_free_result_item((UINT8 *)scan_rst->res[i]);
        scan_rst->res[i] = 0;
    }
    scan_rst->scanu_num = 0;

    sr_free_shell((UINT8 *)scan_rst);
}

void *sr_get_scan_results(void)
{
    return scan_rst_set_ptr;
}

void sr_release_scan_results(SCAN_RST_UPLOAD_PTR ptr)
{
    if(ptr)
    {
        sr_free_all(ptr);
    }
	
    scan_rst_set_ptr = 0;
	resultful_scan_cfm = 0;
	wpa_clear_scan_results();
}

void mr_kmsg_init(void)
{
    co_list_init(&rw_msg_tx_head);
    co_list_init(&rw_msg_rx_head);
}

UINT32 mr_kmsg_fwd(struct ke_msg *msg)
{
    GLOBAL_INT_DECLARATION();

    GLOBAL_INT_DISABLE();
    co_list_push_back(&rw_msg_rx_head, &msg->hdr);
    GLOBAL_INT_RESTORE();

    app_set_sema();

    return 0;
}

void mr_kmsg_flush(void)
{
    while(mr_kmsg_fuzzy_handle())
    {
        ;
    }
}

UINT32 mr_kmsg_fuzzy_handle(void)
{
    UINT32 ret = 0;
    struct ke_msg *msg;
    struct co_list_hdr *node;

    GLOBAL_INT_DECLARATION();

    GLOBAL_INT_DISABLE();
    node = co_list_pop_front(&rw_msg_rx_head);
    GLOBAL_INT_RESTORE();

    if(node)
    {
        msg = (struct ke_msg *)node;
        ke_msg_free(msg);

        ret = 1;
    }

    return ret;
}

UINT32 mr_kmsg_exact_handle(UINT16 rsp)
{
    UINT32 ret = 0;
    struct ke_msg *msg;
    struct co_list_hdr *node;

    GLOBAL_INT_DECLARATION();

    GLOBAL_INT_DISABLE();
    node = co_list_pop_front(&rw_msg_rx_head);
    GLOBAL_INT_RESTORE();

    if(node)
    {
        msg = (struct ke_msg *)node;
        if(rsp == msg->id)
        {
            ret = 1;
        }
        ke_msg_free(msg);
    }

    return ret;
}

void mhdr_connect_user_cb(FUNC_2PARAM_PTR ind_cb, void *ctxt)
{
    wlan_connect_user_cb.cb = ind_cb;
    wlan_connect_user_cb.ctxt_arg = ctxt;
}

void mhdr_assoc_cfm_cb(FUNC_2PARAM_PTR ind_cb, void *ctxt)
{
    assoc_cfm_cb.cb = ind_cb;
    assoc_cfm_cb.ctxt_arg = ctxt;
}

void mhdr_scanu_reg_cb(FUNC_2PARAM_PTR ind_cb, void *ctxt)
{
    scan_cfm_cb.cb = ind_cb;
    scan_cfm_cb.ctxt_arg = ctxt;
}

void mhdr_deauth_evt_cb(FUNC_2PARAM_PTR ind_cb, void *ctxt)
{
    deauth_evt_cb.cb = ind_cb;
    deauth_evt_cb.ctxt_arg = ctxt;
}

void mhdr_deassoc_evt_cb(FUNC_2PARAM_PTR ind_cb, void *ctxt)
{
    deassoc_evt_cb.cb = ind_cb;
    deassoc_evt_cb.ctxt_arg = ctxt;
}

void mhdr_disconnect_ind(void *msg)
{
    struct ke_msg *msg_ptr;
    struct sm_disconnect_ind *disc;

    msg_ptr = (struct ke_msg *)msg;
    disc = (struct sm_disconnect_ind *)msg_ptr->param;

#if CFG_ROLE_LAUNCH
	if(rl_sta_req_is_null())
	{
		rl_sta_cache_request_enter();
	}
	else if(msg && disc 
				&& (VENDOR_CONNECTION_LOSS == disc->reason_code))
	{
		os_printf("VENDOR_CONNECTION_LOSS\r\n");
		rl_sta_cache_request_enter();
	}
	else if(deassoc_evt_cb.cb)
#else
    if(deassoc_evt_cb.cb)
#endif
	{
		os_printf("deassoc_evt_cb\r\n");
        (*deassoc_evt_cb.cb)(deassoc_evt_cb.ctxt_arg, disc->vif_idx);
	}
}

void mhdr_connect_ind(void *msg, UINT32 len)
{
    struct ke_msg *msg_ptr;
    struct sm_connect_indication *conn_ind_ptr;

    msg_ptr = (struct ke_msg *)msg;
    conn_ind_ptr = (struct sm_connect_indication *)msg_ptr->param;
    if(0 == conn_ind_ptr->status_code)
    {
        os_printf("---------SM_CONNECT_IND_ok\r\n");

        bk7011_default_rxsens_setting();

        if(assoc_cfm_cb.cb)
        {
            (*assoc_cfm_cb.cb)(assoc_cfm_cb.ctxt_arg, conn_ind_ptr->vif_idx);
        }

        if(wlan_connect_user_cb.cb)
        {
            (*wlan_connect_user_cb.cb)(wlan_connect_user_cb.ctxt_arg, 0);
        }
    }
    else
    {
        os_printf("---------SM_CONNECT_IND_fail\r\n");
		mhdr_disconnect_ind(msg);
    }

    mcu_prevent_clear(MCU_PS_CONNECT);
}


void mhdr_set_station_status_when_reconnect_over(void)
{
	if(RW_EVT_STA_CONNECTED != g_connect_transient_status)
	{
		g_connect_transient_status = g_connect_abnormal_status;
	}
}

void mhdr_set_abnormal_status(rw_evt_type val)
{
	rw_evt_type pre_status = g_connect_abnormal_status;
	
	if((RW_EVT_STA_PASSWORD_WRONG == pre_status)
			|| (RW_EVT_STA_NO_AP_FOUND == pre_status))
	{
		goto set_exit;
	}

	g_connect_abnormal_status = val;

set_exit:
	return;
}

uint32_t mhdr_is_station_abnormal_status(rw_evt_type val)
{
	uint32_t unusual_flag = 0;

	if((RW_EVT_STA_PASSWORD_WRONG == val)
		|| (RW_EVT_STA_NO_AP_FOUND == val)
		|| (RW_EVT_STA_BEACON_LOSE == val)
		|| (RW_EVT_STA_ASSOC_FULL == val)
		|| (RW_EVT_STA_DISCONNECTED == val)
		|| (RW_EVT_STA_CONNECT_FAILED == val))
	{	
		unusual_flag = 1;
	}
	
	return unusual_flag;
}

void mhdr_set_station_status(rw_evt_type val)
{
    GLOBAL_INT_DECLARATION();
	
    GLOBAL_INT_DISABLE();
    g_connect_transient_status = val;

	if(mhdr_is_station_abnormal_status(val))
	{
		mhdr_set_abnormal_status(val);
	}
    GLOBAL_INT_RESTORE();
}

rw_evt_type mhdr_get_station_status(void)
{
    return g_connect_transient_status;
}

static void sort_scan_result(SCAN_RST_UPLOAD_T *ap_list)
{
    int i, j;
    struct sta_scan_res *tmp;

    if (ap_list->scanu_num == 0)
        return;

    for(i = 0; i < (ap_list->scanu_num - 1); i++)
    {
        for(j = i + 1; j < ap_list->scanu_num; j++)
        {
            if (ap_list->res[j]->level > ap_list->res[i]->level)
            {
                tmp = ap_list->res[j];
                ap_list->res[j] = ap_list->res[i];
                ap_list->res[i] = tmp;
            }
        }
    }
}

UINT32 mhdr_scanu_start_cfm(void *msg, SCAN_RST_UPLOAD_T *ap_list)
{
    struct scanu_start_cfm *cfm;
    struct ke_msg *msg_ptr;

    msg_ptr = (struct ke_msg *)msg;
    cfm = (struct scanu_start_cfm *)msg_ptr->param;

    if(ap_list)
    {
        sort_scan_result(ap_list);
        wpa_buffer_scan_results();
    }

    if(scan_cfm_cb.cb)
    {
        (*scan_cfm_cb.cb)(scan_cfm_cb.ctxt_arg, cfm->vif_idx);
    }

    return RW_SUCCESS;
}


UINT32 mhdr_scanu_result_ind(SCAN_RST_UPLOAD_T *scan_rst, void *msg, UINT32 len)
{
    UINT32 ret, chann;
    UINT8 *elmt_addr;
    UINT32 vies_len, i;
    UINT8 *var_part_addr;
    struct ke_msg *msg_ptr;
    SCAN_RST_ITEM_PTR item;
    SCAN_RST_UPLOAD_PTR result_ptr;
    SCAN_IND_PTR scanu_ret_ptr;
    IEEE802_11_PROBE_RSP_PTR probe_rsp_ieee80211_ptr;
    char on_channel;
    int replace_index = -1;

    ret = RW_SUCCESS;
    result_ptr = scan_rst;

    if (result_ptr->scanu_num >= MAX_BSS_LIST)
        goto scan_rst_exit;

    msg_ptr = (struct ke_msg *)msg;
    scanu_ret_ptr = (SCAN_IND_PTR)msg_ptr->param;
    probe_rsp_ieee80211_ptr =  (IEEE802_11_PROBE_RSP_PTR)scanu_ret_ptr->payload;
    vies_len = scanu_ret_ptr->length - MAC_BEACON_VARIABLE_PART_OFT;
    var_part_addr = probe_rsp_ieee80211_ptr->rsp.variable;

    elmt_addr = (UINT8 *)mac_ie_find((UINT32)var_part_addr, (UINT16)vies_len, MAC_ELTID_DS);
    if(elmt_addr) // adjust channel
    {
        chann = *(elmt_addr + MAC_DS_CHANNEL_OFT);
        if(rw_ieee80211_is_scan_rst_in_countrycode(chann) == 0)
        {
            elmt_addr = (UINT8 *)mac_ie_find((UINT32)var_part_addr,
                                             (UINT16)vies_len,
                                             MAC_ELTID_SSID);
            if(elmt_addr)
            {
                UINT8 ssid_b[MAC_SSID_LEN];
                UINT8 ssid_len = *(elmt_addr + MAC_SSID_LEN_OFT);

                if (ssid_len > MAC_SSID_LEN)
                    ssid_len = MAC_SSID_LEN;

                
                os_memcpy(ssid_b, elmt_addr + MAC_SSID_SSID_OFT, ssid_len);
                os_printf("drop: %s, chan:%d\r\n", ssid_b, chann);
            }
            
            goto scan_rst_exit;
        }

        if (chann == rw_ieee80211_get_chan_id(scanu_ret_ptr->center_freq))
            on_channel = 1;
        else
            on_channel = 0;
    }
    else
    {
        chann = rw_ieee80211_get_chan_id(scanu_ret_ptr->center_freq);
        on_channel = 0;
        os_printf("scan rst no ds param, drop it?\r\n");
    }

    /* check the duplicate bssid*/
    do
    {
        for(i = 0; i < result_ptr->scanu_num; i ++)
        {
            if(!os_memcmp(probe_rsp_ieee80211_ptr->bssid, result_ptr->res[i]->bssid, ETH_ALEN))
            {
                if ((result_ptr->res[i]->on_channel == 1) || (on_channel == 0))
                {
                    goto scan_rst_exit;
                }
                else
                {
                    replace_index = i; // should replace it.
                }
            }
        }
    }while(0);

    item = (SCAN_RST_ITEM_PTR)sr_malloc_result_item(vies_len);
    if (item == NULL)
        goto scan_rst_exit;

    elmt_addr = (UINT8 *)mac_ie_find((UINT32)var_part_addr,
                                     (UINT16)vies_len,
                                     MAC_ELTID_SSID);
    if(elmt_addr)
    {
        UINT8 ssid_len = *(elmt_addr + MAC_SSID_LEN_OFT);

        if (ssid_len > MAC_SSID_LEN)
            ssid_len = MAC_SSID_LEN;

        os_memcpy(item->ssid, elmt_addr + MAC_SSID_SSID_OFT, ssid_len);
    }
    else
    {
        os_printf("NoSSid\r\n");
    }

    os_memcpy(item->bssid, probe_rsp_ieee80211_ptr->bssid, ETH_ALEN);
    item->channel = chann;
    item->beacon_int = probe_rsp_ieee80211_ptr->rsp.beacon_int;
    item->caps = probe_rsp_ieee80211_ptr->rsp.capab_info;
    item->level = scanu_ret_ptr->rssi;
    item->on_channel = on_channel;

    os_memcpy(item->tsf, probe_rsp_ieee80211_ptr->rsp.timestamp, 8);

    item->ie_len = vies_len;
    os_memcpy(item + 1, var_part_addr, vies_len);

    item->security = get_security_type_from_ie((u8 *)var_part_addr, vies_len, item->caps);

    if (replace_index >= 0)
    {
        sr_free_result_item((UINT8 *)result_ptr->res[replace_index]);
        result_ptr->res[replace_index] = item;
    }
    else
    {
        result_ptr->res[result_ptr->scanu_num] = item;
        result_ptr->scanu_num ++;
    }

scan_rst_exit:
    return ret;
}

void rwnx_handle_recv_msg(struct ke_msg *rx_msg)
{
	uint32_t param;
    extern FUNC_1PARAM_PTR bk_wlan_get_status_cb(void);
	FUNC_1PARAM_PTR fn = bk_wlan_get_status_cb();

    switch(rx_msg->id)
    {
    case SCANU_START_CFM:
		if(scan_rst_set_ptr)
		{
        	resultful_scan_cfm = 1;
		}
		
        mhdr_scanu_start_cfm(rx_msg, scan_rst_set_ptr);
        break;

    case SCANU_RESULT_IND:
        if(resultful_scan_cfm && scan_rst_set_ptr)
        {
            sr_release_scan_results(scan_rst_set_ptr);
            scan_rst_set_ptr = 0;
            resultful_scan_cfm = 0;
        }

        if(0 == scan_rst_set_ptr)
        {
            scan_rst_set_ptr = (SCAN_RST_UPLOAD_T *)sr_malloc_shell();
            ASSERT(scan_rst_set_ptr);
            scan_rst_set_ptr->scanu_num = 0;
            scan_rst_set_ptr->res = (SCAN_RST_ITEM_PTR*)&scan_rst_set_ptr[1];
        }

        mhdr_scanu_result_ind(scan_rst_set_ptr, rx_msg, rx_msg->param_len);
        break;

    case SM_CONNECT_IND:
        if(scan_rst_set_ptr)
        {
            sr_release_scan_results(scan_rst_set_ptr);
            scan_rst_set_ptr = 0;
        }

        mhdr_connect_ind(rx_msg, rx_msg->param_len);
        break;

    case SM_DISCONNECT_IND:
        os_printf("SM_DISCONNECT_IND\r\n");
        mhdr_disconnect_ind(rx_msg);
		extern UINT32 rwnx_sys_is_enable_hw_tpc(void);
        if(rwnx_sys_is_enable_hw_tpc() == 0)
            rwnx_cal_set_txpwr(20, 11);
        break;

	case SM_CONNCTION_START_IND:
		if(fn)
		{
			param = RW_EVT_STA_CONNECTING;
			(*fn)(&param);
		}
		mhdr_set_station_status(RW_EVT_STA_CONNECTING);
		break;
		
	case SM_BEACON_LOSE_IND:
		if(fn)
		{
			param = RW_EVT_STA_BEACON_LOSE;
			(*fn)(&param);
		}
		mhdr_set_station_status(RW_EVT_STA_BEACON_LOSE);
		break;

	case SM_DISASSOC_IND:
		{
			struct sm_fail_stat *status_ind;
			
			status_ind = (struct sm_fail_stat *)rx_msg->param;
			os_null_printf("[wzl]SM_DISASSOC_IND:%d, 0x%x\r\n", (status_ind->status), fn);
			switch (status_ind->status)
			{
				case WLAN_REASON_MICHAEL_MIC_FAILURE:
					param = RW_EVT_STA_PASSWORD_WRONG;
#if RL_SUPPORT_FAST_CONNECT
					rl_clear_bssid_info();
#elif (CFG_WPA_CTRL_IFACE && CFG_WLAN_FAST_CONNECT)
					wlan_clear_fast_connect_info();
#endif
					break;
				
				default:
					param = RW_EVT_STA_DISCONNECTED;
					break;
			}
			mhdr_set_station_status(param);
		}
	
		if(fn)
		{
			(*fn)(&param);
		}

		break;
		
	case SM_AUTHEN_FAIL_IND:
		{
			struct sm_fail_stat *status_ind;
			
			status_ind = (struct sm_fail_stat *)rx_msg->param;
			switch (status_ind->status)
			{
				case WLAN_REASON_PREV_AUTH_NOT_VALID:
				case WLAN_REASON_4WAY_HANDSHAKE_TIMEOUT:
					param = RW_EVT_STA_PASSWORD_WRONG;
					break;

				case MAC_RS_DIASSOC_TOO_MANY_STA:
					param = RW_EVT_STA_ASSOC_FULL;
					break;
		
				case WLAN_REASON_DEAUTH_LEAVING:
				default:
					param = RW_EVT_STA_CONNECT_FAILED;
					break;
			}
			if(fn)
			{
				(*fn)(&param);
			}
			mhdr_set_station_status(param);
		}
		break;
		
	case SM_ASSOC_FAIL_INID:
		{
			struct sm_fail_stat *assoc_state;

			assoc_state = (struct sm_fail_stat *)rx_msg->param;
			if(assoc_state->status == MAC_ST_ASSOC_TOO_MANY_STA)
	        {
				param = RW_EVT_STA_ASSOC_FULL;
	        }
				else
	        {
				param = RW_EVT_STA_DISCONNECTED;
	        }
			if(fn)
			{
				(*fn)(&param);
	        }
			mhdr_set_station_status(param);
		}
        break;

    case SM_ASSOC_IND:
		if(fn)
        {
			param = RW_EVT_STA_CONNECTED;
			(*fn)(&param);
        }
        if (mhdr_get_station_status() < RW_EVT_STA_CONNECTED)
        {
            mhdr_set_station_status(RW_EVT_STA_CONNECTED);
        }
        break;

    case APM_ASSOC_IND:
        if(fn)
        {
            param = RW_EVT_AP_CONNECTED;
            (*fn)(&param);
        }
        break;

    case APM_DEASSOC_IND:
        if(fn)
        {
            param = RW_EVT_AP_DISCONNECTED;
            (*fn)(&param);
        }
        break;

    case APM_ASSOC_FAILED_IND:
        if(fn)
        {
            param = RW_EVT_AP_CONNECT_FAILED;
            (*fn)(&param);
        }
        break;

#if CFG_USE_AP_PS
    case MM_PS_CHANGE_IND:
        rwm_msdu_ps_change_ind_handler(rx_msg);
        break;
#endif

    default:
        break;
    }
}

void rwnx_recv_msg(void)
{
    struct ke_msg *rx_msg;
    MSG_SND_NODE_PTR tx_msg;
    struct co_list_hdr *rx_node, *tx_node;

    GLOBAL_INT_DECLARATION();

    while(1)
    {
        uint8_t find = 0;

        rx_node = co_list_pop_front(&rw_msg_rx_head);
        if(!rx_node)
            break;

        rx_msg = (struct ke_msg *)rx_node;

        GLOBAL_INT_DISABLE();
        tx_node = co_list_pick(&rw_msg_tx_head);
        GLOBAL_INT_RESTORE();

        while(tx_node)
        {
            tx_msg = (MSG_SND_NODE_PTR)tx_node;
            if(rx_msg->id == tx_msg->reqid)
            {
                find = 1;
                break;
            }

            GLOBAL_INT_DISABLE();
            tx_node = co_list_next(tx_node);
            GLOBAL_INT_RESTORE();
        }

        if(find)
        {
            int ret;
            GLOBAL_INT_DISABLE();
            co_list_extract(&rw_msg_rx_head, rx_node);
            co_list_extract(&rw_msg_tx_head, tx_node);
            GLOBAL_INT_RESTORE();

            if(tx_msg->cfm && rx_msg->param_len)
                os_memcpy(tx_msg->cfm, &rx_msg->param[0], rx_msg->param_len);

            ret = rtos_set_semaphore(&tx_msg->semaphore);
            ASSERT(0 == ret);
        }
        else
        {
            rwnx_handle_recv_msg(rx_msg);
        }

        ke_msg_free(rx_msg);
    }
}

// eof

