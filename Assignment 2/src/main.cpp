#include <iostream>
#include "md5.h"
using namespace std;

int main() {
    MD5 md5;

    string plain = "12345678";
    string cipher = md5.encrypt(plain);

    cout << "plain: " << plain << endl;
    cout << "cipher: " << cipher << endl;

}