#include "include.h"

#if 0

//GPIO中断示例

AT(.com_text.port)
void port_isr(void)
{
    //uart_putchar('$');
}

//测试OK
void port_int_test(void)
{
    wakeup_gpio_config(IO_PE3, 0, 1);                   //配置PE3开内部上拉，下降沿
    port_irq_register(PORT_INT3_VECTOR, port_isr);      //注册PE3的PORT中断
}

#endif
