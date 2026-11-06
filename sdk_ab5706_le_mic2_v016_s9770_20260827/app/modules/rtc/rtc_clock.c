#include "include.h"

#if RTC_CLOCK_EN

rtc_time_t rtc_tm AT(.buf.rtc.clock);
time_cb_t tm_cb AT(.buf.rtc.clock);


AT(.text.clock.init)
void rtc_clock_init(void)
{
    printf("%s\n", __func__);

    rtc_tm.tm_year = (2025-1970);
    rtc_tm.tm_mon  = 1;
    rtc_tm.tm_mday = 14;
    rtc_tm.tm_hour = 0;
    rtc_tm.tm_min  = 0;
    rtc_tm.tm_sec  = 0;
    rtc_tm.tm_wday = get_weekday(rtc_tm.tm_year, rtc_tm.tm_mon, rtc_tm.tm_mday);
    RTCCNT = rtc_tm_to_time(&rtc_tm);
}

///更新RTC时钟接口
AT(.com_huart.clock.update)
void rtc_clock_update(u32 time_s)
{
    //time_s为1970年1月1日0时0分0秒所经过的秒数
    RTCCNT = time_s;
}

///读取RTC时钟
AT(.text.clock.init)
void rtc_clock_read(void)
{
    rtc_time_to_tm(RTCCNT, &rtc_tm);

    printf("rtc_time:%d/%d/%d %d:%d:%d\n", rtc_tm.tm_year + 1970, rtc_tm.tm_mon, rtc_tm.tm_mday, rtc_tm.tm_hour, rtc_tm.tm_min, rtc_tm.tm_sec );
}

//-----------------------------------------------------
///多少秒后闹钟响
AT(.text.func.clock)
void rtc_set_alarm_relative_time(u32 nsec)
{
    rtc_time_to_tm(RTCCNT, &rtc_tm);            //更新时间结构体
    RTCALM = rtc_tm_to_time(&rtc_tm) + nsec;    //设置闹钟相对于当前时间n秒后

}

//设置多少秒后闹钟唤醒
AT(.text.func.clock)
void rtc_set_alarm_wakeup(u32 nsec)
{
    uint rtccon3 = RTCCON3;

    RTCCON0 |= BIT(18);
    RTCCON0 = (RTCCON0 & ~(0X3<<8)) | (0X2<<8);

    RTCCPND = BIT(17);                          //clear RTC alarm pending
    RTCCON9 = BIT(0);                           //clear alarm pending
    rtc_set_alarm_relative_time(nsec);

    rtccon3 |= BIT(8);                          //RTC alarm wakeup enable
    RTCCON3 = rtccon3;
}

///关闭闹钟
AT(.text.rtc)
void rtc_alarm_disable(void)
{
    printf("%s\n", __func__);
    RTCCPND = BIT(17);                          //clear RTC alarm pending
    RTCCON9 = BIT(0);                           //clear alarm pending
	RTCCON &= ~BIT(8);                          //clear sniff wakeup
    RTCCON3 &= ~BIT(8);                         //clear power down wakeup
}

void sniff_rc_init(void)
{
    printf("%s\n", __func__);
    CLKGAT0 |= BIT(18);
    CLKCON1 = (CLKCON1 & ~(3 << 12)) | (2 << 12);   //freq_clkin select xosc_clk
    CLKCON1 = (CLKCON1 & ~(0x3 << 14)) | (0 << 14); //freqdet_clk select rtc_rc2m
    FREQDETCON = (2000-1) << 16;                    //timer pr

    RTCCON0 |= BIT(2);                              //rtc_rc2m
    RTCCON0 &= ~BIT(20);                            //SNIFF_RC_RTC
    RTCCON0 = (RTCCON0 & ~(3 << 16)) | (1 << 16);   //RI_BIAS = 1
    RTCCON0 |= BIT(18);                             //RI_EN_SNIFF = 1
}

///开机校准
void rtc_power_on_calibration(void)
{
    printf("%s\n", __func__);
    rtc_alarm_disable();
    if (cm_read8(PARAM_RTC_CAL_VALID) == 1) {
        sniff_rc_init();
        rtc_calibration_read(PARAM_RTC_CAL_ADDR);
        rtc_clock_calc_by_rc(1);
        rtc_clock_read();
    }
}

///关机校准
void rtc_pwd_calibration(void)
{
    param_init(sys_cb.rst_reason & RST_RTC_PWRUP);

    u32 rtccon9 = RTCCON9;
    if((rtccon9 & 1) == 1){                                  //周期起来一次校准rc rtc
        printf("%s\n", __func__);
        RTCCON9 = 0xfff;                                     //Clr pending
        if (cm_read8(PARAM_RTC_CAL_VALID) == 1) {
            sniff_rc_init();
            rtc_calibration_read(PARAM_RTC_CAL_ADDR);
            rtc_sleep_process();
            rtc_calibration_write(PARAM_RTC_CAL_ADDR);
            cm_write8(PARAM_RTC_CAL_VALID, 1);
            cm_sync();
            rtc_clock_read();
        }

        sfunc_pwrdown(1);
        return;
    }
    //RTCCON9 = 0xfff;                                       //Clr pending
}

void rtc_pwroff_enter(void)
{
    rtc_sleep_enter();
    rtc_calibration_write(PARAM_RTC_CAL_ADDR);
    cm_write8(PARAM_RTC_CAL_VALID, 1);
    cm_sync();
}
#endif
