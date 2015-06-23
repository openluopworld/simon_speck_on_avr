/*
 * Simon64_128High_Thr_NoSch.asm
 *
 *  Created: 2015/6/20 11:52:17
 *   Author: LuoPeng
 */ 

  /*
  * A minimal RAM Implementation of SIMON
  */

 .def currentRound = r20; the current round [0,39]
 .def totalRound = r21; total round, is 40 (in key generation ) or 44 (in encryption)
 .def zero = r22; the adc operation

 .dseg ; RAM( data segment)
	plainText: .byte 8 ; the 8 bytes of plaintext, from low byte to high byte.
	cipherText: .byte 8 ; the 8 bytes of ciphertext, from low byte to high byte
	keysRAM: .byte 176;

 .cseg ; Flash( code segement)
	;The memory reserved with .db must be an even number of bytes. One or more of your strings must be an odd number of bytes,
	;so the assembler padded it with an extra byte
	; the 44*4 bytes of round keys
	keys: .db 0,   1,   2,   3,   8,   9,   10,  11,  16,  17,  18,  19,  24,  25,  26,  27
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
		  
 encryption:
 
	; move the address of plaintext to register X
	ldi r26, low(plainText) ;
	ldi r27, high(plainText) ;
	; the plaintext is : 00010203 08090a0b(0x)
	; the ciphertext should be: 231,124,58,2,167,182,169,182
	ldi r16, 0x00;
	st x+, r16;
	ldi r16, 0x01;
	st x+, r16;
	ldi r16, 0x02;
	st x+, r16;
	ldi r16, 0x03;
	st x+, r16;
	ldi r16, 0x08;
	st x+, r16;
	ldi r16, 0x09;
	st x+, r16;
	ldi r16, 0x0a;
	st x+, r16;
	ldi r16, 0x0b;
	st x+, r16;

	; transfer the keys from flash to RAM
	ldi r30, low(keys) ;
	ldi r31, high(keys) ;
	ldi r28, low(keysRAM);
	ldi r29, high(keysRAM);
	clr currentRound ;
	ldi totalRound, 176;
transfer:
	lpm r0, z+;
	st y+, r0;
	inc currentRound;
	cp currentRound, totalRound;
brne transfer;
	
	ldi r26, low(plainText) ; 1 cycle
	ldi r27, high(plainText) ;  1 cycle
	; load the plaintext from RAM to registers [r7,...,r0], X = [r7, r6, r5, r4], Y = [r3, r2, r1, r0]
	ld r0, x+ ; the lowest byte
	ld r1, x+ ;
	ld r2, x+ ;
	ld r3, x+ ;
	ld r4, x+ ;
	ld r5, x+ ;
	ld r6, x+ ;
	ld r7, x+ ;
	
	clr currentRound ; set 0, have done rounds ; 1 cycle
	ldi totalRound, 11; the total rounds ; 1 cycle
	clr zero; 1 cycle
	ldi r28, low(keysRAM) ; y is the current address of keys
	ldi r29, high(keysRAM) ;

loop:
	; get the sub key k
	ld r8, y+ ; store the 4 bytes of sub key to K = [r11, r10, r9, r8]
 	ld r9, y+ ;
	ld r10, y+ ;
	ld r11, y+ ;
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

	// loop 2
	ld r8, y+ ; store the 4 bytes of sub key to K = [r11, r10, r9, r8]
 	ld r9, y+ ;
	ld r10, y+ ;
	ld r11, y+ ;
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

	// loop 3
	ld r8, y+ ; store the 4 bytes of sub key to K = [r11, r10, r9, r8]
 	ld r9, y+ ;
	ld r10, y+ ;
	ld r11, y+ ;
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

	// loop 4
	ld r8, y+ ; store the 4 bytes of sub key to K = [r11, r10, r9, r8]
 	ld r9, y+ ;
	ld r10, y+ ;
	ld r11, y+ ;
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
	breq storecipher;
	jmp loop;
storecipher:
	; load the ciphertext from RAM to registers [r7,...,r0]
	st x+, r0; the lowest byte
	st x+, r1;
	st x+, r2;
	st x+, r3;
	st x+, r4;
	st x+, r5;
	st x+, r6;
	st x+, r7;
	ret;


