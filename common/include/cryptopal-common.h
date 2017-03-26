#ifndef CRYPTOPAL_COMMON_H
#define CRYPTOPAL_COMMON_H

#include <stdint.h>
#include <stddef.h>

/**
 * Representation of a key score and plaintext result from @cpal_analysis_try_keys.
 */
struct cpal_analysis_key_score {
	double score;
	uint8_t *key;
	size_t key_len;
	uint8_t *decrypted;
	size_t decrypted_len;
};

/**
 * A score function callback which returns an integer value for a given
 * decrypted ciphertext.
 *
 * @decrypted The decrypted ciphertext to score.
 */
typedef double (*cpal_analysis_score_fn)(const uint8_t *decrypted,
					 const size_t len);

/**
 * A decrypt function callback that attempts to decrypt the @ciphertext
 * with the given @key and stores the result in @output.
 *
 * @key The decryption key.
 * @ciphertext The ciphertext to decrypt.
 * @output [out] The location to store the result in.
 *
 * @return 0 if successful, or a negative value indicating failure.
 */
typedef int (*cpal_analysis_decrypt_fn)(const uint8_t *key, size_t key_len,
					const uint8_t *ciphertext, size_t len,
					uint8_t **output, size_t *output_len);

/**
 * Attempt to decrypt @ciphertext with the given @key, using the
 * @decrypt_fn.  If successful, score the ciphertext and store the result
 * in @score.
 *
 * @ciphertext The ciphertext to attempt decrypting.
 * @len The length of the ciphertext, in bytes.
 * @key The key to attempt decryption with.
 * @score [out] The location to store the result in.  Note: the decrypted text
 * and key stored in @score must be free()'d.
 * @score_fn A function callback to score decrypted ciphertexts.
 * @decrypt_fn A function callback that attempts to decrypt a ciphertext
 * with a given key.
 *
 * @return 0 if successful, < 0 otherwise.
 */
int cpal_analysis_try_key(const uint8_t *ciphertext, const size_t len,
			  const uint8_t *key, const size_t key_len,
			  struct cpal_analysis_key_score *score,
			  cpal_analysis_score_fn score_fn,
			  cpal_analysis_decrypt_fn decrypt_fn);

/**
 * Score a decrypted ciphertext using the statistical distance of the character
 * probabilities in the given @table from the actual frequency of characters in
 * @decrypted.
 *
 * @table Table of character probabilities in an average English text.
 * @decrypted The plaintext to score.
 * @decrypted_len The length of the plaintext.
 */
double cpal_analysis_bhattacharyya_score(const double table[256],
					 const uint8_t *decrypted,
					 const size_t decrypted_len);

int cpal_base16_decode(const char *input, size_t input_size, uint8_t **output,
		       size_t *output_size);
int cpal_base16_encode(const uint8_t *input, const size_t input_size, char **output,
		       size_t *output_size);

int cpal_base32_decode(const char *input, const size_t input_size, uint8_t **output,
		       size_t *output_size);
int cpal_base32_encode(const uint8_t *input, const size_t input_size, char **output,
		       size_t *output_size);

int cpal_base32hex_decode(const char *input, const size_t input_size, char **output,
			  size_t *output_size);
int cpal_base32hex_encode(const uint8_t *input, const size_t input_size,
			  char **output, size_t *output_size);

int cpal_base64_decode(const char *input, const size_t input_size, uint8_t **output,
		       size_t *output_size);
int cpal_base64_encode(const uint8_t *input, const size_t input_size, char **output,
		       size_t *output_size);

int cpal_base64safe_decode(const char *input, const size_t input_size,
			   uint8_t **output, size_t *output_size);
int cpal_base64safe_encode(const uint8_t *input, const size_t input_size,
			   char **output, size_t *output_size);

int cpal_cipher_xor_fixed(const size_t len, const uint8_t *a, const uint8_t *b,
			  uint8_t **output);

int cpal_cipher_xor_bytewise(const uint8_t *input, const size_t len,
			     const uint8_t key, uint8_t **output);

int cpal_cipher_xor_repeating(const uint8_t *input, const size_t len,
			      const uint8_t *key, const size_t key_len,
			      uint8_t **output);

/**
 * Print a buffer to STDOUT and replace any non-printable characters with
 * their equivalent escape codes.
 *
 * @buf The buffer to print.
 * @buf_len The length of the buffer.
 */
void cpal_util_printbuf(const uint8_t *buf, const size_t buf_len);

#endif
