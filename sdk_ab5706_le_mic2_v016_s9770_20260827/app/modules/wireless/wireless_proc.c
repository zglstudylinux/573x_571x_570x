#include "include.h"
#include "wireless.h"

#if WIRELESS_EN

struct wireless_cb_tag wireless_cb;

AT(.com_text.wireless)
u8 wireless_get_status(void)
{
    return (wireless_cb.connected_sta);
}

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

///需要打印err的时候把注释的打开，把aligned改大128
GLOBAL_ALIGNED(64) NO_INLINE
void wireless_audio_bypass(void)
{
    GLOBAL_INT_DISABLE();
#if WARNING_MP3_RES_EN && ADAPTER_USB_MIC_RX_EN
    PICENCLR = BIT(7);                                      //关闭USB中断， USB中断向量号：IRQ_USB_VECTOR = 7
#endif // WARNING_MP3_RES_EN
    if(wireless_cb.bypass_cnt < 255) {
        wireless_cb.bypass_cnt++;
        if(wireless_cb.bypass_cnt == 1) {
            wireless_cb.alg_en = 0;                         //关闭算法
        }
//        GLOBAL_INT_RESTORE();
    }
//    else {
        GLOBAL_INT_RESTORE();
//        printf("wl_err: audio bypass\n");
//    }
}

GLOBAL_ALIGNED(64) NO_INLINE
void wireless_audio_enable(void)
{
    GLOBAL_INT_DISABLE();
    if(wireless_cb.bypass_cnt > 0) {
        wireless_cb.bypass_cnt--;
        if(wireless_cb.bypass_cnt == 0) {
            wireless_cb.alg_en = (bool)wireless_cb.connected_sta;
        }
//        GLOBAL_INT_RESTORE();
    }
//    else {
        GLOBAL_INT_RESTORE();
//        printf("wl_err: audio en\n");
//    }

#if WARNING_MP3_RES_EN && ADAPTER_USB_MIC_RX_EN
    PICENSET = BIT(7);                       //恢复USB中断， USB中断向量号：IRQ_USB_VECTOR = 7
#endif // WARNING_MP3_RES_EN
}

GLOBAL_ALIGNED(64) NO_INLINE
static void wireless_set_alg_en(void)
{
    GLOBAL_INT_DISABLE();
    if(wireless_cb.bypass_cnt == 0) {
        wireless_cb.alg_en = 1;                 //再使能算法
    }
    GLOBAL_INT_RESTORE();
}
#define wireless_clr_alg_en()       wireless_cb.alg_en = 0

AT(.text.wireless.cb)
void wireless_emit_notice(uint evt, void *params)
{
//    u32 tmp;
    u8 *packet = params;
    u8 mic_num;

    switch(evt) {
    case BT_NOTICE_WIRELESS_CONNECTED:
        mic_num = packet[0];
#if WIRELESS_CON_BONDING_EN
        memcpy(sys_cb.wl_bd_param, packet, 8);
#endif
        if(mic_num < WIRELESS_CON_LINK_NB) {
            printf("WL_CON, %d\n", mic_num);
#if DEVICE_INTERPHONE_EN
			wireless_cb.connected_sta |= BIT(mic_num);
			wireless_cb.change_sta[mic_num] = 0;
			wireless_cb.change_flag = 1;
			func_device_interphone_set_con_cmp();
			sys_clk_req(INDEX_KARAOK, SYS_240M);	 //先抬高主频
			wireless_interphone_init();
			wireless_cb.alg_en = 1; 					 //再使能算法
#else
            if(wireless_cb.connected_sta == 0) {
                sys_clk_req(INDEX_KARAOK, SYS_240M);        //先抬高主频
                if(wireless_role_is_adapter()) {
                    wireless_adapter_init();
                    dis_auto_pwroff();
                } else {
                    wireless_device_init();
                }

                wireless_set_alg_en();                      //再使能算法
            }
            wireless_cb.connected_sta |= BIT(mic_num);
            wireless_cb.change_sta[mic_num] = 0;
            wireless_cb.change_flag = 1;
#if WIRELESS_CON_PAIR_MODE
            printf("WIRELESS_CON_ROLE, %d\n", sys_cb.con_role);
            if(sys_cb.con_role) {
                sys_cb.con_role_data[1] = mic_num;       //主麦
            } else {
                sys_cb.con_role_data[2] = mic_num;       //副麦
            }
#endif

#if ADAPTER_EN && ADAPTER_SAVE_PARAM_EN
            if(wireless_role_is_adapter()) {
                if (!wl_device_db_is_exist(&packet[2])) {
                    ///不存在，则增加
                    wl_device_db_add(&packet[2]);
                }

                wireless_con_interact_kick(mic_num);
            }
#endif
#endif // DEVICE_INTERPHONE_EN
        }
#if WIRELESS_CON_BONDING_EN
//        my_printf("wireless_get_status() %d \n",wireless_get_status());
//        my_print_r(bongding_addr_get(),6);
        if ((wireless_get_status() == (BIT(0)|WIRELESS_CON_LINK_NB)) && !(memcmp(bongding_addr_get(),0x00,6))) {  //若连接设备已满且绑定地址为空，则设置一次 ble_reset_con()
            wireless_get_local_bd_addr(bongding_addr_get());                     //取RX本地地址作共同绑定的地址
            wireless_tx_bonding_sync(0,BONDING_SYNC,bongding_addr_get(),7);      //同步需要共同绑定的地址
            if (WIRELESS_CON_LINK_NB == 2){
                wireless_tx_bonding_sync(1,BONDING_SYNC,bongding_addr_get(),7);
            }
            param_write_bonding_addr(bongding_addr_get());                       //共同绑定的地址写进flash内
            ble_reset_con(2);
        }
#endif // WIRELESS_CON_BONDING_EN
#if WIRELESS_CON_PAIR_MODE
            role_defind(mic_num);
#endif
        break;

    case BT_NOTICE_WIRELESS_CONNECT_FAIL:
        mic_num = packet[0];
        if(mic_num < WIRELESS_CON_LINK_NB) {
            printf("WL_CON_FAIL, %d\n", mic_num);
            wireless_cb.change_sta[mic_num] = 1;
            wireless_cb.change_flag = 1;
        }
        break;

    case BT_NOTICE_WIRELESS_DISCONNECT:
        mic_num = packet[0];
        if(mic_num < WIRELESS_CON_LINK_NB) {
            printf("WL_DISCON, %d\n", mic_num);
            wireless_cb.connected_sta &= ~BIT(mic_num);
            wireless_cb.change_sta[mic_num] = 2;
            wireless_cb.change_flag = 1;
#if WIRELESS_CON_PAIR_MODE
            wireless_cb.con_temp = 0;
            if(role_get(mic_num) == 2) {                        //副麦断开
                sys_cb.con_role = 1;
            } else if(role_get(mic_num) == 1) {                 //主麦断开
                sys_cb.con_role = 0;
            }
#endif
            wireless_dump_reset(mic_num);
            if(wireless_cb.connected_sta == 0) {
                wireless_clr_alg_en();                  //先关闭算法
#if DEVICE_INTERPHONE_EN

                wireless_interphone_exit(mic_num);
#else
                if(wireless_role_is_adapter()) {
                    en_auto_pwroff();
                    wireless_adapter_exit(mic_num, 0);
                } else {
                    wireless_device_exit();
                }
#endif // DEVICE_INTERPHONE_EN
#if !ADAPTER_LOCAL_MIC_EQ_DRC_EN
                sys_clk_free(INDEX_KARAOK);             //再还原主频
#endif // ADAPTER_LOCAL_MIC_EQ_DRC_EN
            } else {
                wireless_adapter_exit(mic_num, wireless_cb.connected_sta);
            }
        }
        break;
#if WIRELESS_MIC_BROADCAST_EN
    case BT_NOTICE_BROADCAST_SOURCE_TX_EN:
        printf("BT_NOTICE_BROADCAST_SOURCE_TX_EN\n");
        sys_clk_req(INDEX_KARAOK, SYS_160M);
        sys_clk_req(INDEX_KARAOK, SYS_240M);
        wireless_device_init();
        wireless_cb.alg_en = 1;
        break;

    case BT_NOTICE_BROADCAST_SOURCE_TX_DIS:
        printf("BT_NOTICE_BROADCAST_SOURCE_TX_DIS\n");
        wireless_cb.change_flag = 1;
        wireless_cb.alg_en = 0;
        wireless_device_exit();
        sys_clk_free(INDEX_KARAOK);
        break;

    case BT_NOTICE_BROADCAST_SINK_SYNC_SETUP:
        printf("BT_NOTICE_BROADCAST_SINK_SYNC_SETUP\n");
        sys_clk_req(INDEX_KARAOK, SYS_160M);
        sys_clk_req(INDEX_KARAOK, SYS_240M);
        wireless_adapter_init();
        wireless_cb.alg_en = 1;
        wireless_cb.connected_sta = 1;
        break;

    case BT_NOTICE_BROADCAST_SINK_SYNC_LOST:
        printf("BT_NOTICE_BROADCAST_SINK_SYNC_LOST reason : %d\n", packet[1]);
        {
            u8 reason = packet[1];
            if (reason == 0x08) {
                wireless_cb.change_flag = 1;
            }
            wireless_cb.connected_sta = 0;
            wireless_cb.alg_en = 0;
            wireless_adapter_exit(0, 0);
            sys_clk_free(INDEX_KARAOK);
        }
        break;
#endif
    default:
        break;
    }
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

    if (wireless_role_is_adapter()){                          //适配器

    } else {                                                  //设备端
#if WIRELESS_CON_BONDING_EN
        if(ptr[0] == BONDING_SYNC){
            memcpy(bongding_addr_get(),ptr + 1,6);            //TX端同步RX端获取到的地址
            param_write_bonding_addr(bongding_addr_get());    //保存绑定的地址
            ble_reset_con(1);                                 //设置hash值
        }
#endif // WIRELESS_CON_BONDING_EN
    }

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

extern u32 __code_usb_vma, __code_usb_lma, __code_usb_size;
extern u32 __code_adapter_vma, __code_adapter_lma, __code_adapter_size;
extern u32 __code_device_vma, __code_device_lma, __code_device_size;
extern u32 __buf_start_usbdev, __usb_buf_size;

AT(.text.wireless.init)
void wireless_mic_load_code(void)
{
    if(xcfg_cb.wireless_adapter_en) {
        memcpy(&__code_adapter_vma, &__code_adapter_lma, (u32)&__code_adapter_size);
        printf("load_code(1): %x, %x, %x\n", &__code_adapter_vma, &__code_adapter_lma, (u32)&__code_adapter_size);
        #if (ADAPTER_USB_SPK_EN || ADAPTER_USB_MIC_RX_EN) && (!(WARNING_MP3_RES_EN || WARNING_WSBC_RES_EN))
        memset(&__buf_start_usbdev, 0, (u32)&__usb_buf_size);  //CLR USB BUF
        memcpy(&__code_usb_vma, &__code_usb_lma, (u32)&__code_usb_size);
        printf("load_code(2): %x, %x, %x\n", &__code_usb_vma, &__code_usb_lma, (u32)&__code_usb_size);
        #endif
    } else {
        memcpy(&__code_device_vma, &__code_device_lma, (u32)&__code_device_size);
        printf("load_code(0): %x, %x, %x\n", &__code_device_vma, &__code_device_lma, (u32)&__code_device_size);
    }
}

AT(.text.wireless.init)
void wireless_var_init(void)
{
    wireless_cb.alg_en = 0;
    wireless_mic_role_init();
    wireless_mic_load_code();
    wireless_cmd_init();

    wireless_dump_init();
    wireless_con_adapter_init();
    wireless_con_device_init();
}
#endif
