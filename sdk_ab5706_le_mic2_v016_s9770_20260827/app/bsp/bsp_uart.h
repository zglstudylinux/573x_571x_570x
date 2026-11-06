#ifndef _BSP_UART_H
#define _BSP_UART_H

#define VH_DATA_LEN                     40

#define RC24M_CLK                       24000000

typedef struct {
    u16 header;
    u8  distinguish;                    //厂商识别码
    u8  cmd;
    u8  length;
    u8  buf[VH_DATA_LEN];
    u8  checksum;
    u8  cnt;
    u8  crc;
} vh_packet_t;

typedef struct {
    volatile u8 w_cnt;
    volatile u8 r_cnt;
    u8 resv0;
    u8 resv1;
    u8 *buf;
    u32 ticks;
} uart_cb_t;

typedef struct {
    psfr_t sfr;
    u16 bit;
} uart_map_t;

static const uart_map_t uart1_trmap_tbl[] = {
    [0] = {(psfr_t)(&GPIOASET),     BIT(7)},    //PA7
    [1] = {(psfr_t)(&GPIOFSET),     BIT(0)},    //PF0
    [2] = {NULL,                    BIT(0)},    //vusb
};

static const uart_map_t uart1_rxmap_tbl[] = {
    [0] = {(psfr_t)(&GPIOASET),     BIT(6)},    //PA6
    [1] = {(psfr_t)(&GPIOFSET),     BIT(3)},    //PF3
};

static const uart_map_t uart2_trmap_tbl[] = {
    [0] = {(psfr_t)(&GPIOASET),     BIT(4)},    //PA4
    [1] = {(psfr_t)(&GPIOBSET),     BIT(2)},    //PB2
    [2] = {(psfr_t)(&GPIOESET),     BIT(7)},    //PE7
    [3] = {NULL,                    BIT(0)},    //vusb
};

static const uart_map_t uart2_rxmap_tbl[] = {
    [0] = {(psfr_t)(&GPIOASET),     BIT(3)},    //PA3
    [1] = {(psfr_t)(&GPIOBSET),     BIT(1)},    //PB1
    [2] = {(psfr_t)(&GPIOESET),     BIT(6)},    //PE6
};


extern const u8 vusb_crc8_tbl[256];
void bsp_uart1_init(uint8_t uart_maping,uint8_t double_line_en,u32 baudrate);
void bsp_vusb_uart_dis(void);
u8 bsp_uart1_get(u8 *ch);
u8 bsp_uart_packet_parse(vh_packet_t *p, u8 data);
void bt_get_local_bd_addr(u8 *addr);
void set_vusb_uart_flag(u8 flag);
extern vh_packet_t vh_packet;
void bsp_uart1_tx_putr(u8 *ch , uint8_t len);
isr_t register_isr(int vector, isr_t isr);

void bsp_uart2_init(uint8_t uart_maping,uint8_t double_line_en,u32 baudrate);
u8 bsp_uart2_get(u8 *ch);
void bsp_uart2_tx_putr(u8 *ch , uint8_t len);
#endif // _BSP_UART_H
