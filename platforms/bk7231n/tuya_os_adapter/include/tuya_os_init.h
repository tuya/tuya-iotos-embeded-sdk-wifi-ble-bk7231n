/**
 * @file tuya_os_adapter.h
 * @author sunkz@tuya.com
 * @brief 
 * @version 0.1
 * @date 2020-05-15
 * 
 * @copyright Copyright (c) tuya.inc 2019
 * 
 */
#ifndef _TUYA_OS_INIT_H
#define _TUYA_OS_INIT_H

#ifdef __cplusplus
extern "C" {
#endif

#define TUYA_OS_ADAPT_PLATFORM_VERSION_1_1_0  "1.1.0"
#define TUYA_OS_ADAPT_PLATFORM_VERSION_1_1_1  "1.1.1"

#define TUYA_OS_ADAPT_PLATFORM_VERSION        TUYA_OS_ADAPT_PLATFORM_VERSION_1_1_1


void tuya_os_init(void);
char* tuya_os_adapt_get_ver(void);


#ifdef __cplusplus
}
#endif

#endif /* _TUYA_OS_ADAPTER_H */
