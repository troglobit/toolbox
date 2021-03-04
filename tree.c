/* Simple /bin/tree replacement
 *
 * Copyright (c) 2015-2021  Joachim Wiberg <troglobit@gmail.com>
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
#include <getopt.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define NONE " "
#define PIPE plain ? "|"  : "│"
#define FORK plain ? "|-" : "├─"
#define END  plain ? "`-" : "└─"

static int all = 0;
static int verbose = 0;
static int plain = 0;

static int filter(const struct dirent *entry)
{
	/* Skip current dir ".", and prev dir "..", from list of files */
	if ((1 == strlen(entry->d_name) && entry->d_name[0] == '.') ||
	    (2 == strlen(entry->d_name) && !strcmp(entry->d_name, "..")))
		return 0;

	if (!all && entry->d_name[0] == '.')
		return 0;
	
	return 1;
}

static void get_perms(struct stat *st, char *buf, size_t len)
{
	mode_t m = st->st_mode;

	snprintf(buf, len, "[%c%c%c%c%c%c%c%c%c%c] ",
		 S_ISCHR(m) ? 'c' : S_ISBLK(m) ? 'b' : S_ISFIFO(m) ? 'p' : S_ISLNK(m) ? 'l' : S_ISSOCK(m) ? 's' : '-',
		 (m & S_IRUSR) ? 'r' : '-',
		 (m & S_IWUSR) ? 'w' : '-',
		 (m & S_ISUID) ? 's' : (m & S_IXUSR) ? 'x' : '-',
		 (m & S_IRGRP) ? 'r' : '-',
		 (m & S_IWGRP) ? 'w' : '-',
		 (m & S_ISGID) ? 's' : (m & S_IXGRP) ? 'x' : '-',
		 (m & S_IROTH) ? 'r' : '-',
		 (m & S_IWOTH) ? 'w' : '-',
		 (m & S_IXOTH) ? 'x' : '-'
		);
}

static int descend(char *path, int show_perms, char *pfx)
{
	int i, n, result = 0;
	struct stat st;
	struct dirent **namelist = NULL;

	if (-1 == lstat(path, &st))
		return 1;

	if ((st.st_mode & S_IFMT) != S_IFDIR) {
		errno = ENOTDIR;
		return -1;
	}

	n = scandir(path, &namelist, filter, alphasort);
	if (n) {
		for (i = 0; i < n; i++) {
			char t = ' ', p[14] = "", s[256] = "";
			char buf[512];
			char dir[80];

			if (i + 1 == n) {
				printf("%s%s ", pfx, END);
				snprintf(dir, sizeof(dir), "%s     ", pfx);
			} else {
				printf("%s%s ", pfx, FORK);
				snprintf(dir, sizeof(dir), "%s%s    ", pfx, PIPE);
			}

			snprintf(buf, sizeof(buf), "%s/%s", path, namelist[i]->d_name);
			if (!lstat(buf, &st)) {
				if (show_perms)
					get_perms(&st, p, sizeof(p));
				if ((st.st_mode & S_IFMT) == S_IFDIR)
					t = '/';
				if (S_ISLNK(st.st_mode)) {
					snprintf(s, sizeof(s), "-> ");
					if (-1 == readlink(buf, &s[3], sizeof(s) - 3))
						s[0] = 0;
				}
			}

			printf("%s%s%c%s\n", p, namelist[i]->d_name, t, s);
			if (t == '/')
				result += descend(buf, show_perms, dir);

			free(namelist[i]);
		}

		free(namelist);
	}

	return result;
}

int tree(char *path, int show_perms)
{
	printf("%s\n", path);
	return descend(path, show_perms, "");
}

static int usage(int rc)
{
	fprintf(stderr, "usage: tree [-?hpv] PATH\n");
	return rc;
}

int main(int argc, char *argv[])
{
	int c;

	if (argc < 2)
		return usage(1);

	while ((c = getopt(argc, argv, "h?pv")) != EOF) {
		switch(c) {
		case 'p':
			plain = 1;
			break;

		case 'v':
			verbose = 1;
			break;

		case 'h':
		case '?':
			return usage(0);

		default:
			return usage(1);
		}
	}

	if (optind >= argc)
		return usage(1);

	return tree(argv[optind], verbose);
}

/**
 * Local Variables:
 *  indent-tabs-mode: t
 *  c-file-style: "linux"
 * End:
 */
