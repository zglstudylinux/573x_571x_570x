//#include "include.h"
//#include "func.h"
//#include "func_bt.h"
//
//static call_cfg_t call_cfg AT(.sco_data);
//
//AT(.sco_alg_buf)
//static u8 sco_sdadc_buf[256 * 2];
//
//AT(.rodata.mic_gain)
//const int mic_gain_tbl[16] = {
//    SOFT_GAIN_P0_0DB,
//    SOFT_GAIN_P1_0DB,
//    SOFT_GAIN_P2_0DB,
//    SOFT_GAIN_P3_0DB,
//    SOFT_GAIN_P4_0DB,
//    SOFT_GAIN_P5_0DB,
//    SOFT_GAIN_P6_0DB,
//    SOFT_GAIN_P7_0DB,
//    SOFT_GAIN_P8_0DB,
//    SOFT_GAIN_P9_0DB,
//    SOFT_GAIN_P10_0DB,
//    SOFT_GAIN_P11_0DB,
//    SOFT_GAIN_P12_0DB,
//    SOFT_GAIN_P13_0DB,
//    SOFT_GAIN_P14_0DB,
//    SOFT_GAIN_P15_0DB,
//};
//
//#if BT_SCO_DBG_EN
//void sco_audio_set_param(u8 type, u16 value)
//{
////    aec_cb_t *aec = &call_cfg.aec;
////    nr_cb_t *nr = &call_cfg.nr;
//    //printf("set param[%d]:%d\n", type, value);
//    if (type == AEC_PARAM_NOISE) {
////        xcfg_cb.bt_noise_threshoid = value;
////        nr->threshoid = value;
//    } else if (type == AEC_PARAM_LEVEL) {
//        xcfg_cb.bt_echo_level = value;
////        aec->echo_level = value;
//    } else if (type == AEC_PARAM_OFFSET) {
////       aec->far_offset = value;
//    } else if (type == AEC_PARAM_MIC_ANL_GAIN) {
//        xcfg_cb.mic_anl_gain = value;
////        set_mic_analog_gain(value, (CHANNEL_0 | CHANNEL_1 | CHANNEL_2));
//    } else if (type == AEC_PARAM_MIC_DIG_GAIN) {
//        xcfg_cb.mic_dig_gain = value;
////        sdadc_set_digital_gain((CHANNEL_1 | CHANNEL_2 | CHANNEL_3), (value&0x3f));
//    } else if (type == AEC_PARAM_MIC_POST_GAIN) {
//        xcfg_cb.bt_sco_post_gain = value & 0x0f;
//    }
//}
//#endif
//
//static void bt_sco_eq_init(call_cfg_t *p)
//{
//    u32 mic_eq_addr, mic_eq_len;
//#if BT_MIC_DRC_EN
//    u8 *mic_drc_addr;
//    u32 mic_drc_len;
//#endif
//
//    if (bt_sco_is_msbc()) {
//        mic_eq_addr = RES_BUF_EQ_BT_MIC_16K_EQ;
//        mic_eq_len  = RES_LEN_EQ_BT_MIC_16K_EQ;
//
//    #if BT_MIC_DRC_EN
//        mic_drc_addr = (u8 *)RES_BUF_EQ_BT_MIC_16K_DRC;
//        mic_drc_len = RES_LEN_EQ_BT_MIC_16K_DRC;
//    #endif
//    } else {
//        mic_eq_addr = RES_BUF_EQ_BT_MIC_8K_EQ;
//        mic_eq_len  = RES_LEN_EQ_BT_MIC_8K_EQ;
//
//    #if BT_MIC_DRC_EN
//        mic_drc_addr = (u8 *)RES_BUF_EQ_BT_MIC_8K_DRC;
//        mic_drc_len = RES_LEN_EQ_BT_MIC_8K_DRC;
//    #endif
//    }
//
//    //先初始化
//    uint pacc_en = 0;
//    mic_pacc_init(S16_Q15_SAT, S16_Q15_SAT);    //pacc eq/drc
//    //然后设置参数
//    if (pacc_eq_set_by_res(mic_eq_addr, mic_eq_len)) {
//        call_cfg.mic_eq_en = 1;
//        pacc_en |= MIC_PACC_EQ_BIT;
//    }
//#if BT_MIC_DRC_EN
//    if(pacc_drc_set_by_res((u32)mic_drc_addr, mic_drc_len)) {
//        pacc_en |= MIC_PACC_DRC_BIT;
//    }
//#endif
//    //最后使能
//    mic_pacc_enable(pacc_en);
//
//    pacc_eq_set_post_gain(mic_gain_tbl[xcfg_cb.bt_sco_post_gain] << 8);
//    p->post_gain = (mic_gain_tbl[xcfg_cb.bt_sco_post_gain]);
//
//#ifdef RES_BUF_EQ_CALL_NORMAL_EQ
//    music_eq_set_by_res(RES_BUF_EQ_CALL_NORMAL_EQ, RES_LEN_EQ_CALL_NORMAL_EQ);
//#else
//    music_eq_off();
//#endif
//
//#if DAC_DRC_EN
//    music_drc_off();
//#endif
//}
//
//static void bt_sco_eq_exit(void)
//{
//    mic_pacc_exit();    //pacc eq/drc
//
//    music_eq_set_by_num(sys_cb.eq_mode);
//
//    if(app_eq_set() == 0){
//        music_eq_set_by_num(sys_cb.eq_mode);
//    }
//
//#if DAC_DRC_EN
//    music_drc_on();
//#endif
//}
//
//static void bt_call_alg_init(void)
//{
//    u8 sysclk = SYS_24M;
//    memset(&call_cfg, 0, sizeof(call_cfg_t));
//
//    sys_clk_req(INDEX_VOICE, SYS_100M);
//
//    ///通话算法code初始化
//    bt_sco_code_init();
//
//    ///AEC初始化
//    bt_sco_aec_init(&sysclk, &call_cfg.aec);
//
//    ///通话MIC端降噪算法初始化
//    bt_sco_nr_init(&sysclk, &call_cfg.nr);
//
//    ///通话DAC端降噪算法初始化
//    bt_sco_far_nr_init(&sysclk, &call_cfg.nr);
//
//    ///DAC、MIC EQ初始化
//    bt_sco_eq_init(&call_cfg);
//
//    ///plc算法初始化
//    if (bt_sco_is_msbc() || bt_sco_dnn_en()) {     //如果开了msbc或dnn，则采样率设为16k
//        plc_soft_v2_init(0, 2, 120, 1);
//    } else {
//        plc_soft_v2_init(0, 3, 60, 1);
//    }
//
//    if (SYS_100M != sysclk) {
//        sys_clk_req(INDEX_VOICE, sysclk);
//    }
//
//    bt_call_init(&call_cfg);
//}
//
//static void bt_call_alg_exit(void)
//{
//    bt_sco_nr_exit();
//
//    bt_call_exit();
//
//    bt_sco_eq_exit();
//
//    plc_soft_v2_exit(0);
//
//    sys_clk_free(INDEX_VOICE);
//}
//
//GLOBAL_ALIGNED(64)
//void sco_set_incall_flag(u8 bit)
//{
//    GLOBAL_INT_DISABLE();
//    sys_cb.incall_flag |= bit;
//    GLOBAL_INT_RESTORE();
//}
//
//GLOBAL_ALIGNED(64)
//bool sco_clr_incall_flag(u8 bit)
//{
//    bool ret = false;
//    GLOBAL_INT_DISABLE();
//    if(sys_cb.incall_flag == INCALL_FLAG_FADE) {
//        ret = true;
//    }
//    sys_cb.incall_flag &= ~bit;
//    GLOBAL_INT_RESTORE();
//    return ret;
//}
//
//#if FUNC_BT_EN
//void sco_audio_init(void)
//{
//    fot_update_pause();
//
//    sco_set_incall_flag(INCALL_FLAG_SCO);
//    bt_call_alg_init();
//    dac_set_anl_offset(1);
//
//    memset(sco_sdadc_buf, 0, sizeof(sco_sdadc_buf));
//    audio_path_init(AUDIO_PATH_BTMIC, sco_sdadc_buf);
//    audio_path_start(AUDIO_PATH_BTMIC);
//    bsp_change_volume(bsp_bt_get_hfp_vol(sys_cb.hfp_vol));
//    dac_fade_in();
//}
//
//void sco_audio_exit(void)
//{
//    sco_clr_incall_flag(INCALL_FLAG_SCO);
//    dac_fade_out();
//    dac_aubuf_clr();
//    dac_set_anl_offset(0);
//    bsp_change_volume(sys_cb.a2dp_vol);
//    audio_path_exit(AUDIO_PATH_BTMIC);
//    bt_call_alg_exit();
//
//#if SYS_KARAOK_EN
//    bsp_karaok_init(AUDIO_PATH_KARAOK, FUNC_BT);
//#endif
//
//    fot_update_continue();
//}
//
//static void sfunc_bt_call_process(void)
//{
//    func_process();
//#if BT_TWS_MS_SWITCH_EN
//    if (xcfg_cb.bt_tswi_sco_en && bt_tws_need_switch(0)) {
//        printf("AUDIO SWITCH\n");
//        bt_tws_switch();
//    }
//#endif
//    func_bt_sub_process();
//    func_bt_status();
//}
//
//static void sfunc_bt_call_enter(void)
//{
//    if(sys_cb.pwroff.key_state == PWROFF_W4_TIMEOUT) {  //响铃/通话过程长按不关机时，清掉关机状态
//        sys_cb.pwroff.key_state = PWROFF_IDLE;
//    }
//
//    sco_set_incall_flag(INCALL_FLAG_CALL);
//    if(sys_cb.incall_flag == INCALL_FLAG_FADE) {
//        bsp_change_volume(bsp_bt_get_hfp_vol(sys_cb.hfp_vol));
//        dac_fade_in();
//    }
//#if DAC_DNR_EN
//    dac_dnr_set_sta(0);
//#endif
//#if WIRELESS_EN
//    //wireless_audio_bypass();
//#endif
//}
//
//static void sfunc_bt_call_exit(void)
//{
//#if DAC_DNR_EN
//    if (!bt_is_low_latency()) {
//        dac_dnr_set_sta(sys_cb.dnr_sta);
//    }
//#endif
//    bool vol_change = sco_clr_incall_flag(INCALL_FLAG_CALL);
//    if(vol_change) {
//        bsp_change_volume(sys_cb.a2dp_vol);
//    }
//#if WIRELESS_EN
//    //wireless_audio_enable();
//#endif
//}
//
//AT(.text.func.bt)
//void sfunc_bt_call(void)
//{
//    printf("%s\n", __func__);
//
//    sfunc_bt_call_enter();
//
//    while ((f_bt.disp_status >= BT_STA_OUTGOING) && (func_cb.sta == FUNC_BT)) {
//        sfunc_bt_call_process();
//        sfunc_bt_call_message(msg_dequeue());
//        func_bt_display();
//    }
//    sfunc_bt_call_exit();
//}
//
//#else
//
//void sco_audio_init(void){}
//void sco_audio_exit(void){}
//
//#endif //FUNC_BT_EN
