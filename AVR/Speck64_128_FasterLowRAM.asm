/*
 * Speck64_128_FasterLowRAM.asm
 *
 *  Created: 2015/6/10 20:41:07
 *   Author: LuoPeng
 */ 

	/*
	 * A Fashter Low-RAM SPECK implementation
	 *
	 * The round keys are also in flash.
	 * Two rounds can be implemented without the use of the Move instruction at the final.
	 */
.def currentRound = r16;
.def totalRound = r17;
.def zero = r18;

.dseg
	plainText: .byte 8 ; the 8 bytes of plaintext, from high byte to low byte.
	cipherText: .byte 8 ; the 8 bytes of ciphertext, from high byte to low byte.

.cseg
main:
	; store the test plaintext
	; plaintext: 3b726574 7475432d(0x)
	; ciphertext shoud be:8c6fa548 454e028b(0x)
	ldi r26, low(plainText);
	ldi r27, high(plainText);
	ldi r16, 0x3b;
	st x+, r16;
	ldi r16, 0x72;
	st x+, r16;
	ldi r16, 0x65;
	st x+, r16;
	ldi r16, 0x74;
	st x+, r16;
	ldi r16, 0x74;
	st x+, r16;
	ldi r16, 0x75;
	st x+, r16;
	ldi r16, 0x43;
	st x+, r16;
	ldi r16, 0x2d;
	st x+, r16;

	clr currentRound; the default value is 0
	ldi totalRound, 13;
	clr zero; the default value is 0

	; load plainText
	ldi r26, low(plainText);
	ldi r27, high(plainText);
	ld r7, x+;
	ld r6, x+;
	ld r5, x+;
	ld r4, x+;
	ld r3, x+;
	ld r2, x+;
	ld r1, x+;
	ld r0, x+;

	/*
	 * 
	 * The round keys have been pre-expanded and stored in flash.
     * When a round key k is required, it is loaded from flash directly into a register
	 */
	ldi r30, low(keys<<1);
	ldi r31, high(keys<<1);
loop:
	; load k: [r15, r14, r13, r12], r12 is the lowest byte
	lpm r12, z+;
	lpm r13, z+;
	lpm r14, z+;
	lpm r15, z+;
	; x = S(8)( S(-8)(x) + y)
	add r5, r0; x1 = x1 + y0
	adc r6, r1; x2 = x2 + y1
	adc r7, r2; x3 = x3 + y2
	adc r4, r3; x0 = x0 + y3;
	; k = ( S(-8)(x) + y ) eor k
	eor r12, r5;
	eor r13, r6;
	eor r14, r7;
	eor r15, r4;
	; y = s(3)y
	lsl r0; loop 1
	rol r1;
	rol r2;
	rol r3;
	adc r0, zero;
	lsl r0; loop 2
	rol r1;
	rol r2;
	rol r3;
	adc r0, zero;
	lsl r0; loop 3
	rol r1;
	rol r2;
	rol r3;
	adc r0, zero;
	; y = S(3)(y) eor ( S(-8)(x) + y ) eor k
	eor r0, r12;
	eor r1, r13;
	eor r2, r14;
	eor r3, r15;

	// loop 2
	lpm r4, z+;
	lpm r5, z+;
	lpm r6, z+;
	lpm r7, z+;
	add r13, r0; x1 = x1 + y0
	adc r14, r1; x2 = x2 + y1
	adc r15, r2; x3 = x3 + y2
	adc r12, r3; x0 = x0 + y3;
	eor r4, r13;
	eor r5, r14;
	eor r6, r15;
	eor r7, r12;
	lsl r0; loop 1
	rol r1;
	rol r2;
	rol r3;
	adc r0, zero;
	lsl r0; loop 2
	rol r1;
	rol r2;
	rol r3;
	adc r0, zero;
	lsl r0; loop 3
	rol r1;
	rol r2;
	rol r3;
	adc r0, zero;
	eor r0, r4;
	eor r1, r5;
	eor r2, r6;
	eor r3, r7;
	; finished?
	inc currentRound;
	cp currentRound, totalRound;
	jmp loop;
	breq lastRound; 
lastRound:

	; load k: [r15, r14, r13, r12], r12 is the lowest byte
	lpm r12, z+;
	lpm r13, z+;
	lpm r14, z+;
	lpm r15, z+;
	; x = S(8)( S(-8)(x) + y)
	add r5, r0; x1 = x1 + y0
	adc r6, r1; x2 = x2 + y1
	adc r7, r2; x3 = x3 + y2
	adc r4, r3; x0 = x0 + y3;
	; k = ( S(-8)(x) + y ) eor k
	eor r12, r5;
	eor r13, r6;
	eor r14, r7;
	eor r15, r4;
	; y = s(3)y
	lsl r0; loop 1
	rol r1;
	rol r2;
	rol r3;
	adc r0, zero;
	lsl r0; loop 2
	rol r1;
	rol r2;
	rol r3;
	adc r0, zero;
	lsl r0; loop 3
	rol r1;
	rol r2;
	rol r3;
	adc r0, zero;
	; y = S(3)(y) eor ( S(-8)(x) + y ) eor k
	eor r0, r12;
	eor r1, r13;
	eor r2, r14;
	eor r3, r15;
	; move the value to X
	; can be deleted, so store cipher text directly from [r15,r14,r13,r12]
	;movw r4, r12; r5:r4 = r13:r12
	;movw r6, r14; r7:r6 = r15:r14

	st x+, r15;
	st x+, r14;
	st x+, r13;
	st x+, r12;
	st x+, r3;
	st x+, r2;
	st x+, r1;
	st x+, r0;
	ret;

; the 27*4 bytes of round keys
keys: 
	.db 0x00, 0x01, 0x02, 0x03;
	.db 0x09, 0x03, 0x1d, 0x13;
	.db 0x53, 0x0d, 0xd8, 0xbb;
	.db 0xf3, 0x4d, 0x33, 0x0d;
	.db 0x65, 0x35, 0xa4, 0x7f;
	.db 0x55, 0xce, 0xe6, 0x67;
	.db 0xd2, 0xb3, 0x8c, 0xe9;
	.db 0xbd, 0x6c, 0xc7, 0xaa;
	.db 0xc8, 0x51, 0x59, 0x7f;
	.db 0xc2, 0x82, 0xfa, 0x03;
	.db 0xad, 0x33, 0x35, 0x31;
	.db 0x82, 0x08, 0xf7, 0xdf;
	.db 0x93, 0x7c, 0x48, 0x9e;
	.db 0x28, 0xb9, 0x34, 0xa9;
	.db 0xf5, 0xde, 0x2e, 0xdd;
	.db 0x8d, 0x38, 0xe6, 0x8b;
	.db 0x89, 0x6b, 0x70, 0x1f;
	.db 0xf8, 0xaa, 0x87, 0x2b;
	.db 0x17, 0x6c, 0xd7, 0x12;
	.db 0x6c, 0xcd, 0xac, 0x6e;
	.db 0x12, 0xb9, 0x1a, 0x6a;
	.db 0xca, 0x6b, 0xbc, 0x10;
	.db 0x32, 0xdd, 0x57, 0x60;
	.db 0x81, 0xb3, 0xc9, 0xd3;
	.db 0x3d, 0x81, 0x47, 0xb3;
	.db 0x35, 0x3c, 0x11, 0x8c;
	.db 0x3a, 0x52, 0x6b, 0xfe;