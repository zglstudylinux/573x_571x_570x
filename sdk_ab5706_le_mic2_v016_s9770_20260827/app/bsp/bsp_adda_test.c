#include "include.h"

#define VOL_DONE_POS        28
#define VOL_STEP_POS        24
#define VOL_DIRECT_POS      30

/*
* dac测试程序
*/

int16_t pcm_is_24bits = 0;
int16_t pcm_is_mono = 0;
extern u16 dac_obuf[];
static uint16_t SinData_48k_16bit[48] = {
0x0000, 0x10b5, 0x2121, 0x30fc, 0x4000, 0x4dec, 0x5a82, 0x658d, 0x6eda, 0x7642, 0x7ba3, 0x7ee8, 0x7fff, 0x7ee8, 0x7ba3, 0x7642,
0x6eda, 0x658d, 0x5a82, 0x4dec, 0x4000, 0x30fc, 0x2121, 0x10b5, 0x0000, 0xef4b, 0xdedf, 0xcf04, 0xc000, 0xb214, 0xa57e, 0x9a73,
0x9126, 0x89be, 0x845d, 0x8118, 0x8000, 0x8118, 0x845d, 0x89be, 0x9126, 0x9a73, 0xa57e, 0xb214, 0xc000, 0xcf04, 0xdedf, 0xef4b
};

void dac_aubuf_init_test(void)
{
    uint32_t aubuf_size = 480;
    uint32_t threshold = 240;


    memset((u8 *)dac_obuf, 0, 4*480);
    AUBUF0CON |= BIT(24);                            //Mono2stereo en
    AUBUF0CON |= BIT(18);                            //DAC high sample rate count enable
    AUBUF0CON |= BIT(0);                             //Reset audio Buffer
    AUBUF0SIZE = (aubuf_size - 1);                   //Configure audio buffer size
    AUBUF0SIZE |= threshold << 16;                   //Audio Buffer Threshold
    AUBUF0STARTADDR = DMA_ADR(dac_obuf);             //Configure audio buffer start address

}

void bsp_adda_clk_init (void)
{
    PLL0CON0 |= BIT(2);
    PRCLKCON0 |= BIT(1);//addiv_clken_a
    PRCLKCON0 &= ~BIT(8);//addapll_sel 0:pll_out_dl
    RSTCON0 |= BIT(4);//pllsdm_soft_rstn
    PRCLKDIVCON0 = (PRCLKDIVCON0 & ~0x1f) | (0x9);//adpll_div
    PRCLKCON0 &= ~BIT(9);
    RSTCON0 |= BIT(16);
    CLKCON1 &= ~BIT(2);


    RSTCON0 |= BIT(10);//sddac_soft_rstn
    CLKGAT2 |= BIT(0);//dac_cklen
    CLKGAT2 |= BIT(2);//src2 en
    CLKGAT2 |= BIT(1);//src1 en
}

void bsp_dac_clk_init (void)
{
    bsp_adda_clk_init ();
}

static void dac_dig_power_on(uint16_t dac_fsin,uint16_t src1_src2_en,int16_t is_mono,int16_t is_24bits)
{
    pcm_is_mono = is_mono;
    pcm_is_24bits = is_24bits;

    //dac digital init
    if (1) {               //区分AUPLL的频率, 1=48khz, 0=44.1khz
        DACDIGCON0 |= BIT(1);
    } else {
        DACDIGCON0 &= ~BIT(1);
    }
    DACDIGCON0 |= BIT(23);              //stereo
    if (pcm_is_mono) {
        DACDIGCON0 &= ~BIT(23);         //mono
    }

    DACDIGCON0 = (DACDIGCON0 &~(0xf<<2)) | (dac_fsin<<2);//src0_in_sample
    DACDIGCON0 = (DACDIGCON0 &~(0xf<<9)) | (dac_fsin<<9);//src1_in_sample
    DACDIGCON0 = (DACDIGCON0 &~(0xf<<16)) | (dac_fsin<<16);//src2_in_sample

    /*dac vol init*/
    DACVOLCON = 0x7fff;
    DACVOLCON |= BIT(VOL_DIRECT_POS);                   //set min digital volume

    SRC0VOLCON = 0x4000;
    SRC0VOLCON |= BIT(VOL_DIRECT_POS);
    SRC1VOLCON = 0x3000;
    SRC1VOLCON |= BIT(VOL_DIRECT_POS);
    SRC2VOLCON = 0x0fff;
    SRC2VOLCON |= BIT(VOL_DIRECT_POS);

    AU0LMIXCOEF = (0x4000 << 16) | (0x3fff);//AU0MIXR0    AU0MIXL0
    AU0RMIXCOEF = (0x4000 << 16) | (0x3fff);//AU0MIXR1    AUOMIXL1

    AU1LMIXCOEF = (0x4000 << 16) | (0x3fff);//AU0MIXR0    AU0MIXL0
    AU1RMIXCOEF = (0x4000 << 16) | (0x3fff);//AU0MIXR1    AUOMIXR1

    AU2LMIXCOEF = (0x4000 << 16) | (0x3fff);//AU0MIXR0    AU0MIXL0
    AU2RMIXCOEF = (0x4000 << 16) | (0x3fff);//AU0MIXR1    AUOMIXR1
    DACDIGCON0 |= BIT(24);//src0 mix enable
    DACDIGCON0 |= BIT(25);//src1 mix enable
    DACDIGCON0 |= BIT(26);//src2 mix enable
    /**************/


//    DACDBGCON |= BIT(29);//src0 bypass
//    DACDBGCON |= BIT(30);//src1 bypass
//    DACDBGCON |= BIT(31);//src2 bypass


    if(src1_src2_en == 0){
        DACDIGCON0 &= ~BIT(8);//src1 dis
        DACDIGCON0 &= ~BIT(15);//src2 dis
    }
    else if(src1_src2_en == 1){
        DACDIGCON0 |= BIT(8);
        DACDIGCON0 &= ~BIT(15);
    }
    else if(src1_src2_en == 2){
        DACDIGCON0 &= ~BIT(8);
        DACDIGCON0 |= BIT(15);
    }
    else if(src1_src2_en == 3){
        DACDIGCON0 |= BIT(8);//src1_en
        DACDIGCON0 |= BIT(15);//src2_en
    }

    DACDIGCON0 |= BIT(0);               //digital dac enable

    DACDIGCON0 |= BIT(6);
}

void dac_power_on_test(uint16_t dac_fsin,uint16_t src1_src2_en,int16_t is_mono,int16_t is_24bits)
{
    bsp_dac_clk_init();
    dac_dig_power_on(dac_fsin,src1_src2_en,is_mono,is_24bits);
}

void bsp_sddac_init (uint16_t dac_fsin,uint16_t src1_src2_en,int16_t is_mono,int16_t is_24bits)
{
    CLKGAT2 |= BIT(0);
    dac_aubuf_init_test();
    dac_power_on_test(dac_fsin,src1_src2_en,is_mono,is_24bits);
}

static int sddac_rw_auangcon(void)
{
    uint32_t auangcon;
    /*AUANGCON0 rw*/
    auangcon = AUANGCON0;
    AUANGCON0 = 0xffffffff;
    if(AUANGCON0 != 0xffffffff)
    {
        printf("rw AUANGCON0 = %x\n",AUANGCON0);
        return 1;
    }
    AUANGCON0 = auangcon;

    /*AUANGCON1 rw*/
    auangcon = AUANGCON1;
    AUANGCON1 = 0xffffffff;
    if(AUANGCON1 != 0xffffffff)
    {
        printf("rw AUANGCON1 = %x\n",AUANGCON1);
        return 1;
    }
    AUANGCON1 = auangcon;

    /*AUANGCON2 rw*/
    auangcon = AUANGCON2;
    AUANGCON2 = 0xffffffff;
    if(AUANGCON2 != 0xffffffff)
    {
        printf("rw AUANGCON2 = %x\n",AUANGCON2);
        return 1;
    }
    AUANGCON2 = auangcon;

    /*AUANGCON3 rw*/
    auangcon = AUANGCON3;
    AUANGCON3 = 0xffffffff;
    if(AUANGCON3 != 0xffffffff)
    {
        printf("rw AUANGCON3 = %x\n",AUANGCON3);
        return 1;
    }
    AUANGCON3 = auangcon;

    /*AUANGCON4 rw*/
    auangcon = AUANGCON4;
    AUANGCON4 = 0xffffffff;
    if(AUANGCON4 != 0xffffffff)
    {
        printf("rw AUANGCON4 = %x\n",AUANGCON4);
        return 1;
    }
    AUANGCON4 = auangcon;

    /*AUANGCON5 rw*/
    auangcon = AUANGCON5;
    AUANGCON5 = 0xffffffff;
    if(AUANGCON5 != 0xffffffff)
    {
        printf("rw AUANGCON5 = %x\n",AUANGCON5);
        return 1;
    }
    AUANGCON5 = auangcon;

    /*AUANGCON6 rw*/
    auangcon = AUANGCON6;
    AUANGCON6 = 0xffffffff;
    if(AUANGCON6 != 0xffffffff)
    {
        printf("rw AUANGCON6 = %x\n",AUANGCON6);
        return 1;
    }
    AUANGCON6 = auangcon;

    /*AUANGCON7 rw*/
    auangcon = AUANGCON7;
    AUANGCON7 = 0xffffffff;
    if(AUANGCON7 != 0xffffffff)
    {
        printf("rw AUANGCON7 = %x\n",AUANGCON7);
        return 1;
    }
    AUANGCON7 = auangcon;
    return 0;
}

AT(.com_text.isr.sdadc)
void dac_out_16bit_sample(short left, short right)
{
    if(!pcm_is_mono) {
        AUBUF0DATA = (right << 16) | (left & 0xffff);
    } else {
        AUBUF0DATA = left & 0xffff;
    }
}
void test_sddac(void)
{
    printf("%s\n",__func__);
    bsp_sddac_init (0,3,0,0);//FSIN  SRC1_SRC2_EN  IS_MONO  NONE

    if(!sddac_rw_auangcon()){
        printf("sddac_rw_auangcon test success\n");
    }
    else{
        printf("sddac_rw_auangcon test error\n");
    }

    printf("PLL0DIV = 0x%x, PRCLKCON0 = 0x%x, PRCLKDIVCON0 = 0x%x\n", PLL0DIV, PRCLKCON0, PRCLKDIVCON0);
    while(1) {
        for(int i=0;i<48;i++)
        {
            dac_out_16bit_sample(SinData_48k_16bit[i],0);
        }
        WDT_CLR();
    }
}

/*
* adc测试程序
*/
#define SDADCSIZE               240
#define ADC_FSOUT               0       //0:48k 1:32k 2:24k 3:16k 4:12k 5:8k 6:120k 7:240k

#define CH_MICL0                0x0C    /* MICL       -> left mic */
#define CH_MICL1                0x0D    /* MICR       -> left mic */

#define IRQ_SDADC_VECTOR                10
#define TRAGET_FIFO_CNT                 200
#define aubuf0_fifo_cnt_get()           (AUBUF0FIFOCNT>>18)
#define dac_phase_set(phase)            PHASECOMP0 = (phase&0x00ffffff)

extern u8 sdadc_buf[];
static uint16_t *sdadc_ptr = (u16 *)sdadc_buf;

void bsp_auphy_set_adc_analog_gain (uint8_t auphy, uint16_t channel, uint8_t gain)
{
    fpga_uart_putchar (auphy, '#');
    fpga_uart_putchar (auphy, 'A');
    fpga_uart_putchar (auphy, 'U');
    fpga_uart_putchar (auphy, 'A');
    fpga_uart_putchar (auphy, 'M');
    fpga_uart_putchar (auphy, 'V');
    fpga_uart_putchar (auphy, (channel>>8) & 0xff);
    fpga_uart_putchar (auphy,  channel & 0xff    );
    fpga_uart_putchar (auphy, gain);
    fpga_uart_putchar (auphy, '*');
    delay_us(100);
}

void sdadc_init_test(void)
{
    SDADC0DMAADDR = (uint32_t)sdadc_buf;
    SDADC0DMASIZE = SDADCSIZE - 1;
    SDADC0DMACON =  (0 << 8)    //dma one sample pending int enable
                  | (1 << 7)    //dma half done pending int enable
                  | (1 << 6)    //dma all done pending int enable
                  | (1 << 2)    //dma date mode
                  | (1 << 0)    //dma enable
                  ;

    SDADC0CON =   (0 << 23)     //24bit
                | (0 << 22)     //adc invert analog data en
                | (0 << 21)     //adc -6dB en
                | (0 << 17)     //adc bit-stream sel  0:itself ch bits-stream 7:dac sdm output
                | (1 << 16 )    //clk edge sel  0:pos 1:neg
                | (0 << 6)
                | (0 << 4 )     //adc cic order sel
                | (ADC_FSOUT << 0)  //fsout_sel
                ;

    SDADCCON |= BIT(0);

    printf("SDADC0CON:%08x\n",SDADC0CON);
}

void bsp_adc_clk_init (void)
{
    CLKGAT2 |= BIT(0);//dac_clken
    CLKGAT3    = 0xffe;//adc_ds_clken
    RSTCON0 |= BIT(12);
}

AT(.com_text.isr.sdadc)
void sdadc_fiq(void)
{
    if(SDADCDMAFLAG & BIT(1))
    {
        SDADCDMACLR = BIT(1);
//        os_mq_aupcm_post(50);
    }
    else if(SDADCDMAFLAG & BIT(0))
    {
        SDADCDMACLR = BIT(0);
//        os_mq_aupcm_post(51);
    }
}

//static uint8_t plc_test_cnt = 0;
//uint16_t out_test[240];
AT(.com_text.isr.sdadc)
void sdadc_proc(uint8_t all_done_flag)
{
    uint16_t offset = 0;
    u32 phase = 0;
    u16 aubuf0_fifo_cnt = aubuf0_fifo_cnt_get();

    if (aubuf0_fifo_cnt > (TRAGET_FIFO_CNT+2)) {
        phase = 0xfff;
    } else if (aubuf0_fifo_cnt < (TRAGET_FIFO_CNT-2)) {
        phase = 0xfff000;
    }

    dac_phase_set(phase);

    if (all_done_flag) {
        offset = SDADCSIZE/2;
    }

//    void plc_fpga_process(uint16_t *in, uint16_t *out, bool pkt_stat);
//    plc_test_cnt++;
//    if (plc_test_cnt > 10) {
//        memset(&sdadc_ptr[offset], 0 ,SDADCSIZE);
//        plc_test_cnt = 0;
//        plc_fpga_process(&sdadc_ptr[offset], out_test, 0);
//    } else {
//        plc_fpga_process(&sdadc_ptr[offset], out_test, 1);
//    }

    for(int i=0;i<SDADCSIZE/2;i++)
    {
//        dac_out_16bit_sample(out_test[i],out_test[i]);
        dac_out_16bit_sample(sdadc_ptr[i+offset],sdadc_ptr[i+offset]);
    }
}

void sdadc_irq_init_test(void)
{
    register_isr(IRQ_SDADC_VECTOR, sdadc_fiq);
    PICPR &= ~BIT(IRQ_SDADC_VECTOR);                   //low priority interrupt
	PICEN |= BIT(IRQ_SDADC_VECTOR);
}

void test_sdadc(void)
{
    printf("%s\n",__func__);
    fpga_uart_init ();
    audio_fpga_setup (CH_MICL0, SPR_48000);
    bsp_auphy_set_adc_analog_gain(1,0,10);

    bsp_sddac_init (0,1,0,0);
    SRC0VOLCON = 0x7fff;
    SRC0VOLCON |= BIT(30);

    bsp_adc_clk_init();
    sdadc_init_test();

    sdadc_irq_init_test();

//    void bb_dbg_init(void);
//    bb_dbg_init();
//    void plc_fpga_init(uint16_t sample_rate);
//    plc_fpga_init(0);

    while(1) {
        WDT_CLR();
    }
}


