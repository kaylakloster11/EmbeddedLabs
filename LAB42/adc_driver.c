/*
 * adc_driver.c
 *
 *  Created on: Oct 11, 2017
 *      Author: kk
 */

#include "msp.h"
#include "lab4.h"

/*  GLOBALS     */
   extern uint16_t X_ADC;
   extern uint16_t Y_ADC;
   extern uint16_t Z_ADC;

   extern uint16_t NADC_Temperature;


void ADC_CONFIG(){

#ifdef ADC_PORT5_PIN4
    P5SEL0 = BIT4 ;  //A1
    P5SEL1 = BIT4;  //A1
    P5->DIR &= ~BIT4; //INPUT
    //pull it down.
  //  P5->REN |= BIT4;
  //  P5->OUT |= BIT4;


    while(REF_A->CTL0 & REF_A_CTL0_GENBUSY);
    REF_A->CTL0 |= REF_A_CTL0_VSEL_3 | REF_A_CTL0_ON;
    REF_A->CTL0 |=REF_A_CTL0_TCOFF;


    ADC14->CTL0 |= ADC14_CTL0_SHT0_5 | ADC14_CTL0_ON | ADC14_CTL0_SHP;
    ADC14->CTL1 |= ADC14_CTL1_RES_3; //16 BIT GLORY

    ADC14->MCTL[1] |= ADC14_MCTLN_INCH_1; //MAP MEMORY TO CHANNEL a1

    ADC14->IER0 |= ADC14_IER0_IE1; //INTERRUPT ON MEMORY MAP 1

    while(!(REF_A->CTL0 & REF_A_CTL0_GENRDY));
    ADC14->CTL0 |= ADC14_CTL0_ENC;



    NVIC_EnableIRQ(ADC14_IRQn); //a flag ADC14IFGx is set when x has a conversion result.

#endif

#ifdef INTERNAL_TEMPERATURE_READ
    //Pins for temperature sensor Analog 22, port 8 pin 3.
    //P8SEL0 = BIT3;
    //P8SEL1 = BIT3;
    //Do I need to make port direction input? what about pull up resistor?

    while(REF_A->CTL0 & REF_A_CTL0_GENBUSY);
    REF_A->CTL0 = REF_A_CTL0_VSEL_0 | REF_A_CTL0_ON;
    REF_A->CTL0 &= ~REF_A_CTL0_TCOFF;

    ADC14->CTL0 = ADC14_CTL0_SHT0__96 | ADC14_CTL0_ON | ADC14_CTL0_SHP; //96 clocks, turn on, and sample and hold mode
    ADC14->CTL1 |= ADC14_CTL1_TCMAP | ADC14_CTL1_RES__14BIT; //temp sensor, set resolution: 16 clock cycles

    ADC14->MCTL[0] = ADC14_MCTLN_INCH_22 | ADC14_MCTLN_VRSEL_1; //MAP the temp to the first block of memory temperature sensor analog pin, table 6-52 from overview 100 pin PZ devices

    // need to enable interrupt for conversion of data.
    ADC14->IER0 = ADC14_IER0_IE0; //interrupt for mem[0]

    while(REF_A->CTL0 & REF_A_CTL0_GENBUSY);
    ADC14->CTL0 |= ADC14_CTL0_ENC;
    NVIC_EnableIRQ(ADC14_IRQn); //a flag ADC14IFGx is set when x has a conversion result.
#endif


}

void ADC_CONFIG_Accelerometer(){

    /*
     * Set up the pins for the accelerometer on the Booster Pack MK II
     * According To Booster Pack:
     *      J3.23   = X-Axis
     *      J3.24   = Y-Axis
     *      J3.25   = Z-Axis
     *
     * According to the MSP432p401R
     *     AnalogPin14  = P6.1    = X-Axis
     *     AnalogPin13  = P4.0    = Y-Axis
     *     AnalogPin11  = P4.2    = Z-Axis
     *
     *      Accelerometer has an enable pin and ST pin... :MARK complete, done for us hard wired.
     * */

    //:TODO double check secondary mode values. Table 10.2.6 and Figure 4.1
      P6SEL1 &= ~BIT1;//Primary mode
      P6SEL0 = BIT1;//Primary mode

      P4SEL1 &= ~BIT0; //Primary mode
      P4SEL0 = BIT0; //Primary mode

      P4SEL1 = BIT2; //tertiary mode
      P4SEL0 = BIT2; //tertiary mode

      /*Set directions as input*/
      P4DIR &= ~(BIT0 | BIT2);
      P6DIR &= ~BIT1;

      //:TODO end of sequence (EOS (done), and CONSEQ, and start address(left at default))

    while(REF_A->CTL0 & REF_A_CTL0_GENBUSY);
       REF_A->CTL0 = REF_A_CTL0_VSEL_0 | REF_A_CTL0_ON;
       REF_A->CTL0 &= ~REF_A_CTL0_TCOFF;

       ADC14->CTL0 = ADC14_CTL0_SHT0__96 | ADC14_CTL0_ON | ADC14_CTL0_SHP | ADC14_CTL0_CONSEQ_1; //96 clocks, turn on, and sample and hold mode
       ADC14->CTL1 |= ADC14_CTL1_RES__14BIT; //temp sensor, set resolution: 16 clock cycles

       ADC14->MCTL[0] = ADC14_MCTLN_INCH_14 | ADC14_MCTLN_VRSEL_0;      //X, full range ADG (3.3 to 0v)
       ADC14->MCTL[1] = ADC14_MCTLN_INCH_13 | ADC14_MCTLN_VRSEL_0;      //Y, full range ADG (3.3 to 0v)
       ADC14->MCTL[2] = ADC14_MCTLN_INCH_11 | ADC14_MCTLN_VRSEL_0 | ADC14_MCTLN_EOS;      //Z, full range ADG (3.3 to 0v)



       // need to enable interrupt for conversion of data.
       ADC14->IER0 = ADC14_IER0_IE0; //interrupt for mem[0]

       while(REF_A->CTL0 & REF_A_CTL0_GENBUSY);
       ADC14->CTL0 |= ADC14_CTL0_ENC;
       NVIC_EnableIRQ(ADC14_IRQn); //a flag ADC14IFGx is set when x has a conversion result.

}

/*
 * Joy stick Pins
 *  According to Booster Pack Mk II
 *  There is a slight discrepancy between data sheet schematics. But look at silk screen mask.
 *
 *  J1.2    = X
 *  J1.5    = Sel
 *  J3.26   = Y
 *
 *  According to the MSP432p401 Launchpad
 *  AnalogPin15  = P6.0 =  X = Horizontal                                   Primary
 *  AnalogPin9   = P4.4 =  Y = Vertical                                     Tertiary
 *  AnalogPin12  = P4.1 =  Select Button = press down.                      Primary
 *
 * */

void ADC_CONFIG_Joystick(){
        P6SEL1 &= ~BIT0;//Primary mode
        P6SEL0 = BIT0;//Primary mode

        P4SEL1 &= ~BIT1; //Primary mode
        P4SEL0 = BIT1; //Primary mode

        P4SEL1 = BIT4; //tertiary mode
        P4SEL0 = BIT4; //tertiary mode

        /*Set directions as input*/
        P4DIR &= ~(BIT0 | BIT2);
        P6DIR &= ~BIT1;

        //:TODO end of sequence (EOS (done), and CONSEQ, and start address(left at default))

      while(REF_A->CTL0 & REF_A_CTL0_GENBUSY);
         REF_A->CTL0 = REF_A_CTL0_VSEL_0 | REF_A_CTL0_ON;
         REF_A->CTL0 &= ~REF_A_CTL0_TCOFF;

         ADC14->CTL0 = ADC14_CTL0_SHT0__96 | ADC14_CTL0_ON | ADC14_CTL0_SHP | ADC14_CTL0_CONSEQ_1; //96 clocks, turn on, and sample and hold mode
         ADC14->CTL1 |= ADC14_CTL1_RES__14BIT; //temp sensor, set resolution: 16 clock cycles

         ADC14->MCTL[0] = ADC14_MCTLN_INCH_15 | ADC14_MCTLN_VRSEL_0;      //X, full range ADG (3.3 to 0v)
         ADC14->MCTL[1] = ADC14_MCTLN_INCH_9 | ADC14_MCTLN_VRSEL_0;      //Y, full range ADG (3.3 to 0v)
         ADC14->MCTL[2] = ADC14_MCTLN_INCH_12 | ADC14_MCTLN_VRSEL_0 | ADC14_MCTLN_EOS;      //select, full range ADG (3.3 to 0v)



         // need to enable interrupt for conversion of data.
         ADC14->IER0 = ADC14_IER0_IE0; //interrupt for mem[0]

         while(REF_A->CTL0 & REF_A_CTL0_GENBUSY);
         ADC14->CTL0 |= ADC14_CTL0_ENC;
         NVIC_EnableIRQ(ADC14_IRQn); //a flag ADC14IFGx is set when x has a conversion result.
}


void ADC14_IRQHandler(){
    if(ADC14->IFGR0 & ADC14_IFGR0_IFG0){

        uint32_t TS30 = TLV->ADC14_REF1P2V_TS30C;
        uint32_t TS85 = TLV->ADC14_REF1P2V_TS85C;

        NADC_Temperature = ADC14->MEM[0];  /*global*/

        //set a break point to determine if the interrupt was for mem[0] or channel [22]
        //float calculated_temp = (ADC14->MEM[0] - TLV->ADC14_REF1P2V_TS30C)*((85-30)/(TLV->ADC14_REF1P2V_TS85C- TLV->ADC14_REF1P2V_TS30C)) + 30;
        //reading 3623 in the converted. not sure if trust worthy. like will temp change? i placed finger and saw 0 change., placing on top of
        //computer to see if gets warmer.
        ///what does ADC14 at 14 bit resolution mean? why am i using mod clock?
        //well i think maybe it worked? i got a 3727 now after sitting on top of computer for a few minutes.

       //how to convert? reference is 1.2 v? max is 3.3v? or max 1.2 v? and ground is min?
        //my temp is now at 3719 converted.
        //it looks like maybe the referecnec voltage represents the top value. not the bottom. from 0 to 1.2v is the reference range.
    }

    //:TODO why is htis IFG14 and not mem[0] flag?
    if(ADC14->IFGR0 & ADC14_IFGR0_IFG14){
         X_ADC = ADC14->MEM[0];
         Y_ADC = ADC14->MEM[1];
         Z_ADC = ADC14->MEM[2];
    }

}


