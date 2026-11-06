#include "include.h"

static sdadc_cfg_t mic_cfg = {CH_MIC4,  SPR_48000, 3,  1, 1,  ADC2DAC_EN,  120, 5, NULL};
volatile bool sdadc_w4_kick;

AT(.text.bsp.wireless_mic)
void mic_init(u8 sample_rate, u16 samples, u8 channel)
{
    mic_cfg.sample_rate = sample_rate;
    mic_cfg.samples = samples*2;

#if WIRELESS_MIC_SRC_EN
    //32k采样率 控制2.5ms一个周期的话就是80点kick一次
    mic_cfg.sample_rate = SPR_32000;
    mic_cfg.samples = 80*2;
    mic_cfg.out_ctrl = 0;
#endif

    mic_cfg.bits_mode = WIRELESS_MIC_24B_PCM_EN? 0 : 1;

    get_mic_cfg(0, &mic_cfg.channel, &mic_cfg.dig_gain, &mic_cfg.anl_gain);

//    mic_cfg.callback = (pcm_callback_t)sdadc_dummy;
}

AT(.text.bsp.wireless_mic)
void mic_audio_output_callback_set(audio_callback_t callback)
{
    mic_cfg.callback = (pcm_callback_t)callback;

    //改到蓝牙连接后再初始化，节省未连接功耗
//    sdadc_init(&mic_cfg);
//    sdadc_start(mic_cfg.channel);
}

AT(.text.bsp.wireless_mic)
void mic_start(void)
{
    MIC_LDO_EN();

    //初始化定时器, 用于MIC调速
    wl_tick_time_init();

    int ret = sdadc_init(&mic_cfg);
    if(ret < 0) {
        printf("sdadc_init fail: %d\n", ret);
    }
    sdadc_start(mic_cfg.channel);

    sdadc_w4_kick = true;
}

AT(.com_text.bsp.wireless_mic)
void mic_dma_start(void)
{
    if (sdadc_w4_kick) {
        sdadc_w4_kick = false;
        sdadc_dma_start(mic_cfg.channel);
    }
}

AT(.text.bsp.wireless_mic)
void mic_stop(void)
{
    sdadc_exit(mic_cfg.channel);

    wl_tick_time_exit();

    MIC_LDO_DIS();
}

#if ADAPTER_LOCAL_MIC_MIX_EN
AT(.text.bsp.wireless_mic)
void local_mic_init(void)
{
    mic_init(WIRELESS_MIC_SAMPLE_RATE_SELECT, WIRELESS_MIC_SAMPLES_SELECT, WIRELESS_MIC_CHANNEL_SELECT);
    mic_cfg.callback = mic_mix_process_cb;

    mic_start();
    mic_dma_start();
}
#endif
