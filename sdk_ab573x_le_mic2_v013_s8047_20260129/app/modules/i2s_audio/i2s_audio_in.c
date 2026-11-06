#include "include.h"
#include "i2s_audio.h"
#if I2S_AUDIO_IN_EN

#define I2S_INCACHE_SIZE            (WIRELESS_MIC_SAMPLES_SELECT*2*4)
#define I2S_OUTCACHE_SIZE           (WIRELESS_MIC_SAMPLES_SELECT*2)

typedef struct {
    uint8_t incache[I2S_INCACHE_SIZE];
    uint8_t outcache[I2S_OUTCACHE_SIZE];

    ring_buf_t i2s_rx_ring_buf;
} i2s_audio_buf_t;

static i2s_audio_buf_t i2s_audio_buf AT(.i2s_buf);
static i2s_ctl_t i2s_in_ctl;

const rbuf_tbl_t i2s_rx_buf_tbl[1] = {
    {
        .buf = i2s_audio_buf.incache,
        .size = I2S_INCACHE_SIZE,
    },
};

//I2S接收中断回调函数,缓存和处理接收数据
AT(.com_text.i2s_audio_in_proc)
void i2s_rx_process(void *buf, u32 samples, bool i2s_32bit)
{
#if I2S_MODE_SEL == I2S_MASTER
    //i2s主机的时候需要调速
    wl_save_tick1_time();
#endif

    s32 *ptr32 = (s32*)buf;
    s16 *ptr16 = (s16*)buf;
    if(i2s_32bit) {                                      //I2S_32BIT
        for (int i = 0; i < samples; i++) {              //32BIT ->16bit  for dac out
           ptr16[2*i] =  (s16)(ptr32[2*i] >> 16);
           ptr16[2*i+1] =  (s16)(ptr32[2*i+1] >> 16);
        }
    }

    for(int j = 0; j < samples; j++) {                  //Stereo --> Mono
        ptr16[j] = ptr16[2*j];
    }

    ring_buf_t *ring_buf = &(i2s_audio_buf.i2s_rx_ring_buf);
    ring_buf_put(ring_buf, (u8 *)buf, samples * 2);
}

AT(.com_text.i2s_audio_in_proc)
void i2s_audio_in_input(u8 *ptr, u32 samples, u32 params)
{
    u32 pcm_mode = params & 0xffff;
    bool is_24bit = pcm_mode & PCM_24BIT;
    uint ch_mode = pcm_mode & PCM_CHMASK;

    u8 *out_buf = (u8 *)&(i2s_audio_buf.outcache);

    if (!is_24bit) {
        ring_buf_t *ring_buf = &(i2s_audio_buf.i2s_rx_ring_buf);
        if (ring_buf_get_count(ring_buf) > (samples * 2 * ch_mode)) { //ring_buf样点数够就取
            ring_buf_get(ring_buf,(u8 *)out_buf, samples * 2 * ch_mode);
        }
    }
    if(i2s_in_ctl.callback) {
        i2s_in_ctl.callback((u8 *)out_buf, samples, params);
    }
}

void i2s_audio_input_callback_set(audio_callback_t callback)
{
    i2s_in_ctl.callback = (pcm_callback_t)callback;
}

AT(.text.i2s_audio_in)
void i2s_audio_in_init(u8 sample_rate, u16 samples, u8 channel)
{
    i2s_cfg_t *p_cfg = &i2s_in_ctl.i2s_cfg;
    memset(&i2s_in_ctl, 0, sizeof(i2s_in_ctl));

	memset(&i2s_audio_buf,0,sizeof(i2s_audio_buf));
    ring_buf_init(&(i2s_audio_buf.i2s_rx_ring_buf), i2s_rx_buf_tbl, 1, 0);

#if (I2S_MODE_SEL == I2S_SLAVE)
    p_cfg->mode        = I2S_SLAVE_DMARX;
#else
    p_cfg->mode        = I2S_MASTER_DMARX;
#endif // I2S_MODE_SEL
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

#if (I2S_MODE_SEL == I2S_SLAVE)
    i2s_dma_start();                      //从机时,直接启动DMA
#endif
}

AT(.text.i2s_audio_in)
void i2s_audio_in_exit(void)
{
    i2s_dma_stop();

    memset(&i2s_audio_buf,0,sizeof(i2s_audio_buf));
    ring_buf_init(&(i2s_audio_buf.i2s_rx_ring_buf), i2s_rx_buf_tbl, 1, 0);
}
#endif // I2S_AUDIO_IN_EN
