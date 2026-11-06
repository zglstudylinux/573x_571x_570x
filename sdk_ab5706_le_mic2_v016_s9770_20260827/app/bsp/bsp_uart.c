#include "include.h"


#if UART1_EN || USER_UART1_EN

static uart_cb_t uart_cb;
static u8 uart_rx_buf[64];
uint8_t txrx_mux_port = 0;

AT(.com_rodata.bat)
const char uart1_data[] = "uart1_data: 0x%X\n";

AT(.com_text.uart1.isr)
static void uart1_rx_done_cb(u8 data)
{
#if QTEST_EN && (TEST_INTF_SEL == INTF_UART1)
    if(QTEST_IS_ENABLE()) {
        qtest_packet_uart1_recv(data);
    }
#endif

//    printf(uart1_data,data);
}

AT(.com_text.uart1.isr)
static void uart1_isr(void)
{
    u8 data;
    if(UART1CON & BIT(9)) {
        data = UART1DATA;
        UART1CPND = BIT(9);
        if(tick_check_expire(uart_cb.ticks,50)){
            uart_cb.w_cnt = uart_cb.r_cnt = 0;
        }
        uart_cb.ticks = tick_get();
        uart_cb.buf[uart_cb.w_cnt & 0x3f] = data;
        uart_cb.w_cnt++;
        uart1_rx_done_cb(data);
    }
}

///发送一个字符
AT(.com_text.uart1.send)
void bsp_uart1_puts(uint8_t ch)
{
    UART1DATA = ch;
    while (!(UART1CON & BIT(8)));
    UART1CPND |= BIT(8);
}

///操作单位为一个字符串,固定长度
AT(.com_text.uart1.send)
void bsp_uart1_tx_putr(u8 *ch , uint8_t len)
{
    const uart_map_t *hmap;
    if (UART1CON & BIT(6)){
        hmap = &uart1_trmap_tbl[txrx_mux_port];
        if (!sys_cb.vusb_uart_flag){
            hmap->sfr[GPIOxDIR] &=~ hmap->bit;
        } else {
            PWRCON0 |= BIT(29);                              //VUSB as GPIO output enable
        }
    }

    for (uint8_t i = 0; i < len; i++){
        bsp_uart1_puts((uint8_t) *ch++);
    }

    if (UART1CON & BIT(6)){
        if (!sys_cb.vusb_uart_flag){
            hmap->sfr[GPIOxDIR] |= hmap->bit;
        } else {
            PWRCON0 &= ~BIT(29);                             //VUSB as GPIO output disenable
        }
    }
}

AT(.com_text1.uart)
u8 bsp_uart1_get(u8 *ch)
{
    if (uart_cb.r_cnt != uart_cb.w_cnt) {
        *ch = uart_cb.buf[uart_cb.r_cnt & 0x3f];
        uart_cb.r_cnt++;
        return 1;
    }
    return 0;
}

AT(.text.uart1.isr)
void uart1_register_isr(isr_t isr)
{
    UART1CON |= BIT(2);
    UART1CPND = BIT(9);
    register_isr(IRQ_UART_VECTOR, isr);
    PICPR &= ~BIT(IRQ_UART_VECTOR);
	PICEN |= BIT(IRQ_UART_VECTOR);
}

void bsp_uart1_init(uint8_t uart_maping,uint8_t double_line_en,u32 baudrate)
{
    memset(&uart_cb, 0, sizeof(uart_cb));
    const uart_map_t *hmap;
    uart_cb.buf = uart_rx_buf;
    UART1CON = 0;
    u32 baud = 0;
    uart_maping -= 1;                                               //从0开始排序

    if (double_line_en){                                            //是否使能双线模式
        hmap = &uart1_trmap_tbl[uart_maping];                       //TX
        hmap->sfr[GPIOxDE] |= hmap->bit;
        hmap->sfr[GPIOxSET] |= hmap->bit;
        hmap->sfr[GPIOxDIR] &= ~hmap->bit;
        hmap->sfr[GPIOxFEN] |= hmap->bit;

        hmap = &uart1_rxmap_tbl[uart_maping];                       //RX
        hmap->sfr[GPIOxPU] |= hmap->bit;
        hmap->sfr[GPIOxDE] |= hmap->bit;
        hmap->sfr[GPIOxSET] |= hmap->bit;
        hmap->sfr[GPIOxDIR] |= hmap->bit;
        hmap->sfr[GPIOxFEN] |= hmap->bit;

        FUNCMCON0 = (((uart_maping) + 1) << 28) | ((uart_maping + 1) << 24);
    } else {
        if (uart_maping == UTX1MAP_G3){
            PWRCON0 |= BIT(30);                                    //VUSB as GPIO
            PWRCON0 &= ~BIT(29);                                   //VUSB output disenable
            sys_cb.vusb_uart_flag = 1;
        } else {
            hmap = &uart1_trmap_tbl[uart_maping];                  //TX
            hmap->sfr[GPIOxDE] |= hmap->bit;
            hmap->sfr[GPIOxSET] |= hmap->bit;
            hmap->sfr[GPIOxDIR] |= hmap->bit;
            hmap->sfr[GPIOxFEN] |= hmap->bit;
            hmap->sfr[GPIOxPU] |= hmap->bit;
        }

        txrx_mux_port = uart_maping;
        UART1CON |= BIT(6);                                         //one-line
        FUNCMCON0 = (0x7 << 28) | ((uart_maping + 1) << 24);        //when RX = 0X7, TX RX共用一个IO
    }

    baud = ((RC24M_CLK + (baudrate / 2)) / baudrate) - 1;
    CLKGAT0 |= BIT(7);                                              //enable uart1 clk
    UART1BAUD = (baud << 16) | baud;
    UART1CON |= BIT(7) | BIT(5) | BIT(4) | BIT(0);                  //RX EN, fix baud, 2 Stop bits, Uart EN
    UART1CPND = BIT(8) | BIT(9);
    uart1_register_isr(uart1_isr);
}

#endif //UART1_EN

#if UART2_EN || USER_UART2_EN


static uart_cb_t uart2_cb;
static u8 uart2_rx_buf[64];
uint8_t txrx_mux_port = 0;

//AT(.com_rodata.bat)
//const char uart2_data[] = "uart2_data: 0x%X\n";

AT(.com_text.uart2.isr)
static void uart2_rx_done_cb(u8 data)
{
//    printf(uart2_data,data);
}

AT(.com_text.uart2.isr)
static void uart2_isr(void)
{
    u8 data;
    if(UART2CON & BIT(9)) {
        data = UART2DATA;
        UART2CPND = BIT(9);
        if(tick_check_expire(uart2_cb.ticks,50)){
            uart2_cb.w_cnt = uart2_cb.r_cnt = 0;
        }
        uart2_cb.ticks = tick_get();
        uart2_cb.buf[uart2_cb.w_cnt & 0x3f] = data;
        uart2_cb.w_cnt++;
        uart2_rx_done_cb(data);
    }
}

///发送一个字符
AT(.com_text.uart2.send)
void bsp_uart2_puts(uint8_t ch)
{
    UART2DATA = ch;
    while (!(UART2CON & BIT(8)));
    UART2CPND |= BIT(8);
}

///操作单位为一个字符串,固定长度
AT(.com_text.uart2.send)
void bsp_uart2_tx_putr(u8 *ch , uint8_t len)
{
    const uart_map_t *hmap;
    if (UART2CON & BIT(6)){
        hmap = &uart2_trmap_tbl[txrx_mux_port];
        if (!sys_cb.vusb_uart_flag){
            hmap->sfr[GPIOxDIR] &=~ hmap->bit;
        } else {
            PWRCON0 |= BIT(29);                              //VUSB as GPIO output enable
        }
    }

    for (uint8_t i = 0; i < len; i++){
        bsp_uart2_puts((uint8_t) *ch++);
    }

    if (UART2CON & BIT(6)){
        if (!sys_cb.vusb_uart_flag){
            hmap->sfr[GPIOxDIR] |= hmap->bit;
        } else {
            PWRCON0 &= ~BIT(29);                             //VUSB as GPIO output disenable
        }
    }
}

AT(.com_text2.uart)
u8 bsp_uart2_get(u8 *ch)                                     //uart2接收数据获取
{
    if (uart2_cb.r_cnt != uart2_cb.w_cnt) {
        *ch = uart2_cb.buf[uart2_cb.r_cnt & 0x3f];
        uart2_cb.r_cnt++;
        return 1;
    }
    return 0;
}

AT(.text.uart2.isr)
void uart2_register_isr(isr_t isr)
{
    UART2CON |= BIT(2);
    UART2CPND = BIT(9);
    register_isr(IRQ_UART_VECTOR, isr);
    PICPR &= ~BIT(IRQ_UART_VECTOR);
	PICEN |= BIT(IRQ_UART_VECTOR);
}

void bsp_uart2_init(uint8_t uart_maping,uint8_t double_line_en,u32 baudrate)
{
    memset(&uart2_cb, 0, sizeof(uart2_cb));
    const uart_map_t *hmap;
    uart2_cb.buf = uart2_rx_buf;
    UART2CON = 0;
    u32 baud = 0;
    uart_maping -= 1;                                               //从0开始排序

    if (double_line_en){                                            //是否使能双线模式
        hmap = &uart2_trmap_tbl[uart_maping];                       //TX
        hmap->sfr[GPIOxDE] |= hmap->bit;
        hmap->sfr[GPIOxSET] |= hmap->bit;
        hmap->sfr[GPIOxDIR] &= ~hmap->bit;
        hmap->sfr[GPIOxFEN] |= hmap->bit;

        hmap = &uart2_rxmap_tbl[uart_maping];                       //RX
        hmap->sfr[GPIOxPU] |= hmap->bit;
        hmap->sfr[GPIOxDE] |= hmap->bit;
        hmap->sfr[GPIOxSET] |= hmap->bit;
        hmap->sfr[GPIOxDIR] |= hmap->bit;
        hmap->sfr[GPIOxFEN] |= hmap->bit;

        FUNCMCON2 = (((uart_maping) + 1) << 12) | ((uart_maping + 1) << 8); //G3 RX->12~15bit , TX->8~11bit
    } else {
        if (uart_maping == UTX2MAP_G4){
            PWRCON0 |= BIT(30);                                    //VUSB as GPIO
            PWRCON0 &= ~BIT(29);                                   //VUSB output disenable
            sys_cb.vusb_uart_flag = 1;
        } else {
            hmap = &uart2_trmap_tbl[uart_maping];                  //TX
            hmap->sfr[GPIOxDE] |= hmap->bit;
            hmap->sfr[GPIOxSET] |= hmap->bit;
            hmap->sfr[GPIOxDIR] |= hmap->bit;
            hmap->sfr[GPIOxFEN] |= hmap->bit;
            hmap->sfr[GPIOxPU] |= hmap->bit;
        }

        txrx_mux_port = uart_maping;
        UART2CON |= BIT(6);                                         //one-line
        FUNCMCON2 = (0x7 << 12) | ((uart_maping + 1) << 8);         //when RX = 0X7, TX RX共用一个IO
    }

    CLKGAT0 |= BIT(8);                                                  //enable uart2 clk
    CLKCON1 |= (0x2 << 23);                                             //uart2 clk sel xosc_clk
    baud = ((RC24M_CLK + (baudrate / 2)) / baudrate) - 1;
    UART2BAUD = (baud << 16) | baud;
    UART2CON |= BIT(7) | BIT(5) | BIT(4) | BIT(0);              //RX EN, fix baud, 2 Stop bits, Uart EN
    UART2CPND = BIT(8) | BIT(9);
    uart2_register_isr(uart2_isr);
}
#endif //UART2_EN

void set_vusb_uart_flag(u8 flag)
{
   sys_cb.vusb_uart_flag = flag;
}

void bsp_vusb_uart_dis(void)
{
    PWRCON0 &= ~BIT(30);                            //disable VUSB GPIO
//    UART1CON = 0;
    CLKGAT0 &= ~BIT(7);                             //disable uart1 clk
    sys_cb.vusb_uart_flag = 0;
}
