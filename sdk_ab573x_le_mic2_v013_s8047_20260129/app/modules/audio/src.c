#include "include.h"
#include "src.h"

#define UARTDUMP_SRC_EN     0
#define FRAME_LEN           120
#define SRC_INFO_PRINT      0
#if UARTDUMP_SRC_EN
static u8 dump_head_buf[14*2]AT(.buf.src);

static u8 UartDump_buf[2][14 + FRAME_LEN*2]AT(.buf.src);
static u8 UartDump_buf_temp[14 + FRAME_LEN*2]AT(.buf.src);
extern u8 eq_rx_buf[];
#endif

#if SRC_INFO_PRINT
AT(.com_text.src)
const char src_info[] = "SRC samples = %d, isrcnt = %d (SR_%d)\n";
#endif

static src_cfg_t src_cfg;

WEAK mic_pcm_t src_pcm_obuf[WIRELESS_MIC_SAMPLES_SELECT + 10] AT(.buf.src_obuf);

AT(.text.src_proc)
void src0_init(u8 sample_rate, u16 samples, u8 channel)
{
#if UARTDUMP_SRC_EN
    memset(dump_head_buf, 0, 14*2);
    memset(UartDump_buf, 0, 2*(14 + FRAME_LEN*2));

    huart_t huart0;

    memset(&huart0, 0x00, sizeof(huart0));
    huart0.rx_port = HUART_TR_PB3;        //HUART串口选择: PA15: 0, PB2: 1, PB3: 2, PB4: 3, PB15: 4, PE10: 5, PE13: 6, VUSB: 7
    huart0.tx_port = HUART_TR_PB3;
    huart0.rxbuf_loop = 1;
    huart0.rxbuf      = eq_rx_buf;
    huart0.rxbuf_size = 128;
    huart0.txisr_en = 1;
    huart0.rxisr_en = 0;

    huart_init(&huart0, 8000000);        //波特率设置默认是1.5MHz


    dump_buf_init(dump_head_buf,2, huart_tx, huart_wait_txdone);
#endif

    memset(src_pcm_obuf,0,sizeof(src_pcm_obuf));
    src_init(0, 32000,48000);
}



AT(.text.src_proc)
void src0_stop(void)
{

}

AT(.text.src_proc)
void src0_adjust_speed(int speed)
{

}


//AT(.com_text.src_proc)
//void src0_audio_middle(u8 *ptr)
//{
//    u8 *src_ptr = (u8 *)src_pcm_obuf;
//    for(u8 i=0; i<40;i++){
//        src_ptr[6*i] = ptr[4*i];
//        src_ptr[6*i+1] = ptr[4*i+1];
//        src_ptr[6*i+2] = ptr[4*i+2];
//        src_ptr[6*i+3] = ptr[4*i+3];
//        src_ptr[6*i+4] = ptr[4*i+2];
//        src_ptr[6*i+5] = ptr[4*i+3];
//    }
//}
//u8 test_model[160] = {
//    0x23, 0x00, 0x3c, 0xa8, 0xdb, 0x51, 0xf9, 0x95, 0xc8, 0x22, 0xe0, 0xbc, 0x65, 0xc0, 0x51, 0x0a, 0x9e, 0x81,
//    0x11, 0x07, 0xfd, 0x01, 0x63, 0xdd, 0x89, 0xcd, 0x74,
//    0x2a, 0x00, 0x81, 0x87, 0xa4, 0x6f, 0x0c, 0xfa, 0x20, 0xeb, 0x90, 0x24, 0xb7, 0x1d, 0xd8, 0x00, 0x00, 0x2d,
//    0x67, 0x2b, 0x17, 0x78, 0x6d, 0xb7, 0x91, 0xc0, 0xdc,
//    0xa1, 0x0d, 0x89, 0xd2, 0x2c, 0xcd, 0x89, 0xdd, 0x7c,
//    0xc1, 0x00, 0xc9, 0x1b, 0xc3, 0xf4, 0xe2, 0xc0, 0xc7, 0x75, 0x62, 0xab, 0x18, 0xe1, 0xc3, 0xb3, 0x00, 0x01,
//    0x23, 0x00, 0x3c, 0xa8, 0xdb, 0x51, 0xf9, 0x95, 0xc8, 0x22, 0xe0, 0xbc, 0x65, 0xc0, 0x51, 0x0a, 0x9e, 0x81,
//    0x11, 0x07, 0xfd, 0x01, 0x63, 0xdd, 0x89, 0xcd, 0x74,
//    0x2a, 0x00, 0x81, 0x87, 0xa4, 0x6f, 0x0c, 0xfa, 0x20, 0xeb, 0x90, 0x24, 0xb7, 0x1d, 0xd8, 0x00, 0x00, 0x2d,
//    0x67, 0x2b, 0x17, 0x78, 0x6d, 0xb7, 0x91, 0xc0, 0xdc,
//    0xa1, 0x0d, 0x89, 0xd2, 0x2c, 0xcd, 0x89, 0xdd, 0x7c,
//    0xc1, 0x00, 0xc9, 0x1b, 0xc3, 0xf4, 0xe2, 0xc0, 0xc7, 0x75, 0x62, 0xab, 0x18, 0xe1, 0xc3, 0xb3
//};


AT(.text.src_proc)
void src0_audio_input(u8 *ptr, u32 samples, u32 params)
{
    void *obuf = src_pcm_obuf;

    samples = src_frame_resample(0, (short *)ptr, obuf, samples);


#if SRC_INFO_PRINT
    static u32 ticks = 0;
    static u32 isr_cnt = 0;
    isr_cnt++;
    if(tick_check_expire(ticks,1000)) {
        my_printf(src_info, FRAME_LEN, isr_cnt, FRAME_LEN*isr_cnt);
        isr_cnt = 0;
        ticks = tick_get();
        my_print_r(obuf,20);
    }
#endif

#if UARTDUMP_SRC_EN
    s16 *ptr_temp = (s16 *) UartDump_buf_temp;
    dump_dma_wait();
    dump_put2ram(&UartDump_buf[0][0],obuf,FRAME_LEN*2,0); //算法前数据组包

    dump_put2ram(&UartDump_buf[1][0],ptr_temp,FRAME_LEN*2,1);//算法后数据组包
    dump_dma_kick(&UartDump_buf[0][0], (FRAME_LEN*2 + 14) * 2); //最后再一起吐数据，避免前后kick数据打架
#endif

    if (src_cfg.callback) {
        src_cfg.callback((u8 *)obuf, samples, params);
    }
}

AT(.text.src_proc)
void src0_audio_mute(u8 sta)
{
    src_cfg.mute = sta;
}

AT(.text.src_proc)
void src0_audio_output_callback_set(audio_callback_t callback)
{
    src_cfg.callback = callback;
}
