// Uses libb64 https://sourceforge.net/projects/libb64/
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <b64/cencode.h>
#include <linux/random.h>
#include <sys/syscall.h>
#include <unistd.h>

#include "babydes.h"

// gcc babydes.c -I/usr/local/include/simplerandom-0.12.1 -L/usr/local/lib -lsimplerandom-0.12.1 -static -o babydes
// cc babydes.c yeahbaby.c -I/usr/local/include/simplerandom-0.12.1 -L/usr/local/lib -lsimplerandom-0.12.1 -lb64 -static -o yeahbaby

#define CHECK_TEXT_LEN (128)
#define CHECK_COUNT (1000)

// Perform some tests to compare against the Python implementation
int main () {
	// Key to encrypt/decrypt with
	char * keytext = "abcdabcdabcdabcda";
	base64_encodestate state_in;
	char b64encoded[256];
	char bufferout[CHECK_TEXT_LEN];
	KeyStructure * key;
	char buffer[CHECK_TEXT_LEN];
	int count;
	int same;
	int chars;

	key = create_key(keytext, strlen(keytext));

	strncpy(buffer, "abcdefghijklmnopqrst", 20);
	buffer[20] = 0;
	printf ("Txt: %s\n", buffer);

	// Encode the text
	babydes_enc(random, 20, key, buffer, buffer);

	// Print the ciphertext in base64 format
	base64_init_encodestate(& state_in);
	chars = base64_encode_block(buffer, 20, b64encoded, & state_in);
	base64_encode_blockend(b64encoded + chars, & state_in);
	printf ("Enc: %s", b64encoded);

	// Decrypt the encrypted text
	babydes_dec(random, 20, key, buffer, buffer);
	printf ("Dec: %s\n", buffer);

	// Timing
	// Perform 1000 encryptions and decryptions
	for (count = 0; count < CHECK_COUNT; count++) {
		// Generate random buffer
		syscall(SYS_getrandom, buffer, CHECK_TEXT_LEN, 0);

		// Encrypt then decrypt the result
		babydes_enc(random, CHECK_TEXT_LEN, key, buffer, bufferout);
		babydes_dec(random, CHECK_TEXT_LEN, key, bufferout, bufferout);

		// Check the input and decrypted output are the same
		same = memcmp(buffer, bufferout, CHECK_TEXT_LEN);
		if (same != 0) {
			printf("Txt and Dec do not match\n");
		}
	}

	// Clean up
	destroy_key(key);
}

