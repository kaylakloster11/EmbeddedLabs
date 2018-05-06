/*
 * lab4.c
 *
 *  Created on: Oct 15, 2017
 *      Author: Kayla
 */


/*
 * uart.c
 *
 *  Created on: Sep 25, 2017
 *      Author: Kayla
 */

#include "msp.h"
#include <stdint.h>
#include <string.h>
#include<math.h>
#include <stdio.h>

//extern void EUSCIA0_IRQHandler(void);

void configure_clocks(void){
        CS->KEY = 0x695A; //unlock 5.3.1
        CS->CTL0 = 0 ;
        CS->CTL0 |= CS_CTL0_DCOEN | CS_CTL0_DCORSEL_2;  //enable DCO, then mode 3 for 8 to 16 Mhz
        //CS->CTL0 |=0b1100111000; //and should be 4MHZ, got 4.5 MHZ, -200.5?
        CS->CTL0 |= 0b1011010100; //goal: 4MHZ, got:  4.016MHZ          @-300
       //   CS->CTL0 |= 0b1011010001; //DCTO-303 got: 3.88Mhz
        //CS->CTL0 |= 0b0100000110;


            //we want the baud rate clock to be 4 Mhz

        /*          select clock sources:
         * SELA => ACLK source = reforclock
         * SELS => SMCLK and HSMCLK  = DCO //p 4.4 secondary mode!!
         * SELM => MCLK Source = DCO
         */

        //lets tune the DCO clock with the right frequency. goal is 4MhZ
       // uint32_t myConstant_K = TLV->DCOER_CONSTK_RSEL04; //constant for K_Constant for equation on page 312
        //uint32_t myConstant_FCAL = TLV->DCOIR_FCAL_RSEL04; // constant for F_CAL for equation


        //not a typo definitely needs to be CLT1 and not CTL0
        //lets divide the SMCLK clock by some divisor to get the create the frequency
        CS->CTL1 |= CS_CTL1_SELA__REFOCLK | CS_CTL1_SELS__DCOCLK | CS_CTL1_SELM__DCOCLK;



        CS->KEY = 0; //lock
    }
    //:TODO UART requires 4Mhz Clock so need to be sure to account for that
            // I am going to use the 4Mhz clock value for my CCR[0] value calculation


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

void UART_Configure(){
    configure_clocks();
    P1->SEL0 |= (BIT2 | BIT3);
    P1->SEL1 &= ~(BIT2 | BIT3);
    // UART must be in reset mode to configure
       EUSCI_A0->CTLW0 |= EUSCI_A_CTLW0_SWRST; //reset by setting to 1.


       EUSCI_A0->IFG = 0b0;
       EUSCI_A0->IE |= 0b01;//BIT1 | EUSCI_A_IFG_RXIFG; // set up interrupt enable for both Rx and Tx.




       EUSCI_A0->CTLW0 |= EUSCI_A_CTLW0_SSEL__SMCLK; //frame parameter , enable interrupt on the RX
       //baud rate clock is 4 Mhz
       //what register is UCBR? it needs to be set as 26 is that the word one?
       EUSCI_A0->BRW = 26; //baud rate, so baud rate set at 26 with other settings will result in a rate of 9600


       // UCOS16 = 1,          UCbRx = 26;              UCBRF = 0 ;              UCBRSx = 0xB6
       EUSCI_A0->MCTLW|=  (EUSCI_A_MCTLW_OS16); //from table 22.3.13

       EUSCI_A0->MCTLW|= ((0xB600)) ;//| (EUSCI_A_MCTLW_OS16); //from table 22.3.13


       // CLEAR UCSWRST
       EUSCI_A0->CTLW0 &= ~EUSCI_A_CTLW0_SWRST;
}

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



