#include <iostream>
#include <stdio.h>
#include <fstream>
#include <openssl/x509.h>

using namespace std;

//  X509 *d2i_X509(X509 **px, const unsigned char **in, long len);

int main(int argc, char const *argv[])
{
    // ifstream file;
    // file.open("smp.ruijie.com.cn.cer",ios::in);
    FILE *fr;
    fr=fopen("smp.ruijie.com.cn.cer","rb");
    if(fr==NULL)  
    {  
        cout << "Read Error!"<<endl;
    }  
    unsigned char Cert[4099];  
    unsigned long Certlen = 0;  
    unsigned char *pTmp = NULL;  
    X509 *usrCert = NULL; 

    Certlen=fread(Cert,1,4096,fr);  
    pTmp=Cert;  
    
    usrCert = d2i_X509(NULL,(const unsigned char ** )&pTmp,Certlen);  
    return 0;
}
