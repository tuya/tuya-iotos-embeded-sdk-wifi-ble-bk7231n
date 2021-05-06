#ifndef _MCU_PS_PUB_H_
#define _MCU_PS_PUB_H_

#include "typedef.h"
#include "fake_clock_pub.h"
#include "icu_pub.h"

#define     CFG_MCU_PS_SELECT_120M      1

typedef struct  mcu_ps{
    UINT8 mcu_ps_on;
    int peri_busy_count;
    UINT32 mcu_prevent;
}MCU_PS_INFO;

typedef struct  sctrl_mcu_ps{
    UINT8 hw_sleep ;
    UINT8 first_sleep ;    
    UINT8 mcu_use_dco;
}SCTRL_MCU_PS_INFO;

#define     MCU_PS_CONNECT      CO_BIT(0)
#define     MCU_PS_ADD_KEY      CO_BIT(1)
#define     MCU_PS_BLE_FROBID      (1UL<<(2))

#define CHIP_U_MCU_WKUP_USE_TIMER  1


extern void vTaskStepTick( const TickType_t  );
extern void mcu_ps_init(void);
extern void mcu_ps_exit(void);
extern UINT32 mcu_power_save(UINT32 );
extern void mcu_prevent_clear(UINT32 );
extern void mcu_prevent_set(UINT32 );
extern void peri_busy_count_dec(void );
extern void peri_busy_count_add(void );
extern UINT32 peri_busy_count_get(void );
extern UINT32 mcu_prevent_get(void );
extern UINT32 fclk_update_tick(UINT32 tick);
extern void mcu_ps_dump(void);
extern void ps_pwm_reconfig(UINT32 ,UINT8 );
extern void ps_pwm_resume_tick(void);
extern void ps_pwm_suspend_tick(UINT32 );


extern void ps_timer3_measure_prepare(void);
extern UINT32 mcu_ps_tsf_cal(UINT64);
extern UINT32 mcu_ps_machw_cal(void);
extern UINT32 mcu_ps_machw_reset(void);
extern UINT32 mcu_ps_machw_init(void);

#define  PS_PWM_ID  FCLK_PWM_ID
#if (PS_PWM_ID == PWM0)
#define MCU_PS_PWM_COUNTER      PWM0_COUNTER
#define MCU_PS_PWM_DUTY_CYCLE   PWM0_DUTY_CYCLE
#define PWD_MCU_WAKE_PWM_BIT      PWD_PWM0_CLK_BIT
#elif(PS_PWM_ID == PWM1)
#define MCU_PS_PWM_COUNTER      PWM1_COUNTER
#define MCU_PS_PWM_DUTY_CYCLE   PWM1_DUTY_CYCLE
#define PWD_MCU_WAKE_PWM_BIT      PWD_PWM1_CLK_BIT
#elif(PS_PWM_ID == PWM2)
#define MCU_PS_PWM_COUNTER      PWM2_COUNTER
#define MCU_PS_PWM_DUTY_CYCLE   PWM2_DUTY_CYCLE
#define PWD_MCU_WAKE_PWM_BIT      PWD_PWM2_CLK_BIT
#elif(PS_PWM_ID == PWM3)
#define MCU_PS_PWM_COUNTER      PWM3_COUNTER
#define MCU_PS_PWM_DUTY_CYCLE   PWM3_DUTY_CYCLE
#define PWD_MCU_WAKE_PWM_BIT      PWD_PWM3_CLK_BIT
#elif(PS_PWM_ID == PWM4)
#define MCU_PS_PWM_COUNTER      PWM4_COUNTER
#define MCU_PS_PWM_DUTY_CYCLE   PWM4_DUTY_CYCLE
#define PWD_MCU_WAKE_PWM_BIT      PWD_PWM4_CLK_BIT
#elif(PS_PWM_ID == PWM5)
#define MCU_PS_PWM_COUNTER      PWM5_COUNTER
#define MCU_PS_PWM_DUTY_CYCLE   PWM5_DUTY_CYCLE
#define PWD_MCU_WAKE_PWM_BIT      PWD_PWM5_CLK_BIT
#endif

#endif

