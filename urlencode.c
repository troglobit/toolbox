/* URL encoding (strict)
 *
 * Copyright (c) 2016  Joachim Nilsson <troglobit@gmail.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <errno.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * urlencode() - Convert input string to URL encoded string
 * @buf: Input string to encode
 * @len: Length of input string, or zero for all
 *
 * This function converts the given input string to a URL encoded string
 * returned in an allocated string.  All input characters that are not:
 * a-z, A-Z, 0-9, '-', '.', or '_' are converted to their "URL escaped"
 * version (%NN where NN is a two-digit hexadecimal number).
 *
 * If length is set to 0 (zero), this function uses strlen() on the
 * input string to find out the size.
 *
 * Returns:
 * An allocated URL encoded string, or %NULL on error.
 */
char *urlencode(char *buf, size_t len)
{
	char *tmp;
	size_t i, j;

	if (!buf) {
		errno = EINVAL;
		return NULL;
	}
		
	if (!len)
		len = strlen(buf);

	tmp = calloc(len * 3, sizeof(char));
	if (!tmp)
		return NULL;

	for (i = 0, j = 0; i < len; i++) {
		char c = buf[i];

		if (isalnum(c) || c == '-' || c == '.' || c == '_') {
			tmp[j++] = c;
			continue;
		}

		snprintf(&tmp[j], 4, "%%%02X", c);
		j += 3;
	}

	return tmp;
}

char *urlencode2(char *str, char *buf, size_t len)
{
	size_t i, j;

	if (!buf || !len) {
		errno = EINVAL;
		return NULL;
	}
		
	memset(buf, 0, len);

	for (i = 0, j = 0; i < strlen(str) && j < len; i++) {
		char c = str[i];

		if (isalnum(c) || c == '-' || c == '.' || c == '_') {
			buf[j++] = c;
			continue;
		}

		if (j + 4 < len) {
			snprintf(&buf[j], 4, "%%%02X", c);
			j += 3;
		}
	}

	return buf;
}

/*
 * URL:    ^F*D&S^s~09d
 * ENC:    %5EF%2AD%26S%5Es%7E09d
 * DH.org: %5EF%2AD%26S%5Es%7E09d
 *
 * For details, see http://hn.org/hnRFC10.txt
 */
int main(void)
{
	char url[] = "^F*D&S^s~09d";
	char buf[3 * sizeof(url)];

	printf("URL   : %s\n", url);
	printf("ENC   : %s\n", urlencode(url, strlen(url)));
	printf("ENC2  : %s\n", urlencode2(url, buf, sizeof(buf)));
	printf("DH.org: %%5EF%%2AD%%26S%%5Es%%7E09d\n");

	return 0;
}

/**
 * Local Variables:
 *  compile-command: "gcc -o urlencode urlencode.c && ./urlencode && rm urlencode"
 *  indent-tabs-mode: t
 *  c-file-style: "linux"
 * End:
 */
