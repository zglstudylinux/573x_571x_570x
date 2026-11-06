#include "include.h"
#include "i2s_audio.h"

#if I2S_AUDIO_OUT_EN || I2S_AUDIO_IN_AND_OUT_EN

#define I2S_RX_EN                   0 //如果只有I2S_TX需求, 可以关掉I2S_RX_EN, 省空间
#define I2S_TX_EN                   1

#define I2S_INCACHE_SIZE            (WIRELESS_MIC_SAMPLES_SELECT*2*4)
#define I2S_OUTCACHE_SIZE           (WIRELESS_MIC_SAMPLES_SELECT*2*4)
#define I2S_FRAME_SIZE              (WIRELESS_MIC_SAMPLES_SELECT*2)

#if (I2S_MODE_SEL == I2S_SLAVE)
#define i2s_sync_init()
#define i2s_get_sync(n)
#define i2s_sync_proc()
#else
#define i2s_sync_init()              wl_tick_time_init();
#define i2s_get_sync(n)              wl_get_tick1_time(n)                                                             //获取I2S偏差
#define i2s_sync_proc()              wl_play_sync_tick1(false, WIRELESS_MIC_TX_INTERVAL/WIRELESS_MIC_COMB_NB*2)       //I2S偏差处理
#endif

//调速说明: 正常情况下,下面spi输出的0xa1和0xa2,用逻辑分析仪抓时序的时候, 随着时间变长, 其最大偏差不应该超过15us (可用IO输出观察时序)

static i2s_ctl_t i2s_ctl;

AT(.com_text.i2s_audio_in_out_proc)
void i2s_audio_in_out_dma_kick(uint tick_cnt)
{
//    my_spi_putc(0xa1);
    i2s_get_sync(tick_cnt);
    i2s_dma_start();
}

//----------------------------------------------------------------------------
#if I2S_TX_EN
//DMA方式的时候需要另外开辟buf
struct {
    uint8_t outcache[I2S_OUTCACHE_SIZE];
    uint8_t frame[I2S_FRAME_SIZE];
    ring_buf_t ring_buf;
} i2s_tx_buf_cb;

const rbuf_tbl_t i2s_tx_buf_tbl[] = {
    {
        .buf = i2s_tx_buf_cb.outcache,
        .size = I2S_OUTCACHE_SIZE,
    },
};

AT(.com_text.i2s_audio_in_out_proc)
void i2s_tx_process(void *buf, u32 samples, bool i2s_32bit)
{
    //i2s主机的时候需要调速
    wl_save_tick1_time();
//    my_spi_putc(0xa2);

    s32 *ptr32 = (s32*)buf;
    s16 *ptr16 = (s16*)buf;

    s16 *pcm = (s16 *)&(i2s_tx_buf_cb.frame);
    ring_buf_get(&i2s_tx_buf_cb.ring_buf, (u8 *)pcm, samples * 2);

    if(i2s_32bit) {                                      //I2S_32BIT
        for (int i = 0; i < samples; i++) {              //Mono --> Stereo
           ptr32[2*i] =   (s32)(pcm[i] << 16);
           ptr32[2*i+1] = (s32)(pcm[i] << 16);
        }
    } else {
        for(int j = 0; j < samples; j++) {               //Mono --> Stereo
            ptr16[2*j]     = pcm[j];
            ptr16[2*j + 1] = pcm[j];
        }
    }
}
#endif

//----------------------------------------------------------------------------
#if I2S_RX_EN
struct {
    uint8_t incache[I2S_INCACHE_SIZE];
    ring_buf_t ring_buf;
} i2s_rx_buf_cb;

const rbuf_tbl_t i2s_rx_buf_tbl[] = {
    {
        .buf = i2s_rx_buf_cb.incache,
        .size = I2S_INCACHE_SIZE,
    },
};

//I2S接收中断回调函数,缓存和处理接收数据
AT(.com_text.i2s_audio_in_out_proc)
void i2s_rx_process(void *buf, u32 samples, bool i2s_32bit)
{
    s32 *ptr32 = (s32*)buf;
    s16 *ptr16 = (s16*)buf;

    if(i2s_32bit) {                                      //I2S_32BIT
        for (int i = 0; i < samples; i++) {              //32BIT ->16bit  for dac out
           ptr16[2*i] =  (s16)(ptr32[2*i] >> 16);
           ptr16[2*i+1] =  (s16)(ptr32[2*i+1] >> 16);
        }
    }

    for(int j = 0; j < samples; j++) {                   //Stereo --> Mono
        ptr16[j] = ptr16[2*j];
    }

    ring_buf_t *ring_buf = &(i2s_rx_buf_cb.ring_buf);
    ring_buf_put(ring_buf, (u8 *)buf, samples * 2);
}
#endif

//----------------------------------------------------------------------------
AT(.com_text.i2s_audio_in_out_proc)
void i2s_audio_in_out_input(u8 *ptr, u32 samples, u32 params)
{
    u32 pcm_mode = params & 0xffff;
    bool is_24bit = pcm_mode & PCM_24BIT;

    if (!is_24bit) {
#if I2S_TX_EN
        //使用DMA方式的时候, 数据放到ring_buf, 等中断来取数据
        ring_buf_put(&i2s_tx_buf_cb.ring_buf, ptr, samples * 2);
#endif

#if I2S_RX_EN
        if(ring_buf_get_count(&(i2s_rx_buf_cb.ring_buf)) > (samples * 2)) {
            ring_buf_get(&(i2s_rx_buf_cb.ring_buf), ptr, samples * 2);
        } else {
            memset(ptr, 0, samples * 2);
        }
#endif
    }

    i2s_sync_proc();

    if(i2s_ctl.callback) {
        i2s_ctl.callback(ptr, samples, params);
    }
}

void i2s_audio_in_out_input_callback_set(audio_callback_t callback)
{
    i2s_ctl.callback = (pcm_callback_t)callback;
}

AT(.text.i2s_audio_in_out)
void i2s_audio_in_out_init(u8 sample_rate, u16 samples, u8 channel)
{
    i2s_cfg_t *p_cfg = &i2s_ctl.i2s_cfg;
    memset(&i2s_ctl, 0, sizeof(i2s_ctl));

#if I2S_RX_EN
	memset(&i2s_rx_buf_cb, 0, sizeof(i2s_rx_buf_cb));
    ring_buf_init(&i2s_rx_buf_cb.ring_buf, &i2s_rx_buf_tbl[0], 1, 0);
#endif

#if I2S_TX_EN
	memset(&i2s_tx_buf_cb, 0, sizeof(i2s_tx_buf_cb));
    ring_buf_init(&i2s_tx_buf_cb.ring_buf, &i2s_tx_buf_tbl[0], 1, 0);
#endif

    p_cfg->mode        = I2S_MASTER_DMATX_DMARX;
    p_cfg->iomap       = I2S_MAPPING_SEL;
    p_cfg->bit_mode    = I2S_BIT_MODE;
    p_cfg->data_mode   = I2S_DATA_MODE;
    p_cfg->mclk_sel    = I2S_MCLK_SEL;
    p_cfg->mclk_out_en = I2S_MCLK_EN;
    p_cfg->dma_cfg.samples = I2S_DMA_SAMPLES;

#if I2S_RX_EN
    p_cfg->dma_cfg.isr_rx_callback = i2s_rx_process;
#endif
#if I2S_TX_EN
    p_cfg->dma_cfg.isr_tx_callback = i2s_tx_process;
#endif

    i2s_init(p_cfg);                      //可以改到callback set之后再初始化

#if (I2S_MODE_SEL == I2S_SLAVE)
    i2s_dma_start();                      //从机时或者SRC时,直接启动I2S
#endif

    //初始化定时器, 使用I2S调速
    i2s_sync_init();
}

AT(.text.i2s_audio_in_out)
void i2s_audio_in_out_exit(void)
{
    i2s_dma_stop();

#if I2S_RX_EN
    memset(&i2s_rx_buf_cb, 0, sizeof(i2s_rx_buf_cb));
    ring_buf_init(&i2s_rx_buf_cb.ring_buf, &i2s_rx_buf_tbl[0], 1, 0);
#endif

#if I2S_TX_EN
    memset(&i2s_tx_buf_cb, 0, sizeof(i2s_tx_buf_cb));
    ring_buf_init(&i2s_tx_buf_cb.ring_buf, &i2s_tx_buf_tbl[0], 1, 0);
#endif
}
#endif // I2S_AUDIO_IN_EN
