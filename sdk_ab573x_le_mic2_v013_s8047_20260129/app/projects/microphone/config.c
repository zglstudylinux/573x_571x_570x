#include "include.h"

AT(.rodata.func.table)
const u8 func_sort_table[] = {

#if FUNC_BT_EN
    FUNC_BT,
#endif

#if FUNC_DEVICE_EN
    FUNC_DEVICE,
#endif

#if FUNC_USBDEV_EN
    FUNC_USBDEV,
#endif // FUNC_USBDEV_EN

#if FUNC_IDLE_EN
    FUNC_IDLE,
#endif
};

AT(.text.func)
u8 get_funcs_total(void)
{
    return sizeof(func_sort_table);
}

u32 getcfg_vddio_sel(void)
{
    return xcfg_cb.vddio_sel;
}

u32 getcfg_pmu_mode(void)
{
    u32 pmu_cfg = 0;
    if (BUCK_MODE_EN) {
        pmu_cfg |= BIT(0);      //vddbt buck en
        if (xcfg_cb.sido_mode_en) {
            pmu_cfg |= BIT(2);  //vddbt buck sido
        }
    } else {
        if(xcfg_cb.vddbt_capless_en) {
            pmu_cfg |= BIT(3);  //vddbt capless
        }
    }

    if(xcfg_cb.dac_maxout_en) {
        pmu_cfg |= (DAC_MAXOUT_VCM_SEL<<4);     //DAC大功率
    } else {
        pmu_cfg |= DAC_VCM_SEL<<4;              //DAC正常功率
    }
    pmu_cfg |= VDDBT_LIMIT_SEL<<11;             //vddbt limit, bit11~13
    pmu_cfg |= VDDIO_LIMIT_SEL<<14;             //vddio limit, bit14~16
    return pmu_cfg;
}

u32 getcfg_mic_bias_method(u8 mic_ch)
{
    if (mic_ch == MIC0) {
        return xcfg_cb.mic0_bias_method;
    }
    if (mic_ch == MIC1) {
        return xcfg_cb.mic1_bias_method;
    }
    return 0;
}

u32 getcfg_mic_gain(u8 mic_ch)
{
    if (mic_ch == MIC0) {
        return xcfg_cb.bt_mic0_dig_gain;
    }
    if (mic_ch == MIC1) {
        return xcfg_cb.bt_mic1_dig_gain;
    }
    return 0;
}

u32 getcfg_mic_power_level(void)
{
    return xcfg_cb.mic_pwr_level;
}

#if BT_A2DP_LDAC_AUDIO_EN
u8 *getcfg_soft_key(void)
{
    return xcfg_cb.soft_key;
}
#endif

u32 get_dac_cfg(void)
{
    return (DAC_CH_SEL & 0x0f) | (0x200 * DAC_FAST_SETUP_EN) | (0x400 * DAC_VCM_CAPLESS_EN) | (0x800 * DAC_MAXOUT_EN);
}

void get_mic_cfg(u8 path_idx, u32 *channel, u32 *dig_gain, u32 *anl_gain)
{
    u8 mic_cnt = 0;
    u16 ch_sel = 0;
    u16 mic_list[2] = {xcfg_cb.call_talk_mic, 0};
    u8 mic_mapping_tbl[2] = {CH_MIC0, CH_MIC1};

#if BT_SCO_DMIC_EN
    if (path_idx == AUDIO_PATH_BTMIC) {
        mic_cnt = 2;
    } else if (path_idx == AUDIO_PATH_ASR) {
        mic_cnt = 1;
    }
    printf("MMIC --> %d, SMIC --> %d\n", mic_list[0], mic_list[1]);
#else
    mic_cnt = 1;
    printf("MMIC --> %d\n", mic_list[0]);
#endif

#if WIRELESS_DUAL_MIC_EN
    mic_cnt = 2;
#endif // WIRELESS_DUAL_MIC_EN

    for (u8 i = 0; i < mic_cnt; i++) {
        if ((mic_list[i] + 1) > MIC1) {
            continue;
        }
        ch_sel |= mic_mapping_tbl[mic_list[i]] << (8 * i);
    }

    *channel    = ch_sel;
    *dig_gain   = ((xcfg_cb.bt_mic0_dig_gain) | (xcfg_cb.bt_mic1_dig_gain << 6));
}
