#ifndef _BSP_UART_COMMAND_H
#define _BSP_UART_COMMAND_H


#define UART_COMMAND_PORT_SEL             GPIO_PORT_GET(GPIO_PA4)
#define UART_COMMAND_PIN_SEL              GPIO_PIN_GET(GPIO_PA4)
#define UART_COMMAND_BAUD                 1500000

void uart2_command_init(void);
void uart2_putchar(char ch);

#endif // _BSP_UART_COMMAND_H
