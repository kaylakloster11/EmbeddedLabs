/*
 * lab4.h
 *
 *  Created on: Oct 11, 2017
 *      Author: kk
 */

#include "msp.h"

#ifndef LAB4_H_
#define LAB4_H_

#define INTERNAL_TEMPERATURE_READ       //Un comment to allow for ADC configuration to measure internal temperature sensor
//#define ADC_PORT5_PIN4                  //uncomment to allow for port 6 pin 1 to become ADC
//#define  TEST_ADC_CHANNEL_2
//use same config of the temp sensor and use a different pin

/* @name N_ADC_to_Voltage
 * @brief
 *      Calculate voltage from ADC value
 *      depends on the N_ADC value
 *      v reference
 *      resolution
 * @param = nadc
 * @param = vref
 * @param = resolution
 */
float voltage_from_ADC(uint16_t nadc, float vref, uint8_t resolution_number_of_bits);

void Button_Interrupt_Config(void);

#endif /* LAB4_H_ */


