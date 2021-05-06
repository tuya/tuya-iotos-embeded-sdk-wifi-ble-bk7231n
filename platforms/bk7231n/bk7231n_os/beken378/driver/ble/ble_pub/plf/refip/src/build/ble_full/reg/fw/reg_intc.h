#ifndef _REG_INTC_H_
#define _REG_INTC_H_

#include <stdint.h>
#include "_reg_intc.h"
#include "ble_compiler.h"
#include "architect.h"
#include "ble_reg_access.h"

#define REG_INTC_COUNT 16


/**
 * @brief INTSTATUS register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  16:00              INTSTAT   0x0
 * </pre>
 */
#define INTC_INTSTATUS_ADDR   0x00800048

__INLINE uint32_t intc_status_get(void)
{
    return REG_PL_RD(INTC_INTSTATUS_ADDR);
}

__INLINE void intc_status_clear(uint32_t status)
{
    REG_PL_WR(INTC_INTSTATUS_ADDR,status);
}




/**
 * @brief IRQENABLESET register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  16:00         								   0x0
 * </pre>
 */
#define INTC_FIQIRQENABLE_ADDR   0x00800044


__INLINE void intc_enable_set(uint32_t value)
{
	  uint32_t reg;
		reg = REG_PL_RD(INTC_FIQIRQENABLE_ADDR);
		reg |= value;
    REG_PL_WR(INTC_FIQIRQENABLE_ADDR, reg);
}

__INLINE uint32_t intc_enable_get(void)
{
    return REG_PL_RD(INTC_FIQIRQENABLE_ADDR);
}



/**
 * @brief IRQENABLECLEAR register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  16:00        									   0x0
 * </pre>
 */

__INLINE void intc_enable_clear(uint32_t value)
{
   
	  uint32_t reg;
		reg = REG_PL_RD(INTC_FIQIRQENABLE_ADDR);
		reg &= ~value  ;
    REG_PL_WR(INTC_FIQIRQENABLE_ADDR, reg);
}


/**
 * @brief IRQENABLESET register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  16:00         							   0x0
 * </pre>
 */
#define INTC_MODULEENABLE_ADDR   0x00800040


__INLINE void intc_module_enable_set(uint32_t value)
{
	  uint32_t reg;
		reg = REG_PL_RD(INTC_MODULEENABLE_ADDR);
		reg |= value;
    REG_PL_WR(INTC_MODULEENABLE_ADDR, reg);
}

__INLINE uint32_t intc_module_enable_get(void)
{
    return REG_PL_RD(INTC_MODULEENABLE_ADDR);
}



/**
 * @brief  register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  16:00       				   0x0
 * </pre>
 */

__INLINE void intc_module_enable_clear(uint32_t value)
{
   
	  uint32_t reg;
		reg = REG_PL_RD(INTC_MODULEENABLE_ADDR);
		reg &= ~value ;
    REG_PL_WR(INTC_MODULEENABLE_ADDR, reg);
}



#endif // _REG_INTC_H_

