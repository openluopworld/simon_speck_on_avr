
#include"keys.h"
#include"keyZ.h"
#include"const.h"
#include<iostream>
using namespace std;

/*
 * Simon: compute round keys, the word size is 32(block size is 64)
 *
 * inputKey: the initial keys
 * keys: round keys
 */
void setSimonKeys32 ( unsigned int * inputKey, unsigned int * keys ) {

	/*
	 * The value of const c is (2^n-4). n is the word size. Here is 32.
	 */
	unsigned int c = 0xfffffffc;

	int i;
	for ( i = 0; i < SIMON_KEY_WORDS; i++ )  {
		keys[i] = inputKey[i];
	}

	if ( SIMON_KEY_WORDS == 3 ) {
		for ( i = 0; i < SIMON_ROUNDS-SIMON_KEY_WORDS; i++ ) {
			keys[i+SIMON_KEY_WORDS] = c ^ keys[i] ^
							((keys[i+2]>>3) | (keys[i+2]<<(SIMON_WORD_SIZE-3))) ^
							((keys[i+2]>>4) | (keys[i+2]<<(SIMON_WORD_SIZE-4)));
			// period = (SIMON_SEQUENCE_NUMBER == 0 || SIMON_SEQUENCE_NUMBER == 1)?31:62;
			if ( z[SIMON_SEQUENCE_NUMBER][i%62] == 1 ) {
				keys[i+SIMON_KEY_WORDS] ^=  0x1;
			}
		}
	} else if ( SIMON_KEY_WORDS == 4 ) {
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
 * Simon: compute round keys, the word size is 64(block size is 128)
 *
 * inputKey: the initial keys
 * keys: round keys
 */
void setSimonKeys64 ( unsigned long long * inputKey, unsigned long long * keys ) {

	unsigned long long c = 0xfffffffffffffffc;

	int i;
	for ( i = 0; i < SIMON_KEY_WORDS; i++ )  {
		keys[i] = inputKey[i];
	}

	if ( SIMON_KEY_WORDS == 2 ) {
		for ( i = 0; i < SIMON_ROUNDS-SIMON_KEY_WORDS; i++ ) {
			keys[i+SIMON_KEY_WORDS] = c ^ keys[i] ^ 
									((keys[i+1]>>3) | (keys[i+1]<<(SIMON_WORD_SIZE-3))) ^ 
									((keys[i+1]>>4) | (keys[i+1]<<(SIMON_WORD_SIZE-4)));
			if ( z[SIMON_SEQUENCE_NUMBER][i%62] == 1 ) {
				keys[i+SIMON_KEY_WORDS] ^= 0x1 ;
			}
		}
	} else if ( SIMON_KEY_WORDS == 3 ) {
		for ( i = 0; i < SIMON_ROUNDS-SIMON_KEY_WORDS; i++ ) {
			keys[i+SIMON_KEY_WORDS] = c ^ keys[i] ^
							((keys[i+2]>>3) | (keys[i+2]<<(SIMON_WORD_SIZE-3))) ^
							((keys[i+2]>>4) | (keys[i+2]<<(SIMON_WORD_SIZE-4)));
			if ( z[SIMON_SEQUENCE_NUMBER][i%62] == 1 ) {
				keys[i+SIMON_KEY_WORDS] ^=  0x1;
			}
		}
	} else if ( SIMON_KEY_WORDS == 4 ) {
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
 * Speck: compute round keys, the word size is 32(block size is 64)
 *
 * inputKey: the initial keys
 * keys: round keys
 */
void setSpeckKeys32 ( unsigned int * inputKey, unsigned int * keys ) {
	
	/*
	 * inputKey = (L(m-2), ...,L(0),keys(0)),so the rounds of L is bigger than that of keys by m-2(SPECK_KEY_WORDS-2).
	 * The rounds of keys is 'SPECK_ROUNDS, so the rounds of L is 'SPECK_ROUNDS+SPECK_KEY_WORDS-2'.
	 */
	unsigned int * L = new unsigned int[SPECK_ROUNDS+SPECK_KEY_WORDS-2];

	keys[0] = inputKey[SPECK_KEY_WORDS-1];

	/*
	 * the first SPECK_KEY_WORDS-1 rounds of L
	 */
	int i;
	for ( i = 0; i <= SPECK_KEY_WORDS-2; i++ ) {
		L[SPECK_KEY_WORDS-2-i] = inputKey[i];
	}

	/*
	 * compute round keys
	 */
	for ( i = 0; i < SPECK_ROUNDS-1; i++ ) {
		L[i+SPECK_KEY_WORDS-1] = ( keys[i] + ((L[i]>>SPECK_ROT_A)|(L[i]<<(SPECK_WORD_SIZE-SPECK_ROT_A))) ) ^ ((unsigned int)i);
		keys[i+1] = ( (keys[i]<<SPECK_ROT_B)|(keys[i]>>(SPECK_WORD_SIZE-SPECK_ROT_B)) ) ^ L[i+SPECK_KEY_WORDS-1];
	}

	/*
	 * free memory
	 */
	delete L;

}

/*
 * Speck: compute round keys, the word size is 64(block size is 128)
 *
 * inputKey: the initial keys
 * keys: round keys
 */
void setSpeckKeys64 ( unsigned long long * inputKey, unsigned long long * keys ) {
	/*
	 * inputKey = (L(m-2), ...,L(0),keys(0)),so the rounds of L is bigger than that of keys by m-2(SPECK_KEY_WORDS-2).
	 * The rounds of keys is 'SPECK_ROUNDS, so the rounds of L is 'SPECK_ROUNDS+SPECK_KEY_WORDS-2'.
	 */
	unsigned long long * L = new unsigned long long[SPECK_ROUNDS+SPECK_KEY_WORDS-2];

	keys[0] = inputKey[SPECK_KEY_WORDS-1];

	/*
	 * the first SPECK_KEY_WORDS-1 rounds of L
	 */
	int i;
	for ( i = 0; i <= SPECK_KEY_WORDS-2; i++ ) {
		L[SPECK_KEY_WORDS-2-i] = inputKey[i];
	}

	/*
	 * compute round keys
	 */
	for ( i = 0; i < SPECK_ROUNDS-1; i++ ) {
		L[i+SPECK_KEY_WORDS-1] = ( keys[i] + ((L[i]>>SPECK_ROT_A)|(L[i]<<(SPECK_WORD_SIZE-SPECK_ROT_A))) ) ^ ((unsigned int)i);
		keys[i+1] = ( (keys[i]<<SPECK_ROT_B)|(keys[i]>>(SPECK_WORD_SIZE-SPECK_ROT_B)) ) ^ L[i+SPECK_KEY_WORDS-1];
	}

	/*
	 * free memory
	 */
	delete L;
}