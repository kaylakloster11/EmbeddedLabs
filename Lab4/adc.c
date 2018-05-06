/*
 * adc.c
 *
 *  Created on: Oct 15, 2017
 *      Author: Kayla
 */
#include "msp.h"
#include "adc.h"
#include <stdint.h>
#include "lab4.h"
#include "adc_circbuf.h"

extern float X_NADC;
extern float Y_NADC;
extern float Z_NADC;


#ifdef QUESTION6

extern CircBuf_t * buf;

void PORT1_IRQHandler(){
    int i;
    for(i=0; i<= 5000; i++);
    if(P1->IFG & (BIT1 | BIT4))  {
       ButtonEnter = 1; //initialize uart
        P1->IFG &= ~(BIT1 | BIT4); //clear flag
    }
 }

void TA0_0_IRQHandler() //basically used only to cycle led colors
{
       if (TIMER_A0->CCTL[0] & CCIFG)
       { //ABSOLUTELY necessary will not work otherwise.
           P2->OUT ^= BIT2; //Output set to High
           CB_add_item_to_buffer(buf, NADC_Value);
       }
       TIMER_A0->CTL &= ~TIMER_A_CTL_IFG; //moved here b/c i think it was clearing flags too early. only clear 1 flag a time.
       TIMER_A0->CCTL[0] &= ~CCIFG;
}
#endif


#if defined QUESTION6 || defined QUESTION4 || defined QUESTION6_NOTWORKING
void ADC14_IRQHandler() {
    if(ADC14->IFGR0 & ADC14_IFGR0_IFG0){
        NADC_Value = ADC14->MEM[0];
    }
}
#endif

#ifdef JOYSTICK
void ADC14_IRQHandler() {
   if(ADC14->IFGR0 & ADC14_IFGR0_IFG0){
        X_NADC = (float)ADC14->MEM[0];
    }
    if(ADC14->IFGR0 & ADC14_IFGR0_IFG1){
        Y_NADC = (float)ADC14->MEM[1];
    }
  }
#endif

#ifdef ACCELEROMETER
void configure_ADC()
{
    while(REF_A->CTL0 & REF_A_CTL0_GENBUSY);
    ADC14->CTL0 &= ~ADC14_CTL0_ENC;  //Disable ADC
    REF_A->CTL0 |= REF_A_CTL0_VSEL_0 | REF_A_CTL0_ON; //Enable 1.2 Voltage Reference
    REF_A->CTL0 &= ~REF_A_CTL0_TCOFF;               //Enable Temperature Sensor
    ADC14->CTL0 |= ADC14_CTL0_SHT0_3 | ADC14_CTL0_ON | ADC14_CTL0_SHP;  //Configure ADC - Pulse Sample Mode - ADC14SC - ADC ON - temperature sample period > 30us
    ADC14->CTL1 |= ADC14_CTL1_TCMAP | ADC14_CTL1_RES_3;                 //Configure Internal Temperature sensor channel, 14-bit resolution
    ADC14->MCTL[0] = ADC14_MCTLN_VRSEL_1 | ADC14_MCTLN_INCH_22;         //ADC Voltage Reference = 1.2 V, Temp Analog Channel is A22
    ADC14->IER0 |= ADC14_IER0_IE0;                    //A22, A0 Interrupt Enable

    while(!(REF_A->CTL0 & REF_A_CTL0_GENRDY));                          //Wait for Ref Generator to settle
    ADC14->CTL0 |= ADC14_CTL0_ENC;                                      //Enable ADC
    NVIC_EnableIRQ(ADC14_IRQn);
}

void config_ADC_Accel()
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

void ADC14_IRQHandler()
{
    if(ADC14->IFGR0 & ADC14_IFGR0_IFG0){
         X_NADC = (float)(ADC14->MEM[0]);
     }
    if(ADC14->IFGR0 & ADC14_IFGR0_IFG1){
          Y_NADC = (float)(ADC14->MEM[1]);
          if(Y_NADC < 7650){
                         P2->OUT = 0;
                          P2->OUT |= BIT2;
                      }
         else if(Y_NADC > 8000){
             P2->OUT = 0;
              P2->OUT |= BIT1;
          }
    }
    if(ADC14->IFGR0 & ADC14_IFGR0_IFG2){
              Z_NADC = (float)(ADC14->MEM[2]);
        }
}
#endif


#if defined QUESTION4 || defined QUESTION6 || defined QUESTION6_NOTWORKING
void configure_ADC(void){
    P8->SEL0 |= BIT3; //Set A22 temp sensor analog configuration for P8.3
    P8->SEL1 |= BIT3;
    ADC14->CTL0 &= ~ADC14_CTL0_ENC; //Disable Conversions
    //Initialize the shared reference module
    //By default, REFMSTR=1 => REFCTL is used to configure the internal reference
    while(REF_A->CTL0 & REF_A_CTL0_GENBUSY); //If ref generator busy, WAIT
        REF_A->CTL0 = REF_A_CTL0_VSEL_0 | REF_A_CTL0_ON; //Enable internal 1.2V ref
        REF_A->CTL0 &= ~REF_A_CTL0_TCOFF;  //Turn on Temp Sensor
    //Configure ADC - Pulse sample mode; ADC14SC trigger
    // ADC ON, temperature sample period>30us

#ifdef QUESTION6
/* Left and right button configure */
      P1->SEL0 &= ~(BIT1 | BIT4 | BIT7);     //Use the OR function to set BIT1 and BIT4 to general IO Mode
      P1->SEL1 &= ~(BIT1 | BIT4 |BIT7) ;     //^^^
      P1->DIR |= BIT7; //Set pin P1.7 as an output
      P1->DIR &= ~(BIT1 | BIT4);       //Direction pin is set to zero since BIT1 and BIT4 are inputs
      P1->REN |= BIT1 | BIT4 ;        //REN is set to 1 at BIT1 and BIT4 in order to enable pullup/pulldown
      P1->OUT |= BIT1 | BIT4 ;        //Output resgister is set to 1 to enable the pullup resister
      P1->IFG = 0;                    //interrupt flag is set to zero
      P1->IES |= BIT1  | BIT4;        //high-to-low transition
      P1->IE |= BIT1 | BIT4 ;          //interrupts are enabled

      P1->SEL0 &= ~BIT0; //General I/O Mode
      P1->SEL1 &= ~BIT0;
      P1->DIR |= BIT0; //Set P1.0 LED to output
      P1->OUT &= ~BIT0; //Output set to LOW
#endif


#ifdef QUESTION6_NOTWORKING
     ADC14->CTL0 = (ADC14_CTL0_SHS_1 |  ADC14_CTL0_SHP | ADC14_CTL0_SHT0_5 |
            ADC14_CTL0_ON | ADC14_CTL0_CONSEQ_2); //Configure to make analog-to-digital conversion at rising edge of our TIMERA0 clock input,
                // set to sample-and-hold pulse-mode select, sample-and-hold time is 96 ADC14CLK cycles, ADC14_CTL0_CONSEQ_2
    ADC14->CTL1 |= ADC14_CTL1_TCMAP | ADC14_CTL1_RES__14BIT ; //Conf internal temp sensor channel, set resolution
    /* Left and right button configure */
       P1->SEL0 &= ~(BIT1 | BIT4 | BIT7);     //Use the OR function to set BIT1 and BIT4 to general IO Mode
       P1->SEL1 &= ~(BIT1 | BIT4 |BIT7) ;     //^^^
       P1->DIR |= BIT7; //Set pin P1.7 as an output
       P1->DIR &= ~(BIT1 | BIT4);       //Direction pin is set to zero since BIT1 and BIT4 are inputs
       P1->REN |= BIT1 | BIT4 ;        //REN is set to 1 at BIT1 and BIT4 in order to enable pullup/pulldown
       P1->OUT |= BIT1 | BIT4 ;        //Output resgister is set to 1 to enable the pullup resister
       P1->IFG = 0;                    //interrupt flag is set to zero
       P1->IES |= BIT1  | BIT4;        //high-to-low transition
       P1->IE |= BIT1 | BIT4 ;          //interrupts are enabled
#endif

#if defined QUESTION4 || defined QUESTION6
    ADC14->CTL0 |= ADC14_CTL0_SHT0_5 | ADC14_CTL0_ON | ADC14_CTL0_SHP | ADC14_CTL0_SHS_0;
    ADC14->CTL1 |= ADC14_CTL1_TCMAP | ADC14_CTL1_RES__14BIT ; //Conf internal temp sensor channel, set resolution
#endif

    ADC14->MCTL[0] = (ADC14_MCTLN_VRSEL_0 | ADC14_MCTLN_INCH_22) ; //Map temp analog channel to MEM0/MCTL0, set 3.3v ref
    ADC14->IER0 = (ADC14_IER0_IE0); //Enable MCTL0/MEM0 Interrupts and analog pin 22
    P2->SEL0 &= ~BIT2; //gpio
    P2->SEL1 &= ~BIT2; //gpio
    P2->DIR |= BIT2;
    P2->OUT &= ~BIT2;

    while(!(REF_A->CTL0 & REF_A_CTL0_GENRDY)); //Wait for ref generator to settle
    NVIC_EnableIRQ(ADC14_IRQn); //Enable ADC int in NVIC module
}
#endif


#ifdef JOYSTICK
 void config_ADC_Stick(void){
 /* J1.2 = X
   J1.5 = Sel
   J3.26 = Y
   AnalogPin15  = P6.0 =  X = Horizontal                                   Primary
   AnalogPin9   = P4.4 =  Y = Vertical                                     Tertiary
   AnalogPin12  = P4.1 =  Select Button = press down.                      Primary */

    P6SEL1 &= ~BIT0;//Primary mode
    P6SEL0 |= BIT0;//Primary mode

    P4SEL1 &= ~BIT1; //Primary mode
    P4SEL0 |= BIT1; //Primary mode

    P4SEL1 |= BIT4; //tertiary mode
    P4SEL0 |= BIT4; //tertiary mode

    /*Set directions as input*/
    P4DIR &= ~(BIT0 | BIT2);
    P6DIR &= ~BIT1;

    ADC14->CTL0 &= ~ADC14_CTL0_ENC; //Disable Conversions

    while(REF_A->CTL0 & REF_A_CTL0_GENBUSY);
         REF_A->CTL0 = REF_A_CTL0_VSEL_0 | REF_A_CTL0_ON;
         REF_A->CTL0 &= ~REF_A_CTL0_TCOFF;

         ADC14->CTL0 = ADC14_CTL0_SHT0_5 | ADC14_CTL0_ON | ADC14_CTL0_SHP | ADC14_CTL0_CONSEQ_1; //96 clocks, turn on, and sample and hold mode, and sequence of channels
         ADC14->CTL1 |= ADC14_CTL1_RES__14BIT; //Set resolution: 16 clock cycles

         ADC14->MCTL[0] = ADC14_MCTLN_INCH_15 | ADC14_MCTLN_VRSEL_0;      //X, full range ADG (3.3 to 0v)
         ADC14->MCTL[1] = ADC14_MCTLN_INCH_9 | ADC14_MCTLN_VRSEL_0 | ADC14_MCTLN_EOS;      //Y, full range ADG (3.3 to 0v)
//         ADC14->MCTL[2] = ADC14_MCTLN_INCH_12 | ADC14_MCTLN_VRSEL_0 | ADC14_MCTLN_EOS;      //select, full range ADG (3.3 to 0v)

         // need to enable interrupt for conversion of data.
         ADC14->IER0 = (ADC14_IER0_IE0 | ADC14_IER0_IE1);

         while(REF_A->CTL0 & REF_A_CTL0_GENBUSY);
         NVIC_EnableIRQ(ADC14_IRQn);
    }

#endif



#ifdef QUESTION6

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
#endif


#ifdef QUESTION6_NOTWORKING

void CLK_init()
{
    //Set clock to 48 MHz
    CS->KEY  =   CS_KEY_VAL;             //Unlock CS registers
    CS->CTL0 = 0;
    CS->CTL0 = (CS_CTL0_DCORSEL_1);
    CS->CTL1 |=   (CS_CTL1_DIVS__8 | CS_CTL1_SELA_2 | CS_CTL1_SELS_3 | CS_CTL1_SELM_3);         //Use DCOCLK as source for MCLK, SMCLK + HSMCLK
    CS->KEY =  0;                      //Re-lock CS registers
}


void timer_a0_config(){
    P2->DIR |= BIT4;
    P2->SEL0 |= BIT4;
    P2->SEL1 &= ~BIT4;
    TIMER_A0->R = 0 ;         // Reset count
    TIMER_A0->CTL = (TIMER_A_CTL_SSEL__SMCLK | TIMER_A_CTL_ID__8 | TIMER_A_CTL_MC_1) ; //Set SMCLK Multiplexer mode clock source, divide by 8
        // and Up mode (BIT5,BIT4 = 01) and enable interrupt for timer register (enable TAIE flag)
   TIMER_A0->CCR[1] = 45874;    // Value to count to TAxCCRn holds the data for the comparison to the timer value
          //in the Timer_A Register, TAxR.
    TIMER_A0->CCR[0] = 46874;
    TIMER_A0->CCTL[1] = TIMER_A_CCTLN_OUTMOD_3;   // Capture/Compare interrupt enabled @ peripheral  (CCIE flag enabled)
    TIMER_A0->CTL &= ~(TIMER_A_CCTLN_CAP); // set BIT8 to zero to set to compare mode (CAP = 0)
        TIMER_A0->CTL &= ~(TIMER_A_CCTLN_CAP); // set BIT8 to zero to set to compare mode (CAP = 0)
}
#endif

#ifdef TEST
void CLK_init()
{
    //Set clock to 48 MHz
    CS->KEY  =   CS_KEY_VAL;             //Unlock CS registers
    CS->CTL0 = 0;
    CS->CTL0 = (CS_CTL0_DCORSEL_1);
    CS->CTL1 |=   (CS_CTL1_DIVS__8 | CS_CTL1_SELA_2 | CS_CTL1_SELS_3 | CS_CTL1_SELM_3);         //Use DCOCLK as source for MCLK, SMCLK + HSMCLK
    CS->KEY =  0;                      //Re-lock CS registers
}
void TA0_0_IRQHandler()
{
    if(TIMER_A0->CCTL[0] & TIMER_A_CCTLN_CCIFG ) {
        P1->OUT ^=  BIT0;
        TIMER_A0->CCTL[0] &=  ~TIMER_A_CCTLN_CCIFG;
    }
}

void timer_a0_config(){
    P2->DIR |= BIT4;
    P2->SEL0 |= BIT4;
    P2->SEL1 &= ~BIT4;
    TIMER_A0->R = 0 ;         // Reset count
    TIMER_A0->CTL = (TIMER_A_CTL_SSEL__SMCLK | TIMER_A_CTL_ID__8 | TIMER_A_CTL_MC_1) ; //Set SMCLK Multiplexer mode clock source, divide by 8
        // and Up mode (BIT5,BIT4 = 01) and enable interrupt for timer register (enable TAIE flag)
    TIMER_A0->CCR[1] = 23437;    // First Value to count to TAxCCRn holds the data for the comparison to the timer value
    //in the Timer_A Register, TAxR. (Gives pulse width with 50% Duty Cycle)
    TIMER_A0->CCR[0] = 46874; // Second Value to count to TAxCCRn holds the data for the comparison to the timer value
    //in the Timer_A Register, TAxR.
    TIMER_A0->CCTL[1] = TIMER_A_CCTLN_OUTMOD_6;   // Capture/Compare interrupt enabled @ peripheral  (CCIE flag enabled)
    TIMER_A0->CTL &= ~(TIMER_A_CCTLN_CAP); // set BIT8 to zero to set to compare mode (CAP = 0)
    TIMER_A0->CTL &= ~TIMER_A_CTL_IFG;  //Clear TAIFG interrupt (BIT0)

    /*CONFIGURE RED LED LIGHT*/
    P1->SEL0 &= ~BIT0; //General I/O Mode
    P1->SEL1 &= ~BIT0;
    P1->DIR |= BIT0; //Set P1.0 LED to output
    P1->OUT &= ~BIT0; //Output set to LOW
    /* Enable Interrupts in the NVIC */
    /* Enable Interrupts in the NVIC */
    TIMER_A0->CCTL[0] |= TIMER_A_CCTLN_CCIE;   // Capture/Compare interrupt enabled @ peripheral  (CCIE flag enabled)
        TIMER_A0->CTL &= ~(TIMER_A_CCTLN_CAP); // set BIT8 to zero to set to compare mode (CAP = 0)
        TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG ; //Clear CCIFG interrupt (BIT0)
        TIMER_A0->CTL &= ~TIMER_A_CTL_IFG;  //Clear TAIFG interrupt (BIT0)
        /* Enable Interrupts in the NVIC */
          NVIC_EnableIRQ(TA0_0_IRQn );
}
#endif


