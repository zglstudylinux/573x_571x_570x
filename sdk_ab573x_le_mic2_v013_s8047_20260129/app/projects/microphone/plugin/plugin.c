#include "include.h"

volatile int pwrkey_detect_flag;            //pwrkey 820K用于复用检测的标志。

#if WIRELESS_MIC_AINS4_EN
//AINS4降噪运行中可调状态。
//ains4_noise_nt：1~32767，20*log10(nt/32768) 即降噪量。值越小底噪越小（降噪越深），越大越浅。
static s16 ains4_noise_cur = AINS4_NOISE_NT;

#define AINS4_NOISE_MAX         32767
#define AINS4_NOISE_MIN         1

//PP短按：开/关AINS4降噪（取反标志位后直接下发mute）
AT(.text.plugin.ains4)
void plugin_ains4_toggle(void)
{
    static u8 ains4_mute_flag = 0;

    if (!wireless_cb.connected_sta) {
        return;
    }

    ains4_mute_flag = !ains4_mute_flag;
    ains4_mic_mute_set(ains4_mute_flag);

    printf("AINS4 %s (noise=%d)\n", ains4_mute_flag ? "OFF" : "ON", ains4_noise_cur);
}

//NEXT短按：降噪量加（底噪变大，降噪变浅）
AT(.text.plugin.ains4)
void plugin_ains4_level_up(void)
{
    s16 nt = ains4_noise_cur;
    s16 step = (nt < 200) ? 50 : (nt < 500) ? 100 : (nt < 2000) ? 500 : (nt < 8000) ? 2000 : 8000;

    if (!wireless_cb.connected_sta) {
        return;
    }

    nt += step;
    if (nt > AINS4_NOISE_MAX) { nt = AINS4_NOISE_MAX; }

    ains4_mic_param_set(0, nt);
    ains4_noise_cur = nt;
    printf("AINS4 noise = %d\n", nt);
}

//PREV短按：降噪量减（底噪变小，降噪更深）
AT(.text.plugin.ains4)
void plugin_ains4_level_down(void)
{
    s16 nt = ains4_noise_cur;
    s16 step = (nt <= 200) ? 50 : (nt <= 500) ? 100 : (nt <= 2000) ? 500 : (nt <= 8000) ? 2000 : 8000;

    if (!wireless_cb.connected_sta) {
        return;
    }

    nt -= step;
    if (nt < AINS4_NOISE_MIN) { nt = AINS4_NOISE_MIN; }

    ains4_mic_param_set(0, nt);
    ains4_noise_cur = nt;
    printf("AINS4 noise = %d\n", nt);
}
#endif // WIRELESS_MIC_AINS4_EN


void plugin_init(void)
{
    CLKGAT0 = 0xffffffff;
    CLKGAT1 = 0xffffffff;
    CLKGAT2 = 0xffffffff;
    CLKGAT3 = 0xffffffff;

//    CLKGAT0 = BIT(0)|BIT(12);
//    CLKGAT1 = BIT(1)|BIT(2)|BIT(3)|BIT(18)|BIT(19)|BIT(20)|BIT(21)|BIT(22)|BIT(23)|BIT(24)|BIT(25)|BIT(26)|BIT(27)|BIT(28)|BIT(29)|BIT(30);
//    CLKGAT2 = BIT(2)|BIT(10)|BIT(11)|BIT(12)|BIT(15)|BIT(17)|BIT(23)|BIT(24)|BIT(25)|BIT(26)|BIT(29);
//    CLKGAT3 = 0;
    CLKCON2 &= ~(BIT(29)|BIT(30));  //USB

#if UART0_PRINTF_SEL != PRINTF_NONE
    CLKGAT0 |= BIT(6);
#endif

#if (LANG_SELECT == LANG_EN_ZH)
    multi_lang_init(sys_cb.lang_id);
#endif

#if USB_SUPPORT_EN
    CLKGAT0 |= BIT(15);
#endif
#if SD_SUPPORT_EN
    CLKGAT0 |= BIT(16);
#endif

}

void plugin_var_init(void)
{
    pwrkey_detect_flag = 0;

#if FUNC_AUX_EN
    if (!xcfg_cb.func_aux_en) {
        xcfg_cb.aux_mode_2_pwroff_en = 0;
        xcfg_cb.aux_det_iosel = 0;
        xcfg_cb.auxl_sel = 0;
        xcfg_cb.auxr_sel = 0;
        xcfg_cb.mode_2_aux_en = 0;
    }
#endif // FUNC_AUX_EN

    if (!xcfg_cb.led_disp_en) {
        xcfg_cb.bled_io_sel = 0;
        xcfg_cb.charge_full_bled = 0;
        xcfg_cb.led_sta_config_en = 0;
    }

#if FUNC_AUX_EN
    if (!AUX_DETECT_EN) {
        xcfg_cb.aux_det_iosel = 0;
    }
#endif

    if (!xcfg_cb.led_pwr_en) {
        xcfg_cb.rled_io_sel = 0;
    }

    if (xcfg_cb.buck_mode_en) {
        xcfg_cb.vddbt_capless_en = 0;
    }

    if (!xcfg_cb.charge_en) {
        xcfg_cb.chbox_en = 0;
    }

    if (!xcfg_cb.chbox_en) {
        xcfg_cb.ch_box_type_sel            = 3;
        xcfg_cb.ch_out_auto_pwron_en       = 0;
        xcfg_cb.ch_leakage_sel             = 0;
        xcfg_cb.chg_inbox_pwrdwn_en        = 0;
        xcfg_cb.chbox_out_delay            = 0;
    }

#if (CHARGE_BOX_TYPE == CBOX_SSW)
    if (xcfg_cb.chbox_en) {
        xcfg_cb.ch_box_type_sel            = 2;     //昇生微智能充电仓为维持电压仓
        xcfg_cb.chg_inbox_pwrdwn_en        = 0;
    }
#endif

#if TKEY_SOFT_PWR_EN
    sys_cb.tkey_pwrdwn_en = 1;
#endif

    //PWRKEY模拟硬开关,需要关闭长按10S复位， 第一次上电开机， 长按5秒进配对等功能。
    if (PWRKEY_2_HW_PWRON) {
        xcfg_cb.powkey_10s_reset = 0;
//        xcfg_cb.pwron_frist_bat_en = 0;
        xcfg_cb.bt_pwrkey_nsec_discover = 0;
    }

#if LED_DISP_EN
	led_func_init();
#endif // LED_DISP_EN

    if (!xcfg_cb.mic1_en) {
        xcfg_cb.mic1_bias_method = 0;
    }

#if CHARGE_BOX_TYPE == CBOX_NWB
    xcfg_cb.ch_box_type_sel = 4;
    xcfg_cb.ch_leakage_sel = 0;
#endif

    mic_bias_var_init();

#if TSEN_DETECT_EN
    tsen_var_init();
#endif
}

AT(.com_text.plugin)
void plugin_tmr5ms_isr(void)
{
#if ENERGY_LED_EN
    energy_led_level_calc();
#endif
}

AT(.com_text.plugin)
void plugin_tmr1ms_isr(void)
{
#if ENERGY_LED_EN
    energy_led_scan();
#endif
}

bool bt_hfp_ring_number_en(void)
{
    return (!BT_HFP_INBAND_RING_EN && 0);
}

AT(.com_text.bsp.sys)
bool is_sd_support(void)
{
#if (SD0_MAPPING == SD0MAP_G2) && EQ_DBG_IN_UART
    if (xcfg_cb.huart_en && xcfg_cb.huart_sel == 1) {
        return false;
    }
#endif
    return (SD_SUPPORT_EN);
}

#if USB_SUPPORT_EN
AT(.com_text.bsp.sys)
bool is_usb_support(void)
{
#if EQ_DBG_IN_UART
    if (xcfg_cb.huart_en && xcfg_cb.huart_sel == 2) {
        return false;
    }
#endif
    return USB_SUPPORT_EN;
}
#else
AT(.com_text.bsp.sys)
bool is_usb_support(void)
{
    return false;
}
#endif // USB_SUPPORT_EN

bool is_sleep_dac_off_enable(void)
{
#if FUNC_AUX_EN
    if (xcfg_cb.aux_det_iosel == IO_MUX_MICL) {
        return false;
    }
#endif
    return true;
}

//设置piano提示音播放的数字音量 (0 ~ 0x7fff)
u32 piano_get_digvol_cb(void)
{
    return bsp_volume_vol2dvol(WARNING_VOLUME);
}

//设置提示音播放的数字音量 (0 ~ 0x7fff)
u32 waring_get_digvol_cb(void)
{
    return bsp_volume_vol2dvol(WARNING_VOLUME);
}

void maxvol_tone_play(void)
{
	bsp_res_play(RES_IDX_MAX_VOL);
}

void minvol_tone_play(void)
{

}

void plugin_playmode_warning(void)
{

}

void plugin_lowbat_vol_reduce(void)
{
#if LPWR_REDUCE_VOL_EN
    music_src_set_volume(0x50c0);       //设置音乐源音量达到整体降低系统音量 (范围：0~0x7fff)
#endif // LPWR_REDUCE_VOL_EN
}

void plugin_lowbat_vol_recover(void)
{
#if LPWR_REDUCE_VOL_EN
    music_src_set_volume(0x7fff);       //还原音量
#endif // LPWR_REDUCE_VOL_EN
}

//用于调DAC音质接口函数
void plugin_music_eq(void)
{
    bsp_eq_init();
    sys_cb.eq_mode = 0;
    music_eq_set_by_num(sys_cb.eq_mode);
}

AT(.com_text.port.vbat)
void plugin_vbat_filter(u32 *vbat)
{
#if  VBAT_FILTER_USE_PEAK
    //电源波动比较大的音箱方案, 取一定时间内的电量"最大值"或"次大值",更能真实反应电量.
    #define VBAT_MAX_TIME  (3000/5)   //电量峰值判断时间 3S
    static u16 cnt = 0;
	static u16 vbat_max_cnt = 0;
    static u32 vbat_max[2] = {0,0};
    static u32 vbat_ret = 0;
    u32 vbat_cur = *vbat;
    if (cnt++  < VBAT_MAX_TIME) {
        if (vbat_max[0] < vbat_cur) {
            vbat_max[1] = vbat_max[0];  //vbat_max[1] is less max (次大值)
            vbat_max[0] = vbat_cur;     //vbat_max[0] is max(最大值)
            vbat_max_cnt = 0;
        } else if (vbat_max[0] == vbat_cur) {
            vbat_max_cnt ++;
        }
    } else {
        if (vbat_max_cnt >= VBAT_MAX_TIME/5) {  //总次数的(1/5)都采到最大值,则返回最大值.
            vbat_ret = vbat_max[0];
        } else if (vbat_max[1] != 0) {   //最大值次数较少,则返回次大值(舍弃最大值)
            vbat_ret = vbat_max[1];
        }
        vbat_max[0] = 0;
        vbat_max[1] = 0;
        vbat_max_cnt = 0;
        cnt = 0;
    }
    //返回值
    if (vbat_ret != 0) {
        *vbat = vbat_ret;
    }
#endif
}

//初始化完成, 各方案可能还有些不同参数需要初始化,预留接口到各方案
void plugin_sys_init_finish_callback(void)
{
#if ENERGY_LED_EN
    energy_led_init();
#endif
}

bool plugin_func_idle_enter_check(void)
{
    //可以在这里决定否需要进入idle
    return true;
}



