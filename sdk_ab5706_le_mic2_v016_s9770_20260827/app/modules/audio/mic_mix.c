/*
 * 本地MIC混合无线MIC后，一起输出
 *
 ****************************************************************************************
 */
#include "include.h"
#include "mic_mix.h"

#if ADAPTER_MIX_DRC_EN

static mic_mix_t mic_mix AT(.bss.mic_mix);

s32 pacc_temp_buf[120] AT(.buf.mic_mix);

extern u8 karaok_temp_buf[];

//只有本地麦在线，没有无线麦链接时在此接口处理
AT(.text.mic_mix_proc)
void alg_mic_mix_audio_proc_input(s16 *ptr, u32 samples, s16 *pcm_out)
{
    s16 *rptr = (s16 *)ptr;
    mic_mix.done_sem = 1;

    //s16q15转s32q15
    for(uint i=0; i<samples; i++) {
        pacc_temp_buf[i] = rptr[i];
    }

    mix_pacc_process(pcm_out, (u8 *)pacc_temp_buf, samples);

    mic_mix.done_sem = 0;
}

AT(.text.mic_mix_proc)
void mic_mix_audio_proc(s16 *pcm0, s16 *pcm1, uint8_t samples, s16 *pcm_out)
{
    mic_mix.done_sem = 1;

#if ADAPTER_LOCAL_MIC_EN
    //借用一下buf取数据 mix有线mic数据一共三路麦
    if(karaok_local_mic_pcm_get((u8 *)karaok_temp_buf, samples)) {
        s16 *loc = (void *)karaok_temp_buf;
        for(uint i=0; i<samples; i++) {
            pacc_temp_buf[i] = pcm0[i] + pcm1[i] + loc[i];
        }
    } else
#endif
    {
        //s16q15转s32q15
        for(uint i=0; i<samples; i++) {
            pacc_temp_buf[i] = pcm0[i] + pcm1[i];
        }
    }

    mix_pacc_process(pcm_out, (u8 *)pacc_temp_buf, samples);

    mic_mix.done_sem = 0;
}

AT(.text.mic_mix_proc)
u8 mic_mix_done_get(void)
{
    return mic_mix.done_sem;
}

AT(.text.mic_mix)
void mic_mix_exit(void)
{
    printf("%s\n", __func__);
    while(mic_mix.done_sem);//等待pacc0跑完
    mix_pacc_exit();
}

AT(.text.mic_mix)
void mic_mix_init(void)
{
    printf("%s\n", __func__);
    mic_mix.done_sem = 0;

    //先初始化PACC链路
    mix_pacc_init();

    //然后设置参数
    mix_pacc_set_param();

    //最后使能PACC
    mix_pacc_enable();
}


#endif
