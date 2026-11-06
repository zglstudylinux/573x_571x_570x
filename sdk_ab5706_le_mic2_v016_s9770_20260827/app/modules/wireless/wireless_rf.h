#ifndef _WIRELESS_RF_H
#define _WIRELESS_RF_H


void btrf_power_balance_init(void);
void btrf_power_balance_exit(void);
void btrf_power_balance_level_delay_set(u32 delay_us, u8 level);
void btrf_power_balance_cb(u32 rf_sta);
void timer_isr_for_btrf_power_balance(void);

void bb_rf_ext_ctl_init(void);
void btrf_power_balance_level_set(u8 level);
extern volatile u8 btrf_power_level;
#endif
