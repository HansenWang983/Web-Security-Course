from asn1crypto import x509, pem

with open('cert.pem', 'rb') as f:
    data = f.read()
    if pem.detect(data):
        _, _, data = pem.unarmor(data)
    cert = x509.Certificate.load(data)
    for rdn in cert.subject.chosen:
        for type_value in rdn:
            type_name = type_value['type'].native
            value_class = type_value['value'].chosen.__class__.__name__
            print('%s: %s' % (type_name, value_class))