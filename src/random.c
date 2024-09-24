#ifdef WOLFSSL
#include <wolfssl/options.h>
#include <wolfssl/wolfcrypt/random.h>
#else
#include <openssl/rand.h>
typedef unsigned char byte;
#endif

#ifdef WOLFSSL
static WC_RNG generator;
#endif

int generate_random(byte *data, size_t size)
{
#ifdef WOLFSSL
	if (wc_RNG_GenerateBlock(&generator, data, size) < 0)
		return -1;
#else
	if (RAND_bytes(data, (int)size) < 1)
		return -1;
#endif
	return 0;
}

int generate_random_ascii(byte * data, size_t size)
{
	size_t count = 0;
	while (count < size) {
		byte random[48] = {0};
#ifdef WOLFSSL
		if (wc_RNG_GenerateBlock(&generator, random, 48) < 0)
			return -1;
#else
		if (RAND_bytes(random, 48) < 1)
			return -1;
#endif
		for (size_t i = 0; i < 48; i ++) {
			if (random[i] < 33 || random[i] > 126)
				continue;
			data[count] = random[i];
			count ++;
			if (count == size)
				break;
		}
	}
	return 0;
}

int initialize()
{
#ifdef WOLFSSL
	if (wc_InitRng(&generator) < 0)
		return -1;
#endif
	return 0;
}
