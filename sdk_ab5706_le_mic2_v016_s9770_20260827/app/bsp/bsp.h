#ifndef __BSP_H_
#define __BSP_H_

#include "bsp_i2c.h"
#include "bsp_timer.h"
#include "bsp_huart.h"
#include "bsp_uart.h"
#include "bsp_saradc.h"
#include "bsp_port_int.h"
#include "bsp_ledc.h"
#include "bsp_le_dut.h"
#include "bsp_spi.h"
#include "bsp_uart_command.h"

#define TICK_MAX_DELAY                  0x7fffffff
#define TICKN_GET()                     tick_get()
#define TICKN_IS_EXPIRE(tickn)          (tickn - tick_get() >= TICK_MAX_DELAY)

#define TICK_TIME_GET(t, d)             (t)->tickn = tick_get() + d
#define TICK_ADD(a, b)                  (a + b)
#define TICK_SUB(a, b)                  (a - b)
#define TICK_GEQ(a, b)                  ((a - b) < TICK_MAX_DELAY)
#define TICK_2_TICKN(tick, tickoff)     tick        //非TWS时tick和tickn相等
#define TICKN_2_TICK(tickn, tickoff)    tickn       //非TWS时tick和tickn相等

uint32_t big_endian_read_16( const uint8_t * buffer, int pos);
uint32_t big_endian_read_32( const uint8_t * buffer, int pos);
void big_endian_store_16(uint8_t *buffer, uint16_t pos, uint16_t value);
void big_endian_store_32(uint8_t *buffer, uint16_t pos, uint32_t value);

#endif  // __BSP_H_

