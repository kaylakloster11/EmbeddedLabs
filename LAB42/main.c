#include "msp.h"
#include "adc_driver.h"
#include "lab4.h"
#include "timer.h"
#include "uart.h"
#include"circ_buffer_basic.h"

/*
 * main.c
 */

/*==================================================*/
/*     Globals                                      */

   volatile uint8_t FLAG_Collect_Data = 0; //collect data from the mem[0] when set to 1
   volatile uint8_t Calculate_Stats = 0; // calculate stats on datas :Todo remove this variable
   CircBuf_t myBuffer;
   CircBuf_t * myBufferPTR = &myBuffer;
   volatile int ButtonEnter = 0;

   //:Accelerometer
   volatile uint16_t X_ADC = 0;
   volatile uint16_t Y_ADC = 0;
   volatile uint16_t Z_ADC = 0;

   //:Joystick

   //Temperature
   volatile uint16_t NADC_Temperature = 0;
/*==================================================*/

void main(void)
{

    initialize_Circ_Buffer(&myBufferPTR, 60);

    //enable the button interrupt
    Button_Interrupt_Config();

	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
	timer_a0_config();
	ADC_CONFIG(); //generic ADC, for channel 0.
	//ADC_CONFIG_Accelerometer();
	//ADC_CONFIG_Joystick();
	UART_config();
	configure_clocks();

	/*visual output*/
	P2SEL0 &= ~BIT2; //gpio
	P2SEL1 &= ~BIT2; //gpio
	P2->DIR = BIT2;
	P2->OUT = BIT2;

	P1SEL0 |= BIT0;
	P1SEL1 |= BIT0;
	P1->DIR = BIT1;
	P1->OUT = BIT1;

	__enable_interrupt();
	while(1){



	    ADC14->CTL0 |= ADC14_CTL0_SC | ADC14_CTL0_ENC; //start sample, enable conversion?

	    if(ButtonEnter){
	        UART_putchar_n("print!", 6);
	        __disable_interrupt();
	                 while(!is_Circ_Buf_Empty(&myBufferPTR)){
	                     float Temp =  (float)remove_From_Buffer(&myBufferPTR);
	                     char temp[10];
	                     ftoa(Temp, temp, 3);
	                     uint32_t lengthN = strlen((char *)temp);
	                     UART_putchar_n(&temp, lengthN);
	                   }
	                 clear_Buffer(&myBufferPTR);
	                 ButtonEnter = 0;
	                 __enable_interrupt();
	             }

	    if(FLAG_Collect_Data == 1){
	        //add to buffer but for now just put into variable
	        //:Todo add to buffer
	        __disable_interrupts();
	        add_To_Buffer(&myBufferPTR, NADC_Temperature);
	        __enable_interrupts();
//	        UART_putchar_n("test", 4);
//	        UART_putchar(13);
	        FLAG_Collect_Data = 0;
	    }
	}
}


