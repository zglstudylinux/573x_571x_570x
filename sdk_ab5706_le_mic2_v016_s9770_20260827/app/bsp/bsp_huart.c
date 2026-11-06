#include "include.h"
#include "api.h"

#if HUART_EN && !ADAPTER_HUART_INPUT_EN

AT(.com_huart.text)
void huart_tx_done_cb(void)
{
#if BT_SCO_DUMP_TX_EN || ANC_SW_DUMP_EN
    pcm_dump_tx_done();
#endif
}

AT(.com_huart.text)
void huart_rx_done_cb(void)
{
    if(eq_rx_buf[0] == 0xF5 && eq_rx_buf[1] == 0xA0 && eq_rx_buf[2] == 0xA5 && eq_rx_buf[3] == 0x96 && eq_rx_buf[4] == 0x87 && eq_rx_buf[5] == 0x5A){
		WDT_RST();
		while(1);
	}

#if FUNC_LE_DUT_EN
    huart_hci_cmd_recv(eq_rx_buf, huart_get_rxcnt());
#endif

#if EQ_DBG_IN_UART
    if(bsp_eq_rx_done(eq_rx_buf)){
        return;
    }
#endif
}

void bsp_huart_init(void)
{
    huart_t huart0;
    u32 baud_rate = 1500000;
#if ANC_TEST_TOOL_EN || BT_SCO_DUMP_TX_EN
    baud_rate = 6000000;
#endif
    if (xcfg_cb.huart_sel == HUART_TR_PA7) {
        if (UART0_PRINTF_SEL == PRINTF_PA7) {
            FUNCMCON0 = 0x0f << 8;
        }
    } else if (xcfg_cb.huart_sel == HUART_TR_PB2) {
        if (UART0_PRINTF_SEL == PRINTF_PB2) {
            FUNCMCON0 = 0x0f << 8;
        }
    } else if (xcfg_cb.huart_sel == HUART_TR_PB3) {
        if (UART0_PRINTF_SEL == PRINTF_PB3) {
            FUNCMCON0 = 0x0f << 8;
        }
    }
    if((xcfg_cb.huart_sel == HUART_TR_VUSB)){
         if(!sys_cb.vusb_uart_flag){
            PWRCON0 |= BIT(30);                             //Enable VUSB GPIO
            sys_cb.vusb_uart_flag = 1;
         }else{
             return;
         }
    }
    memset(eq_rx_buf, 0, EQ_BUFFER_LEN);

    memset(&huart0, 0x00, sizeof(huart0));
    huart0.rx_port = xcfg_cb.huart_sel;
    huart0.tx_port = xcfg_cb.huart_sel;
    huart0.rxisr_en = 1;
    huart0.txisr_en = 1;
    huart0.rxbuf   = eq_rx_buf;
    huart0.rxbuf_size = EQ_BUFFER_LEN;

    huart_init(&huart0, baud_rate);
}

#if FUNC_LE_DUT_EN
void bsp_huart_config(uint8_t huart_tx_sel, uint8_t huart_rx_sel, uint32_t baud_rate)
{
    huart_t huart0;

    memset(eq_rx_buf, 0, EQ_BUFFER_LEN);

    memset(&huart0, 0x00, sizeof(huart0));
    huart0.rx_port = huart_rx_sel;
    huart0.tx_port = huart_tx_sel;
    huart0.rxisr_en = 1;
    huart0.txisr_en = 1;
    huart0.rxbuf   = eq_rx_buf;
    huart0.rxbuf_size = EQ_BUFFER_LEN;

    huart_init(&huart0, baud_rate);
}
#endif
#else
void bsp_huart_init(void) {}
#endif

u8* huart_get_rxbuf(u16 *len)
{
    *len  = EQ_BUFFER_LEN;
    return eq_rx_buf;
}

