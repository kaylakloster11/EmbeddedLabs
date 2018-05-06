/*
 * circ_buffer_basic.h
 *
 *  Created on: Oct 6, 2017
 *      Author: kk
 */

#ifndef CIRC_BUFFER_BASIC_H_
#define CIRC_BUFFER_BASIC_H_

#include  "msp.h"

typedef struct{
    uint16_t * baseConst;                       // pointer to the base of the buffer in heap
    volatile uint16_t * head;                // Pointer to first item //start of buffer  // FIFO
    volatile uint16_t * tail;                // Marks the end of buffer, where new things are added. so head only moves when we pop out. and tail moves when we add.
    volatile uint16_t  num_items;          // good to know how many items are in the buffer , so array may be 50 long, but only 3 spots are filled. we don't want to go from 4 to 50.
     uint32_t length;               // Max number of items that can be held
    volatile uint16_t tailPosition;         //This will be useful to know if there was a loop around, if there was then we don't want tail to go past
                                            //out of bounds so if tailPosition == 255 count, then move tail to buf. loop back.
} CircBuf_t;

/*
 * Initializes a circular buffer and stores it at the global CircBuf Pointer.
 * Pass the address of the pointer
 * Uses Malloc to create correct size
 * */
void initialize_Circ_Buffer(CircBuf_t **buf, uint16_t _length);


/*
 * Adds an item to the HEAD of the circular Buffer
 * Increments the number of items.
 * */
void add_To_Buffer(CircBuf_t **buf, uint16_t item);


/*
 * Removes an item from the Tail of the circular Buffer
 * Increments tail position
 * Increment tail pointer
 * Decrements number of items
 * */
uint16_t  remove_From_Buffer(CircBuf_t **buf);


/*
 * Checks size of the current buffer
 * number_of_items = ? = length
 * Returns 1 if full
 * */
uint16_t is_Circ_Buf_Full(CircBuf_t **buf);

/*
 * Checks size of current buffer
 * number_of_items = ? = 0
 * Returns 1 if Empty
 * */
uint16_t is_Circ_Buf_Empty(CircBuf_t **buf);

/* Sets head, tail pointers to base
 *      Sets all pointers to have the value 0;
 *      Sets number_of_items = 0;
 * */
void clear_Buffer(CircBuf_t **buf);

/*
 * Delete memory allocated with Free
 * */
uint16_t delete_Circ_Buffer(CircBuf_t **buf);



/* Helper function
 * Returns the current size of the circular buffer.
 * */
uint16_t currentSize(CircBuf_t **buf);


void print(CircBuf_t *buf);




#endif /* CIRC_BUFFER_BASIC_H_ */
