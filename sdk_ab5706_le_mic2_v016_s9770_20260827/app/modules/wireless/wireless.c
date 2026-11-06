#include "include.h"
#include "api.h"

#define WORK_MODE               MODE_NORMAL

#define D2A_CODEC               (WIRELESS_CON_CODEC_SEL | WS_CODEC_MONO)
#define A2D_CODEC               0

#if WIRELESS_EN
//link
uint8_t cfg_wireless_cntl_bit       = (WIRELESS_CON_CRC_EN*0x1);
uint8_t cfg_wireless_full_duplex    = WIRELESS_CON_FULL_DUPLEX_EN;
uint8_t cfg_le_rssi_thr             = WIRELESS_CON_RSSI_THR;
uint8_t cfg_wireless_link_nb        = WIRELESS_CON_LINK_NB;
uint8_t cfg_wireless_d2a_tx_size    = WIRELESS_MIC_TX_BUFFER_SIZE;                      //传输封包大小
uint8_t cfg_wireless_a2d_tx_size    = WIRELESS_MIC_TX_BUFFER_SIZE;
uint8_t cfg_wireless_con_interval   = WIRELESS_CON_INTERVAL*WIRELESS_CON_LINK_NB;       //连接interval（V1的传输interval），单位1.25ms
uint8_t cfg_wireless_tx_interval    = WIRELESS_MIC_TX_INTERVAL;                         //V2、V3的传输interval，单位1.25ms
uint8_t cfg_wireless_tx_retry       = WIRELESS_MIC_RETRY_NB;                            //V2、V3的传输次数
uint8_t cfg_wireless_tx_comb_nb     = WIRELESS_MIC_COMB_NB;                             //传输封包组合数量
uint8_t cfg_wireless_a2d_tx_retry   = WIRELESS_MIC_TX_INTERVAL;                            //V2、V3、V4的SPK的传输次数
uint8_t cfg_wireless_a2d_tx_comb_nb = 1;
uint16_t cfg_wireless_feat          = ((WIRELESS_CON_VERS<<8)&FEAT_VERS) | (FEAT_D2A*WIRELESS_MIC_EN) | (FEAT_ADAPTER_SAVE * ADAPTER_SAVE_PARAM_EN) | (FEAT_HOP_V1) | (FEAT_A2D*WIRELESS_SPK_EN) | (FEAT_BONDING*WIRELESS_CON_BONDING_EN) | (WIRELESS_CON_LINK_NB&0xf);
uint16_t cfg_wireless_ext_feat      = ((EXT_FEAT_ADV_EXT*0) | (((WIRELESS_CON_VERS>>3)<<1)&EXT_FEAT_VERS));
uint16_t cfg_wireless_d2a_enc_us    = (WIRELESS_MIC_ENC_MAX_US + WIRELESS_MIC_ECHO_DELAY + WIRELESS_MIC_MAGIC_DELAY + WIRELESS_MIC_SRC_DELAY + \
                                       WIRELESS_MIC_AGC_DELAY + WIRELESS_MIC_EQ_DRC_DELAY + WIRELESS_MIC_SOFT_VOL_DELAY + WIRELESS_MIC_DNR_FRE_DELAY);
uint16_t cfg_wireless_d2a_dec_us    = WIRELESS_MIC_DEC_MAX_US;
#if WIRELESS_SPK_EN
uint16_t cfg_wireless_a2d_enc_us    = (WIRELESS_MIC_ENC_MAX_US + WIRELESS_MIC_EQ_DRC_DELAY + WIRELESS_MIC_ECHO_DELAY + WIRELESS_MIC_MAGIC_DELAY + WIRELESS_MIC_SRC_DELAY);
uint16_t cfg_wireless_a2d_dec_us    =  WIRELESS_MIC_DEC_MAX_US ;
#endif

uint8_t cfg_wireless_reconnect_strong  = 2;
uint8_t cfg_wireless_old_bonding_en = 0;
uint8_t cfg_wireless_con_id         = WIRELESS_CON_CON_ID_SEL;   //配对码id值，接收端通过动态切换广播id值，达到连接不同的id值设备
uint8_t cfg_wireless_codec[2]       = {
    D2A_CODEC,      //D2A Codec
    A2D_CODEC,      //A2D Codec
};

//rf
uint8_t cfg_bb_rf_freq_bands        = WIRELESS_CON_FREQ_BAND; // 0:2402M~2480M  1:2200M~2278M  2:2320M~2398M  3:2500M~2578M  4: 2482M~2560M
uint8_t cfg_bb_rf_def_txpwr         = 0;        //降低预置参数RF发射功率，单位0.5dbm
uint8_t cfg_bb_rf_gfsk_pwr          = 127;      //调节GFSK TXPWR，范围：90~127
uint8_t cfg_ble_page_txpwr          = 0;        //降低组队RF发射功率，单位3dbm
uint8_t cfg_ble_page_rssi_thr       = 0;        //设置组队范围rssi

//stack
uint8_t cfg_bt_work_mode            = WORK_MODE;
uint8_t cfg_bt_rf_def_txpwr         = 0;

uint8_t  cfg_bt_tws_tick_freq       = 1;

bool cfg_wireless_broadcast_ch_scan_en = 1;

//设备组队时，首先用设备名称le_name匹配，在setting中可以设置
//同时也检查设备的单/双工、编解码特性，特性不同不允许组队，参见cfg_wireless_feat和cfg_wireless_codec


AT(.com_text.mic_dec)
void wireless_channel_status(u8 idx, u8 chidx, s8 rssi, u8 bfi, u8 chstatus)
{
    if(bfi || chstatus != 0) {
        if(rssi > -80 || (rssi > -85 && chstatus == 250)) {
            ble_con_channel_assess(chidx, false);
        }
    } else {
        ble_con_channel_assess(chidx, true);
    }

    wireless_dump_set_rx_status(idx, chidx, rssi, (bfi || chstatus != 0));
}

const struct le_chnl_map host_ch_class = {
    .map = {0xff, 0xfb, 0xdf, 0xff, 0x1d},
};
///无线麦频点映射处理，可以去掉部分频点
void wireless_host_ch_class_set(void)
{
    mgr_host_ch_class_set(&host_ch_class);
}

#if WIRELESS_CON_BONDING_EN
u8 bt_bongding_addr[6];
u8 *bongding_addr_get(void)
{
    return bt_bongding_addr;
}
#endif //WIRELESS_CON_BONDING_EN
//------------------------------------------------------------------------------------------

bool wireless_discon_all(void)
{
    uint8_t index;
    uint8_t con_sta = wireless_cb.connected_sta;

    for (index = 0; index < WIRELESS_CON_LINK_NB; index++) {
        if (con_sta & BIT(index)) {
            wireless_disconnect_req(index);
        }
    }

    return (bool)(con_sta);
}

//------------------------------------------------------------------------------------------
//设备端判断是否有绑定，如果有就根据绑定的rx地址发起回连
AT(.text.wireless.cb)
bool wireless_get_link_info_addr(uint8_t *wl_addr)
{
#if WIRELESS_CON_BONDING_EN
//    my_printf("%s %d %d\n",__func__,__LINE__,sys_cb.wl_bond_nb);
    if(sys_cb.wl_bond_nb == BONDING_NUM_2) {                  //tx记忆两个rx地址模块
        if(sys_cb.wl_scan_flag) {
            param_bond_addr_read(0);
            memcpy(wl_addr,sys_cb.wl_bd_addr0,6);
            sys_cb.wl_scan_flag = 0;
        } else {
            param_bond_addr_read(1);
            memcpy(wl_addr,sys_cb.wl_bd_addr1,6);
             sys_cb.wl_scan_flag = 1;
        }
        return true;
    } else if (sys_cb.wl_bond_nb == BONDING_NUM_1){           //tx记忆一个rx地址模块
#if WIRELESS_CON_TX_BONDING_NUM == 0
        param_bond_addr_read(0);
        memcpy(wl_addr,sys_cb.wl_bd_addr0,6);
        return true;
#elif WIRELESS_CON_TX_BONDING_NUM == 1
//        my_printf("%s %d %d\n",__func__,__LINE__,sys_cb.wl_scan_flag);
        if(sys_cb.wl_scan_flag) {                        //组队绑定
            param_bond_addr_read(0);
            memcpy(wl_addr,sys_cb.wl_bd_addr0,6);
            sys_cb.wl_scan_flag = 0;
            return true;
        } else {                                         //自由组队
            sys_cb.wl_scan_flag = 1;
            return false;
        }

#endif // WIRELESS_CON_TX_BONDING_NUM
    }

#endif
    return false;
}

//适配端判断是否有绑定信息，如果有就关闭可被发现，只开启可被回连
AT(.text.wireless.cb)
u8 wireless_get_link_info_state(void)
{
#if WIRELESS_CON_BONDING_EN
    return sys_cb.wl_bond_nb;
#else
	return 0;
#endif
}

#if WIRELESS_CON_BONDING_EN
//无线麦组队绑定连接参数更新
AT(.text.wireless.cb)
void wireless_bond_nb_updatae(void)
{
    u8 bond_nb_state = sys_cb.wl_bond_nb;


    if(wireless_role_is_adapter()) {
        if(sys_cb.wl_bond_nb != BONDING_NUM_RX_3 && wireless_get_status()){
            if(sys_cb.wl_bond_nb == BONDING_NUM_0) {
                memcpy(sys_cb.wl_bd_addr0,&sys_cb.wl_bd_param[2],6);
                sys_cb.wl_bond_nb |= BIT(sys_cb.wl_bd_param[0]);
            }

            if(memcmp(sys_cb.wl_bd_addr0,&sys_cb.wl_bd_param[2],6) != 0) {
                memcpy(sys_cb.wl_bd_addr1,&sys_cb.wl_bd_param[2],6);
                sys_cb.wl_bond_nb |= BIT(sys_cb.wl_bd_param[0]);
            }
        }

    } else {
        if(wireless_get_status()){
            if(sys_cb.wl_bond_nb == BONDING_NUM_0){
                memcpy(sys_cb.wl_bd_addr0,&sys_cb.wl_bd_param[2],6);
                sys_cb.wl_bond_nb = BONDING_NUM_1;
            }
//            my_printf("%s %d %d\n",__func__,__LINE__,sys_cb.wl_bond_nb);
#if WIRELESS_CON_TX_BONDING_NUM == 1
            if((memcmp(sys_cb.wl_bd_addr0,&sys_cb.wl_bd_param[2],6) != 0) && sys_cb.wl_bond_nb == BONDING_NUM_1 ) {
                memcpy(sys_cb.wl_bd_addr1,&sys_cb.wl_bd_param[2],6);
                sys_cb.wl_bond_nb = BONDING_NUM_2;
            }
#endif // WIRELESS_CON_TX_BONDING_NUM
        }

    }
    if(bond_nb_state != sys_cb.wl_bond_nb) {
        param_bond_nb_write();
        param_bond_addr_write(0);
        param_bond_addr_write(1);
    }

}


//无线麦组队绑定参数初始化
AT(.text.wireless.cb)
void wireless_bond_init(void)
{
    param_bond_nb_read();
    param_bond_addr_read(0);
    param_bond_addr_read(1);

}

//无线麦组队绑定解绑
AT(.text.wireless.cb)
void wireless_bond_clr(void)
{
    memset(sys_cb.wl_bd_addr0,0,6);
    memset(sys_cb.wl_bd_addr1,0,6);
    sys_cb.wl_bond_nb = 0;
    param_bond_nb_write();
    param_bond_addr_write(0);
    param_bond_addr_write(1);
    printf("wireless_bond_clr\n");
    if(wireless_role_is_adapter()) {
        wireless_cb.change_flag = 1;
        bt_delete_all_link_info(NULL,0);                           //清除配对信息
    }

}
#endif

#if WIRELESS_CHANMAP_USER_DEF_EN
//------------------------------------------------------------------------------------------
//ch_map[5]数组每个bit对应一个信道，总共37个跳频信道
//其中2402,2426,2480是广播信道用于建立连接，详见BLE信道表
//adv_ch..................2402M
//map[0].bit0 --  ch0  -- 2404M
//map[0].bit1 --  ch1  -- 2406M
//map[0].bit2 --  ch2  -- 2408M
//map[1].bit2 --  ch10 -- 2424M
//adv_ch..................2426M
//map[1].bit3 --  ch11 -- 2428M
//map[4].bit3 --  ch35 -- 2476M
//map[4].bit4 --  ch36 -- 2478M
//adv_ch..................2480M

//定义以下三个函数，可以使用自定义跳频表
static u8 my_ch_map[2][5];

//刚连上时的初始跳频表
void wireless_con_ch_map_init(u8 idx, const u8 *ch_map)
{
    memcpy(&my_ch_map[idx][0], ch_map, 5);
    printf("init chmap = ");
    for (int i = 0; i < 5; i++) {
        printf("%x ",my_ch_map[idx][i]);
    }
    printf("\n");
}

//每秒会更新一次跳频，该函数返回自定义跳频表
u8 *wireless_con_ch_map_get(u8 idx)
{
    return &my_ch_map[idx][0];      //注意不能少于2个可用信道，否则无效
}

//信道变化时，通过该函数更新自定义跳频表
//信道可用时对应bit=1，不可用时对应bit=0，注意至少要有2个可用信道
ALIGNED(128)
void wireless_con_ch_map_update(u8 idx, u8 *ch_map)
{
    GLOBAL_INT_DISABLE();
    memcpy(&my_ch_map[idx][0], ch_map, 5);
    GLOBAL_INT_RESTORE();
}

//设置自定义信道示例
void wireless_con_ch_map_set(u8 idx)
{
    u8 ch_map[5] = {0xFF,0xFF,0, 0, 0};

    wireless_con_ch_map_update(idx, ch_map);
}
#else
void wireless_con_ch_map_set(u8 idx) {}
#endif

//------------------------------------------------------------------------------------------
AT(.text.wireless.cb)
int wireless_get_local_name(uint8_t *ws_name)
{
    int len;
    len = strlen(xcfg_cb.wl_name);

    memcpy(ws_name, xcfg_cb.wl_name, len);

    return len;
}

AT(.text.wireless.cb)
void wireless_get_local_bd_addr(u8 *addr)
{
    memcpy(addr, xcfg_cb.bt_addr, 6);
}

AT(.text.wireless.cb)
void wireless_con_set_chmap_cb(u8 idx, const u8 *chmap)
{
    wireless_dump_set_chmap_cb(idx, chmap);
}

//------------------------------------------------------------------------------------------
void bt_get_link_info_flash(void *buf, u16 addr, u16 size)
{
//    printf("bt_read: %04x,%04x, %08lx\n", addr, size, BT_CM_PAGE(addr));
    if ((addr + size) <= PAGE_DATA_SIZE) {
        cm_read(buf, BT_CM_PAGE(addr), size);
    }
//    print_r(buf, size);
}

void bt_put_link_info_flash(void *buf, u16 addr, u16 size)
{
//    printf("bt_write: %04x,%04x, %08lx\n", addr, size, BT_CM_PAGE(addr));
//    print_r(buf, size);
    if ((addr + size) <= PAGE_DATA_SIZE) {
        cm_write(buf, BT_CM_PAGE(addr), size);

    }
}

void bt_get_link_info(void *buf, u16 addr, u16 size)
{
//#if TEST_MODE_BT_INFO
//    if (bt_is_test_mode()) {
//        bt_get_link_info_ram(buf,addr,size);
//    } else
//#endif
    {
        cm_read(buf, BT_CM_PAGE(addr), size);
    }
}

void bt_put_link_info(void *buf, u16 addr, u16 size)
{
//#if TEST_MODE_BT_INFO
//    if (bt_is_test_mode()) {
//        bt_put_link_info_ram(buf,addr,size);
//    } else
//#endif
    {
        cm_write(buf, BT_CM_PAGE(addr), size);
    }
}

void bt_sync_link_info(void)
{
    cm_sync();
}

u8 con_req_addr[6];

//发送ble_connect_req消息之后，库获取连接地址的回调函数
AT(.text.wireless.cb)
bool ble_get_con_req_addr_cb(uint8_t *addr)
{
    memcpy(addr, con_req_addr, 6);
    return true;
}

//发送ble_scan_set_enable消息之后，底层收到数据包的回调函数
//返回值：0=错误的地址，继续扫描；1=地址正确，自动关闭扫描，可发起连接请求
AT(.text.wireless.cb)
bool ble_scan_rx_rep_cb(uint8_t *addr, uint8_t addr_type, uint8_t *adv_ext_data)
{
    printf("addr: ");
    print_r(addr, 6);

    memcpy(con_req_addr, addr, 6);

#if FUNC_DEVICE_EN
    func_device_set_create_con();
#endif
    return true;
}

#if WIRELESS_MIC_BROADCAST_EN
AT(.text.wireless.cb)
bool ble_ws_per_scan_rx_cb(uint8_t *addr, uint8_t addr_type)
{
    printf("addr: ");
    print_r(addr, 6);

    ble_per_scan_set_enable(0);
    ///发起sync，去同步刚刚scan到的设备
    ble_per_sync_set_enable(1);
    return true;
}
#endif

//addr: 连接的设备地址
//timeout: 超时时间（单位ms，0xffff时一直连接不超时）
AT(.text.wireless.api)
void ble_create_con_for_addr(uint8_t *addr, uint16_t timeout)
{
    memcpy(con_req_addr, addr, 6);
    ble_connect_req(timeout);   //发消息后，通过回调函数ble_get_con_req_addr_cb获取连接地址
}

#if (WIRELESS_CON_BONDING_EN)
///清除绑定
void wireless_mic_unbonding(void)
{
    if (wireless_role_is_adapter()){
        bt_delete_all_link_info(NULL,0);                           //清除配对信息
        memset(bongding_addr_get(),0x00,6);                        //清除绑定地址
        param_write_bonding_addr(bongding_addr_get());             //擦除地址
        ble_reset_con(1);
        if(wireless_mic_is_bonding() ){                                //组队绑定
            if(!wireless_get_status()){                                //无蓝牙连接
//                ble_reset_con(1);
                wireless_cb.change_flag = 1;                           //置位连接状态
            }else{
//                ble_reset_con(1);
                ble_disconnect_req(0);
                ble_disconnect_req(1);
            }
        }
    }else{
        if(wireless_mic_is_bonding()){                                 //组队绑定
            bt_delete_all_link_info(NULL,0);                           //清除配对信息
            memset(bongding_addr_get(),0x00,6);                        //清除绑定地址
            param_write_bonding_addr(bongding_addr_get());             //擦除地址
            if(!wireless_get_status()){                                //无蓝牙连接
    //            ble_reset_con(1);                                    /*发射端不在这里修改hash，避免和回连状态打架*/
//                wireless_cb.change_flag = 1;
            }else{
                ble_reset_con(1);
                ble_disconnect_req(0);
            }
        }
    }
}
#else
void wireless_mic_unbonding (void){
    printf("PLEASE CONFIG (WIRELESS_CON_LINK_NB == 1) && (WIRELESS_CON_BONDING_EN)\n");
}
#endif

AT(.text.wireless.init)
void printf_connect_message(void)
{
    my_printf("<TX_INTERVAL>         %d\n",cfg_wireless_tx_interval);
    my_printf("<CON_INTERVAL>        %d\n",cfg_wireless_con_interval);
    my_printf("<WIRELESS_FEAT>       %d\n",cfg_wireless_feat);
    my_printf("<WIRELESS_CODEC>      %d\n",cfg_wireless_codec[0]);
    my_printf("<FREQ_BAND>           %d\n",cfg_bb_rf_freq_bands);
    my_printf("<RETRY>               %d\n",cfg_wireless_tx_retry);
    //my_printf("<DISCON_AUTO_PWROFF>  %d\n",cfg_discon_auto_pwroff);
    my_printf("<CONFIG_RSSI>         %d\n",cfg_le_rssi_thr);
    my_printf("<WS_NAME>             %s\n",xcfg_cb.wl_name);
}


AT(.text.wireless.init)
void wireless_init(void)
{
    cfg_bb_rf_def_txpwr = xcfg_cb.bt_rf_page_pwrdec;
#if WIRELESS_EN && ADAPTER_SAVE_PARAM_EN
    wl_db_init();
#endif
    bt_setup();
    printf_connect_message();
}

AT(.text.wireless.adapter)
void wireless_adapter_init(void)
{
#if (WIRELESS_CON_CODEC_SEL == WS_CODEC_SBC)
    sbc_dec_init(WIRELESS_MIC_SAMPLE_RATE_SELECT, WIRELESS_MIC_SAMPLES_SELECT);
#elif (WIRELESS_CON_CODEC_SEL == WS_CODEC_ADPCM)
    adpcm_dec_init(WIRELESS_MIC_SAMPLE_RATE_SELECT, WIRELESS_MIC_SAMPLES_SELECT);
#elif (WIRELESS_CON_CODEC_SEL == WS_CODEC_LC3S)
    m_lc3s_dec_init(WIRELESS_MIC_SAMPLE_RATE_SELECT,WIRELESS_MIC_SAMPLES_SELECT);
#elif (WIRELESS_CON_CODEC_SEL == WS_CODEC_LC3B)
    lc3b_dec_init(WIRELESS_MIC_SAMPLE_RATE_SELECT, WIRELESS_MIC_SAMPLES_SELECT);
#endif
#if (WIRELESS_MIC_TX_INTERVAL == 1)
    plc_soft_60_init(0, WIRELESS_MIC_SAMPLES_SELECT);
#else
    plc_soft_init(0, WIRELESS_MIC_SAMPLES_SELECT);
    plc_soft_init(1, WIRELESS_MIC_SAMPLES_SELECT);
#endif

//打开本地麦功能在本地麦里头 local_mic_init 初始化 避免运行过程串改
#if (ADAPTER_MIX_DRC_EN && (!ADAPTER_LOCAL_MIC_EN))
    mic_mix_init();
#endif
#if WIRELESS_CON_PWR_CTR
    ws_pwr_ctr_init();
#endif

#if ADAPTER_EN && ADAPTER_SAVE_PARAM_EN
    wireless_con_interact_init();
#endif

    wireless_host_ch_class_set();
}

AT(.text.wireless.adapter)
void wireless_adapter_exit(u8 idx, u8 con_sta)
{
#if WIRELESS_MIC_EN
    mic_dec_reset(idx);
#endif

#if (WIRELESS_MIC_TX_INTERVAL == 1)
    plc_soft_60_exit(idx);
#else
    plc_soft_exit(idx);
	plc_soft_init(0, WIRELESS_MIC_SAMPLES_SELECT);
    plc_soft_init(1, WIRELESS_MIC_SAMPLES_SELECT);
#endif

#if (WIRELESS_CON_CODEC_SEL == WS_CODEC_LC3S)
    m_lc3s_dec_exit(idx);
#elif (WIRELESS_CON_CODEC_SEL == WS_CODEC_LC3B)
    lc3b_dec_exit(idx);
#endif

    wireless_cmd_reset(idx);

#if WIRELESS_CON_PWR_CTR
    ws_pwr_ctr_reset(idx);
#endif

    if(con_sta == 0) {
    }
#if ADAPTER_EN && ADAPTER_SAVE_PARAM_EN
    wireless_con_interact_exit(idx);
#endif
}

#if FUNC_DEVICE_EN
AT(.text.wireless.device)
void wireless_device_init(void)
{
#if (WIRELESS_CON_CODEC_SEL == WS_CODEC_SBC)
    sbc_enc_init(WIRELESS_MIC_SAMPLE_RATE_SELECT, 1);
#elif (WIRELESS_CON_CODEC_SEL == WS_CODEC_ADPCM)
    adpcm_enc_init(WIRELESS_MIC_SAMPLE_RATE_SELECT, WIRELESS_MIC_SAMPLES_SELECT);
#elif (WIRELESS_CON_CODEC_SEL == WS_CODEC_LC3S)
    lc3s_enc_init(WIRELESS_MIC_SAMPLE_RATE_SELECT, WIRELESS_MIC_SAMPLES_SELECT);
#elif (WIRELESS_CON_CODEC_SEL == WS_CODEC_LC3B)
    lc3b_enc_init(WIRELESS_MIC_SAMPLE_RATE_SELECT, WIRELESS_MIC_SAMPLES_SELECT);
#endif

#if DEVICE_INTERPHONE_EN
#if (WIRELESS_CON_CODEC_SEL == WS_CODEC_LC3S)
    m_lc3s_dec_init(WIRELESS_MIC_SAMPLE_RATE_SELECT,WIRELESS_MIC_SAMPLES_SELECT);
#elif (WIRELESS_CON_CODEC_SEL == WS_CODEC_LC3B)
    lc3b_dec_init(WIRELESS_MIC_SAMPLE_RATE_SELECT, WIRELESS_MIC_SAMPLES_SELECT);
#endif
#if (WIRELESS_MIC_TX_INTERVAL == 1)
    plc_soft_60_init(0, WIRELESS_MIC_SAMPLES_SELECT);
#else
    plc_soft_init(0, WIRELESS_MIC_SAMPLES_SELECT);
    plc_soft_init(1, WIRELESS_MIC_SAMPLES_SELECT);
#endif
#endif // DEVICE_INTERPHONE_EN

#if WIRELESS_MIC_EN
    mic_start();
#endif
}

AT(.text.wireless.device)
void wireless_device_exit(void)
{
#if WIRELESS_MIC_EN
    mic_stop();
    mic_enc_reset();
#endif
    wireless_cmd_reset(0);
}
#else
void wireless_device_init(void) {}
void wireless_device_exit(void) {}
#endif

void wireless_status_change(void)
{
    wireless_cb.change_flag = 1;
}

#if DEVICE_INTERPHONE_EN
void wireless_interphone_init(void)
{
    //wireless_adapter_init();
    wireless_device_init();
#if WIRELESS_SPK_TX_EN
    wireless_con_adapter_init();
#endif // WIRELESS_SPK_TX_EN
}

void wireless_interphone_exit(u8 idx)
{
    wireless_adapter_exit(0,0);
    wireless_device_exit();
}
#endif

#endif // WIRELESS_EN
