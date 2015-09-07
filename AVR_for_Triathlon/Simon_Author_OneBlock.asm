/*
 * Simon_Author_OneBlock.asm
 *
 *  Created: 2015/6/28 21:31:01
 *   Author: LuoPeng
 */ 

.def zero = r17;

.def currentRound = r18;
.def totalRound = r19;   total round, is 40 (in key generation ) or 44 (in encryption)
.def currentZ = r20;     the current byte value of z, the 62 bits of z is stored in 8 bytes
.def remain8 = r21;

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
	ldi r26, low(keys);
	ldi r27, high(keys);
	ldi r30, low(initialKeys<<1);
	ldi r31, high(initialKeys<<1);
loadInitialKeys:
	lpm r0, z+;
	st x+, r0;
	inc currentRound;
	cpi currentRound, 16
	brne loadInitialKeys;

	; the const value of c
	ldi constC0, 0xfc;
	ldi constC1, 0xff;
	ldi constC2, 0xff;
	ldi constC3, 0xff;

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
	clr remain8;
	clr currentRound     ; the current round number
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
	adiw r26, 4; adiw rd, K ==== rd+1:rd <- rd+1:rd + K
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
	; k(i) eor [k(i+1) eor S(-3)k(i+3)]
	eor r2, r6;
	eor r3, r7;
	eor r4, r8;
	eor r5, r9;
	; S(-1)[k(i+1) eor S(-3)k(i+3)]
	lsr r9;
	ror r8;
	ror r7;
	bst r6, 0;
	ror r6;
	bld r9, 7;
	; k(i) eor [k(i+1) eor S(-3)k(i+3)] eor S(-1)[k(i+1) eor S(-3)k(i+3)]
	eor r2, r6;
	eor r3, r7;
	eor r4, r8;
	eor r5, r9;
	; k(i) eor [k(i+1) eor S(-3)k(i+3)] eor S(-1)[k(i+1) eor S(-3)k(i+3)] + c + z(i)
	;bld currentZ, 7;
	;bst constC0, 0;
	bst currentZ, 7; bst Rr, b ==== T <- Rr(b). bst is "bit store from register to T"
	bld constC0, 0;  bld Rr, b ==== Rr(b) <- T. bld is "bit load from T to register"
	lsl currentZ;
	eor r2, constC0;
	eor r3, constC1;
	eor r4, constC2;
	eor r5, constC3;
	;k(i+4), is just [r3,r2,r1,r0]
	st x+, r2;
	st x+, r3;
	st x+, r4;
	st x+, r5;
	; set x to the position of k(i+1),should sub 16 not 12 
	sbiw r26, 16; sbiw rd, K ==== rd+1:rd <- rd+1:rd - K
	;have finished?
	inc currentRound;
	inc remain8;
	cpi remain8, 8;
	breq continue;;if the remain8 = 8( the current round is the beisu of 8, such as 8,16,24,32,40)
	jmp keysExtend;
continue:
	clr remain8; start with 0 again
	lpm currentZ,z+;
	cpi currentRound, 40;
	breq end;
	jmp keysExtend;
end:
	;ret;

	/*
	 * Subroutine: encryption
	 * Function:   A minimal RAM Implementation without key schedule of SIMON64/128
	 * 
	 * The sub keys are stored in Flash. The sub keys are transferred to RAM before encryption
	 * Unroll four rounds
	 */
encryption:
	ldi r26, low(plainText) ;
	ldi r27, high(plainText) ;
	; load the plaintext from RAM to registers [r7,...,r0], X = [r7, r6, r5, r4], Y = [r3, r2, r1, r0]
	ld r7, x+ ;
	ld r6, x+ ;
	ld r5, x+ ;
	ld r4, x+ ;
	ld r3, x+ ;
	ld r2, x+ ;
	ld r1, x+ ;
	ld r0, x+ ;
	
	clr currentRound ; set 0, have done rounds ; 1 cycle
	clr zero; 1 cycle
	ldi r28, low(keys) ; y is the current address of keys
	ldi r29, high(keys) ;
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
	inc currentRound;
	cpi currentRound, 44;
	brne loop;
storecipher:
	; load the ciphertext from registers [r7,...,r0] to RAM
	st x+, r7;
	st x+, r6;
	st x+, r5;
	st x+, r4;
	st x+, r3;
	st x+, r2;
	st x+, r1;
	st x+, r0;

	ret;

initialKeys: 
.db 0x00, 0x01, 0x02, 0x03, 0x08, 0x09, 0x0a, 0x0b, 0x10, 0x11, 0x12, 0x13, 0x18, 0x19, 0x1a, 0x1b;

; 1101 1011, 1010 1100, 0110 0101, 1110 0000, 0100 1000, 1010 0111, 0011 0100, 0011 1100
constZ: 
.db 0xdb, 0xac, 0x65, 0xe0, 0x48, 0xa7, 0x34, 0x3c ;