#include<iostream>
#include<time.h>
#include"const.h"
#include"keys.h"
#include"enDecryption.h"
using namespace std;

/*
int SIMON_BLOCK_SIZE;       // block size
int SIMON_KEY_SIZE;         // key size
int SIMON_WORD_SIZE;        // word size ( = SIMON_BLOCK_SIZE/2 )
int SIMON_KEY_WORDS;        // the words of input key ( = SIMON_KEY_SIZE/SIMON_WORD_SIZE )
int SIMON_SEQUENCE_NUMBER;  // the number of const Z(0, 1, 2, 3 or 4)
int SIMON_ROUNDS;           // encryption and decryption rounds

int SPECK_BLOCK_SIZE;       // block size
int SPECK_KEY_SIZE;         // key size
int SPECK_WORD_SIZE;        // word size ( = SPECK_BLOCK_SIZE/2 )
int SPECK_KEY_WORDS;        // the words of input key ( = SPECK_KEY_SIZE/SPECK_WORD_SIZE )
int SPECK_ROT_A;            // the rotate shift parameter a
int SPECK_ROT_B;            // the rotate shift parameter b
int SPECK_ROUNDS;           // encryption and decryption rounds
*/

void simon();
void speck();
int simonChoose ();
int speckChoose ();

/*
 * Simon and Speck Algorithms
 * Version:2.0
 * Time:2015-1-10
 *
 * Unsigned int is used to stand for a part(BLOCK_SIZE/2) if the word size is 32. Otherwise, unsigned long long is used.
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
 * Simon
 * 1£º64/96
 * 2£º64/128
 * 3£º128/128
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

	if ( SIMON_WORD_SIZE == 32 ) {
		int i, j;
		unsigned int * plainText       = new unsigned int[2]; // plainText
		plainText[0] = plainText[1]    = 0x0;
		unsigned int * inputKeys       = new unsigned int[SIMON_KEY_WORDS]; // initial keys
		for ( i = 0; i < SIMON_KEY_WORDS; i++ ) {
			inputKeys[i]               = 0x0;
		}
		unsigned int * keys            = new unsigned int[SIMON_ROUNDS]; // round keys

		unsigned int * plainTextByte   = new unsigned int[SIMON_BLOCK_SIZE/8]; // plainText in byte
		/*for ( i = 0; i < SIMON_BLOCK_SIZE/8; i++ ) {
			plainTextByte[i]           = 0x0;
		}*/
		unsigned int * inputKeysByte   = new unsigned int[SIMON_KEY_SIZE/8];  // initial keys in byte
		/*for ( i = 0; i < SIMON_KEY_SIZE/8; i++ ) {
			inputKeysByte[i]           = 0x0;
		}*/
		unsigned int * cipherTextByte  = new unsigned int[SIMON_BLOCK_SIZE/8];// cipher text in byte
		/*for ( i = 0; i < SIMON_BLOCK_SIZE/8; i++ ) {
			cipherTextByte[i]          = 0x0;
		}*/

		/*
		 * get the plain text and input keys
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
		 * SIMON_BLOCK_SIZE is 64. The higher four bytes is plainText[0], lower four bytes is plainText[1]
		 */
		for ( i = 0; i < 4; i++ ) {
			plainText[0]  = plainText[0] * 256 + plainTextByte[i];
		}
		for ( i = 4; i < SIMON_BLOCK_SIZE/8; i++ ) { 
			plainText[1]  = plainText[1] * 256 + plainTextByte[i];
		}
		//cout<<endl<<endl<<plainText[1]/(256*256*256)<<","<<plainText[1]/(256*256)%256<<","<<plainText[1]%(256*256)/256<<","<<plainText[1]%256<<endl;
		/*
		 * the initial keys in from right to left, so index with the minimal value is biggger round key
		 * if SIMON_KEY_WORDS=3, then inputKeysByte[0...3]=k2;inputKeysByte[4...7]=k1;inputKeysByte[8...11]=k0
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
				 * compute round keys
				 */
				setSimonKeys32 ( inputKeys, keys );
			}
			end = clock();
			/*
			 * = (10000*recordCount*SIMON_KEY_SIZE)/(end-start) bits per millisecond
			 * = (1000*10000*recordCount*SIMON_KEY_SIZE)/(end-start) bits per second 
			 * = (1000*10000*recordCount*SIMON_KEY_SIZE)/£¨1024*1024*(end-start)) billion bits per second
			 * = (10.0*recordCount*SIMON_KEY_SIZE)/£¨1.024*1.024*(end-start)) billion bits per second
			 */
			printf("Keys extend: %d, speed: %0.3f Mbps.\n", 10000*recordCount, 10.0*recordCount*SIMON_KEY_SIZE/(1.024*1.024*(end-start )));
		}

		for ( int i = 0; i < SIMON_ROUNDS; i++ ) {
			cout<<"ÃØÔ¿£º"<<keys[i]%256<<","<<(keys[i]/256%256)<<","<<(keys[i]/(256*256)%256)<<","<<keys[i]/(256*256*256)<<endl;
		}

		/*
		 * set to the plain text before encryption each time.
		 * It can reduce error by repeating many time and computing the average value
		 */
		unsigned int tempPlainHigher   = plainText[0];
		unsigned int tempPlainLower    = plainText[1];
		//cout<<endl<<endl<<plainText[1]/(256*256*256)<<","<<plainText[1]/(256*256)%256<<","<<plainText[1]%(256*256)/256<<","<<plainText[1]%256<<endl;

		for ( recordCount = 4; recordCount <= 1024; recordCount*=2 ) {
			start = clock();
			for ( j = 0; j < 10000*recordCount; j++ ) {
				plainText[0]  = tempPlainHigher;
				plainText[1]  = tempPlainLower;
				encryptionSimon32 ( plainText, keys );
			}
			end = clock();
			/*
			 * = (10000*recordCount*SIMON_BLOCK_SIZE)/(end-start) bits per millisecond
			 * = (1000*10000*recordCount*SIMON_BLOCK_SIZE)/(end-start) bits per second 
			 * = (1000*10000*recordCount*SIMON_BLOCK_SIZE)/£¨1024*1024*(end-start)) billion bits per second
			 * = (10.0*recordCount*SIMON_BLOCK_SIZE)/£¨.1024*1.024*(end-start)) billion bits per second
			 */
			printf("Encryption counts: %d, speed: %0.3f Mbps.\n", 10000*recordCount, 10.0*recordCount*SIMON_BLOCK_SIZE/(1.024*1.024*(end-start )));
		}

		/*
		 * output the cipher text in byte
		 */
		tempPlainHigher   = plainText[0];
		tempPlainLower    = plainText[1];
		for ( i = 3; i >= 0; i-- ) {
			cipherTextByte[i] = tempPlainHigher % 256;
			tempPlainHigher   /= 256;
		}

		/*
		 * error£ºfor ( i = SIMON_BLOCK_SIZE/8; i >= 4; i-- ) {
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
		 * decryption
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
		 * free memory
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
		unsigned long long * inputKeys       = new unsigned long long[SIMON_KEY_WORDS];
		for ( i = 0; i < SIMON_KEY_WORDS; i++ ) {
			inputKeys[i]               = 0x0;
		}
		unsigned long long * keys            = new unsigned long long[SIMON_ROUNDS];
		unsigned long long * plainTextByte   = new unsigned long long[SIMON_BLOCK_SIZE/8];
		unsigned long long * inputKeysByte   = new unsigned long long[SIMON_KEY_SIZE/8];
		unsigned long long * cipherTextByte  = new unsigned long long[SIMON_BLOCK_SIZE/8];

		cout<<"Please input the plainText of "<<SIMON_BLOCK_SIZE/8<<" bytes:"<<endl;
		for ( i = 0; i < SIMON_BLOCK_SIZE/8; i++ ) {
			cin>>plainTextByte[i];
		}
		cout<<"Please input the key of "<<SIMON_KEY_SIZE/8<<" bytes:"<<endl;
		for ( i = 0; i < SIMON_KEY_SIZE/8; i++ ) {
			cin>>inputKeysByte[i];
		}

		/*
		 * SIMON_BLOCK_SIZE is 64. The higher eight bytes is plainText[0], lower eight bytes is plainText[1]
		 */
		for ( i = 0; i < 8; i++ ) {
			plainText[0]  = plainText[0] * 256 + plainTextByte[i];
		}
		for ( i = 8; i < SIMON_BLOCK_SIZE/8; i++ ) { 
			plainText[1]  = plainText[1] * 256 + plainTextByte[i];
		}
		
		for ( i = 0; i < SIMON_KEY_SIZE/8; i++ ) {
			inputKeys[SIMON_KEY_WORDS-1-i/8] = inputKeys[SIMON_KEY_WORDS-1-i/8] * 256 + inputKeysByte[i];
		}

		/*
		 * key schedule
		 */
		time_t start, end;
		int recordCount = 0;
		for ( recordCount = 4; recordCount <= 1024; recordCount*=2 ) {
			start = clock();
			for ( j = 0; j < 10000*recordCount; j++ ) {
				/*
				 * compute round keys
				 */
				setSimonKeys64 ( inputKeys, keys );
			}
			end = clock();
			printf("Keys extend: %d, speed: %0.3f Mbps.\n", 10000*recordCount, 10.0*recordCount*SIMON_KEY_SIZE/(1.024*1.024*(end-start )));
		}

		/*
		 * encryption
		 */
		unsigned long long tempPlainHigher   = plainText[0];
		unsigned long long tempPlainLower    = plainText[1];
		for ( recordCount = 4; recordCount <= 1024; recordCount*=2 ) {
			start = clock();
			for ( j = 0; j < 10000*recordCount; j++ ) {
				plainText[0]  = tempPlainHigher;
				plainText[1]  = tempPlainLower;
				encryptionSimon64 ( plainText, keys );
			}
			end = clock();
			printf("Encryption counts: %d, speed: %0.3f Mbps.\n", 10000*recordCount, 10.0*recordCount*SIMON_BLOCK_SIZE/(1.024*1.024*(end-start )));
		}

		printf("\n\nKeys\n");
		for ( i = 0; i < SIMON_ROUNDS; i++ ) {
			printf("%x,%x,%x,%x\n", keys[i]%256, keys[i]/256%256, keys[i]/256/256%256, keys[i]/256/256/256);
		}
		printf("\n\nEnd\n");


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
		 * decryption
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
		 * free memory
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
 * Choose Simon Algorithm
 * 1£º64/96
 * 2£º64/128
 * 3£º128/128
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
 * Speck
 * 1£º64/96
 * 2£º64/128
 * 3£º128/128
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
		unsigned int * plainText      = new unsigned int[2]; // plain text
		plainText[0] = plainText[1]   = 0x0;
		unsigned int * inputKeys      = new unsigned int[SPECK_KEY_WORDS]; // initial keys
		for ( i = 0; i < SPECK_KEY_WORDS; i++ ) {
			inputKeys[i] = 0;
		}
		unsigned int * keys           = new unsigned int[SPECK_ROUNDS];
		for ( i = 0; i < SPECK_ROUNDS; i++ ) {
			/*
			 * error£ºkeys = 0x0;
			 */
			keys[i] = 0x0;
		}
		unsigned int * plainTextByte  = new unsigned int[SPECK_BLOCK_SIZE/8];
		unsigned int * inputKeysByte  = new unsigned int [SPECK_KEY_SIZE/8];
		unsigned int * cipherTextByte  = new unsigned int [SPECK_BLOCK_SIZE/8];

		/*
		 * get the plain text and initial keys
		 */
		cout<<"Please input the plainText of "<<SPECK_BLOCK_SIZE/8<<" bytes:"<<endl;
		for ( i = 0; i < SPECK_BLOCK_SIZE/8; i++ ) {
			cin>>plainTextByte[i];
		}
		cout<<"Please input the key of "<<SPECK_KEY_SIZE/8<<" bytes:"<<endl;
		for ( i = 0; i < SPECK_KEY_SIZE/8; i++ ) {
			cin>>inputKeysByte[i];
		}

		for ( i = 0; i < 4; i++ ) {
			plainText[0] = plainText[0] * 256 + plainTextByte[i];
		}
		for ( i = 4; i < SPECK_BLOCK_SIZE/8; i++ ) {
			plainText[1] = plainText[1] * 256 + plainTextByte[i];
		}
		for ( i = 0; i < SPECK_KEY_SIZE/8; i++ ) {
			inputKeys[i/4] = inputKeys[i/4] * 256 + inputKeysByte[i];
		}

		/*
		 * key schedule
		 */
		time_t start, end;
		int recordCount = 0;
		for ( recordCount = 4; recordCount <= 1024; recordCount*=2 ) {
			start = clock();
			for ( j = 0; j < 10000*recordCount; j++ ) {
				/*
				 * compute round keys
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
		 * encryption
		 */
		unsigned int tempPlainHigher   = plainText[0];
		unsigned int tempPlainLower    = plainText[1];
		for ( recordCount = 4; recordCount <= 1024; recordCount*=2 ) {
			start = clock();
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
		 * decryption
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
		 * free memory
		 */
		delete cipherTextByte;
		delete inputKeysByte;
		delete plainTextByte;
		delete keys;
		delete inputKeys;
		delete plainText;
	} else if ( SPECK_WORD_SIZE == 64 ) {

		int i, j;
		unsigned long long * plainText      = new unsigned long long[2];
		plainText[0] = plainText[1]   = 0x0;
		unsigned long long * inputKeys      = new unsigned long long[SPECK_KEY_WORDS];
		for ( i = 0; i < SPECK_KEY_WORDS; i++ ) {
			inputKeys[i] = 0;
		}
		unsigned long long * keys           = new unsigned long long[SPECK_ROUNDS];
		for ( i = 0; i < SPECK_ROUNDS; i++ ) {
			keys[i] = 0x0;
		}
		unsigned long long * plainTextByte  = new unsigned long long[SPECK_BLOCK_SIZE/8];
		for ( i = 0; i < SPECK_BLOCK_SIZE/8; i++ ) {
			plainTextByte[i] = 0;
		}
		unsigned long long * inputKeysByte  = new unsigned long long [SPECK_KEY_SIZE/8];
		for ( i = 0; i < SPECK_KEY_SIZE/8; i++ ) {
			inputKeysByte[i] = 0;
		}
		unsigned long long * cipherTextByte  = new unsigned long long [SPECK_BLOCK_SIZE/8];
		for ( i = 0; i < SPECK_BLOCK_SIZE/8; i++ ) {
			cipherTextByte[i] = 0;
		}

		/*
		 * get plain text and initial keys
		 */
		cout<<"Please input the plainText of "<<SPECK_BLOCK_SIZE/8<<" bytes:"<<endl;
		for ( i = 0; i < SPECK_BLOCK_SIZE/8; i++ ) {
			cin>>plainTextByte[i];
		}
		cout<<"Please input the key of "<<SPECK_KEY_SIZE/8<<" bytes:"<<endl;
		for ( i = 0; i < SPECK_KEY_SIZE/8; i++ ) {
			cin>>inputKeysByte[i];
		}

		for ( i = 0; i < 8; i++ ) {
			plainText[0] = plainText[0] * 256 + plainTextByte[i];
		}
		for ( i = 8; i < SPECK_BLOCK_SIZE/8; i++ ) {
			plainText[1] = plainText[1] * 256 + plainTextByte[i];
		}
		for ( i = 0; i < SPECK_KEY_SIZE/8; i++ ) {
			inputKeys[i/8] = inputKeys[i/8] * 256 + inputKeysByte[i];
		}

		/*
		 * key schedule
		 */
		time_t start, end;
		int recordCount = 0;
		for ( recordCount = 4; recordCount <= 1024; recordCount*=2 ) {
			start = clock();
			for ( j = 0; j < 10000*recordCount; j++ ) {
				setSpeckKeys64 ( inputKeys, keys );
			}
			end = clock();
			printf("Keys extend: %d, speed: %0.3f Mbps.\n", 10000*recordCount, 10.0*recordCount*SPECK_KEY_SIZE/(1.024*1.024*(end-start )));
		}
		
		for ( i = 0; i < SPECK_ROUNDS; i++ ) {
			cout<<keys[i]%256<<","<<keys[i]/256%256<<","<<keys[i]/256/256%256<<","<<keys[i]/256/256/256<<endl;
		}

		/*
		 * encryption
		 */
		unsigned long long tempPlainHigher   = plainText[0];
		unsigned long long tempPlainLower    = plainText[1];
		for ( recordCount = 4; recordCount <= 1024; recordCount*=2 ) {
			start = clock();
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
		 * decryption
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
		 * free memory
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
 * Choose Speck Algorithm
 * 1£º64/96
 * 2£º64/128
 * 3£º128/128
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