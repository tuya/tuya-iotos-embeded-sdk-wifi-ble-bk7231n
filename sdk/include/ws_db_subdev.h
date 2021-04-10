#ifndef __TUYA_WS_DB_SUBDEV_H__
#define __TUYA_WS_DB_SUBDEV_H__


#include "tuya_ws_db.h"
#include "gw_intf.h"


#ifdef __cplusplus
    extern "C" {
#endif



#if defined(ENABLE_SUBDEVICE) && (ENABLE_SUBDEVICE==1)
/***********************************************************
*  Function: wd_dev_grp_if_write
*  Input: dev_id
*         grp_if
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET wd_dev_grp_if_write(IN CONST CHAR_T *dev_id,IN CONST GRP_MAG_S *grp);

/***********************************************************
*  Function: wd_dev_grp_if_read
*  Input: dev_id
*  Output: grp
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET wd_dev_grp_if_read(IN CONST CHAR_T *dev_id,OUT GRP_MAG_S *grp);

/***********************************************************
*  Function: wd_dev_grp_if_delete
*  Input: dev_id
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET wd_dev_grp_if_delete(IN CONST CHAR_T *dev_id);

/***********************************************************
*  Function: wd_dev_grp_delete_all
*  Input: none
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET wd_dev_grp_delete_all(VOID);

/***********************************************************
*  Function: wd_dev_scene_if_write
*  Input: dev_id scene
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET wd_dev_scene_if_write(IN CONST CHAR_T *dev_id,IN CONST SCENE_MAG_S *scene);

/***********************************************************
*  Function: wd_dev_scene_if_read
*  Input: dev_id
*  Output: scene
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET wd_dev_scene_if_read(IN CONST CHAR_T *dev_id,OUT SCENE_MAG_S *scene);

/***********************************************************
*  Function: wd_dev_scene_if_delete
*  Input: dev_id
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET wd_dev_scene_if_delete(IN CONST CHAR_T *dev_id);

/***********************************************************
*  Function: wd_dev_sce_delete_all
*  Input: none
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET wd_dev_sce_delete_all(VOID);

/***********************************************************
*  Function: wd_app_user_if_write
*  Input: key, val, len
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET wd_app_user_if_write(IN CONST CHAR_T *key, IN CONST BYTE_T *val, IN CONST UINT_T len);

/***********************************************************
*  Function: wd_app_user_if_read
*  Input: key,
*  Output: val, len
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET wd_app_user_if_read(IN CONST CHAR_T *key, OUT BYTE_T **val, OUT UINT_T *p_len);

/***********************************************************
*  Function: wd_app_user_if_del
*  Input: key
*  Output: null
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET wd_app_user_if_del(IN CONST CHAR_T *key);

/***********************************************************
*  Function: wd_app_user_if_del_all
*  Input: none
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET wd_app_user_if_del_all(VOID);

#endif


#ifdef __cplusplus
} // extern "C"
#endif

#endif  // __TUYA_WS_DB_SUBDEV_H__

