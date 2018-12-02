#include <fstream>
#include "DES.hpp"


//将char字符数组转为二进制bitset
bitset<64> char2Bit(const char s[8])
{
	bitset<64> bits;
	for(int i=0; i<8; ++i)
		for(int j=0; j<8; ++j)
			bits[i*8+j] = ((s[i]>>j) & 1);
	return bits;
}
int main() {
    //要加密的明文
	string s = "Hello, My name is Bob.\nWhat's your name?\nhahaha";
    //设置的初始密钥
	string k = "12345678";
	
    //记录块数
	int block_num = 0;
    //不足8字节整数倍，补齐空格字符
	while(s.size()%8!=0){
		s.append("\n");
	}
    
	block_num = s.size()/8;
	
    //将密钥转换为64位二进制数
	key = char2Bit(k.c_str());		
	// 生成16个子密钥
	generateSubKeys();   

	// 每块密文以二进制方式追加写入 cipher.txt	
	fstream f("cipher.txt", ios::binary | ios::out | ios::app);	
	for(int i=0 ;i<block_num;i++){	
        //每64位截取一个子串
		string sub_string = s.substr(i*8,8);
         //将明文转换为64位二进制数
		bitset<64> plain = char2Bit(sub_string.c_str());
        //使用DES加密
		bitset<64> cipher = encrypt(plain);
        //追加写入ciper.txt
		f.write((char*)&cipher,sizeof(cipher));
	}
	f.close();
 
	// 以二进制读文件 cipher.txt
	fstream fin("cipher.txt", ios::binary | ios::in);
	fstream fout("plain.txt", ios::binary | ios::out | ios::app);
	for(int i=0;i<block_num;i++){
        //文件指针定位每块密文的起始处
		fin.seekp(i*8);
		bitset<64> temp;
		fin.read((char*)&temp, sizeof(temp));
		// 解密，并写入文件 plain.txt
		bitset<64> decipher = decrypt(temp);
		fout.write((char*)&decipher,sizeof(decipher));
	}
	fin.close();
	fout.close();
 
	return 0;
}