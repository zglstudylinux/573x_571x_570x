#include "include.h"

adc_cb_t adc_cb AT(.buf.key.cb);

void adkey_mux_convert_done(void);

u32 mic2sar_get_voltage(void)
{
     return (u32)adc_cb.svrc_val * VBG_VOLTAGE / adc_cb.vbg;
}

AT(.com_text.saradc.process)
bool bsp_saradc_process(void)
{
    if (!saradc_is_finish()) {
        return false;
    }

    //获取ADC转换结果
    saradc_get_result();

#if ADKEY_MUX_SDCLK_EN
    adkey_mux_convert_done();
#endif
#if VBAT_DETECT_EN
//    if (saradc_adc15_is_vrtc()) {
//        adc_cb.vrtc = saradc_get_value10(ADCCH_VRTC);
//    }
    adc_cb.vbg = saradc_get_value10(ADCCH_BGOP);
    sys_cb.vbat = vbat_get_voltage();
#endif // VBAT_DETECT_EN

    if (saradc_adc15_is_svrc()) {
        adc_cb.svrc_val = saradc_get_value10(ADCCH_SVRC);
    }
    saradc_adc15_analog_next();

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
    //saradc_adc15_ana_set_channel(ADCCH15_ANA_VRTC);
    saradc_adc15_analog_next();
    saradc_start(ADKEY_MUX_LED_EN);
    while(!bsp_saradc_process());               //获取一次初值，再kick一次
    vbat_voltage_init();
#else
    saradc_start(ADKEY_MUX_LED_EN);
#endif
}

AT(.text.saradc.init)
uint16_t bsp_saradc_exit(void)
{
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
    saradc_start(ADKEY_MUX_LED_EN);
    while(!bsp_saradc_process());               //获取一次值，再kick一次
#else
    saradc_start(ADKEY_MUX_LED_EN);
#endif
}
