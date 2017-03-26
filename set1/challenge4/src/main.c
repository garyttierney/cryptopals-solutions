#include <cryptopal-common.h>

#include "challenge4_data.h"

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static double ENGLISH_FREQ_TABLE[256];

//@todo - make these functions available from libcryptopal-common

static double english_freq_bhattacharyya_score(const uint8_t *ciphertext,
					       const size_t len)
{
	return cpal_analysis_bhattacharyya_score(ENGLISH_FREQ_TABLE, ciphertext,
						 len);
}

static int xor_decrypt(const uint8_t *key, const size_t key_len,
		       const uint8_t *ciphertext, size_t len, uint8_t **output,
		       size_t *output_len)
{
	if (key_len != 1) {
		return -1;
	}

	int res = cpal_cipher_xor_bytewise(ciphertext, len, *key, output);
	if (res == 0) {
		*output_len = len;
	}

	return res;
}

static int decode_and_score(const char *input, uint8_t key,
			    struct cpal_analysis_key_score *score)
{
	uint8_t *decoded = NULL;
	size_t decoded_len;

	int ret = cpal_base16_decode(input, strlen(input), &decoded, &decoded_len);
	if (ret < 0) {
		return ret;
	}

	int res =
	    cpal_analysis_try_key(decoded, decoded_len, &key, 1, score,
				  english_freq_bhattacharyya_score, xor_decrypt);

	free(decoded);
	return res;
}

int main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;

	int ret = 1;

	static const char *expected_best_plaintext =
	    "Now that the party is jumping";

	cpal_analysis_init_english_probabilities(ENGLISH_FREQ_TABLE);

	struct cpal_analysis_key_score **scores =
	    calloc(CHALLENGE4_NUM_STRINGS, sizeof *scores);
	if (scores == NULL) {
		goto exit;
	}

	int best_idx = 0;
	unsigned int best_key = 0;
	double best_score = 0.0;

	for (size_t idx = 0; idx < CHALLENGE4_NUM_STRINGS; idx++) {
		struct cpal_analysis_key_score *key_scores =
		    calloc(256, sizeof *key_scores);

		for (unsigned int key = 0; key < 256; key++) {
			if (decode_and_score(CHALLENGE4_STRINGS[idx], key,
					     &key_scores[key]) < 0) {
				continue;
			}

			if (key_scores[key].score > best_score) {
				best_score = key_scores[key].score;
				best_idx = idx;
				best_key = key;
			}
		}

		scores[idx] = key_scores;
	}

	printf(
	    "best_string=%s, best_idx=%d, best_key=%#02x best_score=%f, decrypted=",
	    CHALLENGE4_STRINGS[best_idx], best_idx, best_key, best_score);
	cpal_util_printbuf(scores[best_idx][best_key].decrypted,
			   scores[best_idx][best_key].decrypted_len);
	printf("\n");

	ret = strncmp(expected_best_plaintext,
		      (char *)scores[best_idx][best_key].decrypted,
		      strlen(expected_best_plaintext));
exit:
	if (scores != NULL) {
		for (size_t idx = 0; idx < CHALLENGE4_NUM_STRINGS; idx++) {
			for (unsigned int key = 0; key < 256; key++) {
				free(scores[idx][key].key);
				free(scores[idx][key].decrypted);
			}

			free(scores[idx]);
		}

		free(scores);
	}
	return ret;
}
