/*
 * @Author: wls
 * @email: wuls@tuya.com
 * @LastEditors: wls
 * @file name: light_init.h
 * @Description: 灯初始化程序头文件
 * @Copyright: 杭州涂鸦信息技术有限公司
 * @Company: http://www.tuya.com
 * @Date: 2019-05-09 10:49:13
 * @LastEditTime: 2019-05-15 16:54:29
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
