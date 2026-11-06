/*
 * 文件名称: music_effect.c
 * 功能描述: 使用PACC1模块，处理EQ/DRC/MDRC等音效
 *
 * 注意事项：PACC1能分时处理不同cs链路，但要避免多线程同时调用pacc_ctl_proc_cs

 ****************************************************************************************
    code : text.msc_effect_proc 264bytes
    buf  : cache_ram(PACC_FRAME_LEN ==32): 1544 bytes; cs: 936 bytes  coef: 8224 bytes
    buf  : cache_ram(PACC_FRAME_LEN ==64): 2568 bytes; cs: 936 bytes  coef: 8224 bytes
    time :
 */
#include "include.h"
#include "effect_idx.h"
#include "music_effect.h"


#if GLOBAL_MUSIC_EFFECT_EN

#define TRACE_EN                1

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#define TRACE_R(...)            print_r(__VA_ARGS__)
#define TRACE_R32(...)          print_r32(__VA_ARGS__)
#else
#define TRACE(...)
#define TRACE_R(...)
#define TRACE_R32(...)
#endif // TRACE_EN


#define PACC_FRAME_LEN		                    16                      //算法处理帧长
#define PROCESS_OUT_SAMPLES                     128                     //每次存取帧长

static music_effect_cb_t music_effect_cb AT(.buf.msc_effect.pcm);       //模块管理结构体
u8 effect_pcm_tempbuf[PROCESS_OUT_SAMPLES*4]  AT(.buf.msc_effect.pcm);  //解码/PCM数据缓存池

static void *pacc_ctl AT(.buf.msc_effect.pacc_list);                    //pacc硬件模块的控制句柄
static pacc_cs_t *pacc_start_cs[2] AT(.buf.msc_effect.pacc_list);       //pacc链表处理的表头

pacc_cs_t pacc_cs0[MUSIC_PACC_NUM_CS0] AT(.buf.msc_effect.link);
u8 pacc_cs0_enable[MUSIC_PACC_NUM_CS0] AT(.buf.msc_effect.link);        //动态调整音频链路模块使能标志

bq_bd_t pre_bq_bd[2]  AT(.buf.msc_effect.link.all_drc);                 //[0]:L [1]:R

#if MUSIC_EFFECT_DYEQ_EN
dybq_sb_t dybq_sb[2] AT(.buf.msc_effect.link.all_drc);                  //[0]:L [1]:R
#endif

#if MUSIC_EFFECT_MULTI_DRC_EN
bq_bd_t lp_bq0_bd[2] AT(.buf.msc_effect.link.all_drc);                  //[0]:L [1]:R
drc_bd_t lp_drc_bd[2] AT(.buf.msc_effect.link.all_drc);                 //[0]:L [1]:R
bq_bd_t lp_bq1_bd[2] AT(.buf.msc_effect.link.all_drc);                  //[0]:L [1]:R
bq_bd_t bp_bq0_bd[2] AT(.buf.msc_effect.link.all_drc);                  //[0]:L [1]:R
drc_bd_t bp_drc_bd[2] AT(.buf.msc_effect.link.all_drc);                 //[0]:L [1]:R
bq_bd_t bp_bq1_bd[2] AT(.buf.msc_effect.link.all_drc);                  //[0]:L [1]:R
bq_bd_t hp_bq0_bd[2] AT(.buf.msc_effect.link.all_drc);                  //[0]:L [1]:R
drc_bd_t hp_drc_bd[2] AT(.buf.msc_effect.link.all_drc);                 //[0]:L [1]:R
bq_bd_t hp_bq1_bd[2] AT(.buf.msc_effect.link.all_drc);                  //[0]:L [1]:R
#endif

pacc_cs_t pacc_cs1[MUSIC_PACC_NUM_CS1] AT(.buf.msc_effect.link);
u8 pacc_cs1_enable[MUSIC_PACC_NUM_CS1] AT(.buf.msc_effect.link);        //动态调整音频链路模块使能标志

drc_bd_t all_drc_bd[2] AT(.buf.msc_effect.link.all_drc);                //[0]:L [1]:R
bq_bd_t all_bq_bd[2] AT(.buf.msc_effect.link.all_drc);                  //[0]:L [1]:R

u8 pacc_cache0[PACC_FRAME_LEN*4 * 2] AT(.buf.msc_effect.cache);         //双声道24bit
u8 pacc_cache1[PACC_FRAME_LEN*4 * 2] AT(.buf.msc_effect.cache);         //双声道24bit
u8 pacc_cache2[PACC_FRAME_LEN*4 * 2] AT(.buf.msc_effect.cache);         //双声道24bit
u8 pacc_cache3[PACC_FRAME_LEN*4 * 2] AT(.buf.msc_effect.cache);         //双声道24bit

static const pacc_effect_cb music_pacc_cb_tbl0[MUSIC_PACC_NUM_CS0] = {
    {MUSIC_PACC_PRE_EQ_SL_CS0,   PACC_EQ, true, false, pacc_cache0,        NULL, S16_Q15_SAT, pacc_cache1,     S32_Q23, &pacc_cs0[MUSIC_PACC_PRE_EQ_SL_CS0],       &pre_bq_bd[0]},
    {MUSIC_PACC_PRE_EQ_SR_CS0,   PACC_EQ, true,  true, pacc_cache0,        NULL, S16_Q15_SAT, pacc_cache1,     S32_Q23, &pacc_cs0[MUSIC_PACC_PRE_EQ_SR_CS0],       &pre_bq_bd[1]},

#if MUSIC_EFFECT_DYEQ_EN
    {MUSIC_PACC_DYN_EQ_SL_CS0, PACC_DYEQ, true, false, pacc_cache1,        NULL,     S32_Q23, pacc_cache1,     S32_Q23, &pacc_cs0[MUSIC_PACC_DYN_EQ_SL_CS0],         &dybq_sb[0]},
    {MUSIC_PACC_DYN_EQ_SR_CS0, PACC_DYEQ, true,  true, pacc_cache1,        NULL,     S32_Q23, pacc_cache1,     S32_Q23, &pacc_cs0[MUSIC_PACC_DYN_EQ_SR_CS0],         &dybq_sb[1]},
#endif

#if MUSIC_EFFECT_MULTI_DRC_EN
    {MUSIC_PACC_BQ0_LP_SL_CS0,   PACC_EQ, true, false, pacc_cache1,        NULL,     S32_Q23, pacc_cache3,     S32_Q23, &pacc_cs0[MUSIC_PACC_BQ0_LP_SL_CS0],       &lp_bq0_bd[0]},
    {MUSIC_PACC_BQ0_LP_SR_CS0,   PACC_EQ, true,  true, pacc_cache1,        NULL,     S32_Q23, pacc_cache3,     S32_Q23, &pacc_cs0[MUSIC_PACC_BQ0_LP_SR_CS0],       &lp_bq0_bd[1]},
    {MUSIC_PACC_DRC_LP_SL_CS0,  PACC_DRC, true, false, pacc_cache3, pacc_cache1,     S32_Q23, pacc_cache0,     S32_Q23, &pacc_cs0[MUSIC_PACC_DRC_LP_SL_CS0],       &lp_drc_bd[0]},
    {MUSIC_PACC_DRC_LP_SR_CS0,  PACC_DRC, true,  true, pacc_cache3, pacc_cache1,     S32_Q23, pacc_cache0,     S32_Q23, &pacc_cs0[MUSIC_PACC_DRC_LP_SR_CS0],       &lp_drc_bd[1]},
    {MUSIC_PACC_BQ1_LP_SL_CS0,   PACC_EQ, true, false, pacc_cache0,        NULL,     S32_Q23, pacc_cache0,     S32_Q23, &pacc_cs0[MUSIC_PACC_BQ1_LP_SL_CS0],       &lp_bq1_bd[0]},
    {MUSIC_PACC_BQ1_LP_SR_CS0,   PACC_EQ, true,  true, pacc_cache0,        NULL,     S32_Q23, pacc_cache0,     S32_Q23, &pacc_cs0[MUSIC_PACC_BQ1_LP_SR_CS0],       &lp_bq1_bd[1]},

    {MUSIC_PACC_BQ0_BP_SL_CS0,   PACC_EQ, true, false, pacc_cache1,        NULL,     S32_Q23, pacc_cache3,     S32_Q23, &pacc_cs0[MUSIC_PACC_BQ0_BP_SL_CS0],       &bp_bq0_bd[0]},
    {MUSIC_PACC_BQ0_BP_SR_CS0,   PACC_EQ, true,  true, pacc_cache1,        NULL,     S32_Q23, pacc_cache3,     S32_Q23, &pacc_cs0[MUSIC_PACC_BQ0_BP_SR_CS0],       &bp_bq0_bd[1]},
    {MUSIC_PACC_DRC_BP_SL_CS0,  PACC_DRC, true, false, pacc_cache3, pacc_cache1,     S32_Q23, pacc_cache2,     S32_Q23, &pacc_cs0[MUSIC_PACC_DRC_BP_SL_CS0],       &bp_drc_bd[0]},
    {MUSIC_PACC_DRC_BP_SR_CS0,  PACC_DRC, true,  true, pacc_cache3, pacc_cache1,     S32_Q23, pacc_cache2,     S32_Q23, &pacc_cs0[MUSIC_PACC_DRC_BP_SR_CS0],       &bp_drc_bd[1]},
    {MUSIC_PACC_BQ1_BP_SL_CS0,   PACC_EQ, true, false, pacc_cache2,        NULL,     S32_Q23, pacc_cache2,     S32_Q23, &pacc_cs0[MUSIC_PACC_BQ1_BP_SL_CS0],       &bp_bq1_bd[0]},
    {MUSIC_PACC_BQ1_BP_SR_CS0,   PACC_EQ, true,  true, pacc_cache2,        NULL,     S32_Q23, pacc_cache2,     S32_Q23, &pacc_cs0[MUSIC_PACC_BQ1_BP_SR_CS0],       &bp_bq1_bd[1]},

    {MUSIC_PACC_BQ0_HP_SL_CS0,   PACC_EQ, true, false, pacc_cache1,        NULL,     S32_Q23, pacc_cache3,     S32_Q23, &pacc_cs0[MUSIC_PACC_BQ0_HP_SL_CS0],       &hp_bq0_bd[0]},
    {MUSIC_PACC_BQ0_HP_SR_CS0,   PACC_EQ, true,  true, pacc_cache1,        NULL,     S32_Q23, pacc_cache3,     S32_Q23, &pacc_cs0[MUSIC_PACC_BQ0_HP_SR_CS0],       &hp_bq0_bd[1]},
    {MUSIC_PACC_DRC_HP_SL_CS0,  PACC_DRC, true, false, pacc_cache3, pacc_cache1,     S32_Q23, pacc_cache1,     S32_Q23, &pacc_cs0[MUSIC_PACC_DRC_HP_SL_CS0],       &hp_drc_bd[0]},
    {MUSIC_PACC_DRC_HP_SR_CS0,  PACC_DRC, true,  true, pacc_cache3, pacc_cache1,     S32_Q23, pacc_cache1,     S32_Q23, &pacc_cs0[MUSIC_PACC_DRC_HP_SR_CS0],       &hp_drc_bd[1]},
    {MUSIC_PACC_BQ1_HP_SL_CS0,   PACC_EQ, true, false, pacc_cache1,        NULL,     S32_Q23, pacc_cache1,     S32_Q23, &pacc_cs0[MUSIC_PACC_BQ1_HP_SL_CS0],       &hp_bq1_bd[0]},
    {MUSIC_PACC_BQ1_HP_SR_CS0,   PACC_EQ, true,  true, pacc_cache1,        NULL,     S32_Q23, pacc_cache1,     S32_Q23, &pacc_cs0[MUSIC_PACC_BQ1_HP_SR_CS0],       &hp_bq1_bd[1]},
#endif
};

static const pacc_effect_cb music_pacc_cb_tbl1[MUSIC_PACC_NUM_CS1] = {
    {MUSIC_PACC_ALL_EQ_SL_CS1,   PACC_EQ, true, false, pacc_cache1,        NULL,     S32_Q23, pacc_cache1,     S32_Q23, &pacc_cs1[MUSIC_PACC_ALL_EQ_SL_CS1],       &all_bq_bd[0]},
    {MUSIC_PACC_ALL_EQ_SR_CS1,   PACC_EQ, true,  true, pacc_cache1,        NULL,     S32_Q23, pacc_cache1,     S32_Q23, &pacc_cs1[MUSIC_PACC_ALL_EQ_SR_CS1],       &all_bq_bd[1]},
    {MUSIC_PACC_ALL_DRC_SL_CS1, PACC_DRC, true, false, pacc_cache1,        NULL,     S32_Q23, pacc_cache0, S16_Q15_SAT, &pacc_cs1[MUSIC_PACC_ALL_DRC_SL_CS1],      &all_drc_bd[0]},
    {MUSIC_PACC_ALL_DRC_SR_CS1, PACC_DRC, true,  true, pacc_cache1,        NULL,     S32_Q23, pacc_cache0, S16_Q15_SAT, &pacc_cs1[MUSIC_PACC_ALL_DRC_SR_CS1],      &all_drc_bd[1]},
};

AT(.text.msc_pacc.init)
static void music_pacc_init(void)
{
    printf("%s\n", __func__);
    memset(effect_pcm_tempbuf, 0, sizeof(effect_pcm_tempbuf));

    //temp ram init
    memset(pacc_cache0, 0, sizeof(pacc_cache0));
    memset(pacc_cache1, 0, sizeof(pacc_cache1));
    memset(pacc_cache2, 0, sizeof(pacc_cache2));
    memset(pacc_cache3, 0, sizeof(pacc_cache3));

    //初始链路模块状态全部打开
    memset(pacc_cs0_enable, 0x1, MUSIC_PACC_NUM_CS0);
    memset(pacc_cs1_enable, 0x1, MUSIC_PACC_NUM_CS1);

    //link0 ram init
    memset(&pre_bq_bd[0], 0, sizeof(bq_bd_t));
    memset(&pre_bq_bd[1], 0, sizeof(bq_bd_t));

#if MUSIC_EFFECT_DYEQ_EN
    memset(&dybq_sb[0], 0, sizeof(dybq_sb_t));
    memset(&dybq_sb[1], 0, sizeof(dybq_sb_t));
#endif

#if MUSIC_EFFECT_MULTI_DRC_EN
    memset(&lp_bq0_bd[0], 0, sizeof(bq_bd_t));
    memset(&lp_bq0_bd[1], 0, sizeof(bq_bd_t));
    lp_bq0_bd[0].band_cnt = 4;
    lp_bq0_bd[1].band_cnt = 4;

    memset(&lp_drc_bd[0], 0, sizeof(drc_bd_t));
    memset(&lp_drc_bd[1], 0,  sizeof(drc_bd_t));

    memset(&lp_bq1_bd[0], 0, sizeof(bq_bd_t));
    memset(&lp_bq1_bd[1], 0, sizeof(bq_bd_t));
    lp_bq1_bd[0].band_cnt = 4;
    lp_bq1_bd[1].band_cnt = 4;

    memset(&bp_bq0_bd[0], 0, sizeof(bq_bd_t));
    memset(&bp_bq0_bd[1], 0, sizeof(bq_bd_t));
    bp_bq0_bd[0].band_cnt = 4;
    bp_bq0_bd[1].band_cnt = 4;

    memset(&bp_drc_bd[0], 0, sizeof(drc_bd_t));
    memset(&bp_drc_bd[1], 0,  sizeof(drc_bd_t));

    memset(&bp_bq1_bd[0], 0, sizeof(bq_bd_t));
    memset(&bp_bq1_bd[1], 0, sizeof(bq_bd_t));
    bp_bq1_bd[0].band_cnt = 4;
    bp_bq1_bd[1].band_cnt = 4;

    memset(&hp_bq0_bd[0], 0, sizeof(bq_bd_t));
    memset(&hp_bq0_bd[1], 0, sizeof(bq_bd_t));
    hp_bq0_bd[0].band_cnt = 4;
    hp_bq0_bd[1].band_cnt = 4;

    memset(&hp_drc_bd[0], 0, sizeof(drc_bd_t));
    memset(&hp_drc_bd[1], 0,  sizeof(drc_bd_t));

    memset(&hp_bq1_bd[0], 0, sizeof(bq_bd_t));
    memset(&hp_bq1_bd[1], 0, sizeof(bq_bd_t));
    hp_bq1_bd[0].band_cnt = 4;
    hp_bq1_bd[1].band_cnt = 4;
#endif

    //link1 ram init
    memset(&all_bq_bd[0], 0, sizeof(bq_bd_t));
    memset(&all_bq_bd[1], 0, sizeof(bq_bd_t));

    memset(&all_drc_bd[0], 0, sizeof(drc_bd_t));
    memset(&all_drc_bd[1], 0,  sizeof(drc_bd_t));

    pacc_ctl = pacc_effect_init(1); //音乐使用PACC1
    pacc_start_cs[0] = pacc_effect_link((void*)music_pacc_cb_tbl0, MUSIC_PACC_NUM_CS0, PACC_FRAME_LEN);
    pacc_start_cs[1] = pacc_effect_link((void*)music_pacc_cb_tbl1, MUSIC_PACC_NUM_CS1, PACC_FRAME_LEN);
}

//清空cs的zx 并出新初始化一下硬件模块相关寄存器
AT(.text.msc_pacc.init)
static void music_pacc_reinit(void)
{
    //clser zx ram
#if MUSIC_EFFECT_DYEQ_EN
    memset(dybq_sb[0].zx, 0, (4* (2*(DYBQ_BAND_MAX_NB + 1) *4)));
    memset(dybq_sb[1].zx, 0, (4* (2*(DYBQ_BAND_MAX_NB + 1) *4)));
#endif
#if MUSIC_EFFECT_MULTI_DRC_EN
    memset(lp_bq0_bd[0].zx, 0, ( 4*BQ_MAX_CH*(2*(BQ_BAND_MAX_NB+1)) ));
    memset(lp_bq0_bd[1].zx, 0, ( 4*BQ_MAX_CH*(2*(BQ_BAND_MAX_NB+1)) ));
    memset(lp_bq1_bd[0].zx, 0, ( 4*BQ_MAX_CH*(2*(BQ_BAND_MAX_NB+1)) ));
    memset(lp_bq1_bd[1].zx, 0, ( 4*BQ_MAX_CH*(2*(BQ_BAND_MAX_NB+1)) ));

    memset(bp_bq0_bd[0].zx, 0, ( 4*BQ_MAX_CH*(2*(BQ_BAND_MAX_NB+1)) ));
    memset(bp_bq0_bd[1].zx, 0, ( 4*BQ_MAX_CH*(2*(BQ_BAND_MAX_NB+1)) ));
    memset(bp_bq1_bd[0].zx, 0, ( 4*BQ_MAX_CH*(2*(BQ_BAND_MAX_NB+1)) ));
    memset(bp_bq1_bd[1].zx, 0, ( 4*BQ_MAX_CH*(2*(BQ_BAND_MAX_NB+1)) ));

    memset(hp_bq0_bd[0].zx, 0, ( 4*BQ_MAX_CH*(2*(BQ_BAND_MAX_NB+1)) ));
    memset(hp_bq0_bd[1].zx, 0, ( 4*BQ_MAX_CH*(2*(BQ_BAND_MAX_NB+1)) ));
    memset(hp_bq1_bd[0].zx, 0, ( 4*BQ_MAX_CH*(2*(BQ_BAND_MAX_NB+1)) ));
    memset(hp_bq1_bd[1].zx, 0, ( 4*BQ_MAX_CH*(2*(BQ_BAND_MAX_NB+1)) ));
#endif
    memset(pre_bq_bd[0].zx, 0, ( 4*BQ_MAX_CH*(2*(BQ_BAND_MAX_NB+1)) ));
    memset(pre_bq_bd[1].zx, 0, ( 4*BQ_MAX_CH*(2*(BQ_BAND_MAX_NB+1)) ));

    memset(all_bq_bd[0].zx, 0, ( 4*BQ_MAX_CH*(2*(BQ_BAND_MAX_NB+1)) ));
    memset(all_bq_bd[1].zx, 0, ( 4*BQ_MAX_CH*(2*(BQ_BAND_MAX_NB+1)) ));

    pacc_ctl = pacc_effect_init(1); //音乐使用PACC1
}

void music_pacc_relink(void)
{
    printf("%s\n", __func__);
    pacc_start_cs[0] = pacc_effect_relink(pacc_cs0, pacc_cs0_enable, MUSIC_PACC_NUM_CS0);
    pacc_start_cs[1] = pacc_effect_relink(pacc_cs1, pacc_cs1_enable, MUSIC_PACC_NUM_CS1);
}

static void music_pacc_exit(void)
{
    pacc_effect_exit(1);                    //音乐使用PACC1
}

//------------------------------------------------------------------------------------------
void music_pacc_set_all_drc_param(u8 *buf, u32 len, u8 params)
{
//    tool_drc_coef_t *tool_drc_coef = (tool_drc_coef_t *)(buf);
    //链路最后一个cs要控制输出的ran和类型 由于当前sdk的数据流类型不是统一的 所以第一个和最后一个cs不做bypass处理
    //使用disable的默认曲线来bypass
//    if (pacc_cs1_enable[MUSIC_PACC_ALL_DRC_SL_CS1] != tool_drc_coef->enable) {            //用左通道判断
//        pacc_cs1_enable[MUSIC_PACC_ALL_DRC_SL_CS1] = tool_drc_coef->enable;
//        pacc_cs1_enable[MUSIC_PACC_ALL_DRC_SL_CS1] = tool_drc_coef->enable;
//        music_pacc_relink();
//    }
//
//    if (!tool_drc_coef->enable) {
//        return;
//    }
    pacc_effect_set_drc_param(&all_drc_bd[0].drc_coef, &buf[0], params);
    pacc_effect_set_drc_param(&all_drc_bd[1].drc_coef, &buf[0], params);

}

void music_pacc_set_all_eq_param(u8 *buf, u32 len, u8 params)
{
    tool_eq_coef_t *tool_eq_coef = (tool_eq_coef_t *)(buf);

    pacc_effect_set_eq_param(all_bq_bd[0].coef, &buf[0], params);
    pacc_effect_set_eq_param(all_bq_bd[1].coef, &buf[0], params);

    if (pacc_cs1_enable[MUSIC_PACC_ALL_EQ_SL_CS1] != tool_eq_coef->enable) {            //用左通道判断
        pacc_cs1_enable[MUSIC_PACC_ALL_EQ_SL_CS1] = tool_eq_coef->enable;
        pacc_cs1_enable[MUSIC_PACC_ALL_EQ_SR_CS1] = tool_eq_coef->enable;
        music_pacc_relink();
    }
}

void music_pacc_set_pre_eq_param(u8 *buf, u32 len, u8 params)
{
//    tool_eq_coef_t *tool_eq_coef = (tool_eq_coef_t *)(buf);
    //链路首尾cs要控制输出的ran和类型 由于当前sdk的数据流类型不是统一的 所以第一个和最后一个cs不做bypass处理
    //使用disable的默认曲线来bypass
//    if (pacc_cs0_enable[MUSIC_PACC_PRE_EQ_SL_CS0] != tool_eq_coef->enable) {            //用左通道判断
//        pacc_cs0_enable[MUSIC_PACC_PRE_EQ_SL_CS0] = tool_eq_coef->enable;
//        pacc_cs0_enable[MUSIC_PACC_PRE_EQ_SR_CS0] = tool_eq_coef->enable;
//        music_pacc_relink();
//    }
//
//    if (!tool_eq_coef->enable) {
//        return;
//    }

    pacc_effect_set_eq_param(pre_bq_bd[0].coef, &buf[0], params);
    pacc_effect_set_eq_param(pre_bq_bd[1].coef, &buf[0], params);
}

void music_pacc_set_dyeq_param(u8 *buf, u32 len, u8 params)
{
#if MUSIC_EFFECT_DYEQ_EN
    tool_dyeq_coef_t *tool_dyeq_coef = (tool_dyeq_coef_t *)(buf);

    pacc_effect_set_dyeq_param(&dybq_sb[0].drc_coef, &buf[2], params);
    pacc_effect_set_dyeq_param(&dybq_sb[1].drc_coef, &buf[2], params);

    if (pacc_cs0_enable[MUSIC_PACC_DYN_EQ_SL_CS0] != tool_dyeq_coef->enable) {            //用左通道判断
        pacc_cs0_enable[MUSIC_PACC_DYN_EQ_SL_CS0] = tool_dyeq_coef->enable;
        pacc_cs0_enable[MUSIC_PACC_DYN_EQ_SR_CS0] = tool_dyeq_coef->enable;
        music_pacc_relink();
    }
#endif
}

void music_pacc_set_multi_drc_param(u8 *buf, u32 len, u8 params)
{
#if MUSIC_EFFECT_MULTI_DRC_EN
    if (!params) {
        pacc_effect_set_drc_param(&lp_drc_bd[0].drc_coef, &buf[18], params);
        pacc_effect_set_drc_param(&lp_drc_bd[1].drc_coef, &buf[18], params);

        pacc_effect_set_drc_param(&bp_drc_bd[0].drc_coef, &buf[18+78], params);
        pacc_effect_set_drc_param(&bp_drc_bd[1].drc_coef, &buf[18+78], params);

        pacc_effect_set_drc_param(&hp_drc_bd[0].drc_coef, &buf[18+78+78], params);
        pacc_effect_set_drc_param(&hp_drc_bd[1].drc_coef, &buf[18+78+78], params);

        pacc_effect_set_mbdrc_bq_param(lp_bq0_bd[0].coef, lp_bq1_bd[0].coef, &buf[16+78*3+2], params, 0);
        pacc_effect_set_mbdrc_bq_param(lp_bq0_bd[1].coef, lp_bq1_bd[1].coef, &buf[16+78*3+2], params, 0);
        pacc_effect_set_mbdrc_bq_param(bp_bq0_bd[0].coef, bp_bq1_bd[0].coef, &buf[16+78*3+2 + 94+2], params, 1);
        pacc_effect_set_mbdrc_bq_param(bp_bq0_bd[1].coef, bp_bq1_bd[1].coef, &buf[16+78*3+2 + 94+2], params, 1);
        pacc_effect_set_mbdrc_bq_param(hp_bq0_bd[0].coef, hp_bq1_bd[0].coef, &buf[16+78*3+2 + 94+2+94+2], params, 0);
        pacc_effect_set_mbdrc_bq_param(hp_bq0_bd[1].coef, hp_bq1_bd[1].coef, &buf[16+78*3+2 + 94+2+94+2], params, 0);

    } else {
        tool_multi_drc_v3_coef_t *tool_multi_drc_v3_coef = (tool_multi_drc_v3_coef_t *)(buf + 14);
        printf("type %d\n",tool_multi_drc_v3_coef->type);
        switch (tool_multi_drc_v3_coef->type) {
            case 0:
                pacc_effect_set_drc_param(&lp_drc_bd[0].drc_coef, &buf[5+14], params);
                pacc_effect_set_drc_param(&lp_drc_bd[1].drc_coef, &buf[5+14], params);
                break;
            case 1:
                pacc_effect_set_drc_param(&bp_drc_bd[0].drc_coef, &buf[5+14], params);
                pacc_effect_set_drc_param(&bp_drc_bd[1].drc_coef, &buf[5+14], params);
                break;
            case 2:
                pacc_effect_set_drc_param(&hp_drc_bd[0].drc_coef, &buf[5+14], params);
                pacc_effect_set_drc_param(&hp_drc_bd[1].drc_coef, &buf[5+14], params);
                break;
            case 3:
                pacc_effect_set_mbdrc_bq_param(lp_bq0_bd[0].coef, lp_bq1_bd[0].coef, &buf[5+14], params, 0);
                pacc_effect_set_mbdrc_bq_param(lp_bq0_bd[1].coef, lp_bq1_bd[1].coef, &buf[5+14], params, 0);
                break;
            case 4:
                pacc_effect_set_mbdrc_bq_param(bp_bq0_bd[0].coef, bp_bq1_bd[0].coef, &buf[5+14], params, 1);
                pacc_effect_set_mbdrc_bq_param(bp_bq0_bd[1].coef, bp_bq1_bd[1].coef, &buf[5+14], params, 1);
                break;
            case 5:
                pacc_effect_set_mbdrc_bq_param(hp_bq0_bd[0].coef, hp_bq1_bd[0].coef, &buf[5+14], params, 0);
                pacc_effect_set_mbdrc_bq_param(hp_bq0_bd[1].coef, hp_bq1_bd[1].coef, &buf[5+14], params, 0);
                break;
            default:
                break;
        }
    }

    u8 en = buf[1 + 14];
    if (pacc_cs0_enable[MUSIC_PACC_BQ0_LP_SL_CS0] != en) {            //用左通道判断
        memset(&pacc_cs0_enable[MUSIC_PACC_BQ0_LP_SL_CS0], en, 6*3);
        music_pacc_relink();
    }
#endif
}

static bool music_pacc_set_res_param(uint effect_idx)
{
    u32 res_crc, cal_crc;
    u8 offset = effect_info_offset_get(effect_idx);
    u8 *res_addr = effect_res_addr_get(effect_idx);
    u32 res_len = effect_res_len_get(effect_idx);

    if(res_addr == NULL || res_len == 0 || offset >= CFG_MAX) {
        return false;
    }

    //检查校验码，避免工具生成的数据异常
    cal_crc = calc_crc(res_addr, res_len-2, 0xffff); //计算crc
    res_crc = little_endian_read_16(res_addr, res_len-2);//读取上位机的crc
    if (cal_crc != res_crc) {
        TRACE("%s CRC ERROR 0x%x 0x%x\n", effect_info[offset].effect_cfg_name, cal_crc, res_crc);
        return false;
    }

    if(effect_update_callback_tbl[offset].effect_update_callback){
        effect_update_callback_tbl[offset].effect_update_callback(res_addr, res_len, 0);
        return true;
    }

    return false;
}

//上电系统初始化中调用
AT(.text.effect_adjust_by_res_init)WEAK
void music_pacc_set_params(void)
{
    TRACE("%s\n", __func__);

    music_pacc_set_res_param(EFFECT_IDX_MSC_PRE_EQ);
    music_pacc_set_res_param(EFFECT_IDX_MSC_DY_EQ);
    music_pacc_set_res_param(EFFECT_IDX_MSC_MULT_DRC);
    music_pacc_set_res_param(EFFECT_IDX_MSC_ALL_EQ);
    music_pacc_set_res_param(EFFECT_IDX_MSC_ALL_DRC);

    music_pacc_relink();
}

//------------------------------------------------------------------------------------------
AT(.text.msc_effect_init) WEAK
void music_effect_init(void)
{
    memset(&music_effect_cb, 0, sizeof(music_effect_cb_t));
    music_effect_cb.mute    = 1;

    os_sem0_reset();

    //先初始化PACC链路
    music_pacc_init();

    //然后设置参数
    music_pacc_set_params();

    //使能PACC
    music_pacc_reinit();

    //最后打开音频数据流
    music_effect_cb.mute    = 0;
}

AT(.text.msc_effect_proc)
bool music_effect_is_busy(void)
{
    return (music_effect_cb.proc_state > MSC_EFFECT_ABORT);
}

void music_effect_exit(void)
{
    music_effect_cb.mute = 1;       //先停掉新的音频数据流
    while(music_effect_is_busy());  //等当前帧跑完
    music_pacc_exit();              //再关闭pacc
}

//动态开关pacc 1表示关闭  0表示打开
void music_effect_mute_set(uint8_t mute)
{
    TRACE("%s, %d\n", __func__, mute);

    if(mute == music_effect_cb.mute) {      //避免在跑的过程设置mute=0，os_sem0_reset引起信号量丢失
        return;
    }
    if(mute) {
        music_effect_cb.mute = mute;        //先停掉新的音频数据流
        while(music_effect_is_busy());      //等当前帧跑完
        music_pacc_exit();                  //再关闭pacc
    } else {
        os_sem0_reset();
        music_pacc_reinit();                //先重新初始化一下pacc
        music_effect_cb.mute    = mute;     //再打开音频数据流
    }
}

//AT(.com_text.msc_effect_proc)
//const char music_effect_info[] = "MUSIC samples = %d, isrcnt = %d (SR_%d)\n";

//蓝牙SBC音乐解码回调
AT(.text.msc_effect_proc)
void music_effect_pcm_input(u8 *buf, u32 samples)
{
    bool result = false;
    if(func_cb.sta == FUNC_BT) {
        if(!a2dp_is_bypass() && !music_effect_cb.mute) {
            memcpy(effect_pcm_tempbuf, buf, samples*4);
            music_effect_cb.samples = samples;
            music_effect_cb.proc_num = samples/PACC_FRAME_LEN;
            music_effect_cb.proc_state = MSC_EFFECT_PROC;

            kick_music_effect_proc();           //触发线程处理第1个片段
            os_sem0_pend();

            result = (bool)(music_effect_cb.proc_state == MSC_EFFECT_IDLE);
        }
    } else if(func_cb.sta == FUNC_MUSIC) {
        if(!music_effect_cb.mute) {
            memcpy(effect_pcm_tempbuf, buf, samples*4);
            music_effect_cb.samples = samples;
            music_effect_cb.proc_num = samples/PACC_FRAME_LEN;
            music_effect_cb.proc_state = MSC_EFFECT_PROC;

            kick_music_effect_proc();           //触发线程处理第1个片段
            os_sem0_pend();

            result = (bool)(music_effect_cb.proc_state == MSC_EFFECT_IDLE);
        }
    } else {

    }

    if(result) {
        memcpy(buf, effect_pcm_tempbuf, samples*4);
    } else {
        memset(buf, 0, samples*4);
    }
}

AT(.text.msc_effect_proc)
void music_effect_proc_cb(void)
{
    uint samples = music_effect_cb.samples;
    bool is_bypass = false;
    if(func_cb.sta == FUNC_BT) {
        is_bypass = a2dp_is_bypass();
    } else if(func_cb.sta == FUNC_MUSIC) {

    } else {

    }

    if(is_bypass || music_effect_cb.mute) {
        music_effect_cb.proc_state = MSC_EFFECT_ABORT;
        os_sem0_post();
    } else {
        uint proc_num = music_effect_cb.proc_num;
        memcpy(pacc_cache0, &effect_pcm_tempbuf[(samples/PACC_FRAME_LEN - proc_num) * PACC_FRAME_LEN*4], PACC_FRAME_LEN*4);//单声道16bit
        pacc_effect_process(pacc_ctl, pacc_start_cs[0]);

#if MUSIC_EFFECT_MULTI_DRC_EN
        //MULTI_DRC中间结果需要软件参与计算
        if (pacc_cs0_enable[MUSIC_PACC_BQ0_LP_SL_CS0] == 1) {            //多段drc打开
            //SOFT_MIX to RAM1 (S32Q23)
            s32 *rptr0 = (s32 *)pacc_cache0;
            s32 *rptr1 = (s32 *)pacc_cache1;
            s32 *rptr2 = (s32 *)pacc_cache2;
            for(int i = 0;i < PACC_FRAME_LEN*2;i++){
                rptr1[i] = rptr0[i] + rptr1[i] + rptr2[i];
            }
        }
#endif
        pacc_effect_process(pacc_ctl, pacc_start_cs[1]);
        memcpy(&effect_pcm_tempbuf[(samples/PACC_FRAME_LEN - proc_num) * PACC_FRAME_LEN*4], pacc_cache0, PACC_FRAME_LEN*4);//单声道16bit

        music_effect_cb.proc_num--;
        if(music_effect_cb.proc_num > 0) {
            kick_music_effect_proc();       //还有片段未处理完，继续处理
        } else {
            music_effect_cb.proc_state = MSC_EFFECT_IDLE;
            os_sem0_post();
        }
    }
}


AT(.sbcdec.pcm)
bool sbc_pcm_out_process(u32 samples, bool is_tws)
{
    u8 *buf = (u8 *)AUDMAADR;

    music_effect_pcm_input(buf, samples);

    if (!is_tws) {
        return false;//走下一个流程 aubuf0_dma_kick DAC_DMA输出
    }

    return true;
}
//mp3解码音效处理
#if 0
u8 effect_pcm_tempbuf_extra[576*4 *2] AT(.mp3buf.dec); //576立体声 pinpongbuf
volatile s8 mp3_effect_temp_in_cnt = 0;
volatile s8 mp3_effect_temp_out_cnt = 1;
u32 mp3_effect_samples = 0;

AT(.mp3dec.pcm)
void mp3_pcm_out_start(void)
{
    u32 aubufsize = (AUBUF0SIZE & 0xffff) + 1;
    u32 limit = 576;
    //越界保护
    if (aubufsize > limit) {                                //借用AUBUF后半部分, 故要检查下
        return;
    }

    if (!(AUCON0 & BIT(26))) {
        AUDMAADR = DMA_ADR(&dac_obuf[aubufsize]);
        AUCON0 |= BIT(26);                                  //MPEG DMA to RAM
        aubuf0_dma_init();
        memset(effect_pcm_tempbuf_extra, 0, 576*4 *2);
    }
}

AT(.mp3dec.pcm)
void alg_music_effect_proc_cb(void)
{
    u32 i = 0;
    u32 proc_cnt = mp3_effect_samples/PROCESS_OUT_SAMPLES;
    u32 proc_remainder = mp3_effect_samples%PROCESS_OUT_SAMPLES;
    //PROCESS_OUT_SAMPLES = 128， 每次处理128*4（立体声）byte数据
    for(i=0; i<proc_cnt; i++) {
        music_effect_pcm_input(&effect_pcm_tempbuf_extra[(mp3_effect_temp_in_cnt * 576*4) + 512*i], PROCESS_OUT_SAMPLES);
    }
    if(proc_remainder!=0) {
        music_effect_pcm_input(&effect_pcm_tempbuf_extra[(mp3_effect_temp_in_cnt * 576*4) + 512*i], proc_remainder);
    }
}

AT(.mp3dec.pcm)
void mp3_pcm_out_process(u32 samples)
{
    mp3_effect_samples = samples;
    u8 *buf = (u8 *)AUDMAADR;
    u8 *rptr = NULL;
    //in tempbuf
    mp3_effect_temp_in_cnt++;
    if(mp3_effect_temp_in_cnt > 1) {
        mp3_effect_temp_in_cnt = 0;
    }
    memcpy(&effect_pcm_tempbuf_extra[mp3_effect_temp_in_cnt*576*4], buf, samples*4);

    //把本次解码数据推低线程处理音效
    kick_mp3_effect();

    if (samples == 576 || samples == 384) {
        //拿上次的数据推DAC
        mp3_effect_temp_out_cnt++;
        if(mp3_effect_temp_out_cnt > 1) {
            mp3_effect_temp_out_cnt = 0;
        }
        rptr = &effect_pcm_tempbuf_extra[(mp3_effect_temp_out_cnt)*576*4];
        if (((AUCON0 >> 8) & (0x3)) == 0x3 ) { //single
            aubuf0_dma_kick(rptr, samples, 1);
            aubuf0_dma_w4_done();
        } else {
            aubuf0_dma_kick(rptr, samples, 2);
            aubuf0_dma_w4_done();
        }

    } else {
//        printf("^");
    }

}

#else
AT(.mp3dec.pcm)
void alg_music_effect_proc_cb(void){}
#endif
#else

bool music_effect_is_busy(void){return false;}
void music_pacc_set_pre_eq_param(u8 *buf, u32 len, u8 params){}
void music_pacc_set_dyeq_param(u8 *buf, u32 len, u8 params){}
void music_pacc_set_multi_drc_param(u8 *buf, u32 len, u8 params){}
void music_pacc_set_all_eq_param(u8 *buf, u32 len, u8 params){}
void music_pacc_set_all_drc_param(u8 *buf, u32 len, u8 params){}
void alg_music_effect_proc_cb(void){}

#endif
