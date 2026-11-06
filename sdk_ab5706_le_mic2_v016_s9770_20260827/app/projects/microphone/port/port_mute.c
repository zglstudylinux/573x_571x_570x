#include "include.h"

#if LOUDSPEAKER_MUTE_EN

#define MUTE_IO_VUSB           29

///配置工具选择MUTE及AB/D控制的GPIO
static gpio_t mute_gpio;
static gpio_t amp_gpio;
static u8 amp_conf_type = 0;
uint8_t cfg_spk_mute_en = 0;

enum {
    AMPABD_TYPE_LEVEL        = 0,
    AMPABD_TYPE_PULSE,
    AMPABD_TYPE_DIVIDER,
};

void loudspeaker_mute_init(void)
{
    if (!xcfg_cb.spk_mute_en) {
        xcfg_cb.loudspeaker_unmute_delay = 0;
        return;
    }
    u8 io_num = xcfg_cb.spk_mute_io_sel;
    if (io_num == MUTE_IO_VUSB){             //mute脚使用VUSB
        PWRCON0 |= BIT(30);                  //输入使能位，1：使能；0：关闭
        PWRCON0 |= BIT(29);                  //输出功能使能位，1：高电平；0：低电平；不使能：输出高组态
        PWRCON1 |= BIT(0);
        PWRCON1 &= ~BIT(1);                  //low
    }
    gpio_t *p = &mute_gpio;
    bsp_gpio_cfg_init(p, io_num);
    if (p->sfr == NULL) {
        return;
    }
    cfg_spk_mute_en = 1;
    p->sfr[GPIOxDE] |= BIT(p->num);
    p->sfr[GPIOxDIR] &= ~BIT(p->num);
    if (xcfg_cb.high_mute) {
        p->sfr[GPIOxSET] = BIT(p->num);
    } else {
        p->sfr[GPIOxCLR] = BIT(p->num);
    }
}

AT(.com_text.mute)
void loudspeaker_mute(void)
{
    if (!xcfg_cb.spk_mute_en) {
        return;
    }
    gpio_t *p = &mute_gpio;
    if (p->sfr == NULL) {
        if (xcfg_cb.ampabd_io_sel == MUTE_IO_VUSB){
        } else {
            return;
        }
    }
#if LOUDSPEAKER_MUTE_AB_D_EN                      // 分压式AB/D切换
    if (xcfg_cb.ampabd_type == AMPABD_TYPE_DIVIDER){
        p->sfr[GPIOxDIR] &= ~BIT(p->num);
        p->sfr[GPIOx]	 &= ~BIT(p->num);
        p->sfr[GPIOxPU ] &= ~BIT(p->num);
        p->sfr[GPIOxPD ] &= ~BIT(p->num);
    }
#else
    if (xcfg_cb.spk_mute_io_sel == MUTE_IO_VUSB){//mute脚使用VUSB
        PWRCON1 &= ~BIT(1);                      //low
    } else {
        if (xcfg_cb.high_mute) {
            p->sfr[GPIOxSET] = BIT(p->num);
        } else {
            p->sfr[GPIOxCLR] = BIT(p->num);
        }
    }
#endif
    delay_us(200);                               //进功放mute之前，延迟一段时间，避免功放mute慢了，引入其他杂音
}

AT(.com_text.mute)
void loudspeaker_unmute(void)
{
    if (!xcfg_cb.spk_mute_en) {
        return;
    }
    gpio_t *p = &mute_gpio;
    if (p->sfr == NULL) {
        if (xcfg_cb.ampabd_io_sel == MUTE_IO_VUSB){
        } else {
            return;
        }
    }

#if LOUDSPEAKER_MUTE_AB_D_EN                       // 分压式AB/D切换
//	if(func_cb.sta == FUNC_FMRX)
//	{
//	    if (xcfg_cb.ampabd_type == AMPABD_TYPE_DIVIDER){
//            p->sfr[GPIOxDIR] |=  BIT(p->num);
//            p->sfr[GPIOxPU ] |=  BIT(p->num);//PU
//            p->sfr[GPIOxPD ] |=  BIT(p->num);//PD
//	    }
//	}
//	else
	{
		p->sfr[GPIOxDIR] &= ~BIT(p->num);
		p->sfr[GPIOx]	 |=  BIT(p->num);
		p->sfr[GPIOxPU ] &= ~BIT(p->num);
		p->sfr[GPIOxPD ] &= ~BIT(p->num);
	}
#elif LOUDSPEAKER_MUTE_AB_D_PULSE_EN              // 一线脉冲AB/D切换
    if (xcfg_cb.ampabd_io_sel == MUTE_IO_VUSB){
        if (xcfg_cb.high_mute) {
            PWRCON1 &= ~BIT(1);                     //low
        } else {
            PWRCON1 |= BIT(1);                      //high
        }
    } else {
        if (xcfg_cb.high_mute) {
            p->sfr[GPIOxCLR] = BIT(p->num);
        } else {
            p->sfr[GPIOxSET] = BIT(p->num);
        }
    }

    if (xcfg_cb.ampabd_type == AMPABD_TYPE_PULSE && !xcfg_cb.high_mute) {
        if (xcfg_cb.ampabd_io_sel == MUTE_IO_VUSB){ ///mute脚使用VUSB
            PWRCON1 &= ~BIT(1);                     //low
            delay_5ms(2);                           //拉低10ms
            PWRCON1 |= BIT(1);                      //high
            delay_5ms(4);                           //拉高20ms
            if (amp_conf_type) {                    //AB类脉冲控制
                for (uint8_t i = 0; i < 5; i++) {   //AB类需5个脉冲
                    PWRCON1 &= ~BIT(1);             //low
                    delay_us(50);                   //需维持5~95us
                    PWRCON1 |= BIT(1);              //high
                    delay_us(50);                   //需维持5~95us
                }
            } else {                                //D类防破音脉冲控制
                for (uint8_t i = 0; i < 3; i++) {   //D类防破音需3个脉冲
                    PWRCON1 &= ~BIT(1);             //low
                    delay_us(50);
                    PWRCON1 |= BIT(1);              //high
                    delay_us(50);
                }
            }
            PWRCON1 |= BIT(1);                      //high
        } else {
            p->sfr[GPIOxCLR] = BIT(p->num);         //low
            delay_5ms(2);                           //拉低10ms
            p->sfr[GPIOxSET] = BIT(p->num);         //high
            delay_5ms(4);                           //拉高20ms
            if (amp_conf_type) {                    //AB类脉冲控制
                for (uint8_t i = 0; i < 5; i++) {   //AB类需5个脉冲
                    p->sfr[GPIOxCLR] = BIT(p->num); //low
                    delay_us(50);                   //需维持5~95us
                    p->sfr[GPIOxSET] = BIT(p->num); //high
                    delay_us(50);                   //需维持5~95us
                }
            } else {                                //D类防破音脉冲控制
                for (uint8_t i = 0; i < 3; i++) {   //D类防破音 需3个脉冲
                     p->sfr[GPIOxCLR] = BIT(p->num);//low
                    delay_us(50);
                    p->sfr[GPIOxSET] = BIT(p->num); //high
                    delay_us(50);
                }
            }
            p->sfr[GPIOxSET] = BIT(p->num);         //high
        }
    }
#endif
}

AT(.text.mute)
void loudspeaker_disable(void)
{
    if (!xcfg_cb.spk_mute_en) {
        return;
    }
    gpio_t *p = &mute_gpio;
    if (p->sfr == NULL) {
        return;
    }
    p->sfr[GPIOxDIR] |= BIT(p->num);
}

AT(.text.amplifier)
void amp_sel_cfg_d(void)
{
    amp_conf_type = 0;
    gpio_t *p = &amp_gpio;
    if (p->sfr == NULL) {
        return;
    }
    p->sfr[GPIOxDE] |= BIT(p->num);
    p->sfr[GPIOxDIR] &= ~BIT(p->num);
    p->sfr[GPIOxSET] = BIT(p->num);
}

AT(.text.amplifier)
void amp_sel_cfg_init(u8 io_num)
{
    gpio_t *p = &amp_gpio;
    bsp_gpio_cfg_init(p, io_num);
    amp_sel_cfg_d();

    if (io_num == MUTE_IO_VUSB){             //mute脚使用VUSB
        PWRCON0 |= BIT(30);                  //输入使能位，1：使能；0：关闭
        PWRCON0 |= BIT(29);                  //输出功能使能位，1：高电平；0：低电平；不使能：输出高组态
        PWRCON1 |= BIT(0);
        PWRCON1 &= ~BIT(1);                  //low
    }
}

AT(.text.amplifier)
void amp_sel_cfg_ab(void)
{
    amp_conf_type = 1;
    gpio_t *p = &amp_gpio;
    if (p->sfr == NULL) {
        return;
    }
    p->sfr[GPIOxDE] |= BIT(p->num);
    p->sfr[GPIOxDIR] &= ~BIT(p->num);
    p->sfr[GPIOxCLR] = BIT(p->num);
}

AT(.text.amplifier)
void amp_sel_cfg_dis(void)
{
    gpio_t *p = &amp_gpio;
    if (p->sfr == NULL) {
        return;
    }
    p->sfr[GPIOxDE] |= BIT(p->num);
    p->sfr[GPIOxDIR] |= BIT(p->num);
}
#endif
