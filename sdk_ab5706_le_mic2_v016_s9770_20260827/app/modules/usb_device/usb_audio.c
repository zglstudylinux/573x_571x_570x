#include "include.h"
#include "usb_audio.h"

#define TRACE_EN                0

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif


usb_mic_in_cfg_t usb_mic_in_cfg;
#if !ADAPTER_USB_MIC_STEREO_EN
u8 src_in[WIRELESS_MIC_SAMPLES_SELECT*2] AT(.buf.src);
#endif

AT(.com_text.bsp.usb)
void ude_set_mic_status_cb(u8 start)
{
   usb_mic_in_cfg.mic_start = start;
}

AT(.usbdev.com.adj)
void adj_usb_src(void)
{
    if (!usb_mic_in_cfg.mic_start ) {
        return;
    }
    u16 target_low, target_high;
#if ADAPTER_USB_MIC_STEREO_EN
    target_low = 192 * 3;
    target_high = 192 * 6;
#else
    target_low = 192;
    target_high = 1500;
#endif
    u16 usbmic_len = usbmic_len_get();

    if (usbmic_len < target_low) {
#if ADAPTER_USB_MIC_STEREO_EN
        src1_adjust_speed(-65536);
#else
        src_phase_comp_set(-51);   ///多1/120
#endif
    } else if (usbmic_len > target_high) {
#if ADAPTER_USB_MIC_STEREO_EN
        src1_adjust_speed(65536 * 2);
#else
        src_phase_comp_set(51);     ///少1/120
#endif
    } else  {
#if ADAPTER_USB_MIC_STEREO_EN
        src1_adjust_speed(0);
#else
        src_phase_comp_set(0);
#endif
    }
}

AT(.com_text.usb_mic_in)
void usb_mic_in_audio_input(u8 *ptr, u32 samples, int ch_mode, void *params)
{
#if ADAPTER_USB_MIC_STEREO_EN
    samples = samples * 2;
#else 
    memcpy(src_in, ptr, samples*2);
    samples = src_process(src_in, ptr, samples);
#endif
    if (usb_mic_in_cfg.mic_start && !usb_mic_in_cfg.mute) {
        usbmic_sdadc_process(ptr, samples, ch_mode);
    }

    if (usb_mic_in_cfg.callback) {
        usb_mic_in_cfg.callback(ptr, samples, ch_mode, params);
    }
    adj_usb_src();
}

AT(.com_text.usb_mic_in)
void usb_mic_in_audio_output_callback_set(audio_callback_t callback)
{
    usb_mic_in_cfg.callback = callback;
}

AT(.text.usb_mic_in)
void usb_mic_in_audio_mute_set(u8 mute)
{
    usb_mic_in_cfg.mute = mute;
}

AT(.text.usb_mic_in)
void usb_mic_in_init(u8 sample_rate, u16 samples, u8 channel)
{
    memset(&usb_mic_in_cfg, 0, sizeof(usb_mic_in_cfg));
    soft_src_init(2,0);
}
