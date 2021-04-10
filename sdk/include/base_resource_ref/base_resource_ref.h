#ifndef __BASE_RESOURCE_REF_H
#define __BASE_RESOURCE_REF_H

#include "tuya_cloud_com_defs.h"

#ifdef __cplusplus
	extern "C" {
#endif


/***********************************************************
*************************micro define***********************
***********************************************************/


typedef VOID* RESOURCE_REF_HDL;


//===正常使用步骤===
//申请者 alloc
//多个使用者 addRef get release
//申请者 release

OPERATE_RET base_resource_ref_init();

OPERATE_RET base_resource_ref_alloc(VOID *resource, RESOURCE_REF_HDL *hdl);

OPERATE_RET base_resource_ref_get(RESOURCE_REF_HDL hdl, VOID **resource);

OPERATE_RET base_resource_ref_addref(RESOURCE_REF_HDL hdl);

OPERATE_RET base_resource_ref_release(RESOURCE_REF_HDL hdl);


#ifdef __cplusplus
}
#endif
#endif

