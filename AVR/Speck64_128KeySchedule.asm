/*
 * Speck64_128KeySchedule.asm
 *
 *  Created: 2015/6/25 10:17:42
 *   Author: LuoPeng
 */ 

.def zero = r16;
.def four = r17;
.def eight = r18;
.def twelve = r19;
.def currentRound = r20;
.def totalRound = r21;
.def temp = r22;
.def transferL = r23;

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
	ldi four, 4;
	ldi eight, 8;
	ldi twelve, 12;

	; load the l2, l1, l0
	clr currentRound;
	ldi r28, low(lRAM);
	ldi r29, high(lRAM);
	add r28, twelve;
	adc r29, zero;
	ldi r30, low(initialKeys<<1);
	ldi r31, high(initialKeys<<1);
loadL:
	lpm r0, z+;
	st -y, r0;
	inc currentRound;
	cp currentRound, twelve;
	brne loadL;
	; load k0
	ldi r26, low(keysRAM);
	ldi r27, high(keysRAM);
	add r26, four;
	adc r27, zero;
	lpm r0, z+;
	st -x, r0;
	lpm r0, z+;
	st -x, r0;
	lpm r0, z+;
	st -x, r0;
	lpm r0, z+;
	st -x, r0;
	nop;

	rcall keySchedule;
	ret;
	
	/* 
	 * Subroutine: Key schedule
	 * Function:   Compute the sub keys.
	 */
keySchedule:
	clr currentRound;
	ldi totalRound, 27;
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
	sub r26, four;
	sbc r27, zero;

	ldi r30, low(lRAM);
	ldi r31, high(lRAM);
	clr transferL;
L:
	ld temp, y+;
	st z+, temp;
	inc transferL;
	cp transferL, eight;
	brne L;
	ldi r28, low(lRAM);
	ldi r29, high(lRAM);
	st z+, r5;
	st z+, r6;
	st z+, r7;
	st z+, r4;

	; loop control
	inc currentRound;
	cp currentRound, totalRound;
brne subkey;
	nop;
	ret;

; K = (l2, l1, l0, k0)
initialKeys: .db 0x1b, 0x1a, 0x19, 0x18, 0x13, 0x12, 0x11, 0x10, 0x0b, 0x0a, 0x09, 0x08, 0x03, 0x02, 0x01, 0x00;


