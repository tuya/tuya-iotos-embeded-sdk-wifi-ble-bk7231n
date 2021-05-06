/**
****************************************************************************************
*
* @file SCH_PLAN.h
*
* @brief SCH_PLAN main module
*
* Copyright (C) RivieraWaves 2009-2018
*
*
****************************************************************************************
*/

#ifndef _SCH_PLAN_H_
#define _SCH_PLAN_H_

/**
 ****************************************************************************************
 * @addtogroup SCH
 * @brief Entry points of the Scheduling Planner module
 *
 * This module contains the primitives that allow stacks to plan periodic activities
 *
 * @{
 ****************************************************************************************
 */

#include <stdint.h>               // Standard integer definitions
#include <stdbool.h>              // Standard boolean definitions
#include "common_list.h"              // List management functions definitions


/*
 * MACROS
 ****************************************************************************************
 */



/*
 * DEFINES
 ****************************************************************************************
 */



/*
 * DEFINITIONS
 ****************************************************************************************
 */

/// SCH_PLAN error codes
enum sch_plan_error
{
    /// SCH_PLAN request succeed
    SCH_PLAN_ERROR_OK                 = 0,
    /// SCH_PLAN request rejected error
    SCH_PLAN_ERROR_REJECTED,
    /// SCH_PLAN element not found error
    SCH_PLAN_ERROR_NOT_FOUND,
    /// SCH_PLAN request rejected due to bandwidth full error
    SCH_PLAN_ERROR_BW_FULL
};

/// SCH_PLAN mobility levels
enum sch_plan_mb_lvl
{
    /// SCH_PLAN mobility level 0 (not movable)
    SCH_PLAN_MB_LVL_0                 = 0,
    /// SCH_PLAN mobility level 1
    SCH_PLAN_MB_LVL_1,
    /// SCH_PLAN mobility level 2
    SCH_PLAN_MB_LVL_2,
    /// SCH_PLAN mobility level 3
    SCH_PLAN_MB_LVL_3,
    /// SCH_PLAN mobility level 4
    SCH_PLAN_MB_LVL_4
};

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// Interval element strcuture
struct sch_plan_elt_tag
{
    /// List element for chaining in the Interval list
    struct common_list_hdr hdr;
    /// Interval used (in slots)
    uint32_t interval;
    /// Offset used (in slots)
    uint32_t offset;
    /// Minimum duration (in slots)
    uint32_t duration_min;
    /// Maximum duration (in slots)
    uint32_t duration_max;
    /// Connection handle used
    uint16_t conhdl;
    /// Mobility level
    uint8_t mobility;
    /************************************************************************************
     * ISR CALLBACKS
     ************************************************************************************/
    /// Move notification call back function
    void (*cb_move)(uint16_t id);

};

/// API request parameters
struct sch_plan_req_param
{
    /// Interval minimum requested (in slots)
    uint32_t interval_min;
    /// Interval maximum requested (in slots)
    uint32_t interval_max;
    /// Duration minimum requested (in slots)
    uint32_t duration_min;
    /// Duration maximum requested (in slots)
    uint32_t duration_max;
    /// Connection handle
    uint16_t conhdl;
    /// Preferred periodicity (in slots)
    uint8_t pref_period;
    /// Odd offset or even offset
    bool odd_offset;
    /// Interval returned (in slots)
    uint32_t interval;
    /// Min offset returned (in slots)
    uint32_t offset_min;
    /// Max offset returned (in slots)
    uint32_t offset_max;
};

/// API check parameters
struct sch_plan_chk_param
{
    /// Interval (in slots)
    uint32_t interval;
    /// Duration minimum requested (in slots)
    uint32_t duration_min;
    /// Offset requested (in slots)
    uint32_t offset;
    /// Connection handle
    uint16_t conhdl;
};

/*
 * GLOBAL VARIABLE DECLARATIONS
 ****************************************************************************************
 */


/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialize the Scheduling Planner.
 *
 * @param[in] init_type  Type of initialization (@see enum rwip_init_type)
 ****************************************************************************************
 */
void sch_plan_init(uint8_t init_type);

/**
 ****************************************************************************************
 * @brief Insert the element in the common element queue
 ****************************************************************************************
 */
void sch_plan_set(struct sch_plan_elt_tag *elt_to_add);

/**
 ****************************************************************************************
 * @brief Remove the element from the common element queue
 ****************************************************************************************
 */
void sch_plan_rem(struct sch_plan_elt_tag *elt_to_remove);

/**
 ****************************************************************************************
 * @brief Choose appropriate parameters for the activity
 *
 * @param[in] input_param  parameters used to compute output values
 *
 * @param[in] output_param appropriate parameters for the activity
 *
 * @return See sch_plan_error error codes
 ****************************************************************************************
 */
uint8_t sch_plan_req(struct sch_plan_req_param* req_param);

/**
 ****************************************************************************************
 * @brief Check if the provided offset is acceptable
 *
 * @param[in] input_param  parameters used to perform the check
 *
 * @param[in] output_param offset used for the interval
 *
 * @return See sch_plan_error error codes
 ****************************************************************************************
 */
uint8_t sch_plan_chk(struct sch_plan_chk_param* chk_param);

/**
 ****************************************************************************************
 * @brief Choose an appropriate offset according to the interval with a required air duration
 *
 * @param[in]     link_id     Link identifier
 * @param[in]     role        Role of the activity
 * @param[in]     interval    Activity interval (in slots)
 * @param[in]     duration    Activity event duration (in slots)
 * @param[in|out] min_offset  Minimum Activity offset as input/computed (in slots)
 * @param[in|out] max_offset  Maximum Activity offset as input/computed (in slots)
 *
 * @return - SCH_PLAN_ERROR_OK if succeed
 *         - SCH_PLAN_ERROR_REJECTED if invalid parameters
 *         - SCH_PLAN_ERROR_BW_FULL if not able to find available bandwidth
 ****************************************************************************************
 */
uint8_t sch_plan_position_range_compute(uint8_t link_id, uint8_t role, uint16_t interval, uint16_t duration,
                                        uint16_t* min_offset, uint16_t* max_offset);


///@} SCH_PLAN

#endif // _SCH_PLAN_H_
