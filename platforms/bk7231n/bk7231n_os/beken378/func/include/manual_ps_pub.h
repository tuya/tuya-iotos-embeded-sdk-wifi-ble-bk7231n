#ifndef _MANUAL_PS_PUB_H_
#define _MANUAL_PS_PUB_H_


typedef enum {
    PS_DEEP_WAKEUP_GPIO = 1,
    PS_DEEP_WAKEUP_RTC = 2,
    PS_DEEP_WAKEUP_USB = 4,
} PS_DEEP_WAKEUP_WAY;

typedef struct  ps_deep_ctrl {

	/*deep_sleep wakeup modes */
	PS_DEEP_WAKEUP_WAY wake_up_way;
	
    /** @brief	Request deep sleep,and wakeup by gpio,rtc,or usb.
    *
    *   @param	gpio_index_map:Indicate which GPIOs can wake up deep sleep,
    *               if the bit is set to 1.
    *               gpio_index_map is hex and every bits is map to gpio31-gpio0.
    *               ps:gpio1 as uart RX pin must be wake up from falling
    *			gpio_edge_map:The gpio edge bitmap for wakeup gpios,
    *               gpio_edge_map is hex and every bits is map to gpio31-gpio0.
    *               0:high level wakeup,1:low level wakeup.
    *		    gpio_stay_lo_map:The gpio bitmap which need stay ,not change in deep sleep.
    *               gpio_stay_lo_map is hex and every bits is map to gpio31-gpio0.
    *           gpio_last_index_map:Indicate which GPIOs can wake up deep sleep,
    *               if the bit is set to 1.
    * 			    gpio_index_map is hex and every bits is map to gpio39-gpio32.
    *           gpio_last_edge_map:The gpio edge bitmap for wakeup gpios,
    * 			    gpio_edge_map is hex and every bits is map to gpio39-gpio32.
    * 			    0:high level wakeup,1:low level wakeup.
    *           gpio_stay_hi_map:The gpio bitmap which need stay ,not change in deep sleep.
    *               gpio_stay_hi_map is hex and every bits is map to gpio39-gpio32.
    *
    *           sleep_time:the time secound when use PS_DEEP_WAKEUP_RTC wakeup.
    *           lpo_32k_src:the RTC wakeup source.LPO_SELECT_ROSC or LPO_SELECT_32K_XTAL.
    */
	
	UINT32 gpio_index_map;
	UINT32 gpio_edge_map;
	UINT32 gpio_stay_lo_map;
	UINT32 gpio_last_index_map;
	UINT32 gpio_last_edge_map;
	UINT32 gpio_stay_hi_map;
	
	UINT32 sleep_time;
	UINT32 lpo_32k_src;
} PS_DEEP_CTRL_PARAM;

#define     PS_SUPPORT_MANUAL_SLEEP     0

#endif

