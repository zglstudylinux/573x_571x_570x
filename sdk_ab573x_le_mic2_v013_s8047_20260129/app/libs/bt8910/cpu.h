#ifndef __CPU_H
#define __CPU_H

#define STR_CPU     "BT8910"    //for dump version
#define LIB_CPU     BT8910      //for copy libs


#define GLOBAL_INT_DISABLE()    uint32_t cpu_ie = PICCON&BIT(0); PICCONCLR = BIT(0); asm volatile("":::"memory")
#define GLOBAL_INT_RESTORE()    asm volatile("":::"memory"); PICCON |= cpu_ie

#define WDT_CLR()               {WDTCON = 0xa; RTCCON10 = BIT(14);}     //2条WDT一起清
#define WDT_EN()                WDTCON = 0x110
#define WDT_DIS()               WDTCON = 0xaa0
#define WDT_RST()               WDTCON = 0xa000110; while (1)
#define WDT_RST_DELAY()         WDTCON = 0xa100110; while (1)

#define RTC_WDT_CLR()           RTCCON10 = BIT(14)
#define RTC_WDT_DIS()           RTCCON12 |= (0x03<<6)
#define RTC_WDT_EN()            RTCCON12 &= ~(0x03<<6)

#endif // __CPU_H
