#include "include.h"
#include "bt_sco.h"

#if FUNC_BT_EN && BT_HFP_EN

bool bt_sco_is_msbc(void);
extern bool smic_test_mic_en;
extern u8 dmic_test_mic_ch;
extern call_cfg_t *call_cfg;

bool bt_is_calling(void);
void bt_sco_tx_process_do(u32 *ptr, u32 samples);
void dac_aubuf_set_size(u8 flag);
bool bt_sco_is_bypass(void);

//------------------------------------------------------------------------------------------
//Bluetrum_Voice_Record工具dump数据, 建议用v3以上的版本
#if BT_SCO_DUMP_EN || BT_AEC_DUMP_EN || BT_SCO_FAR_DUMP_EN || BT_EQ_DUMP_EN || BT_PLC_DUMP_EN

#define SCO_DUMP_USE_TEMP_BUF       1       //dump数据是否使用缓存buf，减少数据异常的风险，建议打开

static struct sco_dump_tag {
    dump_cb_t cb;
    dump_tx_t tx[DUMP_TX_SIZE];
    u8 frame_cnt[DUMP_SCO_MAX_NB];
} sco_dump AT(.sco_dump.cb);

static u8 sco_dump_header[DUMP_SCO_MAX_NB][8] AT(.sco_dump.header);
#if SCO_DUMP_USE_TEMP_BUF
static s16 sco_dump_buf[DUMP_SCO_MAX_NB][256] AT(.bss1.sco_dump.buf);
#endif

AT(.bt_voice.sco.dump)
//AT(.com_text.sco.dump)
void bt_sco_dump_cb(uint type, void *ptr, uint size)
{
    int idx = -1;

    switch(type) {
        //idx的值按照算法链路处理的顺序由小到大排列，最后处理的放最前面，idx值越大
#if BT_SCO_DUMP_EN || BT_AEC_DUMP_EN
        case DUMP_MIC_NR:           idx++;
        #if BT_SCO_TMIC_EN
        case DUMP_MIC_FB:           idx++;
        #endif
        #if BT_SCO_DMIC_EN || BT_SCO_TMIC_EN
        case DUMP_MIC_FF:           idx++;
        #endif
        case DUMP_MIC_TALK:         idx++;
        case DUMP_AEC_FAR:          idx++;

#elif BT_EQ_DUMP_EN
        case DUMP_EQ_OUTPUT:        idx++;
        case DUMP_MIC_NR:           idx++;
        #if BT_SCO_TMIC_EN
        case DUMP_MIC_FB:           idx++;
        #endif
        #if BT_SCO_DMIC_EN || BT_SCO_TMIC_EN
        case DUMP_MIC_FF:           idx++;
        #endif
        case DUMP_MIC_TALK:         idx++;

#elif BT_SCO_FAR_DUMP_EN
        case DUMP_FAR_NR_OUTPUT:    idx++;
        case DUMP_FAR_NR_INPUT:     idx++;

#elif BT_PLC_DUMP_EN
        case DUMP_PLC_STA:          idx++;
        case DUMP_PLC_OUT:          idx++;
        case DUMP_PLC_IN:           idx++;
#endif
            //需要打印的case
            if ((0 <= idx) && (idx < DUMP_SCO_MAX_NB)) {
                #if SCO_DUMP_USE_TEMP_BUF
                void *buf = sco_dump_buf[idx];
                memcpy(buf, ptr, size);
                pcm_dump_putcs(idx, buf, size, sco_dump.frame_cnt[idx]++);
                #else
                pcm_dump_putcs(idx, ptr, size, sco_dump.frame_cnt[idx]++);
                #endif
            }
            break;

        default :
            //不需要的走default
            break;
    }
}

AT(.text.sco.dump)
void bt_sco_dump_init(void)
{
    if (!xcfg_cb.huart_en) {
        printf("dump huart xcfg init err!\n");
        while(1);
    } else {
        dump_cb_t *dump = &sco_dump.cb;

        memset(&sco_dump, 0, sizeof(struct sco_dump_tag));

        dump->ch_num        = DUMP_SCO_MAX_NB;
        dump->tx            = &sco_dump.tx[0];
        dump->header        = (u8*)&sco_dump_header[0];
        dump->dma_tx        = huart_tx;
        pcm_dump_init(dump);
    }
}

    #define bt_sco_dump(a, b, c)            bt_sco_dump_cb(a, b, c)
#else
    #define bt_sco_dump(a, b, c)
    #define bt_sco_dump_init()
#endif

AT(.bt_voice.plc_dump)
void bt_plc_dump(uint type, void *ptr, uint size)
{
#if BT_PLC_DUMP_EN
    if (type == 0) {
        bt_sco_dump_cb(DUMP_PLC_IN, ptr, size);
    } else if (type == 1) {
        bt_sco_dump_cb(DUMP_PLC_OUT, ptr, size);
    } else if (type == 2) {
        bt_sco_dump_cb(DUMP_PLC_STA, ptr, size);
    }
#endif
}

//AT(.bt_voice.sco.trumpet) WEAK
//void bt_sco_trumpet_nr(s32 *fft_in)
//{
////    if (bt_sco_dbg_get_alg_status(SCO_DBG_TRUMPET)) {
//        trumpet_denoise((int*)fft_in);
////    }
//}

//------------------------------------------------------------------------------------------
#if BT_SCO_DAC_DRC_EN
AT(.bt_voice.dac.drc)
void bt_sco_dac_drc_proc(s16 *ptr, u32 samples)
{
    for(int i = 0; i < samples; ++i) {
        ptr[i] = bt_sco_dac_drc_calc((s32)ptr[i]);
    }
}
#endif

#if BT_SCO_DAC_DNR_EN
AT(.bt_voice.dac.dnr)
void bt_sco_dac_dnr_proc(s16 *ptr, u32 samples, int dnr_thr)
{
	u16 pwr = dnr_buf_maxpow(ptr, samples);
    //printf("%d,", pwr);
    if (pwr <= dnr_thr) {
        memset(ptr, 0, samples*2);
    }
}
#endif

AT(.bt_voice.far_nr_do)
void bt_sco_far_nr_process(s16 *ptr, u16 samples)
{
	bt_sco_dump(DUMP_FAR_NR_INPUT, ptr, samples * 2);

#if BT_SCO_FAR_NR_EN            //通话下行————降噪
    bt_sco_far_peri_nr_process(ptr, samples);
#endif

#if BT_SCO_DAC_DRC_EN           //通话下行————DRC
    bt_sco_dac_drc_proc(ptr, samples);
#endif

#if BT_SCO_DAC_DNR_EN           //通话下行————DNR
    nr_cb_t *nr = &call_cfg->nr;
    bt_sco_dac_dnr_proc(ptr, samples, BT_SCO_DAC_DNR_THR);
#endif

	bt_sco_dump(DUMP_FAR_NR_OUTPUT, ptr, samples * 2);
}

AT(.bt_voice.sco)
void bt_sco_rx_algo_proc(u8 position, u16 *ptr, u8 len)
{
    if ((call_cfg == NULL) || (position == 0)) {
        return;
    }
//    nr_cb_t *nr = &call_cfg->nr;
//
//    if(!bt_is_calling()){
//        u16 data_len = len/2;
//        u16 maxpow = dnr_voice_maxpow_calling((u32*)ptr,data_len);
//        if(maxpow > nr->calling_voice_pow){
//            nr->calling_voice_temp_cnt++;
//        }else if(nr->calling_voice_temp_cnt < nr->calling_voice_cnt){   //过滤一些突然很大的一包数据，需要连续联测5包数据
//            nr->calling_voice_temp_cnt = 0;
//        }
//        if(nr->calling_voice_temp_cnt < nr->calling_voice_cnt){
//            data_len = len*2;
//            memset((u8*)ptr,0x00,data_len);
//        }else{
//            nr->calling_voice_temp_cnt = nr->calling_voice_cnt;
//        }
//    }else{
//        nr->calling_voice_temp_cnt = 0;
//    }

    bt_sco_far_nr_process((s16*)ptr, len/2);
}

//------------------------------------------------------------------------------------------

///实测系统26M时, 每次EQ处理需要300us左右
AT(.bt_voice.mic)
u16 *bt_sco_tx_algo_proc(u16 *ptr, u16 samples)
{
    if (smic_test_mic_en || dmic_test_mic_ch) {     //mic产测流程不过算法
        return ptr;
    }
    if (!call_cfg) {
        return ptr;
    }

#if BT_SCO_EQ_DRC_EN
    if (call_cfg->mic_eq_en) {
        sco_pacc_process((u8 *)ptr, (u8 *)ptr, samples);
    } else
#endif
    {    //走产测流程时，可能不过mic eq，此时硬件后置增益不起效，跑一个软件后置增益
        mic_post_gain_process_s((s16 *)ptr, call_cfg->post_gain, samples);
    }

#if BT_SCO_AGC_EN
    bt_sco_agc_proc_do((s16 *)ptr, samples);
#endif

    bt_sco_dump(DUMP_EQ_OUTPUT, ptr, samples * 2);
    return ptr;
}

AT(.bt_voice.bb.sco_tx)
void bt_sco_tx_process(u8 *ptr, u32 samples, u32 pcm_mode)
{
    if (!bt_sco_is_msbc() && bt_sco_dnn_en()) {     //DNN单麦降噪走窄带通话时，需要把16k转为8k，DNN需要16k因此要转换
        samples = dnn_near_downsample((s16 *)ptr, samples);
    }

    ptr = (u8 *)bt_sco_tx_algo_proc((u16 *)ptr, samples);         		//MIC EQ

    if (!bt_sco_is_bypass()) {
        bt_sco_tx_process_do((u32*)ptr, samples);
    }
}

//不过AEC或ALC接口
AT(.bt_voice.sco.adc)
void bt_adc_process(u8 *ptr, u32 samples, u32 pcm_mode)
{
    if (bt_sco_is_bypass()) {
        return;
    }
    if (call_cfg && call_cfg->nr.nr_type != NR_TYPE_NONE) {
        bt_sco_adc_to_aec_enc(ptr, samples, pcm_mode);
        if (!bt_sco_enc_mic_test(ptr, samples, pcm_mode)) {         // SMIC_DBG_EN || DMIC_DBG_EN
            return;
        }
        sco_pcm_kick_start();
    } else {
        bt_sco_tx_process(ptr, samples, pcm_mode);
    }
}

void bt_call_init(call_cfg_t *p)
{
    if (p == NULL) {
        return;
    }
    call_cfg = p;
    aec_cfg_t *aec = &p->aec;
//    alc_cb_t *alc = &p->alc;
//    nr_cb_t *nr = &p->nr;

    rdft_init();

    //回声消除算法初始化
    if (aec->type == AEC_TYPE_DEFAULT) {
        aec_init();
    }

#if BT_SCO_FAR_NR_EN
    bt_peri_nr_init();
    bt_sco_far_nr_int(call_cfg->nr.far_nr);
#endif

    dac_aubuf_set_size(2);

    ///算法数据dump初始化
    bt_sco_dump_init();

    bt_sco_pcm_buf_init();
}

void bt_call_exit(void)
{
    aec_cfg_t *aec = &call_cfg->aec;
//    alc_cb_t *alc = &call_cfg->alc;

    if (aec->type == AEC_TYPE_DEFAULT) {
        aec_exit();
    } else if (aec->type == AEC_TYPE_ALC) {
        alc_exit();
    }

    rdft_exit();
    bt_peri_nr_exit();

    call_cfg = NULL;
    unlock_code_bt_voice();
}

#endif
