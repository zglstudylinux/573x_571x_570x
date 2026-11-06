#ifndef __MUSIC_EFFECT_H
#define __MUSIC_EFFECT_H


//step0, eq, dyeq, mdrc
enum {
    MUSIC_PACC_PRE_EQ_SL_CS0,
    MUSIC_PACC_PRE_EQ_SR_CS0,

#if MUSIC_EFFECT_DYEQ_EN
    MUSIC_PACC_DYN_EQ_SL_CS0,
    MUSIC_PACC_DYN_EQ_SR_CS0,
#endif

#if MUSIC_EFFECT_MULTI_DRC_EN
    MUSIC_PACC_BQ0_LP_SL_CS0,
    MUSIC_PACC_BQ0_LP_SR_CS0,
    MUSIC_PACC_DRC_LP_SL_CS0,
    MUSIC_PACC_DRC_LP_SR_CS0,
    MUSIC_PACC_BQ1_LP_SL_CS0,
    MUSIC_PACC_BQ1_LP_SR_CS0,

    MUSIC_PACC_BQ0_BP_SL_CS0,
    MUSIC_PACC_BQ0_BP_SR_CS0,
    MUSIC_PACC_DRC_BP_SL_CS0,
    MUSIC_PACC_DRC_BP_SR_CS0,
    MUSIC_PACC_BQ1_BP_SL_CS0,
    MUSIC_PACC_BQ1_BP_SR_CS0,

    MUSIC_PACC_BQ0_HP_SL_CS0,
    MUSIC_PACC_BQ0_HP_SR_CS0,
    MUSIC_PACC_DRC_HP_SL_CS0,
    MUSIC_PACC_DRC_HP_SR_CS0,
    MUSIC_PACC_BQ1_HP_SL_CS0,
    MUSIC_PACC_BQ1_HP_SR_CS0,
#endif

    MUSIC_PACC_NUM_CS0,
};

//step1, all eq_drc
enum {
    MUSIC_PACC_ALL_EQ_SL_CS1,
    MUSIC_PACC_ALL_EQ_SR_CS1,
    MUSIC_PACC_ALL_DRC_SL_CS1,
    MUSIC_PACC_ALL_DRC_SR_CS1,
    MUSIC_PACC_NUM_CS1,
};

enum {
    MSC_EFFECT_IDLE,
    MSC_EFFECT_ABORT,
    MSC_EFFECT_PROC,
};

typedef struct {
    volatile u8 mute;
    volatile u8 proc_num;
    volatile u8 proc_state;
    u16 samples;
} music_effect_cb_t;

//callback
void music_pacc_set_pre_eq_param(u8 *buf, u32 len, u8 params);
void music_pacc_set_dyeq_param(u8 *buf, u32 len, u8 params);
void music_pacc_set_multi_drc_param(u8 *buf, u32 len, u8 params);
void music_pacc_set_all_drc_param(u8 *buf, u32 len, u8 params);
void music_pacc_set_all_eq_param(u8 *buf, u32 len, u8 params);

//api
void music_effect_proc_cb(void);

void music_effect_init(void);
void music_effect_mute_set(uint8_t mute);   //动态开关pacc 1表示MUTE住（关闭），0表示解MUTE（打开声音）
bool music_effect_is_busy(void);            //返回false则没有在运行
void music_effect_exit(void);

#endif
