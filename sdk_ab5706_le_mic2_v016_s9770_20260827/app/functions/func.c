#include "include.h"
#include "func.h"

void func_udisk_insert(u8 func_sta);
void func_udisk_remove(u8 func_sta);
void func_sdcard_insert(u8 func_sta);
void func_sdcard_remove(u8 func_sta);
void func_sdcard1_insert(u8 func_sta);
void func_sdcard1_remove(u8 func_sta);


static const struct {
    const void *msg_tbl;
    u8 dev_online;      //0=不检查，!0=检查对应bit的dev_online
} func_info[] = {
#if FUNC_BT_EN
    [FUNC_BT]       = {bt_music_key_msg_tbl,    0},
#endif
#if FUNC_ADAPTER_EN
    [FUNC_ADAPTER]  = {adapter_key_msg_tbl,     0},
#endif
#if FUNC_DEVICE_EN
    [FUNC_DEVICE]   = {device_key_msg_tbl,      0},
#endif
    [FUNC_CHARGE]   = {NULL, 0},
    [FUNC_PWROFF]   = {NULL, 0},
    [FUNC_BT_IODM]  = {NULL, 0},
    [FUNC_IDLE]     = {NULL, 0},
    [FUNC_LE_DUT]   = {NULL, 0},
};

func_cb_t func_cb AT(.buf.func_cb);

AT(.text.func.bt)
void func_bt_init(void)
{
    if (!sys_cb.bt_is_inited) {
        msg_queue_clear();
        wireless_init();

        sys_cb.bt_is_inited = 1;
    }
}

#if VBAT_DETECT_EN
void lowbat_warning_proc(void)
{
    sys_cb.lbat_warning_delay = 0;          //延时计数清零

    if (sys_cb.lbat_warning_times) {        //低电语音提示次数
        if (RLED_LOWBAT_FOLLOW_EN) {
            led_lowbat_follow_warning();
        }

        bsp_res_play(RES_IDX_LOW_BATTERY);

        if (sys_cb.lbat_warning_times != 0xff) {
            sys_cb.lbat_warning_times--;
        }
    }
}

void lowbat_warning_start(void)
{
    sys_cb.lowbat_flag = 1;
    plugin_lowbat_vol_reduce();         //低电降低音乐音量

    lowbat_warning_proc();
}

void lowbat_warning_stop(void)
{
    sys_cb.lowbat_flag = 0;
    sys_cb.lbat_warning_times = LPWR_WARING_TIMES;
    plugin_lowbat_vol_recover();    //离开低电, 恢复音乐音量
}

AT(.text.func.msg)
void lowbat_process(void)
{
    uint vbat_sta = vbat_get_lpwr_sta();

    switch(vbat_sta) {
    case 2:
        if(sys_cb.lpwr_status == LBAT_STA_W4_ENTER) {
            if(tick_check_expire(sys_cb.lpwr_tick, 1000)) {
                sys_cb.lpwr_status = LBAT_STA_WARNING;

                if (LPWR_OFF_VBAT) {                    //是否关机
                    sys_cb.pwroff.low_bat_ind = 1;
                }
            }
        } else {
            sys_cb.lpwr_status = LBAT_STA_W4_ENTER;
            sys_cb.lpwr_tick = tick_get();
        }

        if(sys_cb.vbat_warning <= sys_cb.vbat_pwroff) {
            goto __lbat_process_sta;
        }
        break;

    case 1:
        sys_cb.lpwr_status = LBAT_STA_IDLE;

__lbat_process_sta:
        if(sys_cb.lbat_status == LBAT_STA_W4_ENTER) {
            if(tick_check_expire(sys_cb.lbat_tick, 1000)) {
                sys_cb.lbat_status = LBAT_STA_WARNING;

                lowbat_warning_start();                 //进入低电模式
            }
        } else if(sys_cb.lbat_status == LBAT_STA_W4_EXIT) {
            sys_cb.lbat_tick = tick_get();
        } else if(sys_cb.lbat_status != LBAT_STA_WARNING) {
            sys_cb.lbat_status = LBAT_STA_W4_ENTER;
            sys_cb.lbat_tick = tick_get();
        }
        break;

    case 0:
        sys_cb.lpwr_status = LBAT_STA_IDLE;

        if(sys_cb.lbat_status == LBAT_STA_W4_EXIT) {
            if(sys_cb.vbat > 3800) {                    //等待恢复到多少v电压
                sys_cb.lbat_tick = tick_get();
            } else if(tick_check_expire(sys_cb.lbat_tick, 500)) {
                sys_cb.lbat_status = LBAT_STA_IDLE;

                lowbat_warning_stop();                  //退出低电模式
            }
        } else if(sys_cb.lbat_status == LBAT_STA_WARNING) {
            sys_cb.lbat_status = LBAT_STA_W4_EXIT;
            sys_cb.lbat_tick = tick_get();
        } else {
            sys_cb.lbat_status = LBAT_STA_IDLE;
        }
        break;
    }

#if TRACE_EN
    static u8 lpwr_sta = 0xff;
    static u8 lbat_sta = 0xff;
    static u32 tick = 0;
    if(lpwr_sta != sys_cb.lpwr_status || lbat_sta != sys_cb.lbat_status || tick_check_expire(tick, 1000)) {
        lpwr_sta = sys_cb.lpwr_status;
        lbat_sta = sys_cb.lbat_status;
        tick = tick_get();

        printf("@@@@@@ lpwr_sta: %d(%d), %d(%d), vbat=%d\n", sys_cb.lpwr_status, sys_cb.vbat_pwroff, sys_cb.lbat_status, sys_cb.vbat_warning, sys_cb.vbat);
    }
#endif

    if(sys_cb.lbat_status >= LBAT_STA_WARNING) {
        //重复播报低电提示音
        if (sys_cb.lbat_warning_delay >= xcfg_cb.lpwr_warning_period) {
            lowbat_warning_proc();
        }
    }

#if LED_LOWBAT_EN
    if (xcfg_cb.rled_lowbat_en) {
        if(sys_cb.lbat_status >= LBAT_STA_WARNING) {
            if ((!CHARGE_DC_IN()) && (!RLED_LOWBAT_FOLLOW_EN)) {
                led_lowbat();
            }
        } else {
            led_lowbat_recover();
        }
    }
#endif
}
#endif  //VBAT_DETECT_EN

AT(.text.func.process)
void pwroff_process(void)
{
    struct pwroff_tag *pwroff = &sys_cb.pwroff;

#if PWRKEY_2_HW_PWRON
    //PWRKEY模拟硬开关关机处理
    if (pwroff->hw_pwrdwn) {
        func_cb.sta = FUNC_PWROFF;
        pwroff->hw_pwrdwn = 0;
    }
#endif

    if(pwroff->key_state == PWROFF_W4_TIMEOUT) {
        if(tick_check_expire(pwroff->key_ticks, pwroff->delay_ticks)) {
            pwroff->key_state = PWROFF_END;
            pwroff->pwr_key_ind = 1;
        }
    }

    if(pwroff->all_flag != 0) {
        if(pwroff->charge_full_ind) {
        } else {
            pwroff->tone_en = 1;
        }

        //rtc_set_alarm_wakeup(20);

        func_cb.sta = FUNC_PWROFF;
    }
}

//提示音过程复用资源时暂停相关功能
void func_audio_bypass(u8 func_sta, u8 res_type)
{
#if WIRELESS_EN
    if(res_type == RES_TYPE_WSBC || res_type == RES_TYPE_MP3) {
        wireless_audio_bypass();
    }
#endif

    switch(func_sta) {

#if FUNC_BT_EN
    case FUNC_BT:
        bt_audio_bypass();
        break;
#endif

    default:
#if WARNING_SYSVOL_ADJ_EN
    {
        //其他模式资源不复用，可以系统音量退避
        u8 vol = bsp_get_vol();
        if(vol > SYS_BACKSTAGE_VOLUME) {
            bsp_change_volume(SYS_BACKSTAGE_VOLUME);
        }
    }
#endif
        break;
    }
}

//提示音过程复用资源时恢复相关功能
void func_audio_restore(u8 func_sta, u8 res_type)
{
    switch(func_sta) {
#if FUNC_BT_EN
    case FUNC_BT:
        bt_audio_enable();
        break;
#endif
    }

#if WIRELESS_EN
    if(res_type == RES_TYPE_WSBC || res_type == RES_TYPE_MP3) {
        void wireless_mic_load_code(void);
        wireless_mic_load_code();
        wireless_audio_enable();
    }
#endif
}

//音量加消息
void func_volume_up(void)
{
    printf("vol_up: %d, (a2dp=%d, hfp=%d)\n", sys_cb.vol, sys_cb.a2dp_vol, sys_cb.hfp_vol);

    bsp_set_volume(bsp_volume_inc(sys_cb.vol));
    if (sys_cb.vol == VOL_MAX) {
        maxvol_tone_play();
    }

    if (func_cb.set_vol_callback) {
        func_cb.set_vol_callback(1);
    }
}

//音量减消息
void func_volume_down(void)
{
    printf("vol_down: %d, (a2dp=%d, hfp=%d)\n", sys_cb.vol, sys_cb.a2dp_vol, sys_cb.hfp_vol);

    bsp_set_volume(bsp_volume_dec(sys_cb.vol));
    if (sys_cb.vol == 0) {
        minvol_tone_play();
    }

    if (func_cb.set_vol_callback) {
        func_cb.set_vol_callback(0);
    }
}

//设备插拔事件处理
void func_dev_process(uint dev_change)
{
}
void ledc_rgb5050_scan(void);
//func common event process
AT(.text.func.process)
void func_process(void)
{
    uint dev_change;
    static u8 recursion = 0;
    recursion++;

    WDT_CLR();
    vusb_reset_clr();
#if LEDC_HW_EN
    ledc_rgb5050_scan();
#endif
    sys_cb.sys_delay = 0;
    if(recursion > 1) {     //检查递归
        printf("func_err, %d, %x\n", recursion, (uint32_t)__builtin_return_address(0));
    }


    //关机也需要处理的事件
#if CHARGE_EN
    if (xcfg_cb.charge_en) {
        bsp_charge_process();
        if (sys_cb.charge_backstage) {
            if (tick_check_expire(sys_cb.charge_tick, 200)) {
                sys_cb.charge_tick = tick_get();
                vusb4s_reset_dis();
                vusb_reset_clr();
//                charge_bled_ctrl();
            }

        }
    }
#endif // CHARGE_EN

    bsp_res_process();
    if(func_cb.curr == FUNC_PWROFF) {
        recursion--;
        return;
    }

    //以下是开机状态处理的事件
    dev_change = sys_cb.dev_active ^ dev_get_online();
    if(dev_change != 0) {
        sys_cb.dev_active ^= dev_change;
        func_dev_process(dev_change);
    }

#if CHARGE_EN
    if (charge_get_dc_sta() && func_cb.sta != FUNC_CHARGE) {
    //使用5V VUSB供电时,保持正常工作,不进入FUNC_CHARGE
#if !CHARGE_5V_POWER_SUPPLY_EN
        if (func_charge_dcin()) {
            if (!CHARGE_ON_WORK_EN) {
                sys_cb.pwroff.tone_en = 0;
                func_cb.sta = FUNC_CHARGE;
            } else if (!sys_cb.charge_backstage) {
                vusb4s_reset_dis();
                sys_cb.charge_backstage = 1;
                sys_cb.charge_tick = tick_get();
            }
        }
#endif
    } else {
         sys_cb.charge_backstage = 0;
    }
#endif

#if VBAT_DETECT_EN
    lowbat_process();
#endif // VBAT_DETECT_EN

    pwroff_process();

    if(sys_cb.bt_is_inited) {
#if ADAPTER_SAVE_PARAM_EN
        wireless_con_interact_process();
#endif
        bt_thread_check_trigger();
        le_popup_process();
    }

    gsensor_process();

    recursion--;
}

//func common message process
AT(.text.func.msg)
void func_message(u16 msg)
{
    switch (msg) {
        case MSG_PWR_HOLD:
            if(sys_cb.pwroff.key_state == PWROFF_IDLE) {
                sys_cb.pwroff.key_ticks = tick_get();
                sys_cb.pwroff.key_state = PWROFF_W4_TIMEOUT;
            }
            break;
        case MSG_PWR_RELEASE:
            if(sys_cb.pwroff.key_state == PWROFF_W4_TIMEOUT) {
                sys_cb.pwroff.key_state = PWROFF_IDLE;
                bsp_key_recover_msg();
            }
            break;
        case MSG_PWR_OFF:
            sys_cb.pwroff.pwr_key_ind = 1;
            break;

        case MSG_VOL_UP:
            func_volume_up();
            break;

        case MSG_VOL_DOWN:
            func_volume_down();
            break;

#if EQ_MODE_EN
        case MSG_CHANGE_EQ:
            sys_set_eq();
            break;
#endif // EQ_MODE_EN

        case MSG_VOL_MUTE:
            if (sys_cb.mute) {
                bsp_sys_unmute();
            } else {
                bsp_sys_mute();
            }
            break;

        case MSG_LEDC_ON_OFF:
#if LEDC_HW_EN
            if (sys_cb.ledc_dispmode){
                bsp_ledc_start();
                sys_cb.ledc_dispmode = 0;
            } else {
                bsp_ledc_exit();
                sys_cb.ledc_dispmode = 1;
            }
#endif // LEDC_HW_EN
            break;
        case MSG_LEDC_CHANGE:
#if LEDC_HW_EN
            rgb_ledc_mode_change();
#endif // LEDC_HW_EN
            break;

        case MSG_SYS_500MS:
            break;

#if FUNC_BT_EN
        case EVT_A2DP_SET_VOL:
        case EVT_TWS_INIT_VOL:
            if((sys_cb.incall_flag & INCALL_FLAG_SCO) == 0) {
                if((func_cb.sta == FUNC_BT) && !bsp_res_is_vol_busy()) {
                    bsp_change_volume(sys_cb.a2dp_vol);
                }
            }
            //no break
        case EVT_TWS_SET_VOL:
            if (sys_cb.incall_flag == 0) {
                if(func_cb.sta == FUNC_BT) {
                    printf("A2DP_VOL: %d\n", sys_cb.a2dp_vol);
                    if(msg != EVT_TWS_INIT_VOL) {   //TWS同步音量，不需要显示
                        gui_box_show_vol();
                    }
                }
            }
            //no break
        case EVT_DEV_SAVE_VOL:
            sys_cb.cm_times = 0;
            sys_cb.cm_vol_change = 1;
            break;
#endif

#if EQ_DBG_IN_UART || EQ_DBG_IN_SPP
        case EVT_ONLINE_SET_EQ:
            bsp_eq_parse_cmd();
            break;
#if EFFECT_DBG_ADJUST_IN_UART
        case EVT_ONLINE_SET_EFFECT:
            toolkit_process();
            break;
#endif
#endif
#if TOOLKIT_AUDIO_DUMP_EN
        case EVT_ONLINE_SET_BTMIC_DUMP:
            bsp_btmic_dump_parse_cmd();
            break;
#endif
#if LANG_SELECT == LANG_EN_ZH
        case EVT_BT_SET_LANG_ID:
            param_lang_id_write();
            param_sync();
            break;
#endif

#if EQ_MODE_EN
        case EVT_BT_SET_EQ:
            music_eq_set_by_num(sys_cb.eq_mode);
            break;
#endif

        case EVT_WL_MIC_KEY_TONE:                                                        //处理消息 话筒按键提示音
            bsp_res_play(RES_IDX_WL_KEY_TONE);
			break;
        default:
            break;
    }

    //调节音量，3秒后写入flash
    if ((sys_cb.cm_vol_change) && (sys_cb.cm_times >= 3)) {
        sys_cb.cm_vol_change = 0;
        param_hfp_vol_write();
        param_sys_vol_write();
        cm_sync();
    }
}

///进入一个功能的总入口
AT(.text.func)
void func_enter(void)
{
    u8 func_sta = func_cb.sta;

    printf("====>func%d: %d->%d\n", 0, func_cb.curr, func_cb.sta);
    func_cb.curr = FUNC_NULL;       //退出当前模式清掉，避免bsp_res_w4时恢复错误
    bsp_res_w4_finish(false);       //切到下一模式前，先等当前提示音播完，此处可能有新设备插入
    if(func_sta != func_cb.sta) {
        printf("====>func%d: %d->%d\n", 1, func_sta, func_cb.sta);
        func_sta = func_cb.sta;
    }

    if (sys_cb.cm_vol_change) {
        sys_cb.cm_vol_change = 0;
        param_sys_vol_write();
    }
    param_sync();
    gui_box_clear();
    reset_sleep_delay();
    reset_pwroff_delay();
    func_cb.set_vol_callback = NULL;
    bsp_clr_mute_sta();
    AMPLIFIER_SEL_D();

    if(func_cb.sta < FUNC_MAX) {
        key_set_msg_tbl(func_info[func_cb.sta].msg_tbl);
    } else {
        key_set_msg_tbl(NULL);
    }

    func_cb.curr = func_cb.sta;     //即将进入的模式
}

AT(.text.func)
void func_exit(void)
{
    u8 chk_dev;
    u8 func_num, func_sta = func_cb.last;
    u8 funcs_total = get_funcs_total();

    for (func_num = 0; func_num != funcs_total; func_num++) {
        if (func_cb.last == func_sort_table[func_num]) {
            break;
        }
    }

    for(uint i=0; i<funcs_total; i++) {
        func_num++;                                     //切换到下一个任务
        if (func_num >= funcs_total) {
            func_num = 0;
        }
        func_sta = func_sort_table[func_num];           //获取新的任务

        chk_dev = func_info[func_sta].dev_online;       //检查设备是否在线，不在线时跳过
        if(chk_dev == 0 || (chk_dev&dev_get_online()) != 0) {
            break;
        }
    }

    func_cb.sta = func_sta;

}

AT(.text.func)
void func_run(void)
{
    printf("%s\n", __func__);

    if (wireless_role_is_adapter()) {
        func_cb.sta = FUNC_ADAPTER;
    } else {
        func_cb.sta = FUNC_DEVICE;
    }

#if FUNC_LE_DUT_EN
    func_cb.sta = FUNC_LE_DUT;
#endif

    while (1) {
        func_enter();
        switch (func_cb.sta) {
#if FUNC_ADAPTER_EN
        case FUNC_ADAPTER:
            func_adapter();
            break;
#endif

#if FUNC_DEVICE_EN
        case FUNC_DEVICE:
            func_device();
            break;
#endif

#if CHARGE_EN
        case FUNC_CHARGE:
            func_charge();
            break;
#endif
#if FUNC_LE_DUT_EN
        case FUNC_LE_DUT:
            func_le_dut();
            break;
#endif
        case FUNC_PWROFF:
            func_pwroff(sys_cb.pwroff.tone_en);
            break;

        default:
            func_exit();
            break;
        }
    }
}
