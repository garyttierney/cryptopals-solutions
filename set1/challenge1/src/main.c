#include <cryptopal-common.h>

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;

	int ret = 1;

	const char *base16_decoded =
	    "I'm killing your brain like a poisonous mushroom";

	const char *base16_input = "49276d206b696c6c696e6720796f757220627261696e206"
				   "c696b65206120706f69736f6e6f7573206d757368726f6f"
				   "6d";

	const char *expected_base64_output =
	    "SSdtIGtpbGxpbmcgeW91ciBicmFpbiBsaWtlIGEgcG9pc29ub3VzIG11c2hyb29t";

	uint8_t *decoded = NULL;
	size_t decoded_len = 0;

	char *encoded = NULL;
	size_t encoded_len = 0;

	char *base16_encoded = NULL;
	size_t base16_encoded_len = 0;

	if (cpal_base16_encode((const uint8_t *)base16_decoded,
			       strlen(base16_decoded), &base16_encoded,
			       &base16_encoded_len) < 0) {
		goto exit;
	}

	if (cpal_base16_decode(base16_input, strlen(base16_input), &decoded,
			       &decoded_len) < 0) {
		goto exit;
	}

	if (cpal_base64_encode(decoded, decoded_len, &encoded, &encoded_len) < 0) {
		goto exit;
	}

	printf("base16_encoded=%s,expected=%s\n", base16_encoded, base16_input);
	printf("base16_decoded=%s,expected=%s\n", decoded, base16_decoded);
	printf("base64_encoded=%s, expected=%s\n", encoded, expected_base64_output);
	ret = strcmp(encoded, expected_base64_output);
exit:
	free(decoded);
	free(encoded);
	free(base16_encoded);

	return ret;
}
