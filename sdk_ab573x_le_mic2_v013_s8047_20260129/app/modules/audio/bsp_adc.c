#include "include.h"
#include "bsp_adc.h"

#if WIRELESS_DUAL_MIC_EN

#define SDADC_DMA_BUF_IDX       2                                       //24bit最多同时用2条ADC, 16bit最多同时用4条ADC
#define SDADC_DMA_SIZE          240
///暂时支持16位
u8 bsp_sdadc_buf[SDADC_DMA_BUF_IDX][SDADC_DMA_SIZE * 2] AT(.adc_buf.buf);

bsp_sdadc_cb_t bsp_sdadc_cb;

u8 *sdadc_dma_buffer_alloc(void)
{
    for (int i = 0; i < SDADC_DMA_BUF_IDX; i++) {
        if ((bsp_sdadc_cb.dma_bitmap & BIT(i)) == 0) {
            bsp_sdadc_cb.dma_bitmap |= BIT(i);
//            my_printf("sdadc dma_buf alloc:%x %x\n", i, bsp_sdadc_buf[i]);
            return bsp_sdadc_buf[i];
        }
    }
    return NULL;
}

void sdadc_dma_buffer_free(u8 *buf)
{
    for (int i = 0; i < SDADC_DMA_BUF_IDX; i++) {
        if ((u32)buf == (u32)bsp_sdadc_buf[i]) {
//            my_printf("sdadc dma_buf free:%x %x\n", i, buf);
            bsp_sdadc_cb.dma_bitmap &= ~BIT(i);
        }
    }
}

#endif // WIRELESS_DUAL_MIC_EN



