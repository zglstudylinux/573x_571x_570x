#include "include.h"
#include "api_alg.h"
#include "dnn_L2.h"

/*
 * 文件名称: dnn_L2.c
 * 功能描述: 本文件为软件dnn_L3处理模块
AT(.buf.dnn_L2);
AT(.rodata.dnn_L2)
AT(.text.dnn_L2_proc)
AT(.text.dnn_L2_init)

 ****************************************************************************************
    code + rodata : 5K
    buf           : 16K
    time          : 2.5ms
 */

#if DNN_L2_EN

#define UARTDUMP_DNN_EN             0                           //DUMP数据一键配置，需要使用定制上位机
#define DNN_L2_INFO_PRINT           0
#define FRAME_LEN                   720                         //算法处理帧长
#define PROCESS_OUT_SAMPLES         120                         //每次存取帧长

static struct tog_bug_tag dnn_l2_tbuf AT(.buf.dnn_L2);          //乒乓buf控制
static s16 dnn_l2_cache_buf[FRAME_LEN*2] AT(.buf.dnn_L2);       //乒乓buf缓存
static u8 dnn_l2_tmp_buf[PROCESS_OUT_SAMPLES*2] AT(.buf.dnn_L2);    //输出buf中转缓存
static void *dnn_l2_proc_ptr = dnn_l2_cache_buf;

static dnn_l2_cb_t dnn_l2_cb AT(.buf.dnn_L2);
static dnn_L2_mic_cfg_t dnn_L2_mic_cfg AT(.buf.dnn_L2);

#if DNN_L2_INFO_PRINT
AT(.com_text.dnn_l2)
const char dnn_L2_info[] = "DNN samples = %d, isrcnt = %d (SR_%d)\n";
#endif

#if UARTDUMP_DNN_EN
static u8 dump_head_buf[14*2]AT(.buf.dnn_L2);

static u8 UartDump_buf[2][14 + FRAME_LEN*2]AT(.buf.dnn_L2);
static u8 UartDump_buf_temp[14 + FRAME_LEN*2]AT(.buf.dnn_L2);
extern u8 eq_rx_buf[];
#endif

AT(.text.dnn_L2_proc)
void dnn_L2_mic_audio_input(u8 *ptr, u32 samples, u32 params)
{
    if (!dnn_L2_mic_cfg.mute && wireless_get_status()) {
        while(samples > 0) {
            uint rlen = (samples > PROCESS_OUT_SAMPLES)? PROCESS_OUT_SAMPLES : samples;
            if(tog_buf_get(&dnn_l2_tbuf, dnn_l2_tmp_buf, rlen*2)) {
                tog_buf_rd_toggle(&dnn_l2_tbuf);
            }

            // 填充block, true表示算法攒帧完毕, kick低优先级线程处理算法
            if(tog_buf_put(&dnn_l2_tbuf, ptr, rlen*2)) {
                dnn_l2_proc_ptr = tog_bug_get_w_block(&dnn_l2_tbuf);
                tog_buf_wr_toggle(&dnn_l2_tbuf);
                dnn_L2_mic_proc_kick_start();
            }

            memcpy(ptr, dnn_l2_tmp_buf, rlen*2);
            if(dnn_L2_mic_cfg.callback) {
                dnn_L2_mic_cfg.callback((void *)ptr, rlen, params);
            }

            samples -= rlen;
        }
    } else {
        if(dnn_L2_mic_cfg.callback) {
            dnn_L2_mic_cfg.callback((void *)ptr, samples, params);
        }
    }

}

//dnn_L2算法启动计算 放在低优先级现场处理
AT(.text.dnn_L2_proc)
void dnn_L2_mic_proc_cb(void)
{
    s16 *ptr = dnn_l2_proc_ptr;

#if UARTDUMP_DNN_EN
    s16 *ptr_temp = (s16 *) UartDump_buf_temp;
    dump_dma_wait();
    dump_put2ram(&UartDump_buf[0][0],ptr,FRAME_LEN*2,0); //算法前数据组包

    dump_put2ram(&UartDump_buf[1][0],ptr_temp,FRAME_LEN*2,1);//算法后数据组包
    dump_dma_kick(&UartDump_buf[0][0], (FRAME_LEN*2 + 14) * 2); //最后再一起吐数据，避免前后kick数据打架
#endif

#if DNN_L2_INFO_PRINT
    static u32 ticks = 0;
    static u32 isr_cnt = 0;
    isr_cnt++;
    if(tick_check_expire(ticks,1000)) {
        my_printf(dnn_L2_info, FRAME_LEN, isr_cnt, FRAME_LEN*isr_cnt);
        isr_cnt = 0;
        ticks = tick_get();
    }
#endif

    dnn_l2_process(ptr);

#if UARTDUMP_DNN_EN
    for(int i = 0; i < FRAME_LEN; i++) {
        ptr_temp[i]  = ptr[i];
    }
#endif // UARTDUMP_DNN_EN

}

AT(.text.dnn_L2_set)
void dnn_L2_mic_output_callback_set(audio_callback_t callback)
{
    dnn_L2_mic_cfg.callback = callback;
}

AT(.text.dnn_L2_set.param)
void dnn_L2_mic_param_set_do(s16 dnn_L2_nt)
{
    dnn_l2_cb.param_printf          = 1;
    //dnn_l2_cb.nt                    = 6;
    dnn_l2_cb.denoiseBound          = dnn_L2_nt;

    dnn_l2_cb.noise_ps_rate         = 1;
    dnn_l2_cb.prior_opt_idx         = 10;
    dnn_l2_cb.prior_opt_ada_en      = 1;

    dnn_l2_cb.low_fre_range         = 15; //
    dnn_l2_cb.music_lev             = 11;

    dnn_l2_cb.nn_only               = 0;
    dnn_l2_cb.nn_only_len           = 16;
    dnn_l2_cb.gain_assign           = 26666;
    dnn_l2_cb.sin_gain_post_en      = 0;
    dnn_l2_cb.sin_gain_post_len     = 0;
    dnn_l2_cb.sin_gain_post_len_f   = 256;

    dnn_l2_cb.smooth_en             = 1;
    dnn_l2_cb.ap_test_en            = 0;
    dnn_l2_cb.hi_gain_len           = 64;
    dnn_l2_cb.sp_thres              = 6000;
}

AT(.text.dnn_L2_init)
void dnn_L2_mic_init(u8 sample_rate, u16 samples, u8 channel)
{
#if UARTDUMP_DNN_EN
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

    memset((uint8_t *)&dnn_l2_cb, 0, sizeof(dnn_l2_cb));
    dnn_L2_mic_param_set_do(DNN_L2_LEVEL);
    dnn_l2_init(&dnn_l2_cb);

    tog_buf_init(&dnn_l2_tbuf, dnn_l2_cache_buf, FRAME_LEN*2);
    memset((uint8_t *)&dnn_L2_mic_cfg, 0, sizeof(dnn_L2_mic_cfg));
}

AT(.text.dnn_L2_exit)
void dnn_L2_mic_exit(void)
{

}

AT(.text.dnn_L2_set.param)
void dnn_L2_mic_param_set(s16 dnn_L2_nt)
{
    dnn_L2_mic_param_set_do(dnn_L2_nt);
    dnn_l2_init(&dnn_l2_cb);
}

AT(.text.dnn_L2_set.mute)
void dnn_L2_mic_mute_set(uint8_t mute)
{
    if (!mute) {
        if (wireless_role_is_adapter()) {
            sys_clk_req(INDEX_WL_ALG_EN, ADAPTER_ALG_EN_CLK);
        } else {
            sys_clk_req(INDEX_WL_ALG_EN, DEVICE_ALG_EN_CLK);
        }
    }

    dnn_L2_mic_cfg.mute = mute;

    if (mute) {
        sys_clk_free(INDEX_WL_ALG_EN);
        tog_buf_init(&dnn_l2_tbuf, dnn_l2_cache_buf, FRAME_LEN*2);
    }
}

AT(.text.dnn_L2_get.mute)
uint8_t dnn_L2_mic_mute_get(void)
{
    return dnn_L2_mic_cfg.mute;
}
#else
void dnn_L2_mic_proc_cb(void){}
#endif
