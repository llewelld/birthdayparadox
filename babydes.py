#!bin/python

# Uses https://github.com/cmcqueen/simplerandom
import simplerandom.iterators as srr
import binascii
from base64 import b64encode
import math
import struct

# BabyDES encrypt
# Variable length Feistel cipher
# Only as secure as the PRF you give it
def babydes_enc(prf, width, key, cleartext):
	assert width % 2 == 0, 'Width must be multiple of two.'
	assert width == len(cleartext), 'Cleartext must have length width'
	halfwidth = width / 2
	left = bytearray(cleartext[:halfwidth])
	right = bytearray(cleartext[halfwidth:])

	# Round 1
	bitstream = prf(halfwidth, key, 0, right)
	for pos in range(halfwidth):
		left[pos] ^= bitstream[pos]

	# Round 2
	bitstream = prf(halfwidth, key, 1, left)
	for pos in range(halfwidth):
		right[pos] ^= bitstream[pos]

	# Round 3
	bitstream = prf(halfwidth, key, 2, right)
	for pos in range(halfwidth):
		left[pos] ^= bitstream[pos]

	return left + right

# BabyDES decrypt
# Variable length Feistel cipher
# Only as secure as the PRF you give it
def babydes_dec(prf, width, key, ciphertext):
	assert width % 2 == 0, 'Width must be multiple of two.'
	assert width == len(ciphertext), 'Ciphertext must have length width'
	halfwidth = width / 2
	left = bytearray(ciphertext[:halfwidth])
	right = bytearray(ciphertext[halfwidth:])

	# Round 1
	bitstream = prf(halfwidth, key, 2, right)
	for pos in range(halfwidth):
		left[pos] ^= bitstream[pos]

	# Round 2
	bitstream = prf(halfwidth, key, 1, left)
	for pos in range(halfwidth):
		right[pos] ^= bitstream[pos]

	# Round 3
	bitstream = prf(halfwidth, key, 0, right)
	for pos in range(halfwidth):
		left[pos] ^= bitstream[pos]

	return left + right

# Non cryptographycally secure PRF
def random(width, key, roundn, datain):
	# Create as many random number sequences as we can get away with
	keys = len(key)
	rngnum = int((keys + 3) / 4);
	rng = []
	for count in range(rngnum):
		shift = count + roundn
		rng.append(srr.KISS(key[shift % keys], key[(shift + 1) % keys], key[(shift + 2) % keys], key[(shift + 3) % keys]))

	bufferout = bytearray()
	# Select output bytes as they come out of thee random number generators
	for x in range(width):
		if x % 4 == 0:
			random = sum(next(y) for y in rng)
		# Ensure output depends on input
		rng[x % rngnum].jumpahead(datain[x])
		random += next(rng[x % rngnum])
		bufferout.append((random >> ((x % 4) * 8)) & 0xff)

	return bufferout

# Create a key sequence from a string
def create_key(key):
	if len(key) % 4 != 0:
		key += '\x00' * (4 - (len(key) % 4))
	keydata = []
	for pos in range(0, len(key), 4):
		keydata.append(struct.unpack("<L", key[pos:pos + 4])[0])
	return keydata

