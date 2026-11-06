#include "include.h"

#define MEM_HEAP_SIZE           9*1024
u8 mem_heap[MEM_HEAP_SIZE] AT(.mem_heap);

const uint8_t *bt_rf_get_param(void)
{
    //优先使用FT参数，其次自定义参数，最后是库预置参数
    if(xcfg_cb.ft_rf_param_en && bt_get_ft_trim_value(&xcfg_cb.rf_pa_gain)) {
        return (const uint8_t *)&xcfg_cb.rf_pa_gain;
    } else if(xcfg_cb.bt_rf_param_en) {
        return (const uint8_t *)&xcfg_cb.rf_pa_gain;
    }
    return NULL;
}

//调试时才打开
//AT(.com_text.debug)
//void debug_dump_info_cache_miss_callback(u32 miss_addr)
//{
//    miss_addr += 0x10000000;
//}

//调试时才打开，否则异常时无法复位
//void sys_error_hook_do(u8 err_no);
//AT(.com_text.err)
//void sys_error_hook(u8 err_no)
//{
//    sys_error_hook_do(err_no);
//}

//正常启动Main函数
int main(void)
{
    sys_cb.rst_reason = sys_rst_init(POWKEY_10S_RESET);
    printf("Hello %s: %08x\n", STR_CPU, sys_cb.rst_reason);
    printf("SDK: v%04X LIBS: v%04X\n", SDK_VERSION, LIBS_VERSION);
    sys_rst_dump(sys_cb.rst_reason);

    sys_init();
    func_run();
    return 0;
}

//升级完成
void update_complete(int mode)
{
    sys_cb.rst_reason = sys_rst_init(POWKEY_10S_RESET);
    sys_update_init();
    if (mode == 0) {
        WDT_DIS();
        while (1);
    }
    WDT_RST();
}
