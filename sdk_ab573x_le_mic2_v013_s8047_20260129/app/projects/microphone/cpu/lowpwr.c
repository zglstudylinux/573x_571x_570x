#include "include.h"
#include "func.h"


AT(.text.pwroff.save)
static void power_save_enter(void)
{
    LOUDSPEAKER_MUTE_DIS();
#if CHARGE_EN
    if (xcfg_cb.charge_en) {
        charge_off();
    }
#endif // CHARGE_EN
    pmu_set_mode(getcfg_pmu_mode() & BIT(3));
    //usb_disable();
    GPIOBDE = BIT(5);                       //pwrkey
    GPIOEDE = 0;
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
void lowpwr_lowbat_mode(void)
{
    lock_code_pwrsave();

    power_save_enter();

    WDT_DIS();
    RTC_WDT_DIS();
    asm("nop");asm("nop");asm("nop");

    WPTCON &= ~BIT(22);                 //cpu_dq_tout_en=0
    LPMCON |= BIT(0);                   //Sleep mode enable
    asm("nop");asm("nop");asm("nop");
    while(1);
}
#if (CHARGE_BOX_TYPE == CBOX_NOR)
///充满关机前等待仓休眠5V掉电
AT(.text.pwroff)
static u8 pwrdown_w4_vusb_offline(void)
{
    u32 timeout = 12000, off_cnt = 0;
    if (xcfg_cb.ch_box_type_sel != 1) {
        while (timeout--) {
            WDT_CLR();
            delay_us(330);                  //5ms
            if (!CHARGE_DC_IN()) {
                off_cnt++;
            } else {
                off_cnt = 0;
            }
            if (off_cnt > 5) {
                return 1;                   //VUSB已掉线，打开VUSB唤醒
            }
        }
    }
    return 0;
}
#endif
//软开关方案，POWER键/低电时，进入关机状态
AT(.text.pwroff.pwrdown)
void lowpwr_pwroff_mode(u8 vusb_wakeup_en)
{
    lock_code_pwrsave();

#if TKEY_EN
    u32 tkey_wakeup_en = sys_cb.tkey_pwrdwn_en;
#if QTEST_EN || TBOX_TEST_EN
    if (qtest_cb.pdn_boat_flag) {
        tkey_wakeup_en = 0;
    }
#endif
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
    power_save_enter();
    if (!vusb_wakeup_en) {
        RTCCON8 = (RTCCON8 & ~BIT(6)) | BIT(1);     //disable charger function
#if (CHARGE_BOX_TYPE == CBOX_NOR)
        vusb_wakeup_en = pwrdown_w4_vusb_offline();
        if (xcfg_cb.ch_box_type_sel == 3) {         //5V完全掉电的仓
            RTCCON3 &= ~BIT(12);                    //RTCCON3[12], INBOX Wakeup disable
        }
#elif (CHARGE_BOX_TYPE == CBOX_NWB)
        RTCCON3 |= BIT(12);                         //RTCCON3[12], INBOX Wakeup enable
        if (CHARGE_DC_IN()) {
            RTCCON8 |= BIT(19);                     //RI_LEAK_USB = 1
            RTCCON8 = (RTCCON8 & ~(3 << 29)) | (0 << 29);
            RTCCON13 &= ~BIT(29);                   //inbox low level wakeup
        } else {
            RTCCON13 |= BIT(29);                    //inbox high level wakeup
        }
#endif
    }

    WDT_DIS();
    vusb4s_reset_dis();                             //VUSB 4s reset disable
    vusb4s_reset_recover_clr();
    RTCCON11 |= BIT(4);                             //VUSB 4s reset select VUSB pull out

    RTCCON11 = (RTCCON11 & ~0x03) | BIT(2);         //WK PIN filter select 8ms
    uint rtccon3 = RTCCON3 & ~BIT(11);
    uint rtccon13 = RTCCON13 & ~(0x3f << 18);       //WK pin5~0 wakeup disable
#if CHARGE_EN
    if ((xcfg_cb.charge_en) && (vusb_wakeup_en)) {
        rtccon3 |= BIT(11);                         //VUSB wakeup enable
        RTCCON3 |= BIT(11);                         //VUSB wakeup enable
        vusb_set_pout_filter();
        delay_us(100);
        RTCCON10 = BIT(3);
    }
#endif
    RTCCON3 = rtccon3 & ~(BIT(10) | BIT(14));       //关WK PIN，再打开，以清除Pending
    PWRCON1 &= ~(0x1F<<14);                         //disable Flash Power Gate
    PWRCON1 |= BIT(18);                             //pdown flash power gate
#if TKEY_EN
    if ((RTCCON3 & BIT(12)) || sys_cb.lowbat_flag) {//仓内或低电关机？
        tkey_wakeup_en = 0;
    }

    if (tkey_wakeup_en) {
        RTCCON1 |= BIT(9);                          //tk digital voltage enable
        RTCCON3 |= BIT(14);                         //Touch key long press wakeup
    } else {
        //保持电源，保证tkey通道能顺利关闭
        RTCCON0 |= BIT(5);                          //tk rst disable
        TKACON0 = 0;
        TKACON1 = 0;
        RTCCON0 &= ~BIT(4);                         //tk interface disable
#if !OFFLINE_LOG_EN
        RTCCON0 &= ~BIT(0);                         //RC2M_RTC Disable
#endif
        RTCCON0 &= ~BIT(5);                         //tk rst enable
        RTCCON1 &= ~BIT(9);                         //tk digital voltage disable

        RTCCON6 &= ~(BIT(11)|BIT(15));              //RC2M_SNIFF Disable
        RTCCON0 &= ~(BIT(17)|BIT(18));
    }
#else
#if !OFFLINE_LOG_EN
    RTCCON0 &= ~BIT(0);                             //RC2M_RTC Disable
#endif
#endif

    RTCCON1 |= BIT(11);                             //VRTC control by hw
    RTCCON0 &= ~(BIT(22) | BIT(23));

    rtccon3 = RTCCON3 & ~0x17;                      //Disable VDDCORE VDDIO VDDBUCK, VDDXOEN
    rtccon3 |= BIT(6);                              //Core power down enable, VDDCORE short disable
    rtccon3 &= ~BIT(22);                            //LVCORE_DIS
    RTCCON3 &= ~BIT(22);                            //LVCORE_DIS, lvcore掉电
    if (VERSIONID > 0) {
        rtccon3 |= BIT(7);                          //RI_EN_VDDIO_AON   RTC 2.9V LDO enable
    } else {
        rtccon3 &= ~BIT(7);                         //RI_EN_VDDIO_AON   RTC 2.9V LDO enable
    }
    rtccon3 |= BIT(4)|BIT(19);                      //pd_core
#if PWRKEY_EN
    rtccon3 |= BIT(10);                             //WK pin wake up enable
    rtccon13 |= BIT(0) | BIT(6) | BIT(18);          //wk pin0 wakeup, input, pullup10k enable
#endif // PWRKEY_EN
#if QTEST_EN || TBOX_TEST_EN
    if (qtest_cb.pdn_boat_flag) {                                         //若船运模式关机，只保留5V唤醒
        rtccon3 &= ~(BIT(14) | BIT(12) | BIT(10) | BIT(9) | BIT(8));      //touch key long press, inbox, wk pin, RTC one second, RTC alarm wakeup disable
        rtccon3 |= BIT(11);                                               //VUSB wakeup enable
    }
#endif
    RTC_WDT_DIS();
    LPMCON |= BIT(3);                               //避免关机过程被残留的pending误唤醒，引起RTC寄存器概率性出错
    RTCCON &= ~(3 << 1);                            //rtc选择sys_clk div2, 避免关机时rtc配置来不及生效，导致关机功耗偏大1.5uA
    RTCCON |= BIT(5);                               //PowerDown Reset，如果有Pending，则马上Reset
    RTCCON13 = rtccon13;
    RTCCON3 = rtccon3;
    WPTCON &= ~BIT(22);                             //cpu_dq_tout_en=0
    LPMCON |= BIT(0);
    LPMCON |= BIT(1);                               //idle mode
    asm("nop");asm("nop");asm("nop");
    while (1);
}

void lowpwr_sleep_proc(sleep_proc_cb_t sleep_proc_cb)
{
    uint32_t usbcon0, usbcon1;
    u16 gpiode_save[5];
    u16 gpiode_en[5];
    u16 adc_ch;

    adc_ch = bsp_saradc_exit();         //close saradc及相关通路模拟
    saradc_set_channel(BIT(ADCCH_VBAT) | BIT(ADCCH_BGOP));

    usbcon0 = USBCON0;                  //需要先关中断再保存
    usbcon1 = USBCON1;
    USBCON0 = BIT(5);
    USBCON1 = 0;
    {
    #if ANC_EN
        if (!sys_cb.anc_start)
    #endif
        {
            dac_clk_source_sel(2);              //dac clk select xosc_clk
            adpll_disable();
        }
    }
    //io analog input
    gpiode_save[1] = GPIOBDE;
    gpiode_save[2] = GPIOEDE;
    gpiode_save[4] = GPIOGDE;

    memset(gpiode_en, 0x00, sizeof(gpiode_en));
#if CHARGE_EN
    if (CHARGE_DC_IN() && (xcfg_cb.rled_io_sel != IO_NONE)) {
        u8 gp = (xcfg_cb.rled_io_sel-1) / 8;
        u8 pin = (xcfg_cb.rled_io_sel-1) % 8;

        gpiode_en[gp] |= BIT(pin);              //保留充电指示灯
    }
#endif

    GPIOBDE = gpiode_en[1] | BIT(5);            //保留PB5(wko)的配置
    GPIOEDE = gpiode_en[2];
    GPIOGDE = 0x3F; //MCP FLASH

#if USER_INEAR_DET_OPT
    INEAR_OPT_PORT_INIT();
#endif

    wakeup_disable();
    sleep_wakeup_config();

    while(1) {
        WDT_CLR();
        if(sleep_proc_cb()) {
            break;
        }
    }

    GPIOBDE = gpiode_save[1];
    GPIOEDE = gpiode_save[2];
    GPIOGDE = gpiode_save[4];

    printf("wakeup\n");
    wakeup_disable();

    USBCON0 = usbcon0;
    USBCON1 = usbcon1;

#if ANC_EN
    if (!sys_cb.anc_start)
#endif
    {
        adpll_init(DAC_OUT_SPR);                //enable adpll
        dac_clk_source_sel(1);                  //dac clk select adda_clk48
    }

    saradc_set_channel(adc_ch);
    bsp_saradc_init();
}

//蓝牙休眠时，唤醒IO配置
void sleep_wakeup_config(void)
{
#if ADKEY_EN
    wakeup_gpio_config(get_adc_gpio_num(ADKEY_CH), 0, 0);           //配置ADKEY IO下降沿唤醒。
#endif // ADKEY_EN

#if ADKEY2_EN
    wakeup_gpio_config(get_adc_gpio_num(ADKEY2_CH), 0, 0);          //配置ADKEY1 IO下降沿唤醒。
#endif // ADKEY2_EN

#if ADKEY_MUX_SDCLK_EN
    wakeup_gpio_config(get_adc_gpio_num(SDCLK_AD_CH), 0 ,0);        //IO下降沿唤醒。
#endif // ADKEY_MUX_SDCLK_EN

#if IOKEY_EN && IOKEY_GPIO_SEL_EN
    //不用工具配置IOKEY时，根据实际使用的IOKEY IO进行修改。
    wakeup_gpio_config(IOKEY_GPIO_SEL0, 0, 1);       //配置IO下降沿唤醒。
    wakeup_gpio_config(IOKEY_GPIO_SEL1, 0, 1);
#endif // IOKEY_EN

#if SC7A20_EN
	wakeup_wko_config(); /*配置PB5作为GPIO唤醒功能*/
	//printf("%s config pb5 wakeup\n", __func__);
#endif

#if PWRKEY_EN
    if ((!PWRKEY_2_HW_PWRON) && !bsp_tkey_wakeup_en()) {
        wakeup_wko_config();
    }
#endif // PWRKEY_EN

}
