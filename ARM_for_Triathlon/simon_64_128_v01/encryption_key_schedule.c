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
 * encryption key schedule
 */
#ifdef AVR
/*----------------------------------------------------------------------------*/
/* Key Schedule -- AVR			                                      */
/*----------------------------------------------------------------------------*/
void Decrypt(uint8_t *block, uint8_t *roundKeys)
{
	/* Add here the cipher decryption implementation */
}

#else
#ifdef MSP
/*----------------------------------------------------------------------------*/
/* Key Schedule -- MSP			                                      */
/*----------------------------------------------------------------------------*/
void Decrypt(uint8_t *block, uint8_t *roundKeys)
{
	
}

#else
#ifdef ARM
/*----------------------------------------------------------------------------*/
/* Key Schedule -- ARM			                                      */
/*----------------------------------------------------------------------------*/
void RunEncryptionKeySchedule(uint8_t *key, uint8_t *roundKeys)
{
	asm volatile (\
        /*--------------------------------------------------------------------*/
        /* r0  - k[i]                                                         */
        /* r1  - k[i+1]                                            	      */
        /* r2  - k[i+3]                                                	      */
        /* r3  - const C                                                      */
        /* r4  - const Z first half(high 32 bits)                             */
        /* r5  - const Z second half(low 32 bits)                             */
        /* r6  - point of master key                                          */
        /* r7  - point of round keys                                          */
        /* r8  - loop counter                                                 */
        /* r9  - temp 0                                                       */
        /* r10 - Temporary 1                                                  */
        /* r11 - Temporary 2                                                  */
        /* r12 - Temporary 3                                                  */
        /*--------------------------------------------------------------------*/
        /* Store all modified registers                                       */
        /*--------------------------------------------------------------------*/
	/* If the exclamation point following register was omitted, then the address register is not altered by the instruction */
        "stmdb        		sp!,   			{r0-r12,lr};            \n"
	/* initialize 								*/
        "mov          		r6,        		%[key];             	\n"
        "mov          		r7,  			%[roundKeys];           \n"
        "mov           		r8,           		#40;              	\n"
	"mov			r3,			#0xfffffffc;		\n"
	"mov			r4,			#0xdbac65e0;		\n"
	"mov			r5,			#0x48a7343c;		\n"
        /* memcpy(roundKeys, key, 16) 						*/
        "ldmia        		r6,      		{r9-r12};              	\n"
	/* r7 is still the start address of round keys after the operation 	*/ 
        "stmia         		r7,      		{r9-r12};             	\n"
"key_schedule_loop:                                    				\n" 
        "ldmia			r7!,			{r0-r1};		\n" /* r7 is the address of k[i+2] */
	"add			r7,			r7,		#4;	\n" /* r7 is the address of k[i+3] */
	"ldmia			r7!,			{r2};			\n" /* r7 is the address of k[i+4] */
	/* S(-3)(k[i+3])  							*/
	"ror			r2,			#3;			\n"
	/* S(-3)(k[i+3]) eor k[i+1] 						*/
	"eor 			r2, 			r2, 		r1;	\n"
	/* k(i) eor [k(i+1) eor S(-3)k(i+3)] 					*/
	"eor			r0,			r0,		r2;	\n"
	/* S(-1)[k(i+1) eor S(-3)k(i+3)] 					*/
	"ror			r2,			#1;			\n"
	/* k(i) eor [k(i+1) eor S(-3)k(i+3)] eor S(-1)[k(i+1) eor S(-3)k(i+3)] 	*/
	"eor			r0,			r0,		r2;	\n"
	/* C eor k(i) eor [k(i+1) eor S(-3)k(i+3)] eor S(-1)[k(i+1) eor S(-3)k(i+3)] */
	"eor 			r0,			r0, 		r3;	\n"
	/* eor first bit of Z 							*/
	/* key schedule is 40 rounds, the first 32 rounds use value of r4, other 8 rounds use first 8 bits value of r5 */
	"cmp			r8,			#8;			\n"
	"beq			low_half_of_Z;					\n" /* if r8 is 8, low half of Z will be used */
	/* logical shift left, and the flag register will be changed. 		*/
	/* The value of flag register is modified according to r4, not the first 1 bit */
	"ands			r9,			r4,		0x80000000;	\n"
	"lsl			r4,			#1;			\n"
	"b			loop_control;					\n"
"low_half_of_Z:									\n"
	"ands			r9,			r5,		0x80000000;	\n"
	"lsl			r5,			#1;			\n"
"loop_control:"
	/* eor 1 if the Z flag( the result of lsls) is 0(means the bit is 1) 	*/
	"eorne			r0,			r0,		#0x1;	\n"
	/* store k[i+4] 							*/
	"stmia			r7!,			{r0};			\n" /* r7 is the address of k[i+5] */
	/* let r7 point k[i+1] 							*/
	"sub			r7,			r7,		#16;	\n"
        /* while (loop_counter > 0)                                           	*/
        "subs          		r8,            		r8,           	#1;	\n"
        "bne           		key_schedule_loop;              		\n" 
        /*----------------------------------------------------------------------*/
        /* Restore registers                                                  	*/
        /*----------------------------------------------------------------------*/
        "ldmia        		sp!,      		{r0-r12,lr};           	\n"
        /*----------------------------------------------------------------------*/
    :
    : [key] "r" (key), [roundKeys] "r" (roundKeys) 
	); 
}

#else
/*----------------------------------------------------------------------------*/
/* Key Schedule -- Default c implementation                                   */
/*----------------------------------------------------------------------------*/
void RunEncryptionKeySchedule(uint8_t *key, uint8_t *roundKeys)
{
		
}
#endif
#endif
#endif
