
#include<iostream>

/*
 * Simon加密：字大小为32
 * plainText：明文
 * keys：密钥
 */
void encryptionSimon32 ( unsigned int * plainText, unsigned int * keys );

/*
 * Simon解密：字大小为32
 * cipherText：密文
 * keys：密钥
 */
void decryptionSimon32 ( unsigned int * cipherText, unsigned int * keys );

/*
 * Simon加密：字大小为32
 * plainText：明文
 * keys：密钥
 */
void encryptionSimon64 ( unsigned long long * plainText, unsigned long long * keys );

/*
 * Simon解密：字大小为64
 * cipherText：密文
 * keys：密钥
 */
void decryptionSimon64 ( unsigned long long * cipherText, unsigned long long * keys );


/*
 * Speck加密：字32位
 * plainText：明文
 * keys：密钥
 */
void encryptionSpeck32 ( unsigned int * plainText, unsigned int * keys );

/*
 * Speck解密：字32位
 * cipherText：密文
 * keys：密钥
 */
void decryptionSpeck32 ( unsigned int * cipherText, unsigned int * keys );

/*
 * Speck加密
 * plainText：明文
 * keys：密钥
 */
void encryptionSpeck64 ( unsigned long long * plainText, unsigned long long * keys );

/*
 * Speck解密
 * cipherText：密文
 * keys：密钥
 */
void decryptionSpeck64 ( unsigned long long * cipherText, unsigned long long * keys );
