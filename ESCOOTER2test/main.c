#include <ADCescooter.h>
#include <CIRCBUFescooter.h>
#include <ESCOOTER.h>
#include "msp.h"
#define SCB_SCR_ENABLE_SLEEPONEXIT (0x00000002)


/**
 * main.c
 */
volatile CircBuf_t * DATA;

volatile char charDist[8];
volatile char charVel[4];

volatile float distance= 0.0;
volatile float distance2= 0.0;

volatile float velocity = 0.0;
volatile uint8_t print = 0;

volatile uint16_t Y_NADC = 0;
volatile uint16_t X_NADC = 0;

volatile float distCOUNT = 0;

volatile int Accel = 0;

volatile int savedir = 0;


void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer
  configure_ADC_Accelerometer();
    UART_Configure();
    config_SYSTICK();
   timer_a1_config();
    GPIO_Configure();

    CB_initialize_buffer(&DATA, BUFFER);

    __enable_irq();

   SCB->SCR &= ~SCB_SCR_ENABLE_SLEEPONEXIT; //Wake up on exit from ISR
   ADC14->CTL0 |=  (ADC14_CTL0_ENC);

    while(1){
        ADC14->CTL0 |=  (ADC14_CTL0_SC);        // Start sampling/conversion
        __sleep(); //Blocks here until conversion finished


        if(print == 1){
            __disable_irq();

            UART_send_byte(0x0A);

            distance = CalcDist();
            uint32_t time1 = getSysTime();
            velocity = getVelocity(time1);

            CB_add_item_to_buffer(DATA, distance);
            CB_add_item_to_buffer(DATA, 'D');
            CB_add_item_to_buffer(DATA, velocity);
            CB_add_item_to_buffer(DATA, 'V');

               ftoa(distance, charDist, 3);
                UART_send_n("Distance:", 9);
                UART_send_n(charDist, 8);

                UART_send_byte(0x20);
                ftoa(velocity, charVel, 2);
                UART_send_n("Velocity:", 9);
                UART_send_n(charVel, 4);
                UART_send_n("meters/second", 13);
                UART_send_byte(0x20);

                distance2 = distance;

                print = 0;

                __enable_irq();
            }
    }


}
