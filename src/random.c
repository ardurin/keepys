#include <wolfssl/options.h>
#include <wolfssl/wolfcrypt/random.h>

static WC_RNG generator;

int generate_random(byte *data, size_t size)
{
	if (wc_RNG_GenerateBlock(&generator, data, size) < 0)
		return -1;
	return 0;
}

int generate_random_ascii(byte * data, size_t size)
{
	size_t count = 0;
	while (count < size) {
		byte random[48] = {0};
		if (wc_RNG_GenerateBlock(&generator, random, 48) < 0)
			return -1;
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
	if (wc_InitRng(&generator) < 0)
		return -1;
	return 0;
}
