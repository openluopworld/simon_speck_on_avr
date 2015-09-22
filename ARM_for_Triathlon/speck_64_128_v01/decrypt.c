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
 * Decryption
 */
#ifdef AVR
/*----------------------------------------------------------------------------*/
/* Decryption -- AVR			                                      */
/*----------------------------------------------------------------------------*/
void Decrypt(uint8_t *block, uint8_t *roundKeys)
{
	
}

#else
#ifdef MSP
/*----------------------------------------------------------------------------*/
/* Decryption -- MSP			                                      */
/*----------------------------------------------------------------------------*/
void Decrypt(uint8_t *block, uint8_t *roundKeys)
{
	
}

#else
#ifdef ARM
/*----------------------------------------------------------------------------*/
/* Decryption -- ARM			                                      */
/*----------------------------------------------------------------------------*/
void Decrypt(uint8_t *block, uint8_t *roundKeys)
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
	"ldmia			r6, 			{r0-r1}; 		\n"
        "stmdb			sp!, 			{r6};			\n"
	"mov			r8,			#27;			\n"
	/* end of keys 								*/
        "add           		r7,            		r7,        	#108;	\n"
"decrypt_round_loop:								\n"
        /* AddRoundKey 								*/
        "ldmdb			r7!,			{r9};             	\n"
	/* k = k eor x 								*/
        "eor			r9,			r9, 		r0;	\n"
	/* x = x eor y 								*/
        "eor			r0,			r0, 		r1;	\n"
	/* x = S(-3)x 								*/
	"ror			r0,			#3;			\n"
	/* y = x 								*/
	"mov			r1,			r0;			\n"
	/* x = k - x 								*/
	"sub			r0,			r9,		r0;	\n"
	/* x = S(8)(k) */
	"mov			r2,			r9,		lsl#8;	\n"
	"mov			r3,			r9,		lsr#24;	\n"
	"eor			r0, 			r2,		r3;	\n"
	/* loop control 							*/
	"subs          		r8,           		r8,		#1;	\n"
        "bne     	    	decrypt_round_loop;				\n" 
        /*--------------------------------------------------------------------*/
        /* Store state                                                        */
        /*--------------------------------------------------------------------*/
        "ldmia        		sp!,            	{r6};           	\n"
        "stmia         		r6,          		{r0-r1};          	\n"
        /*--------------------------------------------------------------------*/
        /* Restore registers                                                  */
        /*--------------------------------------------------------------------*/
        "ldmia        		sp!,      		{r0-r12,lr};		\n"
        /*--------------------------------------------------------------------*/
    :
    : [block] "r" (block), [roundKeys] "r" (roundKeys) 
	); 
}

#else
/*----------------------------------------------------------------------------*/
/* Decryption -- Default C Implementation				      */
/*----------------------------------------------------------------------------*/
#include <stdint.h>

#include "cipher.h"
#include "constants.h"
#include "primitives.h"

void Decrypt(uint8_t *block, uint8_t *roundKeys)
{
	uint32_t *rk = (uint32_t *)roundKeys;
	uint32_t *rightPtr = (uint32_t *)block;
	uint32_t *leftPtr = rightPtr + 1;
	int8_t i;

	for (i = NUMBER_OF_ROUNDS - 1; i >= 0; --i)
	{
		*rightPtr = rorBeta(*rightPtr ^ *leftPtr);
		*leftPtr = rolAlpha((*leftPtr ^ READ_ROUND_KEY_DOUBLE_WORD(rk[i])) - *rightPtr);
	}
}

#endif
#endif
#endif

