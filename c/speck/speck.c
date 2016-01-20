
#include "simon-speck.h"
#include "primitives.h"

/*
 * key schedule
 * inputKey: the original keys
 * keys: round keys
 */
void encryptKeySchedule(const u8 * inputKey, u8 * keys ) {

	
}

/*
 * encrypt
 * plainText: plainText[0] is the lower word(y), [1] is the higher(x). 
 *            plainText has just one block.
 * keys: round keys
 */
void encrypt(u8 * plainText, const u8 * keys ) {

	
}

/*
 * decrypt
 * cipherText: cipherText[0] is the lower word(y), [1] is the higher(x).
 *             cipherText has just one block.
 * keys: round keys
 */
void decrypt(u8 * cipherText, const u8 * keys ) {

	
}
