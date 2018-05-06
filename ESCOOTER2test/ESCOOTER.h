/*
 * ESCOOTER.h
 *
 *  Created on: Nov 1, 2017
 *      Author: Kayla
 */

#ifndef ESCOOTER_H_
#define ESCOOTER_H_

typedef struct
{
    uint16_t distance;
    uint16_t avg_velocity;
    uint16_t direction;
} ScooterInfo_t;

#define FULLROT 14.0f //14 interrupts in one full wheel rotation
#define CIRCUM 311.93f //wheel diameter = 99.2 mm and Circumference = pi*d = 311.93
#define GETMETER 1000.0f
#define FREQ    3000000.0f


float getVelocity(uint32_t time);
uint32_t getSysTime();
float CalcDist();

void config_SYSTICK();

void UART_Configure();
void timer_a1_config();
void GPIO_Configure();
void UART_send_byte(uint8_t data);
void UART_send_n(uint8_t * data, uint32_t length);
void reverse(char s[], int strlength);
void ftoa(float n, char * res, int afterpoint);
int8_t itoa(int n, char s[]);

#endif /* ESCOOTER_H_ */
