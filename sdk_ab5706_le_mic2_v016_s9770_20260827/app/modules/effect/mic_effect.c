/*
 * 文件名称: mic_effect.c
 * 功能描述: 复用PACC0模块，处理EQ/DRC等音效
 *
 * 注意事项：PACC0能分时处理不同cs链路，但要避免多线程同时调用pacc_ctl_proc_cs
 *
 *                  复用场景
 *      music/spk   /   a2dp        /   sco
 *      loc_mic     /   loc_mic     /
 *      loc_mic     /   mix_mic     /
 *                                  /   sco_mic
 *
 */

#include "include.h"
#include "effect_idx.h"
#include "mic_effect.h"

enum {
    PACC0_LOC_MIC   = BIT(0),
    PACC0_MIX_MIC   = BIT(1),
    PACC0_SCO_MIC   = BIT(2),
};

static struct {
    void *ctl;
    u8 enable;
} pacc0_hw = {
    .ctl        = NULL,
    .enable     = 0,
};

void *pacc0_ctl_alloc(uint en_bit)
{
    if(pacc0_hw.ctl == NULL) {
        pacc0_hw.ctl = pacc_effect_init(0);
    }
    pacc0_hw.enable |= en_bit;

    return pacc0_hw.ctl;
}

void pacc0_ctl_free(uint en_bit)
{
    pacc0_hw.enable &= ~en_bit;
    if(pacc0_hw.enable == 0) {
        pacc0_hw.ctl = NULL;
        pacc_effect_exit(0);
    }
}

#if ADAPTER_LOCAL_MIC_EQ_DRC_EN || WIRELESS_MIC_EQ_DRC_EN
//------------------------------------------------------------------------------------------
//本地麦EQ/DRC链路
#define LOC_PACC_FRAME_LEN		                120                 //算法处理帧长

static struct {
    void *pacc_ctl;                                                 //pacc硬件模块的控制句柄
    pacc_cs_t *start_cs;                                            //pacc链表处理的表头
    pacc_cs_t pacc_cs[LOC_MIC_PACC_MAX_CS];                         //pacc_cs列表
    u8 cs_en[LOC_MIC_PACC_MAX_CS];                                  //cs列表使能标志
    u8 cache[LOC_PACC_FRAME_LEN*4];                                 //pacc中间缓存
} loc_mic_pacc AT(.buf.mic_effect.pacc);

static bq_bd_t loc_mic_bq_db AT(.buf.mic_effect);
static drc_bd_t loc_mic_drc_db AT(.buf.mic_effect);


static const pacc_effect_cb loc_mic_pacc_cb_tbl[LOC_MIC_PACC_MAX_CS] = {
    {LOC_MIC_PACC_EQ_CS,   PACC_EQ,    false, false,  loc_mic_pacc.cache,        NULL, S16_Q15_SAT, loc_mic_pacc.cache,  S32_Q23, &loc_mic_pacc.pacc_cs[LOC_MIC_PACC_EQ_CS],        &loc_mic_bq_db},
    {LOC_MIC_PACC_DRC_CS,  PACC_DRC,   false, false,  loc_mic_pacc.cache,        NULL, S32_Q23, loc_mic_pacc.cache,  S16_Q15_SAT, &loc_mic_pacc.pacc_cs[LOC_MIC_PACC_DRC_CS],       &loc_mic_drc_db},
};

void loc_mic_pacc_init(void)
{
    printf("%s\n", __func__);

    //temp ram init
    memset(loc_mic_pacc.cache, 0, sizeof(loc_mic_pacc.cache));

    //初始链路模块状态全部关闭
    memset(loc_mic_pacc.cs_en, 0x0, LOC_MIC_PACC_MAX_CS);

    //ram init
    memset(&loc_mic_bq_db, 0, sizeof(bq_bd_t));
    memset(&loc_mic_drc_db, 0, sizeof(drc_bd_t));

    loc_mic_pacc.pacc_ctl = pacc0_ctl_alloc(PACC0_LOC_MIC);  //麦使用PACC0
    loc_mic_pacc.start_cs = pacc_effect_link((void*)loc_mic_pacc_cb_tbl, LOC_MIC_PACC_MAX_CS, LOC_PACC_FRAME_LEN);
}

void loc_mic_pacc_set_param(void)
{
    u32 drc_addr = (u32)effect_res_addr_get(EFFECT_IDX_LOC_MIC_DRC);
    u32 drc_len = effect_res_len_get(EFFECT_IDX_LOC_MIC_DRC);
    u32 eq_addr = (u32)effect_res_addr_get(EFFECT_IDX_LOC_MIC_EQ);
    u32 eq_len  = effect_res_len_get(EFFECT_IDX_LOC_MIC_EQ);

    if(pacc_drc_set_by_res(&loc_mic_pacc.pacc_cs[LOC_MIC_PACC_DRC_CS], drc_addr, drc_len) == ERROR_NO) {
        loc_mic_pacc.cs_en[LOC_MIC_PACC_DRC_CS] = 1;
    }

    if (pacc_eq_set_by_res(&loc_mic_pacc.pacc_cs[LOC_MIC_PACC_EQ_CS], eq_addr, eq_len) == ERROR_NO) {
        loc_mic_pacc.cs_en[LOC_MIC_PACC_EQ_CS] = 1;
    }
}

void loc_mic_pacc_set_eq_by_param(u8 band_cnt, const u32 *eq_param)
{
    pacc_eq_set_by_param(&loc_mic_pacc.pacc_cs[LOC_MIC_PACC_EQ_CS], band_cnt, eq_param);
}

void loc_mic_pacc_set_drc_by_param(u8 band_cnt, const u32 *drc_param)
{
    pacc_drc_set_by_param(&loc_mic_pacc.pacc_cs[LOC_MIC_PACC_DRC_CS], band_cnt, drc_param);
}

void loc_mic_pacc_enable(void)
{
    printf("%s\n", __func__);

    //根据使能的链路，重新链接cs链路
    loc_mic_pacc.start_cs = pacc_effect_relink(loc_mic_pacc.pacc_cs, loc_mic_pacc.cs_en, LOC_MIC_PACC_MAX_CS);

    //重新设置链头和链尾的数据格式
    pacc_cs_list_set_io_fmt(loc_mic_pacc.start_cs, false, S16_Q15_SAT, S16_Q15_SAT);
}

AT(.com_text.loc_mic)
void loc_mic_pacc_process(u8 *obuf, u8 *ibuf, u32 samples)
{
    pacc_ctl_proc_cs(loc_mic_pacc.pacc_ctl, loc_mic_pacc.start_cs, obuf, ibuf, samples);
}

void loc_mic_pacc_exit(void)
{
    loc_mic_pacc.pacc_ctl = NULL;
    pacc0_ctl_free(PACC0_LOC_MIC);               //麦使用PACC0
}
#endif // ADAPTER_LOCAL_MIC_EQ_DRC_EN

#if ADAPTER_MIX_DRC_EN
//------------------------------------------------------------------------------------------
//无线麦混音后EQ/DRC等链路
#define MIX_PACC_FRAME_LEN		                120                 //算法处理帧长
extern s32 pacc_temp_buf[120];

static struct {
    void *pacc_ctl;                                                 //pacc硬件模块的控制句柄
    pacc_cs_t *start_cs;                                            //pacc链表处理的表头
    pacc_cs_t pacc_cs[MIX_MIC_PACC_MAX_CS];                        //pacc_cs列表
    u8 cs_en[MIX_MIC_PACC_MAX_CS];                                 //cs列表使能标志
//    u8 cache[MIX_PACC_FRAME_LEN*4];                                 //pacc中间缓存
} mix_pacc AT(.buf.mic_mix.pacc);

static bq_bd_t mix_bq_db AT(.buf.mic_mix);
static drc_bd_t mix_drc_db AT(.buf.mic_mix);
#if ADAPTER_FREQ_SHIFT_EN
static freq_shift_bd_t mix_fsh_bd AT(.buf.mic_mix);
#endif

static const pacc_effect_cb mix_pacc_cb_tbl[MIX_MIC_PACC_MAX_CS] = {
    {MIX_MIC_PACC_EQ_CS,    PACC_EQ,    false, false,  pacc_temp_buf,        NULL, S32_Q23, pacc_temp_buf,  S32_Q23,     &mix_pacc.pacc_cs[MIX_MIC_PACC_EQ_CS],        &mix_bq_db},
 #if ADAPTER_FREQ_SHIFT_EN
    {MIX_MIC_PACC_FSH_CS,   PACC_FSH,   false, false,  pacc_temp_buf,        NULL, S32_Q23, pacc_temp_buf,  S32_Q23,      &mix_pacc.pacc_cs[MIX_MIC_PACC_FSH_CS],       &mix_fsh_bd},
#endif
    {MIX_MIC_PACC_DRC_CS,   PACC_DRC,   false, false,  pacc_temp_buf,        NULL, S32_Q23, pacc_temp_buf,  S32_Q23,     &mix_pacc.pacc_cs[MIX_MIC_PACC_DRC_CS],       &mix_drc_db},

};

void mix_pacc_init(void)
{
    printf("%s\n", __func__);

    //temp ram init
//    memset(mix_pacc.cache, 0, sizeof(mix_pacc.cache));

    //初始链路模块状态全部关闭
    memset(mix_pacc.cs_en, 0x0, MIX_MIC_PACC_MAX_CS);

    //ram init
    memset(&mix_drc_db, 0, sizeof(drc_bd_t));
    memset(&mix_bq_db, 0, sizeof(bq_bd_t));
#if ADAPTER_FREQ_SHIFT_EN
    memset(&mix_fsh_bd, 0, sizeof(freq_shift_bd_t));
#endif

    mix_pacc.pacc_ctl = pacc0_ctl_alloc(PACC0_MIX_MIC);  //麦使用PACC0
    mix_pacc.start_cs = pacc_effect_link((void*)mix_pacc_cb_tbl, MIX_MIC_PACC_MAX_CS, MIX_PACC_FRAME_LEN);
}

void mix_pacc_set_param(void)
{
    u32 drc_addr = (u32)effect_res_addr_get(EFFECT_IDX_MIX_DRC);
    u32 drc_len = effect_res_len_get(EFFECT_IDX_MIX_DRC);
    u32 eq_addr = (u32)effect_res_addr_get(EFFECT_IDX_MIX_EQ);
    u32 eq_len  = effect_res_len_get(EFFECT_IDX_MIX_EQ);

    if (pacc_eq_set_by_res(&mix_pacc.pacc_cs[MIX_MIC_PACC_EQ_CS], eq_addr, eq_len) == ERROR_NO) {
        mix_pacc.cs_en[MIX_MIC_PACC_EQ_CS] = 1;
    }

    if(pacc_drc_set_by_res(&mix_pacc.pacc_cs[MIX_MIC_PACC_DRC_CS], drc_addr, drc_len) == ERROR_NO) {
        mix_pacc.cs_en[MIX_MIC_PACC_DRC_CS] = 1;
    }

#if ADAPTER_FREQ_SHIFT_EN
    mix_pacc.cs_en[MIX_MIC_PACC_FSH_CS] = 1;
#endif
}

void mix_pacc_set_eq_by_param(u8 band_cnt, const u32 *eq_param)
{
    pacc_eq_set_by_param(&mix_pacc.pacc_cs[MIX_MIC_PACC_EQ_CS], band_cnt, eq_param);
}

void mix_pacc_set_drc_by_param(u8 band_cnt, const u32 *drc_param)
{
    pacc_drc_set_by_param(&mix_pacc.pacc_cs[MIX_MIC_PACC_DRC_CS], band_cnt, drc_param);
}

void mix_pacc_enable(void)
{
    printf("%s\n", __func__);

    //根据使能的链路，重新链接cs链路
    mix_pacc.start_cs = pacc_effect_relink(mix_pacc.pacc_cs, mix_pacc.cs_en, MIX_MIC_PACC_MAX_CS);

    //重新设置链头和链尾的数据格式
    pacc_cs_list_set_io_fmt(mix_pacc.start_cs, false, S32_Q15, S16_Q15_SAT);
}

AT(.text.mic_mix_proc)
void mix_pacc_process(void *obuf, void *ibuf, u32 samples)
{
    pacc_ctl_proc_cs(mix_pacc.pacc_ctl, mix_pacc.start_cs, obuf, ibuf, samples);
}

void mix_pacc_exit(void)
{
    mix_pacc.pacc_ctl = NULL;
    pacc0_ctl_free(PACC0_MIX_MIC);           //麦使用PACC0
}
#endif // ADAPTER_MIX_DRC_EN

#if BT_SCO_EQ_DRC_EN
//------------------------------------------------------------------------------------------
//通话mic EQ/DRC链路

#define SCO_PACC_FRAME_LEN		                60                 //算法处理帧长

static struct {
    void *pacc_ctl;                                                 //pacc硬件模块的控制句柄
    pacc_cs_t *start_cs;                                            //pacc链表处理的表头
    pacc_cs_t pacc_cs[SCO_PACC_MAX_CS];                             //pacc_cs列表
    u8 cs_en[SCO_PACC_MAX_CS];                                      //cs列表使能标志
    u8 cache[SCO_PACC_FRAME_LEN*4];                                 //pacc中间缓存
} sco_pacc AT(.sco_alg_buf.sco_pacc.link);

static bq_bd_t sco_bq_db AT(.sco_alg_buf.sco_pacc);
static drc_bd_t sco_drc_db AT(.sco_alg_buf.sco_pacc);

static const pacc_effect_cb sco_pacc_cb_tbl[SCO_PACC_MAX_CS] = {
    {SCO_PACC_EQ_CS,   PACC_EQ,    false, false,  sco_pacc.cache,        NULL, S16_Q15_SAT, sco_pacc.cache,  S32_Q23, &sco_pacc.pacc_cs[SCO_PACC_EQ_CS],        &sco_bq_db},
    {SCO_PACC_DRC_CS,  PACC_DRC,   false, false,  sco_pacc.cache,        NULL, S32_Q23, sco_pacc.cache,  S16_Q15_SAT, &sco_pacc.pacc_cs[SCO_PACC_DRC_CS],       &sco_drc_db},
};


void sco_pacc_init(void)
{
    printf("%s\n", __func__);

    //temp ram init
    memset(sco_pacc.cache, 0, sizeof(sco_pacc.cache));

    //初始链路模块状态全部关闭
    memset(sco_pacc.cs_en, 0x0, SCO_PACC_MAX_CS);

    //ram init
    memset(&sco_drc_db, 0, sizeof(drc_bd_t));
    memset(&sco_bq_db, 0, sizeof(bq_bd_t));

    sco_pacc.pacc_ctl = pacc0_ctl_alloc(PACC0_SCO_MIC);  //麦使用PACC0
    sco_pacc.start_cs = pacc_effect_link((void*)sco_pacc_cb_tbl, SCO_PACC_MAX_CS, SCO_PACC_FRAME_LEN);
}

static void sco_pacc_eq_set_param(bool is_msbc)
{
    u32 eq_addr, eq_len;

    if (is_msbc) {
        eq_addr = (u32)effect_res_addr_get(EFFECT_IDX_MSC_MSBC_EQ);
        eq_len  = effect_res_len_get(EFFECT_IDX_MSC_MSBC_EQ);
    } else {
        eq_addr = (u32)effect_res_addr_get(EFFECT_IDX_MSC_CSVD_EQ);
        eq_len  = effect_res_len_get(EFFECT_IDX_MSC_CSVD_EQ);
    }

    if (pacc_eq_set_by_res(&sco_pacc.pacc_cs[SCO_PACC_EQ_CS], eq_addr, eq_len) == ERROR_NO) {
        sco_pacc.cs_en[SCO_PACC_EQ_CS] = 1;
    }
}

static void sco_pacc_drc_set_param(bool is_msbc)
{
    u32 drc_addr, drc_len;

    if (is_msbc) {
        drc_addr = (u32)effect_res_addr_get(EFFECT_IDX_MSC_MSBC_DRC);
        drc_len = effect_res_len_get(EFFECT_IDX_MSC_MSBC_DRC);
    } else {
        drc_addr =(u32)effect_res_addr_get(EFFECT_IDX_MSC_CVSD_DRC);
        drc_len = effect_res_len_get(EFFECT_IDX_MSC_CVSD_DRC);
    }

    if(pacc_drc_set_by_res(&sco_pacc.pacc_cs[SCO_PACC_DRC_CS], drc_addr, drc_len) == ERROR_NO) {
        sco_pacc.cs_en[SCO_PACC_DRC_CS] = 1;
    }
}

void sco_pacc_set_param(bool is_msbc)
{
    sco_pacc_eq_set_param(is_msbc);
    sco_pacc_drc_set_param(is_msbc);
}

void sco_pacc_enable(void)
{
    printf("%s\n", __func__);

    //根据使能的链路，重新链接cs链路
    sco_pacc.start_cs = pacc_effect_relink(sco_pacc.pacc_cs, sco_pacc.cs_en, SCO_PACC_MAX_CS);

    //重新设置链头和链尾的数据格式
    pacc_cs_list_set_io_fmt(sco_pacc.start_cs, false, S16_Q15_SAT, S16_Q15_SAT);
}

AT(.bt_voice.sco_pacc)
void sco_pacc_process(u8 *obuf, u8 *ibuf, u32 samples)
{
    pacc_ctl_proc_cs(sco_pacc.pacc_ctl, sco_pacc.start_cs, obuf, ibuf, samples);
}

void sco_pacc_exit(void)
{
    sco_pacc.pacc_ctl = NULL;
    pacc0_ctl_free(PACC0_SCO_MIC);               //麦使用PACC0
}

bool mic_sco_pacc_init(bool is_msbc)
{
    //先初始化PACC链路
    sco_pacc_init();

    //然后设置参数
    sco_pacc_set_param(is_msbc);

    //最后使能PACC
    sco_pacc_enable();

    return (sco_pacc.start_cs != NULL);
}
#endif // BT_SCO_EQ_DRC_EN
