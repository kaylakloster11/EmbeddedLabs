/*
 * lab1.c
 *
 *  Created on: Sep 6, 2017
 *      Author: Kayla
 */

#include <stdint.h>
#include "lab1.h"
#include "msp.h"

void report_types(void)
{
    uint32_t foo[20];
    foo[0] = sizeof(char);
    foo[1] = sizeof(int);
    foo[2] = sizeof(short);
    foo[3] = sizeof(long);
    foo[4] = sizeof(long int);
    foo[5] = sizeof(short int);
    foo[6] = sizeof(float);
    foo[7] = sizeof(double);
    foo[8] = sizeof(unsigned char);
    foo[9] = sizeof(signed char);
    foo[10] = sizeof(unsigned short);
    foo[11] = sizeof(signed short);
    foo[12] = sizeof(int8_t);
    foo[13] = sizeof(uint8_t);
    foo[14] = sizeof(int16_t);
    foo[15] = sizeof(uint16_t);
    foo[16] = sizeof(int32_t);
    foo[17] = sizeof(uint32_t);
    foo[18] = sizeof(signed int);
    foo[19] = sizeof(unsigned int);

}

void report_pointer_types(void)
{
    uint32_t foo;
    foo = sizeof(char *);
    foo = sizeof(int *);
    foo = sizeof(short *);
    foo = sizeof(long*);
    foo = sizeof(long int*);
    foo = sizeof(short int*);
    foo = sizeof(float*);
    foo = sizeof(double*);
    foo = sizeof(unsigned char*);
    foo = sizeof(signed char*);
    foo = sizeof(unsigned short*);
    foo = sizeof(signed short*);
    foo = sizeof(int8_t*);
    foo = sizeof(uint8_t*);
    foo = sizeof(int16_t*);
    foo = sizeof(uint16_t*);
    foo = sizeof(int32_t*);
    foo = sizeof(uint32_t*);
}

void make_square_wave()
{
    P1out &= 0x00;      // set P1.0 output low
    P1sel1 &= 0x00;    // General I/O mode
    P1dir |= 0X80;       // P1.0 set as output direction
    P1sel0 &= 0x00; //General I/O mode

    unsigned int i=0;
    P1out |= BIT7;
    for( i = 0 ; i < 1 ; i++);
    P1out &= ~BIT7;
}



 void SquareWave_Full()
 {
     P1out &= 0x00;      // set P1.0 output low
     P1sel1 &= 0x00;    // General I/O mode
     P1dir |= 0X80;       // P1.0 set as output direction
     P1sel0 &= 0x00; //General I/O mode

     unsigned int i=0;
     while(1) {
            P1out ^= BIT7;
            for( i = 0 ; i < 13720 ; i++);
        }
 }



