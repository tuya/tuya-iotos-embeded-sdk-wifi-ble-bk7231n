#ifndef _REG_EM_BLE_TX_BUF_H_
#define _REG_EM_BLE_TX_BUF_H_

#include <stdint.h>
#include "_reg_em_ble_tx_buf.h"
#include "compiler.h"
#include "architect.h"
#include "em_map.h"
#include "ble_reg_access.h"

#define REG_EM_BLE_TX_BUF_COUNT 1

#define REG_EM_BLE_TX_BUF_DECODING_MASK 0x00000000

#define REG_EM_BLE_TX_BUF_ADDR_GET(idx) (EM_BLE_TX_BUFFER_OFFSET + (idx) * REG_EM_BLE_TX_BUF_SIZE)

/**
 * @brief TXBUF register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  15:00                TXBUF   0x0
 * </pre>
 */
#define EM_BLE_TXBUF_ADDR   (0x00910000 + EM_BLE_TX_BUFFER_OFFSET)
#define EM_BLE_TXBUF_INDEX  0x00000000
#define EM_BLE_TXBUF_RESET  0x00000000
#define EM_BLE_TXBUF_COUNT  19

__INLINE uint16_t em_ble_txbuf_get(int elt_idx, int reg_idx)
{
    BLE_ASSERT_ERR(reg_idx <= 18);
    return EM_BLE_RD(EM_BLE_TXBUF_ADDR + elt_idx * REG_EM_BLE_TX_BUF_SIZE + reg_idx * 2);
}

__INLINE void em_ble_txbuf_set(int elt_idx, int reg_idx, uint16_t value)
{
    BLE_ASSERT_ERR(reg_idx <= 18);
    EM_BLE_WR(EM_BLE_TXBUF_ADDR + elt_idx * REG_EM_BLE_TX_BUF_SIZE + reg_idx * 2, value);
}

// field definitions
#define EM_BLE_TXBUF_MASK   ((uint16_t)0x0000FFFF)
#define EM_BLE_TXBUF_LSB    0
#define EM_BLE_TXBUF_WIDTH  ((uint16_t)0x00000010)

#define EM_BLE_TXBUF_RST    0x0

__INLINE uint16_t em_ble_txbuf_getf(int elt_idx, int reg_idx)
{
    BLE_ASSERT_ERR(reg_idx <= 18);
    uint16_t localVal = EM_BLE_RD(EM_BLE_TXBUF_ADDR + elt_idx * REG_EM_BLE_TX_BUF_SIZE + reg_idx * 2);
    BLE_ASSERT_ERR((localVal & ~((uint16_t)0x0000FFFF)) == 0);
    return (localVal >> 0);
}

__INLINE void em_ble_txbuf_setf(int elt_idx, int reg_idx, uint16_t txbuf)
{
    BLE_ASSERT_ERR(reg_idx <= 18);
    BLE_ASSERT_ERR((((uint16_t)txbuf << 0) & ~((uint16_t)0x0000FFFF)) == 0);
    EM_BLE_WR(EM_BLE_TXBUF_ADDR + elt_idx * REG_EM_BLE_TX_BUF_SIZE + reg_idx * 2, (uint16_t)txbuf << 0);
}


#endif // _REG_EM_BLE_TX_BUF_H_

