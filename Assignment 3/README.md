# 实验报告

设计并实现一个小程序，读入一个 X.509 数字证书，按照标准定义给出证书中有关项目的中 (英) 文内容陈述。



## X.509 证书结构描述

![1](Assets/1.jpg)



## 数据结构

### 一、证书格式

同样的 X.509证书，可能有不同的编码格式。最常见的就是 PEM 和 DER 两种格式。但是有个比较误导人的地方，这两种格式的文件，扩展名不一定就是 `.pem` 或 `.der`，还有其他一些常见格式。它们除了编码格式可能不同之外，内容也有差别。但是大多数都能相互转换编码格式。但两种证书中都没有私钥信息。一般CA使用自己的私钥将证书进行加密，申请证书者可以用CA的公钥对证书进行解密。区别如下：

1. Base64 编码的证书（PEM）

- 包括`BEGIN/END` 声明

- 使用` openssl x509 -in certificate.pem -text -noout`查看 PEM 格式证书的信息



2. 二进制编码的证书（DER）

- 辨别编码规则  DER（Distinguished Encoding Rules）是 ASCII PEM 格式证书的二进制形式。所有类型的证书和私钥都可以被编码为 DER 格式。
- 它是大多数浏览器的缺省格式，并按 ASN1 DER 格式存储。它是无报头的。
- 不包括`BEGIN/END` 声明。
- PEM 是用文本报头包围的 DER。
- 使用` openssl x509 -in certificate.der -inform der -text -noout`查看 DER 格式证书的信息



格式转换：

```bash
openssl x509 -outform der -in certificate.pem -out certificate.der #PEM 转为 DER

openssl x509 -inform der -in certificate.cer -out certificate.pem #DER 转为 PEM
```



### 二、编码规则

二进制DER文件按照**Type-Length-Value**的编码格式存储ANS.1中的数据结构，其中**ANS.1（abstraction notation standard）为抽象符号标准**， 是为了解决高级语言中结构化数据在网络传输中的结构关系能送达目的地进行还原，类似其他几种数据序列化的方法：XML，JSON等，ASN.1本身只定义了表示信息的抽象句法，但是没有限定其编码的方法，本次**解析的是DER编码规则**。



参考：

1. [ASN1 decoder](https://lapo.it/asn1js/#MIIG4TCCBcmgAwIBAgIQCd0Ux6hVwNaX-SICZIR_jzANBgkqhkiG9w0BAQUFADBmMQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3d3cuZGlnaWNlcnQuY29tMSUwIwYDVQQDExxEaWdpQ2VydCBIaWdoIEFzc3VyYW5jZSBDQS0zMB4XDTEzMDUxNDAwMDAwMFoXDTE2MDUxODEyMDAwMFowYDELMAkGA1UEBhMCQ0ExEDAOBgNVBAgTB0FsYmVydGExEDAOBgNVBAcTB0NhbGdhcnkxGTAXBgNVBAoTEFNBSVQgUG9seXRlY2huaWMxEjAQBgNVBAMMCSouc2FpdC5jYTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAJv2n5mZfX6NV0jZof1WdXGiY5Q_W0yDT6tUdIYUjgS8GDkeZJYjtwUCMYD2Wo3rF_1ZJ8p9p2WBP1F3CVvjgO-VeA7tLJsfuAr-S8GE1q5tGO9-lPFkBAZkU38FNfBUblvz1imWb6ORXMc--HjUlrUB0nr2Ae8T1I3K0XGArHJyW5utJ5Xm8dNEYCcs6EAXchiViVtcZ2xIlSQMs-AqhqnZXo2Tt1H-f_tQhQJeMTkZ2kklUcnQ1izdTigMgkOvNzW4Oyd9Z0sBbxzUpneeH3nUB5bEv3MG4JJx7cAVPE4rqjVbtm3v0QbCL_X0ZncJiKl7heKWO-j3DnDZS_oliIkCAwEAAaOCA48wggOLMB8GA1UdIwQYMBaAFFDqc4nbKfsQj57lASDU3nmZSIP3MB0GA1UdDgQWBBTk00KEbrhrTuVWBY2cPzTJd1c1BTBkBgNVHREEXTBbggkqLnNhaXQuY2GCB3NhaXQuY2GCCmNwLnNhaXQuY2GCDmNwLXVhdC5zYWl0LmNhghd1YXQtaW50ZWdyYXRpb24uc2FpdC5jYYIQdWF0LWFwYXMuc2FpdC5jYTAOBgNVHQ8BAf8EBAMCBaAwHQYDVR0lBBYwFAYIKwYBBQUHAwEGCCsGAQUFBwMCMGEGA1UdHwRaMFgwKqAooCaGJGh0dHA6Ly9jcmwzLmRpZ2ljZXJ0LmNvbS9jYTMtZzIxLmNybDAqoCigJoYkaHR0cDovL2NybDQuZGlnaWNlcnQuY29tL2NhMy1nMjEuY3JsMIIBxAYDVR0gBIIBuzCCAbcwggGzBglghkgBhv1sAQEwggGkMDoGCCsGAQUFBwIBFi5odHRwOi8vd3d3LmRpZ2ljZXJ0LmNvbS9zc2wtY3BzLXJlcG9zaXRvcnkuaHRtMIIBZAYIKwYBBQUHAgIwggFWHoIBUgBBAG4AeQAgAHUAcwBlACAAbwBmACAAdABoAGkAcwAgAEMAZQByAHQAaQBmAGkAYwBhAHQAZQAgAGMAbwBuAHMAdABpAHQAdQB0AGUAcwAgAGEAYwBjAGUAcAB0AGEAbgBjAGUAIABvAGYAIAB0AGgAZQAgAEQAaQBnAGkAQwBlAHIAdAAgAEMAUAAvAEMAUABTACAAYQBuAGQAIAB0AGgAZQAgAFIAZQBsAHkAaQBuAGcAIABQAGEAcgB0AHkAIABBAGcAcgBlAGUAbQBlAG4AdAAgAHcAaABpAGMAaAAgAGwAaQBtAGkAdAAgAGwAaQBhAGIAaQBsAGkAdAB5ACAAYQBuAGQAIABhAHIAZQAgAGkAbgBjAG8AcgBwAG8AcgBhAHQAZQBkACAAaABlAHIAZQBpAG4AIABiAHkAIAByAGUAZgBlAHIAZQBuAGMAZQAuMHsGCCsGAQUFBwEBBG8wbTAkBggrBgEFBQcwAYYYaHR0cDovL29jc3AuZGlnaWNlcnQuY29tMEUGCCsGAQUFBzAChjlodHRwOi8vY2FjZXJ0cy5kaWdpY2VydC5jb20vRGlnaUNlcnRIaWdoQXNzdXJhbmNlQ0EtMy5jcnQwDAYDVR0TAQH_BAIwADANBgkqhkiG9w0BAQUFAAOCAQEAcl2YI0iMOwx2FOjfoA8ioCtGc5eag8Prawz4FFs9pMFZfD_K8QvPycMSkw7kPtVjmuQWxNtRAvCSIhr_urqNLBO5Omerx8aZYCOznsmZpymxMt56DBw-KZrWIodsZx5QjVngbE_qIDLmsYgtKczhTCtgEM1h_IHlO3Ho7IXd2Rr4CqeMoM2v-MTV2FYVEYUHJp0EBU_AMuBjPf6YT_WXMNq6fn-WJpxcqwJJKtBh7c2vRTklahbh1FaiJ0aFJkDH4tasbD69JQ8R2V5OSuGH6Q7EGlpNl-unqtUyKsAL86HvgzF5D51C9TmFXEtXTlPKnjoqn1TC4Rqpqvh-FHWPJQ)
2. [ASN1字段类型](https://nostdahl.com/2017/08/11/x-509-certificates-explained/)



### 三、具体字段

总体嵌套结构：

```
证书总体长度-(4 Bytes)
    -待签名证书-
              -版本号
              -序列号
              -算法标识
              -签发者
              -......
    -签名算法
    -PKCS签名（128 Bytes）
```



1. 之前说过ASN.1以TLV（Tag标签、Length长度、Value值）来表示数据，ASN.1格式中的第一个字节表示了类型，在ASN.1中，有预定义的30种类型，只需要5个bit就够了，剩余的3个bits表示什么呢？

   通常这一个字节被分为了3段。 最高的两位bit[7]和bit[6]表示TagClass，表示这个标签的类

![1](Assets/2.jpg)

接下来的一位bit[5]是表示这个标签是基本类型（0）还是结构化类型（1），如果是结构化类型的话，后面会再跟一层TVL；最后的5位才是Tag Value。

| Tag Value | Tag类型                    |
| --------- | -------------------------- |
| 1         | BOOLEAN                    |
| 2         | INTEGER                    |
| 3         | BIT STRING                 |
| 4         | OCTET STRING               |
| 5         | NULL                       |
| 6         | OBJECT IDENTIFIER          |
| 7         | ObjectDescripion           |
| 8         | EXTERNAL,INSTANCE OF       |
| 9         | REAL                       |
| 10        | ENUMERATED                 |
| 11        | EMBEDDED PDV               |
| 12        | UFT8String                 |
| 13        | RELATIVE-OID               |
| 14        | 保留                       |
| 15        | 保留                       |
| 16        | ,SEQUENCE OF               |
| 17        | SET,SET OF                 |
| 18        | NumericString              |
| 19        | PrintableString            |
| 20        | TeletexString,T61String    |
| 21        | VideotexString             |
| 22        | IA5String                  |
| 23        | UTCTime                    |
| 24        | GeneralizedTime            |
| 25        | GraphicString              |
| 26        | VisibleString,ISO646String |
| 27        | GeneralString              |
| 28        | UniversalString            |
| 29        | CHARACTER STRING           |
| 30        | BMPString                  |
| 31        | 保留                       |

例如：

DER二进制文件的前4个字节构成具有剩余字节的ASN.1序列。

如`30 82 06 E1`根据Type-Length-Value表示，第一个字节30（00110000）表示是一个universal class type，后面是一个结构化类型的SEQUENCE。



2. Length长度字段，有两种编码格式。

- 若长度值小于等于127，则用一个字节表示，bit8 = 0, bit7-bit1 存放长度值；
- 若长度值大于127，则用多个字节表示，可以有2到127个字节。第一个字节的第8位为1，其它低7位给出后面该域使用的字节的数量，从该域第二个字节开始给出数据的长度，高位优先。
- 还有一种特殊情况，这个字节为0x80，表示数据块长度不定，由数据块结束标识结束数据块

继续之前的例子：

如`30 82 06 E1`根据Type-Length-Value表示，第二个字节82（10000010）表示长度值大于127，且长度值用后面两个字节表示，06 E1表示1761为长度值。



3. Value数据块字段：存放数据块的值，具体编码随数据块类型不同而不同

4. 数据块结束标识（可选），结束标示字段，两个字节（0x0000）,只有在长度值为不定时才会出现。



**证书中具体的数据结构**（From RFC 5280）

```
Certificate ::= SEQUENCE {
 tbsCertificate TBSCertificate,
 signatureAlgorithm AlgorithmIdentifier,
 signatureValue BIT STRING }
 
TBSCertificate  ::=  SEQUENCE  {
    version         [0]  EXPLICIT Version DEFAULT v1,
    serialNumber         CertificateSerialNumber,
    signature            AlgorithmIdentifier,
    issuer               Name,
    validity             Validity,
    subject              Name,
    subjectPublicKeyInfo SubjectPublicKeyInfo,
    issuerUniqueID  [1]  IMPLICIT UniqueIdentifier OPTIONAL,
                         -- If present, version shall be v2 or v3
    subjectUniqueID [2]  IMPLICIT UniqueIdentifier OPTIONAL,
                         -- If present, version shall be v2 or v3
    extensions      [3]  EXPLICIT Extensions OPTIONAL
                         -- If present, version shall be v3        }
```



**证书版本**

证书版本是一个整数，现有的证书版本有三个

- V1版本：对应值为0，证书只有基本项时为V1
- V2版本：对应值为1，证书中除了基本型外，还包含了签发者ID与使用者ID
- V3版本：对应值为2，证书中包含扩展项

```
Version  ::=  INTEGER  {  v1(0), v2(1), v3(2)  }
```



**证书序列号**

证书序列号是一串整数，对于每一张证书，证书序列号应该是唯一的。

```
CertificateSerialNumber  ::=  INTEGER
```



**有效期**

有效期是一个时间序列，包含签发时间与过期时间，可选的类型有UTCTime和GeneralizedTime两种。

- UTCTime的格式是yyMMddHHmmssZ
- GeneralizedTime的格式是yyyyMMddHHmmss.sZ 在2050年以后的证书时间必须要使用GeneralizedTime

Tips：GMT时间与UTC时间稍有不同

```
Validity ::= SEQUENCE {
     notBefore      Time,
     notAfter       Time }

Time ::= CHOICE {
     utcTime        UTCTime,
     generalTime    GeneralizedTime }
```



**签发者和使用者**

由于签发者和使用者的类型都是一样的，是一些DN项（Distinguish Name）的集合,所以放在一起来说。

常见的DN项有：

| 属性类型名称             | 含义         | 简写 |
| ------------------------ | ------------ | ---- |
| Common Name              | 通用名称     | CN   |
| Organizational Unit name | 机构单元名称 | OU   |
| Organization name        | 机构名       | O    |
| Locality                 | 地理位置     | L    |
| State or province name   | 州/省名      | S    |
| Country                  | 国名         | C    |

```
UniqueIdentifier  ::=  BIT STRING
```



**公钥信息**

```
SubjectPublicKeyInfo  ::=  SEQUENCE  {
     algorithm            AlgorithmIdentifier,
     subjectPublicKey     BIT STRING  }
```



**扩展选项**

```
Extensions  ::=  SEQUENCE SIZE (1..MAX) OF Extension

Extension  ::=  SEQUENCE  {
     extnID      OBJECT IDENTIFIER,
     critical    BOOLEAN DEFAULT FALSE,
     extnValue   OCTET STRING  }
```





## 运行结果