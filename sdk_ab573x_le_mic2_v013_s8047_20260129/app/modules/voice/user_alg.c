#include "include.h"
#include "api_alg.h"
#include "user_alg.h"

/*
 * 文件名称: user.c
 * 功能描述: 本文件为软件dnn_L3处理模块
AT(.buf.user);
AT(.rodata.user)
AT(.text.user_proc)
AT(.text.user_init)

 ****************************************************************************************
    code + rodata :
    buf           :
    time +encode         :
 */


#if USER_ALG_EN

#define UARTDUMP_USER_EN             0                           //DUMP数据一键配置，需要使用定制上位机
#define USER_INFO_PRINT              0
#define FRAME_LEN                   120                         //算法处理帧长
#define PROCESS_OUT_SAMPLES         120                         //每次存取帧长

static struct tog_bug_tag user_tbuf AT(.buf.user);          //乒乓buf控制
static s16 user_cache_buf[FRAME_LEN*2] AT(.buf.user);       //乒乓buf缓存
static u8 user_tmp_buf[PROCESS_OUT_SAMPLES*2] AT(.buf.user);    //输出buf中转缓存
static void *user_proc_ptr = user_cache_buf;

static user_mic_cfg_t user_mic_cfg AT(.buf.user);

#if USER_INFO_PRINT
AT(.com_text.user)
const char user_info[] = "DNN samples = %d, isrcnt = %d (SR_%d)\n";
#endif

#if UARTDUMP_USER_EN
static u8 dump_head_buf[14*2]AT(.buf.user);

static u8 UartDump_buf[2][14 + FRAME_LEN*2]AT(.buf.user);
static u8 UartDump_buf_temp[14 + FRAME_LEN*2]AT(.buf.user);
extern u8 eq_rx_buf[];
#endif

AT(.text.user_proc)
void user_mic_audio_input(u8 *ptr, u32 samples, u32 params)
{
    if (!user_mic_cfg.mute && wireless_get_status()) {
        while(samples > 0) {
            uint rlen = (samples > PROCESS_OUT_SAMPLES)? PROCESS_OUT_SAMPLES : samples;
            if(tog_buf_get(&user_tbuf, user_tmp_buf, rlen*2)) {
                tog_buf_rd_toggle(&user_tbuf);
            }

            // 填充block, true表示算法攒帧完毕, kick低优先级线程处理算法
            if(tog_buf_put(&user_tbuf, ptr, rlen*2)) {
                user_proc_ptr = tog_bug_get_w_block(&user_tbuf);
                tog_buf_wr_toggle(&user_tbuf);
                user_mic_proc_kick_start();
            }

            memcpy(ptr, user_tmp_buf, rlen*2);
            if(user_mic_cfg.callback) {
                user_mic_cfg.callback((void *)ptr, rlen, params);
            }

            samples -= rlen;
        }
    } else {
        if(user_mic_cfg.callback) {
            user_mic_cfg.callback((void *)ptr, samples, params);
        }
    }

}

//user算法启动计算 放在低优先级现场处理
AT(.text.user_proc)
void user_mic_proc_cb(void)
{
    s16 *ptr = user_proc_ptr;

#if UARTDUMP_USER_EN
    s16 *ptr_temp = (s16 *) UartDump_buf_temp;
    dump_dma_wait();
    dump_put2ram(&UartDump_buf[0][0],ptr,FRAME_LEN*2,0); //算法前数据组包

    dump_put2ram(&UartDump_buf[1][0],ptr_temp,FRAME_LEN*2,1);//算法后数据组包
    dump_dma_kick(&UartDump_buf[0][0], (FRAME_LEN*2 + 14) * 2); //最后再一起吐数据，避免前后kick数据打架
#endif

#if USER_INFO_PRINT
    static u32 ticks = 0;
    static u32 isr_cnt = 0;
    isr_cnt++;
    if(tick_check_expire(ticks,1000)) {
        my_printf(user_info, FRAME_LEN, isr_cnt, FRAME_LEN*isr_cnt);
        isr_cnt = 0;
        ticks = tick_get();
    }
#endif
    ///********用户算法流程**********///
//    dnn_l3_process(ptr);
    ///******************************///
#if UARTDUMP_USER_EN
    for(int i = 0; i < FRAME_LEN; i++) {
        ptr_temp[i]  = ptr[i];
    }
#endif // UARTDUMP_USER_EN

}

AT(.text.user_set)
void user_mic_output_callback_set(audio_callback_t callback)
{
    user_mic_cfg.callback = callback;
}

AT(.text.user_init)
void user_mic_init(u8 sample_rate, u16 samples, u8 channel)
{
#if UARTDUMP_USER_EN
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

    ///*********用户算法初始化**********///
//    user_init();
    ///*********************************///

    tog_buf_init(&user_tbuf, user_cache_buf, FRAME_LEN*2);
    memset((uint8_t *)&user_mic_cfg, 0, sizeof(user_mic_cfg));
}

AT(.text.user_exit)
void user_mic_exit(void)
{

}

AT(.text.user_set.mute)
void user_mic_mute_set(uint8_t mute)
{
    if (!mute) {
        if (wireless_role_is_adapter()) {
            sys_clk_req(INDEX_WL_ALG_EN, ADAPTER_ALG_EN_CLK);
        } else {
            sys_clk_req(INDEX_WL_ALG_EN, DEVICE_ALG_EN_CLK);
        }
    }

    user_mic_cfg.mute = mute;

    if (mute) {
        sys_clk_free(INDEX_WL_ALG_EN);
        tog_buf_init(&user_tbuf, user_cache_buf, FRAME_LEN*2);
    }
}

AT(.text.user_get.mute)
uint8_t user_mic_mute_get(void)
{
    return user_mic_cfg.mute;
}
#else
void user_mic_proc_cb(void){}
#endif
