
/*
 * Simon：计算密钥，字大小为32
 * inputKey：初始的密钥
 * keys：计算后得到的每轮密钥
 */
void setSimonKeys32 ( unsigned int * inputKey, unsigned int * keys );

/*
 * Simon：计算密钥，字大小为64
 * inputKey：初始的密钥
 * keys：计算后得到的每轮密钥
 */
void setSimonKeys64 ( unsigned long long * inputKey, unsigned long long * keys );

/*
 * Speck：计算密钥
 * inputKey：初始的密钥
 * keys：计算后得到的每轮密钥
 */
void setSpeckKeys32 ( unsigned int * inputKey, unsigned int * keys );

/*
 * Speck：计算密钥:64位
 * inputKey：初始的密钥
 * keys：计算后得到的每轮密钥
 */
void setSpeckKeys64 ( unsigned long long * inputKey, unsigned long long * keys );