#include "include.h"
#include "func.h"
#include "func_bt.h"


func_bt_t f_bt;
void uart_cmd_process(void);


GLOBAL_ALIGNED(64)
u16 func_bt_chkclr_warning(u16 bits)
{
    u16 value;
    GLOBAL_INT_DISABLE();
    value = f_bt.warning_status & bits;
    if(value != 0) {
        f_bt.warning_status &= ~value;
        GLOBAL_INT_RESTORE();
        return value;
    }
    GLOBAL_INT_RESTORE();
    return value;
}


#if FUNC_BT_EN
void func_bt_set_dac(u8 enable)
{
    if (bsp_dac_off_for_bt_conn()) {
        if (enable) {
            if (!dac_get_pwr_sta()) {
                dac_restart();
            }
        } else {
            if (dac_get_pwr_sta()) {
                dac_power_off();
            }
        }
    }
}

//切换提示音语言
void func_bt_switch_voice_lang(void)
{
#if (LANG_SELECT == LANG_EN_ZH)
    if (xcfg_cb.lang_id >= LANG_EN_ZH) {
        sys_cb.lang_id = (sys_cb.lang_id) ? 0 : 1;
        multi_lang_init(sys_cb.lang_id);
        param_lang_id_write();
        param_sync();
        if (xcfg_cb.bt_tws_en) {
            bt_tws_sync_setting();                                              //同步语言
        }

        bsp_res_play(RES_IDX_LANGUAGE_EN + sys_cb.lang_id);                     //同步播放语言提示音
    }
#endif
}

#if BT_TWS_EN
static void func_bt_tws_set_channel(void)
{
    if(f_bt.tws_status & FEAT_TWS_FLAG) {   //对箱状态.
        tws_get_lr_channel();
        dac_mono_init(0, sys_cb.tws_left_channel);
    } else {
        dac_mono_init(1, 0);
    }
}
#endif

u8 func_bt_tws_get_channel(void)
{
#if BT_TWS_EN
    return sys_cb.tws_left_channel;
#else
    return false;
#endif
}

void func_bt_warning_do(void)
{

    if(func_bt_chkclr_warning(BT_WARN_TWS_DISCON | BT_WARN_TWS_CON)) {
#if BT_TWS_EN
        if(xcfg_cb.bt_tws_en) {
            if(xcfg_cb.bt_tws_lr_mode != 0) {
                func_bt_tws_set_channel();
            }
        }
#endif
    }

    if(func_bt_chkclr_warning(BT_WARN_DISCON)) {
#if WARNING_BT_DISCONNECT
        if(!bt_tws_is_slave()) {
            bsp_res_play(RES_IDX_DISCONNECT);
            return;
        }
#endif // WARNING_BT_DISCONNECT
    }

    if(func_bt_chkclr_warning(BT_WARN_PAIRING)) {
        if(!bt_tws_is_slave()) {
            bsp_res_play(RES_IDX_PAIRING);
            return;
        }
    }

#if BT_TWS_EN
    if(xcfg_cb.bt_tws_en) {
        u16 tws_warning = func_bt_chkclr_warning(BT_WARN_TWS_SCON | BT_WARN_TWS_MCON );
        if(tws_warning != 0) {
            f_bt.tws_had_pair = 1;
            if (xcfg_cb.bt_tws_lr_mode != 0) {
                func_bt_tws_set_channel();
            }

            if (xcfg_cb.bt_tws_lr_mode >= 8) {      //固定声道方案
                func_bt_tws_set_channel();
                tws_get_lr_channel();
            }

            if (tws_warning & BT_WARN_TWS_MCON) {   //由主耳发起连接提示音
                bsp_res_play(RES_IDX_CONNECTED);
                return;
            }
        }
    }
#endif

	if(func_bt_chkclr_warning(BT_WARN_CON)) {
#if WARNING_BT_CONNECT
        if(!bt_tws_is_slave()) {
            bsp_res_play(RES_IDX_CONNECTED);
            return;
        }
#endif
    }

#if BT_HID_MANU_EN
    //按键手动断开HID Profile的提示音
    if (xcfg_cb.bt_hid_manu_en) {
    #if WARNING_BT_HID_MENU
        if (func_bt_chkclr_warning(BT_WARN_HID_CON)) {
            bsp_res_play(RES_IDX_CAMERA_ON);
        }
    #endif

    #if WARNING_BT_HID_MENU
        if (func_bt_chkclr_warning(BT_WARN_HID_DISCON)) {
            bsp_res_play(RES_IDX_CAMERA_OFF);
        }
    #endif

    #if BT_HID_DISCON_DEFAULT_EN
        if (f_bt.hid_discon_flag) {
            if (bt_hid_is_ready_to_discon()) {
                f_bt.hid_discon_flag = 0;
                bt_hid_disconnect();
            }
        }
    #endif
    }
#endif // BT_HID_MANU_EN
}

AT(.text.func.bt.process)
void func_bt_warning(void)
{
    if(f_bt.warning_status != 0 && !bsp_res_is_full()) {
        func_bt_warning_do();
    }
}

void func_bt_disp_status_do(void)
{
    if(!bt_is_connected()) {
#if WIRELESS_MIC_EN
        if (!wireless_con_get_status())
#endif
        {
            en_auto_pwroff();
            sys_cb.sleep_en = BT_PAIR_SLEEP_EN;
        }
    } else {
        dis_auto_pwroff();
        sys_cb.sleep_en = 1;
    }

    printf("bt_status: %d\n", f_bt.disp_status);

    switch (f_bt.disp_status) {
    case BT_STA_CONNECTING:
        if (BT_RECONN_LED_EN) {
            led_bt_reconnect();
            break;
        }
    case BT_STA_INITING:
    case BT_STA_IDLE:
        led_bt_idle();
#if WARNING_BT_PAIR
            if(f_bt.need_pairing && f_bt.disp_status == BT_STA_IDLE) {
                f_bt.need_pairing = 0;
                if(xcfg_cb.warning_bt_pair && xcfg_cb.bt_tws_en) {
                    f_bt.warning_status |= BT_WARN_PAIRING;
                }
            }
#endif
#if BT_SBC_AUDIO_EFFECT_EN
        sys_clk_free(INDEX_MUSIC);
#endif
        break;
    case BT_STA_SCANNING:
        led_bt_scan();
        break;

    case BT_STA_DISCONNECTING:
        led_bt_connected();
        break;

    case BT_STA_CONNECTED:
#if BT_SBC_AUDIO_EFFECT_EN
        sys_clk_req(INDEX_MUSIC, SYS_240M);
#endif
        led_bt_connected();
        break;
    case BT_STA_INCOMING:
        led_bt_ring();
        break;
    case BT_STA_PLAYING:
        led_bt_play();
        break;
    case BT_STA_OUTGOING:
    case BT_STA_INCALL:
        led_bt_call();
        break;
    }

    if(f_bt.disp_status >= BT_STA_CONNECTED) {
        f_bt.need_pairing = 1;
        func_bt_set_dac(1);
        dac_fade_in();
    } else {
        func_bt_set_dac(0);
    }
}

GLOBAL_ALIGNED(128)
void func_bt_disp_status(void)
{
    uint status = bt_get_disp_status();

    GLOBAL_INT_DISABLE();
    if(f_bt.disp_status != status || f_bt.disp_update) {
        f_bt.disp_status = status;
        f_bt.disp_update = 0;
        GLOBAL_INT_RESTORE();

        func_bt_disp_status_do();
    } else {
        GLOBAL_INT_RESTORE();
    }
}

AT(.text.func.bt.process)
void func_bt_status(void)
{
    func_bt_disp_status();
    {
        func_bt_warning();
    }
}

//借用读参数区做load flash的动作，使flash一直busy
static void func_bt_load_flash(void)
{
    static u32 tick = 0;
    u8 load_buf[32];
    if (tick_check_expire(tick, 1000)) {
//        printf("load flash test\n");
        tick = tick_get();
    }
    cm_read(load_buf, PAGE0(0), 32);
    cm_read(load_buf, PAGE1(0), 32);
    cm_read(load_buf, PAGE2(0), 32);
}

AT(.text.func.bt.process)
void func_bt_sub_process(void)
{
    func_bt_status();

#if TKEY_DEBUG_EN
    bsp_tkey_spp_tx();
#endif

    if ((func_cb.sta == FUNC_BT_DUT || BT_DUT_MODE_EN) && bt_is_connected()) {
        func_bt_load_flash();       //如果在DUT模式就让flash动起来，测试flash对rf的影响
    }
}

AT(.text.func.bt.process)
void func_bt_process(void)
{
    func_process();
    func_bt_sub_process();
#if BT_TWS_MS_SWITCH_EN
    if ((xcfg_cb.bt_tswi_msc_en) && bt_tws_need_switch(0) && !bsp_res_is_playing()) {
        printf("AUDIO SWITCH\n");
        bt_tws_switch();
    }
#endif

    if(sys_cb.pwroff_delay == 0) {
        sys_cb.pwroff.timeout_ind = 1;
        return;
    }
    if(sleep_process(bt_is_allow_sleep)) {
        f_bt.disp_status = 0xff;
    }
}

AT(.text.func.bt)
void func_bt_init(void)
{
    if (!f_bt.bt_is_inited) {
//        msg_queue_clear();
        func_bt_set_dac(0);
        bsp_bt_init();

#if (!ADAPTER_EN) && BT_SBC_AUDIO_EFFECT_EN
        load_code_wl_adapter();
        lock_code_wl_com();
#endif
        f_bt.bt_is_inited = 1;
    }
}

AT(.text.func.bt)
bool func_bt_chk_off(u8 reason)
{
    bool off_flag = (bool)(func_cb.sta != FUNC_BT);

    if (off_flag) {
        if(f_bt.bt_is_inited) {
#if BT_PWRKEY_5S_DISCOVER_EN
            bsp_bt_pwrkey5s_clr();
#endif
            bt_disconnect(reason);
            bt_off();
            func_bt_set_dac(1);
            f_bt.bt_is_inited = 0;
        }
        return true;
    }

    return false;
}

bool func_bt_charge_dcin(void)
{

    if(sys_cb.discon_reason == 0xff) {
        sys_cb.discon_reason = 0;   //不同步关机
    }

    return true;
}

AT(.text.func.bt)
void func_bt_enter(void)
{
    bool break_flag = false;

    bsp_loudspeaker_unmute();
    func_bt_enter_display();
    led_bt_init();
    func_bt_init();
    //en_auto_pwroff();

    if(!break_flag) {
#if WARNING_FUNC_BT
        bsp_res_play(RES_IDX_BT_MODE);
#endif

#if WARNING_BT_WAIT_CONNECT
        bsp_res_play(RES_IDX_WAIT4CONN);
#endif
    }

    f_bt.disp_status = 0xfe;
    f_bt.rec_pause = 0;
    f_bt.pp_2_unmute = 0;
    sys_cb.key2unmute_cnt = 0;

    if(sys_cb.vol != sys_cb.a2dp_vol && (sys_cb.incall_flag == 0)) {
        bsp_change_volume(sys_cb.a2dp_vol);
    }

    bt_redial_init();
    bt_audio_enable();
#if DAC_DNR_EN
    dac_dnr_set_sta(1);
    sys_cb.dnr_sta = 1;
#endif

#if BT_PWRKEY_5S_DISCOVER_EN
    if(bsp_bt_pwrkey5s_check()) {
        f_bt.need_pairing = 0;  //已经播报了
        func_bt_disp_status();
        bsp_res_play(RES_IDX_PAIRING);
    } else {
        func_bt_disp_status();
#if WARNING_BT_PAIR
        if (xcfg_cb.warning_bt_pair && !xcfg_cb.bt_tws_en) {
            bsp_res_play(RES_IDX_PAIRING);
        }
#endif // WARNING_BT_PAIR
    }
#endif
}

AT(.text.func.bt)
void func_bt_exit(void)
{
    u8 discon_reason = sys_cb.discon_reason;

#if BT_SBC_AUDIO_EFFECT_EN
    music_effect_exit();
#endif


    if(discon_reason == 0xff && func_cb.sta == FUNC_PWROFF) {
        discon_reason = 1;   //默认同步关机
    }
    bsp_res_w4_finish(true);

    dac_fade_out();
#if DAC_DNR_EN
    dac_dnr_set_sta(0);
    sys_cb.dnr_sta = 0;
#endif
#if BT_PWRKEY_5S_DISCOVER_EN
    bsp_bt_pwrkey5s_clr();
#endif
    func_bt_exit_display();
    bt_audio_bypass();
#if BT_TWS_EN
    dac_mono_init(1, 0);
#endif

    func_bt_chk_off(discon_reason);

    if(sys_cb.vol != sys_cb.a2dp_vol) {
        bsp_change_volume(sys_cb.vol);
    }

    f_bt.rec_pause = 0;
    f_bt.pp_2_unmute = 0;
    sys_cb.key2unmute_cnt = 0;
    func_bt_set_dac(1);
    unlock_code_btdec();
    func_cb.last = FUNC_BT;
}

AT(.text.func.bt)
void func_bt(void)
{
    printf("%s\n", __func__);

    func_bt_enter();

    while (func_cb.sta == FUNC_BT) {
        func_bt_process();
        func_bt_message(msg_dequeue());
        func_bt_display();
    }

    func_bt_exit();
}

#endif //FUNC_BT_EN
