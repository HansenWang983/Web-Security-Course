#include <iostream>
#include "md5.h"
using namespace std;

int main() {
    MD5 md5;

    string message1 = "How it is going !";
    string digest1 = md5.getDigest(message1);

    string message2 = "!@#$abcd1234! ";
    string digest2 = md5.getDigest(message2);

    
    cout << "message1: " << message1 << endl;
    cout << "128 bits digest1 in hex: " << digest1 << endl;
    cout << endl <<endl;
    cout << "message2: " << message2 << endl;
    cout << "128 bits digest2 in hex: " << digest2 << endl;
}