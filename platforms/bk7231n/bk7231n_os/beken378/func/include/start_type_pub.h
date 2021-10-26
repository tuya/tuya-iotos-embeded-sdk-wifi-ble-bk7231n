#ifndef _START_TYPE_PUB_H_
#define _START_TYPE_PUB_H_

#define     START_TYPE_ADDR        (0x0080a080)

#define     CRASH_XAT0_VALUE              0xbedead00
#define     CRASH_UNDEFINED_VALUE         0xbedead01
#define     CRASH_PREFETCH_ABORT_VALUE    0xbedead02
#define     CRASH_DATA_ABORT_VALUE        0xbedead03
#define     CRASH_UNUSED_VALUE            0xbedead04
#define     CRASH_2ND_XAT0_VALUE          0xbedead05

#define     START_TYPE_DMEMORY_ADDR       (0x0040001c)


typedef enum { 
	RESET_SOURCE_POWERON = 0,
	RESET_SOURCE_REBOOT = 1,
	RESET_SOURCE_WATCHDOG = 2,

	RESET_SOURCE_DEEPPS_GPIO = 3,
	RESET_SOURCE_DEEPPS_RTC = 4,

	RESET_SOURCE_CRASH_XAT0 = 5,
	RESET_SOURCE_CRASH_UNDEFINED = 6,
	RESET_SOURCE_CRASH_PREFETCH_ABORT = 7,
	RESET_SOURCE_CRASH_DATA_ABORT = 8,
	RESET_SOURCE_CRASH_UNUSED = 9,
	RESET_SOURCE_CRASH_PER_XAT0 = 10,

	RESET_SOURCE_UNKNOWN = 0xb,
} RESET_SOURCE_STATUS;

extern  RESET_SOURCE_STATUS bk_misc_init_start_type(void);
extern  void bk_misc_update_set_type(RESET_SOURCE_STATUS type);
extern RESET_SOURCE_STATUS bk_misc_get_start_type();
extern RESET_SOURCE_STATUS sctrl_get_deep_sleep_wake_soure(void);
extern void bk_misc_check_start_type();

#endif ///_START_TYPE_PUB_H_

// eof