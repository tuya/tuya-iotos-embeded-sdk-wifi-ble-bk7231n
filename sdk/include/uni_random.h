/*
uni_random.h
Copyright(C),2018-2020, 涂鸦科技 www.tuya.comm
*/

#ifndef __UNI_RANDOM_H__
#define __UNI_RANDOM_H__


#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif


uint32_t uni_random(void);

int uni_random_string(char* dst, int size);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __UNI_RANDOM_H__ */
