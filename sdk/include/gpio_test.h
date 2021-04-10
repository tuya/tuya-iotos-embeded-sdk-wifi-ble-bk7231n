/***********************************************************
*  File: tuya_gpio_test.h
*  Author: lql
*  Date: 20180502
***********************************************************/
#include "tuya_pin.h"
#include "tuya_iot_config.h"

typedef struct
{
    INT_T   ionum;
    tuya_pin_name_t iopin[8];
} CTRL_GROUP;

#if defined(TY_GPIO_TEST_V2) && (TY_GPIO_TEST_V2 == 1)
typedef struct {
    INT_T group_num;
    CTRL_GROUP group[10];
} GPIO_TEST_TABLE;
#else
typedef struct {
    INT_T group_num;
    CTRL_GROUP *group;
} GPIO_TEST_TABLE;
#endif

VOID gpio_test_enabe(BOOL_T enable);
BOOL_T gpio_test_cb(GPIO_TEST_TABLE * table);
#if defined(TY_GPIO_TEST_V2) && (TY_GPIO_TEST_V2 == 1)
BOOL_T gpio_test_all(IN CONST CHAR_T *in, OUT CHAR_T *out);
#else
BOOL_T gpio_test_all(VOID) ;
#endif
