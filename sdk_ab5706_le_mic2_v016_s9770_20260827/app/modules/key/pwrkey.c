#include "include.h"


AT(.com_text.bsp.key)
void pwrkey10s_counter_clr(void)
{
    if (POWKEY_10S_RESET) {
        RTCCON10 = BIT(10);                 //clear pwrkey10s pending and counter
    }
}

void pwrkey_off(void)
{
    GPIOBDE &= ~BIT(5);
    GPIOBDIR |= BIT(5);
    GPIOBPU &= ~BIT(5);
    GPIOBPD &= ~BIT(5);
    //RTCCON13 &= ~(BIT(0) | BIT(6) | BIT(12) | BIT(18));   //wk pin0 disable
    RTCCON1 &= ~(BIT(0) | BIT(2) | BIT(4));
}

void pwrkey_init(void)
{
#if PWRKEY_EN
    //adcch_io_pu10k_enable(ADCCH_WKO);
    pwr_usage_id = pwrkey_table[0].usage_id;
//    RTCCON13 |= BIT(0) | BIT(6) | BIT(18);  //wk pin0 wakeup, input, pullup10k enable
    RTCCON1 |= (BIT(0) | BIT(2) | BIT(4));    //wk pin0 wakeup, input, pullup10k enable,下降沿唤醒
    RTCCON1 &= ~(BIT(1) | BIT(6) | BIT(8));
//    RTCCON1 &= ~(BIT(2) | BIT(4) | BIT(8)); //wk pin0 wakeup, input, pulldown10k enable,上升沿唤醒
//    RTCCON1 |= BIT(0) | BIT(1) | BIT(6);
    saradc_set_channel(BIT(ADCCH_WKO));
#else
    pwrkey_off();
#endif
}

#if PWRKEY_EN
AT(.com_text.port.key)
uint8_t pwrkey_get_val(void)
{
#if PWRKEY_EN || PWRKEY_2_HW_PWRON
    uint8_t num = 0;

    while (saradc_get_value8(ADCCH_WKO) > pwrkey_table[num].adc_val) {
        num++;
    }

//    static u32 ticks = 0;     //打印查看pwrkey adc值
//    static char str_pwrkey[9] = "[%d,%d]\n";
//    if (tick_check_expire(ticks,300)) {
//        ticks = tick_get();
//        printf(str_pwrkey,num, saradc_get_value8(ADCCH_WKO));
//    }

    return pwrkey_table[num].usage_id;
#else
    return KEY_NULL;
#endif
}

bool pwrkey_get_status(void)
{
    u32 delay = 20;
    u32 pre_sta = 0;
    u32 sta_cnt = 0;
    while(delay--) {
        u32 tmp_sta = (RTCCON & BIT(19));
        if(pre_sta == tmp_sta) {
            sta_cnt++;
        } else {
            pre_sta = tmp_sta;
            sta_cnt = 0;
        }
        delay_ms(1);
        if(sta_cnt > 4) {
            break;
        }
    }

    return (pre_sta == 0)? true : false;
}
#endif
