/*
 * Simon64_128_HighThroughput.asm
 *
 * A High-Throughput/ Low-Energy Implementation of SIMON
 * Keys are stored in RAM, unroll 4 rounds.
 *
 *  Created: 2015/6/2 23:24:09
 *   Author: LuoPeng
 */ 

.def zero = r14; the adc operation
.def remain8 = r15; currentRound % 8
.def eight = r16; control when to load a new z to register
.def four = r17; from k(i+1) to k(i+3), the register should add 4

.def currentRound = r18; the current round [0,39]
.def totalRound = r19; total round, is 40 (in key generation ) or 44 (in encryption)
.def currentZ = r20; the current byte value of z, the 62 bits of z is stored in 8 bytes
.def bitofz = r0; one bit of z is store in the register
.def sixteen = r21;

.def constC0 = r22; the lowest byte const value c
.def constC1 = r23;
.def constC2 = r24;
.def constC3 = r25; the highest byte const value c

 .DSEG ; RAM( data segment)
	plainText: .byte 8 ; the 8 bytes of plaintext, from low byte to high byte.
	cipherText: .byte 8 ; the 8 bytes of ciphertext, from low byte to high byte.
	constZ: .byte 8; 
	keys: .byte 176 ; the 44*4 bytes of round keys

 .CSEG ; Flash( code segement)
	initialKeys: .db 0x00, 0x01, 0x02, 0x03, 0x08, 0x09, 0x0a, 0x0b, 0x10, 0x11, 0x12, 0x13, 0x18, 0x19, 0x1a, 0x1b;

start:

	; load the initial keys from Flash to RAM
	clr currentRound;
	ldi sixteen, 16;
	// load the initial keys
	ldi r26, low(keys);
	ldi r27, high(keys);
	ldi r30, low(initialKeys);
	ldi r31, high(initialKeys);
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

	// load const z
	ldi r28, low(constZ);
	ldi r29, high(constZ);
	ldi currentRound, 0xdb; 1101 1011
	st y+, currentRound;
	ldi currentRound, 0xac; 1010 1100
	st y+, currentRound;
	ldi currentRound, 0x65; 0110 0101
	st y+, currentRound;
	ldi currentRound, 0xe0; 1110 0000
	st y+, currentRound;
	ldi currentRound, 0x48; 0100 1000
	st y+, currentRound;
	ldi currentRound, 0xa7; 1010 0111
	st y+, currentRound;
	ldi currentRound, 0x34; 0011 0100
	st y+, currentRound;
	ldi currentRound, 0x3c; 0011 1100
	st y+, currentRound;

	ldi r26, low(keys);
	ldi r27, high(keys);
	ldi r28, low(constZ);
	ldi r29, high(constZ);
	ld currentZ, y+;

	clr zero;
	clr remain8;
	clr currentRound; the current round number
	ldi totalRound, 40; the total loop times, it is 40 other than 44 because the first 4 is initialized
	ldi eight, 8;
	ldi four, 4;
	ldi sixteen, 16; one round after key generation, X point should sub 16(from k(i+5) to k(i+1))
	clr bitofz;initialize the value to 0

keysExtend:
;	rcall getOneKey;without the subroutine, there is an error named "Relative branch out of reach"
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
	;adc r27, zero; the total byte is 200, so the value of r26 can not be bigger than 256
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
	;sbc r27, zero; R27 = R27 - zero - C, because r16 can not be smaller than 16
	;have finished?
	inc currentRound;
	inc remain8;
	cp remain8, eight;
	breq continue;;if the remain8 = 8( the current round is the beisu of 8, such as 8,16,24,32,40)
	jmp keysExtend;
continue:
	clr remain8; start with 0 again
	ld currentZ,y+;
	cp currentRound, totalRound;
	breq encryption;
	jmp keysExtend;
	nop;
;	ret;

encryption:

	; initialze the plaintext
	ldi r26, low(plainText) ; r26 is the low byte of X
	ldi r27, high(plainText) ; r27 is the high byte of X
	ldi r28, 0x65; suppose the plaintext is 0x656b696c 20646e75;
	st x+, r28;	
	ldi r28, 0x6b;
	st x+, r28;
	ldi r28, 0x69;
	st x+, r28;
	ldi r28, 0x6c;
	st x+, r28;
	ldi r28, 0x20;
	st x+, r28;
	ldi r28, 0x64;
	st x+, r28;
	ldi r28, 0x6e;
	st x+, r28;
	ldi r28, 0x75;
	st x+, r28;

	; load the plaintext from RAM to registers [r7,...,r0], X = [r7, r6, r5, r4], Y = [r3, r2, r1, r0]
	ldi r26, low(plainText) ;
	ldi r27, high(plainText) ;
	ld r0, x+ ; the lowest byte
	ld r1, x+ ;
	ld r2, x+ ;
	ld r3, x+ ;
	ld r4, x+ ;
	ld r5, x+ ;
	ld r6, x+ ;
	ld r7, x+ ;

	ldi r30, low(keys) ; z is the current address of keys
	ldi r31, high(keys) ;
	; initialize before encryption
	clr currentRound ; set 0, have done rounds
	ldi totalRound, 11 ; the total rounds
loop:
	; get the sub key k
	ld r8, z+ ; store the 4 bytes of sub key to K = [r11, r10, r9, r8]
 	ld r9, z+ ;
	ld r10, z+ ;
	ld r11, z+ ;
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
	; move x to t, t stands for [r25, r24, r23, r22]
	movw r22, r4;
	movw r24, r6;
	; t & S8(y)
	and r22, r3;
	and r23, r0;
	and r24, r1;
	and r25, r2;
	; x = S2(x)
	lsl r4;
	rol r5;
	rol r6;
	rol r7;
	adc r4, zero;
	; x = x eor t
	eor r4, r22;
	eor r5, r23;
	eor r6, r24;
	eor r7, r25;
	; x = x eor k
	eor r4, r8;
	eor r5, r9;
	eor r6, r10;
	eor r7, r11;
	
	// loop 2
	; get the sub key k
	ld r8, z+ ; store the 4 bytes of sub key to K = [r11, r10, r9, r8]
 	ld r9, z+ ;
	ld r10, z+ ;
	ld r11, z+ ;
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
	; move x to t, t stands for [r25, r24, r23, r22]
	movw r22, r4;
	movw r24, r6;
	; t & S8(y)
	and r22, r3;
	and r23, r0;
	and r24, r1;
	and r25, r2;
	; x = S2(x)
	lsl r4;
	rol r5;
	rol r6;
	rol r7;
	adc r4, zero;
	; x = x eor t
	eor r4, r22;
	eor r5, r23;
	eor r6, r24;
	eor r7, r25;
	; x = x eor k
	eor r4, r8;
	eor r5, r9;
	eor r6, r10;
	eor r7, r11;

	// loop 3
	; get the sub key k
	ld r8, z+ ; store the 4 bytes of sub key to K = [r11, r10, r9, r8]
 	ld r9, z+ ;
	ld r10, z+ ;
	ld r11, z+ ;
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
	; move x to t, t stands for [r25, r24, r23, r22]
	movw r22, r4;
	movw r24, r6;
	; t & S8(y)
	and r22, r3;
	and r23, r0;
	and r24, r1;
	and r25, r2;
	; x = S2(x)
	lsl r4;
	rol r5;
	rol r6;
	rol r7;
	adc r4, zero;
	; x = x eor t
	eor r4, r22;
	eor r5, r23;
	eor r6, r24;
	eor r7, r25;
	; x = x eor k
	eor r4, r8;
	eor r5, r9;
	eor r6, r10;
	eor r7, r11;

	// loop 4
	; get the sub key k
	ld r8, z+ ; store the 4 bytes of sub key to K = [r11, r10, r9, r8]
 	ld r9, z+ ;
	ld r10, z+ ;
	ld r11, z+ ;
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
	; move x to t, t stands for [r25, r24, r23, r22]
	movw r22, r4;
	movw r24, r6;
	; t & S8(y)
	and r22, r3;
	and r23, r0;
	and r24, r1;
	and r25, r2;
	; x = S2(x)
	lsl r4;
	rol r5;
	rol r6;
	rol r7;
	adc r4, zero;
	; x = x eor t
	eor r4, r22;
	eor r5, r23;
	eor r6, r24;
	eor r7, r25;
	; x = x eor k
	eor r4, r8;
	eor r5, r9;
	eor r6, r10;
	eor r7, r11;

	; have finished?
	inc currentRound;
	cp currentRound, totalRound;
	breq storeCipher;
	jmp loop; if currentRound is not equal to totalRound, encryption again
storeCipher:
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