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
    asm volatile (\
	/*------------------------------------------------------*/
        /* Registers allocation:				*/
        /* r0 - r7 : cipher text				*/
        /* r8 - r11: round key					*/
        /* r12-r15 : 						*/
        /* r16-r23 : 						*/
        /* r24     : currentRound				*/
        /* r25     : 						*/
        /* r26:r27 : X roundKeys				*/
        /* r28:r29 : Y key					*/
        /* r30:r31 : 						*/
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
	"push  	r8;	\n"
	"push  	r9;	\n"
	"push  	r10;	\n"
        "push  	r11;	\n"
        "push 	r24;	\n"
        "push 	r26;	\n" 
        "push 	r27;	\n" 
        "push 	r28;	\n"
        "push 	r29;	\n"
	/* ---------------------------------------------------- */
	/* decryption						*/
/*	"ldi 		r26, 		lo8(block);		\n" */
/*	"ldi 		r27, 		hi8(block);		\n" */
	"clr 		r24;					\n"
/*	"ldi 		r28, 		lo8(roundKeys+ROUND_KEYS_SIZE);\n" */
/*	"ldi 		r29, 		hi8(roundKeys+ROUND_KEYS_SIZE);\n" */
	"adiw		r28,		63;			\n"
	"adiw		r28,		45;			\n"
	/* load the plaintext from RAM to registers [r7,...,r0], X = [r7, r6, r5, r4], Y = [r3, r2, r1, r0] */
	"ld 		r7, 		x+ ;			\n"
	"ld 		r6, 		x+ ;			\n"
	"ld 		r5, 		x+ ;			\n"
	"ld 		r4, 		x+ ;			\n"
	"ld 		r3, 		x+ ;			\n"
	"ld 		r2, 		x+ ;			\n"
	"ld 		r1, 		x+ ;			\n"
	"ld 		r0, 		x+ ;			\n"
"decLoop:							\n"
	/* store the 4 bytes of sub key to K = [r11, r10, r9, r8] */
	"ld 		r11, 		-y;			\n"
	"ld 		r10, 		-y;			\n"
	"ld 		r9, 		-y;			\n"
	"ld 		r8, 		-y;			\n"
	/* k = k eor x 						*/
	"eor 		r8, 		r4;			\n"
	"eor 		r9, 		r5;			\n"
	"eor 		r10, 		r6;			\n"
	"eor 		r11, 		r7;			\n"
	/* x = x eor y 						*/
	"eor 		r4, 		r0;			\n"
	"eor 		r5, 		r1;			\n"
	"eor 		r6, 		r2;			\n"
	"eor 		r7, 		r3;			\n"
	/* x = S(-3)x 						*/
	"lsr 		r7;					\n"
	"ror 		r6;					\n"
	"ror 		r5;					\n"
	"bst 		r4, 		0;			\n"
	"ror 		r4;					\n"
	"bld 		r7, 		7;			\n"
	"lsr 		r7;					\n"
	"ror 		r6;					\n"
	"ror 		r5;					\n"
	"bst 		r4,		0;			\n"
	"ror 		r4;					\n"
	"bld 		r7, 		7;			\n"
	"lsr 		r7;					\n"
	"ror 		r6;					\n"
	"ror 		r5;					\n"
	"bst 		r4, 		0;			\n"
	"ror 		r4;					\n"
	"bld 		r7, 		7;			\n"
	/* y = x 						*/
	"movw 		r0, 		r4;			\n"
	"movw 		r2, 		r6;			\n"
	/* x = k - x 						*/
	"sub 		r8, 		r4;			\n"
	"sbc 		r9, 		r5;			\n"
	"sbc 		r10,		r6;			\n"
	"sbc 		r11, 		r7;			\n"
	/* x = S(8)k 						*/
	"mov 		r4, 		r11;			\n"
	"mov 		r5, 		r8;			\n"
	"mov 		r6, 		r9;			\n"
	"mov 		r7, 		r10;			\n"
	/* loop control						*/
	"inc 		r24;					\n"
	"cpi 		r24, 		27;			\n"
	"brne 		decLoop;				\n"
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
        "pop  r29;       \n"
        "pop  r28;       \n"
        "pop  r27;       \n" 
        "pop  r26;       \n" 
        "pop  r24;       \n" 
	"pop  r11;       \n"
	"pop  r10;       \n"
	"pop  r9;       \n"
	"pop  r8;       \n"
	"pop  r7;        \n"
	"pop  r6;        \n"
	"pop  r5;        \n"
	"pop  r4;        \n"
        "pop  r3;        \n"
        "pop  r2;        \n"
        "pop  r1;        \n"
        "pop  r0;        \n"
    :
    : [block] "x" (block), [roundKeys] "y" (roundKeys)); 
}

#else
#ifdef MSP
/*----------------------------------------------------------------------------*/
/* Decryption -- MSP			                                      */
/*----------------------------------------------------------------------------*/
void Decrypt(uint8_t *block, uint8_t *roundKeys)
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
        "mov    %[block],       r15;					\n"
        "mov    %[roundKeys],   r14;					\n"
	"add	#26*4,		r14;					\n" 
        /*---------------------------------------------------------------*/
        /* load plain text	                                         */
        /*---------------------------------------------------------------*/
        "mov    @r15+,       	r4;					\n"
        "mov    @r15+,       	r5;					\n"
        "mov    @r15+,       	r6;					\n"
        "mov    @r15+,       	r7;					\n"
        /*---------------------------------------------------------------*/
        "mov    #27,            r13;					\n" /* 27 rounds */
"round_loop:								\n"
        /* k = r9:r8;							*/ 
        "mov	0(r14),       	r8;					\n"  
        "mov   	2(r14),        	r9;					\n"
	/* y = y eor x 							*/
	"xor	r6,		r4;					\n"
	"xor	r7,		r5;					\n"
	/* y = S(-3)(y eor x)						*/
	"bit	#1,		r4;					\n"
	"rrc	r5;							\n"
	"rrc	r4;							\n"
	"bit	#1,		r4;					\n"
	"rrc	r5;							\n"
	"rrc	r4;							\n"
	"bit	#1,		r4;					\n"
	"rrc	r5;							\n"
	"rrc	r4;							\n"
	/* x = x eor k							*/
	"xor	r8,		r6;					\n"
	"xor	r9,		r7;					\n"
	/* x = (x eor k) - S(-3)(y eor x) 				*/
	"sub	r4,		r6;					\n"
	"subc	r5,		r7;					\n"
	/* x = S(8) [(x eor k) - S(-3)(y eor x)] 			*/
	/* A byte instruction with a register destination clears the high 8 bits of the register to 0. */
	/* [http://mspgcc.sourceforge.net/manual/x214.html] 		*/
	/* I think the it means the destination regiser. 		*/
  	"swpb 	r6;							\n"
  	"swpb 	r7;							\n"
	"mov.b 	r6, 		r12;					\n"
  	"xor.b 	r7, 		r12;					\n"
  	"xor  	r12, 		r6;					\n"
  	"xor  	r12, 		r7;					\n"
	/* last k 							*/
	"sub	#4,		r14;					\n"
	/* loop control 						*/
        "dec	r13;							\n"
	"jne	round_loop;						\n"
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
	/* x = S(8)(k) 								*/
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

