////#include "include.h"
////#include "api.h"
////
////
////void bt_new_name_init(void);
////void ring_tws_disconnect_cb(void);
////
/////********************************************************/
////extern void bt_tws_set_operation(uint8_t *cmd);
////void dev_vol_set_cb(uint8_t dev_vol, uint8_t media_index, uint8_t setting_type);
////
//////蓝牙连接过程调整主频，加快连接速度
////AT(.text.bts.clk) WEAK
////void bt_set_sys_clk(uint8_t level)
////{
////    if(level) {
////        sys_clk_req(INDEX_STACK, SYS_120M);
////    } else {
////        sys_clk_free(INDEX_STACK);
////    }
////}
////
////void bsp_bt_init(void)
////{
////    //更新配置工具的设置
////    cfg_bt_rf_def_txpwr = xcfg_cb.bt_rf_pwrdec;
////    cfg_bt_page_txpwr = xcfg_cb.bt_rf_page_pwrdec;
////    cfg_ble_page_txpwr = xcfg_cb.ble_rf_page_pwrdec;
////    cfg_ble_page_rssi_thr = xcfg_cb.ble_page_rssi_thr;
////
////    cfg_bt_support_profile = (PROF_A2DP*BT_A2DP_EN*xcfg_cb.bt_a2dp_en) |
////                             (PROF_HFP*BT_HFP_EN*xcfg_cb.bt_sco_en) |
////                             (PROF_SPP*BT_SPP_EN*(xcfg_cb.bt_spp_en||xcfg_cb.eq_dgb_spp_en)) |
////                             (PROF_HID*BT_HID_EN*xcfg_cb.bt_hid_en) |
////                             (PROF_PBAP*BT_PBAP_EN) |
////                             (PROF_HSP*BT_HSP_EN*xcfg_cb.bt_sco_en) |
////                             (PROF_GATT * BT_ATT_EN) |
////                             (PROF_MAP * BT_MAP_EN);
////#if BT_A2DP_VOL_CTRL_EN
////    if(!xcfg_cb.bt_a2dp_vol_ctrl_en) {
////        cfg_bt_a2dp_feature &= ~A2DP_AVRCP_VOL_CTRL;
////        cfg_bt_a2dp_feature1 &= ~A2DP_AVRCP_RECORD_DEVICE_VOL;
////    } else {
////        cfg_bt_a2dp_feature |= A2DP_AVRCP_VOL_CTRL;
////    }
////#endif
////
////#if BT_A2DP_AVRCP_PLAY_STATUS_EN
////    cfg_bt_a2dp_feature |= A2DP_AVRCP_PLAY_STATUS;
////#endif
////
////    cfg_bt_dual_mode = BT_DUAL_MODE_EN * xcfg_cb.ble_en;
////#if LE_FCC_TEST_EN
////    cfg_bt_dual_mode = 1;
////#endif
////    cfg_bt_max_acl_link = BT_2ACL_EN + 1;
////#if BT_TWS_EN
////    cfg_bt_tws_mode = BT_TWS_EN * xcfg_cb.bt_tws_en;
////    if(xcfg_cb.bt_tws_en == 0) {
////        cfg_bt_tws_feat      = 0;
////        cfg_bt_tws_pair_mode = 0;
////    } else {
////        cfg_bt_tws_pair_mode &= ~TWS_PAIR_OP_MASK;
////        cfg_bt_tws_pair_mode |= xcfg_cb.bt_tws_pair_mode & TWS_PAIR_OP_MASK;
////#if BT_TWS_PAIR_BONDING_EN
////        if(xcfg_cb.bt_tws_pair_bonding_en) {
////            cfg_bt_tws_pair_mode |= TWS_PAIR_MS_BONDING;
////            cfg_bt_tws_feat |= TWS_FEAT_MS_BONDING;
////        } else {
////            cfg_bt_tws_pair_mode &= ~TWS_PAIR_MS_BONDING;
////        }
////#endif
////#if BT_TWS_MS_SWITCH_EN
////        if(xcfg_cb.bt_tswi_en) {
////            cfg_bt_tws_feat |= TWS_FEAT_MS_SWITCH;
////        } else {
////            cfg_bt_tws_feat &= ~TWS_FEAT_MS_SWITCH;
////        }
////#endif
////        if(xcfg_cb.bt_tws_lr_mode > 8) {//开机时PWRKEY可能按住，先不检测
////            tws_lr_xcfg_sel();
////        }
////    }
////#endif // BT_TWS_EN
////
////#if BT_HFP_EN
////    if(!xcfg_cb.bt_hfp_ring_number_en) {
////        cfg_bt_hfp_feature &= ~HFP_RING_NUMBER_EN;
////    }
////#endif
////
////    sys_cb.discon_reason = 0xff;
////
////#if BT_FCC_TEST_EN
////    bt_fcc_init();
////#endif
////
////    memset(&f_bt, 0, sizeof(func_bt_t));
////    f_bt.disp_status = 0xfe;
////    f_bt.need_pairing = 1;  //开机若回连不成功，需要播报pairing
////    f_bt.hid_menu_flag = 1;
////    le_popup_init();
////
////    app_var_init();
////
////    bt_setup();
////}
////
////void bsp_bt_close(void)
////{
////}
////
////#if BT_PWRKEY_5S_DISCOVER_EN
////bool bsp_bt_w4_connect(void)
////{
////    if (xcfg_cb.bt_pwrkey_nsec_discover) {
////        while (sys_cb.pwrkey_5s_check) {            //等待检测结束
////            func_process();
//////            WDT_CLR();
//////            delay_5ms(2);
//////            bt_thread_check_trigger();
//////            bsp_res_process();
////        }
////
////        //已检测到长按5S，需要直接进入配对状态。播放PAIRING提示音。
////        if (sys_cb.pwrkey_5s_flag) {
////            return false;
////        }
////    }
////    return true;
////}
////
////bool bsp_bt_pwrkey5s_check(void)
////{
////    bool res = !bsp_bt_w4_connect();
////    delay_5ms(2);
////    return res;
////}
////
////void bsp_bt_pwrkey5s_clr(void)
////{
////    if (!xcfg_cb.bt_pwrkey_nsec_discover) {
////        return;
////    }
////    sys_cb.pwrkey_5s_flag = 0;
////}
////#endif // BT_PWRKEY_5S_DISCOVER_EN
////
////void bsp_bt_vol_set(uint8_t vol)
////{
////    if (sys_cb.incall_flag) {
////        sys_cb.hfp_vol = vol;
////        bt_ctrl_msg(BT_CTL_VOL_CHANGE);
////    } else {
////        sys_cb.a2dp_vol = vol;      //这里不直接设置DAC音量，通过bt_music_vol_change通知系统调节音量
////        printf("BSP_SET_VOL: %d\n", sys_cb.a2dp_vol);
////        bt_music_vol_change();      //之后通过回调函数a2dp_vol_set_cb设置DAC音量
////    }
////}
////
////uint bsp_bt_get_hfp_vol(uint hfp_vol)
////{
////    uint vol;
////    vol = (hfp_vol + 1) * sys_cb.hfp2sys_mul;
////    if (vol > VOL_MAX) {
////        vol = VOL_MAX;
////    }
////    return vol;
////}
////
////void bsp_bt_call_volume_msg(u16 msg)
////{
////    if ((msg == MSG_VOL_UP) && (sys_cb.hfp_vol < 15)) {
////        bt_ctrl_msg(BT_CTL_VOL_UP);
////    } else if ((msg == MSG_VOL_DOWN) && (sys_cb.hfp_vol > 0)) {
////        bt_ctrl_msg(BT_CTL_VOL_DOWN);
////    } else {
////        return;
////    }
////    //printf("call vol: %d\n", sys_cb.hfp_vol);
////}
////
////void bt_emit_notice(uint evt, void *params)
////{
////    u8 *packet = params;
////    u8 opcode = 0;
////    u8 scan_status = 0x03;
////
////    switch(evt) {
////    case BT_NOTICE_INIT_FINISH:
////#if BT_TWS_EN
////        if(xcfg_cb.bt_tws_pair_mode > 1) {
////            bt_tws_set_scan(0x03);
////        }
////#endif
////
////        if(cfg_bt_work_mode == MODE_BQB_RF_BREDR) {
////            opcode = 1;                     //测试模式，不回连，打开可被发现可被连接
////#if BT_PWRKEY_5S_DISCOVER_EN
////        } else if(!bsp_bt_w4_connect()) {
////            opcode = 1;                     //长按5S开机，不回连，打开可被发现可被连接
////#endif
////        } else {
////            if(bt_nor_get_link_info(NULL)) {
////                scan_status = 0x02;         //有回连信息，不开可被发现
////            }
////        }
////        bt_start_work(opcode, scan_status);
////#if LE_WIN10_POPUP
////        ble_adv0_set_ctrl(1);				//打开LE广播，可被win10发现
////#endif
////        break;
////
////    case BT_NOTICE_DISCONNECT:
////        f_bt.warning_status |= BT_WARN_DISCON;
////#if LE_WIN10_POPUP
////        ble_adv0_set_ctrl(1);				//打开LE广播，可被win10发现
////#endif
////        msg_enqueue(EVT_AUTO_PWFOFF_EN);
////        delay_5ms(5);
////        break;
////    case BT_NOTICE_CONNECTED:
////        f_bt.warning_status |= BT_WARN_CON;
////        bt_reset_redial_number(packet[0] & 0x01);
////#if LE_WIN10_POPUP
////        ble_adv0_set_ctrl(0);				//关闭LE广播
////#endif
////#if BT_PWRKEY_5S_DISCOVER_EN
////        bsp_bt_pwrkey5s_clr();
////#endif // BT_PWRKEY_5S_DISCOVER_EN
////        delay_5ms(5);
////        msg_enqueue(EVT_AUTO_PWFOFF_DIS);
////        break;
//////    case BT_NOTICE_CONNECT_FAIL:
//////        if(bt_is_scan_ctrl()) {
//////            bt_set_scan(0x03);      //回连失败，打开可被发现可被连接
//////        }
//////        break;
//////    case BT_NOTICE_LOSTCONNECT:
//////        break;
//////    case BT_NOTICE_INCOMING:
//////    case BT_NOTICE_RING:
//////    case BT_NOTICE_OUTGOING:
//////    case BT_NOTICE_CALL:
//////        break;
////
////    case BT_NOTICE_SCO_CONNECTED:
////    case BT_NOTICE_SCO_KILL:
////#if WIRELESS_EN
////        wireless_status_change();
////#endif
////        break;
////
////    case BT_NOTICE_SET_SPK_GAIN:
////        dev_vol_set_cb(packet[0], packet[1], 1 | BIT(3));
////        break;
////
////    case BT_NOTICE_MUSIC_PLAY:
////        msg_enqueue(EVT_A2DP_MUSIC_PLAY);
////        break;
////
////    case BT_NOTICE_MUSIC_STOP:
////        if (bt_get_disp_status() > BT_STA_PLAYING) {
////            break;
////        }
////        msg_enqueue(EVT_A2DP_MUSIC_STOP);
////        break;
////
////    case BT_NOTICE_MUSIC_CHANGE_VOL:
////        if(packet[0] == 0) {
////            msg_enqueue(MSG_VOL_DOWN);
////        } else {
////            msg_enqueue(MSG_VOL_UP);
////        }
////        break;
////    case BT_NOTICE_MUSIC_SET_VOL:
////        if((sys_cb.incall_flag & INCALL_FLAG_SCO) == 0) {
////            dev_vol_set_cb(packet[0], packet[1], 1);
////        }
////        break;
////    case BT_NOTICE_MUSIC_CHANGE_DEV:
////        dev_vol_set_cb(packet[0], packet[1], 0);
////        break;
////    case BT_NOTICE_CALL_CHANGE_DEV:
////        dev_vol_set_cb(packet[0], packet[1], 0 | BIT(3));
////        break;
////
////    case BT_NOTICE_HID_CONN_EVT:
////#if BT_HID_MANU_EN
////        if (f_bt.hid_menu_flag == 2) {
////            //按键连接/断开HID Profile完成
////            if (packet[0]) {
////                f_bt.warning_status |= BT_WARN_HID_CON;
////            } else {
////                f_bt.warning_status |= BT_WARN_HID_DISCON;
////            }
////            f_bt.hid_menu_flag = 1;
////        }
////#if BT_HID_DISCON_DEFAULT_EN
////        else if (f_bt.hid_menu_flag == 1) {
////            if (packet[0] != 0) {
////                f_bt.hid_discon_flag = 1;
////            }
////        }
////#endif // BT_HID_DISCON_DEFAULT_EN
////#endif // BT_HID_MANU_EN
////        break;
////
////#if BT_TWS_EN
//////    case BT_NOTICE_TWS_SEARCH_FAIL:
//////        break;
//////    case BT_NOTICE_TWS_CONNECT_START:
//////        break;
////    case BT_NOTICE_TWS_DISCONNECT:
////        bsp_res_set_break(false);
////
////        f_bt.tws_status = 0;
////        f_bt.warning_status |= BT_WARN_TWS_DISCON;      //TWS断线不播报提示音，仅更改声道配置
////        msg_enqueue(EVT_BLE_ADV0_BAT);
////        app_tws_disconnect_callback();
////        ring_tws_disconnect_cb();
////        msg_enqueue(EVT_BT_UPDATE_STA);                 //刷新显示
////		if(!bt_is_connected()){
////        	msg_enqueue(EVT_AUTO_PWFOFF_EN);
////		}
////        break;
////    case BT_NOTICE_TWS_CONNECTED:
////        if(bt_tws_is_slave()){
////            ble_adv_dis();      //副机关闭BLE广播
////            res_play_cleanup(); //播tws提示音前先清一下单耳的提示音。不清会导致rpos慢主耳一步，导致一直w4
////        }
////        f_bt.tws_status = packet[0];
////        if(f_bt.tws_status & FEAT_TWS_MUTE_FLAG) {
////            f_bt.warning_status |= BT_WARN_TWS_CON;     //无连接提示音，仅更改声道配置
////        } else if(f_bt.tws_status & FEAT_TWS_ROLE) {
////            f_bt.warning_status |= BT_WARN_TWS_SCON;    //TWS连接提示音
////        } else {
////            f_bt.warning_status |= BT_WARN_TWS_MCON;    //TWS连接提示音
////        }
////        app_tws_connect_callback();
////        msg_enqueue(EVT_BT_UPDATE_STA);                 //刷新显示
////
////        bsp_res_set_break(true);                        //打断开机提示音
////
////		if(bt_tws_is_slave()){
////            msg_enqueue(EVT_AUTO_PWFOFF_DIS);
////        }else{
////            if(!bt_is_connected()){
////                msg_enqueue(EVT_AUTO_PWFOFF_EN);
////            }
////        }
////        break;
//////    case BT_NOTICE_TWS_CONNECT_FAIL:
//////        break;
////    case BT_NOTICE_TWS_LOSTCONNECT:
////        break;
////
////    case BT_NOTICE_TWS_INIT_VOL:
////        sys_cb.a2dp_vol = a2dp_vol_conver(packet[0]);
////        printf("TWS_INIT_VOL: %d\n", sys_cb.a2dp_vol);
////        msg_enqueue(EVT_TWS_INIT_VOL);
////        break;
////
////	case BT_NOTICE_TWS_USER_KEY:
////		tws_user_key_process(params);
////		break;
////
////    case BT_NOTICE_TWS_STATUS_CHANGE:
////        msg_enqueue(EVT_BT_UPDATE_STA);                 //刷新显示
////        break;
////
////    case BT_NOTICE_TWS_ROLE_CHANGE:
////        if(packet[0] == 0) {
////            bsp_res_set_break(false);
////        }
////        break;
////
////#endif
////
//////    case BT_NOTICE_RECON_FINISH:
//////        printf("RECON_FAIL, reason=%d\n", packet[1]);
//////        break;
//////    case BT_NOTICE_ABORT_STATUS:
//////        if(packet[0] != 0) {
//////            printf("ABORT_START\n");
//////        } else {
//////            if(packet[1] == 0 || packet[1] == 0x13) {
//////                printf("ABORT_OK, reason=%d, %d\n", packet[1], bt_nor_is_connected());
//////            } else {
//////                printf("ABORT_FAIL, reason=%d, %d\n", packet[1], bt_nor_is_connected());
//////            }
//////        }
//////        break;
////
////    case BT_NOTICE_LOW_LATENCY_STA:
////        if (packet[0] == 1) {
////            msg_enqueue(EVT_A2DP_LLTY_EN);
////        } else {
////            msg_enqueue(EVT_A2DP_LLTY_DIS);
////        }
////        break;
////
////#if BT_PBAP_EN
////    case BT_NOTICE_PBAP_CONNECTED:
////        printf("===>>> PBAP: Connected\n");
////        break;
////
////    case BT_NOTICE_PBAP_GET_PHONEBOOK_SIZE_COMPLETE:
////        printf("===>>> PBAP: Phonebook size: %u\n", *(u32 *)params);
////        break;
////
////    case BT_NOTICE_PBAP_PULL_PHONEBOOK_COMPLETE:
////        printf("===>>> PBAP: Pull phonebook complete\n");
////        break;
////#endif
////
////    default:
////        break;
////    }
////
////    le_popup_evt_notice(evt, params);
////    app_bt_evt_notice(evt, params);
////}
////
////
