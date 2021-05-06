/**
 ****************************************************************************************
 *
 * @file ble_util_buf.h
 *
 * @brief Main API file for the BLE EM buffer management system
 *
 * Copyright (C) RivieraWaves 2009-2016
 *
 ****************************************************************************************
 */

#ifndef BLE_UTIL_BUF_H_
#define BLE_UTIL_BUF_H_

/**
 ****************************************************************************************
 * @defgroup BLE_UTIL_BUF BLE EM buffer management system
 * @ingroup ROOT
 * @brief BLE EM buffer management system
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <stdint.h>          // standard integer
#include "common_list.h"         // common list definition

/*
 * DEFINES
 ****************************************************************************************
 */

/// ACL data: handle and data length decoding
#define BLE_EM_ACL_DATA_LEN_LSB   (0)
#define BLE_EM_ACL_DATA_LEN_MASK  (0x03FF)
/// Packet boundary flag
#define BLE_EM_ACL_PBF_LSB        (12)
#define BLE_EM_ACL_PBF_MASK       (0x3000)
/// Broadcast flag
#define BLE_EM_ACL_BF_LSB         (14)
#define BLE_EM_ACL_BF_MASK        (0xC000)

#if (BLE_ISO_PRESENT)
#define BLE_UTIL_ISODESC_INVALID 0xFF
#endif // (BLE_ISO_PRESENT)

/*
 * STRUCTURE DEFINITION
 ****************************************************************************************
 */

/// EM LLCP buffer element
struct ble_em_llcp_buf_elt
{
    /// List header
    struct common_list_hdr hdr;
    /// EM buffer pointer
    uint16_t buf_ptr;
    /// Data length (in bytes)
    uint8_t length;
};

/// EM ACL buffer element
struct ble_em_acl_buf_elt
{
    /// List header
    struct common_list_hdr hdr;
    /// EM buffer pointer
    uint16_t buf_ptr;
    /// Data length (in bytes) + Data Flags (PBF + BF)
    uint16_t data_len_flags;
};

/// EM advertising data buffer element
struct ble_em_adv_buf_elt
{
    /// List header
    struct common_list_hdr hdr;
    /// EM buffer pointer
    uint16_t buf_ptr;
    /// Data length (in bytes)
    uint16_t length;
};

#if (BLE_ISO_PRESENT)
/// EM ISO TX/RX Buffer
struct ble_em_iso_buf_elt
{
    /// List header
    struct common_list_hdr hdr;
    /// EM buffer pointer
    uint16_t buf_ptr;
    /// Buffer index
    uint8_t  buf_idx;
};
#endif // (BLE_ISO_PRESENT)

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialize BLE EM buffers management system
 *
 * @param[in] init_type  Type of initialization (@see enum rwip_init_type)
 ****************************************************************************************
 */
void ble_util_buf_init(uint8_t init_type);

/**
 ****************************************************************************************
 * @brief Allocate a LLCP TX buffer
 *
 * @return  Pointer to LLCP TX buffer, NULL if no buffer available
 ****************************************************************************************
 */
struct ble_em_llcp_buf_elt* ble_util_buf_llcp_tx_alloc(void);

/**
 ****************************************************************************************
 * @brief Free a LLCP TX buffer
 *
 * @param[in]  buf  Pointer to LLCP TX buffer
 ****************************************************************************************
 */
void ble_util_buf_llcp_tx_free(uint16_t buf);

/**
 ****************************************************************************************
 * @brief Allocate a RX buffer
 *
 * @return  Pointer to RX buffer, NULL if no buffer available
 ****************************************************************************************
 */
uint16_t ble_util_buf_rx_alloc(void);

/**
 ****************************************************************************************
 * @brief Free a RX buffer
 *
 * @param[in]  buf  Pointer to RX buffer
 ****************************************************************************************
 */
void ble_util_buf_rx_free(uint16_t buf);

/**
 ****************************************************************************************
 * @brief Allocate a ACL TX buffer
 *
 * @return  Pointer to ACL TX buffer, NULL if no buffer available
 ****************************************************************************************
 */
uint16_t ble_util_buf_acl_tx_alloc(void);


/**
 ****************************************************************************************
 * @brief Retrieve TX buffer Element
 *
 * @return  Pointer to ACL TX buffer element
 ****************************************************************************************
 */
struct ble_em_acl_buf_elt* ble_util_buf_acl_tx_elt_get(uint16_t buf);

/**
 ****************************************************************************************
 * @brief Free a ACL TX buffer
 *
 * @param[in]  buf  Pointer to ACL TX buffer
 ****************************************************************************************
 */
void ble_util_buf_acl_tx_free(uint16_t buf);

/**
 ****************************************************************************************
 * @brief Allocate an advertising data TX buffer
 *
 * @return  Pointer to advertising data TX buffer, NULL if no buffer available
 ****************************************************************************************
 */
uint16_t ble_util_buf_adv_tx_alloc(void);

/**
 ****************************************************************************************
 * @brief Free an advertising data TX buffer
 *
 * @param[in]  buf  Pointer to advertising data TX buffer
 ****************************************************************************************
 */
void ble_util_buf_adv_tx_free(uint16_t buf);

#if (BLE_ISO_PRESENT)
/**
 ****************************************************************************************
 * @brief Allocate an Isochronous data TX/RX buffer
 *
 * @param[out]  index  Index of Isochronous data TX/RX buffer
 *
 * @return  Pointer to Isochronous data TX/RX buffer, NULL if no buffer available
 ****************************************************************************************
 */
uint16_t ble_util_buf_iso_alloc(uint8_t* index);

/**
 ****************************************************************************************
 * @brief Free an Isochronous data TX/RX buffer
 *
 * @param[in]  index  Index of Isochronous data TX/RX buffer
 ****************************************************************************************
 */
void ble_util_buf_iso_free(uint8_t index);

/**
 ****************************************************************************************
 * @brief Initialize BLE ISO descriptors management system
 *
 * @param[in] init_type  Type of initialization (@see enum rwip_init_type)
 ****************************************************************************************
 */
void ble_util_isodesc_init(uint8_t init_type);

/**
 ****************************************************************************************
 * @brief Allocate an ISO descriptor
 *
 * @return Descriptor index allocated
 ****************************************************************************************
 */
uint8_t ble_util_isodesc_alloc(void);

/**
 ****************************************************************************************
 * @brief Free an ISO descriptor
 *
 * @param[in] desc_idx Index number of ISO descriptor
 ****************************************************************************************
 */
void ble_util_isodesc_free(uint8_t desc_idx);

#endif // (BLE_ISO_PRESENT)

/// @} BLE_UTIL_BUF

#endif // BLE_UTIL_BUF_H_
