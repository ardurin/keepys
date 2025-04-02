#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#ifdef WOLFSSL
#include <wolfssl/options.h>
#include <wolfssl/wolfcrypt/chacha20_poly1305.h>
#include <wolfssl/wolfcrypt/coding.h>
#else
#include <openssl/evp.h>
typedef unsigned char byte;
#endif
#include "path.h"
#include "random.h"
#include "values.h"

#define AUTHENTICATION 16
#define SALT 12
#define SECRET 16

static inline int base64_size(int size) {
	return (4 * size / 3 + 3) & ~3;
}

int create_secret(Path *path, const char *key)
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

	unsigned int encoded_size = base64_size(size) + 1;

#ifdef WOLFSSL
	wc_ChaCha20Poly1305_Encrypt(
		(byte *)key,
		salt,
		NULL, 0,
		secret, SECRET,
		encrypted_secret,
		authentication
	);

	byte encoded[encoded_size];
	Base64_Encode(plain, AUTHENTICATION + SALT + SECRET, encoded, &encoded_size);
#else
	EVP_CIPHER_CTX *cipher = EVP_CIPHER_CTX_new();
	if (cipher == NULL)
		return ERROR_MEMORY;
	if (EVP_EncryptInit(cipher, EVP_chacha20_poly1305(), (byte *)key, salt) < 1)
		return ERROR_MEMORY;
	int unused;
	if (EVP_EncryptUpdate(cipher, encrypted_secret, &unused, secret, SECRET) < 1)
		return ERROR_MEMORY;
	if (EVP_EncryptFinal(cipher, encrypted_secret, &unused) < 1)
		return ERROR_MEMORY;
	EVP_CIPHER_CTX_ctrl(cipher, EVP_CTRL_GCM_GET_TAG, AUTHENTICATION, authentication);
	EVP_CIPHER_CTX_free(cipher);

	byte encoded[encoded_size];
	EVP_EncodeBlock(encoded, plain, AUTHENTICATION + SALT + SECRET);
	encoded[encoded_size - 1] = '\n';
#endif

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

int get_secret(Path *path, const char *key)
{
	FILE *file = fopen(get_file(path), "r");
	if (file == NULL)
		return ERROR_FILE;
	unsigned int size = base64_size(AUTHENTICATION + SALT + SECRET);
	byte encoded[size];
	if (fread(encoded, 1, size, file) < size)
		return ERROR_FILE;
	fclose(file);

#ifdef WOLFSSL
	// https://www.wolfssl.com/documentation/manuals/wolfssl/group__Base__Encoding.html#function-base64_decode
	unsigned int capacity = (size * 3 + 3) / 4;
	byte plain[capacity];
	Base64_Decode(encoded, size, plain, &capacity);
#else
	byte plain[AUTHENTICATION + SALT + SECRET + 1];
	if (EVP_DecodeBlock(plain, encoded, size) < 0)
		return ERROR_MEMORY;
#endif

	byte *authentication = plain;
	byte *salt = authentication + AUTHENTICATION;
	byte *encrypted_secret = salt + SALT;

	byte secret[SECRET];
#ifdef WOLFSSL
	if (wc_ChaCha20Poly1305_Decrypt(
		(byte *)key,
		salt,
		NULL, 0,
		encrypted_secret, SECRET,
		authentication,
		secret		
	) < 0)
		return ERROR_PASSWORD;
#else
	EVP_CIPHER_CTX *cipher = EVP_CIPHER_CTX_new();
	if (cipher == NULL)
		return ERROR_MEMORY;
	if (EVP_DecryptInit(cipher, EVP_chacha20_poly1305(), (byte *)key, salt) < 1)
		return ERROR_MEMORY;
	int unused;
	if (EVP_DecryptUpdate(cipher, secret, &unused, encrypted_secret, SECRET) < 1)
		return ERROR_MEMORY;
	EVP_CIPHER_CTX_ctrl(cipher, EVP_CTRL_GCM_SET_TAG, AUTHENTICATION, authentication);
	if (EVP_DecryptFinal(cipher, encrypted_secret, &unused) < 1)
		return ERROR_PASSWORD;
	EVP_CIPHER_CTX_free(cipher);
#endif

	fwrite(secret, 1, SECRET, stdout);
	if (isatty(1))
		puts("");

	return 0;
}
