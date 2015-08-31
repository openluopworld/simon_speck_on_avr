 /*
 * Spcek_Sce2.asm
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
 *  Created: 2015/8/30 13:22:15
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
	 * Function:   encyrpt the 128 bytes of data
	 * RAM:        sendData
	 *             keys
	 *             vector
	 * Register:   [r0, r7] plain text 
	 *             [r12, r15] round key
	 *             [r16, r23] the vector or cipher text of last round  
	 *             r24, currentRound
	 *             r25, currentBlock
	 *             X the address of data
	 *             Y the address of keys
	 *             r30 store 0
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
encLoop:
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
	; x = ( S(-8)(x) + y ) eor k
	movw r4, r12; r5:r4 = r13:r12
	movw r6, r14; r7:r6 = r15:r14
	
	inc currentRound;
	cpi currentRound, 27;
	brne encLoop;

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

; 3b726574 7475432d
; cipher text is: 8c6fa548 454e028b
nonce: .db 0x3b, 0x72, 0x65, 0x74, 0x74, 0x75, 0x43, 0x2d

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