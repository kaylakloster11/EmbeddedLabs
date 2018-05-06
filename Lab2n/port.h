/*
 * port.h
 *
 *  Created on: Sep 11, 2017
 *      Author: Kayla
 */

#ifndef PORT_H_
#define PORT_H_

#define FULLROT 14.0f //14 interrupts in one full wheel rotation
#define CIRCUM 311.93f //wheel diameter = 99.2 mm and Circumference = pi*d = 311.93
#define GETMETER 1000.0f

void GPIO_configure(void);
void PORT1_IRQHandler();
int getSysTime();
float CalcDist();

#endif /* PORT_H_ */
