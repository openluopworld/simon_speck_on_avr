/*
 * Speck64_128_HighThroughput.asm
 *
 *  Created: 2015/6/10 21:33:05
 *   Author: LuoPeng
 */ 

	/*
	 * A High-Throughput/Low-RAM SPECK implementation
	 *
	 * The round keys are in RAM.
	 * But the 3-bit rotation is implementated in 14 cycels ( not 15 cycles of before ) using 20 bytes of flash
	 *	 ( not 30 bytes of before ) with the AVR mul instruction
	 * 
	 * The difference is that R0 and R1 should be used for mul instruction
	 */
.def currentRound = r16;
.def totalRound = r17;
.def zero = r18;
.def eight = r19;

.dseg
	plainText: .byte 8 ; the 8 bytes of plaintext, from low byte to high byte.
	cipherText: .byte 8 ; the 8 bytes of ciphertext, from low byte to high byte.
	; the 27*4 bytes of round keys
	keysRAM: .byte 108

.cseg
	keys: .db 0x00, 0x01, 0x02, 0x03;
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

highThroughput:

	clr currentRound; the default value is 0
	ldi totalRound, 9;
	clr zero; the default value is 0
	ldi eight, 8;

	ldi r26, low(plainText);
	ldi r27, high(plainText);

	ld r4, x+;
	ld r5, x+;
	ld r6, x+;
	ld r7, x+;
	ld r8, x+;
	ld r9, x+;
	ld r10, x+;
	ld r11, x+;

	ldi r30, low(keysRAM);
	ldi r31, high(keysRAM);
loop:
	// loop 1
	; load k: [r15, r14, r13, r12], r12 is the lowest byte
	; one round: 
	ld r12, z+;
	ld r13, z+;
	ld r14, z+;
	ld r15, z+;
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
	ld r4, z+;
	ld r5, z+;
	ld r6, z+;
	ld r7, z+;
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
	ld r8, z+;
	ld r9, z+;
	ld r10, z+;
	ld r11, z+;
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
	jmp loop;
	breq end;
end:
	; store cipherText
	;ldi r26, low(plainText);
	;ldi r27, high(plainText);
	st x+, r0;
	st x+, r1;
	st x+, r2;
	st x+, r3;
	st x+, r4;
	st x+, r5;
	st x+, r6;
	st x+, r7;
	ret;


