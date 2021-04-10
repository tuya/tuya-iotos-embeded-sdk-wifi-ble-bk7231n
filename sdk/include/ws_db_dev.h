#ifndef __TUYA_WS_DB_DEV_H__
#define __TUYA_WS_DB_DEV_H__


#include "tuya_ws_db.h"
#include "gw_intf.h"


#ifdef __cplusplus
    extern "C" {
#endif




/***********************************************************
*  Function: wd_dev_if_write
*  Input: ddi
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET wd_dev_if_write(IN DEV_DESC_IF_S *ddi);

/***********************************************************
*  Function: wd_dev_if_delete_all
*  Input: none
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET wd_dev_if_delete_all(VOID);

OPERATE_RET wd_dev_if_delete(IN CONST CHAR_T *p_dev_id);



/***********************************************************
*  Function: wd_dev_if_read
*  Input: index->from 0
*  Output: index->the find index +1 ddi
*  Return: OPERATE_RET
***********************************************************/
//使用该接口遍历时，不允许写和删除，否则可能会出现遍历不完全
//遍历时需要写和删除，得使用 wd_dev_if_traversal_all 接口
OPERATE_RET wd_dev_if_read(INOUT UINT_T *index,OUT DEV_DESC_IF_S *ddi);


//如果某次回调返回失败，遍历会停止
typedef OPERATE_RET (*WD_DEV_IF_TRAVERSAL_CB)(IN DEV_DESC_IF_S *ddi, VOID *param);


//遍历所有的子设备
//支持在遍历过程中，修改已有的节点
OPERATE_RET wd_dev_if_traversal_all(WD_DEV_IF_TRAVERSAL_CB trave_all_cb, VOID * param);

/***********************************************************
*  Function: wd_dev_schema_write
*  Input: s_id schema
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET wd_dev_schema_write(IN CONST CHAR_T *s_id,IN CONST CHAR_T *schema);

/***********************************************************
*  Function: wd_dev_schema_read
*  Input: s_id
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET wd_dev_schema_read(IN CONST CHAR_T *s_id,OUT CHAR_T **schema);


/***********************************************************
*  Function: wd_dev_schema_read
*  Input: s_id
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET wd_dev_schema_delete(IN CONST CHAR_T *s_id);

/***********************************************************
*  Function: wd_dev_schema_write_ver
*  Input: s_id ver
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET wd_dev_schema_write_ver(IN CONST CHAR_T *s_id,IN CONST CHAR_T *ver);

/***********************************************************
*  Function: wd_dev_schema_read_ver
*  Input: s_id ver
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET wd_dev_schema_read_ver(IN CONST CHAR_T *s_id,OUT CHAR_T **ver);

#ifdef __cplusplus
}
#endif

#endif  // __TUYA_WS_DB_DEV_H__

