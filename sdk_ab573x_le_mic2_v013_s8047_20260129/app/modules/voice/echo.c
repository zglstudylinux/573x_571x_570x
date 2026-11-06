#include "include.h"
#include "echo.h"
//#include "api_alg.h"
/*
 * 文件名称: echo.c
 * 功能描述: 本文件为软件ECHO处理模块
 * 暂时只支持48k采样率
 * 24bit数据流处理，由于芯片的设计， 24bit数据流是以s32的结构存在如下

  |   sample  |   sample  |   sample  |   sample  |
  -------------------------------------------------
  |   s32     |   s32     |   s32     |   s32     |
  -------------------------------------------------
  |u8 u8 u8 × |u8 u8 u8 × |u8 u8 u8 × |u8 u8 u8 × |
 * 当需要数据dump和仿真对数据时，需要转回真实的24bit pcm结构 ，如下
  |u8 u8 u8  |u8 u8 u8  |u8 u8 u8  |u8 u8 u8 |
  具体处理可参考 UARTDUMP_ECHO_EN
 ****************************************************************************************
    code :
    buf  : 10000*2 Bytes
    time : 160M 下 120 个点处理 147us
 */
#if ECHO_EN

#define ECHO_SAMPLE_RATE             48000
#define MAX_DELAY_LENGTH             (ECHO_DELAY_BUF_SIZE/((ECHO_SAMPLE_RATE >> 1) * 0.001f))  //最大delay时间
#define FRAME_LEN		             WIRELESS_MIC_SAMPLES_SELECT
#define UARTDUMP_ECHO_EN             0             //DUMP数据一键配置，需要使用定制上位机

#if UARTDUMP_ECHO_EN
static u8 UartDump_buf_temp_24bit[2 * FRAME_LEN*4] AT(.bss.echo); //24bit 双通道
#endif

static echo_init_t echo_init_cfg AT(.buf.echo);        //初始化结构体
static echo_cfg_t echo_cfg AT(.buf.echo);             //管理模块结构体
mic_pcm_t delay_lbuf[ECHO_DELAY_BUF_SIZE] AT(.buf.echo.delay);

AT(.text.echo_proc.input)
void echo_audio_input(u8 *ptr, u32 samples, u32 params)
{
    mic_pcm_t *rptr = (mic_pcm_t *)ptr;
#if UARTDUMP_ECHO_EN
    //IO边沿触发，用来解析对齐用的
	GPIOBDE |=BIT(4);
    GPIOBFEN &= ~(BIT(4));
    GPIOBDIR &= ~(BIT(4));
    GPIOBSET = BIT(4);

    u8 *rptr_24bit_dump = NULL;
    if(samples == 94) {
        rptr_24bit_dump = &UartDump_buf_temp_24bit[0];
    } else if(samples == 146) {
        rptr_24bit_dump = &UartDump_buf_temp_24bit[94*6];
    }
    //原始数据放左通道
    for(u16 i=0; i<samples; i++) {
        rptr_24bit_dump[i*6 + 0] = (rptr[i] >>0)&0xff ;
        rptr_24bit_dump[i*6 + 1] = (rptr[i] >>8)&0xff ;
        rptr_24bit_dump[i*6 + 2] = (rptr[i] >>16)&0xff ;
    }
#endif

    if(!echo_cfg.mute) {
        if(samples) {
            for(u16 i=0; i<samples; i++) {
                echo_process(rptr+i);
            }
        }
    }

#if UARTDUMP_ECHO_EN
    //算法后数据放右通道
    for(u16 i=0; i<samples; i++) {
        rptr_24bit_dump[i*6 + 3] = (rptr[i] >>0)&0xff ;
        rptr_24bit_dump[i*6 + 4] = (rptr[i] >>8)&0xff ;
        rptr_24bit_dump[i*6 + 5] = (rptr[i] >>16)&0xff ;
    }
    //分片处理的后半段 kick硬件huart数据dump
    if(samples == 146) {
        huart_tx(&UartDump_buf_temp_24bit[0], WIRELESS_MIC_SAMPLES_SELECT*6);
    }
    GPIOBCLR = BIT(4);
#endif

    if(echo_cfg.callback) {
        echo_cfg.callback((u8 *)ptr, samples, params);
    }
}

AT(.text.echo_set.callback)
void echo_audio_output_callback_set(audio_callback_t callback)
{
    echo_cfg.callback = callback;
}

AT(.text.echo_set.mute)
void echo_audio_mute_set(uint8_t mute)
{
    echo_cfg.mute = mute;
    if(echo_cfg.mute) {
        memset(delay_lbuf, 0, sizeof(delay_lbuf));
    }
}

AT(.text.echo_proc.mute)
uint8_t echo_audio_mute_get(void)
{
    return echo_cfg.mute;
}

AT(.text.echo_set)
void echo_delay_len_set(u16 delay_len)
{
    echo_audio_param_set(ECHO_LEVEL, delay_len, 1000, 0, ECHO_DRY_USER, ECHO_WET_USER);
}

AT(.text.echo_set)
void echo_delay_level_change(void)
{
    echo_cfg.delay_level += 1;
    if(echo_cfg.delay_level >= ECHO_DELAY_MAX_LEVEL) {
        echo_cfg.delay_level = 0;
    }
    echo_delay_len_set(echo_cfg.delay_level*echo_cfg.delay_len_step);
}

AT(.text.echo_set)
void echo_delay_level_up(void)
{
    if(echo_cfg.delay_level < (ECHO_DELAY_MAX_LEVEL - 1)) {
        echo_cfg.delay_level += 1;
    }
    echo_delay_len_set(echo_cfg.delay_level*echo_cfg.delay_len_step);
}

AT(.text.echo_set)
void echo_delay_level_down(void)
{
    if(echo_cfg.delay_level) {
        echo_cfg.delay_level -= 1;
    }
    echo_delay_len_set(echo_cfg.delay_level*echo_cfg.delay_len_step);
}

AT(.text.echo_get)
u8 echo_delay_level_get(void)
{
    return echo_cfg.delay_level;
}

AT(.text.echo_get)
bool echo_delay_level_is_max_min(void)
{
    if(echo_cfg.delay_level == (ECHO_DELAY_MAX_LEVEL - 1) || echo_cfg.delay_level == 0) {
        return true;
    }
    return false;
}

AT(.text.echo_set)
void echo_delay_level_set(u8 delay_level)
{
    if(delay_level > (ECHO_DELAY_MAX_LEVEL - 1)) {
        delay_level = ECHO_DELAY_MAX_LEVEL - 1;
    }
    echo_cfg.delay_level = delay_level;
    echo_delay_len_set(echo_cfg.delay_level*echo_cfg.delay_len_step);
}

AT(.text.echo_set)
void echo_attenuation_set(u16 attenuation)
{
    echo_audio_param_set(attenuation, (ECHO_DELAY_DEFAULT_LEVEL - 1)*echo_cfg.delay_len_step, 1000, 0, ECHO_DRY_USER, ECHO_WET_USER);
}

AT(.text.echo_set)
void echo_attenuation_level_set(u8 attenuation_level)
{
    if(attenuation_level > (ECHO_ATTENUATION_MAX_LEVEL - 1)) {
        attenuation_level = ECHO_ATTENUATION_MAX_LEVEL - 1;
    }
    echo_cfg.attenuation_level = attenuation_level;
    //可以改成用表的形式听感会好一些
    echo_attenuation_set(echo_cfg.attenuation_level * (90 / (ECHO_ATTENUATION_MAX_LEVEL - 1)) );
}

AT(.text.echo_set)
void echo_attenuation_level_change(void)
{
    echo_cfg.attenuation_level += 1;
    if(echo_cfg.attenuation_level >= ECHO_ATTENUATION_MAX_LEVEL) {
        echo_cfg.attenuation_level = 0;
    }
    echo_attenuation_level_set(echo_cfg.attenuation_level);
}

/// attenuation:    混响次数  0~90
/// delay_length:   每一声混响之间的间隔  0~400
/// cutoffFreq_set: 低通滤波器的截止频率  1~4000
/// lp_filter_en:   低通滤波器的开关      0：关 1：开
/// dry_set:        原始声音               0~49152（Q15即0~1.5）
/// wet_set:        回声湿度，越大所占比例越大  0~32768（Q15 即0~1.0）
/// 默认配置 echo_audio_param_set(55,250,1000,1,32768,15000)
AT(.text.echo_set.param)
void echo_audio_param_set(u16 attenuation ,s32 delay_length ,u32 cutoffFreq_set ,u32 lp_filter_en , u16 dry_set ,u16 wet_set)
{
    if(attenuation >= 90) {
        attenuation = 90;
    }

    if(delay_length >= echo_cfg.max_delay_len) {
        delay_length = echo_cfg.max_delay_len;
    }

//    echo_init_cfg.skip_flag         = 1;
    echo_init_cfg.lp_filter_en      = lp_filter_en;
    echo_init_cfg.attenuation_set   = attenuation;
    echo_init_cfg.delay_set         = delay_length;
    echo_init_cfg.dry               = dry_set;
    echo_init_cfg.wet               = wet_set;
    echo_init_cfg.cutoffFreq_set    = cutoffFreq_set;
//    echo_init_cfg.delay_lbuf_set    = delay_lbuf;
    echo_update_param(&echo_init_cfg, 1);
}

AT(.text.echo_set)
void echo_audio_init(u8 sample_rate, u16 samples, u8 channel)
{
    memset(&echo_cfg, 0, sizeof(echo_cfg_t));
//    memset(&echo_init_cfg, 0, sizeof(echo_init_t));
    memset(delay_lbuf, 0, sizeof(delay_lbuf));

    //管理管理模块初始化设置
    echo_cfg.sample_rate            = sample_rate;
    if(echo_cfg.sample_rate != SPR_48000) {
        printf("Notice Samples config err! ");
    }
    echo_cfg.bits_mode = WIRELESS_MIC_24B_PCM_EN? 0 : 1;
    echo_cfg.max_delay_len          = MAX_DELAY_LENGTH;
    echo_cfg.delay_len_step         = echo_cfg.max_delay_len/(ECHO_DELAY_MAX_LEVEL - 1); //delay挡位步进
    echo_cfg.delay_level            = ECHO_DELAY_DEFAULT_LEVEL - 1; //初始化挡位设置

    //音效参数初始化设置
    echo_init_cfg.skip_flag         = 1;
    echo_init_cfg.lp_filter_en      = 1;
    echo_init_cfg.attenuation_set   = ECHO_LEVEL;
    echo_init_cfg.delay_set         = echo_cfg.delay_level * echo_cfg.delay_len_step;
    echo_init_cfg.dry               = ECHO_DRY_USER;
    echo_init_cfg.wet               = ECHO_WET_USER;
    echo_init_cfg.cutoffFreq_set    = 3000;
    echo_init_cfg.delay_lbuf_set    = delay_lbuf;
    echo_init(&echo_init_cfg);

#if UARTDUMP_ECHO_EN
    memset(UartDump_buf_temp_24bit, 0, sizeof(UartDump_buf_temp_24bit));

    huart_t huart0;
    memset(&huart0, 0x00, sizeof(huart0));
    huart0.rx_port = HUART_TR_PB3;        //HUART串口选择: PA15: 0, PB2: 1, PB3: 2, PB4: 3, PB15: 4, PE10: 5, PE13: 6, VUSB: 7
    huart0.tx_port = HUART_TR_PB3;
    huart0.rxbuf_loop = 1;
    huart0.rxbuf      = eq_rx_buf;
    huart0.rxbuf_size = 128;
    huart0.txisr_en = 1;
    huart0.rxisr_en = 0;
    huart_init(&huart0, 6000000);
#endif

}

AT(.text.echo_exit)
void echo_audio_exit(void)
{
}



#endif //ECHO_EN
