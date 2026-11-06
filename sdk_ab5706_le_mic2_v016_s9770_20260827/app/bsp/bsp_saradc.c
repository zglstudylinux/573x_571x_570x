#include "include.h"

adc_cb_t adc_cb AT(.buf.key.cb);

void adkey_mux_convert_done(void);

//ADC15每次需要选择一个模拟通路
AT(.com_text.saradc.ch15)
void saradc_ch15_analog_disable(u8 ana_ch)
{
    PWRCON0 &= ~BIT(22);                    //DI_EN_BGBUFF_SAR = 0
    RTCCON8 &= ~BIT(15);                    //RI_EN_VUSBDIV = 0
    PWRCON2 &= ~(BIT(27)|BIT(28));          //DI_OE_TSEN = 0
    RTCCON1 &= ~BIT(7);                     //RI_EN_VRTC_MEAS = 0
    AUANGCON1 &= ~BIT(13);                  //DI_MICAUD_MIC2SAR = 0
}
AT(.com_text.saradc.ch15)
void saradc_ch15_analog_enable(u8 ana_ch)
{
    saradc_ch15_analog_disable(ana_ch);

    if (ana_ch == ADCCH15_ANA_MIC) {
//        RTCCON1 |= BIT(7);                  //RI_EN_VRTC_MEAS = 1
        AUANGCON1 |= BIT(13);               //DI_MICAUD_MIC2SAR = 1
    } else if (ana_ch == ADCCH15_ANA_BG) {
        PWRCON0 |= BIT(22);                 //DI_EN_BGBUFF_SAR = 1
    } else if (ana_ch == ADCCH15_ANA_TS) {
        PWRCON2 |= BIT(27)|BIT(28);         //DI_OE_TSEN = 1
    } else if (ana_ch == ADCCH15_ANA_VUSB) {
        RTCCON8 |= BIT(15);                 //RI_EN_VUSBDIV = 1
    }
}

AT(.com_text.saradc.ch15)
u8 saradc_ch15_get_ana_ch(void)
{
    if(saradc_cb.adc15_status != ADCCH15_STA_DONE) {
        return ADCCH15_ANA_MAX;
    }
    return saradc_cb.adc15_ana_cur;
}

AT(.com_text.saradc.ch15)
u8 saradc_ch15_set_ana_ch(u8 ana_ch)
{
    u8 old = saradc_cb.adc15_ana_cur;

    if(ana_ch >= ADCCH15_ANA_MAX) {
        return old;
    }

    //等超过5ms，上一次的转换已经完成
    if(saradc_cb.adc15_status != ADCCH15_STA_DIS) {
        saradc_cb.adc15_status = ADCCH15_STA_DIS;
        delay_5ms(2);
    }

    //切换通道15的模拟通路
    saradc_ch15_analog_enable(ana_ch);

    //设置变量，等待转换完成
    saradc_cb.adc15_ana_cur = ana_ch;
    saradc_cb.adc15_status = ADCCH15_STA_W4_KICK;
    return old;
}

void saradc_ch15_clr_ana_ch(u8 ana_ch)
{
    saradc_cb.adc15_status = ADCCH15_STA_DIS;
    saradc_cb.adc15_ana_cur = ADCCH15_ANA_MAX;
    saradc_ch15_analog_disable(ana_ch);
}


//COM_CHAR str_adc[] = "%x(%x,%x) ";
AT(.com_text.saradc)
u32 get_mic_dc_volt(void)
{
    saradc_ch15_set_ana_ch(ADCCH15_ANA_MIC);
    saradc_start(ADKEY_MUX_LED_EN);
    while(!bsp_saradc_process());
    u32 dc_volt = 3100*adc_cb.vch15[ADCCH15_ANA_MIC]/1024;
    return dc_volt;
}

AT(.com_text.saradc.process)
bool bsp_saradc_process(void)
{
    if (!saradc_is_finish()) {
        return false;
    }

    //获取ADC转换结果
    saradc_get_result();
//    printk(str_adc, saradc_cb.channel, saradc_cb.value[ADCCH_WKO], saradc_cb.value[ADCCH_BGOP]);

#if ADKEY_MUX_SDCLK_EN
    adkey_mux_convert_done();
#endif

#if VBAT_DETECT_EN || TSEN_DETECT_EN
    {
        u8 ana_ch = saradc_ch15_get_ana_ch();
        if(ana_ch < ADCCH15_ANA_MAX) {
            adc_cb.vch15[ana_ch] = saradc_get_value10(15);;
        }
    }
#if VBAT_DETECT_EN
    sys_cb.vbat = vbat_get_voltage();
#endif

#endif

    //启动下一次ADC转换
    saradc_start(ADKEY_MUX_LED_EN);
    return true;
}

AT(.text.saradc.init)
void bsp_saradc_init(void)
{
    memset(&adc_cb, 0, sizeof(adc_cb));
    saradc_init();

#if ADKEY_MUX_LED_EN
    saradc_baud_set(0x09);
#endif // ADKEY_MUX_LED_EN

    //初次启动ADC转换
#if VBAT_DETECT_EN
    saradc_ch15_set_ana_ch(ADCCH15_ANA_BG);
    saradc_start(ADKEY_MUX_LED_EN);
    while(!bsp_saradc_process());               //获取一次值，再kick一次
    vbat_voltage_init();
#else
    saradc_start(ADKEY_MUX_LED_EN);
#endif
}

AT(.text.saradc.init)
uint16_t bsp_saradc_exit(void)
{
//    saradc_ch15_clr_ana_ch(ADCCH15_ANA_BG);
    return saradc_exit();
}

AT(.text.saradc.init)
void bsp_saradc_restart(void)
{
    saradc_init();

#if ADKEY_MUX_LED_EN
    saradc_baud_set(0x09);
#endif

#if VBAT_DETECT_EN
//    saradc_ch15_set_ana_ch(ADCCH15_ANA_BG);
    saradc_start(ADKEY_MUX_LED_EN);
    while(!bsp_saradc_process());               //获取一次值，再kick一次
#else
    saradc_start(ADKEY_MUX_LED_EN);
#endif
}
