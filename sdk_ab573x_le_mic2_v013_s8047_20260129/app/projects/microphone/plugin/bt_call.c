#include "include.h"

#if BT_SCO_SMIC_AI_EN
    static dnn_cb_t dnn_cb AT(.dnn_buf.init);
#elif BT_SCO_DMIC_AI_EN
    static dmns_cb_t dmns_cb AT(.dmdnn_data.init);
#elif BT_SCO_NR_EN
    static ains3_cb_t ains3_cb AT(.ains_buf);
#elif BT_SCO_AIAEC_DNN_EN
    static dnn_aec_ns_cb_t aiaec_dnn_cb AT(.aiaec_buf.sta);
#elif BT_SCO_DMIC_AIAEC_EN
    static dmdnn_aiaec_cb_t dmdnn_aiaec_cb AT(.dmdnn_aiaec_buf.init);
#endif

#if BT_SCO_FAR_NR_EN
static peri_nr_cfg_t far_cfg AT(.nr_buf.far);
#endif

void bt_sco_agc_init(void);
void bt_sco_rec_exit(void);

//每档0.5DB
AT(.rodata.sdadc)
const int aec_dig_gain_tbl[64] = {
    AEC_DIG_P0DB,  AEC_DIG_P0_5DB,  AEC_DIG_P1DB,  AEC_DIG_P1_5DB,  AEC_DIG_P2DB,  AEC_DIG_P2_5DB,  AEC_DIG_P3DB,  AEC_DIG_P3_5DB,
    AEC_DIG_P4DB,  AEC_DIG_P4_5DB,  AEC_DIG_P5DB,  AEC_DIG_P5_5DB,  AEC_DIG_P6DB,  AEC_DIG_P6_5DB,  AEC_DIG_P7DB,  AEC_DIG_P7_5DB,
    AEC_DIG_P8DB,  AEC_DIG_P8_5DB,  AEC_DIG_P9DB,  AEC_DIG_P9_5DB,  AEC_DIG_P10DB, AEC_DIG_P10_5DB, AEC_DIG_P11DB, AEC_DIG_P11_5DB,
    AEC_DIG_P12DB, AEC_DIG_P12_5DB, AEC_DIG_P13DB, AEC_DIG_P13_5DB, AEC_DIG_P14DB, AEC_DIG_P14_5DB, AEC_DIG_P15DB, AEC_DIG_P15_5DB,
    AEC_DIG_P16DB, AEC_DIG_P16_5DB, AEC_DIG_P17DB, AEC_DIG_P17_5DB, AEC_DIG_P18DB, AEC_DIG_P18_5DB, AEC_DIG_P19DB, AEC_DIG_P19_5DB,
    AEC_DIG_P20DB, AEC_DIG_P20_5DB, AEC_DIG_P21DB, AEC_DIG_P21_5DB, AEC_DIG_P22DB, AEC_DIG_P22_5DB, AEC_DIG_P23DB, AEC_DIG_P23_5DB,
    AEC_DIG_P24DB, AEC_DIG_P24_5DB, AEC_DIG_P25DB, AEC_DIG_P25_5DB, AEC_DIG_P26DB, AEC_DIG_P26_5DB, AEC_DIG_P27DB, AEC_DIG_P27_5DB,
    AEC_DIG_P28DB, AEC_DIG_P28_5DB, AEC_DIG_P29DB, AEC_DIG_P29_5DB, AEC_DIG_P30DB, AEC_DIG_P30_5DB, AEC_DIG_P31DB, AEC_DIG_P31_5DB,
};

void bt_sco_code_init(void)
{
    if (!bt_sco_is_msbc()){
        unlock_dcode();
    }
    load_code_bt_voice();
//    load_code_sco_comm();
}

void bt_sco_aec_init(u8 *sysclk, aec_cfg_t *aec)
{
    aec->far_offset         = 0;//xcfg_cb.aec_far_offset;       //声加1+1会用到这个参数，单独放到外面初始化
    aec->ff_mic_ref_en      = BT_AEC_FF_MIC_REF_EN;

#if BT_AEC_EN
    if (xcfg_cb.bt_aec_en) {
        aec->type           = AEC_TYPE_DEFAULT;
        aec->mode           = 1;
        aec->nlp_bypass     = BT_AEC_NLP_BYPASS_EN;
        aec->nlp_level      = BT_ECHO_LEVEL;
        aec->nlp_part       = 1;                            //0:xd, 1:de&xd, 2:xd|xe
        aec->comforn_level  = 10;
        aec->comforn_floor  = 300;
        aec->comforn_en     = 0;
        aec->xe_add_corr    = 16384;
        aec->upbin          = 63;                           //调节最高频率2000Hz
        aec->lowbin         = 7;                            //调节最低频率250Hz
        if (!bt_sco_is_msbc()) {
            aec->upbin *= 2;
            aec->lowbin *= 2;
        }
        aec->bandrange      = aec->upbin - aec->lowbin + 1;
        aec->diverge_th     = 3;
        aec->echo_th        = 66666;
        aec->gamma          = 27852;
        aec->aggrfact       = 22937;                    	//bigger means more residual echo suppress
        aec->mic_ch         = 0;
        aec->qidx           = ((int)(0.35f * (1 << 15)) * aec->bandrange) >> 15;	//smaller means more residual echo suppress
        aec->mu_step        = 60;
        aec->dig_gain       = aec_dig_gain_tbl[xcfg_cb.aec_dig_gain * 2];
        *sysclk = *sysclk < SYS_48M ? SYS_48M : *sysclk;
    }
#endif

#if BT_ALC_EN
    if (xcfg_cb.bt_alc_en) {
        alc_cb_t alc;
        memset(&alc, 0, sizeof(alc_cb_t));

        aec->type           = AEC_TYPE_ALC;
        alc.fade_in_delay   = BT_ALC_FADE_IN_DELAY;
        alc.fade_in_step    = BT_ALC_FADE_IN_STEP;
        alc.fade_out_delay  = BT_ALC_FADE_OUT_DELAY;
        alc.fade_out_step   = BT_ALC_FADE_OUT_STEP;
        alc.far_voice_thr   = BT_ALC_VOICE_THR;
        if (!bt_sco_is_msbc()) {
            alc.fade_in_delay  >>= 1;
            alc.fade_out_delay >>= 1;
            alc.fade_in_step   <<= 1;
            alc.fade_out_step  <<= 1;
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
    far_cfg.mode  = 1;
    far_cfg.level = BT_SCO_FAR_NOISE_LEVEL;
    far_cfg.thr   = BT_SCO_FAR_THR;
    far_cfg.value_ns = BT_SCO_FAR_VALUE_NS;
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

#if BT_SCO_AGC_EN
	///通话AGC算法初始化
	bt_sco_agc_init();
#endif

    bt_sco_near_nr_init(sysclk, nr);

//    if (!xcfg_cb.bt_nr_en) {
//        nr->nr_type = ((nr->nr_type & ~NR_CFG_TYPE_MASK) | NR_TYPE_NONE);
//    }
}

void bt_sco_nr_exit(void)
{
    bt_sco_near_nr_exit();
}


#if BT_SCO_AGC_EN
void bt_sco_agc_init(void)
{
    int bit              = 16;
    int compress_agcDb   = 12;
    int target_agcDbfs   = 3;
    int sampleHzIn       = (bt_sco_is_msbc()) ? (16000) : (8000);

    bt_agc_init(sampleHzIn, bit, compress_agcDb, target_agcDbfs);
}
#endif

#if BT_SCO_DMIC_AI_EN
void bt_sco_dmns_init(u8 *sysclk, nr_cb_t *nr)
{
    if (!xcfg_cb.bt_dmns_en) {  //配置上面的DMNS双麦降噪功能未打开
        printf("DMNS xcfg init warning!\n");
    }
    dmns_cb_t *cb = &dmns_cb;
	memset(&dmns_cb, 0, sizeof(dmns_cb_t));
	nr->nr_type             = NR_TYPE_DMIC_AI;

	//辅助参数
	//cb->param_printf            = 1;
	cb->ai_postF_en             = 1;
	cb->show_out                = 0;  //0表示没有特殊输出,1:YAM,2:YBM,3:f1_tmp,4:f2_tmp,5:talk，6:YAM_R_w, 7:white_dB*x1, 8:bf2_used上之路YAM_R_tmp
									   // 9:风燥范围
	//基本参数
	cb->distance                = xcfg_cb.bt_dmns_distance;
	cb->nt                      = xcfg_cb.bt_dmns_level;

	// 风噪参数
	cb->wind_sig_choose		    = 1;     //0时代表副mic受风小  1代表主mic受风小
	cb->wind_len_used           = 66;    //风噪抑制以及判断使用范围66
	cb->wind_sup_open           = 1;     //是否打开风噪抑制，风噪抑制是加在baseline上的，而非风噪会进入tf
	cb->wind_state_lim          = 25000; //风噪判断的阈值，越大判断风噪范围越大，即使没打开风噪抑制也需要判断是否是风噪
	cb->wind_or_en			    = 1;     //风噪判断的模式，1表示判断为风噪的范围更大
	cb->wind_sup_floor          = 23000; //风噪抑制floor
	cb->Gcoh_thr			    = 13666;
	cb->Gcoh_sum_thr		    = 9666;
	cb->wind_pick_mode          = 0;     //防止风噪放大情况，如果不为0则能量最大值会加以限制，2为选取f1f2最小值作为界限，1为最大值作为界限

	//线性相位差参数
	cb->linear_en               = 1;  //开关，关掉的话highFcanc_256_to， sup180rtf，sup180都不起作用
	cb->linear_len              = 100;
	cb->linear_level            = 5;       //线性相位差线性度，越大越线性
	cb->grad_mu                 = 327;     //grad学习率，越小越慢，不能超过400
	cb->white_detect            = 32767;    //白噪启动（检测）阈值，目标信号是噪声的倍数
	cb->white_nomal_scale       = 4;       //白噪归一化尺度，越大白噪强度越小（但是会导致baseline相位差线性度削弱）,太大了会导致线性相位补偿效果弱
	cb->highFcanc_256_to        = 0;    //最大255，若检测声音方向是180°，高频白噪屏蔽，为255则关闭

	// baseline参数
	cb->comp_mic_fre_L          = 0;
	cb->comp_mic_fre_H          = 255;     //幅度补偿增益上限
	cb->opt_limit               = 32767;   //调整baseline波束宽度，波束后滤波下之路最大增益，越大baseline输出被挖掉的下之路越多
	cb->bf_choose			    = 1;       //baseline的波叔形状选择
	cb->adp_smr                 = 26635;    //自适应补偿平滑系数 X_BMBM_sm[i] X_AMAM_sm[i]
	cb->all_smr                 = 26635;    //全额补偿平滑系数 X_AMAM_sm_w[i] X_BMBM_sm_w[i]

	//RTF参数
	cb->tf_en				    = 1;
	cb->tf_len				    = 256; //smart: range:1-140   zoom:1:256
	cb->tf_norm_en			    = 1;
	cb->tf_norm_only_en		    = 0;
	cb->white_approach_shape    = 1;
	cb->v_white_en			    = 1;           //白噪开关
	cb->tf_SMrate               = 30000;       //最大32767  tf的平滑率
	cb->rtf_post_thres          = 2048;   //判断baseline结构是否为语音的阈值,越大判定噪声范围越大
	cb->white_type_len		    = 0;    //使用gain3_save作为white判断的低频范围，低于这个值的频段相位线性会变弱波束也会变差
	cb->sup180rtf               = 2;           //让180°入射tf更新快一点，越大越快,为0则关闭，与bf3的开启与否没关系

	//相位贴合
	cb->phase_to                = 0;  //如果为0则最终相位为糅合，1为使用talk相位，2为使用ff相位

	// 波束增强模块选择
	cb->bfrein_en			    = 0;//if set "1", the "cb->bfrein3_en" must be "0".
	cb->b_frein2_floor		    = 19666;
	cb->bfrein3_en		        = 1;//if set "1", the "cb->bfrein_en" must be "0".
	cb->bfrein3_ns		        = 3;  //波束增强模块gain的floor，越大floor越小，底噪越小
	cb->bfrein3_intensity       = 1024;  //噪声估计强度，q10越大去噪越强最大不超过u16
	cb->sup180                  = 2;   ////如果关掉bf3或者linear_en任意一个，sup180都失效，为0则关闭，越大抑制越大,越大180°方向噪声谱能量越大
	cb->grad_shift              = 0;  //bf波束宽度调节，越大越容易进入非波束范围，波束越窄
	cb->bf2_used_len            = 0;   //bf2点级别，双上支路使用频段，必须大于bfrein2_len，为0时bfrein2_len也必为0，最大255,会有分层，主要是上之路选了更小的幅度
	// bf2_used_len还没有到理想状态，不能启用！

	//整形
	cb->high_tf_Llim            = 255;  //大于这个频率的tf用的gpost和平滑率略有不同
	cb->high_tf_rate            = 3000;
	cb->high_gpost_appd         = 26384;

	// AI参数
	cb->prior_opt_ada_en	    = 1;
	cb->noise_ps_rate           = 1;
	cb->music_lev			    = 11;
	cb->gain_expand			    = 1024;
	cb->nn_only				    = 0;
	cb->nn_only_len			    = 16;
	cb->hi_fre_en 			    = 1;
	cb->gain_assign			    = 26666;
	cb->sin_gain_post_en	    = 0;
	cb->sin_gain_post_len	    = 128;
	cb->sin_gain_post_len_f	    = 256;
	cb->enlarge_v			    = 1;//1 or 2 or 3
	cb->mask_floor			    = 1000;
	cb->mask_floor_hi		    = 160;
	cb->low_fre_range           = 16; //
	cb->low_fre_range0          = 0;
	cb->low_fre_ns0_range	    = 16;//range:1-200
	cb->prior_opt_idx           = 3;

	cb->sp_thres				= 6000;
	cb->mic1_gain_idx           = 10;
	cb->mic2_gain_idx           = 10;
	cb->smooth_en				= 1;

    bt_dmns_init(&dmns_cb);

    *sysclk = *sysclk < SYS_100M ? SYS_100M : *sysclk;
}
#endif

void bt_sco_near_nr_dft_init(u8 *sysclk, nr_cb_t *nr)
{
    nr->nr_type     = NR_TYPE_NONE;
    *sysclk = *sysclk < SYS_48M ? SYS_48M : *sysclk;

#if BT_SCO_NR_EN
    ains3_cb_t *cb = &ains3_cb;
    memset(cb, 0, sizeof(ains3_cb_t));

    nr->nr_type         = NR_TYPE_AINS;
    cb->nt              = BT_SCO_NR_LEVEL;
    //cb->print_en        = 1;
    cb->prior_opt_idx   = 10;
    cb->ns_ps_rate		= 1;
    cb->trumpet_en		= 0;
	cb->nt_post         = 0;    //0-6 >0才起效 0为不开gain指数化 开的话默认为3
	cb->exp_range	    = 90;

	bt_ains3_init(cb);
    *sysclk = *sysclk < SYS_48M ? SYS_48M : *sysclk;
#endif
}

void bt_sco_near_nr_dft_exit(void)
{
#if BT_SCO_NR_EN
    bt_ains3_exit();
#endif
}

#if BT_SCO_SMIC_AI_EN
void bt_sco_dnn_init(u8 *sysclk, nr_cb_t *nr)
{
    if (!xcfg_cb.bt_dnn_en) {
        printf("DNN xcfg init warning!\n");
        return;
    }

    printf("%s\n", __func__);
    dnn_cb_t *cb = &dnn_cb;
    nr->nr_type = NR_TYPE_SMIC_AI;
    memset(&dnn_cb, 0, sizeof(dnn_cb_t));

	//cb->param_printf           = 1;
	//cb->nt                     = BT_SCO_SMIC_AI_LEVEL;
	//cb->nt_post                = 0; //0-6 >0才起效 0为不开gain指数化 开的话默认为3
	cb->noise_ps_rate          = 1;
	cb->prior_opt_idx	       = 3;
	cb->prior_opt_ada_en	   = 1;

	cb->low_fre_range          = 16; //
	cb->low_fre_range0         = 0;
	//cb->pitch_filter_en		   = 1;
	//cb->ps_lowlimt             = 0;
	cb->mask_floor			   = 1000;
	cb->noise_ceil			   = 0;
	cb->music_lev			   = 11;
	//cb->comforN_level		   = 1;
	cb->gain_expand			   = 1024;
	cb->nn_only				   = 0;
	cb->nn_only_len			   = 16;
	cb->gain_assign			   = 26666;
	cb->sin_gain_post_en	   = 0;
	cb->sin_gain_post_len	   = 128;
	cb->sin_gain_post_len_f	   = 256;
	cb->spp_thr				   = 8000;

	bt_dnn_init(&dnn_cb);
    *sysclk = *sysclk < SYS_60M ? SYS_60M : *sysclk;
}
#endif

#if BT_SCO_AIAEC_DNN_EN
void bt_sco_aiaec_init(u8 *sysclk, nr_cb_t *nr)
{
    if (!xcfg_cb.bt_aiaec_en) {
        printf("AIAEC xcfg init warning!\n");
        return;
    }

    printf("%s\n", __func__);
    nr->nr_type = NR_TYPE_AIAEC;
    dnn_aec_ns_cb_t *cb = &aiaec_dnn_cb;

    memset(&aiaec_dnn_cb, 0, sizeof(dnn_aec_ns_cb_t));

    //cb->param_printf           	= 1;
    cb->nt                     	= BT_SCO_AIAEC_DNN_LEVEL;
    cb->nlp_level               = BT_SCO_AIAEC_NLP_LEVEL;

    cb->gamma                   = 27852;
    cb->nlp_en                  = 1;
    cb->noise_ps_rate          	= 1;
    cb->prior_opt_idx	        = 10;
    cb->prior_opt_ada_en        = 1;
    cb->wind_level			    = 0;
    cb->wind_range			    = 0;
    cb->low_fre_range          	= 15;
    cb->mask_floor			    = 600;
    cb->music_lev			    = 11;
    cb->nn_only				   	= 0;
    cb->nn_only_len			   	= 16;
	cb->gain_assign				= 16666;
    cb->sin_gain_post_en	    = 0;
    cb->sin_gain_post_len	    = 128;
    cb->sin_gain_post_len_f		= 0;
    cb->smooth_en			    = 1;
    cb->far_post_thr		    = 1024;//Q8
	cb->dtd_post_thr		    = 80000000;//Q15

    cb->echo_gain_floor			= 100;
    cb->dtd_smooth			    = 29491;
    cb->single_floor		    = 0;
    cb->gain_assign_nlp			= 16000;
    cb->gain_st_thr				= 12000;
	cb->intensity			    = 1;
	cb->aec_gain_db				= 6;//AEC增益 0-30（放大0到30db）
	cb->nlms_en					= 1;
	cb->nlms_len				= 0;

    bt_aiaec_init(cb);

    *sysclk = *sysclk < SYS_80M ? SYS_80M : *sysclk;
}
#endif

#if BT_SCO_DMIC_AIAEC_EN
void bt_sco_dmdnn_aiaec_init(u8 *sysclk, nr_cb_t *nr)
{
    if (!xcfg_cb.bt_dmdnn_aiaec_en) {
        printf("DMDNN AIAEC xcfg init warning!\n");
        return;
    }

    printf("%s\n", __func__);
    nr->nr_type = NR_TYPE_DM_AIAEC;
    dmdnn_aiaec_cb_t *cb = &dmdnn_aiaec_cb;

    memset(&dmdnn_aiaec_cb, 0, sizeof(dmdnn_aiaec_cb_t));

	//辅助参数
	//cb->param_printf            = 1;
	cb->ai_postF_en             = 1;
	cb->show_out                = 0;  //0表示没有特殊输出,1:YAM,2:YBM,3:f1_tmp,4:f2_tmp,5:talk，6:YAM_R_w, 7:white_dB*x1, 8:bf2_used上之路YAM_R_tmp
									   // 9:风燥范围
	//基本参数
	cb->distance                = xcfg_cb.bt_dm_aiaec_distance;
	cb->nt                      = xcfg_cb.bt_dm_aiaec_level;
	cb->nlp_level               = xcfg_cb.bt_dm_aiaec_echo_level;
	cb->nlp_choose				= BT_SCO_DMIC_AIAEC_NLP_REF;    //0代表主mic，1代表副mic

	// 风噪参数
	cb->wind_sig_choose		    = 1;     //0时代表副mic受风小  1代表主mic受风小
	cb->wind_len_used           = 66;    //风噪抑制以及判断使用范围66
	cb->wind_sup_open           = 1;     //是否打开风噪抑制，风噪抑制是加在baseline上的，而非风噪会进入tf
	cb->wind_state_lim          = 25000; //风噪判断的阈值，越大判断风噪范围越大，即使没打开风噪抑制也需要判断是否是风噪
	cb->wind_or_en			    = 1;     //风噪判断的模式，1表示判断为风噪的范围更大
	cb->wind_sup_floor          = 23000; //风噪抑制floor
	cb->Gcoh_thr			    = 13666;
	cb->Gcoh_sum_thr		    = 9666;
	cb->wind_pick_mode          = 0;     //防止风噪放大情况，如果不为0则能量最大值会加以限制，2为选取f1f2最小值作为界限，1为最大值作为界限

	//线性相位差参数
	cb->linear_en               = 1;  //开关，关掉的话highFcanc_256_to， sup180rtf，sup180都不起作用
	cb->linear_len              = 100;
	cb->linear_level            = 3;       //线性相位差线性度，越大越线性
	cb->white_detect            = 32767;    //白噪启动（检测）阈值，目标信号是噪声的倍数
	cb->white_nomal_scale       = 4;       //白噪归一化尺度，越大白噪强度越小（但是会导致baseline相位差线性度削弱）,太大了会导致线性相位补偿效果弱
	cb->highFcanc_256_to        = 0;    //最大255，若检测声音方向是180°，高频白噪屏蔽，为255则关闭

	// baseline参数
	cb->comp_mic_fre_L          = 0;
	cb->comp_mic_fre_H          = 256;     //幅度补偿增益上限
	cb->opt_limit               = 32767;   //波束后滤波下之路最大增益，越大baseline输出被挖掉的下之路越多
	cb->bf_choose			    = 1;       //baseline的波叔形状选择

	//RTF参数
	cb->tf_en				    = 1;
	cb->tf_len				    = 256; //smart: range:1-140   zoom:1:256
	cb->tf_norm_en			    = 1;
	cb->tf_norm_only_en		    = 0;
	cb->white_approach_shape    = 1;
	cb->v_white_en			    = 1;           //白噪开关
	cb->tf_SMrate               = 30000;       //最大32767  tf的平滑率
	cb->rtf_post_thres          = 2048;   //判断baseline结构是否为语音的阈值
	cb->white_type_len		    = 0;    //使用gain3_save作为white判断的低频范围，低于这个值的频段相位线性会变弱波束也会变差
	cb->sup180rtf               = 4;           //让180°入射tf更新快一点，越大越快,为0则关闭

	//相位贴合
	cb->phase_to                = 0;  //如果为0则最终相位为糅合，1为使用talk相位，2为使用ff相位

	// 波束增强模块选择
	cb->bfrein_en			    = 0;//if set "1", the "cb->bfrein3_en" must be "0".
	cb->b_frein2_floor		    = 19666;
	cb->bfrein3_en		        = 1;//if set "1", the "cb->bfrein_en" must be "0".
	cb->bfrein3_ns		        = 2;  //波束增强模块gain的floor，越大floor越小，底噪越小
	cb->bfrein3_intensity       = 1024;  //噪声估计强度，q10越大去噪越强最大不超过u16
	cb->sup180                  = 4;   ////如果关掉bf3或者linear_en任意一个，sup180都失效，为0则关闭，越大抑制越大,越大180°方向噪声谱能量越大
	cb->grad_shift              = 0;  //bf波束宽度调节，越大越容易进入非波束范围，波束越窄
	cb->bf2_used_len            = 0;   //bf2点级别，双上支路使用频段，必须大于bfrein2_len，为0时bfrein2_len也必为0，最大255,会有分层，主要是上之路选了更小的幅度
	// bf2_used_len还没有到理想状态，不能启用！

	// AI参数
	cb->prior_opt_ada_en	    = 1;
	cb->noise_ps_rate           = 1;
	cb->music_lev			    = 11;
	cb->gain_expand			    = 1024;
	cb->nn_only				    = 0;
	cb->nn_only_len			    = 16;
	cb->hi_fre_en 			    = 1;
	cb->gain_assign			    = 26666;
	cb->sin_gain_post_en	    = 0;
	cb->sin_gain_post_len	    = 128;
	cb->sin_gain_post_len_f	    = 256;
	cb->enlarge_v			    = 1;//1 or 2 or 3
	cb->mask_floor			    = 1000;
	cb->mask_floor_hi		    = 160;
	cb->low_fre_range           = 16; //
	cb->low_fre_range0          = 0;
	cb->low_fre_ns0_range	    = 16;//range:1-200
	cb->prior_opt_idx           = 3;

	cb->sp_thres				= 6000;
	cb->mic1_gain_idx           = 10;
	cb->mic2_gain_idx           = 10;
	cb->gamma                   = 27852;
	cb->nlp_en                  = 1;
	cb->far_post_thr		    = 1024;//Q8
	cb->dtd_post_thr		    = 80000000;//Q15
	cb->echo_gain_floor			= 100;
	cb->dtd_smooth			    = 29491;
	cb->single_floor		    = 0;
	cb->gain_assign_nlp			= 16000;
	cb->gain_st_thr				= 12000;
	cb->aec_gain_db				= 0;//AEC增益 0-30（放大0到30db）
	cb->refer_dfw				= 0;
	cb->nlms_en					= 1;
	cb->nlms_len				= 0;
	cb->smooth_en				= 1;

    bt_dmdnn_aiaec_init(cb);

    *sysclk = *sysclk < SYS_120M ? SYS_120M : *sysclk;
}
#endif

#if BT_SCO_NR_USER_SMIC_EN || BT_SCO_NR_USER_DMIC_EN

typedef struct {
    bool start;
    volatile bool busy;

    s32 mic0[512];
    //s32 mic1[512];
    //s32 far[512];
} nr_user_proc_t;

nr_user_proc_t user_proc AT(.nr_buf.rdft);

AT(.bt_voice.user.alg)
void alg_user_process(void)
{
    //Run your algorithm on the alg thread
}

AT(.bt_voice.user.alg)
void bt_nr_user_fre_proc(s32 *mic0, s32 *mic1, s32 *far)
{
    sco_nr_memswap(mic0, user_proc.mic0, 512 * 4);
    //sco_nr_memswap(mic1, user_proc.mic1, 512 * 4);
    //sco_nr_memswap(far, user_proc.far, 512 * 4);

    if (user_proc.start) {
        alg_user_kick_start();
    }
}

void bt_sco_nr_user_init(u8 *sysclk, nr_cb_t *nr)
{
    printf("nr user init\n");
    nr->nr_type = NR_TYPE_USER;

    sco_pcm_set_nr_kick_func(bt_nr_user_fre_proc);

    *sysclk = *sysclk < SYS_120M ? SYS_120M : *sysclk;
    user_proc.start = true;
}

void bt_sco_nr_user_exit(void)
{
    printf("nr user exit\n");
    uint timeout = 10;              //50ms
    user_proc.start = false;

    while (user_proc.busy) {          //等待当前帧算完
        delay_5ms(1);
        if(--timeout == 0) {
            printf("user alg, timeout\n");
            break;
        }
    }
}
#endif
