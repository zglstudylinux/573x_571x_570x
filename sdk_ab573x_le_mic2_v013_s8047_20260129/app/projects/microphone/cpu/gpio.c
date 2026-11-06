#include "include.h"


AT(.text.bsp.sys.init)
void gpio_pwron_init(void)
{
    //全部设置成模拟GPIO，防止漏电。使用时，自行配置对应数字GPIO
    GPIOBDE = 0;
    GPIOEDE = 0;
    GPIOGDE = 0x3F; //MCP FLASH
}

