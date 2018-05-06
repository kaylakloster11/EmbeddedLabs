/*
 * timer.c
 *
 *  Created on: Sep 10, 2017
 *      Author: kk
 */

#include "timer.h"
#include "msp.h"

extern uint8_t FLAG_Collect_Data;

void timer_a0_config()
{
    __disable_irq();
    TIMER_A0->R = 0;         // Reset count, set to 0 at begining
    TIMER_A0->CTL = TIMER_A_CTL_TASSEL_2|TIMER_A_CTL_ID__8| TIMER_A_CTL_MC__UP;//UP MODE, SOURCE SEL SMCLK, INTERRUPT ENABLE



    TIMER_A0->CCR[0] = 31250;// Value to count to, from lab questions 4727;
    TIMER_A0->CCTL[0] &= ~CCIFG;//clear compare captture flag
    TIMER_A0->CCTL[0] |= TIMER_A_CCTLN_CCIE;// INTERRUPT INABLE CAPTURE COMPARE
    TIMER_A0->EX0 = TIMER_A_EX0_TAIDEX_7;//divide by 8



    NVIC_EnableIRQ(TA0_0_IRQn);
}

void TA0_0_IRQHandler() //basically used only to cycle led colors
{
       if (TIMER_A0->CCTL[0] & CCIFG)
       { //ABSOLUTELY necessary will not work otherwise.
           P2->OUT ^= BIT2;
           FLAG_Collect_Data = 1;
       }
       TIMER_A0->CTL &= ~TIMER_A_CTL_IFG; //moved here b/c i think it was clearing flags too early. only clear 1 flag a time.
       TIMER_A0->CCTL[0] &= ~CCIFG;
}

//so this is the hanlder we need, but unforunately my flag isn't set still!
void TA0_N_IRQHandler()
{ //dedicated for 1-5? and so i guess Ta_0_0 is the handler specifically for cctl[0]
    P1->OUT ^= BIT7;

    if (TIMER_A0->CCTL[1] & CCIFG)
    { //ABSOLUTELY necessary will not work otherwise.

        P1->OUT ^= BIT7;
        P1->OUT ^= BIT0;

    }

    TIMER_A0->CTL &= ~TIMER_A_CTL_IFG; //moved here b/c i think it was clearing flags too early. only clear 1 flag a time.

    TIMER_A0->CCTL[1] &= ~CCIFG;

}
