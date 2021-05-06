#ifndef _SDCARD_H_
#define _SDCARD_H_

#if CFG_USE_SDCARD_HOST
#include "sdcard_pub.h"

/* API */
extern UINT32 sdcard_open(UINT32 op_falag);

extern UINT32 sdcard_close(void);

extern UINT32 sdcard_read(char *user_buf, UINT32 count, UINT32 op_flag);

extern UINT32 sdcard_write(char *user_buf, UINT32 count, UINT32 op_flag);

extern UINT32 sdcard_ctrl(UINT32 cmd, void *parm);

#endif // CFG_USE_SDCARD_HOST

#endif
