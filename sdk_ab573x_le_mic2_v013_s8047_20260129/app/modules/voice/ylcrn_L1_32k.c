#include "include.h"
#include "api_alg.h"
#include "ylcrn_L1_32k.h"

/*
 * 文件名称: ylcrn_L1_32k.c
 * 功能描述: 本文件为软件ylcrn_L1_32k处理模块
AT(.buf.ylcrn_L1_32k);
AT(.rodata.ylcrn_L1_32k)
AT(.text.ylcrn_L1_32k_proc)
AT(.text.ylcrn_L1_32k_init)

 ****************************************************************************************
    code + rodata : 42.6K
    buf           : 34K
    time         : 15ms/20ms   （src+enc+nr）
 */


#if YLCRN_L1_32K_EN

#define UARTDUMP_YLCRN_L1_32K_EN      0                           //DUMP数据一键配置，需要使用定制上位机
#define YLCRN_L1_32K_RDFT_HW          0                         //是否使用硬件FFT处理,软件fft底层处理若是关闭则无法编译通过 (此算法不使用硬件fft)
#define YLCRN_L1_32K_INFO_PRINT       0
#define FRAME_LEN                     480                         //算法处理帧长
#define PROCESS_OUT_SAMPLES           80                         //每次存取帧长

static struct tog_bug_tag ylcrn_L1_32k_tbuf AT(.buf.ylcrn_L1_32k);          //乒乓buf控制
static s16 ylcrn_L1_32k_cache_buf[FRAME_LEN*2] AT(.buf.ylcrn_L1_32k);       //乒乓buf缓存
static u8 ylcrn_L1_32k_tmp_buf[PROCESS_OUT_SAMPLES*2] AT(.buf.ylcrn_L1_32k);    //输出buf中转缓存
static void *ylcrn_L1_32k_proc_ptr = ylcrn_L1_32k_cache_buf;

static ylcrn_L1_32k_cb_t ylcrn_L1_32k_cb AT(.buf.ylcrn_L1_32k);
static ylcrn_L1_32k_mic_cfg_t ylcrn_L1_32k_mic_cfg AT(.buf.ylcrn_L1_32k);

#if YLCRN_L1_32K_RDFT_HW
typedef struct{
    fft_cfg_t fft_cft;
    ifft_cfg_t ifft_cft;
    s32 ylcrn_L1_32k_fft_in[1024];
    s32 ylcrn_L1_32k_fft_out[1024];
    s32 ylcrn_L1_32k_ifft_out[1024];
}ylcrn_L1_32k_rdft_t;
ylcrn_L1_32k_rdft_t ylcrn_L1_32k_rdft AT(.buf.ylcrn_L1_32k_test);
#endif

#if YLCRN_L1_32K_INFO_PRINT
AT(.com_text.ylcrn_L1_32k)
const char ylcrn_L1_32k_info[] = "YLCRN L1 32K samples = %d, isrcnt = %d (SR_%d)\n";
#endif

#if UARTDUMP_YLCRN_L1_32K_EN
static u8 dump_head_buf[14*2]AT(.buf.ylcrn_L1_32k);

static u8 UartDump_buf[2][14 + FRAME_LEN*2]AT(.buf.ylcrn_L1_32k);
static u8 UartDump_buf_temp[14 + FRAME_LEN*2]AT(.buf.ylcrn_L1_32k);
extern u8 eq_rx_buf[];
#endif

AT(.text.ylcrn_L1_32k_proc)
void ylcrn_L1_32k_mic_audio_input(u8 *ptr, u32 samples, u32 params)
{
    if (!ylcrn_L1_32k_mic_cfg.mute && wireless_get_status()) {
        while(samples > 0) {
            uint rlen = (samples > PROCESS_OUT_SAMPLES)? PROCESS_OUT_SAMPLES : samples;
            if(tog_buf_get(&ylcrn_L1_32k_tbuf, ylcrn_L1_32k_tmp_buf, rlen*2)) {
                tog_buf_rd_toggle(&ylcrn_L1_32k_tbuf);
            }

            // 填充block, true表示算法攒帧完毕, kick低优先级线程处理算法
            if(tog_buf_put(&ylcrn_L1_32k_tbuf, ptr, rlen*2)) {
                ylcrn_L1_32k_proc_ptr = tog_bug_get_w_block(&ylcrn_L1_32k_tbuf);
                tog_buf_wr_toggle(&ylcrn_L1_32k_tbuf);
                ylcrn_L1_32k_mic_proc_kick_start();
            }

            memcpy(ptr, ylcrn_L1_32k_tmp_buf, rlen*2);
            if(ylcrn_L1_32k_mic_cfg.callback) {
                ylcrn_L1_32k_mic_cfg.callback((void *)ptr, rlen, params);
            }

            samples -= rlen;
        }
    } else {
        if(ylcrn_L1_32k_mic_cfg.callback) {
            ylcrn_L1_32k_mic_cfg.callback((void *)ptr, samples, params);
        }
    }

}
void fft_test(void);
//ylcrn_L1_32k算法启动计算 放在低优先级现场处理
AT(.text.ylcrn_L1_32k_proc)
void ylcrn_L1_32k_mic_proc_cb(void)
{
    s16 *ptr = ylcrn_L1_32k_proc_ptr;

#if UARTDUMP_YLCRN_L1_32K_EN
    s16 *ptr_temp = (s16 *) UartDump_buf_temp;
    dump_dma_wait();
    dump_put2ram(&UartDump_buf[0][0],ptr,FRAME_LEN*2,0); //算法前数据组包

    dump_put2ram(&UartDump_buf[1][0],ptr_temp,FRAME_LEN*2,1);//算法后数据组包
    dump_dma_kick(&UartDump_buf[0][0], (FRAME_LEN*2 + 14) * 2); //最后再一起吐数据，避免前后kick数据打架
#endif

#if YLCRN_L1_32K_INFO_PRINT
    static u32 ticks = 0;
    static u32 isr_cnt = 0;
    isr_cnt++;
    if(tick_check_expire(ticks,1000)) {
        my_printf(ylcrn_L1_32k_info, FRAME_LEN, isr_cnt, FRAME_LEN*isr_cnt);
        isr_cnt = 0;
        ticks = tick_get();
    }
#endif
#if YLCRN_L1_32K_RDFT_HW

//    for(u8 j=0; j<480;j++){
//        ylcrn_L1_32k_rdft.ylcrn_L1_32k_fft_in[j] = ptr[j];
//    }
//    ylcrn_L1_32k_process_ext_window(ptr,NULL,0,ylcrn_L1_32k_rdft.ylcrn_L1_32k_fft_in);
//    fft_hw(&ylcrn_L1_32k_rdft.fft_cft);

    ylcrn_L1_32k_ns_process((s16 *)ylcrn_L1_32k_rdft.ylcrn_L1_32k_ifft_out);
//    ifft_hw(&ylcrn_L1_32k_rdft.ifft_cft);
//    dnn_process_ext_next(ylcrn_L1_32k_rdft.ylcrn_L1_32k_ifft_out,NULL,0,ptr);
//    for(u8 k=0; k<480;k++){
//        ptr[k] = ylcrn_L1_32k_rdft.ylcrn_L1_32k_ifft_out[k];
//    }
//        fft_test();
#else
//    GPIOBSET |= BIT(1);
    ylcrn_L1_32k_ns_process(ptr);
//    GPIOBCLR |= BIT(1);
#endif  //YLCRN_L1_32K_RDFT_HW



#if UARTDUMP_YLCRN_L1_32K_EN
    for(int i = 0; i < FRAME_LEN; i++) {
        ptr_temp[i]  = ptr[i];
    }
#endif // UARTDUMP_YLCRN_L1_32K_EN

}

AT(.text.ylcrn_L1_32k_set)
void ylcrn_L1_32k_mic_output_callback_set(audio_callback_t callback)
{
    ylcrn_L1_32k_mic_cfg.callback = callback;
}

/*
AT(.rodata.ylcrn_L1_32k)
static u8 test_data[640]={
    0x23, 0x00, 0x3c, 0xa8, 0xdb, 0x51, 0xf9, 0x95, 0xc8, 0x22, 0xe0, 0xbc, 0x65, 0xc0, 0x51, 0x0a, 0x9e, 0x81,
    0x11, 0x07, 0xfd, 0x01, 0x63, 0xdd, 0x89, 0xcd, 0x74,
    0x2a, 0x00, 0x81, 0x87, 0xa4, 0x6f, 0x0c, 0xfa, 0x20, 0xeb, 0x90, 0x24, 0xb7, 0x1d, 0xd8, 0x00, 0x00, 0x2d,
    0x67, 0x2b, 0x17, 0x78, 0x6d, 0xb7, 0x91, 0xc0, 0xdc,
    0xa1, 0x0d, 0x89, 0xd2, 0x2c, 0xcd, 0x89, 0xdd, 0x7c,
    0xc1, 0x00, 0xc9, 0x1b, 0xc3, 0xf4, 0xe2, 0xc0, 0xc7, 0x75, 0x62, 0xab, 0x18, 0xe1, 0xc3, 0xb3, 0x00, 0x01,
    0xa1, 0xc7, 0xc6, 0x0e, 0xb4, 0x4d, 0x91, 0xd5, 0x74,
    0xd3, 0x00, 0xb1, 0x2c, 0x0d, 0xd8, 0xaf, 0xaa, 0xd4, 0x2a, 0xb2, 0x5b, 0xfe, 0x15, 0x74, 0x98, 0xc9, 0x00,
    0xd1, 0xa6, 0xd2, 0xa6, 0x7a, 0x4d, 0x89, 0xc9, 0x84,
    0x23, 0x00, 0x3c, 0xa8, 0xdb, 0x51, 0xf9, 0x95, 0xc8, 0x22, 0xe0, 0xbc, 0x65, 0xc0, 0x51, 0x0a, 0x9e, 0x81,
    0x11, 0x07, 0xfd, 0x01, 0x63, 0xdd, 0x89, 0xcd, 0x74,
    0x2a, 0x00, 0x81, 0x87, 0xa4, 0x6f, 0x0c, 0xfa, 0x20, 0xeb, 0x90, 0x24, 0xb7, 0x1d, 0xd8, 0x00, 0x00, 0x2d,
    0x67, 0x2b, 0x17, 0x78, 0x6d, 0xb7, 0x91, 0xc0, 0xdc,
    0xa1, 0x0d, 0x89, 0xd2, 0x2c, 0xcd, 0x89, 0xdd, 0x7c,
    0xc1, 0x00, 0xc9, 0x1b, 0xc3, 0xf4, 0xe2, 0xc0, 0xc7, 0x75, 0x62, 0xab, 0x18, 0xe1, 0xc3, 0xb3, 0x00, 0x01,
    0xa1, 0xc7, 0xc6, 0x0e, 0xb4, 0x4d, 0x91, 0xd5, 0x74,
    0xd3, 0x00, 0xb1, 0x2c, 0x0d, 0xd8, 0xaf, 0xaa, 0xd4, 0x2a, 0xb2, 0x5b, 0xfe, 0x15, 0x74, 0x98, 0xc9, 0x00,
    0xd1, 0xa6, 0xd2, 0xa6, 0x7a, 0x4d, 0x89, 0xc9, 0x84,
    0x80, 0x00, 0xb1, 0x84, 0x25, 0x8f, 0xd7, 0xbe, 0x16, 0x0e, 0x97, 0x20, 0x55, 0x2c, 0x8b, 0xcd, 0x34, 0x80,
    0x55, 0xe8, 0x3b, 0x0c, 0x8f, 0x5d, 0x21, 0xd1, 0x8c,
    0x66, 0x00, 0x85, 0x6d, 0x32, 0x03, 0xed, 0x99, 0xd7, 0x30, 0x1b, 0x49, 0x89, 0x08, 0xf2, 0xb3, 0x8a, 0x0f,
    0x80, 0xb5, 0xfb, 0x3d, 0xf6, 0x4d,
    0x30, 0x00, 0x68, 0x2b, 0x9a, 0x66, 0x75, 0x4c, 0x1b, 0xb1, 0x99, 0x0d, 0xd7, 0xdf, 0x95, 0x38, 0x00, 0x01,
    0xc2, 0xcb, 0x8c, 0x39, 0xf0, 0x3d, 0x91, 0xb9, 0x8c,
    0x11, 0x00, 0xba, 0x9d, 0xc5, 0xdb, 0x11, 0xb1, 0x87, 0x57, 0x17, 0xec, 0x44, 0x63, 0xe8, 0xd0, 0x80, 0x03,
    0x83, 0x75, 0x82, 0x87, 0x02, 0xcd, 0x89, 0xdd, 0x78,
    0xb9, 0x00, 0x7b, 0xee, 0xe3, 0x70, 0x1b, 0xdb, 0x1e, 0x5f, 0x34, 0x39, 0x74, 0x25, 0xf6, 0x2a, 0x46, 0x9e,

    0x80, 0x00, 0xb1, 0x84, 0x25, 0x8f, 0xd7, 0xbe, 0x16, 0x0e, 0x97, 0x20, 0x55, 0x2c, 0x8b, 0xcd, 0x34, 0x80,
    0x55, 0xe8, 0x3b, 0x0c, 0x8f, 0x5d, 0x21, 0xd1, 0x8c,
    0x66, 0x00, 0x85, 0x6d, 0x32, 0x03, 0xed, 0x99, 0xd7, 0x30, 0x1b, 0x49, 0x89, 0x08, 0xf2, 0xb3, 0x8a, 0x0f,
    0x80, 0xb5, 0xfb, 0x3d, 0xf6, 0x4d,
    0x30, 0x00, 0x68, 0x2b, 0x9a, 0x66, 0x75, 0x4c, 0x1b, 0xb1, 0x99, 0x0d, 0xd7, 0xdf, 0x95, 0x38, 0x00, 0x01,
    0xc2, 0xcb, 0x8c, 0x39, 0xf0, 0x3d, 0x91, 0xb9, 0x8c,
    0x11, 0x00, 0xba, 0x9d, 0xc5, 0xdb, 0x11, 0xb1, 0x87, 0x57, 0x17, 0xec, 0x44, 0x63, 0xe8, 0xd0, 0x80, 0x03,
    0x83, 0x75, 0x82, 0x87, 0x02, 0xcd, 0x89, 0xdd, 0x78,
    0xb9, 0x00, 0x7b, 0xee, 0xe3, 0x70, 0x1b, 0xdb, 0x1e, 0x5f, 0x34, 0x39, 0x74, 0x25, 0xf6, 0x2a, 0x46, 0x9e,
    0x80, 0x00, 0xb1, 0x84, 0x25, 0x8f, 0xd7, 0xbe, 0x16, 0x0e, 0x97, 0x20, 0x55, 0x2c, 0x8b, 0xcd, 0x34, 0x80,
    0x55, 0xe8,
    0x80, 0x00, 0xb1, 0x84, 0x25, 0x8f, 0xd7, 0xbe, 0x16, 0x0e, 0x97, 0x20, 0x55, 0x2c, 0x8b, 0xcd, 0x34, 0x80,
    0x55, 0xe8,
    0x30, 0x00, 0x68, 0x2b, 0x9a, 0x66, 0x75, 0x4c, 0x1b, 0xb1, 0x99, 0x0d, 0xd7, 0xdf, 0x95, 0x38, 0x00, 0x01,
    0xc2, 0xcb,
    0x30, 0x00, 0x68, 0x2b, 0x9a, 0x66, 0x75, 0x4c, 0x1b, 0xb1, 0x99, 0x0d, 0xd7, 0xdf, 0x95, 0x38, 0x00, 0x01,
    0xc2, 0xcb,
    0x30, 0x00, 0x68, 0x2b, 0x9a, 0x66, 0x75, 0x4c, 0x1b, 0xb1, 0x99, 0x0d, 0xd7, 0xdf, 0x95, 0x38, 0x00, 0x01,
    0xc2, 0xcb,
    0x30, 0x00, 0x68, 0x2b, 0x9a, 0x66, 0x75, 0x4c, 0x1b, 0xb1, 0x99, 0x0d, 0xd7, 0xdf, 0x95, 0x38, 0x00, 0x01,
    0xc2, 0xcb,
    0x23, 0x00, 0x3c, 0xa8, 0xdb, 0x51, 0xf9, 0x95, 0xc8, 0x22, 0xe0, 0xbc, 0x65, 0xc0, 0x51, 0x0a, 0x9e, 0x81,
    0x11, 0x07,
    0x23, 0x00, 0x3c, 0xa8, 0xdb, 0x51, 0xf9, 0x95, 0xc8, 0x22, 0xe0, 0xbc, 0x65, 0xc0, 0x51, 0x0a, 0x9e, 0x81,
    0x11, 0x07

};
*/

AT(.text.ylcrn_L1_32k_set.param)
void ylcrn_L1_32k_mic_param_set_do(s16 ylcrn_L1_32k_nt)
{
	ylcrn_L1_32k_cb.param_printf		= 1;
	ylcrn_L1_32k_cb.overdrive			= 32768;

	ylcrn_L1_32k_cb.denoiseBound		= ylcrn_L1_32k_nt;      ///降噪等级，1~32768，越小压越狠
	ylcrn_L1_32k_cb.denoiseBound_wind   = 3000;

	ylcrn_L1_32k_cb.smooth_en			= 1;
	ylcrn_L1_32k_cb.modelUpdatePars0	= 2;
	ylcrn_L1_32k_cb.prior_opt_idx		= 10;
	ylcrn_L1_32k_cb.prior_opt_ada_en	= 1;

	ylcrn_L1_32k_cb.sin_dnn_en			= 1;
	ylcrn_L1_32k_cb.sin_dnn_wind_en		= 1;
	ylcrn_L1_32k_cb.sin_all_en			= 0;
	ylcrn_L1_32k_cb.sin_all_len			= 0;

	ylcrn_L1_32k_cb.low_noise_range		= 16;
	ylcrn_L1_32k_cb.spp_fre_p			= 6000;
	ylcrn_L1_32k_cb.spp_fre_p_wind		= 3000;
	ylcrn_L1_32k_cb.spp_fre_len			= 16;
	//ylcrn_L1_32k_cb.high_gain_len       = 32;
	ylcrn_L1_32k_cb.music_lev			= 16;
	ylcrn_L1_32k_cb.prev_noise_len		= 11;
	ylcrn_L1_32k_cb.gain_assign			= 26666;
	ylcrn_L1_32k_cb.gain_assign_wind	= 16666;
	ylcrn_L1_32k_cb.hi_gain_mode		= 0;
	ylcrn_L1_32k_cb.wind_en				= 0;
	ylcrn_L1_32k_cb.spp_dnn_max_en		= 0;
	ylcrn_L1_32k_cb.gain_hard_thres	    = 32767;
	ylcrn_L1_32k_cb.vad_hard_thres	    = 1000;
	ylcrn_L1_32k_cb.vad_en				= 0;
	ylcrn_L1_32k_cb.high_gain_len       = 64;
    ylcrn_L1_32k_ns_init(&ylcrn_L1_32k_cb);
}

AT(.text.ylcrn_L1_32k_proc.init)
void ylcrn_L1_32k_mic_init(u8 sample_rate, u16 samples, u8 channel)
{
#if YLCRN_L1_32K_RDFT_HW


    ylcrn_L1_32k_rdft.fft_cft.size            = RDFT_1024;
	ylcrn_L1_32k_rdft.fft_cft.input_type      = 1;
    ylcrn_L1_32k_rdft.fft_cft.window_en       = 1;
    ylcrn_L1_32k_rdft.fft_cft.isr_en          = 0;
    ylcrn_L1_32k_rdft.fft_cft.in_addr         = ylcrn_L1_32k_rdft.ylcrn_L1_32k_fft_in;
    ylcrn_L1_32k_rdft.fft_cft.out_addr        = ylcrn_L1_32k_rdft.ylcrn_L1_32k_fft_out;

    ylcrn_L1_32k_rdft.ifft_cft.size           = RDFT_1024;
    ylcrn_L1_32k_rdft.ifft_cft.output_type    = 1;
    ylcrn_L1_32k_rdft.ifft_cft.window_en      = 1;
    ylcrn_L1_32k_rdft.ifft_cft.isr_en         = 0;
    ylcrn_L1_32k_rdft.ifft_cft.overlap_en     = 1;
    ylcrn_L1_32k_rdft.ifft_cft.overlap_len    = 0;
    ylcrn_L1_32k_rdft.ifft_cft.in_addr        = ylcrn_L1_32k_rdft.ylcrn_L1_32k_fft_out;
    ylcrn_L1_32k_rdft.ifft_cft.out_addr       = ylcrn_L1_32k_rdft.ylcrn_L1_32k_ifft_out;

    memset(ylcrn_L1_32k_rdft.ylcrn_L1_32k_fft_in, 0, sizeof(ylcrn_L1_32k_rdft.ylcrn_L1_32k_fft_in));
    memset(ylcrn_L1_32k_rdft.ylcrn_L1_32k_fft_out, 0, sizeof(ylcrn_L1_32k_rdft.ylcrn_L1_32k_fft_out));
    memset(ylcrn_L1_32k_rdft.ylcrn_L1_32k_ifft_out, 0, sizeof(ylcrn_L1_32k_rdft.ylcrn_L1_32k_ifft_out));

    rdft_init();

    CLKGAT2 |= BIT(17);
    CLKGAT2 |= BIT(29);
#endif

//    math_init();
#if UARTDUMP_YLCRN_L1_32K_EN
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
    ylcrn_L1_32k_mic_mute_set(0);
    memset((uint8_t *)&ylcrn_L1_32k_cb, 0, sizeof(ylcrn_L1_32k_cb));
    ylcrn_L1_32k_mic_param_set_do(YLCRN_L1_32K_LEVEL);
//    ylcrn_L1_32k_ns_init(&ylcrn_L1_32k_cb);

#if 0
    u32 j=0;
     memset(ylcrn_L1_32k_cache_buf,0,FRAME_LEN*2*2);
//     print_r(ylcrn_L1_32k_cache_buf,FRAME_LEN*2);
     memcpy(ylcrn_L1_32k_cache_buf,test_data,480);
//     print_r(ylcrn_L1_32k_cache_buf,FRAME_LEN*2);
//    printf("\n out: \n");
//    for (j=0; j<480; j++)
//	{
//		printf("%d ",ylcrn_L1_32k_cache_buf[j]);
//		if((j%16)==0){
//			printf("\n");
//		}
//
//	}
//    printf("out: \n");
//
//     GPIOACLR = BIT(5);
     ylcrn_L1_32k_ns_process(ylcrn_L1_32k_cache_buf);
//     GPIOASET = BIT(5);
//
     printf("out: \n");
    for (j=0; j<480; j++)
	{
		printf("%d ",ylcrn_L1_32k_cache_buf[j]);
		if((j%16)==0){
			printf("\n");
		}

	}


//     print_r16(ylcrn_L1_32k_cache_buf,FRAME_LEN);
//
//     while(1){
//          WDT_CLR();
//     }
#endif

    tog_buf_init(&ylcrn_L1_32k_tbuf, ylcrn_L1_32k_cache_buf, FRAME_LEN*2);
    memset((uint8_t *)&ylcrn_L1_32k_mic_cfg, 0, sizeof(ylcrn_L1_32k_mic_cfg));

//    GPIOBFEN &= ~BIT(1);
//    GPIOBDE |= BIT(1);
//    GPIOBDIR &= ~BIT(1);
//
//    GPIOBFEN &= ~BIT(2);
//    GPIOBDE |= BIT(2);
//    GPIOBDIR &= ~BIT(2);
}

AT(.text.ylcrn_L1_32k_exit)
void ylcrn_L1_32k_mic_exit(void)
{

}

AT(.text.ylcrn_L1_32k_set.param)
void ylcrn_L1_32k_mic_param_set(s16 ylcrn_L1_32k_nt)
{
    ylcrn_L1_32k_mic_param_set_do(ylcrn_L1_32k_nt);
    ylcrn_L1_32k_ns_init(&ylcrn_L1_32k_cb);
}

AT(.text.ylcrn_L1_32k_set.mute)
void ylcrn_L1_32k_mic_mute_set(uint8_t mute)
{
    if (!mute) {
        if (wireless_role_is_adapter()) {
            sys_clk_req(INDEX_WL_ALG_EN, ADAPTER_ALG_EN_CLK);
        } else {
            sys_clk_req(INDEX_WL_ALG_EN, DEVICE_ALG_EN_CLK);
        }
    }

    ylcrn_L1_32k_mic_cfg.mute = mute;

    if (mute) {
        sys_clk_free(INDEX_WL_ALG_EN);
        tog_buf_init(&ylcrn_L1_32k_tbuf, ylcrn_L1_32k_cache_buf, FRAME_LEN*2);
    }
}

AT(.text.ylcrn_L1_32k_get.mute)
uint8_t ylcrn_L1_32k_mic_mute_get(void)
{
    return ylcrn_L1_32k_mic_cfg.mute;
}
#else
void ylcrn_L1_32k_mic_proc_cb(void){}
#endif
