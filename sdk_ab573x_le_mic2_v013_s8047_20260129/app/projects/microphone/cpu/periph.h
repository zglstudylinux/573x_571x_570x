#ifndef __PERIPH_H_
#define __PERIPH_H_

#include "i2c.h"
#include "timer.h"
#include "huart.h"
#include "uart.h"
#include "saradc.h"
#include "port_int.h"
#include "vad.h"
#include "lowpwr.h"
#include "pwm.h"

#include "key/pwrkey.h"
#include "key/adkey.h"
#include "key/tkey.h"
#include "key/irkey.h"

//SPICS, SPICLK
#define FLASH_SLEEP_ENTER()     u32 gpiogde = GPIOGDE; GPIOGDE = BIT(2) | BIT(4)
#define FLASH_SLEEP_EXIT()      GPIOGDE = gpiogde

//WKO
#define WKO_HIGH_WAKEUP_EN()    RTCCON13 |= BIT(24);            //WK PIN0 High level wakeup

//gpio
void gpio_pwron_init(void);
//uart0
void uart0_mapping_sel(void);
//rtc
void rtc_32k_configure(void);
//charge
void charge_box_nwb_init(void);

//tsen
void tsen_var_init(void);
void tsen_process(void);
u32 tsen_get_temperature(void);
#endif // __PERIPH_H_
