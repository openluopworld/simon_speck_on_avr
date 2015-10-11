# Triathlon-Only-AVR
&nbsp;&nbsp;<b>r0-r7 stores the cipher text after encryption, so there is no need to MOV cipher text to r16-r23(stores the init vector). The next block can just be loaded to r16-r23, then eor r0-r7, r17-r23. Encrypt r0-r7 again.</b>
