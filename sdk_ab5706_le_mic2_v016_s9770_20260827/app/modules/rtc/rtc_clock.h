#ifndef _RTC_CLOCK_H
#define _RTC_CLOCK_H

typedef struct {
	u8 tm_sec;
	u8 tm_min;
	u8 tm_hour;
	u8 tm_mday;
	u8 tm_mon;
	u8 tm_year;
	u8 tm_wday;
} rtc_time_t;
extern rtc_time_t rtc_tm;

typedef struct {
    u8 type         : 1,                    //0: 时钟界面， 1：闹钟界面
       setting      : 1,                    //时间调整标志
       disp_update  : 1,
       res          : 5;                    //保留

    u8 index;                               //时间调整index
    u8 cnt;                                 //计数，设置时间界面停留10秒自动恢复
} time_cb_t;

extern time_cb_t tm_cb;


void rtc_clock_init(void);
void rtc_clock_update(u32 time_s);
void rtc_clock_read(void);

void rtc_power_on_calibration(void);
void sniff_rc_init(void);
void rtc_alarm_disable(void);
void rtc_set_alarm_wakeup(u32 nsec);
void rtc_pwd_calibration(void);
void rtc_pwroff_enter(void);
#endif
