/**
* @file tuya_device.c
* @author www.tuya.com
* @brief template demo for SDK WiFi & BLE for BK7231N, hardware: BK7231N NANO SoC board
* @version 0.2
* @date 2022-03-14
*
* @copyright Copyright (c) tuya.inc 2022
*
*/
#define _TUYA_DEVICE_GLOBAL

#include "uni_log.h"
#include "tuya_iot_wifi_api.h"
#include "tuya_hal_system.h"
#include "tuya_iot_com_api.h"
#include "tuya_cloud_error_code.h"
#include "gw_intf.h"
#include "tuya_gpio.h"
#include "tuya_key.h"
#include "tuya_led.h"

#include "tuya_device.h"

/***********************************************************
*************************micro define***********************
***********************************************************/
#define APP_RAW_PRINT_DEBUG 1

/* reset key config */
#define WIFI_KEY_PIN                TY_GPIOA_9  /* reset button pin */
#define WIFI_KEY_TIMER_MS           100         /* key scan poll time, default 100ms */
#define WIFI_KEY_LONG_PRESS_MS      3000        /* long press time */
#define WIFI_KEY_SEQ_PRESS_MS       400
#define WIFI_KEY_LOW_LEVEL_ENABLE   TRUE

/* led config */
#define WIFI_LED_PIN                TY_GPIOA_16 /* led pin */
#define WIFI_LED_LOW_LEVEL_ENABLE   TRUE
#define WIFI_LED_FAST_FLASH_MS      300         /* fast blink time */ 
#define WIFI_LED_SLOW_FLASH_MS      500         /* slow blink time */
#define LED_FLH_FOREVER             0xFFFF

/***********************************************************
***********************typedef define***********************
***********************************************************/

/***********************************************************
***********************variable define**********************
***********************************************************/
LED_HANDLE wifi_led_handle;

/***********************************************************
***********************function define**********************
***********************************************************/

/**
* @brief Input the current connection network status, according to different network status, led make different prompts.
*
* @param[in] cur_stat：current Wi-Fi connect status
* @return none
*/
STATIC VOID wifi_state_led_reminder(IN CONST GW_WIFI_NW_STAT_E cur_stat)
{
    switch (cur_stat)
    {
        case STAT_LOW_POWER:    /* Wi-Fi connection timeout, enter low power mode */
            tuya_set_led_light_type(wifi_led_handle, OL_HIGH, 0, 0); /* turn off */
        break;

        case STAT_UNPROVISION: /* SamrtConfig mode, wait connection */
            tuya_set_led_light_type(wifi_led_handle, OL_FLASH_HIGH, WIFI_LED_FAST_FLASH_MS, LED_FLH_FOREVER); /* fast blink */
        break;

        case STAT_AP_STA_UNCFG: /* AP mode, wait connection */
            tuya_set_led_light_type(wifi_led_handle, OL_FLASH_HIGH, WIFI_LED_SLOW_FLASH_MS, LED_FLH_FOREVER); /* slow blink */
        break;

        case STAT_AP_STA_DISC:
        case STAT_STA_DISC:     /* SamrtConfig/AP mode, connecting... */
            tuya_set_led_light_type(wifi_led_handle, OL_HIGH, 0, 0); /* turn off */
        break;

        case STAT_CLOUD_CONN:
        case STAT_AP_CLOUD_CONN: /* Connect to tuya Cloud */
            tuya_set_led_light_type(wifi_led_handle, OL_LOW, 0, 0); /* turn on */
        break;

        default:
        break;
    }

    return;
}

/**
* @brief button is pressed, call the function to process
*
* @param[in] port: button pin
* @param[in] type: button press type
* @param[in] cnt: press count
* @return none
*/
STATIC VOID wifi_key_process(TY_GPIO_PORT_E port, PUSH_KEY_TYPE_E type, INT_T cnt)
{
    OPERATE_RET op_ret = OPRT_OK;
    UCHAR_T ucConnectMode = 0;

    PR_DEBUG("port:%d, type:%d, cnt:%d", port, type, cnt);

    if (WIFI_KEY_PIN == port) {
        if (LONG_KEY == type) { /* long press enter connect network mode */
            PR_NOTICE("key long press");
            tuya_iot_wf_gw_unactive();
        } else if (NORMAL_KEY == type) {
            PR_NOTICE("key normal press");
        } else {
            PR_NOTICE("key type is no deal");
        }
    }

    return;
}

/**
* @brief initiation key, led
*
* @return none
*/
STATIC VOID wifi_config_init(VOID)
{
    OPERATE_RET op_ret = OPRT_OK;

    /* LED initiation */ 
    op_ret = tuya_create_led_handle(WIFI_LED_PIN, TRUE, &wifi_led_handle);
    if (OPRT_OK != op_ret) {
        PR_ERR("led init err:%d", op_ret);
        return;
    }
    tuya_set_led_light_type(wifi_led_handle, OL_LOW, 0, 0); //turn off

    /* key initiation */
    KEY_USER_DEF_S key_def;

    op_ret = key_init(NULL, 0, WIFI_KEY_TIMER_MS);
    if (OPRT_OK != op_ret) {
        PR_ERR("key init err:%d", op_ret);
        return;
    }

    /* config key parameter */
    memset(&key_def, 0, SIZEOF(key_def));
    key_def.port = WIFI_KEY_PIN;
    key_def.long_key_time = WIFI_KEY_LONG_PRESS_MS;
    key_def.low_level_detect = WIFI_KEY_LOW_LEVEL_ENABLE;
    key_def.lp_tp = LP_ONCE_TRIG;
    key_def.call_back = wifi_key_process;
    key_def.seq_key_detect_time = WIFI_KEY_SEQ_PRESS_MS;

    /* register key */
    op_ret = reg_proc_key(&key_def);
    if (OPRT_OK != op_ret) {
        PR_ERR("register WiFi key err:%d", op_ret);
    }

    return;
}

/**
* @brief pre_gpio_test gpio test pre-interface, used to prepare for the gpio test
*
* @return none
*/
VOID mf_user_pre_gpio_test_cb(VOID) 
{
    return;
}

/**
* @brief erase application data from flash
*
* @return none
*/
STATIC VOID hw_reset_flash_data(VOID)
{
    /* erase application data from flash */
    return;
}

/**
* @brief configuring the write callback interface
*
* @return none
*/
VOID mf_user_callback(VOID) 
{
    hw_reset_flash_data();

    return;
}

/**
* @brief configure to enter the production test callback interface
*
* @return none
*/
VOID mf_user_enter_callback(VOID) 
{
    return;
}

/**
* @brief Finished Product test callbacks
*
* @return OPRT_OK
*/
OPERATE_RET mf_user_product_test_cb(USHORT_T cmd, UCHAR_T *data, UINT_T len, OUT UCHAR_T **ret_data, OUT USHORT_T *ret_len) 
{
    return OPRT_OK;
}

/**
* @brief application initialization prep work
*
* @param[in] none
* @return none
*/
VOID pre_app_init(VOID) 
{
    return;
}

/**
* @brief device initialization prep work
*
* @param[in] none
* @return none
*/
VOID pre_device_init(VOID) 
{
    PR_NOTICE("%s", tuya_iot_get_sdk_info()); /* print SDK information */
    PR_NOTICE("%s:%s", APP_BIN_NAME, DEV_SW_VERSION); /* print the firmware name and version */
    PR_NOTICE("firmware compiled at %s %s", __DATE__, __TIME__); /* print firmware compilation time */
    PR_NOTICE("system reset reason:[%s]", tuya_hal_system_get_rst_info()); /* print system reboot causes */

    SetLogManageAttr(TY_LOG_LEVEL_DEBUG); /* set log level */

    return;
}

/**
* @brief application initialization interface
*
* @param[in] none
* @return none
*/
VOID app_init(VOID) 
{
    /* WiFi key, len initiation */
    wifi_config_init();

    return;
}

/**
* @brief report all dp status
*
* @param[in] none
* @return none
*/
VOID hw_report_all_dp_status(VOID)
{
    //report all dp status

    return;
}

/**
* @brief device activation status change callback
*
* @param[in] status: current status
* @return none
*/
VOID status_changed_cb(IN CONST GW_STATUS_E status)
{
    PR_NOTICE("status_changed_cb is status:%d", status);

    if(GW_NORMAL == status) {
        hw_report_all_dp_status();
    }else if(GW_RESET == status) {
        PR_NOTICE("status is GW_RESET");
    }

    return;
}

/**
* @brief firmware download finish result callback
*
* @param[in] fw: firmware info
* @param[in] download_result: 0 means download succes. other means fail
* @param[in] pri_data: private data
* @return none
*/
VOID upgrade_notify_cb(IN CONST FW_UG_S *fw, IN CONST INT_T download_result, IN PVOID_T pri_data)
{
    PR_DEBUG("download  Finish");
    PR_DEBUG("download_result:%d", download_result);

    return;
}

/**
* @brief firmware download content process callback
*
* @param[in] fw: firmware info
* @param[in] total_len: firmware total size
* @param[in] offset: offset of this download package
* @param[in] data && len: this download package
* @param[out] remain_len: the size left to process in next cb
* @param[in] pri_data: private data
* @return OPRT_OK: success  Other: fail
*/
OPERATE_RET get_file_data_cb(IN CONST FW_UG_S *fw, IN CONST UINT_T total_len, IN CONST UINT_T offset,
                                IN CONST BYTE_T *data, IN CONST UINT_T len, OUT UINT_T *remain_len, IN PVOID_T pri_data)
{
    PR_DEBUG("Rev File Data");
    PR_DEBUG("Total_len:%d ", total_len);
    PR_DEBUG("Offset:%d Len:%d", offset, len);

    return OPRT_OK;
}


/**
* @brief ota inform callback
*
* @param[in] fw: firmware info
* @return 
*/
INT_T gw_ug_inform_cb(IN CONST FW_UG_S *fw)
{
    PR_DEBUG("Rev GW Upgrade Info");
    PR_DEBUG("fw->fw_url:%s", fw->fw_url);
    PR_DEBUG("fw->sw_ver:%s", fw->sw_ver);
    PR_DEBUG("fw->file_size:%d", fw->file_size);

    return tuya_iot_upgrade_gw(fw, get_file_data_cb, upgrade_notify_cb, NULL);
}

/**
* @brief called after reset device or app remove device 
*
* @param[in] type: gateway reset type
* @return none
* @others reset factory clear flash data
*/
VOID gw_reset_cb(IN CONST GW_RESET_TYPE_E type)
{
    PR_DEBUG("gw_reset_cb type:%d", type);

    if(GW_REMOTE_RESET_FACTORY != type) {
        PR_DEBUG("type is GW_REMOTE_RESET_FACTORY");
        return;
    }

    hw_reset_flash_data();

    return;
}

/**
* @brief called after the cloud sends data of type bool, value, enum, string or fault
*
* @param[in] dp: obj dp info
* @return none
*/
VOID dev_obj_dp_cb(IN CONST TY_RECV_OBJ_DP_S *dp)
{
    UCHAR_T i = 0;

    PR_DEBUG("dp->cid:%s dp->dps_cnt:%d", dp->cid, dp->dps_cnt);

    for(i = 0; i < dp->dps_cnt; i++) {
        //deal_dp_proc(&(dp->dps[i]));
        dev_report_dp_json_async(get_gw_cntl()->gw_if.id, dp->dps, dp->dps_cnt);
    }

    return;
}

/**
* @brief called after the cloud sends data of type raw
*
* @param[in] dp: raw dp info
* @return none
*/
VOID dev_raw_dp_cb(IN CONST TY_RECV_RAW_DP_S *dp)
{
    PR_DEBUG("raw data dpid:%d", dp->dpid);
    PR_DEBUG("recv len:%d", dp->len);

#if APP_RAW_PRINT_DEBUG
    INT_T i = 0;
    for(i = 0; i < dp->len; i++) {
        PR_DEBUG_RAW("%02X ", dp->data[i]);
    }
    PR_DEBUG_RAW("\n");
#endif

    return;
}

/**
* @brief query device dp status
*
* @param[in] dp_qry: query info
* @return none
*/
STATIC VOID dev_dp_query_cb(IN CONST TY_DP_QUERY_S *dp_qry) 
{
    PR_NOTICE("Recv DP Query Cmd");

    hw_report_all_dp_status();

    return;
}

/**
* @brief This function is called when the state of the device connection has changed
*
* @param[in] stat: curr network status
* @return none
*/
VOID wf_nw_status_cb(IN CONST GW_WIFI_NW_STAT_E stat)
{
    PR_NOTICE("wf_nw_status_cb, wifi_status:%d", stat);

    wifi_state_led_reminder(stat);

    if (STAT_CLOUD_CONN == stat || STAT_AP_CLOUD_CONN == stat) {
        hw_report_all_dp_status();
    }

    return;
}

/**
* @brief device initialization interface
*
* @param[in] none
* @return OPRT_OK: success, others: please refer to tuya error code description document
*/
OPERATE_RET device_init(VOID) 
{
    OPERATE_RET op_ret = OPRT_OK;

    TY_IOT_CBS_S wf_cbs = {
        status_changed_cb,\ 
        gw_ug_inform_cb,\   
        gw_reset_cb,\
        dev_obj_dp_cb,\
        dev_raw_dp_cb,\
        dev_dp_query_cb,\
        NULL,
    };

    /* tuya IoT framework initialization */
    op_ret = tuya_iot_wf_soc_dev_init_param(GWCM_OLD_PROD, WF_START_SMART_FIRST, &wf_cbs, NULL, PRODUCT_ID, DEV_SW_VERSION);
    if(OPRT_OK != op_ret) {
        PR_ERR("tuya_iot_wf_soc_dev_init_param error,err_num:%d", op_ret);
        return op_ret;
    }

    /* register Wi-Fi connection status change callback function */
    op_ret = tuya_iot_reg_get_wf_nw_stat_cb(wf_nw_status_cb);
    if(OPRT_OK != op_ret) {
        PR_ERR("tuya_iot_reg_get_wf_nw_stat_cb is error,err_num:%d", op_ret);
        return op_ret;
    }

    return op_ret;
}

