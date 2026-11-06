#include "include.h"
#include "bsp_i2s.h"
#include "../../os/os_thread.h"
#if I2S_EN
#define TRACE_EN                1

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

static i2s_cfg_t *i2s_libcfg;
static bool i2s_dmain_in_ptr_idx;
static bool i2s_dmain_out_ptr_idx;
static u8 i2s_dma_rxbuf[I2S_DMABUF_LEN]AT(.i2s_buf.dma);   //若i2s_cfg.mode中有RAMTX或RAMRX,需要该dmabuf做中断缓存
static u8 i2s_dma_txbuf[I2S_DMABUF_LEN]AT(.i2s_buf.dma);   //dma中断缓存
static u32 i2sconsfr;

void i2s_rx_kick_start(void);
void i2s_irq_init(void);

u8 i2s_16bit_clk_div[3][2] = {
    //48KHz, 16bit
    {2-1, 8-1}, //64fs
    {4-1, 4-1}, //128fs
    {8-1, 2-1}, //256fs
};

u8 i2s_32bit_clk_div[3][2] = {
 //bclkdiv //mclk_div
    {8-1,1-1}, //64fs
    {2-1,4-1}, //128fs
    {4-1,2-1}, //256fs
};

AT(.com_text.i2s_audio_in)
u32 *i2s_in_get_obuf(u8 buf_switch_flag)
{
    s32 *i2s_dmain_in_ptr0;
    s32 *i2s_dmain_in_ptr1;
    i2s_dmain_in_ptr0 = (s32 *)&i2s_dma_rxbuf[0];
    i2s_dmain_in_ptr1 = (s32 *)&i2s_dma_rxbuf[I2S_DMABUF_LEN/2];
    if(buf_switch_flag) {
        i2s_dmain_in_ptr_idx = !i2s_dmain_in_ptr_idx;
    }
    return (i2s_dmain_in_ptr_idx == 0)? (u32 *)i2s_dmain_in_ptr0 : (u32 *)i2s_dmain_in_ptr1;
}

AT(.com_text.i2s_audio_out)
u32 *i2s_out_get_obuf(u8 buf_switch_flag)
{
    s32 *i2s_dmain_out_ptr0;
    s32 *i2s_dmain_out_ptr1;
    i2s_dmain_out_ptr0 = (s32 *)&i2s_dma_txbuf[0];
    i2s_dmain_out_ptr1 = (s32 *)&i2s_dma_txbuf[I2S_DMABUF_LEN/2];
    if(buf_switch_flag) {
        i2s_dmain_out_ptr_idx = !i2s_dmain_out_ptr_idx;
    }
    return (i2s_dmain_out_ptr_idx == 0)? (u32 *)i2s_dmain_out_ptr0 : (u32 *)i2s_dmain_out_ptr1;
}

void i2s_clk_set(u32 i2s_clk_ch, u32 i2s_clk_div)
{
    RSTCON0 |= BIT(1);                                  //IIS release reset, enable model function
//    CLKGAT2 |= BIT(0);//与dac有关
    CLKGAT0 |= BIT(10);
    if(i2s_clk_ch > 3)  {
        i2s_clk_ch = 3;
    }
    if (i2s_clk_div > 15) {
        i2s_clk_div = 15;
    }
    CLKDIVCON0 = (CLKDIVCON0 & ~(0x0F<<28)) | (i2s_clk_div << 28);
    CLKCON1 = (CLKCON1 & ~(0x03<<28)) | (i2s_clk_ch << 28);
}

AT(.com_text.bsp_i2s)
void i2s_dma_start(void)
{
    if(!(IISCON0 & BIT(0))) {
        i2sconsfr |= BIT(0);
        IISCON0 = i2sconsfr;           //I2S kick start需要同时操作其他配置寄存器
    }
}

void i2s_dma_stop(void)
{
    IISCON0 &= ~BIT(0);
}


AT(.com_text.bsp_i2s)
void i2s_isr(void)
{
    u32 cache_addr;
    if (IISCON0 & BIT(16)) {        //TX ISR
        IISCPND |= BIT(16);
        cache_addr = (u32)i2s_out_get_obuf(1);
        IISDMAOADR1 = cache_addr;
        if (i2s_libcfg->dma_cfg.isr_tx_callback) {
            i2s_libcfg->dma_cfg.isr_tx_callback((void*)cache_addr, IISDMACNT&0x3ff, IISCON0 & BIT(2));//IISDMACNT寄存器的[0:9]位是存储读写数据位数的，其他位置是标准位，传参应该传[0:9]
        }
    }

    if (IISCON0 & BIT(17)) {        //RX ISR
        IISCPND |= BIT(17);
        cache_addr = (u32)i2s_in_get_obuf(1);
        IISDMAIADR1 = cache_addr;
        if (i2s_libcfg->dma_cfg.isr_rx_callback) {
            i2s_libcfg->dma_cfg.isr_rx_callback((void*)cache_addr,IISDMACNT&0x3ff,IISCON0 & BIT(2));//IISDMACNT寄存器的[0:9]位是存储读写数据位数的，其他位置是标准位，传参应该传[0:9]
#if I2S_AUDIO_IN_EN
            i2s_kick_start();       //调用i2s_audio_in_input
#endif
        }
    }
}

//        G1    G2    G3
//DI  :   PB7   PB1   PE0
//DO  :   PE7   PB2   PE3
//BCLK:   PE5   PB3   PB3
//LRCLK:  PE6   PB4   PB4
//MCLK:   PE4   PB0   PE1

AT(.text.i2s.init)
void i2s_init(i2s_cfg_t *cfg)
{
    i2sconsfr = 0;
    i2s_libcfg = cfg;
    FUNCMCON2 = 0xf;

    if (i2s_libcfg->iomap == I2S_IO_G1) {
        FUNCMCON2 = 1;
        GPIOBDE |= BIT(7);
        GPIOEDE |= (0xf << 4);
        GPIOBFEN |= BIT(7);
        GPIOEFEN |= (0xf << 4);
        if (!(i2s_libcfg->mode & I2SCFG_MASTER)) {      //从机
            GPIOBDIR |= BIT(7);
            GPIOEDIR  = (GPIOEDIR & ~0x80) | 0x70;      //DO output, 其它输入
        } else {                                        //主机
            GPIOBDIR |= BIT(7);
            GPIOEDIR  &= ~0xf0;                         //DI input, 其它输出
        }
    } else if (i2s_libcfg->iomap == I2S_IO_G2) {
        FUNCMCON2 = 2;
        GPIOBDE |= 0x1f;
        GPIOBFEN |= 0x1f;
        if (!(i2s_libcfg->mode & I2SCFG_MASTER)) {                     //从机
            GPIOBDIR  = (GPIOBDIR & ~BIT(2)) | 0x1b;    //DO output, 其它输入
        } else {                                        //主机
            GPIOBDIR  = (GPIOBDIR & ~0x1D) | BIT(1);    //DI input, 其它输出
        }
    } else if (i2s_libcfg->iomap == I2S_IO_G3) {
        FUNCMCON2 = 3;
        GPIOBDE |= BIT(3) | BIT(4);
        GPIOEDE |= BIT(0) | BIT(1) | BIT(3);
        GPIOBFEN |= BIT(3) | BIT(4);
        GPIOEFEN |= BIT(0) | BIT(1) | BIT(3);
		if (!(i2s_libcfg->mode & I2SCFG_MASTER)) {                     //从机
            GPIOBDIR |= BIT(3) | BIT(4);
            GPIOEDIR  = (GPIOEDIR & ~0x8) | 0x3;       //DO output, 其它输入
        } else {                                        //主机
            GPIOBDIR &= ~(BIT(3) | BIT(4));
            GPIOEDIR  = (GPIOBDIR & ~0x0A) | BIT(0);    //DI input, 其它输出
        }
    }

    i2s_clk_set(3,3);     //i2s clk sel设置lrclk时钟为48k,bclk时钟为3M
    IISCON0 = 0;
    IISCON0 = (BIT(16) | BIT(17));        //clear tx/rx pending
    if (i2s_libcfg->mode & I2SCFG_DMA) {  //dmabuf结构: TX_RX同时存在时前一半是TX,后一版半是RX, 如果只有TX或RX,则全部用于TX或RX.
        i2s_dmain_in_ptr_idx = 0;
        i2s_dmain_out_ptr_idx = 0;
        memset(&i2s_dma_txbuf, 0, I2S_DMABUF_LEN);
        memset(&i2s_dma_rxbuf, 0, I2S_DMABUF_LEN);

        i2s_irq_init();
        if (((i2s_libcfg->mode & I2SCFG_TXMASK) == I2SCFG_DMATX) && ((i2s_libcfg->mode & I2SCFG_RXMASK) == I2SCFG_DMARX)) { //RAM RX & RAMTX
            if (I2S_32BIT == i2s_libcfg->bit_mode) {
                IISDMACNT = i2s_libcfg->dma_cfg.samples*2;//32位设置为240，使得触发iis中断的频率为400hz，凑够240*32位数据，每2.5ms触发一次中断，IISDMACNT=1,时钟32位触发一次
            } else {
                IISDMACNT = i2s_libcfg->dma_cfg.samples;//16位设置为120，使得触发iis中断的频率为400hz，凑够120*2*16位数据，每2.5ms触发一次中断，IISDMACNT=1,时钟32位触发一次
            }
            IISDMAIADR0 = (u32)&i2s_dma_rxbuf[0];
            IISDMAIADR1 = (u32)&i2s_dma_rxbuf[I2S_DMABUF_LEN/2];
            IISDMAOADR0 = (u32)&i2s_dma_txbuf[0];
            IISDMAOADR1 = (u32)&i2s_dma_txbuf[I2S_DMABUF_LEN/2];
        } else if (((i2s_libcfg->mode & I2SCFG_TXMASK) == I2SCFG_DMATX) && ((i2s_libcfg->mode & I2SCFG_RXMASK) != I2SCFG_DMARX)) {  //only RAMTX  DMA
            if (I2S_32BIT == i2s_libcfg->bit_mode) {
                IISDMACNT = i2s_libcfg->dma_cfg.samples*2;//32位设置为240，使得触发iis中断的频率为400hz，凑够240*32位数据，每2.5ms触发一次中断，IISDMACNT=1,时钟32位触发一次
            } else {
                IISDMACNT = i2s_libcfg->dma_cfg.samples;//16位设置为120，使得触发iis中断的频率为400hz，凑够120*2*16位数据，每2.5ms触发一次中断，IISDMACNT=1,时钟32位触发一次
            }
            IISDMAOADR0 = (u32)&i2s_dma_txbuf[0];
            IISDMAOADR1 = (u32)&i2s_dma_txbuf[I2S_DMABUF_LEN/2];
        } else if (((i2s_libcfg->mode & I2SCFG_TXMASK) != I2SCFG_DMATX) && ((i2s_libcfg->mode & I2SCFG_RXMASK) == I2SCFG_DMARX)){  //ONLY RXMRX DMA
            if (I2S_32BIT == i2s_libcfg->bit_mode) {
                IISDMACNT = i2s_libcfg->dma_cfg.samples*2;//32位设置为240，使得触发iis中断的频率为400hz，凑够240*32位数据，每2.5ms触发一次中断，IISDMACNT=1,时钟32位触发一次
            } else {
                IISDMACNT = i2s_libcfg->dma_cfg.samples;//16位设置为120，使得触发iis中断的频率为400hz，凑够120*2*16位数据，每2.5ms触发一次中断，IISDMACNT=1,时钟32位触发一次
            }
            IISDMAIADR0 = (u32)&i2s_dma_rxbuf[0];
            IISDMAIADR1 = (u32)&i2s_dma_rxbuf[I2S_DMABUF_LEN/2];
        }
    }
    if (I2S_16BIT == i2s_libcfg->bit_mode) {
        i2sconsfr &= ~BIT(2);   //0: iis bit mode (0:16bit) at master function
        IISBAUD = (i2s_16bit_clk_div[i2s_libcfg->mclk_sel][0] << 7) | i2s_16bit_clk_div[i2s_libcfg->mclk_sel][1];//mclk=12.5m，lrclk应该为48k，bclk应该为1.5M，所以要对mclk进行(7+1)分频
    } else if(I2S_32BIT == i2s_libcfg->bit_mode) {
        i2sconsfr |= BIT(2);     //1: iis bit mode (1:32bit) at master function
        IISBAUD = (i2s_32bit_clk_div[i2s_libcfg->mclk_sel][0] << 7) | i2s_32bit_clk_div[i2s_libcfg->mclk_sel][1];//i2s_32bit_clk_div[2][0]=3<<7|i2s_32bit_clk_div[2][0]=1
    }
    if (I2S_LEFT_JUSTIFIED == i2s_libcfg->data_mode){
        i2sconsfr &= ~BIT(3);     //0: left-justified mode (data delay 0 clock after WS change)
    } else if (I2S_NORMAL == i2s_libcfg->data_mode){
        i2sconsfr |= BIT(3);     //1: IIS normal mode  (data delay 1 clock after WS change)
    }
    i2sconsfr |= BIT(10);     //dma out requet mask delay eanble (system very fast,need set this)
    if (i2s_libcfg->mode & I2SCFG_MASTER) {
        i2sconsfr &= ~BIT(1);      //0 iis is master mode
    } else {
        i2sconsfr |= BIT(1);      //1 iis is slave mode
    }
    if (i2s_libcfg->mclk_out_en) {
        i2sconsfr |= BIT(9);
    }
    if (i2s_libcfg->mode & I2SCFG_DMA) {
        if ((i2s_libcfg->mode & I2SCFG_TXMASK) == I2SCFG_DMATX) {
            i2sconsfr |= (BIT(4) | BIT(5) | BIT(7));      //enble iis DMA output and output interrupt，data OUT source select: RAM
        }
        if ((i2s_libcfg->mode & I2SCFG_RXMASK) == I2SCFG_DMARX) {
            i2sconsfr |= (BIT(6) | BIT(8));      //enble iis DMA input and interrupt
        }
        if ((i2s_libcfg->mode & I2SCFG_TXRXMASK) == I2SCFG_DMARX) { //只有RAMRX 需要把这位置起来才会KICK起来,同时有打开SRCTX时则可以不用设置它
            i2sconsfr |= BIT(4);
        }
    }
    i2sconsfr &= ~BIT(0);                       //IIS EN,先屏蔽，由iis_start在需要时才打开

    if (i2s_libcfg->mode & I2SCFG_SRC) {
        DACDIGCON0 |= BIT(23);
    } else {
        DACDIGCON0 &= ~BIT(23);
    }

    IISCON0 = i2sconsfr;                      //config iis sfor
}

void i2s_exit(void)
{
    i2s_dma_stop();
}

void bsp_i2s_exit(void)
{
    i2s_exit();
    dac_fade_out();//dac淡出
}
#endif // I2S_EN

void i2s_dma_kick(void)
{
#if I2S_AUDIO_IN_EN
    i2s_audio_in_input(NULL,WIRELESS_MIC_SAMPLES_SELECT,1);
#endif
}
