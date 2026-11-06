#ifndef _BSP_SARADC_H
#define _BSP_SARADC_H


enum {
    ADCCH15_ANA_BG        = 0,
    ADCCH15_ANA_TS,
    ADCCH15_ANA_VUSB,
    ADCCH15_ANA_MIC,
    ADCCH15_ANA_MAX,
};

typedef struct {
    union {
        struct {
            u16 vbg;        //ADCCH15_ANA_BG
            u16 tsen;       //ADCCH15_ANA_TS
            u16 vusb;       //ADCCH15_ANA_VUSB
            u16 vmic;       //ADCCH15_ANA_MIC
        };
        u16 vch15[ADCCH15_ANA_MAX];
    };
    u16 vbat2_coef;     //vbat校准参数
    u16 vbg_volt;       //vbg校准参数
    u16 vbat_val;       //vbat电压
    u32 vbat_total;     //vbat均值变量
} adc_cb_t;
extern adc_cb_t adc_cb;

u8 saradc_ch15_get_ana_ch(void);                //ADC15获取当前模拟通道
u8 saradc_ch15_set_ana_ch(u8 ana_ch);           //ADC15设置模拟通路，返回当前模拟通道
void saradc_ch15_clr_ana_ch(u8 ana_ch);         //ADC15清除模拟通路

void bsp_saradc_init(void);
bool bsp_saradc_process(void);
void bsp_saradc_restart(void);
uint16_t bsp_saradc_exit(void);

#define bsp_saradc_set_channel(n)       saradc_set_channel(n)

#endif // _BSP_SARADC_H
