
Version = {
    ""
}
def parse_tag(data):
    print(data)

def read_file(filepath):
    f = open(filepath,'rb')
    data = f.read(100)
    parse_tag(data)


if __name__ == "__main__":
    filename = 'cert.der'
    read_file(filename)