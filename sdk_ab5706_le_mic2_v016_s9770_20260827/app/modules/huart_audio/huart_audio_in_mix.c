#include "include.h"
#include "huart_audio_in_mix.h"
/*
 * 文件名称: huart_audio_in_mix.c
 * 功能描述: 本文件为HUART DMA 输出数字信号 处理模块
    开启本功能后，在线eq调试会失效哦~
 ****************************************************************************************
    code：
    buf:
    time :
 */
#if ADAPTER_HUART_INPUT_EN
#define HUART_AUDIO_BAUD                4000000          //haurt波特率
#define HUART_SAMPLES                   120*2            //16bit = 8bit * 2
#define HUART_TX_PORT_DEF               HUART_TR_PE6     //huart发射引脚
#define HUART_RX_PORT_DEF               HUART_TR_PE7     //huart接收引脚
#define HUART_IN_INFO_PRINT             0
#define PROCESS_OUT_SAMPLES             120                       //每次存取帧长
#define HUART_IN_CACHE_SIZE             HUART_SAMPLES * 2

static huart_audio_in_cfg_t huart_audio_in_cfg AT(.buf.huart);
static huart_rx_str_t huart_rx_str AT(.buf.huart_in);
static u8 huart_audio_in_incache[HUART_IN_CACHE_SIZE] AT(.buf.huart);
static u8 huart_audio_in_buffer[HUART_RX_MAX_LEN] AT(.buf.huart);
static u8 huart_audio_process_cache[HUART_SAMPLES] AT(.buf.huart);
static ring_buf_t huart_ring_buf AT(.buf.huart);

#if HUART_IN_INFO_PRINT
AT(.com_text.haurt_info)
const char huart_in_info[] = "HUART_IN samples = %d, isrcnt = %d (SR_%d)\n";
AT(.com_text.huart_info_1)
const char huart_in_info1[] = "--huart_audio_in_irq_cb = %x--\n";
#endif

static const rbuf_tbl_t huart_buf_tbl[1] = {
    {
        .buf = (u8 *)&huart_audio_in_incache,
        .size = HUART_IN_CACHE_SIZE,
    },
};


AT(.com_text.huart_in_proc.input)
void huart_audio_in_irq_cb(u8 *ptr, u32 samples, int ch_mode)
{
#if HUART_IN_INFO_PRINT
    static u32 ticks = 0;
    static u32 isr_cnt = 0;
    isr_cnt++;
    if (tick_check_expire(ticks,1000)) {
        my_printf(huart_in_info, samples, isr_cnt, samples*isr_cnt);
        isr_cnt = 0;
        ticks = tick_get();
    }
#endif

    if (huart_audio_in_cfg.mute) {
        memset(ptr, 0, HUART_SAMPLES);
        return;
    }

#if ADAPTER_HUART_COMMAND_EN
    huart_rx_str_t *huart = (huart_rx_str_t *)ptr;
    u8 *pcm = huart->rx_buf;
    if (huart->rx_type == TX_TYPE_AUDIO_CMD) {            //解析audio后面附带的命令
        for (u8 i = 0; i < UART_CMMAND_SIZE; i++) {
            uart_command_rx_callback(huart->rx_buf[HUART_SAMPLES + i]);
        }
    }
    if(!wireless_cb.connected_sta && huart_audio_in_cfg.kick_flag) {
        huart_audio_in_cfg.kick_flag = 0;
    }

#endif
    if (huart_audio_in_cfg.kick_flag) {
        ring_buf_put(&huart_ring_buf,ptr,samples * 2 * ch_mode);
    } else {
        if (huart_audio_in_cfg.callback) {
            huart_audio_in_cfg.callback(ptr, samples, ch_mode, NULL);
        }
    }

}

AT(.com_huart.text)
void huart_rx_done_cb(void)
{
    huart_audio_in_irq_cb(huart_audio_in_buffer,PROCESS_OUT_SAMPLES,1);
}

AT(.text.huart.input)
void huart_audio_in_input(u8 *ptr, u32 samples, int ch_mode, void *param)
{
    s16 *rptr = (s16 *)ptr;

    uint frame_size = samples * 2;
    if(!huart_audio_in_cfg.kick_flag) {
        huart_audio_in_cfg.kick_flag = 1;
    }
    if (ring_buf_get(&huart_ring_buf, huart_audio_process_cache, frame_size)) {
        s16 *pcm_huart = (s16 *)huart_audio_process_cache;
        s16 *pcm_16 = (s16 *)ptr;
        s32 sample = 0;
        for (u8 i = 0; i < samples; i++) {
            sample = pcm_16[i] + pcm_huart[i];
            if(sample > 32767) {
                pcm_16[i] = 32767;
            } else if(sample < -32767){
                pcm_16[i] = -32767;
            } else {
                pcm_16[i] = sample;
            }
        }
    }

    if (huart_audio_in_cfg.callback) {
        huart_audio_in_cfg.callback((u8 *)rptr, samples, ch_mode, param);
    }
}

AT(.text.huart.callback)
void huart_audio_in_output_callback_set(audio_callback_t callback)
{
    huart_audio_in_cfg.callback = callback;
}

AT(.text.huart.mute)
void huart_audio_in_mute_set(uint8_t mute)
{
    huart_audio_in_cfg.mute = mute;
}

AT(.text.huart.init)
void huart_audio_in_init(u8 sample_rate, u16 samples, u8 channel)
{
    memset(&huart_audio_in_cfg,0,sizeof(huart_audio_in_cfg));
    ring_buf_init(&huart_ring_buf, &huart_buf_tbl[0], 1, 0);
    huart_t huart0;
    memset(&huart0, 0x00, sizeof(huart0));
    huart0.rxbuf      = huart_audio_in_buffer;
    huart0.rxbuf_size = HUART_RX_MAX_LEN;
    huart0.rxisr_en = 1;
    huart0.txisr_en = 1;
    //如果在setting使能了HUART音频功能,则在setting配置文件决定是否进行HUART初始化
    if (xcfg_cb.huart_audio_in_en) {
        //HUART串口选择: PA6: 0, PA7: 1, PB1: 2, PB2: 3, PB3: 4, PB4: 5, PE6: 6, PE7: 7 PF3:8 VUSB:9
        if(xcfg_cb.huart_in_sel <= HUART_TR_VUSB) {
            huart0.rx_port = xcfg_cb.huart_in_sel;
            huart0.tx_port = xcfg_cb.huart_in_sel;
            huart_init(&huart0, HUART_AUDIO_BAUD);
        } else {
            //NONE
            printf("please select huart audio intput pin....!!!!\n");
            huart_audio_in_mute_set(1);
        }
    } else {
        huart0.rx_port = HUART_RX_PORT_DEF;
        huart0.tx_port = HUART_TX_PORT_DEF;
        huart_init(&huart0, HUART_AUDIO_BAUD);
    }
}

AT(.text.huart.exit)
void huart_audio_in_exit(void)
{

}
#endif // I2S_TX_AUDIO_EN

