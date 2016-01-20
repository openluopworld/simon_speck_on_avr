#ifndef CONST_H
#define CONST_H

/*
 * 外部变量的声明
 * 声明并没有建立变量或者为它们分配存储空间
 */
extern int SIMON_BLOCK_SIZE;
extern int SIMON_KEY_SIZE;
extern int SIMON_WORD_SIZE;
extern int SIMON_KEY_WORDS;
extern int SIMON_SEQUENCE_NUMBER;
extern int SIMON_ROUNDS;

extern int SPECK_BLOCK_SIZE;
extern int SPECK_KEY_SIZE;
extern int SPECK_WORD_SIZE;
extern int SPECK_KEY_WORDS;
extern int SPECK_ROT_A;
extern int SPECK_ROT_B;
extern int SPECK_ROUNDS; 

#endif