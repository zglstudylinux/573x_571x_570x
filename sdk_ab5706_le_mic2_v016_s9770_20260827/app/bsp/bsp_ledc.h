#ifndef __BSP_LEDC_H
#define __BSP_LEDC_H

/**
 * @brief Clock of LEDC_CLK Enumeration.
 */
typedef enum {
    CLK_LEDC_CLK_XOSC24M            = 0,
    CLK_LEDC_CLK_X24M_CLKX2,
    CLK_LEDC_CLK_X24M_CLKDIV3,
    CLK_LEDC_CLK_RC24M,
} CLK_LEDC_CLK_TYPEDEF;

typedef struct {
    uint8_t                 baud;
    uint8_t                 delay;
    uint8_t                 code_0_high;
    uint8_t                 code_1_high;
    uint16_t                reset_high;
    uint16_t                reset_low;
}ledc_timing_t;

typedef struct {
    u8 rgb_mode;
    u8 rgb_mode_old;
    u8 ledc_mode_change_flag;
} rgb_ledc_cfg_t;

typedef struct {
    u8      RGB_R;
    u8      RGB_G;
    u8      RGB_B;
    s16      RGB_r;
    s16      RGB_g;
    s16      RGB_b;
    s16      RGB_1;
    s16      RGB_2;
    s16      RGB_3;
} color_t;

void bsp_ledc_init(void);
void bsp_ledc_start(void);
void bsp_ledc_exit(void);
void bsp_ledc_timing_cfg(ledc_timing_t *ledc_tim_cfg);
void rgb_ledc_mode_change(void);
void rgb_ledc_mode_set(uint8_t rgb_mode_set);
uint8_t rgb_ledc_mode_get(void);
#endif
