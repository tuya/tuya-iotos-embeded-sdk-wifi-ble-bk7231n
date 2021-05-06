/**
 * @file tuya_os_adapt_queue.c
 * @brief 队列操作接口
 *
 * @copyright Copyright(C),2018-2020, 涂鸦科技 www.tuya.com
 *
 */

#include "tuya_os_adapt_queue.h"
#include "tuya_os_adapt_system.h"
#include "tuya_os_adapt_memory.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"


/***********************************************************
*************************micro define***********************
***********************************************************/
typedef struct {
    xQueueHandle queue;
} QUEUE_MANAGE, *P_QUEUE_MANAGE;


/***********************************************************
*************************variable define********************
***********************************************************/

static const TUYA_OS_QUEUE_INTF m_tuya_os_queue_intfs = {
    .init    = tuya_os_adapt_queue_create_init, 
    .free    = tuya_os_adapt_queue_free,
    .post    = tuya_os_adapt_queue_post,
    .fetch   = tuya_os_adapt_queue_fetch,
};

/***********************************************************
*************************function define********************
***********************************************************/
/**
 * @brief create queue
 *
 * @param[out]     queue      queue to be create
 * @param[in]      size       the deep of the queue
 * @return  OPRT_OS_ADAPTER_OK: SUCCESS other:fail
 */
int tuya_os_adapt_queue_create_init(QUEUE_HANDLE *queue, int size)
{
    P_QUEUE_MANAGE pQueueManage;

    if (!queue) {
        return OPRT_OS_ADAPTER_INVALID_PARM;
    }

    *queue = NULL;

    pQueueManage = (P_QUEUE_MANAGE)tuya_os_adapt_system_malloc(sizeof(QUEUE_MANAGE));
    if (pQueueManage == NULL) {
        return OPRT_OS_ADAPTER_MALLOC_FAILED;
    }

    pQueueManage->queue = xQueueCreate(size, sizeof(void *));
    if (pQueueManage->queue == NULL) {
        tuya_os_adapt_system_free(pQueueManage);
        return OPRT_OS_ADAPTER_QUEUE_CREAT_FAILED;
    } else {
        *queue = (QUEUE_HANDLE)pQueueManage;
    }

    return OPRT_OS_ADAPTER_OK;
}

/**
 * @brief free queue
 *
 * @param[in]     queue      queue to be free
 * @return  void
 */
void tuya_os_adapt_queue_free(QUEUE_HANDLE queue)
{
    P_QUEUE_MANAGE pQueueManage;
    pQueueManage = (P_QUEUE_MANAGE)queue;

    if (!queue) {
        return ;
    }

    if (uxQueueMessagesWaiting(pQueueManage->queue)) {
        /* Line for breakpoint.  Should never break here! */
        portNOP();
        // TODO notify the user of failure.
    }

    vQueueDelete(pQueueManage->queue);
    tuya_os_adapt_system_free(pQueueManage);

}

/**
 * @brief fetch msg from queue in timeout ms
 *
 * @param[in]      queue      queue to post
 * @param[in]      msg        msg to post
 * @param[in]      timeout    max time to wait for msg(ms), TUYA_OS_ADAPT_QUEUE_FOREVER means forever wait
 * @return  int OPRT_OS_ADAPTER_OK:success    other:fail
 */
int tuya_os_adapt_queue_post(QUEUE_HANDLE queue, void *msg, unsigned int timeout)
{
    int ret = pdPASS;
    P_QUEUE_MANAGE pQueueManage;
    pQueueManage = (P_QUEUE_MANAGE)queue;

    if (!queue) {
        return OPRT_OS_ADAPTER_INVALID_PARM;
    }

    if(FALSE == tuya_os_adapt_system_isrstatus()) {
        ret = xQueueSend( pQueueManage->queue, &msg, (timeout==TUYA_OS_ADAPT_QUEUE_FOREVER)?portMAX_DELAY:(timeout/ tuya_os_adapt_get_tickratems()));
    } else {
        signed portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
        ret = xQueueSendFromISR( pQueueManage->queue, &msg, &xHigherPriorityTaskWoken);
        portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
    }
    
    if (pdPASS == ret) {
        return OPRT_OS_ADAPTER_OK;
    } else {
        // could not post, queue must be full
        return OPRT_OS_ADAPTER_QUEUE_SEND_FAIL;
    }
}

/**
 * @brief fetch msg from queue in timeout ms
 *
 * @param[in]      queue      queue to post
 * @param[out]     msg        msg to fetch
 * @param[in]      timeout    max time to wait for msg(ms), TUYA_OS_ADAPT_QUEUE_FOREVER means forever wait
 * @return  int OPRT_OS_ADAPTER_OK:success    other:fail
 */
int tuya_os_adapt_queue_fetch(QUEUE_HANDLE queue, void **msg, unsigned int timeout)
{
    void *dummyptr;
    P_QUEUE_MANAGE pQueueManage;
    pQueueManage = (P_QUEUE_MANAGE)queue;

    if (!queue) {
        return OPRT_OS_ADAPTER_INVALID_PARM;
    }

    if (msg == NULL) {
        msg = &dummyptr;
    }

    if (pdTRUE == xQueueReceive(pQueueManage->queue, &(*msg), (timeout == TUYA_OS_ADAPT_QUEUE_FOREVER) ? portMAX_DELAY : (timeout / tuya_os_adapt_get_tickratems()))) {
        return OPRT_OS_ADAPTER_OK;
    } else { // timed out blocking for message
        return OPRT_OS_ADAPTER_QUEUE_RECV_FAIL;
    }
}

OPERATE_RET tuya_os_adapt_reg_queue_intf(void)
{
    return tuya_os_adapt_reg_intf(INTF_QUEUE, (void *)&m_tuya_os_queue_intfs);
}


