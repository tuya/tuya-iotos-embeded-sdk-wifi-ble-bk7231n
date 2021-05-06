

#include <stdint.h>
#include "bim_com.h"
#include "bim_updataImage.h"
#include "bim_application.h"
#include "BK3435_reg.h"
#include "bim_uart.h"

#define Uart_Write_Byte(v)               (REG_APB3_UART_PORT=v)

typedef void (*FUNCPTR)(void);

void delay(uint32_t t)
{
	for(int i  = 1000;i> 0;i--)
	{
		for(int j = 0;j< t;j++)
		{
			;
		}
	}
}

void bim_main(void)	
{
	int32_t main_point = SEC_IMAGE_RUN_CADDR;

	udi_init_bim_env(ICU_CLK_16M);

	flash_wr_protect_none();	
	
	if(1 == udi_select_sec())
	{
		 udi_init_bim_env(ICU_CLK_16M);
		(*(FUNCPTR)main_point)();
	}
	else
	{
	}
}
// eof

