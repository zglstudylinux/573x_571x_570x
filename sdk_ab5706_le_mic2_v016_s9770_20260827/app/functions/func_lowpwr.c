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
    u32 gpiogde = GPIOGDE;
    GPIOGDE = BIT(2) | BIT(4);                  //SPICS, SPICLK

    sys_sleep_proc(lpclk_type);                //enter sleep

    //唤醒后，恢复模块功能
    GPIOGDE = gpiogde;
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
        bsp_saradc_restart();
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
static void sfunc_sleep(void)
{
    uint32_t status, ret, sysclk;
    uint32_t usbcon0, usbcon1;
    u16 gpiode_save[5];
    u16 gpiode_en[5];
    u16 adc_ch;

#if VBAT_DETECT_EN
    if (vbat_get_lpwr_sta() != 0) {     //低电不进sniff mode
        return;
    }
#endif

    printf("%s\n", __func__);
    bt_audio_bypass();
    app_enter_sleep();

    bt_enter_sleep();

    gui_off();
    led_off();
    rled_off();

#if DAC_DNR_EN
    u8 sta = dac_dnr_get_sta();
    dac_dnr_set_sta(0);
#endif

    sfunc_sleep_dac_off(1);
    sys_set_tmr_enable(0, 0);
#if SYS_SLEEP_LEVEL > 2
    sys_set_tmr_tick(0);
#endif

    sys_clk_free_all();                 //释放其它模块sys_clk_req的系统时钟
    sysclk = sys_clk_get();
    sys_clk_set(SYS_24M);

    adc_ch = bsp_saradc_exit();         //close saradc及相关通路模拟
    saradc_set_channel(BIT(ADCCH_VBAT) | BIT(ADCCH_BGOP));

#if CHARGE_EN
    charge_set_stop_time(3600);
    lock_code_charge();
#endif

    usbcon0 = USBCON0;                  //需要先关中断再保存
    usbcon1 = USBCON1;
    USBCON0 = BIT(5);
    USBCON1 = 0;

    {
        dac_clk_source_sel(2);              //dac clk select xosc26m_clk
        adpll_disable();
    }

    //io analog input
    gpiode_save[0] = GPIOADE;
    gpiode_save[1] = GPIOBDE;
    gpiode_save[2] = GPIOEDE;
    gpiode_save[3] = GPIOFDE;
    gpiode_save[4] = GPIOGDE;

    memset(gpiode_en, 0x00, sizeof(gpiode_en));
#if CHARGE_EN
    if (CHARGE_DC_IN() && (xcfg_cb.rled_io_sel != IO_NONE)) {
        u8 gp = (xcfg_cb.rled_io_sel-1) / 8;
        u8 pin = (xcfg_cb.rled_io_sel-1) % 8;

        gpiode_en[gp] |= BIT(pin);              //保留充电指示灯
    }
#endif

    GPIOADE = gpiode_en[0];
    GPIOBDE = gpiode_en[1] | BIT(5);            //保留PB5(wko)的配置
    GPIOEDE = gpiode_en[2];
    GPIOFDE = gpiode_en[3];
    GPIOGDE = 0x3F; //MCP FLASH

    wakeup_disable();
    sleep_wakeup_config();

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

#if WIRELESS_EN
        if(!wireless_is_sleep()) {
            break;
        }
#endif

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
    GPIOADE = gpiode_save[0];
    GPIOBDE = gpiode_save[1];
    GPIOEDE = gpiode_save[2];
    GPIOFDE = gpiode_save[3];
    GPIOGDE = gpiode_save[4];

    printf("wakeup\n");
    wakeup_disable();

    USBCON0 = usbcon0;
    USBCON1 = usbcon1;

    {
        adpll_init(DAC_OUT_SPR);                //enable adpll
        dac_clk_source_sel(1);                  //dac clk select adda_clk48
    }

    saradc_set_channel(adc_ch);
    bsp_saradc_restart();

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

#if DAC_DNR_EN
    dac_dnr_set_sta(sta);
#endif

    bt_exit_sleep();
    bt_audio_enable();
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
        return false;
    }
#endif // CHARGE_EN

    if ( (*is_sleep)()
#if WIRELESS_EN
        && wireless_is_sleep()
#endif
        ) {
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

AT(.text.pwroff.save)
void sfunc_power_save_enter(void)
{
    LOUDSPEAKER_MUTE_DIS();
#if CHARGE_EN
    if (xcfg_cb.charge_en) {
        charge_off();
    }
#endif // CHARGE_EN
    pmu_set_mode(0);
    //usb_disable();
    GPIOADE = 0;
    GPIOBDE = BIT(5);                       //pwrkey
    GPIOEDE = 0;
    GPIOFDE = 0;
    GPIOGDE = 0x3F;                         //MCP FLASH

    PICCONCLR = BIT(0);                     //Globle IE disable
    CLKCON0 |= BIT(0);                      //enable RC
    CLKCON0 = (CLKCON0 & ~(0x3 << 2));      //system clk select RC
    PLL0CON0 &= ~BIT(18);                   //pll10 sdm enable disable
    PLL0CON0 &= ~BIT(6);                    //pll0 analog diable
    PLL0CON0 &= ~BIT(12);                   //pll0 ldo disable

    XOSCCON &= ~(0xf<<24);                  //X26 output DIS
    XOSCCON &= ~BIT(10);                    //X26 DIS
}

//硬开关方案，低电时，进入省电状态
AT(.text.pwroff.lowbat)
void sfunc_lowbat(void)
{
    lock_code_pwrsave();
    sfunc_power_save_enter();
    WDT_DIS();
    RTC_WDT_DIS();
    asm("nop");asm("nop");asm("nop");

    WPTCON &= ~BIT(22);                 //cpu_dq_tout_en=0
    LPMCON |= BIT(0);                   //Sleep mode enable
    asm("nop");asm("nop");asm("nop");
    while(1);
}

//软开关方案，POWER键/低电时，进入关机状态
AT(.text.pwroff.pwrdown)
void sfunc_pwrdown_do(u8 vusb_wakeup_en)
{
#if TKEY_EN
    u32 tkey_wakeup_en = sys_cb.tkey_pwrdwn_en;
#endif

#if RTC_CLOCK_EN
    rtc_set_alarm_wakeup(RTC_CALIBRATION_CYCLE);
#endif

    printf("pwr_down: %d\n", vusb_wakeup_en);
    vusb_reset_clr();
    RTCCON3 &= ~BIT(8);                             //rtc alarm wakeup disable
    RTCCON8 &= ~BIT(15);                            //RI_EN_VUSBDIV = 0
    RTCCON9 = 0xffff;                               //Clr pending
#if TKEY_LOWPWR_WAKEUP_DIS                     //电池无保护板且有内置触摸开关机功能方案，需要打开此宏
    if (sys_cb.vbat < 3050) {
        tkey_wakeup_en = 0;                         //低电关机, 关掉触模唤醒
    }
    if (tkey_wakeup_en) {
        RTCALM = RTCCNT + 300;                      //定时5分钟唤醒检查电池电量
        RTCCON3 |= BIT(8);
    }
#endif
    sfunc_power_save_enter();
    if (!vusb_wakeup_en) {
        RTCCON8 = (RTCCON8 & ~BIT(6)) | BIT(1);     //disable charger function
    }

    WDT_DIS();
    vusb4s_reset_dis();                             //VUSB 4s reset disable
    vusb4s_reset_recover_clr();
    RTCCON11 |= BIT(4);                             //VUSB 4s reset select VUSB pull out

    RTCCON11 = (RTCCON11 & ~0x03) | BIT(2);         //WK PIN filter select 8ms
    uint rtccon3 = RTCCON3 & ~BIT(11);
    //uint rtccon13 = RTCCON13 & ~(0x3f << 18);       //WK pin5~0 wakeup disable
    uint rtccon4 = RTCCON4 & ~(0x7 << 22);
#if CHARGE_EN
    if ((xcfg_cb.charge_en) && (vusb_wakeup_en)) {
        rtccon3 |= BIT(11);                         //VUSB wakeup enable
        RTCCON3 |= BIT(11);                         //VUSB wakeup enable
        RTCCON11 |= BIT(6);                         //VUSB pull out filter VUSB拔出滤波
        delay_us(100);
        RTCCON10 = BIT(3);
    }
#endif
    RTCCON3 = rtccon3 & ~(BIT(10) | BIT(14));       //关WK PIN，再打开，以清除Pending

//    PWRCON1 &= ~(0x1F<<14);                         //disable Flash Power Gate
//    PWRCON1 |= BIT(18);                             //pdown flash power gate
#if TKEY_EN
    if ((RTCCON3 & BIT(12)) || sys_cb.lowbat_flag) {//仓内或低电关机？
        tkey_wakeup_en = 0;
    }

    if (tkey_wakeup_en) {
        RTCCON1 |= BIT(9);                          //tk digital voltage enable
        RTCCON3 |= BIT(14);                         //Touch key long press wakeup
        rtccon4 |= 0x4 << 22;                       //VRTC 0.96V
    } else {
        //保持电源，保证tkey通道能顺利关闭
        RTCCON0 |= BIT(5);                          //tk rst disable
        TKACON0 = 0;
        TKACON1 = 0;
        RTCCON0 &= ~BIT(4);                         //tk interface disable
        RTCCON0 &= ~BIT(0);                         //RC2M_RTC Disable
        RTCCON0 &= ~BIT(5);                         //tk rst enable
        RTCCON1 &= ~BIT(9);                         //tk digital voltage disable
        rtccon4 |= 0x2 << 22;                       //VRTC 0.82V
    }
#else
    RTCCON0 &= ~BIT(0);                             //RC2M_RTC Disable
    rtccon4 |= 0x2 << 22;                           //VRTC 0.82V
#endif
    RTCCON4 = rtccon4;

#if RTC_CLOCK_EN
    RTCCON3 |= BIT(8);                               //rtc alarm wakeup Enable
//    RTCCON0 |= BIT(0);                               //RC2M_RTC Enable
#endif

    RTCCON0 &= ~(BIT(22) | BIT(23));

    rtccon3 = RTCCON3 & ~0x17;                      //Disable VDDCORE VDDIO VDDBUCK, VDDXOEN
    rtccon3 |= BIT(6);                              //Core power down enable, VDDCORE short disable
    rtccon3 &= ~BIT(22);                            //LVCORE_DIS
    rtccon3 &= ~BIT(7);                             //RI_EN_VDDIO_AON
#if PWRKEY_EN
    RTCCON1 |= (BIT(0) | BIT(2) | BIT(4));          //wk pin0 wakeup, input, pullup10k enable,下降沿唤醒
    RTCCON1 &= ~(BIT(1) | BIT(8));
//    RTCCON1 &= ~(BIT(2) | BIT(4) | BIT(8));       //wk pin0 wakeup, input, pulldown10k enable,上升沿唤醒
//    RTCCON1 |= BIT(0) | BIT(1) | BIT(6);
    RTCCON11 |= BIT(2);
#endif // PWRKEY_EN
    RTC_WDT_DIS();
    LPMCON |= BIT(3);                               //避免关机过程被残留的pending误唤醒，引起RTC寄存器概率性出错
    RTCCON &= ~(3 << 1);                            //rtc选择sys_clk div2, 避免关机时rtc配置来不及生效，导致关机功耗偏大1.5uA
    RTCCON |= BIT(5);                               //PowerDown Reset，如果有Pending，则马上Reset
    //RTCCON13 = rtccon13;
#if PWRKEY_EN
    rtccon3 = rtccon3 | BIT(10);                    //WK pin wake up enable
#endif
    RTCCON3 = rtccon3;
    WPTCON &= ~BIT(22);                             //cpu_dq_tout_en=0
    LPMCON |= BIT(0);
    asm("nop");asm("nop");asm("nop");
    while (1);
}


void sfunc_pwrdown(u8 vusb_wakeup_en)
{
    lock_code_pwrsave();
    sfunc_pwrdown_do(vusb_wakeup_en);
}

AT(.text.lowpwr.pwroff)
void func_pwroff(int pwroff_tone_en)
{
    printf("%s: %d\n", __func__, pwroff_tone_en);

    bsp_res_cleanup();

    gsensor_lowpwr();
    led_power_down();

#if RTC_CLOCK_EN
    rtc_pwroff_enter();
#endif
#if WARNING_POWER_OFF
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

#if LEDC_HW_EN
    bsp_ledc_exit();                           //关机前清除灯效
#endif
#if LOUDSPEAKER_MUTE_EN
    bsp_loudspeaker_mute();                    //关机前mute一下功放
#endif // LOUDSPEAKER_MUTE_EN


#if SOFT_POWER_ON_OFF
#if !PWRKEY_2_HW_PWRON
    {
        u8  dcin_cnt = 0;
        while (PWRKEY_IS_PRESS() || TKEY_IS_PRESS()) {      //等待PWRKWY松开
            if (CHARGE_DC_IN()) {
                dcin_cnt++;
                if (dcin_cnt > 3) {
                    sw_reset_kick(SW_RST_DC_IN);            //直接复位进入充电
                }
            } else {
                dcin_cnt = 0;
            }
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
        RTCCON1 |= BIT(6);            //WK PIN0 High level wakeup
    }
    sfunc_pwrdown(1);
#else // SOFT_POWER_ON_OFF
    dac_power_off();                    //dac power down
    bsp_saradc_exit();                  //close saradc及相关通路模拟

    sfunc_lowbat();                     //低电关机进入Sleep Mode
#endif // SOFT_POWER_ON_OFF
}
