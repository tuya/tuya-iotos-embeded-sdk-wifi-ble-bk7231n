/**
* @file light_init.h
* @author www.tuya.com
* @brief light initialization program
* @version 0.2
* @date 2022-03-25
*
* @copyright Copyright (c) tuya.inc 2022
*
*/

#ifndef __LIHGT_INIT_H__
#define __LIHGT_INIT_H__


#include "light_types.h"

#ifdef __cplusplus
extern "C" {
#endif  //__cplusplus 

/**
 * @brief : 灯初始化
 * @param : none 
 * @return: OPERATE_LIGHT 0表示成功，非0请参照tuya error code描述文档 
 */
OPERATE_LIGHT tuya_light_init(VOID);


#ifdef __cplusplus
}
#endif //__cplusplus


#endif  //__LIHGT_INIT_H__
