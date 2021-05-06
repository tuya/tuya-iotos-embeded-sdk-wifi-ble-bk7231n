#ifndef _TX_EVM_PUB_H_
#define _TX_EVM_PUB_H_

#include "hal_desc.h"

#define TX_LEGACY_MODE              (1)
#define TX_HT_VHT_MODE              (2)

#define TX_LEGACY_DATA_LEN_MASK              (0xFFF)
#define TX_HT_VHT_DATA_LEN_MASK              (0xFFFFF)

typedef enum
{
    SINGLE_CARRIER_11B,
    SINGLE_CARRIER_11G,
    SINGLE_CARRIER_BLE
} SC_TYPE_T;

/*******************************************************************************
* Function Declarations
*******************************************************************************/
extern void evm_via_mac_evt(INT32 dummy);
extern void evm_via_mac_begin(void);
extern void evm_via_mac_continue(void);
extern UINT32 evm_bypass_mac_set_tx_data_length(UINT32 modul_format, UINT32 len, UINT32 rate, UINT32 bandwidth, UINT32 need_change);
extern UINT32 evm_bypass_mac_set_rate_mformat(UINT32 ppdu_rate, UINT32 m_format);
extern void evm_bypass_mac_set_channel(UINT32 channel);
extern void evm_via_mac_set_channel(UINT32 channel);
extern void evm_bypass_mac_test(void);
extern void evm_via_mac_set_rate(HW_RATE_E rate, uint32_t modul_format, uint32_t guard_i_tpye);
extern void evm_set_bandwidth(UINT32 bandwidth);
extern void evm_stop_bypass_mac(void);
extern void evm_bypass_mac_set_guard_i_type(UINT32 gi_tpye);
extern void evm_start_bypass_mac(void);
extern void evm_bypass_set_single_carrier(SC_TYPE_T type, UINT32 rate);
extern void evm_bypass_ble_test_start(UINT32 channel);
extern void evm_bypass_ble_test_stop(void);
extern void evm_via_mac_init(void);
extern void evm_via_mac_set_power(UINT32 pwr_idx);
extern void evm_via_mac_set_bandwidth(UINT32 bandwidth);
extern UINT32 evm_bypass_mac_set_txdelay(UINT32 delay_us);

#endif //_TX_EVM_PUB_H_
// eof

