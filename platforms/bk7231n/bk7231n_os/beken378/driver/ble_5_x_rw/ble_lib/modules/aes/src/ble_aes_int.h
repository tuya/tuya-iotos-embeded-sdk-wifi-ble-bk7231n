/**
 ****************************************************************************************
 *
 * @file ble_aes_int.h
 *
 * @brief Header file for AES Internal crypto module
 *
 * Copyright (C) RivieraWaves 2017-2018
 *
 ****************************************************************************************
 */

#ifndef AES_INT_H_
#define AES_INT_H_

/**
 ****************************************************************************************
 * @defgroup AES_INT
 * @ingroup AES
 * @brief  AES_INT Crypto internal definitions
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "rwip_config.h"
#include "ble_aes.h"


#include "common_bt.h"      // Common defines
#include "common_list.h"    // List usage

/*
 * Defines
 ****************************************************************************************
 */

// Size of an a AES Message block in bytes
#define AES_BLOCK_SIZE 16

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */
struct ble_aes_func_env;

/**
 ****************************************************************************************
 * Callback used to continue execution of the AES based function.
 *
 * @param[in] ble_aes_result Result of the AES (aes(key , val))
 *
 * @return True if function execution is over
 ****************************************************************************************
 */
typedef  bool (*ble_aes_func_continue_cb) (struct ble_aes_func_env* ble_aes_env, uint8_t* ble_aes_result);

/// Environment variable required for an AES based function
/// This structure must be Header of all function environment variables
struct ble_aes_func_env
{
    /// used to put AES function in the AES execution queue
    struct common_list_hdr    hdr;
    /// AES continue callback
    ble_aes_func_continue_cb  ble_aes_continue_cb;
    /// AES End callback
    ble_aes_func_result_cb    ble_aes_res_cb;

    /// Key to use for the AES execution
    const uint8_t*       key;
    /// Value to use for AES Cypher/Decypher
    const uint8_t*       val;
    /// Information put in source id message to retrieve requester
    uint32_t             src_info;
};

#if (BLE_EMB_PRESENT || BLE_HOST_PRESENT)
// Structure definition of the AES CMAC algorithm
struct ble_aes_cmac_env
{
    /// AES Environment structure
    struct ble_aes_func_env ble_aes_env;

    /// M: Pointer to the message to be authenticated
    const uint8_t*      message; // pointer to memory allocated by calling function

    /// K: authentication key
    const uint8_t*      auth_key;

    /// T: message authentication code
    uint8_t             auth_code[AES_BLOCK_SIZE];

    /// Length of the message
    uint16_t            message_len;
    ///  Number of blocks (1 block = 16 bytes)
    uint8_t             num_blocks;
    /// Current block to process
    uint8_t             cur_block;
};
#endif // (BLE_EMB_PRESENT || BLE_HOST_PRESENT)

/*
 * GLOBAL VARIABLE DECLARATIONS
 ****************************************************************************************
 */

/// zero block
extern const uint8_t ble_aes_cmac_zero[AES_BLOCK_SIZE];

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Allocate environment for the AES based function execution
 *
 * @param[in] size              Size of the environment to allocate (must be >= sizeof(struct ble_aes_func_env))
 * @param[in] ble_aes_continue_cb   Callback used to send the AES result
 * @param[in] res_cb            Function that will handle end of AES Based algorithm
 * @param[in] src_info          Information used to retrieve requester
 *
 * @return Allocated environment variable
 ****************************************************************************************
 */
struct ble_aes_func_env* ble_aes_alloc(uint16_t size, ble_aes_func_continue_cb ble_aes_continue_cb, ble_aes_func_result_cb res_cb,
                               uint32_t src_info);

/**
 ****************************************************************************************
 * @brief AES Cypher request function.
 *
 * This will queue AES request in the AES execution queue
 * When the AES result is received, the AES continue callback is executed.
 *
 * If AES continue function returns that AES execution is over, a message will be send to destination task
 * with latest AES result.
 *
 * @param[in] env  AES environment
 * @param[in] key  Key used for cyphering
 * @param[in] val  Value to cypher
 ****************************************************************************************
 */
void ble_aes_start(struct ble_aes_func_env* env, const uint8_t* key, const uint8_t *val);

#if (BLE_EMB_PRESENT || BLE_HOST_PRESENT)
/**
 ****************************************************************************************
 * @brief Start the AES CMAC crypto function. Allocate memory for the CMAC and
 *        begins the subkey generation
 *
 * @param[in] enc          AES CMAC Environment
 * @param[in] key          Pointer to the Key to be used
 * @param[in] message      Pointer to the block of data the data on which the CMAC is performed
 * @param[in] message_len  Length (in bytes) of the block of data M
 ****************************************************************************************
 */
void ble_aes_cmac_start(struct ble_aes_cmac_env* env, const uint8_t* key, const uint8_t* message, uint16_t message_len);

/**
 ****************************************************************************************
 * @brief Continue AES CMAC algorithm
 *
 * @param[in] env       AES CMAC Environment
 * @param[in] ble_aes_res   AES Result
 *
 * @return True if algorithm is over, False else
 ****************************************************************************************
 */
bool ble_aes_cmac_continue(struct ble_aes_cmac_env* env, uint8_t* ble_aes_res);

/**
 * @brief Perform a XOR of two numbers.
 *
 * @param[out] result Output 128 bits number: result = a ^ b
 * @param[in]  a      first 128 bits operand
 * @param[in]  b      second 128 bits operand
 * @param[in]  size   number of bytes to XOR
 */
void ble_aes_xor_128(uint8_t* result, const uint8_t* a, const uint8_t* b, uint8_t size);

/**
 * @brief Perform shift left of a 128 bits numvber
 *
 * @param[in]  input  Input 128 bits number
 * @param[out] output Output 128 bits number: output = input << 1
 */
void ble_aes_shift_left_128(const uint8_t* input,uint8_t* output);
#endif // (BLE_EMB_PRESENT || BLE_HOST_PRESENT)

/// @} AES_INT
///
#endif /* AES_INT_H_ */
