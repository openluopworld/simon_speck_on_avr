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
 * Version:2.0
 * Time:2015-1-10
 *
 * 字大小为32位时，用unsigned int代替，由于要进行计算，所以必须是非负数；为64位时用unsigned long long代替
 *
 */
int main () {
	//unsigned int temp1 = 0x00000000;
	//unsigned int temp2 = 0xffffffff;
	//cout<<temp1-temp2<<endl;
	//cout<<temp1<<endl;
	//cout<<temp2<<endl;

	//cout<<sizeof(unsigned long long)<<endl;

	//cout<<7/3<<endl;

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
 * 
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

	/*
	 * SIMON_WORD_SIZE为32位时，用unsigned int表示；此时输入一共64位，分别用两个unsigned int表示高32位和低32位
	 *
	 * SIMON_WORD_SIZE为64位时，用unsigned long long表示
	 */
	if ( SIMON_WORD_SIZE == 32 ) {
		int i, j;
		unsigned int * plainText       = new unsigned int[2]; // 明文
		plainText[0] = plainText[1]    = 0x0;
		unsigned int * inputKeys       = new unsigned int[SIMON_KEY_WORDS]; // 初始密钥，一共有SIMON_KEY_WORDS个
		for ( i = 0; i < SIMON_KEY_WORDS; i++ ) {
			inputKeys[i]               = 0x0;
		}
		unsigned int * keys            = new unsigned int[SIMON_ROUNDS]; // 最终的密钥，一共SIMON_ROUNDS轮

		unsigned int * plainTextByte   = new unsigned int[SIMON_BLOCK_SIZE/8]; // 为了简化输入，每个输入以字节表示，[0,255]
		for ( i = 0; i < SIMON_BLOCK_SIZE/8; i++ ) {
			plainTextByte[i]           = 0x0;
		}
		unsigned int * inputKeysByte   = new unsigned int[SIMON_KEY_SIZE/8];  // 以字节表示的密钥，用于保存输入的密钥
		for ( i = 0; i < SIMON_KEY_SIZE/8; i++ ) {
			inputKeysByte[i]           = 0x0;
		}
		unsigned int * cipherTextByte  = new unsigned int[SIMON_BLOCK_SIZE/8];// 以字节表示的密文
		for ( i = 0; i < SIMON_BLOCK_SIZE/8; i++ ) {
			cipherTextByte[i]          = 0x0;
		}

		/*
		 * 得到输入明文和初始密钥
		 */
		cout<<"Please input the plainText of "<<SIMON_BLOCK_SIZE/8<<" bytes:"<<endl;
		for ( i = 0; i < SIMON_BLOCK_SIZE/8; i++ ) {
			cin>>plainTextByte[i];
		}
		cout<<"Please input the key of "<<SIMON_KEY_SIZE/8<<" bytes:"<<endl;
		for ( i = 0; i < SIMON_KEY_SIZE/8; i++ ) {
			cin>>inputKeysByte[i];
		}

		/*
		 * 此时SIMON_BLOCK_SIZE为64，一共8个字节，高四个字节组成plainText[0]，低四个字节组成plainText[1]
		 */
		for ( i = 0; i < 4; i++ ) {
			plainText[0]  = plainText[0] * 256 + plainTextByte[i];
		}
		for ( i = 4; i < SIMON_BLOCK_SIZE/8; i++ ) { 
			plainText[1]  = plainText[1] * 256 + plainTextByte[i];
		}
		//cout<<endl<<endl<<plainText[1]/(256*256*256)<<","<<plainText[1]/(256*256)%256<<","<<plainText[1]%(256*256)/256<<","<<plainText[1]%256<<endl;
		/*
		 * 密钥是从右往左开始计算，则index最小的表示的是k最大的
		 * 比如SIMON_KEY_WORDS=3；则inputKeysByte[0...3]=k2;inputKeysByte[4...7]=k1;inputKeysByte[8...11]=k0
		 */
		for ( i = 0; i < SIMON_KEY_SIZE/8; i++ ) {
			inputKeys[SIMON_KEY_WORDS-1-i/4] = inputKeys[SIMON_KEY_WORDS-1-i/4] * 256 + inputKeysByte[i];
		}

		time_t start, end;
		int recordCount = 0;
		for ( recordCount = 4; recordCount <= 1024; recordCount*=2 ) {
			start = clock();
			for ( j = 0; j < 10000*recordCount; j++ ) {
				/*
				 * 计算每轮的密钥
				 */
				setSimonKeys32 ( inputKeys, keys );
			}
			end = clock();
			/*
			 * = (10000*recordCount*SIMON_KEY_SIZE)/(end-start) 比特每毫秒
			 * = (1000*10000*recordCount*SIMON_KEY_SIZE)/(end-start) 比特每秒
			 * = (1000*10000*recordCount*SIMON_KEY_SIZE)/（1024*1024*(end-start)) 兆比特每秒
			 * = (10.0*recordCount*SIMON_KEY_SIZE)/（.1024*1.024*(end-start)) 兆比特每秒
			 */
			printf("Keys extend: %d, speed: %0.3f Mbps.\n", 10000*recordCount, 10.0*recordCount*SIMON_KEY_SIZE/(1.024*1.024*(end-start )));
		}

		for ( int i = 0; i < SIMON_ROUNDS; i++ ) {
			cout<<"秘钥："<<keys[i]%256<<","<<(keys[i]/256%256)<<","<<(keys[i]/(256*256)%256)<<","<<keys[i]/(256*256*256)<<endl;
		}

		/*
		 * 保存明文副本，每次加密前重新恢复输入的明文。多次加密求平均可以减少因为加密时间太短带来的计算误差
		 */
		unsigned int tempPlainHigher   = plainText[0];
		unsigned int tempPlainLower    = plainText[1];
		//cout<<endl<<endl<<plainText[1]/(256*256*256)<<","<<plainText[1]/(256*256)%256<<","<<plainText[1]%(256*256)/256<<","<<plainText[1]%256<<endl;

		/*
		 * 多次记录，测试速度是否稳定
		 */
		for ( recordCount = 4; recordCount <= 1024; recordCount*=2 ) {
			/*
			 * 开始时间
			 */
			start = clock();
			/*
			 * 计算多次取平均值
			 */
			for ( j = 0; j < 10000*recordCount; j++ ) {
				plainText[0]  = tempPlainHigher;
				plainText[1]  = tempPlainLower;
				/*
				 * 加密
				 */
				encryptionSimon32 ( plainText, keys );
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
			printf("Encryption counts: %d, speed: %0.3f Mbps.\n", 10000*recordCount, 10.0*recordCount*SIMON_BLOCK_SIZE/(1.024*1.024*(end-start )));
		}

		/*
		 * 加密后的数据以字节形式输出
		 */
		tempPlainHigher   = plainText[0];
		tempPlainLower    = plainText[1];
		for ( i = 3; i >= 0; i-- ) {
			cipherTextByte[i] = tempPlainHigher % 256;
			tempPlainHigher   /= 256;
		}

		/*
		 * 错误：for ( i = SIMON_BLOCK_SIZE/8; i >= 4; i-- ) {
		 * 
		 * 结果运行的结果，第5位（从左往右）是错误的，总是为0，而且最终释放内存时会出现错误
		 * 就是因为数组越界，导致整体结果向后移了一位
		 */
		for ( i = SIMON_BLOCK_SIZE/8-1; i >= 4; i-- ) {
			cipherTextByte[i] = tempPlainLower % 256;
			tempPlainLower    /= 256;
		}
		cout<<"After encryption:"<<endl;
		for ( i = 0; i < SIMON_BLOCK_SIZE/8; i++ ) {
			cout<<cipherTextByte[i]<<"  ";
		}
		cout<<endl;

		/*
		 * 解密，并输出解密后的结果
		 */
		decryptionSimon32 ( plainText, keys );
		for ( i = 3; i >= 0; i-- ) {
			cipherTextByte[i] = plainText[0] % 256;
			plainText[0]      /= 256;
		}
		for ( i = SIMON_BLOCK_SIZE/8-1; i >= 4; i-- ) {
			cipherTextByte[i] = plainText[1] % 256;
			plainText[1]      /= 256;
		}
		cout<<"After decryption:"<<endl;
		for ( i = 0; i < SIMON_BLOCK_SIZE/8; i++ ) {
			cout<<cipherTextByte[i]<<"  ";
		}
		cout<<endl;

		/*
		 * 释放内存
		 */
		delete cipherTextByte;
		delete inputKeysByte;
		delete plainTextByte;
		delete keys;
		delete inputKeys;
		delete plainText;

	} else if ( SIMON_WORD_SIZE == 64 ){

		/*unsigned long long temp = 0x8000000000000000;
		cout<<temp<<endl;*/
		int i, j;
		unsigned long long * plainText       = new unsigned long long[2];
		plainText[0] = plainText[1]    = 0x0;
		unsigned long long * inputKeys       = new unsigned long long[SIMON_KEY_WORDS]; // 初始密钥，一共有SIMON_KEY_WORDS个
		for ( i = 0; i < SIMON_KEY_WORDS; i++ ) {
			inputKeys[i]               = 0x0;
		}
		unsigned long long * keys            = new unsigned long long[SIMON_ROUNDS]; // 最终的密钥，一共SIMON_WORD_SIZE轮
		unsigned long long * plainTextByte   = new unsigned long long[SIMON_BLOCK_SIZE/8]; // 为了简化输入，每个输入以字节表示，[0,255]
		for ( i = 0; i < SIMON_BLOCK_SIZE/8; i++ ) {
			plainTextByte[i]           = 0x0;
		}
		unsigned long long * inputKeysByte   = new unsigned long long[SIMON_KEY_SIZE/8]; // 以字节表示的密钥，用于保存输入的密钥
		for ( i = 0; i < SIMON_KEY_SIZE/8; i++ ) {
			inputKeysByte[i]           = 0x0;
		}
		unsigned long long * cipherTextByte  = new unsigned long long[SIMON_BLOCK_SIZE/8]; // 以字节表示的密文
		for ( i = 0; i < SIMON_BLOCK_SIZE/8; i++ ) {
			cipherTextByte[i]          = 0x0;
		}

		/*
		 * 得到输入明文和初始密钥
		 */
		cout<<"Please input the plainText of "<<SIMON_BLOCK_SIZE/8<<" bytes:"<<endl;
		for ( i = 0; i < SIMON_BLOCK_SIZE/8; i++ ) {
			cin>>plainTextByte[i];
		}
		cout<<"Please input the key of "<<SIMON_KEY_SIZE/8<<" bytes:"<<endl;
		for ( i = 0; i < SIMON_KEY_SIZE/8; i++ ) {
			cin>>inputKeysByte[i];
		}

		/*
		 * 此时SIMON_BLOCK_SIZE为128，一共16个字节
		 */
		for ( i = 0; i < 8; i++ ) {
			plainText[0]  = plainText[0] * 256 + plainTextByte[i];
		}
		for ( i = 8; i < SIMON_BLOCK_SIZE/8; i++ ) { 
			plainText[1]  = plainText[1] * 256 + plainTextByte[i];
		}
		/*
		 * 密钥是从右往左开始计算，则index最小的表示的是k最大的
		 * 比如SIMON_KEY_WORDS=3；则inputKeysByte[0...3]=k2;inputKeysByte[4...7]=k1;inputKeysByte[8...11]=k0
		 */
		for ( i = 0; i < SIMON_KEY_SIZE/8; i++ ) {
			inputKeys[SIMON_KEY_WORDS-1-i/8] = inputKeys[SIMON_KEY_WORDS-1-i/8] * 256 + inputKeysByte[i];
		}

		time_t start, end;
		int recordCount = 0;
		for ( recordCount = 4; recordCount <= 1024; recordCount*=2 ) {
			start = clock();
			for ( j = 0; j < 10000*recordCount; j++ ) {
				/*
				 * 计算每轮的密钥
				 */
				setSimonKeys64 ( inputKeys, keys );
			}
			end = clock();
			/*
			 * = (10000*recordCount*SIMON_KEY_SIZE)/(end-start) 比特每毫秒
			 * = (1000*10000*recordCount*SIMON_KEY_SIZE)/(end-start) 比特每秒
			 * = (1000*10000*recordCount*SIMON_KEY_SIZE)/（1024*1024*(end-start)) 兆比特每秒
			 * = (10.0*recordCount*SIMON_KEY_SIZE)/（1.1024*1.024*(end-start)) 兆比特每秒
			 */
			printf("Keys extend: %d, speed: %0.3f Mbps.\n", 10000*recordCount, 10.0*recordCount*SIMON_KEY_SIZE/(1.024*1.024*(end-start )));
		}

		/*
		 * 保存明文副本，每次加密前重新恢复输入的明文。多次加密求平均可以减少因为加密时间太短带来的计算误差
		 */
		unsigned long long tempPlainHigher   = plainText[0];
		unsigned long long tempPlainLower    = plainText[1];
		
		/*
		 * 多次记录，测试速度是否稳定
		 */
		for ( recordCount = 4; recordCount <= 1024; recordCount*=2 ) {
			/*
			 * 开始时间
			 */
			start = clock();
			/*
			 * 计算多次取平均值
			 */
			for ( j = 0; j < 10000*recordCount; j++ ) {
				plainText[0]  = tempPlainHigher;
				plainText[1]  = tempPlainLower;
				/*
				 * 加密
				 */
				encryptionSimon64 ( plainText, keys );
			}
			/*
			 * 结束时间
			 */
			end = clock();
			/*
			 * = (10000*recordCount*SIMON_BLOCK_SIZE)/(end-start) 比特每毫秒
			 * = (1000*10000*recordCount*SIMON_BLOCK_SIZE)/(end-start) 比特每秒
			 * = (1000*10000*recordCount*SIMON_BLOCK_SIZE)/（1024*1024*(end-start)) 兆比特每秒
			 * = (10.0*recordCount*SIMON_BLOCK_SIZE)/（1.1024*1.024*(end-start)) 兆比特每秒
			 */
			printf("Encryption counts: %d, speed: %0.3f Mbps.\n", 10000*recordCount, 10.0*recordCount*SIMON_BLOCK_SIZE/(1.024*1.024*(end-start )));
		}

		printf("\n\nKeys\n");
		for ( i = 0; i < SIMON_ROUNDS; i++ ) {
			printf("%x,%x,%x,%x\n", keys[i]%256, keys[i]/256%256, keys[i]/256/256%256, keys[i]/256/256/256);
		}
		printf("\n\nEnd\n");


		/*
		 * 将每8个比特位转化为字节所对应的数值，并输出
		 */
		tempPlainHigher   = plainText[0];
		tempPlainLower    = plainText[1];
		for ( i = 7; i >= 0; i-- ) {
			cipherTextByte[i] = tempPlainHigher % 256;
			tempPlainHigher   /= 256;
		}
		for ( i = SIMON_BLOCK_SIZE/8-1; i >= 8; i-- ) {
			cipherTextByte[i] = tempPlainLower % 256;
			tempPlainLower    /= 256;
		}
		cout<<"After encryption:"<<endl;
		for ( i = 0; i < SIMON_BLOCK_SIZE/8; i++ ) {
			cout<<cipherTextByte[i]<<"  ";
		}
		cout<<endl;

		/*
		 * 解密，并输出解密后的结果
		 */
		decryptionSimon64 ( plainText, keys );
		for ( i = 7; i >= 0; i-- ) {
			cipherTextByte[i] = plainText[0] % 256;
			plainText[0]      /= 256;
		}
		for ( i = SIMON_BLOCK_SIZE/8-1; i >= 8; i-- ) {
			cipherTextByte[i] = plainText[1] % 256;
			plainText[1]      /= 256;
		}
		cout<<"After decryption:"<<endl;
		for ( i = 0; i < SIMON_BLOCK_SIZE/8; i++ ) {
			cout<<cipherTextByte[i]<<"  ";
		}
		cout<<endl;

		/*
		 * 释放内存
		 */
		delete cipherTextByte;
		delete inputKeysByte;
		delete plainTextByte;
		delete keys;
		delete inputKeys;
		delete plainText;

	}

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
	
	/*unsigned int temp1 = 0xc0000000;
	unsigned int temp2 = 0x40000001;
	cout<<(temp1+temp2)<<endl;*/

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

	if ( SPECK_WORD_SIZE == 32 ) {
		int i, j;
		unsigned int * plainText      = new unsigned int[2]; // 明文
		plainText[0] = plainText[1]   = 0x0;
		unsigned int * inputKeys      = new unsigned int[SPECK_KEY_WORDS]; // 密钥
		for ( i = 0; i < SPECK_KEY_WORDS; i++ ) {
			inputKeys[i] = 0;
		}
		unsigned int * keys           = new unsigned int[SPECK_ROUNDS]; // 密钥，共SIMON_ROUNDS轮，每轮SIMON_WORD_SIZE位
		for ( i = 0; i < SPECK_ROUNDS; i++ ) {
			/*
			 * 错误记录：keys = 0x0;
			 * 执行出现错误，写入地址0x00000000时发生冲突
			 */
			keys[i] = 0x0;
		}
		unsigned int * plainTextByte  = new unsigned int[SPECK_BLOCK_SIZE/8]; // 以字节表示的明文，用于保存输入的明文
		for ( i = 0; i < SPECK_BLOCK_SIZE/8; i++ ) {
			plainTextByte[i] = 0;
		}
		unsigned int * inputKeysByte  = new unsigned int [SPECK_KEY_SIZE/8]; // 以字节表示的密钥，用于保存输入的密钥
		for ( i = 0; i < SPECK_KEY_SIZE/8; i++ ) {
			inputKeysByte[i] = 0;
		}
		unsigned int * cipherTextByte  = new unsigned int [SPECK_BLOCK_SIZE/8];  // 以字节表示的密文
		for ( i = 0; i < SPECK_BLOCK_SIZE/8; i++ ) {
			cipherTextByte[i] = 0;
		}

		/*
		 * 得到明文和初始密钥
		 */
		cout<<"Please input the plainText of "<<SPECK_BLOCK_SIZE/8<<" bytes:"<<endl;
		for ( i = 0; i < SPECK_BLOCK_SIZE/8; i++ ) {
			cin>>plainTextByte[i];
		}
		cout<<"Please input the key of "<<SPECK_KEY_SIZE/8<<" bytes:"<<endl;
		for ( i = 0; i < SPECK_KEY_SIZE/8; i++ ) {
			cin>>inputKeysByte[i];
		}

		/*
		 * 将明文和密钥以unsigned int表示
		 */
		for ( i = 0; i < 4; i++ ) {
			plainText[0] = plainText[0] * 256 + plainTextByte[i];
		}
		for ( i = 4; i < SPECK_BLOCK_SIZE/8; i++ ) {
			plainText[1] = plainText[1] * 256 + plainTextByte[i];
		}
		for ( i = 0; i < SPECK_KEY_SIZE/8; i++ ) {
			inputKeys[i/4] = inputKeys[i/4] * 256 + inputKeysByte[i];
		}

		time_t start, end;
		int recordCount = 0;
		for ( recordCount = 4; recordCount <= 1024; recordCount*=2 ) {
			start = clock();
			for ( j = 0; j < 10000*recordCount; j++ ) {
				/*
				 * 计算每轮的密钥
				 */
				setSpeckKeys32 ( inputKeys, keys );
			}
			end = clock();
			printf("Keys extend: %d, speed: %0.3f Mbps.\n", 10000*recordCount, 10.0*recordCount*SPECK_KEY_SIZE/(1.024*1.024*(end-start )));
		}
		

		printf("\n\nKeys\n");
		for ( i = 0; i < SPECK_ROUNDS; i++ ) {
			printf("%x,%x,%x,%x\n", keys[i]%256, keys[i]/256%256, keys[i]/256/256%256, keys[i]/256/256/256);
		}
		printf("\n\nEnd\n");
			
		/*
		 * 保存副本 
		 */
		unsigned int tempPlainHigher   = plainText[0];
		unsigned int tempPlainLower    = plainText[1];
		/*
		 * 多次记录，测试速度是否稳定
		 */
		for ( recordCount = 4; recordCount <= 1024; recordCount*=2 ) {
			start = clock();
			/* 
			 * 计算多次取平均值
			 */
			for ( j = 0; j < 10000*recordCount; j++ ) {
				plainText[0] = tempPlainHigher;
				plainText[1] = tempPlainLower;
				encryptionSpeck32 ( plainText, keys );
			}
			end = clock();
			printf("Encryption counts: %d, speed: %0.3f Mbps.\n", 10000*recordCount, 10.0*recordCount*SPECK_BLOCK_SIZE/(1.024*1.024*(end-start )));
		}

		tempPlainHigher   = plainText[0];
		tempPlainLower    = plainText[1];
		for ( i = 3; i >= 0; i-- ) {
			cipherTextByte[i] = tempPlainHigher % 256;
			tempPlainHigher   /= 256;
		}
		for ( i = SPECK_BLOCK_SIZE/8-1; i >= 4; i-- ) {
			cipherTextByte[i] = tempPlainLower % 256;
			tempPlainLower   /= 256;
		}
		cout<<"After encryption:"<<endl;
		for ( i = 0; i < SPECK_BLOCK_SIZE/8; i++ ) {
			cout<<cipherTextByte[i]<<"  ";
		}
		cout<<endl;

		/*
		 * 解密，并输出解密后的结果
		 */
		decryptionSpeck32 ( plainText, keys );
		for ( i = 3; i >= 0; i-- ) {
			cipherTextByte[i] = plainText[0] % 256;
			plainText[0]   /= 256;
		}
		for ( i = SPECK_BLOCK_SIZE/8-1; i >= 4; i-- ) {
			cipherTextByte[i] = plainText[1] % 256;
			plainText[1]   /= 256;
		}
		cout<<"After decryption:"<<endl;
		for ( i = 0; i < SPECK_BLOCK_SIZE/8; i++ ) {
			cout<<cipherTextByte[i]<<"  ";
		}
		cout<<endl;

		/*
		 * 释放内存
		 */
		delete cipherTextByte;
		delete inputKeysByte;
		delete plainTextByte;
		delete keys;
		delete inputKeys;
		delete plainText;
	} else if ( SPECK_WORD_SIZE == 64 ) {

		int i, j;
		unsigned long long * plainText      = new unsigned long long[2]; // 明文
		plainText[0] = plainText[1]   = 0x0;
		unsigned long long * inputKeys      = new unsigned long long[SPECK_KEY_WORDS]; // 密钥
		for ( i = 0; i < SPECK_KEY_WORDS; i++ ) {
			inputKeys[i] = 0;
		}
		unsigned long long * keys           = new unsigned long long[SPECK_ROUNDS]; // 密钥，共SIMON_ROUNDS轮，每轮SIMON_WORD_SIZE位
		for ( i = 0; i < SPECK_ROUNDS; i++ ) {
			keys[i] = 0x0;
		}
		unsigned long long * plainTextByte  = new unsigned long long[SPECK_BLOCK_SIZE/8]; // 以字节表示的明文，用于保存输入的明文
		for ( i = 0; i < SPECK_BLOCK_SIZE/8; i++ ) {
			plainTextByte[i] = 0;
		}
		unsigned long long * inputKeysByte  = new unsigned long long [SPECK_KEY_SIZE/8]; // 以字节表示的密钥，用于保存输入的密钥
		for ( i = 0; i < SPECK_KEY_SIZE/8; i++ ) {
			inputKeysByte[i] = 0;
		}
		unsigned long long * cipherTextByte  = new unsigned long long [SPECK_BLOCK_SIZE/8];  // 以字节表示的密文
		for ( i = 0; i < SPECK_BLOCK_SIZE/8; i++ ) {
			cipherTextByte[i] = 0;
		}

		/*
		 * 得到明文和初始密钥
		 */
		cout<<"Please input the plainText of "<<SPECK_BLOCK_SIZE/8<<" bytes:"<<endl;
		for ( i = 0; i < SPECK_BLOCK_SIZE/8; i++ ) {
			cin>>plainTextByte[i];
		}
		cout<<"Please input the key of "<<SPECK_KEY_SIZE/8<<" bytes:"<<endl;
		for ( i = 0; i < SPECK_KEY_SIZE/8; i++ ) {
			cin>>inputKeysByte[i];
		}

		/*
		 * 将明文和密钥以unsigned int表示
		 */
		for ( i = 0; i < 8; i++ ) {
			plainText[0] = plainText[0] * 256 + plainTextByte[i];
		}
		for ( i = 8; i < SPECK_BLOCK_SIZE/8; i++ ) {
			plainText[1] = plainText[1] * 256 + plainTextByte[i];
		}
		for ( i = 0; i < SPECK_KEY_SIZE/8; i++ ) {
			inputKeys[i/8] = inputKeys[i/8] * 256 + inputKeysByte[i];
		}

		time_t start, end;
		int recordCount = 0;
		for ( recordCount = 4; recordCount <= 1024; recordCount*=2 ) {
			start = clock();
			for ( j = 0; j < 10000*recordCount; j++ ) {
				/*
				 * 计算每轮的密钥
				 */
				setSpeckKeys64 ( inputKeys, keys );
			}
			end = clock();
			printf("Keys extend: %d, speed: %0.3f Mbps.\n", 10000*recordCount, 10.0*recordCount*SPECK_KEY_SIZE/(1.024*1.024*(end-start )));
		}
		
		for ( i = 0; i < SPECK_ROUNDS; i++ ) {
			cout<<keys[i]%256<<","<<keys[i]/256%256<<","<<keys[i]/256/256%256<<","<<keys[i]/256/256/256<<endl;
		}

		/*
		 * 保存副本 
		 */
		unsigned long long tempPlainHigher   = plainText[0];
		unsigned long long tempPlainLower    = plainText[1];
		/*
		 * 多次记录，测试速度是否稳定
		 */
		for ( recordCount = 4; recordCount <= 1024; recordCount*=2 ) {
			start = clock();
			/* 
			 * 计算多次取平均值
			 */
			for ( j = 0; j < 10000*recordCount; j++ ) {
				plainText[0] = tempPlainHigher;
				plainText[1] = tempPlainLower;
				encryptionSpeck64 ( plainText, keys );
			}
			end = clock();
			printf("Encryption counts: %d, speed: %0.3f Mbps.\n", 10000*recordCount, 10.0*recordCount*SPECK_BLOCK_SIZE/(1.024*1.024*(end-start )));
		}

		tempPlainHigher   = plainText[0];
		tempPlainLower    = plainText[1];
		for ( i = 7; i >= 0; i-- ) {
			cipherTextByte[i] = tempPlainHigher % 256;
			tempPlainHigher   /= 256;
		}
		for ( i = SPECK_BLOCK_SIZE/8-1; i >= 8; i-- ) {
			cipherTextByte[i] = tempPlainLower % 256;
			tempPlainLower   /= 256;
		}
		cout<<"After encryption:"<<endl;
		for ( i = 0; i < SPECK_BLOCK_SIZE/8; i++ ) {
			cout<<cipherTextByte[i]<<"  ";
		}
		cout<<endl;

		/*
		 * 解密，并输出解密后的结果
		 */
		decryptionSpeck64 ( plainText, keys );
		for ( i = 7; i >= 0; i-- ) {
			cipherTextByte[i] = plainText[0] % 256;
			plainText[0]   /= 256;
		}
		for ( i = SPECK_BLOCK_SIZE/8-1; i >= 8; i-- ) {
			cipherTextByte[i] = plainText[1] % 256;
			plainText[1]   /= 256;
		}
		cout<<"After decryption:"<<endl;
		for ( i = 0; i < SPECK_BLOCK_SIZE/8; i++ ) {
			cout<<cipherTextByte[i]<<"  ";
		}
		cout<<endl;

		/*
		 * 释放内存
		 */
		delete cipherTextByte;
		delete inputKeysByte;
		delete plainTextByte;
		delete keys;
		delete inputKeys;
		delete plainText;

	}

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