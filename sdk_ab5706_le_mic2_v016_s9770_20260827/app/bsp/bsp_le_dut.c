#include "include.h"
#include "bsp_le_dut.h"
#include "api_uart.h"

#if FUNC_LE_DUT_EN

hci_pkt_cb hci_cmd_pkt;
AT(.com_rodata.hci)
const uint8_t hci_reset[4]={0x01, 0x03, 0x0C, 0x00};

void ble_dut_init(void)
{
    //初始化通信HUART
    bsp_huart_config(HUART_TR_PB3, HUART_TR_PB4, 9600);

    memset(&hci_cmd_pkt, 0x00, sizeof(hci_pkt_cb));
    ble_dut_pkt_buf_set(&hci_cmd_pkt);

#if IODM_TEST_MODE
    ble_hci_tx_callback_set(iodm_dut_msg_send);
    iodm_dut_cmd_recv(HCI_RESET_CMD_OPCODE, NULL, 0);
#else
    ble_hci_tx_callback_set(huart_tx);
    huart_hci_cmd_recv(hci_reset, sizeof(hci_reset));    //先执行一条hci_reset命令
#endif
}

AT(.com_text.le.dut)
void huart_hci_cmd_recv(const uint8_t *data, int len)
{
#if IODM_TEST_MODE
    if(IODM_TEST_MODE) {
        bsp_iodm_packet_dma_recv(data);
        return;
    }
#endif

    if(func_cb.sta != FUNC_LE_DUT) {
        return;
    }

    if (len >= RX_BUF_SIZE || len < PACKET_MIN){
        hci_cmd_pkt.status   = PACKET_RECV_ERR ;

    } else if((hci_cmd_pkt.total_size >= RX_ELEMENT_NUM)){
        hci_cmd_pkt.status   = PACKET_BUF_FULL;
    } else {
        memcpy(hci_cmd_pkt.buf[(hci_cmd_pkt.w_idx)&RX_ELEMENT_NUM_MASK], data, len);
        hci_cmd_pkt.len[(hci_cmd_pkt.w_idx)&RX_ELEMENT_NUM_MASK] = len;
        hci_cmd_pkt.status = PACKET_NO_ERR;
        (hci_cmd_pkt.w_idx)++;
        (hci_cmd_pkt.total_size)++;
    }

    msg_enqueue(EVT_HCI_CMD);
}

//#if IODM_TEST_MODE
/////小牛测控输入,补充成完整的HCI CMD
//AT(.com_text.le.dut)
//void iodm_dut_cmd_recv(uint16_t opcode, u8 *param,  u8 len)
//{
//    if(len > 10) {
//        return;
//    }
//
//    u8 *buf = hci_cmd_pkt.buf[(hci_cmd_pkt.w_idx)&RX_ELEMENT_NUM_MASK];
//    buf[0] = HCI_COMMAND_PACKET;
//    buf[1] = opcode&0xFF;
//    buf[2] = (opcode>>8)&0xFF;
//    buf[3] = len;
//    if(param) {
//        memcpy(buf+4, param, len);
//    }
//    hci_cmd_pkt.len[(hci_cmd_pkt.w_idx)&RX_ELEMENT_NUM_MASK] = len+4;
//    hci_cmd_pkt.status = PACKET_NO_ERR;
//    (hci_cmd_pkt.w_idx)++;
//    (hci_cmd_pkt.total_size)++;
//
//    msg_enqueue(EVT_HCI_CMD);
//}
//
/////小牛测控参数反馈,按照event去解析
//AT(.com_text.le.dut)
//void iodm_dut_msg_send(const void *buf, uint len)
//{
//    const uint8_t *param = buf+3;
//    uint8_t status = param[3];
//    uint16_t opcode = (param[2] << 8) | param[1];
//
//    if(opcode == HCI_LE_TEST_END_CMD_OPCODE && status == 0) {
//        iodm_rsp_buf_set((const uint8_t *)&param[4], 2);
//    }
//}
//#endif
#endif // FUNC_LE_DUT_EN
