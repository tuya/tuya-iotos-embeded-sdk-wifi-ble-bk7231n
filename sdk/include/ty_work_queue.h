#ifndef _TY_WORK_QUEUE_H
#define _TY_WORK_QUEUE_H

#include "tuya_cloud_types.h"

#ifdef __cplusplus
    extern "C" {
#endif

typedef VOID (*TY_WK_CB)(VOID *data);

OPERATE_RET ty_work_queue_init(VOID);

OPERATE_RET ty_work_queue_add(IN CONST TY_WK_CB cb, IN CONST VOID *data);

UINT_T ty_work_queue_depth(VOID);

#ifdef __cplusplus
}
#endif
#endif

