#ifndef _BSP_LE_DUT_H
#define _BSP_LE_DUT_H

#define PACKET_NO_ERR            0
#define PACKET_BUF_FULL          1
#define PACKET_RECV_ERR          2
#define HCI_COMMAND_PACKET       0x01

#define PACKET_MIN               4

#define RX_ELEMENT_NUM           4
#define RX_ELEMENT_NUM_MASK      (RX_ELEMENT_NUM-1)
#define RX_BUF_SIZE              20

typedef struct {
    uint8_t buf[RX_ELEMENT_NUM][RX_BUF_SIZE];
    uint8_t len[RX_ELEMENT_NUM];
    uint8_t w_idx;
    uint8_t r_idx;
    uint8_t total_size;
    uint8_t status;
} hci_pkt_cb;


//enum {
//    HCI_RESET_CMD_OPCODE                                = 0x0C03,
//    HCI_LE_RX_TEST_V1_CMD_OPCODE                        = 0x201D,
//    HCI_LE_TX_TEST_V1_CMD_OPCODE                        = 0x201E,
//    HCI_LE_RX_TEST_V2_CMD_OPCODE                        = 0x2033,
//    HCI_LE_TX_TEST_V2_CMD_OPCODE                        = 0x2034,
//    HCI_LE_TEST_END_CMD_OPCODE                          = 0x201F,
//
//    HCI_VS_TEST_CTRL_CMD_OPCODE                         = 0xFC0A,
//};

//void iodm_dut_cmd_recv(uint16_t opcode, u8 *param,  u8 len);
//void iodm_dut_msg_send(const void *buf, uint len);
//void iodm_rsp_buf_set(const uint8_t *buf, uint8_t len);

void ble_dut_init(void);
void huart_hci_cmd_recv(const uint8_t *data, int len);

//外部api
void ble_dut_pkt_buf_set(void *buf);
void ble_hci_tx_callback_set(void *handle);
void bsp_huart_config(uint8_t huart_tx_sel, uint8_t huart_rx_sel, uint32_t baud_rate);
#endif
