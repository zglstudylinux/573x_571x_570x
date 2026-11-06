#include "include.h"
#include "i2s_audio.h"

//注意：该文件用于I2S从机模式(发送和接收都过cbuf,会增加音频延迟)

#if ADAPTER_I2S_IN_OUT_EN
typedef struct {
    audio_callback_t callback;
    i2s_cfg_t i2s_cfg;
} i2s_in_and_out_ctl_t;

static i2s_audio_buf_t i2s_audio_buf AT(.i2s_buf.dma);
static i2s_in_and_out_ctl_t i2s_in_and_out_ctl;

static const rbuf_tbl_t i2s_buf_tbl[2] = {
    {
        .buf = (u8 *)&(i2s_audio_buf.tx_cache),
        .size = I2S_INCACHE_SIZE,
    },
    {
        .buf = (u8 *)&(i2s_audio_buf.rx_cache),
        .size = I2S_INCACHE_SIZE,
    },
};

AT(.com_text.i2s_mic_in_and_out)
void i2s_tx_process(void *buf, u32 samples, u8 ch_mode, bool iis_32bit)
{
    s16 *pcm = (s16 *)&(i2s_audio_buf.tx_frame);
    uint frame_size = samples*2;

    if (!ring_buf_get(&(i2s_audio_buf.tx_ring_buf), (u8 *)pcm, frame_size)) {
        memset(pcm, 0x00, frame_size);
    }

    if (iis_32bit) {
        s32 *dma_buf = (s32 *)buf;

        for(int i=0;i<samples;i++){
            dma_buf[2*i] = (s32)(pcm[i] << 16);
            dma_buf[2*i+1] = (s32)(pcm[i] << 16);
        }

    } else {
        s16 *dma_buf = (s16 *)buf;

        for(int i=0;i<samples;i++){
            dma_buf[2*i] = (s16)(pcm[i]);
            dma_buf[2*i+1] = (s16)(pcm[i]);
        }
    }
}

//I2S接收中断回调函数,缓存和处理接收数据
AT(.com_text.i2s_mic_in_and_out)
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
    ring_buf_put(&(i2s_audio_buf.rx_ring_buf), (u8 *)ptr16, frame_size);
}

AT(.com_text.i2s_mic_in_and_out)
void i2s_audio_in_and_out_input(u8 *ptr, u32 samples, int ch_mode, void *params)
{
    uint frame_size = samples * 2;
    ring_buf_put(&(i2s_audio_buf.tx_ring_buf), ptr, frame_size);

    u8 *out_buf = (u8 *)&(i2s_audio_buf.rx_frame);    //接收数据

    if (!ring_buf_get(&(i2s_audio_buf.rx_ring_buf), out_buf, frame_size)) {
        memset(out_buf, 0x00, frame_size);
    }

    if(i2s_in_and_out_ctl.callback != NULL) {
        i2s_in_and_out_ctl.callback(out_buf, samples, ch_mode, params);//把IIS收到的数据发给下一层
    }
}

AT(.com_text.i2s_mic_in_and_out)
void i2s_audio_in_and_output_callback_set(audio_callback_t callback)
{
    i2s_in_and_out_ctl.callback = callback;
}

//初始化i2s_audio_buf，清0，绑定cbuf
AT(.com_text.i2s_mic_in_and_out)
void i2s_audio_buf_init(void)
{
    memset(&i2s_audio_buf, 0, sizeof(i2s_audio_buf_t));

    ring_buf_init(&(i2s_audio_buf.tx_ring_buf), &i2s_buf_tbl[0], 1, 0);
    ring_buf_init(&(i2s_audio_buf.rx_ring_buf), &i2s_buf_tbl[1], 1, 0);
}

AT(.com_text.i2s_mic_in_and_out)
void i2s_audio_in_and_out_init(u8 sample_rate, u16 samples, u8 channel)
{
    memset(&i2s_in_and_out_ctl, 0, sizeof(i2s_in_and_out_ctl));

    i2s_audio_buf_init();

    i2s_cfg_t *p_cfg   = &i2s_in_and_out_ctl.i2s_cfg;
#if I2S_MASTER_EN
    p_cfg->mode        = I2S_MASTER_DMATX_DMARX;
#else
    p_cfg->mode        = I2S_SLAVE_DMATX_DMARX;
#endif

    p_cfg->iomap       = I2S_MAPPING_SEL;
    p_cfg->bit_mode    = I2S_BIT_MODE;
    p_cfg->data_mode   = I2S_DATA_MODE;
    p_cfg->mclk_sel    = I2S_MCLK_SEL;
    p_cfg->mclk_out_en = I2S_MCLK_EN;
    if (I2S_DMA_EN) {
        p_cfg->dma_cfg.samples = I2S_DMA_SAMPLES;
        p_cfg->dma_cfg.isr_rx_callback = i2s_rx_process;
        p_cfg->dma_cfg.isr_tx_callback = i2s_tx_process;
    }

    i2s_init(p_cfg);            //I2S初始化
    i2s_dma_start();
}
#endif

