#ifndef _REG_EM_BLE_TX_ISO_BUF_H_
#define _REG_EM_BLE_TX_ISO_BUF_H_

#include <stdint.h>
#include "_reg_em_ble_tx_iso_buf.h"
#include "compiler.h"
#include "architect.h"
#include "em_map.h"
#include "ble_reg_access.h"

#define REG_EM_BLE_TX_ISO_BUF_COUNT 2

#define REG_EM_BLE_TX_ISO_BUF_DECODING_MASK 0x00000003

#define REG_EM_BLE_TX_ISO_BUF_ADDR_GET(idx) (EM_BLE_TX_ISO_BUF_OFFSET + (idx) * REG_EM_BLE_TX_ISO_BUF_SIZE)

/**
 * @brief TXISOBUFSETUP register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     08            TXISOMUTE   0
 *  07:00          TXISOLENGTH   0x0
 * </pre>
 */
#define EM_BLE_TXISOBUFSETUP_ADDR   (0x00910000 + EM_BLE_TX_ISO_BUF_OFFSET)
#define EM_BLE_TXISOBUFSETUP_INDEX  0x00000000
#define EM_BLE_TXISOBUFSETUP_RESET  0x00000000

__INLINE uint16_t em_ble_txisobufsetup_get(int elt_idx)
{
    return EM_BLE_RD(EM_BLE_TXISOBUFSETUP_ADDR + elt_idx * REG_EM_BLE_TX_ISO_BUF_SIZE);
}

__INLINE void em_ble_txisobufsetup_set(int elt_idx, uint16_t value)
{
    EM_BLE_WR(EM_BLE_TXISOBUFSETUP_ADDR + elt_idx * REG_EM_BLE_TX_ISO_BUF_SIZE, value);
}

// field definitions
#define EM_BLE_TXISOMUTE_BIT      ((uint16_t)0x00000100)
#define EM_BLE_TXISOMUTE_POS      8
#define EM_BLE_TXISOLENGTH_MASK   ((uint16_t)0x000000FF)
#define EM_BLE_TXISOLENGTH_LSB    0
#define EM_BLE_TXISOLENGTH_WIDTH  ((uint16_t)0x00000008)

#define EM_BLE_TXISOMUTE_RST      0x0
#define EM_BLE_TXISOLENGTH_RST    0x0

__INLINE void em_ble_txisobufsetup_pack(int elt_idx, uint8_t txisomute, uint8_t txisolength)
{
    BLE_ASSERT_ERR((((uint16_t)txisomute << 8) & ~((uint16_t)0x00000100)) == 0);
    BLE_ASSERT_ERR((((uint16_t)txisolength << 0) & ~((uint16_t)0x000000FF)) == 0);
    EM_BLE_WR(EM_BLE_TXISOBUFSETUP_ADDR + elt_idx * REG_EM_BLE_TX_ISO_BUF_SIZE,  ((uint16_t)txisomute << 8) | ((uint16_t)txisolength << 0));
}

__INLINE void em_ble_txisobufsetup_unpack(int elt_idx, uint8_t* txisomute, uint8_t* txisolength)
{
    uint16_t localVal = EM_BLE_RD(EM_BLE_TXISOBUFSETUP_ADDR + elt_idx * REG_EM_BLE_TX_ISO_BUF_SIZE);

    *txisomute = (localVal & ((uint16_t)0x00000100)) >> 8;
    *txisolength = (localVal & ((uint16_t)0x000000FF)) >> 0;
}

__INLINE uint8_t em_ble_txisobufsetup_txisomute_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(EM_BLE_TXISOBUFSETUP_ADDR + elt_idx * REG_EM_BLE_TX_ISO_BUF_SIZE);
    return ((localVal & ((uint16_t)0x00000100)) >> 8);
}

__INLINE void em_ble_txisobufsetup_txisomute_setf(int elt_idx, uint8_t txisomute)
{
    BLE_ASSERT_ERR((((uint16_t)txisomute << 8) & ~((uint16_t)0x00000100)) == 0);
    EM_BLE_WR(EM_BLE_TXISOBUFSETUP_ADDR + elt_idx * REG_EM_BLE_TX_ISO_BUF_SIZE, (EM_BLE_RD(EM_BLE_TXISOBUFSETUP_ADDR + elt_idx * REG_EM_BLE_TX_ISO_BUF_SIZE) & ~((uint16_t)0x00000100)) | ((uint16_t)txisomute << 8));
}

__INLINE uint8_t em_ble_txisobufsetup_txisolength_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(EM_BLE_TXISOBUFSETUP_ADDR + elt_idx * REG_EM_BLE_TX_ISO_BUF_SIZE);
    return ((localVal & ((uint16_t)0x000000FF)) >> 0);
}

__INLINE void em_ble_txisobufsetup_txisolength_setf(int elt_idx, uint8_t txisolength)
{
    BLE_ASSERT_ERR((((uint16_t)txisolength << 0) & ~((uint16_t)0x000000FF)) == 0);
    EM_BLE_WR(EM_BLE_TXISOBUFSETUP_ADDR + elt_idx * REG_EM_BLE_TX_ISO_BUF_SIZE, (EM_BLE_RD(EM_BLE_TXISOBUFSETUP_ADDR + elt_idx * REG_EM_BLE_TX_ISO_BUF_SIZE) & ~((uint16_t)0x000000FF)) | ((uint16_t)txisolength << 0));
}

/**
 * @brief TXISODATABUF register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  15:00         TXISODATABUF   0x0
 * </pre>
 */
#define EM_BLE_TXISODATABUF_ADDR   (0x00910002 + EM_BLE_TX_ISO_BUF_OFFSET)
#define EM_BLE_TXISODATABUF_INDEX  0x00000001
#define EM_BLE_TXISODATABUF_RESET  0x00000000
#define EM_BLE_TXISODATABUF_COUNT  33

__INLINE uint16_t em_ble_txisodatabuf_get(int elt_idx, int reg_idx)
{
    BLE_ASSERT_ERR(reg_idx <= 32);
    return EM_BLE_RD(EM_BLE_TXISODATABUF_ADDR + elt_idx * REG_EM_BLE_TX_ISO_BUF_SIZE + reg_idx * 2);
}

__INLINE void em_ble_txisodatabuf_set(int elt_idx, int reg_idx, uint16_t value)
{
    BLE_ASSERT_ERR(reg_idx <= 32);
    EM_BLE_WR(EM_BLE_TXISODATABUF_ADDR + elt_idx * REG_EM_BLE_TX_ISO_BUF_SIZE + reg_idx * 2, value);
}

// field definitions
#define EM_BLE_TXISODATABUF_MASK   ((uint16_t)0x0000FFFF)
#define EM_BLE_TXISODATABUF_LSB    0
#define EM_BLE_TXISODATABUF_WIDTH  ((uint16_t)0x00000010)

#define EM_BLE_TXISODATABUF_RST    0x0

__INLINE uint16_t em_ble_txisodatabuf_getf(int elt_idx, int reg_idx)
{
    BLE_ASSERT_ERR(reg_idx <= 32);
    uint16_t localVal = EM_BLE_RD(EM_BLE_TXISODATABUF_ADDR + elt_idx * REG_EM_BLE_TX_ISO_BUF_SIZE + reg_idx * 2);
    BLE_ASSERT_ERR((localVal & ~((uint16_t)0x0000FFFF)) == 0);
    return (localVal >> 0);
}

__INLINE void em_ble_txisodatabuf_setf(int elt_idx, int reg_idx, uint16_t txisodatabuf)
{
    BLE_ASSERT_ERR(reg_idx <= 32);
    BLE_ASSERT_ERR((((uint16_t)txisodatabuf << 0) & ~((uint16_t)0x0000FFFF)) == 0);
    EM_BLE_WR(EM_BLE_TXISODATABUF_ADDR + elt_idx * REG_EM_BLE_TX_ISO_BUF_SIZE + reg_idx * 2, (uint16_t)txisodatabuf << 0);
}


#endif // _REG_EM_BLE_TX_ISO_BUF_H_

