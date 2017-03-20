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
	char *encoded = NULL;
	char *base16_encoded = NULL;

	size_t base16_encoded_size =
	    cpal_base16_encode((const uint8_t *)base16_decoded,
			       strlen(base16_decoded), &base16_encoded);

	if (base16_encoded_size == 0) {
		goto exit;
	}

	printf("base16_encoded=%s,expected=%s\n", base16_encoded, base16_input);

	size_t decoded_size =
	    cpal_base16_decode(base16_input, strlen(base16_input), &decoded);

	if (decoded_size == 0) {
		goto exit;
	}

	printf("base16_decoded=%s,expected=%s\n", decoded, base16_decoded);

	size_t encoded_size = cpal_base64_encode(decoded, decoded_size, &encoded);

	if (encoded_size == 0) {
		goto exit;
	}

	printf("base64_encoded=%s, expected=%s\n", encoded, expected_base64_output);
	ret = strcmp(encoded, expected_base64_output);
exit:
	free(decoded);
	free(encoded);
	free(base16_encoded);

	return ret;
}
