/**
 * @file uni_random.h
 * @brief tuya random module
 * @version 1.0
 * @date 2019-10-30
 * 
 * @copyright Copyright (c) tuya.inc 2019
 * 
 */

#ifndef __UNI_RANDOM_H__
#define __UNI_RANDOM_H__


#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief generate a random 32bit unsigned integer
 * 
 * @return the random 32bit unsigned integer
 */
uint32_t uni_random(void);

/**
 * @brief generate a random string
 * 
 * @param[out] dst the output random string
 * @param[in] size the length of the random string
 * @return 0 on success, others on failed. 
 */
int uni_random_string(char* dst, int size);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __UNI_RANDOM_H__ */
