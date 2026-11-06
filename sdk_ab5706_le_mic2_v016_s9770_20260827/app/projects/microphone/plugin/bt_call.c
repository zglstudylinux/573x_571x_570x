#include "include.h"

//算法初始化的临时结构体变量
typedef union {
#if BT_AEC_EN
    fdnlms_cb_t fdnlms_cb;
#endif
#if BT_SCO_TX_NS_SEL == NS_AINS3
    ains3_cb_t ains3_cb;
#elif BT_SCO_TX_NS_SEL == NS_AINS4
    ains4_cb_t ains4_cb;
#endif
} sco_alg_cb_t;

static sco_alg_cb_t sco_alg_cb AT(.nr_buf.init);

//#if BT_SCO_FAR_NR_EN
//static peri_nr_cfg_t far_cfg AT(.nr_buf.far);
//#endif

void bt_sco_rec_exit(void);
extern const int sdadc_soft_gain_tbl_64[64];

void bt_sco_code_init(void)
{
    load_code_sco_alg();
    lock_code_bt_voice();
}

void bt_sco_aec_init(u8 *sysclk, aec_cb_t *aec)
{
    aec->far_offset     = xcfg_cb.aec_far_offset;
    aec->dig_gain       = sdadc_soft_gain_tbl_64[xcfg_cb.aec_dig_gain * 2];

#if BT_AEC_EN
    if (xcfg_cb.bt_aec_en) {
        fdnlms_cb_t *fd = &sco_alg_cb.fdnlms_cb;
        memset(fd, 0, sizeof(fdnlms_cb_t));

        aec->type           = AEC_TYPE_DEFAULT;
        fd->xe_add_xcorr    = 16384;
        fd->nlp_suppress    = BT_ECHO_LEVEL;
        fd->nlp_bypass      = 0;
        fd->nlp_part_choose = 1;                           //0:xd, 1:de&xd, 2:xd|xe
        fd->UpBin           = 63;                           //调节最高频率2000Hz
        fd->LowBin          = 7;                            //调节最低频率250Hz
        if (!bt_sco_is_msbc()) {
            fd->UpBin       *= 2;
            fd->LowBin      *= 2;
        }
        fd->qIdx            = 0.35f * (1 << 15); //ref bin,Typical value is 0.1f~1.0f, smaller means more Residual Echo Suppress
        fd->qIdx            = ((int)fd->qIdx * (fd->UpBin - fd->LowBin + 1)) >> 15;	//smaller means more residual echo suppress
        fd->diverge_th      = 3;
        fd->echo_th         = 6666;
        fd->gamma_sm        = 27852;
        fd->arrgFact        = 22937;                    	//bigger means more residual echo suppress
        fd->mu_step         = 60;
        fd->dig_gain        = sdadc_soft_gain_tbl_64[xcfg_cb.aec_dig_gain * 2];

        aec_init(fd);
        *sysclk = *sysclk < SYS_48M ? SYS_48M : *sysclk;
    }
#endif

#if BT_ALC_EN
    if (xcfg_cb.bt_alc_en) {
        alc_cb_t alc;
        memset(&alc, 0, sizeof(alc_cb_t));

        aec->type           = AEC_TYPE_ALC;
        alc->fade_in_delay  = BT_ALC_FADE_IN_DELAY;
        alc->fade_in_step   = BT_ALC_FADE_IN_STEP;
        alc->fade_out_delay = BT_ALC_FADE_OUT_DELAY;
        alc->fade_out_step  = BT_ALC_FADE_OUT_STEP;
        alc->far_voice_thr  = BT_ALC_VOICE_THR;
        if (!bt_sco_is_msbc()) {
            alc->fade_in_delay  >>= 1;
            alc->fade_out_delay >>= 1;
            alc->fade_in_step   <<= 1;
            alc->fade_out_step  <<= 1;
        }

        alc_init(&alc);
        *sysclk = *sysclk < SYS_48M ? SYS_48M : *sysclk;
    }
#endif
}

void bt_sco_far_nr_init(u8 *sysclk, nr_cb_t *nr)
{
#if BT_SCO_FAR_NR_EN
    nr->nr_cfg_en |= NR_CFG_FAR_EN;
    memset(&far_cfg, 0, sizeof(peri_nr_cfg_t));
    nr->far_nr = &far_cfg;
    far_cfg.level = BT_SCO_FAR_NOISE_LEVEL;
    far_cfg.thr   = BT_SCO_FAR_THR;
#endif

#if BT_SCO_AGC_EN
    nr->nr_cfg_en       |= NR_CFG_AGC_EN;

    int bit              = 16;
    int compress_agcDb   = 12;
    int target_agcDbfs   = 3;
    int sampleHzIn       = (bt_sco_is_msbc()) ? (16000) : (8000);

    bt_agc_init(sampleHzIn, bit, compress_agcDb, target_agcDbfs);
#endif

#if BT_SCO_DAC_DRC_EN
    nr->nr_cfg_en |= NR_CFG_DAC_DRC_EN;
    bt_sco_dac_drc_init((u8 *)RES_BUF_EQ_CALL_DAC_DRC, RES_LEN_EQ_CALL_DAC_DRC);
#endif

#if BT_SCO_DAC_DNR_EN
    nr->nr_cfg_en |= NR_CFG_DAC_DNR_EN;
    nr->dac_dnr_thr = BT_SCO_DAC_DNR_THR;
#endif

#if BT_SCO_CALLING_NR_EN
    nr->calling_voice_cnt = BT_SCO_CALLING_VOICE_CNT;
    nr->calling_voice_pow = BT_SCO_CALLING_VOICE_POW;
#endif
}

void bt_sco_nr_init(u8 *sysclk, nr_cb_t *nr)
{
#if BT_TRUMPET_NR_EN
    nr->nr_cfg_en |= NR_CFG_TRUMPET_EN;
    trumpet_denoise_init(BT_TRUMPET_NR_LEVEL);
#endif

#if BT_SCO_FADE_EN
    nr->nr_cfg_en |= NR_CFG_SCO_FADE_EN;
#endif

    bt_sco_near_nr_init(sysclk, nr);

    if (!xcfg_cb.bt_nr_en) {
        nr->nr_type = ((nr->nr_type & ~NR_CFG_TYPE_MASK) | NR_TYPE_NONE);
    }
}

void bt_sco_nr_exit(void)
{
    bt_sco_near_nr_exit();
}

#if BT_SCO_TX_NS_SEL == NS_AINS3
void bt_sco_ains3_init(u8 *sysclk, nr_cb_t *nr)
{
    ains3_cb_t *cb = &sco_alg_cb.ains3_cb;
    memset(cb, 0, sizeof(ains3_cb_t));

    nr->nr_type         = NR_TYPE_AINS3;
    cb->nt              = BT_SCO_AINS3_LEVEL;
    //cb->print_en        = 1;
    cb->prior_opt_idx   = 10;
    cb->ns_ps_rate		= 1;
    cb->trumpet_en		= 0;
	cb->nt_post         = 0;    //0-6 >0才起效 0为不开gain指数化 开的话默认为3
	cb->exp_range	    = 90;

	bt_ains3_init(cb);
    *sysclk = *sysclk < SYS_60M ? SYS_60M : *sysclk;
}
#endif

#if BT_SCO_TX_NS_SEL == NS_AINS4
void bt_sco_ains4_init(u8 *sysclk, nr_cb_t *nr)
{
    ains4_cb_t *cb = &sco_alg_cb.ains4_cb;
    memset(cb, 0, sizeof(ains4_cb_t));

    nr->nr_type             = NR_TYPE_AINS4;
	cb->denoiseBound        = BT_SCO_AINS4_LEVEL; //3000;
	cb->overdrive           = 40960;
	cb->smooth_en           = 1;
	cb->modelUpdatePars0    = 2;//0:no use HIST 1:only update one time first 2：always update
	cb->gainHB_rd           = 32767;//0-32767
	cb->delta_k_up          = 2;
	cb->denoiseBound_fix    = 5;//
	cb->yuan_en             = 0;
	cb->enr_thres           = 0;

	bt_ains4_init(cb);
    *sysclk = *sysclk < SYS_120M ? SYS_120M : *sysclk;
//    *sysclk = SYS_160M;
}
#endif
