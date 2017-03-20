#ifndef CRYPTOPAL_ENCODING_INTERNAL_H
#define CRYPTOPAL_ENCODING_INTERNAL_H

#include <stdint.h>
#include <stddef.h>

#define GROUP_MASK(x) ((1 << x) - 1)

#define INITIALIZE_DECODE_TABLE(alphabet, decode_table, case_insensitive)          \
	do {                                                                       \
		static int decode_table_initialized = 0;                           \
		if (!decode_table_initialized) {                                   \
			rfc4648_build_decode_table(alphabet, decode_table,         \
						   case_insensitive);              \
			decode_table_initialized = 1;                              \
		}                                                                  \
	} while (0);

/**
 * Initialize a decoding table used to map bytes to their offsets in the encoding
 * scheme alphabet.
 *
 * @alphabet The alphabet to initialize the decoding table with.
 * @decode_table The decoding table to initialize.
 * @case_insensitive A flag indiciating whether the encoding scheme the decode
 * table is for is case-insensitive.  If true, then treat uppercase and lowercase
 * characters as having the same value.
 */
static void rfc4648_build_decode_table(const char *alphabet, char *decode_table,
				       int case_insensitive);

/**
 * Decode an @input_size string from @input encoded with the given RFC 4648 encoding
 * scheme.
 *
 * @input The string to be decoded.
 * @input_size The length of the string to decode.
 * @output [out] The object to store the decoded data in.
 * @input_group_bits The number of bits in an input unit.
 * @output_group_bits The number of bits which represent a single encoded
 * character from the alphabet.
 * @decode_table A table of 255 elements used to map a character value to an
 * alphabet offset.
 *
 * @return A positive result indiciating the length of the decoded data, or a
 * negated error code.
 */
static int rfc4648_decode(const char *input, const size_t input_size,
			  uint8_t **output, size_t *output_size,
			  const uint8_t input_group_bits,
			  const uint8_t output_group_bits,
			  const char *decode_table);

/**
 * Calculate the expected size for the decoded data with the given @input_size
 * and RFC 4648 encoding scheme @input_group_bits and @output_group_bits.
 *
 * @input_size The number of bytes in the encoded data.
 * @input_group_bits The number of bits in an input group.
 * @output_group_bits The number of bits used to represent an index in the alphabet.
 *
 * @return The expected size of the decoded data.
 */
static size_t rfc4648_decoded_size(const size_t input_size,
				   const uint8_t input_group_bits,
				   const uint8_t output_group_bits);

/**
 * Encode an @input_size range of bytes from @input using an RFC 4648 encoding
 * scheme given input/output group sizes and an alphabet.  After encoding, store the
 * result as a NULL-terminated string in @output. For every @input_group_bits
 * bits of input, there will be (@input_group_bits / @output_group_bits)
 * characters of output.
 *
 * @input The data to be encoded.
 * @input_size The number of bytes to be encoded from @input.
 * @output [out] The object to store the encoded result in.
 * @input_group_bits The number of bits in an input unit.
 * @output_group_bits The number of bits which represent a single encoded
 * character from the alphabet.
 * @alphabet A string representing the alphabet of the encoding scheme.  Must
 * contain exactly 2^output_group_bits elements.
 *
 * @return A positive result indiciating the length of the encoded data, or a
 * negated error code.
 */
static int rfc4648_encode(const uint8_t *input, const size_t input_size,
			  char **output, size_t *output_size,
			  const uint8_t input_group_bits,
			  const uint8_t output_group_bits, const char *alphabet);

/**
 * Calculate the expected size for an encoded buffer of with the given
 * @input_group_bits and @output_group_bits.  Use integer division to avoid floating
 * point operations.
 *
 * @input_size The number of bytes of input.
 * @input_group_bits The number of bits in an input group.
 * @output_group_bits The number of bits used to represent an index in the alphabet.
 */
static size_t rfc4648_encoded_size(const size_t input_size,
				   const uint8_t input_group_bits,
				   const uint8_t output_group_bits);

/**
 * Check if the @alphabet contains the correct number of elements
 * to be a valid alphabet for the number of bits in the given @output_group_bits.
 *
 * @alphabet The encoding scheme alphabet.
 * @output_group_bits The number of bits used to represent an index in the alphabet.
 */
static int rfc4648_valid_alphabet(const char *alphabet,
				  const uint8_t output_group_bits);

#endif
