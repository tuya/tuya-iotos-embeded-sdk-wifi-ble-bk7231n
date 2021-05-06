/**
 * @file adapter_platform.c
 * @brief 平台适配操作接口
 * 
 * @copyright Copyright(C),2018-2020, 涂鸦科技 www.tuya.com
 * 
 */

#define _ADAPTER_PLATFORM_GLOBAL
#include "tuya_os_adapter.h"
#include "tuya_os_adapter_error_code.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _MEM_POOL_GLOBAL
    #define _MEM_POOL_EXT
#else
    #define _MEM_POOL_EXT extern
#endif


#if 0
#define LOGD PR_DEBUG
#define LOGT PR_TRACE
#define LOGN PR_NOTICE
#define LOGE PR_ERR
#else
#define LOGD(...) DiagPrintf("[PLATFORM DEBUG]" __VA_ARGS__)
#define LOGT(...) DiagPrintf("[PLATFORM TRACE]" __VA_ARGS__)
#define LOGN(...) DiagPrintf("[PLATFORM NOTICE]" __VA_ARGS__)
#define LOGE(...) DiagPrintf("[PLATFORM ERROR]" __VA_ARGS__)
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
/***********************************************************
*  Function: adapter_platform_init
*  Input: none
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
int tuya_os_adapter_init(void)
{
    return 0;
}

