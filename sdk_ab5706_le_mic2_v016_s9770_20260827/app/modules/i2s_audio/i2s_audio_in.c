#include "include.h"

#if I2S_AUDIO_IN_EN

static i2s_audio_buf_t i2s_audio_buf AT(.i2s_buf.dma);
static i2s_in_ctl_t i2s_in_ctl;

static const rbuf_tbl_t i2s_buf_tbl[1] = {
    {
        .buf = (u8 *)&(i2s_audio_buf.cache),
        .size = I2S_INCACHE_SIZE,
    },
};

//I2S接收中断回调函数,缓存和处理接收数据
AT(.com_text.i2s_audio_in)
void i2s_rx_process(void *buf, u32 samples, u8 ch_mode, bool i2s_32bit)
{
    s32 *ptr32 = (s32*)buf;
    s16 *ptr16 = (s16*)buf;

    if(i2s_32bit) {                                   //I2S_32BIT
        for (int i = 0; i < samples; i++) {           //32BIT ->16bit  for dac out
           ptr16[2*i] =  (s16)(ptr32[2*i] >> 16);
           ptr16[2*i+1] =  (s16)(ptr32[2*i+1] >> 16);
        }
    }

    for(int j = 0; j < samples; j++) {                //Stereo --> Mono
        ptr16[j] = ptr16[2*j];
    }

    uint frame_size = samples * 2;
    ring_buf_put(&(i2s_audio_buf.ring_buf), (u8 *)ptr16, frame_size);

//    if (i2s_in_ctl.callback) {
//        i2s_in_ctl.callback((u8 *)buf, WIRELESS_MIC_SAMPLES_SELECT, 1, NULL);
//    }
}

AT(.com_text.i2s_audio_in)
void i2s_audio_in_input(u8 *ptr, u32 samples, int ch_mode, void *params)
{
    u8 *out_buf = (u8 *)&(i2s_audio_buf.frame);    //接收数据
    uint frame_size = samples * 2;

    if (!ring_buf_get(&(i2s_audio_buf.ring_buf), out_buf, frame_size)) {
        memset(out_buf, 0x00, frame_size);
    }

    if(i2s_in_ctl.callback) {
        i2s_in_ctl.callback((u8 *)out_buf, samples, ch_mode, params);
    }
}

AT(.com_text.i2s_audio_in)
void i2s_audio_input_callback_set(audio_callback_t callback)
{
    i2s_in_ctl.callback = callback;
}

//初始化i2s_audio_buf
AT(.com_text.i2s_mic_in_and_out)
void i2s_audio_buf_init(void)
{
    memset(&i2s_audio_buf, 0, sizeof(i2s_audio_buf_t));
    ring_buf_init(&(i2s_audio_buf.ring_buf), &i2s_buf_tbl[0], 1, 0);
}

AT(.text.i2s_audio_in)
void i2s_audio_in_init(u8 sample_rate, u16 samples, u8 channel)
{
    memset(&i2s_in_ctl, 0, sizeof(i2s_in_ctl));

    i2s_audio_buf_init();

    i2s_cfg_t *p_cfg = &i2s_in_ctl.i2s_cfg;
#if I2S_MASTER_EN
    p_cfg->mode        = I2S_MASTER_DMARX;
#else
    p_cfg->mode        = I2S_SLAVE_DMARX;
#endif
    p_cfg->iomap       = I2S_MAPPING_SEL;
    p_cfg->bit_mode    = I2S_BIT_MODE;
    p_cfg->data_mode   = I2S_DATA_MODE;
    p_cfg->mclk_sel    = I2S_MCLK_SEL;
    p_cfg->mclk_out_en = I2S_MCLK_EN;
    if (I2S_DMA_EN) {
        p_cfg->dma_cfg.samples = I2S_DMA_SAMPLES;
        p_cfg->dma_cfg.isr_rx_callback = i2s_rx_process;
        p_cfg->dma_cfg.isr_tx_callback = NULL;
    }
    i2s_init(p_cfg);                       //可以改到callback set之后再初始化
    i2s_dma_start();
}
#endif // I2S_AUDIO_IN_EN
