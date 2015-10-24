/* Parse /etc/protocols and /etc/services
 *
 * Copyright (c) 2015  Joachim Nilsson <troglobit@gmail.com>
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>

typedef struct {
	FILE *fp;
	char buf[256];
	char *sep;
} unix_file_t;

int unix_file_open(unix_file_t *uf, char *file, char *sep)
{
	if (!uf || !file || !sep) {
		errno = EINVAL;
		return 1;
	}
		
	memset(uf, 0, sizeof(*uf));
	uf->sep = sep;

	uf->fp = fopen(file, "r");
	if (!uf->fp)
		return 1;

	return 0;
}

void unix_file_close(unix_file_t *uf)
{
	if (uf && uf->fp) {
		fclose(uf->fp);
		uf->fp = NULL;
	}
}

char *unix_file_token(unix_file_t *uf)
{
	char *token;

	if (!uf || !uf->fp || !uf->sep) {
		errno = EINVAL;
		return NULL;
	}

	token = strtok(NULL, uf->sep);
	if (!token) {
		while (fgets(uf->buf, sizeof(uf->buf), uf->fp)) {
			if (uf->buf[0] == '#')
				continue;

			token = strtok(uf->buf, uf->sep);
			if (!token)
				continue;
			
			return token;
		}

		return NULL;
	}

	return token;
}

int unix_file_getent(unix_file_t *uf, char *ent)
{
	char *token;
	size_t token_len, ent_len = strlen(ent);

	if (!uf || !uf->fp) {
		errno = EINVAL;
		return -1;
	}

	while ((token = unix_file_token(uf))) {

		if (token[0] == '#')
			continue;

		token_len = strlen(token);
		if (!strncmp(token, ent, MAX(token_len, ent_len))) {
			token = unix_file_token(uf);
			if (!token) {
				errno = ENOENT;
				return -1;
			}

			return atoi(token);
		}
	}

	errno = ENOENT;
	return -1;
}

int main(void)
{
	int val;
	unix_file_t uf;

	if (unix_file_open(&uf, "/etc/protocols", " \n\t")) {
		perror("Failed opening file");
		return 1;
	}

	val = unix_file_getent(&uf, "udp");
	if (-1 == val)
		perror("Failed locating udp protocol");
	else
		printf("udp has proto %d\n", val);

	unix_file_close(&uf);

	if (unix_file_open(&uf, "/etc/services", " /\n\t")) {
		perror("Failed opening file");
		return 1;
	}
	
	val = unix_file_getent(&uf, "ftp");
	if (-1 == val)
		perror("Failed locating ftp service");
	else
		printf("ftp is inet port %d\n", val);

	unix_file_close(&uf);
}

/**
 * Local Variables:
 *  compile-command: "gcc -o parseprotoserv parseprotoserv.c && ./parseprotoserv"
 *  version-control: t
 *  indent-tabs-mode: t
 *  c-file-style: "linux"
 * End:
 */
