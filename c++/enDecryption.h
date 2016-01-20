
#include<iostream>

/*
 * Simon encryption: word size is 32(block size is 64)
 *
 * plainText: the point of plainText. There is only one block. plainText[0] is the higher parter, [1] is the lower.
 * keys: round keys
 */
void encryptionSimon32 ( unsigned int * plainText, unsigned int * keys );

/*
 * Simon decryption: word size is 32(block size is 64)
 *
 * cipherText: the point of cipherText. There is only one block. cipherText[0] is the higher part, [1] is the lower.
 * keys: round keys
 */
void decryptionSimon32 ( unsigned int * cipherText, unsigned int * keys );

/*
 * Simon encryption: word size is 64(block size is 128)
 *
 * plainText: the point of plainText. There is only one block. plainText[0] is the higher parter, [1] is the lower.
 * keys: round keys
 */
void encryptionSimon64 ( unsigned long long * plainText, unsigned long long * keys );

/*
 * Simon decryption: word size is 64(block size is 128)
 *
 * cipherText: the point of cipherText. There is only one block. cipherText[0] is the higher part, [1] is the lower.
 * keys: round keys
 */
void decryptionSimon64 ( unsigned long long * cipherText, unsigned long long * keys );


/*
 * Speck encryption: word size is 32(block size is 64)
 *
 * plainText: the point of plainText. There is only one block. plainText[0] is the higher parter, [1] is the lower.
 * keys: round keys
 */
void encryptionSpeck32 ( unsigned int * plainText, unsigned int * keys );

/*
 * Speck decryption: word size is 32(block size is 64)
 *
 * cipherText: the point of cipherText. There is only one block. cipherText[0] is the higher parter, [1] is the lower.
 * keys: round keys
 */
void decryptionSpeck32 ( unsigned int * cipherText, unsigned int * keys );

/*
 * Speck encryption: word size is 64(block size is 128)
 *
 * plainText: the point of plainText. There is only one block. plainText[0] is the higher parter, [1] is the lower.
 * keys: round keys
 */
void encryptionSpeck64 ( unsigned long long * plainText, unsigned long long * keys );

/*
 * Speck decryption: word size is 64(block size is 128)
 *
 * cipherText: the point of cipherText. There is only one block. cipherText[0] is the higher parter, [1] is the lower.
 * keys: round keys
 */
void decryptionSpeck64 ( unsigned long long * cipherText, unsigned long long * keys );
