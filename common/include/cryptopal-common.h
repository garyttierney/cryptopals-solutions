#ifndef CRYPTOPAL_COMMON_H
#define CRYPTOPAL_COMMON_H

#include <stdint.h>
#include <stddef.h>

size_t cpal_base16_decode(const char *input, size_t input_size, uint8_t **output);
size_t cpal_base16_encode(const uint8_t *input, const size_t input_size,
			  char **output);

size_t cpal_base32_decode(const char *input, const size_t input_size,
			  uint8_t **output);
size_t cpal_base32_encode(const uint8_t *input, const size_t input_size,
			  char **output);

size_t cpal_base32hex_decode(const char *input, const size_t input_size,
			     char **output);
size_t cpal_base32hex_encode(const uint8_t *input, const size_t input_size,
			     char **output);

size_t cpal_base64_decode(const char *input, const size_t input_size,
			  uint8_t **output);
size_t cpal_base64_encode(const uint8_t *input, const size_t input_size,
			  char **output);

size_t cpal_base64safe_decode(const char *input, const size_t input_size,
			      uint8_t **outpuT);
size_t cpal_base64safe_encode(const uint8_t *input, const size_t input_size,
			      char **output);

int cpal_cipher_xor_fixed(const size_t len, const uint8_t *a, const uint8_t *b,
			  uint8_t **output);

#endif
