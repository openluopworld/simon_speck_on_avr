# Simon-Speck

# C++
Implement on VS2010<br>
C++_1: Without knowing the << and >> operation in C++, an int is used to stand a bit. Therefore, it is very inefficient<br><
C++_2 uses << and >> operation to implement rotate shift left and rotate shift right.<br>
For example, we can use '(X<<n)|(X>>(32-n))' to implement 'rotate shift left X with n bits' on the assumption that X is an unsigned int.<br><
C++_3 is a more efficient and simpler implementation of Simon. Speck has not been implemented yet.<br>
Just change the value of 'SIMON_BLOCK_SIZE' and 'SIMON_KEY_WORDS', then run it. The pair of values can be (64, 3), (64, 4), (128, 2).

# AVR
Implement on Atmel Studio<br>
Simon64/128 and Speck64/128 are implemented on Minimal RAM and High-Throughput condition<sup>[1]</sup>.<br>
Some instructions such as <b>adiw</b> and <b>cpi</b> can be used to reduce the number of code.<br>

# Triathlon-Only-AVR
Implement on Atmel Studio<br>
Two Scenarios: 1)128 bytes in CBC mode; 2) 128 bits in CTR mode<sup>[2,3]</sup><br>

# Triathlon-C
Two scenarios, each with 3 platfrom.

# References
[1] The Simon and Speck Block Ciphers on AVR 8-bit<br>
[2] Triathlon of Lightweight Block Ciphers for the Internet of Things<br>
[3] FELICS - Fair Evaluation of Lightweight Cryptographic Systems<br>
[4] <a href="http://perso.uclouvain.be/fstandae/lightweight_ciphers/">http://perso.uclouvain.be/fstandae/lightweight_ciphers/</a><br>
