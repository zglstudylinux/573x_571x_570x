#include "include.h"
#include "func.h"
#include "func_adapter.h"
//#include "bsp_usbfot.h"

#define TRACE_EN                1

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#define TRACE_R(...)            print_r(__VA_ARGS__)
#else
#define TRACE(...)
#define TRACE_R(...)
#endif // TRACE_EN

#if FUNC_ADAPTER_EN

enum {
    ADAPTER_STA_INIT_IDLE,
    ADAPTER_STA_INIT_W4_CONNECT,
    ADAPTER_STA_START_ACTION,
    ADAPTER_STA_IDLE,

    ADAPTER_STA_BROADCAST_IDLE,
    ADAPTER_STA_BROADCAST_RX,

    ADAPTER_STA_INIT_CONNECT,
    ADAPTER_STA_DELAY,
    ADAPTER_STA_SCAN,
};

static struct {
    uint8_t init_flag;
    uint8_t init_state;
    uint32_t ticks;
    bool usbdev_insert;
    uint tick_delay;
    bool create_con_flag;
} adapter_cb;

//无线MIC适配器处理流程
//ADAPTER_RX -> wireless_d2a_set_rxpkt_cb -> decoder_prio_trans_audio_input -> mic_dec_audio_input --> mic_mix_audio_input-> dac0_out_audio_input -> DAC
//                                                                                                 |-> dac0_out_audio_input -> DAC
static wl_link_list_t mic_rx_cfg_tbl[] = {
/*  模块类型，                  使能，          初始化，                    输入接口，                  设置输出*/
    {DECODER_PRIO_TRANS_TYPE,   1,              decoder_prio_trans_init,    NULL,                       decoder_prio_trans_audio_output_callback_set},
    {MIC_DEC_TYPE,              1,              mic_dec_init,               mic_dec_audio_input,        mic_dec_audio_output_callback_set},
#if ADAPTER_DNN_L1_EN
    {DNN_L1_TYPE,               1,              dnn_L1_mic_init,            dnn_L1_mic_audio_input,     dnn_L1_mic_output_callback_set},
#endif
#if ADAPTER_ECHO_EN
    {ECHO_TYPE,                 1,              echo_audio_init,            echo_audio_input,           echo_audio_output_callback_set},
#endif
#if ADAPTER_I2S_IN_OUT_EN
    {I2S_IN_AND_OUT_TYPE,       1,              i2s_audio_in_out_init,      i2s_audio_in_out_input,     i2s_audio_in_out_input_callback_set},
#endif
#if ADAPTER_DAC_OUTPUT_EN
    {DAC_OUT_TYPE,              1,              dac0_out_init,              dac0_out_audio_input,       dac0_out_audio_output_callback_set},
#endif
};

#if WIRELESS_FULL_DUPLEX
static wl_link_list_t spk_tx_cfg_tbl[] = {//测试usb speaker下行音频#if FUNC_TEST_PRINT//调试打印
/*  模块类型，                 使能，            初始化，                  输入接口，                      设置输出*/
    {MIC_TYPE,                  1,              mic_init,                   NULL,                           mic_audio_output_callback_set},
    {ENCODER_PRIO_TRANS_TYPE,   1,              encoder_prio_trans_init,    encoder_prio_trans_audio_input, encoder_prio_trans_audio_output_callback_set},    //缓存到编码缓冲区，并转到低优先级处理
    {MIC_ENC_TYPE,              1,              mic_enc_init,               mic_enc_audio_input,            mic_enc_audio_output_callback_set},
};
#endif // WIRELESS_FULL_DUPLEX

AT(.text.func.adapter)
void func_adapter_init(void)
{
    if (!adapter_cb.init_flag) {
        adapter_cb.init_flag = 1;
#if WIRELESS_FULL_DUPLEX
        load_code_wl_full_duplex();
        wl_link_list_init(spk_tx_cfg_tbl, (sizeof(spk_tx_cfg_tbl)/sizeof(wl_link_list_t)), WIRELESS_MIC_SAMPLE_RATE_SELECT, WIRELESS_MIC_SAMPLES_SELECT, WIRELESS_MIC_CHANNEL_SELECT);
#else
        load_code_wl_adapter();
#endif
        lock_code_wl_com();

		wl_link_list_init(mic_rx_cfg_tbl, (sizeof(mic_rx_cfg_tbl)/sizeof(wl_link_list_t)), WIRELESS_MIC_SAMPLE_RATE_SELECT, WIRELESS_MIC_SAMPLES_SELECT, WIRELESS_MIC_CHANNEL_SELECT);
    }

    adapter_cb.init_state = ADAPTER_STA_INIT_IDLE;
}

#if WIRELESS_MIC_2TNR_EN
AT(.text.func.adapter)
static void func_adapter_process_do(void)
{
    u8 addr[6];

    if(wireless_cb.change_flag) {
        wireless_cb.change_flag = 0;

        if (adapter_cb.create_con_flag) {
            TRACE("adapter, create_con\n");
            adapter_cb.create_con_flag = 0;
            wireless_scan_set_enable(0);
            wireless_connect_req(500);
            adapter_cb.init_state  = ADAPTER_STA_INIT_CONNECT;
        } else {
            switch(wireless_cb.change_sta) {
            case 0:     //connect success
                adapter_cb.init_state = ADAPTER_STA_START_ACTION;
                break;

            case 1:     //connect fail
                if(!wireless_mic_is_bonding()) {
                    adapter_cb.ticks      = tick_get();
                    adapter_cb.tick_delay = 100+get_random(0xffff)%500;
                    adapter_cb.init_state = ADAPTER_STA_DELAY;
                    break;
                }
                //no break

            case 2:     //disconnect
                adapter_cb.ticks      = tick_get();
                adapter_cb.tick_delay = 400+get_random(0xffff)%500;
                adapter_cb.init_state = ADAPTER_STA_DELAY;
                break;
            }
        }
    }

    switch(adapter_cb.init_state) {
    case ADAPTER_STA_INIT_IDLE:
        TRACE("adapter, init(%d)\n", wireless_mic_is_bonding());
        if(wireless_get_link_info_addr(addr)) {
            //有回连信息，开始回连
            TRACE("adapter, con_req: ");
            TRACE_R(addr, 6);
            wireless_create_con_for_addr(addr, 1500);
            adapter_cb.init_state  = ADAPTER_STA_INIT_CONNECT;
        } else {
            //没有回连信息，搜索组队
            TRACE("adapter, scan_en\n");
            wireless_scan_set_enable(1);
            adapter_cb.init_state  = ADAPTER_STA_SCAN;
            adapter_cb.ticks       = tick_get();
        }
        break;

    case ADAPTER_STA_INIT_CONNECT:
        break;

    case ADAPTER_STA_START_ACTION:
        TRACE("adapter, con_sta(%d): %x\n", wireless_mic_is_bonding(), wireless_cb.connected_sta);

        if(wireless_cb.connected_sta == WIRELESS_CON_STA_MASK) {
            //已连接，关闭扫描
            TRACE("adapter, con_complete\n");
            adapter_cb.init_state = ADAPTER_STA_IDLE;
        } else if(wireless_mic_is_bonding() && wireless_get_link_info_addr(addr)) {
            //有回连信息，开始回连
            TRACE("adapter, con_req: ");
            TRACE_R(addr, 6);
            wireless_create_con_for_addr(addr, 1500);
            adapter_cb.init_state  = ADAPTER_STA_INIT_CONNECT;
        } else {
            //没有回连信息，搜索组队
            TRACE("adapter, scan_en\n");
            wireless_scan_set_enable(1);
            adapter_cb.init_state = ADAPTER_STA_SCAN;
            adapter_cb.ticks      = tick_get();
        }
        break;

    case ADAPTER_STA_SCAN:
//        if(tick_check_expire(adapter_cb.ticks, 400)) {     //省电时可以间歇性扫描
//            wireless_scan_set_enable(0);
//            adapter_cb.ticks      = tick_get();
//            adapter_cb.tick_delay = 500+get_random(0xffff)%200;
//            adapter_cb.init_state = ADAPTER_STA_DELAY;
//        }
        break;

    case ADAPTER_STA_DELAY:
        if(tick_check_expire(adapter_cb.ticks, adapter_cb.tick_delay)) {
            adapter_cb.init_state = ADAPTER_STA_START_ACTION;
        }
        break;
    }
}
#elif WIRELESS_MIC_BROADCAST_EN
AT(.text.func.adapter)
static void func_adapter_process_do(void)
{
    if(wireless_cb.change_flag) {
        wireless_cb.change_flag = 0;
        adapter_cb.init_state = ADAPTER_STA_INIT_IDLE;
    }

    switch(adapter_cb.init_state) {
    case ADAPTER_STA_INIT_IDLE:
        wireless_per_scan_set_enable(1);
        adapter_cb.init_state = ADAPTER_STA_BROADCAST_RX;
        break;
    case ADAPTER_STA_BROADCAST_RX:
        adapter_cb.init_state = ADAPTER_STA_BROADCAST_IDLE;
        break;

    default:
        break;
    }
}
#else
AT(.text.func.adapter)
static void func_adapter_process_do(void)
{
    u8 link_nb;

    if(wireless_cb.change_flag) {
        wireless_cb.change_flag = 0;
#if WIRELESS_CON_BONDING_EN
        wireless_bond_nb_updatae();
#endif
        adapter_cb.init_state = ADAPTER_STA_START_ACTION;
#if WIRELESS_CON_AND_BT_PAIRING_EN
        wireless_adapter_bond_update();
#endif
    }

    switch(adapter_cb.init_state) {
    case ADAPTER_STA_INIT_IDLE:
        link_nb = wireless_get_link_info_state();
        TRACE("adapter, init(%d): %x, %d\n", wireless_mic_is_bonding(), wireless_cb.connected_sta, link_nb);

        if(link_nb != 0) {
            //有回连信息，等待被连接（关闭可被发现）
            adapter_adv_set_enable(1, 0);
            adapter_cb.init_state  = ADAPTER_STA_INIT_W4_CONNECT;
            adapter_cb.ticks       = tick_get();
        } else {
#if WIRELESS_CON_AND_BT_PAIRING_EN
            if(sys_cb.wl_bond_nb > 0) {
                wireless_adapter_pairing_enable(1);
            }
#endif
            //没有回连信息，等待被发现和连接
            adapter_adv_set_enable(1, 1);
            adapter_cb.init_state  = ADAPTER_STA_IDLE;
        }
        break;

    case ADAPTER_STA_INIT_W4_CONNECT:
        if(tick_check_expire(adapter_cb.ticks, 2000)) {
            adapter_cb.init_state = ADAPTER_STA_START_ACTION;
        }
        break;

    case ADAPTER_STA_START_ACTION:
        link_nb = wireless_get_link_info_state();
        TRACE("adapter, con_sta(%d): %x, %d\n", wireless_mic_is_bonding(), wireless_cb.connected_sta, link_nb);

        if(wireless_cb.connected_sta == WIRELESS_CON_STA_MASK) {
            //两个都连上了，关闭可被发现和可被连接
            adapter_adv_set_enable(0, 0);
        } else if(wireless_mic_is_bonding() && link_nb >= WIRELESS_CON_LINK_NB) {
            //组队绑定时，切配对过两个，等待被连接（关闭可被发现）
            adapter_adv_set_enable(1, 0);
        } else {
#if WIRELESS_CON_AND_BT_PAIRING_EN
            if(sys_cb.wl_bond_nb > 0) {
                wireless_adapter_pairing_enable(1);
            }
#endif
            //等待被发现和连接
            adapter_adv_set_enable(1, 1);
        }
        adapter_cb.init_state = ADAPTER_STA_IDLE;
        break;
    }
}
#endif

#if WIRELESS_CON_PAIR_MODE
AT(.text.bsp.wireless_cb)
void wireless_con_role(void)          //判断主副麦
{
    if(!wireless_get_status()) {
        if(tick_check_expire(wireless_cb.con_id_switch_ticks, 3000)){     //3s切换一次广播数据包
            wireless_cb.con_id_switch_ticks = tick_get();
            if(wireless_cb.con_role_switch_flag) {
                wireless_set_con_id(ADAPTER_SET_TX1_CON_ID);
                sys_cb.con_role = 1;
                wireless_cb.con_role_switch_flag = 0;
                printf("Master ADV\n");
            } else {
                wireless_set_con_id(ADAPTER_SET_TX2_CON_ID);
                sys_cb.con_role = 0;
                wireless_cb.con_role_switch_flag = 1;
                printf("Slave ADV\n");
            }
        }
    } else if((wireless_get_status() != 3) && !wireless_cb.con_temp) {
        wireless_cb.con_temp = 1;
        if(sys_cb.con_role) {           //已经连上主麦
            wireless_set_con_id(ADAPTER_SET_TX2_CON_ID);
            sys_cb.con_role = 0;
            printf("Slave ADV\n");
        } else {                       //已经连上副麦
            wireless_set_con_id(ADAPTER_SET_TX1_CON_ID);
            sys_cb.con_role = 1;
            printf("Master ADV\n");
        }
    }
}
#endif

#if ADAPTER_SECOND_CON_VERS_EN
AT(.text.bsp.wireless_cb)
void adapter_second_con_vers_proc(void)          //切换con_vers
{
    u8 con_status = wireless_get_status();

    if (con_status == 0) {
        if (tick_check_expire(wireless_cb.second_tick, 2000)){
            wireless_cb.second_tick = tick_get();
            wireless_set_con_vers_adv(WIRELESS_CON_VERS, 0, 0, 0);
            delay_5ms(10);
            if (wireless_cb.second_idx) {
                wireless_set_con_vers_adv(WIRELESS_CON_VERS, 0, 1, 1);
            } else {
                wireless_set_con_vers_adv(ADAPTER_SECOND_CON_VERS, 1, 1, 1);
            }
            wireless_cb.second_idx = !wireless_cb.second_idx;
        }
    }
}

void adapter_con_vers_set_adv(uint8_t pscan, uint8_t iscan)
{
    u8 con_status = wireless_get_status();

    if ((con_status == 1)) {
        wireless_set_con_vers_adv(ADAPTER_SECOND_CON_VERS, 1, pscan, iscan);
    } else if ((con_status == 2)) {
        wireless_set_con_vers_adv(WIRELESS_CON_VERS, 0, pscan, iscan);
    } else if ((con_status == 3)) {
        wireless_set_con_vers_adv(WIRELESS_CON_VERS, 0, pscan, iscan);
    }
}
#endif

AT(.text.func.process.adapter)
void func_adapter_process(void)
{
#if TRACE_EN
    static u8 sta = 0xff;
    if(sta != adapter_cb.init_state) {
        sta = adapter_cb.init_state;
        TRACE("adapter, state: %d\n", sta);
    }
#endif

#if WIRELESS_CON_PAIR_MODE
    if(!wireless_cb.con_id_switch_ticks) {
        wireless_cb.con_id_switch_ticks = tick_get();
        printf("con_role_init\n");
        wireless_set_con_id(0);
    }

    wireless_con_role();
#endif

#if ADAPTER_SECOND_CON_VERS_EN
    if(!wireless_cb.second_tick) {
        wireless_cb.second_tick = tick_get();
    }
    adapter_second_con_vers_proc();
#endif

#if WIRELESS_MIC_2TNR_EN
    if(((adapter_cb.init_state != ADAPTER_STA_IDLE) && (adapter_cb.init_state != ADAPTER_STA_INIT_CONNECT)) || wireless_cb.change_flag) {
#elif WIRELESS_MIC_BROADCAST_EN
    if(adapter_cb.init_state != ADAPTER_STA_BROADCAST_IDLE || wireless_cb.change_flag) {
#else
    if(adapter_cb.init_state != ADAPTER_STA_IDLE || wireless_cb.change_flag) {
#endif
        func_adapter_process_do();
    }

    wireless_sta_proc();
    func_process();
}

AT(.text.func.adapter)
static void func_adapter_enter(void)
{
#if WIRELESS_CON_BONDING_EN
    wireless_bond_init();
#endif
    func_adapter_init();
    led_bt_idle();
    func_bt_init();
#if ADAPTER_NORMAL_LE_EN
	ble_adv_start_init();
	ble_con_start_init();
#endif

    wireless_adv_set_interval(WIRELESS_MIC_TX_INTERVAL*16);
#if WIRELESS_MIC_2TNR_EN
    wireless_scan_set_param(64,32);
    wireless_set_con_flag(CON_FLAG_T2R, CON_FLAG_T2R);
#endif
#if WIRELESS_CON_AND_BT_PAIRING_EN
    if(sys_cb.wl_bond_nb > 0) {
        wireless_adapter_pairing_enable(1);
    }
#endif
}

AT(.text.func.adapter)
static void func_adapter_exit(void)
{
//    le_mic_discon_all();
    bt_off();
    sys_cb.bt_is_inited = 0;
    func_cb.last = FUNC_ADAPTER;
    adapter_cb.init_flag = 0;
}

#if WIRELESS_MIC_2TNR_EN
AT(.text.func.adapter)
void func_adapter_set_create_con(void)
{
    adapter_cb.create_con_flag = 1;
    wireless_cb.change_flag = 1;
}
#endif

AT(.text.func.adapter)
void func_adapter(void)
{
    printf("%s\n", __func__);

    func_adapter_enter();

    while (func_cb.sta == FUNC_ADAPTER) {
        func_adapter_process();
        func_adapter_message(msg_dequeue());
        //func_adapter_display();
    }

    func_adapter_exit();
}

#endif // FUNC_ADAPTER_EN
