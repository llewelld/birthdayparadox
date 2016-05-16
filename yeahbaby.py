#!bin/python

# Uses https://github.com/cmcqueen/simplerandom
from babydes import babydes_enc, babydes_dec, prf
from base64 import b64encode
import os

# Test BabyDES
text = os.urandom(32)
key = os.urandom(32)
ciphertext = babydes_enc(prf, len(text), key, text)
#print base64.b64encode(ciphertext)
plaintext = babydes_dec(prf, len(text), key, ciphertext)
print '{}: {} =?= {}'.format('Success' if plaintext == text else 'Failure', b64encode(plaintext), b64encode(text))

