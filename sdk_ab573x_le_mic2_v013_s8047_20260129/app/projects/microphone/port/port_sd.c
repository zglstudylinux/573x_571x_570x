#include "include.h"

AT(.text.sdcard)
void sd_gpio_init(u8 type)
{
}
AT(.com_text.sdio)
bool sdcard_detect_is_busy(void){ return false;};

#if !MUSIC_SDCARD1_EN
AT(.text.sdcard)
void sd1_gpio_init(u8 type)
{
}
#endif
