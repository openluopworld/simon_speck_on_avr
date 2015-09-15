/*
 * Speck_Author_OneBlock.asm
 *
 *  Created: 2015/6/25 10:17:42
 *   Author: LuoPeng
 */ 

.def temp = r22;
.def transferL = r23;
.def zero = r24;
.def currentRound = r25;

.dseg ; RAM
	plainText: .byte 8 ; the 8 bytes of plaintext, from low byte to high byte.
	cipherText: .byte 8 ; the 8 bytes of ciphertext, from low byte to high byte.
	lRAM: .byte 12;	
	keysRAM: .byte 108; 27*4

.CSEG
	/*
	 * main function
	 */
main:
	clr zero;
	clr currentRound;
	ldi r26, low(keysRAM);
	ldi r27, high(keysRAM);

	; load the l2, l1, l0
	ldi r28, low(lRAM);
	ldi r29, high(lRAM);
	adiw r26, 12;
	ldi r30, low(initialKeys<<1);
	ldi r31, high(initialKeys<<1);
loadL:
	lpm r0, z+;
	st -y, r0;
	inc currentRound;
	cpi currentRound, 12;
	brne loadL;
	; load k0
	ldi r26, low(keysRAM);
	ldi r27, high(keysRAM);
	adiw r26, 4;
	lpm r0, z+;
	st -x, r0;
	lpm r0, z+;
	st -x, r0;
	lpm r0, z+;
	st -x, r0;
	lpm r0, z+;
	st -x, r0;

	; plaintext: 3b726574 7475432d(0x)
	; ciphertext shoud be:8c6fa548 454e028b(0x)
	ldi r26, low(plainText);
	ldi r27, high(plainText);
	ldi temp, 0x3b;
	st x+, temp;
	ldi temp, 0x72;
	st x+, temp;
	ldi temp, 0x65;
	st x+, temp;
	ldi temp, 0x74;
	st x+, temp;
	ldi temp, 0x74;
	st x+, temp;
	ldi temp, 0x75;
	st x+, temp;
	ldi temp, 0x43;
	st x+, temp;
	ldi temp, 0x2d;
	st x+, temp;
	
	/* 
	 * Subroutine: Key schedule
	 * Function:   Compute the sub keys.
	 */
keySchedule:
	; prepare for key schedule
	clr currentRound;
	clr zero; store the value of zero
	ldi r26, low(keysRAM);
	ldi r27, high(keysRAM);
subkey:	
	ldi r28, low(lRAM);
	ldi r29, high(lRAM);
	;[r3,r2,r1,r0] to store ki
	ld r0, x+;
	ld r1, x+;
	ld r2, x+;
	ld r3, x+;
	;[r7,r6,r5,r4] to store li
	ld r4, y+;
	ld r5, y+;
	ld r6, y+;
	ld r7, y+;
	; k(i)+S(-8)l(i)
	add r5, r0;
	adc r6, r1;
	adc r7, r2;
	adc r4, r3;
	; eor i
	eor r5, currentRound;
	; S3(ki)
	lsl r0;
	rol r1;
	rol r2;
	rol r3;
	adc r0, zero;
	lsl r0;
	rol r1;
	rol r2;
	rol r3;
	adc r0, zero;
	lsl r0;
	rol r1;
	rol r2;
	rol r3;
	adc r0, zero;
	; k(i+1)
	eor r0, r5;
	eor r1, r6;
	eor r2, r7;
	eor r3, r4;
	; store k(i+1)
	st x+, r0;
	st x+, r1;
	st x+, r2;
	st x+, r3;
	sbiw r26, 4;

	; update the lRAM, l[i] is useless.
	ldi r30, low(lRAM);
	ldi r31, high(lRAM);
	clr transferL;
L:
	ld temp, y+;
	st z+, temp;
	inc transferL;
	cpi transferL, 8;
	brne L;
	st z+, r5;
	st z+, r6;
	st z+, r7;
	st z+, r4;

	; loop control
	inc currentRound;
	cpi currentRound, 26; 27 may be wrong
brne subkey;
	;ret;

	/*
	 * Subroutine: encryption
	 * Function:   The low RAM implementation of Speck64/128
	 * 
	 * The round keys have been pre-expanded and stored in flash.
	 * When a round key k is required, it is loaded from flash directly into a register
	 */
encryption:

	/*
	 * load ptelainText from RAM to registers, r7 stores the highest byte, r0 stores the lowest byte.
	 */
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

	ldi r30, low(keysRAM);
	ldi r31, high(keysRAM);
	clr currentRound;   the initial value is 0
	clr zero;           the initial value is 0
loop:
	; load k: [r15, r14, r13, r12], r12 is the lowest byte
	ld r12, z+;
	ld r13, z+;
	ld r14, z+;
	ld r15, z+;
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
	; finished?
	inc currentRound;
	cpi currentRound, 27;
brne loop;

	; store cipherText from registers to RAM
	st x+, r7;
	st x+, r6;
	st x+, r5;
	st x+, r4;
	st x+, r3;
	st x+, r2;
	st x+, r1;
	st x+, r0;
	ret;

; K = (l2, l1, l0, k0)
initialKeys: .db 0x1b, 0x1a, 0x19, 0x18, 0x13, 0x12, 0x11, 0x10, 0x0b, 0x0a, 0x09, 0x08, 0x03, 0x02, 0x01, 0x00;