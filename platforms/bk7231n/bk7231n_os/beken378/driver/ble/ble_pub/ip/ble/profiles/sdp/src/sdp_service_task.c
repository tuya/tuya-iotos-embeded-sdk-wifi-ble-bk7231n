/**
 ****************************************************************************************
 *
 * @file sdp_service_task.c
 *
 * @brief SDP Service Client Task implementation.
 *
 * Copyright (C) Beken 2016-2017
 *
 *
 ****************************************************************************************
 */
/**
 ****************************************************************************************
 * @addtogroup SDPTASK
 * @{
 ****************************************************************************************
 */
/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "rwip_config.h"
#if (BLE_SDP_CLIENT)
#include "gap.h"
#include "attm.h"
#include "sdp_service.h"
#include "sdp_service_task.h"
#include "gattc_task.h"
#include "common_math.h"
#include "uart.h"
/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */
/*
 * GLOBAL FUNCTIONS DEFINITIONS
 ****************************************************************************************
 */
/**
 ****************************************************************************************
 * @brief Handles reception of the @ref BASC_ENABLE_REQ message.
 * The handler enables the NTF Service Client Role.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int sdp_enable_req_handler(kernel_msg_id_t const msgid,
                                  struct sdp_enable_req const *param,
                                  kernel_task_id_t const dest_id,
                                  kernel_task_id_t const src_id)
{
	// Status
	int msg_status = KERNEL_MSG_CONSUMED;
	return (msg_status);
}
/**
 ****************************************************************************************
 * @brief Handles reception of the @ref SDP_READ_INFO_REQ message.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int sdp_read_info_req_handler(kernel_msg_id_t const msgid,
                                     struct sdp_read_info_req const *param,
                                     kernel_task_id_t const dest_id,
                                     kernel_task_id_t const src_id)
{
	printf("%s\r\n",__func__);

	int msg_status = KERNEL_MSG_CONSUMED;
	uint8_t conidx = KERNEL_IDX_GET(dest_id);
	uint8_t char_idx;
	uint16_t handle;
	int i;
	uint8_t desc_nb;
	struct sdp_env_tag *sdp_env =(struct sdp_env_tag *) prf_env_get_from_uuid(param->uuid,0);
	if(sdp_env != NULL)
	{
		char_idx = sdp_env->db_env[conidx].cnx_env->sdp.char_idx;
		desc_nb = sdp_env->db_env[conidx].cnx_env->sdp.descs_nb;
		handle = sdp_env->db_env[conidx].cnx_env->sdp.chars[char_idx].val_hdl;
		uint16_t val_prop =  sdp_env->db_env[conidx].cnx_env->sdp.chars[char_idx].prop;
		printf("desc_nb = 0x%x,char_idx = 0x%x,value_handle = 0x%x\r\n",desc_nb,char_idx,handle);
		if(SDPC_CHAR_NTF_CFG == param->info)
		{
			for(i = 0; i < desc_nb; i++)
			{
				if((ATT_DESC_CLIENT_CHAR_CFG == sdp_env->db_env[conidx].cnx_env->sdp.descs[i].uuid)
				        && (sdp_env->db_env[conidx].cnx_env->sdp.descs[i].char_code == char_idx))
				{
					handle = sdp_env->db_env[conidx].cnx_env->sdp.descs[i].desc_hdl;
					val_prop |= ATT_CHAR_PROP_RD;
					break;
				}
			}
		}
		else if(SDPC_CHAR_USER_DESC_VAL == param->info)
		{
			for(i = 0; i < desc_nb; i++)
			{
				if((ATT_DESC_CHAR_USER_DESCRIPTION == sdp_env->db_env[conidx].cnx_env->sdp.descs[i].uuid)
				        && (sdp_env->db_env[conidx].cnx_env->sdp.descs[i].char_code == char_idx))
				{
					handle = sdp_env->db_env[conidx].cnx_env->sdp.descs[i].desc_hdl;
					val_prop |= ATT_CHAR_PROP_RD;
					break;
				}
			}
		}
		printf("prop = 0x%x,char_idx = 0x%x\r\n",val_prop,char_idx);
		printf("handle = 0x%x\r\n",handle);
		if((val_prop & ATT_CHAR_PROP_RD) == ATT_CHAR_PROP_RD )
		{
			printf("prf_read_char_send \r\n");
			prf_read_char_send(&(sdp_env->prf_env), conidx,handle, handle, handle);
		}
		else
		{
			printf("val_prop not read prop!!!! \r\n");
		}
	}
	else
	{
		printf("param unvalid uuid =  0x%02x\r\n",param->uuid);
	}

	return (msg_status);
}

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref BASC_BATT_LEVEL_NTF_CFG_REQ message.
 * It allows configuration of the peer ntf/stop characteristic for Battery Level Characteristic.
 * Will return an error code if that cfg char does not exist.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int sdp_write_value_info_req_handler(kernel_msg_id_t const msgid,
        struct sdp_write_info_req const *param,
        kernel_task_id_t const dest_id,
        kernel_task_id_t const src_id)

{
	int msg_status = KERNEL_MSG_CONSUMED;

	uint8_t conidx = KERNEL_IDX_GET(dest_id);
	uint8_t char_idx;
	struct sdp_env_tag *sdp_env =(struct sdp_env_tag *) prf_env_get_from_uuid(param->uuid,0);
	if(sdp_env != NULL)
	{
		char_idx = sdp_env->db_env[conidx].cnx_env->sdp.char_idx;
		uint16_t handle = sdp_env->db_env[conidx].cnx_env->sdp.chars[char_idx].val_hdl;
		uint16_t length = param->length;
		uint16_t val_prop =  sdp_env->db_env[conidx].cnx_env->sdp.chars[char_idx].prop;
		uint8_t *buf = (uint8_t *)(&param->data[0]);
		uint8_t operation = GATTC_WRITE_NO_RESPONSE;
		if((val_prop & ATT_CHAR_PROP_WR) == ATT_CHAR_PROP_WR)
		{
			operation = GATTC_WRITE;
			printf("operation = %x\r\n",operation);
		}
		prf_gatt_write(&(sdp_env->prf_env), conidx,handle, buf,length,operation,0);
	}
	else
	{
		printf("param unvalid uuid =  0x%02x\r\n",param->uuid);
	}
	return (msg_status);
}

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref BASC_BATT_LEVEL_NTF_CFG_REQ message.
 * It allows configuration of the peer ntf/stop characteristic for Battery Level Characteristic.
 * Will return an error code if that cfg char does not exist.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int sdp_write_ntf_cfg_req_handler(kernel_msg_id_t const msgid,
        struct sdp_w_ntf_cfg_req const *param,
        kernel_task_id_t const dest_id,
        kernel_task_id_t const src_id)
{
	printf("%s\r\n",__func__);

	int msg_status = KERNEL_MSG_CONSUMED;
	uint8_t conidx = KERNEL_IDX_GET(dest_id);
	uint8_t char_idx;
	// put value in air format
	printf("param->uuid = %x\r\n",param->uuid);
	struct sdp_env_tag *sdp_env =(struct sdp_env_tag *) prf_env_get_from_uuid(param->uuid,param->char_num);
	if(sdp_env != NULL)
	{
		char_idx = sdp_env->db_env[conidx].cnx_env->sdp.char_idx;
		uint16_t handle = sdp_env->db_env[conidx].cnx_env->sdp.chars[char_idx].val_hdl;
		printf("handle = 0x%x\r\n",handle + 1);
		prf_gatt_write_ntf_ind(&(sdp_env->prf_env), conidx,  handle + 1, param->ntf_cfg, 0xa5);
	}
	else
	{
		printf("param unvalid uuid =  0x%02x\r\n",param->uuid);
	}
	return (msg_status);
}

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref GATTC_CMP_EVT message.
 * This generic event is received for different requests, so need to keep track.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gattc_cmp_evt_handler(kernel_msg_id_t const msgid,
                                 struct gattc_cmp_evt const *param,
                                 kernel_task_id_t const dest_id,
                                 kernel_task_id_t const src_id)
{
	printf("sdp %s dest_id = %x\r\n",__func__,dest_id);
	printf("operation = 0x%x,status = 0x%x,seq_num = 0x%x\r\n",param->operation,param->status,param->seq_num);
	struct gattc_cmp_evt *cmp_evt  = KERNEL_MSG_ALLOC(GATTC_CMP_EVT, TASK_APP, dest_id, gattc_cmp_evt);
	cmp_evt->operation  = param->operation;
	cmp_evt->status = param->status;
	cmp_evt->seq_num = param->seq_num ;

	kernel_msg_send(cmp_evt);

	return (KERNEL_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles reception of the @ref GATTC_READ_IND message.
 * Generic event received after every simple read command sent to peer server.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gattc_read_ind_handler(kernel_msg_id_t const msgid,
                                  struct gattc_read_ind const *param,
                                  kernel_task_id_t const dest_id,
                                  kernel_task_id_t const src_id)
{
	int i;

	printf("%s \r\n",__func__);
	printf("handle = 0x%x\r\n",param->handle);
	printf("length = 0x%x\r\n",param->length);
	printf("offset = 0x%x\r\n",param->offset);
	printf("value = 0x");

	for(i =0 ; i < param->length; i++)
	{
		printf("%x ",param->value[i]);
	}
	printf("\r\n");

	return (KERNEL_MSG_CONSUMED);
}



//主机端接收到从设备发过来的Notify的回调函数
/**
 ****************************************************************************************
 * @brief Handles reception of the @ref GATTC_EVENT_IND message.
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gattc_event_ind_handler(kernel_msg_id_t const msgid,
                                   struct gattc_event_ind const *param,
                                   kernel_task_id_t const dest_id,
                                   kernel_task_id_t const src_id)
{
	int i;

#if 1
	printf("RECEIVE Value = \r\n");
	for(i = 0; i< param->length; i++)
	{
		printf("%02x ",param->value[i]);
	}
	printf("\r\n");
#endif
	return (KERNEL_MSG_CONSUMED);
}

//主机端接收到从设备发过来的Indication的回调函数
static int gattc_event_req_ind_handler(kernel_msg_id_t const msgid,
                                       struct gattc_event_ind const *param,
                                       kernel_task_id_t const dest_id,
                                       kernel_task_id_t const src_id)
{
#if 1
	printf("RECIVE value =  \r\n");
	for(int i = 0; i< param->length; i++)
	{
		printf("%02x ",param->value[i]);
	}
	printf("\r\n");
#endif
	struct gattc_event_cfm *cfm  = KERNEL_MSG_ALLOC(GATTC_EVENT_CFM, src_id, dest_id, gattc_event_cfm);
	cfm->handle = param->handle;
	kernel_msg_send(cfm);

	return (KERNEL_MSG_CONSUMED);
}
/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */
/// Default State handlers definition
const struct kernel_msg_handler sdp_default_state[] =
{
	{SDP_ENABLE_REQ,                (kernel_msg_func_t)sdp_enable_req_handler},
	{SDP_READ_INFO_REQ,             (kernel_msg_func_t)sdp_read_info_req_handler},
	{SDP_WRITE_VALUE_INFO_REQ,      (kernel_msg_func_t)sdp_write_value_info_req_handler},
	{SDP_WRITE_NTF_CFG_REQ,         (kernel_msg_func_t)sdp_write_ntf_cfg_req_handler},
	{GATTC_READ_IND,                (kernel_msg_func_t)gattc_read_ind_handler},
	{GATTC_EVENT_IND,               (kernel_msg_func_t)gattc_event_ind_handler},
	{GATTC_EVENT_REQ_IND,           (kernel_msg_func_t)gattc_event_req_ind_handler},
	{GATTC_CMP_EVT,                 (kernel_msg_func_t)gattc_cmp_evt_handler},
};
// Specifies the message handlers that are common to all states.
const struct kernel_state_handler sdp_default_handler = KERNEL_STATE_HANDLER(sdp_default_state);
#endif /* (BLE_BATT_CLIENT) */
/// @} BASCTASK
