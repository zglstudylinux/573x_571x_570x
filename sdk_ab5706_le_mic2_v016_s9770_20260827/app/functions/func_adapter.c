#include "include.h"
#include "func.h"
#include "func_adapter.h"

#define TRACE_EN                0

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#define TRACE_R(...)            print_r(__VA_ARGS__)
#else
#define TRACE(...)
#define TRACE_R(...)
#endif // TRACE_EN

#if FUNC_ADAPTER_EN

enum {
    ADAPTER_STA_IDLE,
    ADAPTER_STA_W4_INIT,
    ADAPTER_STA_W4_CONNECT,
    ADAPTER_STA_W4_RECONN,
    ADAPTER_STA_W4_RECONN_FINISH,
    ADAPTER_STA_W4_ENABLE,
    ADAPTER_STA_W4_DISABLE,
    ADAPTER_STA_W4_DISABLE_FINISH,
    ADAPTER_STA_START_ACTION,

    ADAPTER_STA_BROADCAST_IDLE,
    ADAPTER_STA_BROADCAST_RX,
};

enum {
    ADAPTER_CMD_INVALID,
    ADAPTER_CMD_ENABLE,
    ADAPTER_CMD_DISABLE,
    ADAPTER_CMD_RECONNECT,
};


static struct {
    bool init_flag;
    bool off_flag;
    uint8_t state;
    uint8_t cmd;
    uint32_t ticks;
} adapter_cb;

//无线MIC适配器处理流程
//ADAPTER_RX -> wireless_d2a_set_rxpkt_cb -> decoder_prio_trans_audio_input -> mic_dec_audio_input --> mic_mix_audio_input-> dac0_out_audio_input -> DAC
//                                                                                                 |-> dac0_out_audio_input -> DAC
static wl_link_list_t mic_rx_cfg_tbl[] = {
/*  模块类型，                  使能，          初始化，                    输入接口，                  设置输出*/
    {DECODER_PRIO_TRANS_TYPE,   1,              decoder_prio_trans_init,    NULL,                       decoder_prio_trans_audio_output_callback_set},
    {MIC_DEC_TYPE,              1,              mic_dec_init,               mic_dec_audio_input,        mic_dec_audio_output_callback_set},

#if (ADAPTER_VOICE_CHANGE_V2_EN ||ADAPTER_MAGIC_EN || ADAPTER_AGC_EN || ADAPTER_HOWLING_DNN_EN || ADAPTER_ROOM_REVERB_EN || ADAPTER_SOFT_EQ_EN || ADAPTER_HUART_OUTPUT_EN)
    {ALG_PRIO_TRANS_TYPE,       1,              alg_prio_trans_init,        alg_prio_trans_audio_input, alg_prio_trans_audio_output_callback_set},
#endif

#if ADAPTER_DNR_FRE_EN
    {DNR_FRE_TYPE,              1,              dnr_fre_mic_init,           dnr_fre_mic_audio_input,    dnr_fre_mic_output_callback_set},
#endif
#if ADAPTER_MIC_DNR_EN
    {MIC_DNR_TYPE,              1,              mic_dnr_audio_init,         mic_dnr_audio_input,        mic_dnr_audio_output_callback_set},
#endif
#if ADAPTER_MIC_AINS4_EN
    {AINS_TYPE,                 1,              ains4_mic_init,               ains4_mic_audio_input,               ains4_mic_output_callback_set},
#endif
#if ADAPTER_MIC_DNN_L2_EN
    {DNN_L2_TYPE,               1,              dnn_L2_mic_init,              dnn_L2_mic_audio_input,          dnn_L2_mic_output_callback_set},
#endif
#if ADAPTER_MIC_DNN_L3_EN
    {DNN_L3_TYPE,               1,              dnn_L3_mic_init,              dnn_L3_mic_audio_input,          dnn_L3_mic_output_callback_set},
#endif
#if ADAPTER_HOWLING_DNN_EN
    {HOWLING_AIMASK_TYPE,       1,              howling_dnn_mic_init,         howling_dnn_mic_audio_input,         howling_dnn_mic_output_callback_set},
#endif

#if ADAPTER_AGC_EN
    {AGC_MIC_TYPE,              1,              agc_audio_init,               agc_audio_input,                     agc_audio_output_callback_set},
#endif

#if ADAPTER_ECHO_EN
    {ECHO_TYPE,                 1,              echo_audio_init,              echo_audio_input,                    echo_audio_output_callback_set},
#endif

#if ADAPTER_MAGIC_EN
    {MAGCI_PITCH_SHIFT_TYPE,    1,              magic_audio_init,             magic_audio_input,                   magic_audio_output_callback_set},
#endif

#if ADAPTER_ROOM_REVERB_EN
    {ROOM_REVERB_TYPE,          1,              room_reverb_audio_init,       room_reverb_audio_input,               room_reverb_audio_output_callback_set},
#endif

#if ADAPTER_I2S_OUTPUT_EN
    {I2S_OUT_TYPE,              1,              i2s_audio_out_init,           i2s_audio_out_input,                 i2s_audio_output_callback_set},           //I2S输出
#endif

#if ADAPTER_I2S_IN_EN
    {I2S_IN_TYPE,              1,               i2s_audio_in_init,            i2s_audio_in_input,                  i2s_audio_input_callback_set},            //I2S输入
#endif

#if ADAPTER_I2S_IN_OUT_EN
    {I2S_IN_AND_OUT_TYPE,      1,               i2s_audio_in_and_out_init,    i2s_audio_in_and_out_input,          i2s_audio_in_and_output_callback_set},    //I2S双向传输
#endif

#if ADAPTER_HUART_OUTPUT_EN
    {HUART_TX_TYPE,             1,              huart_audio_out_init,         huart_audio_out_input,               huart_audio_out_output_callback_set},
#endif
#if ADAPTER_HUART_INPUT_EN
    {HUART_RX_TYPE,             1,              huart_audio_in_init,          huart_audio_in_input,                huart_audio_in_output_callback_set},
#endif

#if ADAPTER_DAC_OUTPUT_EN
    {DAC_OUT_TYPE,              1,              dac0_out_init,                dac0_out_audio_input,                dac0_out_audio_output_callback_set},
#endif

#if ADAPTER_USB_MIC_RX_EN
#if ADAPTER_USB_MIC_STEREO_EN
    {SRC_TYPE,                  1,              src1_stereo_init,             src1_audio_input,                     src1_audio_output_callback_set},
#endif
    {USB_MIC_IN_TYPE,           1,              usb_mic_in_init,              usb_mic_in_audio_input,              usb_mic_in_audio_output_callback_set},
#endif
};

#if ADAPTER_USB_SPK_EN
static wl_link_list_t adapter_spk_cfg_tbl[] = {
/*  模块类型，                 使能，            初始化，                  输入接口，                      设置输出*/
    {USB_SPK_OUT_TYPE,         1,                usb_audio_out_init,       NULL,                           usb_audio_out_audio_output_callback_set},

    {DAC2_OUT_TYPE,            1,                dac2_out_init,            dac2_out_audio_input,           dac2_out_audio_output_callback_set},             //DAC2输出（调试SPK通路用）
};
#endif // ADAPTER_USB_SPK_EN

#if WIRELESS_MIC_BROADCAST_EN
AT(.text.func.adapter)
static void func_adapter_process_do(void)
{
    if(wireless_cb.change_flag) {
        wireless_cb.change_flag = 0;
        adapter_cb.state = ADAPTER_STA_W4_INIT;
    }
//    my_printf("adapter_cb.state %d\n",adapter_cb.state);
    switch(adapter_cb.state) {
    case ADAPTER_STA_W4_INIT:
        ble_per_scan_set_enable(1);
        adapter_cb.state = ADAPTER_STA_BROADCAST_RX;
        break;
    case ADAPTER_STA_BROADCAST_RX:
        adapter_cb.state = ADAPTER_STA_BROADCAST_IDLE;
        break;

    default:
        break;
    }
}
#else
AT(.text.func.adapter)
static void func_adapter_process_do(void)
{
    u8 link_nb = 0;

//    if(wireless_cb.change_flag) {
//        wireless_cb.change_flag = 0;
//
//        if(adapter_cb.state > ADAPTER_STA_W4_DISCON) {
//            adapter_cb.state = ADAPTER_STA_START_ACTION;
//        }
//    }

    if (adapter_cb.state == ADAPTER_STA_IDLE) {
        if (wireless_cb.change_flag) {
            wireless_cb.change_flag = false;
#if WIRELESS_CON_BONDING_EN
            wireless_bond_nb_updatae();
#endif
            adapter_cb.state = ADAPTER_STA_START_ACTION;
        } else if (adapter_cb.cmd == ADAPTER_CMD_ENABLE) {
            adapter_cb.cmd = ADAPTER_CMD_INVALID;
            adapter_cb.state = ADAPTER_STA_W4_ENABLE;
        } else if (adapter_cb.cmd == ADAPTER_CMD_DISABLE) {
            adapter_cb.cmd = ADAPTER_CMD_INVALID;
            adapter_cb.state = ADAPTER_STA_W4_DISABLE;
        } else if (adapter_cb.cmd == ADAPTER_CMD_RECONNECT) {
            adapter_cb.cmd = ADAPTER_CMD_INVALID;
            adapter_cb.state = ADAPTER_STA_W4_RECONN;
        }
    }

    switch(adapter_cb.state) {
    case ADAPTER_STA_W4_INIT:
        link_nb = wireless_get_link_info_state();
        TRACE("adapter, init(%d): %x, %d\n", wireless_mic_is_bonding(), wireless_cb.connected_sta, link_nb);

        if(link_nb != 0) {
            //有回连信息，等待被连接（关闭可被发现）
            ble_adv_set_enable(1, 0);
            adapter_cb.state  = ADAPTER_STA_W4_CONNECT;
            adapter_cb.ticks       = tick_get();
        } else {
            //没有回连信息，等待被发现和连接
            ble_adv_set_enable(1, 1);
            adapter_cb.state  = ADAPTER_STA_IDLE;
        }
        break;

    case ADAPTER_STA_W4_CONNECT:
        if(tick_check_expire(adapter_cb.ticks, 2000)) {
            adapter_cb.state = ADAPTER_STA_START_ACTION;
        }
        break;

    case ADAPTER_STA_START_ACTION:
        link_nb = wireless_get_link_info_state();
        TRACE("adapter, con_sta(%d): %x, %d\n", wireless_mic_is_bonding(), wireless_cb.connected_sta, link_nb);

        if(wireless_cb.connected_sta == WIRELESS_CON_STA_MASK) {
            //两个都连上了，关闭可被发现和可被连接
            ble_adv_set_enable(0, 0);
        } else if(wireless_mic_is_bonding() && link_nb >= WIRELESS_CON_LINK_NB) {
            //组队绑定时，切配对过两个，等待被连接（关闭可被发现）
            ble_adv_set_enable(1, 0);
        } else {
            //等待被发现和连接
            ble_adv_set_enable(1, 1);
        }
        adapter_cb.state = ADAPTER_STA_IDLE;
        break;
    case ADAPTER_STA_W4_RECONN:
        //关闭可被发现和可被连接
        ble_adv_set_enable(0, 0);
        //断开所有连接
        wireless_discon_all();
        adapter_cb.state = ADAPTER_STA_W4_RECONN_FINISH;
        break;
    case ADAPTER_STA_W4_RECONN_FINISH:
        if (wireless_cb.adv_sta != 0) {
            break;
        }
        if(wireless_cb.connected_sta != 0) {
            break;
        }

        wireless_cb.change_flag = false;
        adapter_cb.state = ADAPTER_STA_START_ACTION;
        break;
    case ADAPTER_STA_W4_ENABLE:
        wireless_cb.change_flag = false;
        adapter_cb.state = ADAPTER_STA_START_ACTION;
        break;
    case ADAPTER_STA_W4_DISABLE:
        //关闭可被发现和可被连接
        ble_adv_set_enable(0, 0);
        //断开所有连接
        wireless_discon_all();
        adapter_cb.state = ADAPTER_STA_W4_DISABLE_FINISH;
        break;
    case ADAPTER_STA_W4_DISABLE_FINISH:
        if (wireless_cb.adv_sta != 0) {
            break;
        }
        if(wireless_cb.connected_sta != 0) {
            break;
        }

        wireless_cb.change_flag = false;
        adapter_cb.state = ADAPTER_STA_IDLE;
        break;
    default:
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
                ble_set_con_id(ADAPTER_SET_TX1_CON_ID);
                sys_cb.con_role = 1;
                wireless_cb.con_role_switch_flag = 0;
                printf("Master ADV\n");
            } else {
                ble_set_con_id(ADAPTER_SET_TX2_CON_ID);
                sys_cb.con_role = 0;
                wireless_cb.con_role_switch_flag = 1;
                printf("Slave ADV\n");
            }
        }
    } else if((wireless_get_status() != 3) && !wireless_cb.con_temp) {
        wireless_cb.con_temp = 1;
        if(sys_cb.con_role) {           //已经连上主麦
            ble_set_con_id(ADAPTER_SET_TX2_CON_ID);
            sys_cb.con_role = 0;
            printf("Slave ADV\n");
        } else {                       //已经连上副麦
            ble_set_con_id(ADAPTER_SET_TX1_CON_ID);
            sys_cb.con_role = 1;
            printf("Master ADV\n");
        }
    }
}
#endif

AT(.com_text.adapter)
void wireless_adapter_set_reconnect(void)
{
    adapter_cb.cmd = ADAPTER_CMD_RECONNECT;
}

AT(.com_text.adapter)
void wireless_adapter_set_enable(void)
{
    adapter_cb.cmd = ADAPTER_CMD_ENABLE;
}

AT(.com_text.adapter)
void wireless_adapter_set_disable(void)
{
    adapter_cb.cmd = ADAPTER_CMD_DISABLE;
}

AT(.text.func.process.adapter)
void func_adapter_process(void)
{
#if TRACE_EN
    static u8 sta = 0xff;
    if(sta != adapter_cb.state || wireless_cb.change_flag) {
        sta = adapter_cb.state;
        TRACE("adapter, state: %d\n", sta);
    }
#endif

#if WIRELESS_CON_PAIR_MODE
    if(!wireless_cb.con_id_switch_ticks) {
        wireless_cb.con_id_switch_ticks = tick_get();
        printf("con_role_init\n");
        ble_set_con_id(0);
    }

    wireless_con_role();
#endif


    func_adapter_process_do();

#if ADAPTER_USB_MIC_RX_EN | ADAPTER_USB_SPK_EN
    usb_device_process();
#endif

//    led_disp_proc();
    wireless_sta_proc();
    func_process();
}

AT(.com_text.func.adapter_usb)
u8 adapter_usb_init_en(void)
{
    return adapter_cb.init_flag;
}

AT(.text.func.adapter)
void func_adapter_init(void)
{
    if (!adapter_cb.init_flag) {
        adapter_cb.init_flag = 1;
        adapter_cb.off_flag = false;
        lock_code_wl_com();
        wl_link_list_init(mic_rx_cfg_tbl, (sizeof(mic_rx_cfg_tbl)/sizeof(wl_link_list_t)), WIRELESS_MIC_SAMPLE_RATE_SELECT, WIRELESS_MIC_SAMPLES_SELECT, WIRELESS_MIC_CHANNEL_SELECT);

#if ADAPTER_USB_SPK_EN
        wl_link_list_init(adapter_spk_cfg_tbl, (sizeof(adapter_spk_cfg_tbl)/sizeof(wl_link_list_t)), WIRELESS_MIC_SAMPLE_RATE_SELECT, WIRELESS_MIC_SAMPLES_SELECT, WIRELESS_MIC_CHANNEL_SELECT);
#endif

        adapter_cb.state = ADAPTER_STA_W4_INIT;
        adapter_cb.cmd = ADAPTER_CMD_INVALID;

#if ADAPTER_LOCAL_MIC_EN
        local_mic_init();
#endif
    }
}

AT(.text.func.adapter)
static void func_adapter_enter(void)
{
#if WIRELESS_CON_BONDING_EN
    wireless_bond_init();
#endif

#if BT_RF_PWR_BALANCE_EN
    bb_rf_ext_ctl_init();
#endif // BT_RF_PWR_BALANCE_EN
#if ADAPTER_UART_COMMAND_EN
    uart2_command_init();
#endif
    func_adapter_init();
    func_bt_init();
}

AT(.text.func.adapter)
static void func_adapter_exit(void)
{
    bt_off();
    func_cb.last = FUNC_ADAPTER;
    adapter_cb.init_flag = 0;
	sys_cb.bt_is_inited = 0;
	if (wireless_cb.connected_sta) {
		wireless_cb.connected_sta = 0;
	}
}

AT(.text.func.adapter)
void func_adapter(void)
{
    printf("%s\n", __func__);

    func_adapter_enter();

    //多个RX放一起上电的时候, 可以加一个随机延迟, 避免多个RX广播时序撞在一起, 影响连接
    //delay_ms(bt_get_rand()%80);

    while (func_cb.sta == FUNC_ADAPTER) {
        func_adapter_process();
        func_adapter_message(msg_dequeue());
    }

    func_adapter_exit();
}

#endif // FUNC_ADAPTER_EN
