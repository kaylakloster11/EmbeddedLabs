/*
 * adc.h
 *
 *  Created on: Oct 15, 2017
 *      Author: Kayla
 */

#ifndef ADC_H_
#define ADC_H_

#define ACCELEROMETER

#define TC_SENSOR (1.9) //Calibration Values
#define V_SENSOR (0.680) //Temperature sensor offset
#define VREF (3.3) //Reference Voltage
#define RESOLUTION (16384.0) //14-bit resolution

#define JOY_MIDDLE  (8191.5)
#define JOY_MAX (16383.0)

extern volatile uint16_t NADC_Value;
extern volatile int count;
extern volatile int ButtonEnter;

void configure_ADC(void);
void ADC14_IRQHandler();
float GetVsense();
void CLK_init();
void config_ADC_Stick(void);
void config_ADC_Accel(void);
void configure_ADC();

#endif /* ADC_H_ */
