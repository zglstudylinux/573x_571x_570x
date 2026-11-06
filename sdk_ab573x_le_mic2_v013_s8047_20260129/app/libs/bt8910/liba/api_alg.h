#ifndef __API_ALG_H
#define __API_ALG_H
///------------------------------------------------------------------------------------------
///echo算法结构体以及相关API声明
#if WIRELESS_MIC_24B_PCM_EN
typedef s32			mic_pcm_t;
#else
typedef s16			mic_pcm_t;
#endif

typedef struct {
    u8   skip_flag;			/*开启后相同buf最大delay可多一倍*/
	u8   lp_filter_en;       /*滤波器使能*/
	u8   attenuation_set;    /*1-9 ->10percent-90percent*/
	u16  delay_set;          /*1-500ms*/
    u16  dry;
    u16  wet;
    mic_pcm_t *delay_lbuf_set;
    u32  cutoffFreq_set;     /*截至频率选择*/
} echo_init_t;

#if WIRELESS_MIC_24B_PCM_EN
void echo_init_24bit(echo_init_t *p);
void echo_process_24bit(mic_pcm_t *ldata);
void echo_update_param_24bit(echo_init_t *p, u8 fade_en);
#define echo_init           echo_init_24bit
#define echo_process        echo_process_24bit
#define echo_update_param   echo_update_param_24bit
#else
void echo_init_16bit(echo_init_t *p);
void echo_process_16bit(mic_pcm_t *ldata);
void echo_update_param_16bit(echo_init_t *p, u8 fade_en);
#define echo_init       echo_init_16bit
#define echo_process    echo_process_16bit
#define echo_update_param   echo_update_param_16bit
#endif

///------------------------------------------------------------------------------------------
///magic 算法结构体以及相关API声明
#define CircleBufo_Len		1536//0.032*Fs

typedef struct {
//Common use
	s32 fre_shift;
	u32 Fs;
	u32 pitch_levels;
	s32 ph1;
	s32 ph2;
	s16 pWtite1;
	s16 pRead1;
	s16 pRead2;
	s16 CircleBuf[CircleBufo_Len];
} pitch_shift2_cb_t;


void pitch_shift2_init(s32 SampleRate, s32 shift, pitch_shift2_cb_t *st);
void pitch_shift2_process(s16 *data, pitch_shift2_cb_t *st);

///------------------------------------------------------------------------------------------
///ROOM_REVERB 算法结构体以及相关API声明
typedef struct
{
	u16 damping_set;        /*高频阻尼，去除高频部分的齿音，擦类的刺耳声*/
	u16 decay_set;          /*衰减率*/
	u16 wet;
	u16 dry;
	u32 samples;
	u8 hpl;                 /*高通滤波器等级0表示关闭,1-10:50hz-500hz,步进50hz*/
}rvb_init_sb;

void room_reverb_process(s16 *sample, u8 idx);
void reverb_buf_init(rvb_init_sb *p, u8 idx);

///------------------------------------------------------------------------------------------
///软件src算法结构体以及相关API声明
typedef struct{
	u8  src_en;
	u8  resv_en;
	int samplerate_in;
	int samplerate_out;
	int phase_comp;
	int phase_i;
	int phase_o;
	int phase_sum;
} srccon_t;

void src_init(u32 ch_index, u32 spr_in, u32 spr_out);
int src_frame_resample(u32 ch_index, short *src_in, short *src_out, int in_cnt);
int src_frame_resample_24bit(u32 ch_index, int *src_in, int *src_out, int in_cnt);
void src_phase_comp_set(u32 ch_index, int phase);
///------------------------------------------------------------------------------------------
///howling_dnn(ai_mask)算法结构体以及相关API声明
typedef struct {
	//s16 nt;
	s16 mask_floor;
    u8  param_printf;                           //使能参数打印
	s32 howling_f_thres;
	s32 howling_f_thres_hi;
	u16 howling_fre_p;
	s32 hiwling_div_thres;
	u32 std_thres;
	s16  howling_exp_papr;
	s16  howling_exp_papr_hi;
	s16  howling_exp_papr2;
	//u8  sin_post_en;
	u8  all_mask_en;
	u8  smooth_en;

	//u16 pitch_filter_range;
} howling_dnn_cb_t;
void howling_dnn_init(howling_dnn_cb_t *dnn_cb);
int howling_dnn_process(s16 *data, s32 *data_32bits, u8 in_24bits_en);
void howling_dnn_fre_process(s32 *f, u8 data_shift, u8 in_24bits_en);
void howling_dnn_process_ext_window(s16 *data, s32 *data_32bits, u8 in_24bits_en,s32* outbuf);
void howling_dnn_process_ext_next(s32 *data, s32 *data_32bits, u8 in_24bits_en,s16* outbuf);
///------------------------------------------------------------------------------------------
///dnn_L1算法结构体以及相关API声明
typedef struct {
	//s16 nt;
	//u8  nt_post;
	//s16 exp_range_H;
	//s16 exp_range_L;
	u8  model_select;
	u16 min_value;
	u16 nostation_floor;
	u8  wind_thr;
	u8  wind_en;
	u8  noise_ps_rate;
	u8  prior_opt_idx;
	u8  prior_opt_ada_en;
    u8  param_printf;                           //使能参数打印
    u8  wind_level;
	u16 wind_range;
	u16 low_fre_range;
	u16 low_fre_range0;
	u8  pitch_filter_en;
	u16 mask_floor;
	u8  mask_floor_r;
	u8  music_lev;
	u8  comforN_level;
	u16 gain_expand;
	u8  nn_only;
	u16 nn_only_len;
	//s32 noise_db2;
	//s32 noise_db3;
	u8  smooth_en;
	u16 gain_assign;
	u8  enlarge_v;
	s16 sin_gain_post_len;
	s32 noise_db;
	s32 noise_db2;
	s32 noise_db3;
	s32 noise_db4;
	u8  vad_floor_en;
	u32 prior_sum_v;
	s16 spp_thr;
	//u16 pitch_filter_range;
} dnn_L1_cb_t;
void dnn_L1_init(dnn_L1_cb_t *dnn_cb);
int dnn_L1_process(s16 *data, s32 *data_32bits, u8 in_24bits_en);

///---------------------------------------------------------------------------------------------
///DNN_L2 算法结构体以及相关API声明
typedef struct {
	u8  model_select;
	u16 min_value;
	u16 nostation_floor;
	//u8  wind_thr;
	//u8  wind_en;
	u8  noise_ps_rate;
	u8  prior_opt_idx;
	u8  prior_opt_ada_en;
    u8  param_printf;                           //使能参数打印

	u16 low_fre_range;

	u8  music_lev;

	u8  nn_only;
	u16 nn_only_len;
	u16 gain_assign;
	u8  sin_gain_post_en;
	u16 sin_gain_post_len;
	u16 sin_gain_post_len_f;
	s16 denoiseBound;
	u8  smooth_en;
	u8  ap_test_en;
	u16 hi_gain_len;
	s16 sp_thres;

	//u16 pitch_filter_range;
} dnn_l2_cb_t;

void dnn_l2_init(dnn_l2_cb_t *dnn_l2_cb);
int dnn_l2_process(s16 *data);

///---------------------------------------------------------------------------------------------
///DNN_L3 算法结构体以及相关API声明
typedef struct {
	u8  model_select;
	u16 min_value;
	u16 nostation_floor;
	//u8  wind_thr;
	//u8  wind_en;
	u8  noise_ps_rate;
	u8  prior_opt_idx;
	u8  prior_opt_ada_en;
    u8  param_printf;                           //使能参数打印

	u16 low_fre_range;

	u8  music_lev;

	u8  nn_only;
	u16 nn_only_len;
	u16 gain_assign;
	u8  sin_gain_post_en;
	u16 sin_gain_post_len;
	u16 sin_gain_post_len_f;
	s16 denoiseBound;
	u8  smooth_en;
	u8  ap_test_en;
	u16 hi_gain_len;
	s16 sp_thres;

	//u16 pitch_filter_range;
} dnn_l3_cb_t;

void dnn_l3_init(dnn_l3_cb_t *dnn_l3_cb);
int dnn_l3_process(s16 *data);


///---------------------------------------------------------------------------------------------
///DNN_L3_32k 算法结构体以及相关API声明
typedef struct {

	u8  model_select;
	u16 min_value;
	u16 nostation_floor;
	//u8  wind_thr;
	//u8  wind_en;
	u8  noise_ps_rate;
	u8  prior_opt_idx;
	u8  prior_opt_ada_en;
    u8  param_printf;                           //使能参数打印

	u16 low_fre_range;

	u8  music_lev;
	u8  music_lev2;

	u8  nn_only;
	u16 nn_only_len;
	u16 gain_assign;
	u8  sin_gain_post_en;
	u16 sin_gain_post_len;
	u16 sin_gain_post_len_f;
	s16 denoiseBound;
	s16 denoiseBound_hi;
	u8  smooth_en;
	u8  ap_test_en;
	u16 hi_gain_len;
	s16 sp_thres;
    s32 vad_thres;
	s16 prev_noise_len;
	u8  dd_od_ctl_en;
	s16 low_gain_l;
    u8  hi_gain_mode;
    u8  spp_hi_en;
	//u16 pitch_filter_range;
} dnn_l3_32k_cb_t;

void dnn_l3_32k_init(dnn_l3_32k_cb_t *dnn_l3_32k_cb);
int dnn_l3_32k_process(s16 *data);
///---------------------------------------------------------------------------------------------
///dnr fre 算法结构体以及相关API声明
typedef struct {

	//s32 noise_db2;
	//s32 noise_db3;
	s16 denoiseBound;
	s32 overdrive;
	u8  smooth_en;
	s64 enr_thres;
	s16 smooth_v;
	u8  enr_mean_max_en;
	s16 music_lev;
	s32 enr_nr_thr;
	s16 low_fre_range;
	s16 prior_opt_idx;
	s32 in_attack;
	s32 in_release;
	s32 fs;
	s32 noise_init;
	//u16 pitch_filter_range;
} dnr_fre_cb_t;

void dnr_fre_init(dnr_fre_cb_t *dnr_fre_cb);
int dnr_fre_process(s16 *data, s32 *data_32bits, u8 in_24bits_en);

///---------------------------------------------------------------------------------------------
///ylcrn_L1_32k 算法结构体以及相关API声明
typedef struct {
	u8  param_printf;
	s32 overdrive;
	//u8  adaptive_floor;
	u16 denoiseBound;
	u16 denoiseBound_wind;
	u8  wind_en;
	//u16 denoiseBoundLow;
	//u16 denoiseBoundHigh;
	//u16 denoiseBoundProb;
	//u16 denoiseBoundRatio;
	u8  smooth_en;
	u8  modelUpdatePars0;
	//s32 factor;
	//u8  delta_k_up;
	//s16 lquantile_sm;
	//s32 quan_gap;
	//s16 quan_gap_low_len;
	//s32 quan_gap_low;
	u8  sin_dnn_en;
	u8  sin_dnn_wind_en;
	u8  sin_all_en;
	s16 sin_all_len;
	u8  prior_opt_idx;
	u8  prior_opt_ada_en;
	u16 low_noise_range;
	u16 spp_fre_p;
	u16 spp_fre_p_wind;

	u16 spp_fre_len;
	//u16 high_gain_len;
	s16 music_lev;

	s16 prev_noise_len;
	s16 gain_assign;
	s16 gain_assign_wind;
	u8  hi_gain_mode;
	u8 spp_dnn_max_en;
	s16 gain_hard_thres;
	s32 vad_hard_thres;
	u8  vad_en;
	s16 high_gain_len;

	u8 *wind_buf;
	int wind_buf_size;
	void *wn;
} ylcrn_L1_32k_cb_t;
void ylcrn_L1_32k_ns_process(s16* data);
void ylcrn_L1_32k_ns_init(ylcrn_L1_32k_cb_t *p);

///------------------------------------------------------------------------------------------
///AINS4降噪算法结构体以及相关API声明
typedef struct {

	//s32 noise_db2;
	//s32 noise_db3;
	s32 denoiseBound;
	s32 snr_thr;
	u8  overdrive_adapt_en;
	s32 overdrive;
	u8  smooth_en;
	u8  modelUpdatePars0;
	//u8  enlarge_v;
	s16 gainHB_rd;
	u8 delta_k_up;
	//s16 denoiseBound_fix;
	u8 	yuan_en;
	s16 enr_thres;
	s16  low_fre_range;
	s16 prior_opt_idx;
	s16 prior_opt_freh;
	u8  prior_opt_ada_en;
	s16 lquantile_sm;
	s16 hi_gain_len;
	s32 factor;
	s32 quan_gap;
	s16 quan_gap_low_len;
	s32 quan_gap_low;
	s16 speech_update;
	//s16 smooth_v;
	//u8  enr_mean_max_en;
	//s32 enr_nr_thr;
	u8	spp_en;
	s16 ymin_floor;
	s16 ymin_idx;
	s16 gain_ceil;
	s32 gain_ceil_ratio;
	s16 noise_min_floor;
	//s16 smooth_logLrt;
	s16 music_lev;
	u8  noise_min_en;
	s32 ai_vad_hard_thres;
	u16 spp_vad_len;
	s32 qhat_assign;
	s32 qhat_assign_low;
	u8  spp_max_en;
	u8  low_floor_add_l;
	s16 low_mask_floor_add;
	u8  dd_od_ctl_en;
	u8  ai_vad_spp_hard_en;
	s32 spp_thr_noiseprev;
	s16 spp_thr;
	s16 prev_noise_len;
	u8  vad_low_noise_en;
	s16 vad_low_noise_p;
	u8  vad_low_max_en;
	u8  sin_keep_en;
	s32 sin_keep_thr;
	s16 sin_keep_gap;
	u8  sin_coswin_en;
	u8  sin_keep_idx_lim;
	s32 priorModelPars0;
	s16 alp1_dd;

	//u16 pitch_filter_range;
} ains4_cb_t;
void ains4_mic_proc_kick_start(void);
void ains4_init(ains4_cb_t *ains4_cb);
int ains4_process(s16 *data);

#endif // __API_CODEC_H

