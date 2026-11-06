//#include "include.h"
//#include "i2s_audio.h"
//
////注意：该文件用于I2S主机模式(用于降低音频延迟,待调试)
//
//#if ADAPTER_I2S_IN_OUT_EN && I2S_MASTER_EN
//
//#define I2S_CACHE_SIZE            1024
//#define I2S_FRAME_SIZE            480
//
//typedef struct {
//    audio_callback_t callback;
//    i2s_cfg_t i2s_cfg;
//} i2s_in_out_ctl_t;
//
//
//static uint8_t i2s_rx_cache[I2S_CACHE_SIZE] AT(.i2s_buf.master);
//static uint8_t i2s_tx_frame[I2S_FRAME_SIZE] AT(.i2s_buf.master);
//static uint8_t i2s_rx_frame[I2S_FRAME_SIZE] AT(.i2s_buf.master);
//
//static i2s_in_out_ctl_t i2s_in_out_ctl;
//
//AT(.com_text.i2s_in_out.proc)
//void i2s_tx_process(void *buf, u32 samples, bool iis_32bit)
//{
////    GPIOASET = BIT(5);
////    asm("nop");asm("nop");asm("nop");asm("nop");
////    asm("nop");asm("nop");asm("nop");asm("nop");
////    GPIOACLR = BIT(5);
//}
//
////I2S接收中断回调函数,缓存和处理接收数据
//AT(.com_text.i2s_in_out.proc)
//void i2s_rx_process(void *buf, u32 samples, bool i2s_32bit)
//{
//    u32 *ptr32 = (u32*)buf;
//    u16 *ptr16 = (u16*)buf;
//    samples = WIRELESS_MIC_SAMPLES_SELECT;
//
//    if(i2s_32bit) {                                   //I2S_32BIT
//        for (int i = 0; i < samples; i++) {           //32BIT ->16bit  for dac out
//           ptr16[2*i] =  (u16)(ptr32[2*i] >> 16);
//           ptr16[2*i+1] =  (u16)(ptr32[2*i+1] >> 16);
//        }
//    }
//
//    for(int j = 0; j < samples; j++) {                //Stereo --> Mono
//        ptr16[j] = ptr16[2*j];
//    }
//
//    cbuf_input_audio((u8 *)buf, samples*2, &rx_cbuf);
//}
//
//AT(.com_text.i2s_in_out.proc)
//void i2s_frame_process(u8 *obuf, u8 *ibuf, u32 samples, int ch_mode)
//{
//#if I2S_BIT_MODE == I2S_32BIT
//    s32 *ptr = (s32 *)obuf;
//    s16 *ptr16 = (s16 *)ibuf;
//
//    if(ch_mode == 1) {                                      //TX Mono
//        for (int i = 0; i< samples; i++) {                  //16->32位扩展
//            ptr[2*i] = (s32)(ptr16[i] << 0);
//            ptr[2*i+1] = (s32)(ptr16[i] << 0);
//        }
//    } else {                                                //TX Stero
//        for (int i = 0; i< samples; i++) {
//            ptr[2*i] = (s32)(ptr16[2*i] << 0);             //16->32位扩展
//            ptr[2*i+1] = (s32)(ptr16[2*i+1] << 0);
//        }
//    }
//#else
//    s16 *ptr = (s16 *)obuf;
//    s16 *ptr16 = (s16 *)ibuf;
//
//    if(ch_mode == 1) {
//        for (int i = 0; i< samples; i++) {
//            ptr[2*i] = ptr16[i];
//            ptr[2*i+1] = ptr16[i];
//        }
//    } else {
//        memcpy(obuf, ibuf, samples*ch_mode*2);
//    }
//#endif
//}
//
//AT(.com_text.i2s_in_out.proc)
//void i2s_audio_in_and_out_input(u8 *ptr, u32 samples, int ch_mode, void *params)
//{
//    static u16 samples_cnt = 0;
//
//    memcpy(i2s_tx_frame + samples_cnt*2, ptr, samples*2);
//    samples_cnt += samples;
//
//    //存够完整的一帧数据再TX
//    if(samples_cnt >= WIRELESS_MIC_SAMPLES_SELECT) {
//        u8 *i2s_tx_buf = (u8 *)i2s_out_get_obuf(0);
//        i2s_frame_process(i2s_tx_buf, &i2s_tx_frame[0], samples, ch_mode);
//        samples_cnt = 0;
//    }
//
//    u8 *out_buf = &i2s_rx_frame[0];//接收数据
//    u16 rx_total_size = cbuf_total_samples_get(&rx_cbuf);
//    if(rx_total_size >= samples) {
//        cbuf_output_audio(out_buf, samples*2, &rx_cbuf);
//    } else  {
//        memset(out_buf, 0, samples*2);
//    }
//
//    if(i2s_in_out_ctl.callback) {
//        i2s_in_out_ctl.callback(out_buf, samples, ch_mode, params);
//    }
//}
//
//AT(.text.i2s_in_out.set)
//void i2s_audio_in_and_output_callback_set(audio_callback_t callback)
//{
//    i2s_in_out_ctl.callback = callback;
//}
//
//AT(.text.i2s_in_out.init)
//void i2s_audio_in_and_out_init(u8 sample_rate, u16 samples, u8 channel)
//{
//    memset(&i2s_in_out_ctl, 0, sizeof(i2s_in_out_ctl_t));
//
//    memset(i2s_rx_cache, 0, sizeof(i2s_rx_cache));
//    cbuf_init(&rx_cbuf, (u8 *)&i2s_rx_cache[0], I2S_CACHE_SIZE);//初始化cbuf，tx与rx的cache与cbuf绑定
//
//    i2s_cfg_t *p_cfg   = &i2s_in_out_ctl.i2s_cfg;
//    p_cfg->mode        = I2S_MASTER_DMATX_DMARX;//I2S_MASTER_DMATX_DMARX;I2S_MASTER_DMARX; I2S_MASTER_DMATX; I2S_SLAVE_DMATX_DMARX
//    p_cfg->iomap       = I2S_MAPPING_SEL;
//    p_cfg->bit_mode    = I2S_BIT_MODE;
//    p_cfg->data_mode   = I2S_DATA_MODE;
//    p_cfg->mclk_sel    = I2S_MCLK_SEL;
//    p_cfg->mclk_out_en = I2S_MCLK_EN;
//    if (I2S_DMA_EN) {
//        p_cfg->dma_cfg.samples = I2S_DMA_SAMPLES;
//        p_cfg->dma_cfg.isr_rx_callback = i2s_rx_process;
//        p_cfg->dma_cfg.isr_tx_callback = i2s_tx_process;
//    }
//
//    i2s_init(p_cfg);
//
//    //这里先不启动,等无线麦开始解码的时候再启动
//    //i2s_dma_start();
//
//    //调试IO
////    GPIOAFEN &= ~BIT(5);
////    GPIOADE  |= BIT(5);
////    GPIOADIR &= ~BIT(5);
////    GPIOACLR = BIT(5);
//}
//#endif
//
