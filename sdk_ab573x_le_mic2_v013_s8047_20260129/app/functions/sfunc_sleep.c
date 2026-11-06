#include "include.h"
#include "func.h"


AT(.com_text.sleep)
void lowpwr_tout_ticks(void)
{
    if(sys_cb.sleep_delay != -1L && sys_cb.sleep_delay > 0) {
        sys_cb.sleep_delay--;
    }
    if(sys_cb.pwroff_delay != -1L && sys_cb.pwroff_delay > 0) {
        sys_cb.pwroff_delay--;
    }
}

AT(.com_text.sleep)
bool sys_sleep_check(u32 *sleep_time)
{
    if(*sleep_time > sys_cb.sleep_wakeup_time) {
        *sleep_time = sys_cb.sleep_wakeup_time;
        return true;
    }
    return false;
}

AT(.sleep_text.sleep)
void sys_sleep_cb(u8 lpclk_type)
{
    //注意！！！！！！！！！！！！！！！！！
    //此函数只能调用sleep_text或com_text函数

    //此处关掉影响功耗的模块
    FLASH_SLEEP_ENTER();

    sys_sleep_proc(lpclk_type);                //enter sleep

    //唤醒后，恢复模块功能
    FLASH_SLEEP_EXIT();
}

//休眠定时器，500ms进一次
AT(.sleep_text.sleep)
uint32_t sleep_timer(void)
{
    uint32_t ret = 0;

    app_sleep_mode_process();
#if CHARGE_EN
    if (xcfg_cb.charge_en) {
        sleep_charge_process();
    }
#endif // CHARGE_EN
    if (led_bt_sleep()) {
        sys_cb.sleep_wakeup_time = 50000/312;

#if VBAT_DETECT_EN
        bsp_saradc_init();
        if ((xcfg_cb.lowpower_warning_en) && (sys_cb.vbat < (sys_cb.vbat_warning - 50))) {
            //低电需要唤醒sniff mode
            ret = 2;
        }
        bsp_saradc_exit();
#endif // VBAT_DETECT_EN

#if CHARGE_EN
        if (!port_2led_is_sleep_en()) {
            ret = 2;
        }
#endif // CHARGE_EN
    } else {
        sys_cb.sleep_wakeup_time = -1L;
    }
    if(sys_cb.pwroff_delay != -1L) {
        if(sys_cb.pwroff_delay > 5) {
            sys_cb.pwroff_delay -= 5;
        } else {
            sys_cb.pwroff_delay = 0;
            return 1;
        }
    }

    if ((PWRKEY_2_HW_PWRON) && (!PWRKEY_IS_PRESS())){
        ret = 1;
    }
    return ret;
}

AT(.text.lowpwr.sleep)
static void sfunc_sleep_dac_off(u8 off)
{

    if (off) {
        if (SLEEP_DAC_OFF_EN) {
            dac_power_off();                //dac power down
        } else {
            dac_channel_disable();          //only dac channel master disable
        }
    } else {
        if (SLEEP_DAC_OFF_EN) {
            dac_restart();
        } else {
            dac_channel_enable();
        }
    }
}

AT(.text.lowpwr.sleep)
static bool sfunc_sleep_proc_cb(void)
{
    uint32_t status, ret;

    sys_cb.sleep_wakeup_time = -1L;

    while(bt_is_sleep()) {
        WDT_CLR();
        vusb_reset_clr();
        bt_thread_check_trigger();
        status = bt_sleep_proc();
#if SYS_SLEEP_LEVEL > 2
        bb_run_loop();
#endif
        if(status == 1) {
            ret = sleep_timer();
            if(ret) {
                sys_cb.pwroff.timeout_ind = (bool)(ret == 1);
                break;
            }
        }

        if (wakeup_get_status()) {
            break;
        }

#if TKEY_EN
        if (tkey_is_pressed()) {
            break;
        }
#endif

#if (CHARGE_EN || QTEST_EN) && (UART0_PRINTF_SEL != PRINTF_VUSB)
        if (xcfg_cb.charge_en && CHARGE_INBOX()) {
            break;
        }
#endif // CHARGE_EN
        if(app_need_wakeup()){
            break;
        }

        if(le_popup_need_wakeup()){
            break;
        }
    }

    return true;
}

AT(.text.lowpwr.sleep)
static void sfunc_sleep(void)
{
    uint sysclk;

    if(bsp_res_is_playing()) {          //提示音还未播完不进休眠
        return ;
    }

#if VBAT_DETECT_EN
    if (vbat_get_lpwr_sta() != 0) {     //低电不进sniff mode
        return;
    }
#endif // VBAT_DETECT_EN

#if ABP_EN
    if (!abp_is_allow_sleep()) {
        return;
    }
#endif

    printf("%s\n", __func__);

    //休眠前处理
    bt_audio_bypass();
    app_enter_sleep();

#if MUSIC_UDISK_EN
    if (dev_is_online(DEV_UDISK)) {
        udisk_insert();                 //udisk需要重新enum
    }
#endif

    bt_enter_sleep();

    gui_off();
    led_off();
    rled_off();

    sfunc_sleep_dac_off(1);
    sys_set_tmr_enable(0, 0);
#if SYS_SLEEP_LEVEL > 2
    sys_set_tmr_tick(0);
#endif

    sys_clk_free_all();                 //释放其它模块sys_clk_req的系统时钟
    sysclk = sys_clk_get();
    sys_clk_set(SYS_24M);

#if CHARGE_EN
    charge_set_stop_time(3600);
    lock_code_charge();
#endif

    //进入CPU休眠
    lowpwr_sleep_proc(sfunc_sleep_proc_cb);

    //唤醒后恢复

#if CHARGE_EN
    charge_set_stop_time(18000);
    charge_status_disp_cb(sys_cb.charge_sta);          //update充灯状态
    unlock_code_charge();
#endif // CHARGE_EN

    sys_clk_set(sysclk);
#if SYS_SLEEP_LEVEL > 2
    sys_set_tmr_tick(1);
#endif
    sys_set_tmr_enable(1, 1);

    if (DAC_FAST_SETUP_EN) {
        bsp_loudspeaker_mute();
        delay_5ms(2);
    }

    sfunc_sleep_dac_off(0);

    bsp_change_volume(bsp_get_vol());

    bt_exit_sleep();
    bt_audio_enable();

#if ABP_EN
    if (sys_cb.abp_mode) {
        bsp_abp_set_mode(sys_cb.abp_mode);
    }
#endif
    gsensor_wakeup();

    app_exit_sleep();
    printf("sleep_exit\n");
}

AT(.text.lowpwr.sleep)
bool sleep_process(is_sleep_func is_sleep)
{
    if(app_need_wakeup()){
         reset_sleep_delay();
         reset_pwroff_delay();
         return false;
    }


#if CHARGE_EN && (UART0_PRINTF_SEL != PRINTF_VUSB)
    if (xcfg_cb.charge_en && CHARGE_INBOX()) {
#if (CHARGE_BOX_TYPE == CBOX_NOR)
        if((sys_cb.pwroff_delay == -1L)){
            en_auto_pwroff();
        }
#endif
        return false;
    }
#endif // CHARGE_EN

    if ( (*is_sleep)() ) {
        if ((!sys_cb.sleep_en) || (!port_2led_is_sleep_en())) {
            reset_sleep_delay();
            return false;
        }
        if(sys_cb.sleep_delay == 0) {
            sfunc_sleep();
            reset_sleep_delay();
            reset_pwroff_delay();
            return true;
        }
    } else {
        reset_sleep_delay();
    }
    return false;
}

