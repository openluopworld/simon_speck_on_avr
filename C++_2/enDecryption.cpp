
#include<iostream>
#include"enDecryption.h"
#include"const.h"
using namespace std;

/*
 * Simon加密：字大小为32
 * plainText：明文
 * keys：密钥
 */
void encryptionSimon32 ( unsigned int * plainText, unsigned int * keys ) {
	/*
	 * 保存临时密文
	 */
	unsigned int tempCipherHigher = 0x0;
	unsigned int tempCipherLower  = 0x0;


	/*
	 * 进行SIMON_ROUNDS轮加密
	 */
	for ( int i = 0; i < SIMON_ROUNDS; i++ ) {
		/*
		 * 加密后的低32位是明文的高32位
		 */
		tempCipherLower  = plainText[0];
		tempCipherHigher = plainText[1] ^ keys[i] ^ 
						( ((plainText[0]<<1)|(plainText[0]>>(SIMON_WORD_SIZE-1))) & ((plainText[0]<<8)|(plainText[0]>>(SIMON_WORD_SIZE-8))) ) ^
						((plainText[0]<<2)|(plainText[0]>>(SIMON_WORD_SIZE-2)));
		/*
		 * 重新将加密的结果复制到plainText中
		 */
		plainText[0]     = tempCipherHigher;
		plainText[1]     = tempCipherLower;
	}
	
}

/*
 * Simon解密：字大小为32
 * cipherText：密文
 * keys：密钥
 */
void decryptionSimon32 ( unsigned int * cipherText, unsigned int * keys ) {
	/*
	 * 保存临时明文
	 */
	unsigned int tempPlainHigher = 0x0;
	unsigned int tempPlainLower  = 0x0;     

	int i;
	/*
	 * 解密与加密顺序相反，从最后一轮开始
	 */
	for ( i = SIMON_ROUNDS-1; i >= 0; i-- ) {
		tempPlainHigher = cipherText[1];
		tempPlainLower  = cipherText[0] ^ keys[i] ^ 
						( ((cipherText[1]<<1)|(cipherText[1]>>(SIMON_WORD_SIZE-1))) & ((cipherText[1]<<8)|(cipherText[1]>>(SIMON_WORD_SIZE-8))) ) ^
						((cipherText[1]<<2)|(cipherText[1]>>(SIMON_WORD_SIZE-2)));
		cipherText[0]   = tempPlainHigher;
		cipherText[1]   = tempPlainLower;
		
	}

}

/*
 * Simon加密：字大小为32
 * plainText：明文
 * keys：密钥
 */
void encryptionSimon64 ( unsigned long long * plainText, unsigned long long * keys ) {
	/*
	 * 保存临时密文
	 */
	unsigned long long tempCipherHigher = 0x0;
	unsigned long long tempCipherLower  = 0x0;


	/*
	 * 进行SIMON_ROUNDS轮加密
	 */
	for ( int i = 0; i < SIMON_ROUNDS; i++ ) {
		/*
		 * 加密后的低32位是明文的高32位
		 */
		tempCipherLower  = plainText[0];
		tempCipherHigher = plainText[1] ^ keys[i] ^ 
						( ((plainText[0]<<1)|(plainText[0]>>(SIMON_WORD_SIZE-1))) & ((plainText[0]<<8)|(plainText[0]>>(SIMON_WORD_SIZE-8))) ) ^
						((plainText[0]<<2)|(plainText[0]>>(SIMON_WORD_SIZE-2)));
		/*
		 * 重新将加密的结果复制到plainText中
		 */
		plainText[0]     = tempCipherHigher;
		plainText[1]     = tempCipherLower;
	}
}

/*
 * Simon解密：字大小为64
 * cipherText：密文
 * keys：密钥
 */
void decryptionSimon64 ( unsigned long long * cipherText, unsigned long long * keys ) {
	/*
	 * 保存临时密文
	 */
	unsigned long long tempPlainHigher = 0x0;
	unsigned long long tempPlainLower  = 0x0;     

	int i;
	/*
	 * 解密与加密顺序相反，从最后一轮开始
	 */
	for ( i = SIMON_ROUNDS-1; i >= 0; i-- ) {
		tempPlainHigher = cipherText[1];
		tempPlainLower  = cipherText[0] ^ keys[i] ^ 
						( ((cipherText[1]<<1)|(cipherText[1]>>(SIMON_WORD_SIZE-1))) & ((cipherText[1]<<8)|(cipherText[1]>>(SIMON_WORD_SIZE-8))) ) ^
						((cipherText[1]<<2)|(cipherText[1]>>(SIMON_WORD_SIZE-2)));
		cipherText[0]   = tempPlainHigher;
		cipherText[1]   = tempPlainLower;
		
	}
}

/*
 * Speck加密：字32位
 * plainText：明文
 * keys：密钥
 */
void encryptionSpeck32 ( unsigned int * plainText, unsigned int * keys ) {
	/*
	 * 进行SPECK_ROUNDS轮加密
	 */
	unsigned int tempCipherHigher = 0x0; // 保存临时密文
	unsigned int tempCipherLower  = 0x0;
	unsigned int additionModulo   = 0x0; // 临时保存模2^n加法运算结果

	for ( int i = 0; i < SPECK_ROUNDS; i++ ) {
		additionModulo = ((plainText[0]>>SPECK_ROT_A)|(plainText[0]<<(SPECK_WORD_SIZE-SPECK_ROT_A))) + plainText[1];
		/*
		 * 前SPECK_WORD_SIZE位
		 */
		tempCipherHigher = additionModulo ^ keys[i];
		/*
		 * 后SPECK_WORD_SIZE位
		 */
		tempCipherLower  = ( (plainText[1]<<SPECK_ROT_B)|(plainText[1]>>(SPECK_WORD_SIZE-SPECK_ROT_B)) ) ^ additionModulo ^ keys[i];
		/*
		 * 重新将加密的结果复制到plainText中
		 */
		plainText[0] = tempCipherHigher;
		plainText[1] = tempCipherLower;
		//cout<<"加密"<<(i+1)<<":"<<plainText[0]<<","<<plainText[1]<<endl;
	}

}

/*
 * Speck解密：字32位
 * cipherText：密文
 * keys：密钥
 */
void decryptionSpeck32 ( unsigned int * cipherText, unsigned int * keys ) {
	/*
	 * 保存临时明文
	 */
	unsigned int tempPlainHigher = 0x0;
	unsigned int tempPlainLower  = 0x0;       

	/*
	 * 进行SPECK_ROUNDS轮解密
	 */
	unsigned int temp  = 0x0;
	for ( int i = SPECK_ROUNDS-1; i >= 0; i-- ) {
		//cout<<"解密"<<(i+1)<<":"<<cipherText[0]<<","<<cipherText[1]<<endl;

		/*
		 * tempPlainLower = (cipherText[0]>>SPECK_ROT_B)|(cipherText[0]<<(SPECK_WORD_SIZE-SPECK_ROT_B)) ^
		 *		(cipherText[1]>>SPECK_ROT_B)|(cipherText[1]<<(SPECK_WORD_SIZE-SPECK_ROT_B));
		 * 肯能由于异或优先级高于或，所以先执行异或导致结果出错
		 */
		tempPlainLower = ( (cipherText[0]>>SPECK_ROT_B)|(cipherText[0]<<(SPECK_WORD_SIZE-SPECK_ROT_B)) ) ^
				( (cipherText[1]>>SPECK_ROT_B)|(cipherText[1]<<(SPECK_WORD_SIZE-SPECK_ROT_B)) );
		/*
		 * 结果的前SPECK_WORD_SIZE位
		 * 测试发现，在无符号的情况下，小数减大数结果仍为正，相当于借了一位，只是没有表示出来
		 * 
		 * unsigned int temp1 = 0x40000000;
		 * unsigned int temp2 = 0x80000000;
		 * cout<<temp1-temp2<<endl;
		 * cout<<temp1<<endl;
		 * cout<<temp2<<endl;
		 */
		temp = (cipherText[0] ^ keys[i]) - tempPlainLower;
		tempPlainHigher = (temp<<SPECK_ROT_A)|(temp>>(SPECK_WORD_SIZE-SPECK_ROT_A));

		/*
		 * 重新将解密的结果复制到plainText中
		 */
		cipherText[0] = tempPlainHigher;
		cipherText[1] = tempPlainLower;
	}

}

/*
 * Speck加密
 * plainText：明文
 * keys：密钥
 */
void encryptionSpeck64 ( unsigned long long * plainText, unsigned long long  * keys ) {
	/*
	 * 进行SPECK_ROUNDS轮加密
	 */
	unsigned long long tempCipherHigher = 0x0; // 保存临时密文
	unsigned long long tempCipherLower  = 0x0;
	unsigned long long additionModulo   = 0x0; // 临时保存模2^n加法运算结果

	for ( int i = 0; i < SPECK_ROUNDS; i++ ) {
		additionModulo = ((plainText[0]>>SPECK_ROT_A)|(plainText[0]<<(SPECK_WORD_SIZE-SPECK_ROT_A))) + plainText[1];
		/*
		 * 前SPECK_WORD_SIZE位
		 */
		tempCipherHigher = additionModulo ^ keys[i];
		/*
		 * 后SPECK_WORD_SIZE位
		 */
		tempCipherLower  = ( (plainText[1]<<SPECK_ROT_B)|(plainText[1]>>(SPECK_WORD_SIZE-SPECK_ROT_B)) ) ^ additionModulo ^ keys[i];
		/*
		 * 重新将加密的结果复制到plainText中
		 */
		plainText[0] = tempCipherHigher;
		plainText[1] = tempCipherLower;
		//cout<<"加密"<<(i+1)<<":"<<plainText[0]<<","<<plainText[1]<<endl;
	}
}

/*
 * Speck解密
 * cipherText：密文
 * keys：密钥
 */
void decryptionSpeck64 ( unsigned long long * cipherText, unsigned long long * keys ) {
	/*
	 * 保存临时明文
	 */
	unsigned long long tempPlainHigher = 0x0;
	unsigned long long tempPlainLower  = 0x0;       

	/*
	 * 进行SPECK_ROUNDS轮解密
	 */
	unsigned long long temp  = 0x0;
	for ( int i = SPECK_ROUNDS-1; i >= 0; i-- ) {
		tempPlainLower = ( (cipherText[0]>>SPECK_ROT_B)|(cipherText[0]<<(SPECK_WORD_SIZE-SPECK_ROT_B)) ) ^
				( (cipherText[1]>>SPECK_ROT_B)|(cipherText[1]<<(SPECK_WORD_SIZE-SPECK_ROT_B)) );
		temp = (cipherText[0] ^ keys[i]) - tempPlainLower;
		tempPlainHigher = (temp<<SPECK_ROT_A)|(temp>>(SPECK_WORD_SIZE-SPECK_ROT_A));

		/*
		 * 重新将解密的结果复制到plainText中
		 */
		cipherText[0] = tempPlainHigher;
		cipherText[1] = tempPlainLower;
	}
}