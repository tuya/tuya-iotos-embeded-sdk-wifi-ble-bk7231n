#ifndef _BK_TIMER_PUB_H_
#define _BK_TIMER_PUB_H_

#include "typedef.h"

#define TIMER_DEV_NAME                "bk_timer"

#define BK_TIMER_FAILURE                (1)
#define BK_TIMER_SUCCESS                (0)

#define TIMER_CMD_MAGIC              (0xe340000)
enum
{
    CMD_TIMER_UNIT_ENABLE = TIMER_CMD_MAGIC + 1,
    CMD_TIMER_UNIT_DISABLE,
    CMD_TIMER_INIT_PARAM,
    CMD_TIMER_INIT_PARAM_US,
    CMD_TIMER_READ_CNT
};

enum
{
    BKTIMER0     = 0,
    BKTIMER1,
    BKTIMER2,
    BKTIMER3,
    BKTIMER4,
    BKTIMER5,
    BKTIMER_COUNT
};

typedef void (*TFUNC)(UINT8);

typedef struct
{
    UINT8 channel;
    UINT8 div;
    UINT32 period;
    TFUNC t_Int_Handler;
} timer_param_t;

void bk_timer_init(void);
void bk_timer_exit(void);
void bk_timer_isr(void);


#endif //_TIMER_PUB_H_

