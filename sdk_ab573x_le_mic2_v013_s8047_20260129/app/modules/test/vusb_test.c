#include "include.h"
#include "vusb_test.h"
//#include "driver_com.h"
//#include "driver_gpio.h"

#if TBOX_TEST_EN
static product_test_cmd_t product_test_tx_cmd;
static product_test_str_t product_test_str;
static huart_t huart0;

qtest_cb_t qtest_cb;
AT(.com_rodata.qtest.tbl)
const u8 kick_qtest_sig[] = {'S', 'I', 'G'};
AT(.com_text.qtest)
void qtest_set_flag_cb(u8 flag)
{
    qtest_cb.flag = flag;
    qtest_cb.sta = 1;
}
AT(.com_text.qtest)
u8* qtest_get_txbuf(void)
{
    return qtest_cb.other_usage_txbuf;
}
AT(.com_text.qtest)
void qtest_other_usage_process(void)
{
    u8 *tx_buf = qtest_get_txbuf();

    if (qtest_cb.ack != qtest_cb.flag) {
        qtest_cb.ack = qtest_cb.flag;
        qtest_cb.flag = 0;
        if (qtest_cb.ack == QTEST_MODE_DUT) {
            func_cb.sta = FUNC_BT_DUT;
        } else if (qtest_cb.ack == QTEST_MODE_CLR_INFO) {
            //保留

        } else if ((qtest_cb.ack == QTEST_MODE_PWROFF) || (qtest_cb.ack == QTEST_MODE_PWROFF_LP)) {
            if (qtest_cb.ack == QTEST_MODE_PWROFF_LP) {
                qtest_cb.pdn_boat_flag = 1;
            }
            sys_cb.discon_reason = 0;//不同步关耳机
            func_cb.sta = FUNC_PWROFF;
        }else if (qtest_cb.ack == QTEST_MODE_BT) {
            func_cb.sta = FUNC_BT; 
        }
        tx_buf[0] = 'S';
        tx_buf[1] = 'I';
        tx_buf[2] = 'G';
        tx_buf[3] = qtest_cb.ack;

        huart_tx(tx_buf,4);
        qtest_cb.ack = 0;
    }
}
AT(.com_text.mic_emit.proc.wireless_uart)
void product_test_rx_callback(u8 *buf, u16 rx_len)
{
    product_test_cmd_t *huart_rx_ptr = &product_test_str.huart_test_cmd ;//(product_test_cmd_t *)buf;
    if ((huart_rx_ptr->vusb_head == PRODUCT_TEST_HEAD) && (product_test_str.huart_test_cmd.len == rx_len) && (rx_len >= 6)) {
        product_test_str.rx_kick_flag = 1;
    }
    if (QTEST_IS_ENABLE()) {
        if (!memcmp(buf, kick_qtest_sig, sizeof(kick_qtest_sig))) {
            print_r(buf,rx_len);
            qtest_set_flag_cb(buf[3]);
            qtest_other_usage_process();
        }
    }
}

AT(.com_text.mic_emit.proc.wireless_huart)
bool product_test_huart_done(void)
{
    product_test_rx_callback((u8 *)&product_test_str.huart_test_cmd, huart_get_rxcnt());
    if(product_test_str.rx_kick_flag) {
        return true;
    }
    return false;
}

AT(.com_text.mic_emit.proc.wireless_huart)
void product_test_start_rsp_send(void)
{
    product_test_tx_cmd.vusb_head = PRODUCT_TEST_HEAD;
    product_test_tx_cmd.cmd = PRODUCT_TEST_START_RSP;
    product_test_tx_cmd.len = 6;
    product_test_tx_cmd.crc8 = bb_crc8((void *)&product_test_tx_cmd.cmd, product_test_tx_cmd.len - 4);
    huart_tx((u8 *)&product_test_tx_cmd, product_test_tx_cmd.len);
}

AT(.com_text.mic_emit.proc.wireless_huart)
void product_test_get_info_rsp_send(void)
{
    vusb_set_config_t *vusb_get_info_rsp = (vusb_set_config_t *)&product_test_tx_cmd.buf[0];

    product_test_tx_cmd.vusb_head = PRODUCT_TEST_HEAD;
    product_test_tx_cmd.cmd = PRODUCT_GET_INFO_RSP;
    vusb_get_info_rsp->freq_band = WIRELESS_CON_FREQ_BAND;
    vusb_get_info_rsp->tx_intv = WIRELESS_MIC_TX_INTERVAL;
    vusb_get_info_rsp->con_intv = WIRELESS_CON_INTERVAL*2;
    vusb_get_info_rsp->retry = WIRELESS_MIC_RETRY_NB;
    vusb_get_info_rsp->ws_feat = cfg_wireless_feat;
    vusb_get_info_rsp->codec[0] = cfg_wireless_codec[0];
    vusb_get_info_rsp->codec[1] = cfg_wireless_codec[1];
    vusb_get_info_rsp->config_rssi = WIRELESS_CON_RSSI_THR;
    u8 len = strlen(xcfg_cb.wl_name);
    vusb_get_info_rsp->wl_name_len = (len > PRODUCT_TEST_WL_NAME_LEN) ? PRODUCT_TEST_WL_NAME_LEN : len;
    memcpy(vusb_get_info_rsp->wl_name, xcfg_cb.wl_name, vusb_get_info_rsp->wl_name_len);
    product_test_tx_cmd.len = vusb_get_info_rsp->wl_name_len + (sizeof(product_test_cmd_t) - PRODUCT_TEST_WL_NAME_LEN);
    product_test_tx_cmd.crc8 = bb_crc8((void *)&product_test_tx_cmd.cmd, product_test_tx_cmd.len - 4);

    huart_tx((u8 *)&product_test_tx_cmd, product_test_tx_cmd.len);
}

AT(.com_text.mic_emit.proc.wireless_huart)
void vusb_set_config_req_do(uint8_t *buf, uint8_t buf_len)
{
    vusb_set_config_t *vusb_set_config_cmd = (vusb_set_config_t *)buf;

    cfg_bb_rf_freq_bands = vusb_set_config_cmd->freq_band;
    cfg_wireless_tx_interval = vusb_set_config_cmd->tx_intv;
    cfg_wireless_con_interval = vusb_set_config_cmd->con_intv;
    cfg_wireless_tx_retry = vusb_set_config_cmd->retry;
    cfg_wireless_feat = vusb_set_config_cmd->ws_feat;
    cfg_discon_auto_pwroff = vusb_set_config_cmd->discon_auto_pwroff;
    cfg_wireless_codec[0] = vusb_set_config_cmd->codec[0];
    cfg_wireless_codec[1] = vusb_set_config_cmd->codec[1];
    product_test_str.config_rssi = vusb_set_config_cmd->config_rssi;
    product_test_str.wl_name_len = vusb_set_config_cmd->wl_name_len;
    memcpy((u8 *)&product_test_str.wl_name[0], (u8 *)&vusb_set_config_cmd->wl_name[0], vusb_set_config_cmd->wl_name_len);
    product_test_str.wl_config_sucess_flag = 1;
}

AT(.com_text.mic_emit.proc.wireless_huart)
void product_test_set_config_rsp_send(void)
{
    product_test_tx_cmd.vusb_head = PRODUCT_TEST_HEAD;
    product_test_tx_cmd.cmd = PRODUCT_SET_CONFIG_RSP;
    product_test_tx_cmd.len = 6;
    product_test_tx_cmd.crc8 = bb_crc8((void *)&product_test_tx_cmd.cmd, product_test_tx_cmd.len - 4);

    huart_tx((u8 *)&product_test_tx_cmd, product_test_tx_cmd.len);
}

AT(.com_text.at_vusb)
void product_test_rx_process(void)
{
    product_test_cmd_t *huart_rx_ptr = &product_test_str.huart_test_cmd;
    if (product_test_str.rx_kick_flag) {
        print_r(huart_rx_ptr, huart_rx_ptr->len);
        if (huart_rx_ptr->crc8 == bb_crc8((void *)&huart_rx_ptr->cmd, huart_rx_ptr->len - 4)) {
            memset((u8 *)&product_test_tx_cmd, 0, sizeof(product_test_tx_cmd));
            switch(huart_rx_ptr->cmd) {
                case PRODUCT_TEST_START_REQ:
                    product_test_str.vusb_start_test_flag = 1;
                    product_test_start_rsp_send();
                    break;
                case PRODUCT_GET_INFO_REQ:
                    product_test_get_info_rsp_send();
                    break;
                case PRODUCT_SET_CONFIG_REQ:
                    if (product_test_str.vusb_start_test_flag) {
                        vusb_set_config_req_do(huart_rx_ptr->buf, huart_rx_ptr->len);
                        product_test_set_config_rsp_send();
                        ///延时一会让他能发的出去
                        delay_5ms(10);
                    }
                    break;
                default:
                    break;
            }
        }
        product_test_str.rx_kick_flag = 0;

    }
}

uint8_t product_test_is_sucess(void)
{
    return product_test_str.wl_config_sucess_flag;
}

uint8_t product_test_rssi_get(void)
{
    return product_test_str.config_rssi;
}

uint8_t product_test_name_get(u8 *le_name)
{
    memcpy(le_name, (u8 *)&product_test_str.wl_name[0], product_test_str.wl_name_len);

    return product_test_str.wl_name_len;
}

void product_test_init(void)
{
    memset((u8 *)&product_test_tx_cmd, 0, sizeof(product_test_tx_cmd));
    memset((u8 *)&product_test_str, 0, sizeof(product_test_str_t));
    if (UART0_PRINTF_SEL == PRINTF_PB3) {
        FUNCMCON0 = (0xf << 12) | (0xf << 8);           //clear uart0 mapping;
    }
    vusb_reset_set_us_time(1);                         //VUSB reset delay time
    product_test_huart_init();
}

void product_test_exit(void)
{
    huart_exit();
    if (UART0_PRINTF_SEL == PRINTF_PB3) {
        uart0_mapping_sel();
    } else if (xcfg_cb.bled_io_sel == 19) {
#if BSP_LED_EN
        bled_init();
        bled_set_off();
#endif // BSP_LED_EN
    }
}

AT(.com_text.at_vusb)
void bsp_product_test_process(void)
{
    ///在这里做检测，检测50ms内是否收到product_test_start命令，是的话预留1s做通信
    ///如果1s内收到正确的产测命令则唤醒且无视按键唤醒及充电流程，否则正常跑
    u16 test_start_to = PRODUCT_TEST_START_TO;

    product_test_init();
    while(test_start_to) {
        delay_5ms(1);
        product_test_rx_process();
        test_start_to--;
    }
    if (product_test_str.vusb_start_test_flag) {
        test_start_to = PRODUCT_SET_CONFIG_TO;
        while(test_start_to) {
           delay_5ms(1);
            product_test_rx_process();
            test_start_to--;
            if (product_test_is_sucess()) {
                break;
            }
        }
    }
    if (!QTEST_IS_ENABLE()) {
        product_test_exit();
    }
}

AT(text.huart)
void product_test_huart_init(void)
{
    my_printf("%s %d\n",__func__,__LINE__);
//    memset(&hci_rx_buffer, 0x00, sizeof(hci_rx_buffer));
    memset(&huart0, 0x00, sizeof(huart0));

    huart0.rx_port = HUART_TR_PB3;
    huart0.tx_port = HUART_TR_PB3;
//    huart0.txisr_en = 0;
//    huart0.rxbuf_loop = 1;
    huart0.rxisr_en = 1;
    huart0.txisr_en = 0;
    huart0.rxbuf      = (u8 *)&product_test_str.huart_test_cmd;
    huart0.rxbuf_size =  sizeof(product_test_cmd_t);

    huart_init(&huart0, PRODUCT_TEST_BAUD);
}
#else
uint8_t product_test_is_sucess(void)
{
    return 0;
}
#endif
