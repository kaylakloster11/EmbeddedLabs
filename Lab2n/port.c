/*
 * port.c
 *
 *  Created on: Sep 11, 2017
 *      Author: Kayla
 */

#include "msp.h"
#include "timer.h"
#include "port.h"
#include <stdint.h>

extern volatile uint32_t countWAVE;
extern volatile float distCOUNT;
volatile float dist2;

#ifdef SYSTICKtimer
    int getSysTime() {
        uint32_t count = SysTick->VAL;
        return count;
    }

    void PORT1_IRQHandler(){
         if(P1->IFG & BIT7) {
             P1->IFG &= ~BIT7;
         }
     }

#endif

#ifdef TimeButtonCYCLE
    void PORT1_IRQHandler(){
         int i;
         for(i=0; i<= 50000; i++); //delay to allow for debouncing */

           P1->IFG &= 0; //clear flag
           P1->IE &= ~(BIT1 | BIT4) ;
           timer_a0_config();
   }
#endif


#ifdef RGBled
 void PORT1_IRQHandler(){
    int i;
    for(i=0; i<= 50000; i++);
    if(P1->IFG & BIT1)  {
        P2->OUT = (P2->OUT + 1)%8;
        P1->IFG &= ~BIT1; //clear flag
    }
    if(P1->IFG & BIT4) {
        P2->OUT = (P2->OUT + 7 )%8;
        P1->IFG &= ~BIT4; //clear flag
    }
}
#endif


#ifdef LEDtest
  void PORT1_IRQHandler(){
     int i;
     for(i=0; i<= 50000; i++); //delay to allow for debouncing
     if(P1->IFG & BIT1)  {
        P1->OUT ^= BIT0; //toggle led
        P1->IFG &= ~BIT1; //clear flag
    }
     if(P1->IFG & BIT4) {
         P1->OUT ^= BIT0; //toggle led
         P1->IFG &= ~BIT4; //clear flag
     }
}
#endif

#ifdef MANUALtest
  void PORT1_IRQHandler(){
      if(P1->IFG & BIT7) {
          P1->OUT ^= BIT7;
          P1->IFG &= ~BIT7;
      }
  }
#endif

#ifdef Encoder
   void PORT1_IRQHandler(){
        if(P1->IFG & BIT5)  {
            countWAVE++;
            P1->IFG &= ~BIT5; //clear flag
        }
   }
#endif

#ifdef IRBeam
   void PORT1_IRQHandler(){
        if(P1->IFG & BIT5)  {
            P1->OUT ^= BIT0;
            P1->IFG &= ~BIT5; //clear flag
         }
   }
#endif

#ifdef ESCOOTER
   float CalcDist() {
       float getRev = distCOUNT/FULLROT; //getRev = number of wheel rotations
           // number of wheel rotations = (# interrupts/(14 interrupts in on wheel rotation))
       float dist = getRev * CIRCUM; //multiply # wheel rotations by our wheel circumference to get the distance traveled (in mm)
       dist = dist/GETMETER; //divide by 1000 to conver to meters
       return dist;
   }

   void PORT1_IRQHandler(){
       if(P1->IFG & BIT5)  {
           distCOUNT++;
           P1->OUT ^= BIT0;
           dist2 = CalcDist();
           P1->IFG &= ~BIT5; //clear flag
        }
   }
#endif

// Configure the GPIO
void GPIO_configure(void) {

    /*Reset All Configurations*/

#ifdef RGBled
    /* Left and right button configure */
       P1->SEL0 &= ~(BIT1 | BIT4);     //Use the OR function to set BIT1 and BIT4 to general IO Mode
       P1->SEL1 &= ~(BIT1 | BIT4) ;     //^^^
       P1->DIR &= ~(BIT1 | BIT4);       //Direction pin is set to zero since BIT1 and BIT4 are inputs
       P1->REN |= BIT1 | BIT4 ;        //REN is set to 1 at BIT1 and BIT4 in order to enable pullup/pulldown
       P1->OUT |= BIT1 | BIT4 ;        //Output resgister is set to 1 to enable the pullup resister
       P1->IFG = 0;                    //interrupt flag is set to zero
       P1->IE |= BIT1 | BIT4 ;          //interrupts are enabled
       P1->IES |= BIT1  | BIT4;        //high-to-low transition
       P2->SEL0 &= ~(BIT0 | BIT2| BIT1) ;     //GENERAL I/O for RED and BLUE
       P2->SEL1 &= ~(BIT0 | BIT2 | BIT1) ;     //^^^
       P2->OUT &= ~(BIT0 | BIT2 | BIT1) ;      //Direction is set to 1 so the LEDs act as outputs
       P2->DIR |= (BIT0 | BIT2 | BIT1);         //Output is set to low so the LED will only turn on when the interrupt is called
        /* Enable Interrupts in the NVIC */
       NVIC_EnableIRQ(PORT1_IRQn);
#endif

#ifdef Encoder
    //data from encoder
      P1->SEL0 = ~BIT5;
      P1->SEL1 = ~BIT5;
      P1->DIR &= ~BIT5;
      P1->REN |= BIT5;
      P1->OUT |= BIT5;
      P1->IFG = 0;
      P1->IES |= BIT5;
      P1->IE |= BIT5;
      NVIC_EnableIRQ(PORT1_IRQn);
#endif

    /*RED LED on Port 1 configuration*/
#ifdef LEDtest
/* Left and right button configure */
    P1->SEL0 &= ~(BIT1 | BIT4);     //Use the OR function to set BIT1 and BIT4 to general IO Mode
    P1->SEL1 &= ~(BIT1 | BIT4) ;     //^^^
    P1->DIR &= ~(BIT1 | BIT4);       //Direction pin is set to zero since BIT1 and BIT4 are inputs
    P1->REN |= BIT1 | BIT4 ;        //REN is set to 1 at BIT1 and BIT4 in order to enable pullup/pulldown
    P1->OUT |= BIT1 | BIT4 ;        //Output resgister is set to 1 to enable the pullup resister
    P1->IFG = 0;                    //interrupt flag is set to zero
    P1->IE |= BIT1 | BIT4 ;          //interrupts are enabled
    P1->IES |= BIT1  | BIT4;        //high-to-low transition

    /*CONFIGURE RED LED LIGHT*/
    P1->SEL0 &= ~BIT0; //General I/O Mode
    P1->SEL1 &= ~BIT0;
    P1->DIR |= BIT0; //Set P1.0 LED to output
    P1->OUT &= ~BIT0; //Output set to LOW
    /* Enable Interrupts in the NVIC */
       NVIC_EnableIRQ(PORT1_IRQn);

#endif

#ifdef SYSTICKtimer
      P1->SEL0 &= ~BIT7;     //set to general IO Mode
      P1->SEL1 &= ~BIT7;     //^^^
      P1->DIR |= BIT7; //Set pin P1.7 as an output
      P1->IFG = 0;                    //interrupt flag is set to zero
      P1->IE |= BIT7;          //interrupts are enabled
      SysTick->CTRL |= (SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk);
      SysTick->LOAD |= SysTick_LOAD_RELOAD_Msk; //SysTick_LOAD_RELOAD_Msk;
              /* Enable Interrupts in the NVIC */
      NVIC_EnableIRQ(PORT1_IRQn);

#endif

#ifdef MANUALtest
    /* PIN7 configure */
      P1->SEL0 &= ~BIT7;     //set to general IO Mode
      P1->SEL1 &= ~BIT7;     //^^^
      P1->DIR |= BIT7; //Set pin P1.7 as an output
      P1->IFG = 0;                    //interrupt flag is set to zero
      P1->IE |= BIT7;          //interrupts are enabled
    /* Enable Interrupts in the NVIC */
      NVIC_EnableIRQ(PORT1_IRQn);
      P1->OUT |= BIT7 ;  //Set output to high
#endif

#ifdef FirstTimer

    /*CONFIGURE RED LED LIGHT AND PIN7*/
    P1->SEL0 &= ~(BIT0 | BIT7); //General I/O Mode
    P1->SEL1 &= ~(BIT0 | BIT7);
    P1->DIR |= (BIT0 | BIT7); //Set P1.0 LED to output
    P1->OUT &= ~BIT0; //Output set to LOW
    /* PIN7 configure */
           P1->OUT |= BIT7 ;  //Set output to high
#endif

#ifdef TimeButtonCYCLE
 /* Left and right button configure */
    P1->SEL0 &= ~(BIT1 | BIT4);     //Use the OR function to set BIT1 and BIT4 to general IO Mode
    P1->SEL1 &= ~(BIT1 | BIT4) ;     //^^^
    P1->DIR &= ~(BIT1 | BIT4);       //Direction pin is set to zero since BIT1 and BIT4 are inputs
    P1->REN |= BIT1 | BIT4 ;        //REN is set to 1 at BIT1 and BIT4 in order to enable pullup/pulldown
    P1->OUT |= BIT1 | BIT4 ;        //Output resgister is set to 1 to enable the pullup resister
    P1->IFG = 0;                    //interrupt flag is set to zero
    P1->IE |= BIT1 | BIT4 ;          //interrupts are enabled
    P1->IES |= BIT1  | BIT4;        //high-to-low transition
 /* PIN7 configure */
    P1->SEL0 &= ~BIT7; //set to general IO Mode
    P1->SEL1 &= ~BIT7; //^^^
    P1->DIR |= BIT7; //Set pin P1.7 as an output
    P1->IFG = 0;    //interrupt flag is set to zero
    P1->IE |= BIT7;  //interrupts are enabled
    P2->SEL0 &= ~(BIT0 | BIT2| BIT1) ;   //GENERAL I/O for RED and BLUE
    P2->SEL1 &= ~(BIT0 | BIT2 | BIT1) ; //^^^
    P2->OUT &= ~(BIT0 | BIT2 | BIT1) ;  //Direction is set to 1 so the LEDs act as outputs
    P2->DIR |= (BIT0 | BIT2 | BIT1);    //Output is set to low so the LED will only turn on when the interrupt is called
                      /* Enable Interrupts in the NVIC */
    NVIC_EnableIRQ(PORT1_IRQn);
    P1->OUT |= BIT7 ;  //Set output to high
#endif

#ifdef IRBeam
 //data from encoder
     P1->SEL0 = ~BIT5;
     P1->SEL1 = ~BIT5;
     P1->DIR &= ~BIT5;
     P1->REN |= BIT5;
     P1->OUT |= BIT5;
     P1->IFG = 0;
     P1->IES |= BIT5;
     P1->IE |= BIT5;
 /*CONFIGURE RED LED LIGHT*/
     P1->SEL0 &= ~BIT0; //General I/O Mode
     P1->SEL1 &= ~BIT0;
     P1->DIR |= BIT0; //Set P1.0 LED to output
     P1->OUT &= ~BIT0; //Output set to LOW
     NVIC_EnableIRQ(PORT1_IRQn);
#endif

#ifdef ESCOOTER
 //data from encoder
     P1->SEL0 = ~BIT5;
     P1->SEL1 = ~BIT5;
     P1->DIR &= ~BIT5;
     P1->REN |= BIT5;
     P1->OUT |= BIT5;
     P1->IFG = 0;
     P1->IES |= BIT5;
     P1->IE |= BIT5;
 /*CONFIGURE RED LED LIGHT*/
     P1->SEL0 &= ~BIT0; //General I/O Mode
     P1->SEL1 &= ~BIT0;
     P1->DIR |= BIT0; //Set P1.0 LED to output
     P1->OUT &= ~BIT0; //Output set to LOW
     NVIC_EnableIRQ(PORT1_IRQn);
#endif

}

