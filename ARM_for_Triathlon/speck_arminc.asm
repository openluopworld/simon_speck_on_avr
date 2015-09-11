
#include <stdint.h>
#include "constants.h"
#include "cipher.h"

/*
 * Speck
 */

/*
 * key: [k0, l0, l1, l2]. The input should be opposite to the paper
 */
void RunEncryptionKeySchedule(uint8_t *key, uint8_t *roundKeys)
{
    asm volatile (\
        /*--------------------------------------------------------------------*/
        /* r0  - k[i]                                                         */
        /* r1  - l[i]                                            			  */
        /* r2  - l[i+1]                                                 	  */
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
        "stmdb        	sp!,   			{r0-r12,lr};            \n"
		/* initialize */
        "mov          	r6,        		%[key];             	\n"
        "mov          	r7,  			%[roundKeys];           \n"
        "mov           	r8,           	#0;              		\n"
        /* master keys */
        "ldmia        	r6!,      		{r0};              		\n"
		"stmia         	r7,      		{r0};             		\n" /* r7 is still the start address of round keys after the operation */ 
		"ldmia        	r6,      		{r1-r3};              	\n"
	"key_schedule_loop:                                   		\n"
		/* load k[i] */
		"ldmia			r7!,			{r0};					\n"
		/* l(i) = k(i) + S(-8)l(i) */
		"ror			r1, 			#8;						\n"
		"add 			r1,				r1,				r0;		\n"
		/* l(i) = l(i) eor currentRound */
		"eor			r1,				r1,				r8;		\n"
		/* k(i) = S(3)k(i) */
		"mov			r4,				r0,lsl#3;				\n"
		"mov			r5,				r0,lsr#29;				\n"
		"eor			r0, 			r4,				r5;		\n"
		/* k(i) = k(i) eor l(i), the result is k(i+1) */
		"eor			r0,				r0,				r1;		\n"
		/* store k(i+1) */
		"stmia         	r7,      		{r0};             		\n"
		/* update l */
		"mov			r4,				r1;						\n" /* store the new L  */
		"mov			r1,				r2;						\n"
		"mov			r2,				r3;						\n"
		"mov 			r3,				r4;						\n"
        /* loop control */
        "add          	r8,            	r8,          	#1;		\n"
		"cmp			r8,				#26;					\n" /* key schedule rounds is just 26(not 27) */
        "bne           	key_schedule_loop;              		\n" 
        /*--------------------------------------------------------------------*/
        /* Restore registers                                                  */
        /*--------------------------------------------------------------------*/
        "ldmia        	sp!,      		{r0-r12,lr};           	\n"
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
		"mov			r8,				#27;								\n"
	"encrypt_round_loop:                           	   						\n"
		/* load k */
		"ldmia			r7!, 			{r9}; 								\n" /* r7 will add 4 after the instruction */
		/* x = S(-8)(x) + y */
		"ror			r0,				#8;									\n"
		"add			r0,				r0,					r1; 			\n"
		/* k = k eor (S(-8)(x) + y) */
		"eor			r9,				r9, 				r0;				\n"
		/* y = S(3)(y) */
		"mov			r2,				r1,lsl#3;							\n"
		"mov			r3,				r1,lsr#29;							\n"
		"eor			r1, 			r2,					r3;				\n"
		/* y = S(3)(y) eor ( S(-8)(x) + y ) eor k */
		"eor			r1, 			r1,					r9;				\n"
		/* x = ( S(-8)(x) + y ) eor k */
		"mov 			r0,				r9;									\n" /* move k to x */
		/* loop control */
		"subs			r8, 			r8, 			#1;					\n"
		"bne			encrypt_round_loop"
		/*--------------------------------------------------------------------*/
        /* Store state                                                        */
        /*--------------------------------------------------------------------*/
        "ldmia			sp!,			{r6}; 								\n"
        "stmia			r6,				{r0-r1};           					\n"
        /*--------------------------------------------------------------------*/
        /* Restore registers                                                  */
        /*--------------------------------------------------------------------*/
        "ldmia        	sp!,      		{r0-r12,lr};           				\n"
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
		"mov			r8,				#27;								\n"
		/* end of keys */
        "add           	r7,            	r7,        			#108;			\n"
	"decrypt_round_loop:"
        /* AddRoundKey */
        "ldmdb			r7!,			{r9};             					\n"
		/* k = k eor x */
        "eor			r9,				r9, 				r0;				\n"
		/* x = x eor y */
        "eor			r0,				r0, 				r1;				\n"
		/* x = S(-3)x */
		"ror			r0,				#3;									\n"
		/* y = x */
		"mov			r1,				r0;									\n"
		/* x = k - x */
		"sub			r0,				r9,					r0;				\n"
		/* x = S(8)(k) */
		"mov			r2,				r9,lsl#8;							\n"
		"mov			r3,				r9,lsr#24;							\n"
		"eor			r0, 			r2,					r3;				\n"
		/* loop control */
		"subs          	r8,           	r8,       		    #1;				\n"
        "bne     	    decrypt_round_loop;									\n" 
        /*--------------------------------------------------------------------*/
        /* Store state                                                        */
        /*--------------------------------------------------------------------*/
        "ldmia        	sp!,            {r6};           					\n"
        "stmia         	r6,          	{r0-r1};          					\n"
        /*--------------------------------------------------------------------*/
        /* Restore registers                                                  */
        /*--------------------------------------------------------------------*/
        "ldmia        sp!,      {r0-r12,lr};           \n" /*                 */
        /*--------------------------------------------------------------------*/
    :
    : [block] "r" (block), [roundKeys] "r" (roundKeys) 
	); 
}