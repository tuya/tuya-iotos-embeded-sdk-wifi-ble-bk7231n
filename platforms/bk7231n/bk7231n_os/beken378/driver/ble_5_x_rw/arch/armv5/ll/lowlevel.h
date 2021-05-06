/**
 ****************************************************************************************
 *
 * @file ll.h
 *
 * @brief Declaration of low level functions.
 *
 * Copyright (C) RivieraWaves 2009-2015
 *
 *
 ****************************************************************************************
 */

#ifndef LL_H_
#define LL_H_


#include "arch.h"
#include <stdint.h>

#include "compiler.h"

#define KEIL_INTRINSICS_INTERRUPT


/** @brief Enable interrupts globally in the system.
 * This macro must be used when the initialization phase is over and the interrupts
 * can start being handled by the system.
 */
 
/* keil Compiler intrinsics for controlling IRQ and FIQ interrupts
*/

//extern void __enable_irq(void);
//extern void __enable_fiq(void);
//extern int  __disable_irq(void);
//extern int  __disable_fiq(void);

#define __enable_irq	portENABLE_IRQ
#define __enable_fiq	portENABLE_FIQ
#define __disable_irq	portDISABLE_IRQ
#define __disable_fiq	portDISABLE_FIQ

#define GLOBAL_INT_DIS(); \
do { \
    uint32_t  fiq_tmp; \
	  uint32_t  irq_tmp; \
		fiq_tmp = __disable_fiq(); \
		irq_tmp = __disable_irq();

#define GLOBAL_INT_RES(); \
			if(!fiq_tmp)           \
			{                      \
					__enable_fiq();    \
			}                      \
			if(!irq_tmp)           \
			{                      \
					__enable_irq();    \
			}                      \
} while(0) ;                                       

#define GLOBAL_INT_DECLARATION_N() uint32_t fiq_tmp, irq_tmp
#define GLOBAL_INT_DISABLE_N()     do{\
										fiq_tmp = portDISABLE_FIQ();\
										irq_tmp = portDISABLE_IRQ();\
									}while(0)


#define GLOBAL_INT_RESTORE_N()     do{                         \
                                        if(!fiq_tmp)           \
                                        {                      \
                                            portENABLE_FIQ();    \
                                        }                      \
                                        if(!irq_tmp)           \
                                        {                      \
                                            portENABLE_IRQ();    \
                                        }                      \
                                   }while(0)

#endif // LL_H_


