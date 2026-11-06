#include "include.h"
#include "mic_dnr.h"
#include "api_alg.h"
/*
 * 文件名称: mic_dnr.c
 * 功能描述: 本文件为软件mic_dnr mic能量检测处理模块
    当前模块按点计算，配置采样率一项固化48k
 ****************************************************************************************
    code : 218 bytes
    buf  : 12 bytes
    time : 2us
 */

#if MIC_DNR_EN

void mic_dnr_init(u8 v_cnt, u16 v_pow, u8 s_cnt, u16 s_pow);

static mic_dnrpwr_cfg_t mic_dnr_cfg AT(.buf.mic_dnr);

AT(.text.mic_dnr_proc)
void mic_dnr_process(void)
{
    static u32 ticks = 0;
    if (tick_check_expire(ticks,10)) {
        ticks = tick_get();

        u8 ret = mic_noise_detect(mic_dnr_cfg.maxpow_mic);
        if (ret == 1) {
            if (mic_dnr_cfg.mic_dnr_sta ) {
                mic_dnr_cfg.mic_dnr_sta = 0;
                printf("\n----->mic up\n");
                dac1_fade_in();
                dac1_vol_set(DIG_N0DB);
            }
        } else if (ret == 2) {
            if (mic_dnr_cfg.mic_dnr_sta == 0) {
                mic_dnr_cfg.mic_dnr_sta = 1;
                printf("\n----->mic mute\n");
                dac1_fade_in();
                dac1_vol_set(DIG_N60DB);
            }
        }
    }
}

AT(.text.mic_dnr_proc)
void mic_dnr_audio_input(u8 *ptr, u32 samples, int ch_mode, void *params)
{
    s16 *rptr = (s16 *)ptr;

    if(!mic_dnr_cfg.mute){
        mic_dnr_cfg.maxpow_mic = dnr_buf_maxpow(ptr,samples);
    }

    if (mic_dnr_cfg.callback) {
        mic_dnr_cfg.callback((u8 *)rptr, samples, ch_mode, params);
    }
}

AT(.text.mic_dnr_set.callback)
void mic_dnr_audio_output_callback_set(audio_callback_t callback)
{
    mic_dnr_cfg.callback = callback;
}

AT(.text.mic_dnr_init)
void mic_dnr_audio_init(u8 sample_rate, u16 samples, u8 channel)
{
    //管理模块结构体赋值
    memset(&mic_dnr_cfg, 0, sizeof(mic_dnrpwr_cfg_t));
    mic_dnr_cfg.sample_rate = sample_rate;
    mic_dnr_cfg.samples     = samples;

//    mic_dnr_init(2, 500, 100, 1000);        //10ms检测一次 //连续超过2次大于500就认为有声，连续超过100次低于 1000 就认为无声
    mic_dnr_init(1, 1, 1, 1);
}

AT(.text.mic_dnr_set.param)
void mic_dnr_audio_set_param(u16 room_decay_set, u16 room_dry, u16 room_wet)
{

}

AT(.text.mic_dnr_set.mute)
void mic_dnr_audio_mute_set(uint8_t mute)
{
    mic_dnr_cfg.mute = mute;
}

AT(.text.mic_dnr_exit)
void mic_dnr_audio_exit(void)
{

}
#endif
