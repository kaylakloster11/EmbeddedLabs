/*
 * circ_buffeR_basic.c
 *
 *  Created on: Oct 6, 2017
 *      Author: kk
 */

#include <stdlib.h>
#include "uart.h"
#include "circ_buffer_basic.h"
#include "string.h"
extern uint8_t Calculate_Stats;


void initialize_Circ_Buffer(CircBuf_t **myBase, uint16_t _length)
{

    //tests

    if (*myBase != NULL)
    {
        uint16_t *_buffer = (uint16_t *) malloc(_length * sizeof(uint16_t));

        if (_buffer != NULL)
        {
            (*myBase)->baseConst = _buffer;
            (*myBase)->num_items = 0;
            (*myBase)->tailPosition = 0;
            (*myBase)->length = _length;
            (*myBase)->head = _buffer;
            (*myBase)->tail = _buffer;
        }
        else
        {
            //TODO: Error Logging, Malloc Failed
        }
    }
    else
    {
        //TODO: Error Logging       bad pointer        Enum:CB_STATUS
    }
}

void add_To_Buffer(CircBuf_t **buf, uint16_t item)
{
    if (*buf != NULL)
    {



        if (is_Circ_Buf_Empty(buf))
        {
            //Case 1: Empty
            (*buf)->num_items++;
            *(*buf)->head = item; //* once to derefefernce doulbe pointer, then *again to deref head.
            (*buf)->head++; //move the head 1 spot.
        }
        else
        {
            //Case 2: Not Empty, Not Full
            if ((*buf)->num_items != (*buf)->length)
            {
                *(*buf)->head = item; //* once to derefefernce doulbe pointer, then *again to deref head.
                (*buf)->num_items++;
                    if((*buf)->num_items == (*buf)->length){
                        //loop head around
                        (*buf)->head = (*buf)->baseConst; //loop around
                    }
                    else{
                        (*buf)->head++; //move the head 1 spot.
                    }
            }
            else{
             // Case 3: Full circular buffer, do not overwrite?
                //TODO: what happens when full?
//                return;
            }
        }
    }
}

uint16_t remove_From_Buffer(CircBuf_t ** buf)
{
    if (*buf != NULL)
    {
        if (is_Circ_Buf_Empty(buf)) //Case 1: Empty
        {
            return 53;
        }
        else{
            uint8_t oldTail = *(*buf)->tail;
            (*buf)->num_items--;

            //Case 2: Not Empty, tail before head
            if(((*buf)->head - (*buf)->tail) > 0){
                (*buf)->tail++;
                (*buf)->tailPosition++;
            return oldTail;
            }
            else if(((*buf)->head - (*buf)->tail) < 0){
                // Case 3: Not Empty, but head is before tail (loop around)
                //check if tail is at the end or not
                if((*buf)->tailPosition == (*buf)->length){
                    //must require a loop around of the tail
                    (*buf)->tailPosition = 0;
                    (*buf)->tail = (*buf)->baseConst; //send tail back to front.
                }
                else{ //we are not at the end but there was a loop around

                    (*buf)->tail++;
                    (*buf)->tailPosition++;

                }
                return oldTail;
            }
            else{           //Final Case: Not Empty, Not full, 1 item only.
                    (*buf)->tail++;
                    (*buf)->tailPosition++;

            }

        }
    }
}

/*
 *  Helper Functions
 *
 * */

uint16_t is_Circ_Buf_Full(CircBuf_t **buf)
{
    if ((*buf)->num_items == (*buf)->length)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

uint16_t is_Circ_Buf_Empty(CircBuf_t **buf)
{
    if ((*buf)->num_items == 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

uint16_t currentSize(CircBuf_t **buf){
    return (*buf)->num_items;
}

void print(CircBuf_t *buf){
    UART_putchar_n("******************", 18);


    if(buf->num_items ==0)return;

    uint16_t *oldTail = buf->tail;
    uint16_t numberPrinted =0;
    uint16_t oldTailPosition = buf->tailPosition;
    uint16_t dataRead;

    for(numberPrinted =0; numberPrinted < buf->num_items; numberPrinted++){

        /*================= Convert data to ascii =====================================*/


        dataRead = *oldTail;
 /*================= Convert data to ascii =====================================*/
        char dataString[10];

        ftoa(dataRead, dataString, 10);

        UART_putchar_n(dataString, strlen(dataString));
        UART_putchar(13);

  /*================= end data to ascii =====================================*/

       // UART_putchar(dataRead);
        oldTail++;//move the pointer
        oldTailPosition++;

        if(oldTailPosition >= buf->length){
            //send it back to zero.
            oldTail = buf->baseConst;
            oldTailPosition = 0;
        }
        //grab data and print it.


    }
    UART_putchar_n("******************", 18);

}

void clear_Buffer(CircBuf_t ** buf){
    //check to see if valid
    uint16_t * destination = (*buf)->baseConst + ((*buf)->length) *sizeof(uint16_t);

        //clear buffer
    uint16_t *clear =   (*buf)->tail;
        int i;
        uint8_t value = (*buf)->num_items;
        for( i=0; i<value; i++){
           *clear = 0;
           clear ++;
          if(clear== destination){
              clear = (*buf)->baseConst;
              }
            }
        (*buf)->head = (*buf)->baseConst;
        (*buf)->tail = (*buf)->baseConst;
        (*buf)->num_items =0;
        //note: we don't actually clear data, we just say that its available to be written again.
        //do not clear the buffer pointer, we will lose access to it.we are clearning not deleting


}

