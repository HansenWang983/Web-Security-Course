#include "md5.h"

/* -- public --*/
MD5::MD5() {
    buffer = NULL;
    md.push_back(0x67452301);
    md.push_back(0xefcdab89);
    md.push_back(0x98badcfe);
    md.push_back(0x10325476);
}

MD5::~MD5() {
    clear();
}

string MD5::encrypt(string plain) {
    clear();
    padding(plain);

    int group_len = buffer_len / 64;

    for (int i = 0; i < group_len; ++i) 
        h_md5(i);

    return md2str();
}

void MD5::print_buff() {
    cout << "buffer_len = " << buffer_len << endl;
    for (int i = 0; i < buffer_len; ++i) {
        bitset<8> ch = buffer[i];
        cout << ch << " ";
    }
    cout << endl;
}

/* -- private --*/

void MD5::clear() {
    if (buffer != NULL) {
        delete []buffer;
        buffer = NULL;
    }
}

// 填充 padding 和 length
void MD5::padding(string plain) {

    unsigned int plain_len = plain.size();
    // 原始信息位数K
    unsigned long long K = plain.size() * 8;
    // padding位数P
    unsigned int P = K % 512 == 448 ? 512 : (960 - K % 512) % 512;

    unsigned int fill_len = P / 8;
    // padding后的信息字节数
    buffer_len = plain_len + fill_len + 8;
    buffer = new unsigned char[buffer_len];

    // 复制原始消息
    for (int i = 0; i < plain_len; ++i) 
        buffer[i] = plain[i];

    // padding
    buffer[plain_len] = 0x80;
    for (int i = 1; i < fill_len; ++i) 
        buffer[plain_len + i] = 0;

    // 填充原始消息
    for (int i = 0; i < 8; ++i) {
        unsigned char ch = K;
        buffer[plain_len + fill_len + i] = ch;
        K >>= 8;
    }
}

// 压缩函数
// 传入分组序号，从0开始，每64字节一组
void MD5::h_md5(int groupid) {

    int buff_begin = 64 * groupid;

    // 迭代输出
    unsigned int next;

    // 128位缓冲区
    vector<unsigned int> last_md(md);

    const unsigned int rotate_left_bits[4][4] = {
        {7, 12, 17, 22},
        {5, 9, 14, 20},
        {4, 11, 16, 23},
        {6, 10, 15, 21}
    };

    // round = [0, 1, 2, 3] 分别对应 [F, G, H, I] 4轮
    for (int round = 0; round < 4; round++) {
        // 每轮迭代次数
        for (int iter = 0; iter < 16; iter++) {
            // 每轮循环中的一次迭代运算逻辑

            // 当前处理消息分组的第 k 个 (k = 0..15) 32位字
            unsigned int X = uchar2uint(buff_begin + get_x_index(round, iter) * 4);
            // T 表的第 i 个元素，32位字
            // unsigned int t = 0x100000000UL * abs(sin(round * 16 + i + 1));
            unsigned int t = T[round*16+iter];

            switch(round){
                case 0:
                    next = FF(md[0], md[1], md[2], md[3], X, rotate_left_bits[round][iter % 4], t);
                    break;
                case 1:
                    next = GG(md[0], md[1], md[2], md[3], X, rotate_left_bits[round][iter % 4], t);
                    break;
                case 2:
                    next = HH(md[0], md[1], md[2], md[3], X, rotate_left_bits[round][iter % 4], t); 
                    break;
                case 3:
                    next = II(md[0], md[1], md[2], md[3], X, rotate_left_bits[round][iter % 4], t);
                    break;
            }   

            // 缓冲区 (A, B, C, D) 作循环轮换
            md[0] = md[3];
            md[3] = md[2];
            md[2] = md[1];
            md[1] = next;
        }
    }
    // 得到最后一次迭代的结果
    for (int i = 0; i < 4; ++i) 
        md[i] += last_md[i];
}

// 4 个轮函数
unsigned int MD5::FF(unsigned int a,unsigned int b, unsigned int c, unsigned int d,unsigned int X,unsigned int s,unsigned int t){
    unsigned int g = (b & c) | (~b & d) ;
    unsigned int res = a + g + X + t;
    return  rotate_left(res,s) + b;
}
unsigned int MD5::GG(unsigned int a,unsigned int b, unsigned int c, unsigned int d,unsigned int X,unsigned int s,unsigned int t){
    unsigned int g = (b & d) | (c & ~d);
    unsigned int res = a + g + X + t;
    return  rotate_left(res,s) + b;
}
unsigned int MD5::HH(unsigned int a,unsigned int b, unsigned int c, unsigned int d,unsigned int X,unsigned int s,unsigned int t){
    unsigned int g =  b ^ c ^ d;
     unsigned int res = a + g + X + t;
    // res = rotate_left(res + a,s);
    return  rotate_left(res,s) + b;
}
unsigned int MD5::II(unsigned int a,unsigned int b, unsigned int c, unsigned int d,unsigned int X,unsigned int s,unsigned int t){
    unsigned int g = c ^ (b | ~d);
     unsigned int res = a + g + X + t;
    return  rotate_left(res,s) + b;
}

// 返回 MD 缓冲区转换后的 string 格式密文
string MD5::md2str() {
    string res;

    for (int i = 0; i < 4; ++i) {
        unsigned int val = md[i];
        for (int j = 0; j < 4; ++j) {
            unsigned char ch = val;
            res += uchar2hex(ch);
            val >>= 8;
        }
    }

    return res;
}

// 返回 buffer 中 [pos, pos + 3] 四个字节按照 little-endian 组成的 X
unsigned int MD5::uchar2uint(int pos) {
    unsigned int val = 0;
    int end = pos + 3;
    for (int i = end; i >= pos; --i) {
        val |= buffer[i];
        if (i != pos) val <<= 8;
    }
    return val;
}

// 返回 unsigned char 对应的十六进制 string
string MD5::uchar2hex(unsigned char uch) {
    string res;
    unsigned char mask = 0x0F;

    for (int i = 1; i >= 0; --i) {
        char ch = uch >> (i << 2) & mask;
        if (ch < 10) ch += '0';
        else ch += 'A' - 10;
        res += ch;
    }

    return res;
}

// 返回 val 循环左移　bits 位的值
unsigned int MD5::rotate_left(unsigned int val, unsigned int bits) {
    bits %= 32;
    return (val << bits) | (val >> (32 - bits));
}

// 返回第 round 轮迭代中，第 step　步的 X 对应下标
int MD5::get_x_index(int round, int step) {
    if (round == 0) {
        return step;
    } else if (round == 1) {
        return (1 + 5 * step) % 16;
    } else if (round == 2) {
        return (5 + 3 * step) % 16;
    } else {
        return (7 * step) % 16;
    }
}