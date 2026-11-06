#include "include.h"

//mass storage处理回调，避免读写过程太长时间没处理func_process
//type: 0=read, 1=write
AT(.usbdev.com)
WEAK void uds_process_cb(u8 type)
{
    sys_cb.sys_delay = 0;

    if(tick_check_expire(f_ude.run_tick, 200)) {    //200ms
        func_process();
        f_ude.run_tick = tick_get();
    }
}
