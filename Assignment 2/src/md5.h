#ifndef _MD5_H
#define _MD5_H

#include <string>
#include <vector>
#include <cstring>
#include <cmath>
#include <iostream>
#include <bitset>
using std::string;
using std::vector;
using std::bitset;
using std::cout;
using std::endl;
using std::sin;
using std::abs;

// T表
const int T[] = {
    0xd76aa478,0xe8c7b756,0x242070db,0xc1bdceee,
    0xf57c0faf,0x4787c62a,0xa8304613,0xfd469501,
    0x698098d8,0x8b44f7af,0xffff5bb1,0x895cd7be,
    0x6b901122,0xfd987193,0xa679438e,0x49b40821,
    0xf61e2562,0xc040b340,0x265e5a51,0xe9b6c7aa,
    0xd62f105d, 0x2441453,0xd8a1e681,0xe7d3fbc8,
    0x21e1cde6,0xc33707d6,0xf4d50d87,0x455a14ed,
    0xa9e3e905,0xfcefa3f8,0x676f02d9,0x8d2a4c8a,
    0xfffa3942,0x8771f681,0x6d9d6122,0xfde5380c,
    0xa4beea44,0x4bdecfa9,0xf6bb4b60,0xbebfbc70,
    0x289b7ec6,0xeaa127fa,0xd4ef3085, 0x4881d05,
    0xd9d4d039,0xe6db99e5,0x1fa27cf8,0xc4ac5665,
    0xf4292244,0x432aff97,0xab9423a7,0xfc93a039,
    0x655b59c3,0x8f0ccc92,0xffeff47d,0x85845dd1,
    0x6fa87e4f,0xfe2ce6e0,0xa3014314,0x4e0811a1,
    0xf7537e82,0xbd3af235,0x2ad7d2bb,0xeb86d391
};

// X下标
const int X_index[] = {
    0,1,2,3,4,5,6,7,
    8,9,10,11,12,13,14,15,
    1,6,11,0,5,10,15,4,
    9,14,3,8,13,2,7,12,
    5,8,11,14,1,4,7,10,
    13,0,3,6,9,12,15,2,
    0,7,14,5,12,3,10,1,8,
    15,6,13,4,11,2,9
};

// MD5类
class MD5 {
public:
    MD5();
    ~MD5();
    string encrypt(string plain);
    // 输出扩展后的消息
    void print_buff();

private:
    // 128位MD缓冲区，md[0..3] = {A, B, C, D}
    vector<unsigned int> md;
    // padding后的消息
    unsigned char* buffer;
    // padding后的消息的长度
    unsigned int buffer_len;

    // 填充
    void padding(string plain);
    void clear();
    // 压缩函数
    void h_md5(int groupid);

    // 4 个轮函数
    // a,b,c,d为MD缓冲区的4个32-bit寄存器
    // X为当前处理消息分组中的32字节
    // s为循环左移位数
    // t为T表元素
    unsigned int FF(unsigned int a,unsigned int b, unsigned int c, unsigned int d,unsigned int X,unsigned int s,unsigned int t);
    unsigned int GG(unsigned int a,unsigned int b, unsigned int c, unsigned int d,unsigned int X,unsigned int s,unsigned int t);
    unsigned int HH(unsigned int a,unsigned int b, unsigned int c, unsigned int d,unsigned int X,unsigned int s,unsigned int t);
    unsigned int II(unsigned int a,unsigned int b, unsigned int c, unsigned int d,unsigned int X,unsigned int s,unsigned int t);

    // 返回 MD 缓冲区转换后的 string 格式密文 
    string md2str();

    // 返回 buffer 中 [pos, pos + 3] 四个字节按照 little-endian 组成的 X
    unsigned int uchar2uint(int pos);
    
    // 返回 unsigned char 对应的十六进制 string
    string uchar2hex(unsigned char uch);

    // 返回 val 循环左移　bits 位的值
    unsigned int rotate_left(unsigned int val, unsigned int bits);

    // 返回第 round 轮迭代中，第 step　步的 X 对应下标
    int get_x_index(int round, int step);
};

#endif