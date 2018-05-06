/*
 * lab.h
 *
 *  Created on: Sep 5, 2017
 *      Author: Kayla
 */

#ifndef LAB1_H_
#define LAB1_H_

#define MaskSet (0xC031)
#define MaskCl (0x79FF)
#define MaskT (0x0FF0)

#define P1in (*(uint8_t *)0x40004C00)
#define P1sel0 (*(uint8_t *)0x40004C0A)
#define P1sel1 (*(uint8_t *)0x40004C0C)
#define P1out (*(uint8_t *)0x40004C02)
#define P1dir (*(uint8_t *)0x40004C04)

/* report_types will store the size of all the different data types into an array */
void report_types(void);

/* report_pointer_types has a temporary variable that outputs the sizes of each data type pointer*/
void report_pointer_types(void);

/* Measure times it takes to run for loop iterations*/
void make_square_wave();

/* Creates square wave */
void SquareWave_Full();


#endif /*LAB1_H_ */
