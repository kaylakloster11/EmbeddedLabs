/*
 * adc_driver.h
 *
 *  Created on: Oct 11, 2017
 *      Author: kk
 */

#include "msp.h"

#ifndef ADC_DRIVER_H_
#define ADC_DRIVER_H_




void ADC_CONFIG(void);

void ADC_CONFIG_Accelerometer(void);


/* @name ADC_CONFIG_Joystick
 * @brief
 *      takes the inputs from the analog pins (A15, A9, and A13)
 *      stores the values in the mem[0], mem[1], and mem[2] positions
 *      uses consecutive sample triggered on start conversion bit. s
 * @param none
 * */
void ADC_CONFIG_Joystick(void);



#endif /* ADC_DRIVER_H_ */
