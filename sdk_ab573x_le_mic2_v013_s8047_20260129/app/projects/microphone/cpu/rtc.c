#include "include.h"

AT(.text.bsp.sys.init)
void rtc_32k_configure(void)
{
    u32 temp = RTCCON0;

//    //xosc_div768_rtc
//    temp &= ~BIT(6);
//    temp |= BIT(9) | BIT(8);                        //sel xosc_div768_rtc
//    RTCCON0 = temp;
//    RTCCON4 |= BIT(17);                             //xosc24m low power clk enable
//    RTCCON2 = 31249;

    //clk2m_rtc_div32
    temp &= ~BIT(6);
    temp &= ~(BIT(9) | BIT(8));
    temp |= BIT(8);
    temp |= BIT(2) | BIT(0);
    RTCCON0 = temp;
    RTCCON2 = sys_get_rc2m_rtc_clk() / 64 - 1;

}

