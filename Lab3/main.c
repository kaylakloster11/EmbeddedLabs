/**
 * main.c
 */

#include "msp.h"
#include "uart.h"
#include <stdlib.h>
#include "circbuf.h"
#include <stdio.h>
#include <string.h>


CircBuf_t buf1;
CircBuf_t * buf = &buf1;

#ifdef ESCOOTER
    CircBuf_t buf2;
    CircBuf_t * buf_scoot = &buf2;
#endif

volatile int isEnter = 0;
volatile int addItem = 0;
volatile int ButtonEnter = 0;
volatile int ScooterEnter = 0;
volatile uint32_t countWAVE = 0;
volatile float distCOUNT = 0;
volatile float dist2 = 0;

void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
	UART_Configure();
	//configure_clocks();

#if defined QUESTION11 || defined ESCOOTER
    configureButtons();
#endif

#ifdef QUESTION5
	uint8_t phrase[] ="Why not Zoidberg?";
	uint32_t len = 17;
#endif

#ifdef QUESTION8
	CB_Status length = CB_initialize_buffer(&buf, 3);
	uint8_t isEmpty = CB_is_buffer_empty(buf);
//	uint8_t isFull = CB_is_buffer_full(buf);
	CB_Status addItem = CB_add_item_to_buffer(buf, 0xAA);
	CB_Status addItem1 = CB_add_item_to_buffer(buf, 0x88);
	CB_Status addItem2 = CB_add_item_to_buffer(buf, 0x10);
	CB_Status addItem5 = CB_add_item_to_buffer(buf, 0x33);
    uint8_t firstItem = CB_remove_item_from_buffer(buf);
    uint8_t secondItem = CB_remove_item_from_buffer(buf);
    uint8_t thirdItem = CB_remove_item_from_buffer(buf);
    uint8_t isFull2 = CB_is_buffer_full(buf);
    uint8_t isEmpty2 = CB_is_buffer_empty(buf);
	CB_Status isClear = CB_clear_buffer(buf);
#endif

#ifdef QUESTION7
    char str[10] = 0;
    char res[20];
    const char buffer[10] = "9734";
    float n = 324.12;
    itoa(12458, str); //converts int data type to string data type
    ftoa(n, res, 3);
    int atoiCheck;
    atoiCheck = atoi(buffer);
#endif

#if defined QUESTION11 || defined QUESTION10 || defined ESCOOTER
    CB_initialize_buffer(&buf, BUFFER);
#ifdef ESCOOTER
    CB_initialize_buffer(&buf_scoot, 35);
#endif
    __enable_irq();
#endif

#if defined QUESTION3 || defined QUESTION5 || defined QUESTION11 || defined QUESTION10 || defined ESCOOTER
    while (1){
        #ifdef QUESTION3
           while(1){
           UART_send_byte(0xAA);
           }
       #endif

        #ifdef QUESTION5
            UART_send_n(phrase, len);
        #endif

        #if defined QUESTION11 || defined ESCOOTER
#ifdef ESCOOTER
            if(addItem){
                CB_add_item_to_buffer(buf_scoot, 1);
                 addItem = 0;
                        }

           if(ScooterEnter){
              dist2 = CalcDist();
              UART_send_byte(0x0A);
              uint8_t phrase[] ="Distance Traveled (in meters): ";
              uint32_t len = strlen(phrase);
              UART_send_n(phrase, len);
              char printD[10];
              ftoa(dist2, printD, 3);
              uint32_t length_D = strlen((char *)printD);
              UART_send_n(&printD,length_D);
              ScooterEnter = 0;
            }
#endif
            if(isEnter){
                UART_send_byte(0x0A);
                if(CB_is_buffer_full(buf)){
                    uint8_t full[] ="BUFFER FULL!!";
                    uint32_t len = strlen(full);
                    UART_send_n(full, len);
                    UART_send_byte(0x0A);
                }
                uint8_t phrase[] ="Number of Items:";
                uint32_t len = strlen(phrase);
                UART_send_n(phrase, len);
                int num = buf->num_items;
                char numString[3];
                itoa(num, numString);
                uint32_t lengthN = strlen((char *)numString);
                UART_send_n(&numString,lengthN);
                UART_send_byte(0x0A);
                while(!CB_is_buffer_empty(buf)){
                    countLetters(buf);
                    UART_send_byte(CB_remove_item_from_buffer(buf));
                }
               isEnter = 0;
               displayCount();
               LetterCount = 0;
               PuncCount = 0;
               numCount = 0;
               whiteCount = 0;
               otherCount = 0;
               buf->tail = buf->buffer;
               buf->head = buf->buffer;
               UART_send_byte(0x0A);
               UCA0IE |= UCRXIFG;
            }

           if(ButtonEnter){
                UART_send_byte(0x0A);
                int i = 0;
                uint8_t phrase[] ="Number of Items:";
                uint32_t len = strlen(phrase);
                UART_send_n(phrase, len);
                int num = buf->num_items;
                char numString[3] = 0;
                itoa(num, numString);
                uint32_t lengthN = strlen((char *)numString);
                UART_send_n(&numString,lengthN);
                UART_send_byte(0x0A);
                volatile uint8_t * temp = buf->tail;
                for(i=0 ; i<buf->num_items; i++){
                    UART_send_byte(CB_peek_into_buffer(buf, temp));
                    temp++;
                }
                ButtonEnter = 0;
                UCA0IE |= UCRXIFG;
                }

        #endif
    }
#endif
}

#ifdef QUESTION10
void EUSCIA0_IRQHandler(void){
    uint8_t data;
    if (UCA0IFG & UCRXIFG){          //If UART receiver buffer full
        data = UCA0RXBUF;                  //Adds items to the buffer
        CB_add_item_to_buffer(buf, data);
    }
    if(UCA0IFG & UCTXIFG){ //Sends received data back to terminal
        UART_send_byte(CB_remove_item_from_buffer(buf));
     }
  }
#endif

#if defined QUESTION11 || defined ESCOOTER
void EUSCIA0_IRQHandler(void){
    uint8_t data;
    if(UCA0IFG & UCRXIFG){
            if((UCA0RXBUF == 0x0A) || CB_is_buffer_full(buf)){
                UCA0IE &= ~UCRXIFG;
                isEnter = 1;
            }
            else{
                data = UCA0RXBUF;                  //Adds items to the buffer
                CB_add_item_to_buffer(buf, data);
            }
        }
    }
void PORT1_IRQHandler(){
    int i;
    for(i=0; i<= 50000; i++);
    //left button interrupt
    if(P1->IFG & BIT1)  {
        CB_clear_buffer(buf);
        P1->IFG &= ~BIT1; //clear flag
    }
    //right button interrupt
    if(P1->IFG & BIT4) {
        ButtonEnter = 1;
        UCA0IE &= ~UCRXIFG;
        P1->IFG &= ~BIT4;
    }
#ifdef ESCOOTER
    if(P1->IFG & BIT5)  {
          distCOUNT++;
          P1->OUT ^= BIT0;
          if(CB_is_buffer_full(buf_scoot)){
             CB_clear_buffer(buf_scoot);
             ScooterEnter = 1;
          }
          else{
              addItem = 1;
          }
          P1->IFG &= ~BIT5; //clear flag
        }
#endif
}

#endif

