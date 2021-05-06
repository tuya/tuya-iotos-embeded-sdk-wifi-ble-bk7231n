#ifndef _REG_EM_BLE_FT_H_
#define _REG_EM_BLE_FT_H_

#include <stdint.h>
#include "_reg_em_ble_ft.h"
#include "compiler.h"
#include "architect.h"
#include "em_map.h"
#include "ble_reg_access.h"

#define REG_EM_BLE_FT_COUNT 1

#define REG_EM_BLE_FT_DECODING_MASK 0x00000000

#define REG_EM_BLE_FT_ADDR_GET(idx) (EM_BLE_FT_OFFSET + (idx) * REG_EM_BLE_FT_SIZE)

/**
 * @brief FREQTAB register definition
 * <pre>
 *   Bits           Field Name   Reset Value
 *  -----   ------------------   -----------
 *  15:08              FREQODD   0x0
 *  07:00             FREQEVEN   0x0
 * </pre>
 */
#define EM_BLE_FREQTAB_ADDR   (0x00910000 + EM_BLE_FT_OFFSET)
#define EM_BLE_FREQTAB_INDEX  0x00000000
#define EM_BLE_FREQTAB_RESET  0x00000000
#define EM_BLE_FREQTAB_COUNT  20

__INLINE uint16_t em_ble_freqtab_get(int elt_idx, int reg_idx)
{
    BLE_ASSERT_ERR(reg_idx <= 19);
    return EM_BLE_RD(EM_BLE_FREQTAB_ADDR + elt_idx * REG_EM_BLE_FT_SIZE + reg_idx * 2);
}

__INLINE void em_ble_freqtab_set(int elt_idx, int reg_idx, uint16_t value)
{
    BLE_ASSERT_ERR(reg_idx <= 19);
    EM_BLE_WR(EM_BLE_FREQTAB_ADDR + elt_idx * REG_EM_BLE_FT_SIZE + reg_idx * 2, value);
}

// field definitions
#define EM_BLE_FREQODD_MASK    ((uint16_t)0x0000FF00)
#define EM_BLE_FREQODD_LSB     8
#define EM_BLE_FREQODD_WIDTH   ((uint16_t)0x00000008)
#define EM_BLE_FREQEVEN_MASK   ((uint16_t)0x000000FF)
#define EM_BLE_FREQEVEN_LSB    0
#define EM_BLE_FREQEVEN_WIDTH  ((uint16_t)0x00000008)

#define EM_BLE_FREQODD_RST     0x0
#define EM_BLE_FREQEVEN_RST    0x0

__INLINE void em_ble_freqtab_pack(int elt_idx, int reg_idx, uint8_t freqodd, uint8_t freqeven)
{
    BLE_ASSERT_ERR(reg_idx <= 19);
    BLE_ASSERT_ERR((((uint16_t)freqodd << 8) & ~((uint16_t)0x0000FF00)) == 0);
    BLE_ASSERT_ERR((((uint16_t)freqeven << 0) & ~((uint16_t)0x000000FF)) == 0);
    EM_BLE_WR(EM_BLE_FREQTAB_ADDR + elt_idx * REG_EM_BLE_FT_SIZE + reg_idx * 2,  ((uint16_t)freqodd << 8) | ((uint16_t)freqeven << 0));
}

__INLINE void em_ble_freqtab_unpack(int elt_idx, int reg_idx, uint8_t* freqodd, uint8_t* freqeven)
{
    BLE_ASSERT_ERR(reg_idx <= 19);
    uint16_t localVal = EM_BLE_RD(EM_BLE_FREQTAB_ADDR + elt_idx * REG_EM_BLE_FT_SIZE + reg_idx * 2);

    *freqodd = (localVal & ((uint16_t)0x0000FF00)) >> 8;
    *freqeven = (localVal & ((uint16_t)0x000000FF)) >> 0;
}

__INLINE uint8_t em_ble_freqtab_freqodd_getf(int elt_idx, int reg_idx)
{
    BLE_ASSERT_ERR(reg_idx <= 19);
    uint16_t localVal = EM_BLE_RD(EM_BLE_FREQTAB_ADDR + elt_idx * REG_EM_BLE_FT_SIZE + reg_idx * 2);
    return ((localVal & ((uint16_t)0x0000FF00)) >> 8);
}

__INLINE void em_ble_freqtab_freqodd_setf(int elt_idx, int reg_idx, uint8_t freqodd)
{
    BLE_ASSERT_ERR(reg_idx <= 19);
    BLE_ASSERT_ERR((((uint16_t)freqodd << 8) & ~((uint16_t)0x0000FF00)) == 0);
    EM_BLE_WR(EM_BLE_FREQTAB_ADDR + elt_idx * REG_EM_BLE_FT_SIZE + reg_idx * 2, (EM_BLE_RD(EM_BLE_FREQTAB_ADDR + elt_idx * REG_EM_BLE_FT_SIZE + reg_idx * 2) & ~((uint16_t)0x0000FF00)) | ((uint16_t)freqodd << 8));
}

__INLINE uint8_t em_ble_freqtab_freqeven_getf(int elt_idx, int reg_idx)
{
    BLE_ASSERT_ERR(reg_idx <= 19);
    uint16_t localVal = EM_BLE_RD(EM_BLE_FREQTAB_ADDR + elt_idx * REG_EM_BLE_FT_SIZE + reg_idx * 2);
    return ((localVal & ((uint16_t)0x000000FF)) >> 0);
}

__INLINE void em_ble_freqtab_freqeven_setf(int elt_idx, int reg_idx, uint8_t freqeven)
{
    BLE_ASSERT_ERR(reg_idx <= 19);
    BLE_ASSERT_ERR((((uint16_t)freqeven << 0) & ~((uint16_t)0x000000FF)) == 0);
    EM_BLE_WR(EM_BLE_FREQTAB_ADDR + elt_idx * REG_EM_BLE_FT_SIZE + reg_idx * 2, (EM_BLE_RD(EM_BLE_FREQTAB_ADDR + elt_idx * REG_EM_BLE_FT_SIZE + reg_idx * 2) & ~((uint16_t)0x000000FF)) | ((uint16_t)freqeven << 0));
}


#endif // _REG_EM_BLE_FT_H_

