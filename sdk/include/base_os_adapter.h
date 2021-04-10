/**
 * @file base_os_adapter.h
 * @author sunkz@tuya.com
 * @brief 
 * @version 0.1
 * @date 2020-05-08
 * 
 * @copyright Copyright (c) tuya.inc 2019
 * 
 */
#ifndef _BASE_OS_ADAPTER_H
#define _BASE_OS_ADAPTER_H


#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************
*************************micro define***********************
***********************************************************/

/***********************************************************
*************************variable define********************
***********************************************************/

/***********************************************************
*************************function define********************
***********************************************************/

/**
 * @brief tuya_os_intf_init用于系统接口的初始化
 * 
 * @return int 0，成功；非0，失败
 */
int tuya_os_intf_init(void);


#ifdef __cplusplus
}
#endif


#endif /* _TUYA_OS_ADAPTER_H */
