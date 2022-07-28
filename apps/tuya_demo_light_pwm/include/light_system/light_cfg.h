/**
* @file light_cfg.h
* @author www.tuya.com
* @brief 
* @version 0.2
* @date 2022-03-25
*
* @copyright Copyright (c) tuya.inc 2022
*
*/

#ifndef __LIHGT_CFG_H__
#define __LIHGT_CFG_H__


#include "light_types.h"


#ifdef __cplusplus
extern "C" {
#endif	//__cplusplus


/**
 * @brief: 灯框架支持的功能选择
 */
#define LIGHT_CFG_INIT_PARAM_CHECK          0   //灯初始化的时候是否检查参数值
#define LIGHT_CFG_SUPPORT_LOWPOWER          1   //是否支持低功耗
#define LIGHT_CFG_PROD_DRIVER_NEED_INIT     0   //production时驱动是否初始化
#define LIGHT_CFG_ENABLE_GAMMA              1   //使能伽马
#define LIGHT_CFG_ENABLE_GAMMA_CAL          0   //使能伽马计算


#ifdef __cplusplus
}
#endif  //__cplusplus


#endif  //__LIHGT_CFG_H__
