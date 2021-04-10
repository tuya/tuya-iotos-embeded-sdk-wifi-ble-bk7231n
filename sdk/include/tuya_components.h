/**
 * @file tuya_components.h
 * @author Embeded CI system
 * @brief 组件信息
 * @version 0.1
 * @date 2019-08-28
 * 
 * @copyright Copyright (c) tuya.inc 2019
 * 
 */

#ifndef __TUYA_COMPONENTS_H__
#define __TUYA_COMPONENTS_H__

#ifdef __cplusplus
extern "C" {
#endif

#define TUYA_COMPONENT_NAME_LEN 32
#define TUYA_COMPONENT_VERSION_LEN 32

typedef int (*component_init_func)(const void *init_param);
typedef int (*component_reg_func)(const void *reg_param);
typedef int (*component_unreg_func)(const void *unreg_param);
typedef int (*component_active_func)(const void *active_param);
typedef int (*component_unactive_func)(const void *unactive_param);
typedef int (*component_reset_func)(const void *reset_param);

typedef struct {
	component_init_func init;
	component_reg_func reg;
	component_unreg_func unreg;
	component_active_func active;
	component_unactive_func unactive;
	component_reset_func reset;
}component_options_t;

typedef struct {
	char name[TUYA_COMPONENT_NAME_LEN+1];
	char version[TUYA_COMPONENT_VERSION_LEN+1];

	component_options_t options;
}tuya_component_descrition_t;


#ifdef __cplusplus
}
#endif


#endif
