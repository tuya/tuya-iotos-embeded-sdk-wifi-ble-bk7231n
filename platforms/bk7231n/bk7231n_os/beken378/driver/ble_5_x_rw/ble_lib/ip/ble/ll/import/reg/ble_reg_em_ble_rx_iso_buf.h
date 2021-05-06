#ifndef _REG_EM_BLE_RX_ISO_BUF_H_
#define _REG_EM_BLE_RX_ISO_BUF_H_

#include <stdint.h>
#include "_reg_em_ble_rx_iso_buf.h"
#include "compiler.h"
#include "architect.h"
#include "em_map.h"
#include "ble_reg_access.h"

#define REG_EM_BLE_RX_ISO_BUF_COUNT 4

#define REG_EM_BLE_RX_ISO_BUF_DECODING_MASK 0x00000007

#define REG_EM_BLE_RX_ISO_BUF_ADDR_GET(idx) (EM_BLE_RX_ISO_BUF_OFFSET + (idx) * REG_EM_BLE_RX_ISO_BUF_SIZE)

/**
 * @brief RXISOBUFSTAT register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  15:14                 INVL   0x0
 *  07:00          RXISOLENGTH   0x0
 * </pre>
 */
#define EM_BLE_RXISOBUFSTAT_ADDR   (0x00910000 + EM_BLE_RX_ISO_BUF_OFFSET)
#define EM_BLE_RXISOBUFSTAT_INDEX  0x00000000
#define EM_BLE_RXISOBUFSTAT_RESET  0x00000000

__INLINE uint16_t em_ble_rxisobufstat_get(int elt_idx)
{
    return EM_BLE_RD(EM_BLE_RXISOBUFSTAT_ADDR + elt_idx * REG_EM_BLE_RX_ISO_BUF_SIZE);
}

__INLINE void em_ble_rxisobufstat_set(int elt_idx, uint16_t value)
{
    EM_BLE_WR(EM_BLE_RXISOBUFSTAT_ADDR + elt_idx * REG_EM_BLE_RX_ISO_BUF_SIZE, value);
}

// field definitions
#define EM_BLE_INVL_MASK          ((uint16_t)0x0000C000)
#define EM_BLE_INVL_LSB           14
#define EM_BLE_INVL_WIDTH         ((uint16_t)0x00000002)
#define EM_BLE_RXISOLENGTH_MASK   ((uint16_t)0x000000FF)
#define EM_BLE_RXISOLENGTH_LSB    0
#define EM_BLE_RXISOLENGTH_WIDTH  ((uint16_t)0x00000008)

#define EM_BLE_INVL_RST           0x0
#define EM_BLE_RXISOLENGTH_RST    0x0

__INLINE void em_ble_rxisobufstat_pack(int elt_idx, uint8_t invl, uint8_t rxisolength)
{
    BLE_ASSERT_ERR((((uint16_t)invl << 14) & ~((uint16_t)0x0000C000)) == 0);
    BLE_ASSERT_ERR((((uint16_t)rxisolength << 0) & ~((uint16_t)0x000000FF)) == 0);
    EM_BLE_WR(EM_BLE_RXISOBUFSTAT_ADDR + elt_idx * REG_EM_BLE_RX_ISO_BUF_SIZE,  ((uint16_t)invl << 14) | ((uint16_t)rxisolength << 0));
}

__INLINE void em_ble_rxisobufstat_unpack(int elt_idx, uint8_t* invl, uint8_t* rxisolength)
{
    uint16_t localVal = EM_BLE_RD(EM_BLE_RXISOBUFSTAT_ADDR + elt_idx * REG_EM_BLE_RX_ISO_BUF_SIZE);

    *invl = (localVal & ((uint16_t)0x0000C000)) >> 14;
    *rxisolength = (localVal & ((uint16_t)0x000000FF)) >> 0;
}

__INLINE uint8_t em_ble_rxisobufstat_invl_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(EM_BLE_RXISOBUFSTAT_ADDR + elt_idx * REG_EM_BLE_RX_ISO_BUF_SIZE);
    return ((localVal & ((uint16_t)0x0000C000)) >> 14);
}

__INLINE void em_ble_rxisobufstat_invl_setf(int elt_idx, uint8_t invl)
{
    BLE_ASSERT_ERR((((uint16_t)invl << 14) & ~((uint16_t)0x0000C000)) == 0);
    EM_BLE_WR(EM_BLE_RXISOBUFSTAT_ADDR + elt_idx * REG_EM_BLE_RX_ISO_BUF_SIZE, (EM_BLE_RD(EM_BLE_RXISOBUFSTAT_ADDR + elt_idx * REG_EM_BLE_RX_ISO_BUF_SIZE) & ~((uint16_t)0x0000C000)) | ((uint16_t)invl << 14));
}

__INLINE uint8_t em_ble_rxisobufstat_rxisolength_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(EM_BLE_RXISOBUFSTAT_ADDR + elt_idx * REG_EM_BLE_RX_ISO_BUF_SIZE);
    return ((localVal & ((uint16_t)0x000000FF)) >> 0);
}

__INLINE void em_ble_rxisobufstat_rxisolength_setf(int elt_idx, uint8_t rxisolength)
{
    BLE_ASSERT_ERR((((uint16_t)rxisolength << 0) & ~((uint16_t)0x000000FF)) == 0);
    EM_BLE_WR(EM_BLE_RXISOBUFSTAT_ADDR + elt_idx * REG_EM_BLE_RX_ISO_BUF_SIZE, (EM_BLE_RD(EM_BLE_RXISOBUFSTAT_ADDR + elt_idx * REG_EM_BLE_RX_ISO_BUF_SIZE) & ~((uint16_t)0x000000FF)) | ((uint16_t)rxisolength << 0));
}

/**
 * @brief RXISOBUFLBL register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  12:08         RXBUFLINKLBL   0x0
 *  07:03      RXBUFSTREAM_LBL   0x0
 *  02:00       RXBUFGROUP_LBL   0x0
 * </pre>
 */
#define EM_BLE_RXISOBUFLBL_ADDR   (0x00910002 + EM_BLE_RX_ISO_BUF_OFFSET)
#define EM_BLE_RXISOBUFLBL_INDEX  0x00000001
#define EM_BLE_RXISOBUFLBL_RESET  0x00000000

__INLINE uint16_t em_ble_rxisobuflbl_get(int elt_idx)
{
    return EM_BLE_RD(EM_BLE_RXISOBUFLBL_ADDR + elt_idx * REG_EM_BLE_RX_ISO_BUF_SIZE);
}

__INLINE void em_ble_rxisobuflbl_set(int elt_idx, uint16_t value)
{
    EM_BLE_WR(EM_BLE_RXISOBUFLBL_ADDR + elt_idx * REG_EM_BLE_RX_ISO_BUF_SIZE, value);
}

// field definitions
#define EM_BLE_RXBUFLINKLBL_MASK      ((uint16_t)0x00001F00)
#define EM_BLE_RXBUFLINKLBL_LSB       8
#define EM_BLE_RXBUFLINKLBL_WIDTH     ((uint16_t)0x00000005)
#define EM_BLE_RXBUFSTREAM_LBL_MASK   ((uint16_t)0x000000F8)
#define EM_BLE_RXBUFSTREAM_LBL_LSB    3
#define EM_BLE_RXBUFSTREAM_LBL_WIDTH  ((uint16_t)0x00000005)
#define EM_BLE_RXBUFGROUP_LBL_MASK    ((uint16_t)0x00000007)
#define EM_BLE_RXBUFGROUP_LBL_LSB     0
#define EM_BLE_RXBUFGROUP_LBL_WIDTH   ((uint16_t)0x00000003)

#define EM_BLE_RXBUFLINKLBL_RST       0x0
#define EM_BLE_RXBUFSTREAM_LBL_RST    0x0
#define EM_BLE_RXBUFGROUP_LBL_RST     0x0

__INLINE void em_ble_rxisobuflbl_pack(int elt_idx, uint8_t rxbuflinklbl, uint8_t rxbufstreamlbl, uint8_t rxbufgrouplbl)
{
    BLE_ASSERT_ERR((((uint16_t)rxbuflinklbl << 8) & ~((uint16_t)0x00001F00)) == 0);
    BLE_ASSERT_ERR((((uint16_t)rxbufstreamlbl << 3) & ~((uint16_t)0x000000F8)) == 0);
    BLE_ASSERT_ERR((((uint16_t)rxbufgrouplbl << 0) & ~((uint16_t)0x00000007)) == 0);
    EM_BLE_WR(EM_BLE_RXISOBUFLBL_ADDR + elt_idx * REG_EM_BLE_RX_ISO_BUF_SIZE,  ((uint16_t)rxbuflinklbl << 8) | ((uint16_t)rxbufstreamlbl << 3) | ((uint16_t)rxbufgrouplbl << 0));
}

__INLINE void em_ble_rxisobuflbl_unpack(int elt_idx, uint8_t* rxbuflinklbl, uint8_t* rxbufstreamlbl, uint8_t* rxbufgrouplbl)
{
    uint16_t localVal = EM_BLE_RD(EM_BLE_RXISOBUFLBL_ADDR + elt_idx * REG_EM_BLE_RX_ISO_BUF_SIZE);

    *rxbuflinklbl = (localVal & ((uint16_t)0x00001F00)) >> 8;
    *rxbufstreamlbl = (localVal & ((uint16_t)0x000000F8)) >> 3;
    *rxbufgrouplbl = (localVal & ((uint16_t)0x00000007)) >> 0;
}

__INLINE uint8_t em_ble_rxisobuflbl_rxbuflinklbl_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(EM_BLE_RXISOBUFLBL_ADDR + elt_idx * REG_EM_BLE_RX_ISO_BUF_SIZE);
    return ((localVal & ((uint16_t)0x00001F00)) >> 8);
}

__INLINE void em_ble_rxisobuflbl_rxbuflinklbl_setf(int elt_idx, uint8_t rxbuflinklbl)
{
    BLE_ASSERT_ERR((((uint16_t)rxbuflinklbl << 8) & ~((uint16_t)0x00001F00)) == 0);
    EM_BLE_WR(EM_BLE_RXISOBUFLBL_ADDR + elt_idx * REG_EM_BLE_RX_ISO_BUF_SIZE, (EM_BLE_RD(EM_BLE_RXISOBUFLBL_ADDR + elt_idx * REG_EM_BLE_RX_ISO_BUF_SIZE) & ~((uint16_t)0x00001F00)) | ((uint16_t)rxbuflinklbl << 8));
}

__INLINE uint8_t em_ble_rxisobuflbl_rxbufstream_lbl_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(EM_BLE_RXISOBUFLBL_ADDR + elt_idx * REG_EM_BLE_RX_ISO_BUF_SIZE);
    return ((localVal & ((uint16_t)0x000000F8)) >> 3);
}

__INLINE void em_ble_rxisobuflbl_rxbufstream_lbl_setf(int elt_idx, uint8_t rxbufstreamlbl)
{
    BLE_ASSERT_ERR((((uint16_t)rxbufstreamlbl << 3) & ~((uint16_t)0x000000F8)) == 0);
    EM_BLE_WR(EM_BLE_RXISOBUFLBL_ADDR + elt_idx * REG_EM_BLE_RX_ISO_BUF_SIZE, (EM_BLE_RD(EM_BLE_RXISOBUFLBL_ADDR + elt_idx * REG_EM_BLE_RX_ISO_BUF_SIZE) & ~((uint16_t)0x000000F8)) | ((uint16_t)rxbufstreamlbl << 3));
}

__INLINE uint8_t em_ble_rxisobuflbl_rxbufgroup_lbl_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(EM_BLE_RXISOBUFLBL_ADDR + elt_idx * REG_EM_BLE_RX_ISO_BUF_SIZE);
    return ((localVal & ((uint16_t)0x00000007)) >> 0);
}

__INLINE void em_ble_rxisobuflbl_rxbufgroup_lbl_setf(int elt_idx, uint8_t rxbufgrouplbl)
{
    BLE_ASSERT_ERR((((uint16_t)rxbufgrouplbl << 0) & ~((uint16_t)0x00000007)) == 0);
    EM_BLE_WR(EM_BLE_RXISOBUFLBL_ADDR + elt_idx * REG_EM_BLE_RX_ISO_BUF_SIZE, (EM_BLE_RD(EM_BLE_RXISOBUFLBL_ADDR + elt_idx * REG_EM_BLE_RX_ISO_BUF_SIZE) & ~((uint16_t)0x00000007)) | ((uint16_t)rxbufgrouplbl << 0));
}

/**
 * @brief RXISOSUBEVTCNT register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  07:00            SUBEVTCNT   0x0
 * </pre>
 */
#define EM_BLE_RXISOSUBEVTCNT_ADDR   (0x00910004 + EM_BLE_RX_ISO_BUF_OFFSET)
#define EM_BLE_RXISOSUBEVTCNT_INDEX  0x00000002
#define EM_BLE_RXISOSUBEVTCNT_RESET  0x00000000

__INLINE uint16_t em_ble_rxisosubevtcnt_get(int elt_idx)
{
    return EM_BLE_RD(EM_BLE_RXISOSUBEVTCNT_ADDR + elt_idx * REG_EM_BLE_RX_ISO_BUF_SIZE);
}

__INLINE void em_ble_rxisosubevtcnt_set(int elt_idx, uint16_t value)
{
    EM_BLE_WR(EM_BLE_RXISOSUBEVTCNT_ADDR + elt_idx * REG_EM_BLE_RX_ISO_BUF_SIZE, value);
}

// field definitions
#define EM_BLE_SUBEVTCNT_MASK   ((uint16_t)0x000000FF)
#define EM_BLE_SUBEVTCNT_LSB    0
#define EM_BLE_SUBEVTCNT_WIDTH  ((uint16_t)0x00000008)

#define EM_BLE_SUBEVTCNT_RST    0x0

__INLINE uint8_t em_ble_rxisosubevtcnt_subevtcnt_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(EM_BLE_RXISOSUBEVTCNT_ADDR + elt_idx * REG_EM_BLE_RX_ISO_BUF_SIZE);
    BLE_ASSERT_ERR((localVal & ~((uint16_t)0x000000FF)) == 0);
    return (localVal >> 0);
}

__INLINE void em_ble_rxisosubevtcnt_subevtcnt_setf(int elt_idx, uint8_t subevtcnt)
{
    BLE_ASSERT_ERR((((uint16_t)subevtcnt << 0) & ~((uint16_t)0x000000FF)) == 0);
    EM_BLE_WR(EM_BLE_RXISOSUBEVTCNT_ADDR + elt_idx * REG_EM_BLE_RX_ISO_BUF_SIZE, (uint16_t)subevtcnt << 0);
}

/**
 * @brief RXISODATABUF register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  15:00         RXISODATABUF   0x0
 * </pre>
 */
#define EM_BLE_RXISODATABUF_ADDR   (0x00910006 + EM_BLE_RX_ISO_BUF_OFFSET)
#define EM_BLE_RXISODATABUF_INDEX  0x00000003
#define EM_BLE_RXISODATABUF_RESET  0x00000000
#define EM_BLE_RXISODATABUF_COUNT  31

__INLINE uint16_t em_ble_rxisodatabuf_get(int elt_idx, int reg_idx)
{
    BLE_ASSERT_ERR(reg_idx <= 30);
    return EM_BLE_RD(EM_BLE_RXISODATABUF_ADDR + elt_idx * REG_EM_BLE_RX_ISO_BUF_SIZE + reg_idx * 2);
}

__INLINE void em_ble_rxisodatabuf_set(int elt_idx, int reg_idx, uint16_t value)
{
    BLE_ASSERT_ERR(reg_idx <= 30);
    EM_BLE_WR(EM_BLE_RXISODATABUF_ADDR + elt_idx * REG_EM_BLE_RX_ISO_BUF_SIZE + reg_idx * 2, value);
}

// field definitions
#define EM_BLE_RXISODATABUF_MASK   ((uint16_t)0x0000FFFF)
#define EM_BLE_RXISODATABUF_LSB    0
#define EM_BLE_RXISODATABUF_WIDTH  ((uint16_t)0x00000010)

#define EM_BLE_RXISODATABUF_RST    0x0

__INLINE uint16_t em_ble_rxisodatabuf_getf(int elt_idx, int reg_idx)
{
    BLE_ASSERT_ERR(reg_idx <= 30);
    uint16_t localVal = EM_BLE_RD(EM_BLE_RXISODATABUF_ADDR + elt_idx * REG_EM_BLE_RX_ISO_BUF_SIZE + reg_idx * 2);
    BLE_ASSERT_ERR((localVal & ~((uint16_t)0x0000FFFF)) == 0);
    return (localVal >> 0);
}

__INLINE void em_ble_rxisodatabuf_setf(int elt_idx, int reg_idx, uint16_t rxisodatabuf)
{
    BLE_ASSERT_ERR(reg_idx <= 30);
    BLE_ASSERT_ERR((((uint16_t)rxisodatabuf << 0) & ~((uint16_t)0x0000FFFF)) == 0);
    EM_BLE_WR(EM_BLE_RXISODATABUF_ADDR + elt_idx * REG_EM_BLE_RX_ISO_BUF_SIZE + reg_idx * 2, (uint16_t)rxisodatabuf << 0);
}


#endif // _REG_EM_BLE_RX_ISO_BUF_H_

