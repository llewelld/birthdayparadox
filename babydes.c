// Uses simplerandom https://github.com/cmcqueen/simplerandom
// Uses libb64 https://sourceforge.net/projects/libb64/
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <simplerandom.h>
#include <b64/cencode.h>

// gcc babydes.c -I/usr/local/include/simplerandom-0.12.1 -L/usr/local/lib -lsimplerandom-0.12.1 -static -o babydes
// gcc babydes.c -I/usr/local/include/simplerandom-0.12.1 -L/usr/local/lib -lsimplerandom-0.12.1 -lb64 -static -o babydes

typedef struct {
	uint32_t * key;
	unsigned int keylen;
} KeyStructure;

typedef void (*Prf)(unsigned int width, KeyStructure * key, unsigned short roundn, unsigned char * bufferin, unsigned char * bufferout);

KeyStructure * create_key(unsigned char * key, unsigned int keylen);
void destroy_key(KeyStructure * key);

// Non cryptographycally secure PRF
void random(unsigned int width, KeyStructure * key, unsigned short roundn, unsigned char * bufferin, unsigned char * bufferout) {
	SimpleRandomKISS_t * rng_kiss;
	uint32_t random;
	unsigned int pos;
	unsigned int rngnum;
	unsigned int count;
	unsigned int shift;

	// Create as many random number sequences as we can get away with
	rngnum = ((key->keylen + 3) / 4);
	rng_kiss = malloc(sizeof(SimpleRandomKISS_t) * rngnum);
	for (count = 0; count < rngnum; count++) {
		shift = count + roundn;
		simplerandom_kiss_seed(&rng_kiss[count], key->key[shift % key->keylen], key->key[(shift + 1) % key->keylen], key->key[(shift + 2) % key->keylen], key->key[(shift + 3) % key->keylen]);
	}

	// Select output bytes as they come out of thee random number generators
	for (pos = 0; pos < width; pos++) {
		if (pos % 4 == 0) {
			random = 0;
			for (count = 0; count < rngnum; count++) {
				random += simplerandom_kiss_next(&rng_kiss[count]);
			}
		}
		// Ensure output depends on input
		simplerandom_kiss_discard(&rng_kiss[pos % rngnum], bufferin[pos]);
		random += simplerandom_kiss_next(&rng_kiss[pos % rngnum]);
		bufferout[pos] = ((unsigned char *)& random)[pos % 4];
	}
	
	// Free up the RNG structures (can we keep them? Unfortunately we can't)
	free(rng_kiss);
}

// BabyDES encrypt
// Variable length Feistel cipher
// Only as secure as the PRF you give it
void babydes_enc(Prf prf, unsigned int width, KeyStructure * key, unsigned char * bufferin, unsigned char * bufferout) {
	unsigned int halfwidth;
	unsigned char * left;
	unsigned char * right;
	unsigned char * bitstream;
	int pos;
	base64_encodestate state_in;

	halfwidth = width / 2;
	left = bufferout;
	right = bufferout + halfwidth;
	memcpy(left, bufferin, halfwidth);
	memcpy(right, bufferin + halfwidth, halfwidth);
	bitstream = malloc(halfwidth);

	// Round 1
	prf(halfwidth, key, 0, right, bitstream);
	for (pos = 0; pos < halfwidth; pos++) {
		left[pos] ^= bitstream[pos];
	}

	// Round 2
	prf(halfwidth, key, 1, left, bitstream);
	for (pos = 0; pos < halfwidth; pos++) {
		right[pos] ^= bitstream[pos];
	}

	// Round 3
	prf(halfwidth, key, 2, right, bitstream);
	for (pos = 0; pos < halfwidth; pos++) {
		left[pos] ^= bitstream[pos];
	}
	
	free(bitstream);
}

// BabyDES decrypt
// Variable length Feistel cipher
// Only as secure as the PRF you give it
void babydes_dec(Prf prf, unsigned int width, KeyStructure * key, unsigned char * bufferin, unsigned char * bufferout) {
	unsigned int halfwidth;
	unsigned char * left;
	unsigned char * right;
	unsigned char * bitstream;
	int pos;

	halfwidth = width / 2;
	left = bufferout;
	right = bufferout + halfwidth;
	memcpy(left, bufferin, halfwidth);
	memcpy(right, bufferin + halfwidth, halfwidth);
	bitstream = malloc(halfwidth);

	// Round 1
	prf(halfwidth, key, 2, right, bitstream);
	for (pos = 0; pos < halfwidth; pos++) {
		left[pos] ^= bitstream[pos];
	}

	// Round 2
	prf(halfwidth, key, 1, left, bitstream);
	for (pos = 0; pos < halfwidth; pos++) {
		right[pos] ^= bitstream[pos];
	}

	// Round 3
	prf(halfwidth, key, 0, right, bitstream);
	for (pos = 0; pos < halfwidth; pos++) {
		left[pos] ^= bitstream[pos];
	}
	
	free(bitstream);
}

// Create a key from a string
KeyStructure * create_key(unsigned char * key, unsigned int keylen) {
	int intlen;

	KeyStructure * keydata;
	keydata = calloc(sizeof(KeyStructure), 1);

	intlen = ((keylen + 3) / 4);

	keydata->key = calloc(sizeof(uint32_t), intlen);
	memcpy(keydata->key, key, keylen);

	keydata->keylen = intlen;
	
	return keydata;
}

// Free up the memory associated with the key
void destroy_key(KeyStructure * key) {
	if (key != NULL) {
		if (key->key != NULL) {
			free(key->key);
		}
		free(key);
	}
}

void main () {
	unsigned char buffer[21];
	char * keytext = "abcdabcdabcdabcda";
	base64_encodestate state_in;
	char b64encoded[100];
	KeyStructure * key;

	key = create_key(keytext, strlen(keytext));

	strncpy(buffer, "abcdefghijklmnopqrst", 20);
	buffer[20] = 0;
	printf ("Txt: %s\n", buffer);

	babydes_enc(random, 20, key, buffer, buffer);

	base64_init_encodestate(& state_in);
	int count = base64_encode_block(buffer, 20, b64encoded, & state_in);
	base64_encode_blockend(b64encoded + count, & state_in);
	printf ("Enc: %s", b64encoded);

	babydes_dec(random, 20, key, buffer, buffer);
	printf ("Dec: %s\n", buffer);

	destroy_key(key);
}

