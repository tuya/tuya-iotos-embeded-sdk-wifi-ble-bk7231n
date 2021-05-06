/**
 * @file tuya_os_adapt_memory.c
 * @brief 内存操作接口
 * 
 * @copyright Copyright(C),2018-2020, 涂鸦科技 www.tuya.com
 * 
 */

#include "tuya_os_adapt_memory.h"
#include "tuya_os_adapt_output.h"
#include "tuya_os_adapt_system.h"
#include <FreeRTOS.h>



/***********************************************************
*************************micro define***********************
***********************************************************/

/***********************************************************
*************************variable define********************
***********************************************************/
static const TUYA_OS_MEMORY_INTF m_tuya_os_memory_intfs = {
    .malloc  = tuya_os_adapt_system_malloc, 
    .free    = tuya_os_adapt_system_free,
};

/***********************************************************
*************************function define********************
***********************************************************/
/**
 * @brief tuya_os_adapt_system_malloc用于分配内存
 * 
 * @param[in]       size        需要分配的内存大小
 * @return  分配得到的内存指针
 */


#if HAL_MEM_DEBUG
static int	start_record_os_adapt_mem = 0;

void tuya_os_adapt_system_mem_start(void)
{
	start_record_os_adapt_mem = 1;
}
void tuya_os_adapt_system_mem_stop(void)
{
	start_record_os_adapt_mem = 0;
}
int tuya_os_adapt_system_mem_get(void)
{
	return start_record_os_adapt_mem;
}
static int malloc_cnt = 0;
void *__tuya_os_adapt_system_malloc(size_t size,char *file,int line)
#else
void *tuya_os_adapt_system_malloc(const size_t size)
#endif
{
    void *pMalloc;

    pMalloc = pvPortMalloc(size);
#if HAL_MEM_DEBUG
    if(start_record_os_adapt_mem) {
        malloc_cnt++;
        if (file) {
            if(strstr(file,"print_") || strstr(file,"parse_") || \
               strstr(file,"cJSON_")) {
                return pMalloc;
            }
        }

    LOG_DEBUG("%s:%d cnt:%d malloc mp:%p reqSize:%d \r\n",file?file:"UNKNOWN",line,malloc_cnt,pMalloc,size);
    }
#endif
    if(pMalloc == NULL) {
        LOG_ERR("malloc fail, heap left size %d\r\n", tuya_os_adapt_system_getheapsize());
    }
    return pMalloc;
}

/**
 * @brief tuya_os_adapt_system_free用于释放内存
 * 
 * @param[in]       ptr         需要释放的内存指针
 */
#if HAL_MEM_DEBUG
static int free_cnt = 0;
void __tuya_os_adapt_system_free(void      * ptr,char *file,int line)
#else
void tuya_os_adapt_system_free(void* ptr)
#endif
{
    if(ptr == NULL) {
        return;
    }

	vPortFree(ptr);

#if HAL_MEM_DEBUG
    if(start_record_os_adapt_mem) {
        free_cnt++;

        // delete cjson print info
        if (file) {
            if(strstr(file,"print_") || strstr(file,"parse_") || \
               strstr(file,"cJSON_")) {
                return;
            }
        }       

        LOG_DEBUG("%s:%d sub_cnt:%d free mp:%p\r\n",file?file:"UNKNOWN",line,(malloc_cnt-free_cnt),ptr);
    }
#endif
}

OPERATE_RET tuya_os_adapt_reg_memory_intf(void)
{
    return tuya_os_adapt_reg_intf(INTF_MEMORY, (void *)&m_tuya_os_memory_intfs);
}

