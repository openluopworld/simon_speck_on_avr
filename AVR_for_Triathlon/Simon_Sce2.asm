/*
 * Simon_Sce2.asm
 *
 * Encrypt 128 bits of data in CTR mode.
 * 1.Since the 128 bits can not be operated parallelly in AVR. The time is just operating 64 bits.
 * 2.Nonce is stored in flash.
 * 3.Others are same with the paper.
 * 
 * Data In RAM:   sendData: the data needs to be encrypted 
 *                count:    the counter in CTR mode
 * Data In Flash: nonce:    the nonce
 *                keys:     the round keys.( No master key and key schedule is required)
 *
 *  Created: 2015/8/28 18:03:51
 *   Author: LuoPeng
 */ 

 .def currentRound = r24;
 .def zero = r25;

 .dseg
	sendData: .byte 8;
	counter:    .byte 8;

 .cseg
 main:
	; init sendData
	ldi r26, low(sendData);
	ldi r27, high(sendData);
	ldi r16, 0x00;
	st x+, r16;
	ldi r16, 0x00;
	st x+, r16;
	ldi r16, 0x00;
	st x+, r16;
	ldi r16, 0x00;
	st x+, r16;
	ldi r16, 0x00;
	st x+, r16;
	ldi r16, 0x00;
	st x+, r16;
	ldi r16, 0x00;
	st x+, r16;
	ldi r16, 0x00;
	st x+, r16;
	; init counter
	ldi r16, 0x00;
	st x+, r16;
	ldi r16, 0x00;
	st x+, r16;
	ldi r16, 0x00;
	st x+, r16;
	ldi r16, 0x00;
	st x+, r16;
	ldi r16, 0x00;
	st x+, r16;
	ldi r16, 0x00;
	st x+, r16;
	ldi r16, 0x00;
	st x+, r16;
	ldi r16, 0x00;
	st x+, r16;

	rcall encryption;
	ret;

	/*
	 * Subroutine: encryption
	 * Function:   
	 * Regester:   
	 * 
	 */
 encryption:

	clr currentRound;
	clr zero;

	; load the counter
	ldi r26, low(counter);
	ldi r27, high(counter);
	ld r7, x+ ; the highest byte
	ld r6, x+ ;
	ld r5, x+ ;
	ld r4, x+ ;
	ld r3, x+ ;
	ld r2, x+ ;
	ld r1, x+ ;
	ld r0, x+ ; the lowest byte
	; load the nonce. nonce is fixed
	ldi r30, low(nonce<<1);
	ldi r31, high(nonce<<1);
	lpm r23, z+; the highest byte
	lpm r22, z+;
	lpm r21, z+;
	lpm r20, z+;
	lpm r19, z+;
	lpm r18, z+;
	lpm r17, z+;
	lpm r16, z+; the lowest byte
	; counter eor nonce
	eor r7, r23;
	eor r6, r22;
	eor r5, r21;
	eor r4, r20;
	eor r3, r19;
	eor r2, r18;
	eor r1, r17;
	eor r0, r16;
	
	/* 
	 * encrypt (nonce eor counter) 
	 * X = [r7, r6, r5, r4], Y = [r3, r2, r1, r0]
	 */
	ldi r30, low(keys<<1);
	ldi r31, high(keys<<1);
loop:
	; store the 4 bytes of sub key to K = [r11, r10, r9, r8]
	lpm r8, z+;  the lowest byte
 	lpm r9, z+;
	lpm r10, z+;
	lpm r11, z+; the highest byte
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
	cpi currentRound, 44;
brne loop;
	; load the plain text
	ldi r26, low(sendData);
	ldi r27, high(sendData);
	ld r15, x+; the highest byte
	ld r14, x+;
	ld r13, x+;
	ld r12, x+;
	ld r11, x+;
	ld r10, x+;
	ld r9, x+;
	ld r8, x+;
	; eor
	eor r7, r15;
	eor r6, r14;
	eor r5, r13;
	eor r4, r12;
	eor r3, r11;
	eor r2, r10;
	eor r1, r9;
	eor r0, r8;
	; store the cipher text
	st -x, r0;
	st -x, r1;
	st -x, r2;
	st -x, r3;
	st -x, r4;
	st -x, r5;
	st -x, r6;
	st -x, r7;
	ret;

nonce: .db 0x65, 0x6b, 0x69, 0x6c, 0x20, 0x64, 0x6e, 0x75

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


