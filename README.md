# Simon-Speck

# C++
在vs2010平台上实现<br>
C++_1是最初的实现代码，不知道有移位指令，因此每一个bit用一个int代替，执行效率很低<br><br>
C++_2是经过改进后的C++代码，通过C++中的左移和右移运算符代替了最初的方法<br><br>
C++_3 is a more efficient and simpler implementation of Simon. Speck has not been implemented yet.<br>
Just change the value of 'SIMON_BLOCK_SIZE' and 'SIMON_KEY_WORDS', then run it. The pair of value can be (64, 3), (64, 4), (128, 2).<br>

# AVR
在Atmel Studio平台实现<br>
主要实现Simon64/128和Speck64/128，分别针对Minimal RAM和High-Throughput两种类型<br>
参考资料：《The Simon and Speck Block Ciphers on AVR 8-bit Microcontrollers》
