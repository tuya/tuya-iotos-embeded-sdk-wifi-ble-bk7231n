#include "include.h"
#include "cmd_rx_sensitivity.h"
#include "mem_pub.h"
#include "str_pub.h"
#include "rx_sensitivity_pub.h"
#include "fake_clock_pub.h"
#include "uart_debug_pub.h"
#include "schedule_pub.h"
#include "rtos_pub.h"
#include "rtos_error.h"
#include "sys_ctrl_pub.h"
#include "drv_model_pub.h"
#include "bk7011_cal_pub.h"
#include "reg_mdm_cfg.h"
#include "bk7231N_cal.h"
#include "arm_arch.h"

#if CFG_RX_SENSITIVITY_TEST
beken_timer_t rx_sens_tmr = {0};
beken_timer_t rx_sens_ble_tmr = {0};
#endif

UINT32 g_rxsens_start = 0;

extern void bk7011_max_rxsens_setting(void);
extern void bk7011_normal_rxsens_setting(void);

int bk7011_reduce_vdddig_for_rx(int reduce)
{
    static UINT32 default_vdddig = 0;
    UINT32 new_vdddig;
    if (!default_vdddig)
    {
        default_vdddig = sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_GET_VDD_VALUE, NULL);
        if (!default_vdddig)
        {
            return -1;
        }
    }
    if (reduce)
    {
        new_vdddig = default_vdddig - 1;
        sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_SET_VDD_VALUE, (void *)&new_vdddig);
    }
    else
    {
        /* recover vdddig */
        sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_SET_VDD_VALUE, (void *)&default_vdddig);
    }

    return 0;
}

void rxsens_ct_hdl(void *param)
{
#if CFG_RX_SENSITIVITY_TEST
    OSStatus err;
    rx_get_rx_result_end();
    rx_get_rx_result_begin();
    
    if(rx_sens_tmr.handle != NULL) {
        err = rtos_reload_timer(&rx_sens_tmr);
        ASSERT(kNoErr == err);
    }   
#endif // CFG_RX_SENSITIVITY_TEST
}

void rxsens_ble_ct_hdl(void *param)
{
#if CFG_RX_SENSITIVITY_TEST
    OSStatus err;
    rx_get_ble_rx_result();
    if(rx_sens_ble_tmr.handle != NULL) {
        err = rtos_reload_timer(&rx_sens_ble_tmr);
        ASSERT(kNoErr == err);
    }
#endif
}

void rxsens_ct_show_hdl(void *param)
{
#if CFG_RX_SENSITIVITY_TEST
    rx_get_rx_result_end();
#endif // CFG_RX_SENSITIVITY_TEST
}

int do_rx_sensitivity(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
    char cmd0 = 0;
    char cmd1 = 0;
    UINT8 fail = 0;

    UINT32 mode = RXSENS_DEFUALT_MODE;
    UINT32 duration = RXSENS_DEFUALT_DURATION;
    UINT32 channel = RXSENS_DEFUALT_CHANNEL;
    UINT32 ble_channel = RXSENS_DEFUALT_BLE_CHANNEL;
    UINT32 is_ble_test = 0;;
    UINT32 ble_test = 0;

#if CFG_RX_SENSITIVITY_TEST
    OSStatus err;
    UINT8 ret;
 ///   UINT32 reg;
#endif

    UINT32 arg_id = 1;
    UINT32 arg_cnt = argc;

#if CFG_RX_SENSITIVITY_TEST
    uint32_t t_ms = 0;
#endif

    /*step0, parameter conversion*/
    while(arg_cnt > 1)
    {
        if(arg_cnt > 1)
        {
            cmd0 = argv[arg_id][0];
            cmd1 = argv[arg_id][1];
        }

        switch(cmd0)
        {
        case '-':
        {
            arg_cnt -= 1;

            if(arg_cnt < 1)
            {
                fail = 1;
                break;
            }

            arg_cnt -= 1;
            switch(cmd1)
            {
            case 'b':  // bandwidth 0:20M, 1:40M
                mode = os_strtoul(argv[arg_id + 1], NULL, 10);
                break;

            case 'd':  // timer intval
                duration = os_strtoul(argv[arg_id + 1], NULL, 10);
                break;

            case 'c':  // channel
                if(is_ble_test)
                {
                    ble_channel = os_strtoul(argv[arg_id + 1], NULL, 10);
                }
                else
                {
                    channel = os_strtoul(argv[arg_id + 1], NULL, 10);
                }
                break;

            case 'e': { //  0: exit, 1: enter, 2: stop last rx
#if CFG_RX_SENSITIVITY_TEST
                UINT32 op = os_strtoul(argv[arg_id + 1], NULL, 10);
                if(op < RXSENS_G_MAX) {
                    if (op == RXSENS_G_STOP_LASTRX) {
#if (CFG_SOC_NAME == SOC_BK7231N)
                        /* recover MDM_REG202<23>=0 */
                        mdm_cpemode_setf(0);
                        /* recover MDM_REG206<17:16>=0 */
                        mdm_cfgsmooth_setf(0);
                        /* recover TRX_REG12<8:7>=3 */
                        rwnx_cal_set_reg_adda_ldo(3);
                        /* recover rx filter */
                        rwnx_cal_en_rx_filter_offset();
                        bk7011_reduce_vdddig_for_rx(0);
                        /* recover TRX_REG9<22>=0 for band20 */
                        rwnx_cal_set_bw_i2v(0);
#endif
                        g_rxsens_start = 0;
                        if(rx_sens_tmr.handle != NULL) {
                            err = rtos_deinit_timer(&rx_sens_tmr); 
                            ASSERT(kNoErr == err);
                            rx_sens_tmr.handle = NULL; 
                        }
    	            } else {
#if (CFG_SOC_NAME == SOC_BK7231N)
                        /* set TRX_REG12<8:7>=2 for rx */
                        rwnx_cal_set_reg_adda_ldo(2);
                        rwnx_cal_dis_rx_filter_offset();
#endif
                    }
                    return 0;
                } else {
                    return -1;
                }
#endif                
            }

            case 's': {  // start / stop
#if CFG_RX_SENSITIVITY_TEST
                if(!is_ble_test)
                {
                    UINT32 sta = os_strtoul(argv[arg_id + 1], NULL, 10);
                    if(sta) { 
                        rx_clean_rx_statistic_result();
                    }
                    else {
                        rx_get_rx_result_end();
                    }
                }
                else
                {
                    UINT32 sta = os_strtoul(argv[arg_id + 1], NULL, 10);
                    if(sta)
                    {
                        rx_start_ble_rx_counting();
                    }
                    else
                    {
                        rx_get_ble_rx_result();
                    }
                }
#endif
                return 0;
            }

            case 'g': {  // get statistic  0: clean, 1:20M, 2:40M
 #if CFG_RX_SENSITIVITY_TEST
                if(!is_ble_test)
                {
                    UINT32 g_type = os_strtoul(argv[arg_id + 1], NULL, 10);
                    if(g_type < RXSENS_RTYPTE_MAX ) 
                    {
                        if(g_type == RXSENS_RTYPTE_CLEAN) { 
                            rx_clean_rx_statistic_result();
                        }
                        else if(g_type == RXSENS_RTYPTE_20M) { 
                            rx_get_rx20M_statistic_result();
                        }
                        
                        else if(g_type == RXSENS_RTYPTE_40M){
                            rx_get_rx40M_statistic_result();
                        }
                        else if(g_type == RXSENS_RTYPTE_SIG_RES){
                            rxsens_ct_hdl(NULL);
                        }
                        return 0;
                    } else 
                        return -1;
                }
                else
                {
                    UINT32 g_type = os_strtoul(argv[arg_id + 1], NULL, 10);
                    if(g_type == RXSENS_RTYPTE_CLEAN)
                    {
                        rx_clean_ble_rx_result();
                        return 0;
                    }
                    else
                    {
                        return -1;
                    }
                }
#endif
            }

            case 'o': {
                is_ble_test = 1;
                ble_test = os_strtoul(argv[arg_id + 1], NULL, 10); // 1:start 0:stop                
            }
            break;

            default:
                fail = 1;
                break;
            }
        }
        break;

        default:
            fail = 1;
            break;
        }

        if(fail)
        {
            return 1;
        }

        arg_id += 2;
    }

    /*step1, parameter check*/
    if(!((0 < channel)
            || (14 > channel)))
    {
        return 1;
    }

    if(!((0 < ble_channel)
            || (40 > ble_channel)))
    {
        return 1;
    }

    if((mode != 1) && (mode != 0) )
    {
        return 1;
    }

    /*step2, handle*/
#if CFG_RX_SENSITIVITY_TEST

    if(!is_ble_test)
    {
        ret = rs_set_mode(mode);
        if(ret)
        {
            return 1;
        }

        ret = rs_set_channel(channel);
        if(ret)
        {
            return 1;
        } 

        //sys_ctrl_0x42[6:4]=SCTRL_DIGTAL_VDD=4
#if (CFG_SOC_NAME == SOC_BK7231N)
        if(mode == 0)
        {
            /* set MDM_REG206<17:16>=3 for band20 */
            mdm_cfgsmooth_setf(3);
            bk7011_reduce_vdddig_for_rx(1);
            /* recover TRX_REG9<22>=0 for band20 */
            rwnx_cal_set_bw_i2v(0);
        }
        else
        {
            /* set MDM_REG206<17:16>=1 for band40 */
            mdm_cfgsmooth_setf(1);
            bk7011_reduce_vdddig_for_rx(0);
            /* set TRX_REG9<22>=1 for band40 */
            rwnx_cal_set_bw_i2v(1);
        }
#else
        if(mode == 0)
        {
            reg = 3;
            sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_SET_VDD_VALUE, &reg);
        }
        else
        {
            reg = 5;
            sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_SET_VDD_VALUE, &reg);
        }
#endif
        
        rs_test();

#if (CFG_SOC_NAME == SOC_BK7231N)
        extern void phy_enable_lsig_intr(void);
        phy_enable_lsig_intr();
        /* set MDM_REG202<23>=1 for rx */
        mdm_cpemode_setf(1);
#else
        if((channel == 13) ||(channel == 14))
            rwnx_cal_set_reg_adda_ldo(0);
        else
            rwnx_cal_set_reg_adda_ldo(3);
        
        rwnx_cal_set_reg_rx_ldo();
        bk7011_max_rxsens_setting();
        rwnx_cal_dis_extra_txpa();
        
        if(mode == 1)
        {
            if((channel >= 3) && (channel <= 10))
                rwnx_cal_set_40M_extra_setting(1);
            else
                rwnx_cal_set_40M_extra_setting(0);
        }
        else
            rwnx_cal_set_40M_extra_setting(0);
#endif
        
        g_rxsens_start = 1;

        if(duration) {
            rx_get_rx_result_begin();
            //t_ms = fclk_from_sec_to_tick(duration);
            t_ms = duration * 1000;
            
            if(rx_sens_tmr.handle != NULL) {
                err = rtos_deinit_timer(&rx_sens_tmr); 
                ASSERT(kNoErr == err);
                rx_sens_tmr.handle = NULL; 
            } 
            
        	err = rtos_init_timer(&rx_sens_tmr, 
    					t_ms, 
    					rxsens_ct_hdl, 
    					(void *)0);
            ASSERT(kNoErr == err);
        	err = rtos_start_timer(&rx_sens_tmr);
        	ASSERT(kNoErr == err);   
        } else {
            if(rx_sens_tmr.handle != NULL) {
                err = rtos_deinit_timer(&rx_sens_tmr); 
                ASSERT(kNoErr == err);
                rx_sens_tmr.handle = NULL; 
            } 
        }
    }
    else
    {
        if(ble_test)
        {
            os_printf("ble_test\r\n");
            //rwnx_cal_set_txpwr(ble_pwr_mod, 11);
            rs_ble_test_start(ble_channel);
            if(duration) {
                //t_ms = fclk_from_sec_to_tick(duration);
                t_ms = duration * 1000;
                
                if(rx_sens_ble_tmr.handle != NULL) {
                    err = rtos_deinit_timer(&rx_sens_ble_tmr); 
                    ASSERT(kNoErr == err);
                    rx_sens_ble_tmr.handle = NULL; 
                } 
                
            	err = rtos_init_timer(&rx_sens_ble_tmr, 
        					t_ms, 
        					rxsens_ble_ct_hdl, 
        					(void *)0);
                ASSERT(kNoErr == err);
            	err = rtos_start_timer(&rx_sens_ble_tmr);
            	ASSERT(kNoErr == err);   
            } else {
                if(rx_sens_ble_tmr.handle != NULL) {
                    err = rtos_deinit_timer(&rx_sens_ble_tmr); 
                    ASSERT(kNoErr == err);
                    rx_sens_ble_tmr.handle = NULL; 
                } 
            }
        }
        else
        {
            if(rx_sens_ble_tmr.handle != NULL) {
                err = rtos_deinit_timer(&rx_sens_ble_tmr); 
                ASSERT(kNoErr == err);
                rx_sens_ble_tmr.handle = NULL; 
            } 
            rs_ble_test_stop();
        }
    }
    
    
#endif // CFG_RX_SENSITIVITY_TEST

    return 0;
}

// eof

