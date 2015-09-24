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
    asm volatile (\
        /*---------------------------------------------------------------*/
	/* r4  - lower word of y					 */
        /* r5  - higher word of y		                         */
	/* r6  - lower word of x					 */
        /* r7  - higher word of x		                         */
	/* r8  - lower word of k					 */
        /* r9  - higher word of k		                         */
        /* r10 - 		                         		 */
        /* r11 - 		                         		 */
        /* r12 - temp                                       		 */
        /* r13 - Loop counter                                            */
        /* r14 - RoundKeys i                                             */
        /* r15 - block                                           	 */
        /*---------------------------------------------------------------*/
        /* Store all modified registers                                  */
        /*---------------------------------------------------------------*/
	"push   r4;                 \n"
        "push   r5;                 \n"
        "push   r6;                 \n"
        "push   r7;                 \n"
        "push   r8;                 \n"
        "push   r9;                 \n"
        "push   r12;                \n"
        "push   r13;                \n"
        "push   r14;                \n"
        "push   r15;                \n"
        /*---------------------------------------------------------------*/
        "mov    %[block],       r15;\n"
        "mov    %[roundKeys],   r14;\n" 
        /*---------------------------------------------------------------*/
        /* load plain text	                                         */
        /*---------------------------------------------------------------*/
        "mov    @r15+,       	r4;\n"
        "mov    @r15+,       	r5;\n"
        "mov    @r15+,       	r6;\n"
        "mov    @r15+,       	r7;\n
        /*---------------------------------------------------------------*/
        "mov    #27,            r13;\n" /* 27 rounds                     */
"round_loop:\n"
        /* k = r9:r8;	*/ 
        "mov	@r14+,       	r8;\n"  
        "mov   	@r14+,        	r9;\n"
	/* x = S(-8)(x) */
	"mov.b	r6, 		r12;\n"
 	"xor.b	r7, 		r12;\n"
	"swpb	r6;\n"
	"swpb	r7;\n"
	"swpb	r12;\n"
	"xor	r12,		r6;\n"
	"xor	r12,		r7;\n"
	/* x = S(-8)(x) + y */
	"add	r4,		r6;\n"
	"adc	r5,		r7;\n"
	/* x = [S(-8)(x) + y] eor k */
	"eor	r8,		r6;\n"
	"adc	r9,		r7;\n"
	
	/* y = s(3)y */
	"rla	r4;\n" /* S(-1) */
	"rlc	r5;\n"
	"adc	r4;\n"
	"rla	r4;\n" /* S(-1) */
	"rlc	r5;\n"
	"adc	r4;\n"
	"rla	r4;\n" /* S(-1) */
	"rlc	r5;\n"
	"adc	r4;\n"
	/* y = s(3)y eor [S(-8)(x) + y] eor k */
	"eor	r6,		r4;\n"
	"adc	r7,		r5;\n"

	/* loop control */
        "dec	r13;\n"
	"jne	round_loop;\n"
        /*---------------------------------------------------------------*/
        /* Restore registers                                             */
        /*---------------------------------------------------------------*/
        "pop    r15;                \n"
        "pop    r14;                \n"
        "pop    r13;                \n"
        "pop    r12;                \n"
        "pop    r9;                 \n"
        "pop    r8;                 \n"
        "pop    r7;                 \n"
        "pop    r6;                 \n"
        "pop    r5;                 \n"
	"pop    r4;                 \n"
        /*---------------------------------------------------------------*/
    :
    : [block] "m" (block), [roundKeys] "m" (roundKeys)
); 
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


