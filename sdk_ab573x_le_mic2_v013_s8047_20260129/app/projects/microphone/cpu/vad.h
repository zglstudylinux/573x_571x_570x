#ifndef _BSP_VAD_H
#define _BSP_VAD_H
#define VADDMACONX(x)   SFR_RW(SFR19_BASE + (0x17 + x) * 4)

bool vad_is_start(void);
bool vad_is_wake(void);
void vad_sleep_config(bool enter);
void vad_start(void);
void vad_stop(void);
void vad_kws_process(void);
void vad_sdadc_process(u8 *ptr, u32 samples, u32 pcm_mode);

void bsp_vad_start(void);
void bsp_vad_stop(void);
#endif
