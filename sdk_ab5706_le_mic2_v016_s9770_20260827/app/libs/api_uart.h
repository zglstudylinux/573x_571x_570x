#ifndef __API_UART_H__
#define __API_UART_H__

enum {
    HUART_TR_PA6    = 0,
    HUART_TR_PA7,
    HUART_TR_PB1,
    HUART_TR_PB2,
    HUART_TR_PB3,
    HUART_TR_PB4,
    HUART_TR_PE6,
    HUART_TR_PE7,
    HUART_TR_PF3,
    HUART_TR_VUSB,
};

typedef struct {
    psfr_t sfr;
    u16 bit;
} huart_map_t;

typedef struct {
    union {
        struct {
            uint8_t  tx_port : 4;
            uint8_t  rx_port : 4;
            uint8_t  rxisr_en : 1;
            uint8_t  txisr_en : 1;
            uint8_t  rxbuf_loop : 1;
            uint8_t  tx_1st : 1;
        };
        uint16_t all_setting;
    };
    uint16_t rxbuf_size;
    uint8_t *rxbuf;
    huart_map_t huart_map;
} huart_t;

void huart_init(huart_t *huart, uint32_t baud_rate);
void huart_exit(void);
void huart_set_baudrate(uint baudrate);
void huart_putchar(const char ch);
void huart_tx(const void *buf, uint len);
uint huart_get_rxcnt(void);
void huart_rxfifo_clear(void);
char huart_getchar(void);


//UART
typedef void (*isr_t)(void);
void uart1_register_isr(isr_t isr);

//
void uart2_key_mode(void);
void uart2_register_isr(isr_t isr);
#endif // __API_UART_H__
