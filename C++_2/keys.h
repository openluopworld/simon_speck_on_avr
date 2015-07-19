
/*
 * Simon: compute round keys, the word size is 32(block size is 64)
 *
 * inputKey: the initial keys
 * keys: round keys
 */
void setSimonKeys32 ( unsigned int * inputKey, unsigned int * keys );

/*
 * Simon: compute round keys, the word size is 64(block size is 128)
 *
 * inputKey: the initial keys
 * keys: round keys
 */
void setSimonKeys64 ( unsigned long long * inputKey, unsigned long long * keys );

/*
 * Speck: compute round keys, the word size is 32(block size is 64)
 *
 * inputKey: the initial keys
 * keys: round keys
 */
void setSpeckKeys32 ( unsigned int * inputKey, unsigned int * keys );

/*
 * Speck: compute round keys, the word size is 64(block size is 128)
 *
 * inputKey: the initial keys
 * keys: round keys
 */
void setSpeckKeys64 ( unsigned long long * inputKey, unsigned long long * keys );