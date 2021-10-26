#include "include.h"

#include "drv_model_pub.h"
#include "intc_pub.h"
#include "uart_pub.h"

#include "ble.h"
#include "ble_pub.h"
#include "ble_api.h"
#include "rwble.h"
#include "app_task.h"
#include "udebug.h"
#include "typedef.h"
#include "sys_ctrl_pub.h"
#include "icu_pub.h"
#include "intc_pub.h"
#include "common_bt.h"
#include "arm_arch.h"
#include "rw_pub.h"
#include "power_save_pub.h"
#include "gapc_task.h" 
#include "bk7011_cal_pub.h"

#define BLE_MSG_QUEUE_COUNT          (20)

beken_queue_t ble_msg_que = NULL;
beken_thread_t ble_thread_handle = NULL;
#if (CFG_DEFAULT_RF_USER == CFG_RF_USER_BLE)
beken_thread_t rf_switch_thread = NULL;
int rf_loop = 0;
#endif
uint32_t ble_stack_size = 4096;
uint32_t  ble_dut_flag = 0;
ble_event_cb_t ble_event_cb = NULL;
ble_recv_adv_cb_t ble_recv_adv_cb = NULL;
bk_ble_read_cb_t bk_ble_read_cb = NULL;
bk_ble_write_cb_t bk_ble_write_cb = NULL;
ble_role_t ble_role_mode = BLE_ROLE_NONE;
uint8_t ble_switch_old_state = HW_IDLE;
uint8_t ble_switch_mac_sleeped = 0;
uint8_t ble_active = 0;

typedef struct ble_cfg_st {
    struct bd_addr mac;
    char name[APP_DEVICE_NAME_LENGTH_MAX];
}BLE_CFG_ST, *BLE_CFG_PTR; 
BLE_CFG_ST ble_cfg;

static SDD_OPERATIONS ble_op =
{
    ble_ctrl
};


extern /*const */struct bd_addr common_default_bdaddr;
extern void uart_isr(void);
extern void intc_service_change_handler(UINT8 int_num, FUNCPTR isr);
extern void wifi_get_mac_address(char *mac, u8 type);
extern void rwnxl_reset_handle(int dummy);
extern uint32_t ps_get_sleep_prevent(void);
extern UINT32 txl_cntrl_pck_get(void );
extern void sctrl_modem_core_reset(void);
extern void delay(INT32 num);
extern void rw_main(void);
extern void appm_update_param(struct gapc_conn_param *conn_param);

void ble_intc_set(uint32_t enable)
{
    if(enable)
        intc_enable(FIQ_BLE);
    else
        intc_disable(FIQ_BLE);
}

void ble_clk_power_up(void)
{
    UINT32 param;
    param = PWD_BLE_CLK_BIT;
    sddev_control(ICU_DEV_NAME, CMD_TL410_CLK_PWR_UP, &param);
}

void ble_clk_power_down(void)
{
    UINT32 param;
    param = PWD_BLE_CLK_BIT;
    sddev_control(ICU_DEV_NAME, CMD_TL410_CLK_PWR_DOWN, &param);
}

UINT32 rf_wifi_used = 0;

void rf_wifi_used_set(void)
{
    GLOBAL_INT_DECLARATION();
    GLOBAL_INT_DISABLE();
    if(0 == rf_wifi_used)
    {
        rf_wifi_used = 1;
    }
    GLOBAL_INT_RESTORE();
}

void rf_wifi_used_clr(void)
{
    GLOBAL_INT_DECLARATION();
    GLOBAL_INT_DISABLE();
    if(1 == rf_wifi_used)
    {
        rf_wifi_used = 0;
    }
    GLOBAL_INT_RESTORE();
}

UINT32 if_rf_wifi_used(void )
{
    uint32_t value = 0;
    GLOBAL_INT_DECLARATION();
    GLOBAL_INT_DISABLE();
    value =  rf_wifi_used;
    GLOBAL_INT_RESTORE();
    return value;
}

uint8 is_rf_switch_to_ble(void)
{
    UINT32 param;
    
    sddev_control(SCTRL_DEV_NAME, CMD_BLE_RF_BIT_GET, &param);

    return (param > 0 ) ? 1 : 0;
}

void ble_switch_rf_to_wifi(void)
{
	// if in ble dut mode, no need change back to wifi any more.
	// ble dut mode can not exit until power off
	//  if (!is_rf_switch_to_ble() || power_save_if_rf_sleep())
  	if (!is_rf_switch_to_ble())
        return;

    GLOBAL_INT_DECLARATION();
    GLOBAL_INT_DISABLE();
#if ATE_APP_FUN
    if(!get_ate_mode_state())
#endif
    {
        rwnx_cal_recover_txpwr_for_wifi();
    }
    
    rwnx_cal_ble_recover_rfconfig();
    
    sddev_control(SCTRL_DEV_NAME, CMD_BLE_RF_BIT_CLR, NULL);
    extern void sctrl_set_rf_sleep(void);
    sctrl_set_rf_sleep();//after swtich wifi check if can stop rf
	ble_switch_mac_sleeped = 0;
	if(power_save_if_rf_sleep())
	{
		GLOBAL_INT_RESTORE();
		return;
	}
    if (ble_switch_old_state != HW_IDLE && nxmac_current_state_getf() == HW_IDLE)
    {
        if(ke_state_get(TASK_MM) == MM_ACTIVE)
        {
            nxmac_next_state_setf(ble_switch_old_state);
            while (nxmac_current_state_getf() != ble_switch_old_state);
        }
    }


	if (!power_save_if_rf_sleep())
	{
        power_save_rf_ps_wkup_semlist_set();
	}
    GLOBAL_INT_RESTORE();

    //Re-enable MAC interrupts
    nxmac_enable_master_gen_int_en_setf(1);
    nxmac_enable_master_tx_rx_int_en_setf(1);

    //PS_DEBUG_RF_UP_TRIGER;
}

#define MAX_SKIP_CNT 5
uint32_t ble_switch_skip_cnt = 0;
void ble_switch_data_pend(void)
{
    return;
    while(ble_switch_skip_cnt == MAX_SKIP_CNT)
    {
        bk_printf("p");
        rtos_delay_milliseconds(2) ;
    }
}

void ble_switch_clear_mac_interrupts(void)
{
    uint32_t fiq_status; 
        
    hal_machw_disable_int();
    nxmac_tx_rx_int_ack_clear(0xffffffff);
    nxmac_gen_int_ack_clear(0xffffffff);
    fiq_status = sddev_control(ICU_DEV_NAME, CMD_GET_INTR_STATUS, 0);
    fiq_status = fiq_status & 0xFFFF0000;
    sddev_control(ICU_DEV_NAME, CMD_CLR_INTR_STATUS, &fiq_status);
}

void ble_switch_rf_to_ble(void)
{
    if(if_rf_wifi_used())
        return;

    GLOBAL_INT_DECLARATION();
    GLOBAL_INT_DISABLE();

    if (txl_cntrl_pck_get() && ble_switch_skip_cnt < MAX_SKIP_CNT)
    {
        ble_switch_skip_cnt++;
		GLOBAL_INT_RESTORE();
        return;
    }

	if (ps_get_sleep_prevent() & 0x37)
    {
    	GLOBAL_INT_RESTORE();
        return;
    }

    if(REG_READ((ICU_BASE + 19 * 4))
           & (CO_BIT(FIQ_MAC_TX_RX_MISC)
              | CO_BIT(FIQ_MAC_TX_RX_TIMER)
              | CO_BIT(FIQ_MAC_RX_TRIGGER)
              | CO_BIT(FIQ_MAC_TX_TRIGGER)
              | CO_BIT(FIQ_MAC_GENERAL)
              | CO_BIT(FIQ_MAC_PROT_TRIGGER)
              | CO_BIT(FIQ_DPLL_UNLOCK)
              ))
    {
    	GLOBAL_INT_RESTORE();
        return;
    }

    ble_switch_skip_cnt = 0;
    
    ble_switch_old_state = nxmac_current_state_getf();

    // Ask HW to go to IDLE
    if (ble_switch_old_state == HW_ACTIVE)
    {
        uint32_t i_tmp = 0, y_tmp = 0;
        uint32_t v_tmp;
        
        // Ask HW to go to IDLE
        if (nxmac_current_state_getf() != HW_IDLE)
        {
            nxmac_next_state_setf(HW_IDLE);

            while(1) 
            {
                if(nxmac_status_idle_interrupt_getf() == 1)
                    break;
                else
                {
                    i_tmp++;
                    if(i_tmp > 1000)
                    {
                        i_tmp = 0;
                        v_tmp =nxmac_state_cntrl_get();
                        PS_WPRT("s s:0x%x c:0x%x\r\n",v_tmp,power_save_get_sleep_count());
                        y_tmp ++;
                        if(y_tmp > 500)
                        {
                            y_tmp = 0;
                            PS_WPRT("idle rec\r\n");
                            bk_printf("idle rec\r\n");
                            //long time wait idle interrupt,recover!
                            hal_machw_disable_int();
                            rwnxl_reset_handle(0);
                            PS_WPRT("idle rec over\r\n");
                            nxmac_next_state_setf(HW_IDLE);
                        }
                    }
                }
            }
            //while (nxmac_status_idle_interrupt_getf() != 1);
            nxmac_gen_int_ack_clear(NXMAC_IDLE_INTERRUPT_BIT);

            // reset modem.
            sctrl_modem_core_reset();

            // Disable MAC interrupts and clear all existing ones
            ble_switch_clear_mac_interrupts();
        }
    }
    else
        ble_switch_old_state = HW_IDLE;
    
    sctrl_rf_wakeup();//after swtich ble check if need start rf
    
    sddev_control(SCTRL_DEV_NAME, CMD_BLE_RF_BIT_SET, NULL);

    rwnx_cal_ble_set_rfconfig();

#if ATE_APP_FUN    
    if(!get_ate_mode_state())
#endif
    {
        rwnx_cal_set_txpwr_for_ble_boardcast();
    }

    ble_switch_mac_sleeped = 1;

    GLOBAL_INT_RESTORE();

    //PS_DEBUG_RF_UP_TRIGER;
}

void ble_request_rf_by_isr(void)
{
#if (CFG_DEFAULT_RF_USER == CFG_RF_USER_WIFI)
    if(!ble_dut_flag)
    {
    ble_switch_rf_to_ble();
    }
#else
    if (kernel_state_get(TASK_APP) == APPM_CONNECTED)
    {
        ble_switch_rf_to_ble();
    }
#endif
}

void ble_release_rf_by_isr(void)
{
#if (CFG_DEFAULT_RF_USER == CFG_RF_USER_WIFI)
    if(!ble_dut_flag)
    {
	    ble_switch_rf_to_wifi();
    }
#else
    if (kernel_state_get(TASK_APP) == APPM_CONNECTED)
    {
        ble_switch_rf_to_wifi();
    }
#endif    
}

void ble_set_power_up(uint32 up)
{
    if(up)
        sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_BLE_POWERUP, NULL);
    else
    {
#if ATE_APP_FUN
        if(!get_ate_mode_state())
#endif
        {
            sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_BLE_POWERDOWN, NULL);
        }
    }
}

void ble_set_pn9_trx(uint32 param)
{
    UINT32 reg;
    
    if(PN9_RX == param)
    {
        reg = 0x0;
        REG_WRITE(REG_BLE_XVR_TRX_CONFIG_ADDR, reg);
        delay(10);
        reg = 0x2400;
        REG_WRITE(REG_BLE_XVR_TRX_CONFIG_ADDR, reg);
    }
    else
    {
        reg = 0x0;
        REG_WRITE(REG_BLE_XVR_TRX_CONFIG_ADDR, reg);
        delay(10);
        reg = 0x3800;
        REG_WRITE(REG_BLE_XVR_TRX_CONFIG_ADDR, reg);
    }
}

void ble_init(void)
{
	intc_service_register( FIQ_BLE, PRI_FIQ_BLE, ble_isr );

	sddev_register_dev( BLE_DEV_NAME, &ble_op );

	return;
}

void ble_exit(void)
{
	sddev_unregister_dev( BLE_DEV_NAME );

	return;
}

UINT32 ble_ctrl( UINT32 cmd, void *param )
{
    UINT32 reg;
	UINT32 ret = ERR_SUCCESS;

	switch(cmd)
	{
	case CMD_BLE_REG_INIT:
		break;
		
	case CMD_BLE_REG_DEINIT:
		break;

    case CMD_BLE_SET_CHANNEL:
        reg = REG_READ(REG_BLE_XVR_CHANNEL_CONFIG_ADDR);
        reg &= ~(REG_BLE_XVR_CHANNEL_VALUE_MASK << REG_BLE_XVR_CHANNEL_VALUE_POST);
        reg |= (*(UINT32 *)param) << REG_BLE_XVR_CHANNEL_VALUE_POST;
        REG_WRITE(REG_BLE_XVR_CHANNEL_CONFIG_ADDR, reg);
        reg = REG_READ(REG_BLE_XVR_TRX_CONFIG_ADDR);
        reg &= ~(1 << REG_BLE_XVR_TEST_RADIO_POST);
        REG_WRITE(REG_BLE_XVR_TRX_CONFIG_ADDR, reg);
        delay(10);
        reg |= (1 << REG_BLE_XVR_TEST_RADIO_POST);
        REG_WRITE(REG_BLE_XVR_TRX_CONFIG_ADDR, reg);
        break;

    case CMD_BLE_AUTO_CHANNEL_ENABLE:
        reg = REG_READ(REG_BLE_XVR_CHANNEL_CONFIG_ADDR);
        reg |= 1 << REG_BLE_XVR_AUTO_CHANNEL_POST;
        REG_WRITE(REG_BLE_XVR_CHANNEL_CONFIG_ADDR, reg);
        break;

    case CMD_BLE_AUTO_CHANNEL_DISABLE:
        reg = REG_READ(REG_BLE_XVR_CHANNEL_CONFIG_ADDR);
        reg &= ~(1 << REG_BLE_XVR_AUTO_CHANNEL_POST);
        REG_WRITE(REG_BLE_XVR_CHANNEL_CONFIG_ADDR, reg);
        break;

    case CMD_BLE_AUTO_SYNCWD_ENABLE:
        reg = REG_READ(REG_BLE_XVR_CHANNEL_CONFIG_ADDR);
        reg |= 1 << REG_BLE_XVR_AUTO_SYNCWD_POST;
        REG_WRITE(REG_BLE_XVR_CHANNEL_CONFIG_ADDR, reg);
        break;

    case CMD_BLE_AUTO_SYNCWD_DISABLE:
        reg = REG_READ(REG_BLE_XVR_CHANNEL_CONFIG_ADDR);
        reg &= ~(1 << REG_BLE_XVR_AUTO_SYNCWD_POST);
        REG_WRITE(REG_BLE_XVR_CHANNEL_CONFIG_ADDR, reg);
        break;

    case CMD_BLE_SET_PN9_TRX:
        ble_set_pn9_trx(*(UINT32 *)param);
        break;

    case CMD_BLE_SET_GFSK_SYNCWD:
        reg = (*(UINT32 *)param);
        REG_WRITE(REG_BLE_XVR_GFSK_SYNCWD_ADDR, reg);
        break;

    case CMD_BLE_HOLD_PN9_ESTIMATE:
        reg = REG_READ(REG_BLE_XVR_TRX_CONFIG_ADDR);
        reg |= 1 << REG_BLE_XVR_HOLD_ESTIMATE_POST;
        REG_WRITE(REG_BLE_XVR_TRX_CONFIG_ADDR, reg);
        break;

    case CMD_BLE_STOP_COUNTING:
        reg = REG_READ(REG_BLE_XVR_TRX_CONFIG_ADDR);
        reg &= ~(1 << REG_BLE_XVR_PN9_RECV_POST);
        REG_WRITE(REG_BLE_XVR_TRX_CONFIG_ADDR, reg);
        break;

    case CMD_BLE_START_COUNTING:
        reg = REG_READ(REG_BLE_XVR_TRX_CONFIG_ADDR);
        reg |= 1 << REG_BLE_XVR_PN9_RECV_POST;
        reg &= ~(1 << REG_BLE_XVR_HOLD_ESTIMATE_POST);
        REG_WRITE(REG_BLE_XVR_TRX_CONFIG_ADDR, reg);
        break;

	default:
		ret = ERR_CMD_NOT_SUPPORT;
        break;
	}
	
	return ret;
}

void ble_isr(void)
{
	rwble_isr();
	return;
}

static void ble_main( void *arg )
{
    memcpy(&common_default_bdaddr, &ble_cfg.mac, sizeof(struct bd_addr));
    memcpy(&app_dflt_dev_name, &ble_cfg.name, APP_DEVICE_NAME_LENGTH_MAX); 

    if(!ble_dut_flag)
    {
        UINT8 *mac = (UINT8 *)&ble_cfg.mac;
        
        os_printf("ble name:%s, %02x:%02x:%02x:%02x:%02x:%02x\r\n", 
            app_dflt_dev_name, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    }
	rw_main();

    rtos_deinit_queue(&ble_msg_que);
    ble_msg_que = NULL;
#if (CFG_DEFAULT_RF_USER == CFG_RF_USER_BLE)
    rf_loop = 0;
#endif
    ble_thread_handle = NULL;
	rtos_delete_thread(NULL);
}

#if (CFG_DEFAULT_RF_USER == CFG_RF_USER_BLE)
ble_err_t ble_request_rf(rf_time_t *rt)
{
    ble_err_t rc = ERR_SUCCESS;
    rw_evt_type sta_stat;

    if(rt == NULL)
    {
        return ERR_REQ_RF;
    }

    sta_stat = mhdr_get_station_status();

    //bk_printf("sta_stat:%d\r\n", sta_stat);

    if(sta_stat == RW_EVT_STA_GOT_IP)
    {
        if(power_save_if_rf_sleep())
        {
            sctrl_rf_wakeup();

            /* wake up after a beacon package, next beacon will coming in 100 ms */
            UINT32 power_save_time_to_sleep(void);
            rt->ble_rf_time = power_save_time_to_sleep();
            if(rt->ble_rf_time < 10)
            {
                rt->ble_rf_time = 10;
            }
            else
            {
                rt->ble_rf_time -= 3;
            }
            rt->wifi_rf_time = 15;
        }
        else
        {
            /*
            rt->ble_rf_time = 15;
            rt->wifi_rf_time = 60;
            */
            rt->ble_rf_time = 10;
        }
    }
    else if(sta_stat == MSG_IDLE)
    {
        rt->ble_rf_time = 200;
        rt->wifi_rf_time = 10;
    }
    else
    {
        rt->ble_rf_time = 20;
        rt->wifi_rf_time = 100;
    }

    return rc;
}

void ble_release_rf(void *param)
{
    ble_switch_rf_to_wifi();
    return;
}

void rf_switch_loop(void *param)
{
    rt_timer_t ble_timer;
    rf_time_t rf_tm;
    int timeout_tick;
    int *loop = (int *)param;

    ble_timer = rt_timer_create("ble_timer", ble_release_rf, NULL, 10, RT_TIMER_FLAG_ONE_SHOT|RT_TIMER_FLAG_SOFT_TIMER);
    if (NULL == ble_timer)
    {
        bk_printf("create one shot ble timer failed\n");
        return;
    }

    bk_printf("rf_switch_loop start\r\n");

    while (*loop)
    {
        if(ble_request_rf(&rf_tm) == ERR_REQ_RF)
        {
            break;
        }
        /* switch rf to ble if ble can run more than 10 ms */
        if (rf_tm.ble_rf_time > 10)
        {
            timeout_tick = rt_tick_from_millisecond(rf_tm.ble_rf_time);
            rt_timer_control(ble_timer, RT_TIMER_CTRL_SET_TIME, (void *)&timeout_tick);
            rt_timer_start(ble_timer);

            ble_switch_rf_to_ble();
            /* dealing with scan result in this interval... no matter in this task or others */
            timeout_tick = rt_tick_from_millisecond(rf_tm.ble_rf_time + rf_tm.wifi_rf_time);
            rt_thread_delay(timeout_tick);
        }
        else
        {
            timeout_tick = rt_tick_from_millisecond(10);//check power save 10ms later
            rt_thread_delay(timeout_tick);
        }
    }

    rf_switch_thread = NULL;

    rt_timer_delete(ble_timer);

    bk_printf("rf_switch_loop end\r\n");
}
#endif

static void ble_thread_start(void)
{
	OSStatus ret; 

    if(!ble_thread_handle && !ble_msg_que)
    {
    	ret = rtos_init_queue(&ble_msg_que, 
    							"ble_msg_queue",
    							sizeof(BLE_MSG_T),
    							BLE_MSG_QUEUE_COUNT);
        ASSERT(0 == ret);
        
    	ret = rtos_create_thread(&ble_thread_handle, 
    			6,
    			"ble", 
    			(beken_thread_function_t)ble_main, 
    			(unsigned short)ble_stack_size, 
    			(beken_thread_arg_t)0);
    	
        ASSERT(0 == ret);
    }
#if (CFG_DEFAULT_RF_USER == CFG_RF_USER_BLE)
    if(!rf_switch_thread)
    {
        rf_loop = 1;
        rtos_create_thread(&rf_switch_thread, 15, "rf_switch_loop", rf_switch_loop, 2048, &rf_loop);
    }
#endif
}

void ble_stop(void)
{
    if(ble_thread_handle) {
        ble_send_msg(BLE_MSG_EXIT);

        while(ble_thread_handle)
            rtos_delay_milliseconds(100);
    }
}

void ble_activate(char *ble_name)
{
    UINT32 len;

//    bk_wlan_stop(1);

    if((!ble_name) && (!ble_dut_flag)) {
        os_printf("ble start no ble name\r\n");
        ble_name = APP_DFLT_DEVICE_NAME;
    }

    ble_stop();

    memset(&ble_cfg, 0, sizeof(BLE_CFG_ST));
    
    wifi_get_mac_address((char *)&ble_cfg.mac, 2);  // get sta's mac addr 
    ble_cfg.mac.addr[0] += 1; // add 1, diff from wifi's mac 

    len = strlen(ble_name);
    len = (len > APP_DEVICE_NAME_LENGTH_MAX)? APP_DEVICE_NAME_LENGTH_MAX:len;
    memcpy(&ble_cfg.name, ble_name, len);
    
    ble_thread_start();
}

void ble_dut_start(void)
{
    if(!ble_thread_handle) 
    {
        ble_dut_flag = 1;		
		extern uint8_t system_mode;
        system_mode = RW_DUT_MODE;
        os_printf("enter ble dut\r\n");
        rwnx_no_use_tpc_set_pwr();
        sddev_control(SCTRL_DEV_NAME, CMD_BLE_RF_BIT_SET, NULL);

        #if (BLE_DUT_UART_PORT == PORT_UART2)
        intc_service_change_handler(IRQ_UART2, uart_isr);
        #else
        intc_service_change_handler(IRQ_UART1, uart_isr);
        #endif // (BLE_DUT_UART_PORT == PORT_UART2)
        
        ble_activate(NULL);
    }
}

UINT8 ble_is_start(void)
{
    return (ble_thread_handle == NULL)? 0:1;
}

UINT8* ble_get_mac_addr(void)
{
    return (UINT8*)&common_default_bdaddr;
}

UINT8* ble_get_name(void)
{
    return (UINT8*)&app_dflt_dev_name;
}

void ble_send_msg(UINT32 data)
{
	OSStatus ret;
	BLE_MSG_T msg;

    if(ble_msg_que) {
    	msg.data = data;
    	
    	ret = rtos_push_to_queue(&ble_msg_que, &msg, BEKEN_NO_WAIT);
    	if(0 != ret)
    	{
    		//os_printf("ble_send_msg failed\r\n");
    	}
    }
}

int blemsg_is_empty(void)
{
    if(!rtos_is_queue_empty(&ble_msg_que))
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

void ble_update_connection(void)
{
    struct gapc_conn_param  up_param;

    up_param.intv_min   = 39;///78;
    up_param.intv_max   = 40;//80;
    up_param.latency    = 0;
    up_param.time_out   = BLE_UAPDATA_TIMEOUT;

    appm_update_param(&up_param);
}

void ble_update_connection2(void)
{
    struct gapc_conn_param  up_param;

    up_param.intv_min   = 78;
    up_param.intv_max   = 80;
    up_param.latency    = 10;
    up_param.time_out   = BLE_UAPDATA_TIMEOUT;

    appm_update_param(&up_param);
}

unsigned char mhdr_wifi_status = RW_EVT_STA_IDLE;
void ble_is_wifi_status(void)
{
	if((kernel_state_get(TASK_APP) == APPM_CONNECTED)
		&& (mhdr_get_station_status() == RW_EVT_STA_GOT_IP) 
		&& (mhdr_wifi_status != RW_EVT_STA_GOT_IP))
	{
		mhdr_wifi_status = mhdr_get_station_status();
		ble_update_connection2();
    }
	else if((kernel_state_get(TASK_APP) == APPM_CONNECTED)
		&& (mhdr_get_station_status() != RW_EVT_STA_GOT_IP) 
		&& (mhdr_wifi_status == RW_EVT_STA_GOT_IP))
	{
		mhdr_wifi_status = mhdr_get_station_status();
		ble_update_connection();
    }
}

void ble_set_write_cb(bk_ble_write_cb_t func)
{
    bk_ble_write_cb = func;
}

void ble_set_event_cb(ble_event_cb_t func)
{
    ble_event_cb = func;
}

void ble_set_recv_adv_cb(ble_recv_adv_cb_t func)
{
    ble_recv_adv_cb = func;
}

void ble_set_read_cb(bk_ble_read_cb_t func)
{
    bk_ble_read_cb = func;
}

void ble_set_role_mode(ble_role_t role)
{
    ble_role_mode = role;
}

ble_role_t ble_get_role_mode()
{
    return ble_role_mode;
}

UINT32 ble_in_dut_mode(void)
{
    return (ble_dut_flag == 1)  ? 1 :  0;
}
