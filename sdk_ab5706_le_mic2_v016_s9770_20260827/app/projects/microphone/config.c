#include "include.h"

AT(.rodata.func.table)
const u8 func_sort_table[] = {
#if FUNC_BT_EN
    FUNC_BT,
#endif

#if FUNC_ADAPTER_EN
    FUNC_ADAPTER,
#endif

#if FUNC_DEVICE_EN
    FUNC_DEVICE,
#endif

};

AT(.text.func)
u8 get_funcs_total(void)
{
    return sizeof(func_sort_table);
}

u32 getcfg_vddio_sel(void)
{
    return xcfg_cb.vddio_sel;
}

u32 getcfg_vddbt_capless_en(void)
{
    return xcfg_cb.vddbt_capless_en;
}

u32 getcfg_mic_bias_method(u8 mic_ch)
{
    return xcfg_cb.mic_bias_sel << 4 | xcfg_cb.mic_bias_method;
}

u32 getcfg_mic_gain(u8 mic_ch)
{
    return xcfg_cb.mic_anl_gain << 8 | xcfg_cb.mic_dig_gain;
}
