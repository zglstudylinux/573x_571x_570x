#include "include.h"
/*
 * 文件名称: bsp_ledc.c
 * 功能描述: 本文件为硬件LEDC处理模块

    一个灯珠含有的三原色分别用三个 uint8 表示
    |R  G  B | R  G  B | R  G  B | R  G  B |
     u8 u8 u8  u8 u8 u8  u8 u8 u8  u8 u8 u8

     按照顺序填充ledc_dma_buf内容然后kick硬件ledc模块即可驱动RGB灯
 ****************************************************************************************
    code + rodata :
    buf           :
    time          :
 */

#if LEDC_HW_EN

static rgb_ledc_cfg_t rgb_ledc_cfg;
static u8 ledc_dma_buf[LEDC_RGB_NUM*3];
static color_t color_2;

AT(.com_text.ledc)
void ledc_dma_kick(uint32_t addr, uint16_t len)
{
    LEDCADDR = addr;
    LEDCCNT = (0x01 << 31) | len;
}

AT(.com_text.ledc)
void ledc_kick(void)
{
    LEDCCON |= BIT(1);
}

AT(.com_text.ledc)
void ledc_irq_handler(void)
{
    if (LEDCPEND & BIT(0)) {
        LEDCPEND |= BIT(0);
//        ledc_dma_kick((u32)ledc_dma_buf, LEDC_RGB_NUM*3);
//        ledc_kick();
    }
}

void bsp_ledc_timing_cfg(ledc_timing_t *ledc_tim_cfg)
{
    LEDCCON |= (ledc_tim_cfg->baud << 16);
    LEDCCON |= (ledc_tim_cfg->delay << 8);

    LEDCTIX = 0;
    LEDCTIX |= (ledc_tim_cfg->code_1_high << 24);
    LEDCTIX |= ((ledc_tim_cfg->baud - ledc_tim_cfg->code_1_high) << 16);
    LEDCTIX |= (ledc_tim_cfg->code_0_high << 8);
    LEDCTIX |= (ledc_tim_cfg->baud - ledc_tim_cfg->code_0_high);

    LEDCRSTX = 0;
    LEDCRSTX = ((ledc_tim_cfg->reset_low << 16) | ledc_tim_cfg->reset_high);
}

void bsp_ledc_init(void)
{
    memset(&rgb_ledc_cfg,0,sizeof(rgb_ledc_cfg_t));

    ledc_timing_t ledc_timing_def;
    CLKCON2 = (CLKCON2 & ~(3 << 3)) | (CLK_LEDC_CLK_XOSC24M << 3);     //LEDC clk select
    CLKGAT1 |= BIT(8);                                //LEDC clk enable
    FUNCMCON2 = 0xf << 24;

#if (LEDC_MAPPING == LEDCMAP_PA7)
    GPIOADE |= BIT(7);
    GPIOADIR &= ~BIT(7);
    GPIOAPU |= BIT(7);
    GPIOAFEN |= BIT(7);
    FUNCMCON2 = LEDCMAP_PA7;
#elif (LEDC_MAPPING == LEDCMAP_PB2)
    GPIOBDE |= BIT(2);
    GPIOBDIR &= ~BIT(2);
    GPIOBPU |= BIT(2);
    GPIOBFEN |= BIT(2);
    FUNCMCON2 = LEDCMAP_PB2;
#elif (LEDC_MAPPING == LEDCMAP_PB4)
    GPIOBDE |= BIT(4);
    GPIOBDIR &= ~BIT(4);
    GPIOBPU |= BIT(4);
    GPIOBFEN |= BIT(4);
    FUNCMCON2 = LEDCMAP_PB4;
#elif (LEDC_MAPPING == LEDCMAP_PE7)
    GPIOEDE |= BIT(7);
    GPIOEDIR &= ~BIT(7);
    GPIOEPU |= BIT(7);
    GPIOEFEN |= BIT(7);
    FUNCMCON2 = LEDCMAP_PE7;
#elif (LEDC_MAPPING == LEDCMAP_PF0)
    GPIOFDE |= BIT(0);
    GPIOFDIR &= ~BIT(0);
    GPIOFPU |= BIT(0);
    GPIOFFEN |= BIT(0);
    FUNCMCON2 = LEDCMAP_PF0;
#elif (LEDC_MAPPING == LEDCMAP_PF3)
    GPIOFDE |= BIT(3);
    GPIOFDIR &= ~BIT(3);
    GPIOFPU |= BIT(3);
    GPIOFFEN |= BIT(3);
    FUNCMCON2 = LEDCMAP_PF3;
#endif

    LEDCCON = 0;
    ledc_timing_def.baud = 30 - 1;         // period = 1 / (clk_24M / 30) = 1.25us
    ledc_timing_def.delay = 0;             //led - led delay
    ledc_timing_def.code_0_high = 8;       // 1.25us / 30 * 8 = 333ns
    ledc_timing_def.code_1_high = 22;      // 1.25us / 30 * 22 = 917ns
    ledc_timing_def.reset_high = 10;
    ledc_timing_def.reset_low = 300;        //1.25us * 300 = 375us
    bsp_ledc_timing_cfg(&ledc_timing_def);

    LEDCFD = LEDC_RGB_NUM;
    LEDCLP = 0;

    LEDCCON |= BIT(5);  //idle high
    LEDCCON |= (BIT(25) | BIT(4) | (0x01 << 2));    //inv one byte
    sys_irq_init(IRQ_IRRX_VECTOR, 0, ledc_irq_handler);
    LEDCCON |= (BIT(6) | BIT(0));      //ledc int en, ledc en
}

void bsp_ledc_start(void)
{
    LEDCCON |= BIT(0);                //ledc en
    ledc_dma_kick((u32)ledc_dma_buf, LEDC_RGB_NUM*3);
    ledc_kick();
}

void bsp_ledc_exit(void)
{
    memset(ledc_dma_buf,0,sizeof(ledc_dma_buf));
    ledc_dma_kick((u32)ledc_dma_buf, LEDC_RGB_NUM*3);
    ledc_kick();
    delay_us(300);
    LEDCCON &= ~ BIT(0);             //ledc disen
}

void rgb_ledc_mode_change(void)
{
    static uint8_t mode_cnt = 0;
    mode_cnt++;
    if (mode_cnt >= LEDC_MODE_NUM){
        mode_cnt = 0;
    }
    rgb_ledc_mode_set(mode_cnt);
}

void rgb_ledc_mode_set(uint8_t rgb_mode_set)
{
    rgb_ledc_cfg.rgb_mode  = rgb_mode_set;
    rgb_ledc_cfg.ledc_mode_change_flag = 1;
}

uint8_t rgb_ledc_mode_get(void)
{
    return rgb_ledc_cfg.rgb_mode;
}

void ledc_rgb5050_write(u8 rgb_r, u8 rgb_g ,u8 rgb_b ,u8 rgb_num_pos)
{
    u8 *p_dest = (u8 *)ledc_dma_buf;
    for (int i = 0; i < LEDC_RGB_NUM; i++) {
        if(i == rgb_num_pos)
        {
            p_dest[0] = rgb_r;
            p_dest[1] = rgb_g;
            p_dest[2] = rgb_b;
        }
        p_dest += 3;
    }
//    print_r(ledc_dma_buf,LEDC_RGB_NUM*3);
}

void ledc_rgb_breathing(u8 speed)
{
    u8 SV = 5;
	static u8 delay_times = 0;
	static u8 cyc_rgb_flag,RGB_F;
    if (delay_times++ < speed)
        return;
    delay_times = 0;
    switch (cyc_rgb_flag)
    {
        case 0:
            color_2.RGB_b = 0;
            color_2.RGB_g = 0;
            if(RGB_F==0){
                color_2.RGB_r += SV;
            }else {
                color_2.RGB_r -= SV;
            }
            if(color_2.RGB_r>=255)
            {
                color_2.RGB_r=255;RGB_F=1;
            }
            else if(color_2.RGB_r<=0)
            {
                color_2.RGB_r = 0;
                cyc_rgb_flag = 1;
                RGB_F = 0;
            }
            break;
        case 1:
            color_2.RGB_b = 0;
            color_2.RGB_r = 0;
            if(RGB_F==0){
                color_2.RGB_g += SV;
            }else {
                color_2.RGB_g -= SV;
            }
            if(color_2.RGB_g >= 255){
                color_2.RGB_g = 255;
                RGB_F=1;
            }
            else if(color_2.RGB_g<=0){
                color_2.RGB_g = 0;
                cyc_rgb_flag = 2;
                RGB_F = 0;
            }
            break;
        case 2:
            color_2.RGB_g=0;
            color_2.RGB_r=0;
            if(RGB_F==0){color_2.RGB_b+=SV;}
            else {color_2.RGB_b-=SV;}
            if(color_2.RGB_b>=255)
            {
                color_2.RGB_b=255;RGB_F=1;
            }
            else if(color_2.RGB_b<=0)
            {
                color_2.RGB_b=0;
                cyc_rgb_flag=0;
                RGB_F=0;
            }
            break;

        default:
            break;
    }
    memset(ledc_dma_buf,0,sizeof(ledc_dma_buf));
	for(u8 i=0;i<LEDC_RGB_NUM;i++){
        ledc_rgb5050_write(color_2.RGB_r, color_2.RGB_g, color_2.RGB_b, i);
	}
}

void ledc_rgb_runningwater(u16 speed_led)
{
    static u16 cyc_led_delay = 0;
    static u8 led_i = 0;

    if (cyc_led_delay++ < speed_led)
        return;
    led_i++;
    if (led_i >= LEDC_RGB_NUM)
    {
        led_i = 0;
    }
//    printf("led_i = %d\n", led_i);
//    if(cyc_led_delay5 <= 80){
        memset(ledc_dma_buf,0,sizeof(ledc_dma_buf));
        for (size_t i = led_i; i < led_i + 1; i++)    //第0灯   //红
        {
            if (i >= LEDC_RGB_NUM)
            {
                ledc_rgb5050_write(0xe0, 0, 0,  /*LEDC_RGB_NUM -  */(i - LEDC_RGB_NUM));
            }
            else
            {
                ledc_rgb5050_write(0xe0, 0, 0, /* LEDC_RGB_NUM - */ i);
            }
        }
        for (size_t i = led_i+1; i < led_i + 2; i++)  //第1灯
        {
            if (i >= LEDC_RGB_NUM)
            {
//                ledc_rgb5050_write(0xc6, 0x26, 0, /* LEDC_RGB_NUM - */ (i - LEDC_RGB_NUM));   //橙色
                 ledc_rgb5050_write(0xa8, 0, 0x28, /* LEDC_RGB_NUM -  */(i - LEDC_RGB_NUM));  //粉

            }
            else
            {
//                ledc_rgb5050_write(0xc6, 0x26, 0, /* LEDC_RGB_NUM - */ i);
                  ledc_rgb5050_write(0xa8, 0, 0x28, /* LEDC_RGB_NUM - */ i);

            }
        }
        for (size_t i = led_i + 2; i < led_i + 3; i++) //第2灯
        {
            if (i >= LEDC_RGB_NUM)
            {
//                ledc_rgb5050_write(0x75, 0x5c, 0, /* LEDC_RGB_NUM - */ (i - LEDC_RGB_NUM));  //黄
                ledc_rgb5050_write(0x88, 0, 0x48,/*  LEDC_RGB_NUM -  */(i - LEDC_RGB_NUM));  //紫粉
            }
            else
            {
//                ledc_rgb5050_write(0x75, 0x5c, 0, /* LEDC_RGB_NUM - */ i);
                 ledc_rgb5050_write(0x88, 0, 0x48, /* LEDC_RGB_NUM - */ i);
            }
        }
        for (size_t i = led_i + 3; i < led_i + 4; i++)  //第3灯
        {
            if (i >= LEDC_RGB_NUM)
            {
//                ledc_rgb5050_write(0x65, 0x65, 0, /* LEDC_RGB_NUM - */ (i - LEDC_RGB_NUM));  //黄
                ledc_rgb5050_write(0x68, 0, 0x68, /* LEDC_RGB_NUM - */ (i - LEDC_RGB_NUM));  //紫粉
            }
            else
            {
//                ledc_rgb5050_write(0x65, 0x65, 0,  /*LEDC_RGB_NUM - */ i);
                 ledc_rgb5050_write(0x68, 0, 0x68, /* LEDC_RGB_NUM - */ i);
            }
        }
        for (size_t i = led_i + 4; i < led_i + 5; i++)  //第4灯
        {
            if (i >= LEDC_RGB_NUM)
            {
//                ledc_rgb5050_write(0x39, 0x84, 0, /* LEDC_RGB_NUM - */ (i - LEDC_RGB_NUM));  //绿
                ledc_rgb5050_write(0, 0, 0xe2, /* LEDC_RGB_NUM - */ (i - LEDC_RGB_NUM));  //蓝
            }
            else
            {
//                ledc_rgb5050_write(0x39, 0x84, 0, /* LEDC_RGB_NUM - */ i);
                ledc_rgb5050_write(0, 0, 0xe2, /* LEDC_RGB_NUM - */ i);
            }
        }
        for (size_t i = led_i + 5; i < led_i + 6; i++)
        {
            if (i >= LEDC_RGB_NUM)
            {
//                ledc_rgb5050_write(0x29, 0x94, 0, /* LEDC_RGB_NUM - */ (i - LEDC_RGB_NUM));   //绿
                 ledc_rgb5050_write(0, 0x3f, 0xa2, /* LEDC_RGB_NUM - */ (i - LEDC_RGB_NUM));  //浅蓝
            }
            else
            {
//                ledc_rgb5050_write(0x29, 0x94, 0, /* LEDC_RGB_NUM - */ i);
                 ledc_rgb5050_write(0, 0x3f, 0xa2, /* LEDC_RGB_NUM - */ i);
            }
        }
        for (size_t i = led_i + 6; i < led_i + 7; i++)
        {
            if (i >= LEDC_RGB_NUM)
            {
//                ledc_rgb5050_write(0x19, 0xa4, 0, /* LEDC_RGB_NUM - */ (i - LEDC_RGB_NUM));   //绿
                ledc_rgb5050_write(0, 0x6f, 0x68,/*  LEDC_RGB_NUM - */ (i - LEDC_RGB_NUM));  //青色
//                   ledc_rgb5050_write(0, 0x3f, 0xa2, /* LEDC_RGB_NUM - */ (i - LEDC_RGB_NUM));  //浅蓝
            }
            else
            {
//                ledc_rgb5050_write(0x19, 0xa4, 0, /* LEDC_RGB_NUM - */ i);
                ledc_rgb5050_write(0, 0x6f, 0x68, /* LEDC_RGB_NUM - */ i);
//                 ledc_rgb5050_write(0, 0x3f, 0xa2, /* LEDC_RGB_NUM - */ i);
            }
        }
        for (size_t i = led_i + 7; i < led_i + 8; i++)
        {
            if (i >= LEDC_RGB_NUM)
            {
                ledc_rgb5050_write(0, 0xde, 0x16,  /*LEDC_RGB_NUM - */ (i - LEDC_RGB_NUM));   //青色

            }
            else
            {
                ledc_rgb5050_write(0, 0xde, 0x16,  /*LEDC_RGB_NUM - */ i);


            }
        }
        for (size_t i = led_i + 8; i < led_i + 9; i++)
        {
            if (i >= LEDC_RGB_NUM)
            {
//                ledc_rgb5050_write(0, 0xee, 0x26, /* LEDC_RGB_NUM - */ (i - LEDC_RGB_NUM));  //青色
//                  ledc_rgb5050_write(0x65, 0x65, 0, /* LEDC_RGB_NUM - */ (i - LEDC_RGB_NUM));  //黄
                    ledc_rgb5050_write(0x19, 0xa4, 0, /* LEDC_RGB_NUM - */ (i - LEDC_RGB_NUM));   //绿
            }
            else
            {
//                ledc_rgb5050_write(0, 0xee, 0x26, /* LEDC_RGB_NUM - */ i);
//                 ledc_rgb5050_write(0x65, 0x65, 0,  /*LEDC_RGB_NUM - */ i);
                     ledc_rgb5050_write(0x19, 0xa4, 0, /* LEDC_RGB_NUM - */ i);
            }
        }
        for (size_t i = led_i + 9; i < led_i + 10; i++)
        {
            if (i >= LEDC_RGB_NUM)
            {
//                ledc_rgb5050_write(0, 0x93, 0x48, /* LEDC_RGB_NUM - */ (i - LEDC_RGB_NUM));  //青色
                  ledc_rgb5050_write(0x75, 0x5c, 0, /* LEDC_RGB_NUM - */ (i - LEDC_RGB_NUM));  //黄
            }
            else
            {
//                ledc_rgb5050_write(0, 0x93, 0x48,/*LEDC_RGB_NUM - */ i);
                  ledc_rgb5050_write(0x75, 0x5c, 0, /* LEDC_RGB_NUM - */ i);

            }
        }
        for (size_t i = led_i + 10; i < led_i + 11; i++)
        {
            if (i >= LEDC_RGB_NUM)
            {
//                ledc_rgb5050_write(0, 0x6f, 0x68,/*  LEDC_RGB_NUM - */ (i - LEDC_RGB_NUM));  //青色
                 ledc_rgb5050_write(130, 110, 0,/*  LEDC_RGB_NUM - */ (i - LEDC_RGB_NUM));  //黄色
            }
            else
            {
//                ledc_rgb5050_write(0, 0x6f, 0x68, /* LEDC_RGB_NUM - */ i);
                 ledc_rgb5050_write(130, 110, 0, /* LEDC_RGB_NUM - */ i);
            }
        }
        for (size_t i = led_i + 11; i < led_i + 12; i++)
        {
            if (i >= LEDC_RGB_NUM)
            {
//                 ledc_rgb5050_write(0, 0x3f, 0xa2, /* LEDC_RGB_NUM - */ (i - LEDC_RGB_NUM));   //浅蓝
                  ledc_rgb5050_write(0xc6, 0x26, 0, /* LEDC_RGB_NUM - */ (i - LEDC_RGB_NUM));   //橙色
            }
            else
            {
//                ledc_rgb5050_write(0, 0x3f, 0xa2, /* LEDC_RGB_NUM - */ i);
                 ledc_rgb5050_write(0xc6, 0x26, 0, /* LEDC_RGB_NUM - */ i);
            }
        }
    cyc_led_delay = 0;
}

AT(.com_text.ledc)
void ledc_rgb5050_scan(void)
{
    static u32 ticks = 0;
    if (tick_check_expire(ticks,5)) {
        ticks = tick_get();
        if(rgb_ledc_mode_get() == 0){
            ledc_rgb_runningwater(50);
        }else if(rgb_ledc_mode_get() == 1){
            ledc_rgb_breathing(5);
        }
        ledc_dma_kick((u32)ledc_dma_buf, LEDC_RGB_NUM*3);
        ledc_kick();
    }
}
#endif
