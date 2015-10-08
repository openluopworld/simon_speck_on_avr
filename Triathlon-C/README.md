# Instruction Set
C(Carry); V(overflow); N(negative); and Z(zero)<br>
1. Test Only uses the C implementation.<br>

# MSP430
&nbsp;&nbsp;Brief Introduction: 16 16-bit register. Four of the registers are dedicated to program counter(r0 or pc), stack point(r1 or sp), status register(r2 or sr/cg1) and constant generator(r3 or cg2), while the remaining 12 registers(r4-r15) are general-purpose registers. There are 52 instructions in total.<br><br>

1. A byte instruction with a register destination clears the high 8 bits of the register to 0.<sup>[1]</sup><br>
&nbsp;&nbsp;For example, <b>"mov.b r6, r12"</b> means move the lower 8 bits of r6 to r12,<br>
&nbsp;&nbsp;and the higher 8 bits of r12 is set to 0.<br>

2. <b>"bit src, dest"</b> : src & dest; Sets status only, the destination is not written.<sup>[2]</sup><br>

3. Logical instructions set C to the opposite of Z (C is set if the result is NOT zero), and clear V to 0.<sup>[3]</sup><br>
&nbsp;&nbsp;For example: the following instructions rotate shift right [r9:r8] by 1 bit<br>
&nbsp;&nbsp;<b>bit	#1,	r8;</b><br>
&nbsp;&nbsp;<b>rrc	r9;</b><br>
&nbsp;&nbsp;<b>rrc	r8;</b><br>

4. <b>mov(.b) @rs+, rd</b> : Indirect autoincrement. The operand is in memory at the address held in rs, then the register rs is incremented by 1(operation in byte) or 2(operation in word).<sup>[4]</sup><br>
&nbsp;&nbsp;For example: load the plain text from RAM(r15 stores the start address) to register[r7,r6,r5,r4]<br>
&nbsp;&nbsp;<b>mov    @r15+,    r4;</b><br>
&nbsp;&nbsp;<b>mov    @r15+,    r5;</b><br>
&nbsp;&nbsp;<b>mov    @r15+,    r6;</b><br>
&nbsp;&nbsp;<b>mov    @r15+,    r7;</b><br>

5. Instructions in MSP is different with other microcontrollers.<br>
&nbsp;&nbsp;<b>The first register is the source, the second is the destination.</b>
&nbsp;&nbsp;For example, <b>"mov r5, r4"</b> means moving r5 to r4.

6. The byte of higher address is stored in the higher 8 bits of register.(???)<br>

7. ^ is <b>xor</b> in MSP, but <i>eor</i> is AVR.<br>

Refs<br>
[1] <a href="http://mspgcc.sourceforge.net/manual/x214.html" target="_blank">mov.b rs, rd</a><br>
[2] <a href="http://mspgcc.sourceforge.net/manual/x223.html" target="_blank">bit rs, rd</a><br>
[3] <a href="http://mspgcc.sourceforge.net/manual/x82.html" target="_blank">bit	#1,	r8</a><br>
[4] <a href="http://www.ece.utep.edu/courses/web3376/Links_files/MSP430%20Quick%20Reference.pdf" target="_blank">mov(.b) @rs+, rd</a><br>

# AVR
&nbsp;&nbsp;Most of the 133 instructions require a single cycle to execute. The rich instruction set in combimed with the 32 8-bit general purpose registers(r0-r31) with single clock access time. Six of the 32 8-bit registers can be used as three 16-bit indirect register pointers(X, r26-r27; Y, r28-r29; and Z, r30-r31) for addressing the data space.<br><br>

1. Instruction <b>ldi r26, low(key)</b> and <b>ldi r27, high(key)</b> can not be used in assemble c. It should be like this <b>ldi r26, lo8(key)</b> and <b>ldi r27, hi8(key)</b>.<br>

2. Despite using <i>#include"constants.h"</i>, some const values, such as KEY_SIZE, NUMBER_OF_ROUNDS and so on, can still not be used directly. Therefore, immediate numbers are used.<br>

3. The second operand of <b>adiw"</b> is [0, 63]. <br>
Therefore, <b>adiw r18, 176</b> is wrong(operand is out of range). It can be replaced by:<br>
&nbsp;&nbsp;<b>adiw r28, 63;&nbsp;&nbsp;<b>
&nbsp;&nbsp;<b>adiw r28, 63;</b><br>
&nbsp;&nbsp;<b>adiw r28, 50;</b><br>

# ARM
1. <b>mov r4, #0xdbac65e0</b> gives the error message "<a href="http://stackoverflow.com/questions/10261300/invalid-constant-after-fixup" target="_blank">invalid constant (dbac65e0) after fieup</a>"<br>
The following instructions can implement it:<br>
&nbsp;&nbsp;<b>mov r4, #0xdb</b><br>
&nbsp;&nbsp;<b>lsl r4, #8</b><br>
&nbsp;&nbsp;<b>eor r4, r4, #0xac</b><br>
&nbsp;&nbsp;<b>lsl r4, #8</b><br>
&nbsp;&nbsp;<b>eor r4, r4, #0x65</b><br>
&nbsp;&nbsp;<b>lsl r4, #8</b><br>
&nbsp;&nbsp;<b>eor r4, r4, #0xe0</b><br>
