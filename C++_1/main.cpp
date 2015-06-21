#include<iostream>
#include<time.h>
#include"keys.h"
#include"enDecryption.h"
using namespace std;

int SIMON_BLOCK_SIZE;       // Simon明文长度
int SIMON_KEY_SIZE;         // Simon密钥长度
int SIMON_WORD_SIZE;        // Simon字大小
int SIMON_KEY_WORDS;        // Simon密钥字的个数
int SIMON_SEQUENCE_NUMBER;  // Simon对应版本Z的数字
int SIMON_ROUNDS;           // Simon加解密轮数

int SPECK_BLOCK_SIZE;       // Speck块大小
int SPECK_KEY_SIZE;         // Speck密钥大小
int SPECK_WORD_SIZE;        // Speck字大小
int SPECK_KEY_WORDS;        // Speck密钥的字数
int SPECK_ROT_A;            // Speck中移位变量a
int SPECK_ROT_B;            // Speck中移位变量b
int SPECK_ROUNDS;           // Speck加解密轮数

void simon();
void speck();
int simonChoose ();
int speckChoose ();

/*
 * Simon和Speck算法
 *
 * Simon：z0和z1的peoriod（周期）为31，即前31个比特之后，从第32个比特到第62个比特，再重复出现相同的前31个比特，一直这样下去；
 * 同样，z2,z3,z4的周期均为62，即62个比特之后，再重复出现相同的62个比特，一直这样下去。
 *
 * 
 */

int main () {
	
	int flag = 0;
	while( true ) {
		cout<<"*******************************************************************************"<<endl;
		cout<<"                                  1-->SIMON Algorithm                          "<<endl;
		cout<<"                                  2-->SPECK Algorithm                          "<<endl;
		cout<<"                                  3-->EXIT                                     "<<endl;
		cout<<"*******************************************************************************"<<endl<<endl;
		cin>>flag;
		if ( flag == 1 ) {
			simon();
		} else if ( flag == 2 ) {
			speck();
		} else if ( flag == 3 ) {
			break;
		}
	}

	return 0;

}

/*
 * Simon算法
 * 1：64/96
 * 2：64/128
 * 3：128/128
 */
void simon () {
	int flag = simonChoose();
	if ( flag == 1) {
		SIMON_BLOCK_SIZE      = 64;
		SIMON_KEY_SIZE        = 96;
		SIMON_WORD_SIZE       = SIMON_BLOCK_SIZE/2;
		SIMON_KEY_WORDS       = SIMON_KEY_SIZE/SIMON_WORD_SIZE;
		SIMON_SEQUENCE_NUMBER = 2;
		SIMON_ROUNDS          = 42;
	} else if ( flag == 2) {
		SIMON_BLOCK_SIZE      = 64;
		SIMON_KEY_SIZE        = 128;
		SIMON_WORD_SIZE       = SIMON_BLOCK_SIZE/2;
		SIMON_KEY_WORDS       = SIMON_KEY_SIZE/SIMON_WORD_SIZE;
		SIMON_SEQUENCE_NUMBER = 3;
		SIMON_ROUNDS          = 44;
	} else if ( flag == 3) {
		SIMON_BLOCK_SIZE      = 128;
		SIMON_KEY_SIZE        = 128;
		SIMON_WORD_SIZE       = SIMON_BLOCK_SIZE/2;
		SIMON_KEY_WORDS       = SIMON_KEY_SIZE/SIMON_WORD_SIZE;
		SIMON_SEQUENCE_NUMBER = 2;
		SIMON_ROUNDS          = 68;
	}

	int i, j;
	int * plainText       =  new int[SIMON_BLOCK_SIZE];            // 明文比特位的个数
	for ( i = 0; i < SIMON_BLOCK_SIZE; i++ ) {
		plainText[i] = 0;
	}
	int * inputKeys   = new int[SIMON_KEY_SIZE];                   // 密钥比特位的个数
	for ( i = 0; i < SIMON_KEY_SIZE; i++ ) {
		inputKeys[i] = 0;
	}
	int ** keys     = new int* [SIMON_ROUNDS];                     // 密钥，共SIMON_ROUNDS轮，每轮SIMON_WORD_SIZE位
	for ( i = 0; i < SIMON_ROUNDS; i++ ) {
		keys[i] = new int[SIMON_WORD_SIZE];
	}
	for ( i = 0; i < SIMON_ROUNDS; i++ ) {
		for ( j = 0; j < SIMON_WORD_SIZE; j++ ) {
			keys[i][j] = 0;
		}
	}
	int * plainTextHex  = new int[SIMON_BLOCK_SIZE/8];             // 为了简化输入，每个输入以字节表示，[0,255]
	for ( i = 0; i < SIMON_BLOCK_SIZE/8; i++ ) {
		plainTextHex[i] = 0;
	}
	int * inputKeysHex = new int [SIMON_KEY_SIZE/8];               // 以字节表示的密钥，用于保存输入的密钥
	for ( i = 0; i < SIMON_KEY_SIZE/8; i++ ) {
		inputKeysHex[i] = 0;
	}
	int * cipherTextHex = new int [SIMON_BLOCK_SIZE/8];            // 以字节表示的密文
	for ( i = 0; i < SIMON_BLOCK_SIZE/8; i++ ) {
		cipherTextHex[i] = 0;
	}

	/*
	 * 得到明文和初始密钥
	 */
	cout<<"Please input the plainText of "<<SIMON_BLOCK_SIZE/8<<" bytes:"<<endl;
	for ( i = 0; i < SIMON_BLOCK_SIZE/8; i++ ) {
		cin>>plainTextHex[i];
	}
	cout<<"Please input the key of "<<SIMON_KEY_SIZE/8<<" bytes:"<<endl;
	for ( i = 0; i < SIMON_KEY_SIZE/8; i++ ) {
		cin>>inputKeysHex[i];
	}

	/*
	 * 将明文和密钥以二进制形式表示
	 */
	int index;
	for ( i = 0; i < SIMON_BLOCK_SIZE/8; i++ ) {
		index = 7;
		while ( plainTextHex[i] != 0 ) {
			plainText[i*8+index] = plainTextHex[i]%2;
			plainTextHex[i] /= 2;
			index--;
		}
	}
	for ( i = 0; i < SIMON_KEY_SIZE/8; i++ ) {
		index = 7;
		while ( inputKeysHex[i] != 0 ) {
			inputKeys[i*8+index] = inputKeysHex[i]%2;
			inputKeysHex[i] /= 2;
			index--;
		}
	}

	time_t start, end;
	int recordCount = 0;
	for ( recordCount = 1; recordCount <= 32; recordCount*=2 ) {
		start = clock();
		for ( j = 0; j < 10000*recordCount; j++ ) {
			/*
			 * 计算每轮的密钥
			 */
			setSimonKeys ( inputKeys, keys );
		}
		end = clock();
		/*
		 * = (10000*recordCount*SIMON_KEY_SIZE)/(end-start) 比特每毫秒
		 * = (1000*10000*recordCount*SIMON_KEY_SIZE)/(end-start) 比特每秒
		 * = (1000*10000*recordCount*SIMON_KEY_SIZE)/（1024*1024*(end-start)) 兆比特每秒
		 * = (10.0*recordCount*SIMON_KEY_SIZE)/（.1024*1.024*(end-start)) 兆比特每秒
		 */
		printf("Keys extend: %d, speed: %0.2f Mbps.\n", 10000*recordCount, 10.0*recordCount*SIMON_KEY_SIZE/(1.024*1.024*(end-start )));
	}

	
	/*
	 * 保存明文副本，每次加密前重新恢复输入的明文。多次加密求平均可以减少因为加密时间太短带来的计算误差
	 */
	int * tempPlainText = new int[SIMON_BLOCK_SIZE];
	for ( i = 0; i < SIMON_BLOCK_SIZE; i++ ) {
		tempPlainText[i] = plainText[i];
	}

	/*
	 * 多次记录，测试速度是否稳定
	 */
	for ( recordCount = 1; recordCount <= 32; recordCount*=2 ) {
		/*
		 * 开始时间
		 */
		start = clock();
		/*
		 * 计算多次取平均值
		 */
		for ( j = 0; j < 10000*recordCount; j++ ) {
			for ( i = 0; i < SIMON_BLOCK_SIZE; i++ ) {
				plainText[i] = tempPlainText[i];
			}
			/*
			 * 加密
			 */
			encryptionSimon ( plainText, keys );
		}
		/*
		 * 结束时间
		 */
		end = clock();
		/*
		 * = (10000*recordCount*SIMON_BLOCK_SIZE)/(end-start) 比特每毫秒
		 * = (1000*10000*recordCount*SIMON_BLOCK_SIZE)/(end-start) 比特每秒
		 * = (1000*10000*recordCount*SIMON_BLOCK_SIZE)/（1024*1024*(end-start)) 兆比特每秒
		 * = (10.0*recordCount*SIMON_BLOCK_SIZE)/（.1024*1.024*(end-start)) 兆比特每秒
		 */
		printf("Encryption counts: %d, speed: %0.2f Mbps.\n", 10000*recordCount, 10.0*recordCount*SIMON_BLOCK_SIZE/(1.024*1.024*(end-start )));
	}

	/*
	 * 将每8个比特位转化为字节所对应的数值，并输出
	 */
	for ( i = 0; i < SIMON_BLOCK_SIZE/8; i++ ) {
		for ( j = 0; j < 8; j++ ) {
			cipherTextHex[i] = cipherTextHex[i]*2 + plainText[i*8+j];
		}
	}
	cout<<"After encryption:"<<endl;
	for ( i = 0; i < SIMON_BLOCK_SIZE/8; i++ ) {
		cout<<cipherTextHex[i]<<"  ";
	}
	cout<<endl;

	/*
	 * 解密，并输出解密后的结果
	 */
	decryptionSimon ( plainText, keys );
	for ( i = 0; i < SIMON_BLOCK_SIZE/8; i++ ) {
		// 清空结果，之前保存的是加密的结果
		cipherTextHex[i] = 0;      
		for ( j = 0; j < 8; j++ ) {
			cipherTextHex[i] = cipherTextHex[i]*2 + plainText[i*8+j];
		}
	}
	cout<<"After decryption:"<<endl;
	for ( i = 0; i < SIMON_BLOCK_SIZE/8; i++ ) {
		cout<<cipherTextHex[i]<<"  ";
	}
	cout<<endl;

	/*
	 * 释放内存
	 */
	delete tempPlainText;
	delete cipherTextHex;
	delete inputKeysHex;
	delete plainTextHex;
	// 注意数组释放的顺序
	for ( int i = 0; i < SIMON_ROUNDS; i++ ) {
		delete keys[i];
	}
	delete keys;
	delete inputKeys;
	delete plainText;

}

/*
 * 选择Simon算法
 * 1：64/96版本
 * 2：64/128版本
 * 3：128/128版本
 */
int simonChoose () {
	int flag = 0;
	cout<<"***********************************   Simon   *********************************"<<endl;
	cout<<"                                  1-->64/96                                    "<<endl;
	cout<<"                                  2-->64/128                                   "<<endl;
	cout<<"                                  3-->128/128                                  "<<endl;
	cout<<"*******************************************************************************"<<endl<<endl;
	cin>>flag;
	return flag;
}

/*
 * Speck算法
 * 1：64/96版本
 * 2：64/128版本
 * 3：128/128版本
 */
void speck () {
	int flag = speckChoose();
	if ( flag == 1 ) {
		SPECK_BLOCK_SIZE = 64;
		SPECK_KEY_SIZE   = 96;
		SPECK_WORD_SIZE  = SPECK_BLOCK_SIZE/2;
		SPECK_KEY_WORDS  = SPECK_KEY_SIZE/SPECK_WORD_SIZE;
		SPECK_ROT_A      = 8;
		SPECK_ROT_B      = 3;
		SPECK_ROUNDS     = 26;
	} else if ( flag == 2 ) {
		SPECK_BLOCK_SIZE = 64;
		SPECK_KEY_SIZE   = 128;
		SPECK_WORD_SIZE  = SPECK_BLOCK_SIZE/2;
		SPECK_KEY_WORDS  = SPECK_KEY_SIZE/SPECK_WORD_SIZE;
		SPECK_ROT_A      = 8;
		SPECK_ROT_B      = 3;
		SPECK_ROUNDS     = 27;
	} else if ( flag == 3 ) {
		SPECK_BLOCK_SIZE = 128;
		SPECK_KEY_SIZE   = 128;
		SPECK_WORD_SIZE  = SPECK_BLOCK_SIZE/2;
		SPECK_KEY_WORDS  = SPECK_KEY_SIZE/SPECK_WORD_SIZE;
		SPECK_ROT_A      = 8;
		SPECK_ROT_B      = 3;
		SPECK_ROUNDS     = 32;
	}

	int i, j;
	int * plainText      =  new int[SPECK_BLOCK_SIZE];            // 明文
	for ( i = 0; i < SPECK_BLOCK_SIZE; i++ ) {
		plainText[i] = 0;
	}
	int * inputKeys   = new int[SPECK_KEY_SIZE];                   // 密钥
	for ( i = 0; i < SPECK_KEY_SIZE; i++ ) {
		inputKeys[i] = 0;
	}
	int ** keys     = new int * [SPECK_ROUNDS];                     // 密钥，共SIMON_ROUNDS轮，每轮SIMON_WORD_SIZE位
	for ( i = 0; i < SPECK_ROUNDS; i++ ) {
		keys[i] = new int[SPECK_WORD_SIZE];
	}
	for ( i = 0; i < SPECK_ROUNDS; i++ ) {
		for ( j = 0; j < SPECK_WORD_SIZE; j++ ) {
			keys[i][j] = 0;
		}
	}
	int * plainTextHex  = new int[SPECK_BLOCK_SIZE/8];             // 以字节表示的明文，用于保存输入的明文
	for ( i = 0; i < SPECK_BLOCK_SIZE/8; i++ ) {
		plainTextHex[i] = 0;
	}
	int * inputKeysHex = new int [SPECK_KEY_SIZE/8];               // 以字节表示的密钥，用于保存输入的密钥
	for ( i = 0; i < SPECK_KEY_SIZE/8; i++ ) {
		inputKeysHex[i] = 0;
	}
	int * cipherTextHex = new int [SPECK_BLOCK_SIZE/8];            // 以字节表示的密文
	for ( i = 0; i < SPECK_BLOCK_SIZE/8; i++ ) {
		cipherTextHex[i] = 0;
	}

	/*
	 * 得到明文和初始密钥
	 */
	cout<<"Please input the plainText of "<<SPECK_BLOCK_SIZE/8<<" bytes:"<<endl;
	for ( i = 0; i < SPECK_BLOCK_SIZE/8; i++ ) {
		cin>>plainTextHex[i];
	}
	cout<<"Please input the key of "<<SPECK_KEY_SIZE/8<<" bytes:"<<endl;
	for ( i = 0; i < SPECK_KEY_SIZE/8; i++ ) {
		cin>>inputKeysHex[i];
	}

	/*
	 * 将明文和密钥以二进制形式表示
	 */
	int index;
	for ( i = 0; i < SPECK_BLOCK_SIZE/8; i++ ) {
		index = 7;
		while ( plainTextHex[i] != 0 ) {
			plainText[i*8+index] = plainTextHex[i]%2;
			plainTextHex[i] /= 2;
			index--;
		}
	}
	for ( i = 0; i < SPECK_KEY_SIZE/8; i++ ) {
		index = 7;
		while ( inputKeysHex[i] != 0 ) {
			inputKeys[i*8+index] = inputKeysHex[i]%2;
			inputKeysHex[i] /= 2;
			index--;
		}
	}

	time_t start, end;
	int recordCount = 0;
	for ( recordCount = 1; recordCount <= 32; recordCount*=2 ) {
		start = clock();
		for ( j = 0; j < 10000*recordCount; j++ ) {
			/*
			 * 计算每轮的密钥
			 */
			setSpeckKeys( inputKeys, keys );;
		}
		end = clock();
		printf("Keys extend: %d, speed: %0.2f Mbps.\n", 10000*recordCount, 10.0*recordCount*SPECK_KEY_SIZE/(1.024*1.024*(end-start )));
	}
		

	/*
	 * 保存副本 
	 */
	int * tempPlainText = new int[SPECK_BLOCK_SIZE];
	for ( i = 0; i < SPECK_BLOCK_SIZE; i++ ) {
		tempPlainText[i] = plainText[i];
	}

	/*
	 * 多次记录，测试速度是否稳定
	 */
	for ( recordCount = 1; recordCount <= 32; recordCount*=2 ) {
		start = clock();
		/* 
		 * 计算多次取平均值
		 */
		for ( j = 0; j < 10000*recordCount; j++ ) {
			for ( i = 0; i < SPECK_BLOCK_SIZE; i++ ) {
				plainText[i] = tempPlainText[i];
			}
			encryptionSpeck ( plainText, keys );
		}
		end = clock();
		/*
		 * = (10000*recordCount*SPECK_BLOCK_SIZE)/(end-start) 比特每毫秒
		 * = (1000*10000*recordCount*SPECK_BLOCK_SIZE)/(end-start) 比特每秒
		 * = (1000*10000*recordCount*SPECK_BLOCK_SIZE)/（1024*1024*(end-start)) 兆比特每秒
		 * = (10.0*recordCount*SPECK_BLOCK_SIZE)/（1.024*1.024*(end-start)) 兆比特每秒
		 */
		printf("Encryption counts: %d, speed: %0.2f Mbps.\n", 10000*recordCount, 10.0*recordCount*SPECK_BLOCK_SIZE/(1.024*1.024*(end-start )));
	}

	for ( i = 0; i < SPECK_BLOCK_SIZE/8; i++ ) {
		cipherTextHex[i] = 0;
		for ( j = 0; j < 8; j++ ) {
			cipherTextHex[i] = cipherTextHex[i]*2 + plainText[i*8+j];
		}
	}
	cout<<"After encryption:"<<endl;
	for ( i = 0; i < SPECK_BLOCK_SIZE/8; i++ ) {
		cout<<cipherTextHex[i]<<"  ";
	}
	cout<<endl;

	/*
	 * 解密，并输出解密后的结果
	 */
	decryptionSpeck ( plainText, keys );
	for ( i = 0; i < SPECK_BLOCK_SIZE/8; i++ ) {
		cipherTextHex[i] = 0;      // 清空结果，之前保存的是加密的结果
		for ( j = 0; j < 8; j++ ) {
			cipherTextHex[i] = cipherTextHex[i]*2 + plainText[i*8+j];
		}
	}
	cout<<"After decryption:"<<endl;
	for ( i = 0; i < SPECK_BLOCK_SIZE/8; i++ ) {
		cout<<cipherTextHex[i]<<"  ";
	}
	cout<<endl;

	/*
	 * 释放内存
	 */
	delete tempPlainText;
	delete cipherTextHex;
	delete inputKeysHex;
	delete plainTextHex;
	// 注意数组释放的顺序
	for ( int i = 0; i < SPECK_ROUNDS; i++ ) {
		delete keys[i];
	}
	delete keys;
	delete inputKeys;
	delete plainText;

}

/*
 * 选择Speck算法
 * 1：64/96版本
 * 2：64/128版本
 * 3：128/128版本
 */
int speckChoose () {
	int flag = 0;
	cout<<"***********************************   Speck   *********************************"<<endl;
	cout<<"                                  1-->64/96                                    "<<endl;
	cout<<"                                  2-->64/128                                   "<<endl;
	cout<<"                                  3-->128/128                                  "<<endl;
	cout<<"*******************************************************************************"<<endl<<endl;
	cin>>flag;
	return flag;
}