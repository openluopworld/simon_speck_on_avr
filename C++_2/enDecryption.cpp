
#include<iostream>
#include"enDecryption.h"
#include"const.h"
using namespace std;

/*
 * Simon encryption: word size is 32(block size is 64)
 *
 * plainText: the point of plainText. There is only one block. plainText[0] is the higher parter, [1] is the lower.
 * keys: round keys
 */
void encryptionSimon32 ( unsigned int * plainText, unsigned int * keys ) {
	unsigned int tempCipherHigher = 0x0;
	unsigned int tempCipherLower  = 0x0;

	for ( int i = 0; i < SIMON_ROUNDS; i++ ) {
		tempCipherLower  = plainText[0];
		tempCipherHigher = plainText[1] ^ keys[i] ^ 
						( ((plainText[0]<<1)|(plainText[0]>>(SIMON_WORD_SIZE-1))) & ((plainText[0]<<8)|(plainText[0]>>(SIMON_WORD_SIZE-8))) ) ^
						((plainText[0]<<2)|(plainText[0]>>(SIMON_WORD_SIZE-2)));
		plainText[0]     = tempCipherHigher;
		plainText[1]     = tempCipherLower;
	}
	
}

/*
 * Simon decryption: word size is 32(block size is 64)
 *
 * cipherText: the point of cipherText. There is only one block. cipherText[0] is the higher part, [1] is the lower.
 * keys: round keys
 */
void decryptionSimon32 ( unsigned int * cipherText, unsigned int * keys ) {
	unsigned int tempPlainHigher = 0x0;
	unsigned int tempPlainLower  = 0x0;     

	int i;
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
 * Simon encryption: word size is 64(block size is 128)
 *
 * plainText: the point of plainText. There is only one block. plainText[0] is the higher parter, [1] is the lower.
 * keys: round keys
 */
void encryptionSimon64 ( unsigned long long * plainText, unsigned long long * keys ) {
	unsigned long long tempCipherHigher = 0x0;
	unsigned long long tempCipherLower  = 0x0;

	for ( int i = 0; i < SIMON_ROUNDS; i++ ) {
		tempCipherLower  = plainText[0];
		tempCipherHigher = plainText[1] ^ keys[i] ^ 
						( ((plainText[0]<<1)|(plainText[0]>>(SIMON_WORD_SIZE-1))) & ((plainText[0]<<8)|(plainText[0]>>(SIMON_WORD_SIZE-8))) ) ^
						((plainText[0]<<2)|(plainText[0]>>(SIMON_WORD_SIZE-2)));
		plainText[0]     = tempCipherHigher;
		plainText[1]     = tempCipherLower;
	}
}

/*
 * Simon decryption: word size is 64(block size is 128)
 *
 * cipherText: the point of cipherText. There is only one block. cipherText[0] is the higher part, [1] is the lower.
 * keys: round keys
 */
void decryptionSimon64 ( unsigned long long * cipherText, unsigned long long * keys ) {
	unsigned long long tempPlainHigher = 0x0;
	unsigned long long tempPlainLower  = 0x0;     

	int i;
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
 * Speck encryption: word size is 32(block size is 64)
 *
 * plainText: the point of plainText. There is only one block. plainText[0] is the higher parter, [1] is the lower.
 * keys: round keys
 */
void encryptionSpeck32 ( unsigned int * plainText, unsigned int * keys ) {
	unsigned int tempCipherHigher = 0x0; // store the temp cipher
	unsigned int tempCipherLower  = 0x0;
	unsigned int additionModulo   = 0x0; // store the temp value of 'add mod 2^n'

	for ( int i = 0; i < SPECK_ROUNDS; i++ ) {
		additionModulo = ((plainText[0]>>SPECK_ROT_A)|(plainText[0]<<(SPECK_WORD_SIZE-SPECK_ROT_A))) + plainText[1];
		tempCipherHigher = additionModulo ^ keys[i];
		tempCipherLower  = ( (plainText[1]<<SPECK_ROT_B)|(plainText[1]>>(SPECK_WORD_SIZE-SPECK_ROT_B)) ) ^ additionModulo ^ keys[i];

		plainText[0] = tempCipherHigher;
		plainText[1] = tempCipherLower;
		//cout<<"Encryption:"<<(i+1)<<":"<<plainText[0]<<","<<plainText[1]<<endl;
	}

}

/*
 * Speck decryption: word size is 32(block size is 64)
 *
 * cipherText: the point of cipherText. There is only one block. cipherText[0] is the higher parter, [1] is the lower.
 * keys: round keys
 */
void decryptionSpeck32 ( unsigned int * cipherText, unsigned int * keys ) {
	unsigned int tempPlainHigher = 0x0;
	unsigned int tempPlainLower  = 0x0;       

	unsigned int temp  = 0x0;
	for ( int i = SPECK_ROUNDS-1; i >= 0; i-- ) {
		//cout<<"Decryption:"<<(i+1)<<":"<<cipherText[0]<<","<<cipherText[1]<<endl;

		/*
		 * tempPlainLower = (cipherText[0]>>SPECK_ROT_B)|(cipherText[0]<<(SPECK_WORD_SIZE-SPECK_ROT_B)) ^
		 *		(cipherText[1]>>SPECK_ROT_B)|(cipherText[1]<<(SPECK_WORD_SIZE-SPECK_ROT_B));
		 * the priority of ^ is higher than |.
		 */
		tempPlainLower = ( (cipherText[0]>>SPECK_ROT_B)|(cipherText[0]<<(SPECK_WORD_SIZE-SPECK_ROT_B)) ) ^
				( (cipherText[1]>>SPECK_ROT_B)|(cipherText[1]<<(SPECK_WORD_SIZE-SPECK_ROT_B)) );
		/*
		 * the higher SPECK_WORD_SIZE bits
		 * 
		 * unsigned int temp1 = 0x40000000;
		 * unsigned int temp2 = 0x80000000;
		 * cout<<temp1-temp2<<endl; // 0xc0000000
		 * cout<<temp1<<endl;
		 * cout<<temp2<<endl;
		 */
		temp = (cipherText[0] ^ keys[i]) - tempPlainLower;
		tempPlainHigher = (temp<<SPECK_ROT_A)|(temp>>(SPECK_WORD_SIZE-SPECK_ROT_A));

		cipherText[0] = tempPlainHigher;
		cipherText[1] = tempPlainLower;
	}

}

/*
 * Speck encryption: word size is 64(block size is 128)
 *
 * plainText: the point of plainText. There is only one block. plainText[0] is the higher parter, [1] is the lower.
 * keys: round keys
 */
void encryptionSpeck64 ( unsigned long long * plainText, unsigned long long  * keys ) {
	unsigned long long tempCipherHigher = 0x0;
	unsigned long long tempCipherLower  = 0x0;
	unsigned long long additionModulo   = 0x0;

	for ( int i = 0; i < SPECK_ROUNDS; i++ ) {
		additionModulo = ((plainText[0]>>SPECK_ROT_A)|(plainText[0]<<(SPECK_WORD_SIZE-SPECK_ROT_A))) + plainText[1];
		tempCipherHigher = additionModulo ^ keys[i];
		tempCipherLower  = ( (plainText[1]<<SPECK_ROT_B)|(plainText[1]>>(SPECK_WORD_SIZE-SPECK_ROT_B)) ) ^ additionModulo ^ keys[i];
		plainText[0] = tempCipherHigher;
		plainText[1] = tempCipherLower;
	}
}

/*
 * Speck decryption: word size is 64(block size is 128)
 *
 * cipherText: the point of cipherText. There is only one block. cipherText[0] is the higher parter, [1] is the lower.
 * keys: round keys
 */
void decryptionSpeck64 ( unsigned long long * cipherText, unsigned long long * keys ) {
	unsigned long long tempPlainHigher = 0x0;
	unsigned long long tempPlainLower  = 0x0;

	unsigned long long temp  = 0x0;
	for ( int i = SPECK_ROUNDS-1; i >= 0; i-- ) {
		tempPlainLower = ( (cipherText[0]>>SPECK_ROT_B)|(cipherText[0]<<(SPECK_WORD_SIZE-SPECK_ROT_B)) ) ^
				( (cipherText[1]>>SPECK_ROT_B)|(cipherText[1]<<(SPECK_WORD_SIZE-SPECK_ROT_B)) );
		temp = (cipherText[0] ^ keys[i]) - tempPlainLower;
		tempPlainHigher = (temp<<SPECK_ROT_A)|(temp>>(SPECK_WORD_SIZE-SPECK_ROT_A));

		cipherText[0] = tempPlainHigher;
		cipherText[1] = tempPlainLower;
	}
}