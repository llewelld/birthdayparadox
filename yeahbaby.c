// Uses libb64 https://sourceforge.net/projects/libb64/
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <b64/cencode.h>

#include "babydes.h"

// gcc babydes.c -I/usr/local/include/simplerandom-0.12.1 -L/usr/local/lib -lsimplerandom-0.12.1 -static -o babydes
// cc babydes.c yeahbaby.c -I/usr/local/include/simplerandom-0.12.1 -L/usr/local/lib -lsimplerandom-0.12.1 -lb64 -static -o yeahbaby

// Perform some tests to compare against the Python implementation
int main () {
	// Key to encrypt/decrypt with
	char * keytext = "abcdabcdabcdabcda";
	base64_encodestate state_in;
	char b64encoded[100];
	KeyStructure * key;
	char buffer[21];

	key = create_key(keytext, strlen(keytext));

	strncpy(buffer, "abcdefghijklmnopqrst", 20);
	buffer[20] = 0;
	printf ("Txt: %s\n", buffer);

	// Encode the text
	babydes_enc(random, 20, key, buffer, buffer);

	// Print the ciphertext in base64 format
	base64_init_encodestate(& state_in);
	int count = base64_encode_block(buffer, 20, b64encoded, & state_in);
	base64_encode_blockend(b64encoded + count, & state_in);
	printf ("Enc: %s", b64encoded);

	// Decrypt the encrypted text
	babydes_dec(random, 20, key, buffer, buffer);
	printf ("Dec: %s\n", buffer);

	// Clean up
	destroy_key(key);
}

