/*
 * timer.h
 *
 *  Created on: Sep 12, 2017
 *      Author: Kayla
 */

#ifndef TIMER_H_
#define TIMER_H_

void TA0_0_IRQHandler(void);
void timer_a0_config();


#define ESCOOTER



volatile uint32_t countINT;

#endif /* TIMER_H_ */
