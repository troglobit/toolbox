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
	char *sep, *save;
} lfile_t;

lfile_t *lfopen(char *file, char *sep)
{
	lfile_t *lf;

	if (!file || !sep) {
		errno = EINVAL;
		return NULL;
	}

	/* Use calloc for clearing buf on behalf of lftok() */
	lf = calloc(sizeof(*lf), sizeof(char));
	if (lf) {
		lf->fp  = fopen(file, "r");
		lf->sep = sep;

		if (!lf->fp) {
			free(lf);
			return NULL;
		}
	}

	return lf;
}

void lfclose(lfile_t *lf)
{
	if (!lf)
		return;

	if (lf->fp)
		fclose(lf->fp);
	free(lf);
}

char *lftok(lfile_t *lf)
{
	char *token;

	if (!lf || !lf->fp || !lf->sep) {
		errno = EINVAL;
		return NULL;
	}

	token = strtok_r(NULL, lf->sep, &lf->save);
	if (!token) {
		while (fgets(lf->buf, sizeof(lf->buf), lf->fp)) {
			if (lf->buf[0] == '#')
				continue;

			token = strtok_r(lf->buf, lf->sep, &lf->save);
			if (!token)
				continue;
			
			return token;
		}

		errno = ENOENT;
		return NULL;
	}

	return token;
}


char *lfgetkey(lfile_t *lf, char *key)
{
	char *token;

	while ((token = lftok(lf))) {
		if (token[0] == '#')
			continue;

		if (!strncmp(token, key, MAX(strlen(token), strlen(key))))
			return lftok(lf);
	}

	return NULL;
}

int lfgetint(lfile_t *lf, char *key)
{
	char *token = lfgetkey(lf, key);

	if (token)
		return atoi(token);

	return -1;
}

int fgetint(char *file, char *sep, char *key)
{
	int val = -1;
	lfile_t *lf;

	lf = lfopen(file, sep);
	if (lf) {
		val = lfgetint(lf, key);
		lfclose(lf);
	}

	return val;
}

int main(void)
{
	int val;
	lfile_t lf;

	val = fgetint("/etc/protocols", " \n\t", "udp");
	if (val == -1) {
		perror("Failed locating 'udp' protocol");
		return 1;
	}
	printf("udp has proto %d\n", val);

	val = fgetint("/etc/services", " /\n\t", "ftp");
	if (val == -1) {
		perror("Failed locating 'ftp' service");
		return 1;
	}
	printf("ftp is inet port %d\n", val);

	return 0;
}

/**
 * Local Variables:
 *  compile-command: "gcc -Os -fno-stack-protector -fno-ident -o parseprotoserv parseprotoserv.c && ./parseprotoserv"
 *  version-control: t
 *  indent-tabs-mode: t
 *  c-file-style: "linux"
 * End:
 */
