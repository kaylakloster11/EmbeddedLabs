/*
 * uart.c
 *
 *  Created on: Sep 24, 2017
 *      Author: kevinKuwata1
 */

#include <circ_buffer_basic.h>
#include "uart.h"
#include "msp.h"
#include "math.h"

#define BLUE_LED            (BIT2)

    extern CircBuf_t * myBufferPTR;

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


void UART_config(){

    configure_clocks();
/*  Steps for Setting up UART Table 22.3.1
 *      Set UCSWRST
 *      Initialize all registers with UCSWRT = 1
 *      Configure Ports
 *      Clear UCSWST with software, aka clear that 1.
 *      Enable interrupts for UCRxie and UCTxie
 *
 *
 * Resets as all 0's so only the clock sources is the
 *  that need to be changed will change.
 *
 *  "Frame parameters" means the parity, stop bits, etc. but cuz all reset as 0's
 *  no worries.
 * */

//Set up pins.
    //Pin 2 is the RX           pin 3 is the TX
    //RX SETUP
    P1SEL0 |= BIT2;
    P1SEL1 &= ~BIT2;

    //TX SETUP
    P1SEL0 |= BIT3;
    P1SEL1 &= ~BIT3;


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


/*
 * Place Load data into Tx Buffer
 * */
void UART_putchar(uint8_t tx_data){
   while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
    EUSCI_A0->TXBUF = tx_data; //hoping 30 should be '0', for sure ascii
   // EUSCI_A_ifg
}

/*
 *  Iterates through the array, puts into the Tx Buffer
 * */
void UART_putchar_n(uint8_t * data, uint32_t length){
    //data is an array! so you can use pointer math to iterate through
   uint8_t index =0;

   for(index = 0; index <length ; index++){
       UART_putchar(data[index]);
      // for(txDelay = 0; txDelay < 300; txDelay++);
   }
   UART_putchar(13);

}




 void reverse(char str[], int len)        //function to reverse string to get it readable and not backwards
{
    int start, end;
    char temp;
    for (start=0, end = len-1; start<end; start++, end--)
    {
        temp = *(str+start);
        *(str+start)= *(str+end);
        *(str+end) = temp;
    }
}

 int intToStr(int value, char str[], int length)
{
    int i = 0;
    while (value)
    {
        str[i++] = (value%10) + '0';
        value = value/10;
    }
    while (i < length)
    {
        str[i++] = '0';
    }
    reverse(str,i);
    str[i] = '\0';
    return i;
}

extern char itoa (int value, char str[], int base)           //integer to string function
{
    int  i = 0;
    int sign;

    if ((sign = value) < 0){        //What to do if values is negative
        value = -value;         //make value positive so we can work with
    }

    do{
        str[i++] = value % base + '0';
    }while ((value /= base) > 0);

    if (sign < 0){
        str[i++] = '-';
    }
    str[i] = '\0';
    reverse(str,i);

}

extern char ftoa(float f, char str[], int afterpoint)           //float to string function
{
    int ipart = (int)f;

    float fpart = f - (float)ipart;

    int i = intToStr(ipart, str, 0);

    if (afterpoint != 0)
    {
        str[i] = '.';

        fpart = fpart * pow(10,afterpoint);

        intToStr((int)fpart, str + i + 1, afterpoint);
    }
}








/*Interrupt for UART Rx and TX IRQ */
extern void EUSCIA0_IRQHandler(){
    uint16_t delay;

    if(EUSCI_A0->IFG & EUSCI_A_IFG_RXIFG){
        //when a bit is received the RXIF flag is high.
        P2OUT ^= BLUE_LED;
               for(delay =0; delay<200; delay++);
               P2OUT ^= BLUE_LED;
               EUSCI_A0->IFG &= ~EUSCI_A_IFG_RXIFG;//clear the flag.

               //push into the buffer.
               add_To_Buffer(&myBufferPTR, EUSCI_A0->RXBUF);

               //echo that shit back
               //EUSCI_A0->TXBUF = remove_From_Buffer(&myBufferPTR);
#ifdef ECHO_TEST
               EUSCI_A0->TXBUF = EUSCI_A0->RXBUF;
#endif
    }


    if(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG){
    }
}

