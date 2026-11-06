#include "include.h"
#include "wireless.h"

#if WIRELESS_EN

struct wireless_cb_tag wireless_cb;

#if WIRELESS_CON_PAIR_MODE
AT(.text.bsp.wireless_cb)
void role_defind(u8 index)
{
    if(wireless_cb.connected_sta == 3) {
        sys_cb.con_role_data[0] = 1;               //主副麦区分完毕标志位
    } else {
        sys_cb.con_role_data[0] = 0;               //主副麦区分完毕标志位
    }
}

AT(.text.bsp.wireless_cb)
u8 role_get(u8 index)
{
    if(sys_cb.con_role_data[0]) {
        if(index) {
            if(sys_cb.con_role_data[1]) {
                return 1;                    //主麦
            } else {
                return 2;                    //副麦
            }
        } else {
            if(!sys_cb.con_role_data[1]) {
                return 1;                    //主麦
            } else {
                return 2;                    //副麦
            }
        }
    }
    return 0;
}
#endif

AT(.com_text.wireless)
u8 wireless_get_status(void)
{
#if WIRELESS_MIC_BROADCAST_EN
    return sys_cb.bt_is_inited;
#else
    return (wireless_cb.connected_sta);
#endif // WIRELESS_MIC_BROADCAST_EN
}

#define wireless_set_alg_en(en)     wireless_cb.alg_en = en
#define wireless_clr_alg_en()       wireless_cb.alg_en = 0

AT(.text.wireless.cb)
void wireless_emit_notice(uint evt, void *params)
{
//    u32 tmp;
    u8 *packet = params;
    u8 mic_num;
    u8 max_mic_nb = WIRELESS_CON_LINK_NB;
#if WIRELESS_MIC_2TNR_EN
    if (!wireless_role_is_adapter()) {
        max_mic_nb = WIRELESS_CON_2TNR_NB;
    }
#endif

    switch(evt) {
    case BT_NOTICE_WIRELESS_CONNECTED:
        mic_num = packet[0];
#if WIRELESS_CON_BONDING_EN
        memcpy(sys_cb.wl_bd_param, packet, 8);
#endif
        if(mic_num < max_mic_nb) {
            printf("WL_CON, %d\n", mic_num);
#if DEVICE_INTERPHONE_EN
            bool alg_en = (bool)(wireless_cb.bypass_cnt == 0);

            wireless_cb.connected_sta |= BIT(mic_num);
            wireless_cb.change_sta = 0;
            wireless_cb.change_flag = 1;
            func_device_interphone_set_con_cmp();
            sys_clk_req(INDEX_WL_CON, DEVICE_CON_CLK);    //先抬高主频
            wireless_full_duplex_init();
            dis_auto_pwroff();
            led_idle();
            wireless_set_alg_en(alg_en);                //再使能算法
#else
#if WIRELESS_CON_PAIR_MODE
            printf("WIRELESS_CON_ROLE, %d\n", sys_cb.con_role);
            if(sys_cb.con_role) {
                sys_cb.con_role_data[1] = mic_num;       //主麦
            } else {
                sys_cb.con_role_data[2] = mic_num;       //副麦
            }
#endif

            if(wireless_cb.connected_sta == 0) {
                bool alg_en = (bool)(wireless_cb.bypass_cnt == 0);

                if(wireless_role_is_adapter()) {
                    sys_clk_req(INDEX_WL_CON, ADAPTER_CON_CLK);    //先抬高主频
#if WIRELESS_FULL_DUPLEX
                    wireless_full_duplex_init();
#else
                    wireless_adapter_init();
#endif
                } else {
                    sys_clk_req(INDEX_WL_CON, DEVICE_CON_CLK);    //先抬高主频
#if WIRELESS_FULL_DUPLEX
                    wireless_full_duplex_init();
#else
                    wireless_device_init();
#endif

                }
                dis_auto_pwroff();
                led_idle();
                wireless_set_alg_en(alg_en);                //再使能算法
            }
            wireless_cb.connected_sta |= BIT(mic_num);
            wireless_cb.change_sta = 0;
            wireless_cb.change_flag = 1;
#if WIRELESS_CON_PAIR_MODE
            role_defind(mic_num);
#endif
#endif
#if WIRELESS_MIC_2TNR_EN
        } else if (mic_num == max_mic_nb) {
            ///跑到这里说明时d2d链路，或者侦听发射链路，做特殊处理
            wireless_cb.device_con_sta = 1;
            wireless_cb.device_con_role = packet[8];
            wireless_cb.change_flag = 1;
#endif
        }
        break;

    case BT_NOTICE_WIRELESS_CONNECT_FAIL:
        mic_num = packet[0];
        if(mic_num < max_mic_nb) {
            printf("WL_CON_FAIL, %d\n", mic_num);
            wireless_cb.change_sta = 1;
            wireless_cb.change_flag = 1;
#if WIRELESS_MIC_2TNR_EN
        } else if (mic_num == max_mic_nb) {
            wireless_cb.device_con_sta = 0;
            wireless_cb.device_con_role = 0;
            wireless_cb.change_flag = 1;
#endif
        }
        break;

    case BT_NOTICE_WIRELESS_DISCONNECT:
        mic_num = packet[0];
        if(mic_num < max_mic_nb) {
            printf("WL_DISCON, %d\n", mic_num);
            wireless_cb.connected_sta &= ~BIT(mic_num);
            wireless_cb.change_sta = 2;
            wireless_cb.change_flag = 1;
            led_bt_idle();

            wireless_dump_reset(mic_num);
#if WIRELESS_CON_PAIR_MODE
            wireless_cb.con_temp = 0;
            if(role_get(mic_num) == 2) {                        //副麦断开
                sys_cb.con_role = 1;
            } else if(role_get(mic_num) == 1) {                 //主麦断开
                sys_cb.con_role = 0;
            }
#endif
            if(wireless_cb.connected_sta == 0) {
                wireless_clr_alg_en();                  //先关闭算法
#if DEVICE_INTERPHONE_EN
                wireless_full_duplex_exit(mic_num, 0);
                en_auto_pwroff();
#else
                if(wireless_role_is_adapter()) {
                    if (!bt_is_connected()) {
                        en_auto_pwroff();
                    }
#if WIRELESS_FULL_DUPLEX
                    wireless_full_duplex_exit(mic_num, 0);
#else
                    wireless_adapter_exit(mic_num, 0);
#endif
                } else {
#if WIRELESS_FULL_DUPLEX
                    wireless_full_duplex_exit(mic_num, 0);
#else
                    wireless_device_exit();
#endif
					en_auto_pwroff();
                }
#endif
                sys_clk_free(INDEX_WL_CON);             //再还原主频
                sys_clk_free(INDEX_WL_ALG_EN);

#if TBOX_TEST_EN
                if (product_test_is_sucess() && cfg_discon_auto_pwroff) {
                    func_cb.sta = FUNC_PWROFF;
                }
#endif
            } else {
                if(wireless_role_is_adapter()) {
#if WIRELESS_FULL_DUPLEX
                    wireless_full_duplex_exit(mic_num, wireless_cb.connected_sta);
#else
                    wireless_adapter_exit(mic_num, wireless_cb.connected_sta);
#endif
                }
            }
#if WIRELESS_MIC_2TNR_EN
        } else if (mic_num == max_mic_nb) {
            wireless_cb.device_con_sta = 0;
            wireless_cb.device_con_role = 0;
            wireless_cb.change_flag = 1;
#endif
        }
        break;

#if WIRELESS_MIC_BROADCAST_EN
    case BT_NOTICE_BROADCAST_SOURCE_TX_EN:
        printf("BT_NOTICE_BROADCAST_SOURCE_TX_EN\n");
        sys_clk_req(INDEX_WL_CON, DEVICE_CON_CLK);
        wireless_device_init();
        wireless_cb.alg_en = 1;
        break;

    case BT_NOTICE_BROADCAST_SOURCE_TX_DIS:
        printf("BT_NOTICE_BROADCAST_SOURCE_TX_DIS\n");
        wireless_cb.change_flag = 1;
        wireless_cb.alg_en = 0;
        wireless_device_exit();
        sys_clk_free(INDEX_WL_CON);
        sys_clk_free(INDEX_WL_ALG_EN);
        break;

    case BT_NOTICE_BROADCAST_SINK_SYNC_SETUP:
        printf("BT_NOTICE_BROADCAST_SINK_SYNC_SETUP\n");
        sys_clk_req(INDEX_WL_CON, ADAPTER_CON_CLK);
        wireless_adapter_init();
        wireless_cb.alg_en = 1;
        break;

    case BT_NOTICE_BROADCAST_SINK_SYNC_LOST:
        printf("BT_NOTICE_BROADCAST_SINK_SYNC_LOST reason : %d\n", packet[1]);
        {
            u8 reason = packet[1];
            if (reason == 0x08) {
                wireless_cb.change_flag = 1;
            }
            wireless_cb.alg_en = 0;
            wireless_adapter_exit(0, 0);
            sys_clk_free(INDEX_WL_CON);
            sys_clk_free(INDEX_WL_ALG_EN);
        }
        break;
#endif

    default:
        break;
    }
}

AT(.text.wireless.cb)
void wireless_device_fast_exit(void)
{
    wireless_cb.connected_sta = 0;
    wireless_cb.change_sta = 2;
    wireless_cb.change_flag = 1;

    wireless_clr_alg_en();                  //先关闭算法
    wireless_device_exit();

    sys_clk_free(INDEX_KARAOK);             //再还原主频
    led_bt_idle();
}

AT(.text.func.process.wireless)
void wireless_sta_proc(void)
{
//    u32 conn_flag = 0;
//    if(wireless_cb.connected_sta != sys_cb.disp_sta) {
//        if (xcfg_cb.wireless_adapter_en) {
//            if (wireless_cb.connected_sta > 2) {
//                conn_flag = 3;
//            } else if (wireless_cb.connected_sta < 3 && wireless_cb.connected_sta > 0) {
//                conn_flag = 1;
//            } else {
//                conn_flag = 0;
//            }
//        } else if (xcfg_cb.wireless_device_en) {
//            if (wireless_cb.connected_sta) {
//                conn_flag = 1;
//            } else {
//                conn_flag = 0;
//            }
//        }
//        sys_cb.disp_sta = wireless_cb.connected_sta;
//        switch(conn_flag) {
//        case 0:
//            led_bt_idle();
//            break;
//        case 1:
//            led_bt_connected();
//            break;
//        case 3:
//            led_bt_connected_2mics();
//            break;
//        }
//    }

    wireless_dump_proc();
}

AT(.text.wireless_cmd)
void wireless_rx_user_cmd(u8 index, u8 *ptr, u8 len)
{
//    printf("user_cmd%d: \n", index);
//    print_r(ptr, len);
}

AT(.text.wireless.init)
void wireless_mic_role_init(void)
{
#if WIRELESS_MIC_ROLE == 0
    cfg_wireless_role = false;
#elif WIRELESS_MIC_ROLE == 1
    cfg_wireless_role = true;
#elif WIRELESS_MIC_ROLE == 2
    if (xcfg_cb.wireless_adapter_en) {
        cfg_wireless_role = true;
    } else if (xcfg_cb.wireless_device_en) {
        cfg_wireless_role = false;
    } else {
        ///都被去掉默认发射端
        cfg_wireless_role = false;
    }
#endif
}

AT(.text.wireless.init)
void wireless_var_init(void)
{
    wireless_cb.alg_en = 0;
    wireless_mic_role_init();
    wireless_cmd_init();
}
#endif
