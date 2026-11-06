/*
 *  mic.c
 *
 *  Created by zoro on 2024-8-29.
 *
 *  本地mic配置
 */

#include "include.h"
#include "mic.h"

static sdadc_cfg_t mic_cfg = {CH_MIC0,  SPR_48000, 3,  1, 1,  ADC2DAC_EN,  WIRELESS_MIC_SAMPLES_SELECT, NULL};
volatile bool sdadc_w4_kick;

#if !ADAPTER_LOCAL_MIC_EN
AT(.buf.device_mic.mic)
#else
AT(.bss.device_mic.mic)
#endif

static u8 mic_sdadc_buf[WIRELESS_MIC_SAMPLES_SELECT*2*2];

AT(.text.bsp.wireless_mic)
void mic_init(u8 sample_rate, u16 samples, u8 channel)
{
    mic_cfg.sample_rate = sample_rate;
    mic_cfg.samples = samples*2;
    mic_cfg.anl_gain = xcfg_cb.mic_anl_gain;   //模拟增益0~13
    mic_cfg.dig_gain = xcfg_cb.mic_dig_gain;   //数字增益0~63

//    mic_cfg.callback = mic_mix_process_cb;
}

AT(.text.bsp.wireless_mic)
void mic_audio_output_callback_set(audio_callback_t callback)
{
    mic_cfg.callback = callback;

    //改到蓝牙连接后再初始化，节省未连接功耗
//    sdadc_init(&mic_cfg);
//    sdadc_start(mic_cfg.channel);
}

AT(.text.bsp.wireless_mic)
void mic_start(void)
{
    sdadc_init(&mic_cfg, mic_sdadc_buf);
    sdadc_start(mic_cfg.channel);

    sdadc_w4_kick = true;
}

AT(.com_text.bsp.wireless_mic)
void wireless_mic_kick(void)
{
    if (sdadc_w4_kick) {
        sdadc_w4_kick = false;
        sdadc_start_kick(mic_cfg.channel);
    }
}

AT(.text.bsp.wireless_mic)
void mic_stop(void)
{
    sdadc_exit(mic_cfg.channel);
}

AT(.com_text.mictrim)
void mic_leave_out_rc_proc(void)
{
    if (xcfg_cb.mic_bias_method != 1){           //内部电阻电容
        return;
    }

    mic_bias_trim();
}

AT(.text.bsp.wireless_mic)
void mic_leave_out_rc_init(void)
{
    if (xcfg_cb.mic_bias_method == 1){            //内部电阻电容
        saradc_ch15_set_ana_ch(ADCCH15_ANA_MIC);
        mic_bias_trim_init();
        mic_trim_w4_done();
        saradc_ch15_set_ana_ch(ADCCH15_ANA_BG);
    }
}

#if ADAPTER_LOCAL_MIC_EN

enum {
    MIC_STA_STOP,
    MIC_STA_INIT,
    MIC_STA_START,
};

typedef struct {
    u8 mute;
    u8 rec_en;
    uint8_t bypass_cnt;             //记录数据流的开关数量
    volatile u8 status;             //记录麦的状态
    volatile u8 done_sem;           //0:finsh 1:running
//    u16 discard_cnt;                //丢掉adc的开始的帧
    ring_buf_t ring_buf;
} local_mic_cb_t;

static local_mic_cb_t local_mic_cb AT(.buf.karaok.sdadc);
//static u8 local_mic_sdadc_buf[240 * 2] AT(.buf.karaok.sdadc);
u8 karaok_pool_buf[240*2] AT(.buf.karaok);
u8 karaok_temp_buf[240] AT(.buf.karaok);

const rbuf_tbl_t karaok_buf_tbl[1] = {
    {
        .buf = karaok_pool_buf,
        .size = 240*2,
    },
};

AT(.text.karaok_proc)
void karaok_local_mic_mute_set(uint8_t mute)
{
    local_mic_cb.mute = mute;

    if(local_mic_cb.mute) {
        memset(karaok_pool_buf, 0, sizeof(karaok_pool_buf));
        memset(karaok_temp_buf, 0, sizeof(karaok_temp_buf));
    }
}

AT(.text.karaok_proc)
void karaok_local_mic_sdadc_process(u8 *ptr, u32 samples, int ch_mode, void *params)
{

//    if(local_mic_cb.discard_cnt < 200) {    //跳过开始的N帧
//        local_mic_cb.discard_cnt++;
//        return;
//    }

    if(local_mic_cb.mute) {
        return;
    }

#if 1
    uint wl_status = wireless_get_status();

    if(wl_status != 0 && wireless_cb.alg_en) {          //无线麦已连接，且算法处理中
        //先保存到buffer，然后等无线麦线程取走
        ring_buf_put((void *)&local_mic_cb.ring_buf, ptr, samples*2);
    } else if(wl_status == 0 && !mic_mix_done_get())    //无线麦已断开
#endif
    {
        //先保存到buffer
        ring_buf_put((void *)&local_mic_cb.ring_buf, ptr, samples*2);

        //然后主动kick算法线程，在local_mic_effect_process处理音效
        kick_loc_mic_effect_proc();
    }
}

AT(.text.karaok_proc)
bool karaok_local_mic_pcm_get(u8 *ptr, u32 samples)
{
#if MIC_DETECT_EN
    if( !MIC_IS_ONLINE()) {
        return false;
    } else
#endif
    {
        if(local_mic_cb.mute) {
            return false;
        }

        if (ring_buf_get((void *)&local_mic_cb.ring_buf, ptr, samples*2)) {
#if ADAPTER_LOCAL_MIC_EQ_DRC_EN
            loc_mic_pacc_process((u8 *)&ptr[0], (u8 *)&ptr[0], samples);
#endif

            return true;
        } else {
//            printf("F");
            return false;
        }
    }
}

//本地mic算法处理回调函数
AT(.text.karaok_proc)
void local_mic_effect_process(void)
{
    u32 samples = 120;

    if(local_mic_cb.status == MIC_STA_STOP) {
        return;
    }

    local_mic_cb.done_sem = 1;

    if(karaok_local_mic_pcm_get(karaok_temp_buf, samples)) {

//#if KARAOK_LOCAL_MIC_MIC_FIRST_EN
//        alg_mic_first_audio_input(karaok_temp_buf, samples, 1, NULL);
//#endif
//#if KARAOK_LOCAL_MIC_HOWLING_EN
//        alg_howling_mic_audio_input(karaok_temp_buf, samples, 1, NULL);
//#endif
//#if KARAOK_LOCAL_MIC_MAGIC_EN
//        alg_magic_audio_input(karaok_temp_buf, samples, 1, NULL);
//#endif
//#if KARAOK_LOCAL_MIC_ECHO_EN
//        alg_echo_audio_input(karaok_temp_buf, samples, 1, NULL);
//#endif
#if ADAPTER_MIX_DRC_EN
        alg_mic_mix_audio_proc_input((s16 *)&karaok_temp_buf[0], samples, (s16 *)&karaok_temp_buf[0]);
#endif
        aubuf0_dma_w4_done();
        aubuf0_dma_kick(karaok_temp_buf, samples, 1);

    }

    local_mic_cb.done_sem = 0;
}

AT(.text.bsp.wireless_mic)
void local_mic_init(void)
{
    memset((uint8_t *)&local_mic_cb, 0, sizeof(local_mic_cb));
    local_mic_cb.done_sem = 0;
    local_mic_cb.status = MIC_STA_INIT;

    memset(karaok_pool_buf, 0, sizeof(karaok_pool_buf));
    ring_buf_init((void *)&local_mic_cb.ring_buf, &karaok_buf_tbl[0], 1, 0);

#if ADAPTER_LOCAL_MIC_EQ_DRC_EN
    sys_clk_req(INDEX_KARAOK, SYS_240M);        //先抬高主频
    //先初始化PACC链路
    loc_mic_pacc_init();

    //然后设置参数
    loc_mic_pacc_set_param();

    //最后使能PACC
    loc_mic_pacc_enable();
#endif

#if ADAPTER_MIX_DRC_EN
    mic_mix_init();
#endif

    mic_init(WIRELESS_MIC_SAMPLE_RATE_SELECT, WIRELESS_MIC_SAMPLES_SELECT, WIRELESS_MIC_CHANNEL_SELECT);
    mic_cfg.callback = karaok_local_mic_sdadc_process;

    mic_start();

    sdadc_start_kick(mic_cfg.channel);
}
#endif
