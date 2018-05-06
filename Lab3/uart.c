/*
 * uart.c
 *
 *  Created on: Sep 25, 2017
 *      Author: Kayla
 */

#include "msp.h"
#include "uart.h"
#include "circbuf.h"
#include <stdint.h>
#include <string.h>
#include<math.h>

extern void EUSCIA0_IRQHandler(void);

#if defined QUESTION3 || defined QUESTION5 || defined QUESTION10 || defined QUESTION11 || defined ESCOOTER
void UART_send_byte(uint8_t data){
   while((EUSCI_A0->STATW & EUSCI_A_STATW_BUSY)); //May have to change to using the TXIFG flag
        EUSCI_A0->TXBUF = data;
}

void UART_send_n(uint8_t * data, uint32_t length){
    int i = 0;
    for(i=0 ; i<length ; i++){
        UART_send_byte(data[i]);
    }
}
#endif

#if defined QUESTION7 || defined QUESTION11 || defined ESCOOTER
/* We used the website http://www.geeksforgeeks.org for our ftoa and itoa functions, and also used this website to obtain our reverse and FloatReverse functions.
 * We altered some, but not much of the code.
 */

void reverse(char s[])
     {
         int i, j;
         char c;
         for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
             c = s[i];
             s[i] = s[j];
             s[j] = c;
         }
     }

void itoa(int n, char s[])
{
    int i, sign;

    if ((sign = n) < 0)  /* record sign */
        n = -n;          /* make n positive */
    i = 0;
    do {
        s[i++] = n % 10 + '0';
    } while ((n /= 10) > 0);
    if (sign < 0)
        s[i++] = '-';
    s[i] = '\0';
    reverse(s);
}

void FloatReverse(char *str, int len)
{
    int i=0, j=len-1, temp;
    while (i<j)
    {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++; j--;
    }
}

// Converts a given integer x to string str[].  d is the number
 // of digits required in output. If d is more than the number
 // of digits in x, then 0s are added at the beginning.
int intToStr(int x, char str[], int d)
{
    int i = 0;
    while (x)
    {
        str[i++] = (x%10) + '0';
        x = x/10;
    }

    // If number of digits required is more, then
    // add 0s at the beginning
    while (i < d)
        str[i++] = '0';

    FloatReverse(str, i);
    str[i] = '\0';
    return i;
}
// Converts a floating point number to string.
void ftoa(float n, char *res, int afterpoint)
{
    // Extract integer part
    int ipart = (int)n;

    // Extract floating part
    float fpart = n - (float)ipart;

    // convert integer part to string
    int i = intToStr(ipart, res, 0);

    // check for display option after point
    if (afterpoint != 0)
    {
        res[i] = '.';  // add dot

        // Get the value of fraction part upto given no.
        // of points after dot. The third parameter is needed
        // to handle cases like 233.007
        fpart = fpart * pow(10, afterpoint);

        intToStr((int)fpart, res + i + 1, afterpoint);
    }
}

#endif

void UART_Configure(){
    P1->SEL0 |= (BIT2 | BIT3);
    P1->SEL1 &= ~(BIT2 | BIT3);
    EUSCI_A0->CTLW0 &= ~(EUSCI_A_CTLW0_PEN | EUSCI_B_CTLW0_MSB | EUSCI_B_CTLW0_SEVENBIT | EUSCI_A_CTLW0_SPB | EUSCI_B_CTLW0_MODE_MASK); //Set No Parity, LSB First, 8-bit data, 1 start bit, 1 stop bit
    /*set baud rate = 38400 */
    EUSCI_A0->CTLW0 |= EUSCI_A_CTLW0_SSEL__SMCLK;
    EUSCI_A0->BRW = 4;
    EUSCI_A0->MCTLW |= (EUSCI_A_MCTLW_OS16 /* Oversampling mode enabled */ | (EUSCI_A_MCTLW_BRF_MASK & 0x00E0) /*same as ORing w/ 0x00E0, but wanted to include Mask, so know...
         what this configuring, and can easily debug to make sure configuring right bits*/ | (EUSCI_A_MCTLW_BRS_MASK & 0x1000)); /*Used masks when not necessary...
         so I know what I'm doing and easy to debug by holding cursor over mask*/
    EUSCI_A0->IFG = 0;
    EUSCI_A0->CTLW0 &= ~EUSCI_A_CTLW0_SWRST; //Initialize eUSCI */
    EUSCI_A0->IE |= (EUSCI_A_IE_RXIE);

    NVIC_EnableIRQ(EUSCIA0_IRQn);
}

#ifdef ESCOOTER
float CalcDist() {
      float getRev = distCOUNT/FULLROT; //getRev = number of wheel rotation
           // number of wheel rotations = (# interrupts/(14 interrupts in on wheel rotation))
       float dist = getRev * CIRCUM; //multiply # wheel rotations by our wheel circumference to get the distance traveled (in mm)
       dist = dist/GETMETER; //divide by 1000 to convert to meters
       return dist;
   }

#endif
