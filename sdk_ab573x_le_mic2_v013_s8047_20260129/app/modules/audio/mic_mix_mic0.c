#include "include.h"

#if WIRELESS_DUAL_MIC_EN
/*
 * 文件名称: mic_mix_aux.c
 * 功能描述: 本文件用于配置双通道（MIC1 + MIC0）立体声数据转单声道数据发送

 ****************************************************************************************
    code :
    buf  :
    time :
 */

static sdadc_cfg_t mic_mix_cfg ;

AT(.text.dual_mic)
void mic_stereo_init(u8 sample_rate, u16 samples, u8 channel)
{
    memset(&mic_mix_cfg, 0, sizeof(sdadc_cfg_t));
}

AT(.com_text.wireless_mic)
void mic_stereo_audio_input(u8 *ptr, u32 samples, u32 params)
{
    s16 *mic_ptr = (s16 *)ptr;
    params = 0x101;
    for(int i = 0; i < samples; i++) {
        mic_ptr[i] = s_clip16(mic_ptr[2 * i] +  mic_ptr[2 * i + 1]);               //双声道保留单声道（暂时），此时过来的ptr是个双声道数据，L：mic1，R：mic0，这里混合后设置params=0x101保证后续链路依然是按单声道处理
    }

    if (mic_mix_cfg.callback) {
        mic_mix_cfg.callback((u8 *)ptr, samples, params);
    }
}

AT(.text.dual_mic)
void mic_stereo_audio_output_callback_set(audio_callback_t callback)
{
    mic_mix_cfg.callback        = (pcm_callback_t)callback;                    //adc 中断回调用函数
}

#endif //WIRELESS_DUAL_MIC_EN
