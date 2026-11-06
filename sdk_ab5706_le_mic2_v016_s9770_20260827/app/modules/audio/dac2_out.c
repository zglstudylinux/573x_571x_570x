#include "include.h"
#include "dac2_out.h"

static dac2_out_cfg_t dac2_out_cfg;

AT(.com_text.dac2_out)
void dac2_out_audio_input(u8 *ptr, u32 samples, int ch_mode, void *params)
{
    if (dac2_out_cfg.mute) {
        goto dac2_out_callback;
    }

    aubuf2_dma_w4_done();
    aubuf2_dma_kick(ptr, samples, ch_mode);

dac2_out_callback:
    if (dac2_out_cfg.callback) {
        dac2_out_cfg.callback(ptr, samples, ch_mode, params);
    }
}

u32 aufifo2_cnt = 0;
AT(.com_text.dac2.sync)
void dac2_get_fifocnt(u32 tick_cnt)
{
    aufifo2_cnt = AUBUF2FIFOCNT >> 18;
}

AT(.com_text.dac2.sync)
void dac2_play_sync_fifocnt(u16 high_thr, u16 low_thr)
{
    u16 fifo_cnt = aufifo2_cnt;

    if (!(DACDIGCON0 & BIT(20))) {
        DACDIGCON0 |= BIT(20);
    }

    u32 phase = 0;
    if (fifo_cnt <= low_thr) {
        phase = 0xffff00;
    } else if (fifo_cnt >= high_thr) {
        phase = 0xff;
    } else {
        phase = 0x0000;
    }
    PHASECOMP2 = PHASECOMP2 & 0x0f000000;
    PHASECOMP2 = phase;
}

AT(.text.dac2_out)
void dac2_out_audio_output_callback_set(audio_callback_t callback)
{
    dac2_out_cfg.callback = callback;
}

AT(.text.dac2_out)
void dac2_out_audio_mute_set(uint8_t mute)
{
//    if(mute == 0) {
//        dac2_aubuf_init();
//        dac2_spr_set(dac2_out_cfg.sample_rate);
//        dac2_vol_set(DIG_N0DB);
//        dac2_fade_in();
//    }
    dac2_out_cfg.mute = mute;
}

AT(.text.dac2_out)
void dac2_out_init(u8 sample_rate, u16 samples, u8 channel)
{
    memset(&dac2_out_cfg, 0, sizeof(dac2_out_cfg));
//    dac2_out_param_cfg_t *dac2_out_param_temp_ptr = dac2_out_param_get_by_sam_rate(sample_rate);
//    dac2_out_cfg.low_thr = dac2_out_param_temp_ptr->low_thr;
//    dac2_out_cfg.high_thr = dac2_out_param_temp_ptr->high_thr;
    dac2_out_cfg.sample_rate = sample_rate;

    dac2_aubuf_init();
    dac2_spr_set(sample_rate);
    dac2_out_audio_mute_set(0);

    dac2_vol_set(DIG_N0DB);
    dac2_fade_in();
}
