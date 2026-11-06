#ifndef _BSP_GPIO_H
#define _BSP_GPIO_H

enum {
    GPIOxSET    =   0,
    GPIOxCLR,
    GPIOx,
    GPIOxDIR,
    GPIOxDE,
    GPIOxFEN,
    GPIOxDRV,
    GPIOxPU,
    GPIOxPD,
    GPIOxPU200K,
    GPIOxPD200K,
    GPIOxPU300,
    GPIOxPD300,
};

typedef struct {
    psfr_t sfr;             //GPIO SFR ADDR
    u8 num;
    u8 type;                //type = 1,高压IO，没有300R的强上下拉电阻。 type = 0, 普通IO, 有内部300R上下拉电阻。
    u16 pin;
} gpio_t;

#define bsp_gpio_cfg_init(x, y)         gpio_cfg_init(x, y)

void gpio_cfg_init(gpio_t *g, u8 io_num);       //根据GPIO number初始化GPIO结构体
void wakeup_disable(void);
u8 get_adc_gpio_num(u8 adc_ch);
void wakeup_gpio_config(u8 io_num, u8 edge, u8 pupd_sel);    //任意GPIO的边沿唤醒配置, 参数edge: 0下降沿, 1上升沿,  参数pupd  0:不开内部上下拉, 1:开内部上拉, 2:开内部下拉
void wakeup_wko_config(void);                   //配置WKO唤醒
u32 wakeup_get_status(void);                    //获取唤醒状态，公共区函数
u32 wakeup_gpio_get_status(u8 io_num);          //获取某个GPIO的唤醒状态
void adcch_io_pu10k_enable(u8 adc_ch);


///PORT Interrupt
#define PORT_INT0_VECTOR            0           //PB0, PE0
#define PORT_INT1_VECTOR            1           //PB1, PE1
#define PORT_INT2_VECTOR            2           //PB2, PE2
#define PORT_INT3_VECTOR            3           //PB3, PE3
#define PORT_INT4_VECTOR            4           //PB4, PE4
#define PORT_INT5_VECTOR            5           //PB5, PE5
#define PORT_INT6_VECTOR            6           //PB6, PE6
#define PORT_INT7_VECTOR            7           //PB7, PE7
#define MAX_PORT_ISR_NUM            8           //最大支持的IO中断个数

void port_irq_register(int irq_num, isr_t isr);
void port_irq_free(int irq_num);

#endif // _BSP_GPIO_H
