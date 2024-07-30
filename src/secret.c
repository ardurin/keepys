#include <errno.h>
#include <wolfssl/options.h>
#include <wolfssl/wolfcrypt/chacha20_poly1305.h>
#include <wolfssl/wolfcrypt/coding.h>
#include "error.h"
#include "path.h"
#include "random.h"

#define AUTHENTICATION CHACHA20_POLY1305_AEAD_AUTHTAG_SIZE
#define SALT CHACHA20_POLY1305_AEAD_IV_SIZE
#define SECRET 16

#define min(a, b) a < b ? a : b

int create_secret(Path *path, const char *password, size_t password_size)
{
	if (initialize() < 0)
		return ERROR_GENERATOR;

	unsigned int size = AUTHENTICATION + SALT + SECRET;
	byte plain[size];
	byte *authentication = plain;
	byte *salt = authentication + AUTHENTICATION;
	byte *encrypted_secret = salt + SALT;

	byte secret[SECRET];
	if (generate_random_ascii(secret, SECRET) < 0)
		return ERROR_GENERATOR;

	if (generate_random(salt, SALT) < 0)
		return ERROR_GENERATOR;

	byte key[CHACHA20_POLY1305_AEAD_KEYSIZE] = {0};
	size_t limit = min(password_size, CHACHA20_POLY1305_AEAD_KEYSIZE);
	memcpy((void *)key, password, limit);

	wc_ChaCha20Poly1305_Encrypt(
		(byte *)key,
		salt,
		NULL, 0,
		secret, SECRET,
		encrypted_secret,
		authentication
	);

	// base64 data + linebreak
	unsigned int encoded_size = ((4 * size / 3 + 3) & ~3) + 1;
	byte encoded[encoded_size];
	Base64_Encode(plain, AUTHENTICATION + SALT + SECRET, encoded, &encoded_size);

	const char *directory = get_directory(path);
	if (mkdir(directory, 0700) < 0 && errno != EEXIST)
		return ERROR_FILE;
	FILE *file = fopen(get_file(path), "w");
	if (file == NULL)
		return ERROR_FILE;
	fwrite(encoded, 1, encoded_size, file);
	fclose(file);

	return 0;
}

int get_secret(Path *path, const char *password, size_t password_size)
{
	FILE *file = fopen(get_file(path), "r");
	if (file == NULL)
		return ERROR_FILE;
	unsigned int size = (4 * (AUTHENTICATION + SALT + SECRET) / 3 + 3) & ~3;
	byte encoded[size];
	if (fread(encoded, 1, size, file) < size)
		return ERROR_FILE;
	fclose(file);

	// https://www.wolfssl.com/documentation/manuals/wolfssl/group__Base__Encoding.html#function-base64_decode
	unsigned int capacity = (size * 3 + 3) / 4;
	byte plain[capacity];
	Base64_Decode(encoded, size, plain, &capacity);
	byte *authentication = plain;
	byte *salt = authentication + AUTHENTICATION;
	byte *encrypted_secret = salt + SALT;

	byte key[CHACHA20_POLY1305_AEAD_KEYSIZE] = {0};
	size_t limit = min(password_size, CHACHA20_POLY1305_AEAD_KEYSIZE);
	memcpy((void *)key, password, limit);
	
	byte secret[SECRET];
	if (wc_ChaCha20Poly1305_Decrypt(
		key,
		salt,
		NULL, 0,
		encrypted_secret, SECRET,
		authentication,
		secret		
	) < 0)
		return ERROR_PASSWORD;

	fwrite(secret, 1, SECRET, stdout);
	if (isatty(1))
		puts("");

	return 0;
}
