/**
 ****************************************************************************************
 *
 * @file gapc_int.h
 *
 * @brief Generic Access Profile Controller Internal Header.
 *
 * Copyright (C) RivieraWaves 2009-2016
 *
 ****************************************************************************************
 */
#ifndef _GAPC_INT_H_
#define _GAPC_INT_H_

/**
 ****************************************************************************************
 * @addtogroup GAPC_INT Generic Access Profile Controller Internals
 * @ingroup GAPC
 * @brief Handles ALL Internal GAPC API
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "rwip_config.h"
#if (BLE_GAPC)
#include "gapc.h"
#include "gapc_task.h"
#include "common_bt.h"
/*
 * MACROS
 ****************************************************************************************
 */


/*
 * DEFINES
 ****************************************************************************************
 */

/// number of GAP Controller Process
#define GAPC_IDX_MAX                                 BLE_CONNECTION_MAX


/**
 * Repeated Attempts Timer Configuration
 */
/// Repeated Attempts Timer default value (x10ms)
#define GAPC_SMP_REP_ATTEMPTS_TIMER_DEF_VAL         (200)      //2s
/// Repeated Attempts Timer max value (x10ms)
#define GAPC_SMP_REP_ATTEMPTS_TIMER_MAX_VAL         (3000)     //30s
/// Repeated Attempts Timer multiplier
#define GAPC_SMP_REP_ATTEMPTS_TIMER_MULT            (2)


/// states of GAP Controller task
enum gapc_state_id
{
    /// Connection ready state
    GAPC_READY,

    /// Link Operation on-going
    GAPC_LINK_INFO_BUSY     = (1 << GAPC_OP_LINK_INFO),
    /// SMP Operation  on-going
    GAPC_SMP_BUSY           = (1 << GAPC_OP_SMP),
    /// Update Operation  on-going
    GAPC_LINK_UPD_BUSY      = (1 << GAPC_OP_LINK_UPD),
    /// SMP start encryption on-going
    GAPC_ENCRYPT_BUSY       = (1 << GAPC_OP_MAX),

    /// Disconnection  on-going
    GAPC_DISC_BUSY          = 0x1F,
    /// Free state
    GAPC_FREE               = 0X3F,

    /// Number of defined states.
    GAPC_STATE_MAX
};

/// fields definitions.
/// Configuration fields:
///    7    6    5    4    3    2    1    0
/// +----+----+----+----+----+----+----+----+
/// |RFU |SCEN|ROLE|LTK |ENC |BOND| SEC_LVL |
/// +----+----+----+----+----+----+----+----+
enum gapc_fields
{
    /// Link Security Level
    GAPC_SEC_LVL_LSB      = 0,
    GAPC_SEC_LVL_MASK     = 0x03,
    /// Link Bonded or not
    GAPC_BONDED_POS       = 2,
    GAPC_BONDED_BIT       = 0x04,
    /// Encrypted connection or not
    GAPC_ENCRYPTED_POS    = 3,
    GAPC_ENCRYPTED_BIT    = 0x08,
    /// Ltk present and exchanged during pairing
    GAPC_LTK_PRESENT_POS  = 4,
    GAPC_LTK_PRESENT_BIT  = 0x10,
    /// Local connection role
    GAPC_ROLE_POS         = 5,
    GAPC_ROLE_BIT         = 0x20,
    /// Used to know if secure connection is enabled
    GAPC_SC_EN_POS        = 6,
    GAPC_SC_EN_BIT        = 0x40,
};
/*
 * TYPE DECLARATIONS
 ****************************************************************************************
 */

struct l2cc_pdu;

/*
 * GLOBAL VARIABLE DECLARATIONS
 ****************************************************************************************
 */
extern struct gapc_env_tag* gapc_env[GAPC_IDX_MAX];


/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialize Generic Access Profile Controller Module.
 *
 * @param[in] init_type  Type of initialization (@see enum rwip_init_type)
 *
 ****************************************************************************************
 */
void gapc_init(uint8_t init_type);


/**
 ****************************************************************************************
 * @brief A connection has been created, initialize Controller task.
 *
 * This function find first available task index available for new connection.
 * It triggers also connection event to task that has requested the connection.
 *
 * @param[in] msgid      Message id for normal or enhanced privacy
 * @param[in] con_params Connection parameters from lower layers
 * @param[in] requester  Task that request the connection to send indication(s)
 * @param[in] laddr      Local BD Address
 * @param[in] laddr_type Local BD Address Type (PUBLIC or RAND)
 *
 * @return Connection index allocated to the new connection.
 ****************************************************************************************
 */
uint8_t gapc_con_create(kernel_msg_id_t const msgid, struct hci_le_enh_con_cmp_evt const *con_params,
                        kernel_task_id_t requester, bd_addr_t* laddr, uint8_t laddr_type);

/**
 ****************************************************************************************
 * @brief A connection has been disconnected, uninitialized Controller task.
 *
 * unregister connection, and destroy environment variable allocated for current connection.
 *
 * @param[in] conidx  Connection index
 *
 * @return Connection index of the connection.
 ****************************************************************************************
 */
uint8_t gapc_con_cleanup(uint8_t conidx);


/**
 ****************************************************************************************
 * @brief Send Disconnection indication to specific task
 *
 * @param[in] conidx  Connection index
 * @param[in] reason  Disconnection reason
 * @param[in] conhdl  Connection handle
 * @param[in] dest_id Message destination ID
 *
 ****************************************************************************************
 */
void gapc_send_disconect_ind(uint8_t conidx,  uint8_t reason, uint8_t conhdl,
                              kernel_task_id_t dest_id);



/**
 * @brief Send a complete event of ongoing executed operation to requester.
 * It also clean-up variable used for ongoing operation.
 *
 * @param[in] conidx Connection index
 * @param[in] op_type       Operation type.
 * @param[in] status Status of completed operation
 */
void gapc_send_complete_evt(uint8_t conidx, uint8_t op_type, uint8_t status);

/**
 ****************************************************************************************
 * @brief Send operation completed message with status error code not related to a
 * running operation.
 *
 * @param[in] conidx    Connection index
 * @param[in] operation Operation code
 * @param[in] requester requester of operation
 * @param[in] status    Error status code
 ****************************************************************************************
 */
void gapc_send_error_evt(uint8_t conidx, uint8_t operation, const kernel_task_id_t requester, uint8_t status);


/**
 ****************************************************************************************
 * @brief Get operation on going
 *
 * @param[in] conidx        Connection Index
 * @param[in] op_type       Operation type.
 *
 * @return operation code on going
 ****************************************************************************************
 */
uint8_t gapc_get_operation(uint8_t conidx, uint8_t op_type);

/**
 ****************************************************************************************
 * @brief Get operation pointer
 *
 * @param[in] conidx        Connection Index
 * @param[in] op_type       Operation type.
 *
 * @return operation pointer on going
 ****************************************************************************************
 */
void* gapc_get_operation_ptr(uint8_t conidx, uint8_t op_type);


/**
 ****************************************************************************************
 * @brief Set operation pointer
 *
 * @param[in] conidx        Connection Index
 * @param[in] op_type       Operation type.
 * @param[in] op            Operation pointer.
 *
 ****************************************************************************************
 */
void gapc_set_operation_ptr(uint8_t conidx, uint8_t op_type, void* op);

/**
 ****************************************************************************************
 * @brief Operation execution not finish, request kernel to reschedule it in order to
 * continue its execution
 *
 * @param[in] conidx        Connection Index
 * @param[in] op_type       Operation type.
 *
 * @return if operation has been rescheduled (not done if operation pointer is null)
 ****************************************************************************************
 */
bool gapc_reschedule_operation(uint8_t conidx, uint8_t op_type);

/**
 ****************************************************************************************
 * @brief Get requester of on going operation
 *
 * @param[in] conidx        Connection Index
 * @param[in] op_type       Operation type.
 *
 * @return ID of task that requested to execute the operation
 ****************************************************************************************
 */
kernel_task_id_t gapc_get_requester(uint8_t conidx, uint8_t op_type);

/**
 ****************************************************************************************
 * @brief Update link authentication level
 *
 * @param[in] conidx      Connection index
 * @param[in] auth        Link authentication level
 * @param[in] ltk_present Link paired and an LTK has been exchanged
 *
 ****************************************************************************************
 */
void gapc_auth_set(uint8_t conidx, uint8_t auth, bool ltk_present);

/**
 ****************************************************************************************
 * @brief Retrieve link authentication level
 *
 * @param[in] conidx Connection index
 * @return Link authentication level
 ****************************************************************************************
 */
uint8_t gapc_auth_get(uint8_t conidx);

/**
 ****************************************************************************************
 * @brief Update task state
 *
 * @param[in] conidx Connection index
 * @param[in] state to update
 * @param[in] set state to busy (true) or idle (false)
 *
 ****************************************************************************************
 */
void gapc_update_state(uint8_t conidx, kernel_state_t state, bool busy);


/**
 ****************************************************************************************
 * @brief Checks connection parameters values
 *
 * @param[in] intv_max          maximum interval
 * @param[in] intv_min          minimum interval
 * @param[in] latency           latency
 * @param[in] timeout           timeout value
 *
 * @return true if OK, false if not OK
 ****************************************************************************************
 */
bool gapc_param_update_sanity(uint16_t intv_max, uint16_t intv_min, uint16_t latency, uint16_t timeout);

/**
 ****************************************************************************************
 * @brief Get channel selection algorithm used for a given connection identified
 * by its connection index
 *
 * @param[in] conidx    Connection index
 *
 * @return Channel selection algorithm used (0 if algo #1, 1 if algo #2)
 ****************************************************************************************
 */
uint8_t gapc_get_chan_sel_algo(uint8_t conidx);


/**
 ****************************************************************************************
 * @brief Handle reception of a SMP PDU sent by the peer device.
 *
 * @param[in] conidx        Connection Index
 * @param[in] pdu           Unpacked PDU
 * @param[in] status        status of the execution
 ****************************************************************************************
 */
void gapc_smp_pdu_recv(uint8_t conidx, struct l2cc_pdu *pdu, uint8_t status);

/**
 ****************************************************************************************
 * @brief Clean-up SMP operation
 *
 * @param[in] conidx        Connection Index
 ****************************************************************************************
 */
void gapc_smp_cleanup(uint8_t conidx);

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Check if current operation can be processed or not.
 * if it can be proceed, initialize an operation request.
 * If a command complete event with error code can be triggered.
 *
 * Function returns how the message should be handled by message handler.
 *
 * @param[in] conidx        Connection Index
 * @param[in] op_type       Operation type.
 * @param[in] op_msg        Requested operation message (note op_msg cannot be null)
 * @param[in] supp_ops      Supported operations array.
 *                          Latest array value shall be GAPC_NO_OP.
 *
 * @return operation can be executed if message status equals KERNEL_MSG_NO_FREE,
 * else nothing to do, just exit from the handler.
 ****************************************************************************************
 */
int gapc_process_op(uint8_t conidx, uint8_t op_type, void* op_msg, enum gapc_operation* supp_ops);




/*
 * TASK DESCRIPTOR DECLARATIONS
 ****************************************************************************************
 */

#endif // (BLE_GAPC)
/// @} GAPC_INT

#endif /* _GAPC_INT_H_ */
