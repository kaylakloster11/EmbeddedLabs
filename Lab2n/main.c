#include <stdint.h>


#include "msp.h"
#include "port.h"
#include "timer.h"

volatile uint32_t countWAVE = 0;
volatile float distCOUNT = 0;


void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;           // Stop watchdog

GPIO_configure();

#ifdef FirstTimer
    timer_a0_config();
#endif

#ifdef MANUALtest
    P1->IFG |= BIT7;
#endif

#ifdef SYSTICKtimer
    uint32_t time1, time2,diff;
    time1 = getSysTime(); // This function is also very simple
    P1->IFG |= BIT7;
    time2 = getSysTime();
    diff = time1 - time2; //This is slightly more complex if we have an overflow
#endif

   __enable_irq();

       while (1);




}


