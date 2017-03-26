#include <cryptopal-common.h>

#include <errno.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

int cpal_analysis_try_key(const uint8_t *ciphertext, const size_t len,
			  const uint8_t *key, const size_t key_len,
			  struct cpal_analysis_key_score *score,
			  cpal_analysis_score_fn score_fn,
			  cpal_analysis_decrypt_fn decrypt_fn)
{
	int ret = 0;

	uint8_t *key_cpy = NULL;
	uint8_t *decrypted = NULL;
	size_t decrypted_len;

	ret = decrypt_fn(key, key_len, ciphertext, len, &decrypted, &decrypted_len);

	if (ret < 0) {
		goto error;
	}

	key_cpy = calloc(key_len, sizeof *key_cpy);

	if (key_cpy == NULL) {
		ret = -ENOMEM;
		goto error;
	}

	memcpy(key_cpy, key, key_len);

	score->decrypted = decrypted;
	score->decrypted_len = decrypted_len;
	score->key = key_cpy;
	score->key_len = key_len;
	score->score = score_fn(decrypted, decrypted_len);

	return 0;
error:
	free(decrypted);
	free(key_cpy);
	return ret;
}

double cpal_analysis_bhattacharyya_score(const double table[256],
					 const uint8_t *decrypted, const size_t len)
{
	double score = 0.0;

	for (unsigned int val = 0; val < 256; val++) {
		size_t occurrences = 0;
		size_t pos = 0;

		while (pos < len) {
			if (decrypted[pos++] == val) {
				occurrences++;
			}
		}

		double probability = (double)occurrences / (double)len;
		double expected_probability = table[val];

		score += sqrt(expected_probability * probability);
	}

	return score;
}

#define LETTER(table, letter, freq)                                                \
	table[(int)letter] = freq;                                                 \
	table[(int)letter - 32] = freq;

#define CHAR(table, ch, freq) table[(int)ch] = freq;

void cpal_analysis_init_english_probabilities(double table[256])
{
	for (size_t i = 0; i < 256; i++) {
		table[i] = 0.0;
	}

	/*
	 * According to wikipedia, spaces are slightly more frequent than the
	 * letter 'e' in English sentences.  Treat spaces as equivalent to the
	 * letter 'e' for the sake of correctly sorting the decrypted ciphertexts.
	 */
	CHAR(table, ' ', 0.1270)
	LETTER(table, 'a', 0.08167)
	LETTER(table, 'b', 0.01492)
	LETTER(table, 'c', 0.02782)
	LETTER(table, 'd', 0.04253)
	LETTER(table, 'e', 0.1270)
	LETTER(table, 'f', 0.02228)
	LETTER(table, 'g', 0.02015)
	LETTER(table, 'h', 0.06094)
	LETTER(table, 'i', 0.06966)
	LETTER(table, 'j', 0.00153)
	LETTER(table, 'k', 0.00772)
	LETTER(table, 'l', 0.04025)
	LETTER(table, 'm', 0.02406)
	LETTER(table, 'n', 0.06749)
	LETTER(table, 'o', 0.07507)
	LETTER(table, 'p', 0.01929)
	LETTER(table, 'q', 0.00095)
	LETTER(table, 'r', 0.05987)
	LETTER(table, 's', 0.06327)
	LETTER(table, 't', 0.09056)
	LETTER(table, 'u', 0.02758)
	LETTER(table, 'v', 0.00978)
	LETTER(table, 'w', 0.02360)
	LETTER(table, 'x', 0.00150)
	LETTER(table, 'y', 0.01974)
	LETTER(table, 'z', 0.00074)
}
