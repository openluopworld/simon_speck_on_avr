
#ifndef SPECK
#define SPECK

#include <stdint.h>

#define SPECK_BLOCK_SIZE (64)
#define SPECK_WORD_SIZE (32)
#define SPECK_A (8)
#define SPECK_B (3)
#define SPECK_KEY_WORDS (4)
#if (SPECK_KEY_WORDS==3)
	#define SPECK_ROUNDS (26)
#elif (SPECK_KEY_WORDS==4)
	#define SPECK_ROUNDS (27)
#endif

typedef  uint8_t u8;
typedef  uint16_t u16;
typedef  uint32_t u32;
typedef  uint64_t u64;

#endif
