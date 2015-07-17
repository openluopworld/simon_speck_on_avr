#include<iostream>
#include<time.h>
#include"simon-speck.h"
using namespace std;

/*
 * const z
 */
u8 z[5][64] = {
	{1,1,1,1,1,0,1,0,0,0,1,0,0,1,0,1,0,1,1,0,0,0,0,1,1,1,0,0,1,1,0,1,1,1,1,1,0,1,0,0,0,1,0,0,1,0,1,0,1,1,0,0,0,0,1,1,1,0,0,1,1,0},
	{1,0,0,0,1,1,1,0,1,1,1,1,1,0,0,1,0,0,1,1,0,0,0,0,1,0,1,1,0,1,0,1,0,0,0,1,1,1,0,1,1,1,1,1,0,0,1,0,0,1,1,0,0,0,0,1,0,1,1,0,1,0},
	{1,0,1,0,1,1,1,1,0,1,1,1,0,0,0,0,0,0,1,1,0,1,0,0,1,0,0,1,1,0,0,0,1,0,1,0,0,0,0,1,0,0,0,1,1,1,1,1,1,0,0,1,0,1,1,0,1,1,0,0,1,1},
	{1,1,0,1,1,0,1,1,1,0,1,0,1,1,0,0,0,1,1,0,0,1,0,1,1,1,1,0,0,0,0,0,0,1,0,0,1,0,0,0,1,0,1,0,0,1,1,1,0,0,1,1,0,1,0,0,0,0,1,1,1,1},
	{1,1,0,1,0,0,0,1,1,1,1,0,0,1,1,0,1,0,1,1,0,1,1,0,0,0,1,0,0,0,0,0,0,1,0,1,1,1,0,0,0,0,1,1,0,0,1,0,1,0,0,1,0,0,1,1,1,0,1,1,1,1}
			};

//**********************************************************************
// rotate shift
//**********************************************************************
/*
 * rotate shift left x by n bits
 */
inline u32 rol32 (u32 x, int n) {
	return (x<<n) | (x>>(SIMON_WORD_SIZE-n));
}

/*
 * rotate shift left x by n bits
 */
inline u64 rol64 (u64 x, int n) {
	return (x<<n) | (x>>(SIMON_WORD_SIZE-n));
}

/*
 * rotate shift right x by n bits
 */
inline u32 ror32 (u32 x, int n) {
	return (x>>n) | (x<<(SIMON_WORD_SIZE-n));
}

/*
 * rotate shift right x by n bits
 */
inline u64 ror64 (u64 x, int n) {
	return (x>>n) | (x<<(SIMON_WORD_SIZE-n));
}

//**********************************************************************
// Key Schedule
//**********************************************************************
/*
 * Simon£ºword size is 32
 * inputKey£ºthe original keys
 * keys: round keys
 */
void setSimonKeys32 ( u32 * inputKey, u32 * keys ) {

	int i;
	for ( i = 0; i < SIMON_KEY_WORDS; i++ )  {
		keys[i] = inputKey[i];
	}

	u32 temp;
	if ( SIMON_KEY_WORDS == 3 ) {
		for ( i = 0; i < SIMON_ROUNDS-SIMON_KEY_WORDS; i++ ) {
			temp = ror32(keys[i+2], 3);
			temp ^= ror32(temp, 1);
			keys[i+SIMON_KEY_WORDS] = SIMON_CONST_C ^ keys[i] ^ temp;
			// if SIMON_WORD_SIZE is 32, SIMON_CONST_Cthe cycle is 62 whether SIMON_KEY_WORDS is 3 or 4.
			if ( z[SIMON_SEQUENCE_NUMBER][i%62] == 1 ) {
				keys[i+SIMON_KEY_WORDS] ^=  0x1;
			}
		}
	} else if ( SIMON_KEY_WORDS == 4 ) {
		for ( i = 0; i < SIMON_ROUNDS-SIMON_KEY_WORDS; i++ ) {
			temp = ror32(keys[i+3], 3) ^ keys[i+1];
			keys[i+SIMON_KEY_WORDS] = SIMON_CONST_C ^ keys[i] ^ temp ^ ror32(temp, 1);
			if ( z[SIMON_SEQUENCE_NUMBER][i%62] == 1 ) {
				keys[i+SIMON_KEY_WORDS] ^=  0x1;
			}
		}
	}
}

/*
 * Simon£ºword size is 64
 * inputKey£ºthe original keys
 * keys£ºround keys
 */
void setSimonKeys64 ( u64 * inputKey, u64 * keys ){

	int i;
	for ( i = 0; i < SIMON_KEY_WORDS; i++ )  {
		keys[i] = inputKey[i];
	}

	u64 temp;
	if ( SIMON_KEY_WORDS == 2 ) {
		for ( i = 0; i < SIMON_ROUNDS-SIMON_KEY_WORDS; i++ ) {
			temp = ror32(keys[i+1], 3);
			temp ^= ror32(temp, 1);
			keys[i+SIMON_KEY_WORDS] = SIMON_CONST_C ^ keys[i] ^ temp;
			if ( z[SIMON_SEQUENCE_NUMBER][i%62] == 1 ) {
				keys[i+SIMON_KEY_WORDS] ^= 0x1 ;
			}
		}
	}

}

//**********************************************************************
// Encryption and Decryption
//**********************************************************************
/*
 * Simon Encryption£ºword size is 32
 * plainText£ºplainText[0] is the higher word, [1] is the lower. 
 *            plainText has just one block.
 * keys£ºround keys
 */
void encryptionSimon32 ( u32 * plainText, u32 * keys ) {

	u32 tempCipherHigher = 0x0;
	u32 tempCipherLower  = 0x0;

	for ( int i = 0; i < SIMON_ROUNDS; i++ ) {
		tempCipherLower  = plainText[0];
		tempCipherHigher = plainText[1] ^ keys[i] ^ 
						( rol32(plainText[0], 1) & rol32(plainText[0], 8)) ^
						rol32(plainText[0], 2);
		plainText[0]     = tempCipherHigher;
		plainText[1]     = tempCipherLower;
	}
	
}

/*
 * Simon Decryption£ºword size is 32
 * cipherText£ºcipherText[0] is the higher word, [1] is the lower.
 *             cipherText has just one block.
 * keys£ºround keys
 */
void decryptionSimon32 ( u32 * cipherText, u32 * keys ) {

	u32 tempPlainHigher = 0x0;
	u32 tempPlainLower  = 0x0;     

	for ( int i = SIMON_ROUNDS-1; i >= 0; i-- ) {
		tempPlainHigher = cipherText[1];
		tempPlainLower  = cipherText[0] ^ keys[i] ^ 
						( rol32(cipherText[1], 1) & rol32(cipherText[1], 8)) ^
						rol32(cipherText[1], 2);
		cipherText[0]   = tempPlainHigher;
		cipherText[1]   = tempPlainLower;
		
	}

}

/*
 * Simon Encryption£ºword size is 64
 * plainText£ºplainText[0] is the higher word, [1] is the lower. 
 *            plainText has just one block.
 * keys£ºround keys
 */
void encryptionSimon64 ( u64 * plainText, u64 * keys ) {

	u64 tempCipherHigher = 0x0;
	u64 tempCipherLower  = 0x0;

	for ( int i = 0; i < SIMON_ROUNDS; i++ ) {
		tempCipherLower  = plainText[0];
		tempCipherHigher = plainText[1] ^ keys[i] ^ 
						( rol64(plainText[0], 1) & rol64(plainText[0], 8)) ^
						rol64(plainText[0], 2);
		plainText[0]     = tempCipherHigher;
		plainText[1]     = tempCipherLower;
	}
}

/*
 * Simon Decryption£ºword size is 64
 * cipherText£ºcipherText[0] is the higher word, [1] is the lower.
 *             cipherText has just one block.
 * keys£ºround keys
 */
void decryptionSimon64 ( u64 * cipherText, u64 * keys ) {
	
	u64 tempPlainHigher = 0x0;
	u64 tempPlainLower  = 0x0;     

	for ( int i = SIMON_ROUNDS-1; i >= 0; i-- ) {
		tempPlainHigher = cipherText[1];
		tempPlainLower  = cipherText[0] ^ keys[i] ^ 
						( rol64(cipherText[1], 1) & rol64(cipherText[1], 8)) ^
						rol64(cipherText[1], 2);
		cipherText[0]   = tempPlainHigher;
		cipherText[1]   = tempPlainLower;
		
	}
}

//**********************************************************************
// Test
//**********************************************************************
int main () {
	if ( SIMON_BLOCK_SIZE==64 && SIMON_KEY_WORDS==3 )  {
		u32 plainX = 0;
		u32 plainY = 0;
		u32 cipherX = 0;
		u32 cipherY = 0;
		/*
		 * Simon64/96
		 */
		u32 * keys = new u32[SIMON_ROUNDS];
	} else if ( SIMON_BLOCK_SIZE==64 && SIMON_KEY_WORDS==4 ) {
		/*
		 * Simon64/128
		 */
		u32 * keys = new u32[SIMON_ROUNDS];
	} else if ( SIMON_BLOCK_SIZE==128 && SIMON_KEY_WORDS==2 ) {
		/*
		 * Simon128/128
		 */
		u64 * keys = new u64[SIMON_ROUNDS];
	}
	return 0;
}

