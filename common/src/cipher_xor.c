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

int cpal_cipher_xor_bytewise(const uint8_t *input, const size_t len,
			     const uint8_t key, uint8_t **output)
{
	uint8_t *output_tmp = calloc(sizeof *output_tmp, len);

	if (output_tmp == NULL) {
		return -ENOMEM;
	}

	for (size_t i = 0; i < len; i++) {
		output_tmp[i] = input[i] ^ key;
	}

	*output = output_tmp;
	return 0;
}

int cpal_cipher_xor_repeating(const uint8_t *input, size_t len, const uint8_t *key,
			      size_t key_len, uint8_t **output)
{
	uint8_t *output_tmp = calloc(sizeof *output_tmp, len);

	if (output_tmp == NULL) {
		return -ENOMEM;
	}

	for (size_t i = 0; i < len; i++) {
		output_tmp[i] = input[i] ^ key[i % key_len];
	}

	*output = output_tmp;
	return 0;
}
