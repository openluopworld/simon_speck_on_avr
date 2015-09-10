
#include <stdint.h>
#include "constants.h"	

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
		"eor			r3, 			r3,				r4;					\n"
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