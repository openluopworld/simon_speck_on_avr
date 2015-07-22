# Simon-Speck

# C++
Implement on VS2010<br><br>
C++_1 is the earliest implementation. Without knowing the << and >> operation in C++, an int is used to stand a bit. Therefore, it is very inefficient<br><br>
C++_2 uses << and >> operation to implement rotate shift left and rotate shift right.<br>
For example, we can use '(X<<n)|(X>>(32-n))' to implement 'rotate shift left X with n bits' on the assumption that X is an unsigned int.<br><br>
C++_3 is a more efficient and simpler implementation of Simon. Speck has not been implemented yet.<br>
Just change the value of 'SIMON_BLOCK_SIZE' and 'SIMON_KEY_WORDS', then run it. The pair of values can be (64, 3), (64, 4), (128, 2).<br><br>

# AVR
Implement on Atmel Studio<br><br>
Simon64/128 and Speck64/128 are implemented on Minimal RAM and High-Throughput condition.<br>
References:《The Simon and Speck Block Ciphers on AVR 8-bit Microcontrollers》.
