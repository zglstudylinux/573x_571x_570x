#include "include.h"
#include "api_alg.h"
#include "dnn_L1.h"

/*
 * 文件名称: dnn_L1_api.c
 * 功能描述: 本文件为软件dnn_L1处理模块
    AT(.buf.dnn_L1);
    AT(.rodata.dnn_L1)
    AT(.text.dnn_L1_proc)
    AT(.text.dnn_L1_init)

 ****************************************************************************************
    code + rodata :
    buf           :
    time          : JOY{160M}: 3.59ms/5ms
 */

#if DNN_L1_EN

#define UARTDUMP_DNN_EN          0                         //DUMP数据一键配置，需要使用定制上位机
#define DNN_L1_INFO_PRINT        0
#define FRAME_LEN		         240                       //算法处理帧长
#define PROCESS_OUT_SAMPLES      120                       //每次存取帧长
s16 dnn_L1_pingpangcache_temp[FRAME_LEN*2] AT(.buf.dnn_L1);//乒乓buf缓存
u8 dnn_L1_out_buf[PROCESS_OUT_SAMPLES*2] AT(.buf.dnn_L1);  //输出buf中转缓存
WEAK volatile u8 dnn_L1_flag = 0;                          //乒乓buf 切换标志位
uint8_t dnn_L1_in_cnt = 0;                                 //写指针
uint8_t dnn_L1_out_cnt = 0;                                //读指针

static dnn_L1_cb_t dnn_L1_cb AT(.buf.dnn_L1);
static dnn_L1_mic_cfg_t dnn_L1_mic_cfg AT(.buf.dnn_L1);

u8 dnn_L1_input_temp[PROCESS_OUT_SAMPLES*2] AT(.buf.dnn_L1);  //麦数据组包
u8 dnn_L1_output_temp[PROCESS_OUT_SAMPLES*2] AT(.buf.dnn_L1); //麦数据组包
const rbuf_tbl_t dnn_L1_buf_tbl[1] = {
    {
        .buf = dnn_L1_input_temp,
        .size = PROCESS_OUT_SAMPLES*2,
    },
};

#if DNN_L1_INFO_PRINT
AT(.com_text.dnn_L1)
const char dnn_L1_info[] = "DNN_L1 samples = %d, isrcnt = %d (SR_%d)\n";
#endif

#if UARTDUMP_DNN_EN
static u8 dump_head_buf[14*2]AT(.buf.dnn_L1);
static u8 UartDump_buf[2][14 + FRAME_LEN*2]AT(.buf.dnn_L1);
static u8 UartDump_buf_temp[14 + FRAME_LEN*2]AT(.buf.dnn_L1);
#endif

AT(.text.dnn_L1_proc)
void dnn_L1_mic_audio_input(u8 *ptr, u32 samples, u32 params)
{
    if(!dnn_L1_mic_cfg.mute && wireless_get_status()) {
        ring_buf_put((void *)&dnn_L1_mic_cfg.dnn_L1_ring_buf, ptr, samples*2);
        if(ring_buf_get((void *)&dnn_L1_mic_cfg.dnn_L1_ring_buf, dnn_L1_output_temp, PROCESS_OUT_SAMPLES*2)) {
            samples = PROCESS_OUT_SAMPLES;
            uint8_t process_num = samples/PROCESS_OUT_SAMPLES;
            s16 *process_pcm_ptr = (s16 *)ptr;
            if(process_num*PROCESS_OUT_SAMPLES != samples) {
                printf("dnn_L1 input samples err\n");
                return;
            }
            while(process_num) {
                ///OUT取 PING-PONG BUFFER
                memcpy(dnn_L1_out_buf,&dnn_L1_pingpangcache_temp[dnn_L1_out_cnt * PROCESS_OUT_SAMPLES],PROCESS_OUT_SAMPLES*2);
                dnn_L1_out_cnt++;
                if(dnn_L1_out_cnt>=FRAME_LEN*2/PROCESS_OUT_SAMPLES) {
                    dnn_L1_out_cnt = 0;
                }
                ///IN存 PING-PONG BUFFER
                memcpy(&dnn_L1_pingpangcache_temp[dnn_L1_in_cnt * PROCESS_OUT_SAMPLES], process_pcm_ptr, PROCESS_OUT_SAMPLES * 2);
                dnn_L1_in_cnt++;
                ///kick 低优先级线程处理
                if(dnn_L1_in_cnt == FRAME_LEN/PROCESS_OUT_SAMPLES) {
                    dnn_L1_flag = 1;
                    ///发射端处理kick 低现场 ，接收端处理攒够点数直接处理
                    dnn_L1_mic_proc_kick_start();
                } else if(dnn_L1_in_cnt >= FRAME_LEN*2/PROCESS_OUT_SAMPLES) {
                    dnn_L1_in_cnt = 0;
                    dnn_L1_flag = 2;
                    ///发射端处理kick 低现场 ，接收端处理攒够点数直接处理
                    dnn_L1_mic_proc_kick_start();
                }
                memcpy(process_pcm_ptr,dnn_L1_out_buf,PROCESS_OUT_SAMPLES*2);
                /// break while()
                process_num--;

                if(dnn_L1_mic_cfg.callback) {
                    dnn_L1_mic_cfg.callback((void *)ptr, samples, params);
                }
            }
        } else {
            return;
        }
    } else {
        if(dnn_L1_mic_cfg.callback) {
            dnn_L1_mic_cfg.callback((void *)ptr, samples, params);
        }
    }

}

//dnn_L1算法启动计算 放在低优先级现场处理
AT(.text.dnn_L1_proc)
void dnn_L1_mic_proc_cb(void)
{
    s16 *ptr = &dnn_L1_pingpangcache_temp[0];
    if(dnn_L1_flag == 2) {
        ptr = &dnn_L1_pingpangcache_temp[FRAME_LEN];
    }

#if DNN_L1_INFO_PRINT
    static u32 ticks = 0;
    static u32 isr_cnt = 0;
    isr_cnt++;
    if (tick_check_expire(ticks,1000)) {
        my_printf(dnn_L1_info, FRAME_LEN, isr_cnt, FRAME_LEN*isr_cnt);
        isr_cnt = 0;
        ticks = tick_get();
    }
#endif
#if UARTDUMP_DNN_EN
    s16 *ptr_temp = (s16 *) UartDump_buf_temp;
    dump_dma_wait();
    dump_put2ram(&UartDump_buf[0][0],ptr,FRAME_LEN*2,0); //算法前数据组包

    dump_put2ram(&UartDump_buf[1][0],ptr_temp,FRAME_LEN*2,1);//算法后数据组包
    dump_dma_kick(&UartDump_buf[0][0], (FRAME_LEN*2 + 14) * 2); //最后再一起吐数据，避免前后kick数据打架
#endif

    dnn_L1_process(ptr, NULL, 0);

#if UARTDUMP_DNN_EN
    for(int i = 0; i < FRAME_LEN; i++) {
        ptr_temp[i]  = ptr[i];
    }
#endif // UARTDUMP_DNN_EN
    dnn_L1_flag = 0;

}

AT(.text.dnn_L1_set)
void dnn_L1_mic_output_callback_set(audio_callback_t callback)
{
    dnn_L1_mic_cfg.callback = callback;
}

AT(.text.dnn_L1_init)
void dnn_L1_mic_init(u8 sample_rate, u16 samples, u8 channel)
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

    dump_buf_init(dump_head_buf, 2, (void *)huart_tx, huart_wait_txdone);

#endif

    memset((uint8_t *)&dnn_L1_cb, 0, sizeof(dnn_L1_cb));
    memset((uint8_t *)&dnn_L1_mic_cfg, 0, sizeof(dnn_L1_mic_cfg));

    memset(dnn_L1_pingpangcache_temp, 0, FRAME_LEN*2*2);
    memset(dnn_L1_out_buf, 0, PROCESS_OUT_SAMPLES*2);

    dnn_L1_cb.param_printf           = 1;

	dnn_L1_cb.noise_ps_rate          = 1;
	dnn_L1_cb.prior_opt_idx	         = 10;
	dnn_L1_cb.prior_opt_ada_en	     = 1;
	dnn_L1_cb.wind_level		     = 2;
	dnn_L1_cb.wind_range		     = 0;
	dnn_L1_cb.low_fre_range          = 16; //
	dnn_L1_cb.low_fre_range0         = 0;
	//dnn_L1_cb.pitch_filter_en		   = 1;
	dnn_L1_cb.mask_floor		     = 0;
	//dnn_L1_cb.mask_floor_r		   = 0;
	dnn_L1_cb.music_lev			     = 16;
	dnn_L1_cb.comforN_level		     = 1;
	dnn_L1_cb.gain_expand			 = 1024;
	dnn_L1_cb.nn_only				 = 0;
	dnn_L1_cb.nn_only_len		     = 3;
	//dnn_L1_cb.noise_db2			   = -15;
	dnn_L1_cb.smooth_en			     = 1;
	dnn_L1_cb.gain_assign		     = 32767;
	dnn_L1_cb.enlarge_v			     = 1;
	dnn_L1_cb.sin_gain_post_len	     = 80;
	dnn_L1_cb.spp_thr				 = 8000;
	dnn_L1_cb.prior_sum_v			 = 0;
	dnn_L1_cb.noise_db			     = -450;
	dnn_L1_cb.noise_db2			     = -100;
	dnn_L1_cb.noise_db3		         = -26-200;
	//dnn_L1_cb.noise_db4		       = -420;
	dnn_L1_cb.vad_floor_en		     = 0;

    dnn_L1_init(&dnn_L1_cb);

    ring_buf_init((void *)&dnn_L1_mic_cfg.dnn_L1_ring_buf, &dnn_L1_buf_tbl[0], 1, 0);

}

AT(.text.dnn_L1_exit)
void dnn_L1_mic_exit(void)
{

}

AT(.text.dnn_L1_set.param)
void dnn_L1_mic_param_set(int8_t dnn_L1_nt)
{

}

AT(.text.dnn_L1_set.mute)
void dnn_L1_mic_mute_set(uint8_t mute)
{
    dnn_L1_mic_cfg.mute = mute;
}

AT(.text.dnn_L1_get.mute)
uint8_t dnn_L1_mic_mute_get(void)
{
    return dnn_L1_mic_cfg.mute;
}
#else
void dnn_L1_mic_proc_cb(void){}
#endif
