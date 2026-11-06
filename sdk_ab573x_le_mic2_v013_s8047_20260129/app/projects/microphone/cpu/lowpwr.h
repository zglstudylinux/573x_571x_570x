#ifndef __LOWPWR_H
#define __LOWPWR_H

typedef bool (*sleep_proc_cb_t)(void);

void lowpwr_lowbat_mode(void);
void lowpwr_pwroff_mode(u8 vusb_wakeup_en);

void lowpwr_sleep_proc(sleep_proc_cb_t sleep_proc_cb);
void sleep_wakeup_config(void);

#endif // __LOWPWR_H
