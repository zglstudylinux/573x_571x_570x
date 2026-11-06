#include "include.h"
#include "huart_audio_out.h"
#include "api_uart.h"
/*
 * 文件名称: huart_audio_out.c
 * 功能描述: 本文件为HUART DMA 输出数字信号 处理模块
    开启本功能后，在线eq调试会失效哦~
 ****************************************************************************************
    code：
    buf:
    time :
 */
#if ADAPTER_HUART_OUTPUT_EN
#define HUART_AUDIO_BAUD                4000000          //haurt波特率
#define HUART_SAMPLES                   120*2            //16bit = 8bit * 2
#define HUART_TX_PORT_DEF               HUART_TR_PE6     //huart发射引脚
#define HUART_RX_PORT_DEF               HUART_TR_PE7     //huart接收引脚
#define HUART_OUT_CACHE_SIZE            HUART_SAMPLES

static huart_tx_str_t huart_tx_str  AT(.data.huart_cb);
static huart_audio_out_cfg_t huart_audio_out_cfg AT(.buf.huart);
static u8 huart_buffer_tx[HUART_SAMPLES] AT(.buf.huart);
static u8 huart_buffer_tx_cache[HUART_SAMPLES] AT(.buf.huart);
static ring_buf_t huart_ring_buf AT(.buf.huart);
static bool huart_audio_out_flag  = false;

static const rbuf_tbl_t huart_buf_tbl[1] = {
    {
        .buf = (u8 *)&huart_buffer_tx,
        .size = HUART_OUT_CACHE_SIZE,
    },
};

AT(.com_text.huart)
bool huart_role_audio_out(void)
{
    return huart_audio_out_flag;
}
uint8_t test[] = "aaaaa";
//当断开连接时,可用该接口发送命令
AT(.text.huart_proc.input)
void huart_cmd_send(void)
{
    //获取uart_command是否有命令要发
    if (uart_command_tx_value_get()) {
        memset(huart_tx_str.tx_buf, 24, HUART_SAMPLES);
        huart_tx_str.tx_buf_len = HUART_SAMPLES + 1;
        u8 uart_command_buf_len = uart_command_buf_len_get();
        huart_tx_str.tx_type = TX_TYPE_AUDIO_CMD;

        memcpy(huart_tx_str.tx_buf + huart_tx_str.tx_buf_len-1, uart_command_buf_get(), uart_command_buf_len);//uart_command_buf_get

        huart_tx_str.tx_buf_len += (uart_command_buf_len);
        huart_tx((u8 *)&huart_tx_str, huart_tx_str.tx_buf_len);
    }
}

AT(.com_text.huart.input)
void huart_audio_out_input(u8 *ptr, u32 samples, int ch_mode, void *param)
{
    if(!huart_audio_out_cfg.mute){

        if (ring_buf_get(&huart_ring_buf, huart_tx_str.tx_buf, HUART_SAMPLES)) {
            huart_wait_txdone();
            huart_tx_str.tx_buf_len = samples*2 + 1;
#if ADAPTER_HUART_COMMAND_EN
            if (uart_command_tx_value_get()) {              //获取uart_command是否有命令要发
                u8 uart_command_buf_len = uart_command_buf_len_get();
                huart_tx_str.tx_type = TX_TYPE_AUDIO_CMD;
                memcpy(huart_tx_str.tx_buf + huart_tx_str.tx_buf_len-1, uart_command_buf_get(), uart_command_buf_len);//uart_command_buf_get
                huart_tx_str.tx_buf_len += (uart_command_buf_len);
                huart_tx((u8 *)&huart_tx_str, huart_tx_str.tx_buf_len);
            }
#endif
            huart_tx(huart_tx_str.tx_buf, huart_tx_str.tx_buf_len);
        }
        ring_buf_put(&huart_ring_buf,ptr,samples * 2 * ch_mode);
        if (huart_audio_out_cfg.callback) {
            huart_audio_out_cfg.callback(ptr, samples, ch_mode, param);
        }
    }
}

AT(.text.huart.callback)
void huart_audio_out_output_callback_set(audio_callback_t callback)
{
    huart_audio_out_cfg.callback = callback;
}

AT(.text.huart.mute)
void huart_audio_out_mute_set(uint8_t mute)
{
    huart_audio_out_cfg.mute = mute;
}

AT(.text.huart.init)
void huart_audio_out_init(u8 sample_rate, u16 samples, u8 channel)
{
    huart_t huart0;
    memset(&huart_audio_out_cfg,0,sizeof(huart_audio_out_cfg));
    memset(&huart0, 0x00, sizeof(huart0));
    ring_buf_init(&huart_ring_buf, &huart_buf_tbl[0], 1, 0);
    huart0.txisr_en = 1;
    //如果在setting使能了HUART音频功能,则在setting配置文件决定是否进行HUART初始化
    if (xcfg_cb.huart_audio_out_en) {
        if(xcfg_cb.huart_out_sel <= HUART_TR_VUSB) {

            huart_audio_out_flag = true;
            //HUART串口选择: PA6: 0, PA7: 1, PB1: 2, PB2: 3, PB3: 4, PB4: 5, PE6: 6, PE7: 7 PF3:8 VUSB:9
            huart0.rx_port = xcfg_cb.huart_out_sel;
            huart0.tx_port = xcfg_cb.huart_out_sel;
            huart_init(&huart0, HUART_AUDIO_BAUD);

        } else {
            //NONE
            printf("please select huart audio output pin....!!!!\n");
            huart_audio_out_mute_set(1);
        }
    } else {
        huart0.rx_port = HUART_RX_PORT_DEF;
        huart0.tx_port = HUART_TX_PORT_DEF;
        huart_audio_out_flag = true;
        huart_init(&huart0, HUART_AUDIO_BAUD);
    }
}

AT(.text.huart.exit)
void huart_audio_out_exit(void)
{

}
#endif // I2S_TX_AUDIO_EN

