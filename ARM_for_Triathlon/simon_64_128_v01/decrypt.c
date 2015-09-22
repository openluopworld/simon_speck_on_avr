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
 * decryption
 */
#ifdef AVR
/*----------------------------------------------------------------------------*/
/* decryption -- AVR			                                      */
/*----------------------------------------------------------------------------*/
void Decrypt(uint8_t *block, uint8_t *roundKeys)
{
	/* Add here the cipher decryption implementation */
}

#else
#ifdef MSP
/*----------------------------------------------------------------------------*/
/* decryption -- MSP			                                      */
/*----------------------------------------------------------------------------*/
void Decrypt(uint8_t *block, uint8_t *roundKeys)
{
	/* Add here the cipher decryption implementation */
}

#else
#ifdef ARM
/*----------------------------------------------------------------------------*/
/* decryption -- ARM			                                      */
/*----------------------------------------------------------------------------*/
void Decrypt(uint8_t *block, uint8_t *roundKeys)
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
	"ldmia			r6, 			{r0-r1}; 		\n"
        "stmdb			sp!, 			{r6};			\n"
	"mov			r8,			#44;			\n"
	/* end of keys 								*/
        "add           		r7,            		r7,        	#176;	\n"
"decrypt_round_loop:								\n"
        /* laod key 								*/
        "ldmdb			r7!,			{r9};             	\n"
	/* k = k eor x*/
        "eor			r9,			r0, 		r9;	\n"  
	/* x = y, move y to x 							*/
	"mov 			r0, 			r1;			\n"
	/* y = S(1)(y) 								*/
	"ands			r2,			r1,		#0x80000000;	\n"
	"lsl			r1,			#1;			\n"
	"orrne			r1,			r1, 		#0x1;	\n"
	/* t = S(1)y, move y to t 						*/
	"mov 			r5, 			r1;			\n"
	/* t = S(1)y and S(8)(y) 							*/
	"mov			r3,			r0,		lsl#8;	\n"
	"mov			r4,			r0,		lsr#24;	\n"
	"eor			r3, 			r3,		r4;	\n"
	"and			r5,			r5,		r3;	\n"
	/* y = S(2)(y) 								*/
	"ands			r2,			r1,		#0x80000000;	\n"
	"lsl			r1,			#1;			\n"
	"orrne			r1,			r1, 		#0x1;	\n"
	/* y =  [S(1)y and S(8)(y)] eor S(2)y 					*/
	"eor			r1,			r1,		r5;	\n"
	/* y =  [S(1)y and S(8)(y)] eor S(2)y eor k 				*/
	"eor			r1, 			r1,		r9;	\n"
	/* loop control 							*/
	"subs          		r8,           	 	r8,       	#1;	\n"
        "bne     	    	decrypt_round_loop;				\n" 
        /*--------------------------------------------------------------------*/
        /* Store state                                                        */
        /*--------------------------------------------------------------------*/
        "ldmia        		sp!,             	{r6};           	\n"
        "stmia         		r6,          		{r0-r1};           	\n"
        /*--------------------------------------------------------------------*/
        /* Restore registers                                                  */
        /*--------------------------------------------------------------------*/
        "ldmia			sp!,      		{r0-r12,lr};           \n"
        /*--------------------------------------------------------------------*/
    :
    : [block] "r" (block), [roundKeys] "r" (roundKeys) 
	); 
}

#else
/*----------------------------------------------------------------------------*/
/* decryption -- Default c implementation                                     */
/*----------------------------------------------------------------------------*/
#include <stdint.h>

#include "cipher.h"
#include "constants.h"
#include "primitives.h"

void Decrypt(uint8_t *block, uint8_t *roundKeys)
{
    	int8_t i;
	uint32_t *rk = (uint32_t *)roundKeys;
	uint32_t *rightPtr = (uint32_t *)block;
    	uint32_t *leftPtr = (rightPtr + 1);
	uint32_t tmp;
	uint32_t k;

	for (i = NUMBER_OF_ROUNDS - 1; i >= 0; i--)
	{
		k = READ_ROUND_KEY_DOUBLE_WORD(rk[i]);
		tmp = *rightPtr;
		*rightPtr = f(*rightPtr) ^ *leftPtr ^ k;
		*leftPtr = tmp;
	}
}
#endif
#endif
#endif
