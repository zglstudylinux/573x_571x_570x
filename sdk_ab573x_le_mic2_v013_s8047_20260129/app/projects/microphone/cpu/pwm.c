#include "include.h"

#if TIMER5_PWM_EN
//Note: The PWM relate register is effect only for Timer5
//Note: 891x 只有Timer pwm

#define TIMER_PWM_PR    (12000000 / 500 - 1) //500hz 时钟源选择xosc_24m并二分频

//PWM0-5 通道IO mapping table
const u8 pwm_gpio_tbl[18] = {
    IO_PB2, IO_PB0, IO_PE4,     //PWM0 G1 G2 G3
    IO_PB3, IO_PB1, IO_PE5,     //PWM1 G1 G2 G3
    IO_PB4, IO_PE0, IO_PE6,     //PWM2 G1 G2 G3
    IO_PB5, IO_PE1, IO_PE7,     //PWM3 G1 G2 G3
    IO_PB6, IO_PE2, IO_NONE,     //PWM4 G1 G2 G3
    IO_PB7, IO_PE3, IO_NONE,     //PWM5 G1 G2 G3
};

void pwm_init(void)
{
    //分别对应pwm0~5对应的IO，IO_NONE表示不使用
    u8 pwm_list[6] = {IO_PB2, IO_PB3, IO_PB4, IO_NONE, IO_NONE, IO_NONE};
    u8 mapping = 0;
    gpio_t pwm_gpio;
    gpio_t *g = &pwm_gpio;

    //开启时钟源
    CLKGAT0 |= BIT(5);
    asm("nop");asm("nop");
    asm("nop");asm("nop");
    TMR5CON |= 0x2 << 1;    //时钟源选择xosc_24m
    TMR5CON |= 0x1 << 4;    //时钟源2分频

    //io初始化以及通道mapping设置
    for (u8 i = 0; i<6; i++) {
        if (pwm_list[i] == pwm_gpio_tbl[i*3 + 0]) {
            mapping = 1;
        } else if (pwm_list[i] == pwm_gpio_tbl[i*3 + 1]) {
            mapping = 2;
        } else if (pwm_list[i] == pwm_gpio_tbl[i*3 + 2]) {
            if ( 4 == i || 5 == i) { //PWM 4 5没有G3复用
                continue;
            }
            mapping = 3;
        } else {
            continue;
        }
        bsp_gpio_cfg_init(g, pwm_list[i]);
        g->sfr[GPIOxDIR] &= ~BIT(g->num);
        g->sfr[GPIOxDE]  |= BIT(g->num);
        g->sfr[GPIOxFEN] |= BIT(g->num);
        FUNCMCON1 = (FUNCMCON1 & ~(0xF << (8 + i*4))) | (mapping << (8 + i*4));
        TMR5CON |= BIT(16 + 2*i);
    }

    //配置PWM频率
    TMR5PR = TIMER_PWM_PR;
    TMR5CNT = 1;

    //配置占空比 默认75%
    TMR5DUTY0 = TIMER_PWM_PR*75/100;
    TMR5DUTY1 = TIMER_PWM_PR*75/100;
    TMR5DUTY2 = TIMER_PWM_PR*75/100;
//    TMR5DUTY3 = TIMER_PWM_PR*75/100;
//    TMR5DUTY4 = TIMER_PWM_PR*75/100;
//    TMR5DUTY5 = TIMER_PWM_PR*75/100;

    //开启timer
    TMR5CON |= BIT(0);
}

//param： 0-100 （%）
AT(.com_text.pwm_duty_set)
void pwm_duty_write(u8 pwm0_duty, u8 pwm1_duty, u8 pwm2_duty)
{
    //边界保护
    if (pwm0_duty > 100) {
        pwm0_duty = 100;
    }
    if (pwm1_duty > 100) {
        pwm1_duty = 100;
    }
    if (pwm2_duty > 100) {
        pwm2_duty = 100;
    }

    //配置占空比
    TMR5DUTY0 = TIMER_PWM_PR*pwm0_duty/100;
    TMR5DUTY1 = TIMER_PWM_PR*pwm1_duty/100;
    TMR5DUTY2 = TIMER_PWM_PR*pwm2_duty/100;
//    TMR5DUTY3 = TIMER_PWM_PR*75/100;
//    TMR5DUTY4 = TIMER_PWM_PR*75/100;
//    TMR5DUTY5 = TIMER_PWM_PR*75/100;

}

void pwm_close(void)
{
    TMR5CON &= ~BIT(0);
}

#endif // TIMER5_PWM_EN


