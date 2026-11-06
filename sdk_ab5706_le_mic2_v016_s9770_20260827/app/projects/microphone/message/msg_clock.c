#include "include.h"
#include "func.h"
#include "func_clock.h"
//本文件用以处理控制消息 修改时间和定时

#if FUNC_CLOCK_EN
AT(.text.func.clock.msg)
void func_clock_message(u16 msg)
{
    switch (msg) {
        case MSG_CLOCK_SETTING:
            /*测试例程*/
//            func_cb.sta = FUNC_PWROFF;
//            break;
            if (tm_cb.setting) {
                printf("MSG_CLOCK_SETTING 0\n");//退出
                tm_cb.setting = 0;
                tm_cb.index = 0;
                rtc_tm.tm_sec = 0;
                RTCCNT = rtc_tm_to_time(&rtc_tm);

            } else {
                printf("MSG_CLOCK_SETTING 1\n");//进入设置
                tm_cb.setting = 1;
                tm_cb.index = 1;
                tm_cb.cnt = 0;
                gui_box_flicker_set(5, 0xff, tm_cb.index);        //flicker freq 100ms*5
                rtc_time_to_tm(RTCCNT, &rtc_tm);

            }
            break;

        case MSG_CLOCK_PREV:
            if (tm_cb.setting) {
                tm_cb.cnt = 0;
                tm_cb.index = (tm_cb.index == 1) ? 2 : 1;
                gui_box_flicker_set(5, 0xff, tm_cb.index);

            }
            break;

        case MSG_CLOCK_NEXT:
            if (tm_cb.setting) {
                tm_cb.cnt = 0;
                tm_cb.index = (tm_cb.index == 2) ? 1 : 2;
                gui_box_flicker_set(5, 0xff, tm_cb.index);

            }
            break;

        case MSG_CLOCK_MINUP:
            if (tm_cb.setting) {
                tm_cb.cnt = 0;
                if (tm_cb.index == 1) {
                    rtc_tm.tm_min++;
                    if (rtc_tm.tm_min > 59) {
                        rtc_tm.tm_min = 0;
                    }

                } else if (tm_cb.index == 2) {
                    rtc_tm.tm_hour++;
                    if (rtc_tm.tm_hour > 23) {
                        rtc_tm.tm_hour = 0;
                    }

                }
            }
            break;

        case MSG_CLOCK_MINDWON:

            if (tm_cb.setting) {
                tm_cb.cnt = 0;
                if (tm_cb.index == 1) {
                    if (rtc_tm.tm_min > 0) {
                        rtc_tm.tm_min--;
                    } else {
                        rtc_tm.tm_min = 59;
                    }

                } else if (tm_cb.index == 2) {
                    if (rtc_tm.tm_hour > 0) {
                        rtc_tm.tm_hour--;
                    } else {
                        rtc_tm.tm_hour = 23;
                    }

                }
            }
            break;

        case MSG_SYS_500MS:
            if(tm_cb.setting) {
                if (tm_cb.cnt >= 20) {
                    tm_cb.setting = 0;
                    tm_cb.index = 0;
                    tm_cb.cnt = 0;
                }
                tm_cb.cnt++;
            }
            if ((!tm_cb.setting) && (!tm_cb.type)) {
                rtc_time_to_tm(RTCCNT, &rtc_tm);

            }
            break;


        default:
            func_message(msg);
            break;
    }
}
#endif //FUNC_CLOCK_EN
