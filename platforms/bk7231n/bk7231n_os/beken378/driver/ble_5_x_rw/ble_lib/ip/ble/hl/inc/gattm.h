/**
 ****************************************************************************************
 *
 * @file gattm.h
 *
 * @brief Header file - GATT Manager.
 *
 * Copyright (C) RivieraWaves 2009-2016
 *
 *
 ****************************************************************************************
 */

#ifndef GATTM_H_
#define GATTM_H_



/**
 ****************************************************************************************
 * @addtogroup GATTM Generic Attribute Profile Manager
 * @ingroup GATT
 * @brief Generic Attribute Profile.
 *
 * The GATT manager module is responsible for providing an API for all action operations
 * not related to a connection. It's responsible to managing internal database.
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
/* kernel task */
#include "rwip_config.h"

#if (BLE_CENTRAL || BLE_PERIPHERAL)
#include <stdint.h>
#include <stdbool.h>

/*
 * DEFINES
 ****************************************************************************************
 */


/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialization of the GATT manager module.
 * This function performs all the initialization steps of the GATT module.
 *
 * @param[in] init_type  Type of initialization (@see enum rwip_init_type)
 *
 ****************************************************************************************
 */
void gattm_init(uint8_t init_type);


/**
 ****************************************************************************************
 * @brief Initialize GATT attribute database
 *
 * @param[in] start_hdl  Service Start Handle
 *
 * @return status code of attribute database initialization
 * Command status code:
 *  - @ref ATT_ERR_NO_ERROR: If database creation succeeds.
 *  - @ref ATT_ERR_INVALID_HANDLE: If start_hdl given in parameter + nb of attribute override
 *                            some existing services handles.
 *  - @ref ATT_ERR_INSUFF_RESOURCE: There is not enough memory to allocate service buffer.
 *                           or of new attribute cannot be added because all expected
 *                           attributes already add
 ****************************************************************************************
 */
uint8_t gattm_init_attr(uint16_t start_hdl);

/**
 ****************************************************************************************
 * @brief Initialize GATT resources for connection.
 *
 * @param[in] conidx connection record index
 * @param[in] role   device role after connection establishment
 *
 ****************************************************************************************
 */
void gattm_create(uint8_t conidx);
/**
 ****************************************************************************************
 * @brief Cleanup GATT resources for connection
 *
 * @param[in] conidx   connection record index
 *
 ****************************************************************************************
 */
void gattm_cleanup(uint8_t conidx);


#if (BLE_ATTS)

/**
 ****************************************************************************************
 * @brief Convert attribute handle to GATT database attribute index.
 *
 * @param[in] handle    Attribute Handle
 *
 * @return GATT database attribute index (ATT_INVALID_IDX if not found)
 ****************************************************************************************
 */
uint8_t gattm_att_hdl_to_idx(uint16_t handle);

/**
 ****************************************************************************************
 * @brief Convert GATT database attribute index to attribute handle.
 *
 * @param[in] att_idx    GATT database attribute index
 *
 * @return Attribute Handle (ATT_INVALID_HANDLE if not found)
 ****************************************************************************************
 */
uint16_t gattm_att_idx_to_hdl(uint8_t att_idx);
#endif //(BLE_ATTS)
#endif /* (BLE_CENTRAL || BLE_PERIPHERAL) */

/// @} GATTM
#endif // GATTM_H_
