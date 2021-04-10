/***********************************************************
*  File: tuya_led.h
*  Author: nzy
*  Date: 20171117
***********************************************************/
#ifndef __TUYA_LED_H
    #define __TUYA_LED_H

    #include "tuya_cloud_types.h"
    #include "tuya_pin.h"

#ifdef __cplusplus
	extern "C" {
#endif

#ifdef  __TUYA_LED_GLOBALS
    #define __TUYA_LED_EXT
#else
    #define __TUYA_LED_EXT extern
#endif

#define LED_TIMER_UNINIT 0xffff

/***********************************************************
*************************micro define***********************
***********************************************************/
typedef PVOID_T LED_HANDLE; // led handle
typedef enum {
    OL_LOW = 0,    // output level low
    OL_HIGH,       // output level high
    OL_FLASH_LOW,  // when led flash,the level output low first.
                   // when led flash end,the the level output low.
    OL_FLASH_HIGH, // when led flash,the level output high first.
                   // when led flash end,the the level output high.
}LED_LT_E;

/***********************************************************
*************************variable define********************
***********************************************************/

/***********************************************************
*************************function define********************
***********************************************************/
/***********************************************************
*  Function: tuya_create_led_handle
*  Input: port
*         high->default output port high/low
*  Output: handle
*  Return: OPERATE_RET
***********************************************************/
__TUYA_LED_EXT \
 OPERATE_RET tuya_create_led_handle(IN CONST tuya_pin_name_t pinname,IN CONST BOOL_T high,OUT LED_HANDLE *handle);

 /***********************************************************
*  Function: tuya_create_led_handle_select
*  Input: port
*         high->default output port high/low
*  Output: handle
*  Return: OPERATE_RET
***********************************************************/
__TUYA_LED_EXT \
OPERATE_RET tuya_create_led_handle_select(IN CONST tuya_pin_name_t pinname,IN CONST BOOL_T high,OUT LED_HANDLE *handle);

/***********************************************************
*  Function: tuya_set_led_light_type
*  Input: handle type flh_mstime 
*         flh_ms_sumtime->led flash lasttime if(0xffff == flh_ms_sumtime) then flash forever
*  Output: none
*  Return: OPERATE_RET
*  note: if(OL_FLASH_XXX == type) then flh_mstime and flh_ms_sumtime is valid
***********************************************************/
__TUYA_LED_EXT \
VOID tuya_set_led_light_type(IN CONST LED_HANDLE handle,IN CONST LED_LT_E type,\
                                        IN CONST USHORT_T flh_mstime,IN CONST flh_ms_sumtime);



#ifdef __cplusplus
}
#endif
#endif

