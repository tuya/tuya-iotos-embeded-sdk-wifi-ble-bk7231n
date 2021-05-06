#include "include.h"
#include "arm_arch.h"

#if CFG_USE_SDCARD_HOST

#include "sdio_driver.h"

#include "drv_model_pub.h"
#include "sys_ctrl_pub.h"
#include "mem_pub.h"
#include "icu_pub.h"
#include "gpio_pub.h"

/******************************************************************************/
/**************************** platform function *******************************/
/******************************************************************************/

static void
beken_sdcard_set_clk_div(UINT8 clkdiv)
{
    UINT32 reg;

    reg = REG_READ(REG_SDCARD_FIFO_THRESHOLD);
    reg &= ~(SDCARD_FIFO_SD_RATE_SELECT_MASK << SDCARD_FIFO_SD_RATE_SELECT_POSI);
    reg |= ((clkdiv & SDCARD_FIFO_SD_RATE_SELECT_MASK)
            << SDCARD_FIFO_SD_RATE_SELECT_POSI);
    REG_WRITE(REG_SDCARD_FIFO_THRESHOLD, reg);
}


/******************************************************************************/
/**************************** interface function ******************************/
/******************************************************************************/
void sdio_set_low_clk(void)
{
    beken_sdcard_set_clk_div(CLK_200K);
}

void sdio_set_high_clk(void)
{
    beken_sdcard_set_clk_div(CLK_6_5M);
}

void sdio_gpio_config(void)
{
    UINT32 param;
 #if (CFG_SOC_NAME == SOC_BK7221U)
    param = GPIO_CFG_PARAM(SD_DETECT_DEFAULT_GPIO, GMODE_INPUT_PULLUP);
    sddev_control(GPIO_DEV_NAME, CMD_GPIO_CFG, &param);

    param = GFUNC_MODE_SD1_HOST;
 #else
 	param = GFUNC_MODE_SD_HOST;
 #endif
    sddev_control(GPIO_DEV_NAME, CMD_GPIO_ENABLE_SECOND, &param);
}

void sdio_clk_config(UINT8 enable)
{
    UINT32 param;
    UINT32 cmd;

    if(enable)
        cmd = CMD_CLK_PWR_UP;
    else
        cmd = CMD_CLK_PWR_DOWN;

    param = PWD_SDIO_CLK_BIT;
    sddev_control(ICU_DEV_NAME, cmd, &param);
}

void sdio_register_reset(void)
{
    UINT32 reg;

    /* Clear cmd rsp int bit */
    reg = REG_READ(REG_SDCARD_CMD_RSP_INT_SEL);
    REG_WRITE(REG_SDCARD_CMD_RSP_INT_SEL, reg);

    /* Clear tx/rx fifo */
    reg = SDCARD_FIFO_RX_FIFO_RST | SDCARD_FIFO_TX_FIFO_RST;
    REG_WRITE(REG_SDCARD_FIFO_THRESHOLD, reg);

    /* Disabe all sdio interrupt */
    reg = 0;
    REG_WRITE(REG_SDCARD_CMD_RSP_INT_MASK, reg);

    /* Config tx/rx fifo threshold */
    reg = ((SDCARD_RX_FIFO_THRD & SDCARD_FIFO_RX_FIFO_THRESHOLD_MASK)
           << SDCARD_FIFO_RX_FIFO_THRESHOLD_POSI)
          | ((SDCARD_TX_FIFO_THRD & SDCARD_FIFO_TX_FIFO_THRESHOLD_MASK)
             << SDCARD_FIFO_TX_FIFO_THRESHOLD_POSI);
    REG_WRITE(REG_SDCARD_FIFO_THRESHOLD, reg);
}

void sdio_register_reenable(void)
{
    UINT32 reg;

    reg = REG_READ(REG_SDCARD_CMD_RSP_INT_SEL);
    REG_WRITE(REG_SDCARD_CMD_RSP_INT_SEL, reg);

    reg = REG_READ(REG_SDCARD_FIFO_THRESHOLD);
    reg |= (SDCARD_FIFO_RX_FIFO_RST | SDCARD_FIFO_TX_FIFO_RST | SDCARD_FIFO_SD_STA_RST);
    REG_WRITE(REG_SDCARD_FIFO_THRESHOLD, reg);

    vTaskDelay(5);
    /*Config tx/rx fifo threshold*/
    reg = ((SDCARD_RX_FIFO_THRD & SDCARD_FIFO_RX_FIFO_THRESHOLD_MASK)
           << SDCARD_FIFO_RX_FIFO_THRESHOLD_POSI)
          | ((SDCARD_TX_FIFO_THRD & SDCARD_FIFO_TX_FIFO_THRESHOLD_MASK)
             << SDCARD_FIFO_TX_FIFO_THRESHOLD_POSI);
    REG_WRITE(REG_SDCARD_FIFO_THRESHOLD, reg);
}

void sdio_sendcmd_function( UINT8 cmd_index, UINT32 flag,
                            UINT32 timeout, VOID *arg )
{
    UINT32 reg;
    flag &= CMD_FLAG_MASK;

    reg = (UINT32)arg;
    REG_WRITE(REG_SDCARD_CMD_SEND_AGUMENT, reg);

    reg = timeout;
    REG_WRITE(REG_SDCARD_CMD_RSP_TIMER, reg);

    reg = ((((UINT32)cmd_index)&SDCARD_CMD_SEND_CTRL_CMD_INDEX_MASK)
           << SDCARD_CMD_SEND_CTRL_CMD_INDEX_POSI)
          | ((flag & SDCARD_CMD_SEND_CTRL_CMD_FLAGS_MASK)
             << SDCARD_CMD_SEND_CTRL_CMD_FLAGS_POSI)
          | SDCARD_CMD_SEND_CTRL_CMD_START;
    REG_WRITE(REG_SDCARD_CMD_SEND_CTRL, reg);
}

SDIO_Error sdio_wait_cmd_response(UINT32 cmd)
{
    UINT32 reg;
    uint32 timeoutcnt = 0;
    uint32 timeout2cnt = 0;

    while(1)
    {
        reg = REG_READ(REG_SDCARD_CMD_RSP_INT_SEL);
        //wait until cmd response
        if(reg & (SDCARD_CMDRSP_NORSP_END_INT
                  | SDCARD_CMDRSP_RSP_END_INT
                  | SDCARD_CMDRSP_TIMEOUT_INT)
          )
        {
            break;
        }

#if 1
        if(!SD_det_gpio_flag)
        {
            if(timeoutcnt++ >= SD_CLK_PIN_TIMEOUT1)//this value needs to be adjusted
            {
                if(SD_CARD_OFFLINE == sd_clk_is_attached())//detect sdcard is valid or not
                {
                    REG_WRITE(REG_SDCARD_CMD_RSP_INT_SEL, reg);//clear the int flag
                    return SD_CMD_RSP_TIMEOUT;
                }
                else
                {
                    if(timeout2cnt++ > 0x30000)
                    {
                        os_printf("===cmd timeout==:%x\r\n", reg);
                        return SD_CMD_RSP_TIMEOUT;
                    }
                }
            }
        }
        else
        {
            if(SD_CARD_OFFLINE == sd_is_attached())//detect sdcard is valid or not
            {
                REG_WRITE(REG_SDCARD_CMD_RSP_INT_SEL, reg);//clear the int flag
                return SD_CMD_RSP_TIMEOUT;
            }
        }
#endif
    }

    REG_WRITE(REG_SDCARD_CMD_RSP_INT_SEL, SD_CMD_RSP);//clear the int flag
    if((reg & SDCARD_CMDRSP_TIMEOUT_INT) /*||(reg&SDCARD_CMDRSP_NORSP_END_INT)*/ )
    {
        if((cmd != 1))
        {
            SDCARD_WARN("sdcard cmd %x timeout,cmdresp_int_reg:0x%x\r\n", cmd , reg);
        }
        return SD_CMD_RSP_TIMEOUT;
    }
    if(reg & SDCARD_CMDRSP_CMD_CRC_FAIL)
    {

        if((cmd != 41) && (cmd != 2) && (cmd != 9) && (cmd != 1))
        {
            SDCARD_WARN("sdcard cmd %x crcfail,cmdresp_int_reg:0x%x\r\n", cmd , reg);
            return SD_CMD_CRC_FAIL;
        }
    }
    return SD_OK;
}

void sdio_get_cmdresponse_argument(UINT8 num, UINT32 *resp)
{
    switch(num)
    {
    case 0:
        *resp = REG_READ(REG_SDCARD_CMD_RSP_AGUMENT0);
        break;
    case 1:
        *resp = REG_READ(REG_SDCARD_CMD_RSP_AGUMENT1);
        break;
    case 2:
        *resp = REG_READ(REG_SDCARD_CMD_RSP_AGUMENT2);
        break;
    case 3:
        *resp = REG_READ(REG_SDCARD_CMD_RSP_AGUMENT3);
        break;
    default:
        break;
    }
}

void sdio_setup_data(UINT32 data_dir, UINT32 byte_len)
{
    UINT32 reg;
    if(data_dir == SD_DATA_DIR_RD)
    {
        reg = REG_READ(REG_SDCARD_FIFO_THRESHOLD);
        reg &= (3 << 21);
        reg |= 0x3ffff; // set fifo
        REG_WRITE(REG_SDCARD_FIFO_THRESHOLD, reg);

        reg = SDCARD_DATA_REC_CTRL_DATA_EN;
    }
    else
        reg = SDCARD_DATA_REC_CTRL_DATA_WR_DATA_EN;

    reg |= SDCARD_DATA_REC_CTRL_DATA_BYTE_SEL
           | ((byte_len & SDCARD_DATA_REC_CTRL_BLK_SIZE_MASK)
              << SDCARD_DATA_REC_CTRL_BLK_SIZE_POSI)
#if CONFIG_SDCARD_BUSWIDTH_4LINE
           | SDCARD_DATA_REC_CTRL_DATA_BUS
#endif
           ;

    REG_WRITE(REG_SDCARD_DATA_REC_CTRL, reg);
}

void sdio_set_data_timeout(UINT32 timeout)
{
    REG_WRITE(REG_SDCARD_DATA_REC_TIMER, timeout);
}

void driver_sdcard_recv_data_start(int timeout )
{
    REG_WRITE(REG_SDCARD_DATA_REC_TIMER, timeout);
#ifdef CONFIG_APP_SDCARD_4_LINE
    REG_WRITE(REG_SDCARD_DATA_REC_CTRL, (0x1 | (1 << 2) | (512 << 4) | (1 << 17)));
#else
    REG_WRITE(REG_SDCARD_DATA_REC_CTRL, (0x1 | (512 << 4) | (1 << 17)));
#endif
}

void sdcard_set_host_buswidth_4line(void)
{
}

void sdcard_set_host_buswidth_1line(void)
{
}

SDIO_Error sdcard_wait_receive_data(UINT32 *receive_buf)
{
    UINT32 reg, i;
    while(1)
    {
        reg = REG_READ(REG_SDCARD_CMD_RSP_INT_SEL);
        if(reg & (SDCARD_CMDRSP_DATA_REC_END_INT
                  | SDCARD_CMDRSP_DATA_CRC_FAIL
                  | SDCARD_CMDRSP_DATA_TIME_OUT_INT))
        {
            break;
        }
    }

    REG_WRITE(REG_SDCARD_CMD_RSP_INT_SEL, SD_DATA_RSP);//clear the int flag
    if(reg & SDCARD_CMDRSP_DATA_TIME_OUT_INT)
    {
        return SD_DATA_TIMEOUT;
    }
    if(reg & SDCARD_CMDRSP_DATA_CRC_FAIL)
    {
        SDCARD_WARN("sdcard data crcfail,cmdresp_int_reg:0x%x\r\n", reg);
        return SD_DATA_CRC_FAIL;
    }

    for (i = 0; i < (SD_DEFAULT_BLOCK_SIZE >> 2); i++)
    {
        /* wait fifo data valid */
        while(1)
        {
            //software  needn't handle dead-loop,hardware can garantee
            if(REG_READ(REG_SDCARD_FIFO_THRESHOLD)&SDCARD_FIFO_RXFIFO_RD_READY)
                break;
        }
        *(receive_buf + i) = REG_READ(REG_SDCARD_RD_DATA_ADDR);
    }

    return SD_OK;

}

void sdcard_write_data(UINT32 *write_buf)
{
    UINT16 i;
    UINT32 tmpval;
    for(i = 0; i<SD_DEFAULT_BLOCK_SIZE >> 2; i++)
    {
        tmpval = ( (( write_buf[i] & 0xff) << 24 ) |
                   (((write_buf[i] >> 8) & 0xff ) << 16) |
                   (((write_buf[i] >> 16) & 0xff ) << 8) |
                   ((write_buf[i] >> 24) & 0xff ) );
        REG_WRITE(REG_SDCARD_WR_DATA_ADDR, tmpval);
    }

}

SDIO_Error sdcard_wait_write_end(void)
{
    UINT32 reg;
    while(1)
    {
        reg = REG_READ(REG_SDCARD_CMD_RSP_INT_SEL);
        if(reg & (SDCARD_CMDRSP_DATA_WR_END_INT
                  | SDCARD_CMDRSP_DATA_CRC_FAIL
                  | SDCARD_CMDRSP_DATA_TIME_OUT_INT))
        {
            break;
        }
    }

    REG_WRITE(REG_SDCARD_CMD_RSP_INT_SEL, SD_DATA_RSP);//clear the int flag

    if(reg & SDCARD_CMDRSP_DATA_TIME_OUT_INT)
    {
        return SD_DATA_TIMEOUT;
    }
    if(reg & SDCARD_CMDRSP_DATA_CRC_FAIL)
    {
        SDCARD_WARN("sdcard write data crcfail,cmdresp_int_reg:0x%x\r\n", reg);
        return SD_DATA_CRC_FAIL;
    }

    return SD_OK;
}


int wait_Receive_Data(void)
{
    uint32 ret = SD_ERR_LONG_TIME_NO_RESPONS, status = 0;
    uint32 timeoutcnt = 0;
    extern UINT32 rtos_get_time(void);
    uint32 start_tm = rtos_get_time();
    while (1)
    {
        if(rtos_get_time() > start_tm + 4000) // 4s
        {
            ret = SD_ERR_LONG_TIME_NO_RESPONS;
            break;
        }

        status = REG_READ(REG_SDCARD_CMD_RSP_INT_SEL);
        if (status & SDCARD_CMDRSP_DATA_REC_END_INT)
        {
            if (status & SDCARD_CMDRSP_DATA_CRC_FAIL)
            {
                os_printf("aaa\r\n");
                //ret = SD_DATA_CRC_FAIL;
                ret = SD_OK;
            }
            else
            {
                ret = SD_OK;
            }
            break;
        }
        else if (status & SDCARD_CMDRSP_DATA_CRC_FAIL)
        {
            ret = SD_DATA_CRC_FAIL;
            break;
        }
        else if (status & SDCARD_CMDRSP_DATA_TIME_OUT_INT)
        {
            ret = SD_DATA_TIMEOUT;
            break;
        }
        if(!SD_det_gpio_flag)
        {
            if(timeoutcnt++ >= SD_CLK_PIN_TIMEOUT2)
            {
                if (SD_CARD_OFFLINE == sd_clk_is_attached())
                {
                    ret = SD_DATA_TIMEOUT;
                    break;
                }
            }
        }
        else
        {
            if(SD_CARD_OFFLINE == sd_is_attached())
            {
                //detect sdcard is valid or not
                ret = SD_DATA_TIMEOUT;
                break;
            }
        }
    }

    if((!SD_det_gpio_flag) && (SD_ERR_LONG_TIME_NO_RESPONS == ret))
        sd_clk_is_attached();

    REG_WRITE(REG_SDCARD_CMD_RSP_INT_SEL, SD_DATA_RSP);/*< clear the int flag */
    return ret;
}

#endif // CFG_USE_SDCARD_HOST
// EOF

