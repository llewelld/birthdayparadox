#!bin/python

# Uses https://github.com/cmcqueen/simplerandom
import simplerandom.random as srr
import binascii

# BabyDES encrypt
# Variable length Feistel cipher
# Only as secure as the PRF you give it
def babydes_enc(prf, width, key, cleartext):
	assert width % 2 == 0, 'Width must be multiple of two.'
	assert width == len(cleartext), 'Cleartext must have length width'
	halfwidth = width / 2
	left = bytearray(cleartext[:halfwidth])
	right = bytearray(cleartext[halfwidth:])
	keybytes = bytearray(key)

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
	keybytes = bytearray(key)

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
def prf(width, key, roundn, datain):
	rng = srr.KISS(key + str(roundn))
	rng.jumpahead(int(binascii.hexlify(datain), 16))
	return bytearray(rng.getrandbits(8) for x in xrange(width))


