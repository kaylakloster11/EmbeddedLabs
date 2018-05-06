/*
 * ESCOOTER.c
 *
 *  Created on: Nov 1, 2017
 *      Author: Kayla
 */


#include <CIRCBUFescooter.h>
#include <ESCOOTER.h>
#include "msp.h"
#include <stdint.h>
#include <stdio.h>

extern volatile CircBuf_t * pointer1;
extern volatile CircBuf_t * pointer2;
extern volatile CircBuf_t * pointer3;

extern volatile char res[5];
extern volatile char res1[5];
extern volatile char res2[5];

extern volatile float distance;
extern volatile float distance2;

extern volatile uint8_t print;

extern volatile float distCOUNT;


void EUSCIA0_IRQHandler(void)
{
    if(EUSCI_A0->IFG & EUSCI_A_IFG_RXIFG)
    {
       EUSCI_A0->TXBUF = EUSCI_A0->RXBUF;
    }
}

void TA1_0_IRQHandler()
{
    if(TIMER_A1->CCTL[0] & TIMER_A_CCTLN_CCIFG)
    {
       TIMER_A1->CCTL[0] &= ~(TIMER_A_CCTLN_CCIFG);
       print = 1;
    }
}

float CalcDist() {
    float getRev = (distCOUNT)/FULLROT; //getRev = number of wheel rotations
               // number of wheel rotations = (# interrupts/(14 interrupts in on wheel rotation))
    float dist = getRev * CIRCUM; //multiply # wheel rotations by our wheel circumference to get the distance traveled (in mm)
    dist = dist/GETMETER; //divide by 1000 to conver to meters
    return dist;
 }

uint32_t getSysTime(){
    uint32_t count = SysTick->VAL;
    return count;
}

float getVelocity(uint32_t time){
    uint32_t diff = (SysTick -> LOAD) - time;
    float sec = diff/FREQ;
    float distanceDIFF = distance - distance2;
    float speed = (distanceDIFF)/sec;
    return speed;
}

void UART_Configure()
{
    EUSCI_A0->CTLW0 |= EUSCI_A_CTLW0_SWRST; //eUSCI in Reset
    P1->SEL0 |= BIT2 | BIT3; //Primary mode for P1.2 and P1.3
    P1->SEL1 &= ~(BIT2 | BIT3);
    EUSCI_A0->CTLW0 |= EUSCI_A_CTLW0_MODE_0; // UART Mode
    EUSCI_A0->CTLW0 &= ~EUSCI_A_CTLW0_MSB; //LSB first
    EUSCI_A0->CTLW0 &= ~EUSCI_A_CTLW0_SEVENBIT; //7 bit data
    EUSCI_A0->CTLW0 &= ~EUSCI_A_CTLW0_SPB; //1 stop bit
    EUSCI_A0->CTLW0 |= EUSCI_A_CTLW0_UCSSEL_2; //Clock source: SMCLK
    EUSCI_A0->BRW = 1;
    EUSCI_A0->MCTLW |= EUSCI_A_MCTLW_OS16 | (0x00A0) | (0x0100); //Oversampling, BRF = 10, BRS = 0.0529
    EUSCI_A0->CTLW0 &= ~EUSCI_A_CTLW0_SWRST; // Initialize eUSCI

    EUSCI_A0->IFG = 0;
    EUSCI_A0->IE |= BIT0;

    NVIC_EnableIRQ(EUSCIA0_IRQn);
}

void GPIO_Configure()
{
    //Encoder Configs
       P1->SEL0 &= ~BIT6;
       P1->SEL1 &= ~BIT6;
       P1->DIR &= ~BIT6;
       P1->OUT &= ~BIT6;
       P1->IES |= BIT6;
       P1->IFG &= ~BIT6;
       P1->IE |= BIT6;


    /* P1.0 LED */
     P1->SEL0 &= ~BIT0;
     P1->SEL1 &= ~BIT0;
     P1->DIR |= BIT0;
     P1->OUT &= ~BIT0;

     //Enable Interrupts in the NVIC
     NVIC_EnableIRQ(PORT1_IRQn);
     NVIC_SetPriority(PORT1_IRQn, 1);
     P1->IFG = 0;
}

void config_SYSTICK()
{
    SysTick->CTRL |= (SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk);
    SysTick->LOAD |= 300299; //SysTick_LOAD_RELOAD_Msk; //SysTick_LOAD_RELOAD_Msk;
                  /* Enable Interrupts in the NVIC */
}

void PORT1_IRQHandler()
{
        if(P1->IFG & BIT6)
        {
          distCOUNT++;
          P1->OUT ^= BIT0;
          P1->IFG &= ~BIT6;
        }
}

void UART_send_byte(uint8_t data)
{
    while((EUSCI_A0->STATW & EUSCI_A_STATW_BUSY));
    EUSCI_A0->TXBUF = data;
}

void UART_send_n(uint8_t * data, uint32_t length)
{
    unsigned int i=0;
    for(i = 0; i < length; i++)
    {
        if(!data[i])
        {
            UART_send_byte(0x20);
        }
        else
        {
            UART_send_byte(data[i]);
        }
    }
}


void timer_a1_config()
{

    TIMER_A1->R = 0; //Clears the Timer A0 register
    TIMER_A1->CTL |= TIMER_A_CTL_TASSEL_2 | TIMER_A_CTL_ID__8 |TIMER_A_CTL_MC__UP |TIMER_A_CTL_IE;
    TIMER_A1->CTL &= ~(TIMER_A_CCTLN_CAP);
    TIMER_A1->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
    TIMER_A1->CTL &= ~(TIMER_A_CTL_IFG);

    /*TIMER_A_CTL_TASSEL_2 -- sets the clock source as SMCLK
      TIMER_A_CTL_MC_UP -- Up mode: Timer counts up to TAxCCR0
      TIMER_A_CTL_ID_0 -- Clock Divider - divides clock by 1
      TIMER_A_CTL_IE -- Enables interrupt for TIMER A0
    */

    TIMER_A1->EX0 |= TIMER_A_EX0_IDEX__8; //Divides the clock again but this time by 2

    TIMER_A1->CCR[0] = 4687; //Counter value
    TIMER_A1->CCTL[0] |= TIMER_A_CCTLN_CCIE; // Capture/Compare interrupt enabled


    /*Enable Interrupts in NVIC*/
    NVIC_EnableIRQ(TA1_0_IRQn);
}

/*code source is https://en.wikibooks.org/wiki/C_Programming/stdlib.h/itoa */

void reverse(char s[], int strlength)
{
    int i, j;
    char c;

    for(i = 0, j = strlength-1; i < j; i++, j--)
    {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

/*code source http://www.geeksforgeeks.org/convert-floating-point-number-string/ */

void ftoa(float n, char * res, int afterpoint)
{
    //Extract integer part
    int ipart = (int)n;

    //Extract floating part
    float fpart = n - (float)ipart;

    //Convert integer part to string
    int pos = itoa(ipart, res);

    //check for display option after point
    if(afterpoint != 0)
    {
        res[pos] = '.';   //add dot
        /*
         Get the value of the fraction part up to giver number
         of point after dot. The third parameter is needed
         to handle cases like 233.007
         */
        int temp = 1;
        int i = 0;
        for(i = 0; i < afterpoint; i++)
        {
            temp *= 10;
        }

        fpart = fpart * temp;

        if(fpart < 0)
        {
            fpart = abs(fpart);
        }

        itoa((int)fpart, res + pos + 1);
    }
}

/*code source is https://en.wikibooks.org/wiki/C_Programming/stdlib.h/itoa */
int8_t itoa(int n, char s[])
{
    int i, sign;

    if((sign = n) < 0)  /* Record sign */
        n = -n;         /* make n positive */
    i = 0;
    do {                /* generate digits in reverse order */
        s[i++] = n % 10 + '0';   /* get next digit*/
    }  while ((n /= 10) > 0);    /* delete it */
    if (sign < 0)
        s[i++] = '-';
    s[i] = '\0';
    reverse(s, i);

    return i;
}
