#include "include.h"
#include "func.h"

void sfunc_pwrdown(u8 vusb_wakeup_en)
{
#if OFFLINE_LOG_EN
    rtc_32k_configure();
#endif
#if (CHARGE_BOX_TYPE == CBOX_SSW) || QTEST_EN
    bsp_vusb_uart_dis();
#endif
    lowpwr_pwroff_mode(vusb_wakeup_en);
}

AT(.text.lowpwr.pwroff)
void func_pwroff(int pwroff_tone_en)
{
    printf("%s: %d\n", __func__, pwroff_tone_en);
#if OFFLINE_LOG_EN
    offline_log_end();
#endif
    bsp_res_w4_finish(false);
    bsp_res_cleanup();

#if BT_BACKSTAGE_EN
    func_bt_chk_off(sys_cb.discon_reason);
#endif

#if (CHARGE_BOX_TYPE == CBOX_SSW)
    if (vhouse_cb.inbox_sta) {
        charge_box_inbox_wakeup_enable();      //默认inbox 低电平唤醒
    }
#endif
    gsensor_lowpwr();
    led_power_down();

#if WARNING_POWER_OFF
    bsp_res_set_enable(true);
    if (pwroff_tone_en == 1) {
        bsp_res_play(RES_IDX_POWEROFF);
    } else if (pwroff_tone_en == 2) {
#if WARNING_FUNC_AUX
        bsp_res_play(RES_IDX_AUX_MODE);
#endif
    }
#endif

    bsp_res_w4_finish(true);
    bsp_res_set_enable(false);
    bsp_res_cleanup();

    gui_off();

#if SOFT_POWER_ON_OFF
#if !PWRKEY_2_HW_PWRON
    {
        u8  dcin_cnt = 0;
        #if TKEY_EN
            tkey_press_timeout_clr();
        #endif
        while (PWRKEY_IS_PRESS() || TKEY_IS_PRESS()) {      //等待PWRKWY松开
            if (CHARGE_DC_IN()) {
                dcin_cnt++;
                if (dcin_cnt > 3) {
                    sw_reset_kick(SW_RST_DC_IN);            //直接复位进入充电
                }
            } else {
                dcin_cnt = 0;
            }
    #if TKEY_EN
            if (sys_cb.tkey_pwrdwn_en) {
                tkey_press_timeout_process();
            }
    #endif
            delay_5ms(2);
            WDT_CLR();
        }
    }
#endif

    dac_power_off();                    //dac power down
    bsp_saradc_exit();                  //close saradc及相关通路模拟

    if (CHARGE_DC_IN()) {
        WDT_RST();                      //DC_IN, RST to power_on_check()进入假关机
    }

    if ((PWRKEY_2_HW_PWRON) && (sys_cb.poweron_flag)) {
        WKO_HIGH_WAKEUP_EN();
    }
    sfunc_pwrdown(1);
#else // SOFT_POWER_ON_OFF
    dac_power_off();                    //dac power down
    bsp_saradc_exit();                  //close saradc及相关通路模拟

    lowpwr_lowbat_mode();                    //低电关机进入Sleep Mode
#endif // SOFT_POWER_ON_OFF
}
