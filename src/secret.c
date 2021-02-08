#include <errno.h>
#include <wolfssl/options.h>
#include <wolfssl/wolfcrypt/chacha20_poly1305.h>
#include "error.h"
#include "path.h"
#include "random.h"

#define SECRET_SIZE 16
#define min(a, b) a < b ? a : b

int create_secret(Path *path, const char *password, size_t password_size)
{
	if (initialize() < 0)
		return ERROR_GENERATOR;

	byte secret[SECRET_SIZE];
	if (generate_random_ascii(secret, SECRET_SIZE) < 0)
		return ERROR_GENERATOR;

	byte salt[CHACHA20_POLY1305_AEAD_IV_SIZE];
	if (generate_random(salt, CHACHA20_POLY1305_AEAD_IV_SIZE) < 0)
		return ERROR_GENERATOR;

	byte key[CHACHA20_POLY1305_AEAD_KEYSIZE] = {0};
	size_t limit = min(password_size, CHACHA20_POLY1305_AEAD_KEYSIZE);
	memcpy((void *)key, password, limit);

	byte authentication[CHACHA20_POLY1305_AEAD_AUTHTAG_SIZE];
	byte encrypted_secret[SECRET_SIZE];
	wc_ChaCha20Poly1305_Encrypt(
		(byte *)key,
		salt,
		NULL, 0,
		secret, SECRET_SIZE,
		encrypted_secret,
		authentication
	);

	const char *directory = get_directory(path);
	if (mkdir(directory, 0700) < 0 && errno != EEXIST)
		return ERROR_FILE;
	FILE *file = fopen(get_file(path), "w");
	if (file == NULL)
		return ERROR_FILE;
	fwrite(authentication, 1, CHACHA20_POLY1305_AEAD_AUTHTAG_SIZE, file);
	fwrite(salt, 1, CHACHA_IV_BYTES, file);
	fwrite(encrypted_secret, 1, SECRET_SIZE, file);
	fclose(file);

	return 0;
}

int get_secret(Path *path, const char *password, size_t password_size)
{
	FILE *file = fopen(get_file(path), "r");
	if (file == NULL)
		return ERROR_FILE;
	byte authentication[CHACHA20_POLY1305_AEAD_AUTHTAG_SIZE];
	byte salt[CHACHA20_POLY1305_AEAD_IV_SIZE];
	byte encrypted_secret[SECRET_SIZE];
	fread(authentication, 1, CHACHA20_POLY1305_AEAD_AUTHTAG_SIZE, file);
	fread(salt, 1, CHACHA20_POLY1305_AEAD_IV_SIZE, file);
	fread(encrypted_secret, 1, SECRET_SIZE, file);
	fclose(file);

	byte key[CHACHA20_POLY1305_AEAD_KEYSIZE] = {0};
	size_t limit = min(password_size, CHACHA20_POLY1305_AEAD_KEYSIZE);
	memcpy((void *)key, password, limit);
	
	byte secret[SECRET_SIZE];
	if (wc_ChaCha20Poly1305_Decrypt(
		key,
		salt,
		NULL, 0,
		encrypted_secret, SECRET_SIZE,
		authentication,
		secret		
	) < 0)
		return ERROR_PASSWORD;

	fwrite(secret, 1, SECRET_SIZE, stdout);

	return 0;
}
