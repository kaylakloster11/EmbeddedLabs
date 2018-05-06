/*
 * ADCescooter.c
 *
 *  Created on: Nov 1, 2017
 *      Author: Kayla
 */

#include <ADCescooter.h>
#include "msp.h"

extern volatile uint16_t Y_NADC;
extern volatile uint16_t X_NADC;

extern volatile float velocity;

void ADC14_IRQHandler()
{
    if(ADC14->IFGR0 & ADC14_IFGR0_IFG0){
         X_NADC = (float)(ADC14->MEM[0]);
     }
    if(ADC14->IFGR0 & ADC14_IFGR0_IFG1){
          Y_NADC = (float)(ADC14->MEM[1]);

          if(velocity == 0){
              P2->OUT = 0;
            P2->OUT |= BIT0;
                 }
          else if(Y_NADC > 8000){
             P2->OUT = 0;
              P2->OUT |= BIT2;
                      }
         else if(Y_NADC < 7400){
             P2->OUT = 0;
              P2->OUT |= BIT1;
          }
    }
    if(ADC14->IFGR0 & ADC14_IFGR0_IFG2){
              uint16_t Z_NADC = (float)(ADC14->MEM[2]);
        }
}


void configure_ADC_Accelerometer()
{

            /* P2.1 GREEN LED */
                    P2->SEL0 &= ~BIT1;
                    P2->SEL1 &= ~BIT1;
                    P2->DIR |= BIT1;
                    P2->OUT &= ~BIT1;

                    /* P2.1 BLUE LED */
                    P2->SEL0 &= ~BIT2;
                    P2->SEL1 &= ~BIT2;
                    P2->DIR |= BIT2;
                    P2->OUT &= ~BIT2;

                    /* P2.0 RED LED */
                    P2->SEL0 &= ~BIT0;
                    P2->SEL1 &= ~BIT0;
                    P2->DIR |= BIT0;
                    P2->OUT &= ~BIT0;

                    P6SEL1 &= ~BIT1;
                    P6SEL0 |= BIT1;

                  P4SEL1 &= ~BIT0;
                  P4SEL0 |= BIT0;

                  P4SEL1 |= BIT2;
                  P4SEL0 |= BIT2;

                  //Set to Inputs
                  P4DIR &= ~(BIT0 | BIT2);
                  P6DIR &= ~BIT1;

                while(REF_A->CTL0 & REF_A_CTL0_GENBUSY);
                   REF_A->CTL0 = REF_A_CTL0_VSEL_0 | REF_A_CTL0_ON;
                   REF_A->CTL0 &= ~REF_A_CTL0_TCOFF;

                   ADC14->CTL0 = ADC14_CTL0_SHT0__96 | ADC14_CTL0_ON | ADC14_CTL0_SHP | ADC14_CTL0_CONSEQ_1;                            //96 clocks, turn on, sample and hole, sequence
                   ADC14->CTL1 |= ADC14_CTL1_RES__14BIT;

                   ADC14->MCTL[0] = ADC14_MCTLN_INCH_14 | ADC14_MCTLN_VRSEL_0;                                            //Xaxis
                   ADC14->MCTL[1] = ADC14_MCTLN_INCH_13 | ADC14_MCTLN_VRSEL_0 | ADC14_MCTLN_EOS;      //Zaxis

               ADC14->IER0 = (ADC14_IER0_IE0 | ADC14_IER0_IE1 | ADC14_IER0_IE2);

               while(REF_A->CTL0 & REF_A_CTL0_GENBUSY);
               ADC14->CTL0 |= ADC14_CTL0_ENC;
               NVIC_EnableIRQ(ADC14_IRQn);     //a flag ADC14IFGx is set when x has a conversion result.


   }


