#ifndef _REG_BLE_EM_TX_AUDIO_BUFFER_H_
#define _REG_BLE_EM_TX_AUDIO_BUFFER_H_

#include <stdint.h>
#include "_reg_ble_em_tx_audio_buffer.h"
#include "ble_compiler.h"
#include "architect.h"
#include "em_map.h"
#include "ble_reg_access.h"

#define REG_BLE_EM_TX_AUDIO_BUFFER_COUNT 1

#define REG_BLE_EM_TX_AUDIO_BUFFER_DECODING_MASK 0x00000000

#define REG_BLE_EM_TX_AUDIO_BUFFER_ADDR_GET(idx) (EM_BLE_TX_AUDIO_BUFFER_OFFSET + (idx) * REG_BLE_EM_TX_AUDIO_BUFFER_SIZE)

/**
 * @brief TXAUDIOBUF register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  15:00           TXAUDIOBUF   0x0
 * </pre>
 */
#define BLE_TXAUDIOBUF_ADDR   (0x00814000 + EM_BLE_TX_AUDIO_BUFFER_OFFSET)
#define BLE_TXAUDIOBUF_INDEX  0x00000000
#define BLE_TXAUDIOBUF_RESET  0x00000000
#define BLE_TXAUDIOBUF_COUNT  32

__INLINE uint16_t ble_txaudiobuf_get(int elt_idx, int reg_idx)
{
    ASSERT_ERR(reg_idx <= 31);
    return EM_BLE_RD(BLE_TXAUDIOBUF_ADDR + elt_idx * REG_BLE_EM_TX_AUDIO_BUFFER_SIZE + reg_idx * 2);
}

__INLINE void ble_txaudiobuf_set(int elt_idx, int reg_idx, uint16_t value)
{
    ASSERT_ERR(reg_idx <= 31);
    EM_BLE_WR(BLE_TXAUDIOBUF_ADDR + elt_idx * REG_BLE_EM_TX_AUDIO_BUFFER_SIZE + reg_idx * 2, value);
}

// field definitions
#define BLE_TXAUDIOBUF_MASK   ((uint16_t)0x0000FFFF)
#define BLE_TXAUDIOBUF_LSB    0
#define BLE_TXAUDIOBUF_WIDTH  ((uint16_t)0x00000010)

#define BLE_TXAUDIOBUF_RST    0x0

__INLINE uint16_t ble_txaudiobuf_getf(int elt_idx, int reg_idx)
{
    ASSERT_ERR(reg_idx <= 31);
    uint16_t localVal = EM_BLE_RD(BLE_TXAUDIOBUF_ADDR + elt_idx * REG_BLE_EM_TX_AUDIO_BUFFER_SIZE + reg_idx * 2);
    ASSERT_ERR((localVal & ~((uint16_t)0x0000FFFF)) == 0);
    return (localVal >> 0);
}

__INLINE void ble_txaudiobuf_setf(int elt_idx, int reg_idx, uint16_t txaudiobuf)
{
    ASSERT_ERR(reg_idx <= 31);
    ASSERT_ERR((((uint16_t)txaudiobuf << 0) & ~((uint16_t)0x0000FFFF)) == 0);
    EM_BLE_WR(BLE_TXAUDIOBUF_ADDR + elt_idx * REG_BLE_EM_TX_AUDIO_BUFFER_SIZE + reg_idx * 2, (uint16_t)txaudiobuf << 0);
}


#endif // _REG_BLE_EM_TX_AUDIO_BUFFER_H_

