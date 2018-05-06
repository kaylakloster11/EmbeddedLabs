/*
 * uart.h
 *
 *  Created on: Sep 25, 2017
 *      Author: Kayla
 */

#ifndef UART_H_
#define UART_H_

extern volatile uint32_t countWAVE;
extern volatile float distCOUNT;
extern volatile float dist2;

#define FULLROT 7.0f //14 interrupts in one full wheel rotation
#define CIRCUM 311.93f //wheel diameter = 99.2 mm and Circumference = pi*d = 311.93
#define GETMETER 1000.0f

void UART_Configure();
void configure_clocks(void);
void configureESCOOTER(void);

void UART_send_n(uint8_t * data, uint32_t length);
void UART_send_byte(uint8_t data);

int intToStr(int x, char str[], int d);
void FloatReverse(char *str, int len);
int intToStr(int x, char str[], int d);
void ftoa(float n, char *res, int afterpoint);


int getSysTime();
float CalcDist();


#endif /* UART_H_ */
