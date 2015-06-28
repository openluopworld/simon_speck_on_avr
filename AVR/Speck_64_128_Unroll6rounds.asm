/*
 * Speck_64_128_Unroll6rounds.asm
 *
 *  Created: 2015/6/28 21:22:30
 *   Author: LuoPeng
 */ 

.def currentRound = r20;
.def totalRound = r21;
.def zero = r22;
.def eight = r23;

.dseg
	plainText: .byte 8 ; the 8 bytes of plaintext, from high byte to low byte.
	cipherText: .byte 8 ; the 8 bytes of ciphertext, from high byte to low byte.
	keysRAM: .byte 108; the 27*4 bytes of round keys

.cseg
main:
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

	; transfer the sub keys from flash to RAM
	ldi r28, low(keysRAM);
	ldi r29, high(keysRAM);
	ldi r30, low(keys<<1);
	ldi r31, high(keys<<1);
	clr currentRound;
	ldi totalRound, 27;
transfer:
	lpm r0, z+;
	st y+, r0;
	lpm r0, z+;
	st y+, r0;
	lpm r0, z+;
	st y+, r0;
	lpm r0, z+;
	st y+, r0;
	inc currentRound;
	cp currentRound, totalRound;
brne transfer;
	
	rcall highThroughput;
	ret;

	/*
	 * Subrountine: highThroughput
	 * Function：   A High-Throughput/Low-RAM implementation of Speck64/128
	 *
	 * The round keys are in RAM.
	 * But the 3-bit rotation is implementated in 14 cycels ( not 15 cycles of before ) using 20 bytes of flash
	 *   ( not 30 bytes of before ) with the AVR mul instruction
	 *
	 * R0 and R1 must be used for mul instruction. 
	 * 3 rounds must be extended because the result with mul instruction is not in the original registers.
	 */
highThroughput:
	; load the plaintext from RAM to registers
	ldi r26, low(plainText);
	ldi r27, high(plainText);
	ld r11, x+;
	ld r10, x+;
	ld r9, x+;
	ld r8, x+;
	ld r7, x+;
	ld r6, x+;
	ld r5, x+;
	ld r4, x+;

	; prepare for encryption
	clr currentRound; the default value is 0
	ldi totalRound, 4;
	clr zero; the default value is 0
	ldi eight, 8;
	ldi r28, low(keysRAM);
	ldi r29, high(keysRAM);
loop:
	// loop 1
	; load k: [r15, r14, r13, r12], r12 is the lowest byte
	; one round: 
	ld r12, y+;
	ld r13, y+;
	ld r14, y+;
	ld r15, y+;
	; x = S(8)( S(-8)(x) + y)
	; addition modulo 2^n
	add r9, r4; x1 = x1 + y0
	adc r10, r5; x2 = x2 + y1
	adc r11, r6; x3 = x3 + y2
	adc r8, r7; x0 = x0 + y3;
	; k = ( S(-8)(x) + y ) eor k
	eor r12, r9;
	eor r13, r10;
	eor r14, r11;
	eor r15, r8;
	; y = s(3)y
	mul r4, eight;
	movw r8, r0;
	mul r6, eight;
	movw r10, r0;
	mul r5, eight;
	eor r9, r0;
	eor r10, r1;
	mul r7, eight;
	eor r11, r0;
	eor r8, r1;
	; y = S(3)(y) eor ( S(-8)(x) + y ) eor k
	eor r8, r12;
	eor r9, r13;
	eor r10, r14;
	eor r11, r15;

	; // loop 2
	; // after loop 1, K stores X, X does Y, so we should use Y to store K.
	; // That is: K->X  X->Y  Y->K
	; // 每一轮中数据的转移是从寄存器到寄存器，不是从第几个到第几个
	; // X的值保存在K中，Y的值会保存在X中，K作为输入；只是每一轮中X、Y和K真正对应的寄存器会交换
	; // X = [r15, r14, r13, r12]
	; // Y = [r11, r10, r9, r8]
	; // K = [r7, r6, r5, r4]
	ld r4, y+;
	ld r5, y+;
	ld r6, y+;
	ld r7, y+;
	; x = S(8)( S(-8)(x) + y)
	; addition modulo 2^n
	add r13, r8; x1 = x1 + y0
	adc r14, r9; x2 = x2 + y1
	adc r15, r10; x3 = x3 + y2
	adc r12, r11; x0 = x0 + y3;
	; k = ( S(-8)(x) + y ) eor k
	eor r4, r13;
	eor r5, r14;
	eor r6, r15;
	eor r7, r12;
	; y = s(3)y
	mul r8, eight;
	movw r12, r0;
	mul r10, eight;
	movw r14, r0;
	mul r9, eight;
	eor r13, r0;
	eor r14, r1;
	mul r11, eight;
	eor r15, r0;
	eor r12, r1;
	; y = S(3)(y) eor ( S(-8)(x) + y ) eor k
	eor r12, r4;
	eor r13, r5;
	eor r14, r6;
	eor r15, r7;
	; loop 3
	; // Y = [r15, r14, r13, r12]
	; // K = [r11, r10, r9, r8]
	; // X = [r7, r6, r5, r4]
	ld r8, y+;
	ld r9, y+;
	ld r10, y+;
	ld r11, y+;
	; x = S(8)( S(-8)(x) + y)
	; addition modulo 2^n
	add r5, r12; x1 = x1 + y0
	adc r6, r13; x2 = x2 + y1
	adc r7, r14; x3 = x3 + y2
	adc r4, r15; x0 = x0 + y3;
	; k = ( S(-8)(x) + y ) eor k
	eor r8, r5;
	eor r9, r6;
	eor r10, r7;
	eor r11, r4;
	; y = s(3)y
	mul r12, eight;
	movw r4, r0;
	mul r14, eight;
	movw r6, r0;
	mul r13, eight;
	eor r5, r0;
	eor r6, r1;
	mul r15, eight;
	eor r7, r0;
	eor r4, r1;
	; y = S(3)(y) eor ( S(-8)(x) + y ) eor k
	eor r4, r8;
	eor r5, r9;
	eor r6, r10;
	eor r7, r11;

	/*
	 * Unroll 6 rounds
	 */
	// loop 4
	; load k: [r15, r14, r13, r12], r12 is the lowest byte
	; one round: 
	ld r12, y+;
	ld r13, y+;
	ld r14, y+;
	ld r15, y+;
	; x = S(8)( S(-8)(x) + y)
	; addition modulo 2^n
	add r9, r4; x1 = x1 + y0
	adc r10, r5; x2 = x2 + y1
	adc r11, r6; x3 = x3 + y2
	adc r8, r7; x0 = x0 + y3;
	; k = ( S(-8)(x) + y ) eor k
	eor r12, r9;
	eor r13, r10;
	eor r14, r11;
	eor r15, r8;
	; y = s(3)y
	mul r4, eight;
	movw r8, r0;
	mul r6, eight;
	movw r10, r0;
	mul r5, eight;
	eor r9, r0;
	eor r10, r1;
	mul r7, eight;
	eor r11, r0;
	eor r8, r1;
	; y = S(3)(y) eor ( S(-8)(x) + y ) eor k
	eor r8, r12;
	eor r9, r13;
	eor r10, r14;
	eor r11, r15;

	; // loop 5
	; // after loop 1, K stores X, X does Y, so we should use Y to store K.
	; // That is: K->X  X->Y  Y->K
	; // 每一轮中数据的转移是从寄存器到寄存器，不是从第几个到第几个
	; // X的值保存在K中，Y的值会保存在X中，K作为输入；只是每一轮中X、Y和K真正对应的寄存器会交换
	; // X = [r15, r14, r13, r12]
	; // Y = [r11, r10, r9, r8]
	; // K = [r7, r6, r5, r4]
	ld r4, y+;
	ld r5, y+;
	ld r6, y+;
	ld r7, y+;
	; x = S(8)( S(-8)(x) + y)
	; addition modulo 2^n
	add r13, r8; x1 = x1 + y0
	adc r14, r9; x2 = x2 + y1
	adc r15, r10; x3 = x3 + y2
	adc r12, r11; x0 = x0 + y3;
	; k = ( S(-8)(x) + y ) eor k
	eor r4, r13;
	eor r5, r14;
	eor r6, r15;
	eor r7, r12;
	; y = s(3)y
	mul r8, eight;
	movw r12, r0;
	mul r10, eight;
	movw r14, r0;
	mul r9, eight;
	eor r13, r0;
	eor r14, r1;
	mul r11, eight;
	eor r15, r0;
	eor r12, r1;
	; y = S(3)(y) eor ( S(-8)(x) + y ) eor k
	eor r12, r4;
	eor r13, r5;
	eor r14, r6;
	eor r15, r7;
	; loop 6
	; // Y = [r15, r14, r13, r12]
	; // K = [r11, r10, r9, r8]
	; // X = [r7, r6, r5, r4]
	ld r8, y+;
	ld r9, y+;
	ld r10, y+;
	ld r11, y+;
	; x = S(8)( S(-8)(x) + y)
	; addition modulo 2^n
	add r5, r12; x1 = x1 + y0
	adc r6, r13; x2 = x2 + y1
	adc r7, r14; x3 = x3 + y2
	adc r4, r15; x0 = x0 + y3;
	; k = ( S(-8)(x) + y ) eor k
	eor r8, r5;
	eor r9, r6;
	eor r10, r7;
	eor r11, r4;
	; y = s(3)y
	mul r12, eight;
	movw r4, r0;
	mul r14, eight;
	movw r6, r0;
	mul r13, eight;
	eor r5, r0;
	eor r6, r1;
	mul r15, eight;
	eor r7, r0;
	eor r4, r1;
	; y = S(3)(y) eor ( S(-8)(x) + y ) eor k
	eor r4, r8;
	eor r5, r9;
	eor r6, r10;
	eor r7, r11;

	inc currentRound;
	cp currentRound, totalRound;
	breq end;
	jmp loop;
	
end:

	/*
	 * the remain 3 rounds
	 */
	// loop 1
	; load k: [r15, r14, r13, r12], r12 is the lowest byte
	; one round: 
	ld r12, y+;
	ld r13, y+;
	ld r14, y+;
	ld r15, y+;
	; x = S(8)( S(-8)(x) + y)
	; addition modulo 2^n
	add r9, r4; x1 = x1 + y0
	adc r10, r5; x2 = x2 + y1
	adc r11, r6; x3 = x3 + y2
	adc r8, r7; x0 = x0 + y3;
	; k = ( S(-8)(x) + y ) eor k
	eor r12, r9;
	eor r13, r10;
	eor r14, r11;
	eor r15, r8;
	; y = s(3)y
	mul r4, eight;
	movw r8, r0;
	mul r6, eight;
	movw r10, r0;
	mul r5, eight;
	eor r9, r0;
	eor r10, r1;
	mul r7, eight;
	eor r11, r0;
	eor r8, r1;
	; y = S(3)(y) eor ( S(-8)(x) + y ) eor k
	eor r8, r12;
	eor r9, r13;
	eor r10, r14;
	eor r11, r15;

	; // loop 2
	; // after loop 1, K stores X, X does Y, so we should use Y to store K.
	; // That is: K->X  X->Y  Y->K
	; // 每一轮中数据的转移是从寄存器到寄存器，不是从第几个到第几个
	; // X的值保存在K中，Y的值会保存在X中，K作为输入；只是每一轮中X、Y和K真正对应的寄存器会交换
	; // X = [r15, r14, r13, r12]
	; // Y = [r11, r10, r9, r8]
	; // K = [r7, r6, r5, r4]
	ld r4, y+;
	ld r5, y+;
	ld r6, y+;
	ld r7, y+;
	; x = S(8)( S(-8)(x) + y)
	; addition modulo 2^n
	add r13, r8; x1 = x1 + y0
	adc r14, r9; x2 = x2 + y1
	adc r15, r10; x3 = x3 + y2
	adc r12, r11; x0 = x0 + y3;
	; k = ( S(-8)(x) + y ) eor k
	eor r4, r13;
	eor r5, r14;
	eor r6, r15;
	eor r7, r12;
	; y = s(3)y
	mul r8, eight;
	movw r12, r0;
	mul r10, eight;
	movw r14, r0;
	mul r9, eight;
	eor r13, r0;
	eor r14, r1;
	mul r11, eight;
	eor r15, r0;
	eor r12, r1;
	; y = S(3)(y) eor ( S(-8)(x) + y ) eor k
	eor r12, r4;
	eor r13, r5;
	eor r14, r6;
	eor r15, r7;
	; loop 3
	; // Y = [r15, r14, r13, r12]
	; // K = [r11, r10, r9, r8]
	; // X = [r7, r6, r5, r4]
	ld r8, y+;
	ld r9, y+;
	ld r10, y+;
	ld r11, y+;
	; x = S(8)( S(-8)(x) + y)
	; addition modulo 2^n
	add r5, r12; x1 = x1 + y0
	adc r6, r13; x2 = x2 + y1
	adc r7, r14; x3 = x3 + y2
	adc r4, r15; x0 = x0 + y3;
	; k = ( S(-8)(x) + y ) eor k
	eor r8, r5;
	eor r9, r6;
	eor r10, r7;
	eor r11, r4;
	; y = s(3)y
	mul r12, eight;
	movw r4, r0;
	mul r14, eight;
	movw r6, r0;
	mul r13, eight;
	eor r5, r0;
	eor r6, r1;
	mul r15, eight;
	eor r7, r0;
	eor r4, r1;
	; y = S(3)(y) eor ( S(-8)(x) + y ) eor k
	eor r4, r8;
	eor r5, r9;
	eor r6, r10;
	eor r7, r11;


	; store cipherText
	st x+, r11;
	st x+, r10;
	st x+, r9;
	st x+, r8;
	st x+, r7;
	st x+, r6;
	st x+, r5;
	st x+, r4;
	ret;

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

