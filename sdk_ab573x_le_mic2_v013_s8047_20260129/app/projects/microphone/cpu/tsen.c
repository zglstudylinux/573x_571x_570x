#include "include.h"

//内部温度传感器流程,注意不能删掉,会根据温度校准一些芯片参数

#define TRACE_EN                0

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

typedef struct {
    u32 tick;
    u32 temperature;
} tsen_cb_t;

#if TSEN_DETECT_EN

static tsen_cb_t tsen_cb;

void temperature_process(int temperature);

static void tsen_process_do(void)
{
    int vbatvco_freq = vbatvco_freq_get();
    tsen_cb.temperature = vbatvco_freq * 100 / 1239 + 25;  //斜率k = 12.39需更新, 此处乘100是方便计算div;
    TRACE("--------->temperature:%d\n", tsen_cb.temperature);
    temperature_process(tsen_cb.temperature);
}

u32 tsen_get_temperature(void)
{
    return tsen_cb.temperature;
}

AT(.com_text.tsen)
void tsen_process(void)
{
    if (tick_check_expire(tsen_cb.tick, 500)) {
        tsen_cb.tick = tick_get();
        tsen_process_do();
    }
}

void tsen_var_init(void)
{
    memset(&tsen_cb, 0, sizeof(tsen_cb_t));
}
#endif
