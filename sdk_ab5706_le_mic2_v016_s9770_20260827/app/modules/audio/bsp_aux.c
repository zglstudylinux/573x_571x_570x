#include "include.h"

#if FUNC_AUX_EN

AT(.buf.aux.cb)
static u8 aux_sdadc_buf[256 * 2];

/****************************** AUX NOTICE **************************************
///无模拟直通，通路保持audio->adc->dac，配置固定：
* AUXL0_ADC0：       输入：PE6              输出：DACL\DACL&R
* AUXR0_ADC1：       输入：PE7              输出：DACR\DACL&R
* AUXL1_ADC0：       输入：PA6              输出：DACL\DACL&R
* AUXR1_ADC1：       输入：PA7              输出：DACR\DACL&R

///模拟增益范围：
* ANL_GAIN:          AUX_P12DB~AUX_N12DB，-12dB~+12dB 共8级
 *****************************************************************************************/
AT(.text.bsp.aux)
void bsp_aux_start(void)
{
 //   printf("%s\n", __func__);
    dac_fade_out();
    dac_fade_wait();                                            //等待淡出完成
//    u8 path = f_aux.aux2adc & 0x3f;
    u8 aux2adc = (f_aux.aux2adc & BIT(7)) >> 7;
    memset(aux_sdadc_buf, 0, sizeof(aux_sdadc_buf));
    if(aux2adc){
        audio_path_init(AUDIO_PATH_AUX, aux_sdadc_buf);
        audio_path_start(AUDIO_PATH_AUX);
    }else{
        analog_aux2pa_start(bsp_aux_ch_getcfg(),xcfg_cb.aux_anl_gain);
    }
    dac_fade_in();
}

AT(.text.bsp.aux)
void bsp_aux_stop(void)
{
 //   printf("%s\n", __func__);
    dac_fade_out();
    dac_fade_wait();
//    u8 path = f_aux.aux2adc & 0x3f;
    u8 aux2adc = (f_aux.aux2adc & BIT(7)) >> 7;
    if(aux2adc){
        audio_path_exit(AUDIO_PATH_AUX);
    }else{
        analog_aux2pa_exit(bsp_aux_ch_getcfg());
    }
    dac_aubuf_clr();
}

AT(.text.bsp.aux)
void bsp_aux_mute(u8 ch)    //BIT(0)-AUXL; BIT(1)-AUXR
{
    aux_channel_mute(ch);
}

AT(.text.bsp.aux)
void bsp_aux_unmute(u8 ch)  //BIT(0)-AUXL; BIT(1)-AUXR
{
    aux_channel_unmute(ch);
}

// level: 0-AUX_N12DB; 1-AUX_N9DB; 2-AUX_N6DB; 3-AUX_N3DB; 4-AUX_p0DB; 5-AUX_P6DB; 6-AUX_P9DB; 7-AUX_P12DB;
AT(.text.bsp.aux)
void bsp_aux_set_vol(u8 level, u8 ch)  //BIT(0)-AUXL; BIT(1)-AUXR
{
    set_aux_analog_vol(level, ch);
}

#endif



