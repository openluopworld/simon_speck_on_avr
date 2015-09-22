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
void RunEncryptionKeySchedule(uint8_t *key, uint8_t *roundKeys)
{
	
}

#else
#ifdef MSP
/*----------------------------------------------------------------------------*/
/* Key Schedule -- MSP			                                      */
/*----------------------------------------------------------------------------*/
void RunEncryptionKeySchedule(uint8_t *key, uint8_t *roundKeys)
{
	
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
        "bne           	key_schedule_loop;              			\n" 
        /*--------------------------------------------------------------------*/
        /* Restore registers                                                  */
        /*--------------------------------------------------------------------*/
        "ldmia        	sp!,      		{r0-r12,lr};           		\n"
        /*--------------------------------------------------------------------*/
    :
    : [key] "r" (key), [roundKeys] "r" (roundKeys) 
	); 
}

#else
/*----------------------------------------------------------------------------*/
/* Key Schedule -- Default C Implementation				      */
/*----------------------------------------------------------------------------*/
void RunEncryptionKeySchedule(uint8_t *key, uint8_t *roundKeys)
{
	
}

#endif
#endif
#endif

