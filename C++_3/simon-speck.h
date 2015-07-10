
#include<stdint.h>

/*
 * Simon parameters
 */
#define SIMON_BLOCK_SIZE (64)
#define SIMON_KEY_WORDS (3)

#if (SIMON_BLOCK_SIZE==64)
	#if (SIMON_KEY_WORDS==3)
		#define SIMON_WORD_SIZE (32)
		#define SIMON_SEQUENCE_NUMBER (2)
		#define SIMON_CONST_C (0xfffffffc)
		#define SIMON_ROUNDS (42)
	#elif (SIMON_KEY_WORDS==4)
		#define SIMON_WORD_SIZE (32)
		#define SIMON_SEQUENCE_NUMBER (3)
		#define SIMON_CONST_C (0xfffffffffffffffc)
		#define SIMON_ROUNDS (44)
	#endif
#elif (SIMON_BLOCK_SIZE==128)
	#if (SIMON_KEY_WORDS==2)
		#define SIMON_WORD_SIZE (64)
		#define SIMON_SEQUENCE_NUMBER (2)
		#define SIMON_ROUNDS (68)
	#endif
#endif

typedef  unsigned char u8;
typedef  unsigned short u16;
typedef  unsigned int u32;
typedef  uint64_t u64;

/*
 * Simon£ºword size is 32
 * inputKey£ºthe original keys
 * keys: round keys
 */
void setSimonKeys32 ( u32 * inputKey, u64 * keys );

/*
 * Simon£ºword size is 64
 * inputKey£ºthe original keys
 * keys£ºround keys
 */
void setSimonKeys64 ( u64 * inputKey, u64 * keys );

/*
 * Simon Encryption£ºword size is 32
 * plainText£º
 * keys£ºround keys
 */
void encryptionSimon32 ( u32 * plainText, u32 * keys );

/*
 * Simon Decryption£ºword size is 32
 * cipherText£º
 * keys£ºround keys
 */
void decryptionSimon32 ( u32 * cipherText, u32 * keys );

/*
 * Simon Encryption£ºword size is 64
 * plainText£º
 * keys£ºround keys
 */
void encryptionSimon64 ( u64 * plainText, u64 * keys );

/*
 * Simon Decryption£ºword size is 64
 * cipherText£º
 * keys£ºround keys
 */
void decryptionSimon64 ( u64 * cipherText, u64 * keys );