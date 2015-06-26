/*
 * Simon64_128_HighThroughput.asm
 *
 *  Created: 2015/6/2 23:24:09
 *   Author: LuoPeng
 */ 

.def zero = r14;
.def remain8 = r15;      currentRound % 8
.def four = r16;         control when to load a new z to register
.def eight = r17;        from k(i+1) to k(i+3), the register should add 4

.def currentRound = r18;
.def totalRound = r19;   total round, is 40 (in key generation ) or 44 (in encryption)
.def currentZ = r20;     the current byte value of z, the 62 bits of z is stored in 8 bytes
.def sixteen = r21;
.def bitofz = r0;        one bit of z is store in the register

.def constC0 = r22;      the lowest byte const value c
.def constC1 = r23;
.def constC2 = r24;
.def constC3 = r25;      the highest byte const value c

 .DSEG ; RAM( data segment)
	plainText: .byte 8 ; the 8 bytes of plaintext, from low byte to high byte.
	cipherText: .byte 8; the 8 bytes of ciphertext, from low byte to high byte
	keys: .byte 176 ;    the 44*4 bytes of round keys

 .CSEG ; Flash( code segement)
	/*
	 * the main function
	 */
main:
	; load the initial keys from Flash to RAM
	clr currentRound;
	ldi sixteen, 16;
	ldi r26, low(keys);
	ldi r27, high(keys);
	ldi r30, low(initialKeys<<1);
	ldi r31, high(initialKeys<<1);
loadInitialKeys:
	lpm r0, z+;
	st x+, r0;
	inc currentRound;
	cp currentRound, sixteen
	brne loadInitialKeys;

	; the const value of c
	ldi constC0, 0xfc;
	ldi constC1, 0xff;
	ldi constC2, 0xff;
	ldi constC3, 0xff;

	nop;
	rcall keySchedule;
	ret;

	/*
	 * Subroutine: keySchedule
	 * Function:   compute the sub keys.
	 */
keySchedule:
	ldi r26, low(keys);
	ldi r27, high(keys);
	ldi r30, low(constZ<<1);
	ldi r31, high(constZ<<1);
	lpm currentZ, z+;

	clr zero;
	clr remain8;
	clr currentRound     ; the current round number
	ldi totalRound, 40   ; the total loop times, it is 40 other than 44 because the first 4 is initialized
	ldi eight, 8;
	ldi four, 4;
	ldi sixteen, 16      ; one round after key generation, X point should sub 16(from k(i+5) to k(i+1))
	clr bitofz           ;initialize the value to 0

keysExtend:
	; load k(i)
	ld r2, x+;
	ld r3, x+;
	ld r4, x+;
	ld r5, x+;
	; load k(i+1)
	ld r10, x+;
	ld r11, x+;
	ld r12, x+;
	ld r13, x+;
	; S(-3)k(i+3)
	add r26, four;
	adc r27, zero; 
	ld r6, x+;
	ld r7, x+;
	ld r8, x+;
	ld r9, x+;
	lsr r9;
	ror r8;
	ror r7;
	bst r6, 0;
	ror r6;
	bld r9, 7;
	lsr r9;
	ror r8;
	ror r7;
	bst r6, 0;
	ror r6;
	bld r9, 7;
	lsr r9;
	ror r8;
	ror r7;
	bst r6, 0;
	ror r6;
	bld r9, 7;
	;k(i+1) eor S(-3)k(i+3)
	eor r6, r10;
	eor r7, r11;
	eor r8, r12;
	eor r9, r13;
	;S(-1)( k(i+1) eor S(-3)k(i+3) ), T: shift right with 1 bit, not left
	movw r10, r6;
	movw r12, r8;
	lsr r13;
	ror r12;
	ror r11;
	bst r10, 0;
	ror r10;
	bld r13, 7;
	; (I eor S(-1)) ( k(i+1) eor S(-3)k(i+3) )
	eor r6, r10;
	eor r7, r11;
	eor r8, r12;
	eor r9, r13;
	; k(i) eor (I eor S(-1)) ( k(i+1) eor S(-3)k(i+3) )
	eor r2, r6;
	eor r3, r7;
	eor r4, r8;
	eor r5, r9;
	; k(i) eor (I eor S(-1)) ( k(i+1) eor S(-3)k(i+3) ) eor c
	eor r2, constC0;
	eor r3, constC1;
	eor r4, constC2;
	eor r5, constC3;
	;k(i) = k(i) + k(i+1) + S(-1)k(i+1) + S(-4)k(i+3) + S(-3)k(i+3) + c + z(i)
	lsl currentZ;
	adc bitofz, zero; the highest bit of currentZ is in flag
	eor r2, bitofz;
	clr bitofz; set bitofz to 0, it will be used next round
	;k(i+4), is just [r3,r2,r1,r0]
	st x+, r2;
	st x+, r3;
	st x+, r4;
	st x+, r5;
	; set x to the position of k(i+1),should sub 16 not 12 
	sub r26, sixteen; if r26 < 16, the C bit will be 1
	sbc r27, zero; R27 = R27 - zero - C, because r16 can not be smaller than 16
	;have finished?
	inc currentRound;
	inc remain8;
	cp remain8, eight;
	breq continue;;if the remain8 = 8( the current round is the beisu of 8, such as 8,16,24,32,40)
	jmp keysExtend;
continue:
	clr remain8; start with 0 again
	lpm currentZ,z+;
	cp currentRound, totalRound;
	breq end;
	jmp keysExtend;
end:
	ret;

initialKeys: 
.db 0x00, 0x01, 0x02, 0x03, 0x08, 0x09, 0x0a, 0x0b, 0x10, 0x11, 0x12, 0x13, 0x18, 0x19, 0x1a, 0x1b;

; 1101 1011, 1010 1100, 0110 0101, 1110 0000, 0100 1000, 1010 0111, 0011 0100, 0011 1100
constZ: 
.db 0xdb, 0xac, 0x65, 0xe0, 0x48, 0xa7, 0x34, 0x3c ;