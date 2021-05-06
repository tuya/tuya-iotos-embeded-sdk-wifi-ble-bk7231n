#include "include.h"
#include "rw_ieee80211.h"
#include "rw_pub.h"
#include "rwnx.h"
#include "rw_msdu.h"
#include "mem_pub.h"

typedef struct _wifi_cn_code_st_ 
{
    UINT32 init;
    wifi_country_t cfg;
} WIFI_CN_ST, *WIFI_CN_PTR;

WIFI_CN_ST g_country_code = {0};

#define COUNTRY_CODE_CN   {.cc= "CN", .schan=1, .nchan=13, .max_tx_power=0, .policy=WIFI_COUNTRY_POLICY_MANUAL};
#define COUNTRY_CODE_US   {.cc= "US", .schan=1, .nchan=11, .max_tx_power=0, .policy=WIFI_COUNTRY_POLICY_MANUAL};
#define COUNTRY_CODE_EP   {.cc= "EP", .schan=1, .nchan=13, .max_tx_power=0, .policy=WIFI_COUNTRY_POLICY_MANUAL};
#define COUNTRY_CODE_JP   {.cc= "JP", .schan=1, .nchan=14, .max_tx_power=0, .policy=WIFI_COUNTRY_POLICY_MANUAL};
#define COUNTRY_CODE_AU   {.cc= "AU", .schan=1, .nchan=13, .max_tx_power=0, .policy=WIFI_COUNTRY_POLICY_MANUAL};

const static struct ieee80211_channel rw_2ghz_channels[] =
{
    CHAN2G(1, 2412, 0),
    CHAN2G(2, 2417, 0),
    CHAN2G(3, 2422, 0),
    CHAN2G(4, 2427, 0),
    CHAN2G(5, 2432, 0),
    CHAN2G(6, 2437, 0),
    CHAN2G(7, 2442, 0),
    CHAN2G(8, 2447, 0),
    CHAN2G(9, 2452, 0),
    CHAN2G(10, 2457, 0),
    CHAN2G(11, 2462, 0),
    CHAN2G(12, 2467, 0),
    CHAN2G(13, 2472, 0),
    CHAN2G(14, 2484, 0),
};


#ifdef ENABLE_5GHZ_IEEE80211
static struct ieee80211_channel rw_5ghz_a_channels[] =
{
    CHAN5G(34, 0),
    CHAN5G(36, 0),
    CHAN5G(38, 0),
    CHAN5G(40, 0),
    CHAN5G(42, 0),
    CHAN5G(44, 0),
    CHAN5G(46, 0),
    CHAN5G(48, 0),
    CHAN5G(52, 0),
    CHAN5G(56, 0),
    CHAN5G(60, 0),
    CHAN5G(64, 0),
    CHAN5G(100, 0),
    CHAN5G(104, 0),
    CHAN5G(108, 0),
    CHAN5G(112, 0),
    CHAN5G(116, 0),
    CHAN5G(120, 0),
    CHAN5G(124, 0),
    CHAN5G(128, 0),
    CHAN5G(132, 0),
    CHAN5G(136, 0),
    CHAN5G(140, 0),
    CHAN5G(149, 0),
    CHAN5G(153, 0),
    CHAN5G(157, 0),
    CHAN5G(161, 0),
    CHAN5G(165, 0),
    CHAN5G(184, 0),
    CHAN5G(188, 0),
    CHAN5G(192, 0),
    CHAN5G(196, 0),
    CHAN5G(200, 0),
    CHAN5G(204, 0),
    CHAN5G(208, 0),
    CHAN5G(212, 0),
    CHAN5G(216, 0),
};
#endif

WIPHY_T g_wiphy;
const UINT8 beacon[149] =
{
    128, 0, 0, 0, 255, 255, 255, 255, 255, 255, 18, 113, 17, 113, 11, 113,
    18, 113, 17, 113, 11, 113, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    100, 0, 49, 4, 0, 5, 74, 73, 69, 87, 85, 1, 8, 130, 132, 139, // 49  33
    150, 12, 18, 24, 36, 3, 1, 6, 5, 4, 0, 2, 0, 0, 42, 1,        // 6 -->  chan_connect
    0, 50, 4, 48, 72, 96, 108, 45, 26, 12, 0, 27, 255, 0, 0, 0,
    1, 0, 0, 0, 0, 0, 150, 0, 1, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 61, 22, 8, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 221, 24, 0, 80, 242,
    2, 1, 1, 0, 0, 3, 164, 0, 0, 39, 164, 0, 0, 66, 67, 94,
    0, 98, 50, 47, 0
};

UINT32 rw_ieee80211_init(void)
{
    RW_CONNECTOR_T intf;
    struct ieee80211_sta_ht_cap ht_cap = RWNX_HT_CAPABILITIES;
    struct ieee80211_sta_vht_cap vht_cap = RWNX_VHT_CAPABILITIES;

    g_wiphy.bands[IEEE80211_BAND_2GHZ].num_channels = sizeof(rw_2ghz_channels) /
            sizeof(rw_2ghz_channels[0]);
    g_wiphy.bands[IEEE80211_BAND_2GHZ].channels = (struct ieee80211_channel *)rw_2ghz_channels;
    g_wiphy.bands[IEEE80211_BAND_2GHZ].ht_cap = ht_cap;

#ifdef ENABLE_5GHZ_IEEE80211
    g_wiphy.bands[IEEE80211_BAND_5GHZ].num_channels = sizeof(rw_5ghz_a_channels) /
            sizeof(rw_5ghz_a_channels[0]);
    g_wiphy.bands[IEEE80211_BAND_5GHZ].channels = rw_5ghz_a_channels;
#else
    g_wiphy.bands[IEEE80211_BAND_5GHZ].num_channels = 0;
    g_wiphy.bands[IEEE80211_BAND_5GHZ].channels = 0;
#endif
    g_wiphy.bands[IEEE80211_BAND_5GHZ].ht_cap = ht_cap;
    g_wiphy.bands[IEEE80211_BAND_5GHZ].vht_cap = vht_cap;

    g_wiphy.bands[IEEE80211_BAND_60GHZ].num_channels = 0;
    g_wiphy.bands[IEEE80211_BAND_60GHZ].channels = 0;

    intf.msg_outbound_func = mr_kmsg_fwd;
    intf.data_outbound_func = rwm_upload_data;
    intf.rx_alloc_func = rwm_get_rx_free_node;
    intf.get_rx_valid_status_func = rwm_get_rx_valid;
    intf.tx_confirm_func = rwm_tx_confirm;

    rwnxl_register_connector(&intf);

    g_country_code.cfg.cc[0] = 'C';
    g_country_code.cfg.cc[1] = 'N';
    g_country_code.cfg.cc[2] = 0;
    g_country_code.cfg.schan = 1;
    g_country_code.cfg.nchan = 13;
    g_country_code.cfg.max_tx_power = 0;
    g_country_code.cfg.policy = WIFI_COUNTRY_POLICY_MANUAL;

    g_country_code.init = 1;

    return 0;
}

UINT32 rw_ieee80211_set_country(const wifi_country_t *country)
{
    if(country) 
    {
        UINT32 prev_policy;
        
        if(g_country_code.init == 0)
            return kNotInitializedErr;

        prev_policy = g_country_code.cfg.policy;
        
        os_memcpy(&g_country_code.cfg, country, sizeof(wifi_country_t));
        os_printf("rw_ieee80211_set_country code:\r\n");
        os_printf("code: %s\r\n", g_country_code.cfg.cc);
        os_printf("channel: %d - %d\r\n", g_country_code.cfg.schan,
            g_country_code.cfg.schan + g_country_code.cfg.nchan - 1);
        
        if(g_country_code.cfg.policy == WIFI_COUNTRY_POLICY_MANUAL) 
            os_printf("mode: MANUAL\r\n", g_country_code.cfg.policy );
        else
        {
            os_printf("mode: AUTO\r\n", g_country_code.cfg.policy );
            if(prev_policy != g_country_code.cfg.policy) 
            {
                os_printf("in auto mode, need change softap beacon\r\n");
                // to do
            }
        }

        return kNoErr;
    }
    else 
    {
        return kParamErr;
    }
}

UINT32 rw_ieee80211_get_country(wifi_country_t *country)
{
    if(country) 
    {
        if(g_country_code.init == 0)
            return kNotInitializedErr;
        
        os_memcpy(country, &g_country_code.cfg, sizeof(wifi_country_t));

        return kNoErr;
    }
    else 
    {
        return kParamErr;
    }
}

UINT32 rw_ieee80211_get_centre_frequency(UINT32 chan_id)
{
    UINT32 freq = 0;
    struct ieee80211_channel *channels = NULL;

    if((chan_id >= 1) && (chan_id <= 14))
    {
        channels = (struct ieee80211_channel *)rw_2ghz_channels;
    }

#ifdef ENABLE_5GHZ_IEEE80211
    if(chan_id > 14)
    {
    }
#endif

    if(channels)
    {
        freq = channels[chan_id - 1].center_freq;
    }

    ASSERT(freq);
    return freq;
}

UINT8 rw_ieee80211_get_chan_id(UINT32 freq)
{
    int i;
    struct ieee80211_channel *channels = NULL;

    channels = (struct ieee80211_channel *)rw_2ghz_channels;
    for(i = 0; i < 14; i++)
    {
        if(channels[i].center_freq == freq)
        {
            break;
        }
    }

    if(i == 14)
    {
        return 0;
    }

    return i + 1;
}

UINT8 rw_ieee80211_init_scan_chan(struct scanu_start_req *req)
{
    UINT32 i, start_chan, num_chan;

    ASSERT(g_country_code.init);
    ASSERT(req);

    start_chan = g_country_code.cfg.schan;

    if(g_country_code.cfg.policy == WIFI_COUNTRY_POLICY_MANUAL)
        num_chan = g_country_code.cfg.nchan;
    else {
        // auto mode, sta need scan all channel
        num_chan = g_wiphy.bands[IEEE80211_BAND_2GHZ].num_channels;
    }
    
    for(i = 0; i < num_chan; i ++)
    {
        req->chan[i].band = IEEE80211_BAND_2GHZ;
        req->chan[i].flags = 0;
        req->chan[i].freq = rw_ieee80211_get_centre_frequency(i + start_chan);
    }

#ifdef ENABLE_5GHZ_IEEE80211
    for(i = 0; i < g_wiphy.bands[IEEE80211_BAND_5GHZ].num_channels; i ++)
    {
        req->chan[i].band = IEEE80211_BAND_5GHZ;
        req->chan[i].flags = 0;
        req->chan[i].freq = rw_ieee80211_get_centre_frequency(i
                            + SCAN_CHANNEL_2G4
                            + start_chan);

        ASSERT(req->chan[i].freq);
    }
#endif

    req->chan_cnt = num_chan;

    return 0;
}

UINT8 rw_ieee80211_is_scan_rst_in_countrycode(UINT8 freq)
{
    UINT32 start_chan, end_chan;

    ASSERT(g_country_code.init);

    if(g_country_code.cfg.policy != WIFI_COUNTRY_POLICY_MANUAL)
    {
        // auto mode, sta need scan all channel, and get all rst
        return 1;
    }

    start_chan = g_country_code.cfg.schan;
    end_chan = (start_chan + g_country_code.cfg.nchan - 1);
    
    if((freq < start_chan) || (freq > end_chan))
        return 0;

    return 1;
}

#if CFG_IEEE80211N
void rw_ieee80211_set_ht_cap(UINT8 ht_supp)
{
	g_wiphy.bands[IEEE80211_BAND_2GHZ].ht_cap.ht_supported = ht_supp;
	rw_msg_send_me_config_req();
}
#endif

// eof
