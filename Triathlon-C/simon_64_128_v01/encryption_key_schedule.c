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
void RunEncryptionKeySchedule(uint8_t *key, uint8_t *roundKeys)
{
    asm volatile(\	
	/*------------------------------------------------------*/
        /* Registers allocation:				*/
        /* r0	   : 						*/
        /* r1	   : 						*/
        /* r2-r5   : ki						*/
        /* r6-r9   : ki+3					*/
        /* r10-13  : ki+1					*/
        /* r14     :						*/
        /* r15     :						*/
        /* r16     :                            		*/
        /* r17     : temp             				*/
        /* r18     : remain8, currentRound % 8	 		*/
        /* r19     : currentZ, the current byte value of z	*/
        /* r20     : constC0, the lowest byte const value c	*/
        /* r21     : 						*/
        /* r22     : 						*/
        /* r23     : 						*/
        /* r24     : currentRound				*/
        /* r25     : 						*/
        /* r26:r27 : X roundKeys				*/
        /* r28:r29 : Y key					*/
        /* r30:r31 : Z CONSTZ					*/
        /* ---------------------------------------------------- */
        /* Store all modified registers				*/
        /* ---------------------------------------------------- */
	"push	r2;	\n"
        "push  	r3;	\n"
        "push  	r4;	\n"
	"push  	r5;	\n"
	"push  	r6;	\n"
        "push  	r7;	\n"
        "push  	r8;	\n"
	"push 	r9;	\n"
	"push  	r10;	\n"
        "push  	r11;	\n"
        "push  	r12;	\n"
	"push  	r13;	\n"
        "push 	r17; 	\n"
        "push 	r18;	\n"
        "push 	r19;	\n"
        "push 	r20;	\n"
        "push 	r24;	\n"
        "push 	r26;	\n" 
        "push 	r27;	\n" 
        "push 	r28;	\n"
        "push 	r29;	\n"
        "push 	r30;	\n"
        "push 	r31;	\n"
	/* ---------------------------------------------------- */
	/* load master key 					*/
	"ldi		r26,		low(roundKeys);		\n"
	"ldi		r27, 		high(roundKeys);	\n"
	"ldi 		r28,		low(key);		\n"
	"ldi		r29,		high(key);		\n"
	"clr		r24;					\n"
"loadMasterKey:							\n"
	"ld		r17,		y+;			\n"
	"st 		x+, 		r17;			\n"
	"inc 		r24;					\n"
	"cpi 		r24, 		KEY_SIZE;		\n"
	"brne loadMasterKey;\n"
	/* ---------------------------------------------------- */
	/* prepare for the key schedule 			*/
	"ldi 		r26, 		low(roundKeys);		\n"
	"ldi 		r27, 		high(roundKeys);	\n"
	"ldi 		r30, 		lo8(CONSTZ);		\n"
	"ldi 		r31, 		hi8(CONSTZ);		\n"
	"lpm 		r19, 		z+;			\n"
	"clr 		r18;					\n"
	"ldi		r20,		0xfc;			\n"
	"clr 		r24;					\n"
	/* ---------------------------------------------------- */
"keysExtend:							\n"
	/* load k(i) 						*/
	"ld 		r2, 		x+;			\n"
	"ld 		r3, 		x+;			\n"
	"ld 		r4, 		x+;			\n"
	"ld 		r5, 		x+;			\n"
	/* load k(i+1) 						*/
	"ld 		r10, 		x+;			\n"
	"ld 		r11, 		x+;			\n"
	"ld 		r12, 		x+;			\n"
	"ld 		r13, 		x+;			\n"
	/* S(-3)k(i+3) 						*/
	"adiw 		r26, 		4;			\n"
	"ld 		r6, 		x+;			\n"
	"ld 		r7, 		x+;			\n"
	"ld 		r8, 		x+;			\n"
	"ld 		r9, 		x+;			\n"
	"lsr 		r9;					\n" /* S(-1) */
	"ror 		r8;					\n"
	"ror 		r7;					\n"
	"bst 		r6, 		0;			\n"
	"ror 		r6;					\n"
	"bld 		r9, 		7;			\n"
	"lsr 		r9;					\n" /* S(-2) */
	"ror 		r8;					\n"
	"ror 		r7;					\n"
	"bst 		r6, 		0;			\n"
	"ror 		r6;					\n"
	"bld 		r9, 		7;			\n"
	"lsr 		r9;					\n" /* S(-3) */
	"ror 		r8;					\n"
	"ror 		r7;					\n"
	"bst 		r6, 		0;			\n"
	"ror 		r6;					\n"
	"bld 		r9, 		7;			\n"
	/* k(i+1) eor S(-3)k(i+3) 				*/
	"eor 		r6, 		r10;			\n"
	"eor 		r7, 		r11;			\n"
	"eor 		r8, 		r12;			\n"
	"eor 		r9, 		r13;			\n"
	/* k(i) eor [k(i+1) eor S(-3)k(i+3)] 			*/
	"eor 		r2, 		r6;			\n"
	"eor 		r3, 		r7;			\n"
	"eor 		r4, 		r8;			\n"
	"eor 		r5, 		r9;			\n"
	/* S(-1)[k(i+1) eor S(-3)k(i+3)] 			*/
	"lsr 		r9;					\n"
	"ror 		r8;					\n"
	"ror 		r7;					\n"
	"bst 		r6, 		0;			\n"
	"ror 		r6;					\n"
	"bld 		r9, 		7;			\n"
	/* k(i) eor [k(i+1) eor S(-3)k(i+3)] eor S(-1)[k(i+1) eor S(-3)k(i+3)] */
	"eor 		r2, 		r6;			\n"
	"eor 		r3, 		r7;			\n"
	"eor 		r4, 		r8;			\n"
	"eor 		r5, 		r9;			\n"
	/* k(i) eor [k(i+1) eor S(-3)k(i+3)] eor S(-1)[k(i+1) eor S(-3)k(i+3)] eor c eor z(i) */
	/* the least significant bit of the least significant byte of C is 0, so just set the bit to implement "c eor z(i)"*/
	"bst 		r19, 		7;			\n"
	"bld 		r20, 		0;			\n"
	"lsl 		r19;					\n" /* update z for next round use */
	"eor 		r2, 		r20;			\n"
	"com 		r3;					\n"
	"com 		r4;					\n"
	"com 		r5;					\n"
	/* k(i+4), is just [r5,r4,r3,r2] 			*/
	"st 		x+, 		r2;			\n"
	"st 		x+, 		r3;			\n"
	"st 		x+, 		r4;			\n"
	"st 		x+, 		r5;			\n"
	/* set x to the position of k(i+1), should sub 16(from k(i+5) to k(i+1)) not 12 */
	"sbiw 		r26, 		16;			\n"
	/* ---------------------------------------------------- */
	/* loop control						*/
	"inc 		r24;					\n"
	"inc 		r18;					\n"
	"cpi 		r18, 		8;			\n"
	"breq 		nextByteZ;				\n" /* if remain8 = 8, the next byte of constZ should be load */
	"rjmp 		keysExtend;				\n"
"nextByteZ:							\n"
	"clr 		r18;					\n" /* start with 0 again */
	"lpm 		r19, 		z+;			\n"
	"cpi 		r24, 		40;			\n"
	"breq 		scheEnd;				\n"
	"rjmp 		keysExtend;				\n"
"scheEnd:							\n"
	"ret;							\n"
	/* ---------------------------------------------------- */
	/* Restore all modified registers			*/
        "pop  r31;       \n"
        "pop  r30;       \n"
        "pop  r29;       \n"
        "pop  r28;       \n"
        "pop  r27;       \n" 
        "pop  r26;       \n" 
        "pop  r24;       \n" 
	"pop  r20;       \n"
	"pop  r19;       \n"
	"pop  r18;       \n"
        "pop  r17;       \n"
        "pop  r13;       \n"
	"pop  r12;       \n"
	"pop  r11;       \n"
	"pop  r10;       \n"
	"pop  r9;        \n"
	"pop  r8;        \n"
	"pop  r7;        \n"
	"pop  r6;        \n"
	"pop  r5;        \n"
	"pop  r4;        \n"
        "pop  r3;        \n"
        "pop  r2;        \n"
	/* ---------------------------------------------------- */
    :
    : [key] "x" (key), [roundKeys] "z" (roundKeys), [CONSTZ] "" (CONSTZ));
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
	/* r6  - lower word of k(i+1)					 */
        /* r7  - higher word of k(i+1)		                         */
	/* r8  - lower word of k(i+3)					 */
        /* r9  - higher word of k(i+3)		                         */
	/* r10 - 				                         */
	/* r11 - 				                         */
        /* r12 - point of Const Z eor 3                                  */
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
        "push   r12;                \n"
        "push   r13;                \n"
        "push   r14;                \n"
        "push   r15;                \n"
        /*--------------------------------------------------------------*/
        "mov    %[key],         r15;\n"
        "mov    %[roundKeys],   r14;\n" 
	"mov	%[Z_XOR_3],	r12;\n"
        /*--------------------------------------------------------------*/
        /* load master key						*/
        /*--------------------------------------------------------------*/
        "mov    @r15+,       	0(r14);\n" /* r15 will add two, r14 unchanged. */ 
        "mov    @r15+,       	2(r14);\n"
        "mov    @r15+,       	4(r14);\n"
        "mov    @r15+,       	6(r14);\n"
        "mov    @r15+,       	8(r14);\n"
        "mov    @r15+,      	10(r14);\n"
        "mov    @r15+,      	12(r14);\n"
        "mov    @r15+,      	14(r14);\n"
        /*---------------------------------------------------------------*/
        "mov    #40,            r13;\n" /* 40 rounds                     */
"round_loop:\n"
        /* ki = r5:r4;	*/ 
        "mov	0(r14),       	r4;\n"  
        "mov   	2(r14),        	r5;\n" 
        /* k(i+1) = r7:r6;	*/
        "mov   	4(r14),        	r6;\n"  
        "mov   	6(r14),        	r7;\n"
	/* k(i+3) = r9:r8							*/
        "mov   	12(r14),       	r8;\n"  
        "mov   	14(r14),       	r9;\n"
	/* S(-3)(k(i+3)) */
	"bit	#1,		r8;\n" /* S(-1) */
	"rrc	r9;\n"
	"rrc	r8;\n"
	"bit	#1,		r8;\n" /* S(-2) */
	"rrc	r9;\n"
	"rrc	r8;\n"
	"bit	#1,		r8;\n" /* S(-3) */
	"rrc	r9;\n"
	"rrc	r8;\n"
	/* S(-3)(k(i+3)) eor k(i+1) */
	"eor	r6, 		r8;\n" /* result is in r9:r8 */
	"eor	r7,		r9;\n"
	/* move r8:r9 to r6:r7 */
	"mov	r8,		r6;\n"
	"mov	r9,		r7;\n"
	/* S(-1)[S(-3)(k(i+3)) eor k(i+1)] */
	"bit	#1,		r8;\n"
	"rrc	r9;\n"
	"rrc	r8;\n"
	/* [I eor S(-1)][S(-3)(k(i+3)) eor k(i+1)] */
	"eor	r6,		r8;\n"
	"eor	r7,		r9;\n"
	/* ki eor [I eor S(-1)][S(-3)(k(i+3)) eor k(i+1)] */
	"eor	r4,		r8;\n"
	"eor	r5,		r9;\n"
	/* load Z eor C */
	"mov.b	@r12+,       	r4;\n" /* 0                          */
	"eor.b	r4,		r8;\n"
	"mov	r8,		16(r14);\n"
	"mov	r9,		18(r14);\n"

	/* points to ki+1 */
	"add	#4,		r14;\n"

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
    : [key] "m" (key), [roundKeys] "m" (roundKeys), [Z_XOR_3] "" (Z_XOR_3)); 
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
#include <stdint.h>

#include "cipher.h"
#include "constants.h"
#include "primitives.h"

void RunEncryptionKeySchedule(uint8_t *key, uint8_t *roundKeys)
{
	uint8_t i;
	uint8_t z_xor_3;
	uint32_t temp;
	uint32_t *mk = (uint32_t *)key;
	uint32_t *rk = (uint32_t *)roundKeys;

	rk[0] = mk[0];
    	rk[1] = mk[1];
    	rk[2] = mk[2];
	rk[3] = mk[3];
    	for (i = 4; i < NUMBER_OF_ROUNDS; ++i)
    	{
        	temp = ror1(rk[i - 1]);
		temp = ror1(temp);
		temp = ror1(temp);
        	temp = temp ^ ror1(temp);
		temp = temp ^ rk[i-3] ^ ror1(rk[i-3]);
		z_xor_3 = READ_Z_BYTE(Z_XOR_3[(i - 4)]);
		rk[i] = ~(rk[i - 4]) ^ temp ^ (uint32_t)z_xor_3;
    	}
}
#endif
#endif
#endif

