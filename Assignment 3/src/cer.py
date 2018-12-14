

class X509(object):

    def  __init__(self,filename):

        self.f = open(filename, 'rb')
        self.Tag = False
        self.Extension = False
        self.Type = 0

    def parse_tag(self):

        if  self.Tag == False:
            data = self.f.read(1)
            if(len(data) == 0):
                print('read end of file')
                exit(0)
            else:
                self.Type = ord(data)
        else: 
            # 如果之前为Tag，则进行第二次的Type处理，不继续读
            self.Tag = False

        # 判断是否为Unversal Type
        if self.Type < 0x80:
            # print("TagClass: universal")
            if self.Type == 1:
                print("Type: BOOLEAN")
            elif self.Type == 2:
                print("Type: INTEGER")
                parse_string = self.parse_Integer() 
            elif self.Type == 3:
                print("Type: BIT STRING")
                # parse_string = self.parse_BitString() 
                if self.Extension == True:
                    parse_string = self.parse_PrintableString()
                else:
                    parse_string = self.parse_BitString()
            elif self.Type == 4:
                print("Type: OCTET STRING")
            elif self.Type == 5:
                print("Type: NULL")
                # 跳过下一字节
                self.f.read(1)
            elif self.Type == 6:
                print("Type: OBJECT IDENTIFIER(OID)")
                parse_string = self.parse_OID() 
            elif self.Type == 0x0c:
                print("Type: UFT8String")
                parse_string = self.parse_PrintableString() 
            elif self.Type == 0x13:
                print("Type: PrintableString")
                parse_string = self.parse_PrintableString() 
            elif self.Type == 0x17 or self.Type == 0x18:
                print("Type: UTCTime & GeneralizedTime")
                parse_string = self.parse_Time() 
            elif self.Type == 0x30 or self.Type == 0x31:
                print("Type: SEQUENCE & SET")
                self.handle_NextSequence()
            elif self.Type == 0:
                print("Type: 0")
                # 跳过Length
                nextByte = ord(self.f.read(1))
                if(nextByte > 0x80):
                    length = self.parse_Length(nextByte)
                self.parse_tag()

        elif self.Type >= 0xa0:
            # print("TagClass: context-specific")
            # 显式Tag
            explicit_tag = self.Type - 0xa0
            self.Tag = True
            if(explicit_tag == 0):
                print("version [0] EXPLICIT Version DEFAULT v1")
            elif explicit_tag == 3:
                self.Extension = True
            self.Type = explicit_tag
            self.parse_tag()

        else:
            # 隐式Tag
            implicit_tag = self.Type - 0x80
            self.Tag = True
            if implicit_tag == 1:
                print ("--------------------issuerUniqueID--------------------")
            elif implicit_tag == 2:
                print ("--------------------subjectUniqueID--------------------")
            self.Type = implicit_tag
            self.parse_tag()

    
    def parse_Length(self,nextByte):
        length = 0
        # 大于127，则用多个字节表示，可以有2到127个字节
        if nextByte > 0x80: 
            nextByte -= 0x80
            for i in range(nextByte):
                length *= 256
                length += ord(self.f.read(1))
        # 小于等于127，则用一个字节表示
        else:
            length = nextByte
        return length

    def parse_Integer(self):
        res_string = ""
        nextByte = ord(self.f.read(1))
        length = self.parse_Length(nextByte)
        for i in range(0, length):
            # 16进制表示
            res_string += hex(ord(self.f.read(1)))[2:]
        return res_string

    def parse_BitString(self):
        res_string = ""
        nextByte = ord(self.f.read(1))
        length = self.parse_Length(nextByte)
        for i in range(0, length):
            # 16进制表示
            temp_str = hex(ord(self.f.read(1)))[2:]
            # 不足两位则高位补0
            if len(temp_str) != 2:
                temp_str = '0' + temp_str
            res_string += temp_str
        return res_string

    def parse_OID(self):
        res_string = ""
        nextByte = ord(self.f.read(1))
        length = self.parse_Length(nextByte)
        # 处理value第一个字节的前两个部分
        initialByte = ord(self.f.read(1))
        c1 = int(initialByte/40)
        res_string += str(c1) + '.' + str(int(initialByte - c1*40))
        # 处理剩余部分
        i = 0
        while i < length - 1:
            # 当前部分的值
            componet_num = 0
            currentByte = ord(self.f.read(1))
            i += 1
            # 最高位为1表示当前部分由多个字节存储
            while(currentByte & 0x80 != 0): 
                currentByte &= 0x7F
                componet_num += currentByte
                componet_num *= 128
                # 继续读取下一个字节
                currentByte = ord(self.f.read(1))
                i += 1
            componet_num += currentByte
            res_string += '.' + str(int(componet_num))
            
        # print(res_string)
        return res_string

    def parse_PrintableString(self):
        res_string = ""
        nextByte = ord(self.f.read(1))
        length = self.parse_Length(nextByte)
        for i in range(0, length):
            # 以ASCII码显示
            res_string +=  str(self.f.read(1))[2:-1]
        return res_string

    # YYMMDDHHMMSSZ
    # Z is a literal representation of ‘Zulu’ time
    def parse_Time(self):
        res_string = ""
        nextByte = ord(self.f.read(1))
        length = self.parse_Length(nextByte)
        for i in range(0, length):
            res_string += str(self.f.read(1))[2:-1]
        return res_string


    def handle_NextSequence(self):
        nextByte = ord(self.f.read(1))
        length = self.parse_Length(nextByte)
        for i in range(0, length):
            self.parse_tag()

if __name__ == "__main__":
    x509 = X509('cert.der')
    x509.parse_tag()