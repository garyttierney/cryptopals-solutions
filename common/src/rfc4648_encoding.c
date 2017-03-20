/*
 * Implementation of RFC 4648 encoding (aka: base16, base32, and base64 encoding).
 *
 * See: https://tools.ietf.org/rfc/rfc4648.txt
 */

#include "rfc4648_encoding_internal.h"

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

/**
 * The character used to represent padding in encoding schemes
 * which have input groups larger than 8 bits.
 */
static const char RFC4648_PADDING = '=';

/**
 * base16 uses an index size of 4 bits, to produce 2 output
 * characters for every 8 bits of input.
 */
static uint8_t BASE16_OUTPUT_GROUP_BITS = 4;
static uint8_t BASE16_INPUT_GROUP_BITS = 8;

static const char *BASE16_ALPHABET = "0123456789ABCDEF";
static char BASE16_DECODE_TABLE[256];

/**
 * base32 uses an index size of 5 bits, to produce 8 output
 * characters for every 40 bits of input.
 */
static uint8_t BASE32_OUTPUT_GROUP_BITS = 5;
static uint8_t BASE32_INPUT_GROUP_BITS = 40;

static const char *BASE32_ALPHABET = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";
static char BASE32_DECODE_TABLE[256];

/**
 * A separate base32 encoding scheme, using the "extended hex" alphabet.
 */
static const char *BASE32HEX_ALPHABET = "0123456789ABCDEFGHIJKLMNOPQRSTUV";
static char BASE32HEX_DECODE_TABLE[256];

/**
 * base64 uses an index size of 6 bits, to produce 4 output
 * characters for every 24 bits of input.
 */
static uint8_t BASE64_OUTPUT_GROUP_BITS = 6;
static uint8_t BASE64_INPUT_GROUP_BITS = 24;

static const char *BASE64_ALPHABET =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static char BASE64_DECODE_TABLE[256];

/**
 * A separate base64 encoding scheme, using a filename and URL safe alphabet.
 */
static const char *BASE64SAFE_ALPHABET =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";
static char BASE64SAFE_DECODE_TABLE[256];

static void rfc4648_build_decode_table(const char *alphabet, char *decode_table,
				       int case_insensitive)
{
	for (size_t offset = 0; offset < 256; offset++) {
		decode_table[offset] = -1;
	}

	size_t alphabet_len = strnlen(alphabet, 256);

	for (size_t offset = 0; offset < alphabet_len; offset++) {
		uint8_t val = (uint8_t)alphabet[offset];

		if (isalpha(val) && case_insensitive) {
			uint8_t upperval = toupper(val);
			uint8_t lowerval = tolower(val);

			decode_table[upperval] = offset;
			decode_table[lowerval] = offset;
		} else {
			decode_table[val] = offset;
		}
	}
}

static size_t rfc4648_decode(const char *input, const size_t input_size,
			     uint8_t **output, uint8_t input_group_bits,
			     uint8_t output_group_bits, const char *decode_table)
{
	assert(input_group_bits <= 64);
	assert(input_group_bits % 8 == 0);
	assert(output_group_bits < input_group_bits);

	int output_groups = input_group_bits / output_group_bits;

	if (input == NULL || (input_size % output_groups) != 0) {
		return -EINVAL;
	}

	size_t output_size =
	    rfc4648_decoded_size(input_size, input_group_bits, output_group_bits);
	size_t padding_pos = input_size - 1;

	while (input[padding_pos--] == RFC4648_PADDING) {
		output_size--;
	}

	uint8_t *output_tmp = calloc(sizeof *output_tmp, output_size);

	if (output_tmp == NULL) {
		return -ENOMEM;
	}

	size_t input_pos = 0;
	uint8_t *output_buffer = output_tmp;

	while (input_pos < input_size) {
		int input_group_offset = input_group_bits;
		int output_group_offset = (output_groups - 1) * output_group_bits;

		uint64_t encoded = 0;

		while (output_group_offset >= 0) {
			char input_value = input[input_pos++];
			uint8_t value = input_value != RFC4648_PADDING
					    ? decode_table[(int)input_value]
					    : 0;

			encoded += value << output_group_offset;
			output_group_offset -= output_group_bits;
		}

		while (input_group_offset > 0) {
			input_group_offset -= 8;
			*output_buffer++ = (encoded >> input_group_offset);
		}
	}

	*output = output_tmp;
	return output_size;
}

static size_t rfc4648_decoded_size(const size_t input_size,
				   uint8_t input_group_bits,
				   uint8_t output_group_bits)
{
	uint8_t input_group_bytes = input_group_bits / 8;
	uint8_t output_groups = input_group_bits / output_group_bits;

	return input_size / output_groups * input_group_bytes;
}

static size_t rfc4648_encode(const uint8_t *input, const size_t input_size,
			     char **output, uint8_t input_group_bits,
			     uint8_t output_group_bits, const char *alphabet)
{
	// Only allow input groups that can be stored
	// in a 64 bit int
	assert(input_group_bits <= 64);
	assert(input_group_bits % 8 == 0);
	assert(output_group_bits < input_group_bits);
	assert(rfc4648_valid_alphabet(alphabet, output_group_bits));

	if (input == NULL) {
		return -EINVAL;
	}

	size_t output_size =
	    rfc4648_encoded_size(input_size, input_group_bits, output_group_bits);
	char *output_tmp = calloc(sizeof *output_tmp, output_size);

	if (output_tmp == NULL) {
		return -ENOMEM;
	}

	size_t input_pos = 0;
	int output_groups = input_group_bits / output_group_bits;
	char *output_buffer = output_tmp;

	while (input_pos < input_size) {
		int input_group_offset = input_group_bits;
		int output_group_offset = (output_groups - 1) * output_group_bits;
		;
		int padded_output_groups = -1;

		uint64_t encoded = 0;

		// Extract the bits of the input groups into a 64 bit integer
		while (input_group_offset > 0) {
			if (input_pos >= input_size) {
				padded_output_groups =
				    input_group_offset / output_group_bits;
				break;
			}

			uint8_t value = input[input_pos++];

			input_group_offset -= 8;
			encoded += value << input_group_offset;
		}

		while (output_group_offset >= 0) {
			unsigned int alphabet_offset =
			    (encoded >> output_group_offset) &
			    GROUP_MASK(output_group_bits);

			if (output_group_offset / output_group_bits <
			    padded_output_groups) {
				*output_buffer++ = RFC4648_PADDING;
			} else {
				*output_buffer++ = alphabet[alphabet_offset];
			}

			output_group_offset -= output_group_bits;
		}
	}

	*output_buffer++ = '\0';
	*output = output_tmp;
	return output_size;
}

static size_t rfc4648_encoded_size(const size_t input_size,
				   uint8_t input_group_bits,
				   uint8_t output_group_bits)
{
	uint8_t output_groups = input_group_bits / output_group_bits;
	uint8_t input_group_bytes = input_group_bits / 8;
	uint8_t input_groups = (input_size - 1) / input_group_bytes;

	return input_groups * output_groups + output_groups + 1;
}

static int rfc4648_valid_alphabet(const char *alphabet, uint8_t output_group_bits)
{
	size_t expected_alphabet_len = 1 << output_group_bits;
	size_t alphabet_len = strlen(alphabet);

	return alphabet_len == expected_alphabet_len;
}

size_t cpal_base16_encode(const uint8_t *input, const size_t input_size,
			  char **output)
{
	return rfc4648_encode(input, input_size, output, BASE16_INPUT_GROUP_BITS,
			      BASE16_OUTPUT_GROUP_BITS, BASE16_ALPHABET);
}

size_t cpal_base16_decode(const char *input, const size_t input_size,
			  uint8_t **output)
{
	INITIALIZE_DECODE_TABLE(BASE16_ALPHABET, BASE16_DECODE_TABLE, 1);

	return rfc4648_decode(input, input_size, output, BASE16_INPUT_GROUP_BITS,
			      BASE16_OUTPUT_GROUP_BITS, BASE16_DECODE_TABLE);
}

size_t cpal_base32_encode(const uint8_t *input, const size_t input_size,
			  char **output)
{
	return rfc4648_encode(input, input_size, output, BASE32_INPUT_GROUP_BITS,
			      BASE32_OUTPUT_GROUP_BITS, BASE32_ALPHABET);
}

size_t cpal_base32_decode(const char *input, const size_t input_size,
			  uint8_t **output)
{
	INITIALIZE_DECODE_TABLE(BASE32_ALPHABET, BASE32_DECODE_TABLE, 0);

	return rfc4648_decode(input, input_size, output, BASE32_INPUT_GROUP_BITS,
			      BASE32_OUTPUT_GROUP_BITS, BASE32_DECODE_TABLE);
}

size_t cpal_base32hex_encode(const uint8_t *input, const size_t input_size,
			     char **output)
{
	return rfc4648_encode(input, input_size, output, BASE32_INPUT_GROUP_BITS,
			      BASE32_OUTPUT_GROUP_BITS, BASE32HEX_ALPHABET);
}

size_t cpal_base32hex_decode(const char *input, const size_t input_size,
			     uint8_t **output)
{
	INITIALIZE_DECODE_TABLE(BASE32HEX_ALPHABET, BASE32HEX_DECODE_TABLE, 1);

	return rfc4648_decode(input, input_size, output, BASE32_INPUT_GROUP_BITS,
			      BASE32_OUTPUT_GROUP_BITS, BASE32HEX_DECODE_TABLE);
}

size_t cpal_base64_encode(const uint8_t *input, const size_t input_size,
			  char **output)
{
	return rfc4648_encode(input, input_size, output, BASE64_INPUT_GROUP_BITS,
			      BASE64_OUTPUT_GROUP_BITS, BASE64_ALPHABET);
}

size_t cpal_base64_decode(const char *input, const size_t input_size,
			  uint8_t **output)
{
	INITIALIZE_DECODE_TABLE(BASE64_ALPHABET, BASE64_DECODE_TABLE, 0);

	return rfc4648_decode(input, input_size, output, BASE64_INPUT_GROUP_BITS,
			      BASE64_OUTPUT_GROUP_BITS, BASE64_DECODE_TABLE);
}

size_t cpal_base64safe_encode(const uint8_t *input, const size_t input_size,
			      char **output)
{
	return rfc4648_encode(input, input_size, output, BASE64_INPUT_GROUP_BITS,
			      BASE64_OUTPUT_GROUP_BITS, BASE64SAFE_ALPHABET);
}

size_t cpal_base64safe_decode(const char *input, const size_t input_size,
			      uint8_t **output)
{
	INITIALIZE_DECODE_TABLE(BASE64SAFE_ALPHABET, BASE64SAFE_DECODE_TABLE, 0);

	return rfc4648_decode(input, input_size, output, BASE64_INPUT_GROUP_BITS,
			      BASE64_OUTPUT_GROUP_BITS, BASE64SAFE_DECODE_TABLE);
}
