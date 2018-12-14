import queue

# 版本号映射
Version = {
    "0" : "v1",
    "1" : "v2",
    "2" : "v3"
}

# DN映射
DN = {
    "2.5.4.6" : "Country Name",
    "2.5.4.10" : "Organization Name",
    "2.5.4.11" : "Organizational Unit Name",
    "2.5.4.3" : "Common Name",
    "2.5.4.8" : "Sate or Province Name",
    "2.5.4.7" : "Locality Name",
}

# 签名算法映射
ALGORITHM = {
    '1.2.840.10040.4.1': 'DSAEncryption',
    "1.2.840.10040.4.3" : "sha1WithDSAEncryption",
    "1.2.840.113549.1.1.1" :"RSAEncryption",
    "1.2.840.113549.1.1.2" : "md2WithRSAEncryption",
    "1.2.840.113549.1.1.3" : "md4WithRSAEncryption",
    "1.2.840.113549.1.1.4" : "md5WithRSAEncryption",
    "1.2.840.113549.1.1.5" : "sha1WithRSAEncryption",
}

filename = 'cert.der'
f = open(filename,'rb')
Extension = False
Tag = False
Type = 0
parse_string = ""

# 整型输出选项
INTEGER_CHIOCE = queue.Queue()
INTEGER_CHIOCE.put("Version")
INTEGER_CHIOCE.put("Default Version")
INTEGER_CHIOCE.put("Serial Number")

# 16进制输出选项
OCTSTR_CHIOCE = queue.Queue()
OCTSTR_CHIOCE.put("Subject Public Key Encryption")
OCTSTR_CHIOCE.put("SIGNATURE Encryption")

# 时间输出选项
TIME_CHIOCE = queue.Queue()
TIME_CHIOCE.put("Not Before")
TIME_CHIOCE.put("Not After")

# Object输出选项
OBJ_CHIOCE = queue.Queue() 
OBJ_CHIOCE.put("Signature Algorithm")
OBJ_CHIOCE.put("Issuer")
OBJ_CHIOCE.put("Subject")
OBJ_CHIOCE.put("Subject Public Key Info")
OBJ_CHIOCE.put("Signature Algorithm")

def parse_Length(nextByte):
    length = 0
    # 大于127，则用多个字节表示，可以有2到127个字节
    if nextByte > 0x80: 
        nextByte -= 0x80
        for i in range(nextByte):
            length *= 256
            length += ord(f.read(1))
    # 小于等于127，则用一个字节表示
    else:
        length = nextByte
    return length

def parse_Integer():
    res_string = ""
    nextByte = ord(f.read(1))
    # 得到字节长度长度
    length = parse_Length(nextByte)
    for i in range(0, length):
        # 16进制表示
        res_string += hex(ord(f.read(1)))[2:]
    return res_string

def parse_BitString():
    res_string = ""
    nextByte = ord(f.read(1))
    length = parse_Length(nextByte)
    for i in range(0, length):
        # 16进制表示
        temp_str = hex(ord(f.read(1)))[2:]
        # 不足两位则高位补0
        if len(temp_str) != 2:
            temp_str = '0' + temp_str
        res_string += temp_str
    return res_string

def parse_OID():
    res_string = ""
    nextByte = ord(f.read(1))
    length = parse_Length(nextByte)
    # 处理value第一个字节的前两个部分
    initialByte = ord(f.read(1))
    c1 = int(initialByte/40)
    res_string += str(c1) + '.' + str(int(initialByte - c1*40))
    # 处理剩余部分
    i = 0
    while i < length - 1:
        # 当前部分的值
        componet_num = 0
        currentByte = ord(f.read(1))
        i += 1
        # 最高位为1表示当前部分由多个字节存储
        while(currentByte & 0x80 != 0): 
            currentByte &= 0x7F
            componet_num += currentByte
            componet_num *= 128
            # 继续读取下一个字节
            currentByte = ord(f.read(1))
            i += 1
        componet_num += currentByte
        res_string += '.' + str(int(componet_num))
    return res_string

def parse_PrintableString():
    res_string = ""
    nextByte = ord(f.read(1))
    length = parse_Length(nextByte)
    for i in range(0, length):
        res_string +=  str(f.read(1))[2:-1]
    return res_string

# YYMMDDHHMMSSZ
# Z is a literal representation of ‘Zulu’ time
def parse_Time():
    res_string = ""
    nextByte = ord(f.read(1))
    length = parse_Length(nextByte)
    for i in range(0, length):
        res_string += str(f.read(1))[2:-1]
    return res_string


def handle_NextSequence():
    nextByte = ord(f.read(1))
    length = parse_Length(nextByte)
    for i in range(0, length):
        parse_tag()

def print_parse_string(parse_string):
    global Extension
    global Type 
    global Tag
    global OBJ_Count

    if Extension == True:
        print("--------------------EXTENSION BEGIN--------------------")
        # print(parse_string)
        print("--------------------EXTENSION End----------------------")
        Extension = False
        return

    if Type == 2:
        choice = INTEGER_CHIOCE.get()
        if choice == "Serial Number":
            temp = ""
            for i in range(0,len(parse_string)-3):
                temp +=  parse_string[i:i+2] + ":"
            temp += parse_string[len(parse_string)-2:len(parse_string)]
            print(choice,":\n",temp,"\n")
        elif choice != "Default Version":
            print(choice,": ",Version[parse_string],"\n")

    elif Type == 3:
        choice = OCTSTR_CHIOCE.get()
        if choice == "Subject Public Key Encryption":
            parse_string = parse_string[18:18+514]
            temp = ""
            for i in range(0,len(parse_string)-3):
                temp +=  parse_string[i:i+2] + ":"
            temp += parse_string[len(parse_string)-2:len(parse_string)]
            print(choice,":\n",temp,"\n")
        else: 
            parse_string = parse_string[2:]
            temp = ""
            for i in range(0,len(parse_string)-3):
                temp +=  parse_string[i:i+2] + ":"
            temp += parse_string[len(parse_string)-2:len(parse_string)]
            print(choice,":\n",temp,"\n")

    elif Type == 5:
        print(parse_string,"\n")
        
    elif Type == 6:
        # DN
        if DN.get(parse_string,-1) != -1:
            parse_string = DN[parse_string]
            if parse_string == "Country Name":
                choice = OBJ_CHIOCE.get()
                print(choice)
            print(parse_string)
        # Algorithm
        elif ALGORITHM.get(parse_string, -1)!= -1:
            parse_string = ALGORITHM[parse_string]
            choice = OBJ_CHIOCE.get()
            print(choice,parse_string)

    elif Type == 0x17 or Type == 0x18:
        choice = TIME_CHIOCE.get()
        if choice == "Not Before":
            print("VALIDITY",)
            print(choice,": ",parse_string)
        else:
            print(choice,": ",parse_string)

    elif Type == 0x13 or Type == 0x0c:
        print(parse_string,"\n")

def parse_tag():   
    global Extension
    global Type 
    global Tag
    global parse_string

    if Tag == False:
        data = f.read(1)
        if(len(data) == 0):
            print('read end of file')
            exit(0)
        else:
            Type = ord(data)
    else: 
        # 如果上一个为Tag，则跳过之后的Length读取
        # 读取下一Length字节
        # f.read(1)
        Tag = False 

    # 判断是否为Unversal Type
    if Type < 0x80:
        # print("TagClass: universal")
        if Type == 1:
            print("Type: BOOLEAN")
        elif Type == 2:
            # print("Type: INTEGER")
            parse_string = parse_Integer() 
        elif Type == 3:
            # print("Type: BIT STRING")
            parse_string = parse_BitString() 
        elif Type == 4:
            print("Type: OCTET STRING")
        elif Type == 5:
            # print("Type: NULL")
            f.read(1)
        elif Type == 6:
            # print("Type: OBJECT IDENTIFIER(OID)")
            parse_string = parse_OID() 
        elif Type == 0x0c:
            # print("Type: UFT8String")
            parse_string = parse_PrintableString() 
        elif Type == 0x13:
            # print("Type: PrintableString")
            parse_string = parse_PrintableString() 
        elif Type == 0x17 or Type == 0x18:
            # print("Type: UTCTime & GeneralizedTime")
            parse_string = parse_Time() 
        elif Type == 0x30 or Type == 0x31:
            # print("Type: SEQUENCE & SET")
            handle_NextSequence()
        elif Type == 0:
            # print("Type: 0")
            # 跳过
            nextByte = ord(f.read(1))
            if(nextByte > 0x80):
                length = parse_Length(nextByte)
            parse_tag()
        
        # 输出
        print_parse_string(parse_string)


    elif Type >= 0x80 and Type < 0xa0:
        # 隐式Tag
        implicit_tag = Type - 0x80
        Tag = True
        Type = implicit_tag
        parse_tag()

    elif Type >= 0xa0:
        # print("TagClass: context-specific")
        # 显式Tag
        explicit_tag = Type - 0xa0
        Tag = True
        if explicit_tag == 3:
            Extension = True
        Type = explicit_tag
        parse_tag()

if __name__ == "__main__":
    parse_tag()