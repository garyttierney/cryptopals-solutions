#include <cryptopal-common.h>

#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

int cpal_cipher_xor_fixed(const size_t len, const uint8_t *a, const uint8_t *b,
			  uint8_t **output)
{
	uint8_t *output_tmp = calloc(sizeof *output_tmp, len);

	if (output_tmp == NULL) {
		return -ENOMEM;
	}

	for (size_t i = 0; i < len; i++) {
		output_tmp[i] = a[i] ^ b[i];
	}

	*output = output_tmp;
	return 0;
}
