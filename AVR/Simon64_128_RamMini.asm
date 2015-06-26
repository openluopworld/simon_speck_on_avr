/*
 * Simon64_128_RamMini.asm
 *
 *  Created: 2015/6/2 11:05:10
 *   Author: LuoPeng
 */ 

 /*
  * A minimal RAM Implementation of SIMON
  */

 .def currentRound = r20; the current round [0,39]
 .def totalRound = r21; total round, is 40 (in key generation ) or 44 (in encryption)
 .def zero = r22; the adc operation

 .dseg ; RAM( data segment)
	plainText: .byte 8 ; the 8 bytes of plaintext, from high byte to low byte.
	cipherText: .byte 8 ; the 8 bytes of ciphertext, from high byte to low byte

 .cseg ; Flash( code segement)

	/*
	 * main function
	 */
 main:
	; load the text plaintext: 656b696c 20646e75(0x,from the paper)
	; the cipher text should be: 44c8fc20 b9dfa07a
	ldi r26, low(plainText) ; 1 cycle
	ldi r27, high(plainText) ;  1 cycle
	ldi r16, 0x65;
	st x+, r16;
	ldi r16, 0x6b;
	st x+, r16;
	ldi r16, 0x69;
	st x+, r16;
	ldi r16, 0x6c;
	st x+, r16;
	ldi r16, 0x20;
	st x+, r16;
	ldi r16, 0x64;
	st x+, r16;
	ldi r16, 0x6e;
	st x+, r16;
	ldi r16, 0x75;
	st x+, r16;

	rcall encryption;
	ret;

	/*
	 * Subroutine: encryption
	 * Function:   the minimal RAM implementation of Simon64/128
	 *
	 * the sub keys are stored in Flash
	 */
 encryption:
	clr currentRound ; set 0, have done rounds ; 1 cycle
	ldi totalRound, 44; the total rounds ; 1 cycle
	clr zero; 1 cycle

	ldi r26, low(plainText) ; 1 cycle
	ldi r27, high(plainText) ;  1 cycle
	; load the plaintext from RAM to registers [r7,...,r0], X = [r7, r6, r5, r4], Y = [r3, r2, r1, r0]
	ld r7, x+ ; the highest byte
	ld r6, x+ ;
	ld r5, x+ ;
	ld r4, x+ ;
	ld r3, x+ ;
	ld r2, x+ ;
	ld r1, x+ ;
	ld r0, x+ ;
	
	; should use "<<1"
	; ldi r30, low(keys);
	; ldi r31, high(keys);
	ldi r30, low(keys<<1) ; z is the current address of keys
	ldi r31, high(keys<<1) ;

loop:
	; get the sub key k
	lpm r8, z+ ; store the 4 bytes of sub key to K = [r11, r10, r9, r8]
 	lpm r9, z+ ;
	lpm r10, z+ ;
	lpm r11, z+ ;
	nop;
	; k = k eor y
	eor r8, r0;
	eor r9, r1;
	eor r10, r2;
	eor r11, r3 ;
	; move x to y 
	movw r0, r4; the index must be even ( R1:R0 = R5:R4)
	movw r2, r6; ( R3:R2 : R7:R6 )
	; rotate x by left with 1 bit
	lsl r4; logical shift left: bit 0 is cleared, bit 7 is loaded into the C flag of the SREG
	rol r5; rotate left through carry: the C flag in shifted into bit 0, bit 7 is shifted into the C flag
	rol r6;
	rol r7;
	adc r4, zero;
	; move x to t, t stands for [r15, r14, r13, r12]
	movw r12, r4;
	movw r14, r6;
	; t & S8(y)
	and r12, r3;
	and r13, r0;
	and r14, r1;
	and r15, r2;
	; x = S2(x)
	lsl r4;
	rol r5;
	rol r6;
	rol r7;
	adc r4, zero;
	; x = x eor t
	eor r4, r12;
	eor r5, r13;
	eor r6, r14;
	eor r7, r15;
	; x = x eor k
	eor r4, r8;
	eor r5, r9;
	eor r6, r10;
	eor r7, r11;
	inc currentRound;
	cp currentRound, totalRound;
brne loop;

	nop;
	; load the ciphertext from registers [r7,...,r0] RAM
	st x+, r7; the highest byte
	st x+, r6;
	st x+, r5;
	st x+, r4;
	st x+, r3;
	st x+, r2;
	st x+, r1;
	st x+, r0;
	ret;


;The memory reserved with .db must be an even number of bytes. One or more of your strings must be an odd number of bytes,
;so the assembler padded it with an extra byte if the number of bytes is not even
; the 44*4 bytes of round keys, the sub keys is from the original paper <<Simon and Speck>>
keys: 
.db 0,   1,   2,   3,   8,   9,   10,  11, 16,  17,  18,  19,  24,  25,  26,  27
.db 195, 17,  160, 112, 73,  236, 112, 183, 53,  232, 227, 87,  66,  188, 151, 211
.db 31,  248, 220, 148, 24,  95,  75,  191, 185, 171, 93,  142, 99,  168, 244, 219
.db 252, 40,  12,  205, 17,  153, 182, 92,  165, 18,  241, 121, 99,  88,  32,  119
.db 18,  12,  136, 153, 88,  124, 233, 28,  69,  33,  237, 200, 184, 219, 0,   184
.db 86,  39,  106, 232, 221, 212, 6,   124, 10,  223, 82,  171, 168, 102, 127, 36
.db 166, 124, 88,  83,  241, 19,  92,  210, 75,  182, 131, 69,  13,  150, 156, 125
.db 243, 194, 191, 239, 19,  133, 237, 137, 78,  252, 141, 48,  54,  42,  26,  191
.db 112, 157, 73,  225, 255, 210, 228, 76,  239, 235, 183, 50,  193, 5,   117, 196
.db 232, 41,  233, 208, 185, 132, 228, 143, 238, 75,  5,   66,  226, 186, 119, 175
.db 2,   156, 25,  24,  28,  63,  158, 113, 147, 247, 28,  12,  150, 70,  223, 21