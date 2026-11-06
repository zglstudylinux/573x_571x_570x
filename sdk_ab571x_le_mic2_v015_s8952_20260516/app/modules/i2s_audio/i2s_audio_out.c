#include "include.h"
#include "i2s_audio.h"


#if I2S_AUDIO_OUT_EN
typedef struct {
    audio_callback_t callback;
    i2s_cfg_t i2s_cfg;
} i2s_out_ctl_t;

static i2s_out_ctl_t i2s_out_ctl;

AT(.com_text.i2s_mic_out)
void i2s_fram_16_to_32(u8 *obuf, u8 *ibuf, u32 samples, int ch_mode)
{
    s32 *ptr = (s32 *)obuf;
    s16 *ptr16 = (s16 *)ibuf;
    if(ch_mode == 1) {                                             //TX Mono
        for (int i = 0; i< samples; i++) {                         //16->32位扩展
            ptr[2*i] = (s32)(ptr16[i] << 16);
            ptr[2*i+1] = (s32)(ptr16[i] << 16);
        }
    } else {                                                       //TX Stero
        for (int i = 0; i< samples; i++) {
            ptr[2*i] = (s32)(ptr16[2*i] << 16);                    //16->32位扩展
            ptr[2*i+1] = (s32)(ptr16[2*i+1] << 16);
        }
    }
}

AT(.com_text.i2s_audio_out)
void i2s_audio_out_input(u8 *ptr, u32 samples, int ch_mode, void *params)
{
    u8 *obuf = (u8 *)i2s_out_get_obuf(0);
    i2s_fram_16_to_32(obuf, ptr, samples, ch_mode);

    if(i2s_out_ctl.callback != NULL) {
        i2s_out_ctl.callback(ptr, samples, ch_mode, params);
    }
}

AT(.com_text.i2s_audio_out)
void i2s_audio_output_callback_set(audio_callback_t callback)
{
    i2s_out_ctl.callback = callback;
}

AT(.text.i2s_audio_out)
void i2s_audio_out_init(u8 sample_rate, u16 samples, u8 channel)
{
    i2s_cfg_t *p_cfg = &i2s_out_ctl.i2s_cfg;
    memset(&i2s_out_ctl, 0, sizeof(i2s_out_ctl));

    p_cfg->mode        = I2S_MASTER_DMATX;
    p_cfg->iomap       = I2S_MAPPING_SEL;
    p_cfg->bit_mode    = I2S_BIT_MODE;
    p_cfg->data_mode   = I2S_DATA_MODE;
    p_cfg->mclk_sel    = I2S_MCLK_SEL;
    p_cfg->mclk_out_en = I2S_MCLK_EN;
    if (I2S_DMA_EN) {
        p_cfg->dma_cfg.samples = I2S_DMA_SAMPLES;
        p_cfg->dma_cfg.isr_tx_callback = NULL;
        p_cfg->dma_cfg.isr_rx_callback = NULL;
    }
    i2s_init(p_cfg);            //I2S初始化
    i2s_dma_start();
}
#endif
