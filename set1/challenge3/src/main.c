#include <cryptopal-common.h>

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#define LETTER(letter, freq)                                                       \
	ENGLISH_FREQ_TABLE[(int)letter] = freq;                                    \
	ENGLISH_FREQ_TABLE[(int)letter - 32] = freq;

#define CHAR(ch, freq) ENGLISH_FREQ_TABLE[(int)ch] = freq;

static double ENGLISH_FREQ_TABLE[256];

static void initialize_english_freq_table(void)
{
	for (size_t i = 0; i < 256; i++) {
		ENGLISH_FREQ_TABLE[i] = 0.0;
	}

	/*
	 * According to wikipedia, spaces are slightly more frequent than the
	 * letter 'e' in English sentences.  Treat spaces as equivalent to the
	 * letter 'e' for the sake of correctly sorting the decrypted ciphertexts.
	 */
	CHAR(' ', 0.1270)
	LETTER('a', 0.08167)
	LETTER('b', 0.01492)
	LETTER('c', 0.02782)
	LETTER('d', 0.04253)
	LETTER('e', 0.1270)
	LETTER('f', 0.02228)
	LETTER('g', 0.02015)
	LETTER('h', 0.06094)
	LETTER('i', 0.06966)
	LETTER('j', 0.00153)
	LETTER('k', 0.00772)
	LETTER('l', 0.04025)
	LETTER('m', 0.02406)
	LETTER('n', 0.06749)
	LETTER('o', 0.07507)
	LETTER('p', 0.01929)
	LETTER('q', 0.00095)
	LETTER('r', 0.05987)
	LETTER('s', 0.06327)
	LETTER('t', 0.09056)
	LETTER('u', 0.02758)
	LETTER('v', 0.00978)
	LETTER('w', 0.02360)
	LETTER('x', 0.00150)
	LETTER('y', 0.01974)
	LETTER('z', 0.00074)
}

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

static int score_compare(const void *p1, const void *p2)
{
	const struct cpal_analysis_key_score *s1 = p1;
	const struct cpal_analysis_key_score *s2 = p2;

	double a = s1->score;
	double b = s2->score;

	if (a < b) {
		return 1;
	} else if (a > b) {
		return -1;
	} else {
		return 0;
	}
}

int main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;

	int ret = 1;

	struct cpal_analysis_key_score scores[256];
	size_t scores_len = sizeof(scores) / sizeof(*scores);
	memset(&scores, 0, sizeof(scores));

	const char *expected_plaintext = "Cooking MC's like a pound of bacon";
	const char *encoded_ciphertext =
	    "1b37373331363f78151b7f2b783431333d78397828372d363c78373e783a393b3736";

	uint8_t *decoded_ciphertext = NULL;
	size_t decoded_ciphertext_len = 0;

	if (cpal_base16_decode(encoded_ciphertext, strlen(encoded_ciphertext),
			       &decoded_ciphertext, &decoded_ciphertext_len) < 0) {
		goto exit;
	}

	initialize_english_freq_table();

	for (unsigned int key = 0; key < scores_len; key++) {
		uint8_t kval = (uint8_t)key;

		if (cpal_analysis_try_key(
			decoded_ciphertext, decoded_ciphertext_len, &kval, 1,
			&scores[key], english_freq_bhattacharyya_score,
			xor_decrypt) < 0) {
			printf("failed decrypting with key: %02x\n", kval);
		}
	}

	qsort(scores, scores_len, sizeof(struct cpal_analysis_key_score),
	      score_compare);

	printf("top 5 results:\n");

	for (unsigned int i = 0; i < 5; i++) {
		printf("rank=%d, key=%#02x, score=%f, plaintext=", i + 1,
		       *scores[i].key, scores[i].score);
		cpal_util_printbuf(scores[i].decrypted, scores[i].decrypted_len);
		printf("\n");
	}

	ret = strncmp(expected_plaintext, (const char *)scores[0].decrypted,
		      strlen(expected_plaintext));
exit:
	free(decoded_ciphertext);
	for (unsigned int i = 0; i < scores_len; i++) {
		free(scores[i].key);
		free(scores[i].decrypted);
	}

	return ret;
}
