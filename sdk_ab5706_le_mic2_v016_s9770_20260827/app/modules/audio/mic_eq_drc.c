/*
 * 文件名称: mic_eq_drc.c
 * 功能描述: EQ_DRC主要用来处理音效,主频160M，120个点,EQ和DRC算法时间分别为250us和180us左右;
 *           MIX_DRC主要用来一拖二音频混合,主频160M，120个点,算法时间为180us左右;
 ****************************************************************************************
 */
#include "include.h"
#include "mic_eq_drc.h"

#if WIRELESS_MIC_SOFT_VOL_EN

//AT(.com_rodata.sdadc.gain_tbl)
const u32 mic_soft_gain_tbl_64[128] = {
    //负增益
    SOFT_GAIN_N110DB,SOFT_GAIN_N110DB,SOFT_GAIN_N65DB,SOFT_GAIN_N60DB,SOFT_GAIN_N59DB,
    SOFT_GAIN_N58DB,SOFT_GAIN_N57DB,SOFT_GAIN_N56DB,SOFT_GAIN_N55DB,SOFT_GAIN_N54DB,
    SOFT_GAIN_N53DB,SOFT_GAIN_N52DB,SOFT_GAIN_N51DB,SOFT_GAIN_N50DB,SOFT_GAIN_N49DB,
    SOFT_GAIN_N48DB,SOFT_GAIN_N47DB,SOFT_GAIN_N46DB,SOFT_GAIN_N45DB,SOFT_GAIN_N44DB,
    SOFT_GAIN_N43DB,SOFT_GAIN_N42DB,SOFT_GAIN_N41DB,SOFT_GAIN_N40DB,SOFT_GAIN_N39DB,
    SOFT_GAIN_N38DB,SOFT_GAIN_N37DB,SOFT_GAIN_N36DB,SOFT_GAIN_N35DB,SOFT_GAIN_N34DB,
    SOFT_GAIN_N33DB,SOFT_GAIN_N32DB,SOFT_GAIN_N31DB,SOFT_GAIN_N30DB,SOFT_GAIN_N29DB,
    SOFT_GAIN_N28DB,SOFT_GAIN_N27DB,SOFT_GAIN_N26DB,SOFT_GAIN_N25DB,SOFT_GAIN_N24DB,
    SOFT_GAIN_N23DB,SOFT_GAIN_N22DB,SOFT_GAIN_N21DB,SOFT_GAIN_N20DB,SOFT_GAIN_N19DB,
    SOFT_GAIN_N18DB,SOFT_GAIN_N17DB,SOFT_GAIN_N16DB,SOFT_GAIN_N15DB,SOFT_GAIN_N14DB,
    SOFT_GAIN_N13DB,SOFT_GAIN_N12DB,SOFT_GAIN_N11DB,SOFT_GAIN_N10DB,SOFT_GAIN_N9DB,
    SOFT_GAIN_N8DB,SOFT_GAIN_N7DB,SOFT_GAIN_N6DB,SOFT_GAIN_N5DB,SOFT_GAIN_N4DB,
    SOFT_GAIN_N3DB,SOFT_GAIN_N2DB,SOFT_GAIN_N1DB,SOFT_GAIN_N0DB,

    //正增益
    SOFT_GAIN_P0_0DB, SOFT_GAIN_P0_5DB, SOFT_GAIN_P1_0DB, SOFT_GAIN_P1_5DB, SOFT_GAIN_P2_0DB,
    SOFT_GAIN_P2_5DB, SOFT_GAIN_P3_0DB, SOFT_GAIN_P3_5DB, SOFT_GAIN_P4_0DB, SOFT_GAIN_P4_5DB,
    SOFT_GAIN_P5_0DB, SOFT_GAIN_P5_5DB, SOFT_GAIN_P6_0DB, SOFT_GAIN_P6_5DB, SOFT_GAIN_P7_0DB,
    SOFT_GAIN_P7_5DB, SOFT_GAIN_P8_0DB, SOFT_GAIN_P8_5DB, SOFT_GAIN_P9_0DB, SOFT_GAIN_P9_5DB,
    SOFT_GAIN_P10_0DB, SOFT_GAIN_P10_5DB, SOFT_GAIN_P11_0DB, SOFT_GAIN_P11_5DB, SOFT_GAIN_P12_0DB,
    SOFT_GAIN_P12_5DB, SOFT_GAIN_P13_0DB, SOFT_GAIN_P13_5DB, SOFT_GAIN_P14_0DB, SOFT_GAIN_P14_5DB,
    SOFT_GAIN_P15_0DB, SOFT_GAIN_P15_5DB, SOFT_GAIN_P16_0DB, SOFT_GAIN_P16_5DB, SOFT_GAIN_P17_0DB,
    SOFT_GAIN_P17_5DB, SOFT_GAIN_P18_0DB, SOFT_GAIN_P18_5DB, SOFT_GAIN_P19_0DB, SOFT_GAIN_P19_5DB,
    SOFT_GAIN_P20_0DB, SOFT_GAIN_P20_5DB, SOFT_GAIN_P21_0DB, SOFT_GAIN_P21_5DB, SOFT_GAIN_P22_0DB,
    SOFT_GAIN_P22_5DB, SOFT_GAIN_P23_0DB, SOFT_GAIN_P23_5DB, SOFT_GAIN_P24_0DB, SOFT_GAIN_P24_5DB,
    SOFT_GAIN_P25_0DB, SOFT_GAIN_P25_5DB, SOFT_GAIN_P26_0DB, SOFT_GAIN_P26_5DB, SOFT_GAIN_P27_0DB,
    SOFT_GAIN_P27_5DB, SOFT_GAIN_P28_0DB, SOFT_GAIN_P28_5DB, SOFT_GAIN_P29_0DB, SOFT_GAIN_P29_5DB,
    SOFT_GAIN_P30_0DB, SOFT_GAIN_P30_5DB, SOFT_GAIN_P31_0DB, SOFT_GAIN_P31_5DB,
};

typedef struct {
    u8 mute;
    u8 sample_rate;
    u16 samples;
    u8 drc_mic_en;
    u8 gain_tbl_index;
    u16 target_gain;
    audio_callback_t callback;
} drc_mic_cfg_t;

static drc_mic_cfg_t drc_mic_cfg;
static drc_v3_cb_t soft_drc_cb;
static const u8 soft_gain_range[SOFT_GAIN_MAX_LEVEL+1] = {0,5,12,24,34,44,55,60,64};

// 0 ~ 127 0~63为负增益 64~127为正增益
AT(.text.mic_drc.gain)
void wireless_mic_set_soft_gain(u8 level)
{
    drc_mic_cfg.gain_tbl_index = (level >= SOFT_GAIN_MAX_LEVEL)? SOFT_GAIN_MAX_LEVEL : level;

    drc_mic_cfg.target_gain = mic_soft_gain_tbl_64[soft_gain_range[drc_mic_cfg.gain_tbl_index]];

//    printf("gain_tbl_index = %d,target_gain = %d\n",drc_mic_cfg.gain_tbl_index, drc_mic_cfg.target_gain);
}

void soft_gain_up(void)
{
    if (drc_mic_cfg.gain_tbl_index) {
        wireless_mic_set_soft_gain(drc_mic_cfg.gain_tbl_index-1);
    }
}

void soft_gain_down(void)
{
    wireless_mic_set_soft_gain(drc_mic_cfg.gain_tbl_index+1);
}

uint8_t soft_gain_level_get(void)
{
    return drc_mic_cfg.gain_tbl_index;
}

bool soft_gain_level_is_max_min(void)
{
    if ((drc_mic_cfg.gain_tbl_index == 0) || (drc_mic_cfg.gain_tbl_index == SOFT_GAIN_MAX_LEVEL)) {
        return true;
    }

    return false;
}

AT(.text.mic_soft_gain)
void drc_mic_audio_input(u8 *ptr, u32 samples, int ch_mode, void *params)
{
    s16 * rptr = (s16 *)ptr;
    if (drc_mic_cfg.drc_mic_en) {
        //32bit 拓展
        s32 p_left;
        for (u16 i = 0; i < samples; ++i) {
            p_left = rptr[i];
            p_left = __builtin_muls_shift15(p_left, drc_mic_cfg.target_gain);
            rptr[i]  = drc_v3_calc(p_left, &soft_drc_cb);
        }
    }
    if (drc_mic_cfg.callback) {
        drc_mic_cfg.callback((u8 *)rptr, samples,ch_mode, params);
    }
}

AT(.text.mic_drc)
void drc_mic_audio_output_callback_set(audio_callback_t callback)
{
    drc_mic_cfg.callback = callback;
}

AT(.text.mic_drc.init)
void drc_mic_init(u8 sample_rate, u16 samples, u8 channel)
{
    printf("drc_mic_init\n");
    memset(&soft_drc_cb, 0, sizeof(soft_drc_cb));
    memset(&drc_mic_cfg, 0, sizeof(drc_mic_cfg_t));

    drc_mic_cfg.drc_mic_en = 1;

    uint first_level =  (SOFT_GAIN_DEFAULT_LEVEL >= SOFT_GAIN_MAX_LEVEL)? SOFT_GAIN_MAX_LEVEL : SOFT_GAIN_DEFAULT_LEVEL;
    wireless_mic_set_soft_gain(first_level);

    drc_v3_init((const void *)RES_BUF_WS_MIC_48K_DRC, RES_LEN_WS_MIC_48K_DRC, &soft_drc_cb);
}
#endif

#if WIRELESS_MIC_EQ_DRC_EN

typedef struct {
    u8 mute;
    u8 sample_rate;
    u16 samples;
    audio_callback_t callback;
} mic_eq_drc_cfg_t;

static mic_eq_drc_cfg_t mic_eq_drc AT(.buf.mic_eq_drc);

AT(.text.mic_eq_drc_proc.input)
void mic_eq_drc_audio_input(u8 *ptr, u32 samples, int ch_mode, void *params)
{
    if (!mic_eq_drc.mute) {
        loc_mic_pacc_process((u8 *)&ptr[0], (u8 *)&ptr[0], samples);
    }
    mic_eq_drc.callback(ptr, samples, ch_mode, params);
}
AT(.text.mic_eq_drc_proc.callback)
void mic_eq_drc_audio_output_callback_set(audio_callback_t callback)
{
    mic_eq_drc.callback = callback;
}
AT(.text.mic_eq_drc_proc.init)
void mic_eq_drc_init(u8 sample_rate, u16 samples, u8 channel)
{
    //先初始化PACC链路
    loc_mic_pacc_init();

    //然后设置参数
    loc_mic_pacc_set_param();

    //最后使能PACC
    loc_mic_pacc_enable();
    
    mic_eq_drc.mute = 0;
    printf("%s\n",__func__);

}
AT(.text.mic_eq_drc_proc.mute.set)
void mic_eq_drc_mute_set(u8 mute)
{
    mic_eq_drc.mute = mute;
}
AT(.text.mic_eq_drc_proc.mute.get)
u8 mic_eq_drc_mute_get()
{
    return mic_eq_drc.mute;
}
#endif

