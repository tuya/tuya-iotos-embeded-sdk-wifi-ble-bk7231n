#ifndef _REG_TIMER_H_
#define _REG_TIMER_H_

#include <stdint.h>
#include "_reg_timer.h"
#include "ble_compiler.h"
#include "architect.h"
#include "ble_reg_access.h"

#define REG_TIMER_COUNT 4

#define REG_TIMER_DECODING_MASK 0x0000000F

/**
 * @brief TIME_CTRL register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     00               ENABLE   0
 * </pre>
 */
#define TMR_TIME_CTRL_ADDR   0x1000E000
#define TMR_TIME_CTRL_OFFSET 0x00000000
#define TMR_TIME_CTRL_INDEX  0x00000000
#define TMR_TIME_CTRL_RESET  0x00000000

__INLINE uint32_t tmr_time_ctrl_get(void)
{
    return REG_PL_RD(TMR_TIME_CTRL_ADDR);
}

__INLINE void tmr_time_ctrl_set(uint32_t value)
{
    REG_PL_WR(TMR_TIME_CTRL_ADDR, value);
}

// field definitions
#define TMR_ENABLE_BIT    ((uint32_t)0x00000001)
#define TMR_ENABLE_POS    0

#define TMR_ENABLE_RST    0x0

__INLINE uint8_t tmr_enable_getf(void)
{
    uint32_t localVal = REG_PL_RD(TMR_TIME_CTRL_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x00000001)) == 0);
    return (localVal >> 0);
}

__INLINE void tmr_enable_setf(uint8_t enable)
{
    ASSERT_ERR((((uint32_t)enable << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(TMR_TIME_CTRL_ADDR, (uint32_t)enable << 0);
}

/**
 * @brief CURRENT_TIME register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  23:00                 time   0x0
 * </pre>
 */
#define TMR_CURRENT_TIME_ADDR   0x1000E004
#define TMR_CURRENT_TIME_OFFSET 0x00000004
#define TMR_CURRENT_TIME_INDEX  0x00000001
#define TMR_CURRENT_TIME_RESET  0x00000000

__INLINE uint32_t tmr_current_time_get(void)
{
    return REG_PL_RD(TMR_CURRENT_TIME_ADDR);
}

__INLINE void tmr_current_time_set(uint32_t value)
{
    REG_PL_WR(TMR_CURRENT_TIME_ADDR, value);
}

// field definitions
#define TMR_TIME_MASK   ((uint32_t)0x00FFFFFF)
#define TMR_TIME_LSB    0
#define TMR_TIME_WIDTH  ((uint32_t)0x00000018)

#define TMR_TIME_RST    0x0

__INLINE uint32_t tmr_time_getf(void)
{
    uint32_t localVal = REG_PL_RD(TMR_CURRENT_TIME_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x00FFFFFF)) == 0);
    return (localVal >> 0);
}

/**
 * @brief GROSS_TIME register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  23:00                gross   0x0
 * </pre>
 */
#define TMR_GROSS_TIME_ADDR   0x1000E008
#define TMR_GROSS_TIME_OFFSET 0x00000008
#define TMR_GROSS_TIME_INDEX  0x00000002
#define TMR_GROSS_TIME_RESET  0x00000000

__INLINE uint32_t tmr_gross_time_get(void)
{
    return REG_PL_RD(TMR_GROSS_TIME_ADDR);
}

__INLINE void tmr_gross_time_set(uint32_t value)
{
    REG_PL_WR(TMR_GROSS_TIME_ADDR, value);
}

// field definitions
#define TMR_GROSS_MASK   ((uint32_t)0x00FFFFFF)
#define TMR_GROSS_LSB    0
#define TMR_GROSS_WIDTH  ((uint32_t)0x00000018)

#define TMR_GROSS_RST    0x0

__INLINE uint32_t tmr_gross_getf(void)
{
    uint32_t localVal = REG_PL_RD(TMR_GROSS_TIME_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x00FFFFFF)) == 0);
    return (localVal >> 0);
}

__INLINE void tmr_gross_setf(uint32_t gross)
{
    ASSERT_ERR((((uint32_t)gross << 0) & ~((uint32_t)0x00FFFFFF)) == 0);
    REG_PL_WR(TMR_GROSS_TIME_ADDR, (uint32_t)gross << 0);
}

/**
 * @brief STATUS register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     00               expire   0
 * </pre>
 */
#define TMR_STATUS_ADDR   0x1000E00C
#define TMR_STATUS_OFFSET 0x0000000C
#define TMR_STATUS_INDEX  0x00000003
#define TMR_STATUS_RESET  0x00000000

__INLINE uint32_t tmr_status_get(void)
{
    return REG_PL_RD(TMR_STATUS_ADDR);
}

__INLINE void tmr_status_set(uint32_t value)
{
    REG_PL_WR(TMR_STATUS_ADDR, value);
}

// field definitions
#define TMR_EXPIRE_BIT    ((uint32_t)0x00000001)
#define TMR_EXPIRE_POS    0

#define TMR_EXPIRE_RST    0x0

__INLINE uint8_t tmr_expire_getf(void)
{
    uint32_t localVal = REG_PL_RD(TMR_STATUS_ADDR);
    ASSERT_ERR((localVal & ~((uint32_t)0x00000001)) == 0);
    return (localVal >> 0);
}

__INLINE void tmr_expire_setf(uint8_t expire)
{
    ASSERT_ERR((((uint32_t)expire << 0) & ~((uint32_t)0x00000001)) == 0);
    REG_PL_WR(TMR_STATUS_ADDR, (uint32_t)expire << 0);
}


#endif // _REG_TIMER_H_

