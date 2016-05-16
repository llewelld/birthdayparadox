#!bin/python

import random
from base64 import b64encode
import os

# BabyDES encrypt
# Variable length Feistel cipher
# Only as secure as the PRF you give it
def babydes_enc(prf, width, key, cleartext):
	assert width % 2 == 0, 'width must be multiple of two.'
	assert width == len(cleartext), 'Cleartext must have length width'
	halfwidth = width / 2
	left = bytearray(cleartext[:halfwidth])
	right = bytearray(cleartext[halfwidth:])
	keybytes = bytearray(key)

	# Round 1
	bitstream = prf(halfwidth, key, right)
	for pos in range(halfwidth):
		left[pos] ^= bitstream[pos]

	# Round 2
	bitstream = prf(halfwidth, key, left)
	for pos in range(halfwidth):
		right[pos] ^= bitstream[pos]

	# Round 3
	bitstream = prf(halfwidth, key, right)
	for pos in range(halfwidth):
		left[pos] ^= bitstream[pos]

	return left + right

# BabyDES decrypt
# Variable length Feistel cipher
# Only as secure as the PRF you give it
def babydes_dec(prf, width, key, ciphertext):
	assert width % 2 == 0, 'width must be multiple of two.'
	assert width == len(ciphertext), 'Ciphertext must have length width'
	halfwidth = width / 2
	left = bytearray(ciphertext[:halfwidth])
	right = bytearray(ciphertext[halfwidth:])
	keybytes = bytearray(key)

	# Round 1
	bitstream = prf(halfwidth, key, right)
	for pos in range(halfwidth):
		left[pos] ^= bitstream[pos]

	# Round 2
	bitstream = prf(halfwidth, key, left)
	for pos in range(halfwidth):
		right[pos] ^= bitstream[pos]

	# Round 3
	bitstream = prf(halfwidth, key, right)
	for pos in range(halfwidth):
		left[pos] ^= bitstream[pos]

	return left + right


# Non cryptographycally secure PRF
def prf(width, key, datain):
	random.seed(key)
	random.jumpahead(datain)
	return bytearray(random.getrandbits(8) for x in xrange(width))

text = os.urandom(32)
key = os.urandom(32)
ciphertext = babydes_enc(prf, len(text), key, text)
#print base64.b64encode(ciphertext)
plaintext = babydes_dec(prf, len(text), key, ciphertext)
print '{}: {} =?= {}'.format('Success' if plaintext == text else 'Failure', b64encode(plaintext), b64encode(text))

