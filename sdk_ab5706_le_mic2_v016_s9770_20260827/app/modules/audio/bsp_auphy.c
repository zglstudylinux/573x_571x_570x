#include "include.h"

#if FPGA_EN

static u8 fuart_txbuf[48];
static const char str_phy_cmd_mic_ch[] = "#AUAMM\x00\x00*";
static const char str_phy_cmd_mic_gain[] = "#AUAMV\x00\x00\x00*";

AT(.text.fpga)
void fpga_uart_putchar(u8 auphy, char ch)
{
    if (auphy & BIT(0)) {
        while (!(FPGAUTCON & BIT(8)));
        FPGAUTDATA = ch;
    }
    if (auphy & BIT(1)) {
        while (!(FPGAUTCON2 & BIT(8)));
        FPGAUTDAT2 = ch;
    }
}

AT(.text.fpga)
void fpga_uart_putcs(u8 auphy, const void *buf, uint len)
{
    const char *txbuf = buf;
    while (len--) {
        fpga_uart_putchar(auphy, *txbuf++);
    }
}

////切换AUX, MIC功能
//AT(.text.fpga)
//void bsp_auphy_adc_mode(u8 auphy, u8 mode)
//{
//    fpga_uart_putchar(auphy, '#');
//    fpga_uart_putchar(auphy, 'A');
//    fpga_uart_putchar(auphy, 'U');
//    fpga_uart_putchar(auphy, 'M');
//    if (mode == 1) {
//        fpga_uart_putchar(auphy, 'A');
//    } else if (mode == 2) {
//        //左AUX单进双出
//        fpga_uart_putchar(auphy, 'B');
//    } else if (mode == 3) {
//        //右AUX单进双出
//        fpga_uart_putchar(auphy, 'C');
//    } else if (mode == 4) {
//        //右MIC
//        fpga_uart_putchar(auphy, 'R');
//    } else if (mode == 5) {
//        //双MIC
//        fpga_uart_putchar(auphy, 'S');
//    } else {
//        //左MIC
//        fpga_uart_putchar(auphy, 'M');
//    }
//    fpga_uart_putchar(auphy, '*');
//}
//
////开启VCMBUF
//AT(.text.fpga)
//void bsp_auphy_vcmbuf_en(void)
//{
//    u8 auphy = 1;
//    fpga_uart_putchar(auphy, '#');
//    fpga_uart_putchar(auphy, 'A');
//    fpga_uart_putchar(auphy, 'U');
//    fpga_uart_putchar(auphy, 'D');
//    fpga_uart_putchar(auphy, 'V');
//    fpga_uart_putchar(auphy, '1');
//    fpga_uart_putchar(auphy, '*');
//}

//vol：0~16
void bsp_auphy_set_dac_volume(u8 vol)
{
    u8 auphy = 1;
    fpga_uart_putchar(auphy, '#');
    fpga_uart_putchar(auphy, 'A');
    fpga_uart_putchar(auphy, 'U');
    fpga_uart_putchar(auphy, 'V');
    fpga_uart_putchar(auphy, 'V');  //多空了1byte
    fpga_uart_putchar(auphy, 'V');
    fpga_uart_putchar(auphy, vol);
    fpga_uart_putchar(auphy, '*');
}

void bsp_auphy_set_mic_analog_gain(u16 channel, u8 gain)
{
    u16 phy_ch = 0;
    u8 micn = channel & 0xf;
    u8 micm = (channel >> 8) & 0xf;

    phy_ch |= micn > 0? ((0x0c << ((micn - 1) % 2) * 4)) : 0;   //对应queen phy的mic channel
    phy_ch |= micm > 0? ((0x0c << ((micm - 1) % 2) * 4)) : 0;

    u8 cmd_len = sizeof(str_phy_cmd_mic_gain) - 1;
    memcpy(fuart_txbuf, str_phy_cmd_mic_gain, cmd_len);
    fuart_txbuf[6] = phy_ch >> 8;
    fuart_txbuf[7] = phy_ch >> 0;
    fuart_txbuf[8] = gain;
    fpga_uart_putcs(1, fuart_txbuf, cmd_len);
}

void audio_fpag_setup(u16 channel)
{
//    printf("channel: %x\n", channel);
    u8 cmd_len = sizeof(str_phy_cmd_mic_ch) - 1;
    memcpy(fuart_txbuf, str_phy_cmd_mic_ch, cmd_len);
    fuart_txbuf[6] = 0x0c;
    fuart_txbuf[7] = 0xcc;

    fpga_uart_putcs(1, fuart_txbuf, cmd_len);
}

//AT(.text.fpga)
//void fpga_uart_init(void)
//{
//    u32 uart_baud = 0;
//    u32 sys_clk = get_sysclk_nhz();
//    uart_baud = (((sys_clk + (115200 / 2)) / 115200) - 1);
//    //audio phy 1
//    FPGAUTBAUD = (uart_baud << 16) | uart_baud;
//    FPGAUTCON |= BIT(7) | BIT(0);
//
//    //audio phy 2
//    FPGAUTBAUD2 = (uart_baud << 16) | uart_baud;
//    FPGAUTCON2 |= BIT(7) | BIT(0);
//
//    //printf("FPGAUTBAUD = %x, FPGAUTBAUD2 = %x\n",FPGAUTBAUD, FPGAUTBAUD2);
//
//    fpga_uart_putcs(3, "#RESET*", 7);
//}

void fpga_uart_init (void)
{
    uint32_t uart_baud;

    u32 sys_clk = get_sysclk_nhz();
    uart_baud = (sys_clk + 115200/2) / 115200 - 1;

    FPGAUTBAUD  = (uart_baud << 16) | uart_baud;
    FPGAUTCON  |= BIT(7) | BIT(0);

//    FPGAUTBAUD2 = (uart_baud << 16) | uart_baud;
//    FPGAUTCON2 |= BIT(7) | BIT(0);
}

#define CHANNEL_M               0x0F00
#define CHANNEL_N               0xF000
#define CH_MICL0                0x0C    /* MICL       -> left mic */
#define CH_MICR0                0xC0    /* MICR       -> right mic */
#define CH_MICM0                0xC00   /* MICM       -> adcm mic */

void bsp_auphy_adc_mode_test (uint8_t auphy, uint16_t mode)
{
    fpga_uart_putchar (auphy, '#');
    fpga_uart_putchar (auphy, 'A');
    fpga_uart_putchar (auphy, 'U');
    fpga_uart_putchar (auphy, 'A');
    fpga_uart_putchar (auphy, 'M');
    fpga_uart_putchar (auphy, 'M');
    fpga_uart_putchar (auphy, (mode>>8) & 0xff );
    fpga_uart_putchar (auphy,  mode & 0xff     );
    fpga_uart_putchar (auphy, '*');
    delay_us(10000);
}

void audio_fpga_setup (uint16_t channel, uint8_t spr)
{
    // auphy1配置
    if ((channel & CHANNEL_L) || (channel & CHANNEL_R) || (channel & CHANNEL_M)) {
        if        (channel == CH_MICL0) {
            // 配置左MIC
            FPGATEST |= BIT(8);         // 这个BIT修复phy MIC gain配置异常
            bsp_auphy_adc_mode_test (1, CH_MICL0);
        } else if (channel == CH_MICR0) {
            // 配置右MIC
            FPGATEST |= BIT(8);
            bsp_auphy_adc_mode_test (1, CH_MICR0);
        } else if (channel == CH_MICR0) {
            // 配置MICM
            FPGATEST |= BIT(8);
            bsp_auphy_adc_mode_test (1, CH_MICM0);
        } else if (channel == (CH_MICL0 | CH_MICR0)) {
            // 配置双MIC LR
            FPGATEST |= BIT(8);
            bsp_auphy_adc_mode_test (1, CH_MICL0 | CH_MICR0);
        } else if (channel == (CH_MICL0 | CH_MICR0)) {
            // 配置双MIC LM
            FPGATEST |= BIT(8);
            bsp_auphy_adc_mode_test (1, CH_MICL0 | CH_MICM0);
        } else if (channel == (CH_MICL0 | CH_MICR0)) {
            // 配置双MIC RM
            FPGATEST |= BIT(8);
            bsp_auphy_adc_mode_test (1, CH_MICM0 | CH_MICR0);
        } else if (channel == (CH_MICL0 | CH_MICR0 | CH_MICM0)) {
            // 配置三MIC
            FPGATEST |= BIT(8);
            bsp_auphy_adc_mode_test (1, CH_MICM0 | CH_MICL0 | CH_MICR0);
        } else if (channel == CHANNEL_L) {
            // 左SDADC单进双出
            bsp_auphy_adc_mode_test (1, 2);
        }
    }
}

#endif
