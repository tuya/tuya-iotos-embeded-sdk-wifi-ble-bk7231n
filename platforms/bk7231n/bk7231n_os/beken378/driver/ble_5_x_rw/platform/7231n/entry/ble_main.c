/**
 ****************************************************************************************
 *
 * @file arch_main.c
 *
 * @brief Main loop of the application.
 *
 * Copyright (C) RivieraWaves 2009-2015
 *
 *
 ****************************************************************************************
 */
#include "rwip_config.h" // RW SW configuration
#include "architect.h"      // architectural platform definitions
#include <stdlib.h>    // standard lib functions
#include <stddef.h>    // standard definitions
#include <stdint.h>    // standard integer definition
#include <stdbool.h>   // boolean definition
#include <string.h>   // boolean definition
#include "rwip.h"      // RW SW initialization
#include "prf.h"      // RW SW initialization
#include "rwble.h"
#include "uart_pub.h"
#include "rtos_pub.h"
#include "ble.h"
#include "ble_pub.h"
#include "ble_api.h"
#include "sys_ctrl_pub.h"
#include "icu_pub.h"
#include "intc_pub.h"
#include "drv_model_pub.h"
#include "uart_ble.h"
#include "ble_reg_blecore.h"
#include "BK3633_RegList.h"
#include "param_config.h"
#include "common_utils.h"

beken_queue_t ble_msg_que = NULL;
beken_thread_t ble_thread_handle = NULL;
uint8_t ble_system_mode;
uint8_t tx_pwr_idx;

enum {
	DUT_IDLE,
	DUT_RUNNING,
};

uint8_t ble_dut_status = DUT_IDLE;

const struct rwip_eif_api uart_api =
{
    ble_uart_read,
    ble_uart_write,
    ble_uart_flow_on,
    ble_uart_flow_off,
};

struct ble_session_env ble_session_ctx =
{
	.ble_active = 0,
	.system_sleep_flag = 0,
	.ble_first_sleep = 1,
	.ble_deep_sleep = 0,
	.ble_init_over = 0,
};

void set_ble_active_status(char active)
{
	ble_session_ctx.ble_active = (active > 0)?1:0;
}

int get_ble_active_status(void)
{
	return (ble_session_ctx.ble_active != 0)?1:0;
}

static SDD_OPERATIONS ble_op =
{
    ble_ctrl
};

void assert_err(const char *condition, const char * file, int line)
{
	bk_printf("%s,condition %s,file %s,line = %d\r\n",__func__,condition,file,line);
}

void assert_param(int param0, int param1, const char * file, int line)
{
	bk_printf("%s,param0 = %d,param1 = %d,file = %s,line = %d\r\n",__func__,param0,param1,file,line);
}

void assert_warn(int param0, int param1, const char * file, int line)
{
	bk_printf("%s,param0 = %d,param1 = %d,file = %s,line = %d\r\n",__func__,param0,param1,file,line);
}

void dump_data(uint8_t* data, uint16_t length)
{
	bk_printf("%s,data = %d,length = %d,file = %s,line = %d\r\n",__func__,data,length);
}

void platform_reset(uint32_t error)
{    
	bk_printf("reset error = %x\r\n", error);

	//watch dog reset
	extern void bk_reboot(void);
	bk_reboot();
}

void ble_set_power_up(uint32 up)
{
	if (up) {
		sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_BLE_POWERUP, NULL);
	} else {
		sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_BLE_POWERDOWN, NULL);
	}
}
#include "common_bt_defines.h"
extern void wifi_get_mac_address(char *mac, u8 type);
void bdaddr_env_init(void)
{
        uint8_t sta_mac[BD_ADDR_LEN];
        uint8_t *ble_mac = &common_default_bdaddr.addr[0];

        wifi_get_mac_address((char *)sta_mac, CONFIG_ROLE_STA);
        sta_mac[5] += 1; // add 1, diff from wifi's mac

        for (int i = 0; i < BD_ADDR_LEN; i++) {
                ble_mac[i] = sta_mac[BD_ADDR_LEN - 1 - i];
        }
        bk_printf("ble mac:%02x-%02x-%02x-%02x-%02x-%02x\r\n",
                ble_mac[5], ble_mac[4], ble_mac[3], ble_mac[2], ble_mac[1], ble_mac[0]);
}

void ble_sys_mode_init(uint8_t mode)
{
	ble_system_mode = mode;
}

uint8_t ble_get_sys_mode(void)
{
	return ble_system_mode;
}

void ble_clk_power_up(uint32 up)
{
	UINT32 param;
	param = PWD_BLE_CLK_BIT;
	
	if (up) {
		sddev_control(ICU_DEV_NAME, CMD_TL410_CLK_PWR_UP, &param);
	} else {
		sddev_control(ICU_DEV_NAME, CMD_TL410_CLK_PWR_DOWN, &param);	
	}
}

void ble_intc_enable(uint32_t enable)
{
    if (enable) {
		intc_enable(FIQ_BTDM);
        intc_enable(FIQ_BLE);
    } else {
		intc_disable(FIQ_BTDM);
		intc_disable(FIQ_BLE);
    }
}

void ble_btdm_isr(void)
{
	rwip_isr();
	ble_send_msg(BLE_MSG_POLL);
	return;
}

void ble_ble_isr(void)
{
	rwble_isr();
	ble_send_msg(BLE_MSG_POLL);
	return;
}

void ble_init(void)
{
	intc_service_register( FIQ_BLE, PRI_FIQ_BLE, ble_ble_isr );
	intc_service_register( FIQ_BTDM, PRI_FIQ_BTDM, ble_btdm_isr );

	sddev_register_dev( BLE_DEV_NAME, &ble_op );

	return;
}

void ble_exit(void)
{
	sddev_unregister_dev( BLE_DEV_NAME );

	return;

}

void ble_send_msg(UINT32 data)
{
	OSStatus ret;
	BLE_MSG_T msg;

    if (ble_msg_que) {
    	msg.data = data;    	
    	ret = rtos_push_to_queue(&ble_msg_que, &msg, BEKEN_NO_WAIT);
    }
}

extern int bk7011_reduce_vdddig_for_rx(int reduce);
extern void intc_service_change_handler(UINT8 int_num, FUNCPTR isr);
#define		BLE_DUT_DIVISION			(8)

void enter_dut_fcc_mode(void)
{
	bk_printf("enter dut mode\r\n");
	uint32_t reg;
	static uint32_t default_mclk_mux = MCLK_SELECT_DPLL;
	static uint32_t default_mclk_div = BLE_DUT_DIVISION;

#ifdef FOR_TEST
#if (BLE_DUT_UART_PORT == PORT_UART2)
	intc_service_change_handler(IRQ_UART2, ble_uart_isr);
#else
	intc_service_change_handler(IRQ_UART1, ble_uart_isr);
#endif
#endif
	while (1) {
		OSStatus err;
		BLE_MSG_T msg;

		err = rtos_pop_from_queue(&ble_msg_que, &msg, BEKEN_WAIT_FOREVER);
		if (kNoErr == err) {
			switch (msg.data) {
				case BLE_MSG_POLL:
					//schedule all pending events      
					rwip_schedule();
					break;
				case BLE_MSG_DUT:
					//ble test code for debug pin
					//ble_diagcntl_set(0x00008383);
					uart_h4tl_data_switch();
					break;
				case BLE_DUT_EXIT:
					bk_printf("exit ble dut\r\n");
					bk7011_reduce_vdddig_for_rx(0);
					ble_dut_status = DUT_IDLE;
					sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_MCLK_SELECT, &default_mclk_mux);
					sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_MCLK_DIVISION, &default_mclk_div);

					sddev_control(SCTRL_DEV_NAME, CMD_BLE_RF_BIT_CLR, NULL);
					extern void ble_cal_recover_txpwr(void);
					ble_cal_recover_txpwr();
#if (BLE_DUT_UART_PORT == PORT_UART2)
					intc_service_change_handler(IRQ_UART2, uart2_isr);
#else
					intc_service_change_handler(IRQ_UART1, uart1_isr);
#endif
					break;
				case BLE_DUT_START:
					bk_printf("enter ble dut\r\n");
					bk7011_reduce_vdddig_for_rx(1);
					ble_dut_status = DUT_RUNNING;
					sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_MCLK_MUX_GET, &default_mclk_mux);
					sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_MCLK_DIV_GET, &default_mclk_div);
					reg = MCLK_SELECT_DPLL;
					sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_MCLK_SELECT, &reg);
					reg = BLE_DUT_DIVISION;
					sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_MCLK_DIVISION, &reg);

					sddev_control(SCTRL_DEV_NAME, CMD_BLE_RF_BIT_SET, NULL);
					extern void ble_cal_set_txpwr(uint8_t idx);
					ble_cal_set_txpwr(tx_pwr_idx);
#if (BLE_DUT_UART_PORT == PORT_UART2)
					intc_service_change_handler(IRQ_UART2, ble_uart_isr);
#else
					intc_service_change_handler(IRQ_UART1, ble_uart_isr);
#endif
					break;
				default:
					break;
			}
		}
	}
}

extern UINT32 ble_ps_enabled(void );
void enter_normal_app_mode(void)
{
	bk_printf("enter normal mode\r\n");

	while (1) {
		OSStatus err;
		BLE_MSG_T msg;

		err = rtos_pop_from_queue(&ble_msg_que, &msg, BEKEN_WAIT_FOREVER);
		if (kNoErr == err) {
			switch (msg.data) {
				case BLE_MSG_POLL:
					//schedule all pending events
					//ble test code for debug pin
					//ble_diagcntl_set(0x00008383);
					rwip_schedule();
					break;
				default:
					break;
			}
		}

 		if (ble_ps_enabled())
 		{
			GLOBAL_INT_DIS();
			rwip_sleep();
			GLOBAL_INT_RES();
		}
	}
}

extern void ble_thread_exit_cb(void);
extern uint32_t get_ate_mode_state(void);
void ble_thread_main(void *arg)
{
	ble_set_power_up(1);

	ble_clk_power_up(1);

	ble_intc_enable(1);
	
	if (get_ate_mode_state()) {
		ble_sys_mode_init(DUT_FCC_MODE);
	} else {
		ble_sys_mode_init(NORMAL_MODE);
	}

	bdaddr_env_init();

	ble_uart_init();

#if defined(CFG_PTA_SWITCH_RF_EN) && (CFG_PTA_SWITCH_RF_EN == 0)
	*(volatile unsigned int *)(0x800008) &= 0xFEFFFFFF;   ////PTA disable
#else
	*(volatile unsigned int *)(0x800008) |= (~0xFEFFFFFFU);   ////PTA disable
#endif

#if (NVDS_SUPPORT)
	nvds_init();
#endif

	rwip_init(0);

#if (CFG_SUPPORT_MANUAL_CALI)
	extern uint8_t manual_cal_get_ble_pwr_idx(uint8_t channel);
	tx_pwr_idx = manual_cal_get_ble_pwr_idx(19);
#else
	tx_pwr_idx = 16;
#endif

	bk_printf("tx_pwr_idx:%d\r\n", tx_pwr_idx);

	if (ble_get_sys_mode() == DUT_FCC_MODE) {
		enter_dut_fcc_mode();
	} else {
		enter_normal_app_mode();
	}

	ble_thread_exit_cb();
	rtos_deinit_queue(&ble_msg_que);
	ble_msg_que = NULL;
	ble_thread_handle = NULL;
	rtos_delete_thread(NULL);
}

void ble_entry(void)
{
    OSStatus ret;

    if (!ble_thread_handle && !ble_msg_que) {
    	ret = rtos_init_queue(&ble_msg_que, 
    							"ble_msg_queue",
    							sizeof(BLE_MSG_T),
    							BLE_MSG_QUEUE_COUNT);
        ASSERT(0 == ret);
        
    	ret = rtos_create_thread(&ble_thread_handle, 
    			4,
    			"ble", 
    			(beken_thread_function_t)ble_thread_main, 
    			BLE_STACK_SIZE, 
    			(beken_thread_arg_t)0);
    	
        ASSERT(0 == ret);
    }
}

UINT32 ble_ctrl( UINT32 cmd, void *param )
{
	UINT32 reg;
	UINT32 ret = ERR_SUCCESS;

	switch (cmd) {
	case CMD_BLE_REG_INIT:
		break;

	case CMD_BLE_REG_DEINIT:
		break;

	case CMD_BLE_SET_CHANNEL:
		reg = REG_READ(BLE_XVR_REG24);
		reg &= ~(BLE_XVR_CHAN_MASK << BLE_XVR_CHAN_POST);
		reg |= (*(UINT32 *)param) << BLE_XVR_CHAN_POST;
		REG_WRITE(BLE_XVR_REG24, reg);
		break;

	case CMD_BLE_START_TX:
		reg = REG_READ(BLE_XVR_REG24);
		reg &= ~(BLE_XVR_AUTO_CHAN_MASK << BLE_XVR_AUTO_CHAN_POST);
		REG_WRITE(BLE_XVR_REG24, reg);
		if ((*(UINT8 *)param) == 0x00) {
			reg = 0x3800;
		} else if ((*(UINT8 *)param) == 0x01) {
			reg = 0x3100;
		} else {
			bk_printf("unknow ble test mode\r\n");
		}
		REG_WRITE(BLE_XVR_REG25, reg);
		break;

	case CMD_BLE_STOP_TX:
		reg = REG_READ(BLE_XVR_REG24);
		reg |= (BLE_XVR_AUTO_CHAN_MASK << BLE_XVR_AUTO_CHAN_POST);
		REG_WRITE(BLE_XVR_REG24, reg);
		reg = 0;
		REG_WRITE(BLE_XVR_REG25, reg);
		break;

	default:
		ret = ERR_CMD_NOT_SUPPORT;
		break;
	}

	return ret;
}

UINT8 ble_is_start(void)
{
    return (ble_thread_handle == NULL)? 0:1;
}

extern void ble_switch_rf_to_wifi(void);
void ble_thread_exit_cb(void)
{
	
	if(ble_ps_enabled())
	{
		GLOBAL_INT_DIS();
	#if CFG_USE_BLE_PS
		sctrl_rf_wakeup();
		ble_switch_rf_to_wifi();
	#endif
		ble_session_ctx.ble_deep_sleep = 0;
		ble_session_ctx.ble_first_sleep = 1;
		ble_session_ctx.ble_sleep_enable = 0;
		ble_session_ctx.ble_init_over = 0;
		GLOBAL_INT_RES();
	}
	set_ble_active_status(0);
	
	ble_switch_rf_to_wifi();
}

void ble_dut_start(void)
{
	ble_send_msg(BLE_DUT_START);
}

UINT32 ble_in_dut_mode(void)
{
	return (ble_dut_status == DUT_IDLE) ? 0 : 1;
}

const struct rwip_eif_api* rwip_eif_get(uint8_t idx)
{
    return &uart_api;
}

void ble_update_tx_pwr(void)
{
#if (CFG_SUPPORT_MANUAL_CALI)
	extern uint8_t manual_cal_get_ble_pwr_idx(uint8_t channel);
	tx_pwr_idx = manual_cal_get_ble_pwr_idx(19);
#else
	tx_pwr_idx = 16;
#endif
}

