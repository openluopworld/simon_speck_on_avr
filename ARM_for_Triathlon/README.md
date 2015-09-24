# Instruction Set
C(Carry); V(overflow); N(negative); and Z(zero)

# AVR

# MSP430
&nbsp;&nbsp;Brief Introduction: 16 16-bit register. Four of the registers are dedicated to program counter(r0 or pc), stack point(r1 or sp), status register(r2 or sr/cg1) and constant generator(r3 or cg2), while the remaining 12 registers(r4-r15) are general-purpose registers. There are 52 instructions in total.<br><br>

1. A byte instruction with a register destination clears the high 8 bits of the register to 0.<sup>[1]</sup><br>
&nbsp;&nbsp;For example, <font color="red">"mov.b r6, r12"<font> means move the lower 8 bits of r6 to r12,<br>
&nbsp;&nbsp;and the higher 8 bits of r12 is set to 0.<br>

2. <font color="red">bit src, dest</font> : src & dest; Sets status only, the destination is not written.<sup>[2]</sup><br>

3. Logical instructions set C to the opposite of Z (C is set if the result is NOT zero), and clear V to 0.<sup>[3]</sup><br>
&nbsp;&nbsp;For example: the following instructions rotate shift right [r9:r8] by 1 bit<br>
&nbsp;&nbsp;<font color="red">bit	#1,	r8;</font><br>
&nbsp;&nbsp;<font color="red">rrc	r9;</font><br>
&nbsp;&nbsp;<font color="red">rrc	r8;</font><br>

4. <font color="red">mov(.b) @rs+, rd</font> : Indirect autoincrement. The operand is in memory at the address held in rs, then the register rs is incremented by 1(operation in byte) or 2(operation in word).<sup>[4]</sup><br>
&nbsp;&nbsp;For example: load the plain text from RAM(r15 stores the start address) to register[r7,r6,r5,r4]<br>
&nbsp;&nbsp;<font color="red">mov    @r15+,    r4;</font><br>
&nbsp;&nbsp;<font color="red">mov    @r15+,    r5;</font><br>
&nbsp;&nbsp;<font color="red">mov    @r15+,    r6;</font><br>
&nbsp;&nbsp;<font color="red">mov    @r15+,    r7;</font><br>

5. The byte of higher address is stored in the higher 8 bits of register.(???)<br>

Refs
[1] <a href="http://mspgcc.sourceforge.net/manual/x214.html">mov.b rs, rd</a><br>
[2] <a href="http://mspgcc.sourceforge.net/manual/x223.html">bit rs, rd</a><br>
[3] <a href="http://mspgcc.sourceforge.net/manual/x82.html">bit	#1,	r8</a><br>
[4] <a href="http://www.ece.utep.edu/courses/web3376/Links_files/MSP430%20Quick%20Reference.pdf">mov(.b) @rs+, rd</a><br>

# ARM
