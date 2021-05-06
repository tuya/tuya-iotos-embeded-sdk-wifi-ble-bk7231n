#ifndef _REG_BLE_EM_CS_H_
#define _REG_BLE_EM_CS_H_

#include <stdint.h>
#include "_reg_ble_em_cs.h"
#include "ble_compiler.h"
#include "architect.h"
#include "em_map.h"
#include "ble_reg_access.h"

#define REG_BLE_EM_CS_COUNT 46

#define REG_BLE_EM_CS_DECODING_MASK 0x0000007F

#define REG_BLE_EM_CS_ADDR_GET(idx) (EM_BLE_CS_OFFSET + (idx) * REG_BLE_EM_CS_SIZE)

/**
 * @brief CNTL register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  15:12                  PTI   0x0
 *     10             TXBSY_EN   0
 *     09             RXBSY_EN   0
 *     08              DNABORT   0
 *  04:00               FORMAT   0x0
 * </pre>
 */
#define BLE_CNTL_ADDR   (0x00814000 + EM_BLE_CS_OFFSET)
#define BLE_CNTL_INDEX  0x00000000
#define BLE_CNTL_RESET  0x00000000

__INLINE uint16_t ble_cntl_get(int elt_idx)
{
    return EM_BLE_RD(BLE_CNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
}

__INLINE void ble_cntl_set(int elt_idx, uint16_t value)
{
    EM_BLE_WR(BLE_CNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, value);
}

// field definitions
#define BLE_PTI_MASK        ((uint16_t)0x0000F000)
#define BLE_PTI_LSB         12
#define BLE_PTI_WIDTH       ((uint16_t)0x00000004)
#define BLE_TXBSY_EN_BIT    ((uint16_t)0x00000400)
#define BLE_TXBSY_EN_POS    10
#define BLE_RXBSY_EN_BIT    ((uint16_t)0x00000200)
#define BLE_RXBSY_EN_POS    9
#define BLE_DNABORT_BIT     ((uint16_t)0x00000100)
#define BLE_DNABORT_POS     8
#define BLE_FORMAT_MASK     ((uint16_t)0x0000001F)
#define BLE_FORMAT_LSB      0
#define BLE_FORMAT_WIDTH    ((uint16_t)0x00000005)

#define BLE_PTI_RST         0x0
#define BLE_TXBSY_EN_RST    0x0
#define BLE_RXBSY_EN_RST    0x0
#define BLE_DNABORT_RST     0x0
#define BLE_FORMAT_RST      0x0

__INLINE void ble_cntl_pack(int elt_idx, uint8_t pti, uint8_t txbsyen, uint8_t rxbsyen, uint8_t dnabort, uint8_t format)
{
    ASSERT_ERR((((uint16_t)pti << 12) & ~((uint16_t)0x0000F000)) == 0);
    ASSERT_ERR((((uint16_t)txbsyen << 10) & ~((uint16_t)0x00000400)) == 0);
    ASSERT_ERR((((uint16_t)rxbsyen << 9) & ~((uint16_t)0x00000200)) == 0);
    ASSERT_ERR((((uint16_t)dnabort << 8) & ~((uint16_t)0x00000100)) == 0);
    ASSERT_ERR((((uint16_t)format << 0) & ~((uint16_t)0x0000001F)) == 0);
    EM_BLE_WR(BLE_CNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE,  ((uint16_t)pti << 12) | ((uint16_t)txbsyen << 10) | ((uint16_t)rxbsyen << 9) | ((uint16_t)dnabort << 8) | ((uint16_t)format << 0));
}

__INLINE void ble_cntl_unpack(int elt_idx, uint8_t* pti, uint8_t* txbsyen, uint8_t* rxbsyen, uint8_t* dnabort, uint8_t* format)
{
    uint16_t localVal = EM_BLE_RD(BLE_CNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);

    *pti = (localVal & ((uint16_t)0x0000F000)) >> 12;
    *txbsyen = (localVal & ((uint16_t)0x00000400)) >> 10;
    *rxbsyen = (localVal & ((uint16_t)0x00000200)) >> 9;
    *dnabort = (localVal & ((uint16_t)0x00000100)) >> 8;
    *format = (localVal & ((uint16_t)0x0000001F)) >> 0;
}

__INLINE uint8_t ble_pti_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(BLE_CNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
    return ((localVal & ((uint16_t)0x0000F000)) >> 12);
}

__INLINE void ble_pti_setf(int elt_idx, uint8_t pti)
{
    ASSERT_ERR((((uint16_t)pti << 12) & ~((uint16_t)0x0000F000)) == 0);
    EM_BLE_WR(BLE_CNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, (EM_BLE_RD(BLE_CNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE) & ~((uint16_t)0x0000F000)) | ((uint16_t)pti << 12));
}

__INLINE uint8_t ble_txbsy_en_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(BLE_CNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
    return ((localVal & ((uint16_t)0x00000400)) >> 10);
}

__INLINE void ble_txbsy_en_setf(int elt_idx, uint8_t txbsyen)
{
    ASSERT_ERR((((uint16_t)txbsyen << 10) & ~((uint16_t)0x00000400)) == 0);
    EM_BLE_WR(BLE_CNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, (EM_BLE_RD(BLE_CNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE) & ~((uint16_t)0x00000400)) | ((uint16_t)txbsyen << 10));
}

__INLINE uint8_t ble_rxbsy_en_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(BLE_CNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
    return ((localVal & ((uint16_t)0x00000200)) >> 9);
}

__INLINE void ble_rxbsy_en_setf(int elt_idx, uint8_t rxbsyen)
{
    ASSERT_ERR((((uint16_t)rxbsyen << 9) & ~((uint16_t)0x00000200)) == 0);
    EM_BLE_WR(BLE_CNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, (EM_BLE_RD(BLE_CNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE) & ~((uint16_t)0x00000200)) | ((uint16_t)rxbsyen << 9));
}

__INLINE uint8_t ble_dnabort_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(BLE_CNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
    return ((localVal & ((uint16_t)0x00000100)) >> 8);
}

__INLINE void ble_dnabort_setf(int elt_idx, uint8_t dnabort)
{
    ASSERT_ERR((((uint16_t)dnabort << 8) & ~((uint16_t)0x00000100)) == 0);
    EM_BLE_WR(BLE_CNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, (EM_BLE_RD(BLE_CNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE) & ~((uint16_t)0x00000100)) | ((uint16_t)dnabort << 8));
}

__INLINE uint8_t ble_format_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(BLE_CNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
    return ((localVal & ((uint16_t)0x0000001F)) >> 0);
}

__INLINE void ble_format_setf(int elt_idx, uint8_t format)
{
    ASSERT_ERR((((uint16_t)format << 0) & ~((uint16_t)0x0000001F)) == 0);
    EM_BLE_WR(BLE_CNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, (EM_BLE_RD(BLE_CNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE) & ~((uint16_t)0x0000001F)) | ((uint16_t)format << 0));
}

/**
 * @brief FCNTOFFSET register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  09:00           FCNTOFFSET   0x0
 * </pre>
 */
#define BLE_FCNTOFFSET_ADDR   (0x00814002 + EM_BLE_CS_OFFSET)
#define BLE_FCNTOFFSET_INDEX  0x00000001
#define BLE_FCNTOFFSET_RESET  0x00000000

__INLINE uint16_t ble_fcntoffset_get(int elt_idx)
{
    return EM_BLE_RD(BLE_FCNTOFFSET_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
}

__INLINE void ble_fcntoffset_set(int elt_idx, uint16_t value)
{
    EM_BLE_WR(BLE_FCNTOFFSET_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, value);
}

// field definitions
#define BLE_FCNTOFFSET_MASK   ((uint16_t)0x000003FF)
#define BLE_FCNTOFFSET_LSB    0
#define BLE_FCNTOFFSET_WIDTH  ((uint16_t)0x0000000A)

#define BLE_FCNTOFFSET_RST    0x0

__INLINE uint16_t ble_fcntoffset_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(BLE_FCNTOFFSET_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
    ASSERT_ERR((localVal & ~((uint16_t)0x000003FF)) == 0);
    return (localVal >> 0);
}

__INLINE void ble_fcntoffset_setf(int elt_idx, uint16_t fcntoffset)
{
    ASSERT_ERR((((uint16_t)fcntoffset << 0) & ~((uint16_t)0x000003FF)) == 0);
    EM_BLE_WR(BLE_FCNTOFFSET_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, (uint16_t)fcntoffset << 0);
}

/**
 * @brief LINK register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     11        NULLRXLLIDFLT   0
 *     10           CRYPT_MODE   0
 *     09           TXCRYPT_EN   0
 *     08           RXCRYPT_EN   0
 *  04:00              LINKLBL   0x0
 * </pre>
 */
#define BLE_LINK_ADDR   (0x00814004 + EM_BLE_CS_OFFSET)
#define BLE_LINK_INDEX  0x00000002
#define BLE_LINK_RESET  0x00000000

__INLINE uint16_t ble_link_get(int elt_idx)
{
    return EM_BLE_RD(BLE_LINK_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
}

__INLINE void ble_link_set(int elt_idx, uint16_t value)
{
    EM_BLE_WR(BLE_LINK_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, value);
}

// field definitions
#define BLE_NULLRXLLIDFLT_BIT    ((uint16_t)0x00000800)
#define BLE_NULLRXLLIDFLT_POS    11
#define BLE_CRYPT_MODE_BIT       ((uint16_t)0x00000400)
#define BLE_CRYPT_MODE_POS       10
#define BLE_TXCRYPT_EN_BIT       ((uint16_t)0x00000200)
#define BLE_TXCRYPT_EN_POS       9
#define BLE_RXCRYPT_EN_BIT       ((uint16_t)0x00000100)
#define BLE_RXCRYPT_EN_POS       8
#define BLE_LINKLBL_MASK         ((uint16_t)0x0000001F)
#define BLE_LINKLBL_LSB          0
#define BLE_LINKLBL_WIDTH        ((uint16_t)0x00000005)

#define BLE_NULLRXLLIDFLT_RST    0x0
#define BLE_CRYPT_MODE_RST       0x0
#define BLE_TXCRYPT_EN_RST       0x0
#define BLE_RXCRYPT_EN_RST       0x0
#define BLE_LINKLBL_RST          0x0

__INLINE void ble_link_pack(int elt_idx, uint8_t nullrxllidflt, uint8_t cryptmode, uint8_t txcrypten, uint8_t rxcrypten, uint8_t linklbl)
{
    ASSERT_ERR((((uint16_t)nullrxllidflt << 11) & ~((uint16_t)0x00000800)) == 0);
    ASSERT_ERR((((uint16_t)cryptmode << 10) & ~((uint16_t)0x00000400)) == 0);
    ASSERT_ERR((((uint16_t)txcrypten << 9) & ~((uint16_t)0x00000200)) == 0);
    ASSERT_ERR((((uint16_t)rxcrypten << 8) & ~((uint16_t)0x00000100)) == 0);
    ASSERT_ERR((((uint16_t)linklbl << 0) & ~((uint16_t)0x0000001F)) == 0);
    EM_BLE_WR(BLE_LINK_ADDR + elt_idx * REG_BLE_EM_CS_SIZE,  ((uint16_t)nullrxllidflt << 11) | ((uint16_t)cryptmode << 10) | ((uint16_t)txcrypten << 9) | ((uint16_t)rxcrypten << 8) | ((uint16_t)linklbl << 0));
}

__INLINE void ble_link_unpack(int elt_idx, uint8_t* nullrxllidflt, uint8_t* cryptmode, uint8_t* txcrypten, uint8_t* rxcrypten, uint8_t* linklbl)
{
    uint16_t localVal = EM_BLE_RD(BLE_LINK_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);

    *nullrxllidflt = (localVal & ((uint16_t)0x00000800)) >> 11;
    *cryptmode = (localVal & ((uint16_t)0x00000400)) >> 10;
    *txcrypten = (localVal & ((uint16_t)0x00000200)) >> 9;
    *rxcrypten = (localVal & ((uint16_t)0x00000100)) >> 8;
    *linklbl = (localVal & ((uint16_t)0x0000001F)) >> 0;
}

__INLINE uint8_t ble_nullrxllidflt_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(BLE_LINK_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
    return ((localVal & ((uint16_t)0x00000800)) >> 11);
}

__INLINE void ble_nullrxllidflt_setf(int elt_idx, uint8_t nullrxllidflt)
{
    ASSERT_ERR((((uint16_t)nullrxllidflt << 11) & ~((uint16_t)0x00000800)) == 0);
    EM_BLE_WR(BLE_LINK_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, (EM_BLE_RD(BLE_LINK_ADDR + elt_idx * REG_BLE_EM_CS_SIZE) & ~((uint16_t)0x00000800)) | ((uint16_t)nullrxllidflt << 11));
}

__INLINE uint8_t ble_crypt_mode_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(BLE_LINK_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
    return ((localVal & ((uint16_t)0x00000400)) >> 10);
}

__INLINE void ble_crypt_mode_setf(int elt_idx, uint8_t cryptmode)
{
    ASSERT_ERR((((uint16_t)cryptmode << 10) & ~((uint16_t)0x00000400)) == 0);
    EM_BLE_WR(BLE_LINK_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, (EM_BLE_RD(BLE_LINK_ADDR + elt_idx * REG_BLE_EM_CS_SIZE) & ~((uint16_t)0x00000400)) | ((uint16_t)cryptmode << 10));
}

__INLINE uint8_t ble_txcrypt_en_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(BLE_LINK_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
    return ((localVal & ((uint16_t)0x00000200)) >> 9);
}

__INLINE void ble_txcrypt_en_setf(int elt_idx, uint8_t txcrypten)
{
    ASSERT_ERR((((uint16_t)txcrypten << 9) & ~((uint16_t)0x00000200)) == 0);
    EM_BLE_WR(BLE_LINK_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, (EM_BLE_RD(BLE_LINK_ADDR + elt_idx * REG_BLE_EM_CS_SIZE) & ~((uint16_t)0x00000200)) | ((uint16_t)txcrypten << 9));
}

__INLINE uint8_t ble_rxcrypt_en_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(BLE_LINK_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
    return ((localVal & ((uint16_t)0x00000100)) >> 8);
}

__INLINE void ble_rxcrypt_en_setf(int elt_idx, uint8_t rxcrypten)
{
    ASSERT_ERR((((uint16_t)rxcrypten << 8) & ~((uint16_t)0x00000100)) == 0);
    EM_BLE_WR(BLE_LINK_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, (EM_BLE_RD(BLE_LINK_ADDR + elt_idx * REG_BLE_EM_CS_SIZE) & ~((uint16_t)0x00000100)) | ((uint16_t)rxcrypten << 8));
}

__INLINE uint8_t ble_linklbl_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(BLE_LINK_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
    return ((localVal & ((uint16_t)0x0000001F)) >> 0);
}

__INLINE void ble_linklbl_setf(int elt_idx, uint8_t linklbl)
{
    ASSERT_ERR((((uint16_t)linklbl << 0) & ~((uint16_t)0x0000001F)) == 0);
    EM_BLE_WR(BLE_LINK_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, (EM_BLE_RD(BLE_LINK_ADDR + elt_idx * REG_BLE_EM_CS_SIZE) & ~((uint16_t)0x0000001F)) | ((uint16_t)linklbl << 0));
}

/**
 * @brief THRCNTL_RATECNTL register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  15:12                RXTHR   0x0
 *  03:02               RXRATE   0x0
 *  01:00               TXRATE   0x0
 * </pre>
 */
#define BLE_THRCNTL_RATECNTL_ADDR   (0x00814006 + EM_BLE_CS_OFFSET)
#define BLE_THRCNTL_RATECNTL_INDEX  0x00000003
#define BLE_THRCNTL_RATECNTL_RESET  0x00000000

__INLINE uint16_t ble_thrcntl_ratecntl_get(int elt_idx)
{
    return EM_BLE_RD(BLE_THRCNTL_RATECNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
}

__INLINE void ble_thrcntl_ratecntl_set(int elt_idx, uint16_t value)
{
    EM_BLE_WR(BLE_THRCNTL_RATECNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, value);
}

// field definitions
#define BLE_RXTHR_MASK    ((uint16_t)0x0000F000)
#define BLE_RXTHR_LSB     12
#define BLE_RXTHR_WIDTH   ((uint16_t)0x00000004)
#define BLE_RXRATE_MASK   ((uint16_t)0x0000000C)
#define BLE_RXRATE_LSB    2
#define BLE_RXRATE_WIDTH  ((uint16_t)0x00000002)
#define BLE_TXRATE_MASK   ((uint16_t)0x00000003)
#define BLE_TXRATE_LSB    0
#define BLE_TXRATE_WIDTH  ((uint16_t)0x00000002)

#define BLE_RXTHR_RST     0x0
#define BLE_RXRATE_RST    0x0
#define BLE_TXRATE_RST    0x0

__INLINE void ble_thrcntl_ratecntl_pack(int elt_idx, uint8_t rxthr, uint8_t rxrate, uint8_t txrate)
{
    ASSERT_ERR((((uint16_t)rxthr << 12) & ~((uint16_t)0x0000F000)) == 0);
    ASSERT_ERR((((uint16_t)rxrate << 2) & ~((uint16_t)0x0000000C)) == 0);
    ASSERT_ERR((((uint16_t)txrate << 0) & ~((uint16_t)0x00000003)) == 0);
    EM_BLE_WR(BLE_THRCNTL_RATECNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE,  ((uint16_t)rxthr << 12) | ((uint16_t)rxrate << 2) | ((uint16_t)txrate << 0));
}

__INLINE void ble_thrcntl_ratecntl_unpack(int elt_idx, uint8_t* rxthr, uint8_t* rxrate, uint8_t* txrate)
{
    uint16_t localVal = EM_BLE_RD(BLE_THRCNTL_RATECNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);

    *rxthr = (localVal & ((uint16_t)0x0000F000)) >> 12;
    *rxrate = (localVal & ((uint16_t)0x0000000C)) >> 2;
    *txrate = (localVal & ((uint16_t)0x00000003)) >> 0;
}

__INLINE uint8_t ble_rxthr_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(BLE_THRCNTL_RATECNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
    return ((localVal & ((uint16_t)0x0000F000)) >> 12);
}

__INLINE void ble_rxthr_setf(int elt_idx, uint8_t rxthr)
{
    ASSERT_ERR((((uint16_t)rxthr << 12) & ~((uint16_t)0x0000F000)) == 0);
    EM_BLE_WR(BLE_THRCNTL_RATECNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, (EM_BLE_RD(BLE_THRCNTL_RATECNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE) & ~((uint16_t)0x0000F000)) | ((uint16_t)rxthr << 12));
}

__INLINE uint8_t ble_rxrate_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(BLE_THRCNTL_RATECNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
    return ((localVal & ((uint16_t)0x0000000C)) >> 2);
}

__INLINE void ble_rxrate_setf(int elt_idx, uint8_t rxrate)
{
    ASSERT_ERR((((uint16_t)rxrate << 2) & ~((uint16_t)0x0000000C)) == 0);
    EM_BLE_WR(BLE_THRCNTL_RATECNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, (EM_BLE_RD(BLE_THRCNTL_RATECNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE) & ~((uint16_t)0x0000000C)) | ((uint16_t)rxrate << 2));
}

__INLINE uint8_t ble_txrate_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(BLE_THRCNTL_RATECNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
    return ((localVal & ((uint16_t)0x00000003)) >> 0);
}

__INLINE void ble_txrate_setf(int elt_idx, uint8_t txrate)
{
    ASSERT_ERR((((uint16_t)txrate << 0) & ~((uint16_t)0x00000003)) == 0);
    EM_BLE_WR(BLE_THRCNTL_RATECNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, (EM_BLE_RD(BLE_THRCNTL_RATECNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE) & ~((uint16_t)0x00000003)) | ((uint16_t)txrate << 0));
}

/**
 * @brief SYNCWL register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  15:00            SYNCWORDL   0x0
 * </pre>
 */
#define BLE_SYNCWL_ADDR   (0x00814008 + EM_BLE_CS_OFFSET)
#define BLE_SYNCWL_INDEX  0x00000004
#define BLE_SYNCWL_RESET  0x00000000

__INLINE uint16_t ble_syncwl_get(int elt_idx)
{
    return EM_BLE_RD(BLE_SYNCWL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
}

__INLINE void ble_syncwl_set(int elt_idx, uint16_t value)
{
    EM_BLE_WR(BLE_SYNCWL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, value);
}

// field definitions
#define BLE_SYNCWORDL_MASK   ((uint16_t)0x0000FFFF)
#define BLE_SYNCWORDL_LSB    0
#define BLE_SYNCWORDL_WIDTH  ((uint16_t)0x00000010)

#define BLE_SYNCWORDL_RST    0x0

__INLINE uint16_t ble_syncwordl_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(BLE_SYNCWL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
    ASSERT_ERR((localVal & ~((uint16_t)0x0000FFFF)) == 0);
    return (localVal >> 0);
}

__INLINE void ble_syncwordl_setf(int elt_idx, uint16_t syncwordl)
{
    ASSERT_ERR((((uint16_t)syncwordl << 0) & ~((uint16_t)0x0000FFFF)) == 0);
    EM_BLE_WR(BLE_SYNCWL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, (uint16_t)syncwordl << 0);
}

/**
 * @brief SYNCWH register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  15:00            SYNCWORDH   0x0
 * </pre>
 */
#define BLE_SYNCWH_ADDR   (0x0081400A + EM_BLE_CS_OFFSET)
#define BLE_SYNCWH_INDEX  0x00000005
#define BLE_SYNCWH_RESET  0x00000000

__INLINE uint16_t ble_syncwh_get(int elt_idx)
{
    return EM_BLE_RD(BLE_SYNCWH_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
}

__INLINE void ble_syncwh_set(int elt_idx, uint16_t value)
{
    EM_BLE_WR(BLE_SYNCWH_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, value);
}

// field definitions
#define BLE_SYNCWORDH_MASK   ((uint16_t)0x0000FFFF)
#define BLE_SYNCWORDH_LSB    0
#define BLE_SYNCWORDH_WIDTH  ((uint16_t)0x00000010)

#define BLE_SYNCWORDH_RST    0x0

__INLINE uint16_t ble_syncwordh_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(BLE_SYNCWH_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
    ASSERT_ERR((localVal & ~((uint16_t)0x0000FFFF)) == 0);
    return (localVal >> 0);
}

__INLINE void ble_syncwordh_setf(int elt_idx, uint16_t syncwordh)
{
    ASSERT_ERR((((uint16_t)syncwordh << 0) & ~((uint16_t)0x0000FFFF)) == 0);
    EM_BLE_WR(BLE_SYNCWH_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, (uint16_t)syncwordh << 0);
}

/**
 * @brief CRCINIT0 register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  15:00             CRCINIT0   0x0
 * </pre>
 */
#define BLE_CRCINIT0_ADDR   (0x0081400C + EM_BLE_CS_OFFSET)
#define BLE_CRCINIT0_INDEX  0x00000006
#define BLE_CRCINIT0_RESET  0x00000000

__INLINE uint16_t ble_crcinit0_get(int elt_idx)
{
    return EM_BLE_RD(BLE_CRCINIT0_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
}

__INLINE void ble_crcinit0_set(int elt_idx, uint16_t value)
{
    EM_BLE_WR(BLE_CRCINIT0_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, value);
}

// field definitions
#define BLE_CRCINIT0_MASK   ((uint16_t)0x0000FFFF)
#define BLE_CRCINIT0_LSB    0
#define BLE_CRCINIT0_WIDTH  ((uint16_t)0x00000010)

#define BLE_CRCINIT0_RST    0x0

__INLINE uint16_t ble_crcinit0_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(BLE_CRCINIT0_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
    ASSERT_ERR((localVal & ~((uint16_t)0x0000FFFF)) == 0);
    return (localVal >> 0);
}

__INLINE void ble_crcinit0_setf(int elt_idx, uint16_t crcinit0)
{
    ASSERT_ERR((((uint16_t)crcinit0 << 0) & ~((uint16_t)0x0000FFFF)) == 0);
    EM_BLE_WR(BLE_CRCINIT0_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, (uint16_t)crcinit0 << 0);
}

/**
 * @brief CRCINIT1 register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  07:00             CRCINIT1   0x0
 * </pre>
 */
#define BLE_CRCINIT1_ADDR   (0x0081400E + EM_BLE_CS_OFFSET)
#define BLE_CRCINIT1_INDEX  0x00000007
#define BLE_CRCINIT1_RESET  0x00000000

__INLINE uint16_t ble_crcinit1_get(int elt_idx)
{
    return EM_BLE_RD(BLE_CRCINIT1_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
}

__INLINE void ble_crcinit1_set(int elt_idx, uint16_t value)
{
    EM_BLE_WR(BLE_CRCINIT1_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, value);
}

// field definitions
#define BLE_CRCINIT1_MASK   ((uint16_t)0x000000FF)
#define BLE_CRCINIT1_LSB    0
#define BLE_CRCINIT1_WIDTH  ((uint16_t)0x00000008)

#define BLE_CRCINIT1_RST    0x0

__INLINE uint8_t ble_crcinit1_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(BLE_CRCINIT1_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
    ASSERT_ERR((localVal & ~((uint16_t)0x000000FF)) == 0);
    return (localVal >> 0);
}

__INLINE void ble_crcinit1_setf(int elt_idx, uint8_t crcinit1)
{
    ASSERT_ERR((((uint16_t)crcinit1 << 0) & ~((uint16_t)0x000000FF)) == 0);
    EM_BLE_WR(BLE_CRCINIT1_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, (uint16_t)crcinit1 << 0);
}

/**
 * @brief FILTPOL_RALCNTL register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  15:08        FILTER_POLICY   0x0
 *     02        LOCAL_RPA_SEL   0
 *     01             RAL_MODE   0
 *     00               RAL_EN   0
 * </pre>
 */
#define BLE_FILTPOL_RALCNTL_ADDR   (0x00814010 + EM_BLE_CS_OFFSET)
#define BLE_FILTPOL_RALCNTL_INDEX  0x00000008
#define BLE_FILTPOL_RALCNTL_RESET  0x00000000

__INLINE uint16_t ble_filtpol_ralcntl_get(int elt_idx)
{
    return EM_BLE_RD(BLE_FILTPOL_RALCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
}

__INLINE void ble_filtpol_ralcntl_set(int elt_idx, uint16_t value)
{
    EM_BLE_WR(BLE_FILTPOL_RALCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, value);
}

// field definitions
#define BLE_FILTER_POLICY_MASK   ((uint16_t)0x0000FF00)
#define BLE_FILTER_POLICY_LSB    8
#define BLE_FILTER_POLICY_WIDTH  ((uint16_t)0x00000008)
#define BLE_LOCAL_RPA_SEL_BIT    ((uint16_t)0x00000004)
#define BLE_LOCAL_RPA_SEL_POS    2
#define BLE_RAL_MODE_BIT         ((uint16_t)0x00000002)
#define BLE_RAL_MODE_POS         1
#define BLE_RAL_EN_BIT           ((uint16_t)0x00000001)
#define BLE_RAL_EN_POS           0

#define BLE_FILTER_POLICY_RST    0x0
#define BLE_LOCAL_RPA_SEL_RST    0x0
#define BLE_RAL_MODE_RST         0x0
#define BLE_RAL_EN_RST           0x0

__INLINE void ble_filtpol_ralcntl_pack(int elt_idx, uint8_t filterpolicy, uint8_t localrpasel, uint8_t ralmode, uint8_t ralen)
{
    ASSERT_ERR((((uint16_t)filterpolicy << 8) & ~((uint16_t)0x0000FF00)) == 0);
    ASSERT_ERR((((uint16_t)localrpasel << 2) & ~((uint16_t)0x00000004)) == 0);
    ASSERT_ERR((((uint16_t)ralmode << 1) & ~((uint16_t)0x00000002)) == 0);
    ASSERT_ERR((((uint16_t)ralen << 0) & ~((uint16_t)0x00000001)) == 0);
    EM_BLE_WR(BLE_FILTPOL_RALCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE,  ((uint16_t)filterpolicy << 8) | ((uint16_t)localrpasel << 2) | ((uint16_t)ralmode << 1) | ((uint16_t)ralen << 0));
}

__INLINE void ble_filtpol_ralcntl_unpack(int elt_idx, uint8_t* filterpolicy, uint8_t* localrpasel, uint8_t* ralmode, uint8_t* ralen)
{
    uint16_t localVal = EM_BLE_RD(BLE_FILTPOL_RALCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);

    *filterpolicy = (localVal & ((uint16_t)0x0000FF00)) >> 8;
    *localrpasel = (localVal & ((uint16_t)0x00000004)) >> 2;
    *ralmode = (localVal & ((uint16_t)0x00000002)) >> 1;
    *ralen = (localVal & ((uint16_t)0x00000001)) >> 0;
}

__INLINE uint8_t ble_filter_policy_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(BLE_FILTPOL_RALCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
    return ((localVal & ((uint16_t)0x0000FF00)) >> 8);
}

__INLINE void ble_filter_policy_setf(int elt_idx, uint8_t filterpolicy)
{
    ASSERT_ERR((((uint16_t)filterpolicy << 8) & ~((uint16_t)0x0000FF00)) == 0);
    EM_BLE_WR(BLE_FILTPOL_RALCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, (EM_BLE_RD(BLE_FILTPOL_RALCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE) & ~((uint16_t)0x0000FF00)) | ((uint16_t)filterpolicy << 8));
}

__INLINE uint8_t ble_local_rpa_sel_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(BLE_FILTPOL_RALCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
    return ((localVal & ((uint16_t)0x00000004)) >> 2);
}

__INLINE void ble_local_rpa_sel_setf(int elt_idx, uint8_t localrpasel)
{
    ASSERT_ERR((((uint16_t)localrpasel << 2) & ~((uint16_t)0x00000004)) == 0);
    EM_BLE_WR(BLE_FILTPOL_RALCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, (EM_BLE_RD(BLE_FILTPOL_RALCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE) & ~((uint16_t)0x00000004)) | ((uint16_t)localrpasel << 2));
}

__INLINE uint8_t ble_ral_mode_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(BLE_FILTPOL_RALCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
    return ((localVal & ((uint16_t)0x00000002)) >> 1);
}

__INLINE void ble_ral_mode_setf(int elt_idx, uint8_t ralmode)
{
    ASSERT_ERR((((uint16_t)ralmode << 1) & ~((uint16_t)0x00000002)) == 0);
    EM_BLE_WR(BLE_FILTPOL_RALCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, (EM_BLE_RD(BLE_FILTPOL_RALCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE) & ~((uint16_t)0x00000002)) | ((uint16_t)ralmode << 1));
}

__INLINE uint8_t ble_ral_en_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(BLE_FILTPOL_RALCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
    return ((localVal & ((uint16_t)0x00000001)) >> 0);
}

__INLINE void ble_ral_en_setf(int elt_idx, uint8_t ralen)
{
    ASSERT_ERR((((uint16_t)ralen << 0) & ~((uint16_t)0x00000001)) == 0);
    EM_BLE_WR(BLE_FILTPOL_RALCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, (EM_BLE_RD(BLE_FILTPOL_RALCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE) & ~((uint16_t)0x00000001)) | ((uint16_t)ralen << 0));
}

/**
 * @brief HOPCNTL register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     15                FH_EN   0
 *  12:08              HOP_INT   0x0
 *  05:00               CH_IDX   0x0
 * </pre>
 */
#define BLE_HOPCNTL_ADDR   (0x00814012 + EM_BLE_CS_OFFSET)
#define BLE_HOPCNTL_INDEX  0x00000009
#define BLE_HOPCNTL_RESET  0x00000000

__INLINE uint16_t ble_hopcntl_get(int elt_idx)
{
    return EM_BLE_RD(BLE_HOPCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
}

__INLINE void ble_hopcntl_set(int elt_idx, uint16_t value)
{
    EM_BLE_WR(BLE_HOPCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, value);
}

// field definitions
#define BLE_FH_EN_BIT      ((uint16_t)0x00008000)
#define BLE_FH_EN_POS      15
#define BLE_HOP_INT_MASK   ((uint16_t)0x00001F00)
#define BLE_HOP_INT_LSB    8
#define BLE_HOP_INT_WIDTH  ((uint16_t)0x00000005)
#define BLE_CH_IDX_MASK    ((uint16_t)0x0000003F)
#define BLE_CH_IDX_LSB     0
#define BLE_CH_IDX_WIDTH   ((uint16_t)0x00000006)

#define BLE_FH_EN_RST      0x0
#define BLE_HOP_INT_RST    0x0
#define BLE_CH_IDX_RST     0x0

__INLINE void ble_hopcntl_pack(int elt_idx, uint8_t fhen, uint8_t hopint, uint8_t chidx)
{
    ASSERT_ERR((((uint16_t)fhen << 15) & ~((uint16_t)0x00008000)) == 0);
    ASSERT_ERR((((uint16_t)hopint << 8) & ~((uint16_t)0x00001F00)) == 0);
    ASSERT_ERR((((uint16_t)chidx << 0) & ~((uint16_t)0x0000003F)) == 0);
    EM_BLE_WR(BLE_HOPCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE,  ((uint16_t)fhen << 15) | ((uint16_t)hopint << 8) | ((uint16_t)chidx << 0));
}

__INLINE void ble_hopcntl_unpack(int elt_idx, uint8_t* fhen, uint8_t* hopint, uint8_t* chidx)
{
    uint16_t localVal = EM_BLE_RD(BLE_HOPCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);

    *fhen = (localVal & ((uint16_t)0x00008000)) >> 15;
    *hopint = (localVal & ((uint16_t)0x00001F00)) >> 8;
    *chidx = (localVal & ((uint16_t)0x0000003F)) >> 0;
}

__INLINE uint8_t ble_fh_en_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(BLE_HOPCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
    return ((localVal & ((uint16_t)0x00008000)) >> 15);
}

__INLINE void ble_fh_en_setf(int elt_idx, uint8_t fhen)
{
    ASSERT_ERR((((uint16_t)fhen << 15) & ~((uint16_t)0x00008000)) == 0);
    EM_BLE_WR(BLE_HOPCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, (EM_BLE_RD(BLE_HOPCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE) & ~((uint16_t)0x00008000)) | ((uint16_t)fhen << 15));
}

__INLINE uint8_t ble_hop_int_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(BLE_HOPCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
    return ((localVal & ((uint16_t)0x00001F00)) >> 8);
}

__INLINE void ble_hop_int_setf(int elt_idx, uint8_t hopint)
{
    ASSERT_ERR((((uint16_t)hopint << 8) & ~((uint16_t)0x00001F00)) == 0);
    EM_BLE_WR(BLE_HOPCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, (EM_BLE_RD(BLE_HOPCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE) & ~((uint16_t)0x00001F00)) | ((uint16_t)hopint << 8));
}

__INLINE uint8_t ble_ch_idx_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(BLE_HOPCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
    return ((localVal & ((uint16_t)0x0000003F)) >> 0);
}

__INLINE void ble_ch_idx_setf(int elt_idx, uint8_t chidx)
{
    ASSERT_ERR((((uint16_t)chidx << 0) & ~((uint16_t)0x0000003F)) == 0);
    EM_BLE_WR(BLE_HOPCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, (EM_BLE_RD(BLE_HOPCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE) & ~((uint16_t)0x0000003F)) | ((uint16_t)chidx << 0));
}

/**
 * @brief TXRXCNTL register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     15          RXBUFF_FULL   0
 *     14            LASTEMPTY   0
 *     13                   SN   0
 *     12                 NESN   0
 *     11           RXBFMICERR   0
 *  07:00                TXPWR   0x0
 * </pre>
 */
#define BLE_TXRXCNTL_ADDR   (0x00814014 + EM_BLE_CS_OFFSET)
#define BLE_TXRXCNTL_INDEX  0x0000000A
#define BLE_TXRXCNTL_RESET  0x00000000

__INLINE uint16_t ble_txrxcntl_get(int elt_idx)
{
    return EM_BLE_RD(BLE_TXRXCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
}

__INLINE void ble_txrxcntl_set(int elt_idx, uint16_t value)
{
    EM_BLE_WR(BLE_TXRXCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, value);
}

// field definitions
#define BLE_RXBUFF_FULL_BIT    ((uint16_t)0x00008000)
#define BLE_RXBUFF_FULL_POS    15
#define BLE_LASTEMPTY_BIT      ((uint16_t)0x00004000)
#define BLE_LASTEMPTY_POS      14
#define BLE_SN_BIT             ((uint16_t)0x00002000)
#define BLE_SN_POS             13
#define BLE_NESN_BIT           ((uint16_t)0x00001000)
#define BLE_NESN_POS           12
#define BLE_RXBFMICERR_BIT     ((uint16_t)0x00000800)
#define BLE_RXBFMICERR_POS     11
#define BLE_TXPWR_MASK         ((uint16_t)0x000000FF)
#define BLE_TXPWR_LSB          0
#define BLE_TXPWR_WIDTH        ((uint16_t)0x00000008)

#define BLE_RXBUFF_FULL_RST    0x0
#define BLE_LASTEMPTY_RST      0x0
#define BLE_SN_RST             0x0
#define BLE_NESN_RST           0x0
#define BLE_RXBFMICERR_RST     0x0
#define BLE_TXPWR_RST          0x0

__INLINE void ble_txrxcntl_pack(int elt_idx, uint8_t rxbufffull, uint8_t lastempty, uint8_t sn, uint8_t nesn, uint8_t txpwr)
{
    ASSERT_ERR((((uint16_t)rxbufffull << 15) & ~((uint16_t)0x00008000)) == 0);
    ASSERT_ERR((((uint16_t)lastempty << 14) & ~((uint16_t)0x00004000)) == 0);
    ASSERT_ERR((((uint16_t)sn << 13) & ~((uint16_t)0x00002000)) == 0);
    ASSERT_ERR((((uint16_t)nesn << 12) & ~((uint16_t)0x00001000)) == 0);
    ASSERT_ERR((((uint16_t)txpwr << 0) & ~((uint16_t)0x000000FF)) == 0);
    EM_BLE_WR(BLE_TXRXCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE,  ((uint16_t)rxbufffull << 15) | ((uint16_t)lastempty << 14) | ((uint16_t)sn << 13) | ((uint16_t)nesn << 12) | ((uint16_t)txpwr << 0));
}

__INLINE void ble_txrxcntl_unpack(int elt_idx, uint8_t* rxbufffull, uint8_t* lastempty, uint8_t* sn, uint8_t* nesn, uint8_t* rxbfmicerr, uint8_t* txpwr)
{
    uint16_t localVal = EM_BLE_RD(BLE_TXRXCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);

    *rxbufffull = (localVal & ((uint16_t)0x00008000)) >> 15;
    *lastempty = (localVal & ((uint16_t)0x00004000)) >> 14;
    *sn = (localVal & ((uint16_t)0x00002000)) >> 13;
    *nesn = (localVal & ((uint16_t)0x00001000)) >> 12;
    *rxbfmicerr = (localVal & ((uint16_t)0x00000800)) >> 11;
    *txpwr = (localVal & ((uint16_t)0x000000FF)) >> 0;
}

__INLINE uint8_t ble_rxbuff_full_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(BLE_TXRXCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
    return ((localVal & ((uint16_t)0x00008000)) >> 15);
}

__INLINE void ble_rxbuff_full_setf(int elt_idx, uint8_t rxbufffull)
{
    ASSERT_ERR((((uint16_t)rxbufffull << 15) & ~((uint16_t)0x00008000)) == 0);
    EM_BLE_WR(BLE_TXRXCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, (EM_BLE_RD(BLE_TXRXCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE) & ~((uint16_t)0x00008000)) | ((uint16_t)rxbufffull << 15));
}

__INLINE uint8_t ble_lastempty_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(BLE_TXRXCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
    return ((localVal & ((uint16_t)0x00004000)) >> 14);
}

__INLINE void ble_lastempty_setf(int elt_idx, uint8_t lastempty)
{
    ASSERT_ERR((((uint16_t)lastempty << 14) & ~((uint16_t)0x00004000)) == 0);
    EM_BLE_WR(BLE_TXRXCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, (EM_BLE_RD(BLE_TXRXCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE) & ~((uint16_t)0x00004000)) | ((uint16_t)lastempty << 14));
}

__INLINE uint8_t ble_sn_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(BLE_TXRXCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
    return ((localVal & ((uint16_t)0x00002000)) >> 13);
}

__INLINE void ble_sn_setf(int elt_idx, uint8_t sn)
{
    ASSERT_ERR((((uint16_t)sn << 13) & ~((uint16_t)0x00002000)) == 0);
    EM_BLE_WR(BLE_TXRXCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, (EM_BLE_RD(BLE_TXRXCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE) & ~((uint16_t)0x00002000)) | ((uint16_t)sn << 13));
}

__INLINE uint8_t ble_nesn_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(BLE_TXRXCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
    return ((localVal & ((uint16_t)0x00001000)) >> 12);
}

__INLINE void ble_nesn_setf(int elt_idx, uint8_t nesn)
{
    ASSERT_ERR((((uint16_t)nesn << 12) & ~((uint16_t)0x00001000)) == 0);
    EM_BLE_WR(BLE_TXRXCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, (EM_BLE_RD(BLE_TXRXCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE) & ~((uint16_t)0x00001000)) | ((uint16_t)nesn << 12));
}

__INLINE uint8_t ble_rxbfmicerr_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(BLE_TXRXCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
    return ((localVal & ((uint16_t)0x00000800)) >> 11);
}

__INLINE uint8_t ble_txpwr_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(BLE_TXRXCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
    return ((localVal & ((uint16_t)0x000000FF)) >> 0);
}

__INLINE void ble_txpwr_setf(int elt_idx, uint8_t txpwr)
{
    ASSERT_ERR((((uint16_t)txpwr << 0) & ~((uint16_t)0x000000FF)) == 0);
    EM_BLE_WR(BLE_TXRXCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, (EM_BLE_RD(BLE_TXRXCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE) & ~((uint16_t)0x000000FF)) | ((uint16_t)txpwr << 0));
}

/**
 * @brief RXWINCNTL register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     15               RXWIDE   0
 *  13:00              RXWINSZ   0x0
 * </pre>
 */
#define BLE_RXWINCNTL_ADDR   (0x00814016 + EM_BLE_CS_OFFSET)
#define BLE_RXWINCNTL_INDEX  0x0000000B
#define BLE_RXWINCNTL_RESET  0x00000000

__INLINE uint16_t ble_rxwincntl_get(int elt_idx)
{
    return EM_BLE_RD(BLE_RXWINCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
}

__INLINE void ble_rxwincntl_set(int elt_idx, uint16_t value)
{
    EM_BLE_WR(BLE_RXWINCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, value);
}

// field definitions
#define BLE_RXWIDE_BIT     ((uint16_t)0x00008000)
#define BLE_RXWIDE_POS     15
#define BLE_RXWINSZ_MASK   ((uint16_t)0x00001F)//((uint16_t)0x00003FFF)
#define BLE_RXWINSZ_LSB    0
#define BLE_RXWINSZ_WIDTH  ((uint16_t)0x0000000E)

#define BLE_RXWIDE_RST     0x0
#define BLE_RXWINSZ_RST    0x0

__INLINE void ble_rxwincntl_pack(int elt_idx, uint8_t rxwide, uint16_t rxwinsz)
{
    ASSERT_ERR((((uint16_t)rxwide << 15) & ~((uint16_t)0x00008000)) == 0);
    ASSERT_ERR((((uint16_t)rxwinsz << 0) & ~((uint16_t)0x00003FFF)) == 0);
    EM_BLE_WR(BLE_RXWINCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE,  ((uint16_t)rxwide << 15) | ((uint16_t)rxwinsz << 0));
}

__INLINE void ble_rxwincntl_unpack(int elt_idx, uint8_t* rxwide, uint16_t* rxwinsz)
{
    uint16_t localVal = EM_BLE_RD(BLE_RXWINCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);

    *rxwide = (localVal & ((uint16_t)0x00008000)) >> 15;
    *rxwinsz = (localVal & ((uint16_t)0x00003FFF)) >> 0;
}

__INLINE uint8_t ble_rxwide_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(BLE_RXWINCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
    return ((localVal & ((uint16_t)0x00008000)) >> 15);
}

__INLINE void ble_rxwide_setf(int elt_idx, uint8_t rxwide)
{
    ASSERT_ERR((((uint16_t)rxwide << 15) & ~((uint16_t)0x00008000)) == 0);
    EM_BLE_WR(BLE_RXWINCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, (EM_BLE_RD(BLE_RXWINCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE) & ~((uint16_t)0x00008000)) | ((uint16_t)rxwide << 15));
}

__INLINE uint16_t ble_rxwinsz_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(BLE_RXWINCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
    return ((localVal & ((uint16_t)0x00003FFF)) >> 0);
}

__INLINE void ble_rxwinsz_setf(int elt_idx, uint16_t rxwinsz)
{
    ASSERT_ERR((((uint16_t)rxwinsz << 0) & ~((uint16_t)0x00003FFF)) == 0);
    EM_BLE_WR(BLE_RXWINCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, (EM_BLE_RD(BLE_RXWINCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE) & ~((uint16_t)0x00003FFF)) | ((uint16_t)rxwinsz << 0));
}

/**
 * @brief TXDESCPTR register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  14:00            TXDESCPTR   0x0
 * </pre>
 */
#define BLE_TXDESCPTR_ADDR   (0x00814018 + EM_BLE_CS_OFFSET)
#define BLE_TXDESCPTR_INDEX  0x0000000C
#define BLE_TXDESCPTR_RESET  0x00000000

__INLINE uint16_t ble_txdescptr_get(int elt_idx)
{
    return EM_BLE_RD(BLE_TXDESCPTR_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
}

__INLINE void ble_txdescptr_set(int elt_idx, uint16_t value)
{
    EM_BLE_WR(BLE_TXDESCPTR_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, value);
}

// field definitions
#define BLE_TXDESCPTR_MASK   ((uint16_t)0x00007FFF)
#define BLE_TXDESCPTR_LSB    0
#define BLE_TXDESCPTR_WIDTH  ((uint16_t)0x0000000F)

#define BLE_TXDESCPTR_RST    0x0

__INLINE uint16_t ble_txdescptr_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(BLE_TXDESCPTR_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
    ASSERT_ERR((localVal & ~((uint16_t)0x00007FFF)) == 0);
    return (localVal >> 0);
}

__INLINE void ble_txdescptr_setf(int elt_idx, uint16_t txdescptr)
{
    ASSERT_ERR((((uint16_t)txdescptr << 0) & ~((uint16_t)0x00007FFF)) == 0);
    EM_BLE_WR(BLE_TXDESCPTR_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, (uint16_t)txdescptr << 0);
}

/**
 * @brief WINOFFSET register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  15:00            WINOFFSET   0x0
 * </pre>
 */
#define BLE_WINOFFSET_ADDR   (0x0081401A + EM_BLE_CS_OFFSET)
#define BLE_WINOFFSET_INDEX  0x0000000D
#define BLE_WINOFFSET_RESET  0x00000000

__INLINE uint16_t ble_winoffset_get(int elt_idx)
{
    return EM_BLE_RD(BLE_WINOFFSET_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
}

__INLINE void ble_winoffset_set(int elt_idx, uint16_t value)
{
    EM_BLE_WR(BLE_WINOFFSET_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, value);
}

// field definitions
#define BLE_WINOFFSET_MASK   ((uint16_t)0x0000FFFF)
#define BLE_WINOFFSET_LSB    0
#define BLE_WINOFFSET_WIDTH  ((uint16_t)0x00000010)

#define BLE_WINOFFSET_RST    0x0

__INLINE uint16_t ble_winoffset_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(BLE_WINOFFSET_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
    ASSERT_ERR((localVal & ~((uint16_t)0x0000FFFF)) == 0);
    return (localVal >> 0);
}

__INLINE void ble_winoffset_setf(int elt_idx, uint16_t winoffset)
{
    ASSERT_ERR((((uint16_t)winoffset << 0) & ~((uint16_t)0x0000FFFF)) == 0);
    EM_BLE_WR(BLE_WINOFFSET_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, (uint16_t)winoffset << 0);
}

/**
 * @brief MINEVTIME register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  15:00            MINEVTIME   0x0
 * </pre>
 */
#define BLE_MINEVTIME_ADDR   (0x0081401A + EM_BLE_CS_OFFSET)
#define BLE_MINEVTIME_INDEX  0x0000000D
#define BLE_MINEVTIME_RESET  0x00000000

__INLINE uint16_t ble_minevtime_get(int elt_idx)
{
    return EM_BLE_RD(BLE_MINEVTIME_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
}

__INLINE void ble_minevtime_set(int elt_idx, uint16_t value)
{
    EM_BLE_WR(BLE_MINEVTIME_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, value);
}

// field definitions
#define BLE_MINEVTIME_MASK   ((uint16_t)0x0000FFFF)
#define BLE_MINEVTIME_LSB    0
#define BLE_MINEVTIME_WIDTH  ((uint16_t)0x00000010)

#define BLE_MINEVTIME_RST    0x0

__INLINE uint16_t ble_minevtime_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(BLE_MINEVTIME_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
    ASSERT_ERR((localVal & ~((uint16_t)0x0000FFFF)) == 0);
    return (localVal >> 0);
}

__INLINE void ble_minevtime_setf(int elt_idx, uint16_t minevtime)
{
    ASSERT_ERR((((uint16_t)minevtime << 0) & ~((uint16_t)0x0000FFFF)) == 0);
    EM_BLE_WR(BLE_MINEVTIME_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, (uint16_t)minevtime << 0);
}

/**
 * @brief MAXEVTIME register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  15:00            MAXEVTIME   0x0
 * </pre>
 */
#define BLE_MAXEVTIME_ADDR   (0x0081401C + EM_BLE_CS_OFFSET)
#define BLE_MAXEVTIME_INDEX  0x0000000E
#define BLE_MAXEVTIME_RESET  0x00000000

__INLINE uint16_t ble_maxevtime_get(int elt_idx)
{
    return EM_BLE_RD(BLE_MAXEVTIME_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
}

__INLINE void ble_maxevtime_set(int elt_idx, uint16_t value)
{
    EM_BLE_WR(BLE_MAXEVTIME_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, value);
}

// field definitions
#define BLE_MAXEVTIME_MASK   ((uint16_t)0x0000FFFF)
#define BLE_MAXEVTIME_LSB    0
#define BLE_MAXEVTIME_WIDTH  ((uint16_t)0x00000010)

#define BLE_MAXEVTIME_RST    0x0

__INLINE uint16_t ble_maxevtime_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(BLE_MAXEVTIME_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
    ASSERT_ERR((localVal & ~((uint16_t)0x0000FFFF)) == 0);
    return (localVal >> 0);
}

__INLINE void ble_maxevtime_setf(int elt_idx, uint16_t maxevtime)
{
    ASSERT_ERR((((uint16_t)maxevtime << 0) & ~((uint16_t)0x0000FFFF)) == 0);
    EM_BLE_WR(BLE_MAXEVTIME_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, (uint16_t)maxevtime << 0);
}

/**
 * @brief CONNINTERVAL register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  15:00         CONNINTERVAL   0x0
 * </pre>
 */
#define BLE_CONNINTERVAL_ADDR   (0x0081401E + EM_BLE_CS_OFFSET)
#define BLE_CONNINTERVAL_INDEX  0x0000000F
#define BLE_CONNINTERVAL_RESET  0x00000000

__INLINE uint16_t ble_conninterval_get(int elt_idx)
{
    return EM_BLE_RD(BLE_CONNINTERVAL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
}

__INLINE void ble_conninterval_set(int elt_idx, uint16_t value)
{
    EM_BLE_WR(BLE_CONNINTERVAL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, value);
}

// field definitions
#define BLE_CONNINTERVAL_MASK   ((uint16_t)0x0000FFFF)
#define BLE_CONNINTERVAL_LSB    0
#define BLE_CONNINTERVAL_WIDTH  ((uint16_t)0x00000010)

#define BLE_CONNINTERVAL_RST    0x0

__INLINE uint16_t ble_conninterval_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(BLE_CONNINTERVAL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
    ASSERT_ERR((localVal & ~((uint16_t)0x0000FFFF)) == 0);
    return (localVal >> 0);
}

__INLINE void ble_conninterval_setf(int elt_idx, uint16_t conninterval)
{
    ASSERT_ERR((((uint16_t)conninterval << 0) & ~((uint16_t)0x0000FFFF)) == 0);
    EM_BLE_WR(BLE_CONNINTERVAL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, (uint16_t)conninterval << 0);
}

/**
 * @brief CHMAP0 register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  15:00             LLCHMAP0   0xFFFF
 * </pre>
 */
#define BLE_CHMAP0_ADDR   (0x0081401E + EM_BLE_CS_OFFSET)
#define BLE_CHMAP0_INDEX  0x0000000F
#define BLE_CHMAP0_RESET  0x0000FFFF

__INLINE uint16_t ble_chmap0_get(int elt_idx)
{
    return EM_BLE_RD(BLE_CHMAP0_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
}

__INLINE void ble_chmap0_set(int elt_idx, uint16_t value)
{
    EM_BLE_WR(BLE_CHMAP0_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, value);
}

// field definitions
#define BLE_LLCHMAP0_MASK   ((uint16_t)0x0000FFFF)
#define BLE_LLCHMAP0_LSB    0
#define BLE_LLCHMAP0_WIDTH  ((uint16_t)0x00000010)

#define BLE_LLCHMAP0_RST    0xFFFF

__INLINE uint16_t ble_llchmap0_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(BLE_CHMAP0_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
    ASSERT_ERR((localVal & ~((uint16_t)0x0000FFFF)) == 0);
    return (localVal >> 0);
}

__INLINE void ble_llchmap0_setf(int elt_idx, uint16_t llchmap0)
{
    ASSERT_ERR((((uint16_t)llchmap0 << 0) & ~((uint16_t)0x0000FFFF)) == 0);
    EM_BLE_WR(BLE_CHMAP0_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, (uint16_t)llchmap0 << 0);
}

/**
 * @brief CHMAP1 register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  15:00             LLCHMAP1   0xFFFF
 * </pre>
 */
#define BLE_CHMAP1_ADDR   (0x00814020 + EM_BLE_CS_OFFSET)
#define BLE_CHMAP1_INDEX  0x00000010
#define BLE_CHMAP1_RESET  0x0000FFFF

__INLINE uint16_t ble_chmap1_get(int elt_idx)
{
    return EM_BLE_RD(BLE_CHMAP1_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
}

__INLINE void ble_chmap1_set(int elt_idx, uint16_t value)
{
    EM_BLE_WR(BLE_CHMAP1_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, value);
}

// field definitions
#define BLE_LLCHMAP1_MASK   ((uint16_t)0x0000FFFF)
#define BLE_LLCHMAP1_LSB    0
#define BLE_LLCHMAP1_WIDTH  ((uint16_t)0x00000010)

#define BLE_LLCHMAP1_RST    0xFFFF

__INLINE uint16_t ble_llchmap1_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(BLE_CHMAP1_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
    ASSERT_ERR((localVal & ~((uint16_t)0x0000FFFF)) == 0);
    return (localVal >> 0);
}

__INLINE void ble_llchmap1_setf(int elt_idx, uint16_t llchmap1)
{
    ASSERT_ERR((((uint16_t)llchmap1 << 0) & ~((uint16_t)0x0000FFFF)) == 0);
    EM_BLE_WR(BLE_CHMAP1_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, (uint16_t)llchmap1 << 0);
}

/**
 * @brief CHMAP2 register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  13:08             NBCHGOOD   0x25
 *  04:00             LLCHMAP3   0x1F
 * </pre>
 */
#define BLE_CHMAP2_ADDR   (0x00814022 + EM_BLE_CS_OFFSET)
#define BLE_CHMAP2_INDEX  0x00000011
#define BLE_CHMAP2_RESET  0x0000251F

__INLINE uint16_t ble_chmap2_get(int elt_idx)
{
    return EM_BLE_RD(BLE_CHMAP2_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
}

__INLINE void ble_chmap2_set(int elt_idx, uint16_t value)
{
    EM_BLE_WR(BLE_CHMAP2_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, value);
}

// field definitions
#define BLE_NBCHGOOD_MASK   ((uint16_t)0x00003F00)
#define BLE_NBCHGOOD_LSB    8
#define BLE_NBCHGOOD_WIDTH  ((uint16_t)0x00000006)
#define BLE_LLCHMAP3_MASK   ((uint16_t)0x0000001F)
#define BLE_LLCHMAP3_LSB    0
#define BLE_LLCHMAP3_WIDTH  ((uint16_t)0x00000005)

#define BLE_NBCHGOOD_RST    0x25
#define BLE_LLCHMAP3_RST    0x1F

__INLINE void ble_chmap2_pack(int elt_idx, uint8_t nbchgood, uint8_t llchmap3)
{
    ASSERT_ERR((((uint16_t)nbchgood << 8) & ~((uint16_t)0x00003F00)) == 0);
    ASSERT_ERR((((uint16_t)llchmap3 << 0) & ~((uint16_t)0x0000001F)) == 0);
    EM_BLE_WR(BLE_CHMAP2_ADDR + elt_idx * REG_BLE_EM_CS_SIZE,  ((uint16_t)nbchgood << 8) | ((uint16_t)llchmap3 << 0));
}

__INLINE void ble_chmap2_unpack(int elt_idx, uint8_t* nbchgood, uint8_t* llchmap3)
{
    uint16_t localVal = EM_BLE_RD(BLE_CHMAP2_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);

    *nbchgood = (localVal & ((uint16_t)0x00003F00)) >> 8;
    *llchmap3 = (localVal & ((uint16_t)0x0000001F)) >> 0;
}

__INLINE uint8_t ble_nbchgood_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(BLE_CHMAP2_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
    return ((localVal & ((uint16_t)0x00003F00)) >> 8);
}

__INLINE void ble_nbchgood_setf(int elt_idx, uint8_t nbchgood)
{
    ASSERT_ERR((((uint16_t)nbchgood << 8) & ~((uint16_t)0x00003F00)) == 0);
    EM_BLE_WR(BLE_CHMAP2_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, (EM_BLE_RD(BLE_CHMAP2_ADDR + elt_idx * REG_BLE_EM_CS_SIZE) & ~((uint16_t)0x00003F00)) | ((uint16_t)nbchgood << 8));
}

__INLINE uint8_t ble_llchmap3_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(BLE_CHMAP2_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
    return ((localVal & ((uint16_t)0x0000001F)) >> 0);
}

__INLINE void ble_llchmap3_setf(int elt_idx, uint8_t llchmap3)
{
    ASSERT_ERR((((uint16_t)llchmap3 << 0) & ~((uint16_t)0x0000001F)) == 0);
    EM_BLE_WR(BLE_CHMAP2_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, (EM_BLE_RD(BLE_CHMAP2_ADDR + elt_idx * REG_BLE_EM_CS_SIZE) & ~((uint16_t)0x0000001F)) | ((uint16_t)llchmap3 << 0));
}

/**
 * @brief RXMAXBUF register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  07:00             RXMAXBUF   0x0
 * </pre>
 */
#define BLE_RXMAXBUF_ADDR   (0x00814024 + EM_BLE_CS_OFFSET)
#define BLE_RXMAXBUF_INDEX  0x00000012
#define BLE_RXMAXBUF_RESET  0x00000000

__INLINE uint16_t ble_rxmaxbuf_get(int elt_idx)
{
    return EM_BLE_RD(BLE_RXMAXBUF_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
}

__INLINE void ble_rxmaxbuf_set(int elt_idx, uint16_t value)
{
    EM_BLE_WR(BLE_RXMAXBUF_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, value);
}

// field definitions
#define BLE_RXMAXBUF_MASK   ((uint16_t)0x000000FF)
#define BLE_RXMAXBUF_LSB    0
#define BLE_RXMAXBUF_WIDTH  ((uint16_t)0x00000008)

#define BLE_RXMAXBUF_RST    0x0

__INLINE uint8_t ble_rxmaxbuf_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(BLE_RXMAXBUF_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
    ASSERT_ERR((localVal & ~((uint16_t)0x000000FF)) == 0);
    return (localVal >> 0);
}

__INLINE void ble_rxmaxbuf_setf(int elt_idx, uint8_t rxmaxbuf)
{
    ASSERT_ERR((((uint16_t)rxmaxbuf << 0) & ~((uint16_t)0x000000FF)) == 0);
    EM_BLE_WR(BLE_RXMAXBUF_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, (uint16_t)rxmaxbuf << 0);
}

/**
 * @brief RXMAXTIME register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  12:00            RXMAXTIME   0x0
 * </pre>
 */
#define BLE_RXMAXTIME_ADDR   (0x00814026 + EM_BLE_CS_OFFSET)
#define BLE_RXMAXTIME_INDEX  0x00000013
#define BLE_RXMAXTIME_RESET  0x00000000

__INLINE uint16_t ble_rxmaxtime_get(int elt_idx)
{
    return EM_BLE_RD(BLE_RXMAXTIME_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
}

__INLINE void ble_rxmaxtime_set(int elt_idx, uint16_t value)
{
    EM_BLE_WR(BLE_RXMAXTIME_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, value);
}

// field definitions
#define BLE_RXMAXTIME_MASK   ((uint16_t)0x00001FFF)
#define BLE_RXMAXTIME_LSB    0
#define BLE_RXMAXTIME_WIDTH  ((uint16_t)0x0000000D)

#define BLE_RXMAXTIME_RST    0x0

__INLINE uint16_t ble_rxmaxtime_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(BLE_RXMAXTIME_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
    ASSERT_ERR((localVal & ~((uint16_t)0x00001FFF)) == 0);
    return (localVal >> 0);
}

__INLINE void ble_rxmaxtime_setf(int elt_idx, uint16_t rxmaxtime)
{
    ASSERT_ERR((((uint16_t)rxmaxtime << 0) & ~((uint16_t)0x00001FFF)) == 0);
    EM_BLE_WR(BLE_RXMAXTIME_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, (uint16_t)rxmaxtime << 0);
}

/**
 * @brief SK register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  15:00                   SK   0x0
 * </pre>
 */
#define BLE_SK_ADDR   (0x00814028 + EM_BLE_CS_OFFSET)
#define BLE_SK_INDEX  0x00000014
#define BLE_SK_RESET  0x00000000
#define BLE_SK_COUNT  8

__INLINE uint16_t ble_sk_get(int elt_idx, int reg_idx)
{
    ASSERT_ERR(reg_idx <= 7);
    return EM_BLE_RD(BLE_SK_ADDR + elt_idx * REG_BLE_EM_CS_SIZE + reg_idx * 2);
}

__INLINE void ble_sk_set(int elt_idx, int reg_idx, uint16_t value)
{
    ASSERT_ERR(reg_idx <= 7);
    EM_BLE_WR(BLE_SK_ADDR + elt_idx * REG_BLE_EM_CS_SIZE + reg_idx * 2, value);
}

// field definitions
#define BLE_SK_MASK   ((uint16_t)0x0000FFFF)
#define BLE_SK_LSB    0
#define BLE_SK_WIDTH  ((uint16_t)0x00000010)

#define BLE_SK_RST    0x0

__INLINE uint16_t ble_sk_getf(int elt_idx, int reg_idx)
{
    ASSERT_ERR(reg_idx <= 7);
    uint16_t localVal = EM_BLE_RD(BLE_SK_ADDR + elt_idx * REG_BLE_EM_CS_SIZE + reg_idx * 2);
    ASSERT_ERR((localVal & ~((uint16_t)0x0000FFFF)) == 0);
    return (localVal >> 0);
}

__INLINE void ble_sk_setf(int elt_idx, int reg_idx, uint16_t sk)
{
    ASSERT_ERR(reg_idx <= 7);
    ASSERT_ERR((((uint16_t)sk << 0) & ~((uint16_t)0x0000FFFF)) == 0);
    EM_BLE_WR(BLE_SK_ADDR + elt_idx * REG_BLE_EM_CS_SIZE + reg_idx * 2, (uint16_t)sk << 0);
}

/**
 * @brief ADV_BD_ADDR register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  15:00          ADV_BD_ADDR   0x0
 * </pre>
 */
#define BLE_ADV_BD_ADDR_ADDR   (0x00814028 + EM_BLE_CS_OFFSET)
#define BLE_ADV_BD_ADDR_INDEX  0x00000014
#define BLE_ADV_BD_ADDR_RESET  0x00000000
#define BLE_ADV_BD_ADDR_COUNT  3

__INLINE uint16_t ble_adv_bd_addr_get(int elt_idx, int reg_idx)
{
    ASSERT_ERR(reg_idx <= 2);
    return EM_BLE_RD(BLE_ADV_BD_ADDR_ADDR + elt_idx * REG_BLE_EM_CS_SIZE + reg_idx * 2);
}

__INLINE void ble_adv_bd_addr_set(int elt_idx, int reg_idx, uint16_t value)
{
    ASSERT_ERR(reg_idx <= 2);
    EM_BLE_WR(BLE_ADV_BD_ADDR_ADDR + elt_idx * REG_BLE_EM_CS_SIZE + reg_idx * 2, value);
}

// field definitions
#define BLE_ADV_BD_ADDR_MASK   ((uint16_t)0x0000FFFF)
#define BLE_ADV_BD_ADDR_LSB    0
#define BLE_ADV_BD_ADDR_WIDTH  ((uint16_t)0x00000010)

#define BLE_ADV_BD_ADDR_RST    0x0

__INLINE uint16_t ble_adv_bd_addr_getf(int elt_idx, int reg_idx)
{
    ASSERT_ERR(reg_idx <= 2);
    uint16_t localVal = EM_BLE_RD(BLE_ADV_BD_ADDR_ADDR + elt_idx * REG_BLE_EM_CS_SIZE + reg_idx * 2);
    ASSERT_ERR((localVal & ~((uint16_t)0x0000FFFF)) == 0);
    return (localVal >> 0);
}

__INLINE void ble_adv_bd_addr_setf(int elt_idx, int reg_idx, uint16_t advbdaddr)
{
    ASSERT_ERR(reg_idx <= 2);
    ASSERT_ERR((((uint16_t)advbdaddr << 0) & ~((uint16_t)0x0000FFFF)) == 0);
    EM_BLE_WR(BLE_ADV_BD_ADDR_ADDR + elt_idx * REG_BLE_EM_CS_SIZE + reg_idx * 2, (uint16_t)advbdaddr << 0);
}

/**
 * @brief PEER_RALPTR register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  15:00          PEER_RALPTR   0x0
 * </pre>
 */
#define BLE_PEER_RALPTR_ADDR   (0x00814028 + EM_BLE_CS_OFFSET)
#define BLE_PEER_RALPTR_INDEX  0x00000014
#define BLE_PEER_RALPTR_RESET  0x00000000

__INLINE uint16_t ble_peer_ralptr_get(int elt_idx)
{
    return EM_BLE_RD(BLE_PEER_RALPTR_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
}

__INLINE void ble_peer_ralptr_set(int elt_idx, uint16_t value)
{
    EM_BLE_WR(BLE_PEER_RALPTR_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, value);
}

// field definitions
#define BLE_PEER_RALPTR_MASK   ((uint16_t)0x0000FFFF)
#define BLE_PEER_RALPTR_LSB    0
#define BLE_PEER_RALPTR_WIDTH  ((uint16_t)0x00000010)

#define BLE_PEER_RALPTR_RST    0x0

__INLINE uint16_t ble_peer_ralptr_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(BLE_PEER_RALPTR_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
    ASSERT_ERR((localVal & ~((uint16_t)0x0000FFFF)) == 0);
    return (localVal >> 0);
}

__INLINE void ble_peer_ralptr_setf(int elt_idx, uint16_t peerralptr)
{
    ASSERT_ERR((((uint16_t)peerralptr << 0) & ~((uint16_t)0x0000FFFF)) == 0);
    EM_BLE_WR(BLE_PEER_RALPTR_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, (uint16_t)peerralptr << 0);
}

/**
 * @brief ADV_BD_ADDR_TYPE register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     00     ADV_BD_ADDR_TYPE   0
 * </pre>
 */
#define BLE_ADV_BD_ADDR_TYPE_ADDR   (0x0081402E + EM_BLE_CS_OFFSET)
#define BLE_ADV_BD_ADDR_TYPE_INDEX  0x00000017
#define BLE_ADV_BD_ADDR_TYPE_RESET  0x00000000

__INLINE uint16_t ble_adv_bd_addr_type_get(int elt_idx)
{
    return EM_BLE_RD(BLE_ADV_BD_ADDR_TYPE_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
}

__INLINE void ble_adv_bd_addr_type_set(int elt_idx, uint16_t value)
{
    EM_BLE_WR(BLE_ADV_BD_ADDR_TYPE_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, value);
}

// field definitions
#define BLE_ADV_BD_ADDR_TYPE_BIT    ((uint16_t)0x00000001)
#define BLE_ADV_BD_ADDR_TYPE_POS    0

#define BLE_ADV_BD_ADDR_TYPE_RST    0x0

__INLINE uint8_t ble_adv_bd_addr_type_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(BLE_ADV_BD_ADDR_TYPE_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
    ASSERT_ERR((localVal & ~((uint16_t)0x00000001)) == 0);
    return (localVal >> 0);
}

__INLINE void ble_adv_bd_addr_type_setf(int elt_idx, uint8_t advbdaddrtype)
{
    ASSERT_ERR((((uint16_t)advbdaddrtype << 0) & ~((uint16_t)0x00000001)) == 0);
    EM_BLE_WR(BLE_ADV_BD_ADDR_TYPE_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, (uint16_t)advbdaddrtype << 0);
}

/**
 * @brief IV register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  15:00                   IV   0x0
 * </pre>
 */
#define BLE_IV_ADDR   (0x00814038 + EM_BLE_CS_OFFSET)
#define BLE_IV_INDEX  0x0000001C
#define BLE_IV_RESET  0x00000000
#define BLE_IV_COUNT  4

__INLINE uint16_t ble_iv_get(int elt_idx, int reg_idx)
{
    ASSERT_ERR(reg_idx <= 3);
    return EM_BLE_RD(BLE_IV_ADDR + elt_idx * REG_BLE_EM_CS_SIZE + reg_idx * 2);
}

__INLINE void ble_iv_set(int elt_idx, int reg_idx, uint16_t value)
{
    ASSERT_ERR(reg_idx <= 3);
    EM_BLE_WR(BLE_IV_ADDR + elt_idx * REG_BLE_EM_CS_SIZE + reg_idx * 2, value);
}

// field definitions
#define BLE_IV_MASK   ((uint16_t)0x0000FFFF)
#define BLE_IV_LSB    0
#define BLE_IV_WIDTH  ((uint16_t)0x00000010)

#define BLE_IV_RST    0x0

__INLINE uint16_t ble_iv_getf(int elt_idx, int reg_idx)
{
    ASSERT_ERR(reg_idx <= 3);
    uint16_t localVal = EM_BLE_RD(BLE_IV_ADDR + elt_idx * REG_BLE_EM_CS_SIZE + reg_idx * 2);
    ASSERT_ERR((localVal & ~((uint16_t)0x0000FFFF)) == 0);
    return (localVal >> 0);
}

__INLINE void ble_iv_setf(int elt_idx, int reg_idx, uint16_t iv)
{
    ASSERT_ERR(reg_idx <= 3);
    ASSERT_ERR((((uint16_t)iv << 0) & ~((uint16_t)0x0000FFFF)) == 0);
    EM_BLE_WR(BLE_IV_ADDR + elt_idx * REG_BLE_EM_CS_SIZE + reg_idx * 2, (uint16_t)iv << 0);
}

/**
 * @brief TXWINOFFSET register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  15:00          TXWINOFFSET   0x0
 * </pre>
 */
#define BLE_TXWINOFFSET_ADDR   (0x00814040 + EM_BLE_CS_OFFSET)
#define BLE_TXWINOFFSET_INDEX  0x00000020
#define BLE_TXWINOFFSET_RESET  0x00000000

__INLINE uint16_t ble_txwinoffset_get(int elt_idx)
{
    return EM_BLE_RD(BLE_TXWINOFFSET_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
}

__INLINE void ble_txwinoffset_set(int elt_idx, uint16_t value)
{
    EM_BLE_WR(BLE_TXWINOFFSET_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, value);
}

// field definitions
#define BLE_TXWINOFFSET_MASK   ((uint16_t)0x0000FFFF)
#define BLE_TXWINOFFSET_LSB    0
#define BLE_TXWINOFFSET_WIDTH  ((uint16_t)0x00000010)

#define BLE_TXWINOFFSET_RST    0x0

__INLINE uint16_t ble_txwinoffset_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(BLE_TXWINOFFSET_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
    ASSERT_ERR((localVal & ~((uint16_t)0x0000FFFF)) == 0);
    return (localVal >> 0);
}

__INLINE void ble_txwinoffset_setf(int elt_idx, uint16_t txwinoffset)
{
    ASSERT_ERR((((uint16_t)txwinoffset << 0) & ~((uint16_t)0x0000FFFF)) == 0);
    EM_BLE_WR(BLE_TXWINOFFSET_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, (uint16_t)txwinoffset << 0);
}

/**
 * @brief TXCCMPKTCNT0 register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  15:00         TXCCMPKTCNT0   0x0
 * </pre>
 */
#define BLE_TXCCMPKTCNT0_ADDR   (0x00814040 + EM_BLE_CS_OFFSET)
#define BLE_TXCCMPKTCNT0_INDEX  0x00000020
#define BLE_TXCCMPKTCNT0_RESET  0x00000000

__INLINE uint16_t ble_txccmpktcnt0_get(int elt_idx)
{
    return EM_BLE_RD(BLE_TXCCMPKTCNT0_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
}

__INLINE void ble_txccmpktcnt0_set(int elt_idx, uint16_t value)
{
    EM_BLE_WR(BLE_TXCCMPKTCNT0_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, value);
}

// field definitions
#define BLE_TXCCMPKTCNT0_MASK   ((uint16_t)0x0000FFFF)
#define BLE_TXCCMPKTCNT0_LSB    0
#define BLE_TXCCMPKTCNT0_WIDTH  ((uint16_t)0x00000010)

#define BLE_TXCCMPKTCNT0_RST    0x0

__INLINE uint16_t ble_txccmpktcnt0_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(BLE_TXCCMPKTCNT0_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
    ASSERT_ERR((localVal & ~((uint16_t)0x0000FFFF)) == 0);
    return (localVal >> 0);
}

__INLINE void ble_txccmpktcnt0_setf(int elt_idx, uint16_t txccmpktcnt0)
{
    ASSERT_ERR((((uint16_t)txccmpktcnt0 << 0) & ~((uint16_t)0x0000FFFF)) == 0);
    EM_BLE_WR(BLE_TXCCMPKTCNT0_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, (uint16_t)txccmpktcnt0 << 0);
}

/**
 * @brief TXCCMPKTCNT1 register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  15:00         TXCCMPKTCNT1   0x0
 * </pre>
 */
#define BLE_TXCCMPKTCNT1_ADDR   (0x00814042 + EM_BLE_CS_OFFSET)
#define BLE_TXCCMPKTCNT1_INDEX  0x00000021
#define BLE_TXCCMPKTCNT1_RESET  0x00000000

__INLINE uint16_t ble_txccmpktcnt1_get(int elt_idx)
{
    return EM_BLE_RD(BLE_TXCCMPKTCNT1_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
}

__INLINE void ble_txccmpktcnt1_set(int elt_idx, uint16_t value)
{
    EM_BLE_WR(BLE_TXCCMPKTCNT1_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, value);
}

// field definitions
#define BLE_TXCCMPKTCNT1_MASK   ((uint16_t)0x0000FFFF)
#define BLE_TXCCMPKTCNT1_LSB    0
#define BLE_TXCCMPKTCNT1_WIDTH  ((uint16_t)0x00000010)

#define BLE_TXCCMPKTCNT1_RST    0x0

__INLINE uint16_t ble_txccmpktcnt1_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(BLE_TXCCMPKTCNT1_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
    ASSERT_ERR((localVal & ~((uint16_t)0x0000FFFF)) == 0);
    return (localVal >> 0);
}

__INLINE void ble_txccmpktcnt1_setf(int elt_idx, uint16_t txccmpktcnt1)
{
    ASSERT_ERR((((uint16_t)txccmpktcnt1 << 0) & ~((uint16_t)0x0000FFFF)) == 0);
    EM_BLE_WR(BLE_TXCCMPKTCNT1_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, (uint16_t)txccmpktcnt1 << 0);
}

/**
 * @brief TXCCMPKTCNT2 register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  06:00         TXCCMPKTCNT2   0x0
 * </pre>
 */
#define BLE_TXCCMPKTCNT2_ADDR   (0x00814044 + EM_BLE_CS_OFFSET)
#define BLE_TXCCMPKTCNT2_INDEX  0x00000022
#define BLE_TXCCMPKTCNT2_RESET  0x00000000

__INLINE uint16_t ble_txccmpktcnt2_get(int elt_idx)
{
    return EM_BLE_RD(BLE_TXCCMPKTCNT2_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
}

__INLINE void ble_txccmpktcnt2_set(int elt_idx, uint16_t value)
{
    EM_BLE_WR(BLE_TXCCMPKTCNT2_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, value);
}

// field definitions
#define BLE_TXCCMPKTCNT2_MASK   ((uint16_t)0x0000007F)
#define BLE_TXCCMPKTCNT2_LSB    0
#define BLE_TXCCMPKTCNT2_WIDTH  ((uint16_t)0x00000007)

#define BLE_TXCCMPKTCNT2_RST    0x0

__INLINE uint8_t ble_txccmpktcnt2_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(BLE_TXCCMPKTCNT2_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
    ASSERT_ERR((localVal & ~((uint16_t)0x0000007F)) == 0);
    return (localVal >> 0);
}

__INLINE void ble_txccmpktcnt2_setf(int elt_idx, uint8_t txccmpktcnt2)
{
    ASSERT_ERR((((uint16_t)txccmpktcnt2 << 0) & ~((uint16_t)0x0000007F)) == 0);
    EM_BLE_WR(BLE_TXCCMPKTCNT2_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, (uint16_t)txccmpktcnt2 << 0);
}

/**
 * @brief RXCCMPKTCNT0 register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  15:00         RXCCMPKTCNT0   0x0
 * </pre>
 */
#define BLE_RXCCMPKTCNT0_ADDR   (0x00814046 + EM_BLE_CS_OFFSET)
#define BLE_RXCCMPKTCNT0_INDEX  0x00000023
#define BLE_RXCCMPKTCNT0_RESET  0x00000000

__INLINE uint16_t ble_rxccmpktcnt0_get(int elt_idx)
{
    return EM_BLE_RD(BLE_RXCCMPKTCNT0_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
}

__INLINE void ble_rxccmpktcnt0_set(int elt_idx, uint16_t value)
{
    EM_BLE_WR(BLE_RXCCMPKTCNT0_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, value);
}

// field definitions
#define BLE_RXCCMPKTCNT0_MASK   ((uint16_t)0x0000FFFF)
#define BLE_RXCCMPKTCNT0_LSB    0
#define BLE_RXCCMPKTCNT0_WIDTH  ((uint16_t)0x00000010)

#define BLE_RXCCMPKTCNT0_RST    0x0

__INLINE uint16_t ble_rxccmpktcnt0_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(BLE_RXCCMPKTCNT0_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
    ASSERT_ERR((localVal & ~((uint16_t)0x0000FFFF)) == 0);
    return (localVal >> 0);
}

__INLINE void ble_rxccmpktcnt0_setf(int elt_idx, uint16_t rxccmpktcnt0)
{
    ASSERT_ERR((((uint16_t)rxccmpktcnt0 << 0) & ~((uint16_t)0x0000FFFF)) == 0);
    EM_BLE_WR(BLE_RXCCMPKTCNT0_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, (uint16_t)rxccmpktcnt0 << 0);
}

/**
 * @brief RXCCMPKTCNT1 register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  15:00         RXCCMPKTCNT1   0x0
 * </pre>
 */
#define BLE_RXCCMPKTCNT1_ADDR   (0x00814048 + EM_BLE_CS_OFFSET)
#define BLE_RXCCMPKTCNT1_INDEX  0x00000024
#define BLE_RXCCMPKTCNT1_RESET  0x00000000

__INLINE uint16_t ble_rxccmpktcnt1_get(int elt_idx)
{
    return EM_BLE_RD(BLE_RXCCMPKTCNT1_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
}

__INLINE void ble_rxccmpktcnt1_set(int elt_idx, uint16_t value)
{
    EM_BLE_WR(BLE_RXCCMPKTCNT1_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, value);
}

// field definitions
#define BLE_RXCCMPKTCNT1_MASK   ((uint16_t)0x0000FFFF)
#define BLE_RXCCMPKTCNT1_LSB    0
#define BLE_RXCCMPKTCNT1_WIDTH  ((uint16_t)0x00000010)

#define BLE_RXCCMPKTCNT1_RST    0x0

__INLINE uint16_t ble_rxccmpktcnt1_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(BLE_RXCCMPKTCNT1_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
    ASSERT_ERR((localVal & ~((uint16_t)0x0000FFFF)) == 0);
    return (localVal >> 0);
}

__INLINE void ble_rxccmpktcnt1_setf(int elt_idx, uint16_t rxccmpktcnt1)
{
    ASSERT_ERR((((uint16_t)rxccmpktcnt1 << 0) & ~((uint16_t)0x0000FFFF)) == 0);
    EM_BLE_WR(BLE_RXCCMPKTCNT1_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, (uint16_t)rxccmpktcnt1 << 0);
}

/**
 * @brief RXCCMPKTCNT2 register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  06:00         RXCCMPKTCNT2   0x0
 * </pre>
 */
#define BLE_RXCCMPKTCNT2_ADDR   (0x0081404A + EM_BLE_CS_OFFSET)
#define BLE_RXCCMPKTCNT2_INDEX  0x00000025
#define BLE_RXCCMPKTCNT2_RESET  0x00000000

__INLINE uint16_t ble_rxccmpktcnt2_get(int elt_idx)
{
    return EM_BLE_RD(BLE_RXCCMPKTCNT2_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
}

__INLINE void ble_rxccmpktcnt2_set(int elt_idx, uint16_t value)
{
    EM_BLE_WR(BLE_RXCCMPKTCNT2_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, value);
}

// field definitions
#define BLE_RXCCMPKTCNT2_MASK   ((uint16_t)0x0000007F)
#define BLE_RXCCMPKTCNT2_LSB    0
#define BLE_RXCCMPKTCNT2_WIDTH  ((uint16_t)0x00000007)

#define BLE_RXCCMPKTCNT2_RST    0x0

__INLINE uint8_t ble_rxccmpktcnt2_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(BLE_RXCCMPKTCNT2_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
    ASSERT_ERR((localVal & ~((uint16_t)0x0000007F)) == 0);
    return (localVal >> 0);
}

__INLINE void ble_rxccmpktcnt2_setf(int elt_idx, uint8_t rxccmpktcnt2)
{
    ASSERT_ERR((((uint16_t)rxccmpktcnt2 << 0) & ~((uint16_t)0x0000007F)) == 0);
    EM_BLE_WR(BLE_RXCCMPKTCNT2_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, (uint16_t)rxccmpktcnt2 << 0);
}

/**
 * @brief EVTCNT register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  15:00              EVT_CNT   0x0
 * </pre>
 */
#define BLE_EVTCNT_ADDR   (0x0081404C + EM_BLE_CS_OFFSET)
#define BLE_EVTCNT_INDEX  0x00000026
#define BLE_EVTCNT_RESET  0x00000000

__INLINE uint16_t ble_evtcnt_get(int elt_idx)
{
    return EM_BLE_RD(BLE_EVTCNT_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
}

__INLINE void ble_evtcnt_set(int elt_idx, uint16_t value)
{
    EM_BLE_WR(BLE_EVTCNT_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, value);
}

// field definitions
#define BLE_EVT_CNT_MASK   ((uint16_t)0x0000FFFF)
#define BLE_EVT_CNT_LSB    0
#define BLE_EVT_CNT_WIDTH  ((uint16_t)0x00000010)

#define BLE_EVT_CNT_RST    0x0

__INLINE uint16_t ble_evt_cnt_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(BLE_EVTCNT_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
    ASSERT_ERR((localVal & ~((uint16_t)0x0000FFFF)) == 0);
    return (localVal >> 0);
}

__INLINE void ble_evt_cnt_setf(int elt_idx, uint16_t evtcnt)
{
    ASSERT_ERR((((uint16_t)evtcnt << 0) & ~((uint16_t)0x0000FFFF)) == 0);
    EM_BLE_WR(BLE_EVTCNT_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, (uint16_t)evtcnt << 0);
}

/**
 * @brief ISOTXRXCNTL register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     15       ISORXBUFF_FULL   0
 *     14         ISOLASTEMPTY   0
 *     13                ISOMD   0
 *     12                ISOSN   0
 *     11              ISONESN   0
 *     09              ISORETX   0
 *     08              ISORSVD   0
 *     03           ISOWAITACK   0
 *     02            ISOLASTMD   0
 *     01          ISOLASTNESN   0
 *     00            ISOLASTSN   0
 * </pre>
 */
#define BLE_ISOTXRXCNTL_ADDR   (0x0081404E + EM_BLE_CS_OFFSET)
#define BLE_ISOTXRXCNTL_INDEX  0x00000027
#define BLE_ISOTXRXCNTL_RESET  0x00000000

__INLINE uint16_t ble_isotxrxcntl_get(int elt_idx)
{
    return EM_BLE_RD(BLE_ISOTXRXCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
}

__INLINE void ble_isotxrxcntl_set(int elt_idx, uint16_t value)
{
    EM_BLE_WR(BLE_ISOTXRXCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, value);
}

// field definitions
#define BLE_ISORXBUFF_FULL_BIT    ((uint16_t)0x00008000)
#define BLE_ISORXBUFF_FULL_POS    15
#define BLE_ISOLASTEMPTY_BIT      ((uint16_t)0x00004000)
#define BLE_ISOLASTEMPTY_POS      14
#define BLE_ISOMD_BIT             ((uint16_t)0x00002000)
#define BLE_ISOMD_POS             13
#define BLE_ISOSN_BIT             ((uint16_t)0x00001000)
#define BLE_ISOSN_POS             12
#define BLE_ISONESN_BIT           ((uint16_t)0x00000800)
#define BLE_ISONESN_POS           11
#define BLE_ISORETX_BIT           ((uint16_t)0x00000200)
#define BLE_ISORETX_POS           9
#define BLE_ISORSVD_BIT           ((uint16_t)0x00000100)
#define BLE_ISORSVD_POS           8
#define BLE_ISOWAITACK_BIT        ((uint16_t)0x00000008)
#define BLE_ISOWAITACK_POS        3
#define BLE_ISOLASTMD_BIT         ((uint16_t)0x00000004)
#define BLE_ISOLASTMD_POS         2
#define BLE_ISOLASTNESN_BIT       ((uint16_t)0x00000002)
#define BLE_ISOLASTNESN_POS       1
#define BLE_ISOLASTSN_BIT         ((uint16_t)0x00000001)
#define BLE_ISOLASTSN_POS         0

#define BLE_ISORXBUFF_FULL_RST    0x0
#define BLE_ISOLASTEMPTY_RST      0x0
#define BLE_ISOMD_RST             0x0
#define BLE_ISOSN_RST             0x0
#define BLE_ISONESN_RST           0x0
#define BLE_ISORETX_RST           0x0
#define BLE_ISORSVD_RST           0x0
#define BLE_ISOWAITACK_RST        0x0
#define BLE_ISOLASTMD_RST         0x0
#define BLE_ISOLASTNESN_RST       0x0
#define BLE_ISOLASTSN_RST         0x0

__INLINE void ble_isotxrxcntl_pack(int elt_idx, uint8_t isorxbufffull, uint8_t isolastempty, uint8_t isomd, uint8_t isosn, uint8_t isonesn, uint8_t isoretx, uint8_t isorsvd, uint8_t isowaitack, uint8_t isolastmd, uint8_t isolastnesn, uint8_t isolastsn)
{
    ASSERT_ERR((((uint16_t)isorxbufffull << 15) & ~((uint16_t)0x00008000)) == 0);
    ASSERT_ERR((((uint16_t)isolastempty << 14) & ~((uint16_t)0x00004000)) == 0);
    ASSERT_ERR((((uint16_t)isomd << 13) & ~((uint16_t)0x00002000)) == 0);
    ASSERT_ERR((((uint16_t)isosn << 12) & ~((uint16_t)0x00001000)) == 0);
    ASSERT_ERR((((uint16_t)isonesn << 11) & ~((uint16_t)0x00000800)) == 0);
    ASSERT_ERR((((uint16_t)isoretx << 9) & ~((uint16_t)0x00000200)) == 0);
    ASSERT_ERR((((uint16_t)isorsvd << 8) & ~((uint16_t)0x00000100)) == 0);
    ASSERT_ERR((((uint16_t)isowaitack << 3) & ~((uint16_t)0x00000008)) == 0);
    ASSERT_ERR((((uint16_t)isolastmd << 2) & ~((uint16_t)0x00000004)) == 0);
    ASSERT_ERR((((uint16_t)isolastnesn << 1) & ~((uint16_t)0x00000002)) == 0);
    ASSERT_ERR((((uint16_t)isolastsn << 0) & ~((uint16_t)0x00000001)) == 0);
    EM_BLE_WR(BLE_ISOTXRXCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE,  ((uint16_t)isorxbufffull << 15) | ((uint16_t)isolastempty << 14) | ((uint16_t)isomd << 13) | ((uint16_t)isosn << 12) | ((uint16_t)isonesn << 11) | ((uint16_t)isoretx << 9) | ((uint16_t)isorsvd << 8) | ((uint16_t)isowaitack << 3) | ((uint16_t)isolastmd << 2) | ((uint16_t)isolastnesn << 1) | ((uint16_t)isolastsn << 0));
}

__INLINE void ble_isotxrxcntl_unpack(int elt_idx, uint8_t* isorxbufffull, uint8_t* isolastempty, uint8_t* isomd, uint8_t* isosn, uint8_t* isonesn, uint8_t* isoretx, uint8_t* isorsvd, uint8_t* isowaitack, uint8_t* isolastmd, uint8_t* isolastnesn, uint8_t* isolastsn)
{
    uint16_t localVal = EM_BLE_RD(BLE_ISOTXRXCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);

    *isorxbufffull = (localVal & ((uint16_t)0x00008000)) >> 15;
    *isolastempty = (localVal & ((uint16_t)0x00004000)) >> 14;
    *isomd = (localVal & ((uint16_t)0x00002000)) >> 13;
    *isosn = (localVal & ((uint16_t)0x00001000)) >> 12;
    *isonesn = (localVal & ((uint16_t)0x00000800)) >> 11;
    *isoretx = (localVal & ((uint16_t)0x00000200)) >> 9;
    *isorsvd = (localVal & ((uint16_t)0x00000100)) >> 8;
    *isowaitack = (localVal & ((uint16_t)0x00000008)) >> 3;
    *isolastmd = (localVal & ((uint16_t)0x00000004)) >> 2;
    *isolastnesn = (localVal & ((uint16_t)0x00000002)) >> 1;
    *isolastsn = (localVal & ((uint16_t)0x00000001)) >> 0;
}

__INLINE uint8_t ble_isorxbuff_full_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(BLE_ISOTXRXCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
    return ((localVal & ((uint16_t)0x00008000)) >> 15);
}

__INLINE void ble_isorxbuff_full_setf(int elt_idx, uint8_t isorxbufffull)
{
    ASSERT_ERR((((uint16_t)isorxbufffull << 15) & ~((uint16_t)0x00008000)) == 0);
    EM_BLE_WR(BLE_ISOTXRXCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, (EM_BLE_RD(BLE_ISOTXRXCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE) & ~((uint16_t)0x00008000)) | ((uint16_t)isorxbufffull << 15));
}

__INLINE uint8_t ble_isolastempty_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(BLE_ISOTXRXCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
    return ((localVal & ((uint16_t)0x00004000)) >> 14);
}

__INLINE void ble_isolastempty_setf(int elt_idx, uint8_t isolastempty)
{
    ASSERT_ERR((((uint16_t)isolastempty << 14) & ~((uint16_t)0x00004000)) == 0);
    EM_BLE_WR(BLE_ISOTXRXCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, (EM_BLE_RD(BLE_ISOTXRXCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE) & ~((uint16_t)0x00004000)) | ((uint16_t)isolastempty << 14));
}

__INLINE uint8_t ble_isomd_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(BLE_ISOTXRXCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
    return ((localVal & ((uint16_t)0x00002000)) >> 13);
}

__INLINE void ble_isomd_setf(int elt_idx, uint8_t isomd)
{
    ASSERT_ERR((((uint16_t)isomd << 13) & ~((uint16_t)0x00002000)) == 0);
    EM_BLE_WR(BLE_ISOTXRXCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, (EM_BLE_RD(BLE_ISOTXRXCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE) & ~((uint16_t)0x00002000)) | ((uint16_t)isomd << 13));
}

__INLINE uint8_t ble_isosn_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(BLE_ISOTXRXCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
    return ((localVal & ((uint16_t)0x00001000)) >> 12);
}

__INLINE void ble_isosn_setf(int elt_idx, uint8_t isosn)
{
    ASSERT_ERR((((uint16_t)isosn << 12) & ~((uint16_t)0x00001000)) == 0);
    EM_BLE_WR(BLE_ISOTXRXCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, (EM_BLE_RD(BLE_ISOTXRXCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE) & ~((uint16_t)0x00001000)) | ((uint16_t)isosn << 12));
}

__INLINE uint8_t ble_isonesn_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(BLE_ISOTXRXCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
    return ((localVal & ((uint16_t)0x00000800)) >> 11);
}

__INLINE void ble_isonesn_setf(int elt_idx, uint8_t isonesn)
{
    ASSERT_ERR((((uint16_t)isonesn << 11) & ~((uint16_t)0x00000800)) == 0);
    EM_BLE_WR(BLE_ISOTXRXCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, (EM_BLE_RD(BLE_ISOTXRXCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE) & ~((uint16_t)0x00000800)) | ((uint16_t)isonesn << 11));
}

__INLINE uint8_t ble_isoretx_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(BLE_ISOTXRXCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
    return ((localVal & ((uint16_t)0x00000200)) >> 9);
}

__INLINE void ble_isoretx_setf(int elt_idx, uint8_t isoretx)
{
    ASSERT_ERR((((uint16_t)isoretx << 9) & ~((uint16_t)0x00000200)) == 0);
    EM_BLE_WR(BLE_ISOTXRXCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, (EM_BLE_RD(BLE_ISOTXRXCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE) & ~((uint16_t)0x00000200)) | ((uint16_t)isoretx << 9));
}

__INLINE uint8_t ble_isorsvd_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(BLE_ISOTXRXCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
    return ((localVal & ((uint16_t)0x00000100)) >> 8);
}

__INLINE void ble_isorsvd_setf(int elt_idx, uint8_t isorsvd)
{
    ASSERT_ERR((((uint16_t)isorsvd << 8) & ~((uint16_t)0x00000100)) == 0);
    EM_BLE_WR(BLE_ISOTXRXCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, (EM_BLE_RD(BLE_ISOTXRXCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE) & ~((uint16_t)0x00000100)) | ((uint16_t)isorsvd << 8));
}

__INLINE uint8_t ble_isowaitack_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(BLE_ISOTXRXCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
    return ((localVal & ((uint16_t)0x00000008)) >> 3);
}

__INLINE void ble_isowaitack_setf(int elt_idx, uint8_t isowaitack)
{
    ASSERT_ERR((((uint16_t)isowaitack << 3) & ~((uint16_t)0x00000008)) == 0);
    EM_BLE_WR(BLE_ISOTXRXCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, (EM_BLE_RD(BLE_ISOTXRXCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE) & ~((uint16_t)0x00000008)) | ((uint16_t)isowaitack << 3));
}

__INLINE uint8_t ble_isolastmd_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(BLE_ISOTXRXCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
    return ((localVal & ((uint16_t)0x00000004)) >> 2);
}

__INLINE void ble_isolastmd_setf(int elt_idx, uint8_t isolastmd)
{
    ASSERT_ERR((((uint16_t)isolastmd << 2) & ~((uint16_t)0x00000004)) == 0);
    EM_BLE_WR(BLE_ISOTXRXCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, (EM_BLE_RD(BLE_ISOTXRXCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE) & ~((uint16_t)0x00000004)) | ((uint16_t)isolastmd << 2));
}

__INLINE uint8_t ble_isolastnesn_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(BLE_ISOTXRXCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
    return ((localVal & ((uint16_t)0x00000002)) >> 1);
}

__INLINE void ble_isolastnesn_setf(int elt_idx, uint8_t isolastnesn)
{
    ASSERT_ERR((((uint16_t)isolastnesn << 1) & ~((uint16_t)0x00000002)) == 0);
    EM_BLE_WR(BLE_ISOTXRXCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, (EM_BLE_RD(BLE_ISOTXRXCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE) & ~((uint16_t)0x00000002)) | ((uint16_t)isolastnesn << 1));
}

__INLINE uint8_t ble_isolastsn_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(BLE_ISOTXRXCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
    return ((localVal & ((uint16_t)0x00000001)) >> 0);
}

__INLINE void ble_isolastsn_setf(int elt_idx, uint8_t isolastsn)
{
    ASSERT_ERR((((uint16_t)isolastsn << 0) & ~((uint16_t)0x00000001)) == 0);
    EM_BLE_WR(BLE_ISOTXRXCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, (EM_BLE_RD(BLE_ISOTXRXCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE) & ~((uint16_t)0x00000001)) | ((uint16_t)isolastsn << 0));
}

/**
 * @brief BTCNTSYNC0 register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  15:00           BTCNTSYNC0   0x0
 * </pre>
 */
#define BLE_BTCNTSYNC0_ADDR   (0x00814050 + EM_BLE_CS_OFFSET)
#define BLE_BTCNTSYNC0_INDEX  0x00000028
#define BLE_BTCNTSYNC0_RESET  0x00000000

__INLINE uint16_t ble_btcntsync0_get(int elt_idx)
{
    return EM_BLE_RD(BLE_BTCNTSYNC0_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
}

__INLINE void ble_btcntsync0_set(int elt_idx, uint16_t value)
{
    EM_BLE_WR(BLE_BTCNTSYNC0_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, value);
}

// field definitions
#define BLE_BTCNTSYNC0_MASK   ((uint16_t)0x0000FFFF)
#define BLE_BTCNTSYNC0_LSB    0
#define BLE_BTCNTSYNC0_WIDTH  ((uint16_t)0x00000010)

#define BLE_BTCNTSYNC0_RST    0x0

__INLINE uint16_t ble_btcntsync0_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(BLE_BTCNTSYNC0_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
    ASSERT_ERR((localVal & ~((uint16_t)0x0000FFFF)) == 0);
    return (localVal >> 0);
}

__INLINE void ble_btcntsync0_setf(int elt_idx, uint16_t btcntsync0)
{
    ASSERT_ERR((((uint16_t)btcntsync0 << 0) & ~((uint16_t)0x0000FFFF)) == 0);
    EM_BLE_WR(BLE_BTCNTSYNC0_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, (uint16_t)btcntsync0 << 0);
}

/**
 * @brief BTCNTSYNC1 register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  10:00           BTCNTSYNC1   0x0
 * </pre>
 */
#define BLE_BTCNTSYNC1_ADDR   (0x00814052 + EM_BLE_CS_OFFSET)
#define BLE_BTCNTSYNC1_INDEX  0x00000029
#define BLE_BTCNTSYNC1_RESET  0x00000000

__INLINE uint16_t ble_btcntsync1_get(int elt_idx)
{
    return EM_BLE_RD(BLE_BTCNTSYNC1_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
}

__INLINE void ble_btcntsync1_set(int elt_idx, uint16_t value)
{
    EM_BLE_WR(BLE_BTCNTSYNC1_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, value);
}

// field definitions
#define BLE_BTCNTSYNC1_MASK   ((uint16_t)0x000007FF)
#define BLE_BTCNTSYNC1_LSB    0
#define BLE_BTCNTSYNC1_WIDTH  ((uint16_t)0x0000000B)

#define BLE_BTCNTSYNC1_RST    0x0

__INLINE uint16_t ble_btcntsync1_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(BLE_BTCNTSYNC1_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
    ASSERT_ERR((localVal & ~((uint16_t)0x000007FF)) == 0);
    return (localVal >> 0);
}

__INLINE void ble_btcntsync1_setf(int elt_idx, uint16_t btcntsync1)
{
    ASSERT_ERR((((uint16_t)btcntsync1 << 0) & ~((uint16_t)0x000007FF)) == 0);
    EM_BLE_WR(BLE_BTCNTSYNC1_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, (uint16_t)btcntsync1 << 0);
}

/**
 * @brief FCNTSYNC register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *     15              EVTRXOK   0
 *  09:00           FCNTRXSYNC   0x0
 * </pre>
 */
#define BLE_FCNTSYNC_ADDR   (0x00814054 + EM_BLE_CS_OFFSET)
#define BLE_FCNTSYNC_INDEX  0x0000002A
#define BLE_FCNTSYNC_RESET  0x00000000

__INLINE uint16_t ble_fcntsync_get(int elt_idx)
{
    return EM_BLE_RD(BLE_FCNTSYNC_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
}

__INLINE void ble_fcntsync_set(int elt_idx, uint16_t value)
{
    EM_BLE_WR(BLE_FCNTSYNC_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, value);
}

// field definitions
#define BLE_EVTRXOK_BIT       ((uint16_t)0x00008000)
#define BLE_EVTRXOK_POS       15
#define BLE_FCNTRXSYNC_MASK   ((uint16_t)0x000003FF)
#define BLE_FCNTRXSYNC_LSB    0
#define BLE_FCNTRXSYNC_WIDTH  ((uint16_t)0x0000000A)

#define BLE_EVTRXOK_RST       0x0
#define BLE_FCNTRXSYNC_RST    0x0

__INLINE void ble_fcntsync_unpack(int elt_idx, uint8_t* evtrxok, uint16_t* fcntrxsync)
{
    uint16_t localVal = EM_BLE_RD(BLE_FCNTSYNC_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);

    *evtrxok = (localVal & ((uint16_t)0x00008000)) >> 15;
    *fcntrxsync = (localVal & ((uint16_t)0x000003FF)) >> 0;
}

__INLINE uint8_t ble_evtrxok_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(BLE_FCNTSYNC_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
    return ((localVal & ((uint16_t)0x00008000)) >> 15);
}

__INLINE uint16_t ble_fcntrxsync_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(BLE_FCNTSYNC_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
    return ((localVal & ((uint16_t)0x000003FF)) >> 0);
}

__INLINE void ble_fcntrxsync_setf(int elt_idx, uint16_t fcntrxsync)
{
    ASSERT_ERR((((uint16_t)fcntrxsync << 0) & ~((uint16_t)0x000003FF)) == 0);
    EM_BLE_WR(BLE_FCNTSYNC_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, (uint16_t)fcntrxsync << 0);
}

/**
 * @brief TXRXDESCCNT register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  15:08         ACLRXDESCCNT   0x0
 *  07:00         ACLTXDESCCNT   0x0
 * </pre>
 */
#define BLE_TXRXDESCCNT_ADDR   (0x00814056 + EM_BLE_CS_OFFSET)
#define BLE_TXRXDESCCNT_INDEX  0x0000002B
#define BLE_TXRXDESCCNT_RESET  0x00000000

__INLINE uint16_t ble_txrxdesccnt_get(int elt_idx)
{
    return EM_BLE_RD(BLE_TXRXDESCCNT_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
}

__INLINE void ble_txrxdesccnt_set(int elt_idx, uint16_t value)
{
    EM_BLE_WR(BLE_TXRXDESCCNT_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, value);
}

// field definitions
#define BLE_ACLRXDESCCNT_MASK   ((uint16_t)0x0000FF00)
#define BLE_ACLRXDESCCNT_LSB    8
#define BLE_ACLRXDESCCNT_WIDTH  ((uint16_t)0x00000008)
#define BLE_ACLTXDESCCNT_MASK   ((uint16_t)0x000000FF)
#define BLE_ACLTXDESCCNT_LSB    0
#define BLE_ACLTXDESCCNT_WIDTH  ((uint16_t)0x00000008)

#define BLE_ACLRXDESCCNT_RST    0x0
#define BLE_ACLTXDESCCNT_RST    0x0

__INLINE void ble_txrxdesccnt_pack(int elt_idx, uint8_t aclrxdesccnt, uint8_t acltxdesccnt)
{
    ASSERT_ERR((((uint16_t)aclrxdesccnt << 8) & ~((uint16_t)0x0000FF00)) == 0);
    ASSERT_ERR((((uint16_t)acltxdesccnt << 0) & ~((uint16_t)0x000000FF)) == 0);
    EM_BLE_WR(BLE_TXRXDESCCNT_ADDR + elt_idx * REG_BLE_EM_CS_SIZE,  ((uint16_t)aclrxdesccnt << 8) | ((uint16_t)acltxdesccnt << 0));
}

__INLINE void ble_txrxdesccnt_unpack(int elt_idx, uint8_t* aclrxdesccnt, uint8_t* acltxdesccnt)
{
    uint16_t localVal = EM_BLE_RD(BLE_TXRXDESCCNT_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);

    *aclrxdesccnt = (localVal & ((uint16_t)0x0000FF00)) >> 8;
    *acltxdesccnt = (localVal & ((uint16_t)0x000000FF)) >> 0;
}

__INLINE uint8_t ble_aclrxdesccnt_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(BLE_TXRXDESCCNT_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
    return ((localVal & ((uint16_t)0x0000FF00)) >> 8);
}

__INLINE void ble_aclrxdesccnt_setf(int elt_idx, uint8_t aclrxdesccnt)
{
    ASSERT_ERR((((uint16_t)aclrxdesccnt << 8) & ~((uint16_t)0x0000FF00)) == 0);
    EM_BLE_WR(BLE_TXRXDESCCNT_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, (EM_BLE_RD(BLE_TXRXDESCCNT_ADDR + elt_idx * REG_BLE_EM_CS_SIZE) & ~((uint16_t)0x0000FF00)) | ((uint16_t)aclrxdesccnt << 8));
}

__INLINE uint8_t ble_acltxdesccnt_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(BLE_TXRXDESCCNT_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
    return ((localVal & ((uint16_t)0x000000FF)) >> 0);
}

__INLINE void ble_acltxdesccnt_setf(int elt_idx, uint8_t acltxdesccnt)
{
    ASSERT_ERR((((uint16_t)acltxdesccnt << 0) & ~((uint16_t)0x000000FF)) == 0);
    EM_BLE_WR(BLE_TXRXDESCCNT_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, (EM_BLE_RD(BLE_TXRXDESCCNT_ADDR + elt_idx * REG_BLE_EM_CS_SIZE) & ~((uint16_t)0x000000FF)) | ((uint16_t)acltxdesccnt << 0));
}

/**
 * @brief ISOTXRXPKTCNT register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  15:08         ISORXPKTCNTL   0x0
 *  07:00         ISOTXPKTCNTL   0x0
 * </pre>
 */
#define BLE_ISOTXRXPKTCNT_ADDR   (0x00814058 + EM_BLE_CS_OFFSET)
#define BLE_ISOTXRXPKTCNT_INDEX  0x0000002C
#define BLE_ISOTXRXPKTCNT_RESET  0x00000000

__INLINE uint16_t ble_isotxrxpktcnt_get(int elt_idx)
{
    return EM_BLE_RD(BLE_ISOTXRXPKTCNT_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
}

__INLINE void ble_isotxrxpktcnt_set(int elt_idx, uint16_t value)
{
    EM_BLE_WR(BLE_ISOTXRXPKTCNT_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, value);
}

// field definitions
#define BLE_ISORXPKTCNTL_MASK   ((uint16_t)0x0000FF00)
#define BLE_ISORXPKTCNTL_LSB    8
#define BLE_ISORXPKTCNTL_WIDTH  ((uint16_t)0x00000008)
#define BLE_ISOTXPKTCNTL_MASK   ((uint16_t)0x000000FF)
#define BLE_ISOTXPKTCNTL_LSB    0
#define BLE_ISOTXPKTCNTL_WIDTH  ((uint16_t)0x00000008)

#define BLE_ISORXPKTCNTL_RST    0x0
#define BLE_ISOTXPKTCNTL_RST    0x0

__INLINE void ble_isotxrxpktcnt_pack(int elt_idx, uint8_t isorxpktcntl, uint8_t isotxpktcntl)
{
    ASSERT_ERR((((uint16_t)isorxpktcntl << 8) & ~((uint16_t)0x0000FF00)) == 0);
    ASSERT_ERR((((uint16_t)isotxpktcntl << 0) & ~((uint16_t)0x000000FF)) == 0);
    EM_BLE_WR(BLE_ISOTXRXPKTCNT_ADDR + elt_idx * REG_BLE_EM_CS_SIZE,  ((uint16_t)isorxpktcntl << 8) | ((uint16_t)isotxpktcntl << 0));
}

__INLINE void ble_isotxrxpktcnt_unpack(int elt_idx, uint8_t* isorxpktcntl, uint8_t* isotxpktcntl)
{
    uint16_t localVal = EM_BLE_RD(BLE_ISOTXRXPKTCNT_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);

    *isorxpktcntl = (localVal & ((uint16_t)0x0000FF00)) >> 8;
    *isotxpktcntl = (localVal & ((uint16_t)0x000000FF)) >> 0;
}

__INLINE uint8_t ble_isorxpktcntl_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(BLE_ISOTXRXPKTCNT_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
    return ((localVal & ((uint16_t)0x0000FF00)) >> 8);
}

__INLINE void ble_isorxpktcntl_setf(int elt_idx, uint8_t isorxpktcntl)
{
    ASSERT_ERR((((uint16_t)isorxpktcntl << 8) & ~((uint16_t)0x0000FF00)) == 0);
    EM_BLE_WR(BLE_ISOTXRXPKTCNT_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, (EM_BLE_RD(BLE_ISOTXRXPKTCNT_ADDR + elt_idx * REG_BLE_EM_CS_SIZE) & ~((uint16_t)0x0000FF00)) | ((uint16_t)isorxpktcntl << 8));
}

__INLINE uint8_t ble_isotxpktcntl_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(BLE_ISOTXRXPKTCNT_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
    return ((localVal & ((uint16_t)0x000000FF)) >> 0);
}

__INLINE void ble_isotxpktcntl_setf(int elt_idx, uint8_t isotxpktcntl)
{
    ASSERT_ERR((((uint16_t)isotxpktcntl << 0) & ~((uint16_t)0x000000FF)) == 0);
    EM_BLE_WR(BLE_ISOTXRXPKTCNT_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, (EM_BLE_RD(BLE_ISOTXRXPKTCNT_ADDR + elt_idx * REG_BLE_EM_CS_SIZE) & ~((uint16_t)0x000000FF)) | ((uint16_t)isotxpktcntl << 0));
}

/**
 * @brief DMPRIOCNTL register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  15:13          PRIOINCSTEP   0x0
 *  12:08              MINPRIO   0x0
 *     07             CONFLICT   0
 *  04:00          CURRENTPRIO   0x0
 * </pre>
 */
#define BLE_DMPRIOCNTL_ADDR   (0x0081405A + EM_BLE_CS_OFFSET)
#define BLE_DMPRIOCNTL_INDEX  0x0000002D
#define BLE_DMPRIOCNTL_RESET  0x00000000

__INLINE uint16_t ble_dmpriocntl_get(int elt_idx)
{
    return EM_BLE_RD(BLE_DMPRIOCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
}

__INLINE void ble_dmpriocntl_set(int elt_idx, uint16_t value)
{
    EM_BLE_WR(BLE_DMPRIOCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, value);
}

// field definitions
#define BLE_PRIOINCSTEP_MASK   ((uint16_t)0x0000E000)
#define BLE_PRIOINCSTEP_LSB    13
#define BLE_PRIOINCSTEP_WIDTH  ((uint16_t)0x00000003)
#define BLE_MINPRIO_MASK       ((uint16_t)0x00001F00)
#define BLE_MINPRIO_LSB        8
#define BLE_MINPRIO_WIDTH      ((uint16_t)0x00000005)
#define BLE_CONFLICT_BIT       ((uint16_t)0x00000080)
#define BLE_CONFLICT_POS       7
#define BLE_CURRENTPRIO_MASK   ((uint16_t)0x0000001F)
#define BLE_CURRENTPRIO_LSB    0
#define BLE_CURRENTPRIO_WIDTH  ((uint16_t)0x00000005)

#define BLE_PRIOINCSTEP_RST    0x0
#define BLE_MINPRIO_RST        0x0
#define BLE_CONFLICT_RST       0x0
#define BLE_CURRENTPRIO_RST    0x0

__INLINE void ble_dmpriocntl_pack(int elt_idx, uint8_t prioincstep, uint8_t minprio, uint8_t conflict, uint8_t currentprio)
{
    ASSERT_ERR((((uint16_t)prioincstep << 13) & ~((uint16_t)0x0000E000)) == 0);
    ASSERT_ERR((((uint16_t)minprio << 8) & ~((uint16_t)0x00001F00)) == 0);
    ASSERT_ERR((((uint16_t)conflict << 7) & ~((uint16_t)0x00000080)) == 0);
    ASSERT_ERR((((uint16_t)currentprio << 0) & ~((uint16_t)0x0000001F)) == 0);
    EM_BLE_WR(BLE_DMPRIOCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE,  ((uint16_t)prioincstep << 13) | ((uint16_t)minprio << 8) | ((uint16_t)conflict << 7) | ((uint16_t)currentprio << 0));
}

__INLINE void ble_dmpriocntl_unpack(int elt_idx, uint8_t* prioincstep, uint8_t* minprio, uint8_t* conflict, uint8_t* currentprio)
{
    uint16_t localVal = EM_BLE_RD(BLE_DMPRIOCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);

    *prioincstep = (localVal & ((uint16_t)0x0000E000)) >> 13;
    *minprio = (localVal & ((uint16_t)0x00001F00)) >> 8;
    *conflict = (localVal & ((uint16_t)0x00000080)) >> 7;
    *currentprio = (localVal & ((uint16_t)0x0000001F)) >> 0;
}

__INLINE uint8_t ble_prioincstep_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(BLE_DMPRIOCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
    return ((localVal & ((uint16_t)0x0000E000)) >> 13);
}

__INLINE void ble_prioincstep_setf(int elt_idx, uint8_t prioincstep)
{
    ASSERT_ERR((((uint16_t)prioincstep << 13) & ~((uint16_t)0x0000E000)) == 0);
    EM_BLE_WR(BLE_DMPRIOCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, (EM_BLE_RD(BLE_DMPRIOCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE) & ~((uint16_t)0x0000E000)) | ((uint16_t)prioincstep << 13));
}

__INLINE uint8_t ble_minprio_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(BLE_DMPRIOCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
    return ((localVal & ((uint16_t)0x00001F00)) >> 8);
}

__INLINE void ble_minprio_setf(int elt_idx, uint8_t minprio)
{
    ASSERT_ERR((((uint16_t)minprio << 8) & ~((uint16_t)0x00001F00)) == 0);
    EM_BLE_WR(BLE_DMPRIOCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, (EM_BLE_RD(BLE_DMPRIOCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE) & ~((uint16_t)0x00001F00)) | ((uint16_t)minprio << 8));
}

__INLINE uint8_t ble_conflict_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(BLE_DMPRIOCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
    return ((localVal & ((uint16_t)0x00000080)) >> 7);
}

__INLINE void ble_conflict_setf(int elt_idx, uint8_t conflict)
{
    ASSERT_ERR((((uint16_t)conflict << 7) & ~((uint16_t)0x00000080)) == 0);
    EM_BLE_WR(BLE_DMPRIOCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, (EM_BLE_RD(BLE_DMPRIOCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE) & ~((uint16_t)0x00000080)) | ((uint16_t)conflict << 7));
}

__INLINE uint8_t ble_currentprio_getf(int elt_idx)
{
    uint16_t localVal = EM_BLE_RD(BLE_DMPRIOCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE);
    return ((localVal & ((uint16_t)0x0000001F)) >> 0);
}

__INLINE void ble_currentprio_setf(int elt_idx, uint8_t currentprio)
{
    ASSERT_ERR((((uint16_t)currentprio << 0) & ~((uint16_t)0x0000001F)) == 0);
    EM_BLE_WR(BLE_DMPRIOCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE, (EM_BLE_RD(BLE_DMPRIOCNTL_ADDR + elt_idx * REG_BLE_EM_CS_SIZE) & ~((uint16_t)0x0000001F)) | ((uint16_t)currentprio << 0));
}


#endif // _REG_BLE_EM_CS_H_

