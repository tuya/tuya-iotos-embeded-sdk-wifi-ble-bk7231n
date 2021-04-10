#ifndef __TUYA_WS_DB_GW_H__
#define __TUYA_WS_DB_GW_H__


#include "tuya_ws_db.h"
#include "gw_intf.h"


#ifdef __cplusplus
    extern "C" {
#endif



/***********************************************************
*  Function: wd_gw_base_if_write
*  Input: bi
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET wd_gw_base_if_write(IN GW_BASE_IF_S *bi);

/***********************************************************
*  Function: wd_gw_base_if_read
*  Input: none
*  Output: bi
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET wd_gw_base_if_read(OUT GW_BASE_IF_S *bi);

/***********************************************************
*  Function: wd_gw_wsm_write
*  Input: wsm
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET wd_gw_wsm_write(IN GW_WORK_STAT_MAG_S *wsm);

/***********************************************************
*  Function: wd_gw_wsm_read
*  Input: none
*  Output: wsm
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET wd_gw_wsm_read(OUT GW_WORK_STAT_MAG_S *wsm);

/***********************************************************
*  Function: wd_gw_desc_if_write
*  Input: di
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET wd_gw_desc_if_write(IN GW_DESC_IF_S *di);

/***********************************************************
*  Function: wd_gw_desc_if_read
*  Input: none
*  Output: di
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET wd_gw_desc_if_read(OUT GW_DESC_IF_S *di);

/***********************************************************
*  Function: wd_gw_desc_if_delete
*  Input: none
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET wd_gw_desc_if_delete(VOID_T);

/***********************************************************
*  Function: wd_gw_actv_if_write
*  Input: ai
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET wd_gw_actv_if_write(IN GW_ACTV_IF_S *ai);

/***********************************************************
*  Function: wd_gw_actv_if_read
*  Input: none
*  Output: ai
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET wd_gw_actv_if_read(OUT GW_ACTV_IF_S *ai);

/***********************************************************
*  Function: wd_gw_sigmesh_if_write
*  Input: si
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET wd_gw_sigmesh_if_write(IN GW_SIGMESH_IF_S *si);

/***********************************************************
*  Function: wd_gw_sigmesh_if_read
*  Input: si
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
OPERATE_RET wd_gw_sigmesh_if_read(IN GW_SIGMESH_IF_S *si);



#ifdef __cplusplus
}
#endif

#endif  // __TUYA_WS_DB_GW_H__

