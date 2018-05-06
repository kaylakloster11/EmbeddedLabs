/*
 * adc_circbuf.c
 *
 *  Created on: Oct 15, 2017
 *      Author: Kayla
 */


/*
 * circbuf.c
 *
 *  Created on: Sep 25, 2017
 *      Author: Kayla
 */

#include "msp.h"
#include "adc_circbuf.h"
#include <stdint.h>
#include <stdlib.h>
#include "adc.h"

#ifdef QUESTION6

volatile int LetterCount, PuncCount, numCount, otherCount, whiteCount;

//Setup buffer in heap and initialize structure variable
CB_Status CB_initialize_buffer(CircBuf_t ** buf, uint32_t length){
    (*buf)->buffer = (uint16_t *)malloc(length);
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
CB_Status CB_add_item_to_buffer(CircBuf_t * buf, uint16_t item){
    if(buf->num_items == buf->length){
        if(buf->head == buf->tail){
           if(buf->head == (buf->buffer +(buf->length - 1))){     // if it's on the end and buffer full
                buf->tail = buf->buffer;
                *buf->head = item;
                buf->head = buf->buffer;  //wrap around to beginning
                return CB_STATUS_OVERWRITE;
            }
            else{
                buf->tail++;
                *buf->head = item;
                buf->head++;
            }
        }
        else{
           if(buf->head == (buf->buffer +(buf->length - 1))){     // if it's on the end and buffer full
                *buf->head = item;
                buf->head = buf->buffer;  //wrap around to beginning
                return CB_STATUS_OVERWRITE;
            }
        }
    }

    else { //both head and tail at beginning or middle of buffer
        *buf->head = item;
        buf->head++;
        buf->num_items++;
        return CB_STATUS_MID;
    }
}

//Remove item from buffer and adjust params in the structure
uint16_t CB_remove_item_from_buffer(CircBuf_t * buf){
    uint16_t lastbyte;
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

#endif














