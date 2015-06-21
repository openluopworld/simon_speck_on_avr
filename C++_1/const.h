#ifndef CONST_H
#define CONST_H

//#include<iostream>：不是最顶层定义的，所以不需要增加iostrean库

/*
 * Simon明文长度
 */
extern int SIMON_BLOCK_SIZE;

/*
 * Simon密钥长度
 */
extern int SIMON_KEY_SIZE;

/*
 * Simon字大小
 */
extern int SIMON_WORD_SIZE;

/*
 * Simon密钥字的个数
 */
extern int SIMON_KEY_WORDS;

/*
 * Simon对应版本Z的数字
 */
extern int SIMON_SEQUENCE_NUMBER;

/*
 * 加解密轮数
 */
extern int SIMON_ROUNDS;


/*
 * Speck块大小
 */
extern int SPECK_BLOCK_SIZE;

/*
 * Speck密钥大小
 */
extern int SPECK_KEY_SIZE;

/*
 * Speck字大小
 */
extern int SPECK_WORD_SIZE;

/*
 * Speck密钥的字数
 */
extern int SPECK_KEY_WORDS;

/*
 * Speck中移位变量a
 */
extern int SPECK_ROT_A;

/*
 * Speck中移位变量b
 */
extern int SPECK_ROT_B;

/*
 * Speck加解密轮数
 */
extern int SPECK_ROUNDS; 

#endif