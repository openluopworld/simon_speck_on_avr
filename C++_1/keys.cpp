
#include"keys.h"
#include"keyZ.h"
#include"const.h"
#include<iostream>
using namespace std;

/*
 * Simon：计算密钥
 * inputKey：初始的密钥
 * keys：计算后得到的每轮密钥
 */
void setSimonKeys ( int * inputKey, int ** keys ) {

	/*
	 * 算法中的常数c，大小为2^n - 4，其中n是字的长度，即SIMON_WORD_SIZE
	 * 转化为二进制，即最低两位为0，其它位全为1
	 */
	int * c = new int[SIMON_WORD_SIZE];
	for ( int i = 0; i < SIMON_WORD_SIZE-2; i++ ) {
		c[i] = 1;
	}
	c[SIMON_WORD_SIZE-2] = 0;
	c[SIMON_WORD_SIZE-1] = 0;

	/*
	 * 根据初始密码得到前SIMON_KEY_WORDS轮密钥
	 * inputKey的长度是SIMON_KEY_SIZE(=SIMON_WORD_SIZE*SIMON_KEY_WORDS)
	 * 从右往左一次为k0,k1,k(m-1)
	 */
	int i, j;
	for ( i = SIMON_KEY_WORDS-1; i >= 0; i-- ) {
		for ( j = SIMON_WORD_SIZE-1; j >= 0; j-- ) {
			keys[SIMON_KEY_WORDS-1-i][j] = inputKey[i*SIMON_WORD_SIZE+j];
		}
	}

	/*
	 * 求解后面轮的密钥
	 * 先求其它的异或，最后求Zji，如果为1则对最低位进行修改，否则不变
	 */
	if ( SIMON_KEY_WORDS == 2 ) {
		for ( i = 0; i < SIMON_ROUNDS-SIMON_KEY_WORDS; i++ ) {
			for ( j = 0; j < SIMON_WORD_SIZE; j++ ) {
				/*
				 * 右移3位，则该位对应的数值减3，但是如果为负数取模得到的是负数，所以先加上SIMON_WORD_SIZE后在取模，
				 * 即(x-3+SIMON_WORD_SIZE)%SIMON_WORD_SIZE。同理，右移1位、4位做相应处理
				 */
				keys[i+SIMON_KEY_WORDS][j] = (c[j] + 
								keys[i][j] +
								keys[i+1][(j-3+SIMON_WORD_SIZE)%SIMON_WORD_SIZE] +
								keys[i+1][(j-4+SIMON_WORD_SIZE)%SIMON_WORD_SIZE] 
								) % 2;
			}
			/*
			 * 最后与z[j][i]进行异或运算，当SIMON_KEY_WORDS确定时，j也是确定的
			 */
			if ( z[SIMON_SEQUENCE_NUMBER][i%62] == 1 ) {
				keys[i+SIMON_KEY_WORDS][SIMON_WORD_SIZE-1] =  ( keys[i+SIMON_KEY_WORDS][SIMON_WORD_SIZE-1] + 1 ) % 2;
			}
		}
	} else if ( SIMON_KEY_WORDS == 3 ) {
		int cycle = (SIMON_SEQUENCE_NUMBER == 0 || SIMON_SEQUENCE_NUMBER == 1)?31:62;
		for ( i = 0; i < SIMON_ROUNDS-SIMON_KEY_WORDS; i++ ) {
			for ( j = 0; j < SIMON_WORD_SIZE; j++ ) {
				keys[i+SIMON_KEY_WORDS][j] = (c[j] + 
								keys[i][j] +
								keys[i+2][(j-3+SIMON_WORD_SIZE)%SIMON_WORD_SIZE] +
								keys[i+2][(j-4+SIMON_WORD_SIZE)%SIMON_WORD_SIZE] 
								) % 2;
			}
			if ( z[SIMON_SEQUENCE_NUMBER][i%cycle] == 1 ) {
				keys[i+SIMON_KEY_WORDS][SIMON_WORD_SIZE-1] =  ( keys[i+SIMON_KEY_WORDS][SIMON_WORD_SIZE-1] + 1 ) % 2;
			}
		}
	} else if ( SIMON_KEY_WORDS == 4 ) {
		int cycle = (SIMON_SEQUENCE_NUMBER == 0 || SIMON_SEQUENCE_NUMBER == 1)?31:62;
		for ( i = 0; i < SIMON_ROUNDS-SIMON_KEY_WORDS; i++ ) {
			for ( j = 0; j < SIMON_WORD_SIZE; j++ ) {
				keys[i+SIMON_KEY_WORDS][j] = (c[j] + 
								keys[i][j] +
								(keys[i+3][(j-3+SIMON_WORD_SIZE)%SIMON_WORD_SIZE]+keys[i+1][j])%2 +
								(keys[i+3][(j-4+SIMON_WORD_SIZE)%SIMON_WORD_SIZE]+keys[i+1][(j-1+SIMON_WORD_SIZE)%SIMON_WORD_SIZE])%2 
								) % 2;
			}
			if ( z[SIMON_SEQUENCE_NUMBER][i%cycle] == 1 ) {
				keys[i+SIMON_KEY_WORDS][SIMON_WORD_SIZE-1] =  ( keys[i+SIMON_KEY_WORDS][SIMON_WORD_SIZE-1] + 1 ) % 2;
			}
		}
	}
	/*
	 * 释放c
	 */
	delete c;
}

/*
 * Speck：计算密钥
 * inputKey：初始的密钥
 * keys：计算后得到的每轮密钥
 */
void setSpeckKeys ( int * inputKey, int ** keys ) {
	int i, j;
	/*
	 * inputKey = (L(m-2), ...,L(0),keys(0));则L的轮数比keys多m-2(即SPECK_KEY_WORDS-2)
	 * keys的轮数是SPECK_ROUNDS，则L的轮数为SPECK_ROUNDS+SPECK_KEY_WORDS-2
	 */
	int ** L = new int * [SPECK_ROUNDS+SPECK_KEY_WORDS-2];
	for ( i = 0; i < SPECK_ROUNDS+SPECK_KEY_WORDS-2; i++ ) {
		L[i] = new int[SPECK_WORD_SIZE];
	}

	/*
	 * 找到keys[0]的起始下标
	 */
	int beginIndex = (SPECK_KEY_WORDS-1)*SPECK_WORD_SIZE;
	for ( i = beginIndex; i < SPECK_KEY_SIZE; i++ ) {
		keys[0][i-beginIndex] = inputKey[i];
	}

	/*
	 * 计算L的前SPECK_KEY_WORDS-1轮
	 */
	for ( i = SPECK_KEY_WORDS-2; i >= 0; i-- ) {
		for ( j = SPECK_WORD_SIZE-1; j >= 0; j-- ) {
			L[SPECK_KEY_WORDS-2-i][j] = inputKey[i*SPECK_WORD_SIZE+j];
		}
	}

	/*
	 * 计算密钥
	 */
	int index;
	int temp;
	int jinwei;
	for ( i = 0; i < SPECK_ROUNDS-1; i++ ) {
		/*
		 * 计算L[i+SPECK_KEY_WORDS-1]
		 * 先进行模2^n的加法运算，再与i求异或
		 * 模2^n的加法运算就是一般带进位的加法，从最低位（即数组的最高位）开始加，由于是模2^n，最后一个进位正好可以舍弃
		 */
		jinwei = 0;
		for ( j = SPECK_WORD_SIZE-1; j >= 0; j-- ) {
			L[i+SPECK_KEY_WORDS-1][j] = ( keys[i][j] + L[i][(j-SPECK_ROT_A+SPECK_WORD_SIZE)%SPECK_WORD_SIZE] + jinwei ) % 2;
			jinwei = ( keys[i][j] + L[i][(j-SPECK_ROT_A+SPECK_WORD_SIZE)%SPECK_WORD_SIZE] + jinwei) / 2;
		}
		temp = i;
		index = SPECK_WORD_SIZE-1;
		while ( temp != 0 ) {
			if ( temp % 2 == 1 ) {
				L[i+SPECK_KEY_WORDS-1][index] = ( L[i+SPECK_KEY_WORDS-1][index] + 1 ) % 2;
			}
			temp /= 2;
			index--;
		}

		/*
		 * 计算keys[i+1]
		 * 
		 * for ( j = 0; j < SPECK_WORD_SIZE-1; j++ ) {
		 * 花了好大劲才找出这个错误，不知道为什么会写成-1的形式
		 * 由于每轮密钥的最后一个二进制位不正确，导致加密结果不正确，但是解密后又能还原
		 */
		for ( j = 0; j < SPECK_WORD_SIZE; j++ ) {
			keys[i+1][j] = ( keys[i][(j+SPECK_ROT_B)%SPECK_WORD_SIZE] + L[i+SPECK_KEY_WORDS-1][j] ) % 2;
		}
	}

	//cout<<endl<<"L:"<<endl;
	//for ( i = 0; i < SPECK_ROUNDS+SPECK_KEY_WORDS-2; i++ ) {
	//	cout<<i<<":";
	//	for ( j = 0; j < SPECK_WORD_SIZE; j++ ) {
	//		cout<<L[i][j]<<" ";
	//	}
	//	cout<<endl;
	//}
	//cout<<endl;

	/*
	 * 释放内存
	 */
	for ( i = 0; i < SPECK_ROUNDS+SPECK_KEY_WORDS-2; i++ ) {
		delete L[i];
	}
	delete L;

}