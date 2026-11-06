#include "include.h"
#include "i2s_audio.h"

#if I2S_AUDIO_OUT_EN
typedef struct {
    audio_callback_t callback;
    i2s_cfg_t i2s_cfg;
} i2s_out_ctl_t;

static i2s_audio_buf_t i2s_audio_buf AT(.i2s_buf.dma);
static i2s_out_ctl_t i2s_out_ctl;

static const rbuf_tbl_t i2s_buf_tbl[1] = {
    {
        .buf = (u8 *)&(i2s_audio_buf.cache),
        .size = I2S_INCACHE_SIZE,
    },
};

#if (I2S_SAMPLES_OUT_SET == I2S_16000)
u8 out_buf_16k[80] AT(.i2s_buf);
#endif
AT(.com_text.i2s_mic_in_and_out)
void i2s_tx_process(void *buf, u32 samples, u8 ch_mode, bool iis_32bit)
{
    s16 *pcm = (s16 *)&(i2s_audio_buf.frame);
    uint frame_size = samples*2;

    if (!ring_buf_get(&(i2s_audio_buf.ring_buf), (u8 *)pcm, frame_size)) {
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

AT(.com_text.i2s_audio_out)
void i2s_audio_out_input(u8 *ptr, u32 samples, int ch_mode, void *params)
{
    uint frame_size = samples * 2;
#if (I2S_SAMPLES_OUT_SET == I2S_48000)
    ring_buf_put(&(i2s_audio_buf.ring_buf), ptr, frame_size);
#elif (I2S_SAMPLES_OUT_SET == I2S_16000)
    short *in_pcm = (short *)ptr;
	short *out_pcm = (short *)out_buf_16k;

    uint out_cnt = src_frame_resample(0, in_pcm, out_pcm, samples);
    ring_buf_put(&(i2s_audio_buf.ring_buf), out_buf_16k, out_cnt * 2 * ch_mode);
#endif
    if(i2s_out_ctl.callback != NULL) {
        i2s_out_ctl.callback(ptr, samples, ch_mode, params);
    }
}

AT(.com_text.i2s_audio_out)
void i2s_audio_output_callback_set(audio_callback_t callback)
{
    i2s_out_ctl.callback = callback;
}

//初始化i2s_audio_buf
AT(.com_text.i2s_mic_in_and_out)
void i2s_audio_buf_init(void)
{
    memset(&i2s_audio_buf, 0, sizeof(i2s_audio_buf_t));
    ring_buf_init(&(i2s_audio_buf.ring_buf), &i2s_buf_tbl[0], 1, 0);
}

AT(.text.i2s_audio_out)
void i2s_audio_out_init(u8 sample_rate, u16 samples, u8 channel)
{
    i2s_cfg_t *p_cfg = &i2s_out_ctl.i2s_cfg;
    memset(&i2s_out_ctl, 0, sizeof(i2s_out_ctl));

    i2s_audio_buf_init();

#if I2S_MASTER_EN
    p_cfg->mode        = I2S_MASTER_DMATX;
#else
    p_cfg->mode        = I2S_SLAVE_DMATX;
#endif
    p_cfg->iomap       = I2S_MAPPING_SEL;
    p_cfg->bit_mode    = I2S_BIT_MODE;
    p_cfg->data_mode   = I2S_DATA_MODE;
    p_cfg->mclk_sel    = I2S_MCLK_SEL;
    p_cfg->mclk_out_en = I2S_MCLK_EN;
    if (I2S_DMA_EN) {
#if (I2S_SAMPLES_OUT_SET == I2S_16000)
        src_init(0, 48000,16000);
#endif
        p_cfg->dma_cfg.samples = I2S_DMA_SAMPLES;
        p_cfg->dma_cfg.isr_tx_callback = i2s_tx_process;
        p_cfg->dma_cfg.isr_rx_callback = NULL;
    }

    i2s_init(p_cfg);            //I2S初始化
    i2s_dma_start();
}
#endif
