#include <cryptopal-common.h>

#include <ctype.h>
#include <stdio.h>

void cpal_util_printbuf(const uint8_t *buf, const size_t buf_len)
{
	size_t offset = 0;

	while (offset < buf_len) {
		uint8_t val = buf[offset];

		if (iscntrl(val)) {
			fprintf(stdout, "\\x%02x", val);
		} else {
			fputc(val, stdout);
		}

		offset++;
	}
}
