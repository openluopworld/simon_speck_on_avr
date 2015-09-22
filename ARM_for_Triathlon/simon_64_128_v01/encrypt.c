/*
 *
 * University of Luxembourg
 * Laboratory of Algorithmics, Cryptology and Security (LACS)
 *
 * FELICS - Fair Evaluation of Lightweight Cryptographic Systems
 *
 * Copyright (C) 2015 University of Luxembourg
 *
 * Written in 2015 by Daniel Dinu <dumitru-daniel.dinu@uni.lu>
 *
 * This file is part of FELICS.
 *
 * FELICS is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * FELICS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <stdint.h>

#include "cipher.h"
#include "constants.h"

/*
 * Simon_64_128_v01
 * encryption
 */
#ifdef AVR
/*----------------------------------------------------------------------------*/
/* Encryption -- AVR			                                      */
/*----------------------------------------------------------------------------*/
void Encrypt(uint8_t *block, uint8_t *roundKeys)
{
	/* Add here the cipher encryption implementation */
}

#else
#ifdef MSP
/*----------------------------------------------------------------------------*/
/* Encryption -- MSP			                                      */
/*----------------------------------------------------------------------------*/
void Encrypt(uint8_t *block, uint8_t *roundKeys)
{
	/* Add here the cipher encryption implementation */
}

#else
#ifdef ARM
/*----------------------------------------------------------------------------*/
/* Encryption -- ARM			                                      */
/*----------------------------------------------------------------------------*/
/*
 * LSL provides the value of a register multiplied by a power of two. 
 * LSR provides the unsigned value of a register divided by a variable power of two.
 * Both instructions insert zeros into the vacated bit positions.
 *
 * ROR provides the value of the contents of a register rotated by a value.
 * The bits that are rotated off the right end are inserted into the vacated bit positions on the left
 */
void Encrypt(uint8_t *block, uint8_t *roundKeys)
{
	asm volatile (\
	/*--------------------------------------------------------------------*/
        /* r0  - X high byte to low byte                                      */
        /* r1  - Y high byte to low byte                                      */
        /* r2  - Temporary use                                                */
        /* r3  - Temporary use                                                */
        /* r4  - Temporary use                                                */
        /* r5  - Temporary use                                                */
        /* r6  - point of block                                               */
        /* r7  - point of roundKeys                                           */
        /* r8  - loop counter                                                 */
        /* r9  - round key                                                    */
        /*--------------------------------------------------------------------*/
        /* Store all modified registers                                       */
        /*--------------------------------------------------------------------*/
	"stmdb			sp!,			{r0-r12,lr};		\n" 
        "mov			r6, 			%[block]; 		\n" 
        "mov			r7, 			%[roundKeys]; 		\n"
	/* value of r6 will not change, if it's 'ldmia r6!,{r0-r1}', then r6 will add 8 after the instruction */
	"ldmia			r6, 			{r0-r1}; 		\n"
        "stmdb			sp!, 			{r6};			\n"
	"mov			r8,			#44;			\n"
"encrypt_round_loop:                           	   				\n"
	"ldmia			r7!, 			{r9}; 			\n"
	/* k eor y								*/
	"eor			r9,			r1, 		r9;	\n"
	/* y = x	                                                      	*/
	"mov			r1,			r0;			\n"
	/* x = S(1)x		                                         	*/
	"ands			r2,			r0,		#0x80000000;	\n"
	"lsl			r0,			#1;			\n"
	"orrne			r0,			r0, 		#0x1;	\n"
	/* t1 = S(1)x                                                      	*/
	"mov			r2,			r0;			\n"
	/* t2 = S(8)x								*/
	"mov			r3,			r1,		lsl#8;	\n"
	"mov			r4,			r1,		lsr#24;	\n"
	"eor			r3, 			r3,		r4;	\n"
	/* t1 = S(1)x & S(8)x							*/
	"and			r2,			r2,		r3;	\n"
	/* x = S(2)x , x rotate left with 1 bit again 				*/
	"ands			r3,			r0,		#0x80000000;	\n"
	"lsl			r0,			#1;			\n"
	"orrne			r0,			r0, 		#0x1;	\n"
	/* x eor t1 								*/
	"eor			r0, 			r0, 		r2;	\n"
	/* x eor k 								*/
	"eor			r0, 			r0, 		r9;	\n"
	/* loop control 							*/
	"subs			r8, 			r8, 		#1;	\n"
	"bne			encrypt_round_loop;				\n"
	/*--------------------------------------------------------------------*/
        /* Store state                                                        */
        /*--------------------------------------------------------------------*/
        "ldmia			sp!,			{r6}; 			\n"
        "stmia			r6,			{r0-r1};		\n"
        /*--------------------------------------------------------------------*/
        /* Restore registers                                                  */
        /*--------------------------------------------------------------------*/
        "ldmia        sp!,      {r0-r12,lr};           \n" /*                 */
        /*--------------------------------------------------------------------*/
	:
    : [block] "r" (block), [roundKeys] "r" (roundKeys) 
	);
}

#else
/*----------------------------------------------------------------------------*/
/* Encryption -- Default c implementation                                     */
/*----------------------------------------------------------------------------*/
void Encrypt(uint8_t *block, uint8_t *roundKeys)
{
	/* Add here the cipher encryption implementation */
}
#endif
#endif
#endif
