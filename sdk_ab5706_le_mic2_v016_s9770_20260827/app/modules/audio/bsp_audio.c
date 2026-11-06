#include "include.h"


//MIC analog gain: 0~14(共15级), step 3DB (0db ~ +42db)
//adadc digital gain: 0~63, step 0.5 DB, 保存在gain的低6bit
const sdadc_cfg_t rec_cfg_tbl[1] = {
/*   通道,             采样率,    模拟增益, 数字增益,    BITS,    通路控制,    样点数,   回调函数*/
//    {AUX_CHANNEL_CFG,  SPR_44100,   2,         0,       0xff,     ADC2DAC_EN,    256,    aux_sdadc_callback},            /* AUX     */
//    {MIC_CHANNEL_CFG,  SPR_48000,   10,        0,       0xff,     ADC2DAC_EN,    256,    speaker_sdadc_callback},        /* SPEAKER */
//    {MIC_CHANNEL_CFG,  SPR_8000,    12,        0,       1,        ADC2DAC_EN,    240,    bt_sdadc_callback},             /* BTMIC   */
//    {MIC_CHANNEL_CFG,  SPR_48000,   12,        0,       1,        ADC2DAC_EN,    256,    usbmic_sdadc_callback},         /* USBMIC  */
//    {MIC_CHANNEL_CFG,  SPR_44100,   12,        0,       0xff,     ADC2SRC_EN,    256,    karaok_sdadc_callback},         /* KARAOK  */
//    {MIC_CHANNEL_CFG,  SPR_16000,   12,        0,       0xff,     ADC2DAC_EN,    256,    opus_sdadc_callback},           /* opus  */
//    {MIC_CHANNEL_CFG,  SPR_48000,   10,        0,       0xff,     ADC2DAC_EN,    256,    iodm_test_sdadc_callback},      /* IODM TEST */
};

/*****************************************************************************
 * 功能   : 初始化对应AUDIO_PATH
 * 输入   : audio_path_idx 对应不同功能使用的audio通路
 * 注意   : 区分bt_call和其他状态
 * 返回   : 无
 *****************************************************************************/
void audio_path_init(u8 path_idx, u8 *sdadc_buf)
{
    sdadc_cfg_t cfg;
    memcpy(&cfg, &rec_cfg_tbl[path_idx], sizeof(sdadc_cfg_t));

#if FPGA_EN
    audio_fpag_setup(cfg.channel);
    bsp_auphy_set_mic_analog_gain(cfg.channel, xcfg_cb.mic_anl_gain);
#endif

    sdadc_init(&cfg, sdadc_buf);

}

/*****************************************************************************
 * 功能   : 启动AUDIO采集和DAC数据处理
 * 输入   : audio_path_idx 对应不同功能使用的audio通路
 * 注意   : channel需要和init时保持一致，否则通路会启动失败
 * 返回   : 无
 *****************************************************************************/
void audio_path_start(u8 path_idx)
{
    sdadc_cfg_t cfg;
    memcpy(&cfg, &rec_cfg_tbl[path_idx], sizeof(sdadc_cfg_t));

    sdadc_start(cfg.channel);
}

/*****************************************************************************
 * 功能   : 关闭对应AUDIO_PATH
 * 输入   : audio_path_idx 对应不同功能使用的audio通路
 * 注意   : 关闭audio后，功耗要和打开audio之前保持一致
 * 返回   : 无
 *****************************************************************************/
void audio_path_exit(u8 path_idx)
{
    sdadc_cfg_t cfg;
    memcpy(&cfg, &rec_cfg_tbl[path_idx], sizeof(sdadc_cfg_t));

    sdadc_exit(cfg.channel);

    {
        adpll_spr_set(DAC_OUT_SPR);
    }
}
