#include <stdio.h>
#include <string.h>

// Opaque structure used for holding keys
// Usee create_key function to populate from a string
typedef struct _KeyStructure KeyStructure;

// Function prototype for pseuso random functions to pass into the Feistel structure
// resulting in a pseudo-random permutation
typedef void (*Prf)(unsigned int width, KeyStructure const * key, unsigned short roundn, char const * bufferin, char * bufferout);

// Create and destroy key structures
KeyStructure * create_key(char const * key, unsigned int keylen);
void destroy_key(KeyStructure * key);

// Non cryptographycally secure PRF
void random(unsigned int width, KeyStructure const * key, unsigned short roundn, char const * bufferin, char * bufferout);

// BabyDES encrypt
// Variable length Feistel cipher
// Only as secure as the PRF you give it
void babydes_enc(Prf prf, unsigned int width, KeyStructure const * key, char const * bufferin, char * bufferout);

// BabyDES decrypt
// Variable length Feistel cipher
// Only as secure as the PRF you give it
void babydes_dec(Prf prf, unsigned int width, KeyStructure const * key, char const * bufferin, char * bufferout);


