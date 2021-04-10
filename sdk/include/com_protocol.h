/***********************************************************
*  File: com_protocol.h 
*  Author: nzy
*  Date: 20170412
***********************************************************/
#ifndef _COM_PROTOCOL_H
    #define _COM_PROTOCOL_H

    #include "tuya_cloud_types.h"
    #include "tuya_cloud_com_defs.h"

#ifdef __cplusplus
	extern "C" {
#endif

#ifdef  __COM_PROTOCOL_GLOBALS
    #define __COM_PROTOCOL_EXT
#else
    #define __COM_PROTOCOL_EXT extern
#endif

/***********************************************************
*************************micro define***********************
***********************************************************/

/***********************************************************
*************************variable define********************
***********************************************************/

/***********************************************************
*************************function define********************
***********************************************************/
/***********************************************************
*  Function: parse_data_with_cmd
*  Input: cmd data len
*  Output: out_data
*  Return: OPERATE_RET
***********************************************************/
__COM_PROTOCOL_EXT \
OPERATE_RET parse_data_with_cmd(IN CONST DP_CMD_TYPE_E cmd, IN CONST BYTE_T *data,\
                                IN CONST INT_T len,OUT CHAR_T **out_data);


/***********************************************************
*  Function: pack_data_with_cmd
*  Input: cmd src pro num
*  Output: out_data out_len
*  Return: OPERATE_RET
***********************************************************/
__COM_PROTOCOL_EXT \
OPERATE_RET pack_data_with_cmd(IN CONST DP_CMD_TYPE_E cmd,IN CONST CHAR_T *src,\
                               IN CONST UINT_T pro,IN CONST UINT_T num,\
                               OUT BYTE_T **out,OUT UINT_T *out_len);

#ifdef __cplusplus
}
#endif
#endif

