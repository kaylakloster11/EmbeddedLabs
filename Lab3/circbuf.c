/*
 * circbuf.c
 *
 *  Created on: Sep 25, 2017
 *      Author: Kayla
 */

#include "msp.h"
#include "circbuf.h"
#include <stdint.h>
#include <stdlib.h>
#include "uart.h"

volatile int LetterCount, PuncCount, numCount, otherCount, whiteCount;

//Setup buffer in heap and initialize structure variable
CB_Status CB_initialize_buffer(CircBuf_t ** buf, uint32_t length){
    (*buf)->buffer = (uint8_t *)malloc(length);
    if(!(*buf)->buffer)        //buffer is zero -> return an error
        {
            return CB_STATUS_ERROR;
        }
    (*buf)->head = (*buf)->buffer;
    (*buf)->length = length;
    (*buf)->tail = (*buf)->buffer;
    (*buf)->num_items = 0;
    return CB_STATUS_INITIAL;
}

//Set contents of buffer to zero and reset buffer pointer to a clean state
CB_Status CB_clear_buffer(CircBuf_t * buf){
    buf->num_items = 0;
    buf->head = buf->buffer;
    buf->tail = buf->buffer;
    return CB_STATUS_INITIAL;

}

//Delete buffer from dynamic memory allocation
CB_Status CB_delete_buffer(CircBuf_t * buf){
    free(buf);
    return CB_STATUS_EMPTY;
}

//Return non-zero value if buffer is full, zero value if not
int8_t CB_is_buffer_full(CircBuf_t * buf){
    if(buf->num_items == buf->length){
        return 1;
    }
    else{
        return 0;
    }
}

//Return zero of buffer not empty, non-zero value if empty
int8_t CB_is_buffer_empty(CircBuf_t * buf){
    if(buf->num_items == 0){
        return 1;
    }
    else{
        return 0;
    }
}

//Add new item to buffer and adjust the params in the structure
CB_Status CB_add_item_to_buffer(CircBuf_t * buf, uint8_t item){
    if((buf->num_items == buf->length) && (buf->head == (buf->buffer +(buf->length - 1)))){     // if it's on the end and buffer full
            return CB_STATUS_FULL;
        }
    else if (buf->head == (buf->buffer + (buf->length - 1))){ //if head is at end, but still space bc pointer not at end
            *buf->head = item;
            buf->head = buf->buffer;  //wrap around to beginning
            buf->num_items++;
            return CB_STATUS_WRAP;
     }

    else { //both head and tail at beginning or middle of buffer
        *buf->head = item;
        buf->head++;
        buf->num_items++;
        return CB_STATUS_MID;
    }
}

//Remove item from buffer and adjust params in the structure
uint8_t CB_remove_item_from_buffer(CircBuf_t * buf){
    uint8_t lastbyte;
    if(CB_is_buffer_empty(buf)){ //if nothing in buffer
        return lastbyte = 0;
    }
    else{
        lastbyte =  *buf->tail;
        buf->num_items--;
        buf->tail++;
        return lastbyte;
    }
}

#if defined QUESTION11 || defined ESCOOTER
void countLetters(CircBuf_t * buf){

    if (((*buf->tail >= 65) && (*buf->tail <= 90)) || ((*buf->tail >= 97) && (*buf->tail <= 122))) LetterCount++;
    if (((*buf->tail >= 33) && (*buf->tail <= 47)) || ((*buf->tail >= 58) && (*buf->tail <= 64)) || ((*buf->tail >= 91) && (*buf->tail <= 96)) || (*buf->tail >= 123)) PuncCount++;
    if ((*buf->tail >= 48) && (*buf->tail <= 57)) numCount++;
    if ((*buf->tail == 32) || (*buf->tail == 9)) whiteCount++;
    if ((*buf->tail < 32) && (*buf->tail != 9)) otherCount++;
}

void displayCount(void){
    char Let[] = "# of Letters: ";
    uint32_t Alen = strlen(Let);       //Length of string above
    UART_send_n(&Let, Alen);
    char number_items_a[3];
    itoa(LetterCount,number_items_a);
    uint32_t leng_a = strlen((char *)number_items_a);
    UART_send_n(&number_items_a,leng_a);
    UART_send_byte(0x0A);

    char PuncS[] = "# of Punctions: ";
    uint32_t Plen = strlen(PuncS);
    UART_send_n(&PuncS, Plen);
    char number_items_p[3];
    itoa(PuncCount,number_items_p);
    uint32_t leng_p = strlen((char *)number_items_p);
    UART_send_n(&number_items_p,leng_p);
    UART_send_byte(0x0A);

    char num[] = "# of Numbers: ";
    uint32_t numlen = strlen(num);
    UART_send_n(&num, numlen);
    char number_items_n[3];
    itoa(numCount,number_items_n);
    uint32_t leng_n = strlen((char *)number_items_n);
    UART_send_n(&number_items_n,leng_n);
    UART_send_byte(0x0A);

    //WHITESPACE
    char white[] = "# of Whitespace: ";
    uint32_t Wlen = strlen(white);          //Length of string above
    UART_send_n(&white, Wlen);
    char number_items_w[3];
    itoa(whiteCount,number_items_w);
    uint32_t leng_w = strlen((char *)number_items_w);
    UART_send_n(number_items_w,leng_w);
    UART_send_byte(0x0A);

    //NON-PRINTABLES
    char otherC[] = "# Other Characters: ";
    uint32_t Olen = strlen(otherC);       //Length of string above
    UART_send_n(&otherC, Olen);
    char number_items_o[3];
    itoa(otherCount,number_items_o);
    uint32_t leng_o = strlen((char *)number_items_o);
    UART_send_n(number_items_o,leng_o);
    UART_send_byte(0x0A);

    }

uint8_t CB_peek_into_buffer(CircBuf_t * buf, volatile uint8_t * temp){
    uint8_t lastbyte;
    if(CB_is_buffer_empty(buf)){ //if nothing in buffer
           return lastbyte = 0;
       }
    else{
        lastbyte =  *temp;
        return lastbyte;
        }
  }

void configureButtons(void){
#ifdef QUESTION11
/* Left and right button configure */
   P1->SEL0 &= ~(BIT1 | BIT4 | BIT7);     //Use the OR function to set BIT1 and BIT4 to general IO Mode
   P1->SEL1 &= ~(BIT1 | BIT4 |BIT7) ;     //^^^
   P1->DIR |= BIT7; //Set pin P1.7 as an output
   P1->DIR &= ~(BIT1 | BIT4);       //Direction pin is set to zero since BIT1 and BIT4 are inputs
   P1->REN |= BIT1 | BIT4 ;        //REN is set to 1 at BIT1 and BIT4 in order to enable pullup/pulldown
   P1->OUT |= BIT1 | BIT4 ;        //Output resgister is set to 1 to enable the pullup resister
#endif

#ifdef ESCOOTER
 //data from encoder
   /* Left and right button configure */
     P1->SEL0 &= ~(BIT1 | BIT4 | BIT5);     //Use the OR function to set BIT1 and BIT4 to general IO Mode
     P1->SEL1 &= ~(BIT1 | BIT4 |BIT5) ;
     P1->DIR &= ~(BIT1 | BIT4 | BIT5);       //Direction pin is set to zero since BIT1 and BIT4 are inputs
     P1->REN |= BIT1 | BIT4 | BIT5 ;        //REN is set to 1 at BIT1 and BIT4 in order to enable pullup/pulldown
     P1->OUT |= BIT1 | BIT4 |BIT5 ;        //Output resgister is set to 1 to enable the pullup resister
     P1->IFG = 0;
 /*CONFIGURE RED LED LIGHT*/
     P1->SEL0 &= ~BIT0; //General I/O Mode
     P1->SEL1 &= ~BIT0;
     P1->DIR |= BIT0; //Set P1.0 LED to output
     P1->OUT &= ~BIT0; //Output set to LOW
#endif
   P1->IFG = 0;                    //interrupt flag is set to zero
   P1->IES |= BIT1  | BIT4 | BIT5;        //high-to-low transition
   P1->IE |= BIT1 | BIT4 | BIT5;          //interrupts are enabled


   NVIC_EnableIRQ(PORT1_IRQn);
}

#endif















