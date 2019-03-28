#ifndef _PWM_CHECK_H
#define _PWM_CHECK_H
#include "sys.h"
float READ_Pwm_chanel(GPIO_TypeDef * GPIOx, uint16_t GPIO_Pin);
void PWM_Gpio_Init(void);
void pwm_get(void);
#endif
