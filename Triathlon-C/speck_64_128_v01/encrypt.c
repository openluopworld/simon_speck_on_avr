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
    asm volatile (\
	/*------------------------------------------------------*/
        /* Registers allocation:				*/
        /* r0 - r7 : plain text					*/
        /* r8 - r11: 						*/
        /* r12-r15 : round key					*/
        /* r16-r23 : 						*/
        /* r24     : currentRound				*/
        /* r25     : 						*/
        /* r26:r27 : X roundKeys				*/
        /* r28:r29 : Y key					*/
        /* r30:r31 : Z r30 stores 0				*/
        /* ---------------------------------------------------- */
        /* Store all modified registers				*/
        /* ---------------------------------------------------- */
	"push	r0;	\n"
	"push	r1;	\n"
	"push	r2;	\n"
        "push  	r3;	\n"
        "push  	r4;	\n"
	"push  	r5;	\n"
	"push  	r6;	\n"
        "push  	r7;	\n"
	"push  	r12;	\n"
	"push  	r13;	\n"
	"push  	r14;	\n"
        "push  	r15;	\n"
        "push 	r24;	\n"
        "push 	r26;	\n" 
        "push 	r27;	\n" 
        "push 	r28;	\n"
        "push 	r29;	\n"
	"push	r30;	\n"
	"push	r31;	\n"
        /* ---------------------------------------------------- */
	/* encryption						*/
/*	"ldi 		r26, 		lo8(block);		\n" */
/*	"ldi 		r27, 		hi8(block);		\n" */
	"clr 		r24;					\n"
/*	"ldi 		r28, 		lo8(roundKeys);		\n" */
/*	"ldi 		r29, 		hi8(roundKeys);		\n" */
	/* load the plaintext from RAM to registers [r7,...,r0], X = [r7, r6, r5, r4], Y = [r3, r2, r1, r0] */
	"ld 		r7, 		x+ ;			\n"
	"ld 		r6, 		x+ ;			\n"
	"ld 		r5, 		x+ ;			\n"
	"ld 		r4, 		x+ ;			\n"
	"ld 		r3, 		x+ ;			\n"
	"ld 		r2, 		x+ ;			\n"
	"ld 		r1, 		x+ ;			\n"
	"ld 		r0, 		x+ ;			\n"
	"clr 		r30;					\n"
"encLoop:							\n"
	/* load k: [r15, r14, r13, r12], r12 is the lowest byte */
	"ld 		r12, 		y+;			\n"
	"ld 		r13, 		y+;			\n"
	"ld 		r14, 		y+;			\n"
	"ld 		r15, 		y+;			\n"
	/* x = S(8)( S(-8)(x) + y) 				*/
	"add 		r5, 		r0;			\n" /* x1 = x1 + y0 */
	"adc 		r6, 		r1;			\n" /* x2 = x2 + y1 */
	"adc 		r7, 		r2;			\n" /* x3 = x3 + y2 */
	"adc 		r4, 		r3;			\n" /* x0 = x0 + y3 */
	/* k = ( S(-8)(x) + y ) eor k 				*/
	"eor 		r12, 		r5;			\n"
	"eor 		r13, 		r6;			\n"
	"eor 		r14, 		r7;			\n"
	"eor 		r15, 		r4;			\n"
	/* y = s(3)y 						*/
	"lsl 		r0;					\n" /* loop 1 */
	"rol 		r1;					\n"
	"rol 		r2;					\n"
	"rol 		r3;					\n"
	"adc 		r0, 		r30;			\n"
	"lsl 		r0;					\n" /* loop 2 */
	"rol 		r1;					\n"
	"rol 		r2;					\n"
	"rol 		r3;					\n"
	"adc 		r0, 		r30;			\n"
	"lsl 		r0;					\n" /* loop 3 */
	"rol 		r1;					\n"
	"rol 		r2;					\n"
	"rol 		r3;					\n"
	"adc 		r0, 		r30;			\n"
	/* y = S(3)(y) eor ( S(-8)(x) + y ) eor k 		*/
	"eor 		r0, 		r12;			\n"
	"eor 		r1, 		r13;			\n"
	"eor 		r2, 		r14;			\n"
	"eor 		r3, 		r15;			\n"
	/* x = ( S(-8)(x) + y ) eor k 				*/
	"movw 		r4, 		r12;			\n" /* r5:r4 = r13:r12 */
	"movw 		r6, 		r14;			\n" /* r7:r6 = r15:r14 */
	/* loop control 					*/
	"inc 		r24;					\n"
	"cpi 		r24, 		27;			\n"
	"brne 		encLoop;				\n"
	/* ---------------------------------------------------- */
	/* move cipher text back to plain text 			*/
	"st 		-x, 		r0;			\n"
	"st 		-x, 		r1;			\n"
	"st 		-x, 		r2;			\n"
	"st 		-x, 		r3;			\n"
	"st 		-x, 		r4;			\n"
	"st 		-x, 		r5;			\n"
	"st 		-x, 		r6;			\n"
	"st 		-x, 		r7;			\n"
	/* ---------------------------------------------------- */
	/* Restore all modified registers			*/
	"pop  r31;	 \n"
	"pop  r30;       \n"
        "pop  r29;       \n"
        "pop  r28;       \n"
        "pop  r27;       \n" 
        "pop  r26;       \n" 
        "pop  r24;       \n" 
	"pop  r15;       \n"
	"pop  r14;       \n"
	"pop  r13;       \n"
	"pop  r12;       \n"
	"pop  r7;        \n"
	"pop  r6;        \n"
	"pop  r5;        \n"
	"pop  r4;        \n"
        "pop  r3;        \n"
        "pop  r2;        \n"
        "pop  r1;        \n"
        "pop  r0;        \n"
    :
    : [block] "x" (block), [roundKeys] "y" (roundKeys)
); 
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
        "mov    %[block],       r15;	\n"
        "mov    %[roundKeys],   r14;	\n" 
        /*---------------------------------------------------------------*/
        /* load plain text	                                         */
        /*---------------------------------------------------------------*/
        "mov    @r15+,       	r4;	\n"
        "mov    @r15+,       	r5;	\n"
        "mov    @r15+,       	r6;	\n"
        "mov    @r15+,       	r7;	\n"
        /*---------------------------------------------------------------*/
        "mov    #27,            r13;	\n" /* 27 rounds                 */
"round_loop:				\n"
        /* k = r9:r8;			*/ 
        "mov	@r14+,       	r8;	\n"  
        "mov   	@r14+,        	r9;	\n"
	/* x = S(-8)(x) 		*/
	"mov.b	r6, 		r12;	\n"
 	"xor.b	r7, 		r12;	\n"
	"swpb	r6;			\n"
	"swpb	r7;			\n"
	"swpb	r12;			\n"
	"xor	r12,		r6;	\n"
	"xor	r12,		r7;	\n"
	/* x = S(-8)(x) + y 		*/
	"add	r4,		r6;	\n"
	"adc	r5,		r7;	\n"
	/* x = [S(-8)(x) + y] eor k 	*/
	"xor	r8,		r6;	\n"
	"xor	r9,		r7;	\n"
	/* y = s(3)y 			*/
	"rla	r4;			\n" /* S(-1) */
	"rlc	r5;			\n"
	"adc	r4;			\n"
	"rla	r4;			\n" /* S(-1) */
	"rlc	r5;			\n"
	"adc	r4;			\n"
	"rla	r4;			\n" /* S(-1) */
	"rlc	r5;			\n"
	"adc	r4;			\n"
	/* y = s(3)y eor [S(-8)(x) + y] eor k */
	"xor	r6,		r4;	\n"
	"xor	r7,		r5;	\n"
	/* loop control 		*/
        "dec	r13;			\n"
	"jne	round_loop;		\n"
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
    : [block] "" (block), [roundKeys] "" (roundKeys)
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
    : [block] "" (block), [roundKeys] "" (roundKeys) 
	);
}

#else
/*----------------------------------------------------------------------------*/
/* Encryption -- Default C Implementation				      */
/*----------------------------------------------------------------------------*/
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


