#pragma once

#include "../../../common/typedef.h"
#include "../../../driver/include/drv_model_pub.h"
#include "../../../driver/include/icu_pub.h"
#include "../../../driver/include/gpio_pub.h"
#include "../../../driver/include/intc_pub.h"
#include "../../../driver/entry/arch.h"
#include "../../../os/FreeRTOSv9.0.0/FreeRTOS/Source/portable/Keil/ARM968es/portmacro.h"
#include "../../../../../apps/OpenBK7231N_App/src/logging/logging.h"

#define clk_set_spi_clk_26m(id) \
    UINT32 param = PCLK_POSI_SPI; \
    sddev_control(ICU_DEV_NAME, CMD_CONF_PCLK_26M, &param);
    
#define clk_set_spi_clk_dco(id) \
    UINT32 param = PCLK_POSI_SPI; \
    sddev_control(ICU_DEV_NAME, CMD_CONF_PCLK_DCO, &param);

#define SPI_SET_PIN_(i) {\
	    uint32_t val = GFUNC_MODE_SPI; \
	    sddev_control(GPIO_DEV_NAME, CMD_GPIO_ENABLE_SECOND, &val); \
    }

#define power_up_spi(id) {\
		UINT32 param = PWD_SPI_CLK_BIT; \
		sddev_control(ICU_DEV_NAME, CMD_CLK_PWR_UP, &param); \
    }

#define power_down_spi(id) {\
    UINT32 param = PWD_SPI_CLK_BIT; \
    sddev_control(ICU_DEV_NAME, CMD_CLK_PWR_DOWN, &param); \
    }

#define icu_enable_spi_interrupt(id) {\
		UINT32 param = (IRQ_SPI_BIT); \
		sddev_control(ICU_DEV_NAME, CMD_ICU_INT_ENABLE, &param); \
    }

#define icu_disable_spi_interrupt(id) {\
		UINT32 param = (IRQ_SPI_BIT); \
		sddev_control(ICU_DEV_NAME, CMD_ICU_INT_DISABLE, &param); \
    }

// FIXME: bk_int_isr_register registers always and only spi
#define bk_int_isr_register(int_src, isr, null) \
	intc_service_register(IRQ_SPI, PRI_IRQ_SPI, isr);

#define bk_int_isr_unregister(int_src)

#if 0
#define BK_LOGE(tag, fmt, ...) ADDLOG_ERROR(14, fmt, ##__VA_ARGS__)
#define SPI_LOGE(fmt, ...) ADDLOG_ERROR(14, fmt, ##__VA_ARGS__)
#define SPI_LOGW(fmt, ...) ADDLOG_WARN(14, fmt, ##__VA_ARGS__)
#define SPI_LOGD(fmt, ...) ADDLOG_DEBUG(14, fmt, ##__VA_ARGS__)
#else
#define HAL_LOGW(tag, fmt, ...)
#define HAL_LOGI(fmt, ...)
#define BK_LOGE(tag, fmt, ...)
#define SPI_LOGE(fmt, ...)
#define SPI_LOGW(fmt, ...)
#define SPI_LOGD(fmt, ...)
#endif

#define rtos_disable_int() \
    { \
    GLOBAL_INT_DECLARATION(); \
    GLOBAL_INT_DISABLE();
    
#define rtos_enable_int(lvl) \
    GLOBAL_INT_RESTORE(); \
    }
