#ifndef DES_HPP
#define DES_HPP

#include <iostream>
#include <bitset>
#include <string>
#include <cmath>
using namespace std;

// 初始64位密钥
bitset<64> key;
// 16个子密钥
bitset<48> subKeys[16];

// 初始置换表
int IP[8][8] = {
				{58, 50, 42, 34, 26, 18, 10, 2},			
            	{60, 52, 44, 36, 28, 20, 12, 4},			
	            {62, 54, 46, 38, 30, 22, 14, 6},			
	            {64, 56, 48, 40, 32, 24, 16, 8},			
	            {57, 49, 41, 33, 25, 17, 9,  1},			
	            {59, 51, 43, 35, 27, 19, 11, 3},			
	            {61, 53, 45, 37, 29, 21, 13, 5},			
	            {63, 55, 47, 39, 31, 23, 15, 7}
	       	};

// 终止置换表
int FP[8][8] = {
				{40, 8, 48, 16, 56, 24, 64, 32},			  
            	{39, 7, 47, 15, 55, 23, 63, 31},			  
            	{38, 6, 46, 14, 54, 22, 62, 30},			  
            	{37, 5, 45, 13, 53, 21, 61, 29},			  
            	{36, 4, 44, 12, 52, 20, 60, 28},			  
            	{35, 3, 43, 11, 51, 19, 59, 27},			  
            	{34, 2, 42, 10, 50, 18, 58, 26},			  
            	{33, 1, 41,  9, 49, 17, 57, 25}
            }; 

// 每轮左移的位数
int leftShiftBits[16] = {1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1};

// 密钥置换表，将64位密钥变成56位
int PC_1[8][7] = {
					{57, 49, 41, 33, 25, 17, 9},			   
					{1, 58, 50, 42, 34, 26, 18},			  
					{10,  2, 59, 51, 43, 35, 27},			  
					{19, 11,  3, 60, 52, 44, 36},			  
					{63, 55, 47, 39, 31, 23, 15},			   
					{7, 62, 54, 46, 38, 30, 22},			  
					{14,  6, 61, 53, 45, 37, 29},			  
					{21, 13,  5, 28, 20, 12,  4}
	        	};  

// 压缩置换，将56位密钥压缩成48位子密钥
int PC_2[8][6] = {
					{14, 17, 11, 24,  1,  5},			   
      				{3, 28, 15,  6, 21,  10},			  
      				{23, 19, 12,  4, 26,  8},			  
      				{16,  7, 27, 20, 13,  2},			  
      				{41, 52, 31, 37, 47, 55},			  
      				{30, 40, 51, 45, 33, 48},			  
      				{44, 49, 39, 56, 34, 53},			  
      				{46, 42, 50, 36, 29, 32}
      			}; 

// E-扩展置换表，将32位明文扩展成48位明文	
int E[8][6] = {
				{32,  1,  2,  3,  4,  5},		    
        		{4,  5,  6,  7,  8,   9},		    
        		{8,  9, 10, 11, 12,  13},		   
        		{12, 13, 14, 15, 16, 17},		   
        		{16, 17, 18, 19, 20, 21},		   
        		{20, 21, 22, 23, 24, 25},		   
        		{24, 25, 26, 27, 28, 29},		   
        		{28, 29, 30, 31, 32,  1}
        	}; 

// S盒，每个S盒是4x16的置换表，6位 -> 4位
int S_BOX[8][4][16] = {	
    {  		
        {14,4,13,1,2,15,11,8,3,10,6,12,5,9,0,7},  		
        {0,15,7,4,14,2,13,1,10,6,12,11,9,5,3,8},  		
        {4,1,14,8,13,6,2,11,15,12,9,7,3,10,5,0}, 		
        {15,12,8,2,4,9,1,7,5,11,3,14,10,0,6,13} 	
    },	
    {  		
        {15,1,8,14,6,11,3,4,9,7,2,13,12,0,5,10},  		
        {3,13,4,7,15,2,8,14,12,0,1,10,6,9,11,5}, 		
        {0,14,7,11,10,4,13,1,5,8,12,6,9,3,2,15},  		
        {13,8,10,1,3,15,4,2,11,6,7,12,0,5,14,9}  	
    }, 	
    {  		
        {10,0,9,14,6,3,15,5,1,13,12,7,11,4,2,8},  		
        {13,7,0,9,3,4,6,10,2,8,5,14,12,11,15,1},  		
        {13,6,4,9,8,15,3,0,11,1,2,12,5,10,14,7},  		
        {1,10,13,0,6,9,8,7,4,15,14,3,11,5,2,12}  	
    }, 	
    {  		
        {7,13,14,3,0,6,9,10,1,2,8,5,11,12,4,15},  		
        {13,8,11,5,6,15,0,3,4,7,2,12,1,10,14,9},  		
        {10,6,9,0,12,11,7,13,15,1,3,14,5,2,8,4},  		
        {3,15,0,6,10,1,13,8,9,4,5,11,12,7,2,14}  	
    },	
    {  		
        {2,12,4,1,7,10,11,6,8,5,3,15,13,0,14,9},  		
        {14,11,2,12,4,7,13,1,5,0,15,10,3,9,8,6},  		
        {4,2,1,11,10,13,7,8,15,9,12,5,6,3,0,14},  		
        {11,8,12,7,1,14,2,13,6,15,0,9,10,4,5,3}  	
    },	
    {  		
        {12,1,10,15,9,2,6,8,0,13,3,4,14,7,5,11},  		
        {10,15,4,2,7,12,9,5,6,1,13,14,0,11,3,8},  		
        {9,14,15,5,2,8,12,3,7,0,4,10,1,13,11,6},  		
        {4,3,2,12,9,5,15,10,11,14,1,7,6,0,8,13}  	
    }, 	
    {  		
        {4,11,2,14,15,0,8,13,3,12,9,7,5,10,6,1},  		
        {13,0,11,7,4,9,1,10,14,3,5,12,2,15,8,6},  		
        {1,4,11,13,12,3,7,14,10,15,6,8,0,5,9,2},  		
        {6,11,13,8,1,4,10,7,9,5,0,15,14,2,3,12}  	
    }, 	
    {  		
        {13,2,8,4,6,15,11,1,10,9,3,14,5,0,12,7},  		
        {1,15,13,8,10,3,7,4,12,5,6,11,0,14,9,2},  		
        {7,11,4,1,9,12,14,2,0,6,10,13,15,3,5,8},  		
        {2,1,14,7,4,10,8,13,15,12,9,0,3,5,6,11}  	
    } 
}; 

// P置换，32位 -> 32位
int P[8][4] = {
				{16,  7, 20, 21},		   
	            {29, 12, 28, 17},		    
	            {1, 15, 23,  26},		    
	            {5, 18, 31,  10},		    
	            {2,  8, 24,  14},		   
	            {32, 27,  3,  9},		   
	            {19, 13, 30,  6},		   
	            {22, 11,  4, 25} 
	        };
 

//具体算法

//对56位密钥的前后28位C0和D0进行循环左移
bitset<28> leftShift(bitset<28> keys, int shift_bit)
{
	bitset<28> tmp = keys;
	for(int i=27; i>=0; i--)
	{
		if(i-shift_bit<0)
			keys[i] = tmp[i-shift_bit+28];//末位到高位
		else
			keys[i] = tmp[i-shift_bit]; 
	}
	return keys;
}
 
//16个子密钥生成
void generateSubKeys() 
{
	bitset<56> originKey;
	bitset<48> compressedKey;
	bitset<28> left;
	bitset<28> right;

	// 进行PC_1置换，去掉奇偶标记位，将64位密钥变成56位
	for (int i=0; i<8; i++){
		for(int j=0;j<7;j++)
			originKey[55-i*8-j] = key[64 - PC_1[i][j]];
	}

	// 生成子密钥，保存在 subKeyss[16] 中
	for(int round=0; round<16; ++round) 
	{
		// 分离前28位与后28位
		for(int i=28; i<56; ++i)
			left[i-28] = originKey[i];
		for(int i=0; i<28; ++i)
			right[i] = originKey[i];

		// 左移
		left = leftShift(left, leftShiftBits[round]);
		right = leftShift(right, leftShiftBits[round]);

		// 合并成56位
		for(int i=28; i<56; ++i)
			originKey[i] = left[i-28];
		for(int i=0; i<28; ++i)
			originKey[i] = right[i];

		// 进行PC_2压缩置换，由56位得到48位子密钥
		for(int i=0; i<8; i++){
			for(int j=0;j<6;j++)
				compressedKey[47-i*8-j] = originKey[56 - PC_2[i][j]];
		}

		//保存子密钥
		for(int i=0;i<48;i++)
			subKeys[round][i] = compressedKey[i];
	}
}
 
// Feistel 轮函数，接收32位数据和48位子密钥，产生一个32位的输出            
bitset<32> feistel(bitset<32> R, bitset<48> subkey)
{
	bitset<48> expand_R;

	// 第一步：E-扩展，32 -> 48
	for(int i=0; i<8; i++){
		for(int j=0;j<6;j++)
			expand_R[47-i*8-j] = R[32-E[i][j]];
	}

	// 第二步：与子密钥异或
	expand_R = expand_R ^ subkey;

	// 第三步：查找S_BOX置换表索引
	bitset<32> result;
	int x = 0,msb=0,lsb=0;

	// 每6位进行一次查找，输出4位，总共8次查找
	for(int i=0; i<8; i++)
	{	
		// 第一位和最后一位组成的二进制数转换成十进制数字为S盒置换表行数	
		msb = expand_R[47-i*6];
		lsb = expand_R[47-i*6-5];
		int row = msb*2 + lsb;

		//	中间4位组成的二进制数转换成十进制数字为S盒置换表列数
		int other_bits[4];
		for(int j=0;j<4;j++){
			other_bits[j] = expand_R[47-i*6-j-1];
		}
		int col = 0;
		for(int j=0;j<4;j++){
			col += other_bits[j]*pow(2.0,3-j);
		}

		// 索引表项
		int num = S_BOX[i][row][col];

		// 转换成二进制
		bitset<4> binary(num);
	
		result[31-x] = binary[3];
		result[31-x-1] = binary[2];
		result[31-x-2] = binary[1];
		result[31-x-3] = binary[0];
		x += 4;
	}

	// 第四步：P-置换，32 -> 32
	bitset<32> tmp = result;
	for(int i=0; i<8; i++){
		for(int j=0;j<4;j++)
			result[31-i] = tmp[32-P[i][j]];
	}
	return result;
}

//DES完整加密
//输入：64位二进制明文
//输出：64位二进制密文
bitset<64> encrypt(bitset<64>& plain)
{
	bitset<64> cipher;
	bitset<64> currentBits;
	bitset<32> left;
	bitset<32> right;
	bitset<32> newLeft;

	// 第一步：初始置换IP
	for(int i=0; i<8; i++){
		for(int j=0 ; j<8 ;j++)
			currentBits[63-i*8-j] = plain[64-IP[i][j]];
	}	

	// 第二步：获取 Li 和 Ri
	for(int i=32; i<64; ++i)
		left[i-32] = currentBits[i];
	for(int i=0; i<32; ++i)
		right[i] = currentBits[i];

	// 第三步：共16轮迭代
	for(int round=0; round<16; ++round)
	{
		newLeft = right;
		right = left ^ feistel(right,subKeys[round]);
		left = newLeft;
	}

	// 第四步：合并L16和R16，最后转换为R16L16
	for(int i=0; i<32; ++i)
		cipher[i] = left[i];
	for(int i=32; i<64; ++i)
		cipher[i] = right[i-32];

	// 第五步：终止置换FP
	currentBits = cipher;
	for(int i=0; i<8; i++){
		for(int j=0;j<8;j++)
			cipher[63-i*8-j] = currentBits[64-FP[i][j]];
	}

	// 返回密文
	return cipher;
}
 
//DES完整解密
//输入：64位二进制密文
//输出：64位二进制明文
bitset<64> decrypt(bitset<64>& cipher)
{
	bitset<64> plain;
	bitset<64> currentBits;
	bitset<32> left;
	bitset<32> right;
	bitset<32> newLeft;

	// 第一步：初始置换IP
	for(int i=0; i<8; i++){
		for(int j=0 ; j<8 ;j++)
			currentBits[63-i*8-j] = cipher[64-IP[i][j]];
	}	

	// 第二步：获取 Li 和 Ri
	for(int i=32; i<64; ++i)
		left[i-32] = currentBits[i];
	for(int i=0; i<32; ++i)
		right[i] = currentBits[i];

	// 第三步：共16轮迭代（子密钥逆序应用）
	for(int round=0; round<16; ++round)
	{
		newLeft = right;
		right = left ^ feistel(right,subKeys[15-round]);
		left = newLeft;
	}

	// 第四步：合并L16和R16，注意合并为 R16L16
	for(int i=0; i<32; ++i)
		plain[i] = left[i];
	for(int i=32; i<64; ++i)
		plain[i] = right[i-32];

	// 第五步：结尾置换FP
	currentBits = plain;
	for(int i=0; i<8; i++){
		for(int j=0;j<8;j++)
			plain[63-i*8-j] = currentBits[64-FP[i][j]];
	}

	// 返回明文
	return plain;
}

#endif
 