#ifndef _PWM_H_
#define _PWM_H_

#if (CFG_SOC_NAME == SOC_BK7231N)


#ifdef  PWM_DEBUG
#define PWM_PRT      bk_printf
#define PWM_WARN     warning_prf
#define PWM_FATAL    bk_printf
#else
#define PWM_PRT      null_prf
#define PWM_WARN     null_prf
#define PWM_FATAL    null_prf
#endif

#define REG_PWM_BASE_ADDR                   	(0x00802B00UL)
#define PWM_CHANNEL_NUMBER_ALL              	6
#define PWM_CHANNEL_NUMBER_MAX              	(PWM_CHANNEL_NUMBER_ALL - 1)
#define PWM_CHANNEL_NUMBER_MIN              	0

#define REG_PWM_GROUP_ADDR(x)               	(REG_PWM_BASE_ADDR + (0x40 * x))

#define REG_PWM_GROUP_CTRL_ADDR(x)          	(REG_PWM_GROUP_ADDR(x) + 0x00 * 4)
#define REG_PWM_CTRL_MASK                   	0xFFFFFFFFUL
#define REG_PWM_GROUP_CTRL(x)               	(*((volatile unsigned long *) REG_PWM_GROUP_CTRL_ADDR(x)))

#define PWM_GROUP_MODE_SET_BIT(x)				(8*x)

#define PWM_GROUP_PWM_ENABLE_BIT(x)				(8*x + 3)
#define PWM_GROUP_PWM_ENABLE_MASK(x)			(1 <<PWM_GROUP_PWM_ENABLE_BIT(x))

#define PWM_GROUP_PWM_INT_ENABLE_BIT(x)			(8*x + 4)
#define PWM_GROUP_PWM_STOP_BIT(x)				(8*x + 5)

#define PWM_GROUP_PWM_INT_LEVL_BIT(x)			(8*x + 6)
#define PWM_GROUP_PWM_INT_LEVL_MASK(x)			(1<< PWM_GROUP_PWM_INT_LEVL_BIT(x))

#define PWM_GROUP_PWM_CFG_UPDATA_BIT(x)			(8*x + 7)
#define PWM_GROUP_PWM_CFG_UPDATA_MASK(x)		(1<< PWM_GROUP_PWM_CFG_UPDATA_BIT(x))


#define PWM_GROUP_PWM_PRE_DIV_BIT				16
#define PWM_GROUP_PWM_PRE_DIV_MASK				(0xFF << 16)

#define PWM_GROUP_PWM_GROUP_MODE_BIT			24
#define PWM_GROUP_PWM_GROUP_MODE_MASK			1 << 24

#define PWM_GROUP_PWM_GROUP_MODE_ENABLE_BIT		25	
#define PWM_GROUP_PWM_GROUP_MODE_ENABLE_MASK	1 << 25	

#define PWM_GROUP_PWM_INT_STAT_BIT(x)			(x + 30)
#define PWM_GROUP_PWM_INT_STAT_CLEAR(x)			(1 << PWM_GROUP_PWM_INT_STAT_BIT(x))
#define PWM_GROUP_PWM_INT_STAT_MASK(x)			(1 << PWM_GROUP_PWM_INT_STAT_BIT(x))


#define REG_GROUP_PWM0_T1_ADDR(x)           (REG_PWM_GROUP_ADDR(x) + 0x01 * 4)
#define REG_GROUP_PWM0_T1(x)                (*((volatile unsigned long *) REG_GROUP_PWM0_T1_ADDR(x) ))

#define REG_GROUP_PWM0_T2_ADDR(x)           (REG_PWM_GROUP_ADDR(x) + 0x02 * 4)
#define REG_GROUP_PWM0_T2(x)                (*((volatile unsigned long *) REG_GROUP_PWM0_T2_ADDR(x) ))

#define REG_GROUP_PWM0_T3_ADDR(x)           (REG_PWM_GROUP_ADDR(x) + 0x03 * 4)
#define REG_GROUP_PWM0_T3(x)                (*((volatile unsigned long *) REG_GROUP_PWM0_T3_ADDR(x) ))

#define REG_GROUP_PWM0_T4_ADDR(x)           (REG_PWM_GROUP_ADDR(x) + 0x04 * 4)
#define REG_GROUP_PWM0_T4(x)                (*((volatile unsigned long *) REG_GROUP_PWM0_T4_ADDR(x) ))



#define REG_GROUP_PWM1_T1_ADDR(x)           (REG_PWM_GROUP_ADDR(x) + 0x05 * 4)
#define REG_GROUP_PWM1_T1(x)                (*((volatile unsigned long *) REG_GROUP_PWM1_T1_ADDR(x) ))

#define REG_GROUP_PWM1_T2_ADDR(x)           (REG_PWM_GROUP_ADDR(x) + 0x06 * 4)
#define REG_GROUP_PWM1_T2(x)                (*((volatile unsigned long *) REG_GROUP_PWM1_T2_ADDR(x) ))

#define REG_GROUP_PWM1_T3_ADDR(x)           (REG_PWM_GROUP_ADDR(x) + 0x07 * 4)
#define REG_GROUP_PWM1_T3(x)                (*((volatile unsigned long *) REG_GROUP_PWM1_T3_ADDR(x) ))

#define REG_GROUP_PWM1_T4_ADDR(x)           (REG_PWM_GROUP_ADDR(x) + 0x08 * 4)
#define REG_GROUP_PWM1_T4(x)                (*((volatile unsigned long *) REG_GROUP_PWM1_T4_ADDR(x) ))


#define REG_GROUP_PWM_CPU_ADDR(x)           (REG_PWM_GROUP_ADDR(x) + 0x09 * 4)
#define REG_GROUP_PWM_CPU(x)                (*((volatile unsigned long *) REG_GROUP_PWM_CPU_ADDR(x) ))

#define PWM_CPU_RD0							 1 << 0
#define PWM_CPU_RD1							 1 << 1


#define REG_GROUP_PWM0_RD_DATA_ADDR(x)      (REG_PWM_GROUP_ADDR(x) + 0x0a * 4)
#define REG_GROUP_PWM0_RD_DATA_(x)          (*((volatile unsigned long *) REG_GROUP_PWM0_RD_DATA_ADDR(x) ))

#define REG_GROUP_PWM1_RD_DATA_ADDR(x)      (REG_PWM_GROUP_ADDR(x) + 0x0b * 4)
#define REG_GROUP_PWM1_RD_DATA_(x)          (*((volatile unsigned long *) REG_GROUP_PWM1_RD_DATA_ADDR(x) ))


#define CHANNEL_NO                                  6

UINT32 pwm_ctrl(UINT32 cmd, void *param);

void pwm_init(void);
void pwm_exit(void);
void pwm_isr(void);
extern UINT8 init_pwm_param(pwm_param_t *pwm_param, UINT8 enable);
extern void pwm_group_mode_enable(UINT8 ucChannel);
extern UINT8 pwm_init_levl_set_low(UINT8 ucChannel);
extern UINT8 pwm_init_levl_set_high(UINT8 ucChannel);
extern void pwm_unit_enable(UINT8 ucChannel);
extern UINT8 pwm_update_param_enable(UINT8 ucChannel);
extern UINT8 pwm_group_update_param_enable(UINT8 channel1,UINT8 channel2,pwm_param_t *pwm_param);
extern UINT8 pwm_nogroup_update_param_enable(UINT8 channel1,UINT8 channel2,pwm_param_t *pwm_param);


#endif 

#endif //_PWM_H_
