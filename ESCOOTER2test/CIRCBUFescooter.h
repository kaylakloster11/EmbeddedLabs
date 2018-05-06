/*
 * CIRCBUFescooter.h
 *
 *  Created on: Nov 1, 2017
 *      Author: Kayla
 */

#ifndef CIRCBUFESCOOTER_H_
#define CIRCBUFESCOOTER_H_


#include "msp.h"
#define BUFFER (60)

typedef struct
{
   uint16_t * buffer;                    // Pointer to the base of the buffer in the heap
   volatile uint16_t * head;             // Pointer to the first item
   volatile uint16_t * tail;             // Pointer to the last item
   volatile uint32_t num_items;          // Number of items in buffer
   volatile uint32_t length;            // Max number of items the buffer can hold
} CircBuf_t;


typedef enum CB_Status{
    // Circular Buffer error (general)
    CB_STATUS_EMPTY = 0,
    CB_STATUS_ERROR = 0,
    CB_STATUS_FULL = 1,
    CB_STATUS_OVERWRITE = 1,
    CB_STATUS_INITIAL = 1,
    CB_STATUS_WRAP = 1,
    CB_STATUS_MID = 2,
} CB_Status;


CB_Status CB_initialize_buffer(CircBuf_t ** buf, uint32_t length);

CB_Status CB_add_item_to_buffer(CircBuf_t * buf, uint16_t item);

CB_Status CB_delete_buffer(CircBuf_t * buf);

CB_Status CB_clear_buffer(CircBuf_t * buf);

uint8_t remove_item_from_buffer(CircBuf_t * buf);

int8_t CB_is_buffer_empty(CircBuf_t * buf);

int8_t CB_is_buffer_full(CircBuf_t * buf);



#endif /* CIRCBUFESCOOTER_H_ */
