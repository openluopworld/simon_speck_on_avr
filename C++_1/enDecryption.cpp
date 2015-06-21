
#include"enDecryption.h"
#include"const.h"

/*
 * Simon加密
 * plainText：明文
 * keys：密钥
 */
void encryptionSimon ( int * plainText, int ** keys ) {
	/*
	 * 保存临时密文
	 */
	int * tempCipher = new int[SIMON_BLOCK_SIZE];

	/*
	 * 进行SIMON_ROUNDS轮加密
	 */
	int i, j;
	for ( i = 0; i < SIMON_ROUNDS; i++ ) {
		for ( j = 0; j < SIMON_WORD_SIZE; j++ ) {
			tempCipher[j+SIMON_WORD_SIZE] = plainText[j];
		}
		for ( j = 0; j < SIMON_WORD_SIZE; j++ ) {
			/*
			 * 左移1位，则该位对应的数值加1，即(x+1)%32
			 * 两个二进制数字相加再对2取整即为这两个数的与运算结果
			 */
			tempCipher[j] = ((plainText[(j+1)%SIMON_WORD_SIZE] + plainText[(j+8)%SIMON_WORD_SIZE])/2 +
							plainText[j+SIMON_WORD_SIZE] +
							plainText[(j+2)%SIMON_WORD_SIZE] +
							keys[i][j]) % 2;
		}

		/*
		 * 重新将加密的结果复制到plainText中
		 */
		for ( j = 0; j < SIMON_BLOCK_SIZE; j++ ) {
			plainText[j] = tempCipher[j];
		}
	}
	
	/*
	 * 释放内存
	 */
	delete tempCipher;
}

/*
 * Simon解密
 * cipherText：密文
 * keys：密钥
 */
void decryptionSimon ( int * cipherText, int ** keys ) {
	/*
	 * 保存临时解密的明文
	 */
	int * tempPlain = new int[SIMON_BLOCK_SIZE];       

	int i, j;
	/*
	 * 解密与加密顺序相反，从最后一轮开始
	 */
	for ( i = SIMON_ROUNDS-1; i >= 0; i-- ) {
		for ( j = 0; j < SIMON_WORD_SIZE; j++ ) {
			tempPlain[j] = cipherText[j+SIMON_WORD_SIZE];
		}
		for ( j = SIMON_WORD_SIZE; j < SIMON_BLOCK_SIZE; j++ ) {
			tempPlain[j] = ((cipherText[(j+1)%SIMON_WORD_SIZE+SIMON_WORD_SIZE] + cipherText[(j+8)%SIMON_WORD_SIZE+SIMON_WORD_SIZE])/2 +
							cipherText[(j+2)%SIMON_WORD_SIZE+SIMON_WORD_SIZE] +
							cipherText[j-SIMON_WORD_SIZE] +
							keys[i][j-SIMON_WORD_SIZE]) % 2; 
		}
		for ( j = 0; j < SIMON_BLOCK_SIZE; j++ ) {
			cipherText[j] = tempPlain[j];
		}
	}
	delete tempPlain;
}

/*
 * Speck加密
 * plainText：明文
 * keys：密钥
 */
void encryptionSpeck ( int * plainText, int ** keys ) {
	/*
	 * 进行SPECK_ROUNDS轮加密
	 */
	int i, j;
	int * tempCipher = new int[SPECK_BLOCK_SIZE];        // 保存临时密文
	int * additionModulo = new int[SPECK_WORD_SIZE];     // 临时保存模2^n加法运算结果
	int jinwei;

	for ( i = 0; i < SPECK_ROUNDS; i++ ) {
		jinwei = 0;
		for ( j = SPECK_WORD_SIZE-1; j >= 0; j--) {
			additionModulo[j] = ( plainText[(j-SPECK_ROT_A+SPECK_WORD_SIZE)%SPECK_WORD_SIZE] + plainText[j+SPECK_WORD_SIZE] + jinwei ) % 2;
			jinwei = ( plainText[(j-SPECK_ROT_A+SPECK_WORD_SIZE)%SPECK_WORD_SIZE] + plainText[j+SPECK_WORD_SIZE] + jinwei ) / 2;
		}

		/*
		 * 前SPECK_WORD_SIZE位
		 */
		for ( j = 0; j < SPECK_WORD_SIZE; j++ ) {
			tempCipher[j] = ( additionModulo[j] + keys[i][j]) % 2;
		}

		/*
		 * 后SPECK_WORD_SIZE位
		 */
		for ( j = SPECK_WORD_SIZE; j < SPECK_BLOCK_SIZE; j++ ) {
			tempCipher[j] = ( additionModulo[j-SPECK_WORD_SIZE] + 
							keys[i][j-SPECK_WORD_SIZE] + 
							plainText[(j+SPECK_ROT_B)%SPECK_WORD_SIZE+SPECK_WORD_SIZE]) % 2;
		}

		/*
		 * 重新将加密的结果复制到plainText中
		 */
		for ( j = 0; j < SPECK_BLOCK_SIZE; j++ ) {
			plainText[j] = tempCipher[j];
		}
	}
	delete additionModulo;
	delete tempCipher;
}

/*
 * Speck解密
 * cipherText：密文
 * keys：密钥
 */
void decryptionSpeck ( int * cipherText, int ** keys ) {
	/*
	 * 保存临时密文
	 */
	int * tempCipher = new int[SPECK_BLOCK_SIZE];        

	/*
	 * 进行SPECK_ROUNDS轮加密
	 */
	int i, j;
	int * first  = new int[SPECK_WORD_SIZE];
	int * second = new int[SPECK_WORD_SIZE];
	int jiewei;
	for ( i = SPECK_ROUNDS-1; i >= 0; i-- ) {
		for ( j = 0; j < SPECK_WORD_SIZE; j++ ) {
			first[j] = (cipherText[j] + keys[i][j] ) % 2;
		}
		for ( j = 0; j < SPECK_WORD_SIZE; j++ ) {
			second[j] = (cipherText[(j-SPECK_ROT_B+SPECK_WORD_SIZE)%SPECK_WORD_SIZE] +
						cipherText[(j-SPECK_ROT_B+SPECK_WORD_SIZE)%SPECK_WORD_SIZE+SPECK_WORD_SIZE] ) % 2;
		}

		/*
		 * 结果的前SPECK_WORD_SIZE位
		 * 
		 * 减法运算与加法不一样：先做减法，在进行循环左移SPECK_ROT_A位；不能先移位再减
		 */
		jiewei = 0;
		for ( j = SPECK_WORD_SIZE-1; j >= 0; j-- ) {
			// 借位相减
			tempCipher[j] = first[j]-jiewei-second[j];
			if ( tempCipher[j] < 0 ) {
				tempCipher[j] += 2;
				jiewei = 1;
			} else {
				jiewei = 0;
			}
		}

		/*
		 * 重新将解密的结果复制到plainText中
		 */
		for ( j = 0; j < SPECK_WORD_SIZE; j++ ) {
			cipherText[j] = tempCipher[(j+SPECK_ROT_A)%SPECK_WORD_SIZE];
		}
		for ( j = SPECK_WORD_SIZE; j < SPECK_BLOCK_SIZE; j++ ) {
			cipherText[j] = second[j-SPECK_WORD_SIZE];
		}
	}
	
	/*
	 * 释放内存
	 */
	delete second;
	delete first;
	delete tempCipher;
}