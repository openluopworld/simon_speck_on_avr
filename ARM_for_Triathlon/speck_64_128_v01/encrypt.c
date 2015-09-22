/*
 *
 * University of Luxembourg
 * Laboratory of Algorithmics, Cryptology and Security (LACS)
 *
 * FELICS - Fair Evaluation of Lightweight Cryptographic Systems
 *
 * Copyright (C) 2015 University of Luxembourg
 *
 * Written in 2015 by Yann Le Corre <yann.lecorre@uni.lu>
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
#include "primitives.h"


/*
 * Speck_64_128_v01
 * Encryption
 */
#ifdef AVR
/*----------------------------------------------------------------------------*/
/* Encryption -- AVR			                                      */
/*----------------------------------------------------------------------------*/
void Encrypt(uint8_t *block, uint8_t *roundKeys)
{
	
}

#else
#ifdef MSP
/*----------------------------------------------------------------------------*/
/* Encryption -- MSP			                                      */
/*----------------------------------------------------------------------------*/
void Encrypt(uint8_t *block, uint8_t *roundKeys)
{
	
}

#else
#ifdef ARM
/*----------------------------------------------------------------------------*/
/* Encryption -- ARM			                                      */
/*----------------------------------------------------------------------------*/
void Encrypt(uint8_t *block, uint8_t *roundKeys)
{
    asm volatile (\
	/*--------------------------------------------------------------------*/
        /* r0  - X high byte to low byte                                      */
        /* r1  - Y high byte to low byte                                      */
        /* r2  - Temporary use                                                */
        /* r3  - Temporary use                                                */
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
	"mov			r8,			#27;			\n"
"encrypt_round_loop:                           	   				\n"
	/* load k 								*/
	"ldmia			r7!, 			{r9}; 			\n" /* r7 will add 4 after the instruction */
	/* x = S(-8)(x) + y 							*/
	"ror			r0,			#8;			\n"
	"add			r0,			r0,		r1; 	\n"
	/* x = k eor (S(-8)(x) + y) 						*/
	"eor			r0,			r9, 		r0;	\n"
	/* y = S(3)(y) 								*/
	"mov			r2,			r1,		lsl#3;	\n"
	"mov			r3,			r1,		lsr#29;	\n"
	"eor			r1, 			r2,		r3;	\n"
	/* y = S(3)(y) eor ( S(-8)(x) + y ) eor k 				*/
	"eor			r1, 			r1,		r0;	\n"
	/* loop control 							*/
	"subs			r8, 			r8, 		#1;	\n"
	"bne			encrypt_round_loop;				\n"
	/*--------------------------------------------------------------------*/
        /* Store state                                                        */
        /*--------------------------------------------------------------------*/
        "ldmia			sp!,			{r6}; 			\n"
        "stmia			r6,			{r0-r1};           	\n"
        /*--------------------------------------------------------------------*/
        /* Restore registers                                                  */
        /*--------------------------------------------------------------------*/
        "ldmia        		sp!,      		{r0-r12,lr};           	\n"
        /*--------------------------------------------------------------------*/
	:
    : [block] "r" (block), [roundKeys] "r" (roundKeys) 
	);
}

#else
/*----------------------------------------------------------------------------*/
/* Encryption -- Default C Implementation				      */
/*----------------------------------------------------------------------------*/
#include <stdint.h>

#include "cipher.h"
#include "constants.h"
#include "primitives.h"

void Encrypt(uint8_t *block, uint8_t *roundKeys)
{
	uint32_t *rk = (uint32_t *)roundKeys;
	uint32_t *rightPtr = (uint32_t *)block;
	uint32_t *leftPtr = rightPtr + 1;
	uint8_t i;

	for (i = 0; i < NUMBER_OF_ROUNDS; ++i)
	{
		*leftPtr = (rorAlpha(*leftPtr) + *rightPtr) ^ READ_ROUND_KEY_DOUBLE_WORD(rk[i]);
		*rightPtr = rolBeta(*rightPtr) ^ *leftPtr;
	}
}

#endif
#endif
#endif


