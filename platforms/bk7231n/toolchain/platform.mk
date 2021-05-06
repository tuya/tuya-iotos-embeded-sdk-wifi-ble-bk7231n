TUYA_PLATFORM_DIR := $(dir $(lastword $(MAKEFILE_LIST)))/../


# tuya os adapter includes
TUYA_PLATFORM_CFLAGS := -I$(TUYA_PLATFORM_DIR)/tuya_os_adapter/include
TUYA_PLATFORM_CFLAGS += -I$(TUYA_PLATFORM_DIR)/tuya_os_adapter/include/driver
TUYA_PLATFORM_CFLAGS += -I$(TUYA_PLATFORM_DIR)/tuya_os_adapter/include/system



TUYA_PLATFORM_CFLAGS += -g -mthumb -mcpu=arm968e-s -march=armv5te -mthumb-interwork -mlittle-endian -Os -std=c99 -ffunction-sections -Wall -fsigned-char -fdata-sections -Wunknown-pragmas -nostdlib -Wno-unused-function -Wno-unused-but-set-variable
