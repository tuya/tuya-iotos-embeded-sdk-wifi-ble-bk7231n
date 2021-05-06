/**
 ****************************************************************************************
 *
 * @file ecc_p256.h
 *
 * @brief  ECC functions for P256
 *
 * Copyright (C) RivieraWaves 2009-2015
 *
 ****************************************************************************************
 */

#ifndef ECC_P256_H_
#define ECC_P256_H_


/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "rwip_config.h"
#include <stdint.h>
#include <stdbool.h>
#include "kernel_task.h"

/*
 * DEFINES
 ****************************************************************************************
 */

#define ECC_PUBLICKEY_GENERATION 0x01
#define ECC_DHKEY_GENERATION     0x02


/*
 * STRUCTURE DEFINITIONS
 ****************************************************************************************
 */

/// Multiplication result message structure
/*@TRACE*/
struct ecc_result_ind
{
    uint8_t key_res_x[32];
    uint8_t key_res_y[32];
};


/*
 * VARIABLE DECLARATION
 ****************************************************************************************
 */

/// Debug Private Key
extern const uint8_t DebugE256SecretKey[32];


/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialize Elliptic Curve algorithm
 *
 * @param[in] init_type  Type of initialization (@see enum rwip_init_type)
 ****************************************************************************************
 */
void ecc_init(uint8_t init_type);

/**
 ****************************************************************************************
 * @brief Generate a Secret Key compliant with ECC P256 algorithm
 *
 * If key is forced, just check its validity
 *
 * @param[out] secret_key Private key - MSB First
 * @param[in]  forced True if provided key is forced, else generate it.
 ****************************************************************************************
 */
void ecc_gen_new_secret_key(uint8_t* secret_key, bool forced);

/**
 ****************************************************************************************
 * @brief Generate a new Public key pair using ECC P256 algorithm
 *
 * @param[in] secret_key Private key - MSB First
 * @param[in] blocking   Force to run full algorithm without continue mode
 ****************************************************************************************
 */
void ecc_gen_new_public_key(uint8_t* secret_key256, kernel_msg_id_t msg_id, kernel_task_id_t task_id);

/**
 ****************************************************************************************
 * @brief Generate a new DHKey using ECC P256 algorithm
 *
 * @param[in] key_type   Type of key to generate (1: public key | 2: DH key)
 * @param[in] secret_key Private key                  - MSB First
 * @param[in] pub_key_x  Peer public key x coordinate - LSB First
 * @param[in] pub_key_y  Peer public key y coordinate - LSB First
 * @param[in] msg_id     Message task ID for the result indication
 * @param[in] task_id    Client task ID (Task type + instance)
 *
 * @return status   0 if key generation is started, > 0 otherwise
 ****************************************************************************************
 */
uint8_t ecc_generate_key256(uint8_t key_type, const uint8_t* secret_key, const uint8_t* public_key_x, const uint8_t* public_key_y, kernel_msg_id_t msg_id, kernel_task_id_t task_id);

/**
 ****************************************************************************************
 * @brief Abort a current DHKey generation procedure
 *
 * @param[in] task_id    Client task ID (Task type + instance)
 ****************************************************************************************
 */
void ecc_abort_key256_generation(kernel_task_id_t task_id);

/**
 ****************************************************************************************
 * @brief Retrieve debug private and public keys
 *
 * @param[out] secret_key Private key             - MSB First
 * @param[out] pub_key_x  Public key x coordinate - LSB First
 * @param[out] pub_key_y  Public key y coordinate - LSB First
 ****************************************************************************************
 */
void ecc_get_debug_Keys(uint8_t*secret_key, uint8_t* pub_key_x, uint8_t* pub_key_y);


#endif /* ECC_P256_H_ */
