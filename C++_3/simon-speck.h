
#include<stdint.h>

/*
 * Simon parameters
 */
#define SIMON_BLOCK_SIZE (64) // the initial value
#define SIMON_KEY_WORDS (3) // the initail value

#if (SIMON_BLOCK_SIZE==64)
	#define SIMON_WORD_SIZE (32)
	#define SIMON_CONST_C (0xfffffffc)
	#if (SIMON_KEY_WORDS==3)
		#define SIMON_SEQUENCE_NUMBER (2)
		#define SIMON_ROUNDS (42)
	#elif (SIMON_KEY_WORDS==4)
		#define SIMON_SEQUENCE_NUMBER (3)
		#define SIMON_ROUNDS (44)
	#endif
#elif (SIMON_BLOCK_SIZE==128)
	#define SIMON_WORD_SIZE (64)
	#define SIMON_CONST_C (0xfffffffffffffffc)
	#if (SIMON_KEY_WORDS==2)
		#define SIMON_SEQUENCE_NUMBER (2)
		#define SIMON_ROUNDS (68)
	#endif
#endif

typedef  unsigned char u8;
typedef  unsigned short u16;
typedef  unsigned int u32;
typedef  uint64_t u64;

/*
 * rotate shift left x by n bits
 */
inline u32 rol32 (u32 x, int n);

/*
 * rotate shift left x by n bits
 */
inline u64 rol64 (u64 x, int n);

/*
 * rotate shift right x by n bits
 */
inline u32 ror32 (u32 x, int n);

/*
 * rotate shift right x by n bits
 */
inline u64 ror64 (u64 x, int n);

/*
 * Simon£ºword size is 32
 * inputKey£ºthe original keys
 * keys: round keys
 */
void setSimonKeys32 ( u32 * inputKey, u32 * keys );

/*
 * Simon£ºword size is 64
 * inputKey£ºthe original keys
 * keys£ºround keys
 */
void setSimonKeys64 ( u64 * inputKey, u64 * keys );

/*
 * Simon Encryption£ºword size is 32
 * plainText£ºplainText[0] is the higher word, [1] is the lower. 
 *            plainText has just one block.
 * keys£ºround keys
 */
void encryptionSimon32 ( u32 * plainText, u32 * keys );

/*
 * Simon Decryption£ºword size is 32
 * cipherText£ºcipherText[0] is the higher word, [1] is the lower.
 *             cipherText has just one block.
 * keys£ºround keys
 */
void decryptionSimon32 ( u32 * cipherText, u32 * keys );

/*
 * Simon Encryption£ºword size is 64
 * plainText£ºplainText[0] is the higher word, [1] is the lower. 
 *            plainText has just one block.
 * keys£ºround keys
 */
void encryptionSimon64 ( u64 * plainText, u64 * keys );

/*
 * Simon Decryption£ºword size is 64
 * cipherText£ºcipherText[0] is the higher word, [1] is the lower.
 *             cipherText has just one block.
 * keys£ºround keys
 */
void decryptionSimon64 ( u64 * cipherText, u64 * keys );