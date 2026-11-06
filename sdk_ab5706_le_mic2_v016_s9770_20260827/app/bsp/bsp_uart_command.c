#include "include.h"
#include "bsp_uart_command.h"
#include "bsp_uart.h"
#include "wireless_coexist.h"

#if ADAPTER_UART_COMMAND_EN

AT(.com_text.isr)
static void uart2_isr(void)
{
    if(UART2CON & BIT(9)) {
        uint16_t data = UART2DATA;
        UART2CPND = BIT(9);
        wireless_coexist_uart_isr(data);
    }
}

AT(.com_text.uart2)
void uart2_putchar(char ch)
{
    UART2DATA = ch;
    while (!(UART2CON & BIT(8)));
    UART2CPND |= BIT(8);
}

void uart2_command_init(void)
{
    const uart_map_t *hmap;
    UART2CON = 0;
    u32 baud = 0;
    u8 uart_maping = 0;                                       //配置PA4

    hmap = &uart2_trmap_tbl[uart_maping];                  //TX
    hmap->sfr[GPIOxDE] |= hmap->bit;
    hmap->sfr[GPIOxSET] |= hmap->bit;
    hmap->sfr[GPIOxDIR] |= hmap->bit;
    hmap->sfr[GPIOxFEN] |= hmap->bit;
    hmap->sfr[GPIOxPU] |= hmap->bit;

    UART2CON |= BIT(6);                                         //one-line
    FUNCMCON2 = (0x7 << 12) | ((uart_maping + 1) << 8);         //when RX = 0X7, TX RX共用一个IO


    CLKGAT0 |= BIT(8);                                                  //enable uart2 clk
    CLKCON1 |= (0x2 << 23);                                             //uart2 clk sel xosc_clk
    baud = ((RC24M_CLK + (UART_COMMAND_BAUD / 2)) / UART_COMMAND_BAUD) - 1;
    UART2BAUD = (baud << 16) | baud;
    UART2CON |= BIT(7) | BIT(5) | BIT(4) | BIT(0);              //RX EN, fix baud, 2 Stop bits, Uart EN
    UART2CPND = BIT(8) | BIT(9);

    UART2CON |= BIT(2);
    UART2CPND = BIT(9);
    register_isr(IRQ_UART_VECTOR, uart2_isr);
    PICPR &= ~BIT(IRQ_UART_VECTOR);
	PICEN |= BIT(IRQ_UART_VECTOR);
}
#endif

