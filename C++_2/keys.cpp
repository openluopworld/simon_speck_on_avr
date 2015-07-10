
#include"keys.h"
#include"keyZ.h"
#include"const.h"
#include<iostream>
using namespace std;

/*
 * Simon：计算密钥，字大小为32
 * inputKey：初始的密钥
 * keys：计算后得到的每轮密钥
 */
void setSimonKeys32 ( unsigned int * inputKey, unsigned int * keys ) {

	/*
	 * 算法中的常数c，大小为2^n - 4，其中n是字的长度，即SIMON_WORD_SIZE
	 * 转化为二进制，即最低两位为0，其它位全为1
	 */
	unsigned int c = 0xfffffffc;

	int i;
	for ( i = 0; i < SIMON_KEY_WORDS; i++ )  {
		keys[i] = inputKey[i];
	}

	/*
	 * 求解后面轮的密钥
	 * 先求其它的异或，最后求Zji，如果为1则对最低位进行修改，否则不变
	 */
	if ( SIMON_KEY_WORDS == 3 ) {
		for ( i = 0; i < SIMON_ROUNDS-SIMON_KEY_WORDS; i++ ) {
			keys[i+SIMON_KEY_WORDS] = c ^ keys[i] ^
							((keys[i+2]>>3) | (keys[i+2]<<(SIMON_WORD_SIZE-3))) ^
							((keys[i+2]>>4) | (keys[i+2]<<(SIMON_WORD_SIZE-4)));
			// SIMON_WORD_SIZE为32时，无论SIMON_KEY_WORDS为3还是4，周期都是62
			if ( z[SIMON_SEQUENCE_NUMBER][i%62] == 1 ) {
				keys[i+SIMON_KEY_WORDS] ^=  0x1;
			}
		}
	} else if ( SIMON_KEY_WORDS == 4 ) {
		// int cycle = (SIMON_SEQUENCE_NUMBER == 0 || SIMON_SEQUENCE_NUMBER == 1)?31:62;
		unsigned int temp = 0x00000000;
		for ( i = 0; i < SIMON_ROUNDS-SIMON_KEY_WORDS; i++ ) {
			temp = ((keys[i+3]>>3) | (keys[i+3]<<(SIMON_WORD_SIZE-3))) ^ keys[i+1];
			keys[i+SIMON_KEY_WORDS] = c ^ keys[i] ^ temp ^ ((temp>>1) | (temp<<(SIMON_WORD_SIZE-1)));
			if ( z[SIMON_SEQUENCE_NUMBER][i%62] == 1 ) {
				keys[i+SIMON_KEY_WORDS] ^=  0x00000001;
			}
		}
	}

}

/*
 * Simon：计算密钥，字大小为64
 * inputKey：初始的密钥
 * keys：计算后得到的每轮密钥
 */
void setSimonKeys64 ( unsigned long long * inputKey, unsigned long long * keys ) {

	/*
	 * 算法中的常数c，大小为2^n - 4，其中n是字的长度，即SIMON_WORD_SIZE
	 * 转化为二进制，即最低两位为0，其它位全为1
	 */
	unsigned long long c = 0xfffffffffffffffc;

	int i;
	for ( i = 0; i < SIMON_KEY_WORDS; i++ )  {
		keys[i] = inputKey[i];
	}

	/*
	 * 求解后面轮的密钥
	 * 先求其它的异或，最后求Zji，如果为1则对最低位进行修改，否则不变
	 */
	if ( SIMON_KEY_WORDS == 2 ) {
		for ( i = 0; i < SIMON_ROUNDS-SIMON_KEY_WORDS; i++ ) {
			/*
			 * 没有循环移位，可以通过分别向左、向右移位再或得到
			 */
			keys[i+SIMON_KEY_WORDS] = c ^ keys[i] ^ 
									((keys[i+1]>>3) | (keys[i+1]<<(SIMON_WORD_SIZE-3))) ^ 
									((keys[i+1]>>4) | (keys[i+1]<<(SIMON_WORD_SIZE-4)));
			/*
			 * 最后与z[j][i]进行异或运算，当SIMON_KEY_WORDS确定时，j也是确定的
			 */
			if ( z[SIMON_SEQUENCE_NUMBER][i%62] == 1 ) {
				keys[i+SIMON_KEY_WORDS] ^= 0x1 ;
			}
		}
	} else if ( SIMON_KEY_WORDS == 3 ) {
		for ( i = 0; i < SIMON_ROUNDS-SIMON_KEY_WORDS; i++ ) {
			keys[i+SIMON_KEY_WORDS] = c ^ keys[i] ^
							((keys[i+2]>>3) | (keys[i+2]<<(SIMON_WORD_SIZE-3))) ^
							((keys[i+2]>>4) | (keys[i+2]<<(SIMON_WORD_SIZE-4)));
			// SIMON_WORD_SIZE为32时，无论SIMON_KEY_WORDS为3还是4，周期都是62
			if ( z[SIMON_SEQUENCE_NUMBER][i%62] == 1 ) {
				keys[i+SIMON_KEY_WORDS] ^=  0x1;
			}
		}
	} else if ( SIMON_KEY_WORDS == 4 ) {
		// int cycle = (SIMON_SEQUENCE_NUMBER == 0 || SIMON_SEQUENCE_NUMBER == 1)?31:62;
		unsigned long long temp = 0x0123456789abcdef;
		for ( i = 0; i < SIMON_ROUNDS-SIMON_KEY_WORDS; i++ ) {
			temp = ((keys[i+3]>>3) | (keys[i+3]<<(SIMON_WORD_SIZE-3))) ^ keys[i+1];
			keys[i+SIMON_KEY_WORDS] = c ^ keys[i] ^ temp ^ ((temp>>1) | (temp<<(SIMON_WORD_SIZE-1)));
			if ( z[SIMON_SEQUENCE_NUMBER][i%62] == 1 ) {
				keys[i+SIMON_KEY_WORDS] ^=  0x1;
			}
		}
	}

}

/*
 * Speck：计算密钥
 * inputKey：初始的密钥
 * keys：计算后得到的每轮密钥
 */
void setSpeckKeys32 ( unsigned int * inputKey, unsigned int * keys ) {
	int i;
	/*
	 * inputKey = (L(m-2), ...,L(0),keys(0));则L的轮数比keys多m-2(即SPECK_KEY_WORDS-2)
	 * keys的轮数是SPECK_ROUNDS，则L的轮数为SPECK_ROUNDS+SPECK_KEY_WORDS-2
	 */
	unsigned int * L = new unsigned int[SPECK_ROUNDS+SPECK_KEY_WORDS-2];

	/*
	 * 找到keys[0]的起始下标
	 */
	keys[0] = inputKey[SPECK_KEY_WORDS-1];

	/*
	 * 计算L的前SPECK_KEY_WORDS-1轮
	 */
	for ( i = 0; i <= SPECK_KEY_WORDS-2; i++ ) {
		L[SPECK_KEY_WORDS-2-i] = inputKey[i];
	}

	/*
	 * 计算密钥
	 */
	for ( i = 0; i < SPECK_ROUNDS-1; i++ ) {
		L[i+SPECK_KEY_WORDS-1] = ( keys[i] + ((L[i]>>SPECK_ROT_A)|(L[i]<<(SPECK_WORD_SIZE-SPECK_ROT_A))) ) ^ ((unsigned int)i);
		keys[i+1] = ( (keys[i]<<SPECK_ROT_B)|(keys[i]>>(SPECK_WORD_SIZE-SPECK_ROT_B)) ) ^ L[i+SPECK_KEY_WORDS-1];
	}

	/*
	 * 释放内存
	 */
	delete L;

}

/*
 * Speck：计算密钥:64位
 * inputKey：初始的密钥
 * keys：计算后得到的每轮密钥
 */
void setSpeckKeys64 ( unsigned long long * inputKey, unsigned long long * keys ) {
	int i;
	/*
	 * inputKey = (L(m-2), ...,L(0),keys(0));则L的轮数比keys多m-2(即SPECK_KEY_WORDS-2)
	 * keys的轮数是SPECK_ROUNDS，则L的轮数为SPECK_ROUNDS+SPECK_KEY_WORDS-2
	 */
	unsigned long long * L = new unsigned long long[SPECK_ROUNDS+SPECK_KEY_WORDS-2];

	/*
	 * 找到keys[0]的起始下标
	 */
	keys[0] = inputKey[SPECK_KEY_WORDS-1];

	/*
	 * 计算L的前SPECK_KEY_WORDS-1轮
	 */
	for ( i = 0; i <= SPECK_KEY_WORDS-2; i++ ) {
		L[SPECK_KEY_WORDS-2-i] = inputKey[i];
	}

	/*
	 * 计算密钥
	 */
	for ( i = 0; i < SPECK_ROUNDS-1; i++ ) {
		L[i+SPECK_KEY_WORDS-1] = ( keys[i] + ((L[i]>>SPECK_ROT_A)|(L[i]<<(SPECK_WORD_SIZE-SPECK_ROT_A))) ) ^ ((unsigned int)i);
		keys[i+1] = ( (keys[i]<<SPECK_ROT_B)|(keys[i]>>(SPECK_WORD_SIZE-SPECK_ROT_B)) ) ^ L[i+SPECK_KEY_WORDS-1];
	}

	/*
	 * 释放内存
	 */
	delete L;
}