#ifndef __TUYA_SUBDEV_RCD_STO_H__
#define __TUYA_SUBDEV_RCD_STO_H__


#include "tuya_ws_db.h"
#include "gw_intf.h"
#include "ws_db_dev.h"


#ifdef __cplusplus
    extern "C" {
#endif



#if defined(ENABLE_SUBDEVICE) && (ENABLE_SUBDEVICE==1)

/***********************************************************
*  Function: wd_dev_if_rcd_write
*  Input: ddi
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET wd_dev_if_rcd_write(IN DEV_DESC_IF_S *ddi);

/***********************************************************
*  Function: wd_dev_if_rcd_delete_all
*  Input: none
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET wd_dev_if_rcd_delete_all(VOID);

OPERATE_RET wd_dev_if_rcd_delete(IN CONST CHAR_T *p_dev_id);



/***********************************************************
*  Function: wd_dev_if_rcd_read
*  Input: index->from 0
*  Output: index->the find index +1 ddi
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET wd_dev_if_rcd_read(INOUT UINT_T *index,OUT DEV_DESC_IF_S *ddi);


OPERATE_RET wd_dev_if_rcd_traversal_all(WD_DEV_IF_TRAVERSAL_CB trave_all_cb, VOID * cb_param);

/***********************************************************
*  Function: wd_dev_schema_rcd_write
*  Input: s_id schema
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET wd_dev_schema_rcd_write(IN CONST CHAR_T *s_id,IN CONST CHAR_T *schema);

/***********************************************************
*  Function: wd_dev_schema_rcd_read
*  Input: s_id
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET wd_dev_schema_rcd_read(IN CONST CHAR_T *s_id,OUT CHAR_T **schema);


/***********************************************************
*  Function: wd_dev_schema_rcd_read
*  Input: s_id
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET wd_dev_schema_rcd_delete(IN CONST CHAR_T *s_id);

#endif


#ifdef __cplusplus
} // extern "C"
#endif

#endif  // __TUYA_SUBDEV_RCD_STO_H__

