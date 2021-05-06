/*============================================================================
*                                                                            *
* Copyright (C) by Tuya Inc                                                  *
* All rights reserved                                                        *
*                                                                            *
=============================================================================*/


/*============================ INCLUDES ======================================*/
#include "tuya_adc.h"
#include "tuya_os_adapt_output.h"

#include "typedef.h"
#include "saradc_pub.h"
#include "drv_model_pub.h"
#include "FreeRTOS.h"
#include "task.h"

/*============================ MACROS ========================================*/
#define ADC_DEV_NUM         4
#define ADC_BUF_SIZE        1

/*============================ TYPES =========================================*/



/*============================ PROTOTYPES ====================================*/
static int adc_dev_init    (tuya_adc_t *adc, tuya_adc_cfg_t *cfg);
static int adc_dev_convert    (tuya_adc_t *adc, uint16_t *result);
static int adc_dev_control(tuya_adc_t *adc, uint8_t cmd, VOID *arg);
static int adc_dev_deinit(tuya_adc_t *adc);

/*============================ LOCAL VARIABLES ===============================*/
static unsigned char g_adc_init[ADC_DEV_NUM] = {FALSE};
static unsigned short adc_buf[ADC_BUF_SIZE];

static tuya_adc_t s_adc_dev[ADC_DEV_NUM];
static saradc_desc_t adc_desc[ADC_DEV_NUM];  //ADC结构体


static const tuya_adc_ops_t  adc_dev_ops = {
    .init       = adc_dev_init,
    .convert    = adc_dev_convert,
    .control    = adc_dev_control,
    .deinit     = adc_dev_deinit,
};
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/
int platform_adc_init(void)
{
    int  i = 0;

    for (i = 0; i < ADC_DEV_NUM; i++) {
        s_adc_dev[i].ops = (tuya_adc_ops_t *)&adc_dev_ops;
        tuya_driver_register(&s_adc_dev[i].node, TUYA_DRV_ADC, i);
    }
    
    return OPRT_OS_ADAPTER_OK;
}

static int adc_dev_init    (tuya_adc_t *adc, tuya_adc_cfg_t *cfg)
{
    unsigned char adc_channel = cfg->pin;
    
    if(adc_channel >= ADC_DEV_NUM) {
        return OPRT_OS_ADAPTER_INVALID_PARM;
    }
    adc_desc[adc_channel].channel = cfg->pin + 1;  //ADC1 -6
    adc_desc[adc_channel].current_read_data_cnt = 0;
    adc_desc[adc_channel].current_sample_data_cnt = 0;
    adc_desc[adc_channel].pData = &adc_buf[0];
    adc_desc[adc_channel].data_buff_size = ADC_BUF_SIZE;
    adc_desc[adc_channel].mode = 3;//adc->mode;  //后续可以更改！在cfg中进行配置
    adc_desc[adc_channel].filter = 0;
    adc_desc[adc_channel].has_data = 0;
    adc_desc[adc_channel].all_done = 0;
    adc_desc[adc_channel].pre_div = 8;
    adc_desc[adc_channel].samp_rate = 2; //后续可以更改！在cfg中进行配置
    adc_desc[adc_channel].p_Int_Handler = saradc_disable;

    g_adc_init[adc_channel] = TRUE;

    return OPRT_OS_ADAPTER_OK;
}

/**
 * @brief: ADC 适配转换，ADC检测范围0~2.4V 0~4096
 * @param[in]: 
 * @return: OPRT_OK
 */
static int adc_dev_convert    (tuya_adc_t *adc, uint16_t *result)
{
    unsigned char i = 0;
    unsigned int status;
    int adc_hdl;
    static unsigned short last_adc = 0;

    unsigned char adc_channel = adc->cfg.pin;

    if(!g_adc_init[adc_channel]){
        LOG_ERR("adc not init!\r\n");
        return OPRT_OS_ADAPTER_COM_ERROR;
    }
    
    GLOBAL_INT_DECLARATION();

    *(unsigned short*)result = 0xFFFF;
    
    memset(adc_desc[adc_channel].pData, 0, adc_desc[adc_channel].data_buff_size * SIZEOF(unsigned short));

    GLOBAL_INT_DISABLE();

    adc_hdl = ddev_open(SARADC_DEV_NAME, &status, (unsigned int)&adc_desc[adc_channel]); 
    if ((DD_HANDLE_UNVALID == adc_hdl) || (SARADC_SUCCESS != status))
    {
        if (SARADC_SUCCESS != status)
        {
            ddev_close(adc_hdl);
        }
        adc_hdl = DD_HANDLE_UNVALID;
        GLOBAL_INT_RESTORE();
        LOG_ERR("tuya_hal_adc_value_get ddev_open error:%d\r\n", status);
        return -1;
    }
    GLOBAL_INT_RESTORE();

    while (adc_desc[adc_channel].all_done == 0)
    {
        i ++;
        if (i > 100)
        {
            LOG_ERR("tuya_hal_adc_value_get: timeout!\r\n");
            break;
        }
        vTaskDelay(1);
    }
    
    if(adc_desc[adc_channel].current_sample_data_cnt >= 1) //此处简化直接就是采集1次，1这个数值后续可以优化
    {
        *(USHORT_T*)result = adc_desc[adc_channel].pData[0];
        adc_desc[adc_channel].has_data = 0;
        adc_desc[adc_channel].current_sample_data_cnt = 0;
        last_adc = *(USHORT_T*)result;
    } else {
        *(USHORT_T*)result = last_adc;
    }
    
    ddev_close(adc_hdl);

    saradc_ensure_close();

    return OPRT_OS_ADAPTER_OK;
}


static int adc_dev_control(tuya_adc_t *adc, uint8_t cmd, VOID *arg)
{
    unsigned short adc_buf = 0;
    int ret;
    
    switch(cmd) {
        case TUYA_ADC_VOLTAGE_CMD:

            ret = adc_dev_convert(adc, &adc_buf);
            if(ret != OPRT_OS_ADAPTER_OK) {
                return OPRT_OS_ADAPTER_COM_ERROR;
            }
            *(unsigned int *)arg = (adc_buf / 4096.0 * 2400); // 单位mv
            
            break;
       default:
            break;
    }
    return OPRT_OS_ADAPTER_OK;
}

static int adc_dev_deinit(tuya_adc_t  *asc)
{
    return OPRT_OS_ADAPTER_OK;
}

