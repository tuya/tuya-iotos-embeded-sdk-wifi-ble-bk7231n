/**
 ****************************************************************************************
 *
 * @file arch_main.c
 *
 * @brief Main loop of the application.
 *
 * Copyright (C) Beken Corp 2011-2020
 *
 ****************************************************************************************
 */
#include "include.h"
#include "driver_pub.h"
#include "func_pub.h"
#include "app.h"
#include "uart.h"
#include "arm_arch.h"
#include "ate_app.h"
#include "bk7011_cal_pub.h"
#include "start_type_pub.h"

beken_semaphore_t extended_app_sema = NULL;
uint32_t  extended_app_stack_size = 2048;

extern void user_main_entry(void);

void extended_app_launch_over(void)
{  
	OSStatus ret;
	ret = rtos_set_semaphore(&extended_app_sema);
	
	(void)ret;
}
    
void extended_app_waiting_for_launch(void)
{
	OSStatus ret;

	ret = rtos_get_semaphore(&extended_app_sema, BEKEN_WAIT_FOREVER);
	ASSERT(kNoErr == ret);
	
	(void)ret;
}

static void extended_app_task_handler(void *arg)
{
    /* step 0: function layer initialization*/
    func_init_extended();  
	
#ifdef VCS_RELEASE_VERSION
	bk_printf("Version:%s\r\n",VCS_RELEASE_VERSION);
#endif

    /* step 1: startup application layer*/
	#if CFG_ENABLE_ATE_FEATURE
    if(get_ate_mode_state())
    {
	    ate_start();
    }
    else
	#endif // CFG_ENABLE_ATE_FEATURE
    {
	    app_start();
    }
         
	extended_app_launch_over();
	
    rtos_delete_thread( NULL );
}

void extended_app_launch(void)
{
	OSStatus ret;
	
    ret = rtos_init_semaphore(&extended_app_sema, 1);
	ASSERT(kNoErr == ret);

	ret = rtos_create_thread(NULL,
					   THD_EXTENDED_APP_PRIORITY,
					   "extended_app",
					   (beken_thread_function_t)extended_app_task_handler,
					   extended_app_stack_size,
					   (beken_thread_arg_t)0);
	ASSERT(kNoErr == ret);
}

void entry_main(void)
{
    /* disable ATE when enable JTAG since TUYA using GPIO21 as check port */
#if ATE_APP_FUN && !CFG_JTAG_ENABLE
    ate_app_init();
#endif

    bk_misc_init_start_type();
    
    /* step 1: driver layer initialization*/
    driver_init();
	bk_misc_check_start_type();
	func_init_basic();

#if ATE_APP_FUN
   	if(!get_ate_mode_state())
#endif
   	{
		user_main_entry();	
   	}
	extended_app_launch();
	
#if (!CFG_SUPPORT_RTT)
    vTaskStartScheduler();
#endif
}
// eof

