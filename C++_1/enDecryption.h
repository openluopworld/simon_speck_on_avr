
#include<iostream>

/*
 * Simon加密
 * plainText：明文
 * keys：密钥
 */
void encryptionSimon ( int * plainText, int ** keys );

/*
 * Simon解密
 * cipherText：密文
 * keys：密钥
 */
void decryptionSimon ( int * cipherText, int ** keys );


/*
 * Speck加密
 * plainText：明文
 * keys：密钥
 */
void encryptionSpeck ( int * plainText, int ** keys );

/*
 * Speck解密
 * cipherText：密文
 * keys：密钥
 */
void decryptionSpeck ( int * cipherText, int ** keys );
