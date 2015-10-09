
#include"const.h"

/*
 * 外部变量的定义
 * 变量定义才分配存储空间
 * 外部变量的初始化只能出现在其定义中
 */
int SIMON_BLOCK_SIZE;       // block size
int SIMON_KEY_SIZE;         // key size
int SIMON_WORD_SIZE;        // word size ( = SIMON_BLOCK_SIZE/2 )
int SIMON_KEY_WORDS;        // the words of input key ( = SIMON_KEY_SIZE/SIMON_WORD_SIZE )
int SIMON_SEQUENCE_NUMBER;  // the number of const Z(0, 1, 2, 3 or 4)
int SIMON_ROUNDS;           // encryption and decryption rounds

int SPECK_BLOCK_SIZE;       // block size
int SPECK_KEY_SIZE;         // key size
int SPECK_WORD_SIZE;        // word size ( = SPECK_BLOCK_SIZE/2 )
int SPECK_KEY_WORDS;        // the words of input key ( = SPECK_KEY_SIZE/SPECK_WORD_SIZE )
int SPECK_ROT_A;            // the rotate shift parameter a
int SPECK_ROT_B;            // the rotate shift parameter b
int SPECK_ROUNDS;           // encryption and decryption rounds