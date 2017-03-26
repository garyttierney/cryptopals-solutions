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

		double probability = (double) occurrences / (double) len;
		double expected_probability = table[val];

		score += sqrt(expected_probability * probability);
	}

	return score;
}
