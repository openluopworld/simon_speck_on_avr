
#include <stdint.h>
#include "constants.h"
#include "cipher.h"

/*
 * Simon
 */
 
void RunEncryptionKeySchedule(uint8_t *key, uint8_t *roundKeys)
{
    asm volatile (\
        /*--------------------------------------------------------------------*/
        /* r0  - k[i]                                                         */
        /* r1  - k[i+1]                                            			  */
        /* r2  - k[i+3]                                                 	  */
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
        "stmdb        	sp!,   			{r0-r12,lr};            \n"
		/* initialize */
        "mov          	r6,        		%[key];             	\n"
        "mov          	r7,  			%[roundKeys];           \n"
        "mov           	r8,           	#40;              		\n"
		"mov			r3,				#0xfffffffc;			\n"
		"mov			r4,				#0xdbac65e0;			\n"
		"mov			r5,				#0x48a7343c;			\n"
        /* memcpy(roundKeys, key, 16) */
        "ldmia        	r6,      		{r9-r12};              	\n"  
        "stmia         	r7,      		{r9-r12};             	\n" /* r7 is still the start address of round keys after the operation */ 
	"key_schedule_loop:                                    		\n" 
        "ldmia			r7!,			{r0-r1};				\n" /* r7 is the address of k[i+2] */
		"add			r7,				r7,				#4;		\n" /* r7 is the address of k[i+3] */
		"ldmia			r7!,			{r2};					\n" /* r7 is the address of k[i+4] */
		/* S(-3)(k[i+3]) */
		"ror			r2,				#3;						\n"
		/* S(-3)(k[i+3]) eor k[i+1] */
		"eor 			r2, 			r2, 			r1;		\n"
		/* k(i) eor [k(i+1) eor S(-3)k(i+3)] */
		"eor			r0,				r0,				r2;		\n"
		/* S(-1)[k(i+1) eor S(-3)k(i+3)] */
		"ror			r2,				#1;						\n"
		/* k(i) eor [k(i+1) eor S(-3)k(i+3)] eor S(-1)[k(i+1) eor S(-3)k(i+3)] */
		"eor			r0,				r0,				r2;		\n"
		/* C eor k(i) eor [k(i+1) eor S(-3)k(i+3)] eor S(-1)[k(i+1) eor S(-3)k(i+3)] */
		"eor 			r0,				r0, 			r3;		\n"
		/* eor first bit of Z */
		/* key schedule is 40 rounds, the first 32 rounds use value of r4, other 8 rounds use first 8 bits value of r5 */
		"cmp			r8,				#8;						\n"
		"beq			low_half_of_Z;\n" /* if r8 is 8, low half of Z will be used */
		"lsls			r4,				#1;						\n"
		"b				loop_control;\n"
	"low_half_of_Z:"
		"lsls			r5,				#1;						\n"
	"loop_control:"
		/* eor 1 if the Z flag( the result of lsls) is 0(means the bit is 1) */
		"eorne			r0,				r0,				#0x1;	\n"
		/* store k[i+4] */
		"stmia			r7!,			{r0};					\n" /* r7 is the address of k[i+5] */
		/* let r7 point k[i+1] */
		"sub			r7,				r7,				#16;	\n"
        /* while (loop_counter > 0)                                           */
        /*--------------------------------------------------------------------*/
        "subs          r8,            r8,           #1;			\n"
        "bne           key_schedule_loop;              			\n" 
        /*--------------------------------------------------------------------*/
        /* Restore registers                                                  */
        /*--------------------------------------------------------------------*/
        "ldmia        sp!,      {r0-r12,lr};           			\n"
        /*--------------------------------------------------------------------*/
    :
    : [key] "r" (key), [roundKeys] "r" (roundKeys) 
	); 
}

/*
 * Only encrypt one block now
 */
void Encrypt(uint8_t *block, uint8_t *roundKeys)
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
		"stmdb			sp!,			{r0-r12,lr};						\n" 
        "mov			r6, 			%[block]; 							\n" 
        "mov			r7, 			%[roundKeys]; 						\n"
		/* value of r6 will not change, if it's 'ldmia r6!,{r0-r1}', then r6 will add 8 after the instruction */
		"ldmia			r6, 			{r0-r1}; 							\n"
        "stmdb			sp!, 			{r6};								\n"
		"mov			r8,				#44;								\n"
	"encrypt_round_loop:                           	   						\n"
		"ldmia			r7!, 			{r9}; 								\n"
		"eor			r9,				r1, 				r9;				\n"
		/* move x to y                                                      */
		"mov			r1,				r0;									\n"
		/* x rotate left with 1 bit                                         */
		"ands			r2,				r0,					#0x80000000;	\n"
		"lsl			r0,				#1;									\n"
		"orrne			r0,				r0, 				#0x1;			\n"
		/* move x to t                                                      */
		"mov			r2,				r0;									\n"
		"mov			r3,				r1,lsl#8;							\n"
		"mov			r4,				r1,lsr#24;							\n"
		"eor			r3, 			r3,					r4;				\n"
		"and			r2,				r2,					r3;				\n"
		/* x rotate left with 1 bit again */
		"ands			r2,				r0,					#0x80000000;	\n"
		"lsl			r0,				#1;									\n"
		"orrne			r0,				r0, 				#0x1;			\n"
		/* x eor t */
		"eor			r0, 			r0, 			r2;					\n"
		/* x eor k */
		"eor			r0, 			r0, 			r9;					\n"
		/* loop control */
		"subs			r8, 			r8, 			#1"
		"bne			encrypt_round_loop"
		/*--------------------------------------------------------------------*/
        /* Store state                                                        */
        /*--------------------------------------------------------------------*/
        "ldmia			sp!,			{r6}; 								\n"
        "stmia			r6,				{r0-r1};           					\n"
        /*--------------------------------------------------------------------*/
        /* Restore registers                                                  */
        /*--------------------------------------------------------------------*/
        "ldmia        sp!,      {r0-r12,lr};           \n" /*                 */
        /*--------------------------------------------------------------------*/
	:
    : [block] "r" (block), [roundKeys] "r" (roundKeys) 
	);
} 

/*
 * Only decrypt one block now
 */
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
		"stmdb			sp!,			{r0-r12,lr};						\n" 
        "mov			r6, 			%[block]; 							\n" 
        "mov			r7, 			%[roundKeys]; 						\n"
		"ldmia			r6, 			{r0-r1}; 							\n"
        "stmdb			sp!, 			{r6};								\n"
		"mov			r8,				#44;								\n"
		/* end of keys */
        "add           	r7,            	r7,        			#176;			\n"
	"decrypt_round_loop:"
        /* AddRoundKey */
        "ldmdb			r7!,			{r9};             					\n"
        "eor			r0,				r0, 				r9;				\n"  
		/* move y to x */
		"mov 			r0, 			r1;"
		/* S(1)(y) */
		"ands			r2,				r1,					#0x80000000;	\n"
		"lsl			r1,				#1;									\n"
		"orrne			r1,				r1, 				#0x1;			\n"
		/* move y to t */
		"mov 			r5, 			r1;									\n"
		/* t and S(8)(x) */
		"mov			r3,				r0,lsl#8;							\n"
		"mov			r4,				r0,lsr#24;							\n"
		"eor			r3, 			r3,					r4;				\n"
		"and			r5,				r5,					r3;				\n"
		/* y = S(2)(y) */
		"ands			r2,				r1,					#0x80000000;	\n"
		"lsl			r1,				#1;									\n"
		"orrne			r1,				r1, 				#0x1;			\n"
		/* y eor t */
		"eor			r1,				r1,					r5"
		/* y eor k */
		"eor			r1, 			r1,					r9"
		/* loop control */
		"subs          	r8,           	 r8,       		    #1;				\n"
        "bne     	    decrypt_round_loop;									\n" 
        /*--------------------------------------------------------------------*/
        /* Store state                                                        */
        /*--------------------------------------------------------------------*/
        "ldmia        sp!,             {r6};           \n"
        "stmia         r6,          {r0-r1};           \n"
        /*--------------------------------------------------------------------*/
        /* Restore registers                                                  */
        /*--------------------------------------------------------------------*/
        "ldmia        sp!,      {r0-r12,lr};           \n" /*                 */
        /*--------------------------------------------------------------------*/
    :
    : [block] "r" (block), [roundKeys] "r" (roundKeys) 
	); 
}