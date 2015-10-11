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
 * Key Schedule
 */
#ifdef AVR
/*----------------------------------------------------------------------------*/
/* Key Schedule -- AVR			                                      */
/*----------------------------------------------------------------------------*/
/* L in */
void RunEncryptionKeySchedule(uint8_t *key, uint8_t *roundKeys)
{
    asm volatile (\
	/*------------------------------------------------------*/
        /* Registers allocation:				*/
        /* r0 - r3 : ki, from low byte to high byte		*/
        /* r4 - r7 : li, from low byte to high byte		*/
        /* r8 - r20: 						*/
        /* r21     : store 0					*/
        /* r22     : temp					*/
        /* r23     : 						*/
        /* r24     : currentRound				*/
        /* r25     : 						*/
        /* r26:r27 : X roundKeys				*/
        /* r28:r29 : Y L					*/
        /* r30:r31 : Z key					*/
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
        "push 	r21;	\n"
        "push 	r22;	\n"
        "push 	r24;	\n"
        "push 	r26;	\n" 
        "push 	r27;	\n" 
        "push 	r28;	\n"
        "push 	r29;	\n"
	"push 	r30;	\n"
        "push 	r31;	\n"
	/*---------------------------------------------------------------*/
	/* store master key						 */
/*	"ldi 		r26, 		lo8(roundKeys);			\n" */
/*	"ldi 		r27, 		hi8(roundKeys);			\n" */
/*	"ldi 		r30, 		lo8(key);			\n" */
/*	"ldi 		r31, 		hi8(key);			\n" */
	"ld 		r22, 		z+;				\n"
	"st 		x+, 		r22;				\n"
	"ld 		r22, 		z+;				\n"
	"st 		x+, 		r22;				\n"
	"ld 		r22, 		z+;				\n"
	"st 		x+, 		r22;				\n"
	"ld 		r22, 		z+;				\n"
	"st 		x+, 		r22;				\n"
/*	"ldi 		r28, 		lo8(L);				\n" */
/*	"ldi 		r29, 		hi8(L);				\n" */
	"clr 		r24;						\n"
"loadL:									\n"
	"ld 		r22, 		z+;				\n"
	"st 		y+, 		r22;				\n"
	"inc 		r24;						\n"
	"cpi 		r24, 		12;				\n"
	"brne 		loadL;						\n"
	/*---------------------------------------------------------------*/
	/* key schedule							 */
	"clr 		r24;						\n"
	"clr 		r21;						\n" /* store 0 */
/*	"ldi 		r26, 		lo8(roundKeys);			\n" */
/*	"ldi 		r27, 		hi8(roundKeys);			\n" */
	"sbiw		r26,		4;				\n"
/*	"ldi 		r28, 		lo8(L);				\n" */
/*	"ldi 		r29, 		hi8(L);				\n" */
	"sbiw		r28,		12;				\n"
"subkey:								\n"
	/* [r3,r2,r1,r0] to store ki 					*/
	"ld 		r0, 		x+;				\n"
	"ld 		r1, 		x+;				\n"
	"ld 		r2, 		x+;				\n"
	"ld 		r3, 		x+;				\n"
	/* [r7,r6,r5,r4] to store li 					*/
	"ld 		r4, 		y+;				\n"
	"ld 		r5, 		y+;				\n"
	"ld 		r6, 		y+;				\n"
	"ld 		r7, 		y+;				\n"
	/* k(i)+S(-8)l(i)						*/
	"add 		r5, 		r0;				\n"
	"adc 		r6, 		r1;				\n"
	"adc 		r7, 		r2;				\n"
	"adc 		r4, 		r3;				\n"
	/* eor i 							*/
	"eor 		r5, 		r24;				\n"
	/* S3(ki) 							*/
	"lsl 		r0;						\n"
	"rol 		r1;						\n"
	"rol 		r2;						\n"
	"rol 		r3;						\n"
	"adc 		r0, 		r21;				\n"
	"lsl 		r0;						\n"
	"rol 		r1;						\n"
	"rol 		r2;						\n"
	"rol 		r3;						\n"
	"adc 		r0, 		r21;				\n"
	"lsl 		r0;						\n"
	"rol 		r1;						\n"
	"rol 		r2;						\n"
	"rol 		r3;						\n"
	"adc 		r0, 		r21;				\n"
	/* k(i+1) 							*/
	"eor 		r0, 		r5;				\n"
	"eor 		r1, 		r6;				\n"
	"eor 		r2, 		r7;				\n"
	"eor 		r3, 		r4;				\n"
	/* store k(i+1) 						*/
	"st 		x+, 		r0;				\n"
	"st 		x+, 		r1;				\n"
	"st 		x+, 		r2;				\n"
	"st 		x+,	 	r3;				\n"
	/* set x to k(i+1) 						*/
	"sbiw 		r26, 		4;				\n"
	/* store l(i+3) 						*/
	"adiw 		r28, 		8;				\n"
	"st 		y+, 		r5;				\n"
	"st 		y+, 		r6;				\n"
	"st 		y+, 		r7;				\n"
	"st 		y+, 		r4;				\n"
	"sbiw 		r28, 		12;				\n"
	/* loop control 						*/
	"inc 		r24;						\n"
	"cpi 		r24, 		26;				\n"
	"brne 		subkey;						\n"
	/* ---------------------------------------------------- */
	/* Restore all modified registers			*/
	"pop  r31;       \n"
        "pop  r30;       \n"
        "pop  r29;       \n"
        "pop  r28;       \n"
        "pop  r27;       \n" 
        "pop  r26;       \n" 
        "pop  r24;       \n" 
	"pop  r22;       \n"
	"pop  r21;       \n"
	"pop  r7;        \n"
	"pop  r6;        \n"
	"pop  r5;        \n"
	"pop  r4;        \n"
        "pop  r3;        \n"
        "pop  r2;        \n"
        "pop  r1;        \n"
        "pop  r0;        \n"
	/* ---------------------------------------------------- */
    :
    : [key] "z" (key), [roundKeys] "x" (roundKeys), [L] "y" (L)); 
}

#else
#ifdef MSP
/*----------------------------------------------------------------------------*/
/* Key Schedule -- MSP			                                      */
/*----------------------------------------------------------------------------*/
void RunEncryptionKeySchedule(uint8_t *key, uint8_t *roundKeys)
{
    asm volatile (\
        /*---------------------------------------------------------------*/
	/* r4  - lower word of ki					 */
        /* r5  - higher word of ki		                         */
	/* r6  - lower word of li					 */
        /* r7  - higher word of li		                         */
	/* r8  - lower word of l(i+1)					 */
        /* r9  - higher word of l(i+1)		                         */
	/* r10 - lower word of l(i+2)					 */
        /* r11 - higher word of l(i+2)		                         */
	/* r12 - 				                         */
        /* r13 - Loop counter                                            */
        /* r14 - RoundKeys i                                             */
        /* r15 - Key                                                     */
        /*---------------------------------------------------------------*/
        /* Store all modified registers                                  */
        /*---------------------------------------------------------------*/
	"push   r4;                 \n"
        "push   r5;                 \n"
        "push   r6;                 \n"
        "push   r7;                 \n"
        "push   r8;                 \n"
        "push   r9;                 \n"
        "push   r10;                \n"
	"push   r11;                \n"
        "push   r13;                \n"
        "push   r14;                \n"
        "push   r15;                \n"
        /*--------------------------------------------------------------*/
        "mov    %[key],         r15;	\n"
        "mov    %[roundKeys],   r14;	\n" 
        /*--------------------------------------------------------------*/
        /* load master key						*/
        /*--------------------------------------------------------------*/
        "mov    @r15+,       	0(r14);	\n" 
        "mov    @r15+,       	2(r14);	\n"
        "mov    @r15+,       	r6;	\n"
        "mov    @r15+,       	r7;	\n"
        "mov    @r15+,       	r8;	\n"
        "mov    @r15+,      	r9;	\n"
        "mov    @r15+,      	r10;	\n"
        "mov    @r15+,      	r11;	\n"
        /*---------------------------------------------------------------*/
        "mov    #0,            	r13;	\n"
"round_loop:				\n"
	/* S(-8)(li), ki is not used now */
	"mov.b	r6, 		r4;	\n"
 	"xor.b	r7, 		r4;	\n"
	"swpb	r6;			\n"
	"swpb	r7;			\n"
	"swpb	r4;			\n"
	"xor	r4,		r6;	\n"
	"xor	r4,		r7;	\n"
        /* ki = r5:r4;			*/ 
        "mov	@r14+,       	r4;	\n"  
        "mov   	@r14+,        	r5;	\n" 
        /* li = ki + S(-8)(li);	*/
        "add	r4,		r6;	\n"
	"adc	r5,		r7;	\n"
	/* li = [ki + S(-8)(li)] eor i 	*/
        "xor   	r13,       	r6;	\n"
	/* ki = S(3)ki 			*/
	"rla	r4;			\n" /* S(-1) */
	"rlc	r5;			\n"
	"adc	r4;			\n"
	"rla	r4;			\n" /* S(-1) */
	"rlc	r5;			\n"
	"adc	r4;			\n"
	"rla	r4;			\n" /* S(-1) */
	"rlc	r5;			\n"
	"adc	r4;			\n"
	/* ki = S(3)ki eor [[ki + S(-8)(li)] eor i] */
	"xor	r6,		r4;	\n"
	"xor	r7,		r5;	\n"
	/* store ki+1. r14 has pointed to the address of ki+1 by instruction "@r14+" */
	"mov	r4,		0(r14);	\n"
	"mov	r5,		2(r14);	\n"
	/* update l 			*/
	"mov	r6,		r4;	\n" /* store li+3 */
	"mov	r7,		r5;	\n"
	"mov	r8,		r6;	\n" /* update li, covered by li+1 */
	"mov	r9,		r7;	\n"
	"mov	r10,		r8;	\n" /* update li+1, covered by li+2 */
	"mov	r11,		r9;	\n"
	"mov	r4,		r10;	\n" /* update li+2, covered by li+3 */
	"mov	r5,		r11;	\n"
	/*---------------------------------------------------------------*/
	/* loop control 		*/
        "inc	r13;			\n"
	"cmp	#26,		r13;	\n" /* r13-26, sets status only; the destination is not written */
	"jne	round_loop;		\n"
        /*---------------------------------------------------------------*/
        /* Restore registers                                             */
        /*---------------------------------------------------------------*/
        "pop    r15;                \n"
        "pop    r14;                \n"
        "pop    r13;                \n"
        "pop    r11;                \n"
	"pop    r10;                \n"
        "pop    r9;                 \n"
        "pop    r8;                 \n"
        "pop    r7;                 \n"
        "pop    r6;                 \n"
        "pop    r5;                 \n"
	"pop    r4;                 \n"
        /*---------------------------------------------------------------*/
    :
    : [key] "" (key), [roundKeys] "" (roundKeys)); 
}

#else
#ifdef ARM
/*----------------------------------------------------------------------------*/
/* Key Schedule -- ARM			                                      */
/*----------------------------------------------------------------------------*/

/* key: [k0, l0, l1, l2]. The input should be opposite to the paper 	      */
void RunEncryptionKeySchedule(uint8_t *key, uint8_t *roundKeys)
{
    asm volatile (\
        /*--------------------------------------------------------------------*/
        /* r0  - k[i]                                                         */
        /* r1  - l[i]                                            	      */
        /* r2  - l[i+1]                                                       */
        /* r3  - l[i+2]                                                       */
        /* r4  - temp                                                         */
        /* r5  - temp                                                         */
        /* r6  - point of master key                                          */
        /* r7  - point of round keys                                          */
        /* r8  - loop counter                                                 */
        /*--------------------------------------------------------------------*/
        /* Store all modified registers                                       */
        /*--------------------------------------------------------------------*/
	/* If the exclamation point following register was omitted, then the address register is not altered by the instruction */
        "stmdb        		sp!,   			{r0-r12,lr};            \n"
	/* initialize 								*/
        "mov          		r6,        		%[key];             	\n"
        "mov          		r7,  			%[roundKeys];           \n"
        "mov           		r8,           		#0;              	\n"
        /* master keys 								*/
        "ldmia        		r6!,      		{r0};              	\n"
	/* r7 is still the start address of round keys after the operation 	*/ 
	"stmia         		r7,      		{r0};             	\n" 
	"ldmia        		r6,      		{r1-r3};              	\n"
"key_schedule_loop:                                   				\n"
	/* load k[i] 								*/
	"ldmia			r7!,			{r0};			\n"
	/* l(i) = k(i) + S(-8)l(i)						 */
	"ror			r1, 			#8;			\n"
	"add 			r1,			r1,		r0;	\n"
	/* l(i) = l(i) eor currentRound 					*/
	"eor			r1,			r1,		r8;	\n"
	/* k(i) = S(3)k(i) 							*/
	"mov			r4,			r0,		lsl#3;	\n"
	"mov			r5,			r0,		lsr#29;	\n"
	"eor			r0, 			r4,		r5;	\n"
	/* k(i) = k(i) eor l(i), the result is k(i+1) 				*/
	"eor			r0,			r0,		r1;	\n"
	/* store k(i+1) 							*/
	"stmia         		r7,      		{r0};             	\n"
	/* update l */
	"mov			r4,			r1;			\n" /* store the new L  */
	"mov			r1,			r2;			\n"
	"mov			r2,			r3;			\n"
	"mov 			r3,			r4;			\n"
        /* loop control */
        "add          		r8,            		r8,          	#1;	\n"
	"cmp			r8,			#26;			\n" /* key schedule rounds is just 26(not 27) */
        "bne           		key_schedule_loop;              		\n" 
        /*--------------------------------------------------------------------*/
        /* Restore registers                                                  */
        /*--------------------------------------------------------------------*/
        "ldmia        	sp!,      		{r0-r12,lr};           		\n"
        /*--------------------------------------------------------------------*/
    :
    : [key] "" (key), [roundKeys] "" (roundKeys) 
	); 
}

#else
/*----------------------------------------------------------------------------*/
/* Key Schedule -- Default C Implementation				      */
/*----------------------------------------------------------------------------*/
void RunEncryptionKeySchedule(uint8_t *key, uint8_t *roundKeys)
{
	uint8_t i;
	uint32_t *rk = (uint32_t *)roundKeys;
	uint32_t *mk = (uint32_t *)key;
	uint32_t lp3;
	uint32_t lp2;
	uint32_t lp1;
	uint32_t lp0;

	rk[0] = mk[0];
	for (i = 0; i < NUMBER_OF_ROUNDS - 1; ++i)
	{
        	if (i == 0)
        	{
          		lp0 = mk[1];
          		lp1 = mk[2];
			lp2 = mk[3];
       	 	}
       	 	else
        	{
            		lp0 = lp1;
            		lp1 = lp2;
			lp2 = lp3;
        	}
        	lp3 = (rorAlpha(lp0) + rk[i]) ^ i;
        	rk[i + 1] = rolBeta(rk[i]) ^ lp3;
    	}
}

#endif
#endif
#endif

