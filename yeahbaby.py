#!bin/python

# Uses https://github.com/cmcqueen/simplerandom
from babydes import babydes_enc, babydes_dec, random, create_key
from base64 import b64encode
import os

# Test BabyDES
#text = os.urandom(32)
#key = os.urandom(32)
#ciphertext = babydes_enc(random, len(text), key, text)
#print base64.b64encode(ciphertext)
#plaintext = babydes_dec(random, len(text), key, ciphertext)
#print '{}: {} =?= {}\n'.format('Success' if plaintext == text else 'Failure', b64encode(plaintext), b64encode(text))

text = "abcdefghijklmnopqrst"
key = create_key("abcdabcdabcdabcda")
print 'Txt: {}'.format(text)
ciphertext = babydes_enc(random, len(text), key, text)
print 'Enc: {}'.format(b64encode(ciphertext))
plaintext = babydes_dec(random, len(text), key, ciphertext)
print 'Dec: {}'.format(plaintext)

