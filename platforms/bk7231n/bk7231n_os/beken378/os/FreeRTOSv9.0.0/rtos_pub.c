#include <string.h>
#include <stdlib.h>

#include "rtos_error.h"
#include "common.h"
#include "sys_rtos.h"
#include "timers.h"
#include "rtos_pub.h"
#include "generic.h"

#include "includes.h"
#include "uart_pub.h"
#include "string.h"

/******************************************************
 *                      Macros
 ******************************************************/

#define cmd_printf os_printf

/******************************************************
 *                    Constants
 ******************************************************/
#ifndef TIMER_THREAD_STACK_SIZE
#define TIMER_THREAD_STACK_SIZE      1024 + 4*1024
#endif
#define TIMER_QUEUE_LENGTH  5

/*
 * Macros used by vListTask to indicate which state a task is in.
 */
#define tskBLOCKED_CHAR     ( 'B' )
#define tskREADY_CHAR       ( 'R' )
#define tskDELETED_CHAR     ( 'D' )
#define tskSUSPENDED_CHAR   ( 'S' )

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

typedef tmrTIMER_CALLBACK native_timer_handler_t;
typedef pdTASK_CODE       native_thread_t;

/******************************************************
 *                    Structures
 ******************************************************/

typedef struct
{
    event_handler_t function;
    void*           arg;
} timer_queue_message_t;

typedef struct
{
    event_handler_t function;
    void*           arg;
} beken_event_message_t;


/******************************************************
 *               Function Declarations
 ******************************************************/
extern void rtos_stack_overflow(char *taskname);
extern uint32_t platform_is_in_interrupt_context( void );
#if (CFG_SOC_NAME == SOC_BK7231N)
OSStatus rtos_push_to_queue( beken_queue_t* queue, void* message, uint32_t timeout_ms ) __SECTION(".itcm");
#endif

/******************************************************
 *               Variables Definitions
 ******************************************************/
xTaskHandle  app_thread_handle;
static beken_time_t beken_time_offset = 0;


uint32_t  max_syscall_int_prio;
uint32_t  ms_to_tick_ratio = 2;

uint32_t rtos_max_priorities = RTOS_HIGHEST_PRIORITY - RTOS_LOWEST_PRIORITY + 1;

/******************************************************
 *               Function Definitions
 ******************************************************/
OSStatus rtos_create_thread( beken_thread_t* thread, uint8_t priority, const char* name, 
                        beken_thread_function_t function, uint32_t stack_size, beken_thread_arg_t arg )
{
    /* Limit priority to default lib priority */
    if ( priority > RTOS_HIGHEST_PRIORITY )
    {
        priority = RTOS_HIGHEST_PRIORITY;
    }

    if( pdPASS == _xTaskCreate( (native_thread_t)function, name, 
            (unsigned short) (stack_size/sizeof( portSTACK_TYPE )), 
            (void *)arg, BK_PRIORITY_TO_NATIVE_PRIORITY(priority), 
            thread ) )
    {
        return kNoErr;
    }
    else
    {
        return kGeneralErr;
    }
}

OSStatus rtos_delete_thread( beken_thread_t* thread )
{
    if ( thread == NULL )
    {
        vTaskDelete( NULL );
    }
    else if ( xTaskIsTaskFinished( *thread ) != pdTRUE )
    {
        vTaskDelete( *thread );
    }
    return kNoErr;
}

OSStatus rtos_thread_join(beken_thread_t* thread)
{
    while ( xTaskIsTaskFinished( *thread ) != pdTRUE )
    {
        rtos_delay_milliseconds(10);
    }
    
    return kNoErr;
}

BOOL rtos_is_current_thread( beken_thread_t* thread )
{
    if ( xTaskGetCurrentTaskHandle() == *thread )
    {
        return true;
    }
    else
    {
        return false;
    }
}

beken_thread_t* rtos_get_current_thread( void )
{
    return (beken_thread_t *)xTaskGetCurrentTaskHandle();
}

#if FreeRTOS_VERSION_MAJOR == 7
/* Old deployment, may has some problem */
OSStatus rtos_print_thread_status( char* pcWriteBuffer, int xWriteBufferLen )
{
    cmd_printf("%-30s Status  Prio    Stack   TCB\r\n", "Name");
    cmd_printf("------------------------------------------------------------");
    if (xWriteBufferLen >= 256)
        vTaskList((signed char *)pcWriteBuffer);
    else {
        char buf[256];

        vTaskList((signed char *)buf);
        strncpy(pcWriteBuffer, buf, xWriteBufferLen);
    }
    return kNoErr;
}

#else
static char *prvWriteNameToBuffer( char *pcBuffer, const char *pcTaskName )
{
    long x;

    /* Start by copying the entire string. */
    strcpy( pcBuffer, pcTaskName );

    /* Pad the end of the string with spaces to ensure columns line up when
    printed out. */
    for( x = strlen( pcBuffer ); x < ( configMAX_TASK_NAME_LEN - 1 ); x++ )
    {
        pcBuffer[ x ] = ' ';
    }

    /* Terminate. */
    pcBuffer[ x ] = 0x00;

    /* Return the new end of string. */
    return &( pcBuffer[ x ] );
}

/* Re-write vTaskList to add a buffer size parameter */
OSStatus rtos_print_thread_status( char* pcWriteBuffer, int xWriteBufferLen )
{
    TaskStatus_t *pxTaskStatusArray;
    unsigned portBASE_TYPE uxCurrentNumberOfTasks = uxTaskGetNumberOfTasks();
    volatile UBaseType_t uxArraySize, x;
    char cStatus;
    char pcTaskStatusStr[48];
    char *pcTaskStatusStrTmp;

    /* Make sure the write buffer does not contain a string. */
    *pcWriteBuffer = 0x00;

    /* Take a snapshot of the number of tasks in case it changes while this
    function is executing. */
    uxArraySize = uxCurrentNumberOfTasks;

    /* Allocate an array index for each task.  NOTE!  if
     configSUPPORT_DYNAMIC_ALLOCATION is set to 0 then pvPortMalloc() will
     equate to NULL. */
    pxTaskStatusArray = pvPortMalloc( uxCurrentNumberOfTasks * sizeof(TaskStatus_t) );

    cmd_printf("%-12s Status     Prio    Stack   TCB\r\n", "Name");
    cmd_printf("-------------------------------------------\r\n");

    xWriteBufferLen-=strlen(pcWriteBuffer);

    if ( pxTaskStatusArray != NULL )
    {
        /* Generate the (binary) data. */
        uxArraySize = uxTaskGetSystemState( pxTaskStatusArray, uxArraySize, NULL );

        /* Create a human readable table from the binary data. */
        for ( x = 0; x < uxArraySize; x++ )
        {
            switch ( pxTaskStatusArray[x].eCurrentState )
            {
                case eReady:
                    cStatus = tskREADY_CHAR;
                    break;

                case eBlocked:
                    cStatus = tskBLOCKED_CHAR;
                    break;

                case eSuspended:
                    cStatus = tskSUSPENDED_CHAR;
                    break;

                case eDeleted:
                    cStatus = tskDELETED_CHAR;
                    break;

                default: /* Should not get here, but it is included
                 to prevent static checking errors. */
                    cStatus = 0x00;
                    break;
            }

            /* Write the task name to the string, padding with spaces so it
             can be printed in tabular form more easily. */
            pcTaskStatusStrTmp = pcTaskStatusStr;
            pcTaskStatusStrTmp = prvWriteNameToBuffer( pcTaskStatusStrTmp, pxTaskStatusArray[x].pcTaskName );
            //pcWriteBuffer = prvWriteNameToBuffer( pcWriteBuffer, pxTaskStatusArray[x].pcTaskName );

            /* Write the rest of the string. */
            sprintf( pcTaskStatusStrTmp, "\t%c\t%u\t%u\t%u\r\n", cStatus,
                     BK_PRIORITY_TO_NATIVE_PRIORITY((unsigned int) pxTaskStatusArray[x].uxCurrentPriority),
                     (unsigned int) pxTaskStatusArray[x].usStackHighWaterMark,
                     (unsigned int) pxTaskStatusArray[x].xTaskNumber );

            if( xWriteBufferLen < strlen( pcTaskStatusStr ) )
            {
                for ( x = 0; x < xWriteBufferLen; x++ )
                {
                    *(pcWriteBuffer+x) = '.';
                }
                break;
            }
            else
            {
                strncpy( pcWriteBuffer, pcTaskStatusStr, xWriteBufferLen);
                xWriteBufferLen -= strlen( pcTaskStatusStr );
                pcWriteBuffer += strlen( pcWriteBuffer );
            }
        }

        /* Free the array again.  NOTE!  If configSUPPORT_DYNAMIC_ALLOCATION
         is 0 then vPortFree() will be #defined to nothing. */
        vPortFree( pxTaskStatusArray );
    }
    else
    {
        mtCOVERAGE_TEST_MARKER();
    }

    return kNoErr;
}
#endif

OSStatus rtos_check_stack( void )
{
    //  TODO: Add stack checking here.
    return kNoErr;
}

OSStatus rtos_thread_force_awake( beken_thread_t* thread )
{
#if FreeRTOS_VERSION_MAJOR < 9
    vTaskForceAwake(*thread);
#else
    xTaskAbortDelay(*thread);
#endif
    return kNoErr;
}

OSStatus rtos_print_stack(beken_thread_t* thread)
{
    extern void fiq_handler();
    extern void _exit(int status);
    extern char _itcmcode_ram_begin;
    extern char _itcmcode_ram_end;

    beken_thread_t cur_thread;
    TaskStatus_t xTaskStatus;
    uint32_t call_stack_buf[32] = {0};
    uint32_t code_start_addr;
    uint32_t code_end_addr;
    uint32_t stack_start_addr;
    uint32_t stack_end_addr;
    uint32_t sp;
    uint32_t pc;
    int call_stack_index = 0;

    /* TODO: it should be more portable to calculate code space, like _stext _etext */
    code_start_addr = (uint32_t)fiq_handler;
    code_end_addr = (uint32_t)(&_exit);

    if (NULL == thread) {
        cur_thread = rtos_get_current_thread();
        thread = &cur_thread;
    }

    memset(&xTaskStatus, 0x00, sizeof(xTaskStatus));
    vTaskGetInfo(*thread, &xTaskStatus, pdFALSE, eInvalid);

    /**
      * WARNING: FreeRTOS can't return stack_size or stack_pointer,
      * since TaskHandle_t -> TCB_t*
      * and pxTopOfStack is the first member of TCB_t
      */
    stack_start_addr   = (uint32_t)xTaskStatus.pxStackBase;
    stack_end_addr   = stack_start_addr + 4096;
    sp = *(uint32_t*)(xTaskStatus.xHandle);
    bk_printf("task=%.16s,stack=[0x%x,0x%x],sp=0x%x,hdl=0x%x\n", xTaskStatus.pcTaskName, stack_start_addr, stack_end_addr, sp, xTaskStatus.xHandle);
    bk_printf("code=[0x%x,0x%x],itcm=[0x%x,0x%x]\n", code_start_addr, code_end_addr, &_itcmcode_ram_begin, &_itcmcode_ram_end);
#if portSTACK_GROWTH < 0
    for (; sp < stack_end_addr; sp += sizeof(size_t))
#else
    for (; sp >= stack_start_addr; sp -= sizeof(size_t))
#endif
    {
        pc = *((uint32_t *) sp);
        /* FreeRTOS using arm other than thumb instruction, so the pc must be an even number */
        if ((pc & 1) == 0) {
            //continue;
        }
        if (((code_start_addr < pc) && (pc < code_end_addr))
#if (CFG_SOC_NAME == SOC_BK7231N)
        || (((uint32_t)&_itcmcode_ram_begin <= pc) && (pc < (uint32_t)&_itcmcode_ram_end))
#endif
        ) {
            call_stack_buf[call_stack_index] = pc;
            call_stack_index++;
            if (call_stack_index >= sizeof(call_stack_buf) / sizeof(call_stack_buf[0])) {
                break;
            }
        }
    }
    if (call_stack_index > 0)
    {
        int index;
        bk_printf("arm-none-eabi-addr2line -e beken7231n.elf -a -f");
        for (index = 0; index < call_stack_index; index++) {
            bk_printf(" %lx", call_stack_buf[index]);
        }
        bk_printf("\n");
    }
    return kNoErr;
}

OSStatus rtos_print_stacks()
{
    TaskStatus_t *pxTaskStatusArray;
    unsigned portBASE_TYPE uxCurrentNumberOfTasks = uxTaskGetNumberOfTasks();
    volatile UBaseType_t uxArraySize, x;

    /* Take a snapshot of the number of tasks in case it changes while this
    function is executing. */
    uxArraySize = uxCurrentNumberOfTasks;

    /* Allocate an array index for each task.  NOTE!  if
     configSUPPORT_DYNAMIC_ALLOCATION is set to 0 then pvPortMalloc() will
     equate to NULL. */
    pxTaskStatusArray = pvPortMalloc( uxCurrentNumberOfTasks * sizeof(TaskStatus_t) );

    if ( pxTaskStatusArray != NULL )
    {
        /* Generate the (binary) data. */
        uxArraySize = uxTaskGetSystemState( pxTaskStatusArray, uxArraySize, NULL );

        /* Create a human readable table from the binary data. */
        for ( x = 0; x < uxArraySize; x++ )
        {
            rtos_print_stack(&pxTaskStatusArray[x].xHandle);
        }

        /* Free the array again.  NOTE!  If configSUPPORT_DYNAMIC_ALLOCATION
         is 0 then vPortFree() will be #defined to nothing. */
        vPortFree( pxTaskStatusArray );
    }

    return kNoErr;
}

void rtos_thread_sleep(uint32_t seconds)
{
    rtos_delay_milliseconds(seconds * 1000);
}

OSStatus beken_time_get_time(beken_time_t* time_ptr)
{
    *time_ptr = (beken_time_t) ( xTaskGetTickCount() * ms_to_tick_ratio ) + beken_time_offset;
    return kNoErr;
}

OSStatus beken_time_set_time(beken_time_t* time_ptr)
{
    beken_time_offset = *time_ptr - (beken_time_t) ( xTaskGetTickCount() * ms_to_tick_ratio );
    return kNoErr;
}

OSStatus rtos_init_semaphore( beken_semaphore_t* semaphore, int maxCount )
{
    *semaphore = xSemaphoreCreateCounting( (unsigned portBASE_TYPE) maxCount, (unsigned portBASE_TYPE) 0 );

    return ( *semaphore != NULL ) ? kNoErr : kGeneralErr;
}

OSStatus rtos_get_semaphore( beken_semaphore_t* semaphore, uint32_t timeout_ms )
{
    uint32_t time;

    if(timeout_ms == BEKEN_WAIT_FOREVER)
        time = portMAX_DELAY;
    else
        time = timeout_ms / ms_to_tick_ratio;
        
    if ( pdTRUE == xSemaphoreTake( *semaphore, (portTickType) (time) ) )
    {
        return kNoErr;
    }
    else
    {
        return kTimeoutErr;
    }
}
int rtos_get_sema_count( beken_semaphore_t* semaphore )
{
	return uxSemaphoreGetCount( *semaphore );
}

int rtos_set_semaphore( beken_semaphore_t* semaphore )
{
    signed portBASE_TYPE result;

    if ( platform_is_in_interrupt_context() == RTOS_SUCCESS )
    {
        signed portBASE_TYPE xHigherPriorityTaskWoken;
        result = xSemaphoreGiveFromISR( *semaphore, &xHigherPriorityTaskWoken );

        /* If xSemaphoreGiveFromISR() unblocked a task, and the unblocked task has
         * a higher priority than the currently executing task, then
         * xHigherPriorityTaskWoken will have been set to pdTRUE and this ISR should
         * return directly to the higher priority unblocked task.
         */
        portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
    }
    else
    {
        result = xSemaphoreGive( *semaphore );
    }

    return ( result == pdPASS )? kNoErr : kGeneralErr;
}

OSStatus rtos_deinit_semaphore( beken_semaphore_t* semaphore )
{
    if (semaphore != NULL)
    {
        vQueueDelete( *semaphore );
        *semaphore = NULL;
    }
    return kNoErr;
}

void rtos_enter_critical( void )
{
    vPortEnterCritical();
}

void rtos_exit_critical( void )
{
    vPortExitCritical();
}

OSStatus rtos_init_mutex( beken_mutex_t* mutex )
{
    /* Mutex uses priority inheritance */
    *mutex = xSemaphoreCreateMutex();
    if ( *mutex == NULL )
    {
        return kGeneralErr;
    }

    return kNoErr;
}

OSStatus rtos_lock_mutex( beken_mutex_t* mutex )
{
    if ( xSemaphoreTake( *mutex, BEKEN_WAIT_FOREVER ) != pdPASS )
    {
        return kGeneralErr;
    }

    return kNoErr;
}


OSStatus rtos_unlock_mutex( beken_mutex_t* mutex )
{
    if ( xSemaphoreGive( *mutex ) != pdPASS )
    {
        return kGeneralErr;
    }

    return kNoErr;
}

OSStatus rtos_deinit_mutex( beken_mutex_t* mutex )
{
    vSemaphoreDelete( *mutex );
    *mutex = NULL;
    return kNoErr;
}

OSStatus rtos_init_queue( beken_queue_t* queue, const char* name, uint32_t message_size, uint32_t number_of_messages )
{
    UNUSED_PARAMETER(name);

    if ( ( *queue = xQueueCreate( number_of_messages, message_size ) ) == NULL )
    {
        return kGeneralErr;
    }

    return kNoErr;
}

OSStatus rtos_push_to_queue( beken_queue_t* queue, void* message, uint32_t timeout_ms )
{
    signed portBASE_TYPE result;

    if ( platform_is_in_interrupt_context() == RTOS_SUCCESS )
    {
        signed portBASE_TYPE xHigherPriorityTaskWoken;
        result = xQueueSendToBackFromISR( *queue, message, &xHigherPriorityTaskWoken );

        /* If xQueueSendToBackFromISR() unblocked a task, and the unblocked task has
         * a higher priority than the currently executing task, then
         * xHigherPriorityTaskWoken will have been set to pdTRUE and this ISR should
         * return directly to the higher priority unblocked task.
         */
        portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
    }
    else
    {
        uint32_t time;
        
        if(timeout_ms == BEKEN_WAIT_FOREVER)
            time = portMAX_DELAY;
        else
            time = timeout_ms / ms_to_tick_ratio;
        
        result = xQueueSendToBack( *queue, message, (portTickType) ( time ) );
    }

    return ( result == pdPASS )? kNoErr : kGeneralErr;
}


OSStatus rtos_push_to_queue_front( beken_queue_t* queue, void* message, uint32_t timeout_ms )
{
    signed portBASE_TYPE result;

    if ( platform_is_in_interrupt_context() == RTOS_SUCCESS )
    {
        signed portBASE_TYPE xHigherPriorityTaskWoken;
        result = xQueueSendToFrontFromISR( *queue, message, &xHigherPriorityTaskWoken );

        /* If xQueueSendToBackFromISR() unblocked a task, and the unblocked task has
         * a higher priority than the currently executing task, then
         * xHigherPriorityTaskWoken will have been set to pdTRUE and this ISR should
         * return directly to the higher priority unblocked task.
         */
        portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
    }
    else
    {
        uint32_t time;
        
        if(timeout_ms == BEKEN_WAIT_FOREVER)
            time = portMAX_DELAY;
        else
            time = timeout_ms / ms_to_tick_ratio;
        
        result = xQueueSendToFront( *queue, message, (portTickType) ( time ) );
    }

    return ( result == pdPASS )? kNoErr : kGeneralErr;
}


OSStatus rtos_pop_from_queue( beken_queue_t* queue, void* message, uint32_t timeout_ms )
{
    uint32_t time;

    if(timeout_ms == BEKEN_WAIT_FOREVER)
        time = portMAX_DELAY;
    else
        time = timeout_ms / ms_to_tick_ratio;
    
    if ( xQueueReceive( *queue, message, ( time ) ) != pdPASS )
    {
        return kGeneralErr;
    }

    return kNoErr;
}


OSStatus rtos_deinit_queue( beken_queue_t* queue )
{
    vQueueDelete( *queue );
    *queue = NULL;
    return kNoErr;
}

BOOL rtos_is_queue_empty( beken_queue_t* queue )
{
    signed portBASE_TYPE result;

    taskENTER_CRITICAL();
    result = xQueueIsQueueEmptyFromISR( *queue );
    taskEXIT_CRITICAL();

    return ( result != 0 ) ? true : false;
}

BOOL rtos_is_queue_full( beken_queue_t* queue )
{
    signed portBASE_TYPE result;

    taskENTER_CRITICAL();
    result = xQueueIsQueueFullFromISR( *queue );
    taskEXIT_CRITICAL();

    return ( result != 0 ) ? true : false;
}

static void timer_callback2( xTimerHandle handle )
{
    beken2_timer_t *timer = (beken2_timer_t*) pvTimerGetTimerID( handle );

	if(BEKEN_MAGIC_WORD != timer->beken_magic)
	{
		return;
	}
    if ( timer->function )
    {
        timer->function( timer->left_arg, timer->right_arg );
    }
}

static void timer_callback1( xTimerHandle handle )
{
    beken_timer_t *timer = (beken_timer_t*) pvTimerGetTimerID( handle );

    if ( timer->function )
    {
        timer->function( timer->arg);
    }
}

OSStatus rtos_start_oneshot_timer( beken2_timer_t* timer )
{
    signed portBASE_TYPE result;

    if ( platform_is_in_interrupt_context() == RTOS_SUCCESS ) {
        signed portBASE_TYPE xHigherPriorityTaskWoken;
        result = xTimerStartFromISR(timer->handle, &xHigherPriorityTaskWoken );
        portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
    } 
	else
	{
        result = xTimerStart( timer->handle, BEKEN_WAIT_FOREVER );
	}

    if ( result != pdPASS )
    {
        return kGeneralErr;
    }

    return kNoErr;
}

OSStatus rtos_deinit_oneshot_timer( beken2_timer_t* timer )
{
	OSStatus ret = kNoErr;
	GLOBAL_INT_DECLARATION();

	GLOBAL_INT_DISABLE();
    if ( xTimerDelete( timer->handle, BEKEN_WAIT_FOREVER ) != pdPASS )
    {
        ret = kGeneralErr;
    }
	else
	{
		timer->handle = 0;
		timer->function = 0;
		timer->left_arg = 0;
		timer->right_arg = 0;
		timer->beken_magic = 0;
	}
	GLOBAL_INT_RESTORE();

    return ret;
}

OSStatus rtos_stop_oneshot_timer( beken2_timer_t* timer )
{
    signed portBASE_TYPE result;

    if ( platform_is_in_interrupt_context() == RTOS_SUCCESS ) {
        signed portBASE_TYPE xHigherPriorityTaskWoken;
        result = xTimerStopFromISR(timer->handle, &xHigherPriorityTaskWoken );
        portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
    } else
        result = xTimerStop( timer->handle, BEKEN_WAIT_FOREVER );

    if ( result != pdPASS )
    {
        return kGeneralErr;
    }

    return kNoErr;
}


BOOL rtos_is_oneshot_timer_init( beken2_timer_t* timer )
{
    return timer->handle ? true : false;
}

BOOL rtos_is_oneshot_timer_running( beken2_timer_t* timer )
{
    return ( xTimerIsTimerActive( timer->handle ) != 0 ) ? true : false;
}

OSStatus rtos_oneshot_reload_timer( beken2_timer_t* timer )
{
    signed portBASE_TYPE result;

    if ( platform_is_in_interrupt_context() == RTOS_SUCCESS ) 
	{
        signed portBASE_TYPE xHigherPriorityTaskWoken;
        result = xTimerResetFromISR(timer->handle, &xHigherPriorityTaskWoken );
        portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
    } 
	else
        result = xTimerReset( timer->handle, BEKEN_WAIT_FOREVER );

    if ( result != pdPASS )
    {
        return kGeneralErr;
    }

    return kNoErr;
}

OSStatus rtos_init_oneshot_timer( beken2_timer_t *timer, 
									uint32_t time_ms, 
									timer_2handler_t function,
									void* larg, 
									void* rarg )
{
	OSStatus ret = kNoErr;
	
	GLOBAL_INT_DECLARATION();

	GLOBAL_INT_DISABLE();
    timer->function = function;
    timer->left_arg = larg;
    timer->right_arg = rarg;	
	timer->beken_magic = BEKEN_MAGIC_WORD;
    timer->handle = _xTimerCreate("", 
								(portTickType)( time_ms / ms_to_tick_ratio ), 
								pdFALSE, 
								timer, 
								(native_timer_handler_t) timer_callback2 );
    if ( timer->handle == NULL )
    {
        ret = kGeneralErr;
    }

	GLOBAL_INT_RESTORE();

    return ret;
}

OSStatus rtos_init_timer( beken_timer_t *timer, 
									uint32_t time_ms, 
									timer_handler_t function, 
									void* arg )
{
	OSStatus ret = kNoErr;
	
	GLOBAL_INT_DECLARATION();

	GLOBAL_INT_DISABLE();
    timer->function = function;
    timer->arg      = arg;
	
    timer->handle = _xTimerCreate("", 
								(portTickType)( time_ms / ms_to_tick_ratio ), 
								pdTRUE, 
								timer, 
								(native_timer_handler_t) timer_callback1 );
    if ( timer->handle == NULL )
    {
        ret = kGeneralErr;
    }

	GLOBAL_INT_RESTORE();

    return ret;;
}

OSStatus rtos_start_timer( beken_timer_t* timer )
{
    signed portBASE_TYPE result;

    if ( platform_is_in_interrupt_context() == RTOS_SUCCESS ) {
        signed portBASE_TYPE xHigherPriorityTaskWoken;
        result = xTimerStartFromISR(timer->handle, &xHigherPriorityTaskWoken );
        portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
    } else
        result = xTimerStart( timer->handle, BEKEN_WAIT_FOREVER );

    if ( result != pdPASS )
    {
        return kGeneralErr;
    }

    return kNoErr;
}

OSStatus rtos_stop_timer( beken_timer_t* timer )
{
    signed portBASE_TYPE result;

    if ( platform_is_in_interrupt_context() == RTOS_SUCCESS ) {
        signed portBASE_TYPE xHigherPriorityTaskWoken;
        result = xTimerStopFromISR(timer->handle, &xHigherPriorityTaskWoken );
        portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
    } else
        result = xTimerStop( timer->handle, BEKEN_WAIT_FOREVER );

    if ( result != pdPASS )
    {
        return kGeneralErr;
    }

    return kNoErr;
}

OSStatus rtos_reload_timer( beken_timer_t* timer )
{
    signed portBASE_TYPE result;

    if ( platform_is_in_interrupt_context() == RTOS_SUCCESS ) {
        signed portBASE_TYPE xHigherPriorityTaskWoken;
        result = xTimerResetFromISR(timer->handle, &xHigherPriorityTaskWoken );
        portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
    } else
        result = xTimerReset( timer->handle, BEKEN_WAIT_FOREVER );

    if ( result != pdPASS )
    {
        return kGeneralErr;
    }

    return kNoErr;
}

OSStatus rtos_change_period( beken_timer_t* timer, uint32_t time_ms)
{
   signed portBASE_TYPE result;

    if ( platform_is_in_interrupt_context() == RTOS_SUCCESS ) {
        signed portBASE_TYPE xHigherPriorityTaskWoken;
        result = xTimerChangePeriodFromISR(timer->handle, 
                                     (portTickType)( time_ms / ms_to_tick_ratio ),
                                     &xHigherPriorityTaskWoken);
        portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
    } else
        result = xTimerChangePeriod( timer->handle, 
                                     (portTickType)( time_ms / ms_to_tick_ratio ),
                                     0 );

    if ( result != pdPASS )
    {
        return kGeneralErr;
    }

    return kNoErr;
   
}

OSStatus rtos_deinit_timer( beken_timer_t* timer )
{
	OSStatus ret = kNoErr;
	GLOBAL_INT_DECLARATION();

	GLOBAL_INT_DISABLE();
    if ( xTimerDelete( timer->handle, BEKEN_WAIT_FOREVER ) != pdPASS )
    {
        ret = kGeneralErr;
    }
	else
	{
		timer->handle = 0;
	}
	GLOBAL_INT_RESTORE();

    return ret;
}

uint32_t rtos_get_timer_expiry_time( beken_timer_t* timer )
{
    return xTimerGetExpiryTime( timer->handle);
}

uint32_t rtos_get_next_expire_time()
{
    return xTimerGetNextExpireTime();
}

uint32_t rtos_get_current_timer_count(void)
{
    return xTimerGetCurrentTimerCount();
}

BOOL rtos_is_timer_init( beken_timer_t* timer )
{
    return timer->handle ? true : false;
}


BOOL rtos_is_timer_running( beken_timer_t* timer )
{
    return ( xTimerIsTimerActive( timer->handle ) != 0 ) ? true : false;
}

OSStatus rtos_init_event_flags( beken_event_flags_t* event_flags )
{
    UNUSED_PARAMETER( event_flags );
	
    return kUnsupportedErr;
}

OSStatus rtos_wait_for_event_flags( beken_event_flags_t* event_flags, uint32_t flags_to_wait_for, uint32_t* flags_set, beken_bool_t clear_set_flags, beken_event_flags_wait_option_t wait_option, uint32_t timeout_ms )
{
    UNUSED_PARAMETER( event_flags );
    UNUSED_PARAMETER( flags_to_wait_for );
    UNUSED_PARAMETER( flags_set );
    UNUSED_PARAMETER( clear_set_flags );
    UNUSED_PARAMETER( wait_option );
    UNUSED_PARAMETER( timeout_ms );

    return kUnsupportedErr;
}

OSStatus rtos_set_event_flags( beken_event_flags_t* event_flags, uint32_t flags_to_set )
{
    UNUSED_PARAMETER( event_flags );
    UNUSED_PARAMETER( flags_to_set );
	
    return kUnsupportedErr;
}

OSStatus rtos_deinit_event_flags( beken_event_flags_t* event_flags )
{
    UNUSED_PARAMETER( event_flags );
	
    return kUnsupportedErr;
}

void rtos_suspend_thread(beken_thread_t* thread)
{
    if (thread == NULL)
        vTaskSuspend(NULL);
    else
        vTaskSuspend(*thread);
}

void rtos_resume_thread(beken_thread_t* thread)
{
    if (thread == NULL)
        vTaskResume(NULL);
    else
        vTaskResume(*thread);
}

/**
 * Gets time in milliseconds since RTOS start
 *
 * @Note: since this is only 32 bits, it will roll over every 49 days, 17 hours.
 *
 * @returns Time in milliseconds since RTOS started.
 */
beken_time_t rtos_get_time( void )
{
    return (beken_time_t) ( xTaskGetTickCount() * ms_to_tick_ratio );
}

/**
 * Delay for a number of milliseconds
 *
 * Processing of this function depends on the minimum sleep
 * time resolution of the RTOS.
 * The current thread sleeps for the longest period possible which
 * is less than the delay required, then makes up the difference
 * with a tight loop
 *
 * @return OSStatus : kNoErr if delay was successful
 *
 */
OSStatus rtos_delay_milliseconds( uint32_t num_ms )
{
    uint32_t ticks;

    ticks = num_ms / ms_to_tick_ratio;
    if (ticks == 0)
        ticks = 1;

    vTaskDelay( (portTickType) ticks );

    return kNoErr;
}

/*-----------------------------------------------------------*/
void vApplicationStackOverflowHook( xTaskHandle *pxTask, signed portCHAR *pcTaskName )
{
    UNUSED_PARAMETER( pxTask );
    UNUSED_PARAMETER( pcTaskName ); 

    rtos_stack_overflow((char*)pcTaskName);
}

/*-----------------------------------------------------------*/
void vApplicationMallocFailedHook( void )
{
}


/*-----------------------------------------------------------*/
void *beken_malloc( size_t xWantedSize )
{
	return pvPortMalloc(xWantedSize);
}

/*-----------------------------------------------------------*/
void beken_free( void *pv )
{
	vPortFree(pv);
}

/*-----------------------------------------------------------*/
void *beken_realloc( void *pv, size_t xWantedSize )
{
	return pvPortRealloc(pv, xWantedSize);
}


//#endif

