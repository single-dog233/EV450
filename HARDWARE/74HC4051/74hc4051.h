#ifndef _74HC4051_H
#define _74HC4051_H

#include "sys.h"
#define adc2 ADC_Channel_0
#define adc3 ADC_Channel_1
#define adc1 ADC_Channel_2
#define adc0 ADC_Channel_3
#define adc4 ADC_Channel_4
#define adc5 ADC_Channel_5
#define adc7 ADC_Channel_6
#define adc6 ADC_Channel_7
void voltage_get(void);
void chip_74hc4051_adc_init(void);
#endif
