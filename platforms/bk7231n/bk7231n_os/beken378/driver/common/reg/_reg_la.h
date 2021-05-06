#ifndef __REG_LA_H_
#define __REG_LA_H_

#include "sys_config.h"

#if (CFG_SOC_NAME == SOC_BK7231N)
#define REG_LA_OFFSET 0x00000000

#define REG_LA_BASE_ADDR 0x00808000
#else
#define REG_LA_SIZE 64
#define REG_LA_OFFSET 0x00800000

#define REG_LA_BASE_ADDR 0x10E00000
#endif

#endif // __REG_LA_H_

