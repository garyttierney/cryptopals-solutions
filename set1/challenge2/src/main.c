#include <cryptopal-common.h>

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *buffer_to_string(const uint8_t *buffer, const size_t size)
{
	char *string = calloc(sizeof *string, size + 1);
	if (string == NULL) {
		return NULL;
	}

	memcpy(string, buffer, size);
	string[size] = '\0';

	return string;
}

int main(int argc, char *argv[])
{
	(void) argc;
	(void) argv;

	int ret = 1;

	const char *a_encoded = "1c0111001f010100061a024b53535009181c";
	const char *b_encoded = "686974207468652062756c6c277320657965";
	const char *expected_xor_result = "746865206b696420646f6e277420706c6179";

	uint8_t *a_decoded = NULL;
	uint8_t *b_decoded = NULL;
	uint8_t *xor_result = NULL;
	char *xor_result_string = NULL;
	char *xor_result_encoded = NULL;

	size_t a_size =
	    cpal_base16_decode(a_encoded, strlen(a_encoded), &a_decoded);
	if (a_size == 0) {
		goto exit;
	}

	size_t b_size =
	    cpal_base16_decode(b_encoded, strlen(b_encoded), &b_decoded);
	if (b_size == 0) {
		goto exit;
	}

	if (a_size != b_size) {
		goto exit;
	}

	if (cpal_cipher_xor_fixed(a_size, a_decoded, b_decoded, &xor_result) != 0) {
		goto exit;
	}

	xor_result_string = buffer_to_string(xor_result, a_size);
	if (xor_result_string == NULL) {
		goto exit;
	}

	if (cpal_base16_encode(xor_result, a_size, &xor_result_encoded) == 0) {
		goto exit;
	}
	
	printf("xor_result=%s\n", xor_result_string);
	printf("xor_result_encoded=%s,expected=%s\n", xor_result_encoded, expected_xor_result);
	ret = strcasecmp(xor_result_encoded, expected_xor_result);
exit:
	free(a_decoded);
	free(b_decoded);
	free(xor_result);
	free(xor_result_string);
	free(xor_result_encoded);

	return ret;
}
