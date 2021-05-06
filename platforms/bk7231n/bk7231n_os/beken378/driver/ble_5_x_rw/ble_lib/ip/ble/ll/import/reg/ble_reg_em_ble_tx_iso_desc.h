#ifndef _REG_EM_BLE_TX_ISO_DESC_H_
#define _REG_EM_BLE_TX_ISO_DESC_H_

#include <stdint.h>
#include "_reg_em_ble_tx_iso_desc.h"
#include "compiler.h"
#include "architect.h"
#include "em_map.h"
#include "ble_reg_access.h"

#define REG_EM_BLE_TX_ISO_DESC_COUNT 6

#define REG_EM_BLE_TX_ISO_DESC_DECODING_MASK 0x0000000F

#define REG_EM_BLE_TX_ISO_DESC_ADDR_GET(idx) (EM_BLE_TX_ISO_DESC_OFFSET + (idx) * REG_EM_BLE_TX_ISO_DESC_SIZE)

/**
 * @brief TXISOPTR register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     15               TXDONE   0
 *  13:00             NEXT_PTR   0x0
 * </pre>
 */
#define EM_BLE_TXISOPTR_ADDR   (0x00910000 + EM_BLE_TX_ISO_DESC_OFFSET)
#define EM_BLE_TXISOPTR_INDEX  0x00000000
#define EM_BLE_TXISOPTR_RESET  0x00000000

__INLINE uint16_t em_ble_txisoptr_get(int elt_idx)
{
    return EM_BLE_RD(EM_BLE_TXISOPTR_ADDR + elt_idx * REG_EM_BLE_TX_ISO_DESC_SIZE);
}

__INLINE void em_ble_txisoptr_set(int elt_idx, uint16_t value)
{
    EM_BLE_WR(EM_BLE_TXISOPTR_ADDR + elt_idx * REG_EM_BLE_TX_ISO_DESC_SIZE, value);
}

// field definitions
#define EM_BLE_TXDONE_BIT      ((uint16_t)0x00008000)
#define EM_BLE_TXDONE_POS      15
#define EM_BLE_NEXT_PTR_MASK   ((uint16_t)0x00003FFF)
#define EM_BLE_NEXT_PTR_LSB    0
#define EM_BLE_NEXT_PTR_WIDTH  ((uint16_t)0x0000000E)

#define EM_BLE_TXDONE_RST      0x0
#define EM_BLE_NEXT_PTR_RST    0x0

__INLINE void em_ble_txisoptr_pack(int elt_idx, uint8_t txdone, uint16_t nextptr)
{
    BLE_ASSERT_ERR((((uint16_t)txdone << 15) & ~((uint16_t)0x00008000)) == 0);
    BLE_ASSERT_ERR((((uint16_t)nextptr << 0) & ~((uint16_t)0x00003FFF)) == 0);
    EM_BLE_WR(EM_BLE_TXISOPTR_ADDR + elt_idx * REG_EM_BLE_TX_ISO_DESC_SIZE,  ((uint16_t)txdone << 15) | ((uint16_t)nextptr << 0));
}

__INLINE void em_ble_txisoptr_unpack(int elt_idx, uint8_t* txdone, uint16_t* nextptr)
{
    uint16_t localVal = EM_BLE_RD(EM_BLE_TXISOPTR_ADDR + elt_idx * REG_EM_BLE_TX_ISO_DESC_SIZE);

    *txdone = (localVal & ((uint16_t)0x00008000)) >> 15;
    *nextptr = (localVal & ((uint16_t)0x00003FFF)) >> 0;
}

__INLINE uint8_t em_ble_txisoptr_txdone_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(EM_BLE_TXISOPTR_ADDR + elt_idx * REG_EM_BLE_TX_ISO_DESC_SIZE);
    return ((localVal & ((uint16_t)0x00008000)) >> 15);
}

__INLINE void em_ble_txisoptr_txdone_setf(int elt_idx, uint8_t txdone)
{
    BLE_ASSERT_ERR((((uint16_t)txdone << 15) & ~((uint16_t)0x00008000)) == 0);
    EM_BLE_WR(EM_BLE_TXISOPTR_ADDR + elt_idx * REG_EM_BLE_TX_ISO_DESC_SIZE, (EM_BLE_RD(EM_BLE_TXISOPTR_ADDR + elt_idx * REG_EM_BLE_TX_ISO_DESC_SIZE) & ~((uint16_t)0x00008000)) | ((uint16_t)txdone << 15));
}

__INLINE uint16_t em_ble_txisoptr_next_ptr_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(EM_BLE_TXISOPTR_ADDR + elt_idx * REG_EM_BLE_TX_ISO_DESC_SIZE);
    return ((localVal & ((uint16_t)0x00003FFF)) >> 0);
}

__INLINE void em_ble_txisoptr_next_ptr_setf(int elt_idx, uint16_t nextptr)
{
    BLE_ASSERT_ERR((((uint16_t)nextptr << 0) & ~((uint16_t)0x00003FFF)) == 0);
    EM_BLE_WR(EM_BLE_TXISOPTR_ADDR + elt_idx * REG_EM_BLE_TX_ISO_DESC_SIZE, (EM_BLE_RD(EM_BLE_TXISOPTR_ADDR + elt_idx * REG_EM_BLE_TX_ISO_DESC_SIZE) & ~((uint16_t)0x00003FFF)) | ((uint16_t)nextptr << 0));
}

/**
 * @brief TXISOCNT0 register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  15:00           TXPLD_CNT0   0x0
 * </pre>
 */
#define EM_BLE_TXISOCNT0_ADDR   (0x00910002 + EM_BLE_TX_ISO_DESC_OFFSET)
#define EM_BLE_TXISOCNT0_INDEX  0x00000001
#define EM_BLE_TXISOCNT0_RESET  0x00000000

__INLINE uint16_t em_ble_txisocnt0_get(int elt_idx)
{
    return EM_BLE_RD(EM_BLE_TXISOCNT0_ADDR + elt_idx * REG_EM_BLE_TX_ISO_DESC_SIZE);
}

__INLINE void em_ble_txisocnt0_set(int elt_idx, uint16_t value)
{
    EM_BLE_WR(EM_BLE_TXISOCNT0_ADDR + elt_idx * REG_EM_BLE_TX_ISO_DESC_SIZE, value);
}

// field definitions
#define EM_BLE_TXPLD_CNT0_MASK   ((uint16_t)0x0000FFFF)
#define EM_BLE_TXPLD_CNT0_LSB    0
#define EM_BLE_TXPLD_CNT0_WIDTH  ((uint16_t)0x00000010)

#define EM_BLE_TXPLD_CNT0_RST    0x0

__INLINE uint16_t em_ble_txisocnt0_txpld_cnt0_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(EM_BLE_TXISOCNT0_ADDR + elt_idx * REG_EM_BLE_TX_ISO_DESC_SIZE);
    BLE_ASSERT_ERR((localVal & ~((uint16_t)0x0000FFFF)) == 0);
    return (localVal >> 0);
}

__INLINE void em_ble_txisocnt0_txpld_cnt0_setf(int elt_idx, uint16_t txpldcnt0)
{
    BLE_ASSERT_ERR((((uint16_t)txpldcnt0 << 0) & ~((uint16_t)0x0000FFFF)) == 0);
    EM_BLE_WR(EM_BLE_TXISOCNT0_ADDR + elt_idx * REG_EM_BLE_TX_ISO_DESC_SIZE, (uint16_t)txpldcnt0 << 0);
}

/**
 * @brief TXISOCNT1 register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  15:00           TXPLD_CNT1   0x0
 * </pre>
 */
#define EM_BLE_TXISOCNT1_ADDR   (0x00910004 + EM_BLE_TX_ISO_DESC_OFFSET)
#define EM_BLE_TXISOCNT1_INDEX  0x00000002
#define EM_BLE_TXISOCNT1_RESET  0x00000000

__INLINE uint16_t em_ble_txisocnt1_get(int elt_idx)
{
    return EM_BLE_RD(EM_BLE_TXISOCNT1_ADDR + elt_idx * REG_EM_BLE_TX_ISO_DESC_SIZE);
}

__INLINE void em_ble_txisocnt1_set(int elt_idx, uint16_t value)
{
    EM_BLE_WR(EM_BLE_TXISOCNT1_ADDR + elt_idx * REG_EM_BLE_TX_ISO_DESC_SIZE, value);
}

// field definitions
#define EM_BLE_TXPLD_CNT1_MASK   ((uint16_t)0x0000FFFF)
#define EM_BLE_TXPLD_CNT1_LSB    0
#define EM_BLE_TXPLD_CNT1_WIDTH  ((uint16_t)0x00000010)

#define EM_BLE_TXPLD_CNT1_RST    0x0

__INLINE uint16_t em_ble_txisocnt1_txpld_cnt1_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(EM_BLE_TXISOCNT1_ADDR + elt_idx * REG_EM_BLE_TX_ISO_DESC_SIZE);
    BLE_ASSERT_ERR((localVal & ~((uint16_t)0x0000FFFF)) == 0);
    return (localVal >> 0);
}

__INLINE void em_ble_txisocnt1_txpld_cnt1_setf(int elt_idx, uint16_t txpldcnt1)
{
    BLE_ASSERT_ERR((((uint16_t)txpldcnt1 << 0) & ~((uint16_t)0x0000FFFF)) == 0);
    EM_BLE_WR(EM_BLE_TXISOCNT1_ADDR + elt_idx * REG_EM_BLE_TX_ISO_DESC_SIZE, (uint16_t)txpldcnt1 << 0);
}

/**
 * @brief TXISOCNT2 register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  15:08       TXFLUSHINSTANT   0x0
 *  06:00            TXISOCNT2   0x0
 * </pre>
 */
#define EM_BLE_TXISOCNT2_ADDR   (0x00910006 + EM_BLE_TX_ISO_DESC_OFFSET)
#define EM_BLE_TXISOCNT2_INDEX  0x00000003
#define EM_BLE_TXISOCNT2_RESET  0x00000000

__INLINE uint16_t em_ble_txisocnt2_get(int elt_idx)
{
    return EM_BLE_RD(EM_BLE_TXISOCNT2_ADDR + elt_idx * REG_EM_BLE_TX_ISO_DESC_SIZE);
}

__INLINE void em_ble_txisocnt2_set(int elt_idx, uint16_t value)
{
    EM_BLE_WR(EM_BLE_TXISOCNT2_ADDR + elt_idx * REG_EM_BLE_TX_ISO_DESC_SIZE, value);
}

// field definitions
#define EM_BLE_TXFLUSHINSTANT_MASK   ((uint16_t)0x0000FF00)
#define EM_BLE_TXFLUSHINSTANT_LSB    8
#define EM_BLE_TXFLUSHINSTANT_WIDTH  ((uint16_t)0x00000008)
#define EM_BLE_TXISOCNT2_MASK        ((uint16_t)0x0000007F)
#define EM_BLE_TXISOCNT2_LSB         0
#define EM_BLE_TXISOCNT2_WIDTH       ((uint16_t)0x00000007)

#define EM_BLE_TXFLUSHINSTANT_RST    0x0
#define EM_BLE_TXISOCNT2_RST         0x0

__INLINE void em_ble_txisocnt2_pack(int elt_idx, uint8_t txflushinstant, uint8_t txisocnt2)
{
    BLE_ASSERT_ERR((((uint16_t)txflushinstant << 8) & ~((uint16_t)0x0000FF00)) == 0);
    BLE_ASSERT_ERR((((uint16_t)txisocnt2 << 0) & ~((uint16_t)0x0000007F)) == 0);
    EM_BLE_WR(EM_BLE_TXISOCNT2_ADDR + elt_idx * REG_EM_BLE_TX_ISO_DESC_SIZE,  ((uint16_t)txflushinstant << 8) | ((uint16_t)txisocnt2 << 0));
}

__INLINE void em_ble_txisocnt2_unpack(int elt_idx, uint8_t* txflushinstant, uint8_t* txisocnt2)
{
    uint16_t localVal = EM_BLE_RD(EM_BLE_TXISOCNT2_ADDR + elt_idx * REG_EM_BLE_TX_ISO_DESC_SIZE);

    *txflushinstant = (localVal & ((uint16_t)0x0000FF00)) >> 8;
    *txisocnt2 = (localVal & ((uint16_t)0x0000007F)) >> 0;
}

__INLINE uint8_t em_ble_txisocnt2_txflushinstant_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(EM_BLE_TXISOCNT2_ADDR + elt_idx * REG_EM_BLE_TX_ISO_DESC_SIZE);
    return ((localVal & ((uint16_t)0x0000FF00)) >> 8);
}

__INLINE void em_ble_txisocnt2_txflushinstant_setf(int elt_idx, uint8_t txflushinstant)
{
    BLE_ASSERT_ERR((((uint16_t)txflushinstant << 8) & ~((uint16_t)0x0000FF00)) == 0);
    EM_BLE_WR(EM_BLE_TXISOCNT2_ADDR + elt_idx * REG_EM_BLE_TX_ISO_DESC_SIZE, (EM_BLE_RD(EM_BLE_TXISOCNT2_ADDR + elt_idx * REG_EM_BLE_TX_ISO_DESC_SIZE) & ~((uint16_t)0x0000FF00)) | ((uint16_t)txflushinstant << 8));
}

__INLINE uint8_t em_ble_txisocnt2_txisocnt2_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(EM_BLE_TXISOCNT2_ADDR + elt_idx * REG_EM_BLE_TX_ISO_DESC_SIZE);
    return ((localVal & ((uint16_t)0x0000007F)) >> 0);
}

__INLINE void em_ble_txisocnt2_txisocnt2_setf(int elt_idx, uint8_t txisocnt2)
{
    BLE_ASSERT_ERR((((uint16_t)txisocnt2 << 0) & ~((uint16_t)0x0000007F)) == 0);
    EM_BLE_WR(EM_BLE_TXISOCNT2_ADDR + elt_idx * REG_EM_BLE_TX_ISO_DESC_SIZE, (EM_BLE_RD(EM_BLE_TXISOCNT2_ADDR + elt_idx * REG_EM_BLE_TX_ISO_DESC_SIZE) & ~((uint16_t)0x0000007F)) | ((uint16_t)txisocnt2 << 0));
}

/**
 * @brief TXISOPHM0 register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  07:05           TXISOM0RFU   0x0
 *     04                 TXMD   0
 *     03                 TXSN   0
 *     02               TXNESN   0
 *  01:00               TXLLID   0x0
 * </pre>
 */
#define EM_BLE_TXISOPHM0_ADDR   (0x00910008 + EM_BLE_TX_ISO_DESC_OFFSET)
#define EM_BLE_TXISOPHM0_INDEX  0x00000004
#define EM_BLE_TXISOPHM0_RESET  0x00000000

__INLINE uint16_t em_ble_txisophm0_get(int elt_idx)
{
    return EM_BLE_RD(EM_BLE_TXISOPHM0_ADDR + elt_idx * REG_EM_BLE_TX_ISO_DESC_SIZE);
}

__INLINE void em_ble_txisophm0_set(int elt_idx, uint16_t value)
{
    EM_BLE_WR(EM_BLE_TXISOPHM0_ADDR + elt_idx * REG_EM_BLE_TX_ISO_DESC_SIZE, value);
}

// field definitions
#define EM_BLE_TXISOM0RFU_MASK   ((uint16_t)0x000000E0)
#define EM_BLE_TXISOM0RFU_LSB    5
#define EM_BLE_TXISOM0RFU_WIDTH  ((uint16_t)0x00000003)
#define EM_BLE_TXMD_BIT          ((uint16_t)0x00000010)
#define EM_BLE_TXMD_POS          4
#define EM_BLE_TXSN_BIT          ((uint16_t)0x00000008)
#define EM_BLE_TXSN_POS          3
#define EM_BLE_TXNESN_BIT        ((uint16_t)0x00000004)
#define EM_BLE_TXNESN_POS        2
#define EM_BLE_TXLLID_MASK       ((uint16_t)0x00000003)
#define EM_BLE_TXLLID_LSB        0
#define EM_BLE_TXLLID_WIDTH      ((uint16_t)0x00000002)

#define EM_BLE_TXISOM0RFU_RST    0x0
#define EM_BLE_TXMD_RST          0x0
#define EM_BLE_TXSN_RST          0x0
#define EM_BLE_TXNESN_RST        0x0
#define EM_BLE_TXLLID_RST        0x0

__INLINE void em_ble_txisophm0_pack(int elt_idx, uint8_t txisom0rfu, uint8_t txmd, uint8_t txsn, uint8_t txnesn, uint8_t txllid)
{
    BLE_ASSERT_ERR((((uint16_t)txisom0rfu << 5) & ~((uint16_t)0x000000E0)) == 0);
    BLE_ASSERT_ERR((((uint16_t)txmd << 4) & ~((uint16_t)0x00000010)) == 0);
    BLE_ASSERT_ERR((((uint16_t)txsn << 3) & ~((uint16_t)0x00000008)) == 0);
    BLE_ASSERT_ERR((((uint16_t)txnesn << 2) & ~((uint16_t)0x00000004)) == 0);
    BLE_ASSERT_ERR((((uint16_t)txllid << 0) & ~((uint16_t)0x00000003)) == 0);
    EM_BLE_WR(EM_BLE_TXISOPHM0_ADDR + elt_idx * REG_EM_BLE_TX_ISO_DESC_SIZE,  ((uint16_t)txisom0rfu << 5) | ((uint16_t)txmd << 4) | ((uint16_t)txsn << 3) | ((uint16_t)txnesn << 2) | ((uint16_t)txllid << 0));
}

__INLINE void em_ble_txisophm0_unpack(int elt_idx, uint8_t* txisom0rfu, uint8_t* txmd, uint8_t* txsn, uint8_t* txnesn, uint8_t* txllid)
{
    uint16_t localVal = EM_BLE_RD(EM_BLE_TXISOPHM0_ADDR + elt_idx * REG_EM_BLE_TX_ISO_DESC_SIZE);

    *txisom0rfu = (localVal & ((uint16_t)0x000000E0)) >> 5;
    *txmd = (localVal & ((uint16_t)0x00000010)) >> 4;
    *txsn = (localVal & ((uint16_t)0x00000008)) >> 3;
    *txnesn = (localVal & ((uint16_t)0x00000004)) >> 2;
    *txllid = (localVal & ((uint16_t)0x00000003)) >> 0;
}

__INLINE uint8_t em_ble_txisophm0_txisom0rfu_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(EM_BLE_TXISOPHM0_ADDR + elt_idx * REG_EM_BLE_TX_ISO_DESC_SIZE);
    return ((localVal & ((uint16_t)0x000000E0)) >> 5);
}

__INLINE void em_ble_txisophm0_txisom0rfu_setf(int elt_idx, uint8_t txisom0rfu)
{
    BLE_ASSERT_ERR((((uint16_t)txisom0rfu << 5) & ~((uint16_t)0x000000E0)) == 0);
    EM_BLE_WR(EM_BLE_TXISOPHM0_ADDR + elt_idx * REG_EM_BLE_TX_ISO_DESC_SIZE, (EM_BLE_RD(EM_BLE_TXISOPHM0_ADDR + elt_idx * REG_EM_BLE_TX_ISO_DESC_SIZE) & ~((uint16_t)0x000000E0)) | ((uint16_t)txisom0rfu << 5));
}

__INLINE uint8_t em_ble_txisophm0_txmd_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(EM_BLE_TXISOPHM0_ADDR + elt_idx * REG_EM_BLE_TX_ISO_DESC_SIZE);
    return ((localVal & ((uint16_t)0x00000010)) >> 4);
}

__INLINE void em_ble_txisophm0_txmd_setf(int elt_idx, uint8_t txmd)
{
    BLE_ASSERT_ERR((((uint16_t)txmd << 4) & ~((uint16_t)0x00000010)) == 0);
    EM_BLE_WR(EM_BLE_TXISOPHM0_ADDR + elt_idx * REG_EM_BLE_TX_ISO_DESC_SIZE, (EM_BLE_RD(EM_BLE_TXISOPHM0_ADDR + elt_idx * REG_EM_BLE_TX_ISO_DESC_SIZE) & ~((uint16_t)0x00000010)) | ((uint16_t)txmd << 4));
}

__INLINE uint8_t em_ble_txisophm0_txsn_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(EM_BLE_TXISOPHM0_ADDR + elt_idx * REG_EM_BLE_TX_ISO_DESC_SIZE);
    return ((localVal & ((uint16_t)0x00000008)) >> 3);
}

__INLINE void em_ble_txisophm0_txsn_setf(int elt_idx, uint8_t txsn)
{
    BLE_ASSERT_ERR((((uint16_t)txsn << 3) & ~((uint16_t)0x00000008)) == 0);
    EM_BLE_WR(EM_BLE_TXISOPHM0_ADDR + elt_idx * REG_EM_BLE_TX_ISO_DESC_SIZE, (EM_BLE_RD(EM_BLE_TXISOPHM0_ADDR + elt_idx * REG_EM_BLE_TX_ISO_DESC_SIZE) & ~((uint16_t)0x00000008)) | ((uint16_t)txsn << 3));
}

__INLINE uint8_t em_ble_txisophm0_txnesn_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(EM_BLE_TXISOPHM0_ADDR + elt_idx * REG_EM_BLE_TX_ISO_DESC_SIZE);
    return ((localVal & ((uint16_t)0x00000004)) >> 2);
}

__INLINE void em_ble_txisophm0_txnesn_setf(int elt_idx, uint8_t txnesn)
{
    BLE_ASSERT_ERR((((uint16_t)txnesn << 2) & ~((uint16_t)0x00000004)) == 0);
    EM_BLE_WR(EM_BLE_TXISOPHM0_ADDR + elt_idx * REG_EM_BLE_TX_ISO_DESC_SIZE, (EM_BLE_RD(EM_BLE_TXISOPHM0_ADDR + elt_idx * REG_EM_BLE_TX_ISO_DESC_SIZE) & ~((uint16_t)0x00000004)) | ((uint16_t)txnesn << 2));
}

__INLINE uint8_t em_ble_txisophm0_txllid_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(EM_BLE_TXISOPHM0_ADDR + elt_idx * REG_EM_BLE_TX_ISO_DESC_SIZE);
    return ((localVal & ((uint16_t)0x00000003)) >> 0);
}

__INLINE void em_ble_txisophm0_txllid_setf(int elt_idx, uint8_t txllid)
{
    BLE_ASSERT_ERR((((uint16_t)txllid << 0) & ~((uint16_t)0x00000003)) == 0);
    EM_BLE_WR(EM_BLE_TXISOPHM0_ADDR + elt_idx * REG_EM_BLE_TX_ISO_DESC_SIZE, (EM_BLE_RD(EM_BLE_TXISOPHM0_ADDR + elt_idx * REG_EM_BLE_TX_ISO_DESC_SIZE) & ~((uint16_t)0x00000003)) | ((uint16_t)txllid << 0));
}

/**
 * @brief TXISOBUFPTR register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  13:00          TXISOBUFPTR   0x0
 * </pre>
 */
#define EM_BLE_TXISOBUFPTR_ADDR   (0x0091000A + EM_BLE_TX_ISO_DESC_OFFSET)
#define EM_BLE_TXISOBUFPTR_INDEX  0x00000005
#define EM_BLE_TXISOBUFPTR_RESET  0x00000000

__INLINE uint16_t em_ble_txisobufptr_get(int elt_idx)
{
    return EM_BLE_RD(EM_BLE_TXISOBUFPTR_ADDR + elt_idx * REG_EM_BLE_TX_ISO_DESC_SIZE);
}

__INLINE void em_ble_txisobufptr_set(int elt_idx, uint16_t value)
{
    EM_BLE_WR(EM_BLE_TXISOBUFPTR_ADDR + elt_idx * REG_EM_BLE_TX_ISO_DESC_SIZE, value);
}

// field definitions
#define EM_BLE_TXISOBUFPTR_MASK   ((uint16_t)0x00003FFF)
#define EM_BLE_TXISOBUFPTR_LSB    0
#define EM_BLE_TXISOBUFPTR_WIDTH  ((uint16_t)0x0000000E)

#define EM_BLE_TXISOBUFPTR_RST    0x0

__INLINE uint16_t em_ble_txisobufptr_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(EM_BLE_TXISOBUFPTR_ADDR + elt_idx * REG_EM_BLE_TX_ISO_DESC_SIZE);
    BLE_ASSERT_ERR((localVal & ~((uint16_t)0x00003FFF)) == 0);
    return (localVal >> 0);
}

__INLINE void em_ble_txisobufptr_setf(int elt_idx, uint16_t txisobufptr)
{
    BLE_ASSERT_ERR((((uint16_t)txisobufptr << 0) & ~((uint16_t)0x00003FFF)) == 0);
    EM_BLE_WR(EM_BLE_TXISOBUFPTR_ADDR + elt_idx * REG_EM_BLE_TX_ISO_DESC_SIZE, (uint16_t)txisobufptr << 0);
}


#endif // _REG_EM_BLE_TX_ISO_DESC_H_

