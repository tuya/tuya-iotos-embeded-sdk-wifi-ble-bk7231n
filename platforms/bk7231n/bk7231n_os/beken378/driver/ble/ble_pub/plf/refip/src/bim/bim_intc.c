/**
 ****************************************************************************************
 *
 * @file bim_intc.c
 *
 * @brief Definition of the Interrupt Controller (INTCTRL) API.
 *
 * Copyright (C) RivieraWaves 2009-2015
 *
 *
 ****************************************************************************************
 */

#include "bim_intc.h"


typedef void (*FUNCPTR_T)(void);


void Undefined_Exception(void)
{
	while(1)
	{
		//UART_PRINTF("%s \r\n",__func__);
		//uart_putchar("Undefined_Exception\r\n");
	}

}
void SoftwareInterrupt_Exception(void)
{
	while(1)
	{
		//UART_PRINTF("%s \r\n",__func__);
		//uart_putchar("SoftwareInterrupt_Exception\r\n");
	}

}
void PrefetchAbort_Exception(void)
{
	while(1)
	{
		//UART_PRINTF("%s \r\n",__func__);
		//uart_putchar("PrefetchAbort_Exception\r\n");
	}

}
void DataAbort_Exception(void)
{
	while(1)
	{
		//UART_PRINTF("%s \r\n",__func__);
		//uart_putchar("DataAbort_Exception\r\n");
	}

} 
	
void Reserved_Exception(void)
{
	while(1)
	{
		//UART_PRINTF("%s \r\n",__func__);
		//uart_putchar("Reserved_Exception\r\n");
	}

}

