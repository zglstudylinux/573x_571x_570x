#include "include.h"

#if !VAD_EN
///自定义asr mic buff，如果不使用vad功能，但需要改动mic的samples和地址的话，
///只能在这两个函数修改，bsp_audio.c那边修改无效
//u32 asr_get_adc_buff_addr(void)
//{
//    return DMA_ADR(); //返回地址自定义地址
//}
//
//u32 asr_get_adc_buff_len(void)
//{
//    return 0;        //返回样点数，即上面自定义buf的总长度/2
//}
#else
#define ASR_DUMP_EN                     0       //是否dump mic数据
#define TRACE_EN                        0
#if TRACE_EN
#define TRACE(...)                      printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#define     KWS_SAMPLES                 512             //KWS一帧的样点数，固定值
#define     VAD_NEG_CNT                 0X0F            //非人声阈值，范围0x0000到0X3FFF
#define     VAD_POS_CNT                 0X20            //人声阈值，越小对声音越灵敏，范围0x0000到0X3FFF
#define     KWS_RUNTIME_LIMIT           3000            //语音识别算法运行时间限制，0为不限制，单位 ms

#define     VAD_NEG_FLAG_THRESHOLD      6               //voice_flag转为0的neg_flag_cnt阈值
#define     VAD_REBOOT_THRESOLD         200             //vad reboot的cnt阈值
#define     VAD_DMA_NUM_MAX             12              //VAD DMA 共保存VAD_DMA_NUM_MAX帧mic数据
#define     VAD_PREFETCH_NUM            8               //取VAD起来之前的数据帧数
#define     VAD_DMA_SIZE                512* VAD_DMA_NUM_MAX

void huart_wait_txdone(void);
void asr_kick_start(void);
AT(.vad.mic_buf)
u16 vad_sdadc_buf[VAD_DMA_SIZE];

typedef enum {
    VAD_NORMAL = 0,
    VAD_SLEEP,
    VAD_W4_WAKE,
} VAD_SLEEP_STA;

typedef enum {
    VAD_IDLE = 0,
    VAD_START,
    VAD_CONTINUE,
} VAD_STA;

typedef struct {
    volatile u8 sta;            //vad唤醒状态
    volatile u8 sleep_flag;
    volatile u16 offset;        //VADDMACON0的高16位, +1:u8
    u8 start;                   //vad功能是否启动

    u16 vad_reboot_cnt;         //vad reboot 计数
    u16 *speech_start;          //语音在adcdma上对应的起始地址
    u32  start_tick;            //唤醒时的tick
    u32  dma_len;               //DMA总长度
    u8  *dma_ptr;               //DMA 中断触发模式，half or all done
    u8  *dma_ptr_t;
    u32  dma_addr_end;

    u16 neg_flag_cnt;
    u8 voice_flag;
} vad_cb_t;
static vad_cb_t vad_cb;

///自定义asr mic buff
u32 asr_get_adc_buff_addr(void)
{
    return DMA_ADR(vad_sdadc_buf);
}

u32 asr_get_adc_buff_len(void)
{
    return VAD_DMA_SIZE;        //mic samples
}

#if ASR_DUMP_EN
static u8 asr_dump_header[8] AT(.vad.dump_header);
static s16 asr_dump_buffer[512] AT(.vad.dump_buffer);
void asr_huart_init(void)
{
    asr_dump_header[0] = 0x36;
    asr_dump_header[1] = 0xAD;
    asr_dump_header[2] = 0xf9;
    asr_dump_header[3] = 0x54;
}

AT(.com_text.asr_dump)
static bool asr_huart_putcs_do(const void *buf, uint len)
{
    memcpy(asr_dump_buffer, buf, len);
    huart_tx(asr_dump_buffer, len);
    return true;
}

AT(.com_text.asr_dump)
bool asr_huart_putcs(const void *buf, uint len)
{
    static u8 frame_num = 0;
    bool ret;
    asr_dump_header[4] = 2;                 //type
    asr_dump_header[5] = (u8)(len >> 8);
    asr_dump_header[6] = (u8)(len >> 0);
    asr_dump_header[7] = frame_num++;
    ret = asr_huart_putcs_do(asr_dump_header, 8);
    huart_wait_txdone();
    if (!ret) {
        return false;
    }
    ret = asr_huart_putcs_do(buf, len);
    return ret;
}
#endif

///语音识别回调函数
void asr_callback_process(short *ptr)
{
#if ASR_DUMP_EN
    asr_huart_putcs(ptr,512*2);
#endif

}

//计算预取前面frames_num帧的offset
AT(.vad_text.vad)
static void vad_prefetch_cal(u16 frames_num)
{
    u32 vad_len = frames_num * 128;
    //VAD要取512个u16的整数倍，offset要往前推成整数
    if(vad_cb.offset < KWS_SAMPLES) {
        vad_cb.offset  = 0;
    } else {
        vad_cb.offset -= ((vad_cb.offset)%KWS_SAMPLES);
    }
	//计算往前推frames_num帧之后，VAD在DMA上的起始地址
    if(vad_cb.offset >= vad_len) {
        vad_cb.speech_start  = (u16 *)(asr_get_adc_buff_addr() + ((vad_cb.offset - vad_len)<<1));
    } else {
        vad_cb.speech_start  = (u16 *)(asr_get_adc_buff_addr() + ((vad_cb.dma_len - (vad_len - vad_cb.offset))<<1));
    }
}

//取出前面(512点/帧)的数据
AT(.vad_text.vad)
static u32 vad_prefetch_read(void)
{
    u32 p = (u32)vad_cb.speech_start;
    vad_cb.speech_start += KWS_SAMPLES;
    if ((u32)vad_cb.speech_start >= vad_cb.dma_addr_end) {
        vad_cb.speech_start = (u16 *)asr_get_adc_buff_addr();
    }
    return p;
}

AT(.vad_text.vad)
static u32 vad_dma_audio_read(u16 offset)
{
    u32 rptr;
    if (vad_cb.offset > 0) {
        rptr = asr_get_adc_buff_addr() + vad_cb.offset * 2;
    } else {
        rptr = (u32)vad_cb.dma_ptr_t;
    }
    u32 p = (u32)(rptr + (offset << 1));

    if (p >= vad_cb.dma_addr_end) {
        //TRACE("vad_dma_audio_read p:%x  vad_cb.dma_addr_end:%x\n", p, vad_cb.dma_addr_end);
        p = asr_get_adc_buff_addr();
    }
	return p;
}

AT(.vad_text.vad)
static void vad_reboot(void)
{
    TRACE("%s\n",__func__);
    VADCON0 &= ~BIT(0);                 //VAD disable
    vad_cb.vad_reboot_cnt = 0;
    VADCON0 |= BIT(0);                  //VAD enable
}

AT(.vad_text.vad)
static void vad_reset(void)
{
    TRACE("%s\n",__func__);
    VADCON0 &= ~BIT(0);                 //VAD disable
    vad_cb.sta = VAD_IDLE;
    vad_cb.offset = 0;
    vad_cb.speech_start = 0;
    vad_cb.start_tick = 0;
    sys_clk_free(INDEX_KARAOK);
    VADCON0 |= BIT(0);                  //VAD enable
    SDADC0DMACON |= BIT(9);             //VAD interrupt enable
}

AT(.vad_text.vad)
bool vad_is_start(void)
{
    return vad_cb.start;
}

///返回VAD是否已经唤醒
AT(.vad_text.vad)
bool vad_is_wake(void)
{
    return (bool)(vad_cb.sta >= VAD_START);
}

///VAD启动检测
AT(.vad_text.vad)
void vad_start(void)
{
    vad_reset();
    SDADC0DMACON &= ~BIT(9);            //VAD interrupt disable
    SDADCDMACLR |= BIT(24);             //clear VAD interrupt flag
    SDADC0DMACON |= BIT(9);             //VAD interrupt enable

	VADCON0 |= BIT(6);                  // tick
	VADCON0 |= BIT(0);                  // VAD enable

    vad_cb.start = 1;
	TRACE("VADCON0 %08x\n",VADCON0);
	TRACE("VADCON1 %08x\n",VADCON1);
	TRACE("VADCON2 %08x\n",VADCON2);
    TRACE("SDADCDMACON:%08x\n", SDADCDMACON);
}

AT(.vad_text.vad)
void vad_stop(void)
{
    SDADC0DMACON &= ~BIT(9);             //VAD interrupt disable
    SDADCDMACLR |= BIT(24);
    VADCON0 &= ~BIT(0);                 //VAD disable
    VADCON0 &= ~BIT(6);                 // tick
    vad_cb.start = 0;
}

///进出休眠模式的配置
AT(.vad_text.vad)
void vad_sleep_config(bool enter)
{
    if (enter) {
        vad_cb.sleep_flag = VAD_SLEEP;
        vad_reset();
    } else {
        vad_cb.sleep_flag = VAD_NORMAL;
        if (vad_cb.sta == VAD_START && vad_cb.sleep_flag == VAD_NORMAL) {
            asr_kick_start();
        }
    }
}

AT(.vad_text.vad)
static void vad_continue_process(void)
{
    s16 num, num2;
    u8 i;
    int16_t *rptr;
    u16 offset, dma_half_size;
    num2 = 0;
    offset = vad_cb.offset;

    vad_cb.dma_ptr_t = vad_cb.dma_ptr;
    dma_half_size = vad_cb.dma_len >> 1;
    num = dma_half_size >> 9;                   //除以512, 计算DMA中有num个512帧
    if (offset > 0) {                           //刚取完前面帧，不需要取DMA所有数据
        if ((u32)vad_cb.dma_ptr_t == asr_get_adc_buff_addr()) {
            if (offset <= dma_half_size) {      //VAD起来的offset位于half done前 //offset==dma_half_size, num-num2=0
                num2 = offset>>9;               // 除以512
            } else {                            //dma half done, 但是VAD起来的offset位于all done前
                offset = (dma_half_size<<1) - offset;
                num2 = -(offset>>9);
            }
        } else {
            if (offset > dma_half_size) {       //VAD起来的offset位于all done前  //offset==dma_half_size, num-num2=0
                offset = offset - dma_half_size;
                num2 = offset>>9;
            } else {                            //dma all done, 但是VAD起来的offset位于half done前
                offset = dma_half_size - offset;
                num2 = -(offset>>9);
            }
        }
    }
    if (num2 > num) {
        TRACE("------------vad offset error!--------\n");
        vad_reset();                            //VAD强制清零
        return;
    }
    num -= num2;
#if DEBUG_EN
    GPIOASET = BIT(14);
#endif
    TRACE("num %d %d\n", num, num2);
    for (i = 0; i < num; i++) {
        rptr = (int16_t *)vad_dma_audio_read(KWS_SAMPLES * i);
        asr_callback_process(rptr);
        if (i != 0 && i % 5 == 0) delay_5ms(1); //主动释放线程5ms
    }
#if DEBUG_EN
    GPIOACLR = BIT(14);
#endif
    vad_cb.offset = 0;
}

AT(.vad_text.vad.proc)
void vad_kws_process(void)
{
    if ((KWS_RUNTIME_LIMIT != 0) && (vad_cb.start_tick > 0)) {
        if (tick_check_expire(vad_cb.start_tick, KWS_RUNTIME_LIMIT)) { //超过KWS限制的时间
            vad_reset();
        }
    }

    if (vad_cb.sta == VAD_START) {
        sys_clk_req(INDEX_KARAOK,SYS_160M);                             //借用INDEX_KARAOK
        vad_prefetch_cal(VAD_PREFETCH_NUM * 512 / KWS_SAMPLES);
        int16_t *rptr;
        for (u8 i = 0; i < VAD_PREFETCH_NUM; i++) {
            //先把当前DMA的数据取出来扔到asr alg
            rptr = (int16_t *)vad_prefetch_read();
            asr_callback_process(rptr);
            if (i != 0 && i % 5 == 0) delay_5ms(1);                     //主动释放线程5ms
        }
        TRACE("VAD_START\n");
        TRACE("vad_pre_num: %d\n", VAD_PREFETCH_NUM);
        TRACE("vad offset: %x\n", vad_cb.offset);
        vad_cb.sta = VAD_CONTINUE;
    } else if (vad_cb.sta == VAD_CONTINUE){
        vad_continue_process();
    }
}

AT(.vad_text.vad.proc)
void asr_kws_process(void)
{
    vad_kws_process();
}

AT(.com_text.vad)
void vad_isr(void)
{
    if (SDADCDMAFLAG & BIT(24)) {
        SDADCDMACLR |= BIT(24);
        SDADC0DMACON &= ~BIT(9);                        //VAD interrupt disable
        vad_cb.offset = VADDMACON>> 16;                 //获取VAD中断时，VAD DMA BUFF的偏移地址
        vad_cb.start_tick = tick_get();
        vad_cb.sta = VAD_START;
        if (vad_cb.sleep_flag == VAD_SLEEP) {
            vad_cb.sleep_flag = VAD_W4_WAKE;
        } else if (vad_cb.sleep_flag == VAD_NORMAL) {
            asr_kick_start();
        }
    }
}

///SDADC DMA中断
AT(.com_text.vad)
void vad_sdadc_check_voice(void)
{
    if (SDADCDMAFLAG & BIT(24)) {   //VAD voice
        SDADCDMACLR |= BIT(24);
        if (vad_cb.sta == VAD_CONTINUE) {
            if (vad_cb.neg_flag_cnt) {
                vad_cb.neg_flag_cnt--;
                if (vad_cb.neg_flag_cnt == (VAD_NEG_FLAG_THRESHOLD - 1)) {
                    vad_cb.neg_flag_cnt = VAD_NEG_FLAG_THRESHOLD / 2;
                    vad_cb.voice_flag = 1;
                }
            }
        }
    } else {                        //VAD not voice
        if (vad_cb.sta == VAD_CONTINUE) {
            if (vad_cb.neg_flag_cnt >= VAD_NEG_FLAG_THRESHOLD) {
                vad_cb.neg_flag_cnt = VAD_NEG_FLAG_THRESHOLD;
                vad_cb.voice_flag = 0;
            } else {
                vad_cb.neg_flag_cnt++;
            }
        }
    }
}

AT(.com_text.vad.proc)
void vad_sdadc_process(u8 *ptr, u32 samples, u32 pcm_mode)
{
    if (vad_is_start()) {
        vad_cb.dma_ptr = ptr;
        vad_cb.vad_reboot_cnt++;
        if (vad_cb.vad_reboot_cnt >= VAD_REBOOT_THRESOLD ) {
            vad_reboot();
        }

        if (((vad_cb.sta == VAD_CONTINUE) || (vad_cb.sta == VAD_START )) && (vad_cb.sleep_flag == VAD_NORMAL)) {
            asr_kick_start();
        }
    }
    vad_sdadc_check_voice();
}

u8 vad_init(void)
{
    if (DAC_24BITS_EN) {
        printf("vad_init err, please select dac_16bit\n");
        return 0;
    }

	memset(&vad_cb,0,sizeof(vad_cb_t));
    vad_cb.dma_len = asr_get_adc_buff_len();
    vad_cb.dma_addr_end = asr_get_adc_buff_addr() + vad_cb.dma_len * 2;
    vad_cb.sta = VAD_IDLE;
    vad_cb.start = 0;

    CLKGAT3 |= BIT(12);         //vad_clken
    delay_5ms(1);
    VADCON0 = 0;
    u8 mic_index = xcfg_cb.call_talk_mic;
    VADCON0 |= (mic_index << 1);
	VADCON1 = (VAD_NEG_CNT | (VAD_POS_CNT << 16));
	VADCON2 = 0x00FF000F;

	return 1;
}

void bsp_vad_start(void)
{
#if ASR_DUMP_EN
    asr_huart_init();
#endif
    audio_path_init(AUDIO_PATH_ASR);
    audio_path_start(AUDIO_PATH_ASR);
    vad_init();
    vad_start();
    dac_fade_in();
}

void bsp_vad_stop(void)
{
    vad_stop();
    audio_path_exit(AUDIO_PATH_ASR);
}
#endif
