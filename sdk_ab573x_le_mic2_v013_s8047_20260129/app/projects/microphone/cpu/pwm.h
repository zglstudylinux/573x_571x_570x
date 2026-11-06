#ifndef __PWM_H__
#define __PWM_H__

void pwm_init(void);
void pwm_duty_write(u8 pwm0_duty, u8 pwm1_duty, u8 pwm2_duty);
void pwm_close(void);

#endif
