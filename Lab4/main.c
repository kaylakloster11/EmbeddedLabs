 #include "msp.h"
#include "lab4.h"
#include "adc.h"
#include "adc_circbuf.h"
#include <stdint.h>
#define SCB_SCR_ENABLE_SLEEPONEXIT (0x00000002)

/**
 * main.c
 */

volatile uint16_t NADC_Value;
volatile int ButtonEnter = 0;

volatile float X_NADC = 0;
volatile float Y_NADC = 0;
volatile float Z_NADC = 0;



#ifdef QUESTION6
    CircBuf_t buf1;
    CircBuf_t * buf = &buf1;
#endif

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

#ifdef JOYSTICK
    config_ADC_Stick();
    UART_Configure();
#endif

#ifdef ACCELEROMETER
    config_ADC_Accel();
    //configure_ADC();
    UART_Configure();
    uint8_t labelX[] =" X-Axis: ";
    uint8_t labelY[] = " Y-Axis: ";
    uint8_t labelZ[] = " Z-Axis: ";
#endif


#ifdef QUESTION6
    configure_ADC(); //Configure ADC for interrupts
     UART_Configure();
     timer_a0_config();
    // configure_clocks();
     CB_initialize_buffer(&buf, BUFFER);
     NVIC_EnableIRQ(PORT1_IRQn);
#endif

#ifdef QUESTION6_NOTWORKING
     configure_ADC(); //Configure ADC for interrupts
    UART_Configure();
    CLK_init();
    timer_a0_config();
    CB_initialize_buffer(&buf, BUFFER);
    NVIC_EnableIRQ(PORT1_IRQn);
#endif

#ifdef QUESTION4
    configure_ADC(); //Configure ADC for interrupts
    UART_Configure();
#endif

    __enable_interrupt();
    SCB->SCR &= ~SCB_SCR_ENABLE_SLEEPONEXIT; //Wake up on exit from ISR
    ADC14->CTL0 |=  (ADC14_CTL0_ENC);

#ifdef TEST
    CLK_init();
    timer_a0_config();
    __enable_interrupt();
#endif

#ifdef QUESTION4
    uint8_t labelC[] =" Celsius";
    uint8_t labelF[] = " Fahrenheit";
    uint8_t labelK[] = " Kelvin";
    float V_sense, TempC, TempF, TempK;
#endif

    while(1) {

#ifdef ACCELEROMETER
     ADC14->CTL0 |=  (ADC14_CTL0_SC);        // Start sampling/conversion
     __sleep(); //Blocks here until conversion finished
     char printX[10], printY[10], printZ[10];
     /*float X_NADC2, Y_NADC2, Z_NADC2;
     X_NADC2 = X_NADC - 8192.0;
     Y_NADC2 = Y_NADC - 8192.0;
     Z_NADC2 = Z_NADC - 8192.0;

     if(X_NADC2 < 0){
         X_NADC2 = X_NADC2 - (2*X_NADC2);
         ftoa(X_NADC2, printX, 2);
         uint32_t length_X = strlen((char *)printX);
         UART_send_n(labelX, 9);
         UART_send_byte('-');
         UART_send_n(&printX,length_X);
     }
     else{
         ftoa(X_NADC2, printX, 2);
         uint32_t length_X = strlen((char *)printX);
         UART_send_n(labelX, 9);
         UART_send_n(&printX,length_X);
     }

     if(Y_NADC2 < 0){
          Y_NADC2 = Y_NADC2 - (2*Y_NADC2);
          ftoa(Y_NADC2, printY, 2);
          uint32_t length_Y = strlen((char *)printY);
          UART_send_n(labelY, 9);
          UART_send_byte('-');
          UART_send_n(&printY,length_Y);
      }
     else{
         ftoa(Y_NADC2, printY, 2);
         uint32_t length_Y = strlen((char *)printY);
         UART_send_n(labelY, 9);
         UART_send_n(&printY, length_Y);

     }

     if(Z_NADC2 < 0){
         Z_NADC2 = Z_NADC2 - (2*Z_NADC2);
         ftoa(X_NADC2, printX, 2);
         uint32_t length_Z = strlen((char *)printZ);
         UART_send_n(labelZ, 9);
         UART_send_byte('-');
         UART_send_n(&printX,length_Z);
       }
     else{
         ftoa(Z_NADC2, printZ, 2);
         uint32_t length_Z = strlen((char *)printZ);
         UART_send_n(labelZ, 9);
         UART_send_n(&printZ, length_Z);
     } */

     ftoa(X_NADC, printX, 2);
              uint32_t length_X = strlen((char *)printX);
              UART_send_n(labelX, 9);
              UART_send_n(&printX,length_X);

     ftoa(Y_NADC, printY, 2);
      uint32_t length_Y = strlen((char *)printY);
      UART_send_n(labelY, 9);
      UART_send_n(&printY, length_Y);
     UART_send_byte(0x0A);

     ftoa(Z_NADC, printZ, 2);
              uint32_t length_Z = strlen((char *)printZ);
              UART_send_n(labelZ, 9);
              UART_send_n(&printZ, length_Z);

#endif

#ifdef JOYSTICK
        ADC14->CTL0 |=  (ADC14_CTL0_SC);        // Start sampling/conversion
   __sleep(); //Blocks here until conversion finished
   if((0 <= Y_NADC) && (Y_NADC <= JOY_MIDDLE)){
       if((0 <= X_NADC) && (X_NADC <= JOY_MIDDLE)){
           uint8_t phraseL[] = "Lower Left";
           uint32_t lenL = 10;
           UART_send_n(phraseL, lenL);
           UART_send_byte(0x20);
       }
       if((JOY_MIDDLE <= X_NADC) && (X_NADC <= JOY_MAX)){
           uint8_t phraseL2[] = "Lower Right";
           uint32_t lenL2 = 11;
           UART_send_n(phraseL2, lenL2);
           UART_send_byte(0x20);
       }
   }
   else{
       if((0 <= X_NADC) && (X_NADC <= JOY_MIDDLE)){
           uint8_t phraseU[] = "Upper Left";
           uint32_t lenU = 10;
           UART_send_n(phraseU, lenU);
           UART_send_byte(0x20);
        }
        if((JOY_MIDDLE <= X_NADC) && (X_NADC <= JOY_MAX)){
            uint8_t phraseU2[] = "Upper Right";
            uint32_t lenU2 = 11;
            UART_send_n(phraseU2, lenU2);
            UART_send_byte(0x20);
         }
   }
#endif

#ifdef QUESTION4
       ADC14->CTL0 |=  ADC14_CTL0_SC;        // Start sampling/conversion
       __sleep(); //Blocks here until conversion finished
#endif

#ifdef QUESTION4
        V_sense = (((float)NADC_Value * VREF)/RESOLUTION);
         TempC = ((V_sense - V_SENSOR)/TC_SENSOR)*1000;
         TempK = TempC + 273.15;
         TempF = (TempC * 1.8 ) + 32.0;
         char printC[10], printK[10], printF[10];
         ftoa(TempC, printC, 3);
         ftoa(TempK, printK, 3);
         ftoa(TempF, printF, 3);
         uint32_t length_C = strlen((char *)printC);
         uint32_t length_K = strlen((char *)printK);
         uint32_t length_F = strlen((char *)printF);
         UART_send_n(&printC,length_C);
         UART_send_n(labelC, 8);
         UART_send_byte(0x20);
         UART_send_n(&printK,length_K);
         UART_send_n(labelK, 7);
         UART_send_byte(0x20);
         UART_send_n(&printF,length_F);
         UART_send_n(labelF, 11);
         UART_send_byte(0x0A);
#endif

#ifdef QUESTION6
   ADC14->CTL0 |=  ADC14_CTL0_SC;        // Start sampling/conversion
    __sleep(); //Blocks here until conversion finished
    if(CB_is_buffer_full(buf)){
          P1->OUT |= BIT0;
        }
    if(ButtonEnter){
        __disable_interrupt();
          float getNADC = (float)CB_remove_item_from_buffer(buf);
          while(!CB_is_buffer_empty(buf)){
              float getNADC = (float)CB_remove_item_from_buffer(buf);
              float V_sense = ((getNADC * VREF)/RESOLUTION);
              float Temp = ((V_sense - V_SENSOR)/TC_SENSOR)*1000;
              char temp[10];
              ftoa(Temp, temp, 3);
              uint32_t lengthN = strlen((char *)temp);
              UART_send_n(&temp, lengthN);
              UART_send_byte(0x0A);
            }
          CB_clear_buffer(buf);
          P1->OUT &= ~BIT0;
          ButtonEnter = 0;
          __enable_interrupt();
      }

#endif

#ifdef QUESTION6_NOTWORKING
    __sleep(); //Blocks here until conversion finished
    if(ButtonEnter){
        __disable_interrupt();
        ADC14->IER0 &= ~(ADC14_IER0_IE0);
          float getNADC = (float)CB_remove_item_from_buffer(buf);
          while(!CB_is_buffer_empty(buf)){
              float getNADC = (float)CB_remove_item_from_buffer(buf);
              float V_sense = ((getNADC * VREF)/RESOLUTION);
              float Temp = ((V_sense - V_SENSOR)/TC_SENSOR)*1000;
              char temp[10];
              ftoa(Temp, temp, 3);
              uint32_t lengthN = strlen((char *)temp);
              UART_send_n(&temp, lengthN);
            }
          CB_clear_buffer(buf);
          ADC14->IER0 |= (ADC14_IER0_IE0);
          __enable_interrupt();
      }

    if(CB_is_buffer_full(buf)){
       P1->DIR |= BIT0;
     }
    CB_add_item_to_buffer(buf, NADC_Value);

#endif
        __no_operation(); //No-Operation, good for breakpoint
    }
}









