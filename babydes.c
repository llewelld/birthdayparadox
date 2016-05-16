// Uses https://github.com/cmcqueen/simplerandom
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <simplerandom.h>

// gcc babydes.c -I/usr/local/include/simplerandom-0.12.1 -L/usr/local/lib -lsimplerandom-0.12.1 -static -o babydes

typedef void (*Prf)(unsigned int width, char * key, int keylen, unsigned short roundn, unsigned char * bufferin, unsigned char * bufferout);

void random(unsigned int width, char * key, int keylen, unsigned short roundn, unsigned char * bufferin, unsigned char * bufferout) {
/*
	static SimpleRandomKISS_t rng_kiss;
	uint32_t seed_array[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
	simplerandom_kiss_seed_array(&rng_kiss, seed_array, 8, true);
	simplerandom_kiss_discard(&rng_kiss, 1000000000000uLL);
	uint32_t rng_values_array[8];
	int i;

	for (i = 0; i < 8; ++i) {
		rng_values_array[i] = simplerandom_kiss_next(&rng_kiss);
		printf ("Output: %u\n", rng_values_array[i]);
	}
*/
	int pos;
	for (pos = 0; pos < width; pos++) {
		bufferout[pos] = (roundn + pos + key[pos % keylen] + bufferin[pos]) & 0xff;
	}
}

void babydes_enc(Prf prf, unsigned int width, char * key, unsigned int keylen, unsigned char * bufferin, unsigned char * bufferout) {
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
	prf(halfwidth, key, keylen, 0, right, bitstream);
	for (pos = 0; pos < halfwidth; pos++) {
		left[pos] ^= bitstream[pos];
	}

	// Round 2
	prf(halfwidth, key, keylen, 1, left, bitstream);
	for (pos = 0; pos < halfwidth; pos++) {
		right[pos] ^= bitstream[pos];
	}

	// Round 3
	prf(halfwidth, key, keylen, 2, right, bitstream);
	for (pos = 0; pos < halfwidth; pos++) {
		left[pos] ^= bitstream[pos];
	}
	
	free(bitstream);
}

void babydes_dec(Prf prf, unsigned int width, char * key, unsigned int keylen, unsigned char * bufferin, unsigned char * bufferout) {
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
	prf(halfwidth, key, keylen, 2, right, bitstream);
	for (pos = 0; pos < halfwidth; pos++) {
		left[pos] ^= bitstream[pos];
	}

	// Round 2
	prf(halfwidth, key, keylen, 1, left, bitstream);
	for (pos = 0; pos < halfwidth; pos++) {
		right[pos] ^= bitstream[pos];
	}

	// Round 3
	prf(halfwidth, key, keylen, 0, right, bitstream);
	for (pos = 0; pos < halfwidth; pos++) {
		left[pos] ^= bitstream[pos];
	}
	
	free(bitstream);
}

void main () {
	unsigned char buffer[21];
	unsigned char * key = "hello";
	unsigned int keylen;

	keylen = strlen(key);
	strncpy(buffer, "abcdefghijklmnopqrst", 20);
	buffer[20] = 0;
	printf ("In: %s\n", buffer);

	babydes_enc(random, 20, key, keylen, buffer, buffer);
	printf ("Out: %s\n", buffer);

	babydes_dec(random, 20, key, keylen, buffer, buffer);
	printf ("Final: %s\n", buffer);
}

