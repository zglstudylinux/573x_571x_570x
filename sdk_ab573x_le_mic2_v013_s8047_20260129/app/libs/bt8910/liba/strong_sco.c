/**********************************************************************
*
*   strong_sco.c
*   定义库里面通话算法部分WEAK函数的Strong函数，动态关闭库代码
***********************************************************************/
#include "include.h"

/*****************************************************************************
 * Module    : AEC算法强定义
 *****************************************************************************/

#if !BT_AEC_EN && !BT_ALC_EN
void aec_process(void) {}
#endif

#if !BT_AEC_EN
AT(.com_text.weak.isr.aec)
bool aec_isr(void) { return false; }
AT(.bt_voice.aec)
void aec_nlms_process(u8 mic_sel) {}
AT(.bt_voice.aec)
void aec_nlp_process(void) {}
void bt_aec_process(u8 *ptr, u32 samples, u32 pcm_mode) {}
void aec_init(void) {}
void aec_exit(void) {}
#endif

#if !BT_ALC_EN
void alc_init(alc_cb_t *alc) {}
void alc_exit(void) {}
void alc_process(s32 *dfw, s32 *xfw) {}
AT(.bt_voice.alc)
void bt_alc_process(u8 *ptr, u32 samples, u32 pcm_mode) {};
#endif


/*****************************************************************************
 * Module    : 通话其他部分强定义
 *****************************************************************************/

#if !BT_SCO_DUMP_EN && !BT_AEC_DUMP_EN && !BT_SCO_FAR_DUMP_EN && !BT_EQ_DUMP_EN && !BT_PLC_DUMP_EN
AT(.bt_voice.sco.dump)
void bt_sco_dump_cb(uint type, void *ptr, uint size) {}
#endif

#if !SDADC_DRC_EN
bool sdadc_drc_v3_init(u8 *drc_addr, int drc_len){return false;}
AT(.com_text.sdadc.drc_v3)
bool sdadc_pacc_drc_calc(s16 *ptr, u32 samples, u32 pcm_mode){return false;}
#endif

/*****************************************************************************
 * Module    : 通话上行降噪算法强定义
 *****************************************************************************/
#if !BT_TRUMPET_NR_EN
void trumpet_denoise_init(u8 level) {}
void trumpet_denoise(int *fft_in) {}
AT(.bt_voice.sco.trumpet)
void bt_sco_trumpet_nr(s32 *fft_in) {}
#endif

#if !BT_SCO_AGC_EN
void bt_agc_init(s32 sampleHzIn, s32 bit, s32 agcDb, s32 agcDbfs) {}
void bt_sco_agc_proc_do(s16 *ptr, int samples) {}
#endif

//#if !BT_SCO_SMIC_AI_EN && !BT_SCO_DMIC_AI_EN
//AT(.bt_voice.sco)
//bool bt_sco_dnn_en(void) {return 0;}
//void dnn_far_upsample(s16 *out, s16 *in, u32 samples, u8 step) {}
//u32 dnn_near_downsample(s16 *ptr, u32 samples) {return 0;}
//#endif

#if !BT_SCO_SMIC_AI_EN
void bt_dnn_init(void *alg_cb) {}
void dnn_sm_process(void) {}
void bt_dnn_exit(void) {}
#endif

#if !BT_SCO_DMIC_AI_EN
void bt_dmns_init(void *alg_cb) {}
void dnn_dm_process(void) {}
void bt_dmns_exit(void) {}
#endif

#if !BT_SCO_NR_USER_SMIC_EN && !BT_SCO_NR_USER_DMIC_EN
void alg_user_process(void) {}
void bt_nr_user_fre_proc(s32 *mic0, s32 *mic1, s32 *far) {}
void bt_sco_nr_user_init(u8 *sysclk, nr_cb_t *nr) {}
void bt_sco_nr_user_exit(void) {}
#endif

#if !BT_SCO_AEC_USER_EN
AT(.bt_voice.soc_nr.far)
void bt_sco_far_get(s16 *buf) {}
void bt_sco_far_cache_init(void) {}
#endif

#if !BT_SCO_NR_EN
int ains3_sm_process(s32 *efw) {return 0;}
void bt_ains3_init(void *alg_cb) {}
void bt_ains3_exit(void) {}
#endif

#if !BT_SCO_AIAEC_DNN_EN
void aiaec_sm_process(void) {}
void bt_aiaec_init(void *alg_cb) {}
void bt_aiaec_exit(void) {}
#endif

#if !BT_SCO_DMIC_AIAEC_EN
void bt_dmdnn_aiaec_init(void *alg_cb) {}
void dmdnn_aiaec_process(void) {}
void bt_dmdnn_aiaec_exit(void) {}
#endif


/*****************************************************************************
 * Module    : 通话下行降噪算法强定义
 *****************************************************************************/
#if !BT_SCO_FAR_NR_EN
AT(.com_text.weak.isr.peri_nr)
void peri_nr_isr(void) {}
AT(.bt_voice.peri_nr)
void bt_sco_far_peri_nr_process(s16 *ptr, u16 samples) {}
void bt_sco_far_nr_int(void *cfg) {}
void bt_peri_nr_exit(void) {}
void bt_peri_nr_init(void) {}
#endif

#if !BT_SCO_CALLING_NR_EN
AT(.bt_voice.sco)
u16 dnr_voice_maxpow_calling(u32 *ptr, u16 len){return 0;}
#endif

#if !BT_SCO_DAC_DRC_EN
bool bt_sco_dac_drc_init(u8 *drc_addr, int drc_len) {return false;}
AT(.bt_voice.dac.drc)
s16 bt_sco_dac_drc_calc(s32 data) {return 0;}
AT(.bt_voice.dac.drc)
void bt_sco_dac_drc_proc(s16 *ptr, u32 samples) {}
#endif

#if !BT_SCO_DAC_DNR_EN
AT(.bt_voice.dac.dnr)
void bt_sco_dac_dnr_proc(s16 *ptr, u32 samples, int dnr_thr) {}
#endif

#if !BT_HFP_EN
void bt_sco_pcm_process(void (*out_func) (s16 *buf)){}
void sco_init_var(void){}

void sco_buffer_init(void){}
void sco_buffer_exit(void){}
#endif
